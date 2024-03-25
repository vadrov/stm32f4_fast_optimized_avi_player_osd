Copyright (C)2019-2023, VadRov, all right reserved / www.youtube.com/@VadRov / www.dzen.ru/vadrov
# STM32F4 видеоплеер для проигрывания AVI-файлов (файл с расширением avi, контейнер riff)
Допускается свободное распространение. При любом способе распространения указание автора ОБЯЗАТЕЛЬНО. В случае внесения изменений и распространения модификаций указание первоначального автора ОБЯЗАТЕЛЬНО. Распространяется по типу "как есть", то есть использование осуществляется на свой страх и риск. Автор не предоставляет никаких гарантий.
## Компоненты:
- Отладочная плата (МК) STM32F401x;
- SPI дисплей на контроллере ILI9341 или ST7789 разрешением 240x240 или 320x240;
- модуль SD карты;
- аудио-ЦАП PCM5102;
- инкрементарный механический энкодер;
- конденсатор керамический 0.01-0.1 мкФ - 2 шт.
## В файле display_config.h (папка Display) определяется разрешение применяемого дисплея (320x240 или 240x240), а также контроллер дисплея: ST7789 либо ILI9341. Ненужные строки конфигурации комментируются, а остаются только те, которые соответствуют фактически применяемому дисплейному модулю. По умолчанию проект настроен на использование дисплейного модуля разрешением 320x240 пикселей на контроллере ILI9341.
## Подключение дисплейного модуля (на базе контроллера ILI9341):
![image](https://user-images.githubusercontent.com/111627147/233799705-78c288c1-d07f-414f-806e-08f1d835d297.png)
## Подключение модуля sd-карты:
![image](https://user-images.githubusercontent.com/111627147/233799770-7a0b10fd-66d6-4256-beb9-91e4e590242c.png)
## Подключение модуля аудио-ЦАП PCM5102:
![image](https://user-images.githubusercontent.com/111627147/233799861-5a9cf7d4-9ae6-48cb-aef2-774aa1f30381.png)
## Подключение инкрементарного механического энкодера:
![image](https://user-images.githubusercontent.com/111627147/233801356-1e604c75-00a0-4896-a546-d7579f2f6b5b.png)
## Видео о проекте видеоплеера:
[![Watch the video](https://img.youtube.com/vi/sC5nm7vdN6k/maxresdefault.jpg)](https://youtu.be/sC5nm7vdN6k)
## Требования к формату AVI:
- поток видео mjpeg (motion jpeg), т.е. видеокадры должны быть закодированы jpeg кодеком;
- поток аудио mp3, т.е. звук должен быть закодирован mp3 кодеком.\
Для преобразования видеофайла к поддерживаемому плеером формату рекомендуется использовать библиотеку ffmpeg. Например, следующая командная строка преобразует mp4-видео в поддерживаемый плеером формат avi с размером кадра 320х240, частотой 25 кадров в секунду, с качеством кодирования 15:\
      **ffmpeg -i video.mp4 -c:a mp3 -c:v mjpeg -s 320x240 -r 25 -q 15 video.avi**\
где:
- -i video.mp4 - указание на файл-источник, который вы хотите преобразовать в поддерживаемый плееером формат avi;
- video.avi - выходной файл с расширением avi, в котором будет сохранен результат преобразования;
- -c:a mp3 - указание на то, что "звуковая дорожка" (аудиопоток) в выходном файле должна быть в формате кодека mp3;
- -c:v mjpeg - указание на то, что "видео дорожка" (видеопоток) в выходном файле должна быть в формате кодека mjpeg (набор картинок, закодированных кодеком jpeg, т.н., motion jpeg);
-	-s 320x240 - указание на размер видеокадра в выходном файле avi (ширина, высота);
- -r 25 - указание на частоту видеокадров в выходном файле avi (кадров в секунду);
-	-q 15 - указание на качество кодирования файла avi (от 1 (наилучшее) до 31 (наихудшее)).
Качество звука можно задать ключом -b, означающим bitrate (скорость потока в бит/с). Например, такой ключ, как,  -b:a 128k, задаст для звукового потока скорость в 128 кбит/с, что считается для кодека mp3 начальным уровнем при кодировании музыкальных данных (32-96 кбит/с достаточно для кодирования речи, а от 96 кбит/с и более - музыки низкого качества). Потока в 256 кбит/с достаточно для высокого качества музыки при кодировании mp3. 320 кбит/с - предел для кодека mp3.\
При преобразовании видео учитывайте соотношение сторон кадра исходного видео. Например, если исходное видео имело соотношение сторон кадра 16:9, то при использовании дисплея с шириной 320 пикселей, высота выходного кадра должна быть 180 пикселей. Т.е. параметр s для приведенного примера должен быть задан в виде 320х180. Если соотношение сторон исходного кадра 4:3, то при использовании дисплея с шириной 320 пикселей высота выходного кадра должна быть 240 пикселей. Т.е. параметр s для этого примера должен быть задан в виде 320х240. Параметр q определяет качество кодирования и может принимать значения от 1 до 31. С увеличением q качество снижается, а с уменьшением - увеличивается. Этот параметр, естественно, влияет на производительность программного декодера jpeg. Золотая середина этого параметра для малопроизводительного микроконтроллера 15-20. При значении 15 для stm32f401ccu6, работающем на частоте 84 МГц, декодер jpeg для кадра размером 320х240 показывает
производительность от 11 до 25 кадров в секунду (40 - 90 мс на декодирование одного кадра изображения), одновременно декодируя mp3 поток.\
Плеер обеспечивает неплохую синхронизацию аудио и видеопотоков. Причем, подсчет времени воспроизведения, а, соответственно, и синхронизация аудио с видео осуществляются по прерываниям потока DMA, обслуживающего DAC. В прерывании потока DMA, обслуживающего DAC, увеличивается счетчик длительности воспроизведения, рассчитываемой по аудиокадрам, на величину длительности воспроизведения звукового кадра, определяемую по формуле:\
      **d = 1000000 * samples / samplerate**\
где samples - количество сэмплов в кадре, samplerate - частота дискретизации/сэмплирования(Гц), d - длительность воспроизведения звукового кадра (мкс).\
Таким образом, при воспроизведении нет привязки к реальному времени. Ход внутреннего времени плеера	задает тактирование модуля I2S. Расхождение от реального времени определяется разницей между заданной и реальной частотой дискретизации/сэмплирования. Так, для частоты дискретизации 44100 Гц реальная	частота дискретизации составляет 44100.46875 Гц при 16-битном формате данных и 44099.50781 Гц	при 32-битном формате данных в случае использования кварцевого резонатора на 25 Мгц и встроенного модуля I2S PLL stm32f401ccu6. Т.е. в обоих случаях ошибка не превышает 0.0011% (см. таблицу 91, RM0368 REV 5).	Синхронизация аудио и видео осуществляется в пределах времени отображения одного видеокадра. Это время	определяется из соответствующего поля avi файла (dwMicroSecPerFrame, время отображения кадра в мкс).	С каждым новым видеокадром (не важно при этом пропущен он или показан) общее время воспроизведения,	рассчитываемое по видеокадрам, увеличивается на указанную величину. В свою очередь, с каждым	воспроизведенным аудиокадром увеличивается на величину d общее время воспроизведения, рассчитываемое по аудиокадрам (в прерывании потока DMA, обслуживающего DAC). По разнице между этими значениями времени	плеер принимает одно из трех решений:
- декодировать видеокадр, если разница времени не превышает длительности отображения одного видеокадра;
- пропустить декодирование видеокадра (переход на следующий видеокадр с инкрементом счетчика воспроизведения для видео), если время, рассчитываемое по видеокадрам, отстает от времени, рассчитанного по воспроизведенным аудиокадрам;
- организовать задержку, равную разнице времени (после чего перейти к декодированию кадра), если время, расчитанное по видеокадрам, превышает время, рассчитанное по аудиокадрам.\
Демонстрируемый проект использует следующие драйвера, модули, библиотеки ...:
## Для декодирования jpeg изображений используется модифицированная и оптимизированная для stm32 библиотека TJpgDec - Tiny JPEG Decompressor R0.03 (папка JPEG)
Portions copyright (C) 2021, ChaN,   all right reserved.\
Portions сopyright (C) 2022-2023, VadRov, all right reserved.
## Для работы с SD картой используется драйвер SD карты (папка FATFS)
Portions copyright (C) 2014, ChaN, all rights reserved.\
Portions copyright (C) 2017, kiwih, all rights reserved.\
Portions сopyright (C) 2019, VadRov, all right reserved.
## Для работы с файловой системой используется библиотека FatFs Generic FAT Filesystem Module  R0.15 w/patch1 (папка FATFS)
Copyright (C) 2022, ChaN, all right reserved.
## Драйвер DAC на базе PCM5102 (папка PCM5102)
Copyright (C) 2019, VadRov, all right reserved.
## Драйвер дисплея V1.4 с вариантом "ASM-турбо" (папка Display)
Copyright (C) 2019-2023 VadRov, all right reserved.
## Fixed-point MP3 decoder (папка MP3Helix)
Portions Copyright (c) 1995-2002 RealNetworks, Inc. All Rights Reserved.
## Файловый менеджер (папка FileManager)
Copyright (C) 2022, VadRov, all right reserved.
## Модуль для работы с кнопоками (папка Keyboard)
Copyright (C) 2021, VadRov, all right reserved.
## Графическая библиотека для работы с объектами-примитивами в двумерном пространстве microGL2D (папка MicroGL2D)
Copyright (C) 2022, VadRov, all right reserved.
## Драйвер энкодера для STM32F4 (папка Encoder)
Copyright (C) 2019, VadRov, all right reserved.
## Процедуры работы со строками (папка MyString)
Copyright (C) 2019, VadRov, all right reserved.

Автор проекта: **VadRov**\
Контакты: [Youtube](https://www.youtube.com/@VadRov) [Дзен](https://dzen.ru/vadrov) [VK](https://vk.com/vadrov) [Telegram](https://t.me/vadrov_channel)\
Поддержать автора: [donate.qiwi](https://donate.qiwi.com/payin/VadRov)  [donate.yoomoney](https://yoomoney.ru/to/4100117522443917)
