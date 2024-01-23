################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../AVI/avi.c \
../AVI/jpeg_data.c \
../AVI/mp3_callback.c \
../AVI/player.c \
../AVI/riff_read_header.c \
../AVI/textures.c 

OBJS += \
./AVI/avi.o \
./AVI/jpeg_data.o \
./AVI/mp3_callback.o \
./AVI/player.o \
./AVI/riff_read_header.o \
./AVI/textures.o 

C_DEPS += \
./AVI/avi.d \
./AVI/jpeg_data.d \
./AVI/mp3_callback.d \
./AVI/player.d \
./AVI/riff_read_header.d \
./AVI/textures.d 


# Each subdirectory must supply rules for building sources it contributes
AVI/%.o AVI/%.su AVI/%.cyclo: ../AVI/%.c AVI/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DSTM32F401xC -DUSE_FULL_LL_DRIVER -DHSE_VALUE=25000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DEXTERNAL_CLOCK_VALUE=12288000 -DHSI_VALUE=16000000 -DLSI_VALUE=32000 -DPREFETCH_ENABLE=1 -DINSTRUCTION_CACHE_ENABLE=1 -DDATA_CACHE_ENABLE=1 -DVDD_VALUE=3400 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/Display" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/FATFS" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/PCM5102" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/MP3Helix/include" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/JPEG" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/AVI" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/FileManager" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/Keyboard" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/MicroGL2D" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/Encoder" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/MyString" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-AVI

clean-AVI:
	-$(RM) ./AVI/avi.cyclo ./AVI/avi.d ./AVI/avi.o ./AVI/avi.su ./AVI/jpeg_data.cyclo ./AVI/jpeg_data.d ./AVI/jpeg_data.o ./AVI/jpeg_data.su ./AVI/mp3_callback.cyclo ./AVI/mp3_callback.d ./AVI/mp3_callback.o ./AVI/mp3_callback.su ./AVI/player.cyclo ./AVI/player.d ./AVI/player.o ./AVI/player.su ./AVI/riff_read_header.cyclo ./AVI/riff_read_header.d ./AVI/riff_read_header.o ./AVI/riff_read_header.su ./AVI/textures.cyclo ./AVI/textures.d ./AVI/textures.o ./AVI/textures.su

.PHONY: clean-AVI

