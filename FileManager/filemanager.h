/* ------------------------------------------------------------------------------------/
 /     Модуль файлового менеджера, реализованный с применением библиотеки FATFS
 * ------------------------------------------------------------------------------------/
 *  Автор: VadRov
 *  Copyright (C) 2021, VadRov, all right reserved.
 *
 *  Допускается свободное распространение.
 *  При любом способе распространения указание автора ОБЯЗАТЕЛЬНО.
 *  В случае внесения изменений и распространения модификаций указание первоначального автора ОБЯЗАТЕЛЬНО.
 *  Распространяется по типу "как есть", то есть использование осуществляется на свой страх и риск.
 *  Автор не предоставляет никаких гарантий.
 *
 *		Основные возможности файлового менеджера:
 *  - Вывод менеджера в окне, параметры (координата верхнего левого угла, ширина и высота окна, шрифт, цвет)
 *    которого задает пользователь;
 *  - Сокращение имен файлов и каталогов (если они не помещаются в окне) по типу менеджера Total Commander;
 *  - Нет ограничений на количество файлов и подкаталогов в каталоге;
 *  - Запоминание позиции курсора в вышестоящем каталоге и восстановление этой позиции при возврате на него
 *    с вложенного подкаталога;
 *  - Вывод иконок (значков) файлов и каталогов;
 *  - Поддержка вложенных каталогов с уровнем вложенности, ограниченным размером оперативной памяти
 *    микроконтроллера.
 *
 *		Модуль базируется на библиотеках автора:
 *  - драйвер работы с SD-картой по SPI c использованием DMA (Авторы: ChaN (2014), kiwih (2017), VadRov (2019),
 *    каталог FATFS/Target)
 *    https://www.youtube.com/watch?v=z99bLhlnukM
 *  - драйвер для работы с дисплеями по SPI с использованием DMA (каталог Display)
 *    https://www.youtube.com/watch?v=8tIJ16riJqo
 *  - модуль для работы с кнопками с фильтром дребезга, автоповтором и буферризацией ввода (каталог Keyboard)
 *    https://www.youtube.com/watch?v=e-w5HS75neg
 *  - процедуры работы со строками (каталог MyString).
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 *
 */

#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

#include "display.h"

/* типы файлов */
typedef enum {
	FILE_TYPE_AUDIO = 0, /* файлы с аудио (музыкальные) */
	FILE_TYPE_PICTURE,   /* файлы с изображениями */
	FILE_TYPE_DATA,		 /* файлы с различными данными */
	FILE_TYPE_VIDEO,	 /* файлы с видео */
	FILE_TYPE_OTHER		 /* любые другие типы файлов */
} File_Manager_File_Types;

/* статусы завершения последнего сеанса файлового менеджера */
typedef enum {
	File_Manager_OK = 0,	/* завершено без ошибок */
	File_Manager_Error_Win, /* завершено с ошибкой из-за некорректных параметров окна */
	File_Manager_Error_Disk /* завершено с ошибкой из-за проблем с чтением диска */
} File_Manager_Status;

/* состояния флага активности менеджера */
typedef enum {
	File_Manager_Active = 0, /* вывод окна менеджера с возможностью выбора файлов */
	File_Manager_Passive,    /* только вывод окна менеджера, а выбор файлов недоступен */
} File_Manager_Active_Flag;

/* тип определяет цветовую схему файлового менеджера */
typedef struct {			 /* Цвета: */
	uint32_t text_color_dir, /* -текста наименования каталога; */
	text_color_file,		 /* -текста наименования файла; */
	text_color_cursor,  	 /* -текста выбранного файла/каталога; */
	bg_color, 				 /* -фона окружения; */
	bg_line1_color, 		 /* -фона четных строк; */
	bg_line2_color, 		 /* -фона нечетных строк; */
	cursor_color, 			 /* -курсора на текущем файле/каталоге; */
	slider_color, 			 /* -ползунка прокрутки; */
	stripe_color; 			 /* -полосы прокрутки. */
	uint8_t fl_cursor_fill;  /* Флаг закрашивания курсора:
	 	 	 	 	 	 	    0 - текст выбранного файла/каталога обводится прямоугольником цвета курсора (cursor_color)
	 	 	 	 	 	 	    >0 - фон текста выбранного файла/каталога приравнивается цвету курсора (cursor_color) */
} File_Manager_Color_Scheme;

struct File_Manager_Handler;

