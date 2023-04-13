/*
 *  Библиотека работы с аудио-форматами (кодеки)
 *
 *  Author: VadRov
 *  Copyright (C) 2019, VadRov, all right reserved.
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
#ifndef RIFF_READ_HEADER_H_
#define RIFF_READ_HEADER_H_

#include "ff.h"
#include "main.h"
#include "player.h"

#define CHUNK_NAME(c1,c2,c3,c4)  (((uint32_t)c4<<24)+((uint32_t)c3<<16)+((uint16_t)c2<<8)+(uint8_t)c1)

#define chunk_RIFF		CHUNK_NAME('R','I','F','F')
#define chunk_fmt		CHUNK_NAME('f','m','t',' ')
#define chunk_data		CHUNK_NAME('d','a','t','a')
#define chunk_DISP		CHUNK_NAME('D','I','S','P')
#define chunk_LIST		CHUNK_NAME('L','I','S','T')
#define chunk_fact		CHUNK_NAME('f','a','c','t')
#define chunk_PEAK		CHUNK_NAME('P','E','A','K')
#define chunk_INFO		CHUNK_NAME('I','N','F','O')
#define chunk_id3		CHUNK_NAME('i','d','3',' ')

#define chunk_hdrl		CHUNK_NAME('h','d','r','l')
#define chunk_avih		CHUNK_NAME('a','v','i','h')
#define chunk_strl		CHUNK_NAME('s','t','r','l')
#define chunk_strh		CHUNK_NAME('s','t','r','h')
#define chunk_strf		CHUNK_NAME('s','t','r','f')
#define chunk_vids		CHUNK_NAME('v','i','d','s')
#define chunk_auds		CHUNK_NAME('a','u','d','s')
#define chunk_JUNK		CHUNK_NAME('J','U','N','K')
#define chunk_movi		CHUNK_NAME('m','o','v','i')
#define chunk_idx1		CHUNK_NAME('i','d','x','1')

#define TYPE_AVI  		CHUNK_NAME('A','V','I',' ')
#define TYPE_AVIX  		CHUNK_NAME('A','V','I','X')
#define TYPE_WAVE 		CHUNK_NAME('W','A','V','E')
#define TYPE_RMID 		CHUNK_NAME('R','M','I','D')
#define TYPE_RDIB 		CHUNK_NAME('R','D','I','B')
#define TYPE_RMMP 		CHUNK_NAME('R','M','M','P')
#define TYPE_PAL  		CHUNK_NAME('P','A','L',' ')

//известные аудио форматы WAV
#define WAVE_FORMAT_UNKNOWN 0x0000 /* Microsoft Corporation */
#define WAVE_FORMAT_PCM 0x0001 /* Microsoft Corporation */
#define WAVE_FORMAT_ADPCM 0x0002 /* Microsoft Corporation */
#define WAVE_FORMAT_IEEE_FLOAT 0x0003 /* Microsoft Corporation */
#define WAVE_FORMAT_VSELP 0x0004 /* Compaq Computer Corp. */
#define WAVE_FORMAT_IBM_CVSD 0x0005 /* IBM Corporation */
#define WAVE_FORMAT_ALAW 0x0006 /* Microsoft Corporation */
#define WAVE_FORMAT_MULAW 0x0007 /* Microsoft Corporation */
#define WAVE_FORMAT_DTS 0x0008 /* Microsoft Corporation */
#define WAVE_FORMAT_DRM 0x0009 /* Microsoft Corporation */
#define WAVE_FORMAT_OKI_ADPCM 0x0010 /* OKI */
#define WAVE_FORMAT_DVI_ADPCM 0x0011 /* Intel Corporation */
#define WAVE_FORMAT_IMA_ADPCM (WAVE_FORMAT_DVI_ADPCM) /* Intel Corporation */
#define WAVE_FORMAT_MEDIASPACE_ADPCM 0x0012 /* Videologic */
#define WAVE_FORMAT_SIERRA_ADPCM 0x0013 /* Sierra Semiconductor Corp */
#define WAVE_FORMAT_G723_ADPCM 0x0014 /* Antex Electronics Corporation */
#define WAVE_FORMAT_DIGISTD 0x0015 /* DSP Solutions, Inc. */
#define WAVE_FORMAT_DIGIFIX 0x0016 /* DSP Solutions, Inc. */
#define WAVE_FORMAT_DIALOGIC_OKI_ADPCM 0x0017 /* Dialogic Corporation */
#define WAVE_FORMAT_MEDIAVISION_ADPCM 0x0018 /* Media Vision, Inc. */
#define WAVE_FORMAT_CU_CODEC 0x0019 /* Hewlett-Packard Company */
#define WAVE_FORMAT_YAMAHA_ADPCM 0x0020 /* Yamaha Corporation of America */
#define WAVE_FORMAT_SONARC 0x0021 /* Speech Compression */
#define WAVE_FORMAT_DSPGROUP_TRUESPEECH 0x0022 /* DSP Group, Inc */
#define WAVE_FORMAT_ECHOSC1 0x0023 /* Echo Speech Corporation */
#define WAVE_FORMAT_AUDIOFILE_AF36 0x0024 /* Virtual Music, Inc. */
#define WAVE_FORMAT_APTX 0x0025 /* Audio Processing Technology */
#define WAVE_FORMAT_AUDIOFILE_AF10 0x0026 /* Virtual Music, Inc. */
#define WAVE_FORMAT_PROSODY_1612 0x0027 /* Aculab plc */
#define WAVE_FORMAT_LRC 0x0028 /* Merging Technologies S.A. */
#define WAVE_FORMAT_DOLBY_AC2 0x0030 /* Dolby Laboratories */
#define WAVE_FORMAT_GSM610 0x0031 /* Microsoft Corporation */
#define WAVE_FORMAT_MSNAUDIO 0x0032 /* Microsoft Corporation */
#define WAVE_FORMAT_ANTEX_ADPCME 0x0033 /* Antex Electronics Corporation */
#define WAVE_FORMAT_CONTROL_RES_VQLPC 0x0034 /* Control Resources Limited */
#define WAVE_FORMAT_DIGIREAL 0x0035 /* DSP Solutions, Inc. */
#define WAVE_FORMAT_DIGIADPCM 0x0036 /* DSP Solutions, Inc. */
#define WAVE_FORMAT_CONTROL_RES_CR10 0x0037 /* Control Resources Limited */
#define WAVE_FORMAT_NMS_VBXADPCM 0x0038 /* Natural MicroSystems */
#define WAVE_FORMAT_CS_IMAADPCM 0x0039 /* Crystal Semiconductor IMA ADPCM */
#define WAVE_FORMAT_ECHOSC3 0x003A /* Echo Speech Corporation */
#define WAVE_FORMAT_ROCKWELL_ADPCM 0x003B /* Rockwell International */
#define WAVE_FORMAT_ROCKWELL_DIGITALK 0x003C /* Rockwell International */
#define WAVE_FORMAT_XEBEC 0x003D /* Xebec Multimedia Solutions Limited */
#define WAVE_FORMAT_G721_ADPCM 0x0040 /* Antex Electronics Corporation */
#define WAVE_FORMAT_G728_CELP 0x0041 /* Antex Electronics Corporation */
#define WAVE_FORMAT_MSG723 0x0042 /* Microsoft Corporation */
#define WAVE_FORMAT_MPEG 0x0050 /* Microsoft Corporation */
#define WAVE_FORMAT_RT24 0x0052 /* InSoft, Inc. */
#define WAVE_FORMAT_PAC 0x0053 /* InSoft, Inc. */
#define WAVE_FORMAT_MPEGLAYER3 0x0055 /* ISO/MPEG Layer3 Format Tag */
#define WAVE_FORMAT_LUCENT_G723 0x0059 /* Lucent Technologies */
#define WAVE_FORMAT_CIRRUS 0x0060 /* Cirrus Logic */
#define WAVE_FORMAT_ESPCM 0x0061 /* ESS Technology */
#define WAVE_FORMAT_VOXWARE 0x0062 /* Voxware Inc */
#define WAVE_FORMAT_CANOPUS_ATRAC 0x0063 /* Canopus, co., Ltd. */
#define WAVE_FORMAT_G726_ADPCM 0x0064 /* APICOM */
#define WAVE_FORMAT_G722_ADPCM 0x0065 /* APICOM */
#define WAVE_FORMAT_DSAT_DISPLAY 0x0067 /* Microsoft Corporation */
#define WAVE_FORMAT_VOXWARE_BYTE_ALIGNED 0x0069 /* Voxware Inc */
#define WAVE_FORMAT_VOXWARE_AC8 0x0070 /* Voxware Inc */
#define WAVE_FORMAT_VOXWARE_AC10 0x0071 /* Voxware Inc */
#define WAVE_FORMAT_VOXWARE_AC16 0x0072 /* Voxware Inc */
#define WAVE_FORMAT_VOXWARE_AC20 0x0073 /* Voxware Inc */
#define WAVE_FORMAT_VOXWARE_RT24 0x0074 /* Voxware Inc */
#define WAVE_FORMAT_VOXWARE_RT29 0x0075 /* Voxware Inc */
#define WAVE_FORMAT_VOXWARE_RT29HW 0x0076 /* Voxware Inc */
#define WAVE_FORMAT_VOXWARE_VR12 0x0077 /* Voxware Inc */
#define WAVE_FORMAT_VOXWARE_VR18 0x0078 /* Voxware Inc */
#define WAVE_FORMAT_VOXWARE_TQ40 0x0079 /* Voxware Inc */
#define WAVE_FORMAT_SOFTSOUND 0x0080 /* Softsound, Ltd. */
#define WAVE_FORMAT_VOXWARE_TQ60 0x0081 /* Voxware Inc */
#define WAVE_FORMAT_MSRT24 0x0082 /* Microsoft Corporation */
#define WAVE_FORMAT_G729A 0x0083 /* AT&amp;T Labs, Inc. */
#define WAVE_FORMAT_MVI_MVI2 0x0084 /* Motion Pixels */
#define WAVE_FORMAT_DF_G726 0x0085 /* DataFusion Systems (Pty) (Ltd) */
#define WAVE_FORMAT_DF_GSM610 0x0086 /* DataFusion Systems (Pty) (Ltd) */
#define WAVE_FORMAT_ISIAUDIO 0x0088 /* Iterated Systems, Inc. */
#define WAVE_FORMAT_ONLIVE 0x0089 /* OnLive! Technologies, Inc. */
#define WAVE_FORMAT_SBC24 0x0091 /* Siemens Business Communications Sys */
#define WAVE_FORMAT_DOLBY_AC3_SPDIF 0x0092 /* Sonic Foundry */
#define WAVE_FORMAT_MEDIASONIC_G723 0x0093 /* MediaSonic */
#define WAVE_FORMAT_PROSODY_8KBPS 0x0094 /* Aculab plc */
#define WAVE_FORMAT_ZYXEL_ADPCM 0x0097 /* ZyXEL Communications, Inc. */
#define WAVE_FORMAT_PHILIPS_LPCBB 0x0098 /* Philips Speech Processing */
#define WAVE_FORMAT_PACKED 0x0099 /* Studer Professional Audio AG */
#define WAVE_FORMAT_MALDEN_PHONYTALK 0x00A0 /* Malden Electronics Ltd. */
#define WAVE_FORMAT_RHETOREX_ADPCM 0x0100 /* Rhetorex Inc. */
#define WAVE_FORMAT_IRAT 0x0101 /* BeCubed Software Inc. */
#define WAVE_FORMAT_VIVO_G723 0x0111 /* Vivo Software */
#define WAVE_FORMAT_VIVO_SIREN 0x0112 /* Vivo Software */
#define WAVE_FORMAT_DIGITAL_G723 0x0123 /* Digital Equipment Corporation */
#define WAVE_FORMAT_SANYO_LD_ADPCM 0x0125 /* Sanyo Electric Co., Ltd. */
#define WAVE_FORMAT_SIPROLAB_ACEPLNET 0x0130 /* Sipro Lab Telecom Inc. */
#define WAVE_FORMAT_SIPROLAB_ACELP4800 0x0131 /* Sipro Lab Telecom Inc. */
#define WAVE_FORMAT_SIPROLAB_ACELP8V3 0x0132 /* Sipro Lab Telecom Inc. */
#define WAVE_FORMAT_SIPROLAB_G729 0x0133 /* Sipro Lab Telecom Inc. */
#define WAVE_FORMAT_SIPROLAB_G729A 0x0134 /* Sipro Lab Telecom Inc. */
#define WAVE_FORMAT_SIPROLAB_KELVIN 0x0135 /* Sipro Lab Telecom Inc. */
#define WAVE_FORMAT_G726ADPCM 0x0140 /* Dictaphone Corporation */
#define WAVE_FORMAT_QUALCOMM_PUREVOICE 0x0150 /* Qualcomm, Inc. */
#define WAVE_FORMAT_QUALCOMM_HALFRATE 0x0151 /* Qualcomm, Inc. */
#define WAVE_FORMAT_TUBGSM 0x0155 /* Ring Zero Systems, Inc. */
#define WAVE_FORMAT_MSAUDIO1 0x0160 /* Microsoft Corporation */
#define WAVE_FORMAT_UNISYS_NAP_ADPCM 0x0170 /* Unisys Corp. */
#define WAVE_FORMAT_UNISYS_NAP_ULAW 0x0171 /* Unisys Corp. */
#define WAVE_FORMAT_UNISYS_NAP_ALAW 0x0172 /* Unisys Corp. */
#define WAVE_FORMAT_UNISYS_NAP_16K 0x0173 /* Unisys Corp. */
#define WAVE_FORMAT_CREATIVE_ADPCM 0x0200 /* Creative Labs, Inc */
#define WAVE_FORMAT_CREATIVE_FASTSPEECH8 0x0202 /* Creative Labs, Inc */
#define WAVE_FORMAT_CREATIVE_FASTSPEECH10 0x0203 /* Creative Labs, Inc */
#define WAVE_FORMAT_UHER_ADPCM 0x0210 /* UHER informatic GmbH */
#define WAVE_FORMAT_QUARTERDECK 0x0220 /* Quarterdeck Corporation */
#define WAVE_FORMAT_ILINK_VC 0x0230 /* I-link Worldwide */
#define WAVE_FORMAT_RAW_SPORT 0x0240 /* Aureal Semiconductor */
#define WAVE_FORMAT_ESST_AC3 0x0241 /* ESS Technology, Inc. */
#define WAVE_FORMAT_IPI_HSX 0x0250 /* Interactive Products, Inc. */
#define WAVE_FORMAT_IPI_RPELP 0x0251 /* Interactive Products, Inc. */
#define WAVE_FORMAT_CS2 0x0260 /* Consistent Software */
#define WAVE_FORMAT_SONY_SCX 0x0270 /* Sony Corp. */
#define WAVE_FORMAT_FM_TOWNS_SND 0x0300 /* Fujitsu Corp. */
#define WAVE_FORMAT_BTV_DIGITAL 0x0400 /* Brooktree Corporation */
#define WAVE_FORMAT_QDESIGN_MUSIC 0x0450 /* QDesign Corporation */
#define WAVE_FORMAT_VME_VMPCM 0x0680 /* AT&amp;T Labs, Inc. */
#define WAVE_FORMAT_TPC 0x0681 /* AT&amp;T Labs, Inc. */
#define WAVE_FORMAT_OLIGSM 0x1000 /* Ing C. Olivetti &amp; C., S.p.A. */
#define WAVE_FORMAT_OLIADPCM 0x1001 /* Ing C. Olivetti &amp; C., S.p.A. */
#define WAVE_FORMAT_OLICELP 0x1002 /* Ing C. Olivetti &amp; C., S.p.A. */
#define WAVE_FORMAT_OLISBC 0x1003 /* Ing C. Olivetti &amp; C., S.p.A. */
#define WAVE_FORMAT_OLIOPR 0x1004 /* Ing C. Olivetti &amp; C., S.p.A. */
#define WAVE_FORMAT_LH_CODEC 0x1100 /* Lernout &amp; Hauspie */
#define WAVE_FORMAT_NORRIS 0x1400 /* Norris Communications, Inc. */
#define WAVE_FORMAT_SOUNDSPACE_MUSICOMPRESS 0x1500 /* AT&amp;T Labs, Inc. */
#define WAVE_FORMAT_DVM 0x2000 /* FAST Multimedia AG */

