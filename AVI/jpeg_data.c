/*
 *  Author: VadRov
 *  Copyright (C) 2023, VadRov, all right reserved.
 *
 *  Фоновое изображение в формате jpeg 320х240
 *  для заставки avi плеера
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

#include "jpeg_chan.h"

const uint8_t fon_data[]={
			0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46,
			0x49, 0x46, 0x00, 0x01, 0x02, 0x00, 0x00, 0x60,
			0x00, 0x60, 0x00, 0x00, 0xFF, 0xFE, 0x00, 0x0F,
			0x4C, 0x61, 0x76, 0x63, 0x35, 0x39, 0x2E, 0x37,
			0x2E, 0x31, 0x30, 0x33, 0x00, 0xFF, 0xDB, 0x00,
			0x43, 0x00, 0x08, 0x14, 0x14, 0x17, 0x14, 0x17,
			0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x20, 0x1E,
			0x20, 0x21, 0x21, 0x21, 0x20, 0x20, 0x20, 0x20,
			0x21, 0x21, 0x21, 0x24, 0x24, 0x24, 0x2A, 0x2A,
			0x2A, 0x24, 0x24, 0x24, 0x21, 0x21, 0x24, 0x24,
			0x28, 0x28, 0x2A, 0x2A, 0x2E, 0x2F, 0x2E, 0x2B,
			0x2B, 0x2A, 0x2B, 0x2F, 0x2F, 0x32, 0x32, 0x32,
			0x3C, 0x3C, 0x39, 0x39, 0x46, 0x46, 0x48, 0x56,
			0x56, 0x67, 0xFF, 0xC4, 0x00, 0x7A, 0x00, 0x01,
			0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
			0x03, 0x05, 0x02, 0x01, 0x00, 0x06, 0x07, 0x01,
			0x00, 0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x02, 0x04, 0x03, 0x01, 0x00, 0x05, 0x10, 0x01,
			0x01, 0x00, 0x02, 0x01, 0x02, 0x04, 0x06, 0x03,
			0x00, 0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
			0x01, 0x02, 0x11, 0xF0, 0x21, 0x12, 0x31, 0x71,
			0x41, 0x03, 0x91, 0xD1, 0xF1, 0x61, 0x81, 0x51,
			0xE1, 0xC1, 0xA1, 0x22, 0x04, 0x13, 0xB1, 0x32,
			0x72, 0x11, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
			0x02, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x01, 0x11, 0x02, 0x12, 0x31,
			0x03, 0x21, 0x71, 0x51, 0x04, 0x22, 0xFF, 0xC0,
			0x00, 0x11, 0x08, 0x00, 0xF0, 0x01, 0x40, 0x03,
			0x01, 0x22, 0x00, 0x02, 0x11, 0x00, 0x03, 0x11,
			0x00, 0xFF, 0xDA, 0x00, 0x0C, 0x03, 0x01, 0x00,
			0x02, 0x11, 0x03, 0x11, 0x00, 0x3F, 0x00, 0xFE,
			0x28, 0xA4, 0x7C, 0xE8, 0xD1, 0x35, 0xA2, 0xD1,
			0x18, 0x42, 0x88, 0xD7, 0x7A, 0x52, 0x3D, 0x74,
			0x34, 0x9D, 0xC5, 0xE3, 0x85, 0xE0, 0x93, 0xAB,
			0xAB, 0x23, 0xED, 0x2B, 0x06, 0x5D, 0xDA, 0xF2,
			0x3C, 0x26, 0x41, 0xA1, 0x5E, 0xC8, 0xB4, 0x8F,
			0x64, 0x3B, 0xDB, 0xC7, 0xFC, 0x1A, 0x36, 0xA9,
			0x86, 0x04, 0xF3, 0xD1, 0x42, 0x70, 0xF6, 0xEE,
			0x77, 0xF5, 0x27, 0xAF, 0xF5, 0xE6, 0x22, 0xA2,
			0xE1, 0xED, 0xDF, 0x72, 0xEB, 0xE3, 0x6F, 0xFF,
			0x00, 0x1B, 0x7D, 0xB8, 0xE1, 0x35, 0x27, 0x9D,
			0xE9, 0xD5, 0x7E, 0x98, 0xCD, 0x4F, 0x4E, 0x75,
			0xA0, 0x65, 0x58, 0xC4, 0xF2, 0xCB, 0x7C, 0xA0,
			0x52, 0x2B, 0x9D, 0x08, 0x01, 0xE9, 0x3D, 0x1D,
			0xA7, 0xDD, 0x96, 0xFD, 0xBC, 0xF9, 0xB7, 0x09,
			0x97, 0x62, 0x3D, 0xB6, 0xB7, 0x7B, 0x31, 0x9F,
			0x7E, 0x7E, 0x87, 0xCA, 0xB1, 0x4D, 0x63, 0x5C,
			0x24, 0xF5, 0xE7, 0x98, 0xB4, 0xFA, 0x1D, 0x09,
			0x88, 0x05, 0x1A, 0x9B, 0x45, 0xA0, 0x33, 0x02,
			0xA3, 0xD3, 0x68, 0xB5, 0x35, 0xE0, 0x88, 0x94,
			0x80, 0x17, 0x19, 0x02, 0x51, 0x02, 0xC3, 0xA4,
			0x42, 0x49, 0xA8, 0x82, 0x6B, 0x26, 0x15, 0x11,
			0x70, 0xAB, 0x90, 0x8C, 0xD5, 0x5C, 0x45, 0x54,
			0x41, 0xE1, 0x31, 0xC3, 0xB8, 0xA2, 0x65, 0x47,
			0x7A, 0x73, 0x09, 0x1A, 0x0F, 0x09, 0x88, 0xC2,
			0xA7, 0x88, 0xD3, 0xA4, 0x11, 0x23, 0xCD, 0x75,
			0x22, 0x45, 0x0A, 0xEB, 0xD9, 0x0C, 0x91, 0xF4,
			0x85, 0x4C, 0x46, 0x56, 0xD7, 0x38, 0xE3, 0xBA,
			0xD7, 0x93, 0x5E, 0x8F, 0x31, 0xC7, 0x51, 0xA1,
			0xED, 0xFB, 0x7D, 0xFF, 0x00, 0xFF, 0x00, 0x33,
			0xD4, 0x45, 0xBE, 0x5C, 0x7B, 0x7E, 0xDF, 0x7F,
			0x97, 0xEF, 0xFA, 0x8D, 0x7F, 0xFF, 0x00, 0x33,
			0x53, 0x5D, 0x14, 0xB7, 0x53, 0x50, 0x1B, 0x42,
			0x2F, 0x84, 0xF2, 0xA3, 0x10, 0xEE, 0x63, 0x68,
			0x93, 0x17, 0x4E, 0xFB, 0x2D, 0xFB, 0x7C, 0x5A,
			0x3D, 0xB2, 0x7F, 0x2F, 0x98, 0xDC, 0x17, 0xB6,
			0x4F, 0xBF, 0x9F, 0xF5, 0x27, 0x47, 0x15, 0x5B,
			0x50, 0xAD, 0x60, 0xB9, 0x33, 0xF2, 0x68, 0x5E,
			0x78, 0x8B, 0x67, 0x3A, 0x35, 0xCC, 0xDB, 0x03,
			0xBC, 0xD3, 0x52, 0xCE, 0x75, 0x0E, 0xC0, 0x98,
			0x8C, 0xDA, 0x2D, 0x68, 0x51, 0x28, 0x0D, 0x40,
			0x68, 0xD4, 0xDA, 0x35, 0x09, 0x88, 0x1D, 0x1E,
			0x97, 0x47, 0x49, 0x78, 0x2D, 0x46, 0x95, 0x50,
			0x0A, 0xA3, 0x24, 0x42, 0x49, 0xAC, 0x82, 0x6B,
			0xA6, 0x01, 0xA0, 0x9A, 0xE9, 0x85, 0x43, 0x22,
			0xF0, 0x78, 0x44, 0x51, 0x05, 0x94, 0xD3, 0xC8,
			0xBC, 0x55, 0x17, 0xB8, 0x9B, 0x05, 0xD1, 0xB8,
			0x8D, 0x1A, 0xFA, 0x42, 0x71, 0x7D, 0xAE, 0xAB,
			0xE3, 0x14, 0x42, 0xD2, 0xB5, 0xD4, 0xB9, 0x1C,
			0xEB, 0xA9, 0xF8, 0xCE, 0x75, 0x19, 0x3B, 0x55,
			0xC7, 0x13, 0xF0, 0xC7, 0xD5, 0xF6, 0x38, 0xEF,
			0xFB, 0xF9, 0x35, 0xF0, 0xC3, 0xBA, 0xFE, 0xBE,
			0xE3, 0x2A, 0xE3, 0x0F, 0x6E, 0xE5, 0x7D, 0x64,
			0x9E, 0x37, 0xFA, 0x8D, 0x6B, 0xD2, 0x6A, 0x7A,
			0x29, 0xD2, 0x4D, 0x78, 0x7E, 0xBE, 0x74, 0x1B,
			0x40, 0x2F, 0x84, 0xAD, 0x73, 0x25, 0xFB, 0xAF,
			0x31, 0xD9, 0x3E, 0x1E, 0x13, 0x9F, 0xBE, 0xA2,
			0x0E, 0x23, 0x30, 0xFD, 0xF5, 0xFB, 0x7D, 0x56,
			0x7D, 0xCE, 0xAF, 0x02, 0x37, 0x09, 0xAB, 0xCF,
			0x17, 0x9A, 0x6B, 0x06, 0x47, 0x5C, 0xFE, 0x4D,
			0xD2, 0x5A, 0x68, 0x01, 0xB3, 0xED, 0xCF, 0x21,
			0xE9, 0xFA, 0xE7, 0x51, 0xEB, 0x40, 0xCC, 0xB0,
			0x3A, 0xD5, 0xA1, 0x58, 0x21, 0xC6, 0x5D, 0x81,
			0x59, 0xCE, 0xAD, 0x3A, 0x1D, 0x4C, 0xDC, 0x67,
			0x51, 0x69, 0xD4, 0x6B, 0x00, 0x66, 0x01, 0x47,
			0xA7, 0x58, 0x3D, 0x89, 0xAF, 0x03, 0xA8, 0x1B,
			0x62, 0x20, 0x58, 0x44, 0x8A, 0x48, 0x2A, 0x8A,
			0xE0, 0x94, 0x80, 0xA0, 0xC9, 0x10, 0x90, 0x15,
			0x2A, 0x29, 0xA4, 0x88, 0x8A, 0x23, 0x55, 0x86,
			0x44, 0x26, 0x89, 0x91, 0x42, 0xF5, 0x79, 0x15,
			0xC7, 0xA5, 0x75, 0x89, 0xBD, 0xBB, 0x50, 0xB5,
			0xAE, 0xBB, 0x77, 0x3C, 0x96, 0x93, 0xAA, 0xDE,
			0xDC, 0xF4, 0xA4, 0xCC, 0x35, 0x7C, 0xAA, 0x85,
			0x6D, 0x5B, 0x5F, 0xF2, 0x68, 0x61, 0x1C, 0x5C,
			0x7A, 0xDF, 0x36, 0xB7, 0xB7, 0x85, 0xDF, 0x3E,
			0x2A, 0x13, 0xB4, 0xAF, 0x6F, 0x0B, 0x7A, 0x37,
			0xA4, 0xED, 0x9C, 0xF8, 0xB9, 0xC7, 0x19, 0x84,
			0xD2, 0x9A, 0xFD, 0xA5, 0x6A, 0x92, 0x67, 0xEC,
			0x5B, 0xBA, 0xF3, 0x5A, 0xFB, 0x93, 0xFE, 0x20,
			0xE7, 0x39, 0x72, 0xF9, 0xD0, 0x82, 0xE5, 0xD3,
			0xA7, 0x6C, 0x13, 0x87, 0xCA, 0x3D, 0x63, 0x50,
			0xD2, 0x37, 0x9E, 0x25, 0xE9, 0x36, 0x84, 0x2B,
			0xCF, 0x98, 0xD5, 0xA1, 0x45, 0xBC, 0xD8, 0xD2,
			0x67, 0x58, 0x1D, 0x8D, 0x2A, 0x1D, 0x10, 0x59,
			0x76, 0x05, 0x79, 0xF4, 0x69, 0x50, 0xE8, 0x57,
			0x8C, 0xEA, 0x2D, 0x68, 0x58, 0x2D, 0x80, 0x35,
			0x00, 0xB0, 0x7A, 0x7D, 0x88, 0x58, 0x13, 0x12,
			0x81, 0xA4, 0xB4, 0x76, 0x92, 0xD0, 0x14, 0xD0,
			0x12, 0x36, 0xC4, 0x34, 0x05, 0x83, 0x44, 0xBA,
			0x85, 0x0A, 0xD0, 0x4A, 0x89, 0x35, 0x0A, 0x92,
			0xAB, 0xC2, 0x62, 0x10, 0x98, 0xA2, 0x74, 0x88,
			0x6E, 0x22, 0xC8, 0x76, 0x31, 0x58, 0x5A, 0x97,
			0x8C, 0x9E, 0x4D, 0x3C, 0x66, 0xBC, 0x7E, 0x21,
			0x62, 0xD8, 0xC1, 0x68, 0x43, 0xAA, 0x5C, 0xF6,
			0xF7, 0xD6, 0x78, 0xF3, 0xA3, 0x4B, 0xFF, 0x00,
			0x3D, 0xEA, 0xFE, 0xFF, 0x00, 0xA2, 0x7D, 0x9C,
			0x7F, 0x0F, 0xD8, 0x61, 0xFF, 0x00, 0x5B, 0xBB,
			0x1B, 0xAF, 0x5F, 0x4F, 0xBF, 0xEE, 0x79, 0x8A,
			0xDF, 0x24, 0xA4, 0xBD, 0xDF, 0xC3, 0xF2, 0x5F,
			0xF9, 0xFF, 0x00, 0xCA, 0xF9, 0xFF, 0x00, 0x6F,
			0xD2, 0x7B, 0x7E, 0xDF, 0x6C, 0xFB, 0xF3, 0xA3,
			0x53, 0x0F, 0xFA, 0xF7, 0x76, 0xD9, 0xEA, 0xBE,
			0x53, 0x48, 0x5E, 0xFF, 0x00, 0x86, 0xB9, 0xFA,
			0xAF, 0x3F, 0x9A, 0x0E, 0xB4, 0x85, 0xBC, 0xFA,
			0xA9, 0x68, 0xDB, 0xE7, 0x47, 0x46, 0x57, 0x2E,
			0x1D, 0x73, 0xD5, 0xF2, 0x88, 0xB9, 0x74, 0xFB,
			0xE2, 0xE9, 0xCD, 0x7D, 0xA5, 0x1F, 0x69, 0xD8,
			0x04, 0xF1, 0xE7, 0xC5, 0xD3, 0xE6, 0x0D, 0x27,
			0x0B, 0x69, 0xC0, 0xC0, 0x2D, 0x83, 0xD3, 0x34,
			0x3D, 0x1A, 0x54, 0x0B, 0x3C, 0xBE, 0x21, 0xD6,
			0x95, 0x0E, 0xF3, 0xEA, 0x34, 0x59, 0xB7, 0xF0,
			0x1D, 0x69, 0x51, 0x6B, 0x84, 0xCC, 0xB0, 0x7B,
			0x1A, 0x76, 0x0D, 0x60, 0x57, 0x95, 0x9B, 0x67,
			0x92, 0x36, 0x34, 0xF4, 0x85, 0x8C, 0x5E, 0x56,
			0x6E, 0x91, 0xB1, 0xA3, 0x62, 0x16, 0x05, 0x6D,
			0x67, 0x58, 0x35, 0x68, 0x51, 0x28, 0x16, 0x80,
			0x51, 0xA9, 0xB4, 0x4A, 0x91, 0x98, 0x2D, 0x1E,
			0x93, 0x46, 0xA9, 0x18, 0x84, 0x42, 0xA0, 0x90,
			0xC9, 0x14, 0x4E, 0x97, 0x1A, 0x18, 0x89, 0x8B,
			0x47, 0x15, 0x61, 0x3A, 0x6E, 0x3B, 0x6C, 0xE1,
			0xB6, 0x5E, 0x2D, 0x8C, 0x3A, 0xDF, 0x92, 0xF1,
			0xE6, 0xF6, 0xFD, 0x37, 0xB3, 0xF9, 0xFF, 0x00,
			0x1F, 0xD1, 0xFD, 0x9B, 0xFF, 0x00, 0x18, 0xFC,
			0x1F, 0xB1, 0xED, 0xFE, 0xF9, 0xFC, 0xBF, 0x63,
			0x8E, 0x5A, 0x2F, 0xF6, 0x2F, 0xFE, 0x7F, 0xF9,
			0xDA, 0xD6, 0xCF, 0x29, 0xA7, 0xE7, 0xB3, 0xD2,
			0xF9, 0x66, 0xCC, 0xB9, 0x23, 0xCF, 0x26, 0xBE,
			0xCE, 0xFD, 0x38, 0xD2, 0x5A, 0x55, 0xC9, 0x87,
			0x9C, 0x9E, 0xBC, 0xBE, 0x2F, 0xB4, 0xE9, 0xD0,
			0x9A, 0xE1, 0xD7, 0x3D, 0x3F, 0xA5, 0x1D, 0x73,
			0xEA, 0xC1, 0x63, 0x87, 0x4E, 0xB5, 0xE4, 0xF7,
			0x5E, 0x7C, 0xFC, 0x00, 0x4F, 0x1F, 0x3A, 0x7C,
			0xC6, 0xA7, 0xA7, 0x0B, 0x38, 0x68, 0x50, 0x1E,
			0x95, 0x62, 0x35, 0x54, 0x68, 0x36, 0x7E, 0x03,
			0xD3, 0x42, 0x8B, 0x54, 0x42, 0x81, 0x60, 0xB6,
			0x34, 0x2C, 0x40, 0x40, 0x03, 0xB5, 0x0B, 0x8F,
			0x39, 0xD1, 0xA1, 0xAF, 0x24, 0xB4, 0xC1, 0x33,
			0xFB, 0x79, 0xFC, 0xA1, 0x71, 0x69, 0x68, 0x7B,
			0x1C, 0xAE, 0xB3, 0x6C, 0x16, 0xC6, 0x95, 0x0B,
			0x20, 0x18, 0x8C, 0xDA, 0x15, 0x3F, 0x20, 0x68,
			0x0D, 0xC0, 0xA8, 0x94, 0xCA, 0x1D, 0x44, 0xD4,
			0x12, 0x8D, 0x4A, 0xA2, 0xD4, 0x69, 0x98, 0x54,
			0x37, 0x10, 0xA1, 0xD8, 0xA9, 0x12, 0xA7, 0x62,
			0xD1, 0xC4, 0x0C, 0x5B, 0x3E, 0xDE, 0x17, 0x3A,
			0x62, 0x11, 0xE8, 0xCC, 0x31, 0xB6, 0xF4, 0x7E,
			0xA7, 0xDA, 0xC2, 0x63, 0xF7, 0xBC, 0xE8, 0x16,
			0x12, 0x63, 0x35, 0x3F, 0x3F, 0xCB, 0xF4, 0x3E,
			0xD6, 0x3A, 0x9D, 0xD7, 0xD7, 0xC3, 0xF3, 0xEB,
			0xF9, 0x55, 0xE5, 0xDB, 0xEB, 0xF4, 0xD8, 0xF6,
			0xE7, 0x6C, 0xFE, 0x55, 0xEF, 0x0E, 0xE5, 0xA1,
			0x7B, 0x93, 0xC1, 0xFA, 0xC6, 0x87, 0x73, 0xD0,
			0x36, 0x54, 0xAD, 0xC6, 0x6A, 0xD2, 0xBA, 0xE7,
			0xD5, 0x27, 0xC1, 0x1A, 0x8E, 0x9C, 0xBA, 0x08,
			0xDD, 0x7F, 0x9E, 0x6E, 0x9C, 0xFF, 0x00, 0x8E,
			0xC2, 0x37, 0xAF, 0x5F, 0x3D, 0x08, 0x9E, 0xBC,
			0x7C, 0xF9, 0xCD, 0x79, 0xCF, 0x57, 0x2E, 0xDC,
			0x35, 0x89, 0xF3, 0x95, 0x1A, 0xBF, 0x3D, 0x52,
			0x12, 0x74, 0x4A, 0x2D, 0x3A, 0x8D, 0x56, 0x2F,
			0x42, 0xB1, 0x2B, 0x39, 0xD4, 0xAD, 0x25, 0xA1,
			0xA2, 0x2E, 0x92, 0xD7, 0x99, 0x7A, 0xE7, 0xC9,
			0x2E, 0x78, 0x35, 0xC2, 0xD9, 0xCE, 0xA2, 0x56,
			0x85, 0x0F, 0x26, 0x35, 0x9B, 0x93, 0x3F, 0x2E,
			0x75, 0x69, 0x65, 0xCE, 0x8C, 0xEC, 0x82, 0x6B,
			0x96, 0x75, 0x06, 0x9D, 0x90, 0x15, 0x33, 0xD0,
			0x3A, 0x25, 0x2A, 0x89, 0x51, 0x37, 0x06, 0xA2,
			0xD2, 0x68, 0xD5, 0x3A, 0x66, 0x19, 0x0E, 0xC4,
			0x08, 0xD6, 0xF6, 0xF1, 0xDF, 0x97, 0x3C, 0x07,
			0x0B, 0xF4, 0xD0, 0xF6, 0xB0, 0xEE, 0xBF, 0x6E,
			0x74, 0x8F, 0xD4, 0xE1, 0xD2, 0x6A, 0x32, 0x30,
			0xE8, 0xD1, 0xC7, 0x79, 0x59, 0x27, 0x5B, 0x4D,
			0x47, 0x93, 0xDF, 0xE5, 0xBD, 0xEC, 0xE1, 0xDF,
			0x7E, 0xD3, 0xC7, 0xFA, 0x8F, 0xD2, 0x03, 0x84,
			0xEC, 0xC6, 0x63, 0x3E, 0xBF, 0xBA, 0xA6, 0x59,
			0x08, 0x9B, 0x9C, 0xB2, 0x4A, 0x0F, 0x6B, 0xA8,
			0x34, 0x8D, 0x84, 0xCB, 0xCE, 0x8C, 0xFF, 0x00,
			0xF1, 0x78, 0x01, 0xB4, 0x37, 0xB7, 0xBC, 0xFA,
			0x8B, 0x2B, 0xB0, 0xAB, 0xA4, 0xAD, 0xB1, 0x65,
			0x5A, 0x01, 0x48, 0xBC, 0x76, 0x9A, 0x89, 0xAC,
			0xE9, 0xEF, 0x3D, 0x5F, 0x3D, 0x08, 0xDE, 0xBC,
			0x78, 0xF5, 0xCE, 0x78, 0xE5, 0xD3, 0xC7, 0x31,
			0x34, 0xBE, 0x2A, 0xA5, 0x46, 0x9A, 0x34, 0x6A,
			0x45, 0x19, 0x58, 0x85, 0x41, 0x25, 0x92, 0x1A,
			0x29, 0xF3, 0xD3, 0x91, 0xC5, 0x5B, 0x9C, 0xFD,
			0xA2, 0xD6, 0x0F, 0x79, 0xFC, 0x01, 0x91, 0xD5,
			0x9F, 0x95, 0x73, 0x99, 0xF9, 0x33, 0xB2, 0xE7,
			0xD4, 0xFC, 0xAB, 0x37, 0x20, 0x9D, 0xE4, 0x2C,
			0xB9, 0xE2, 0x05, 0x36, 0xD0, 0x6A, 0x54, 0xEC,
			0x12, 0x89, 0x4A, 0xA2, 0x54, 0x8D, 0x41, 0xE8,
			0xB5, 0x7A, 0x35, 0x46, 0x99, 0x8D, 0x3C, 0x26,
			0xDB, 0x78, 0xB2, 0x65, 0x36, 0x55, 0xE1, 0x4E,
			0x9A, 0xF2, 0xBF, 0x5D, 0xFF, 0x00, 0x5B, 0x0E,
			0xD9, 0xDF, 0x7C, 0x6F, 0x87, 0x97, 0xEF, 0xF2,
			0xFC, 0x8F, 0xB3, 0x8F, 0x7E, 0x5D, 0x7C, 0x27,
			0x8F, 0xC9, 0xFB, 0x3E, 0xE3, 0x11, 0xE5, 0x7D,
			0x9F, 0xC6, 0xC7, 0x70, 0xF9, 0x64, 0x3F, 0x77,
			0x41, 0xEE, 0x4A, 0x11, 0x23, 0x64, 0x40, 0x65,
			0x26, 0x39, 0x85, 0xF3, 0xD1, 0xD6, 0xD0, 0x75,
			0xCE, 0x48, 0xE7, 0x15, 0xB5, 0xE5, 0xDF, 0x20,
			0x2B, 0x4A, 0xC1, 0x19, 0x29, 0x30, 0x28, 0x44,
			0x02, 0xD0, 0xB8, 0xB0, 0xD1, 0x64, 0x97, 0x8B,
			0xBA, 0x49, 0xDE, 0xD3, 0x55, 0xD3, 0xD7, 0x2F,
			0x5C, 0xD7, 0x8E, 0x1D, 0x38, 0x73, 0x1C, 0xA5,
			0x5D, 0xA7, 0x55, 0x4D, 0x21, 0xEA, 0xD5, 0x1B,
			0xCF, 0x90, 0xD0, 0xA8, 0xF3, 0xC2, 0x20, 0xAD,
			0xE7, 0x3F, 0x49, 0x0D, 0x34, 0xD3, 0xBF, 0x15,
			0x07, 0xBC, 0xF1, 0x68, 0x11, 0xBC, 0xF1, 0x67,
			0xE5, 0xF9, 0xFC, 0x19, 0x93, 0x37, 0x2E, 0x57,
			0x0E, 0x05, 0x93, 0x3A, 0x99, 0x90, 0x19, 0x00,
			0xEF, 0x21, 0xD0, 0x69, 0x79, 0x05, 0x51, 0x39,
			0x06, 0xA2, 0xD2, 0x28, 0x95, 0x33, 0x50, 0x7A,
			0x2A, 0xD4, 0x6A, 0x89, 0x98, 0xD6, 0x94, 0xB8,
			0xCD, 0x8D, 0x9F, 0x66, 0x75, 0xDF, 0xEB, 0xC1,
			0x68, 0x5F, 0xA7, 0xEA, 0x7D, 0xA9, 0xFF, 0x00,
			0x9E, 0x3A, 0xF8, 0xB4, 0x31, 0xCB, 0xD5, 0x87,
			0xDC, 0x74, 0xBD, 0x0D, 0x3C, 0x6E, 0xA3, 0x5B,
			0xB9, 0xCE, 0xC1, 0xDB, 0xA9, 0x44, 0x8E, 0x34,
			0xF1, 0x3F, 0x16, 0x5E, 0x34, 0xF8, 0x24, 0x29,
			0x6F, 0x93, 0x74, 0xE0, 0x28, 0xA4, 0x1D, 0x47,
			0x34, 0xD9, 0x48, 0x9C, 0xE7, 0xA8, 0x50, 0x88,
			0x11, 0x1C, 0xB0, 0x73, 0x9B, 0x22, 0x26, 0xBC,
			0x21, 0x51, 0x94, 0x4D, 0x65, 0xB6, 0xE9, 0x17,
			0xA1, 0x1B, 0xB4, 0xDD, 0x38, 0x68, 0x5E, 0x27,
			0x5D, 0xA5, 0x46, 0x04, 0xE8, 0xD5, 0x6A, 0x3D,
			0x51, 0x1A, 0x95, 0xBC, 0xFA, 0xA2, 0xEE, 0xA3,
			0xB1, 0xA2, 0xE1, 0x1E, 0x69, 0xDD, 0x1E, 0xFE,
			0x79, 0xF6, 0x6B, 0x10, 0xCB, 0x9E, 0x0C, 0xDC,
			0x8C, 0xAC, 0xFC, 0x98, 0xB4, 0x0F, 0x26, 0x7D,
			0x2F, 0x20, 0x72, 0x4C, 0xF4, 0x13, 0x20, 0xA9,
			0x54, 0x2A, 0x89, 0xC8, 0x3D, 0x0E, 0x93, 0x43,
			0xA9, 0x9A, 0x88, 0x51, 0xAA, 0xD4, 0x6A, 0x81,
			0x98, 0xD0, 0xC7, 0xAD, 0xD3, 0xF4, 0x38, 0xDD,
			0x4D, 0x30, 0xFD, 0xBF, 0xD9, 0xFB, 0x5E, 0x13,
			0xE9, 0xB1, 0x8D, 0xEA, 0xD0, 0x95, 0x8D, 0x81,
			0xDB, 0x30, 0x4A, 0xC3, 0xF6, 0x4C, 0x66, 0x4A,
			0x6E, 0x22, 0x42, 0xC6, 0xB6, 0x34, 0xEC, 0x59,
			0x38, 0xD3, 0xF1, 0xAA, 0x13, 0xAD, 0x18, 0xEF,
			0xE0, 0x3C, 0x5D, 0xA8, 0x3A, 0x76, 0xE1, 0xF3,
			0x98, 0x41, 0x12, 0x85, 0xCE, 0x8B, 0x4E, 0x78,
			0xB0, 0x46, 0x4A, 0xBC, 0xA1, 0xED, 0x5D, 0xB0,
			0x5A, 0x7E, 0xFC, 0xDD, 0x6C, 0x2D, 0xAD, 0xB0,
			0x2B, 0xA5, 0x6D, 0xEE, 0xC6, 0xDB, 0xED, 0xF3,
			0xEA, 0xC1, 0x69, 0x2F, 0x77, 0xCE, 0x83, 0x6D,
			0xF3, 0x9D, 0xAB, 0x24, 0xE7, 0x68, 0xD6, 0x87,
			0x5F, 0x5E, 0x7D, 0x47, 0xB7, 0x9D, 0x1E, 0x8F,
			0x68, 0xD2, 0x7D, 0x52, 0xDB, 0xC7, 0x0D, 0x03,
			0x8A, 0x2D, 0x5A, 0xF3, 0xC0, 0x5C, 0x9A, 0xD1,
			0x72, 0x67, 0xE5, 0x4A, 0xCB, 0x9C, 0xF4, 0x67,
			0xE5, 0x40, 0x6B, 0x98, 0x3E, 0x40, 0x52, 0x28,
			0x54, 0x07, 0x60, 0xF4, 0x4A, 0xBD, 0xA2, 0x54,
			0x4D, 0x41, 0xE8, 0x74, 0x9A, 0x25, 0x46, 0x9A,
			0x83, 0xD1, 0xD6, 0xA3, 0xA4, 0x66, 0x36, 0x25,
			0x5A, 0x02, 0x5E, 0x0A, 0xC2, 0xD5, 0xB1, 0x17,
			0xD8, 0x3B, 0x75, 0xB5, 0xCA, 0xE3, 0x4E, 0x53,
			0xB1, 0xAC, 0xA9, 0x4E, 0xC6, 0xA8, 0x5E, 0xC6,
			0xB4, 0x3F, 0x1A, 0xC8, 0xC4, 0xFC, 0x6F, 0x3F,
			0x95, 0x08, 0xD8, 0xD5, 0x84, 0xCE, 0x7D, 0x59,
			0xD2, 0x98, 0x32, 0xB4, 0x87, 0xA9, 0xBD, 0x70,
			0x54, 0xE7, 0xAB, 0xBE, 0x73, 0xF6, 0x83, 0xD7,
			0x38, 0xDD, 0xF3, 0xAA, 0x9B, 0xF3, 0x06, 0x55,
			0x76, 0xE7, 0x19, 0x2B, 0xBD, 0x85, 0xBF, 0x2F,
			0xF5, 0xD6, 0xD8, 0xD3, 0x9E, 0xEC, 0x2D, 0xBD,
			0xDF, 0x3A, 0xB1, 0xBA, 0x6E, 0xDC, 0xF7, 0x07,
			0xB7, 0x3D, 0xCE, 0x76, 0x9B, 0xDD, 0xCF, 0x9A,
			0x57, 0x21, 0x3B, 0x93, 0xB9, 0x7D, 0xDA, 0xE5,
			0xED, 0x47, 0x63, 0xF7, 0x3C, 0xEE, 0x6B, 0x08,
			0x73, 0xCF, 0x04, 0x5C, 0xB9, 0xCF, 0xAD, 0x0F,
			0x2A, 0xB5, 0x02, 0xD6, 0x29, 0x10, 0xCA, 0x83,
			0x95, 0x5F, 0x2A, 0x0D, 0xA0, 0x39, 0x10, 0xA1,
			0x55, 0xED, 0x0E, 0xA6, 0x6E, 0x21, 0x45, 0xAB,
			0xD1, 0x2A, 0x46, 0x62, 0x14, 0x4A, 0x45, 0x16,
			0xA3, 0x4C, 0xC4, 0x2A, 0x0B, 0x54, 0x52, 0x31,
			0x0B, 0x68, 0xE2, 0xCD, 0xC7, 0xC4, 0xF5, 0x22,
			0x14, 0xAD, 0xA9, 0x28, 0x3B, 0x5E, 0x55, 0x51,
			0xC6, 0x9C, 0x3A, 0x56, 0x4C, 0x3A, 0x55, 0x0B,
			0x58, 0xD6, 0x94, 0xFC, 0x6B, 0x22, 0x53, 0xA5,
			0x58, 0x9D, 0x8D, 0x79, 0x4B, 0x95, 0x93, 0x29,
			0x72, 0x8C, 0xA5, 0x8D, 0x2D, 0xF3, 0xAB, 0xDE,
			0x7A, 0x09, 0xB7, 0x7B, 0x12, 0x24, 0xBE, 0xD8,
			0xFB, 0x7D, 0xBE, 0x75, 0x73, 0x08, 0xDB, 0xAD,
			0x89, 0xB7, 0x9B, 0x70, 0xB0, 0xFD, 0xF2, 0xBD,
			0xDF, 0x90, 0x3D, 0xCF, 0xBB, 0xB9, 0xD1, 0xC1,
			0xC3, 0xFB, 0x9E, 0x77, 0x01, 0xDC, 0xF3, 0xBB,
			0x9D, 0x58, 0xEC, 0x37, 0xB9, 0x3E, 0xF6, 0x7F,
			0x72, 0x57, 0x27, 0x0F, 0x1A, 0x3D, 0xC9, 0x77,
			0x33, 0xFB, 0x9C, 0xF7, 0x73, 0xA3, 0x07, 0xE5,
			0xA3, 0xDC, 0xF3, 0x6C, 0xFD, 0xBB, 0xEE, 0x73,
			0xB1, 0xA5, 0xB7, 0xBB, 0x07, 0xB9, 0xD7, 0x77,
			0x3E, 0x6D, 0x0E, 0x29, 0x95, 0x0A, 0xDE, 0x7C,
			0xDD, 0x5A, 0x1D, 0x0A, 0xB2, 0x27, 0x42, 0xB5,
			0x6B, 0x43, 0xB4, 0x07, 0x22, 0x36, 0x89, 0x55,
			0xA2, 0xD4, 0x8D, 0x44, 0xA8, 0xD5, 0x6A, 0x35,
			0x4C, 0xC4, 0x46, 0x8B, 0x57, 0xA3, 0xD4, 0x8C,
			0x44, 0x2A, 0x2A, 0xD4, 0x53, 0x5A, 0x1B, 0x81,
			0x28, 0x4E, 0x8F, 0x86, 0x8A, 0xDB, 0x5E, 0x04,
			0x44, 0x10, 0x29, 0xF2, 0x99, 0x2B, 0x32, 0x17,
			0x2A, 0xA5, 0xEC, 0x6A, 0xCA, 0x74, 0xAC, 0x79,
			0x4D, 0x95, 0x52, 0xB6, 0x35, 0xA6, 0x45, 0x4A,
			0xC9, 0x99, 0x11, 0xDC, 0xA1, 0x6B, 0x1A, 0x9D,
			0xCA, 0xED, 0x97, 0xB7, 0x7D, 0xDE, 0x42, 0x4B,
			0xCB, 0x4F, 0x6F, 0xB6, 0xCF, 0xEE, 0x75, 0xDC,
			0xE0, 0xF9, 0x37, 0x7C, 0xFA, 0x38, 0xD8, 0x9D,
			0xCE, 0x36, 0xE7, 0x61, 0xDD, 0xDC, 0xE8, 0xF3,
			0x60, 0x77, 0x3E, 0xEE, 0x70, 0xBC, 0x9D, 0xB4,
			0xFB, 0x82, 0xEE, 0x71, 0x6B, 0x05, 0xE4, 0xBE,
			0xE4, 0xBB, 0x84, 0xB5, 0x3D, 0xB0, 0x78, 0x67,
			0x73, 0x9E, 0xE0, 0xF6, 0xF3, 0x6C, 0x1E, 0x1F,
			0xB7, 0x7B, 0x67, 0xED, 0xDE, 0xDC, 0xCC, 0x69,
			0xF7, 0x3A, 0xDF, 0x3E, 0x6C, 0xFD, 0xBA, 0xD8,
			0x93, 0xC2, 0x6D, 0x1E, 0xD7, 0x1B, 0x42, 0xD6,
			0x0E, 0x47, 0x94, 0x3A, 0xA5, 0xA3, 0x54, 0xCC,
			0x48, 0x95, 0xA3, 0x55, 0x28, 0xF5, 0x33, 0x11,
			0x34, 0x2A, 0x95, 0x0A, 0x05, 0xE2, 0x54, 0x6A,
			0xB5, 0x1E, 0xA4, 0xBC, 0x46, 0xA4, 0xA2, 0x29,
			0xAD, 0x1A, 0x29, 0xBE, 0xA9, 0x0D, 0x35, 0xD6,
			0x19, 0x57, 0x30, 0xC8, 0x4C, 0xA0, 0xC5, 0xA5,
			0x51, 0x0A, 0xD0, 0x94, 0xB9, 0x59, 0x92, 0x93,
			0x2A, 0xA8, 0x58, 0xD3, 0x99, 0x2F, 0xB6, 0x5E,
			0xD5, 0xD8, 0xF5, 0x0C, 0x69, 0x6D, 0xDF, 0x73,
			0x3B, 0xB9, 0xEF, 0x70, 0xB4, 0x38, 0xD2, 0xEE,
			0x75, 0xDC, 0xCD, 0xEE, 0x7B, 0xDC, 0xD0, 0xF9,
			0x68, 0xF7, 0x39, 0xEE, 0xE7, 0xCC, 0x2E, 0xE7,
			0x3D, 0xCE, 0x76, 0x19, 0xB7, 0xDB, 0x0B, 0x6F,
			0x36, 0xC6, 0xE1, 0xDB, 0x4F, 0x63, 0x6D, 0xCE,
			0xDC, 0xEC, 0x23, 0x6E, 0x36, 0x86, 0xDC, 0x6C,
			0x23, 0xC2, 0x36, 0xFB, 0x62, 0xED, 0xF6, 0xDC,
			0x2C, 0x2B, 0x6E, 0xF6, 0x16, 0xDE, 0xED, 0xCE,
			0xC3, 0xF6, 0xEF, 0x60, 0x6D, 0xD6, 0xC4, 0x9E,
			0x19, 0xB4, 0xED, 0x1F, 0x6E, 0x36, 0x16, 0xE3,
			0xAB, 0x50, 0xDB, 0xED, 0xA0, 0x15, 0x71, 0xF0,
			0xF5, 0xD2, 0x21, 0x56, 0x38, 0xA8, 0x3B, 0xA8,
			0x54, 0x97, 0x8E, 0x28, 0xF5, 0x44, 0x00, 0xB4,
			0x70, 0x92, 0xA9, 0xD4, 0xD7, 0x8F, 0xFF, 0xD9
};

const iPicture_jpg fon_file = {.data = fon_data, .size = sizeof (fon_data)};
