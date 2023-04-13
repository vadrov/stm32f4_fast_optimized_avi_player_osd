/*
 *  Author: VadRov
 *  Copyright (C) 2022, VadRov, all right reserved.
 *
 *  Текстуры
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
#include "microgl2d.h"

const uint16_t image_data_avatar[1600] = { //текстура моего аватара
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xf7be, 0xe73c, 0xe71c, 0xe71c, 0xef7d, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xd6ba, 0x9cf3,
		    0x8410, 0x7bef, 0x8410, 0x8430, 0x8430, 0x7bef, 0x7bef, 0x8c71,
		    0xc618, 0xffdf, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xc618, 0x73ae, 0x7bef, 0xb5b6,
		    0xe71c, 0xffdf, 0xffff, 0xffff, 0xffff, 0xffff, 0xef7d, 0xc638,
		    0x8c71, 0x6b6d, 0xad75, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xef5d, 0x7bcf, 0x8410, 0xdefb, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffdf, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xef7d, 0x9492, 0x738e, 0xe71c, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xd69a, 0x630c, 0xc618, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffdf, 0xef5d,
		    0xffdf, 0xffff, 0xffff, 0xce79, 0x632c, 0xd69a, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xce79, 0x630c, 0xe73c, 0xffff, 0xffdf, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xdefb, 0xad55, 0xffff, 0x9cd3, 0x6b4d,
		    0x7bcf, 0xb596, 0xffff, 0xffff, 0xe73c, 0x632c, 0xdedb, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xdefb,
		    0x630c, 0xe73c, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xbdd7, 0x73ae, 0x7bef, 0x73ae, 0x632c, 0xffdf,
		    0xc638, 0x73ae, 0xffff, 0xd69a, 0xffff, 0xdedb, 0x6b6d, 0xf7be,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x6b6d,
		    0xce79, 0xffff, 0xffdf, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0x632c, 0xdedb, 0xffff, 0xc638, 0xef5d, 0xffff,
		    0xef7d, 0x73ae, 0x73ae, 0x5aeb, 0xbdf7, 0xffff, 0xa534, 0x94b2,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xa514, 0x9492,
		    0xffff, 0xffdf, 0xffff, 0xf7be, 0xef5d, 0xffff, 0xffff, 0xffff,
		    0xffdf, 0xffdf, 0x8c71, 0xa514, 0xffff, 0xc638, 0x8c51, 0x8430,
		    0xbdf7, 0xf7be, 0xe73c, 0xf79e, 0x632c, 0xef7d, 0xffff, 0x738e,
		    0xe73c, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xf79e, 0x6b4d, 0xf79e,
		    0xffff, 0xef7d, 0xe71c, 0x73ae, 0x5aeb, 0xad75, 0xc618, 0xffff,
		    0xbdd7, 0x630c, 0x7bcf, 0xf7be, 0x9cd3, 0x7bcf, 0xd6ba, 0xdedb,
		    0x7bef, 0x9492, 0xffff, 0xc638, 0x73ae, 0xef7d, 0xffff, 0xad55,
		    0xa514, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xad55, 0x9cd3, 0xffff,
		    0xe73c, 0x7bcf, 0x5aeb, 0xa514, 0xef7d, 0x738e, 0x73ae, 0x9cd3,
		    0x8c51, 0xd69a, 0xffff, 0xd6ba, 0x73ae, 0xffff, 0xffff, 0xffff,
		    0xffff, 0x738e, 0xe73c, 0x8430, 0xa514, 0xffff, 0xffff, 0xe73c,
		    0x7bef, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x7bcf, 0xdefb, 0xffff,
		    0xc638, 0x738e, 0xffff, 0xb596, 0x8430, 0xdedb, 0xd69a, 0x632c,
		    0x7bef, 0x9492, 0xffff, 0xb596, 0xa514, 0xffff, 0xffdf, 0xffdf,
		    0xffff, 0xa514, 0xb5b6, 0xef7d, 0x738e, 0x8c51, 0xffff, 0xffff,
		    0x7bef, 0xe73c, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xe71c, 0x7bcf, 0xffff, 0xffff,
		    0x9492, 0x8c51, 0xbdd7, 0x7bef, 0xd69a, 0x738e, 0xce59, 0x8c71,
		    0x8430, 0x9492, 0x94b2, 0xd6ba, 0x8410, 0xffff, 0xffff, 0xffff,
		    0xffff, 0x8430, 0xc638, 0xffff, 0xdedb, 0x7bef, 0xffff, 0xffff,
		    0x9492, 0xc638, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xbdd7, 0x94b2, 0xffff, 0xef5d,
		    0x52aa, 0xffff, 0x8c71, 0xb5b6, 0xffff, 0x8c51, 0xbdd7, 0xc618,
		    0x73ae, 0x9cd3, 0xa514, 0xffff, 0x73ae, 0xa514, 0xffff, 0xffff,
		    0xa514, 0x7bcf, 0xffff, 0x9cf3, 0x6b4d, 0xa514, 0xffff, 0xffff,
		    0xa534, 0xb596, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0x9cf3, 0xb5b6, 0xffff, 0xffff,
		    0xa514, 0x6b6d, 0xdedb, 0x73ae, 0x8410, 0x8c51, 0xe71c, 0x5acb,
		    0xc618, 0x630c, 0xffdf, 0xffff, 0xf7be, 0x9492, 0x73ae, 0x7bcf,
		    0x9492, 0xffff, 0xad55, 0x7bef, 0xce79, 0xf79e, 0xffff, 0xffff,
		    0xb596, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0x94b2, 0xbdf7, 0xffff, 0xffff,
		    0xef5d, 0x630c, 0xdefb, 0xce59, 0xdedb, 0xce59, 0xbdd7, 0x6b4d,
		    0xffdf, 0xb596, 0x8410, 0x7bef, 0x8c71, 0xffff, 0xffdf, 0xf7be,
		    0xef7d, 0xffff, 0xd69a, 0x73ae, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xb596, 0xa514, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xad75, 0x9cd3, 0xffff, 0xffdf,
		    0xffff, 0x9cf3, 0x6b4d, 0x630c, 0xbdd7, 0x630c, 0x8410, 0xb5b6,
		    0x8c51, 0x8410, 0x738e, 0xc638, 0x73ae, 0xd69a, 0xffff, 0x7bef,
		    0x5aeb, 0xad55, 0x8c51, 0x9492, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xad75, 0xad55, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xef7d, 0x738e, 0xffdf, 0xffff,
		    0xffff, 0xffdf, 0xffff, 0xce79, 0x7bcf, 0xce59, 0xbdf7, 0x5acb,
		    0x8c71, 0xad75, 0x6b6d, 0x7bef, 0x8c51, 0x7bef, 0x7bef, 0x8430,
		    0xffff, 0x7bef, 0xb5b6, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0x9cf3, 0xb5b6, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffdf, 0xffff, 0x7bef, 0xd69a, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xf7be, 0x52aa, 0xce59,
		    0xe71c, 0xbdd7, 0xffff, 0x630c, 0xa534, 0xe73c, 0xbdf7, 0xef7d,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0x8c51, 0xce59, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xd69a, 0x6b6d, 0xffdf, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x6b4d, 0xe71c,
		    0x7bef, 0x8430, 0x9cd3, 0xef5d, 0x528a, 0xef5d, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0x73ae, 0xef5d, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xef7d, 0x632c, 0xdedb, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xa514, 0x73ae, 0xf79e,
		    0x7bef, 0x7bef, 0xb596, 0xb5b6, 0x738e, 0xffdf, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xce79,
		    0x7bef, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0x738e, 0xbdd7, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x630c, 0xad55,
		    0xf79e, 0xce59, 0xef7d, 0x8410, 0x8c51, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x7bcf,
		    0xc638, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0x94b2, 0x94b2, 0xffff, 0xffdf, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xb5b6, 0x5aeb,
		    0x632c, 0xb596, 0x5acb, 0x738e, 0xdefb, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x7bcf,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xf7be, 0x632c, 0xf7be, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xdefb,
		    0xc638, 0x738e, 0xbdf7, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x73ae, 0xd69a,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0x9492, 0x8410, 0xbdd7, 0xe73c, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xd6ba, 0x7bef, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffdf, 0xc618, 0x9cf3, 0x630c, 0xef7d, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xa534, 0xad55, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xef7d, 0x7bcf, 0xffdf, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x9492, 0xc638, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0x9492, 0x9cd3, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x8c71, 0xce79, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xef5d, 0x5acb, 0xd6ba, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x9492, 0xc638, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xdedb, 0x632c, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x94b2, 0xc618, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x8c51, 0xad75, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x9cf3, 0xbdd7, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xbdf7, 0x9492, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xa534, 0xad75, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xb596, 0x9cf3, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xb596, 0xa514, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xef7d, 0x632c, 0xce79,
		    0xffff, 0xffff, 0xffdf, 0xef5d, 0xd69a, 0xb596, 0x8410, 0xbdf7,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc618, 0x94b2, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xdedb, 0x7bcf,
		    0x7bcf, 0x7bef, 0x7bcf, 0x7bef, 0x8430, 0xad75, 0x94b2, 0x8c51,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xce79, 0x8c51, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xf7be, 0xf79e, 0xf7be, 0xffff, 0xffff, 0xffff, 0xce79, 0x9492,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xdefb, 0x8410, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffdf, 0xffff, 0xd69a, 0x8c51,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xef5d, 0x7bcf, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xdedb, 0x8c51,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffdf, 0x7bcf, 0xf7be,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xdedb, 0x7bef,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x73ae, 0xe73c,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xef7d, 0xc618,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc618, 0xef7d,
		    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
};

const uint16_t image_data_volume[1024] = { //текстура значка громкости с альфа-каналом
	    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	    0x1888, 0x7888, 0xb888, 0xd888, 0xe777, 0xe777, 0xc666, 0x9555,
	    0x4222, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1888, 0xa888,
	    0xf888, 0xf888, 0xf888, 0xf888, 0xf888, 0xf888, 0xf888, 0xf888,
	    0xf777, 0xd555, 0x5222, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x4999, 0xe999, 0xf999,
	    0xf988, 0xf776, 0xf666, 0xf666, 0xf666, 0xf777, 0xf888, 0xf888,
	    0xf888, 0xf777, 0xf666, 0xb332, 0x1100, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x0000, 0x0000, 0x0000, 0x6999, 0xf999, 0xf888, 0xf777,
	    0xf999, 0xfccc, 0xfddd, 0xfddd, 0xfddd, 0xfbbb, 0xf888, 0xf666,
	    0xf877, 0xf888, 0xf777, 0xf777, 0xd333, 0x6444, 0x7555, 0x7555,
	    0x6555, 0x3555, 0x0666, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x0000, 0x0000, 0x4999, 0xf999, 0xf777, 0xf999, 0xfddd,
	    0xfddd, 0xfddd, 0xfddd, 0xfddd, 0xfddd, 0xfddd, 0xfddd, 0xfddd,
	    0xf988, 0xf666, 0xf888, 0xf777, 0xf777, 0xf333, 0xf777, 0xf666,
	    0xf666, 0xf777, 0xe666, 0x7555, 0x0555, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x0000, 0x1999, 0xea99, 0xf777, 0xfbbb, 0xfddd, 0xfddd,
	    0xfaaa, 0xf888, 0xf777, 0xf777, 0xf888, 0xfa99, 0xfddd, 0xfedd,
	    0xfddd, 0xfbbb, 0xf666, 0xf888, 0xf777, 0xf666, 0xf443, 0xf999,
	    0xf777, 0xf666, 0xf999, 0xf888, 0xb555, 0x0333, 0x0000, 0x0000,
	    0x0000, 0x0000, 0x9aaa, 0xf877, 0xfbbb, 0xfddd, 0xfccc, 0xf777,
	    0xf777, 0xf888, 0xf888, 0xf999, 0xf999, 0xf999, 0xf888, 0xfaaa,
	    0xfeee, 0xfedd, 0xfccc, 0xf666, 0xf877, 0xf777, 0xf555, 0xf666,
	    0xf999, 0xf888, 0xf666, 0xf999, 0xf999, 0xa555, 0x0000, 0x0000,
	    0x0000, 0x2999, 0xf999, 0xf999, 0xfedd, 0xfbbb, 0xf777, 0xf888,
	    0xf888, 0xf999, 0xf999, 0xfa99, 0xfaaa, 0xfaaa, 0xfaaa, 0xf999,
	    0xf999, 0xfedd, 0xfeee, 0xfbbb, 0xf666, 0xf777, 0xf777, 0xf333,
	    0xf999, 0xfa99, 0xf777, 0xf888, 0xfaaa, 0xf888, 0x5544, 0x0000,
	    0x0000, 0x9aaa, 0xf777, 0xfddd, 0xfccc, 0xf777, 0xf888, 0xf888,
	    0xf999, 0xf999, 0xfaaa, 0xfaaa, 0xfaaa, 0xfaaa, 0xfbaa, 0xfbba,
	    0xfaaa, 0xf999, 0xfeee, 0xfeee, 0xf888, 0xf777, 0xf777, 0xf555,
	    0xf555, 0xfaaa, 0xfa99, 0xf665, 0xfaaa, 0xfaaa, 0xd555, 0x0000,
	    0x0999, 0xe999, 0xf999, 0xfeee, 0xf888, 0xf877, 0xf888, 0xf999,
	    0xf999, 0xfaaa, 0xfaaa, 0xfaaa, 0xfbbb, 0xfbbb, 0xfbbb, 0xfbbb,
	    0xfbbb, 0xfaaa, 0xfaaa, 0xfeee, 0xfddd, 0xf666, 0xf777, 0xf777,
	    0xf333, 0xfaaa, 0xfaaa, 0xf777, 0xf888, 0xfaaa, 0xf888, 0x4444,
	    0x3aaa, 0xf777, 0xfddd, 0xfccc, 0xf777, 0xf888, 0xf998, 0xf999,
	    0xfaaa, 0xfaaa, 0xfbaa, 0xfbbb, 0xfbbb, 0xfbbb, 0xfbbb, 0xfbbb,
	    0xf888, 0xf655, 0xf555, 0xfddd, 0xfeee, 0xf888, 0xf777, 0xf777,
	    0xf444, 0xf877, 0xfaaa, 0xfaa9, 0xf666, 0xfaaa, 0xfaaa, 0x8444,
	    0x7aaa, 0xf887, 0xfeee, 0xf999, 0xf887, 0xf888, 0xf999, 0xfaaa,
	    0xfaaa, 0xfaaa, 0xfbbb, 0xfbbb, 0xfcbb, 0xfccc, 0xfccc, 0xf999,
	    0xf999, 0xf999, 0xf877, 0xf999, 0xfeee, 0xfccc, 0xf655, 0xf777,
	    0xf655, 0xf555, 0xfbaa, 0xfbbb, 0xf655, 0xfbbb, 0xfaaa, 0xb544,
	    0xaaaa, 0xf888, 0xfeee, 0xf777, 0xf888, 0xf998, 0xf999, 0xfaaa,
	    0xfaaa, 0xfbbb, 0xfbbb, 0xfccc, 0xfccc, 0xfccc, 0xfaaa, 0xf999,
	    0xfbbb, 0xfbbb, 0xfaaa, 0xf777, 0xfeee, 0xfeee, 0xf766, 0xf777,
	    0xf666, 0xf333, 0xfbbb, 0xfbbb, 0xf666, 0xf999, 0xf888, 0xd554,
	    0xc999, 0xf999, 0xfddd, 0xf777, 0xf888, 0xf999, 0xf999, 0xfaaa,
	    0xfaaa, 0xfbbb, 0xfbbb, 0xfccc, 0xfccc, 0xfddd, 0xf888, 0xf999,
	    0xfbbb, 0xfbbb, 0xfaaa, 0xf666, 0xfccb, 0xfeee, 0xf888, 0xf777,
	    0xf666, 0xf332, 0xf999, 0xf888, 0xf555, 0xf776, 0xf777, 0xd544,
	    0xd998, 0xfaaa, 0xfccc, 0xf777, 0xf888, 0xf999, 0xf999, 0xfaaa,
	    0xfaaa, 0xfaaa, 0xfbbb, 0xfbbb, 0xfccc, 0xfccc, 0xf666, 0xf877,
	    0xfaaa, 0xfaaa, 0xf888, 0xf555, 0xf999, 0xfeee, 0xfaaa, 0xf666,
	    0xf666, 0xf333, 0xf666, 0xf777, 0xf555, 0xf666, 0xf777, 0xc444,
	    0xd888, 0xfbaa, 0xfbbb, 0xf777, 0xf888, 0xf888, 0xf999, 0xf999,
	    0xf999, 0xfaaa, 0xfaaa, 0xfaab, 0xfbbb, 0xfddd, 0xf766, 0xf544,
	    0xf666, 0xf666, 0xf555, 0xf444, 0xf887, 0xfeee, 0xfbbb, 0xf666,
	    0xf666, 0xf433, 0xf555, 0xf666, 0xf544, 0xf665, 0xf666, 0x9333,
	    0xd988, 0xfaaa, 0xfccc, 0xf777, 0xf888, 0xf888, 0xf888, 0xf899,
	    0xf999, 0xf9aa, 0xfbbb, 0xfccc, 0xfddd, 0xfddd, 0xf999, 0xf443,
	    0xf444, 0xf444, 0xf443, 0xf433, 0xf766, 0xfeee, 0xfccc, 0xf555,
	    0xf666, 0xf433, 0xf544, 0xf666, 0xf444, 0xf655, 0xf544, 0x4333,
	    0xb999, 0xf999, 0xfccc, 0xf777, 0xf888, 0xf888, 0xf889, 0xf999,
	    0xfaaa, 0xfbbb, 0xfccc, 0xfccc, 0xfccc, 0xfccd, 0xfddd, 0xf554,
	    0xf433, 0xf433, 0xf333, 0xf333, 0xf666, 0xfeee, 0xfccc, 0xf555,
	    0xf666, 0xf433, 0xf444, 0xf555, 0xf333, 0xf555, 0xe333, 0x0330,
	    0x9999, 0xf888, 0xfeee, 0xf777, 0xf888, 0xf999, 0xfaaa, 0xfaaa,
	    0xfbbb, 0xfbbb, 0xfbbb, 0xfbbb, 0xfbbb, 0xfddd, 0xfeed, 0xfccc,
	    0xf555, 0xf333, 0xf333, 0xf332, 0xf666, 0xfeee, 0xfbbb, 0xf655,
	    0xf666, 0xf333, 0xf444, 0xf544, 0xf333, 0xf443, 0x6322, 0x0000,
	    0x6888, 0xf776, 0xfeee, 0xf888, 0xf888, 0xf999, 0xfa99, 0xfaaa,
	    0xfaaa, 0xfaaa, 0xfaaa, 0xfaaa, 0xfccc, 0xfddd, 0xfddd, 0xfddd,
	    0xfddd, 0xfaaa, 0xf988, 0xf999, 0xf999, 0xfeee, 0xf999, 0xf666,
	    0xf666, 0xf322, 0xf444, 0xf333, 0xf333, 0xb322, 0x0000, 0x0000,
	    0x3888, 0xf777, 0xfccc, 0xfaaa, 0xf888, 0xf888, 0xf999, 0xf999,
	    0xf999, 0xf999, 0xf999, 0xfbbb, 0xfccc, 0xfccc, 0xfccc, 0xfdcc,
	    0xfdcc, 0xfccc, 0xfccc, 0xfccb, 0xfaaa, 0xfeee, 0xf877, 0xf666,
	    0xf655, 0xf222, 0xf444, 0xf222, 0xd222, 0x0222, 0x0000, 0x0000,
	    0x0000, 0xe888, 0xf888, 0xfddd, 0xf777, 0xf888, 0xf889, 0xf999,
	    0xf999, 0xf889, 0xfaaa, 0xfbbb, 0xfbbb, 0xfccc, 0xfccc, 0xfccc,
	    0xfccc, 0xfccc, 0xfccc, 0xfa99, 0xfccc, 0xfeed, 0xf555, 0xf666,
	    0xf555, 0xf222, 0xf222, 0xc221, 0x1221, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x8888, 0xf666, 0xfddd, 0xf999, 0xf888, 0xf888, 0xf888,
	    0xf888, 0xf999, 0xfaaa, 0xfbbb, 0xfbbb, 0xfbbb, 0xfbbb, 0xfbbb,
	    0xfbbb, 0xfbbb, 0xfbbb, 0xf999, 0xfeee, 0xfbbb, 0xf555, 0xf655,
	    0xf433, 0xf221, 0x8221, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x2877, 0xf777, 0xf887, 0xfddd, 0xf777, 0xf888, 0xf888,
	    0xf888, 0xfaaa, 0xfaaa, 0xfaaa, 0xfbba, 0xfbbb, 0xfbbb, 0xfbbb,
	    0xfbbb, 0xfbbb, 0xf999, 0xfccc, 0xfedd, 0xf666, 0xf655, 0xf555,
	    0xf222, 0x5211, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x0000, 0x9777, 0xf666, 0xfbbb, 0xfccc, 0xf777, 0xf888,
	    0xf989, 0xf999, 0xfaaa, 0xfaaa, 0xfaaa, 0xfaaa, 0xfaaa, 0xfaaa,
	    0xfaaa, 0xfaaa, 0xfaaa, 0xfedd, 0xfbbb, 0xf554, 0xf555, 0xf444,
	    0x8111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x0000, 0x1777, 0xe777, 0xf655, 0xfccc, 0xfbbb, 0xf777,
	    0xf888, 0xf988, 0xf999, 0xf999, 0xfaaa, 0xfaaa, 0xfaaa, 0xfaaa,
	    0xf999, 0xf999, 0xfddd, 0xfddd, 0xf555, 0xf555, 0xf555, 0xf222,
	    0x1110, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x0000, 0x0000, 0x5766, 0xf777, 0xf555, 0xfcbb, 0xfccc,
	    0xf888, 0xf777, 0xf888, 0xf999, 0xf999, 0xf999, 0xf999, 0xf888,
	    0xfaaa, 0xfddd, 0xfddd, 0xf555, 0xf555, 0xf555, 0xf433, 0x6111,
	    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x0000, 0x0000, 0x0000, 0x8666, 0xf666, 0xf555, 0xf999,
	    0xfddd, 0xfbbb, 0xf999, 0xf888, 0xf888, 0xf988, 0xfaaa, 0xfddd,
	    0xfddd, 0xfbbb, 0xf555, 0xf555, 0xf555, 0xf444, 0xa211, 0x0000,
	    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x9666, 0xf666, 0xf655,
	    0xf655, 0xf999, 0xfddd, 0xfddd, 0xfddd, 0xfddd, 0xfddd, 0xfbbb,
	    0xf777, 0xf444, 0xf555, 0xf555, 0xf444, 0xb221, 0x0100, 0x0000,
	    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6665, 0xf666,
	    0xf666, 0xf555, 0xf444, 0xf555, 0xf666, 0xf655, 0xf444, 0xf444,
	    0xf555, 0xf555, 0xf544, 0xf443, 0x9221, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2555,
	    0xb555, 0xf555, 0xf555, 0xf555, 0xf555, 0xf555, 0xf555, 0xf555,
	    0xf544, 0xf444, 0xd333, 0x4211, 0x0000, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x2555, 0x7555, 0xb554, 0xd554, 0xe444, 0xe444, 0xc444,
	    0x9333, 0x4211, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
 };

const uint16_t image_data_x[1024] = { //текстура значка X с альфа-каналом
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0b33, 0x4c44, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x4c44, 0x0b33, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0b33, 0xcd55, 0xfe77, 0x6c44,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x6c44, 0xfe77, 0xcd55, 0x0b33, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0b33, 0xcd55, 0xff88, 0xff88, 0xfe77,
0x6c44, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6c44,
0xfe77, 0xff88, 0xff88, 0xcd55, 0x0b33, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0b33, 0xcd55, 0xff88, 0xff88, 0xff88, 0xff88,
0xfe77, 0x6c44, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6c44, 0xfe77,
0xff88, 0xff88, 0xff88, 0xff88, 0xcd55, 0x0b33, 0x0000, 0x0000,
0x0000, 0x0000, 0x4c44, 0xfe77, 0xff88, 0xff88, 0xff88, 0xff88,
0xff88, 0xfe77, 0x6c44, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6c44, 0xfe77, 0xff88,
0xff88, 0xff88, 0xff88, 0xff88, 0xfe77, 0x4c44, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x6c44, 0xfe77, 0xff88, 0xff88, 0xff88,
0xff88, 0xff88, 0xfe77, 0x6c44, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x6c44, 0xfe77, 0xff88, 0xff88,
0xff88, 0xff88, 0xff88, 0xfe77, 0x6c44, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x6c44, 0xfe77, 0xff88, 0xff88,
0xff88, 0xff88, 0xff88, 0xfe77, 0x6c44, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x6c44, 0xfe77, 0xff88, 0xff88, 0xff88,
0xff88, 0xff88, 0xfe77, 0x6c44, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6c44, 0xfe77, 0xff88,
0xff88, 0xff88, 0xff88, 0xff88, 0xfe77, 0x6c44, 0x0000, 0x0000,
0x0000, 0x0000, 0x6c44, 0xfe77, 0xff88, 0xff88, 0xff88, 0xff88,
0xff88, 0xfe77, 0x6c44, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6c44, 0xfe77,
0xff88, 0xff88, 0xff88, 0xff88, 0xff88, 0xfe77, 0x6c44, 0x0000,
0x0000, 0x6c44, 0xfe77, 0xff88, 0xff88, 0xff88, 0xff88, 0xff88,
0xfe77, 0x6c44, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6c44,
0xfe77, 0xff88, 0xff88, 0xff88, 0xff88, 0xff88, 0xfe77, 0x6c44,
0x6c44, 0xfe77, 0xff88, 0xff88, 0xff88, 0xff88, 0xff88, 0xfe77,
0x6c44, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x6c44, 0xfe77, 0xff88, 0xff88, 0xff88, 0xff88, 0xff88, 0xfe77,
0xfe77, 0xff88, 0xff88, 0xff88, 0xff88, 0xff88, 0xfe77, 0x6c44,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x6c44, 0xfe77, 0xff88, 0xff88, 0xff88, 0xff88, 0xff88,
0xff88, 0xff88, 0xff88, 0xff88, 0xff88, 0xfe77, 0x6c44, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x6c44, 0xfe77, 0xff88, 0xff88, 0xff88, 0xff88,
0xff88, 0xff88, 0xff88, 0xff88, 0xfe77, 0x6c44, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x6c44, 0xfe77, 0xff88, 0xff88, 0xff88,
0xff88, 0xff88, 0xff88, 0xfe77, 0x6c44, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x6c44, 0xfe77, 0xff88, 0xff88, 0xff88,
0xff88, 0xff88, 0xff88, 0xfe77, 0x6c44, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x6c44, 0xfe77, 0xff88, 0xff88, 0xff88, 0xff88,
0xff88, 0xff88, 0xff88, 0xff88, 0xfe77, 0x6c44, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x6c44, 0xfe77, 0xff88, 0xff88, 0xff88, 0xff88, 0xff88,
0xff88, 0xff88, 0xff88, 0xff88, 0xff88, 0xfe77, 0x6c44, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x6c44, 0xfe77, 0xff88, 0xff88, 0xff88, 0xff88, 0xff88, 0xfe77,
0xfe77, 0xff88, 0xff88, 0xff88, 0xff88, 0xff88, 0xfe77, 0x6c44,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6c44,
0xfe77, 0xff88, 0xff88, 0xff88, 0xff88, 0xff88, 0xfe77, 0x6c44,
0x6c44, 0xfe77, 0xff88, 0xff88, 0xff88, 0xff88, 0xff88, 0xfe77,
0x6c44, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6c44, 0xfe77,
0xff88, 0xff88, 0xff88, 0xff88, 0xff88, 0xfe77, 0x6c44, 0x0000,
0x0000, 0x6c44, 0xfe77, 0xff88, 0xff88, 0xff88, 0xff88, 0xff88,
0xfe77, 0x6c44, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6c44, 0xfe77, 0xff88,
0xff88, 0xff88, 0xff88, 0xff88, 0xfe77, 0x6c44, 0x0000, 0x0000,
0x0000, 0x0000, 0x6c44, 0xfe77, 0xff88, 0xff88, 0xff88, 0xff88,
0xff88, 0xfe77, 0x6c44, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x6c44, 0xfe77, 0xff88, 0xff88,
0xff88, 0xff88, 0xff88, 0xfe77, 0x6c44, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x6c44, 0xfe77, 0xff88, 0xff88, 0xff88,
0xff88, 0xff88, 0xfe77, 0x6c44, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x6c44, 0xfe77, 0xff88, 0xff88, 0xff88,
0xff88, 0xff88, 0xfe77, 0x6c44, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x6c44, 0xfe77, 0xff88, 0xff88,
0xff88, 0xff88, 0xff88, 0xfe77, 0x6c44, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x4c44, 0xfe77, 0xff88, 0xff88, 0xff88, 0xff88,
0xff88, 0xfe77, 0x6c44, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6c44, 0xfe77, 0xff88,
0xff88, 0xff88, 0xff88, 0xff88, 0xfe77, 0x4c44, 0x0000, 0x0000,
0x0000, 0x0000, 0x0b33, 0xcd55, 0xff88, 0xff88, 0xff88, 0xff88,
0xfe77, 0x6c44, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6c44, 0xfe77,
0xff88, 0xff88, 0xff88, 0xff88, 0xcd55, 0x0b33, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0b33, 0xcd55, 0xff88, 0xff88, 0xfe77,
0x6c44, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6c44,
0xfe77, 0xff88, 0xff88, 0xcd55, 0x0b33, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0b33, 0xcd55, 0xfe77, 0x6c44,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x6c44, 0xfe77, 0xcd55, 0x0b33, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0b33, 0x4c44, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x4c44, 0x0b33, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};



const MGL_IMAGE image_avatar  = { image_data_avatar, 40, 40, MGL_IMAGE_COLOR_R5G6B5 };
const MGL_IMAGE image_volume  = { image_data_volume, 32, 32, MGL_IMAGE_COLOR_A4R4G4B4 };
const MGL_IMAGE image_x  = 		{ image_data_x, 	 32, 32, MGL_IMAGE_COLOR_A4R4G4B4 };