//статусы выполнения процедуры чтения заголовка wav файла
typedef enum {
		RIFF_OK = 0,		//заголовок прочитан без ошибок
		RIFF_FMT_ERROR,   	//ошибка в формате
		RIFF_IO_ERROR,		//ошибка чтения заголовка файла
		RIFF_PCM_ERROR,		//данные закодированы кодеком (формат не PCM)
		RIFF_CNL_ERROR,		//ошибка в числе каналов воспроизведения
		RIFF_BPS_ERROR,		//ошибка в разрядности оцифровки данных
		RIFF_SR_ERROR		//ошибка в частоте дискретизации
} RIFF_HDR_STATUS;
//-----------------------------------------------------------------------------------------------------------------------------

typedef enum {
	FourccType_AVI,
	FourccType_WAVE,
	FourccType_RMID,
	FourccType_RDIB,
	FourccType_RMMP,
	FourccType_PAL,
	FourccType_Unknow
} FourccType;

typedef struct {
	uint32_t chunk_id;
	uint32_t chunk_size;
	uint32_t chunk_type;
} Riff_Chunk;


#pragma pack(push, 1)
typedef struct
{
	uint32_t ChunID; // должен быть 'avih'
	uint32_t ChunkSize; // Размер этой структуры данных, исключая первые 8 байтов (поля ID и Size)
	uint32_t dwMicroSecPerFrame; // Время, необходимое для отображения каждого кадра, мкс
	uint32_t dwMaxBytesPerSec; // Максимальная скорость передачи данных
	uint32_t dwPaddingGranularity; // Длина блока записи должна быть кратна этому значению, обычно 2048
	uint32_t dwFlages; // Специальные атрибуты файла AVI, например, содержит ли он индексные блоки и хранятся ли аудио и видео данные поперек
	uint32_t dwTotalFrame; // Общее количество кадров в файле
	uint32_t dwInitialFrames; //Cколько нужно кадров перед началом воспроизведения
	uint32_t dwStreams; // Количество потоков данных, содержащихся в файле
	uint32_t dwSuggestedBufferSize; // Рекомендуемый размер буфера
	uint32_t dwWidth; //ширина изображения
	uint32_t dwHeight; //Высота изображения
	uint32_t dwReserved[4]; // Зарезервированное значение
} MainAVIHeader;

