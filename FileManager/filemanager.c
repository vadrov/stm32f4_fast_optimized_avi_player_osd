/* ------------------------------------------------------------------------------------/
 /     Модуль файлового менеджера, реализованный с применением библиотеки FATFS
 * ------------------------------------------------------------------------------------/
 *  Автор: VadRov
 *  Copyright (C) 2021, VadRov, all right reserved.
 *
 *  Допускается свободное распространение.
 *  При любом способе распространения указание автора ОБЯЗАТЕЛЬНО.
 *  В случае внесения изменений и распространения модификаций указание первоначального автора ОБЯЗАТЕЛЬНО.
 *  Распространяется по типу "как есть", то есть использование осуществляется на свой страх и риск.
 *  Автор не предоставляет никаких гарантий.
 *
 *		Основные возможности файлового менеджера:
 *  - Вывод менеджера в окне, параметры (координата верхнего левого угла, ширина и высота окна, шрифт, цвет)
 *    которого задает пользователь;
 *  - Сокращение имен файлов и каталогов (если они не помещаются в окне) по типу менеджера Total Commander;
 *  - Нет ограничений на количество файлов и подкаталогов в каталоге;
 *  - Запоминание позиции курсора в вышестоящем каталоге и восстановление этой позиции при возврате на него
 *    с вложенного подкаталога;
 *  - Вывод иконок (значков) файлов и каталогов;
 *  - Поддержка вложенных каталогов с уровнем вложенности, ограниченным размером оперативной памяти
 *    микроконтроллера.
 *
 *		Модуль базируется на библиотеках автора:
 *  - драйвер работы с SD-картой по SPI c использованием DMA (Авторы: ChaN (2014), kiwih (2017), VadRov (2019),
 *    каталог FATFS)
 *    https://www.youtube.com/watch?v=z99bLhlnukM
 *  - драйвер для работы с дисплеями по SPI с использованием DMA (каталог Display)
 *    https://www.youtube.com/watch?v=8tIJ16riJqo
 *  - модуль для работы с кнопками с фильтром дребезга, автоповтором и буферризацией ввода (каталог Keyboard)
 *    https://www.youtube.com/watch?v=e-w5HS75neg
 *  - процедуры работы со строками (каталог MyString).
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 *
 */

#include "ff.h" 		//библиотека FATFS
#include "mystring.h" 	//процедуры для работы со строками
#include "keyboard.h" 	//модуль работы с кнопками
#include "icons.h"	    //модуль со значками файлов и каталогов
#include "filemanager.h"
#include <string.h>
#include <stdlib.h>

#define DISABLE_IRQ		__disable_irq()
#define ENABLE_IRQ		__enable_irq()

const char FM_audio_types[]   = ".mpa.MPA.m3u.M3U.asf.ASF.amr.AMR.aif.AIF.aiff.AIFF.ape.APE.wav.WAV.wave.WAVE.mp3.MP3.wma.WMA.aac.AAC.m4a.M4A.flac.FLAC.ogg.OGG.ac3.AC3.mid.MID.midi.MIDI.mod.MOD.";
const char FM_picture_types[] = ".pic.PIC.pct.PCT.avif.AVIF.webp.WEBP.svg.SVG.raw.RAW.ico.ICO.eps.EPS.bmp.BMP.png.PNG.jpg.JPG.jpeg.JPEG.jpe.JPE.jfif.JFIF.gif.GIF.tga.TGA.tiff.TIFF.pcx.PCX.psd.PSD.dib.DIB.";
const char FM_data_types[]    = ".epub.EPUB.fb2.FB2.pptx.PPTX.ppt.PPT.log.LOG.tex.TEX.text.TEXT.djvu.DJVU.txt.TXT.dat.DAT.pdf.PDF.c.C.cpp.CPP.h.H.doc.DOC.docx.DOCX.rtf.RTF.py.PY.xls.XLS.bas.BAS.js.JS.html.HTML.php.PHP.";
const char FM_video_types[]   = ".webm.WEBM.vob.VOB.wmv.WMV.ts.TS.rm.RM.h264.H264.avi.AVI.mov.MOV.mkv.MKV.3gp.3GP.mpeg.MPEG.mpg.MPG.mp4.MP4.mjpeg.MJPEG.";

