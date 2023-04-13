/*
 *  Copyright (C) 2019-2023, VadRov, all right reserved.
 *
 *	Воспроизведение AVI
 *
 *  Допускается свободное распространение.
 *  При любом способе распространения указание автора ОБЯЗАТЕЛЬНО.
 *  В случае внесения изменений и распространения модификаций указание первоначального автора ОБЯЗАТЕЛЬНО.
 *  Распространяется по типу "как есть", то есть использование осуществляется на свой страх и риск.
 *  Автор не предоставляет никаких гарантий.
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 */
#include "display.h"
#include "riff_read_header.h"
#include "jpeg_chan.h"
#include "mp3_callback.h"
#include "pcm5102.h"
#include "ff.h"
#include "mystring.h"
#include "player.h"
#include "avi.h"
#include "microgl2d.h"
#include "textures.h"
#include "keyboard.h"
#include <stdlib.h>
#include <string.h>
#include "encoder.h"

extern uint32_t millis;				//Счетчик "системного времени".

extern iPicture_jpg fon_file;		//файл с фоном в формате Jpeg во флеш микроконтроллера.

extern uint32_t OSD_cycle_counter; 	//Cчетчик тактов, затрачиваемых на отображение OSD
									//при выводе текущего кадра.
uint8_t AVI_color_mode = 1;			//Формат цвета AVI: 1 - R5G6B5 цветной, 3 - R5G6B5 оттенки серого.

uint8_t AVI_volume = 15;			//Громкость звука: от 0 (минимум, выключен) до 16 (максимум).

MGL_OBJ *AVI_osd = 0;				//Указатель на OSD объект

#define  LCD_Load_JPG(lcd, x, y, w, h, image_stream, location) \
		 ((uint8_t) LCD_Load_JPG_chan ((LCD_Handler*)(lcd), (uint16_t)(x), (uint16_t)(y), (uint16_t)(w), (uint16_t)(h), (void*)(image_stream), (PictureLocation)(location)))

/*
 * Воспроизведение видео в заданном окне:
 * dir - путь до файла
 * fname - имя файла
 * lcd - указатель на обработчик дисплея
 * x, y - координаты левого верхнего угла окна для вывода видеокадра
 * win_wdt, win_hgt - ширина и высота окна вывода видеокадра
 */
