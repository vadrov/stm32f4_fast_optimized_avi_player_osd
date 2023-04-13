/*
 *  Author: VadRov
 *  Copyright (C) 2019, VadRov, all right reserved.
 *
 *	Драйвер аудио-ЦАП (Audio Stereo DAC) PCM5102
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
 *
 */

#include "pcm5102.h"

//инициализация I2S3
void Init_DAC(void)
{
	LL_I2S_InitTypeDef I2S_InitStruct = {0};
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	//включаем тактирование spi3/i2s3
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI3);
	//включаем тактирование порта B
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	//включаем тактирование порта A
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

	//настраиваем модуль PLLI2S
	LL_RCC_PLLI2S_ConfigDomain_I2S(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLI2SM_DIV_25, 302, LL_RCC_PLLI2SR_DIV_2);

	//включаем модуль PLLI2S
	RCC->CR |= RCC_CR_PLLI2SON;

	//ждем активации модуля PLLI2S
	while (!(RCC->CR & RCC_CR_PLLI2SRDY)) { __NOP(); }

	//включаем тактирование I2S от модуля PLLI2S
	RCC->CFGR &= ~RCC_CFGR_I2SSRC;

	/* I2S3 конфигурация GPIO (для режима transmit master):
	  PB3   ------> I2S3_CK (AF06)
	  PB5   ------> I2S3_SD (AF06)
	  PA15	------> I2S3_WS (AF06)
	*/
	//выводы PB3 и PB5
	GPIO_InitStruct.Pin = LL_GPIO_PIN_3|LL_GPIO_PIN_5;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	//вывод PA15
	GPIO_InitStruct.Pin = LL_GPIO_PIN_15;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//включаем тактирование контроллера DMA1
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
	//настраиваем приоритет прерываний
	NVIC_SetPriority(DMA1_Stream5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1, 0));
	//включаем прерывания от потока
	NVIC_EnableIRQ(DMA1_Stream5_IRQn);

	//настройка DMA
	LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_5, LL_DMA_CHANNEL_0);
	LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_5, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
	LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_5, LL_DMA_PRIORITY_HIGH);
	LL_DMA_SetMode(DMA1, LL_DMA_STREAM_5, LL_DMA_MODE_CIRCULAR);
	LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_5, LL_DMA_PERIPH_NOINCREMENT);
	LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_5, LL_DMA_MEMORY_INCREMENT);
	LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_5, LL_DMA_PDATAALIGN_HALFWORD);
	LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_5, LL_DMA_MDATAALIGN_HALFWORD);
	LL_DMA_EnableFifoMode(DMA1, LL_DMA_STREAM_5);								//используем стек FIFO для DMA
	LL_DMA_SetFIFOThreshold(DMA1, LL_DMA_STREAM_5, LL_DMA_FIFOTHRESHOLD_FULL);	//уровень заполненности стека FIFO - полный
	LL_DMA_SetMemoryBurstxfer(DMA1, LL_DMA_STREAM_5, LL_DMA_MBURST_INC8);
	LL_DMA_SetPeriphBurstxfer(DMA1, LL_DMA_STREAM_5, LL_DMA_PBURST_SINGLE);

	//настройка I2S3
	I2S_InitStruct.Mode = LL_I2S_MODE_MASTER_TX;
	I2S_InitStruct.Standard = LL_I2S_STANDARD_PHILIPS;
	I2S_InitStruct.MCLKOutput = LL_I2S_MCLK_OUTPUT_DISABLE;
	I2S_InitStruct.DataFormat = LL_I2S_DATAFORMAT_16B;
	I2S_InitStruct.ClockPolarity = LL_I2S_POLARITY_LOW;
	I2S_InitStruct.AudioFreq = LL_I2S_AUDIOFREQ_44K;
	LL_I2S_Init(SPI3, &I2S_InitStruct);
}

