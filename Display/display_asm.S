/*
 *	Драйвер управления дисплеями по SPI
 *  Author: VadRov
 *  Copyright (C) 2019 - 2023, VadRov, all right reserved.
 *
 *  Допускается свободное распространение.
 *  При любом способе распространения указание автора ОБЯЗАТЕЛЬНО.
 *  В случае внесения изменений и распространения модификаций указание первоначального автора ОБЯЗАТЕЛЬНО.
 *  Распространяется по типу "как есть", то есть использование осуществляется на свой страх и риск.
 *  Автор не предоставляет никаких гарантий.
 *
 *  Версия: для STM32F4
 *  Ассемблерный ("турбо") вариант.

 *  Включает следующие функции драйвера дисплея, реализованные на ассемблере:
 *  void LCD_SetActiveWindow(LCD_Handler* lcd, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
 *  void LCD_WriteData(LCD_Handler *lcd, uint16_t *data, uint32_t len)
 *  void LCD_WriteDataDMA(LCD_Handler *lcd, uint16_t *data, uint32_t len)
 *  void LCD_FillWindow(LCD_Handler* lcd, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
 *  void LCD_DrawImage(LCD_Handler* lcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data, uint8_t dma_use_flag)
 *  void Display_TC_Callback(DMA_TypeDef *dma_x, uint32_t stream)
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 *
 */

.syntax unified
.cpu cortex-m4
.thumb

#include "display_config.h"

.if LCD_USE_ASSEMBLER

#include "display_offsets.h"

//void LCD_SetActiveWindow(LCD_Handler* lcd, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
//на выходе устанавливает 16-битный формат кадра spi
		.section  .text.LCD_SetActiveWindow
		.weak  LCD_SetActiveWindow
		.type  LCD_SetActiveWindow, %function
