/*----------------------------------------------------------------------------/
/ TJpgDec - Tiny JPEG Decompressor R0.03                      (C)ChaN, 2021
/-----------------------------------------------------------------------------/
/ The TJpgDec is a generic JPEG decompressor module for tiny embedded systems.
/ This is a free software that opened for education, research and commercial
/  developments under license policy of following terms.
/
/  Copyright (C) 2021, ChaN, all right reserved.
/
/ * The TJpgDec module is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/---------------------------------------------------------------------------------------/
/ Oct 04, 2011 R0.01  First release.
/ Feb 19, 2012 R0.01a Fixed decompression fails when scan starts with an escape seq.
/ Sep 03, 2012 R0.01b Added JD_TBLCLIP option.
/ Mar 16, 2019 R0.01c Supprted stdint.h.
/ Jul 01, 2020 R0.01d Fixed wrong integer type usage.
/ May 08, 2021 R0.02  Supprted grayscale image. Separated configuration options.
/ Jun 11, 2021 R0.02a Some performance improvement.
/ Jul 01, 2021 R0.03  Added JD_FASTDECODE option.
/                     Some performance improvement.
/---------------------------------------------------------------------------------------*/
/*
 *  Portions сopyright (C) 2022-2023, VadRov, all right reserved.
 *  Оптимизация (в т.ч., для stm32), улучшения, дополнения:
 *  - В структуре JDEC буфер workbuf увеличен в два раза, введен указатель смещения
 *  в этом буфере для использования DMA. Декодер не простаивает, ожидая вывода на
 *  дисплей ранее декодированного блока;
 *  - Введен дополнительный формат цвета пикселя Grayscale_16. 32 оттенка серого в 16-битном
 *    формате цвета R5G6B5. Производительность в сравнении с цветным RGB565 выше
 *    до 1.5 - 2 раз.
 * 	- Оптимизирована процедура mcu_output: существенно улучшена производительность
 * 	  для цвета в формате R5G6B5.
 * 	  Исключен промежуточный этап преобразования цвета из формата R8G8B8 в R5G6B5. R5G6B5 теперь
 * 	  формируется непосредственно без дополнительных преобразований;
 * 	- Оптимизирована процедура mcu_load;
 * 	- Реализованы процедуры быстрого преобразования YCbCr в R5G6B5 и R8G8B8, Y в 16
 * 	  битный монохромный (32 оттенка серого);
 * 	- Реализовано быстрое заполнение областей памяти заданным значением шириной:
 * 			- в байт (memset_8 - asm);
 * 			- в полуслово (memset_16 - asm);
 * 			- в слово (memset_32 - asm).
 * 	- Реализовано быстрое копирование областей памяти (memcpy_8 - побайтно, memcpy_16 по полусловам);
 * 	- Реализован asm-вариант функции block_idct (обратное дискретное косинусное преобразование).
 * 	Для подключения оптимизации stm32 используется определение JD_FAST_STM32 в заголовочном файле
 *  tjpgdcnf.h
 *
 * https://www.youtube.com/@VadRov
 * https://dzen.ru/vadrov
 * https://vk.com/vadrov
 *----------------------------------------------------------------------------------------------*/

#include "tjpgd.h"
#include <stdlib.h>

#if JD_FASTDECODE == 2
#define HUFF_BIT	10	/* Bit length to apply fast huffman decode */
#define HUFF_LEN	(1 << HUFF_BIT)
#define HUFF_MASK	(HUFF_LEN - 1)
#endif

extern uint8_t AVI_color_mode;

#if (JD_FAST_STM32 == 1)
/*
 * Преобразование цвета из YCbCr в R5G6B5
 * Copyright (C) 2023, VadRov, all right reserved.
 * на simd для stm32
 */
static inline uint16_t Convert_YCbCr_to_R5G6B5(int yy, int cb, int cr)
{
	uint16_t color;
	asm volatile (	"movs 	r2, #45						\n\t"
					"mul 	r2, r2, %[cr]				\n\t"   //r = 45 * cr
					"add 	r2, %[yy], r2, asr #5		\n\t"	//r = yy + (r >> 5)
					"usat 	r2, #5, r2, asr #3			\n\t"	//r = usat(5, r >> 3)
					"movs 	r1, #23						\n\t"
					"mul 	r1, r1, %[cr]				\n\t"	//g = 23 * cr
					"movs 	r0, #11						\n\t"
					"mla 	r1, r0, %[cb], r1			\n\t"	//g = g + 11 * cb
					"sub 	r1, %[yy], r1, asr #5		\n\t"	//g = yy - (g >> 5)
					"usat 	r1, #5, r1, asr #3			\n\t"	//g = usat(5, g >> 3)
					"movs 	r0, #113					\n\t"
					"mul 	r0, r0, %[cb]				\n\t"	//b = 113 * cb
					"add 	r0, %[yy], r0, asr #6		\n\t"	//b = yy + (b >> 6)
					"usat 	r0, #5, r0, asr #3			\n\t"	//color = usat(5, b >> 3)
					"orr 	r0, r0, r1, lsl #6			\n\t"	//color = color | (g << 6)
					"orr 	%[color], r0, r2, lsl #11	\n\t"	//color = color | (r << 11)
					: [color]"=&r"(color)
					: [yy]"r"(yy), [cb]"r"(cb), [cr]"r"(cr)
					: "r0", "r1", "r2");
	return color;
}

/*
 * Преобразование цвета из Y (яркость) в монохромный (оттенки серого) 16 бит
 * Copyright (C) 2023, VadRov, all right reserved.
 * на simd для stm32
 */
static inline uint16_t Convert_Y_to_Grayscale16(int yy)
{
	uint16_t color;
	asm volatile (	"usat 	r0, #5, %[yy], asr #3			\n\t"
					"orr	%[color], r0, r0, lsl #6		\n\t"
					"orr 	%[color], %[color], r0, lsl #11	\n\t"
					: [color]"=&r"(color)
					: [yy]"r"(yy)
					: "r0");
	return color;
}

/*
 * Преобразование цвета из YCbCr в R8G8B8
 * Copyright (C) 2023, VadRov, all right reserved.
 * на simd для stm32
 */
static inline void Convert_YCbCr_to_R8G8B8(int yy, int cb, int cr, uint8_t *r1, uint8_t *g1, uint8_t *b1)
{
	uint8_t r, g, b;
	asm volatile (	"movs	r0, #45						\n\t"
					"mul	r0, r0, %[cr]				\n\t"
					"add	r0, %[yy], r0, asr #5		\n\t"
					"usat	%[r], #8, r0				\n\t"	//r
					"movs	r0, #23						\n\t"
					"mul	r1, r0, %[cr]				\n\t"
					"movs	r0, #11						\n\t"
					"mla	r0, r0, %[cb], r1			\n\t"
					"sub	r0, %[yy], r0, asr #5		\n\t"
					"usat	%[g], #8, r0				\n\t"	//g
					"movs	r0, #113					\n\t"
					"mul	r0, r0, %[cb]				\n\t"
					"add	r0, %[yy], r0, asr #6		\n\t"
					"usat	%[b], #8, r0				\n\t"	//b
					: [r]"=&r"(r), [g]"=&r"(g), [b]"=&r"(b)
					: [yy]"r"(yy), [cb]"r"(cb), [cr]"r"(cr)
					: "r0", "r1" );
	*r1 = r;
	*g1 = g;
	*b1 = b;
}

/*
 * Заполнение области памяти заданным значением шириной в слово (32 бита)
 * Copyright (C) 2023, VadRov, all right reserved.
 */
static inline void memset_32(void *mem, int value, unsigned int size)
{
	asm volatile (	"orrs	%[size], %[size]\n\t"
					"beq	2f\n\t"
					"1:\n\t"
					"str	%[value], [%[mem]], #4\n\t"
					"subs	%[size], #1\n\t"
					"bne	1b\n\t"
					"2:\n\t"
					: [mem]"+r"(mem), [size]"+r"(size)
					: [value]"r"(value)  );
}

/*
 * Заполнение области памяти заданным значением шириной в полуслово (16 бит)
 * Copyright (C) 2023, VadRov, all right reserved.
 */
static inline void memset_16(void *mem, int value, unsigned int size)
{
	asm volatile (	"orrs	%[size], %[size]\n\t"
					"beq	2f\n\t"
					"1:\n\t"
					"strh	%[value], [%[mem]], #2\n\t"
					"subs	%[size], #1\n\t"
					"bne	1b\n\t"
					"2:\n\t"
					: [mem]"+r"(mem), [size]"+r"(size)
					: [value]"r"(value)  );
}

/*
 * Заполнение области памяти заданным значением шириной в байт (8 бит)
 * Copyright (C) 2023, VadRov, all right reserved.
 */
static inline void memset_8(void *mem, int value, unsigned int size)
{
	asm volatile (	"orrs	%[size], %[size]\n\t"
					"beq	2f\n\t"
					"1:\n\t"
					"strb	%[value], [%[mem]], #1\n\t"
					"subs	%[size], #1\n\t"
					"bne	1b\n\t"
					"2:\n\t"
					: [mem]"+r"(mem), [size]"+r"(size)
					: [value]"r"(value)  );
}

/*
 * Копирование одной области памяти в другую (побайтно)
 * Copyright (C) 2023, VadRov, all right reserved.
 */
static inline void memcpy_8(void *dst, void *src, unsigned int size)
{
	asm volatile (	"orrs	%[size], %[size]\n\t"
					"beq	2f\n\t"
					"1:\n\t"
					"ldrb	r0, [%[src]], #1\n\t"
					"strb	r0, [%[dst]], #1\n\t"
					"subs	%[size], #1\n\t"
					"bne	1b\n\t"
					"2:\n\t"
					: [dst]"+r"(dst), [src]"+r"(src), [size]"+r"(size)
					:
					: "r0" );
}

/*
 * Копирование одной области памяти в другую (по полусловам)
 * Copyright (C) 2023, VadRov, all right reserved.
 */
static inline void memcpy_16(void *dst, void *src, unsigned int size)
{
	asm volatile (	"orrs	%[size], %[size]\n\t"
					"beq	2f\n\t"
					"1:\n\t"
					"ldrh	r0, [%[src]], #2\n\t"
					"strh	r0, [%[dst]], #2\n\t"
					"subs	%[size], #1\n\t"
					"bne	1b\n\t"
					"2:\n\t"
					: [dst]"+r"(dst), [src]"+r"(src), [size]"+r"(size)
					:
					: "r0" );
}

/*
 * Обратное дискретное косинусное преобразование по алгоритму Arai (Arai Algorithm)
 * см.: http://ijarse.com/images/fullpdf/1457613577_1003B.pdf
 * Copyright (C) 2023, VadRov, all right reserved.
 */
