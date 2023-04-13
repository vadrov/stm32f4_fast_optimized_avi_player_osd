/*
 *  Author: VadRov
 *  Copyright (C) 2022, VadRov, all right reserved.
 *
 *  Графическая библиотека для работы с объектами-примитивами в двумерном пространстве microGL2D
 *  Версия: mini (урезанная)
 *
 *  Допускается свободное распространение.
 *  При любом способе распространения указание автора ОБЯЗАТЕЛЬНО.
 *  В случае внесения изменений и распространения модификаций указание первоначального автора ОБЯЗАТЕЛЬНО.
 *  Распространяется по типу "как есть", то есть использование осуществляется на свой страх и риск.
 *  Автор не предоставляет никаких гарантий.
 *
 *  Поддерживаются такие простые графические объекты-примитивы, как:
 *  - треугольник;
 *  - прямоугольник;
 *  - окружность;
 *  - текст.
 *  Доступна заливка примитивов однотонным цветом, градиентом, текстурой.
 *  Свойства объектов-примитивов, градиента и текстуры задаются и могут изменяться в любой момент.
 *  Текстуры могут иметь различные форматы (см. описание типов). Доступна прозрачность (от 0 (непрозрачен) до 255 (абсолютно прозрачен))
 *  объектов-примитивов.
 *  Для текстур, например, доступны такие свойства, как:
 *  - поворот на произвольный угол;
 *  - повтор по заданным осям координат;
 *  - переворот (зеркальное отображение) относительно заданных осей координат.
 *  Градиент может быть линейным (с произволым углом deg наклона прямой, проходящей через центр объекта) и радиальным.
 *  Для градиента задаются ключевые точки. Смещение ключевой точки от начала отсчета задается в % (от 0% до 100%), а
 *  цвет в ключевой точке задается в формате R8G8B8, например, 0xFFF8F2, где R = 0xFF, G = 0xF8, B = 0xF2.
 *
 *  https://www.youtube.com/c/VadRov
 *  https://zen.yandex.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 *
 */

#ifndef MICROGL2D_H_
#define MICROGL2D_H_

#include "fonts.h"

//Типы объектов-примитивов
typedef enum {
	MGL_OBJ_TYPE_TRIANGLE,		//треугольник
	MGL_OBJ_TYPE_FILLTRIANGLE,	//закрашенный треугольник
	MGL_OBJ_TYPE_RECTANGLE,	    //прямоугольник
	MGL_OBJ_TYPE_FILLRECTANGLE,	//закрашенный прямоугольник
	MGL_OBJ_TYPE_CIRCLE,		//окружность
	MGL_OBJ_TYPE_FILLCIRCLE,	//закрашенная окружность
	MGL_OBJ_TYPE_TEXT,			//текст
} MGL_OBJ_TYPES;

//Типы градиента
typedef enum {
	MGL_GRADIENT_LINEAR = 0, //линейный градиент
	MGL_GRADIENT_RADIAL,	 //радиальный градиент
} MGL_GRADIENT_TYPES;

//Ключевая точка градиента
typedef struct {
	uint8_t offset;			//смещение точки от начала отсчета, в процентах по отношению к линейному размеру: 0 - 100 %
	uint32_t color;			//цвет в ключевой точке
	uint8_t flag_mix;		//флаг, определяюший смешивание цветов при переходе от одной ключевой точки
							//к другой: !=0 - смешивать, 0 - не смешивать.
	void *next;				//указатель на следующую ключевую точку (так формируется список ключевых точек градиента)
} MGL_GRADIENT_POINT;

//Данные градиента
typedef struct {
	MGL_GRADIENT_TYPES g_type;		 //тип градиента
	int deg;						 //угол
	MGL_GRADIENT_POINT *points_list; //список с ключевыми точками градиента (список смен цвета)
} MGL_GRADIENT;

//Режим цвета изображения
typedef enum {
	MGL_IMAGE_COLOR_R3G3B2 = 0,	//8 бит без прозрачности: по 3 бита на составляющие r, g и 2 бита на составляющую b (256 цветов)
	MGL_IMAGE_COLOR_R5G6B5,		//16 бит без прозрачности: по 5 бит на составляющие r, b и 6 бит на составляющую g (65536 цветов)
	MGL_IMAGE_COLOR_A4R4G4B4,	//16 бит с прозрачностью: по 4 бита на составляющую цвета и 4 бита на прозрачность (4096 цветов)
	MGL_IMAGE_COLOR_R8G8B8,		//24 бита без прозрачности: по 8 бит на составляющую цвета (16777216 цветов)
	MGL_IMAGE_COLOR_A8R8G8B8,	//32 бита с прозрачностью: по 8 бит на составляющую цвета и 8 бит на прозрачность (16777216 цветов)
} MGL_IMAGE_COLOR_MODES;

//Данные изображения
typedef struct {
	const void *data;			//указатель на массив с информацией о цвете
	int w, h;					//ширина и высота изображения
	MGL_IMAGE_COLOR_MODES mode; //тип цвета
} MGL_IMAGE;

//Свойства текстуры
#define	MGL_TEXTURE_REPEAT_X	1	//повторять по оси x (изображение повторяется по оси х при ширине текстуры меньшей ширины объекта)
#define	MGL_TEXTURE_REPEAT_Y	2	//повторять по оси y (изображение повторяется по оси y при высоте текстуры меньшей высоты объекта)
#define	MGL_TEXTURE_FLIP_X		4	//зеркально отобразить по оси x
#define	MGL_TEXTURE_FLIP_Y		8	//зеркально отобразить по оси y

