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
 *
 */

#ifndef INC_ASM_H_
#define INC_ASM_H_

#include "display.h"

void memset_word(void *, int, unsigned int);
void memset_halfword(void *, int, unsigned int);
void memset_byte(void *, int, unsigned int);

#endif /* INC_ASM_H_ */