static inline void block_idct (int32_t* src, jd_yuv_t* dst)
{
	asm volatile(	"mov	r12, #8					\n\t"	//r12 = 8
					"1:								\n\t"	//метка 1
					"ldr	r2, [%[src]], #4		\n\t"	//v0, src += 4
					"ldr	r3, [%[src], #16*4-4]	\n\t"	//v1
					"ldr	r4, [%[src], #32*4-4]	\n\t"	//v2
					"ldr	r5, [%[src], #48*4-4]	\n\t"	//v3
					"mov	lr, #5792				\n\t"	//M13 = 5792
					"subs	r6, r3, r5				\n\t"	//v1 - v3
					"mul	r6, lr, r6				\n\t"	//M13 * (v1 - v3)
					"adds	r5, r5, r3				\n\t"	//v3 = v3 + v1
					"rsb	r6, r5, r6, asr #12		\n\t"	//t11 = M13 * (v1 - v3) >> 12 - v3n
					"subs	r7, r2, r4				\n\t"	//t12 = v0 - v2
					"adds	r3, r6, r7				\n\t"	//v1 = t11 + t12
					"add	r8, r2, r4				\n\t"	//t10 = v0 + v2
					"subs	r4, r7, r6				\n\t"	//v2 = t12 - t11
					"add	r2, r8, r5				\n\t"	//v0 = t10 + v3
					"sub	r5, r8, r5				\n\t"	//v3 = t10 - v3
					"ldr	r6, [%[src], #56*4-4]	\n\t"	//v4
					"ldr	r7, [%[src], #8*4-4]	\n\t"	//v5
					"ldr	r8, [%[src], #40*4-4]	\n\t"	//v6
					"ldr	r9, [%[src], #24*4-4]	\n\t"	//v7
					"add	r10, r7, r6				\n\t"	//t11 = v5 + v4
					"add	lr, r9, r8				\n\t"	//v7p = v7 + v6
					"add	r11, lr, r10			\n\t"	//v7n = v7p + t11
					"sub	r10, r10, lr			\n\t"	//t11 = t11 - v7p
					"add	lr, r2, r11				\n\t"	//v0 + v7
					"str	lr, [%[src], #-4]		\n\t"
					"sub	r2, r2, r11				\n\t"	//v0 - v7
					"str	r2, [%[src], #56*4-4]	\n\t"
					"mov	r2, #5792				\n\t"	//M13 = 5792
					"mul	lr, r2, r10				\n\t"	//t11 = M13 * t11
					"sub	r10, r7, r6				\n\t"	//t10 = v5 - v4
					"sub	r9, r8, r9				\n\t"	//t12 = v6 - v7
					"add	r6, r10, r9				\n\t"	//t10 + t12
					"mov	r2, #7568				\n\t"	//M5 = 7568
					"mul	r6, r2, r6				\n\t"	//M5 * (t10 + t12)
					"asrs	r6, r6, #12				\n\t"	//t13 = M5 * (t10 + t12) >> 12
					"mov	r2, #10703				\n\t"	//M4 = 10703
					"mul	r2, r2, r9				\n\t"	//M4 * t12
					"sub	r8, r6, r2, asr #12 	\n\t"	//v6 = t13 - M4 * t12>>12
					"sub	r8, r8, r11				\n\t"	//v6 = t13 - M4 * t12>>12 - v7n
					"rsb	r7, r8, lr, asr #12 	\n\t"   //v5 = M13 * (t11 - v7)>>12 - v6
					"mov	r2, #4433				\n\t"	//M2 = 4433
					"mul	r2, r2, r10				\n\t"	//M2 * t10
					"sub	r6, r6, r2, asr #12 	\n\t"	//v4 = t13 - (M2 * t10 >> 12)
					"subs	r6, r6, r7				\n\t"	//v4 = v4 - v5
					"add	r2, r3, r8				\n\t"	//v1 + v6
					"str	r2, [%[src], #8*4-4]	\n\t"
					"sub	r2, r3, r8				\n\t"	//v1 - v6
					"str	r2, [%[src], #48*4-4]	\n\t"
					"adds	r2, r4, r7				\n\t"	//v2 + v5
					"str	r2, [%[src], #16*4-4]	\n\t"
					"subs	r2, r4, r7				\n\t"	//v2 - v5
					"str	r2, [%[src], #40*4-4]	\n\t"
					"adds	r2, r5, r6				\n\t"	//v3 + v4
					"str	r2, [%[src], #24*4-4]	\n\t"
					"subs	r2, r5, r6				\n\t"	//v3 - v4
					"str	r2, [%[src], #32*4-4]	\n\t"
					"subs	r12, #1					\n\t"	//r12--
					"bne	1b						\n\t"	//r12 != 0 ? переход на метку 1 : выход из цикла
					"mov	r12, #8					\n\t"	//r12 = 8
					"subs	%[src], #32				\n\t"	//src -= 32
					"2:								\n\t"	//метка 2
					"ldr	r2, [%[src]], #32		\n\t"	//v0, src += 4
					"ldr	r3, [%[src], #2*4-32]	\n\t"	//v1
					"ldr	r4, [%[src], #4*4-32]	\n\t"	//v2
					"ldr	r5, [%[src], #6*4-32]	\n\t"	//v3
					"add	r2, #32768				\n\t"	//v0 += 32768
					"mov	lr, #5792				\n\t"	//M13 = 5792
					"subs	r6, r3, r5				\n\t"	//v1 - v3
					"mul	r6, lr, r6				\n\t"	//M13 * (v1 - v3)
					"adds	r5, r5, r3				\n\t"	//v3 = v3 + v1
					"rsb	r6, r5, r6, asr #12		\n\t"	//t11 = M13 * (v1 - v3) >> 12 - v3n
					"subs	r7, r2, r4				\n\t"	//t12 = v0 - v2
					"adds	r3, r6, r7				\n\t"	//v1 = t11 + t12
					"add	r8, r2, r4				\n\t"	//t10 = v0 + v2
					"subs	r4, r7, r6				\n\t"	//v2 = t12 - t11
					"add	r2, r8, r5				\n\t"	//v0 = t10 + v3
					"sub	r5, r8, r5				\n\t"	//v3 = t10 - v3
					"ldr	r6, [%[src], #7*4-32]	\n\t"	//v4
					"ldr	r7, [%[src], #1*4-32]	\n\t"	//v5
					"ldr	r8, [%[src], #5*4-32]	\n\t"	//v6
					"ldr	r9, [%[src], #3*4-32]	\n\t"	//v7
					"add	r10, r7, r6				\n\t"	//t11 = v5 + v4
					"add	lr, r9, r8				\n\t"	//v7p = v7 + v6
					"add	r11, lr, r10			\n\t"	//v7n = v7p + t11
					"sub	r10, r10, lr			\n\t"	//t11 = t11 - v7p
					"add	lr, r2, r11				\n\t"	//v0 + v7
					"asr	lr, lr, #8				\n\t"	//(v0 + v7) >> 8
					"strh	lr, [%[dst]], #16		\n\t"	//dst += 16
					"sub	r2, r2, r11				\n\t"	//v0 - v7
					"asrs	r2, r2, #8				\n\t"	//(v0 - v7) >> 8
					"strh	r2, [%[dst], #7*2-16]	\n\t"
					"mov	r2, #5792				\n\t"	//M13 = 5792
					"mul	lr, r2, r10				\n\t"	//t11 = M13 * t11
					"sub	r10, r7, r6				\n\t"	//t10 = v5 - v4
					"sub	r9, r8, r9				\n\t"	//t12 = v6 - v7
					"add	r6, r10, r9				\n\t"	//t10 + t12
					"mov	r2, #7568				\n\t"	//M5 = 7568
					"mul	r6, r2, r6				\n\t"	//M5 * (t10 + t12)
					"asrs	r6, r6, #12				\n\t"	//t13 = M5 * (t10 + t12) >> 12
					"mov	r2, #10703				\n\t"	//M4 = 10703
					"mul	r2, r2, r9				\n\t"	//M4 * t12
					"sub	r8, r6, r2, asr #12 	\n\t"	//v6 = t13 - M4 * t12>>12
					"sub	r8, r8, r11				\n\t"	//v6 = t13 - M4 * t12>>12 - v7n
					"rsb	r7, r8, lr, asr #12 	\n\t"   //v5 = M13 * (t11 - v7)>>12 - v6
					"mov	r2, #4433				\n\t"	//M2 = 4433
					"mul	r2, r2, r10				\n\t"	//M2 * t10
					"sub	r6, r6, r2, asr #12 	\n\t"	//v4 = t13 - (M2 * t10 >> 12)
					"subs	r6, r6, r7				\n\t"	//v4 = v4 - v5
					"add	r2, r3, r8				\n\t"	//v1 + v6
					"asrs	r2, r2, #8				\n\t"	//(v1 + v6) >> 8
					"strh	r2, [%[dst], #1*2-16]	\n\t"
					"sub	r2, r3, r8				\n\t"	//v1 - v6
					"asrs	r2, r2, #8				\n\t"	//(v1 - v6) >> 8
					"strh	r2, [%[dst], #6*2-16]	\n\t"
					"adds	r2, r4, r7				\n\t"	//v2 + v5
					"asrs	r2, r2, #8				\n\t"	//(v2 + v5) >> 8
					"strh	r2, [%[dst], #2*2-16]	\n\t"
					"subs	r2, r4, r7				\n\t"	//v2 - v5
					"asrs	r2, r2, #8				\n\t"	//(v2 - v5) >> 8
					"strh	r2, [%[dst], #5*2-16]	\n\t"
					"adds	r2, r5, r6				\n\t"	//v3 + v4
					"asrs	r2, r2, #8				\n\t"	//(v3 + v4) >> 8
					"strh	r2, [%[dst], #3*2-16]	\n\t"
					"subs	r2, r5, r6				\n\t"	//v3 - v4
					"asrs	r2, r2, #8				\n\t"	//(v3 - v4) >> 8
					"strh	r2, [%[dst], #4*2-16]	\n\t"
					"subs	r12, #1					\n\t"	//r12--
					"bne	2b						\n\t"	//r12 != 0 ? переход на метку 2 : выход из цикла
					:[src]"+r"(src), [dst]"+r"(dst)
					:
					:"r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "lr");
}

#endif

/*-----------------------------------------------*/
/* Zigzag-order to raster-order conversion table */
/*-----------------------------------------------*/

static const uint8_t Zig[64] = {	/* Zigzag-order to raster-order conversion table */
	 0,  1,  8, 16,  9,  2,  3, 10, 17, 24, 32, 25, 18, 11,  4,  5,
	12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13,  6,  7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};



/*-------------------------------------------------*/
/* Input scale factor of Arai algorithm            */
/* (scaled up  16 bits for fixed point operations) */
/*-------------------------------------------------*/

static const uint16_t Ipsf[64] = {	/* See also aa_idct.png */
	(uint16_t)(1.00000*8192), (uint16_t)(1.38704*8192), (uint16_t)(1.30656*8192), (uint16_t)(1.17588*8192), (uint16_t)(1.00000*8192), (uint16_t)(0.78570*8192), (uint16_t)(0.54120*8192), (uint16_t)(0.27590*8192),
	(uint16_t)(1.38704*8192), (uint16_t)(1.92388*8192), (uint16_t)(1.81226*8192), (uint16_t)(1.63099*8192), (uint16_t)(1.38704*8192), (uint16_t)(1.08979*8192), (uint16_t)(0.75066*8192), (uint16_t)(0.38268*8192),
	(uint16_t)(1.30656*8192), (uint16_t)(1.81226*8192), (uint16_t)(1.70711*8192), (uint16_t)(1.53636*8192), (uint16_t)(1.30656*8192), (uint16_t)(1.02656*8192), (uint16_t)(0.70711*8192), (uint16_t)(0.36048*8192),
	(uint16_t)(1.17588*8192), (uint16_t)(1.63099*8192), (uint16_t)(1.53636*8192), (uint16_t)(1.38268*8192), (uint16_t)(1.17588*8192), (uint16_t)(0.92388*8192), (uint16_t)(0.63638*8192), (uint16_t)(0.32442*8192),
	(uint16_t)(1.00000*8192), (uint16_t)(1.38704*8192), (uint16_t)(1.30656*8192), (uint16_t)(1.17588*8192), (uint16_t)(1.00000*8192), (uint16_t)(0.78570*8192), (uint16_t)(0.54120*8192), (uint16_t)(0.27590*8192),
	(uint16_t)(0.78570*8192), (uint16_t)(1.08979*8192), (uint16_t)(1.02656*8192), (uint16_t)(0.92388*8192), (uint16_t)(0.78570*8192), (uint16_t)(0.61732*8192), (uint16_t)(0.42522*8192), (uint16_t)(0.21677*8192),
	(uint16_t)(0.54120*8192), (uint16_t)(0.75066*8192), (uint16_t)(0.70711*8192), (uint16_t)(0.63638*8192), (uint16_t)(0.54120*8192), (uint16_t)(0.42522*8192), (uint16_t)(0.29290*8192), (uint16_t)(0.14932*8192),
	(uint16_t)(0.27590*8192), (uint16_t)(0.38268*8192), (uint16_t)(0.36048*8192), (uint16_t)(0.32442*8192), (uint16_t)(0.27590*8192), (uint16_t)(0.21678*8192), (uint16_t)(0.14932*8192), (uint16_t)(0.07612*8192)
};



/*---------------------------------------------*/
/* Conversion table for fast clipping process  */
/*---------------------------------------------*/

#if (JD_FAST_STM32 == 0)

#if JD_TBLCLIP

#if (JD_FORMAT == 0 || JD_FORMAT == 1 || (JD_FORMAT == 3 && JD_FASTDECODE == 0))

#define BYTECLIP(v) Clip8[(unsigned int)(v) & 0x3FF]

static const uint8_t Clip8[1024] = {
	/* 0..255 */
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
	64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
	96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
	128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
	192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
	224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
	/* 256..511 */
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	/* -512..-257 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* -256..-1 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
#endif

#else	/* JD_TBLCLIP */

static inline uint8_t BYTECLIP (int val)
{
	if (val < 0) return 0;
	if (val > 255) return 255;
	return (uint8_t)val;
}

#endif

#endif //cortex_m

/*-----------------------------------------------------------------------*/
/* Allocate a memory block from memory pool                              */
/*-----------------------------------------------------------------------*/

static void* alloc_pool (	/* Pointer to allocated memory block (NULL:no memory available) */
	JDEC* jd,				/* Pointer to the decompressor object */
	size_t ndata			/* Number of bytes to allocate */
)
{
	char *rp = 0;


	ndata = (ndata + 3) & ~3;			/* Align block size to the word boundary */

	if (jd->sz_pool >= ndata) {
		jd->sz_pool -= ndata;
		rp = (char*)jd->pool;			/* Get start of available memory pool */
		jd->pool = (void*)(rp + ndata);	/* Allocate requierd bytes */
	}

	return (void*)rp;	/* Return allocated memory block (NULL:no memory to allocate) */
}




/*-----------------------------------------------------------------------*/
/* Create de-quantization and prescaling tables with a DQT segment       */
/*-----------------------------------------------------------------------*/

static JRESULT create_qt_tbl (	/* 0:OK, !0:Failed */
	JDEC* jd,				/* Pointer to the decompressor object */
	const uint8_t* data,	/* Pointer to the quantizer tables */
	size_t ndata			/* Size of input data */
)
{
	unsigned int i, zi;
	uint8_t d;
	int32_t *pb;
	while (ndata) {	/* Process all tables in the segment */
		if (ndata < 65) return JDR_FMT1;	/* Err: table size is unaligned */
		ndata -= 65;
		d = *data++;							/* Get table property */
		if (d & 0xF0) return JDR_FMT1;			/* Err: not 8-bit resolution */
		i = d & 3;								/* Get table ID */
		pb = alloc_pool(jd, 64 * sizeof (int32_t));/* Allocate a memory block for the table */
		if (!pb) return JDR_MEM1;				/* Err: not enough memory */
		jd->qttbl[i] = pb;						/* Register the table */
		for (i = 0; i < 64; i++) {				/* Load the table */
			zi = Zig[i];						/* Zigzag-order to raster-order conversion */
			pb[zi] = (int32_t)((uint32_t)*data++ * Ipsf[zi]);	/* Apply scale factor of Arai algorithm to the de-quantizers */
		}
	}

	return JDR_OK;
}




/*-----------------------------------------------------------------------*/
/* Create huffman code tables with a DHT segment                         */
/*-----------------------------------------------------------------------*/

static JRESULT create_huffman_tbl (	/* 0:OK, !0:Failed */
	JDEC* jd,					/* Pointer to the decompressor object */
	const uint8_t* data,		/* Pointer to the packed huffman tables */
	size_t ndata				/* Size of input data */
)
{
	unsigned int i, j, b, cls, num;
	size_t np;
	uint8_t d, *pb, *pd;
	uint16_t hc, *ph;


	while (ndata) {	/* Process all tables in the segment */
		if (ndata < 17) return JDR_FMT1;	/* Err: wrong data size */
		ndata -= 17;
		d = *data++;						/* Get table number and class */
		if (d & 0xEE) return JDR_FMT1;		/* Err: invalid class/number */
		cls = d >> 4; num = d & 0x0F;		/* class = dc(0)/ac(1), table number = 0/1 */
		pb = alloc_pool(jd, 16);			/* Allocate a memory block for the bit distribution table */
		if (!pb) return JDR_MEM1;			/* Err: not enough memory */
		jd->huffbits[num][cls] = pb;
		for (np = i = 0; i < 16; i++) {		/* Load number of patterns for 1 to 16-bit code */
			np += (pb[i] = *data++);		/* Get sum of code words for each code */
		}
		ph = alloc_pool(jd, np * sizeof (uint16_t));/* Allocate a memory block for the code word table */
		if (!ph) return JDR_MEM1;			/* Err: not enough memory */
		jd->huffcode[num][cls] = ph;
		hc = 0;
		for (j = i = 0; i < 16; i++) {		/* Re-build huffman code word table */
			b = pb[i];
			while (b--) ph[j++] = hc++;
			hc <<= 1;
		}

		if (ndata < np) return JDR_FMT1;	/* Err: wrong data size */
		ndata -= np;
		pd = alloc_pool(jd, np);			/* Allocate a memory block for the decoded data */
		if (!pd) return JDR_MEM1;			/* Err: not enough memory */
		jd->huffdata[num][cls] = pd;
		for (i = 0; i < np; i++) {			/* Load decoded data corresponds to each code word */
			d = *data++;
			if (!cls && d > 11) return JDR_FMT1;
			pd[i] = d;
		}
#if (JD_FASTDECODE == 2)
		{	/* Create fast huffman decode table */
			unsigned int td, ti;
#if (JD_FAST_STM32 == 0)
			unsigned int span;
#endif
			uint16_t *tbl_ac = 0;
			uint8_t *tbl_dc = 0;
			if (cls) {
				tbl_ac = alloc_pool(jd, HUFF_LEN * sizeof (uint16_t));	/* LUT for AC elements */
				if (!tbl_ac) return JDR_MEM1;		/* Err: not enough memory */
				jd->hufflut_ac[num] = tbl_ac;
#if (JD_FAST_STM32 == 0)
				memset(tbl_ac, 0xFF, HUFF_LEN * sizeof (uint16_t));		/* Default value (0xFFFF: may be long code) */
#else
				memset_32(tbl_ac, 0xffffffff, HUFF_LEN/2); //оптимизация
#endif
			} else {
				tbl_dc = alloc_pool(jd, HUFF_LEN * sizeof (uint8_t));	/* LUT for AC elements */
				if (!tbl_dc) return JDR_MEM1;		/* Err: not enough memory */
				jd->hufflut_dc[num] = tbl_dc;
#if (JD_FAST_STM32 == 0)
				memset(tbl_dc, 0xFF, HUFF_LEN * sizeof (uint8_t));		/* Default value (0xFF: may be long code) */ //оригинал
#else
				memset_32(tbl_dc, 0xffffffff, HUFF_LEN/4); //оптимизация
#endif
			}
			for (i = b = 0; b < HUFF_BIT; b++) {	/* Create LUT */
				for (j = pb[b]; j; j--) {
					ti = ph[i] << (HUFF_BIT - 1 - b) & HUFF_MASK;	/* Index of input pattern for the code */
					if (cls) {
						td = pd[i++] | ((b + 1) << 8);	/* b15..b8: code length, b7..b0: zero run and data length */
#if (JD_FAST_STM32 == 0)
						for (span = 1 << (HUFF_BIT - 1 - b); span; span--, tbl_ac[ti++] = (uint16_t)td) ; //оригинал
#else
						memset_16(&tbl_ac[ti], (uint16_t)td, 1 << (HUFF_BIT - 1 - b)); //оптимизация
#endif
					} else {
						td = pd[i++] | ((b + 1) << 4);	/* b7..b4: code length, b3..b0: data length */
#if (JD_FAST_STM32 == 0)
						for (span = 1 << (HUFF_BIT - 1 - b); span; span--, tbl_dc[ti++] = (uint8_t)td) ; //оригинал
#else
						memset_8(&tbl_dc[ti], (uint8_t)td, 1 << (HUFF_BIT - 1 - b)); //оптимизация
#endif
					}
				}
			}
			jd->longofs[num][cls] = i;	/* Code table offset for long code */
		}
#endif
	}

	return JDR_OK;
}


/*-----------------------------------------------------------------------*/
/* Extract a huffman decoded data from input stream                      */
/*-----------------------------------------------------------------------*/

static int huffext (	/* >=0: decoded data, <0: error code */
	JDEC* jd,			/* Pointer to the decompressor object */
	unsigned int id,	/* Table ID (0:Y, 1:C) */
	unsigned int cls	/* Table class (0:DC, 1:AC) */
)
{
	size_t dc = jd->dctr;
	uint8_t *dp = jd->dptr;
	unsigned int d, flg = 0;

#if JD_FASTDECODE == 0
	uint8_t bm, nd, bl;
	const uint8_t *hb = jd->huffbits[id][cls];	/* Bit distribution table */
	const uint16_t *hc = jd->huffcode[id][cls];	/* Code word table */
	const uint8_t *hd = jd->huffdata[id][cls];	/* Data table */


	bm = jd->dbit;	/* Bit mask to extract */
	d = 0; bl = 16;	/* Max code length */
	do {
		if (!bm) {		/* Next byte? */
			if (!dc) {	/* No input data is available, re-fill input buffer */
				dp = jd->inbuf;	/* Top of input buffer */
				dc = jd->infunc(jd, dp, JD_SZBUF);
				if (!dc) return 0 - (int)JDR_INP;	/* Err: read error or wrong stream termination */
			} else {
				dp++;	/* Next data ptr */
			}
			dc--;		/* Decrement number of available bytes */
			if (flg) {		/* In flag sequence? */
				flg = 0;	/* Exit flag sequence */
				if (*dp != 0) return 0 - (int)JDR_FMT1;	/* Err: unexpected flag is detected (may be collapted data) */
				*dp = 0xFF;				/* The flag is a data 0xFF */
			} else {
				if (*dp == 0xFF) {		/* Is start of flag sequence? */
					flg = 1; continue;	/* Enter flag sequence, get trailing byte */
				}
			}
			bm = 0x80;		/* Read from MSB */
		}
		d <<= 1;			/* Get a bit */
		if (*dp & bm) d++;
		bm >>= 1;

		for (nd = *hb++; nd; nd--) {	/* Search the code word in this bit length */
			if (d == *hc++) {	/* Matched? */
				jd->dbit = bm; jd->dctr = dc; jd->dptr = dp;
				return *hd;		/* Return the decoded data */
			}
			hd++;
		}
		bl--;
	} while (bl);

#else
	const uint8_t *hb, *hd;
	const uint16_t *hc;
	unsigned int nc, bl, wbit = jd->dbit % 32;
	uint32_t w = jd->wreg & ((1UL << wbit) - 1);


	while (wbit < 16) {	/* Prepare 16 bits into the working register */
		if (jd->marker) {
			d = 0xFF;	/* Input stream has stalled for a marker. Generate stuff bits */
		} else {
			if (!dc) {	/* Buffer empty, re-fill input buffer */
				dp = jd->inbuf;						/* Top of input buffer */
				dc = jd->infunc(jd, dp, JD_SZBUF);
				if (!dc) return 0 - (int)JDR_INP;	/* Err: read error or wrong stream termination */
			}
			d = *dp++; dc--;
			if (flg) {		/* In flag sequence? */
				flg = 0;	/* Exit flag sequence */
				if (d != 0) jd->marker = d;	/* Not an escape of 0xFF but a marker */
				d = 0xFF;
			} else {
				if (d == 0xFF) {		/* Is start of flag sequence? */
					flg = 1; continue;	/* Enter flag sequence, get trailing byte */
				}
			}
		}
		w = w << 8 | d;	/* Shift 8 bits in the working register */
		wbit += 8;
	}
	jd->dctr = dc; jd->dptr = dp;
	jd->wreg = w;

#if JD_FASTDECODE == 2
	/* Table serch for the short codes */
	d = (unsigned int)(w >> (wbit - HUFF_BIT));	/* Short code as table index */
	if (cls) {	/* AC element */
		d = jd->hufflut_ac[id][d];	/* Table decode */
		if (d != 0xFFFF) {	/* It is done if hit in short code */
			jd->dbit = wbit - (d >> 8);	/* Snip the code length */
			return d & 0xFF;	/* b7..0: zero run and following data bits */
		}
	} else {	/* DC element */
		d = jd->hufflut_dc[id][d];	/* Table decode */
		if (d != 0xFF) {	/* It is done if hit in short code */
			jd->dbit = wbit - (d >> 4);	/* Snip the code length  */
			return d & 0xF;	/* b3..0: following data bits */
		}
	}

	/* Incremental serch for the codes longer than HUFF_BIT */
	hb = jd->huffbits[id][cls] + HUFF_BIT;				/* Bit distribution table */
	hc = jd->huffcode[id][cls] + jd->longofs[id][cls];	/* Code word table */
	hd = jd->huffdata[id][cls] + jd->longofs[id][cls];	/* Data table */
	bl = HUFF_BIT + 1;
#else
	/* Incremental serch for all codes */
	hb = jd->huffbits[id][cls];	/* Bit distribution table */
	hc = jd->huffcode[id][cls];	/* Code word table */
	hd = jd->huffdata[id][cls];	/* Data table */
	bl = 1;
#endif
	for ( ; bl <= 16; bl++) {	/* Incremental search */
		nc = *hb++;
		if (nc) {
			d = w >> (wbit - bl);
			do {	/* Search the code word in this bit length */
				if (d == *hc++) {		/* Matched? */
					jd->dbit = wbit - bl;	/* Snip the huffman code */
					return *hd;			/* Return the decoded data */
				}
				hd++;
			} while (--nc);
		}
	}
#endif

	return 0 - (int)JDR_FMT1;	/* Err: code not found (may be collapted data) */
}




/*-----------------------------------------------------------------------*/
/* Extract N bits from input stream                                      */
/*-----------------------------------------------------------------------*/

static inline int bitext (	/* >=0: extracted data, <0: error code */
	JDEC* jd,			/* Pointer to the decompressor object */
	unsigned int nbit	/* Number of bits to extract (1 to 16) */
)
{
	size_t dc = jd->dctr;
	uint8_t *dp = jd->dptr;
	unsigned int d, flg = 0;

#if JD_FASTDECODE == 0
	uint8_t mbit = jd->dbit;

	d = 0;
	do {
		if (!mbit) {			/* Next byte? */
			if (!dc) {			/* No input data is available, re-fill input buffer */
				dp = jd->inbuf;	/* Top of input buffer */
				dc = jd->infunc(jd, dp, JD_SZBUF);
				if (!dc) return 0 - (int)JDR_INP;	/* Err: read error or wrong stream termination */
			} else {
				dp++;			/* Next data ptr */
			}
			dc--;				/* Decrement number of available bytes */
			if (flg) {			/* In flag sequence? */
				flg = 0;		/* Exit flag sequence */
				if (*dp != 0) return 0 - (int)JDR_FMT1;	/* Err: unexpected flag is detected (may be collapted data) */
				*dp = 0xFF;		/* The flag is a data 0xFF */
			} else {
				if (*dp == 0xFF) {		/* Is start of flag sequence? */
					flg = 1; continue;	/* Enter flag sequence */
				}
			}
			mbit = 0x80;		/* Read from MSB */
		}
		d <<= 1;	/* Get a bit */
		if (*dp & mbit) d |= 1;
		mbit >>= 1;
		nbit--;
	} while (nbit);

	jd->dbit = mbit; jd->dctr = dc; jd->dptr = dp;
	return (int)d;

#else
	unsigned int wbit = jd->dbit % 32;
	uint32_t w = jd->wreg & ((1UL << wbit) - 1);


	while (wbit < nbit) {	/* Prepare nbit bits into the working register */
		if (jd->marker) {
			d = 0xFF;	/* Input stream stalled, generate stuff bits */
		} else {
			if (!dc) {	/* Buffer empty, re-fill input buffer */
				dp = jd->inbuf;	/* Top of input buffer */
				dc = jd->infunc(jd, dp, JD_SZBUF);
				if (!dc) return 0 - (int)JDR_INP;	/* Err: read error or wrong stream termination */
			}
			d = *dp++; dc--;
			if (flg) {		/* In flag sequence? */
				flg = 0;	/* Exit flag sequence */
				if (d != 0) jd->marker = d;	/* Not an escape of 0xFF but a marker */
				d = 0xFF;
			} else {
				if (d == 0xFF) {		/* Is start of flag sequence? */
					flg = 1; continue;	/* Enter flag sequence, get trailing byte */
				}
			}
		}
		w = w << 8 | d;	/* Get 8 bits into the working register */
		wbit += 8;
	}
	jd->wreg = w; jd->dbit = wbit - nbit;
	jd->dctr = dc; jd->dptr = dp;

	return (int)(w >> ((wbit - nbit) % 32));
#endif
}




/*-----------------------------------------------------------------------*/
/* Process restart interval                                              */
/*-----------------------------------------------------------------------*/

static JRESULT restart (
	JDEC* jd,		/* Pointer to the decompressor object */
	uint16_t rstn	/* Expected restert sequense number */
)
{
	unsigned int i;
	uint8_t *dp = jd->dptr;
	size_t dc = jd->dctr;

#if JD_FASTDECODE == 0
	uint16_t d = 0;

	/* Get two bytes from the input stream */
	for (i = 0; i < 2; i++) {
		if (!dc) {	/* No input data is available, re-fill input buffer */
			dp = jd->inbuf;
			dc = jd->infunc(jd, dp, JD_SZBUF);
			if (!dc) return JDR_INP;
		} else {
			dp++;
		}
		dc--;
		d = d << 8 | *dp;	/* Get a byte */
	}
	jd->dptr = dp; jd->dctr = dc; jd->dbit = 0;

	/* Check the marker */
	if ((d & 0xFFD8) != 0xFFD0 || (d & 7) != (rstn & 7)) {
		return JDR_FMT1;	/* Err: expected RSTn marker is not detected (may be collapted data) */
	}

#else
	uint16_t marker;


	if (jd->marker) {	/* Generate a maker if it has been detected */
		marker = 0xFF00 | jd->marker;
		jd->marker = 0;
	} else {
		marker = 0;
		for (i = 0; i < 2; i++) {	/* Get a restart marker */
			if (!dc) {		/* No input data is available, re-fill input buffer */
				dp = jd->inbuf;
				dc = jd->infunc(jd, dp, JD_SZBUF);
				if (!dc) return JDR_INP;
			}
			marker = (marker << 8) | *dp++;	/* Get a byte */
			dc--;
		}
		jd->dptr = dp; jd->dctr = dc;
	}

	/* Check the marker */
	if ((marker & 0xFFD8) != 0xFFD0 || (marker & 7) != (rstn & 7)) {
		return JDR_FMT1;	/* Err: expected RSTn marker was not detected (may be collapted data) */
	}

	jd->dbit = 0;			/* Discard stuff bits */
#endif

	jd->dcv[2] = jd->dcv[1] = jd->dcv[0] = 0;	/* Reset DC offset */
	return JDR_OK;
}

/*-----------------------------------------------------------------------*/
/* Apply Inverse-DCT in Arai Algorithm (see also aa_idct.png)            */
/*-----------------------------------------------------------------------*/

#if (JD_FAST_STM32 == 0)
static inline void block_idct (int32_t* src, jd_yuv_t* dst)
{
	const int32_t M13 = (int32_t)(1.41421*4096), M2 = (int32_t)(1.08239*4096), M4 = (int32_t)(2.61313*4096), M5 = (int32_t)(1.84776*4096);
	int32_t v0, v1, v2, v3, v4, v5, v6, v7;
	int32_t t10, t11, t12, t13;
	int i;

	for (i = 0; i < 8; i++) {
		v0 = src[8 * 0];
		v1 = src[8 * 2];
		v2 = src[8 * 4];
		v3 = src[8 * 6];

		t10 = v0 + v2;
		t12 = v0 - v2;
		t11 = (v1 - v3) * M13 >> 12;
		v3 += v1;
		t11 -= v3;
		v0 = t10 + v3;
		v3 = t10 - v3;
		v1 = t11 + t12;
		v2 = t12 - t11;

		v4 = src[8 * 7];
		v5 = src[8 * 1];
		v6 = src[8 * 5];
		v7 = src[8 * 3];

		t10 = v5 - v4;
		t11 = v5 + v4;
		t12 = v6 - v7;
		v7 += v6;
		v5 = (t11 - v7) * M13 >> 12;
		v7 += t11;
		t13 = (t10 + t12) * M5 >> 12;
		v4 = t13 - (t10 * M2 >> 12);
		v6 = t13 - (t12 * M4 >> 12) - v7;
		v5 -= v6;
		v4 -= v5;

		src[8 * 0] = v0 + v7;
		src[8 * 7] = v0 - v7;
		src[8 * 1] = v1 + v6;
		src[8 * 6] = v1 - v6;
		src[8 * 2] = v2 + v5;
		src[8 * 5] = v2 - v5;
		src[8 * 3] = v3 + v4;
		src[8 * 4] = v3 - v4;

		src++;
	}

	src -= 8;
	for (i = 0; i < 8; i++) {
		v0 = src[0] + (128L << 8);
		v1 = src[2];
		v2 = src[4];
		v3 = src[6];

		t10 = v0 + v2;
		t12 = v0 - v2;
		t11 = (v1 - v3) * M13 >> 12;
		v3 += v1;
		t11 -= v3;
		v0 = t10 + v3;
		v3 = t10 - v3;
		v1 = t11 + t12;
		v2 = t12 - t11;

		v4 = src[7];
		v5 = src[1];
		v6 = src[5];
		v7 = src[3];

		t10 = v5 - v4;
		t11 = v5 + v4;
		t12 = v6 - v7;
		v7 += v6;
		v5 = (t11 - v7) * M13 >> 12;
		v7 += t11;
		t13 = (t10 + t12) * M5 >> 12;
		v4 = t13 - (t10 * M2 >> 12);
		v6 = t13 - (t12 * M4 >> 12) - v7;
		v5 -= v6;
		v4 -= v5;

#if JD_FASTDECODE >= 1
		dst[0] = (int16_t)((v0 + v7) >> 8);
		dst[7] = (int16_t)((v0 - v7) >> 8);
		dst[1] = (int16_t)((v1 + v6) >> 8);
		dst[6] = (int16_t)((v1 - v6) >> 8);
		dst[2] = (int16_t)((v2 + v5) >> 8);
		dst[5] = (int16_t)((v2 - v5) >> 8);
		dst[3] = (int16_t)((v3 + v4) >> 8);
		dst[4] = (int16_t)((v3 - v4) >> 8);
#else
		dst[0] = BYTECLIP((v0 + v7) >> 8);
		dst[7] = BYTECLIP((v0 - v7) >> 8);
		dst[1] = BYTECLIP((v1 + v6) >> 8);
		dst[6] = BYTECLIP((v1 - v6) >> 8);
		dst[2] = BYTECLIP((v2 + v5) >> 8);
		dst[5] = BYTECLIP((v2 - v5) >> 8);
		dst[3] = BYTECLIP((v3 + v4) >> 8);
		dst[4] = BYTECLIP((v3 - v4) >> 8);
#endif

		dst += 8; src += 8;
	}
}
#endif

/*-----------------------------------------------------------------------*/
/* Load all blocks in an MCU into working buffer                         */
/*-----------------------------------------------------------------------*/

static inline JRESULT mcu_load (
	JDEC* jd		/* Pointer to the decompressor object */
)
{
	//int32_t *tmp = (int32_t*)jd->workbuf; /* Block working buffer for de-quantize and IDCT */
	//++++++++++++++++++++++ Изменения для двойного буфера ++++++++++++++++++++++
	int32_t *tmp = (int32_t*)(jd->workbuf + jd->offset);
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	int d, e;
	unsigned int blk, nby, i, bc, z, id, cmp;
	jd_yuv_t *bp;
	const int32_t *dqf;

	nby = jd->msx * jd->msy;	/* Number of Y blocks (1, 2 or 4) */
	bp = jd->mcubuf;			/* Pointer to the first block of MCU */

	for (blk = 0; blk < nby + 2; blk++) {	/* Get nby Y blocks and two C blocks */
		cmp = (blk < nby) ? 0 : blk - nby + 1;	/* Component number 0:Y, 1:Cb, 2:Cr */

		if (cmp && jd->ncomp != 3) {		/* Clear C blocks if not exist (monochrome image) */
#if (JD_FAST_STM32 == 0)
			for (i = 0; i < 64; bp[i++] = 128) ; //оригинал
#else
			memset_32(bp, 0x00800080, 64/2); //вариант оптимизации
#endif
		} else {							/* Load Y/C blocks from input stream */
			id = cmp ? 1 : 0;						/* Huffman table ID of this component */

			/* Extract a DC element from input stream */
			d = huffext(jd, id, 0);					/* Extract a huffman coded data (bit length) */
			if (d < 0) return (JRESULT)(0 - d);		/* Err: invalid code or input */
			bc = (unsigned int)d;
			d = jd->dcv[cmp];						/* DC value of previous block */
			if (bc) {								/* If there is any difference from previous block */
				e = bitext(jd, bc);					/* Extract data bits */
				if (e < 0) return (JRESULT)(0 - e);	/* Err: input */
				bc = 1 << (bc - 1);					/* MSB position */
				if (!(e & bc)) e -= (bc << 1) - 1;	/* Restore negative value if needed */
				d += e;								/* Get current value */
				jd->dcv[cmp] = (int16_t)d;			/* Save current DC value for next block */
			}
			dqf = jd->qttbl[jd->qtid[cmp]];			/* De-quantizer table ID for this component */
			tmp[0] = d * dqf[0] >> 8;				/* De-quantize, apply scale factor of Arai algorithm and descale 8 bits */

			/* Extract following 63 AC elements from input stream */
#if (JD_FAST_STM32 == 0)
			memset(&tmp[1], 0, 63 * sizeof (int32_t));	/* Initialize all AC elements */ //оригинал
#else
			memset_32(&tmp[1], 0, 63); //вариант оптимизации
#endif
			z = 1;		/* Top of the AC elements (in zigzag-order) */
			do {
				d = huffext(jd, id, 1);				/* Extract a huffman coded value (zero runs and bit length) */
				if (d == 0) break;					/* EOB? */
				if (d < 0) return (JRESULT)(0 - d);	/* Err: invalid code or input error */
				bc = (unsigned int)d;
				z += bc >> 4;						/* Skip leading zero run */
				if (z >= 64) return JDR_FMT1;		/* Too long zero run */
				if (bc &= 0x0F) {					/* Bit length? */
					d = bitext(jd, bc);				/* Extract data bits */
					if (d < 0) return (JRESULT)(0 - d);	/* Err: input device */
					bc = 1 << (bc - 1);				/* MSB position */
					if (!(d & bc)) d -= (bc << 1) - 1;	/* Restore negative value if needed */
					i = Zig[z];						/* Get raster-order index */
					tmp[i] = d * dqf[i] >> 8;		/* De-quantize, apply scale factor of Arai algorithm and descale 8 bits */
				}
			} while (++z < 64);		/* Next AC element */

			if ((jd->color_format != 2 && jd->color_format != 3) || !cmp) {	/* C components may not be processed if in grayscale output */
				if (z == 1 || (JD_USE_SCALE && jd->scale == 3)) {	/* If no AC element or scale ratio is 1/8, IDCT can be ommited and the block is filled with DC value */
					d = (jd_yuv_t)((*tmp / 256) + 128);
					if (JD_FASTDECODE >= 1) {
#if (JD_FAST_STM32 == 0)
						for (i = 0; i < 64; bp[i++] = d) ; //оригинал
#else
						memset_32(bp, (d << 16) | d, 64/2); //вариант оптимизации
#endif
					} else {
#if (JD_FAST_STM32 == 0)
						memset(bp, d, 64); //оригинал
#else
						//оптимизация
						d &= 0xff;
						memset_32(bp, (d << 24 ) | (d << 16) | (d << 8) | d, 64/4);
#endif
					}
				} else {
					block_idct(tmp, bp);	/* Apply IDCT and store the block to the MCU buffer */
				}
			}
		}
		bp += 64;				/* Next block */
	}

	return JDR_OK;	/* All blocks have been loaded successfully */
}

#if (JD_FAST_STM32 == 1) //JD_FAST_STM32 = 1
/*-----------------------------------------------------------------------*/
/* Output an MCU: Convert YCrCb to RGB and output it in RGB form         */
/*-----------------------------------------------------------------------*/
static inline JRESULT mcu_output (
	JDEC* jd,								//Указатель на объект декодера
	int (*outfunc)(JDEC*, void*, JRECT*),	//Указатель на функцию вывода блока данных с изображением на дисплей
	unsigned int x,							//Координаты вывода блока данных с изображением на дисплей
	unsigned int y
)
{
	//++++++++++++ workbuf для двойного буфера ++++++++++++
	void *workbuf = jd->workbuf + jd->offset;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++
	unsigned int ix, iy, mx, my, rx, ry;
	int yy, cb, cr;
	jd_yuv_t *pc, *py;
	uint8_t *pix = (uint8_t*)workbuf;
	uint8_t r1 = 0, g1 = 0, b1 = 0;
	JRECT rect;

	mx = jd->msx * 8; my = jd->msy * 8;					//Ширина и высота блока изображения, пиксели
	rx = (x + mx <= jd->width) ? mx : jd->width - x;	//Ширина и высота блока изображения для вывода в буфер кадра (на дисплей)
	ry = (y + my <= jd->height) ? my : jd->height - y;	//Возможно, потребуется "подрезка" блока, если он выходит за границы изображения
	if (JD_USE_SCALE) {									//Если разрешено масштабирование:
		rx >>= jd->scale; ry >>= jd->scale;				//Масштабируем размеры блока для вывода в буфер кадра (на дисплей)
		if (!rx || !ry) return JDR_OK;					//Если ширина или высота блока для вывода в буфер кадра (на дисплей) равна 0, выходим.
			x >>= jd->scale; y >>= jd->scale;			//Масштабируем координаты блока.

	}
	rect.left = x; rect.right = x + rx - 1;				//Формируем параметры (координаты левого верхнего и правого нижнего углов)
	rect.top = y; rect.bottom = y + ry - 1;				//прямоугольной области с изображением в буфере кадра (на дисплее)

	if (!JD_USE_SCALE || jd->scale != 3) {	//Не для масштабирования 1/8
		if (jd->color_format == 0 || jd->color_format == 1) {
			for (iy = 0; iy < my; iy++) {
				pc = py = jd->mcubuf;
				if (my == 16) {	//Блок с удвоенной высотой?
					pc += 64 * 4 + (iy >> 1) * 8;
					if (iy >= 8) py += 64;
				}
				else {	//Блок с одинарной высотой
					pc += mx * 8 + iy * 8;
				}
				py += iy * 8;
				for (ix = 0; ix < mx; ix++) {
					cb = pc[0] - 128; 	//Получаем компоненты Cb и Cr and remove offset */
					cr = pc[64] - 128;
					if (mx == 16) {					//Блок с удвоенной шириной?
						if (ix == 8) py += 64 - 8;	/* Jump to next block if double block heigt */
						pc += ix & 1;				//Инкрементируем указатель цветности для каждого нечетного пикселя
					}
					else {						//Блок с одинарной шириной
						pc++;					//Инкрементируем указатель цветности
					}
					yy = *py++;			//Получаем компоненту интенсивности Y
					if (jd->color_format == 0) {
						Convert_YCbCr_to_R8G8B8(yy, cb, cr, &r1, &g1, &b1);
						*pix++ = r1;
						*pix++ = g1;
						*pix++ = b1;
					}
					else {
						*((uint16_t*)pix) = Convert_YCbCr_to_R5G6B5(yy, cb, cr);
						pix += 2;
					}
				}
			}
		}
		else { //Grayscale 8-bit, Grayscale 16-bit
			asm volatile (	"movs 	r3, #0						\n\t" // инициализация счетчика в r3(iy) = 0
							"1:									\n\t" // метка 1
							"add	r2, %[mcubuf], r3, lsl #3+1	\n\t" // инициализация адреса в r2(py) = mcubuf + r3(iy)*8*2
																  	  // (смещение по индексу 8, но в байтах это 16, т.к. массив из полуслов)
							"cmp	%[my], #16					\n\t" // сравнение my c 16
							"bne	2f							\n\t" // если  my != 16, то переход на метку 2
							"cmp	r3, #8						\n\t" // сравнение r3(iy) c 8
							"it 	cs							\n\t" // it-блок для условия беззнакового больше или равно (r3(iy) >= 8)
							"addcs	r2, #64*2					\n\t" // если больше или равно, то увеличиваем адрес в r2(py) на заданное смещение
																  	  // (смещение по индексу 64, но в байтах это 128, т.к. массив из полуслов)
							"2:									\n\t" // метка 2
							"movs	r1, #0						\n\t" // инициализация счетчика в r1(ix) = 0
							"3:									\n\t" // метка 3
							"cmp	%[mx], #16					\n\t" // сравнение mx c 16
							"bne	4f							\n\t" // если  mx != 16, то переход на метку 4
							"cmp	r1, #8						\n\t" // сравнение r1(ix) c 8
							"it 	eq							\n\t" // it-блок для условия равно (r1(ix) == 8)
							"addeq	r2, #56*2					\n\t" // если равно, то увеличиваем адрес в r2(py) на заданное смещение
																  	  // (смещение по индексу 56, но в байтах это 112, т.к. массив из полуслов)
							"4:									\n\t" // метка 4
							"ldrsh	r0, [r2], #1*2				\n\t" // считываем в r0 значение интенсивности, как полуслово со знаком,
																  	  // по адресу в r2 (py), с пост-инкрементом (после чтения) r2 на 2 (r0 = *py++)
							"cmp %[col_fmt], #2					\n\t"
							"bne 41f							\n\t"
																	  //Grayscale 8-bit, формат оттенки серого 8 бит
							"usat 	r0, #8, r0					\n\t" // беззнаковое насыщение (ограничение) значения в r0 до 8 разряда r0 = usat(8, r0)
																  	  // (после выполнения инструкции в r0 значение от 0 до 255)
							"strb	r0, [%[pix]], #1			\n\t" // записываем значение из r0, как байт, в память по адресу в pix,
																  	  // с пост-инкрементом (после записи) адреса в pix на 1 (*pix++ = r0)
							"b		42f							\n\t"
							"41:								\n\t" //Grayscale 16-bit, формат оттенки серого 16 бит
							"usat 	r0, #5, r0, asr #3			\n\t" // беззнаковое насыщение (ограничение) значения в r0 до 5 разряда с предварительным
																  	  // арифметическим сдвигом вправо на 3 разряда r0 = usat(5, r0 >> 3)
																  	  // (после выполнения инструкции в r0 значение от 0 до 31)
							"orr	r0, r0, r0, lsl #6			\n\t" // операция Логического ИЛИ значения в r0 со значением в r0 с предварительным
																  	  // логическим сдвигом на 6 разрядов влево r0 = r0 | (r0 << 6)
							"orr 	r0, r0, r0, lsl #11			\n\t" // операция Логического ИЛИ значения в r0 со значением в r0 с предварительным
				  	  	  	  	  	  	  	  	  	  	  	  	  	  // логическим сдвигом на 11 разрядов влево r0 = r0 | (r0 << 11)
							"strh	r0, [%[pix]], #1*2			\n\t" // записываем значение из r0, как полуслово, в память по адресу в pix,
																  	  // с пост-инкрементом (после записи) адреса в pix на 2 (*pix++ = r0)
							"42:								\n\t"
							"adds	r1, #1						\n\t" // инкремент счетчика r1(ix)
							"cmp	r1, %[mx]					\n\t" // сравнение r1 c mx
							"bcc	3b							\n\t" // если r1 < mx, то переход на метку 3
							"adds	r3, #1						\n\t" // инкремент счетчика r3(iy)
							"cmp	r3, %[my]					\n\t" // сравнение r3 c my
							"bcc	1b							\n\t" // если r3 < my, то переход на метку 1
							:[pix]"+r"(pix)
							:[mcubuf]"r"(jd->mcubuf), [my]"r"(my), [mx]"r"(mx), [col_fmt]"r"(jd->color_format)
							:"r0", "r1", "r2", "r3"	);
		}
		//Масштабирование, если это необходимо
		if (JD_USE_SCALE && jd->scale) {
			unsigned int x, y, r, g, b, s, w, a;
			uint8_t *op = (uint8_t*)workbuf;
			//Расчет средних значений составляющих цвета для каждого квадрата, соответствующего пикселю
			s = jd->scale * 2;	//Число значений для расчета среднего
			w = 1 << jd->scale;	//Размерность квадрата
			a = mx - w; //Количество байт, которое необходимо пропустить для перехода на следующую строку
			int k_byte = 1;
			if (jd->color_format == 0) { //RGB888 (24-bit/pix)
				k_byte = 3;
			}
			else if (jd->color_format == 1 || jd->color_format == 3) {
				k_byte = 2;
			}
			a *= k_byte;
			for (iy = 0; iy < my; iy += w) {
				for (ix = 0; ix < mx; ix += w) {
					pix = (uint8_t*)workbuf + (iy * mx + ix) * k_byte;
					r = g = b = 0;
					for (y = 0; y < w; y++) {	//Накопление значений цветовых составляющих в квадрате
						for (x = 0; x < w; x++) {
							if (jd->color_format == 0) { //RGB888 (24-bit/pix)
								r += *pix++;	//Накопление R
								g += *pix++;	//Накопление G
								b += *pix++;	//Накопление B
							}
							else if (jd->color_format == 1) { //RGB565 (16-bit/pix)
								r += ((*(uint16_t*)pix) >> 11) & 0x1f;		//Накопление R
								g += ((*(uint16_t*)pix) >> 6) & 0x1f;		//Накопление G
								b += (*(uint16_t*)pix) & 0x1f;//Накопление B
								pix += 2;
							}
							else if (jd->color_format == 2) { //Grayscale (8-bit/pix)
								r += *pix++;			//Накопление Y (Grayscale 8-bit) */
							}
							else { //Grayscale_16 (16-bit/pix)
								r += (*((uint16_t*)pix)) & 0x1f;	//Накопление Y (Grayscale 16-bit) */
								pix += 2;
							}
						}
						pix += a; //Переход на следующую строку
					}
					//Запись в массив усредненных значений цветовых составляющих
					if (jd->color_format == 0) { //RGB888 (24-bit/pix)
						*op++ = (uint8_t)(r >> s);	//компонента R
						*op++ = (uint8_t)(g >> s);	//компонента G
						*op++ = (uint8_t)(b >> s);	//компонента B
					}
					else if (jd->color_format == 1) { //RGB565 (16-bit/pix)
						*((uint16_t*)op) = ((r >> s) << 11) |	//компонента R
										   ((g >> s) << 6)  |	//компонента G
										   (b >> s);		  	//компонента B
						op += 2;
					}
					else if (jd->color_format == 2) { //Grayscale (8-bit/pix)
						*op++ = (uint8_t)(r >> s);	/* средняя интенсивность Y (оттенки серого) */
					}
					else { //Grayscale_16 (16-bit/pix)
						r >>= s; //средняя интенсивность Y (оттенки серого)
						*((uint16_t*)op) = (r << 11) | (r << 6) | r; //значение компонент R, G, B одинаково
						op += 2;
					}
				}
			}
		}
	}
	else {	//Только для коэффициента масштабирования 1/8
		pc = jd->mcubuf + mx * my;
		cb = pc[0] - 128;	//Получение значений компонент Cb/Cr
		cr = pc[64] - 128;
		for (iy = 0; iy < my; iy += 8) {
			py = jd->mcubuf;
			if (iy == 8) py += 64 * 2;
			for (ix = 0; ix < mx; ix += 8) {
				yy = *py;	//Получение значения компоненты Y (интенсивность)
				py += 64;
				if (jd->color_format == 0) { //RGB888 (24-bit/pix)
					Convert_YCbCr_to_R8G8B8(yy, cb, cr, &r1, &g1, &b1); //Преобразование YCbCr в R8G8B8
					*pix++ = r1;
					*pix++ = g1;
					*pix++ = b1;
				}
				else if (jd->color_format == 1) { //R5G6B5 (16-bit/pix)
					*((uint16_t*)pix) = Convert_YCbCr_to_R5G6B5(yy, cb, cr); //Преобразование YCbCr в R5G6B5
					pix += 2;
				}
				else if (jd->color_format == 2) { //Grayscale (8-bit/pix)
					*pix++ = yy;
				}
				else { //Grayscale_16 (16-bit/pix)
					*((uint16_t*)pix) = Convert_Y_to_Grayscale16(yy); //Преобразование Y в R5G6B5 (32 оттенка серого)
					pix += 2;
				}
			}
		}
	}
	//Обрезка блока, если требуется.
	//Отсекаем по ширине пиксели, которые не попадают в область вывода,
	//перестраивая массив с информацией о цвете.
	mx >>= jd->scale;
	if (rx < mx) {	// если rx < mx, то перестраиваем пиксели под область вывода
		uint8_t *s, *d;
		s = d = (uint8_t*)workbuf;
		if (jd->color_format == 1 || jd->color_format == 3) {	//R5G6B5 (16-bit/pix), Grayscale_16 (16-bit/pix)
			unsigned int rx1 = rx;
			rx *= 2; //2 байта на пиксель
			mx *= 2;
			for (y = 1; y < ry; y++) { //Перемещаем пиксели в массиве для новой области вывода, начиная со 2 строки
				memcpy_16(d += rx, s += mx, rx1);
			}
		}
		else { //R8G8B8, Crayscale 8-bit
			if (jd->color_format == 0) { //R8G8B8 - 3 байта на пиксель
				rx *= 3;
				mx *= 3;
			}
			for (y = 1; y < ry; y++) { //Перемещаем пиксели в массиве для новой области вывода, начиная со 2 строки
				memcpy_8(d += rx, s += mx, rx);
			}
		}
	}
	//Вывод блока на дисплей
	return outfunc(jd, workbuf, &rect);
}

#else //JD_FAST_STM32 = 0
/*-----------------------------------------------------------------------*/
/* Output an MCU: Convert YCrCb to RGB and output it in RGB form         */
/*-----------------------------------------------------------------------*/
static JRESULT mcu_output (
	JDEC* jd,			/* Pointer to the decompressor object */
	int (*outfunc)(JDEC*, void*, JRECT*),	/* RGB output function */
	unsigned int x,		/* MCU location in the image */
	unsigned int y		/* MCU location in the image */
)
{
#if (JD_FORMAT == 3)
#error JD_FORMAT = 3 is only supported when optimizing for stm32 is enabled (JD_FAST_STM32 = 1)
#endif
	//++++++++++++ Введена workbuf для двойного буфера ++++++++++++
	void *workbuf = jd->workbuf + jd->offset;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	const int CVACC = (sizeof (int) > 2) ? 1024 : 128;	/* Adaptive accuracy for both 16-/32-bit systems */
	unsigned int ix, iy, mx, my, rx, ry;
	int yy, cb, cr;
	jd_yuv_t *py, *pc;
	uint8_t *pix;
	JRECT rect;


	mx = jd->msx * 8; my = jd->msy * 8;					/* MCU size (pixel) */
	rx = (x + mx <= jd->width) ? mx : jd->width - x;	/* Output rectangular size (it may be clipped at right/bottom end of image) */
	ry = (y + my <= jd->height) ? my : jd->height - y;
	if (JD_USE_SCALE) {
		rx >>= jd->scale; ry >>= jd->scale;
		if (!rx || !ry) return JDR_OK;					/* Skip this MCU if all pixel is to be rounded off */
		x >>= jd->scale; y >>= jd->scale;
	}
	rect.left = x; rect.right = x + rx - 1;				/* Rectangular area in the frame buffer */
	rect.top = y; rect.bottom = y + ry - 1;


	if (!JD_USE_SCALE || jd->scale != 3) {	/* Not for 1/8 scaling */
		pix = (uint8_t*)workbuf;

		if (JD_FORMAT != 2) {	/* RGB output (build an RGB MCU from Y/C component) */
			for (iy = 0; iy < my; iy++) {
				pc = py = jd->mcubuf;
				if (my == 16) {		/* Double block height? */
					pc += 64 * 4 + (iy >> 1) * 8;
					if (iy >= 8) py += 64;
				} else {			/* Single block height */
					pc += mx * 8 + iy * 8;
				}
				py += iy * 8;
				for (ix = 0; ix < mx; ix++) {
					cb = pc[0] - 128; 	/* Get Cb/Cr component and remove offset */
					cr = pc[64] - 128;
					if (mx == 16) {					/* Double block width? */
						if (ix == 8) py += 64 - 8;	/* Jump to next block if double block heigt */
						pc += ix & 1;				/* Step forward chroma pointer every two pixels */
					} else {						/* Single block width */
						pc++;						/* Step forward chroma pointer every pixel */
					}
					yy = *py++;			/* Get Y component */
					*pix++ = /*R*/ BYTECLIP(yy + ((int)(1.402 * CVACC) * cr) / CVACC);
					*pix++ = /*G*/ BYTECLIP(yy - ((int)(0.344 * CVACC) * cb + (int)(0.714 * CVACC) * cr) / CVACC);
					*pix++ = /*B*/ BYTECLIP(yy + ((int)(1.772 * CVACC) * cb) / CVACC);
				}
			}
		} else {	/* Monochrome output (build a grayscale MCU from Y comopnent) */
			for (iy = 0; iy < my; iy++) {
				py = jd->mcubuf + iy * 8;
				if (my == 16) {		/* Double block height? */
					if (iy >= 8) py += 64;
				}
				for (ix = 0; ix < mx; ix++) {
					if (mx == 16) {					/* Double block width? */
						if (ix == 8) py += 64 - 8;	/* Jump to next block if double block height */
					}
					*pix++ = (uint8_t)*py++;			/* Get and store a Y value as grayscale */
				}
			}
		}

		/* Descale the MCU rectangular if needed */
		if (JD_USE_SCALE && jd->scale) {
			unsigned int x, y, r, g, b, s, w, a;
			uint8_t *op;

			/* Get averaged RGB value of each square correcponds to a pixel */
			s = jd->scale * 2;	/* Number of shifts for averaging */
			w = 1 << jd->scale;	/* Width of square */
			a = (mx - w) * (JD_FORMAT != 2 ? 3 : 1);	/* Bytes to skip for next line in the square */
			op = (uint8_t*)workbuf;
			for (iy = 0; iy < my; iy += w) {
				for (ix = 0; ix < mx; ix += w) {
					pix = (uint8_t*)workbuf + (iy * mx + ix) * (JD_FORMAT != 2 ? 3 : 1);
					r = g = b = 0;
					for (y = 0; y < w; y++) {	/* Accumulate RGB value in the square */
						for (x = 0; x < w; x++) {
							r += *pix++;	/* Accumulate R or Y (monochrome output) */
							if (JD_FORMAT != 2) {	/* RGB output? */
								g += *pix++;	/* Accumulate G */
								b += *pix++;	/* Accumulate B */
							}
						}
						pix += a;
					}							/* Put the averaged pixel value */
					*op++ = (uint8_t)(r >> s);	/* Put R or Y (monochrome output) */
					if (JD_FORMAT != 2) {	/* RGB output? */
						*op++ = (uint8_t)(g >> s);	/* Put G */
						*op++ = (uint8_t)(b >> s);	/* Put B */
					}
				}
			}
		}

	} else {	/* For only 1/8 scaling (left-top pixel in each block are the DC value of the block) */

		/* Build a 1/8 descaled RGB MCU from discrete comopnents */
		pix = (uint8_t*)workbuf;
		pc = jd->mcubuf + mx * my;
		cb = pc[0] - 128;		/* Get Cb/Cr component and restore right level */
		cr = pc[64] - 128;
		for (iy = 0; iy < my; iy += 8) {
			py = jd->mcubuf;
			if (iy == 8) py += 64 * 2;
			for (ix = 0; ix < mx; ix += 8) {
				yy = *py;	/* Get Y component */
				py += 64;
				if (JD_FORMAT != 2) {
					*pix++ = /*R*/ BYTECLIP(yy + ((int)(1.402 * CVACC) * cr / CVACC));
					*pix++ = /*G*/ BYTECLIP(yy - ((int)(0.344 * CVACC) * cb + (int)(0.714 * CVACC) * cr) / CVACC);
					*pix++ = /*B*/ BYTECLIP(yy + ((int)(1.772 * CVACC) * cb / CVACC));
				} else {
					*pix++ = yy;
				}
			}
		}
	}

	/* Squeeze up pixel table if a part of MCU is to be truncated */
	mx >>= jd->scale;
	if (rx < mx) {	/* Is the MCU spans rigit edge? */
		uint8_t *s, *d;
		unsigned int x, y;

		s = d = (uint8_t*)workbuf;
		for (y = 0; y < ry; y++) {
			for (x = 0; x < rx; x++) {	/* Copy effective pixels */
				*d++ = *s++;
				if (JD_FORMAT != 2) {
					*d++ = *s++;
					*d++ = *s++;
				}
			}
			s += (mx - rx) * (JD_FORMAT != 2 ? 3 : 1);	/* Skip truncated pixels */
		}
	}

	/* Convert RGB888 to RGB565 if needed */
	if (JD_FORMAT == 1) {
		uint8_t *s = (uint8_t*)workbuf;
		uint16_t w, *d = (uint16_t*)s;
		unsigned int n = rx * ry;

		do {
			w = (*s++ & 0xF8) << 8;		/* RRRRR----------- */
			w |= (*s++ & 0xFC) << 3;	/* -----GGGGGG----- */
			w |= *s++ >> 3;				/* -----------BBBBB */
			*d++ = w;
		} while (--n);
	}

	/* Output the rectangular */
	return outfunc(jd, workbuf, &rect);
}
#endif /* JD_FAST_STM32 */

/*-----------------------------------------------------------------------*/
/* Analyze the JPEG image and Initialize decompressor object             */
/*-----------------------------------------------------------------------*/

#define	LDB_WORD(ptr)		(uint16_t)(((uint16_t)*((uint8_t*)(ptr))<<8)|(uint16_t)*(uint8_t*)((ptr)+1))


JRESULT jd_prepare (
	JDEC* jd,				/* Blank decompressor object */
	size_t (*infunc)(JDEC*, uint8_t*, size_t),	/* JPEG strem input function */
	void* pool,				/* Working buffer for the decompression session */
	size_t sz_pool,			/* Size of working buffer */
	void* dev				/* I/O device identifier for the session */
)
{
	uint8_t *seg, b;
	uint16_t marker;
	unsigned int n, i, ofs;
	size_t len;
	JRESULT rc;

#if (JD_FAST_STM32 == 0)
	memset(jd, 0, sizeof (JDEC));	/* Clear decompression object (this might be a problem if machine's null pointer is not all bits zero) */
#else
	memset_8(jd, 0, sizeof (JDEC));
#endif
	/****************************** +++++++++++++++++++++++++++ ****************************/
	jd->color_format = AVI_color_mode; //записываем в переменную значение формата цвета
	/***************************************************************************************/
	jd->pool = pool;		/* Work memroy */
	jd->sz_pool = sz_pool;	/* Size of given work memory */
	jd->infunc = infunc;	/* Stream input function */
	jd->device = dev;		/* I/O device identifier */

	jd->inbuf = seg = alloc_pool(jd, JD_SZBUF);		/* Allocate stream input buffer */
	if (!seg) return JDR_MEM1;

	ofs = marker = 0;		/* Find SOI marker */
	do {
		if (jd->infunc(jd, seg, 1) != 1) return JDR_INP;	/* Err: SOI was not detected */
		ofs++;
		marker = marker << 8 | seg[0];
	} while (marker != 0xFFD8);

	for (;;) {				/* Parse JPEG segments */
		/* Get a JPEG marker */
		if (jd->infunc(jd, seg, 4) != 4) return JDR_INP;
		marker = LDB_WORD(seg);		/* Marker */
		len = LDB_WORD(seg + 2);	/* Length field */
		if (len <= 2 || (marker >> 8) != 0xFF) return JDR_FMT1;
		len -= 2;			/* Segent content size */
		ofs += 4 + len;		/* Number of bytes loaded */

		switch (marker & 0xFF) {
		case 0xC0:	/* SOF0 (baseline JPEG) */
			if (len > JD_SZBUF) return JDR_MEM2;
			if (jd->infunc(jd, seg, len) != len) return JDR_INP;	/* Load segment data */

			jd->width = LDB_WORD(&seg[3]);		/* Image width in unit of pixel */
			jd->height = LDB_WORD(&seg[1]);		/* Image height in unit of pixel */
			jd->ncomp = seg[5];					/* Number of color components */
			if (jd->ncomp != 3 && jd->ncomp != 1) return JDR_FMT3;	/* Err: Supports only Grayscale and Y/Cb/Cr */

			/* Check each image component */
			for (i = 0; i < jd->ncomp; i++) {
				b = seg[7 + 3 * i];							/* Get sampling factor */
				if (i == 0) {	/* Y component */
					if (b != 0x11 && b != 0x22 && b != 0x21) {	/* Check sampling factor */
						return JDR_FMT3;					/* Err: Supports only 4:4:4, 4:2:0 or 4:2:2 */
					}
					jd->msx = b >> 4; jd->msy = b & 15;		/* Size of MCU [blocks] */
				} else {		/* Cb/Cr component */
					if (b != 0x11) return JDR_FMT3;			/* Err: Sampling factor of Cb/Cr must be 1 */
				}
				jd->qtid[i] = seg[8 + 3 * i];				/* Get dequantizer table ID for this component */
				if (jd->qtid[i] > 3) return JDR_FMT3;		/* Err: Invalid ID */
			}
			break;

		case 0xDD:	/* DRI - Define Restart Interval */
			if (len > JD_SZBUF) return JDR_MEM2;
			if (jd->infunc(jd, seg, len) != len) return JDR_INP;	/* Load segment data */

			jd->nrst = LDB_WORD(seg);	/* Get restart interval (MCUs) */
			break;

		case 0xC4:	/* DHT - Define Huffman Tables */
			if (len > JD_SZBUF) return JDR_MEM2;
			if (jd->infunc(jd, seg, len) != len) return JDR_INP;	/* Load segment data */

			rc = create_huffman_tbl(jd, seg, len);	/* Create huffman tables */
			if (rc) return rc;
			break;

		case 0xDB:	/* DQT - Define Quaitizer Tables */
			if (len > JD_SZBUF) return JDR_MEM2;
			if (jd->infunc(jd, seg, len) != len) return JDR_INP;	/* Load segment data */

			rc = create_qt_tbl(jd, seg, len);	/* Create de-quantizer tables */
			if (rc) return rc;
			break;

		case 0xDA:	/* SOS - Start of Scan */
			if (len > JD_SZBUF) return JDR_MEM2;
			if (jd->infunc(jd, seg, len) != len) return JDR_INP;	/* Load segment data */

			if (!jd->width || !jd->height) return JDR_FMT1;	/* Err: Invalid image size */
			if (seg[0] != jd->ncomp) return JDR_FMT3;		/* Err: Wrong color components */

			/* Check if all tables corresponding to each components have been loaded */
			for (i = 0; i < jd->ncomp; i++) {
				b = seg[2 + 2 * i];	/* Get huffman table ID */
				if (b != 0x00 && b != 0x11)	return JDR_FMT3;	/* Err: Different table number for DC/AC element */
				n = i ? 1 : 0;							/* Component class */
				if (!jd->huffbits[n][0] || !jd->huffbits[n][1]) {	/* Check huffman table for this component */
					return JDR_FMT1;					/* Err: Nnot loaded */
				}
				if (!jd->qttbl[jd->qtid[i]]) {			/* Check dequantizer table for this component */
					return JDR_FMT1;					/* Err: Not loaded */
				}
			}

			/* Allocate working buffer for MCU and pixel output */
			n = jd->msy * jd->msx;						/* Number of Y blocks in the MCU */
			if (!n) return JDR_FMT1;					/* Err: SOF0 has not been loaded */
			len = n * 64 * 2 + 64;						/* Allocate buffer for IDCT and RGB output */
			if (len < 256) len = 256;					/* but at least 256 byte is required for IDCT */

			//++++++++++++ изменения для двойного буфера (DMA) +++++++++++++++++++++
			//jd->workbuf = alloc_pool(jd, len);		//оригинал
			jd->workbuf = alloc_pool(jd, 2 * len);		/* and it may occupy a part of following MCU working buffer for RGB output */
			jd->offset_value = len;
			jd->offset = 0;
			//----------------------------------------------------------------------

			if (!jd->workbuf) return JDR_MEM1;			/* Err: not enough memory */
			jd->mcubuf = alloc_pool(jd, (n + 2) * 64 * sizeof (jd_yuv_t));	/* Allocate MCU working buffer */
			if (!jd->mcubuf) return JDR_MEM1;			/* Err: not enough memory */

			/* Align stream read offset to JD_SZBUF */
			if (ofs %= JD_SZBUF) {
				jd->dctr = jd->infunc(jd, seg + ofs, (size_t)(JD_SZBUF - ofs));
			}
			jd->dptr = seg + ofs - (JD_FASTDECODE ? 0 : 1);

			return JDR_OK;		/* Initialization succeeded. Ready to decompress the JPEG image. */

		case 0xC1:	/* SOF1 */
		case 0xC2:	/* SOF2 */
		case 0xC3:	/* SOF3 */
		case 0xC5:	/* SOF5 */
		case 0xC6:	/* SOF6 */
		case 0xC7:	/* SOF7 */
		case 0xC9:	/* SOF9 */
		case 0xCA:	/* SOF10 */
		case 0xCB:	/* SOF11 */
		case 0xCD:	/* SOF13 */
		case 0xCE:	/* SOF14 */
		case 0xCF:	/* SOF15 */
		case 0xD9:	/* EOI */
			return JDR_FMT3;	/* Unsuppoted JPEG standard (may be progressive JPEG) */

		default:	/* Unknown segment (comment, exif or etc..) */
			/* Skip segment data (null pointer specifies to remove data from the stream) */
			if (jd->infunc(jd, 0, len) != len) return JDR_INP;
		}
	}
}




/*-----------------------------------------------------------------------*/
/* Start to decompress the JPEG picture                                  */
/*-----------------------------------------------------------------------*/

JRESULT jd_decomp (
	JDEC* jd,								/* Initialized decompression object */
	int (*outfunc)(JDEC*, void*, JRECT*),	/* RGB output function */
	uint8_t scale							/* Output de-scaling factor (0 to 3) */
)
{
	unsigned int x, y, mx, my;
	uint16_t rst, rsc;
	JRESULT rc;


	if (scale > (JD_USE_SCALE ? 3 : 0)) return JDR_PAR;
	jd->scale = scale;

	mx = jd->msx * 8; my = jd->msy * 8;			/* Size of the MCU (pixel) */

	jd->dcv[2] = jd->dcv[1] = jd->dcv[0] = 0;	/* Initialize DC values */
	rst = rsc = 0;

	rc = JDR_OK;
	for (y = 0; y < jd->height; y += my) {		/* Vertical loop of MCUs */
		for (x = 0; x < jd->width; x += mx) {	/* Horizontal loop of MCUs */
			if (jd->nrst && rst++ == jd->nrst) {	/* Process restart interval if enabled */
				rc = restart(jd, rsc++);
				if (rc != JDR_OK) return rc;
				rst = 1;
			}
			rc = mcu_load(jd);					/* Load an MCU (decompress huffman coded stream, dequantize and apply IDCT) */
			if (rc != JDR_OK) return rc;
			rc = mcu_output(jd, outfunc, x, y);	/* Output the MCU (YCbCr to RGB, scaling and output) */
			if (rc != JDR_OK) return rc;
			jd->offset = jd->offset > 0 ? 0 : jd->offset_value;
		}
	}
	return rc;
}
