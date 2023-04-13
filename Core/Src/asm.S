/*
 *  Author: VadRov
 *  Copyright (C) 2022, VadRov, all right reserved.
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
 */
.syntax unified
.cpu cortex-m4
.thumb

		.section  .text.memset_word
		.weak  memset_word
		.type  memset_word, %function
memset_word:
		//r0 = mem, r1 = value, r2 = size
		cbz		r2, memset_word2
memset_word1:
		str		r1, [r0], #4
		subs	r2, #1
		bne		memset_word1
memset_word2:
		bx		lr
.size  memset_word, .-memset_word

		.section  .text.memset_halfword
		.weak  memset_halfword
		.type  memset_halfword, %function
memset_halfword:
		//r0 = mem, r1 = value, r2 = size
		cbz		r2, memset_halfword2
memset_halfword1:
		strh	r1, [r0], #2
		subs	r2, #1
		bne		memset_halfword1
memset_halfword2:
		bx		lr
.size  memset_halfword, .-memset_halfword

		.section  .text.memset_byte
		.weak  memset_byte
		.type  memset_byte, %function
memset_byte:
		//r0 = mem, r1 = value, r2 = size
		cbz		r2, memset_byte2
memset_byte1:
		strb	r1, [r0], #1
		subs	r2, #1
		bne		memset_byte1
memset_byte2:
		bx		lr
.size  memset_byte, .-memset_byte
