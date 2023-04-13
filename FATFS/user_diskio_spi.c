/*
 ******************************************************************************
  * @file    user_diskio_spi.c
  * @brief   This file contains the common defines and functions prototypes for
  *          the user_diskio_spi driver implementation
  ******************************************************************************
  * Portions copyright (C) 2014, ChaN, all rights reserved.
  * Portions copyright (C) 2017, kiwih, all rights reserved.
  *
  * This software is a free software and there is NO WARRANTY.
  * No restriction on use. You can use, modify and redistribute it for
  * personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
  * Redistributions of source code must retain the above copyright notice.
  *
  ******************************************************************************
 *
 *  Portions copyright (C) 2019, VadRov, all right reserved.
 *  низкоуровневый драйвер для работы с SD картой по SPI, в том числе, с DMA
 *  Модифицировано: 2019, VadRov
 *  версия 1.1 (NO_HAL)
 *
 *  https://zen.yandex.ru/vadrov
 *  https://www.youtube.com/c/VadRov
 *  https://vk.com/vadrov
 */

#include "user_diskio_spi.h"
#include "main.h"

extern uint32_t millis; //Счетчик миллисекунд. Эта переменная должна увеличиваться на 1
						//каждую миллисекунду, например, в прерывании системного таймера
						//SysTick

//-------------------------------- Настройки SPI и DMA ----------------------------
#define SPI_SD				SPI2 	//SPI, к которому подключена карта
#define SD_INI_SPEED		6 		//скорость SPI при инициализации карты:
/*									0 - FCLK/2,
									1 - FCLK/4,
									2 - FCLK/8,
									3 - FCLK/16,
									4 - FCLK/32,
									5 - FCLK/64,
									6 - FCLK/128,
									7 - FCLK/256
*/
#define SD_MAX_SPEED		0 		//скорость SPI после инициализации карты

#define SD_USE_DMA					//использование DMA -> закомментировать, если без DMA

#ifdef SD_USE_DMA           		//А ниже настройки каналов DMA (активны при включенном DMA):
#define SD_DMA_CNTR_RX		DMA1	//контроллер DMA для входящих данных
#define SD_DMA_STREAM_RX	3		//номер потока DMA для входящих данных
#define SD_DMA_CNTR_TX		DMA1	//контроллер DMA для исходящих данных
#define SD_DMA_STREAM_TX	4		//номер потока DMA для исходящих данных
#endif

//Здесь ничего менять не надо, если выполнить перечисленные действия
#define GPIO_SD_CS			SD_CS_GPIO_Port	//порт выбора чипа -> в CubeIDE ввести метку для соответствующего пина "SD_CS"
#define GPIO_SD_CS_PIN		SD_CS_Pin		//пин порта выбора чипа

#define SD_TIMEOUT			100		//Допустимое время ожидания готовности, в мс

//----------------------------------- Настройки закончились ---------------------------------

/* Definitions for MMC/SDC command */
#define CMD0		(0x40+0)	/* GO_IDLE_STATE */
#define CMD1		(0x40+1)	/* SEND_OP_COND (MMC) */
#define ACMD41		(0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD8		(0x40+8)	/* SEND_IF_COND */
#define CMD9		(0x40+9)	/* SEND_CSD */
#define CMD10		(0x40+10)	/* SEND_CID */
#define CMD12		(0x40+12)	/* STOP_TRANSMISSION */
#define ACMD13		(0xC0+13)	/* SD_STATUS (SDC) */
#define CMD16		(0x40+16)	/* SET_BLOCKLEN */
#define CMD17		(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD18		(0x40+18)	/* READ_MULTIPLE_BLOCK */
#define CMD23		(0x40+23)	/* SET_BLOCK_COUNT (MMC) */
#define ACMD23		(0xC0+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24		(0x40+24)	/* WRITE_BLOCK */
#define CMD25		(0x40+25)	/* WRITE_MULTIPLE_BLOCK */
#define CMD55		(0x40+55)	/* APP_CMD */
#define CMD58		(0x40+58)	/* READ_OCR */