void PlayAVI(char *dir, char *fname, LCD_Handler *lcd, uint16_t x, uint16_t y, uint16_t win_wdt, uint16_t win_hgt)
{
	FIL file; //Файловый объект
	//"Склеим" путь до файла с именем файла
	char *tmp = my_strsplice(2, "/", dir, fname);
	//Откроем файл для чтения (FA_READ) с проверкой на ошибку при открытии res
	FRESULT res = f_open(&file, tmp, FA_READ);
	free(tmp);
	if (res != FR_OK) return; //Ошибка открытия файла
	RiffHDR RHdr; //Объявим обработчик riff
	memset(&RHdr, 0, sizeof(RiffHDR)); //Инициализация обработчика riff
	//Теперь проверим, а дествительно ли это AVI файл. Если это он, то считаем все необходимые для
	//воспроизведения параметры
	if (Read_RIFF_Header(&RHdr, &file) == RIFF_OK && RHdr.mediaType == FourccType_AVI) {
		//Указатель чтения файла перемещаем на первый кадр данных
		f_lseek(&file, RHdr.startPosData);
		//Объявим структуру, которая определяет загрузчик (чанк, chunk, "кусок") кадра в потоках avi
		struct {
			uint8_t streamCode[4];	//Тип кадра, по которому мы будем определять, с каким кадром
									//имеем дело (с видеокадром или аудиокадром)
			uint32_t len;			//Длина данных кадра. Она должна быть кратна 2, если нет, то мы должны
									//увеличить на 1 это значение.
		} chunk;
		//Уточняем координаты левого верхнего угла окна, в которое будем выводить видеокадры (центровка в заданном окне)
		if (win_hgt > RHdr.AVIHeader.dwHeight) {
			y += (win_hgt - (uint16_t)RHdr.AVIHeader.dwHeight)/2;
		}
		if (win_wdt > RHdr.AVIHeader.dwWidth) {
			x += (win_wdt - (uint16_t)RHdr.AVIHeader.dwWidth)/2;
		}
		if ((RHdr.vidFmt.biCompression != 0x47504A4D) || 	//Если кодек видеопотока не MJPEG или
			(RHdr.audFmt.wFormatTag != 0x0055)          ) {	//кодек аудиопотока не mp3,
			if (RHdr.AVIStreams) free(RHdr.AVIStreams); 	//то очищаем параметры потоков avi,
			f_close(&file); 								//закрываем файл file
			return;											//и прекращаем работу плеера (выходим)
		}
		//Делаем копию файлового объекта AVI файла, так как у нас будет два независимых потока:
		//один - аудиопоток, а другой - видеопоток.
		FIL file1 = file;
		//------------------------------ Настройка аудиопотока, декодера mp3 и DAC ---------------------------------
		//Объявим переменную с пользовательскими данными декодера
		Player_client_data playdata;
		//Выделим память для данных mp3 декодера
		Player_client_mp3 *mp3  = (Player_client_mp3 *)calloc(1, sizeof(Player_client_mp3));
		//Создадим плеер
		player1 = PlayerNew();
		//Инициализируем пользовательские данные декодера
		playdata.client_data_decoder = mp3;
		playdata.file = &file;
		//Инициализируем плеер
		PlayerInit(player1, mp3_read_header_callback, mp3_start_play_callback, mp3_update_play_callback, mp3_stop_callback, &playdata);
		//Читаем загрузчик аудиопотока для настройки параметров декодера и подготовки к старту плеера
		PlayerReadHeader(player1);
		//Стартуем плеер (создается PCM буфер, считываются и декодируются в PCM буфер два первых кадра из аудиопотока)
		PlayerStartPlay(player1);
		//Настраиваем аудио ЦАП (audio DAC)
		Setup_DAC(AudioDAC_I2S, player1->audioCFG.samplerate, LL_I2S_DATAFORMAT_16B_EXTENDED);
		//-------------------------------------------------------------------------------------------------------------
		uint64_t t_audio, t_video = 0; //Счетчики времени по аудио-/видеокадрам (для синхронизации), мкс
		uint32_t ms_per_frame = 0; //Счетчик времени декодирования видеокадра
		uint32_t MksPerFrame = RHdr.AVIHeader.dwMicroSecPerFrame; //Время отображения одного видеокадра, мкс
		UINT read_b = 0; //Переменная для фактического числа считанных байт из файлового потока
		uint8_t res1 = 0, skip_frame = 0; //Флаги: ошибок в файловом потоке и пропуска кадра
		iFile_jpg jpg_file; //Структура с данными кадра в файловом потоке (указатель на обработчик файла и длина кадра)
		FSIZE_t pos; //Текущая позиция в файловом потоке
		uint32_t p;
		char buff[20];
		jpg_file.file = &file1;

		AVI_osd = 0;		//запрет вывода меню OSD при старте плеера
		AVI_color_mode = 1;	//инициализация формата цвета (1 - цветной, 3 - оттенки серого)
		uint32_t keys;		//переменная для "ключей" "кнопок"-событий
		uint8_t m_p_all = 2, m_p = 0; //Всего пунктов в меню OSD и номер текущего пункта под курсором

		//---- Настройка объектов информационного окна-меню OSD (опции) --------
		MGL_OBJ *osd1 = MGL_ObjectAdd(0, MGL_OBJ_TYPE_FILLRECTANGLE);
		MGL_SetRectangle(osd1, 0, 0, 200, 80, COLOR_DARKGREY);
		MGL_ObjectSetTransparency(osd1, 128);
		MGL_OBJ *border = MGL_ObjectAdd(osd1, MGL_OBJ_TYPE_RECTANGLE);
		MGL_SetRectangle(border, 3, 3, 197, 77, COLOR_WHITE);
		char text_clr_md[50] = "Color mode:  "; //13 символов
		my_strcpy(&text_clr_md[13], AVI_color_mode == 1 ? " Coloured" : "Grayscale");
		MGL_OBJ *pl_clr = MGL_ObjectAdd(osd1, MGL_OBJ_TYPE_TEXT);
		MGL_SetText(pl_clr, 10, 10, text_clr_md, &Font_8x13, 0, COLOR_YELLOW);
		char text_fr_core[50] = "Core speed, MHz:   "; //19 символов
		if (SystemCoreClock >= 100000000) {
			utoa(SystemCoreClock/1000000, &text_fr_core[19], 10);
		}
		else {
			text_fr_core[19] = ' ';
			utoa(SystemCoreClock/1000000, &text_fr_core[20], 10);
		}
		MGL_OBJ *pl_core = MGL_ObjectAdd(osd1, MGL_OBJ_TYPE_TEXT);
		MGL_SetText(pl_core, 10, 23, text_fr_core, &Font_8x13, 0, COLOR_YELLOW);
		char text_fps[50] = "Decoding time, ms: "; //19 символов
		MGL_OBJ *pl_fps = MGL_ObjectAdd(osd1, MGL_OBJ_TYPE_TEXT);
		MGL_SetText(pl_fps, 10, 45, text_fps, &Font_8x13, 0, COLOR_YELLOW);
		MGL_GRADIENT *grad1 = MGL_GradientCreate(MGL_GRADIENT_LINEAR);
		MGL_GradientAddColor(grad1, 0, COLOR_RED, 1);
		MGL_GradientAddColor(grad1, 50, COLOR_GREEN, 1);
		MGL_GradientAddColor(grad1, 100, COLOR_BLUE, 1);
		MGL_GradientSetDeg(grad1, 90);
		MGL_OBJ *rect = MGL_ObjectAdd(osd1, MGL_OBJ_TYPE_FILLRECTANGLE);
		MGL_SetRectangle(rect, 10, 60, 190, 70, COLOR_BLUE);
		MGL_ObjectSetGradient(rect, grad1);
		char text_prnm[] = "(c)VadRov  AVI Player";
		MGL_OBJ *pl_name = MGL_ObjectAdd(osd1, MGL_OBJ_TYPE_TEXT);
		MGL_SetText(pl_name, 16, 59, text_prnm, &Font_8x13, 0, COLOR_WHITE);
		MGL_OBJ *m_cur = MGL_ObjectAdd(osd1, MGL_OBJ_TYPE_FILLRECTANGLE);
		MGL_SetRectangle(m_cur, 6, 10 + m_p * 13, 194, 23 + m_p * 13, COLOR_RED);
		MGL_ObjectSetTransparency(m_cur, 128);
		//переносим окно OSD в центр дисплея
		MGL_ObjectListMove(osd1, (lcd->Width - 200) / 2, (lcd->Height - 80) / 2);
		//--------------------------------------------------------------------------

		//----- Настройка объектов информационного окна-меню OSD (громкость) -------
		MGL_OBJ *osd2 = MGL_ObjectAdd(0, MGL_OBJ_TYPE_FILLRECTANGLE);
		MGL_SetRectangle(osd2, 0, 0, 200, 41, COLOR_DARKGREY);
		MGL_ObjectSetTransparency(osd2, 128);
		MGL_OBJ *border1 = MGL_ObjectAdd(osd2, MGL_OBJ_TYPE_RECTANGLE);
		MGL_SetRectangle(border1, 3, 3, 197, 38, COLOR_WHITE);
		MGL_TEXTURE texture_volume = {(MGL_IMAGE *)&image_volume, 0, 0};
		MGL_OBJ *volume = MGL_ObjectAdd(osd2, MGL_OBJ_TYPE_FILLCIRCLE);
		MGL_SetCircle(volume, 21, 20, 16, 0);
		MGL_ObjectSetTexture(volume, &texture_volume);
		MGL_OBJ *volume_bar = MGL_ObjectAdd(osd2, MGL_OBJ_TYPE_FILLRECTANGLE);
		MGL_TEXTURE texture_x = {(MGL_IMAGE *)&image_x, 0, 0};
		MGL_OBJ *volume_x = MGL_ObjectAdd(osd2, MGL_OBJ_TYPE_FILLCIRCLE);
		MGL_SetCircle(volume_x, 21, 20, 16, 0);
		MGL_ObjectSetTexture(volume_x, &texture_x);
		char text_mute[] = "Mute"; //19 символов
		MGL_OBJ *mute = MGL_ObjectAdd(osd2, MGL_OBJ_TYPE_TEXT);
		MGL_SetText(mute, 40, 14, text_mute, &Font_8x13, 0, COLOR_YELLOW);
		if (!AVI_volume) {
			MGL_ObjectSetVisible(volume_bar, 0);
		}
		else {
			uint32_t col_bar = (AVI_volume > 3 && AVI_volume < 10) ? COLOR_YELLOW : ((AVI_volume >= 10) ? COLOR_GREEN : COLOR_RED);
			MGL_SetRectangle(volume_bar, 40, 14, 40 + ((AVI_volume * (194 - 40 + 1 - 2)) >> 4), 26, col_bar);
			MGL_ObjectSetVisible(volume_x, 0);
			MGL_ObjectSetVisible(mute, 0);
		}
		//переносим окно OSD в центр дисплея
		MGL_ObjectListMove(osd2, (lcd->Width - 200) / 2, (lcd->Height - 42) / 2);
		//--------------------------------------------------------------------------

		uint32_t osd_time = millis; //Это счетчик времени для периода неактивности пользователя
									//при включенном окне OSD "Опции".
		uint32_t osd_time1 = millis;//Это счетчик времени для периода неактивности пользователя
									//при включенном окне OSD "Громкость".
		LCD_Fill(lcd, COLOR_BLACK); //Закрашиваем дисплей черным цветом.
		//Cтартуем аудиопоток (запускаем DAC). Дополнительный коэффициент 2 в samples
		//используется для данных в формате DATAFORMAT_16B и DATAFORMAT_16B_EXTENDED, т.к. DMA в режиме half word.
		Start_DAC(AudioDAC_I2S, AudioDAC_DMA, AudioDAC_DMA_Stream, player1->outbuf, 2 * 2 * player1->audioCFG.samples);
		while (!res1 && file1.fptr < file1.obj.objsize) { //В цикле читаем кадры из потока до тех пор,
														  //пока не будет ошибки чтения либо конца потока
			//Запрещаем прерывания потока DMA, обслуживающего DAC (иначе не избежать коллизий,
			//связанных с одновременным доступом по SPI к SD карте).
			NVIC_DisableIRQ(AudioDAC_IRQn);
			//Читаем загрузчик кадра - чанк (в нем информация о типе и длине кадра).
			res = f_read(&file1, &chunk, 8, &read_b);
			//Разрешаем прерывания потока DMA, обслуживающего DAC.
			NVIC_EnableIRQ(AudioDAC_IRQn);
			if (res || read_b != 8) break; //Если возникла ошибка при чтении - прерываем цикл
			//-------------------------- проверка на следующий "кусок" AVI файла --------------------------
			//Если avi файл больше 1 Гб, то видео может быть "порезано" на "куски", начинающиеся с чанка RIFF
			//формат заголовка "куска" "RIFF[nnnn]AVIXLIST[mmmm]movi, где:
			//nnnn и mmmm длины чанков. После чанка movi продолжают следовать кадры.
			if (*((uint32_t*)chunk.streamCode) == chunk_RIFF) { //Обнаружили новый "кусок" AVI?
				NVIC_DisableIRQ(AudioDAC_IRQn);
				res = f_read(&file1, buff, 16, &read_b);
				NVIC_EnableIRQ(AudioDAC_IRQn);
				if (res || read_b != 16) break; //Если возникла ошибка при чтении - прерываем цикл.
				if ( *((uint32_t*)&buff[0]) == TYPE_AVIX  &&   //Проверяем формат заголовка "куска"
					 *((uint32_t*)&buff[4]) == chunk_LIST &&
					 *((uint32_t*)&buff[12]) == chunk_movi   ) continue; //Успех -> возобновление чтения кадров.
				else break; //Формат заголовка продолжения avi неверный -> прерываем цикл, завершая работу плеера.
			}
			pos = file1.fptr; //Запоминаем позицию указателя чтения в файловом потоке.
			jpg_file.size = chunk.len & 1 ? chunk.len + 1 : chunk.len; //Размер данных кадра должен быть четным.
			if ((chunk.streamCode[2]=='d' && chunk.streamCode[3]=='c') ||
				(chunk.streamCode[2]=='D' && chunk.streamCode[3]=='C')	 ) { //Проверка на то, что текущий кадр является видеокадром.
				t_video += MksPerFrame; //Увеличиваем счетчик длительности по видеокадрам.
				if (!jpg_file.size) { //Пустой кадр пропускаем.
					continue;
				}
				if (!skip_frame) { //Если сброшен флаг пропуска видеокадра, то декодируем текущий кадр.
					ms_per_frame  = millis; //Определяем время декодирования кадра: инициализация счетчика.
					res1 = LCD_Load_JPG(lcd, x, y, win_wdt, win_hgt, &jpg_file, PICTURE_IN_FILE);
					ms_per_frame = millis - ms_per_frame - OSD_cycle_counter / (SystemCoreClock/1000); //Вычитаем "затраты" на OSD
					utoa(ms_per_frame, &text_fps[19], 10); //Преобразуем время декодирования кадра в строку.
				}
			}
			if (jpg_file.size) { //Перемещаем указатель позиции чтения в файле только в том случае,
								 //если не вычитали кадр. Например, если кадр был пропущен в целях синхронизации
								 //или он был аудиокадром и его также надо пропустить.
				NVIC_DisableIRQ(AudioDAC_IRQn);
				res = f_lseek(&file1, pos + jpg_file.size);
				NVIC_EnableIRQ(AudioDAC_IRQn);
				if (res) break;
			}
			NVIC_DisableIRQ(AudioDAC_IRQn); //Выключаем прерывание, обслуживающее DAC, чтобы не было коллизий с player1,
											//которая может быть очищена в прерывании в самое неподходящее время.
			if (player1) { //Если плеер активен (вдруг возникнет ошибка воспроизведения аудиопотока)
				t_audio = player1->PlayTimeMks; //Время, прошедшее с момента старта воспроизведения аудиопотока
				NVIC_EnableIRQ(AudioDAC_IRQn); //Включаем прерывание, обслуживающее DAC.
				//--------- Синхронизация с допуском в пределах длительности показа одного видеокадра ----------
				//Синхронизация не нужна, если отставание/опережение между потоками в пределах длительности видеокадра.
				if ((t_video >= t_audio - MksPerFrame / 2) && (t_video <= t_audio + MksPerFrame / 2)) {
					skip_frame = 0;
				}
				//Есть опережение видеопотока над аудиопотоком: ждем, когда аудиопоток "догонит" видеопоток.
				else if (t_video > t_audio) {
					p = (uint32_t)((t_video - t_audio) / 1000);
					if (p) LL_mDelay(p);
					skip_frame = 0;
				}
				//Есть опережение аудиопотока над видеопотоком: пропускаем следующий видеокадр, чтобы догнать аудиопоток.
				else {
					skip_frame = 1;
				}
				//--------------------------------------------------------------------------------------------------------
			}
			else {
				NVIC_EnableIRQ(AudioDAC_IRQn); //Включаем прерывание, обслуживающее DAC.
				break; //Аудиопоток завершился -> прерываем основной цикл.
			}
			//------------------------------------- OSD и обработка событий энкодера -------------------------------------
			if (AVI_osd == osd1 && millis - osd_time >= 10000) { //Скрываем OSD меню "Опции", если нет активности
				AVI_osd = 0;							 		 //пользователя 10 и более секунд.
			}
			if (AVI_osd == osd2 && millis - osd_time1 >= 5000) { //Скрываем OSD меню "Громкость", если нет активности
				AVI_osd = 0;							 		 //пользователя 5 и более секунд.
			}
			if (KEYB_kbhit()) { 				//Проверка на наличие события от энкодера.
				keys = KEYB_Inkeys(); 			//Считываем ключи "кнопок"-событий энкодера.
				if (AVI_osd != osd1) {	//OSD "Опции" скрыто.
					if (keys & (1 << KEYB_RIGHT)) { //Кратковременное нажатие кнопки -> показать osd.
						AVI_osd = osd1;
						osd_time = millis; //Инициализация времени "неактивности" пользователя.
					}
					else if (!AVI_osd && (keys & (1 << KEYB_LEFT))) break; //Удержание кнопки -> выход из плеера в менеджер файлов.
					else if (keys & (1 << KEYB_DOWN)) { //Увеличение громкости.
						if (AVI_volume < 16) {
							AVI_volume++;
							uint32_t col_bar = (AVI_volume > 3 && AVI_volume < 10) ? COLOR_YELLOW : ((AVI_volume >= 10) ? COLOR_GREEN : COLOR_RED);
							MGL_SetRectangle(volume_bar, 40, 14, 40 + ((AVI_volume * (194 - 40 + 1 - 2)) >> 4), 26, col_bar);
							MGL_ObjectMove(volume_bar, (lcd->Width - 200) / 2, (lcd->Height - 42) / 2);
							MGL_ObjectSetVisible(volume_bar, 1);
							MGL_ObjectSetVisible(volume_x, 0);
							MGL_ObjectSetVisible(mute, 0);
						}
						AVI_osd = osd2;
						osd_time1 = millis; //Инициализация времени "неактивности" пользователя.
					}
					else if (keys & (1 << KEYB_UP)) { //Уменьшение громкости.
						if (AVI_volume) {
							AVI_volume--;
							if (!AVI_volume) {
								MGL_ObjectSetVisible(volume_bar, 0);
								MGL_ObjectSetVisible(volume_x, 1);
								MGL_ObjectSetVisible(mute, 1);
							}
							else {
								uint32_t col_bar = (AVI_volume > 3 && AVI_volume < 10) ? COLOR_YELLOW : ((AVI_volume >= 10) ? COLOR_GREEN : COLOR_RED);
								MGL_SetRectangle(volume_bar, 40, 14, 40 + ((AVI_volume * (194 - 40 + 1 - 2)) >> 4), 26, col_bar);
								MGL_ObjectMove(volume_bar, (lcd->Width - 200) / 2, (lcd->Height - 42) / 2);
								MGL_ObjectSetVisible(volume_x, 0);
								MGL_ObjectSetVisible(mute, 0);
							}
						}
						AVI_osd = osd2;
						osd_time1 = millis; //Инициализация времени "неактивности" пользователя
					}
				}
				else { //OSD отображено.
					if (keys & (1 << KEYB_RIGHT)) { //Кратковременное нажатие кнопки -> скрыть osd.
						AVI_osd = 0;
					}
					else if (keys & (1 << KEYB_UP)) { //Передвижение по меню OSD вверх на 1 пункт.
						if (m_p) {
							m_p--;
							MGL_ObjectMove(m_cur, 0, -13); //Вверх.
						}
					}
					else if (keys & (1 << KEYB_DOWN)) { //Передвижение по меню OSD вниз на 1 пункт.
						if (m_p < m_p_all - 1) {
								m_p++;
								MGL_ObjectMove(m_cur, 0, 13); //Вниз.
						}
					}
					else if ((keys & (1 << KEYB_F1)) || (keys & (1 << KEYB_F2))) { //Изменение опции под курсором.
						uint32_t pllN;
						switch (m_p) { //Выбор процедуры в зависимости от номера пункта меню под курсором.
							case 0: //Изменение формата цвета.
								AVI_color_mode = AVI_color_mode == 1 ? 3 : 1;
								my_strcpy(&text_clr_md[13], AVI_color_mode == 1 ? " Coloured" : "Grayscale");
								break;
							case 1: //Изменение частоты ядра: номинал 84 МГц, разгон 120 МГц
									//(изменяется и частота шин периферийных устройств).
//--------------------------- Разгон для STM32F401xB/C, STM32F401xD/E ----------------------------
#if defined(STM32F401xB) || defined(STM32F401xC) || defined(STM32F401xD) || defined(STM32F401xE)
								if (SystemCoreClock == 84000000) {
									LL_FLASH_SetLatency(LL_FLASH_LATENCY_3); 				//Переходя на повышенную частоту,
									while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_3) {  }	//предварительно увеличиваем задержку чтения флеш-памяти.
									pllN = 240;												//Множитель PLLN для увеличения частоты (разгона).
								}
								else {														//Переход на максимальную номинальную частоту.
									pllN = 168;												//Множитель PLLN для максимальной номинальной частоты.
								}
								LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSE);			//Выбираем HSE, как источник тактирования.
								while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSE) {  }	//Ожидаем переход на новый источник тактирования.
								LL_RCC_PLL_Disable();													//Выключаем модуль PLL.
								LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_25, pllN, LL_RCC_PLLP_DIV_2); //Меняем коэффициенты модуля PLL.
								LL_RCC_PLL_Enable();										//Включаем модуль PLL.
								while(LL_RCC_PLL_IsReady() != 1) {  }						//Ожидаем готовности модуля PLL.
								LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);			//Выбираем PLL, как источник тактирования.
								while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {  } //Ожидаем переход на новый источник тактирования.
								if (SystemCoreClock == 120000000) {							//Если переходоим на номинальную частоту, то уменьшаем задержку флеш-памяти
									LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);				//Задаем уменьшенную задержку.
									while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2) {  }	//Ждем активации заданного значения.
									SystemCoreClock = 84000000;								//Переопределяем частоту ядра на максимальную номинальную частоту.
								}
								else {														//Переход на повышенную частоту.
									SystemCoreClock = 120000000;							//Переопределяем частоту ядра на повышенную частоту.
								}
								SysTick_Config(SystemCoreClock/1000);						//В связи с изменением частоты ядра, перенастраиваем
																							//системный таймер SysTick для корректной работы счетчика "системного времени".
