################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include PCM5102/subdir.mk
-include MyString/subdir.mk
-include MicroGL2D/subdir.mk
-include MP3Helix/src/subdir.mk
-include Keyboard/subdir.mk
-include JPEG/subdir.mk
-include FileManager/subdir.mk
-include FATFS/subdir.mk
-include Encoder/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Display/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include AVI/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := stm32f401ccu6_avi_player
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
stm32f401ccu6_avi_player.elf \

MAP_FILES += \
stm32f401ccu6_avi_player.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
stm32f401ccu6_avi_player.list \

OBJCOPY_HEX += \
stm32f401ccu6_avi_player.hex \

OBJCOPY_BIN += \
stm32f401ccu6_avi_player.bin \


# All Target
all:
	+@$(MAKE) --no-print-directory pre-build && $(MAKE) --no-print-directory main-build

# Main-build Target
main-build: stm32f401ccu6_avi_player.elf secondary-outputs

# Tool invocations
stm32f401ccu6_avi_player.elf stm32f401ccu6_avi_player.map: $(OBJS) $(USER_OBJS) C:\Users\SamaraPRO\STM32Cube\Repository\stm32f401ccu6_spi_sdcard\STM32F401CCUX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "stm32f401ccu6_avi_player.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\SamaraPRO\STM32Cube\Repository\stm32f401ccu6_spi_sdcard\STM32F401CCUX_FLASH.ld" --specs=nosys.specs -Wl,-Map="stm32f401ccu6_avi_player.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

stm32f401ccu6_avi_player.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "stm32f401ccu6_avi_player.list"
	@echo 'Finished building: $@'
	@echo ' '

stm32f401ccu6_avi_player.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "stm32f401ccu6_avi_player.hex"
	@echo 'Finished building: $@'
	@echo ' '

stm32f401ccu6_avi_player.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "stm32f401ccu6_avi_player.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) default.size.stdout stm32f401ccu6_avi_player.bin stm32f401ccu6_avi_player.elf stm32f401ccu6_avi_player.hex stm32f401ccu6_avi_player.list stm32f401ccu6_avi_player.map
	-@echo ' '

pre-build:
	make -f ../Display/makefile
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified pre-build

-include ../makefile.targets
