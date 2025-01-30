Copyright (C)2019-2023, VadRov, all right reserved / www.youtube.com/@VadRov / www.dzen.ru/vadrov
# STM32F4 video player for playing AVI files (file with extension avi, container riff)
Free distribution is permitted. In any way of distribution, the author's indication is MANDATORY. In case of changes and distribution of modifications, the indication of the original author is MANDATORY. Distributed "as is", i.e. use is at your own risk. The author does not provide any guarantees.
## Components:
- Development board STM32F401x;
- SPI display on ILI9341 or ST7789 controller with a resolution of 240x240 or 320x240;
- SD card module;
- PCM5102 audio DAC;
- incremental mechanical encoder;
- ceramic capacitor 0.01-0.1 μF - 2 pcs.
## Selecting the display module controller and its resolution:
The display_config.h file (Display folder) defines the resolution of the display being used (320x240 or 240x240), as well as the display controller: ST7789 or ILI9341. Unnecessary configuration lines are commented out, and only those that correspond to the actual display module being used remain. By default, the project is configured to use a display module with a resolution of 320x240 pixels on the ILI9341 controller:
![image](https://github.com/vadrov/stm32f4_fast_optimized_avi_player_osd/assets/111627147/bc4febba-9923-42a1-be76-f511d46d08da)
## Connecting the display module (based on the ILI9341 controller):
![image](https://user-images.githubusercontent.com/111627147/233799705-78c288c1-d07f-414f-806e-08f1d835d297.png)
## Connecting the SD card module:
![image](https://user-images.githubusercontent.com/111627147/233799770-7a0b10fd-66d6-4256-beb9-91e4e590242c.png)
## Connecting the PCM5102 audio DAC module:
![image](https://user-images.githubusercontent.com/111627147/233799861-5a9cf7d4-9ae6-48cb-aef2-774aa1f30381.png)
## Connecting an incremental mechanical encoder:
![image](https://user-images.githubusercontent.com/111627147/233801356-1e604c75-00a0-4896-a546-d7579f2f6b5b.png)
## Video about the video player project:
[![Watch the video](https://img.youtube.com/vi/sC5nm7vdN6k/maxresdefault.jpg)](https://youtu.be/sC5nm7vdN6k)
## Requirements for the AVI format:
- mjpeg (motion jpeg) video stream, i.e. video frames must be encoded with a jpeg codec;
- mp3 audio stream, i.e. sound must be encoded with an mp3 codec.\
To convert a video file to a format supported by the player, it is recommended to use the ffmpeg library. For example, the following command line converts mp4 video to a supported avi format with a frame size of 320x240, a frequency of 25 frames per second, and an encoding quality of 15:\
**ffmpeg -i video.mp4 -c:a mp3 -c:v mjpeg -s 320x240 -r 25 -q 15 video.avi**\
where:
- -i video.mp4 - specifies the source file you want to convert to the avi format supported by the player;
- video.avi - the output file with the avi extension, in which the conversion result will be saved;
- -c:a mp3 - specifies that the "soundtrack" (audio stream) in the output file must be in the mp3 codec format;
- -c:v mjpeg - specifies that the "video track" (video stream) in the output file must be in the mjpeg codec format (a set of images encoded with the jpeg codec, the so-called motion jpeg);
- -s 320x240 - specifies the video frame size in the output avi file (width, height);
- -r 25 - specifies the video frame rate in the output avi file (frames per second);
- -q 15 - specifies the quality of avi file encoding (from 1 (best) to 31 (worst)).
Audio quality can be set with the -b switch, meaning bitrate (stream speed in bits/sec). For example, a switch such as -b:a 128k will set the audio stream speed to 128 kbps, which is considered the entry level for the mp3 codec when encoding music data (32-96 kbps is enough for speech encoding, and 96 kbps and more are low-quality music). A stream of 256 kbps is enough for high-quality music when encoding mp3. 320 kbps is the limit for the mp3 codec.\
When converting video, take into account the aspect ratio of the source video. For example, if the source video had an aspect ratio of 16:9, then when using a display with a width of 320 pixels, the height of the output frame should be 180 pixels. That is, The s parameter for the given example should be set as 320x180. If the aspect ratio of the original frame is 4:3, then when using a display with a width of 320 pixels, the height of the output frame should be 240 pixels. That is, the s parameter for this example should be set as 320x240. The q parameter determines the encoding quality and can take values ​​from 1 to 31. As q increases, the quality decreases, and as q decreases, it increases. This parameter naturally affects the performance of the jpeg software decoder. The golden mean of this parameter for a low-performance microcontroller is 15-20. At a value of 15 for stm32f401ccu6, operating at a frequency of 84 MHz, the jpeg decoder for a frame of 320x240 size shows performance from 11 to 25 frames per second (40 - 90 ms for decoding one frame of the image), simultaneously decoding the mp3 stream. \
The player provides good synchronization of audio and video streams. Moreover, the calculation of the playback time, and, accordingly, the synchronization of audio with video are carried out by interruptions of the DMA stream servicing the DAC. When interrupting the DMA stream servicing the DAC, the playback duration counter calculated by audio frames is increased by the playback duration of the audio frame, determined by the formula:
**d = 1000000 * samples / samplerate**
where samples is the number of samples in a frame, samplerate is the sampling/sampling frequency (Hz), d is the playback duration of the audio frame (μs).\
Thus, there is no reference to real time during playback. The flow of the player's internal time is determined by the clocking of the I2S module. The deviation from real time is determined by the difference between the specified and real sampling/sampling frequency. Thus, for a sampling frequency of 44100 Hz, the actual sampling frequency is 44100.46875 Hz for a 16-bit data format and 44099.50781 Hz for a 32-bit data format in the case of using a 25 MHz quartz resonator and an integrated I2S PLL stm32f401ccu6 module. That is, in both cases the error does not exceed 0.0011% (see Table 91, RM0368 REV 5). Audio and video synchronization is performed within the display time of one video frame. This time is determined from the corresponding field of the avi file (dwMicroSecPerFrame, frame display time in μs). With each new video frame (it does not matter whether it is skipped or shown), the total playback time calculated by video frames increases by the specified value. In turn, with each played audio frame, the total playback time calculated by audio frames (in the interrupt of the DMA stream servicing the DAC) increases by d. Based on the difference between these time values, the player makes one of three decisions:
- decode the video frame if the time difference does not exceed the duration of displaying one video frame;
- skip decoding the video frame (go to the next video frame with an increment of the playback counter for the video), if the time calculated by video frames lags behind the time calculated by the played audio frames;
- organize a delay equal to the time difference (and then go to decoding the frame), if the time calculated by video frames exceeds the time calculated by audio frames.\
The demonstrated project uses the following drivers, modules, libraries ...:
## For decoding jpeg images, the modified and optimized for stm32 library TJpgDec is used - Tiny JPEG Decompressor R0.03 (folder JPEG)
Portions copyright (C) 2021, ChaN,   all right reserved.\
Portions сopyright (C) 2022-2023, VadRov, all right reserved.
## To work with the SD card, use the SD card driver (FATFS folder)
Portions copyright (C) 2014, ChaN, all rights reserved.\
Portions copyright (C) 2017, kiwih, all rights reserved.\
Portions сopyright (C) 2019, VadRov, all right reserved.
## To work with the file system, the library FatFs Generic FAT Filesystem Module R0.15 w/patch1 (FATFS folder) is used
Copyright (C) 2022, ChaN, all right reserved.
## PCM5102 based DAC driver (PCM5102 folder)
Copyright (C) 2019, VadRov, all right reserved.
## Display Driver V1.4 with "ASM-turbo" option (Display folder)
Copyright (C) 2019-2023 VadRov, all right reserved.
## Fixed-point MP3 decoder (папка MP3Helix)
Portions Copyright (c) 1995-2002 RealNetworks, Inc. All Rights Reserved.
## File manager (FileManager folder)
Copyright (C) 2022, VadRov, all right reserved.
## Module for working with buttons (Keyboard folder)
Copyright (C) 2021, VadRov, all right reserved.
## Graphic library for working with primitive objects in two-dimensional space microGL2D (MicroGL2D folder)
Copyright (C) 2022, VadRov, all right reserved.
## Encoder driver for STM32F4 (Encoder folder)
Copyright (C) 2019, VadRov, all right reserved.
## String procedures (MyString folder)
Copyright (C) 2019, VadRov, all right reserved.

Project author: **VadRov**\
Contacts: [Youtube](https://www.youtube.com/@VadRov) [Dzen](https://dzen.ru/vadrov) [VK](https://vk.com/vadrov) [Telegram](https://t.me/vadrov_channel)\
Donate: [donate.yoomoney](https://yoomoney.ru/to/4100117522443917)
