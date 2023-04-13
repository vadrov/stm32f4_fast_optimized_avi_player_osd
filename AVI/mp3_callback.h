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

#ifndef MP3_CALLBACK_H_
#define MP3_CALLBACK_H_

#include "ff.h"
#include "player.h"
#include "mp3dec.h"

typedef struct {
	HMP3Decoder mp3Decoder;
	MP3FrameInfo mp3FrameInfo;
	FSIZE_t startPosData;
	uint8_t inbuffer[MAINBUF_SIZE];
} Player_client_mp3;

PlayerReadHeaderStatus mp3_read_header_callback (Player *player);
PlayerStartPlayStatus  mp3_start_play_callback (Player *player);
PlayerUpdateStatus     mp3_update_play_callback (Player *player);
PlayerStopStatus       mp3_stop_callback (Player *player);

#endif /* MP3_CALLBACK_H_ */
