/* USER CODE BEGIN Header */
/*
 *  Copyright (C)2019-2023, VadRov, all right reserved.
 *
 *  Видеоплеер для проигрывания AVI-файлов (файл с расширением avi, контейнер riff)
 *
 *  Допускается свободное распространение.
 *  При любом способе распространения указание автора ОБЯЗАТЕЛЬНО.
 *  В случае внесения изменений и распространения модификаций указание первоначального автора ОБЯЗАТЕЛЬНО.
 *  Распространяется по типу "как есть", то есть использование осуществляется на свой страх и риск.
 *  Автор не предоставляет никаких гарантий.
 *
 *  В файле display_config.h (папка Display) определяется разрешение применяемого дисплея (320x240 или
 *  240x240), а также контроллер дисплея: ST7789 либо ILI9341. Ненужные строки комментируются, а остаются
 *  только те, которые соответствуют фактически применяемому дисплейному модулю. По умолчанию проект настроен
 *  на использование дисплейного модуля разрешением 320x240 пикселей на контроллере ILI9341.
 *
 *  Требования к формату AVI:
 *  - поток видео mjpeg (motion jpeg), т.е. видеокадры должны быть закодированы jpeg кодеком;
 *  - поток аудио mp3, т.е. звук должен быть закодирован mp3 кодеком.
 *
 *  Размер кадра изображения выбирайте исходя из параметров имеющегося дисплея.
 *  Для преобразования видеофайла к поддерживаемому плеером формату рекомендуется использовать
 *  библиотеку ffmpeg. Например, следующая командная строка преобразует mp4-видео в
 *  поддерживаемый плеером формат avi с размером кадра 320х240, частотой 25 кадров в секунду,
 *  с качеством кодирования 15:
 *  	ffmpeg -i video.mp4 -c:a mp3 -c:v mjpeg -s 320x240 -r 25 -q 15 video.avi
 *  где:
 *  	-i video.mp4 - указание на файл-источник, который вы хотите преобразовать в поддерживаемый
 *  плееером формат avi;
 *  	video.avi - выходной файл с расширением avi, в котором будет сохранен результат преобразования;
 *  	-c:a mp3 - указание на то, что "звуковая дорожка" (аудиопоток) в выходном файле должна быть в
 *  формате кодека mp3;
 *  	-c:v mjpeg - указание на то, что "видео дорожка" (видеопоток) в выходном файле должна быть в формате
 *  кодека mjpeg (набор картинок, закодированных кодеком jpeg, т.н., motion jpeg);
 *  	-s 320x240 - указание на размер видеокадра в выходном файле avi (ширина, высота);
 *  	-r 25 - указание на частоту видеокадров в выходном файле avi (кадров в секунду);
 *  	-q 15 - указание на качество кодирования файла avi (от 1 (наилучшее) до 31 (наихудшее)).
 *  Качество звука можно задать ключом -b, означающим bitrate (скорость потока в бит/с).
 *  Например, такой ключ, как,  -b:a 128k, задаст для звукового потока скорость в 128 кбит/с, что считается
 *  для кодека mp3 начальным уровнем при кодировании музыкальных данных (32-96 кбит/с достаточно для
 *  кодирования речи, а от 96 кбит/с и более - музыки низкого качества). Потока в 256 кбит/с достаточно
 *  для высокого качества музыки при кодировании mp3. 320 кбит/с - предел для кодека mp3.
 *  При преобразовании видео учитывайте соотношение сторон кадра исходного видео. Например, если
 *  исходное видео имело соотношение сторон кадра 16:9, то при использовании дисплея с шириной 320
 *  пикселей, высота выходного кадра должна быть 180 пикселей. Т.е. параметр s для приведенного
 *  примера должен быть задан в виде 320х180. Если соотношение сторон исходного кадра 4:3, то при
 *  использовании дисплея с шириной 320 пикселей высота выходного кадра должна быть 240 пикселей.
 *  Т.е. параметр s для этого примера должен быть задан в виде 320х240.
 *  Параметр q определяет качество кодирования и может принимать значения от 1 до 31. С увеличением
 *  q качество снижается, а с уменьшением - увеличивается. Этот параметр, естественно, влияет
 *  на производительность программного декодера jpeg. Золотая середина этого параметра 15-20. При значении 15
 *  для stm32f401ccu6, работающем на частоте 84 МГц, декодер jpeg для кадра размером 320х240 показывает
 *  производительность от 11 до 25 кадров в секунду (40 - 90 мс на декодирование одного кадра изображения),
 *  одновременно декодируя mp3 поток.
 *  Плеер обеспечивает неплохую синхронизацию аудио и видеопотоков. Причем, подсчет времени
 *  воспроизведения, а, соответственно, и синхронизация аудио с видео осуществляются по прерываниям
 *  потока DMA, обслуживающего DAC. В прерывании потока DMA, обслуживающего DAC, увеличивается счетчик
 *  длительности воспроизведения, рассчитываемой по аудиокадрам, на величину длительности воспроизведения звукового
 *  кадра, определяемую по формуле:
 *  						d = 1000000 * samples / samplerate,
 *  где samples - количество сэмплов в кадре, samplerate - частота дискретизации/сэмплирования(Гц),
 *  d - длительность воспроизведения звукового кадра (мкс).
 *	Таким образом, при воспроизведении нет привязки к реальному времени. Ход внутреннего времени плеера
 *	задает тактирование модуля I2S. Расхождение от реального времени определяется разницей между заданной
 *	и реальной частотой дискретизации/сэмплирования. Так, для частоты дискретизации 44100 Гц реальная
 *	частота дискретизации составляет 44100.46875 Гц при 16-битном формате данных и 44099.50781 Гц
 *	при 32-битном формате данных в случае использования кварцевого резонатора на 25 Мгц и встроенного модуля
 *	I2S PLL stm32f401ccu6. Т.е. в обоих случаях ошибка не превышает 0.0011% (см. таблицу 91, RM0368 REV 5).
 *	Синхронизация аудио и видео осуществляется в пределах времени отображения одного видеокадра. Это время
 *	определяется из соответствующего поля avi файла (dwMicroSecPerFrame, время отображения кадра в мкс).
 *	С каждым новым видеокадром (не важно при этом пропущен он или показан) общее время воспроизведения,
 *	рассчитываемое по видеокадрам, увеличивается на указанную величину. В свою очередь, с каждым
 *	воспроизведенным аудиокадром увеличивается на величину d общее время воспроизведения, рассчитываемое
 *	по аудиокадрам (в прерывании потока DMA, обслуживающего DAC). По разнице между этими значениями времени
 *	плеер принимает одно из трех решений:
 *		 - декодировать видеокадр, если разница времени не превышает длительности отображения одного
 *		 видеокадра;
 *		 - пропустить декодирование видеокадра (переход на следующий видеокадр с инкрементом счетчика
 *		 воспроизведения для видео), если время, рассчитываемое по видеокадрам, отстает от времени,
 *		 рассчитанного по воспроизведенным аудиокадрам;
 *		 - организовать задержку, равную разнице времени (после чего перейти к декодированию кадра), если
 *		 время, расчитанное по видеокадрам, превышает время, рассчитанное по аудиокадрам.
 *
 *  Демонстрируемый проект использует следующие драйвера, модули, библиотеки ...:
 *
 *  Для декодирования jpeg изображений используется модифицированная и оптимизированная для stm32
 *  библиотека TJpgDec - Tiny JPEG Decompressor R0.03 (папка JPEG)
 *  Portions copyright (C) 2021, ChaN,   all right reserved.
 *  Portions сopyright (C) 2022-2023, VadRov, all right reserved.
 *
 *  Для работы с SD картой используется драйвер SD карты (папка FATFS)
 *  Portions copyright (C) 2014, ChaN, all rights reserved.
 *  Portions copyright (C) 2017, kiwih, all rights reserved.
 *  Portions сopyright (C) 2019, VadRov, all right reserved.
 *
 *  Для работы с файловой системой используется библиотека FatFs -
 *  Generic FAT Filesystem Module  R0.15 w/patch1 (папка FATFS)
 *  Copyright (C) 2022, ChaN, all right reserved.
 *
 *  Драйвер DAC на базе PCM5102 (папка PCM5102)
 *  Copyright (C) 2019, VadRov, all right reserved.
 *
 *  Драйвер дисплея V1.4 с вариантом "ASM-турбо" (папка Display)
 *  Copyright (C) 2019-2023 VadRov, all right reserved.
 *
 *  Fixed-point MP3 decoder (папка MP3Helix)
 *  Portions Copyright (c) 1995-2002 RealNetworks, Inc. All Rights Reserved.
 *
 *  Файловый менеджер (папка FileManager)
 *  Copyright (C) 2022, VadRov, all right reserved.
 *
 *  Модуль для работы с кнопоками (папка Keyboard)
 *  Copyright (C) 2021, VadRov, all right reserved.
 *
 *  Графическая библиотека для работы с объектами-примитивами
 *  в двумерном пространстве microGL2D (папка MicroGL2D)
 *  Copyright (C) 2022, VadRov, all right reserved.
 *
 *  Драйвер энкодера для STM32F4 (папка Encoder)
 *  Copyright (C) 2019, VadRov, all right reserved.
 *
 *  Процедуры работы со строками (папка MyString)
 *  Copyright (C) 2019, VadRov, all right reserved.
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "display.h" 		/* Драйвер/библиотека дисплея */
#include "display_config.h" /* Конфигрурация дисплея (там настройки для вашего дисплейного модуля) */
#if defined(LCD_SIZE_240X240)
#define DISPLAY_SIZE1		240
#define DISPLAY_SIZE2		240
#elif defined(LCD_SIZE_320X240)
#define DISPLAY_SIZE1		320
#define DISPLAY_SIZE2		240
#endif
#if defined(LCD_ILI9341_DRIVER)
#include "ili9341.h" 		/* Поддержка дисплея на базе контроллера ili9341 */
#define DISPLAY_CONTROLLER_WIDTH	ILI9341_CONTROLLER_WIDTH
#define DISPLAY_CONTROLLER_HEIGHT	ILI9341_CONTROLLER_HEIGHT
#define DISPLAY_Init		ILI9341_Init
#define DISPLAY_SetWindow	ILI9341_SetWindow
#define DISPLAY_SleepIn		ILI9341_SleepIn
#define DISPLAY_SleepOut	ILI9341_SleepOut
#elif defined(LCD_ST7789_DRIVER)
#include "st7789.h" 		/* Поддержка дисплея на базе контроллера st7789 */
#define DISPLAY_CONTROLLER_WIDTH	ST7789_CONTROLLER_WIDTH
#define DISPLAY_CONTROLLER_HEIGHT	ST7789_CONTROLLER_HEIGHT
#define DISPLAY_Init		ST7789_Init
#define DISPLAY_SetWindow	ST7789_SetWindow
#define DISPLAY_SleepIn		ST7789_SleepIn
#define DISPLAY_SleepOut	ST7789_SleepOut
#endif
#include "pcm5102.h" 		/* драйвер DAC PCM5102 */
#include "ff.h"		 		/* библиотека FatFS */
#include "avi.h"	 		/* плеер AVI */
#include "filemanager.h"	/* файловый менеджер */
#include "keyboard.h"		/* модуль работы с кнопками */
#include "encoder.h"		/* драйвер энкодера */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

