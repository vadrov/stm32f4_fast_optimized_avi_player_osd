/*
 *  Author: VadRov
 *  Copyright (C) 2019, VadRov, all right reserved.
 *
 *  Драйвер инкрементарного механического энкодера.
 *
 *  Допускается свободное распространение.
 *  При любом способе распространения указание автора ОБЯЗАТЕЛЬНО.
 *  В случае внесения изменений и распространения модификаций указание первоначального автора ОБЯЗАТЕЛЬНО.
 *  Распространяется по типу "как есть", то есть использование осуществляется на свой страх и риск.
 *  Автор не предоставляет никаких гарантий.
 *
 *  Версия: для STM32F4
 *
 *	Требования:
 *	- три вывода МК, настроенные как входы с внешними прерыванием EXTI по спадающему и нарастающему фронтам;
 *	- системный таймер SysTick, настроенный на прерывания с частотой 1000 раз в секунду (используется для
 *	"системного времени" при установке временного порога для устранения дребезга и определения времени
 *	нажатия на кнопку);
 *	- инкрементарный механический энкодер с кнопкой, т.е. с тремя выходами: A, B, C (кнопка);
 *	- в идеале пара керамических конденсаторов 0.02-0.1 мкФ (по одному в параллель между выходами
 *	энкодера A и "землей", В и "землей");
 *	- подтяжка входов МК к питанию (внутренняя - опция Pull up либо внешняя - через резисторы).
 *
 *	Особенности и возможности:
 *	- Используются прерывания EXTI, что полностью исключает необходимость постоянного циклического
 *	опроса состояния энкодера, как это могло бы быть в случае использовании чрезмерно частых прерываний
 *	по обновлению таймера. Т.е. анализ состояний энкодера осуществляется только тогда, когда это
 *	требуется - по прерываниям, генерируемым при изменении фронтов сигналов на его выходах.
 *	- Помехоустойчивость и четкость работы энкодера за счет алгоритма обработки состояний выходов энкодера и
 *	использования прерываний "в две стороны" (по спадающему - falling и нарастающему - rising фронтам на входах МК).
 *	- За счет применения прерываний "в две стороны" одновременно на двух выходах энкодера A и B нет необходимости в
 *	организации искусственных задержек в обработчике прерываний, как это можно встретить в различных источниках
 *	в сети, где используется только прерывание по спадающему фронту на одном из двух выходов энкодера. Также за
 *	счет применения	такого подхода снижаются требования к качеству энкодера. Драйвер, в целом, одинаково неплохо
 *	отрабатывает как медленные, так и быстрые вращения ручки энкодера.
 *	- Нет никаких искусственных задержек в программе драйвера.
 *	- Малая вычислительная нагрузка на ядро МК.
 *	- Драйвер генерирует события: вращения ручки энкодера вперед/назад, кратковременного нажатия на кнопку и
 *	удержания кнопки, вращения ручки энкодера вперед/назад при нажатой кнопке.
 *	- Драйвер позволяет менять местами генерацию событий вращения вперед/назад и событий вращения вперед/назад при
 *	нажатой кнопке, т.е. допускает программное инвертирование направления работы энкодера без необходимости
 *	аппаратного переподключения выходов.
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include "main.h"

#define ENCODER_THRESHOLD	4	/* Абсолютное пороговое значение счетчика энкодера для подтверждения направления вращения.
								   Для применяемого энкодера и входов МК с настроенными прерываниями EXTI по фронтам
								   rising/falling направление вращения определяется минимум за 4 прерывания.
								 */

#define ENCODER_BTN_BOUNCE	50	/* Период времени в мс, в течение которого гарантировано завершается переходный
								   процесс (дребезг) при нажатии и отпускании кнопки энкодера.
								 */

#define ENCODER_BTN_HOLD	2000 /* Период времени в мс, определяющий событие удержания кнопки.
								    Событие кратковременного нажатия на кнопку ENCODER_EV_BTN_CLICK наступает
								    после нажатия и отпускания кнопки, если период времени, прошедший с момента
								    нажатия на кнопку до момента отпускания кнопки, меньше значения ENCODER_BTN_HOLD.
								    Если же этот период времени равен или превышает значение ENCODER_BTN_HOLD,
								    то имеет место событие удержания кнопки ENCODER_EV_BTN_HOLD.
								  */

/* Определение 7 событий энкодера: */
#define	ENCODER_EV_NONE			0	/* - нет событий (незначимое событие); */
		       /* Остальные 6 событий значимые */
#define	ENCODER_EV_FWD			1	/* - вращение вперед, по часовой стрелке и т.п.; */
#define	ENCODER_EV_BCK			2	/* - вращение назад, против часовой стрелки и т.п.; */
#define	ENCODER_EV_BTN_CLICK	4	/* - кратковременное нажатие кнопки; */
#define	ENCODER_EV_BTN_HOLD		8	/* - удержание кнопки; */
#define ENCODER_EV_FWD_BTN		16  /* - вращение вперед, по часовой стрелке и т.п. при нажатой кнопке; */
#define ENCODER_EV_BCK_BTN		32  /* - вращение назад, против часовой стрелки и т.п. при нажатой кнопке. */