//Данные текстуры
typedef struct {
	MGL_IMAGE *image;	//указатель на данные изображения
	int alpha;			//угол поворота текстуры, градусы
	uint8_t features;	//свойства текстуры
} MGL_TEXTURE;

//Обработчик графического объекта
typedef struct {
	MGL_OBJ_TYPES obj_type; //тип объекта
	void *object;			//указатель на объект с данными
	uint8_t transparency;   //прозрачность объекта (0 - абсолютно непрозрачен, ..., 255 - невидим)
	uint8_t plane;			//положение объекта - план: спереди, сзади
							//(0 - на переднем плане - над всеми, ..., 255 - задний план - под всеми)
	uint8_t visible;		//видимость: видим (!=0) или невидим (0)
	MGL_GRADIENT *gradient;	//градиент (определяет цвета и направление градиента)
	MGL_TEXTURE *texture;  	//текстура (определяет данные изображения текстуры и ее свойства)
	char *name;				//имя объекта
	void *parent;			//указатель на родительский объект
	void *next;				//указатель на следующий объект
	void *prev;				//указатель на предыдущий объект
} MGL_OBJ;

//Данные треугольника
typedef struct {
	int x1, y1, x2, y2, x3, y3;	//координаты вершин
	uint32_t color;				//цвет
} MGL_OBJ_TRIANGLE;

//Данные прямоугольника
typedef struct {
	int x1, y1, x2, y2;	//координаты вершин (левой верхней и правой нижней)
	uint32_t color;		//цвет
} MGL_OBJ_RECTANGLE;

//Данные окружности/круга
typedef struct {
	int x, y, r;	//координаты центра и радиус
	uint32_t color;	//цвет
} MGL_OBJ_CIRCLE;

//Данные текста
typedef struct {
	int x, y;		//координаты стартовой позиции печати, относительно левой верхней вершины блока
	char *txt;		//указатель на строку текста
	FontDef *font;	//указатель на шрифт
	uint8_t bold;	//флаг толщины символов (0 - обычные, !=0 - утолщенные)
	uint32_t color;	//цвет
} MGL_OBJ_TEXT;

//Добавляет объект
MGL_OBJ* MGL_ObjectAdd(MGL_OBJ *obj, MGL_OBJ_TYPES type);
//Удаляет объект
MGL_OBJ* MGL_ObjectDelete(MGL_OBJ *obj);
//Удаляет все объекты в списке (взаимосвязанные объекты, ссылающиеся друг на друга)
void MGL_ObjectsListDelete(MGL_OBJ *obj);
//Устанавливает параметры треугольника
void MGL_SetTriangle(MGL_OBJ *obj, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);
//Устанавливает параметры прямоугольника
void MGL_SetRectangle(MGL_OBJ *obj, int x1, int y1, int x2, int y2, uint32_t color);
//Устанавливает параметры окружности
void MGL_SetCircle(MGL_OBJ *obj, int x, int y, int r, uint32_t color);
//Устанавливает параметры текста
void MGL_SetText(MGL_OBJ *obj, int x, int y, char *txt, FontDef *font, uint8_t bold, uint32_t color);
//Устанавливает текстуру для объекта
void MGL_ObjectSetTexture(MGL_OBJ *obj, MGL_TEXTURE *texture);
//Устанавливает градиент для объекта
void MGL_ObjectSetGradient(MGL_OBJ *obj, MGL_GRADIENT *gradient);
//Создает градиент
MGL_GRADIENT* MGL_GradientCreate(MGL_GRADIENT_TYPES type);
//Добавляет ключевую точку в градиент
void MGL_GradientAddColor(MGL_GRADIENT *gradient, uint8_t offset, uint32_t color, uint8_t flag_mix);
//Удаляет градиент
void MGL_GradientDelete(MGL_GRADIENT *gradient);
//Задает угол градиента
void MGL_GradientSetDeg(MGL_GRADIENT *gradient, int deg);
//Устанавливает прозрачность объекта
void MGL_ObjectSetTransparency(MGL_OBJ *obj, uint8_t tr);
//Устанавливает план объекта (на самом переднем плане - 0... на самом заднем плане - 255)
void MGL_ObjectSetPlane(MGL_OBJ *obj, uint8_t plane);
//Устанавливает видимость объекта (0 - невидимый, != 0 - видимый)
void MGL_ObjectSetVisible(MGL_OBJ *obj, uint8_t visible);
//Перемещает объект на расстояние по оси x на dx, по оси y на dy
void MGL_ObjectMove(MGL_OBJ *obj, int dx, int dy);
//Перемещает все объекты в списке на расстояние по оси x на dx, по оси y на dy
void MGL_ObjectListMove(MGL_OBJ *obj_list, int dx, int dy);
//Задает прозрачность для объектов списка (0 - абсолютно непрозрачен. 255 - невидим)
void MGL_ObjectListTransparency(MGL_OBJ *obj_list, uint8_t tr);

//Отрисовывает в буфер объекты (их части), попавшие в текущее окно вывода
void MGL_RenderObjects(MGL_OBJ *obj, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t *data);
#endif /* MICROGL2D_H_ */
