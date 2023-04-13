/*
 *  Copyright (C)2023, VadRov, all right reserved.
 *
 *  Display Driver v1.4+ (includes fast assembler subroutines).
 *  Contains variable offsets in the LCD_Handler structure for use from an assembler program.
 *
 *  Допускается свободное распространение.
 *  При любом способе распространения указание автора ОБЯЗАТЕЛЬНО.
 *  В случае внесения изменений и распространения модификаций указание первоначального автора ОБЯЗАТЕЛЬНО.
 *  Распространяется по типу "как есть", то есть использование осуществляется на свой страх и риск.
 *  Автор не предоставляет никаких гарантий.
 *
 *  Генерация заголовочного файла display_offsets.h со смещениями переменных в структуре LCD_Handler.
 *  Эту программу на C компилятор сначала транслирует в программу на ассемблере, а затем сборщик
 *  сформирует заголовочный файл display_offsets.h, выделив в файле display_offsets.s по заданному
 *  шаблону (#define name #value) нужные строки и преобразовав их в формат макросов C (#define name value).
 *
 *  Для подключения автогенерации заголовочного файла в настройке проекта C/C++ Build -> Settings -> Build Steps
 *  в графу Command раздела Pre-build steps неоходимо записать следующую команду:
 *  					make -f ../Display/makefile
 *	Таким образом, до стадии сборки проекта будет автоматически сгенерирован заголовочный файл display_offsets.h,
 *	содержащий макросы со смещениями переменных структуры LCD_Handler. Эти смещения используются функциями
 *	дисплейного драйвера, реализованными на ассемблере (при условии их подключения опцией LCD_USE_ASSEMBLER
 *	в заголовочном файле display_config.h).
 *
 *  Необходимо будет скорректировать переменные сборки core и include в файле-сборщике makefile (каталог Display),
 *  прописав в нем соответствующие применяемому микроконтроллеру параметры (ядро и драйвера для него).
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 */

#include "display.h"
#include <stddef.h>

//Макрос формирования строки в программе на ассемблере по шаблону: #define name #value
#define DEFINE(sym, val) asm volatile("\n#define " #sym " %0 " : : "i" (val))
#define OFFSET(sym, str, var) DEFINE(sym, offsetof(str, var))

void LCD_GenerateOffsets(void)
{
	//Формируем макросы, определяющие смещения используемых переменных структуры LCD_Handler
	OFFSET(lcd_width, LCD_Handler, Width);
	OFFSET(lcd_height, LCD_Handler, Height);
	OFFSET(lcd_x_offs, LCD_Handler, x_offs);
	OFFSET(lcd_y_offs, LCD_Handler, y_offs);
	OFFSET(lcd_spi, LCD_Handler, spi_data.spi);
	OFFSET(lcd_dma_ctrl, LCD_Handler, spi_data.dma_tx.dma);
	OFFSET(lcd_dma_strm, LCD_Handler, spi_data.dma_tx.stream);
	OFFSET(lcd_dc_port, LCD_Handler, spi_data.dc_port);
	OFFSET(lcd_dc_pin, LCD_Handler, spi_data.dc_pin);
	OFFSET(lcd_cs_port, LCD_Handler, spi_data.cs_port);
	OFFSET(lcd_cs_pin, LCD_Handler, spi_data.cs_pin);
	OFFSET(lcd_sz_mem, LCD_Handler, size_mem);
	OFFSET(lcd_cs_ctrl, LCD_Handler, cs_control);
	OFFSET(lcd_dc_ctrl, LCD_Handler, dc_control);
	OFFSET(lcd_fill_clr, LCD_Handler, fill_color);
}
