/*
 *  Copyright (C) 2019, VadRov, all right reserved.
 *
 *  Процедуры работы со строками
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

#ifndef MYSTRING_H_
#define MYSTRING_H_

#include "main.h"

//заполняет строку заданным значением (0...255)
void my_strset(char *str, uint8_t code, uint32_t len);

//возвращает длину строки
uint32_t my_strlen(const char *str);

//копирует строку 2 в строку 1
void my_strcpy(char *str1, const char *str2);

//копирует строку 2 в строку 1, перенося максимум len символов, но не более длины строки str2
void my_strcpy_fixed_sym(char *str1, const char *str2, int len);

//выделяет память под строку и копирует строку в эту область памяти
//возвращает указатель на область памяти, в которую скопирована строка
char* my_strcpy_mem(const char *str);

//присоединяет строку 2 к строке 1
void my_strcat(char *str1, const char *str2);

//присоединяет строку 2 к строке 1, перенося максимум len символов, но не более длины строки str2
void my_strcat_fixed_sym(char *str1, const char *str2, int len);

//поиск строки str в буфере buffer
//размер буфера size
char *memstr(char *buffer, char *str, uint32_t size);

//сшивает несколько строк в одну строку с разделителем
//n - число строк, separator - строка-разделитель, дальше строки количеством n
char* my_strsplice (int n, const char *separator, ...);

#endif /* MYSTRING_H_ */