typedef struct
{
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
} RECT;

typedef struct
{
	 uint32_t fccType; // 4 байта, указывающих тип потока данных, vids указывает на поток видео данных, а auds - на поток аудио данных
	 uint32_t fccHandler; //4 байта, которые представляют код драйвера для декомпрессии потока данных
	 uint32_t dwFlags; // Атрибуты потока данных
	 uint16_t wPriority; // Воспроизвести приоритет этого потока данных
	 uint16_t wLanguage; // Код языка аудио
	 uint32_t dwInitalFrames; //сколько нужно кадров, прежде чем начать воспроизведение
	 uint32_t dwScale; // Объем данных, размер каждого кадра видео или размер выборки аудио
	 uint32_t dwRate; // dwScale / dwRate = количество выборок в секунду
	 uint32_t dwStart; // Позиция, с которой начинает воспроизводиться поток данных, в dwScale
	 uint32_t dwLength; // Объем данных потока данных, в dwScale
	 uint32_t dwSuggestedBufferSize; // Рекомендуемый размер буфера
	 uint32_t dwQuality; // Параметр качества, чем больше значение, тем лучше качество
	 uint32_t dwSampleSize; // размер выборки аудио
	 RECT rcFrame; // Прямоугольник, занимаемый видеоизображением
} AVIStreamHeader;

