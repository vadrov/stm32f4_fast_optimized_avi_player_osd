	.cpu cortex-m4
	.arch armv7e-m
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"display_offsets.c"
	.text
	.section	.rodata
	.align	2
	.type	STREAM_OFFSET_TAB, %object
	.size	STREAM_OFFSET_TAB, 8
STREAM_OFFSET_TAB:
	.ascii	"\020(@Xp\210\240\270"
	.align	2
	.type	OFFSET_TAB_CCMRx, %object
	.size	OFFSET_TAB_CCMRx, 7
OFFSET_TAB_CCMRx:
	.ascii	"\000\000\000\000\004\004\004"
	.align	2
	.type	SHIFT_TAB_OCxx, %object
	.size	SHIFT_TAB_OCxx, 7
SHIFT_TAB_OCxx:
	.ascii	"\000\000\010\000\000\000\010"
	.align	2
	.type	SHIFT_TAB_ICxx, %object
	.size	SHIFT_TAB_ICxx, 7
SHIFT_TAB_ICxx:
	.ascii	"\000\000\010\000\000\000\010"
	.align	2
	.type	SHIFT_TAB_CCxP, %object
	.size	SHIFT_TAB_CCxP, 7
SHIFT_TAB_CCxP:
	.ascii	"\000\002\004\006\010\012\014"
	.align	2
	.type	SHIFT_TAB_OISx, %object
	.size	SHIFT_TAB_OISx, 7
SHIFT_TAB_OISx:
	.ascii	"\000\001\002\003\004\005\006"
	.text
	.align	1
	.global	LCD_GenerateOffsets
	.syntax unified
	.thumb
	.thumb_func
	.type	LCD_GenerateOffsets, %function
LCD_GenerateOffsets:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	push	{r7}
	add	r7, sp, #0
	.syntax unified
@ 45 "../Display/display_offsets.c" 1
	
#define lcd_width #4 
@ 0 "" 2
@ 46 "../Display/display_offsets.c" 1
	
#define lcd_height #6 
@ 0 "" 2
@ 47 "../Display/display_offsets.c" 1
	
#define lcd_x_offs #10 
@ 0 "" 2
@ 48 "../Display/display_offsets.c" 1
	
#define lcd_y_offs #12 
@ 0 "" 2
@ 49 "../Display/display_offsets.c" 1
	
#define lcd_spi #36 
@ 0 "" 2
@ 50 "../Display/display_offsets.c" 1
	
#define lcd_dma_ctrl #40 
@ 0 "" 2
@ 51 "../Display/display_offsets.c" 1
	
#define lcd_dma_strm #44 
@ 0 "" 2
@ 52 "../Display/display_offsets.c" 1
	
#define lcd_dc_port #56 
@ 0 "" 2
@ 53 "../Display/display_offsets.c" 1
	
#define lcd_dc_pin #60 
@ 0 "" 2
@ 54 "../Display/display_offsets.c" 1
	
#define lcd_cs_port #64 
@ 0 "" 2
@ 55 "../Display/display_offsets.c" 1
	
#define lcd_cs_pin #68 
@ 0 "" 2
@ 56 "../Display/display_offsets.c" 1
	
#define lcd_sz_mem #96 
@ 0 "" 2
@ 57 "../Display/display_offsets.c" 1
	
#define lcd_cs_ctrl #101 
@ 0 "" 2
@ 58 "../Display/display_offsets.c" 1
	
#define lcd_dc_ctrl #102 
@ 0 "" 2
@ 59 "../Display/display_offsets.c" 1
	
#define lcd_fill_clr #104 
@ 0 "" 2
	.thumb
	.syntax unified
	nop
	mov	sp, r7
	@ sp needed
	pop	{r7}
	bx	lr
	.size	LCD_GenerateOffsets, .-LCD_GenerateOffsets
	.ident	"GCC: (GNU Tools for STM32 10.3-2021.10.20211105-1100) 10.3.1 20210824 (release)"