#endif //Разгон для STM32F401xB/C, STM32F401xD/E
								if (SystemCoreClock >= 100000000) {							//Значение частоты больше 100 МГц (три символа) ?
									utoa(SystemCoreClock/1000000, &text_fr_core[19], 10);	//Перевод частоты ядра в МГц и преобразование ее значения в строку текста.
								}
								else {
									text_fr_core[19] = ' ';									//Тут то же самое, но для частоты меньшей 100 МГц (два символа).
									utoa(SystemCoreClock/1000000, &text_fr_core[20], 10);	//Подвинем на 1 символ вправо значение частоты (для выравнивая
								}															//данных в окне OSD по правому краю).
								break;
							default:
								break;
						}
					}
					osd_time = millis; //Переинициализация времени "неактивности" пользователя.
				}
			}
			//----------------------------------------------------------------------------------------------------
		}
		//Если аудиопоток еще активен, то надо его выключить перед выходом из плеера:
		NVIC_DisableIRQ(AudioDAC_IRQn); //Выключаем прерывание, обслуживающее DAC, чтобы не было коллизий с player1,
										//которая может быть очищена в прерывании в самое неподходящее время.
		if (player1) {
			Stop_DAC(AudioDAC_I2S, AudioDAC_DMA, AudioDAC_DMA_Stream); //останавливаем DAC;
			PlayerStop(player1);		//выключаем плеер;
			PlayerDelete(player1);		//удаляем плеер;
			player1 = 0;				//инициализируем указатель на плеер.
		}
		NVIC_ClearPendingIRQ(AudioDAC_IRQn); //Очищаем отложенное прерывание от потока DMA DAC.
		NVIC_EnableIRQ(AudioDAC_IRQn); //Включаем прерывание, обслуживающее поток DMA DAC.
		f_close(&file1); //Закрываем файл file1.
		if (RHdr.AVIStreams) free(RHdr.AVIStreams); //Очищаем параметры потоков avi.
		MGL_GradientDelete(grad1); 		//Удаляем объекты MGL, используемые
		MGL_ObjectsListDelete(osd1);	//для создания OSD изображений
		MGL_ObjectsListDelete(osd2);
		return;
	}
	f_close(&file); //Закрываем файл file.
	if (RHdr.AVIStreams) free(RHdr.AVIStreams); //Очищаем параметры потоков avi.
}