typedef struct
{
	uint32_t biSize;
	int32_t biWidth;
	int32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	int32_t biXPelsPerMeter;
	int32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
} BITMAPINFOHEADER;

typedef struct
{
	uint16_t wFormatTag;
	uint16_t nChannels; // Количество каналов
	uint32_t nSamplesPerSec; // Частота выборки
	uint32_t nAvgBytesPerSec; // Количество данных в секунду в звуке WAVE
	uint16_t nBlockAlign; // Флаг выравнивания блока данных
	uint16_t biSize; // Размер этой структуры
} WAVEFORMAT;
#pragma pack(pop)

//структура содержит информацию о некоторых параметрах wav файла
typedef struct {
	FourccType mediaType;	//тип медиа данных потока, например FourccType_WAVE - звуковой файл в формате WAV

	AudioConfig audioCFG;	//параметры аудио
	uint16_t audioFormat;   //аудиоформат: 1 - PCM, если не 1, то кодирован. 3 - float (формат данных с плавающей запятой)
	AudioMeta audioMeta;

	MainAVIHeader AVIHeader;  		//хидер avi
	AVIStreamHeader *AVIStreams; 	//хидеры потоков avi (видео, аудио, текст)
	uint8_t AllStreams;				//количество потоков avi
	BITMAPINFOHEADER vidFmt;		//параметры видеопотока
	WAVEFORMAT audFmt;				//параметры аудио потока
	FSIZE_t startPosIdx1;   		//стартовая позиция индексных данных в файле
	uint32_t lenghtIdx1;    		//длина индексных данных

	FSIZE_t startPosData;   //стартовая позиция блока данных в файле
	uint32_t lenghtData;    //длина блока данных
} RiffHDR;

RIFF_HDR_STATUS Read_RIFF_Header (RiffHDR *RHdr, FIL *file);

#endif /* RIFF_READ_HEADER_H_ */