/* Поиск расширения файла в строке с проверямыми расширениями
*  name - строка с именем файла
*  str_types - строка с проверяемыми расширениями
*  возвращает 1, если файл имеет одно из расширений, указанных в строке str_types, иначе - 0 */
static uint8_t findext(char *name, const char *str_types)
{
	//определяем расширение
	int j = my_strlen(name), f = 0;
	while (j--) {
		if (name[j] == '.')	{
			f = 1;
	   		break;
	   	}
	}
	if (!f) return 0;
	char str[my_strlen(&name[j]) + 2];
	my_strcpy(str, &name[j]);
	my_strcat(str, ".");
	if (strstr(str_types, str)) return 1;
	return 0;
}

/* Преобразует DOS-овскую кодировку (CP866) в кодировку CP1251 */
void Dos866toCp1251 (char *fn)
{
	for(int i = 0; i < strlen(fn); i++)	{
		if ((fn[i] >= 128) && (fn[i] <= 175)) fn[i] += 64;  //A...Я,а..п
		else if ((fn[i] >= 224) && (fn[i] <= 239)) fn[i] += 16; //р..я
		else if (fn[i] == 240) fn[i] = 168; // Ё
		else if (fn[i] == 241) fn[i] = 184;// ё
	}
}

/* Возвращает тип файла в зависимости от его расширения */
static File_Manager_File_Types filetype (char *name)
{
	if (findext(name, FM_audio_types))   return FILE_TYPE_AUDIO;
	if (findext(name, FM_picture_types)) return FILE_TYPE_PICTURE;
	if (findext(name, FM_data_types))    return FILE_TYPE_DATA;
	if (findext(name, FM_video_types))   return FILE_TYPE_VIDEO;
	return FILE_TYPE_OTHER;
}

/* Выводит на дисплей значок файла/каталога.
   Размер значка приводится (масштабируется) к размеру шрифта. */
static void DrawIcon(LCD_Handler *lcd, uint16_t x, uint16_t y, const Icon_Image *icon, int w, int h)
{
	uint16_t color;
	int i, j, i1, j1;
	SPI_TypeDef *spi = lcd->spi_data.spi;
	LCD_SetActiveWindow(lcd, x, y, x + w - 1, y + h - 1);
	LCD_ResCS(lcd);
	LCD_SetDC(lcd);
	spi->CR1 &= ~SPI_CR1_SPE; // SPI выключаем, чтобы изменить параметры
	spi->CR1 &= ~ (SPI_CR1_BIDIMODE |  	//здесь задаем режим
				   SPI_CR1_RXONLY |   	//  Transmit only
				   SPI_CR1_CRCEN | 		//выключаем аппаратный расчет CRC
				   SPI_CR1_DFF); 		//8-битная передача
	if (lcd->data_bus == LCD_DATA_16BIT_BUS) {
		spi->CR1 |= SPI_CR1_DFF; //16-битная передача
	}
	spi->CR1 |= SPI_CR1_SPE; // SPI включаем
	//масштабирование значка методом ближайшего соседа (простая линейная интерполяция)
	for (i = 0; i < h; i++) {
		i1 = (icon->height * i) / h;
		for (j = 0; j < w; j++) {
			j1 = (icon->width * j) / w;
			color = *(icon->data + i1 * icon->width + j1);
			while (!(spi->SR & SPI_SR_TXE)) {  } //ждем окончания передачи
			if (lcd->data_bus == LCD_DATA_16BIT_BUS) {
				spi->DR = color;
			}
			else {
				uint8_t color1 = color & 0xFF, color2 = color >> 8;
				*((volatile uint8_t *)&spi->DR) = color1;
				while (!(spi->SR & SPI_SR_TXE)) {  } //ждем окончания передачи
				*((volatile uint8_t *)&spi->DR) = color2;
			}
		}
	}
	while (!(spi->SR & SPI_SR_TXE)) {  } //ждем окончания передачи
	while (spi->SR & SPI_SR_BSY) {  } //ждем когда SPI освободится
	//выключаем spi
	spi->CR1 &= ~SPI_CR1_SPE;
	LCD_SetCS(lcd);
}

