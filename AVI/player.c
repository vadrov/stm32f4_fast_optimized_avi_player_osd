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
#include "player.h"
#include "mystring.h"
#include <string.h>
#include <stdlib.h>

Player *player1 = 0;

/* Создает плеер и возвращает указатель на него */
Player* PlayerNew (void)
{
	return (Player*)calloc(1, sizeof(Player));
}

/* Удаляет плеер с очисткой всех данных */
void PlayerDelete (Player *player)
{
	if (player)	{
		void *decoder = ((Player_client_data *)player->client_data)->client_data_decoder;
		if (decoder) free (decoder);
		if (player->outbuf) free(player->outbuf);
		if (player->filename) free(player->filename);
		FreeAudioMeta(&player->audioMeta);
		free(player);
	}
}

/* Инициализирует плеер */
PlayerInitStatus PlayerInit (Player *player, PlayerReadHeaderCallback header_callback, PlayerStartPlayCallback start_callback,
							 PlayerUpdateCallback update_callback, PlayerStopCallback stop_callback, void *client_data)
{
	if ( player == 0 || header_callback == 0 || start_callback == 0 || update_callback == 0 || stop_callback == 0)
		return PLAYER_INIT_ERROR;
	player->header_callback = header_callback;
	player->start_callback = start_callback;
	player->update_callback = update_callback;
	player->stop_callback = stop_callback;
	player->client_data = client_data;
	player->status = PLAYER_INIT;
	return PLAYER_INIT_OK;
}

/* Читает заголовок потока, формирует установочные данные для плеера */
PlayerReadHeaderStatus PlayerReadHeader(Player *player)
{
	player->status = PLAYER_READ_HEADER;
	return player->header_callback(player);
}

/* Подготавливает плеер к запуску воспроизведения
   После вызова этой функции можно запускать DAC  */
PlayerStartPlayStatus PlayerStartPlay(Player *player)
{
	AudioConfig *audioCFG = &player->audioCFG;
	//по количеству сэмплов в кадре выделяем необходимый объем буфера памяти (памяти на 2 кадра сэмплов)
	uint32_t *outbuf = calloc(2 * audioCFG->samples, sizeof(uint32_t));
	if (outbuf == 0) return PLAYER_START_ERROR;
	player->offset = 0;
	player->outbuf = outbuf;
	//декодируем первый кадр
	if (player->start_callback(player) != PLAYER_START_OK) return PLAYER_START_ERROR;
	player->offset = audioCFG->samples;
	//декодируем второй кадр
	if (player->update_callback(player) != PLAYER_UPDATE_OK) return PLAYER_START_ERROR;
	player->status = PLAYER_START_PLAY;
	player->fl_update = 0;
	player->PlayTimeMks = 0; //счетчик воспроизведения, в мс
	//длительность воспроизведения 1 кадра, в мкс
	player->FrameTimeMks = (1000000 * audioCFG->samples) / audioCFG->samplerate;
	return PLAYER_START_OK;
}

/* Обновляет данные в освободившейся части PCM буфера */
PlayerUpdateStatus PlayerUpdate(Player *player)
{
	AudioConfig *audioCFG = &player->audioCFG;
	player->status = PLAYER_UPDATE_PLAY;
	player->offset = (player->offset==0) ? audioCFG->samples : 0;
	int res = player->update_callback(player);
	player->fl_update = 0;
	return res;
}

/* Останавливает плеер.
   Перед вызовом этой функции надо остановить DAC */
PlayerStopStatus PlayerStop(Player *player)
{
	player->status = PLAYER_STOP;
	return player->stop_callback(player);
}

/* Очищает метаданные композиции */
void FreeAudioMeta(AudioMeta *audioMeta)
{
	if (audioMeta) {
		if (audioMeta->title) free(audioMeta->title);
		if (audioMeta->artist) free(audioMeta->artist);
		if (audioMeta->album) free(audioMeta->album);
		if (audioMeta->genre) free(audioMeta->genre);
		if (audioMeta->date) free(audioMeta->date);
		if (audioMeta->comments) free(audioMeta->comments);
		if (audioMeta->trackNumber) free(audioMeta->trackNumber);
	}
}