/*
static void Render2D (LCD_Handler *lcd, MGL_OBJ *obj, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t mem_size = lcd->Width;
	uint8_t use_dma = 0;
	if (lcd->spi_data.dma_tx.dma) {
		mem_size *= 2;
		use_dma = 1;
	}
	uint16_t render_buf[mem_size];
	uint16_t *render_ptr = render_buf;
	MGL_OBJ *obj_ptr;
	LCD_SetActiveWindow(lcd, x1, y1, x2, y2);
	lcd->cs_control = lcd->dc_control = 1;
	LCD_ResCS(lcd);
	LCD_SetDC(lcd);
	for (uint16_t y = y1; y <= y2; y++)	{
		obj_ptr = obj;
		while (obj_ptr) {
			MGL_RenderObjects(obj_ptr, x1, y, x2, y, render_ptr);
			obj_ptr = obj_ptr->next;
		}
		if (use_dma) {
			while(LCD_GetState(lcd) != LCD_STATE_READY) {  }
			LCD_WriteDataDMA(lcd, render_ptr, (uint32_t)mem_size / 2);
			render_ptr = (render_ptr == render_buf) ? render_buf + mem_size / 2: render_buf;
		}
		else {
			LCD_WriteData(lcd, render_ptr, (uint32_t)mem_size);
		}
	}
	if (use_dma) {
		while(LCD_GetState(lcd) != LCD_STATE_READY) {  }
	}
	lcd->cs_control = lcd->dc_control = 0;
	LCD_SetCS(lcd);
}
*/