/* MMC card type flags (MMC_GET_TYPE) */
#define CT_MMC		0x01            /* MMC ver 3 */
#define CT_SD1		0x02            /* SD ver 1 */
#define CT_SD2		0x04            /* SD ver 2 */
#define CT_SDC		(CT_SD1|CT_SD2) /* SD */
#define CT_BLOCK	0x08            /* Block addressing */

#define SELECT()			(GPIO_SD_CS->BSRR = (uint32_t)GPIO_SD_CS_PIN << 16)
#define DESELECT()			(GPIO_SD_CS->BSRR = GPIO_SD_CS_PIN)

//скорость интерфейса SPI
//speed от 0 (FCLK/2) до 7 (FCLK/256)
static void set_sd_interface_speed(uint8_t speed)
{
	if (speed > 7) speed = 7;
	SPI_SD->CR1 &= ~SPI_CR1_SPE; //SPI отключено
	SPI_SD->CR1 &= ~(0x07UL<<(3U)); //маска скорости
	SPI_SD->CR1 |= (uint32_t)(speed<<(3U));
	SPI_SD->CR1 |= SPI_CR1_SPE; // SPI включено
}

static BYTE spi_rw(BYTE wval)
{
	SPI_SD->DR = wval;
	while(!(SPI_SD->SR & SPI_SR_RXNE));
	return SPI_SD->DR;
}

#define xmit_spi(dat)		 spi_rw(dat)
#define rcvr_spi()			 spi_rw(0xFF)
#define rcvr_spi_m(dst)  *(dst)=spi_rw(0xFF)

#ifdef SD_USE_DMA

typedef enum {
	SPI_Transfer_Receive = 0,
	SPI_Transfer_Transmit
} SPI_TransferType;