/* Назначает обработчик дисплея для файлового менеджера */
static inline void FileManagerSetDisplay(File_Manager_Handler *fm, LCD_Handler *lcd)
{
	fm->lcd = lcd;
}

/* Устанавливает параметры окна (координаты, ширина, высота) для файлового менеджера */
static inline void FileManagerSetWin(File_Manager_Handler *fm, int x, int y, int win_wdt, int win_hgt)
{
	fm->x = x;
	fm->y = y;
	fm->win_wdt = win_wdt;
	fm->win_hgt = win_hgt;
}

/* Устанавливает цветовую схему файлового менеджера */
static inline void FileManagerSetColor(File_Manager_Handler *fm, File_Manager_Color_Scheme *color_scheme)
{
	fm->color_scheme = color_scheme;
}

/* Устанавливает шрифт для файлового менеджера */
static inline void FileManagerSetFont(File_Manager_Handler *fm, FontDef *font)
{
	fm->font = font;
}

/* Устанавливает номера битов кнопок управления: вверх, вниз, ввод */
static inline void FileManagerSetKeys(File_Manager_Handler *fm, uint8_t key_up, uint8_t key_down, uint8_t key_enter)
{
	fm->key_up = key_up;
	fm->key_down = key_down;
	fm->key_enter = key_enter;
}

/* Определяет значение флага активности режима выбора файлов. Флаг определяет включена
   или выключена возможность перемещения по каталогам и файлам, выбора файлов. */
static inline void FileManagerChangeActiveFlag(File_Manager_Handler *fm, File_Manager_Active_Flag fl)
{
	fm->fl_no_use = fl;
}

/* Возвращает статус завершения последнего вызова файлового менеджера */
static inline uint8_t FileManagerGetStatus(File_Manager_Handler *fm)
{
	return fm->status;
}

/* Возвращает указатель на строку с путем до выбранного файла */
static inline char* FileManagerGetFilePath(File_Manager_Handler *fm)
{
	return fm->dir_name;
}

/* Возвращает указатель на строку с именем выбранного файла */
static inline char* FileManagerGetFileName(File_Manager_Handler *fm)
{
	return fm->file_name;
}

/*------------------------------ Файловый менеджер -----------------------------
  При успешном завершении (status = File_Manager_OK) возвращает путь до файла в переменной dir_name,
  имя выбранного файла в переменной file_name структуры обработчика. Статус завершения -
  в переменной status структуры обработчика. Статус можно получить функцией fm->GetStatus(fm),
  где fm - указатель на обработчик. Указатель на путь можно получить функцией fm->GetFilePath(fm),
  где fm - указатель на обработчик. Указатель на имя файла можно получить функцией fm->GetFileName(fm),
  где fm - указатель на обработчик. */