//счетчик "системного времени", мс
volatile uint32_t millis = 0;

//Переменные для библиотеки FatFs
char Path[4];
FATFS FatFS;

//Обработчик энкодера
ENCODER_Handler encoder1;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI2_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void DWT_init (void)
{
	if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk))	{
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	#ifdef __CORE_CM7_H_GENERIC
	        DWT->LAR = 0xC5ACCE55;  //разблокирование доступа к DWT регистру
	#endif
	   DWT->CYCCNT = 0;
	   DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	/* включаем кэширование инструкций, кэширование данных и систему предварительной выборки инструкций */
	FLASH->ACR |= (0x1UL << 9U) | (0x1UL << 10U) | (0x1UL << 8U);
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),15, 0));

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	//настраиваем системный таймер (прерывания 1000 раз в секунду)
	SysTick_Config(SystemCoreClock/1000);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI2_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

	//Инициализация I2S3
    Init_DAC();
	DWT_init();

	//------------------- Настройка параметров и инициализация дисплея ---------------------
	LCD_BackLight_data bl_dat = { .htim_bk = TIM3,
		  	  	  	  	  	  	  .channel_htim_bk = LL_TIM_CHANNEL_CH1,
    							  .blk_port = 0,
    							  .blk_pin = 0,
    							  .bk_percent = 75 };

	LCD_DMA_TypeDef hdma_spi1_tx = { .dma = DMA2,
		  	  	  	  	  	  	     .stream = LL_DMA_STREAM_3 };

	LCD_SPI_Connected_data spi_dat = { .spi = SPI1,
  		  							   .dma_tx = hdma_spi1_tx,
   									   .reset_port = LCD_RES_GPIO_Port,
   									   .reset_pin = LCD_RES_Pin,
   									   .dc_port = LCD_DC_GPIO_Port,
   									   .dc_pin = LCD_DC_Pin,
   									   .cs_port = LCD_CS_GPIO_Port,
   									   .cs_pin = LCD_CS_Pin  };
