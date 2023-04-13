/*
 *  Author: VadRov
 *  Copyright (C) 2022 - 2023, VadRov, all right reserved.
 *
 *	Сопряжение библиотеки TJpgDec с драйвером дисплея
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

#ifndef INC_JPEG_CHAN_H_
#define INC_JPEG_CHAN_H_

#include "display.h"

#define JPEG_CHAN_WORK_BUFFER_SIZE	12000 //максимум памяти, выделяемый для декодера

typedef enum {
	PICTURE_IN_FILE,
	PICTURE_IN_MEMORY
} PictureLocation;

typedef struct {
	const uint8_t *data;	//указатель на массив с данными изображения
	const uint32_t size;	//размер массива
} iPicture_jpg;

typedef struct {
	void *file;   			//указатель на обработчик файла
	volatile uint32_t size;	//размер файла
} iFile_jpg;

typedef struct {
    void *file;   			//входной поток
    LCD_Handler *lcd;		//указатель на обработчик дисплея
    uint16_t x_offs, y_offs;
} IODEV;

uint8_t LCD_Load_JPG_chan (LCD_Handler *lcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, void *image_stream, PictureLocation location);

#endif /* INC_JPEG_CHAN_H_ */