static void FileManagerShow(File_Manager_Handler *fm)
{
	int sym_to_x = fm->win_wdt / fm->font->width; //символов шрифта по x
	int sym_to_y = fm->win_hgt / fm->font->height; //символов шрифта по y
	/* Проверка параметров окна вывода файлового менеджера */
	if (fm->x < 0 ||							/* ------------------------------ */
		fm->y < 0 ||							/* Выход за */
		fm->x + fm->win_wdt > fm->lcd->Width || /* границы дисплея ? */
		fm->y + fm->win_hgt > fm->lcd->Height|| /* ------------------------------ */
		sym_to_x < 3 ||							/* ширина окна минимум 3 колонки текста */
		sym_to_y < 1  ) {						/* высота окна минимум 1 строка текста */

		fm->status = File_Manager_Error_Win;
		return;
	}
	char name[FF_MAX_LFN + 1];
	FRESULT res;
	DIR Dir;
	FILINFO Fno, Fno_cur;
	uint32_t keys = 0;
	const Icon_Image *icon;
	File_Manager_Color_Scheme *color_scheme = fm->color_scheme;
	int win_wdt_int = sym_to_x * fm->font->width;
	int win_hgt_int = sym_to_y * fm->font->height;
/* точка входа при переходе/возврате на/с уровень/уровня вниз/вверх */
filemanager1:
	DISABLE_IRQ;
	res = f_opendir(&Dir, fm->dir_name);
	ENABLE_IRQ;
	if (res != FR_OK) {
		fm->status = File_Manager_Error_Disk;
		return;
	}
	if (!fm->fl_last_selection)	{
		fm->current_pos = fm->current_pos_buf[fm->depth];
		fm->pos_y = fm->pos_y_buf[fm->depth];
	}
    /* подсчет количества файлов и подкаталогов в текущем каталоге */
    int all_files = 1; /* счетчик файлов и каталогов с 1, т.к. учитываем отображение пункта возврата на уровень вверх */
    while(1) {
    	DISABLE_IRQ;
    	res = f_readdir(&Dir, &Fno);
    	ENABLE_IRQ;
    	if (res || !Fno.fname[0]) break;
    	if (!(Fno.fattrib & (AM_HID | AM_SYS))) /* не учитываем скрытые и системные файлы (их отображать не будем) */
    		all_files++;
    }
	/* определяем высоту полосы вертикальной прокрутки */
	int vert_scroll_hgt = sym_to_y * win_hgt_int / all_files;
	/* если высота полосы прокрутки меньше высоты символа, то высота полосы = высоте символа */
	if (vert_scroll_hgt < fm->font->height) vert_scroll_hgt = fm->font->height;
	uint8_t vert_scroll_enable = 0;
	if ((all_files > sym_to_y) && (sym_to_x > 3)) vert_scroll_enable = 1;
    while(1) {
    	DISABLE_IRQ;
    	res = f_readdir(&Dir, 0);
    	ENABLE_IRQ;
    	if ( res != FR_OK) break;
       	int i = 0, yyy = 0;
       	while (i < all_files) {
       		name[0] = 0;
   			if (i) {
   				DISABLE_IRQ;
   				res = f_readdir(&Dir, &Fno);
   				ENABLE_IRQ;
   				if (res || !Fno.fname[0]) break;
   				/* скрытый или системный пропускаем */
   				if ((Fno.fattrib & (AM_HID | AM_SYS))) continue;
   			}
   			/* позиция чтения попадает в область вывода ? */
   			if ((i >= fm->current_pos - fm->pos_y) &&
   				(i < fm->current_pos - fm->pos_y + sym_to_y)) {
   				uint32_t text_color = color_scheme->text_color_file;
   				/* на уровень вверх? */
   				if (!i) {
   					my_strcpy_fixed_sym(name, "..", sym_to_x - 1);
   					icon = &Icon_Prev;
   				}
   				else {
   					/* ширина строки меньше или равна ширине окна? */
   					if (my_strlen(Fno.fname) <= sym_to_x - 1 - vert_scroll_enable)
   						my_strcpy(name, Fno.fname);
   					else {
   						if (Fno.fattrib & AM_DIR) { /* каталог? */
   							my_strcpy_fixed_sym(name, Fno.fname, sym_to_x - 1 - 2 - vert_scroll_enable);
   							my_strcat(name, "..");
   						}
   						else { /* не каталог */
   							int j = my_strlen(Fno.fname), f = 0;
   							while (j) {
   								if (Fno.fname[j-1] == '.') {
   									f = 1;
   									break;
   								}
   								j--;
   							}
   							if (!f || j == 1)	{
   								my_strcpy_fixed_sym(name, Fno.fname, sym_to_x - 1 - 2 - vert_scroll_enable);
   								my_strcat(name, "..");
   							}
   							else {
   								my_strcpy_fixed_sym(name, Fno.fname, sym_to_x - 1 - 2 - vert_scroll_enable - my_strlen(Fno.fname + j));
   								my_strcat_fixed_sym(name, "..", sym_to_x - 1 - vert_scroll_enable - (my_strlen(name) + my_strlen(Fno.fname + j)));
   								my_strcat_fixed_sym(name, Fno.fname + j, sym_to_x - 1 - vert_scroll_enable - my_strlen(name));
   							}
   						}
   					}
   					/* если каталог, то рисуем перед его именем значок папки */
       				if (Fno.fattrib & AM_DIR) {
       					icon = &Icon_Catalog;
       					text_color = color_scheme->text_color_dir;
       				}
       				/* если не каталог, то перед каждым названием файла рисуем значок, соответствующий его типу */
       				else {
       					File_Manager_File_Types f_type = filetype(Fno.fname);
       					if (f_type == FILE_TYPE_AUDIO) icon = &Icon_Audio;
       					else if (f_type == FILE_TYPE_PICTURE) icon = &Icon_Picture;
       					else if (f_type == FILE_TYPE_DATA) icon = &Icon_Text;
       					else if (f_type == FILE_TYPE_VIDEO) icon = &Icon_Video;
       					else icon = &Icon_Other;
       				}
       				/* запоминаем информацию о текущем файле/каталоге */
       				if (i == fm->current_pos) {
       					memcpy(&Fno_cur, &Fno, sizeof(FILINFO));
       				}
       			}
   				/* выводим значок файла/каталога (c масштабированием, если размер значка отличается от размера шрифта) */
   				DrawIcon(fm->lcd, fm->x, fm->y + yyy, icon, fm->font->width, fm->font->height);
   				/* меняем кодировку с cp866 на cp1251, для корректного отображения имени файла на дисплее */
       			Dos866toCp1251(name);
    			/* при необходимости дополним пробелами строку с названием файла/подкаталога */
    			int j = my_strlen(name);
    			int len_sym_add = sym_to_x - 1 - vert_scroll_enable;
    			while (j < len_sym_add)	{
    				name[j] = ' ';
    				j++;
    			}
    			name[j] = 0; /* символ конца строки */
    			/* выводим название файла/подкаталога с учетом позиции и цветовой схемы */
    			uint32_t bg_color = color_scheme->bg_line1_color;
    			if (i % 2) bg_color = color_scheme->bg_line2_color;
   				if (i == fm->current_pos) {
   					text_color = color_scheme->text_color_cursor;
   					if (color_scheme->fl_cursor_fill) bg_color = color_scheme->cursor_color;
   				}
   				LCD_WriteString(fm->lcd,
    							fm->x + fm->font->width,
								fm->y + yyy,
								name,
								fm->font,
								text_color,
								bg_color,
								LCD_SYMBOL_PRINT_FAST);
    			yyy += fm->font->height; /* переходим к следующей строке окна */
       		}
   			i++; /* к следующему файлу/подкаталогу */
       	}
       	/* дописываем при необходимости пустые строки в окно */
       	if (yyy < win_hgt_int) {
       		LCD_FillWindow( fm->lcd,
       						fm->x,
							fm->y + yyy,
							fm->x + win_wdt_int - vert_scroll_enable * fm->font->width - 1,
							fm->y + win_hgt_int - 1,
							color_scheme->bg_color);
       	}
       	/* выход, если режим выбора файлов неактивен */
       	if (fm->fl_no_use == File_Manager_Passive) return;
       	/* прямоугольник вокруг имени выбранного файла/подкаталога */
       	if (!color_scheme->fl_cursor_fill) {
       		LCD_DrawRectangle(	fm->lcd,
       							fm->x,
								fm->y + fm->pos_y * fm->font->height,
								fm->x + win_wdt_int - vert_scroll_enable * fm->font->width - 1,
								fm->y + (fm->pos_y + 1) * fm->font->height - 1,
								color_scheme->cursor_color);
       	}
       	/* вертикальная полоса прокрутки окна менеджера активна, если
       	   число строк, помещающихся в окне, меньше количества файлов/каталогов */
       	if (vert_scroll_enable)	{
       		/* позиция полосы прокрутки */
       		int pos_scroll_y = (fm->current_pos - fm->pos_y) * (win_hgt_int - vert_scroll_hgt) / (all_files - sym_to_y);
       		/* верхняя часть полосы прокрутки */
       		if (pos_scroll_y) {
       			LCD_DrawFilledRectangle(fm->lcd,
           					  	    	fm->x + win_wdt_int - fm->font->width,
										fm->y,
										fm->x + win_wdt_int - 1,
										fm->y + pos_scroll_y - 1,
										color_scheme->stripe_color);
       		}
       		/* ползунок полосы прокрутки */
       		LCD_DrawFilledRectangle(fm->lcd,
           					  	    fm->x + win_wdt_int - fm->font->width,
									fm->y + pos_scroll_y,
									fm->x + win_wdt_int - 1,
									fm->y + pos_scroll_y + vert_scroll_hgt - 1,
									color_scheme->slider_color);
       		/* нижняя часть полосы прокрутки */
       		if (pos_scroll_y + vert_scroll_hgt < win_hgt_int) {
       			LCD_DrawFilledRectangle(fm->lcd,
       					  	    		fm->x + win_wdt_int - fm->font->width,
										fm->y + pos_scroll_y + vert_scroll_hgt,
										fm->x + win_wdt_int - 1,
										fm->y + win_hgt_int - 1,
										color_scheme->stripe_color);
       		}
       	}
       	/* ожидание нажатия нужных кнопок при выполнении определенных условий */
       	do {
       		keys = KEYB_Inkeys();
       		if (((keys & (1<<fm->key_down)) && (fm->current_pos < all_files - 1)) ||
       			((keys & (1<<fm->key_up)) && (fm->current_pos > 0))	) break;
       	} while (!(keys & (1<<fm->key_enter)));
       	/* движение по каталогу вниз */
       	if (keys & (1<<fm->key_down)) {
       		fm->pos_y++;
       		if ((fm->pos_y >= sym_to_y) ||
       			(fm->pos_y >= all_files) ) fm->pos_y--;
       		fm->current_pos++;
       	}
       	/* движение по каталогу вверх */
       	if (keys & (1<<fm->key_up)) {
       		fm->pos_y--;
       		if (fm->pos_y < 0) fm->pos_y = 0;
       		fm->current_pos--;
       	}
       	/* выбор элемента каталога */
       	if (keys & (1<<fm->key_enter)) {
       		/* возврат на уровень вверх */
       		if (!fm->current_pos) {
       			if (fm->depth) {
       				int l = my_strlen(fm->dir_name) - 1, k = 0;
       				while (l >= 0 && !k) {
       					if (fm->dir_name[l] == '/') k++;
       					fm->dir_name[l] = 0;
       					l--;
       				}
       				fm->dir_name = (char *)realloc(fm->dir_name, l + 2);
       				fm->current_pos_buf = (int *)realloc(fm->current_pos_buf, fm->depth * sizeof(int));
       				fm->pos_y_buf = (int *)realloc(fm->pos_y_buf, fm->depth * sizeof(int));
       				fm->depth--;
       			}
       			else {
       				fm->current_pos_buf[0] = 0;
       				fm->pos_y_buf[0] = 0;
       			}
       			fm->fl_last_selection = 0;
       			DISABLE_IRQ;
       			f_closedir(&Dir);
       			ENABLE_IRQ;
       			goto filemanager1;
       		}
       		/* переход в подкаталог */
       		if (Fno_cur.fattrib & AM_DIR) {
       			fm->dir_name = (TCHAR *)realloc(fm->dir_name, my_strlen(fm->dir_name) + 1 + 1 + my_strlen(Fno_cur.fname));
       			my_strcat(fm->dir_name, "/");
       			my_strcat(fm->dir_name, Fno_cur.fname);
       			fm->current_pos_buf[fm->depth] = fm->current_pos;
       			fm->pos_y_buf[fm->depth] = fm->pos_y;
       			fm->depth++;
       			fm->current_pos_buf = (int *)realloc(fm->current_pos_buf, (fm->depth + 1) * sizeof(int));
       			fm->pos_y_buf = (int *)realloc(fm->pos_y_buf, (fm->depth + 1) * sizeof(int));
       			fm->current_pos_buf[fm->depth] = 0;
       			fm->pos_y_buf[fm->depth] = 0;
       			fm->fl_last_selection = 0;
       			DISABLE_IRQ;
       			f_closedir(&Dir);
       			ENABLE_IRQ;
       			goto filemanager1;
       		}
       		/* Файл выбран */
       		else {
       		    if (fm->file_name) free(fm->file_name);
       		    fm->file_name = my_strcpy_mem(Fno_cur.fname);
       		    fm->status = File_Manager_OK;
       		    fm->fl_last_selection = 1;
       		    DISABLE_IRQ;
       		    f_closedir(&Dir);
       		    ENABLE_IRQ;
       			return;
       		}
       		/*--------------------------------------------------------------------*/
       	}
    }
    fm->status = File_Manager_Error_Disk;
	return;
}