#ifndef LCD_DYNAMIC_MEM
LCD_Handler lcd1;
#endif
LCD = LCD_DisplayAdd( LCD,
#ifndef LCD_DYNAMIC_MEM
	  	  	  	  	  &lcd1,
#endif
					  DISPLAY_SIZE1,
					  DISPLAY_SIZE2,
					  DISPLAY_CONTROLLER_WIDTH,
					  DISPLAY_CONTROLLER_HEIGHT,
					  /* Задаем смещение по ширине и высоте для нестандартных или бракованных дисплеев: */
					  0,	/* смещение по ширине дисплейной матрицы */
					  0,	/* смещение по высоте дисплейной матрицы */
					  PAGE_ORIENTATION_LANDSCAPE,
					  DISPLAY_Init,
					  DISPLAY_SetWindow,
					  DISPLAY_SleepIn,
					  DISPLAY_SleepOut,
					  &spi_dat,
					  LCD_DATA_16BIT_BUS,
					  bl_dat );

	LCD_Handler *lcd = LCD; //указатель на первый дисплей в списке
	LCD_Init(lcd);
	LCD_Fill(lcd, 0x319bb1);
	//------------------------------------------------------------------------------------------

	//----------------------------------- Монтирование диска -----------------------------------
	(void)f_mount(&FatFS, Path, 0);
	//------------------------------------------------------------------------------------------

	/* ---------------------------- Настройка файлового менеджера ---------------------------- */
	/* Определение цветовой схемы */
	File_Manager_Color_Scheme color_scheme_pl = { COLOR_YELLOW,		/* цвет текста наименования каталога */
			  	  	  	  	  	  	  	  	  	  COLOR_WHITE,		/* цвет текста наименования файла */
												  COLOR_BLACK,		/* цвет текста выбранного файла/каталога */
												  0x319bb1,			/* цвет фона окна менеджера (для пустых строк) */
												  0x319bb1,			/* цвет фона четных строк */
												  0x319bb1,			/* цвет фона нечетных строк */
												  COLOR_LIGHTGREY,	/* цвет курсора текущего файла/каталога */
												  COLOR_LIGHTGREY,	/* цвет ползунка вертикальной прокрутки */
												  0x319bb1,			/* цвет полосы вертикальной прокрутки */
												  1					/* флаг закрашивания курсора:
																  	  	  0 - текст выбранного файла/каталога обводится
																  	  	  	  прямоугольником цвета курсора (cursor_color).
														 	 	  	  	  >0 - фон текста выбранного файла/каталога приравнивается
														 	 	  	  	  	  цвету курсора (cursor_color) */
												 	 };

	File_Manager_Handler *fm = FileManagerNew();	/* Создание обработчика файлового менеджера */
	fm->SetDisplay(fm, lcd);						/* Дисплей, на который выводится файловый менеджер (указатель) */
	fm->SetWin(fm, 0, 0, lcd->Width, lcd->Height);/* Параметры окна файлового менеджера:
	  	  	  	  	  	  	  	  	  	  	  	  	   - позиция левого верхнего угла окна по горизонтали;
	  	  	  	  	  	  	  	  	  	  	  	  	   - позиция левого верхнего угла окна по вертикали;
	  	  	  	  	  	  	  	  	  	  	  	  	   - ширина окна;
	  	  	  	  	  	  	  	  	  	  	  	  	   - высота окна. */
	fm->SetColor(fm, &color_scheme_pl);				 /* Цветовая схема (указатель) */
	fm->SetFont(fm, &Font_8x13);					 /* Шрифт (указатель) */
	fm->SetKeys(fm, KEYB_UP, KEYB_DOWN, KEYB_RIGHT); /* Кнопки управления:
	   	   	   	   	   	   	   	   	   	   	   	   	    - номер бита кнопки вверх;
	   	   	   	   	   	   	   	   	   	   	   	   	    - номер бита кнопки вниз;
	   	   	   	   	   	   	   	   	   	   	   	   	    - номер бита кнопки ввод/выбор. */
	//-----------------------------------------------------------------------------------------

	//----------------------------------- Настройка энкодера ----------------------------------
	//Будем работать с модулем Keyboard и эмулировать для него нажатие кнопок через
	//специальную функцию, связывающую обработчик энкодера с модулем для работы с кнопками.
	//Такую роль выполняет функция EncoderEventToKeyboard, находящаяся в файле stm32f4xx_it.c
	KEYB_all_button = 3; //Используем 3 кнопки для модуля Keyboard
	//Инициализируем обработчик энкодера
	EncoderInit(&encoder1,
				ENCODER_A_GPIO_Port,
				ENCODER_A_Pin,
				ENCODER_B_GPIO_Port,
				ENCODER_B_Pin,
				ENCODER_C_GPIO_Port,
				ENCODER_C_Pin);
	EncoderSetInversion(&encoder1, ENCODER_INV_OFF); //Если энкодер работает в противоположную сторону
													 //от ожидаемого направления, то здесь можно задать
													 //инверсию для энкодера, передав функции параметр ENCODER_INV_ON.
													 //Это позволит не переподключать выводы энкодера A и B.
	//------------------------------------------------------------------------------------------


	//Заставка с информацией о программе и авторе
	AVIIntro(lcd);

	uint8_t f_popup = 1;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
		/* Заливаем дисплей синим цветом (цветом фона файлового менеджера) */
    	LCD_Fill(lcd, 0x319bb1);
		/* "Всплывающее" окно файлового менеджера */
		if (f_popup) {
			FileManagerPopupWin(fm, 5);
			f_popup = 0;
		}
		/* Запуск менеджера файлов */
		fm->Show(fm);
		/* Проверка статуса завершения работы менеджера файлов */
		if (fm->GetStatus(fm) == File_Manager_OK) { /* Завершение без ошибок с выбором файла? */
			/* Запускаем видеооплеер */
			PlayAVI(fm->GetFilePath(fm), fm->GetFileName(fm), lcd, 0, 0, lcd->Width, lcd->Height);
		}
		else {
			/* Выводим сообщение, что работа файлового менеджера завершена с ошибкой */
			LCD_WriteString(lcd, 20, 0, "Error FileManager!", fm->font, fm->color_scheme->text_color_file, fm->color_scheme->bg_color, LCD_SYMBOL_PRINT_FAST);
			/* Ожидаем нажатия любой кнопки */
			while(!KEYB_kbhit()) { }
		}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE2);
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_HSE_EnableCSS();
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_25, 168, LL_RCC_PLLP_DIV_2);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  while (LL_PWR_IsActiveFlag_VOS() == 0)
  {
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_Init1msTick(84000000);
  LL_SetSystemCoreClock(84000000);
  LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**SPI1 GPIO Configuration
  PA5   ------> SPI1_SCK
  PA7   ------> SPI1_MOSI
  */
  GPIO_InitStruct.Pin = LCD_SCL_Pin|LCD_SDA_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* SPI1 DMA Init */

  /* SPI1_TX Init */
  LL_DMA_SetChannelSelection(DMA2, LL_DMA_STREAM_3, LL_DMA_CHANNEL_3);

  LL_DMA_SetDataTransferDirection(DMA2, LL_DMA_STREAM_3, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetStreamPriorityLevel(DMA2, LL_DMA_STREAM_3, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA2, LL_DMA_STREAM_3, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA2, LL_DMA_STREAM_3, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA2, LL_DMA_STREAM_3, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA2, LL_DMA_STREAM_3, LL_DMA_PDATAALIGN_HALFWORD);

  LL_DMA_SetMemorySize(DMA2, LL_DMA_STREAM_3, LL_DMA_MDATAALIGN_HALFWORD);

  LL_DMA_EnableFifoMode(DMA2, LL_DMA_STREAM_3);

  LL_DMA_SetFIFOThreshold(DMA2, LL_DMA_STREAM_3, LL_DMA_FIFOTHRESHOLD_FULL);

  LL_DMA_SetMemoryBurstxfer(DMA2, LL_DMA_STREAM_3, LL_DMA_MBURST_INC8);

  LL_DMA_SetPeriphBurstxfer(DMA2, LL_DMA_STREAM_3, LL_DMA_PBURST_SINGLE);

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_16BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 10;
  LL_SPI_Init(SPI1, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);
  /* USER CODE BEGIN SPI1_Init 2 */
#if defined(LCD_ILI9341_DRIVER)
  LL_GPIO_SetPinPull(LCD_SCL_GPIO_Port, LCD_SCL_Pin, LL_GPIO_PULL_NO);
  LL_GPIO_SetPinPull(LCD_SDA_GPIO_Port, LCD_SDA_Pin, LL_GPIO_PULL_NO);
  LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_LOW);
#elif  defined(LCD_ST7789_DRIVER)
  LL_GPIO_SetPinPull(LCD_SCL_GPIO_Port, LCD_SCL_Pin, LL_GPIO_PULL_UP);
  LL_GPIO_SetPinPull(LCD_SDA_GPIO_Port, LCD_SDA_Pin, LL_GPIO_PULL_UP);
  LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_HIGH);
