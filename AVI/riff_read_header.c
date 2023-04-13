/*
 *  Библиотека работы с аудио-форматами (кодеки)
 *
 *  Author: VadRov
 *  Copyright (C) 2019, VadRov, all right reserved.
 *
 *  Допускается свободное распространение.
 *  При любом способе распространения указание автора ОБЯЗАТЕЛЬНО.
 *  В случае внесения изменений и распространения модификаций указание первоначального автора ОБЯЗАТЕЛЬНО.
 *  Распространяется по типу "как есть", то есть использование осуществляется на свой страх и риск.
 *  Автор не предоставляет никаких гарантий.
 *
 *  https://www.youtube.com/c/VadRov
 *  https://zen.yandex.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 *
 */
#include "riff_read_header.h"
#include <stdlib.h>

//Чтение заголовка файла с контейнером riff
//на входе указатель на структуру riffHeader с информацией о параметрах воспроизведения
//на выходе статус
RIFF_HDR_STATUS Read_RIFF_Header (RiffHDR *RHdr, FIL *file)
{
	Riff_Chunk chunk;
	UINT b_read = 0;
	FRESULT res;
	res = f_read(file, &chunk, 12, &b_read); //читаем первые 12 байт заголовка
	if (res || b_read != 12) return RIFF_IO_ERROR;
	if (chunk.chunk_id != chunk_RIFF) return RIFF_FMT_ERROR; //проверка на чанк RIFF
	if (chunk.chunk_type != TYPE_AVI) return RIFF_FMT_ERROR; //проверка на тип AVI
	RHdr->AllStreams = 0; //инициализация количества потоков в AVI
	FSIZE_t len_stream = 0;
	while (file->fptr < file->obj.objsize) { //читаем чанки из AVI пока не просмотрим весь файл
		res = f_read(file, &chunk, 12, &b_read); //читаем текущий чанк с информацией о id, размере и типе
		if (res || b_read != 12) return RIFF_IO_ERROR;
		switch (chunk.chunk_id) { //проверяем чанк по id
			case chunk_LIST:	//LIST
				switch(chunk.chunk_type) { //проверяем тип:
					case chunk_hdrl:	//это загрузчик AVI
						res = f_read(file, &RHdr->AVIHeader, sizeof(MainAVIHeader), &b_read);
						if (res || b_read != sizeof(MainAVIHeader)) return RIFF_IO_ERROR;
						if (RHdr->AVIHeader.ChunID != chunk_avih) return RIFF_FMT_ERROR;
						break;
					case chunk_strl: //это новый поток AVI
						len_stream = chunk.chunk_size - 4;
						RHdr->AllStreams++; //увеличиваем счетчик потоков в AVI
						//переопределяем размер массива с потоками AVI
						RHdr->AVIStreams = (AVIStreamHeader *)realloc(RHdr->AVIStreams, RHdr->AllStreams * sizeof(AVIStreamHeader));
						res = f_read(file, &chunk, 8, &b_read); //читаем следующий чанк
						if (res || b_read != 8) return RIFF_IO_ERROR;
						len_stream -= 8;
						if (chunk.chunk_id != chunk_strh) return RIFF_FMT_ERROR; //им должен быть чанк strh - загрузчик потока
						//читаем загрузчик потока
						res = f_read(file, &RHdr->AVIStreams[RHdr->AllStreams-1], sizeof(AVIStreamHeader), &b_read);
						if (res || b_read != sizeof(AVIStreamHeader)) return RIFF_IO_ERROR;
						len_stream -= sizeof(AVIStreamHeader);
						res = f_read(file, &chunk, 8, &b_read); //читаем следующий чанк
						if (res || b_read != 8) return RIFF_IO_ERROR;
						len_stream -= 8;
						if (chunk.chunk_id != chunk_strf) return RIFF_FMT_ERROR; //им должен быть чанк strf
						switch(RHdr->AVIStreams[RHdr->AllStreams-1].fccType) { //в зависимости от типа потока, считываем его загрузчик
							case chunk_vids: //это видеопоток
								res = f_read(file, &RHdr->vidFmt, sizeof(BITMAPINFOHEADER), &b_read);
								if (res || b_read != sizeof(BITMAPINFOHEADER)) return RIFF_IO_ERROR;
								len_stream -= sizeof(BITMAPINFOHEADER);
								break;
							case chunk_auds: //это аудиопоток
								res = f_read(file, &RHdr->audFmt, sizeof(WAVEFORMAT), &b_read);
								if (res || b_read != sizeof(WAVEFORMAT)) return RIFF_IO_ERROR;
								len_stream -= sizeof(WAVEFORMAT);
								break;
							default: //пропускаем неизвестный/ненужный поток
								break;
						}
						f_lseek(file, file->fptr + len_stream); //пропускаем непрочитанные данные
						break;
					case chunk_movi: //чанк movi - после него следуют аудио и видеокадры
						RHdr->lenghtData = chunk.chunk_size; //запоминаем длину чанка и
						RHdr->startPosData = file->fptr;	 //местоположение старта кадров
						f_lseek(file, file->fptr + chunk.chunk_size - 4);
						break;
					default:
						f_lseek(file, file->fptr + chunk.chunk_size - 4);
						break;
				}
				break;
			case chunk_idx1: //чанк с индексной информацией (для перемотки), запоминаем его местоположение
				RHdr->lenghtIdx1 = chunk.chunk_size;
				RHdr->startPosIdx1 = file->fptr - 4;
				f_lseek(file, file->fptr + chunk.chunk_size - 4); //пропускаем индексную информацию
				break;
			default: //пропускаем неизвестный/ненужный чанк
				f_lseek(file, file->fptr + chunk.chunk_size - 4);
				break;
		}
	}
	return RIFF_OK;
}