//SPI DMA прием/передача полный дуплекс мастер для STM32F4 - без внешних прерываний по событиям
//(обработка флагов внутри)
static void SPI_DMA_TransmitReceive(SPI_TransferType type, const BYTE* buff, uint16_t buff_size)
{
	uint8_t tmp = 0xFF;
	uint8_t shift[8] = {0, 6, 16, 22, 0, 6, 16, 22}; //битовое смещение во флаговых регистрах IFCR (L и H) и ISR(L и H)
	//определяем адреса регистров IFCR и ISR контроллеров DMA (у STM32F4 два контроллера DMA: DMA1 и DMA2)
	volatile uint32_t *ifcr_rx = (SD_DMA_STREAM_RX > 3) ? &(SD_DMA_CNTR_RX->HIFCR) : &(SD_DMA_CNTR_RX->LIFCR);
	volatile uint32_t *ifcr_tx = (SD_DMA_STREAM_TX > 3) ? &(SD_DMA_CNTR_TX->HIFCR) : &(SD_DMA_CNTR_TX->LIFCR);
	volatile uint32_t *isr_rx =  (SD_DMA_STREAM_RX > 3) ? &(SD_DMA_CNTR_RX->HISR) : &(SD_DMA_CNTR_RX->LISR);
	volatile uint32_t *isr_tx =  (SD_DMA_STREAM_TX > 3) ? &(SD_DMA_CNTR_TX->HISR) : &(SD_DMA_CNTR_TX->LISR);
	//определяем адреса потоков (у STM32F4 в каждом из двух контроллеров DMA по восемь потоков: stream0, ..., stream7)
	DMA_Stream_TypeDef *dma_RX = ((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)SD_DMA_CNTR_RX + STREAM_OFFSET_TAB[SD_DMA_STREAM_RX])));
	DMA_Stream_TypeDef *dma_TX = ((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)SD_DMA_CNTR_TX + STREAM_OFFSET_TAB[SD_DMA_STREAM_TX])));
	//выключаем потоки DMA (ряд параметров потоков DMA можно настроить только при отключенных потоках)
	dma_RX->CR &= ~DMA_SxCR_EN;
	dma_TX->CR &= ~DMA_SxCR_EN;
	while ((dma_RX->CR & DMA_SxCR_EN) || (dma_TX->CR & DMA_SxCR_EN)) { } //ждем отключения потоков DMA
	//сбрасываем флаги прерываний потока rx (записью 1 в бит регистра ifcr мы сбрасываем соответствующий флаг в регистре флагов isr)
	*ifcr_rx = 0x3F<<shift[SD_DMA_STREAM_RX]; //у STM32F4 регистры ifcr и isr состоят из двух слов: l и h (по 32 бита на слово)
											  //ifcr доступен только для записи, а isr только для чтения,
											  //причем, установка в 0 конкретного бита ifcr игнорируется.
	//сбрасываем флаги прерываний потока tx
	*ifcr_tx = 0x3F<<shift[SD_DMA_STREAM_TX];
	//разрешаем spi отправлять запросы к DMA
	SPI_SD->CR2 |= (SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN); //установкой/сбросом этих битов контрольного регистра, мы разрешаем/запрещаем spi управлять dma
														//RXDMAEN - разрешает/запрещает запросы к DMA на прием данных
														//TXDMAEN - разрешает/запрещает запросы к DMA на передачу данных
	//запрещаем прерывания по всем событиям потока приема rx и запрещаем режим двойного буфера
	dma_RX->CR &= ~(DMA_SxCR_DMEIE | DMA_SxCR_HTIE | DMA_SxCR_TEIE | DMA_SxCR_TCIE | DMA_SxCR_DBM); //сброс/установка соответствующего бита
	dma_RX->FCR &= ~DMA_SxFCR_FEIE;																	//запрещает/разрешает прерывания по
																									//событию, определяемому назначением этого бита:
																									//DME - прерывание по ошибке DMA (прямого режима)
																									//HT - прерывание по половине передачи
																									//TE - прерывание по ошибке передачи
																									//TC - прерывание по окончанию передачи
																									//FE - прерывание по ошибке стека FIFO (обработка очереди данных)
																									//при соответствующем событии выставятся флаги в isr,
																									//независимо от того, разрешено или запрещено прерывание по
																									//данному событию
	//запрещаем прерывания по всем событиям потока передачи tx и запрещаем режим двойного буфера
	dma_TX->CR &= ~(DMA_SxCR_DMEIE | DMA_SxCR_HTIE | DMA_SxCR_TEIE | DMA_SxCR_TCIE | DMA_SxCR_DBM);
	dma_TX->FCR &= ~DMA_SxFCR_FEIE;
	//включаем SPI
	SPI_SD->CR1 |= SPI_CR1_SPE;
	//настраиваем потоки DMA для приема и передачи данных с карты/на карту
	if(type == SPI_Transfer_Receive) //прием
	{
		dma_RX->CR &= ~DMA_SxCR_PINC; //инкремент адреса периферии приемника отключен
		dma_RX->CR |= DMA_SxCR_MINC;  //инкремент адреса памяти приемника включен
		dma_TX->CR &= ~(DMA_SxCR_PINC | DMA_SxCR_MINC); //инкремент адресов периферии и памяти источника отключен
		dma_TX->M0AR = (uint32_t)&tmp; //адрес в памяти с данными для исходящего потока
		dma_RX->M0AR = (uint32_t)buff; //адрес в памяти для данных входящего потока
	}
	else if (type == SPI_Transfer_Transmit) //передача
	{
		dma_RX->CR &= ~(DMA_SxCR_PINC | DMA_SxCR_MINC); //инкремент адресов периферии и памяти приемника отключен
		dma_TX->CR &= ~DMA_SxCR_PINC; //инкремент адреса периферии источника отключен
		dma_TX->CR |= DMA_SxCR_MINC;  //инкремент адреса памяти источника включен
		dma_TX->M0AR = (uint32_t)buff; //адрес в памяти с данными для исходящего потока
		dma_RX->M0AR = (uint32_t)&tmp; //адрес в памяти для данных входящего потока
	}
	dma_RX->PAR = dma_TX->PAR = (uint32_t)(&SPI_SD->DR); //приемник/источник периферия - адрес регистра DR spi
	dma_RX->NDTR = dma_TX->NDTR = (uint32_t)buff_size; //размер принимаемых/передаваемых данных
	dma_RX->CR |= (DMA_SxCR_EN); //включение потока приема
	dma_TX->CR |= (DMA_SxCR_EN); //включение потока передачи
	//ждем, пока не установятся флаги окончания приема/передачи (TCIFx, x - номер потока)
	while (!(((*isr_tx) & (DMA_LISR_TCIF0<<shift[SD_DMA_STREAM_TX])) && ((*isr_rx) & (DMA_LISR_TCIF0<<shift[SD_DMA_STREAM_RX])))) {  }
	//выключаем потоки DMA
	dma_TX->CR &= ~DMA_SxCR_EN;
	dma_RX->CR &= ~DMA_SxCR_EN;
	while ((dma_TX->CR & DMA_SxCR_EN) || (dma_RX->CR & DMA_SxCR_EN)) {  } //ждем отключения потоков DMA
	//запрещаем SPI отправлять запросы к DMA
	SPI_SD->CR2 &= ~(SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);
}

