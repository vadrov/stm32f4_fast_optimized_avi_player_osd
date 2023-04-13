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

#include "mp3_callback.h"
#include "riff_read_header.h"
#include <string.h>

static int mp3_process_decode(Player *player);
static void mp3_mono_to_dual_channel(uint16_t *outbuf, int samples);

PlayerReadHeaderStatus mp3_read_header_callback (Player *player)
{
	const uint16_t samplerateTab[3][3] = {
	  {44100, 48000, 32000},		/* MPEG-1 */
	  {22050, 24000, 16000},		/* MPEG-2 */
	  {11025, 12000,  8000},		/* MPEG-2.5 */
	};
	const uint16_t samplesPerFrameTab[3][3] = {
		{384, 1152, 1152 }, /* MPEG1 */
		{384, 1152,  576 }, /* MPEG2 */
		{384, 1152,  576 }, /* MPEG2.5 */
	};
	Player_client_data *client_data = (Player_client_data*)player->client_data;
	Player_client_mp3 *decoder = (Player_client_mp3 *)client_data->client_data_decoder;
	AudioConfig *audioCFG = &player->audioCFG;
	FIL *file = (FIL *)client_data->file;
	decoder->startPosData = file->fptr;
	//первый фрейм для чтения заголовка
	uint8_t buf[4];
	struct {
		uint8_t streamCode[4];
		uint32_t len;
	} chunk;
	FRESULT res;
	UINT b_read = 0;
	FSIZE_t pos;
	int step;
	while (file->fptr < file->obj.objsize) {
		res = f_read(file, &chunk, 8, &b_read);
		if (res || b_read != 8) return PLAYER_READ_HEADER_ERROR;
		pos = file->fptr;
		//Размер кадра должен быть четным
		step = (chunk.len & 1) ? chunk.len + 1: chunk.len;
		if (!step) continue;
		if ((chunk.streamCode[2]=='w' && chunk.streamCode[3]=='b') ||
			(chunk.streamCode[2]=='W' && chunk.streamCode[3]=='B')    ) {
			if (step > MAINBUF_SIZE) return PLAYER_READ_HEADER_ERROR;
			res = f_read(file, buf, 4, &b_read);
			if (res || b_read != 4) return PLAYER_READ_HEADER_ERROR;
			int ver = ((buf[1] >> 3) & 0x03) == 0 ? MPEG25 : ((((buf[1] >> 3) & 0x03) & 0x01) ? MPEG1 : MPEG2); //версия MPEG
			int layer = 4 - ((buf[1] >> 1) & 0x03);	//версия Layer
			int srIdx = (buf[2] >> 2) & 0x03; //индекс частоты дискретизации
			if (srIdx >= 3 || layer != 3) return PLAYER_READ_HEADER_ERROR;
			audioCFG->channels = ((buf[3] >> 6) & 0x03) == 3 ? 1 : 2; //количество каналов
			audioCFG->samples = (uint32_t)(samplesPerFrameTab[ver][layer-1]); //сэмплов во фрейме
			audioCFG->bitpersample = 16;	//бит в сэмпле
			audioCFG->samplerate = (uint32_t)(samplerateTab[ver][srIdx]); //частота дискретизации
			return PLAYER_READ_HEADER_OK;
		}
		if (f_lseek(file, pos + step)) return 1;
	}
	return PLAYER_READ_HEADER_ERROR;
}

PlayerStartPlayStatus mp3_start_play_callback (Player *player)
{
	Player_client_data *client_data = (Player_client_data*)player->client_data;
	Player_client_mp3 *decoder = (Player_client_mp3 *)client_data->client_data_decoder;
	FIL *file = (FIL *)client_data->file;
	decoder->mp3Decoder = MP3InitDecoder();
	if (!decoder->mp3Decoder) return PLAYER_START_ERROR;
	if (f_lseek(file, decoder->startPosData)) return PLAYER_START_ERROR;
	if (mp3_process_decode(player)==0) return PLAYER_START_OK;
	return PLAYER_START_ERROR;
}