/* Определение режима инверсии (изменение направления работы) энкодера: */
#define	ENCODER_INV_OFF			0	/* - нет инверсии */
#define	ENCODER_INV_ON			1	/* - работа в режиме инверсии */

/* Определение выходов энкодера, вызывающих прерывание */
#define ENCODER_IRQ_NONE		0	/* Выход неопределен; */
#define ENCODER_IRQ_A			1	/* Выход A; */
#define ENCODER_IRQ_B			2	/* Выход B; */
#define ENCODER_IRQ_C			3	/* Выход C (кнопка). */

/* Структура описывает обработчик энкодера */
typedef struct {
	GPIO_TypeDef *a_port;				/* Порт и номер его вывода, к которому подключен выход A энкодера. */
	uint16_t a_pin;
	GPIO_TypeDef *b_port;				/* Порт и номер его вывода, к которому подключен выход B энкодера. */
	uint16_t b_pin;
	GPIO_TypeDef *c_port;				/* Порт и номер его вывода, к которому подключен выход C энкодера (кнопка). */
	uint16_t c_pin;
	volatile uint8_t fl_click_btn;		/* Флаг нажатия кнопки. Устанавливается в момент нажатия на кнопку,
										   сбрасывается в момент отпускания кнопки.
										 */
	volatile uint32_t time_click_btn;	/* Время в мс, соответствующее времени удержания кнопки.
										   В момент нажатия на кнопку и установки флага fl_click_btn в эту переменную
										   заносится значение системного времени millis. При отпускании кнопки
										   из значения текущего системного времени в переменной millis вычитается значение
										   переменной fl_click_btn. Полученное значение определяет время нажатия на кнопку,
										   по длительности которого можно установить, к какому событию отнести нажатие
										   на кнопку: к кратковременному нажатию или удержанию.
										 */
	volatile uint32_t time_bounce_btn;	/* Время, соответствующее периоду времени, в течение которого мы считаем, что
										   заканчивают протекать переходные процессы (дребезг), вызванные нажатием и
										   отпусканием кнопки. В моменты нажатия/отпускания кнопки и, соответственно,
										   при установке/сбросе флага fl_click_btn в эту переменную заносится значение
										   системного времени millis, увеличенное на величину периода времени ENCODER_BTN_BOUNCE,
										   в течение которого гарантированно заканчивают протекать переходные процессы.
										   Обработчик внешнего прерывания, вызванный изменением фронта сигнала на выходе
										   C энкодера (кнопке), программно блокируется до тех пор, пока системное
										   время millis не достигнет значения времени в переменной time_bounce_btn.
										   Именно по такому алгоритму осуществляется фильтрация дребезга без необходимости
										   организации искуственных задержек, замедляющих работу программы.
										 */
    uint8_t inversion;					/* Флаг, меняющий местами направление работы энкодера. В процессе работы
    									   этот флаг может быть изменен пользователем функцией с прототипом:
    									   void EncoderSetInversion(ENCODER_Handler *enc, uint8_t inv);
    									 */
	volatile uint8_t inc_idx;			/* Индекс в таблице для определения приращений к счетчику encCounter. */
    volatile int encCounter;			/* Счетчик энкодера. По его абсолютному значению будем определять наличие вращения ручки
    									   энкодера, сравнивая его с пороговым значением ENCODER_THRESHOLD. А по знаку этого
    									   значения - направление вращения.
    									 */
    volatile uint8_t prev_event;		/* Предыдущее событие */
} ENCODER_Handler;

/* Инициализирует энкодер enc. */
void EncoderInit(ENCODER_Handler *enc,
				 GPIO_TypeDef *a_port, uint16_t a_pin,
				 GPIO_TypeDef *b_port, uint16_t b_pin,
				 GPIO_TypeDef *c_port, uint16_t c_pin);

/*
 * Опрашивает состояние энкодера и обновляет его обработчик. Возвращает событие,
 *  которое сформировалось к моменту поступления запроса на обновление.
 */
uint8_t EncoderUpdate(ENCODER_Handler *enc, uint8_t ch_irq);

/*
 * Определяет режим работы энкодера: прямой (inv = ENCODER_INV_OFF) и обратный/инверсный (inv = ENCODER_INV_ON).
 * При инверсном режиме работы меняются местами события энкодера: ENCODER_EV_FWD с ENCODER_EV_BCK,
 * ENCODER_EV_FWD_BTN с ENCODER_EV_BCK_BTN.
 */
void EncoderSetInversion(ENCODER_Handler *enc, uint8_t inv);

#endif /* ENCODER_H_ */