#else
static void spi_r_multi(BYTE *rval, uint16_t cnt)
{
	uint8_t* pRxData = (uint8_t*)rval;
	uint16_t txCnt = cnt;

	while(txCnt > 0)
	{
		SPI_SD->DR = 0xFF;
		txCnt--;
		while(!(SPI_SD->SR & SPI_SR_RXNE));
		*pRxData = SPI_SD->DR;
		pRxData++;
	}
}
#endif // SD_USE_DMA

/* SD card definitions */
static volatile DSTATUS Stat = STA_NOINIT; //	Disk status
static BYTE CardType;	 // Card type flags

static BYTE wait_ready(void)
{
	BYTE res;
	rcvr_spi();
	uint32_t tick = millis;
	do {
		res = rcvr_spi();
		if (millis - tick > SD_TIMEOUT) { //Проверка на TIMEOUT
			res = 0;
			break;
		}
	} while (res != 0xFF);
	return res;
}

static BYTE send_cmd(BYTE cmd, LBA_t arg)
{
	BYTE res;

	if(cmd & 0x80){	// ACMD<n> is the command sequence of CMD55-CMD<n>
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if(res > 1) return res;
	}

	DESELECT();
	SELECT();
	if(wait_ready() != 0xFF){
		return 0xFF;
	}

	// transmit cmd + arg + crc
	xmit_spi(cmd);					// Start + Command index
	int b_sector = 8 * (sizeof(LBA_t) - 1);
	for ( ; b_sector >= 0; b_sector -= 8) {
		xmit_spi((BYTE)(arg >> b_sector));
	}
	BYTE crc = 0x01; // Dummy CRC + Stop
	if(cmd == CMD0) crc = 0x95;			// Valid CRC for CMD0(0)
	if(cmd == CMD8) crc = 0x87;			// Valid CRC for CMD8(0x1AA)
	xmit_spi(crc);

	// Receive command response
	if(cmd == CMD12) rcvr_spi();		// Skip a stuff byte when stop reading

	// Wait for a valid response in timeout of 10 attempts
	BYTE attempt = 10;
	do {
		res = rcvr_spi();
	} while ((res & 0x80) && (--attempt));

	return res;
}

DSTATUS USER_SPI_initialize(BYTE drv)
{
	BYTE n, cmd, ty = 0, ocr[4];
	DESELECT();
	set_sd_interface_speed(SD_INI_SPEED); //медленное spi
	SELECT();

	for(n=10; n; n--) rcvr_spi();	// 80 dummy clocks

	if(0x01 == send_cmd(CMD0, 0))
	{
		// Enter Idle state
		if(0x01 == send_cmd(CMD8, 0x1AA))
		{
			// SDHC ?
			for(n=0; n<4; n++) ocr[n] = rcvr_spi(); // Get trailing return value of R7 response

			if((0x01 == ocr[2]) && (0xAA == ocr[3]))
			{
				// The card can work at VDD range of 2.7-3.6V
				while(send_cmd(ACMD41, (1UL << 30)));	// Wait for leaving idle state (ACMD41 with HCS bit)

				if(0x00 == send_cmd(CMD58, 0))
				{
					// Check CCS bit in the OCR
					for(n=0; n<4; n++) ocr[n] = rcvr_spi();
					ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
				}
			}
		}
		else
		{
			// SDSC or MMC
			if(send_cmd(ACMD41, 0) <= 0x01)
			{
				// SDSC
				ty = CT_SD1;
				cmd = ACMD41;
			}
			else
			{
				// MMC
				ty = CT_MMC;
				cmd = CMD1;
			}

			while(send_cmd(cmd, 0)); // Wait for leaving idle state

			if(send_cmd(CMD16, 512) != 0)	// Set R/W block length to 512
				ty = 0;
		}
	}
	CardType = ty;
	DESELECT();

	if(ty)
	{
		// Initialization succeeded
		Stat &= ~STA_NOINIT; // Clear STA_NOINIT
		set_sd_interface_speed(SD_MAX_SPEED); // быстрое SPI
	}
	else
	{
		// Initialization failed
		if(!(Stat & STA_NOINIT))
		{
			SELECT();
			wait_ready();
			DESELECT();
		}

		Stat |= STA_NOINIT;
	}

	return Stat;
}

