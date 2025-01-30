################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32f401ccux.s 

OBJS += \
./Core/Startup/startup_stm32f401ccux.o 

S_DEPS += \
./Core/Startup/startup_stm32f401ccux.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -c -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/Display" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/FATFS" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/PCM5102" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/MP3Helix/include" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/JPEG" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/AVI" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/FileManager" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/Keyboard" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/MicroGL2D" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/Encoder" -I"C:/Users/Vadim/STM32Cube/Repository/stm32f401ccu6_avi_player/MyString" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32f401ccux.d ./Core/Startup/startup_stm32f401ccux.o

.PHONY: clean-Core-2f-Startup