void Start_DAC(SPI_TypeDef *spi, DMA_TypeDef *dma_x, uint32_t stream, uint32_t *buff, uint32_t buff_size)
{
	spi->I2SCFGR |= (SPI_I2SCFGR_I2SMOD | SPI_I2SCFGR_I2SE); //выбираем режим i2s и включаем i2s
	DMA_Stream_TypeDef *dma_TX = ((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)dma_x + STREAM_OFFSET_TAB[stream])));
	uint8_t shift[8] = {0, 6, 16, 22, 0, 6, 16, 22}; //битовое смещение во флаговых регистрах IFCR (L и H)
	volatile uint32_t *ifcr_tx = (stream > 3) ? &(dma_x->HIFCR) : &(dma_x->LIFCR);
	dma_TX->CR &= ~DMA_SxCR_EN; 				//выключаем поток DMA
	while (dma_TX->CR & DMA_SxCR_EN) {__NOP();} //ждем отключения потока DMA
	*ifcr_tx = 0x3F<<shift[stream]; 			//сбрасываем флаги прерываний tx
	spi->CR2 |= SPI_CR2_TXDMAEN; 				//разрешаем spi отправлять запросы к DMA
	//запрещаем прерывания по некоторым событиям канала передачи tx и режим двойного буфера
	dma_TX->CR &= ~(DMA_SxCR_DMEIE | DMA_SxCR_TEIE | DMA_SxCR_DBM);
	dma_TX->FCR &= ~DMA_SxFCR_FEIE;
	//разрешаем прерывания по окончанию передачи и по передаче половины буфера
	dma_TX->CR |= (DMA_SxCR_TCIE | DMA_SxCR_HTIE);
	//настраиваем адреса и длину
	dma_TX->PAR = (uint32_t)(&spi->DR); //приемник периферия - адрес регистра DR spi
	dma_TX->M0AR = (uint32_t)buff; 		//источник память - адрес буфера исходящих данных
	dma_TX->NDTR = (uint32_t)buff_size; //размер передаваемых данных
	dma_TX->CR |= (DMA_SxCR_EN); 		//включение потока DMA
}

void Update_DAC(SPI_TypeDef *spi, DMA_TypeDef *dma_x, uint32_t stream, uint32_t *buff, uint32_t buff_size)
{
	DMA_Stream_TypeDef *dma_TX = ((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)dma_x + STREAM_OFFSET_TAB[stream])));
	uint8_t shift[8] = {0, 6, 16, 22, 0, 6, 16, 22}; //битовое смещение во флаговых регистрах IFCR (L и H)
	volatile uint32_t *ifcr_tx = (stream > 3) ? &(dma_x->HIFCR) : &(dma_x->LIFCR);
	dma_TX->CR &= ~DMA_SxCR_EN; 				//выключаем поток DMA
	while (dma_TX->CR & DMA_SxCR_EN) {__NOP();} //ждем отключения канала DMA
	*ifcr_tx = 0x3F<<shift[stream];				//сбрасываем флаги прерываний tx
	//---------------------------- Обновляем данные буфера ------------------------------
	dma_TX->M0AR = (uint32_t)buff; 		//источник память - адрес буфера исходящих данных
	dma_TX->NDTR = (uint32_t)buff_size; //размер передаваемых данных
	//-----------------------------------------------------------------------------------
	dma_TX->CR |= (DMA_SxCR_EN); 		//включение канала передачи
}

void Pause_DAC(SPI_TypeDef *spi)
{
	spi->CR2 &= ~SPI_CR2_TXDMAEN; //запрещаем spi отправлять запросы к DMA
}

void Resume_DAC(SPI_TypeDef *spi)
{
	spi->CR2 |= SPI_CR2_TXDMAEN; //разрешаем spi отправлять запросы к DMA
}

void Stop_DAC(SPI_TypeDef *spi, DMA_TypeDef *dma_x, uint32_t stream)
{
	DMA_Stream_TypeDef *dma_TX = ((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)dma_x + STREAM_OFFSET_TAB[stream])));
	uint8_t shift[8] = {0, 6, 16, 22, 0, 6, 16, 22}; //битовое смещение во флаговых регистрах IFCR (L и H)
	volatile uint32_t *ifcr_tx = (stream > 3) ? &(dma_x->HIFCR) : &(dma_x->LIFCR);
	spi->CR2 &= ~SPI_CR2_TXDMAEN; 				//запрещаем spi отправлять запросы к DMA
	dma_TX->CR &= ~DMA_SxCR_EN; 				//выключаем поток DMA
	while (dma_TX->CR & DMA_SxCR_EN) {__NOP();} //ждем отключения канала DMA
	*ifcr_tx = 0x3F<<shift[stream];				//сбрасываем флаги прерываний tx
	spi->I2SCFGR &= ~SPI_I2SCFGR_I2SE; 			//выключаем i2s
}