DSTATUS USER_SPI_status(BYTE drv)
{
	return Stat;
}

// NOTE: btr must be multiple of 4
static uint8_t rcvr_datablock(BYTE *buff, UINT btr)
{
	BYTE token;
	uint32_t tick = millis;
	do {
		token = rcvr_spi();
		if (millis - tick > SD_TIMEOUT) { //Проверка на TIMEOUT
			token = 0;
			break;
		}
	} while (token == 0xFF); // wait for data packet in timeout of 100ms
	if(token != 0xFE) return 0;	// not a valid data token

#ifdef SD_USE_DMA
	SPI_DMA_TransmitReceive(SPI_Transfer_Receive, buff, btr);
#else
	if(btr == 512)
	{
		spi_r_multi(buff, btr);
	}
	else if(btr == 16)
	{
		spi_r_multi(buff, btr);
	}
	else
	{
		do {
			rcvr_spi_m(buff++);
			rcvr_spi_m(buff++);
			rcvr_spi_m(buff++);
			rcvr_spi_m(buff++);
		} while (btr -= 4);
	}
#endif

	rcvr_spi(); // discard CRC
	rcvr_spi();
	return 1;
}

DRESULT USER_SPI_read(BYTE drv, BYTE *buff, LBA_t sector, UINT count)
{
	if(!count) return RES_PARERR;
	if(Stat & STA_NOINIT) return RES_NOTRDY;
	if(!(CardType & CT_BLOCK)) sector *= 512;	// Convert to byte address if needed

	if(1 == count)
	{
		// CMD17 - READ_SINGLE_BLOCK
		if(0x00 == send_cmd(CMD17, sector))
		{
			if(rcvr_datablock(buff, 512)) count = 0;
		}
	}
	else
	{
		// CMD18 - READ_MULTIPLE_BLOCK
		if(0x00 == send_cmd(CMD18, sector))
		{
			do {
				if(!rcvr_datablock(buff, 512)) break;
				buff += 512;
			} while (--count);
			send_cmd(CMD12, 0); // CMD12 - STOP_TRANSMISSION
		}
	}
	DESELECT();

	return count ? RES_ERROR : RES_OK;
}

#if _FS_READONLY == 0
// token - Data/Stop token
static uint8_t xmit_datablock(const BYTE *buff, BYTE token)
{
	BYTE resp;
#ifndef SD_USE_DMA
	BYTE wc;
#endif

	if(0xFF != wait_ready()) return 0;
	xmit_spi(token); // transmit Data token

	if(0xFD != token) // Is data token
	{
		// transmit the 512 byte data block to MMC
#ifdef SD_USE_DMA
		SPI_DMA_TransmitReceive(SPI_Transfer_Transmit, buff, 512);
#else
		wc = 0;
		do {
			xmit_spi(*buff++);
			xmit_spi(*buff++);
		} while (--wc);
#endif

		// CRC (Dummy)
		xmit_spi(0xFF);
		xmit_spi(0xFF);

		// Receive data response
		resp = rcvr_spi();
		if((resp & 0x1F) != 0x05) // If not accepted, return with error
			return 0;
	}

	return 1;
}

