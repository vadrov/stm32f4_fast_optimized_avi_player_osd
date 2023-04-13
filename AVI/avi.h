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
#ifndef AVI_H_
#define AVI_H_

#include "display.h"

//----------------------Конфигурация аудио-ЦАП (настройки пользователя) ------------------------
#define AudioDAC_I2S		SPI3				//spi, на базе которого используется i2s для dac
#define AudioDAC_DMA		DMA1				//контроллер dma, обслуживающий dac
#define AudioDAC_DMA_Stream	LL_DMA_STREAM_5		//поток контроллера dma, обслуживающий dac
#define AudioDAC_IRQn		DMA1_Stream5_IRQn	//прерывание в потоке DMA, обслуживающем dac
//----------------------------------------------------------------------------------------------

void PlayAVI(char *dir, char *fname, LCD_Handler *lcd, uint16_t x, uint16_t y, uint16_t win_wdt, uint16_t win_hgt);
void AVIIntro(LCD_Handler *lcd);

#endif /* AVI_H_ */