/* типы коллбэков обработчика менеджера */
typedef void (*filemanagersetdisplaycallback)(struct File_Manager_Handler *filemanager, LCD_Handler *lcd);
typedef void (*filemanagersetwincallback)(struct File_Manager_Handler *filemanager, int x, int y, int win_wdt, int win_hgt);
typedef void (*filemanagersetcolorcallback)(struct File_Manager_Handler *fm, File_Manager_Color_Scheme *color_scheme);
typedef void (*filemanagersetfontcallback)(struct File_Manager_Handler *filemanager, FontDef *font);
typedef void (*filemanagersetkeyscallback)(struct File_Manager_Handler *filemanager, uint8_t key_up, uint8_t key_down, uint8_t key_enter);
typedef void (*filemanagerchangeactiveflagcallback)(struct File_Manager_Handler *filemanager, File_Manager_Active_Flag fl);
typedef void (*filemanagershowcallback)(struct File_Manager_Handler *filemanager);
typedef File_Manager_Status (*filemanagergetdtatuscallback)(struct File_Manager_Handler *filemanager);
typedef char* (*filemanagergetfilepathcallback)(struct File_Manager_Handler *filemanager);
typedef char* (*filemanagergetfilenamecallback)(struct File_Manager_Handler *filemanager);

/* обработчик файлового менеджера */
typedef struct File_Manager_Handler {
	LCD_Handler *lcd; 	/* Указатель на обработчик дисплея */
	uint16_t x,			/* Параметры окна файлового менеджера: позиция левого верхнего угла окна по горизонтали */
		     y, 		/* позиция левого верхнего угла окна по вертикали */
	   win_wdt, 		/* ширина окна */
	   win_hgt;			/* высота окна */
	File_Manager_Color_Scheme *color_scheme; /* Указатель на цветовую схему для отображения файлового менеджера */
	FontDef *font; 		/* Указатель на шрифт */
	char *dir_name; 	/* Указатель на строку с текущем путем */
	char *file_name; 	/* Указатель на строку с именем выбранного файла */
	int *current_pos_buf, *pos_y_buf; 	/* Буферы для хранения позиций в родительских каталогах */
	int depth;							/* Текущая глубина в дереве каталогов */
	File_Manager_Status status; 		/* Статус последнего вызова файлового менеджера */
	uint8_t fl_last_selection; 			/* Флаг, использующийся для восстановления текущих позиций при повторном вызове менеджера */
	int current_pos, pos_y; 			/* Позиции в текущем каталоге */
	File_Manager_Active_Flag fl_no_use;	/* Флаг, определяющий включена или выключена возможность перемещения по каталогам и файлам.
	 	 	 	 	 	  	  	  	  	   Если флаг установлен, то выводится окно файлового менеджера с текущим списком файлов и каталогов.
	 	 	 	 	 	  	  	  	  	   При этом возможность перемещения по файлам и каталогам недоступна. Работа менеджера завершается.
	 	 	 	 	 	  	  	  	  	   Если флаг сброшен, то выводится окно и есть возможность перемещения по файлам и каталогам,
	 	 	 	 	 	  	  	  	  	   а работа менеджера завершается после выбора файла */
	uint8_t key_up, key_down, key_enter; /* Номера битов, определяющих кнопки управления: вверх, вниз, ввод */
	filemanagersetdisplaycallback SetDisplay; 	/* Назначает обработчик дисплея для файлового менеджера */
	filemanagersetwincallback SetWin; 			/* Устанавливает параметры окна (координаты, ширина, высота) для файлового менеджера */
	filemanagersetcolorcallback SetColor; 		/* Устанавливает цветовую схему для отображения файлового менеджера */
	filemanagersetfontcallback SetFont; 		/* Устанавливает шрифт для файлового менеджера */
	filemanagersetkeyscallback SetKeys; 		/* Устанавливает номера битов для управлящих клавиш: вверх, вниз, ввод */
	filemanagerchangeactiveflagcallback ChangeActiveFlag; /* Изменяет флаг активности fl_no_use менеджера */
	filemanagershowcallback Show; 				/* Запускает файловый менеджер в заданном окне, с заданными параметрами для выбора файла */
	filemanagergetdtatuscallback GetStatus; 	/* Возвращает статус завершения последнего вызова файлового менеджера */
	filemanagergetfilepathcallback GetFilePath; /* Возвращает указатель на строку с путем до выбранного файла */
	filemanagergetfilenamecallback GetFileName; /* Возвращает указатель на строку с именем выбранного файла */
} File_Manager_Handler;

/* Создает новый обработчик файлового менеджера и возвращает указатель на него */
File_Manager_Handler* FileManagerNew(void);

/* Удаляет обработчик файлового менеджера  */
void FileManagerDelete(File_Manager_Handler *fm);

/* Создает эффект "всплывающего" окна файлового менеджера */
void FileManagerPopupWin(File_Manager_Handler *fm, uint32_t p_millis);

#endif /* FILEMANAGER_H_ */