/* Создает обработчик файлового менеджера и возвращает указатель на него */
File_Manager_Handler* FileManagerNew(void)
{
	File_Manager_Handler *fm = (File_Manager_Handler*)calloc(1, sizeof(File_Manager_Handler));
	fm->current_pos_buf = (int *)calloc(1, sizeof(int));
	fm->pos_y_buf = (int *)calloc(1, sizeof(int));
	fm->dir_name = my_strcpy_mem("");
	fm->SetDisplay = FileManagerSetDisplay;
	fm->SetWin = FileManagerSetWin;
	fm->SetColor = FileManagerSetColor;
	fm->SetFont = FileManagerSetFont;
	fm->SetKeys = FileManagerSetKeys;
	fm->ChangeActiveFlag = FileManagerChangeActiveFlag;
	fm->Show = FileManagerShow;
	fm->GetStatus = FileManagerGetStatus;
	fm->GetFilePath = FileManagerGetFilePath;
	fm->GetFileName = FileManagerGetFileName;
	return fm;
}

/* Удаляет обработчик файлового менеджера  */
void FileManagerDelete(File_Manager_Handler *fm)
{
	if (fm) {
		if (fm->current_pos_buf) free(fm->current_pos_buf);
		if (fm->pos_y_buf)		 free(fm->pos_y_buf);
		if (fm->dir_name) 		 free(fm->dir_name);
		if (fm->file_name)		 free(fm->file_name);
		free(fm);
	}
}

