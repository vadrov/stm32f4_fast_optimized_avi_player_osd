################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FATFS/diskio.c \
../FATFS/ff.c \
../FATFS/ffsystem.c \
../FATFS/ffunicode.c \
../FATFS/user_diskio_spi.c 

OBJS += \
./FATFS/diskio.o \
./FATFS/ff.o \
./FATFS/ffsystem.o \
./FATFS/ffunicode.o \
./FATFS/user_diskio_spi.o 

C_DEPS += \
./FATFS/diskio.d \
./FATFS/ff.d \
./FATFS/ffsystem.d \
./FATFS/ffunicode.d \
./FATFS/user_diskio_spi.d 


# Each subdirectory must supply rules for building sources it contributes
FATFS/%.o FATFS/%.su FATFS/%.cyclo: ../FATFS/%.c FATFS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DSTM32F401xC -DUSE_FULL_LL_DRIVER -DHSE_VALUE=25000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DEXTERNAL_CLOCK_VALUE=12288000 -DHSI_VALUE=16000000 -DLSI_VALUE=32000 -DPREFETCH_ENABLE=1 -DINSTRUCTION_CACHE_ENABLE=1 -DDATA_CACHE_ENABLE=1 -DVDD_VALUE=3400 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/Display" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/FATFS" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/PCM5102" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/MP3Helix/include" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/JPEG" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/AVI" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/FileManager" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/Keyboard" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/MicroGL2D" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/Encoder" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/MyString" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-FATFS

clean-FATFS:
	-$(RM) ./FATFS/diskio.cyclo ./FATFS/diskio.d ./FATFS/diskio.o ./FATFS/diskio.su ./FATFS/ff.cyclo ./FATFS/ff.d ./FATFS/ff.o ./FATFS/ff.su ./FATFS/ffsystem.cyclo ./FATFS/ffsystem.d ./FATFS/ffsystem.o ./FATFS/ffsystem.su ./FATFS/ffunicode.cyclo ./FATFS/ffunicode.d ./FATFS/ffunicode.o ./FATFS/ffunicode.su ./FATFS/user_diskio_spi.cyclo ./FATFS/user_diskio_spi.d ./FATFS/user_diskio_spi.o ./FATFS/user_diskio_spi.su

.PHONY: clean-FATFS

