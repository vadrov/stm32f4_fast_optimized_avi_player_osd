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
#ifndef PLAYER_H_
#define PLAYER_H_

#include "main.h"

//статусы для функции инициализации плейера
typedef enum {
	PLAYER_INIT_OK = 0,
	PLAYER_INIT_ERROR
} PlayerInitStatus;

//статусы для функции чтения заголовка
typedef enum {
	PLAYER_READ_HEADER_OK = 0,
	PLAYER_READ_HEADER_ERROR
} PlayerReadHeaderStatus;

//статусы для функции старта воспроизведения
typedef enum {
	PLAYER_START_OK = 0,
	PLAYER_START_ERROR
} PlayerStartPlayStatus;

//статусы для функции обновления воспроизведения
typedef enum {
	PLAYER_UPDATE_OK = 0,
	PLAYER_UPDATE_END_OF_FILE,
	PLAYER_UPDATE_ERROR
} PlayerUpdateStatus;

//статусы для функции остановки воспроизведения
typedef enum {
	PLAYER_STOP_OK=0,
	PLAYER_STOP_ERROR
}PlayerStopStatus;

//статусы текущего состояния плейера
typedef enum {
	PLAYER_INIT = 0,
	PLAYER_OPEN_FILE,
	PLAYER_READ_HEADER,
	PLAYER_START_PLAY,
	PLAYER_UPDATE_PLAY,
	PLAYER_STOP
} PlayerStatus;

typedef enum {
	PLAY_FILE_OK,
	PLAY_FILE_ALLOC_ERROR,
	PLAY_FILE_INIT_ERROR,
	PLAY_FILE_OPEN_ERROR,
	PLAY_FILE_HEADER_ERROR,
	PLAY_FILE_PICTURE_ERROR,
	PLAY_FILE_START_ERROR,
	PLAY_FILE_DAC_ERROR,
	PLAY_FILE_UPDATE_ERROR,
	PLAY_FILE_UNKNOW_FORMAT
} PlayerPlayFileStatus;

struct Player;

//определение типов для функций "коллбэков"
typedef PlayerReadHeaderStatus (*PlayerReadHeaderCallback)(struct Player *player);
typedef PlayerStartPlayStatus (*PlayerStartPlayCallback)(struct Player *player);
typedef PlayerUpdateStatus (*PlayerUpdateCallback)(struct Player *player);
typedef PlayerStopStatus (*PlayerStopCallback)(struct Player *player);

//определение типа Player_client_data с клиентскими данными
typedef struct {
	void *file;					//входной поток
	void *client_data_decoder;  //указатель на данные для конкретного пользовательского декодера
} Player_client_data;

//структура с параметрами аудио
typedef struct {
	uint32_t channels;							//количество каналов (моно, стерео)
	uint32_t bitpersample;						//разрядность/точность данных (8 бит, 16 бит, 24 бит, 32 бит)
	uint32_t samplerate;						//частота дискретизиции
	uint32_t samples;							//количество сэмплов
} AudioConfig;

typedef struct {
	char mimestr[40];		//MIME-тип/подтип изображения, например: image/jpeg, image/png
	unsigned long startpos;	//стартовая позиция изображения в музыкальном файле
} PictureData;

typedef struct {
	char *title;								//название композиции
	char *artist;								//исполнитель
	char *album;								//альбом
	char *genre;								//жанр
	char *comments;								//комментарий
	char *date;									//год
	char *trackNumber;							//порядковый номер композиции в альбоме
	PictureData picdata;						//данные обложки
	uint32_t duration;							//длительность композиции, в секундах
	volatile uint32_t current_duration;			//время с начала воспроизведения, в секундах
} AudioMeta;

//определение типа Player
typedef struct Player {
	char *filename;							    //имя файла
	int mediatype;								//тип музыкальных медиаданных (wav, mp3, ...)
	AudioConfig audioCFG;						//параметры аудио
	AudioMeta audioMeta;						//описание композиции (название, исполнитель и т.п.)
	uint32_t *outbuf;							//указатель на выходной PCM буфер. Данные в буфере 32-битные (word), располагаются поканально: LLRR (по 2 байта на канал)
	volatile int32_t offset;					//смещение в буфере выходных данных
	PlayerReadHeaderCallback header_callback;	//указатель на функцию чтения заголовка файла
	PlayerStartPlayCallback start_callback;		//указатель на функцию запуска воспроизведения
	PlayerUpdateCallback update_callback;		//указатель на функцию обновления данных для воспроизведения
	PlayerStopCallback stop_callback;			//указатель на функцию остановки воспроизведения
	volatile PlayerStatus status;				//текущий статус плейера (что сделал, т.е. последняя из выполненных операций:
												//инициализация, чтение заголовка, открытие файла, старт, обновление, стоп)
	void *client_data;							//указатель на клиентские данные
	volatile uint8_t fl_update;					//флаг запроса обновления (0 - обновление не требуется, 1 - требуется обновление)
	volatile uint64_t PlayTimeMks;				//время с начала воспроизведения, мкс
	uint64_t FrameTimeMks;						//время воспроизведения одного кадра, мкс
} Player;

extern Player* player1;

//создает плейер
Player* PlayerNew (void);

//удаляет плейер
void PlayerDelete (Player *player);

//инициализирует плейер для конкретного типа воспроизводимого файла
PlayerInitStatus PlayerInit(Player *player, PlayerReadHeaderCallback header_callback, PlayerStartPlayCallback start_callback,
							PlayerUpdateCallback update_callback, PlayerStopCallback stop_callback, void *client_data);

//читает заголовок файла
PlayerReadHeaderStatus PlayerReadHeader(Player *player);

//стартует воспроизведение
PlayerStartPlayStatus PlayerStartPlay(Player *player);

//обновляет данные для воспроизведения
PlayerUpdateStatus PlayerUpdate(Player *player);

//останавливает воспроизведение
PlayerStopStatus PlayerStop(Player *player);

//освобождает память, занятую структурой AudioMeta (название композиции, исполнитель и т.п.)
void FreeAudioMeta(AudioMeta *audioMeta);

#endif /* PLAYER_H_ */