/* Создает эффект "всплывающего" окна файлового менеджера
   p_millis определяет задержку (в миллисекундах) анимации */
void FileManagerPopupWin(File_Manager_Handler *fm, uint32_t p_millis)
{
	/* сохранение параметров окна менеджера */
	int x_w = fm->x, y_w = fm->y, w_w = fm->win_wdt, h_w = fm->win_hgt;
	int font_width = fm->font->width, font_height = fm->font->height;
	w_w = font_width * (w_w / font_width);
	h_w = font_height * (h_w / font_height);
	/* стартовые ширина и высота окна */
	int wdt0 = 3 * font_width;
	int hgt0 = 1 * font_height;
	/* проверка допустимости параметров окна */
	if (wdt0 > w_w || hgt0 > h_w) return;
	int x0, y0;
	/* количество шагов приращения ширины и высоты окна */
	int step1 = ((w_w - wdt0) / 2) / font_width;
	int step2 = ((h_w - hgt0) / 2) / font_height;
	/* максимальное число шагов */
	int step = (step1 > step2) ? step1: step2;
	/* выключение возможности выбора файлов */
	fm->ChangeActiveFlag(fm, File_Manager_Passive);
	/* запуск цикла анимации, в котором за step шагов
	   увеличиваем размеры окна до заданных */
	for (int i = 0; i < step; i++) {
		/* позиция левого верхнего угла окна */
		x0 = x_w + (w_w - wdt0) / 2;
		y0 = y_w + (h_w - hgt0) / 2;
		/* обновление параметров окна менеджера */
		fm->SetWin(fm, x0, y0, wdt0, hgt0);
		/* вывод окна менеджера */
		fm->Show(fm);
		/* задержка анимации */
		LL_mDelay(p_millis);
		/* увеличение размеров окна */
		wdt0 += 2 * font_width;
		hgt0 += 2 * font_height;
		/* проверка новых размеров окна на выход за пределы заданных */
		if (wdt0 > w_w) wdt0 -= 2 * font_width;
		if (hgt0 > h_w) hgt0 -= 2 * font_height;
	}
	/* включение возможности выбора файлов */
	fm->ChangeActiveFlag(fm, File_Manager_Active);
	/* восстановление параметров окна менеджера */
	fm->SetWin(fm, x_w, y_w, w_w, h_w);
}