PlayerUpdateStatus mp3_update_play_callback (Player *player)
{
	Player_client_data *client_data = (Player_client_data*)player->client_data;
	FIL *file = (FIL *)client_data->file;
	if (mp3_process_decode(player)==0) return PLAYER_UPDATE_OK;
	if (file->fptr >= f_size(file) - 1) return PLAYER_UPDATE_END_OF_FILE;
	return PLAYER_UPDATE_ERROR;
}

PlayerStopStatus mp3_stop_callback (Player *player)
{
	Player_client_data *client_data = (Player_client_data*)player->client_data;
	Player_client_mp3 *decoder = (Player_client_mp3 *)client_data->client_data_decoder;
	FIL *file = (FIL *)client_data->file;
	f_close(file);
	MP3FreeDecoder(decoder->mp3Decoder);
	return PLAYER_STOP_OK;
}

static int mp3_process_decode(Player *player)
{
	Player_client_data *client_data = (Player_client_data*)player->client_data;
	Player_client_mp3 *decoder = (Player_client_mp3 *)client_data->client_data_decoder;
	AudioConfig *audioCFG = &player->audioCFG;
	FIL *file = (FIL *)client_data->file;
	int nDecodeRes;
	struct {
		uint8_t streamCode[4];
		uint32_t len;
	} chunk;
	FRESULT res;
	UINT b_read = 0;
	FSIZE_t pos;
	int step;
	while (file->fptr < file->obj.objsize) {
		res = f_read(file, &chunk, 8, &b_read);
		if (res || b_read != 8) return 1;
		pos = file->fptr;
		step = chunk.len & 1 ? chunk.len + 1 : chunk.len;
		if (!chunk.len) continue;
		//Если avi файл больше 1 Гб, то он может состоять из нескольких "кусков", начинающихся с чанка RIFF
		//формат заголовка "куска" "RIFF[nnnn]AVIXLIST[mmmm]movi, где:
		//nnnn и mmmm длины данных чанков. После чанка movi продолжают следовать кадры
		if (*((uint32_t*)chunk.streamCode) == chunk_RIFF) { //обнаружили новый "кусок" AVI
			res = f_read(file, decoder->inbuffer, 16, &b_read);
			if (res || b_read != 16) return 1; //Если возникла ошибка при чтении -> выходим с ошибкой
			if (  *((uint32_t*)&decoder->inbuffer[0]) == TYPE_AVIX  &&        //проверяем формат заголовка "куска"
				  *((uint32_t*)&decoder->inbuffer[4]) == chunk_LIST &&
				  *((uint32_t*)&decoder->inbuffer[12]) == chunk_movi   ) continue;
			else return 1; //формат заголовка неверный -> выходим с ошибкой
		}
		if ((chunk.streamCode[2]=='w' && chunk.streamCode[3]=='b') ||
			(chunk.streamCode[2]=='W' && chunk.streamCode[3]=='B')    ) {
			if (step > MAINBUF_SIZE) return 1;
			res = f_read(file, decoder->inbuffer, step, &b_read);
			if (res || b_read != step) return 1;
			uint8_t *pInData = decoder->inbuffer;
			nDecodeRes = MP3Decode(decoder->mp3Decoder, &pInData, (int*)&step, (short*)&player->outbuf[player->offset], 0);
			if (nDecodeRes==ERR_MP3_NONE) {
				//если только 1 моно канал, то модифицируем его в два моно канала (dual mono)
				if (audioCFG->channels == 1) {
					mp3_mono_to_dual_channel((uint16_t*)&player->outbuf[player->offset], audioCFG->samples);
				}
				return 0;
			}
			if (nDecodeRes==ERR_MP3_MAINDATA_UNDERFLOW || nDecodeRes==ERR_MP3_FREE_BITRATE_SYNC) continue;
			return 1;
		}
		if (f_lseek(file, pos + step)) return 1;
	}
	return 1;
}

//преобразование 1 моно канала в 2 моно канала (dual mono)
//увеличивается размер выходного буфера в 2 раза
static inline void mp3_mono_to_dual_channel(uint16_t *outbuf, int samples)
{
	for (int i = 1; i <= samples; i++) {
		outbuf[2 * (samples - i)] = outbuf[samples - i];
		outbuf[2 * (samples - i) + 1] = outbuf[samples - i];
	}
}
