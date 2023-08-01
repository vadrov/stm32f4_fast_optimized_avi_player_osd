################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MyString/mystring.c 

OBJS += \
./MyString/mystring.o 

C_DEPS += \
./MyString/mystring.d 


# Each subdirectory must supply rules for building sources it contributes
MyString/%.o MyString/%.su MyString/%.cyclo: ../MyString/%.c MyString/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DSTM32F401xC -DUSE_FULL_LL_DRIVER -DHSE_VALUE=25000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DEXTERNAL_CLOCK_VALUE=12288000 -DHSI_VALUE=16000000 -DLSI_VALUE=32000 -DPREFETCH_ENABLE=1 -DINSTRUCTION_CACHE_ENABLE=1 -DDATA_CACHE_ENABLE=1 -DVDD_VALUE=3400 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/SamaraPRO/STM32Cube/Repository/stm32f401ccu6_spi_sdcard/Display" -I"C:/Users/SamaraPRO/STM32Cube/Repository/stm32f401ccu6_spi_sdcard/FATFS" -I"C:/Users/SamaraPRO/STM32Cube/Repository/stm32f401ccu6_spi_sdcard/PCM5102" -I"C:/Users/SamaraPRO/STM32Cube/Repository/stm32f401ccu6_spi_sdcard/MP3Helix/include" -I"C:/Users/SamaraPRO/STM32Cube/Repository/stm32f401ccu6_spi_sdcard/JPEG" -I"C:/Users/SamaraPRO/STM32Cube/Repository/stm32f401ccu6_spi_sdcard/AVI" -I"C:/Users/SamaraPRO/STM32Cube/Repository/stm32f401ccu6_spi_sdcard/FileManager" -I"C:/Users/SamaraPRO/STM32Cube/Repository/stm32f401ccu6_spi_sdcard/Keyboard" -I"C:/Users/SamaraPRO/STM32Cube/Repository/stm32f401ccu6_spi_sdcard/MicroGL2D" -I"C:/Users/SamaraPRO/STM32Cube/Repository/stm32f401ccu6_spi_sdcard/Encoder" -I"C:/Users/SamaraPRO/STM32Cube/Repository/stm32f401ccu6_spi_sdcard/MyString" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-MyString

clean-MyString:
	-$(RM) ./MyString/mystring.cyclo ./MyString/mystring.d ./MyString/mystring.o ./MyString/mystring.su

.PHONY: clean-MyString