#endif
  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  /**SPI2 GPIO Configuration
  PB13   ------> SPI2_SCK
  PB14   ------> SPI2_MISO
  PB15   ------> SPI2_MOSI
  */
  GPIO_InitStruct.Pin = SD_CLK_Pin|SD_MISO_Pin|SD_MOSI_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* SPI2 DMA Init */

  /* SPI2_RX Init */
  LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_3, LL_DMA_CHANNEL_0);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_3, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_3, LL_DMA_PRIORITY_MEDIUM);

  LL_DMA_SetMode(DMA1, LL_DMA_STREAM_3, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_3, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_3, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_3, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_3, LL_DMA_MDATAALIGN_BYTE);

  LL_DMA_EnableFifoMode(DMA1, LL_DMA_STREAM_3);

  LL_DMA_SetFIFOThreshold(DMA1, LL_DMA_STREAM_3, LL_DMA_FIFOTHRESHOLD_FULL);

  LL_DMA_SetMemoryBurstxfer(DMA1, LL_DMA_STREAM_3, LL_DMA_MBURST_INC8);

  LL_DMA_SetPeriphBurstxfer(DMA1, LL_DMA_STREAM_3, LL_DMA_PBURST_SINGLE);

  /* SPI2_TX Init */
  LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_4, LL_DMA_CHANNEL_0);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_4, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_4, LL_DMA_PRIORITY_MEDIUM);

  LL_DMA_SetMode(DMA1, LL_DMA_STREAM_4, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_4, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_4, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_4, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_4, LL_DMA_MDATAALIGN_BYTE);

  LL_DMA_EnableFifoMode(DMA1, LL_DMA_STREAM_4);

  LL_DMA_SetFIFOThreshold(DMA1, LL_DMA_STREAM_4, LL_DMA_FIFOTHRESHOLD_FULL);

  LL_DMA_SetMemoryBurstxfer(DMA1, LL_DMA_STREAM_4, LL_DMA_MBURST_INC8);

  LL_DMA_SetPeriphBurstxfer(DMA1, LL_DMA_STREAM_4, LL_DMA_PBURST_SINGLE);

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV128;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 10;
  LL_SPI_Init(SPI2, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI2, LL_SPI_PROTOCOL_MOTOROLA);
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};
  LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  TIM_InitStruct.Prescaler = 999;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 209;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM3, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM3);
  LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH1);
  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 105;
  TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
  LL_TIM_OC_Init(TIM3, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM3, LL_TIM_CHANNEL_CH1);
  LL_TIM_SetTriggerOutput(TIM3, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM3);
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**TIM3 GPIO Configuration
  PA6   ------> TIM3_CH1
  */
  GPIO_InitStruct.Pin = LCD_BLK_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_2;
  LL_GPIO_Init(LCD_BLK_GPIO_Port, &GPIO_InitStruct);

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* Init with LL driver */
  /* DMA controller clock enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);

  /* DMA interrupt init */
  /* DMA1_Stream3_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Stream3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),2, 0));
  NVIC_EnableIRQ(DMA1_Stream3_IRQn);
  /* DMA1_Stream4_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Stream4_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),2, 0));
  NVIC_EnableIRQ(DMA1_Stream4_IRQn);
  /* DMA2_Stream3_IRQn interrupt configuration */
  NVIC_SetPriority(DMA2_Stream3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),3, 0));
  NVIC_EnableIRQ(DMA2_Stream3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

  /**/
  LL_GPIO_SetOutputPin(LED_INDCTR_GPIO_Port, LED_INDCTR_Pin);

  /**/
  LL_GPIO_SetOutputPin(GPIOA, LCD_DC_Pin|LCD_RES_Pin|LCD_CS_Pin);

  /**/
  LL_GPIO_SetOutputPin(GPIOB, T_CS_Pin|SD_CS_Pin);

  /**/
  GPIO_InitStruct.Pin = LED_INDCTR_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED_INDCTR_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LCD_DC_Pin|LCD_RES_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LCD_CS_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = T_CS_Pin|SD_CS_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /**/
  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE0);

  /**/
  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE1);

  /**/
  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTB, LL_SYSCFG_EXTI_LINE10);

  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_0;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
  LL_EXTI_Init(&EXTI_InitStruct);

  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_1;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
  LL_EXTI_Init(&EXTI_InitStruct);

  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_10;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
  LL_EXTI_Init(&EXTI_InitStruct);

  /**/
  LL_GPIO_SetPinPull(ENCODER_A_GPIO_Port, ENCODER_A_Pin, LL_GPIO_PULL_UP);

  /**/
  LL_GPIO_SetPinPull(ENCODER_B_GPIO_Port, ENCODER_B_Pin, LL_GPIO_PULL_UP);

  /**/
  LL_GPIO_SetPinPull(ENCODER_C_GPIO_Port, ENCODER_C_Pin, LL_GPIO_PULL_UP);

  /**/
  LL_GPIO_SetPinMode(ENCODER_A_GPIO_Port, ENCODER_A_Pin, LL_GPIO_MODE_INPUT);

  /**/
  LL_GPIO_SetPinMode(ENCODER_B_GPIO_Port, ENCODER_B_Pin, LL_GPIO_MODE_INPUT);

  /**/
  LL_GPIO_SetPinMode(ENCODER_C_GPIO_Port, ENCODER_C_Pin, LL_GPIO_MODE_INPUT);

  /* EXTI interrupt init*/
  NVIC_SetPriority(EXTI0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1, 0));
  NVIC_EnableIRQ(EXTI0_IRQn);
  NVIC_SetPriority(EXTI1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1, 0));
  NVIC_EnableIRQ(EXTI1_IRQn);
  NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1, 0));
  NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