LCD_SetActiveWindow:
 		//r0 = lcd, r1 = x1, r2 = y1, r3 = x2, [sp + 0] = y2
 		push 	{r4, r5, lr}  			//сохраняем регистры, sp = sp - 12
 		//----------- формируем в стеке список команд с параметрами ----------
		//список формируется в обратном порядке (по правилу работы стека:
		//LIFO (англ. last in, first out, «последним пришёл — первым ушёл»)
		movs	r5, #0x2c				//команда 0x2c контроллера дисплея
		ldrh	lr, [r0, #lcd_y_offs]	//y_offs из обработчика Lcd
		ldrh	r4, [sp, #12] 			//y2 = [sp + 12] из стека
		add		r4, r4, lr				//y2 = y2 + y_offs
		add		r2, r2, lr				//y1 = y1 + y_offs
		push 	{r2, r4, r5}	 		//в стек: 0x2c, y2, y1
		movs	r5, #0x2b				//команда 0x2b контроллера дисплея
		ldrh	lr, [r0, #lcd_x_offs]	//x_offs из обработчика Lcd
		add		r4, r3, lr				//x2 = x2 + x_offs
		add		r3, r1, lr				//x1 = x1 + x_offs
		movs	r2, 0x2a				//команда 0x2a контроллера дисплея
		push 	{r2, r3, r4, r5}		//в стек: 0x2b, x2, x1, 0x2a
		ldr		lr, [r0, #lcd_spi]		//spi из обработчика Lcd
		ldr		r3, [r0, #lcd_cs_port]	//порт cs из обработчика Lcd
		ldr 	r4, [r0, #lcd_dc_port]	//порт dc из обработчика Lcd
		ldrh	r5, [r0, #lcd_cs_pin]	//пин cs из обработчика Lcd
		ldrh	r12, [r0, #lcd_dc_pin]	//пин dc из обработчика Lcd
.LCD_SetWin1:							//ожидание освобождения spi
		ldr 	r0, [lr, #0x00] 		//читаем регистр CR1
		movs	r1, r0					//сохраняем CR1 в r1 для последующего использования
		ands	r0, r0, #64  			//проверяем состояние бита SPE рег. CR1
		bne		.LCD_SetWin1 			//если бит установлен, то идем на повторный опрос
		//------------------- настройка spi -------------------
		bic		r1, r1, #0b1010110000000000 //очистка: BIDIMODE (15 бит), CRCEN (13 бит), DFF (11 бит), RXONLY (10 бит)
		orr		r1, r1, #64					//установка SPE в 1 (6 бит) - включение spi
		str		r1, [lr, #0x00] 			//запись в рег. CR1
		//-----------------------------------------------------
		cbz		r3, .LCD_SetWin1_1		//проверка: поддерживается ли cs?
		mov		r0, r5, lsl #16			//cs low
		str		r0, [r3, #0x18] 		//запись в BSRR регистр порта
.LCD_SetWin1_1:
		mov		r0, r12, lsl #16		//dc low
		str		r0, [r4, #0x18]			//запись в BSRR регистр порта
		pop		{r0}					//"вытаскиваем" команду из стека
		strb	r0, [lr, #0x0c]			//запись команды в регистр DR
.LCD_SetWin2:							//ожидание готовности к приему новых данных spi
		ldr 	r1, [lr, #0x08] 		//чтение из регистра SR
		ands	r1, r1, #2 				//проверяем 1 бит (флаг TXE)
		beq		.LCD_SetWin2			//пока бит сброшен продолжаем опрос статуса
.LCD_SetWin3:
		ldr 	r1, [lr, #0x08]			//чтение из регистра SR
		ands	r1, r1, #128 			//проверяем 7 бит (флаг BSY)
		bne		.LCD_SetWin3			//пока бит установлен продолжаем опрос статуса
		cmp		r0, #0x2c				//последняя команда (без параметров)?
		beq		.LCD_SetWin6_1			//идем на завершение
		str		r12, [r4, #0x18]		//dc hi
		movs	r2, #2					//r2 = 2, цикл по числу параметров команды
.LCD_SetWin3_1:
		pop		{r0}					//"вытаскиваем" параметр из стека
		mov 	r1, r0, lsr #8			//x1 = x >> 8
		strb	r1, [lr, #0x0c]			//передаем старший байт параметра
.LCD_SetWin4:
		ldr 	r1, [lr, #0x08] 		//чтение из регистра SR
		ands	r1, r1, #2 				//проверяем 1 бит (флаг TXE)
		beq		.LCD_SetWin4
		strb	r0, [lr, #0x0c]			//передаем младший байт параметра
.LCD_SetWin5:
		ldr 	r1, [lr, #0x08]			//ждем флаг TXE
		lsls	r1, r1, #2
		beq		.LCD_SetWin5
		subs	r2, r2, #1				//уменьшаем счетчик с обновлением флагов
		bne		.LCD_SetWin3_1			//в счетчике не ноль -> передаем второй параметр команды
.LCD_SetWin6:
		ldr 	r1, [lr, #0x08]			//ждем флаг BSY
		ands	r1, r1, #128
		bne		.LCD_SetWin6
		b		.LCD_SetWin1_1			//переход к следующей команде
.LCD_SetWin6_1:
		cbz		r3, .LCD_SetWin7		//проверка: поддерживается ли cs?
		str		r5, [r3, #0x18]			//cs hi
.LCD_SetWin7:
		ldr 	r0, [lr, #0x00]			//читаем рег. CR1 spi
		bic		r0, r0, #64				//сбросим 6 бит (SPE)
		orr		r0, r0, #2048			//установим 11 бит (DEF) - 16-битный кадр spi
		str 	r0, [lr, #0x00]			//записываем рег. CR1 spi
		pop 	{r4, r5, pc}			//восстанавливаем регистры, адрес возврата из подпрограммы помещается в pc
.size  LCD_SetActiveWindow, .-LCD_SetActiveWindow

//void LCD_WriteData(LCD_Handler *lcd, uint16_t *data, uint32_t len)
//на входе кадр spi должен быть 16 бит (LCD_SetActiveWindow устанавливает 16-битный кадр spi при выходе)
		.section  .text.LCD_WriteData
		.weak  LCD_WriteData
		.type  LCD_WriteData, %function
LCD_WriteData:
		//r0 = lcd, r1 = data, r2 = len
		push 	{r4, r5, lr}			//сохраняем регистры
		eors	r5, r5					//флаг - в режим копирования (инкремент адресов памяти включен)
.LCD_WriteData0:
		ldr		lr, [r0, #lcd_spi]		//получаем адрес spi из обработчика lcd
.LCD_WriteData1:						//ожидание освобождения spi, если оно занято
		ldr 	r3, [lr, #0x00] 		//читаем регистр CR1
		mov		r12, r3					//сохраняем CR1 в r1 для последующего использования
		ands	r3, r3, #64  			//проверяем состояние бита SPE рег. CR1 (6 бит), 64 определяет 6 бит
		bne		.LCD_WriteData1			//если бит установлен (не ноль), то продолжим ожидание
		orr		r12, r12, #64			//установим 6 бит (SPE) - включение spi, 64 определяет 6 бит
		str 	r12, [lr, #0x00]		//записываем в рег. CR1 spi
		ldr		r3, [r0, #lcd_cs_port]	//получаем адрес порта вывода cs из обработчика lcd
		cbz		r3, .LCD_WriteData2		//проверка: поддерживается ли cs (проверка адреса порта на ноль)?
		ldrh	r12, [r0, #lcd_cs_pin]	//получаем номер пина cs из обработчика lcd
		mov		r4, r12, lsl #16		//cs low (смещение бита пина на 16 бит влево)
		str		r4, [r3, #0x18] 		//запись значения из r4 в BSRR регистр порта
.LCD_WriteData2:
		ldr 	r4, [r0, #lcd_dc_port]	//получаем адрес порта вывода dc из обработчика lcd
		ldrh	r0, [r0, #lcd_dc_pin]	//получаем номер пина dc из обработчика lcd
		str		r0, [r4, #0x18]			//dc hi, запись в BSRR регистр порта
.LCD_WriteData3:
		ldrh	r0, [r1, #0]			//берем полуслово по адресу в r1
		str		r0, [lr, #12]			//и помещаем его в рег. DR spi
		orrs	r5, r5					//проверка флага копирование/заливка на ноль
		cbnz	r5, .LCD_WriteData4		//если не ноль, то пропускаем инкремент адреса в r1
		adds	r1, #2					//если 0 (режим копирования), то увеличиваем адрес в r1 на 2 (копирование полуслов)
.LCD_WriteData4:
		ldr 	r0, [lr, #0x08]			//ждем флаг TXE (1 бит)
		ands	r0, r0, #2				//2 определяет 1 бит
		beq		.LCD_WriteData4			//если ноль
		subs	r2, #1					//уменьшаем счетчик
		bne		.LCD_WriteData3			//в счетчике не ноль
.LCD_WriteData5:
		ldr 	r0, [lr, #0x08]			//ждем флаг BSY (бит 7)
		ands	r0, r0, #128			//128 определяет 7 бит
		bne		.LCD_WriteData5   		//если не ноль, то продолжим ожидание
		cbz		r3, .LCD_WriteData6		//проверка: поддерживается ли cs?
		str		r12, [r3, #0x18]		//cs hi, запись в BSRR регистр порта
.LCD_WriteData6:
		ldr 	r0, [lr, #0x00]			//читаем рег. CR1 spi
		bic		r0, r0, #64				//сбросим 6 бит (SPE), 64 определяет 6 бит
		str 	r0, [lr, #0x00]			//записываем рег. CR1 spi
		pop 	{r4, r5, pc}			//восстанавливаем регистры, адрес возврата из подпрограммы помещается в pc
.size  LCD_WriteData, .-LCD_WriteData

//void LCD_WriteDataDMA(LCD_Handler *lcd, uint16_t *data, uint32_t len)
//на входе кадр spi должен быть 16 бит (LCD_SetActiveWindow устанавливает 16-битный кадр spi при выходе)
		.section  .text.LCD_WriteDataDMA
		.weak  LCD_WriteDataDMA
		.type  LCD_WriteDataDMA, %function
LCD_WriteDataDMA:
		//r0 = lcd, r1 = data, r2 = len
		ldr		r12, [r0, #lcd_dma_ctrl]//dma из обработчика lcd
		orrs	r12, r12				//проверка: dma == 0 ?
		beq		LCD_WriteData			//если dma == 0, то переходим к процедуре передачи без DMA
		push 	{r4, r5, lr}			//сохраняем регистры
		eors	r5, r5					//флаг - в режим копирования (инкремент адресов памяти включен)
.LCD_WriteDataDMA0:
		ldr		lr, [r0, #lcd_spi]		//spi из обработчика lcd
.LCD_WriteDataDMA1:						//ожидание освобождения spi, если оно занято
		ldr 	r3, [lr, #0x00] 		//читаем регистр CR1
		movs	r4, r3					//сохраняем CR1 в r1 для последующего использования
		ands	r3, r3, #64  			//проверяем состояние бита SPE рег. CR1
		bne		.LCD_WriteData1			//если бит установлен (не ноль), то продолжим ожидание
		orr		r4, r4, #64				//установим 6 бит (SPE) - включение spi
		str 	r4, [lr, #0x00]			//записываем в рег. CR1 spi
		ldr		r4, [lr, #0x04]			//читаем рег. CR2 spi
		orr		r4, r4, #2				//установим 1 бит (TXDMAEN) CR2
		str 	r4, [lr, #0x04]			//записываем в рег. CR2 spi
		ldr		r3, [r0, #lcd_cs_port]	//порт cs из обработчика lcd
		cbz		r3, .LCD_WriteDataDMA2	//проверка: поддерживается ли cs?
		ldrh	r4, [r0, #lcd_cs_pin]	//пин cs из обработчика lcd
		movs	r4, r4, lsl #16			//cs low
		str		r4, [r3, #0x18] 		//запись в BSRR регистр порта
.LCD_WriteDataDMA2:
		ldr 	r3, [r0, #lcd_dc_port]	//порт dc из обработчика lcd
		ldrh	r4, [r0, #lcd_dc_pin]	//пин dc из обработчика lcd
		str		r4, [r3, #0x18]			//dc hi
		add		r4, r0, #lcd_sz_mem		//в r4 адрес переменной lcd->size_mem
		ldr		r0, [r0, #lcd_dma_strm]	//номер потока (stream) DMA из обработчика lcd
		movs 	r3, #24					//множитель для формулы: адрес потока = адрес DMA + 24 * номер потока + 16
		mla		r12, r0, r3, r12		//r12 = r12 + r0 * r3
		add		r0, r12, #16			//r0 = r12 + 16, в r0 - адрес потока DMA
		movw	r3, #0xffff				//в r3 запиcать 0xffff
		cmp		r2, r3					//r2 > 65535 ?
		itee	gt						//выполнение IT-блока из 3-х команд:
		subgt	r2, r2, r3				//останется передать len = len - 65535, для текущей передачи 65535
		movle	r3, r2					//для текущей передачи len
		movle	r2, #0x00				//останется передать 0
		str		r2, [r4]				//запись в переменную обработчика lcd->size_mem сколько данных останется передать
		str		r3, [r0, #4]			//запись в регистр NDTR DMA количества данных для текущей передачи
		add		lr, lr, #0x0c			//адрес регистра DR spi, 0x0c смещение относительно базового адреса spi, определяющее DR
		str		lr, [r0, #8]			//запись в рег. PAR DMA адреса регистра DR spi (spi->DR)
		str		r1, [r0, #12]			//запись в рег. M0AR DMA адреса блока данных для передачи (data)
		ldr		r1, [r0, #0]			//чтение рег. CR DMA
		orrs	r5, r5					//флаг копирование/заливка
		ite		eq						//IT-блок по типу if ноль then else
		orreq	r1, r1, #1024			//установка 10 (MINC) бита - режим копирования блока данных на дисплей (флаг в r5 = 0)
		bicne	r1, r1, #1024			//сброс 10 (MINC) бита - режим заливки одним цветом (флаг r5 != 0)
		orr		r1, r1, #1				//установка 1 (CR_EN) бита - включение/разрешение DMA потока
		bic		r1, r1, #512			//сброс 9 (PINC) бита - инкремент адреса периферии отключен
		str		r1, [r0, #0]			//запись параметра в рег. CR DMA
		pop 	{r4, r5, pc}			//восстанавливаем регистры, адрес возврата из подпрограммы помещается в pc
.size  LCD_WriteDataDMA, .-LCD_WriteDataDMA

//void LCD_FillWindow(LCD_Handler* lcd, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
		.section  .text.LCD_FillWindow
		.weak  LCD_FillWindow
		.type  LCD_FillWindow, %function
LCD_FillWindow:
		//r0 = lcd, r1 = x1, r2 = y1, r3 = x2, [sp + 0] = y2, [sp + 4] = color
		push	{r4, r5, lr}			//сохраняем регистры, sp = sp - 12
		cmp		r1, r3					//x1 > x2 ? Сортировка по возрастанию
		ittt	gt						//IT - блок
		movgt	lr, r1					//lr = r1, lr = x1
		movgt	r1, r3					//r1 = r3, x1 = x2
		movgt	r3, lr					//r3 = lr, x2 = lr
		ldrh	r4, [sp, #12]			//получаем из стека параметр y2
		cmp		r2, r4					//y1 > y2 ?  Сортировка по возрастанию
		ittt	gt						//IT - блок
		movgt	lr, r2					//lr = r2, lr = y1
		movgt	r2, r4					//r2 = r4, y1 = y2
		movgt	r4, lr					//r4 = lr, y2 = lr
		ldrh	lr, [r0, #lcd_width]	//lcd->Width
		sub		lr, lr, #1				//lcd->Width - 1
		cmp		r1, lr					//x1 > lcd->Width - 1 ?
		bgt		.LCD_FillWindow_exit	//выход, если x1 выходит за границы дисплея
		ldrh	r12, [r0, #lcd_height]	//lcd->Height
		sub		r12, r12, #1			//lcd->Height - 1
		cmp		r2, r12					//y1 > lcd->Height - 1
		bgt		.LCD_FillWindow_exit	//выход, если y1 выходит за границы дисплея
		cmp		r3, lr					//x2 > lcd->Width - 1
		it		gt						//"обрезаем" x2, если он выходит за границы дисплея
		movgt	r3, lr					//x2 = lcd->Width - 1
		cmp		r4, r12					//y2 > lcd->Height - 1
		it		gt						//"обрезаем" y2, если он выходит за границы дисплея
		movgt	r4, r12					//y2 = lcd->Height - 1
		sub		sp, #8					//sp = sp - 8 (cоглашение о передаче параметров через стек)
		str		r4, [sp, #0]			//помещаем y2 на вершину стека
		//--------------  Расчет len = (x2 - x1 + 1) * (y2 - y1 + 1) ---------------------------
		subs	r5, r3, r1				//r5 = x2 - x1
		add 	r5, #1					//r5 = r5 + 1
		subs	r4, r4, r2				//r4 = y2 - y1
		mla		r5, r5, r4, r5			//r5 = r5 * r4 + r5 - сэкономили на 1 команде add r4, #1
		//--------------------------------------------------------------------------------------
		movs	r4, r0					//сохраняем r0 в r4 (чтобы не потерять адрес lcd)
		bl		LCD_SetActiveWindow 	//установка окна вывода: передаем r0(lcd), r1(x1), r2(y1), r3(x2), [sp+0](y2)
		add		sp, #8					//восстанавливаем указатель стека
		ldr		r0, [sp, #16]			//получаем из стека параметр color (R8G8B8)
		//------------- преобразование цвета R8G8B8 в R5G6B5 --------------------------------------------------
		ubfx	r1, r0, #3, #5			//копируем в r1 5 бит составляющей b начиная с 3 бита
		ubfx	r2, r0, #10, #6			//копируем в r2 6 бит составляющей g начиная с 10 бита
		orr		r1, r1, r2, lsl #5		//объединяем r1 с r2, смещенной на 5 бит влево
		ubfx	r2, r0, #19, #5			//копируем в r2 5 бит составляющей r начиная с 19 бита
		orr		r1, r1, r2, lsl #11 	//объединяем r1 с r2, смещенной на 11 бит влево
		//------------------------------------------------------------------------------------------------------
		strh	r1, [r4, #lcd_fill_clr]	//записываем цвет в переменную lcd->fill_color (адрес переменной lcd + #104)
		movs	r0, r4					//r0 = lcd
		add		r1,	r0, #lcd_fill_clr	//r1 = &lcd->fill_color
		movs	r2, r5					//r2 = len
		movs	r5, #1					//флаг для LCD_WriteDataDMA/LCD_WriteData
		ldr		r12, [r0, #lcd_dma_ctrl]//dma из обработчика lcd
		orrs	r12, r12            	//проверка на 0 в r12 (в r12 ноль, если DMA не поддерживается)
		beq		.LCD_WriteData0			//точка входа в процедуру LCD_WriteData (с флагом 1 в r5)
		b		.LCD_WriteDataDMA0  	//точка входа в процедуру LCD_WriteDataDMA (с флагом 1 в r5)
.LCD_FillWindow_exit:
		pop		{r4, r5, pc}			//восстанавливаем регистры, адрес возврата из подпрограммы помещается в pc
.size  LCD_FillWindow, .-LCD_FillWindow

//void LCD_DrawImage(LCD_Handler* lcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data, uint8_t dma_use_flag)
		.section  .text.LCD_DrawImage
		.weak  LCD_DrawImage
		.type  LCD_DrawImage, %function
LCD_DrawImage:
		//r0 = lcd, r1 = x, r2 = y, r3 = w, [sp + 0] = h, [sp + 4] = data, [sp + 8] = dma_use_flag
		push	{r4, r5, lr}			//сохраняем регистры
		ldrh	lr, [r0, #lcd_width]	//lr = lcd->Width
		cmp		r1, lr					//x >= lcd->Width ?
		bcs		.LCD_DrawImage_exit 	//x за границами диапазона - выходим
		add		r12, r1, r3				//r12 = x + w
		cmp		r12, lr					//x + w > lcd->Width ?
		bhi		.LCD_DrawImage_exit 	//x за границами диапазона - выходим
		ldrh	lr, [r0, #lcd_height]	//lr = lcd->Height
		cmp		r2, lr					//y >= lcd->Height ?
		bcs		.LCD_DrawImage_exit		//y за границами диапазона - выходим
		ldr		r4, [sp, #12]			//r4 = h
		adds	r5, r2, r4				//r5 = y + h
		cmp		r5, lr					//y + h > lcd->Height ?
		bhi		.LCD_DrawImage_exit		//y за границами диапазона - выходим
		mul		r4, r4, r3				//r4 = w * h
		subs	r5, #1					//r5 = y + h - 1
		sub		sp, #8					//sp = sp - 8 --> указатель стека перемещаем вверх для записи передаваемого параметра
		str		r5, [sp, #0]			//r5 (y + h - 1) - записываем на вершину стека
		sub		r3, r12, #1				//r3 = x + w - 1
		movs	r5, r0					//r5 = r0 = lcd
		bl		LCD_SetActiveWindow 	//r0 = lcd, r1 = x, r2 = y, r3 = x + w - 1, [sp + 0] = y + h - 1
		add		sp, #8					//восстанавливаем указатель стека
		movs	r0, r5					//r0 = r5 = lcd
		ldr		r1, [sp, #16]			//r1 = data - адрес данных для передачи (параметр из стека)
		movs	r2, r4					//r2 = w * h
		ldr		r3, [sp, #20]			//r3 = dma_use_flag - флаг, определяющий использование DMA (параметр из стека)
		pop		{r4, r5, lr}			//восстанавливаем регистры из стека
		orrs	r3, r3					//проверка использования DMA: dma_use_flag == 0 ?
		beq		LCD_WriteData			//==0: переход на LCD_WriteData - без DMA
		b		LCD_WriteDataDMA		//переход на LCD_WriteDataDMA	- с DMA
.LCD_DrawImage_exit:
		pop		{r4, r5, pc}			//восстанавливаем регистры, адрес возврата из подпрограммы помещается в pc
.size  LCD_DrawImage, .-LCD_DrawImage

//void Display_TC_Callback(DMA_TypeDef *dma_x, uint32_t stream)
		.section  .text.Display_TC_Callback
		.weak  Display_TC_Callback
		.type  Display_TC_Callback, %function
Display_TC_Callback:
		//r0 = dma_x, r1 = stream
		push	{lr}					//запоминаем в стеке адрес возврата из подпрограммы
		movs	r3, r1					//в r3 номер потока stream DMA (0...7)
		cmp		r3, #3					//проверка: stream > 3 ?
		itte	gt						//it-блок по типу if больше then else
		subgt	r3, #4					//stream = stream - 4
		movgt	lr, #0x0c				//смещение адреса для регистра HIFCR DMA
		movle	lr, #0x08				//смещение адреса для регистра LIFCR DMA
		adr		r12, .DMA_ShiftTbl		//адрес таблицы положения флаговых битов регистра IFCR (hi и low)
		ldrb	r12, [r12, r3]			//получение из таблицы стартовой позиции флаговых битов регистра IFCR
		movs	r2, #0x3f				//маска для сброса флагов
		mov		r2, r2, lsl r12			//сдвигаем маску на местоположение очищаемых флагов
		str		r2, [r0, lr]			//записываем маску в рег. IFCR
		movs	r2, #24					//множитель для формулы: адрес потока = адрес DMA + 24 * номер потока + 16)
		mla		r0, r1, r2, r0			//r0 = r0(dma) + r1(stream) * r2(24)
		add		r0, #16					//r0 = r0(dma) + 16, на выходе в r0 адрес потока DMA
		ldr		r1, [r0, #0]			//чтение регистра CR DMA потока
		bic		r1, r1, #1				//сбрасываем CR_EN бит (выключаем поток)
		str		r1, [r0, #0]			//записываем в рег. CR
.Display_TC_Callback1:					//ждем отключения потока DMA
		ldr		r1, [r0, #0]			//чтение регистра CR DMA потока
		ands	r1, r1, #1				//проверка бита 0 (CR_EN)
		bne		.Display_TC_Callback1	//если не ноль, то идем на новый круг, считывая состояние рег. CR DMA потока
		ldr		r3, =LCD				//определяем адрес местоположения переменной списка дисплеев
		ldr		r3, [r3]				//"вытаскиваем" указатель на первый дисплей в списке
		ldr		r2, [r3, #lcd_sz_mem]	//в r2 значение переменной lcd->size_mem из обработчика lcd
		cbz		r2, .Display_TC_Callback2	//если данных для передачи больше нет, то переходим на завершение передачи
		movw	r1, 0xffff				//записать в r1 0xffff (65535)
		cmp		r2, r1					//сравнить r2 c r1 (нас интересует: r2 > 65535 или нет)
		itee	gt						//выполнение IT-блока по типу: if больше then else
		subgt	r2, r2, r1				//остаток len = len - 65535, для текущей передачи 65535
		movle	r1, r2					//на передачу len
		movle	r2, 0					//осталось передать 0
		str		r1, [r0, #4]			//запись r1 в регистр NDTR DMA
		str		r2, [r3, #lcd_sz_mem]	//запись r3 в переменную lcd->size_mem (остаток для следующей передачи)
		ldr		r1, [r0]				//чтение рег. CR DMA
		orr		r1, r1, #1				//установка бита CR_EN рег. CR DMA (включение потока)
		str		r1, [r0]				//запись в рег. CR DMA
		pop		{pc}					//восстанавливаем в рег. pc адрес возврата из подпрограммы (переход произойдет автоматически)
.Display_TC_Callback2:					//завершение передачи
		ldr		r0, [r3, #lcd_spi]		//в r0 - указатель на spi из обработчика Lcd
		ldr		r1, [r0, #0x04]			//читаем регистр CR2 spi
		bic		r1, r1, #2				//сбрасываем бит TXDMAEN в регистре CR2
		str		r1, [r0, #0x04]			//записываем в регистр CR2
.Display_TC_Callback3:					//ждем, когда spi освободится
		ldr		r1, [r0, #0x08]			//читаем регистр SR spi
		ands	r1, r1, #128			//проверяем состояние бита 7 (BSY) регистра SR spi
		bne		.Display_TC_Callback3	//если бит установлен (не ноль), то spi занято, и повторяем опрос состояния рег. SR
		ldr		r2, [r3, #lcd_cs_port]	//порт cs из обработчика Lcd
		cbz		r2, .Display_TC_Callback4	//если управление по cs не поддерживается, то "перескакиваем" через 2 команды
		ldrh	r1, [r3, #lcd_cs_pin]	//пин cs из обработчика Lcd
		str		r1, [r2, #0x18]			//cs hi, записываем 1 в соответствующие разряды рег. BSRR порта
.Display_TC_Callback4:
		ldr		r1, [r0, #0x00]			//читаем рег. CR1 spi
		bic		r1, r1, #64				//сбрасываем бит 6 (SPE) рег. CR1 spi
		str		r1, [r0, #0x00]			//записываем в CR1 spi
		pop		{pc}					//восстанавливаем в рег. pc адрес возврата из подпрограммы (переход произойдет автоматически)
.DMA_ShiftTbl:							//таблица стартовых положений флаговых бит потоков DMA рег. IFCR (HIFCR, LIFCR)
		.byte	0, 6, 16, 22
.size  Display_TC_Callback, .-Display_TC_Callback

.endif
		.end