DRESULT USER_SPI_write(BYTE drv, const BYTE *buff, LBA_t sector, UINT count)
{
	if(!count) return RES_PARERR;
	if(Stat & STA_NOINIT) return RES_NOTRDY;
	if(Stat & STA_PROTECT) return RES_WRPRT;
	if(!(CardType & CT_BLOCK)) sector *= 512;	// Convert to byte address if needed

	if(1 == count)
	{
		// CMD24 - WRITE_BLOCK
		if((0x00 == send_cmd(CMD24, sector)) && xmit_datablock(buff, 0xFE)) count = 0;
	}
	else
	{
		// CMD25 - WRITE_MULTIPLE_BLOCK
		if(CardType & CT_SDC) send_cmd(ACMD23, count);
		if(0x00 == send_cmd(CMD25, sector))
		{
			do {
				if(!xmit_datablock(buff, 0xFC)) break;
				buff += 512;
			} while (--count);

			if(!xmit_datablock(0, 0xFD)) // STOP_TRAN token
				count = 1;
		}
	}
	DESELECT();

	return count ? RES_ERROR : RES_OK;
}
#endif /* _READONLY == 0 */

DRESULT USER_SPI_ioctl(BYTE drv, BYTE ctrl,	void *buff)
{
	DRESULT res = RES_ERROR;
	BYTE n, csd[16], *ptr = buff;
	WORD csize;

	if(Stat & STA_NOINIT) return RES_NOTRDY;

	switch(ctrl)
	{
		case CTRL_SYNC:
			// Make sure that no pending write process
			if(0xFF == wait_ready()) res = RES_OK;
		break;

		case GET_SECTOR_COUNT:
			// Get number of sectors on the disk (DWORD)
			if((0x00 == send_cmd(CMD9, 0)) && rcvr_datablock(csd, 16))
			{
				if((csd[0] >> 6) == 1)
				{
					// SDC version 2.00
					csize = csd[9] + ((WORD)csd[8] << 8) + 1;
					*(DWORD*)buff = (DWORD)csize << 10;
				}
				else
				{
					// SDC version 1.XX or MMC
					n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
					csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
					*(DWORD*)buff = (DWORD)csize << (n - 9);
				}
				res = RES_OK;
			}
		break;

		case GET_SECTOR_SIZE:
			// Get R/W sector size (WORD)
			*(WORD*)buff = 512;
			res = RES_OK;
		break;

		case GET_BLOCK_SIZE:
			// Get erase block size in unit of sector (DWORD)
			if(CardType & CT_SD2)
			{
				// SDC version 2.00
				if(0x00 == send_cmd(ACMD13, 0))
				{
					// Read SD status
					rcvr_spi();
					if(rcvr_datablock(csd, 16))
					{
						// Read partial block
						for (n = 64 - 16; n; n--) rcvr_spi();	// Purge trailing data
						*(DWORD*)buff = 16UL << (csd[10] >> 4);
						res = RES_OK;
					}
				}
			}
			else
			{
				// SDC version 1.XX or MMC
				if((0x00 == send_cmd(CMD9, 0)) && rcvr_datablock(csd, 16))
				{
					// Read CSD
					if(CardType & CT_SD1)
					{
						// SDC version 1.XX
						*(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
					}
					else
					{
						// MMC
						*(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
					}
					res = RES_OK;
				}
			}
		break;

		case MMC_GET_TYPE:
			// Get card type flags (1 byte)
			*ptr = CardType;
			res = RES_OK;
		break;

		case MMC_GET_CSD:
			// Receive CSD (CMD9) as a data block (16 bytes)
			if((0x00 == send_cmd(CMD9, 0)) && rcvr_datablock(ptr, 16)) res = RES_OK;
		break;

		case MMC_GET_CID:
			// Receive CID (CMD10) as a data block (16 bytes)
			if((0x00 == send_cmd(CMD10, 0)) && rcvr_datablock(ptr, 16))	res = RES_OK;
		break;

		case MMC_GET_OCR:
			// Receive OCR (CMD58) as an R3 resp (4 bytes)
			if(0x00 == send_cmd(CMD58, 0))
			{
				for(n=4; n; n--) *ptr++ = rcvr_spi();
				res = RES_OK;
			}
		break;

		case MMC_GET_SDSTAT:
			// Receive SD status (ACMD13) as a data block (64 bytes)
			if(0x00 == send_cmd(ACMD13, 0))
			{
				rcvr_spi();
				if(rcvr_datablock(ptr, 64)) res = RES_OK;
			}
		break;

		default:
			res = RES_PARERR;
	}

	DESELECT();
	return res;
}