void AVIIntro(LCD_Handler *lcd)
{
	int dx = (lcd->Width - (30+70+45+5))/2;
	int dy = (lcd->Height - 45)/2;

	MGL_OBJ *intro = MGL_ObjectAdd(0, MGL_OBJ_TYPE_FILLRECTANGLE);
	MGL_SetRectangle(intro, 0, 0, lcd->Width - 1, lcd->Height - 1, COLOR_BLACK);

	MGL_GRADIENT *grad_sym = MGL_GradientCreate(MGL_GRADIENT_LINEAR);
	MGL_GradientAddColor(grad_sym,   0, COLOR_YELLOW, 1);
	MGL_GradientAddColor(grad_sym, 100, COLOR_RED,   1);
	MGL_GradientSetDeg(grad_sym, 0);

	MGL_OBJ *sym_A = MGL_ObjectAdd(intro, MGL_OBJ_TYPE_FILLTRIANGLE);
	MGL_SetTriangle(sym_A, 15, 0, 0, 45, 30, 45, 0);
	MGL_ObjectSetGradient(sym_A, grad_sym);

	MGL_OBJ *sym_V = MGL_ObjectAdd(intro, MGL_OBJ_TYPE_FILLTRIANGLE);
	MGL_SetTriangle(sym_V, 0+20, 0, 30+20, 0, 15+20, 45, 0);
	MGL_ObjectSetGradient(sym_V, grad_sym);

	MGL_OBJ *sym_I = MGL_ObjectAdd(intro, MGL_OBJ_TYPE_FILLRECTANGLE);
	MGL_SetRectangle(sym_I, 0+55, 0, 10+55, 45, 0);
	MGL_ObjectSetGradient(sym_I, grad_sym);

	MGL_TEXTURE texture_avatar = {(MGL_IMAGE *)&image_avatar, 0, 0};
	MGL_OBJ *avatar = MGL_ObjectAdd(intro, MGL_OBJ_TYPE_FILLCIRCLE);
	MGL_SetCircle(avatar, 0+70+45/2, 45/2, 45/2, 0);
	MGL_ObjectSetTexture(avatar, &texture_avatar);

	MGL_OBJ *play = MGL_ObjectAdd(intro, MGL_OBJ_TYPE_FILLTRIANGLE);
	MGL_SetTriangle(play, 0+70+45+5, 0+7, 0+70+45+5, 30+7, 30+70+45+5, 15+7, 0);
	MGL_ObjectSetGradient(play, grad_sym);

	MGL_ObjectListMove(sym_A, dx, dy);

	char txt_prnm[] = "STM32 AVI Player";
	char txt_cprt[] = "(C)2023 VadRoV";
	MGL_OBJ *prnm = MGL_ObjectAdd(intro, MGL_OBJ_TYPE_TEXT);
	MGL_SetText(prnm, lcd->Width, dy - 13, txt_prnm, &Font_8x13, 0, COLOR_WHITE);
	MGL_OBJ *cprt = MGL_ObjectAdd(intro, MGL_OBJ_TYPE_TEXT);
	MGL_SetText(cprt, -8 * my_strlen(txt_cprt) - 8 * (my_strlen(txt_prnm) - my_strlen(txt_cprt)) / 2, dy + 45, txt_cprt, &Font_8x13, 0, COLOR_WHITE);

	AVI_osd = intro;

	int tr = 255, tr1 = 0;
	int x = lcd->Width - dx - ((30+70+45+5) - 8 * my_strlen(txt_prnm))/2;
	while (x > 0) {
		MGL_ObjectSetTransparency(intro, tr1);
		MGL_ObjectListTransparency(sym_A, tr);
		MGL_ObjectMove(prnm, -4, 0);
		MGL_ObjectMove(cprt, +4, 0);
		if (LCD_Load_JPG(lcd, 0, 0, lcd->Width, lcd->Height, &fon_file, PICTURE_IN_MEMORY)) break;
		texture_avatar.alpha += 8;
		if (texture_avatar.alpha > 360) texture_avatar.alpha = 360;
		tr -= 8;
		tr1 += 10;
		if (tr < 0) tr = 0;
		if (tr1 > 255) {
			MGL_ObjectSetVisible(intro, 0);
		}
		x -= 4;
	}

	LL_mDelay(2000);

	tr = 0;
	while (tr <= 255) {
		if (LCD_Load_JPG(lcd, 0, 0, lcd->Width, lcd->Height, &fon_file, PICTURE_IN_MEMORY)) break;
		MGL_ObjectListMove(sym_A, 0, -4);
		tr += 15;
		MGL_ObjectListTransparency(sym_A, tr);
	}

	MGL_GradientDelete(grad_sym);
	MGL_ObjectsListDelete(intro);
	AVI_osd = 0;
}