uint8_t Setup_DAC (SPI_TypeDef *spi, uint32_t SampleRate, uint32_t DataFormat)
{
	LL_I2S_InitTypeDef I2S_InitStruct = {0};
	uint32_t pll_N = 0, pll_R = 0;

	typedef struct {
		uint32_t freq, pll_N16, pll_R16, pll_N32, pll_R32;
	} PLL_data;

	//Таблица с настройками модуля PLLI2S
	const PLL_data pll_table[] = {	{  8000, 192, LL_RCC_PLLI2SR_DIV_2, 192, LL_RCC_PLLI2SR_DIV_3},
									{ 16000, 192, LL_RCC_PLLI2SR_DIV_3, 256, LL_RCC_PLLI2SR_DIV_2},
									{ 22050, 290, LL_RCC_PLLI2SR_DIV_3, 302, LL_RCC_PLLI2SR_DIV_2},
									{ 32000, 256, LL_RCC_PLLI2SR_DIV_2, 256, LL_RCC_PLLI2SR_DIV_5},
									{ 44100, 302, LL_RCC_PLLI2SR_DIV_2, 429, LL_RCC_PLLI2SR_DIV_4},
									{ 48000, 192, LL_RCC_PLLI2SR_DIV_5, 384, LL_RCC_PLLI2SR_DIV_5},
									{ 96000, 384, LL_RCC_PLLI2SR_DIV_5, 424, LL_RCC_PLLI2SR_DIV_3},
									{192000, 424, LL_RCC_PLLI2SR_DIV_3, 258, LL_RCC_PLLI2SR_DIV_3}  };
	const PLL_data *tbl_ptr = pll_table;
	uint8_t i = 0;
	//ищем в таблице настройки для модуля PLLI2S
	while (i < 8) {
		if (SampleRate == tbl_ptr->freq) {
			if (DataFormat == LL_I2S_DATAFORMAT_16B ||
				DataFormat == LL_I2S_DATAFORMAT_16B_EXTENDED) {
				pll_N = tbl_ptr->pll_N16;
				pll_R = tbl_ptr->pll_R16;
			}
			else {
				pll_N = tbl_ptr->pll_N32;
				pll_R = tbl_ptr->pll_R32;
			}
			break;
		}
		tbl_ptr++;
		i++;
	}
	if (!pll_N) {
		return 1;
	}

	spi->I2SCFGR &= ~SPI_I2SCFGR_I2SE; //выключаем I2S для настройки
	RCC->CR &= ~RCC_CR_PLLI2SON; //выключаем модуль PLLI2S
	LL_RCC_PLLI2S_ConfigDomain_I2S(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLI2SM_DIV_25, pll_N, pll_R); //настраиваем модуль PLLI2S
	RCC->CR = RCC_CR_PLLI2SON; //включаем модуль PLLI2S
	while (!(RCC->CR & RCC_CR_PLLI2SRDY)) { __NOP(); } //ждем активации модуля PLLI2S

	//настройка I2S
	I2S_InitStruct.Mode = LL_I2S_MODE_MASTER_TX;
	I2S_InitStruct.Standard = LL_I2S_STANDARD_PHILIPS;
	I2S_InitStruct.MCLKOutput = LL_I2S_MCLK_OUTPUT_DISABLE;
	I2S_InitStruct.DataFormat = DataFormat;
	I2S_InitStruct.ClockPolarity = LL_I2S_POLARITY_LOW;
	I2S_InitStruct.AudioFreq = SampleRate;
	return LL_I2S_Init(spi, &I2S_InitStruct) == SUCCESS ? 0 : 1;
}
