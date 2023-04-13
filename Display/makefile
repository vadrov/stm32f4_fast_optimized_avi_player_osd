# Copyright (C)2023, VadRov, all right reserved.
#
# Генерация заголовочного файла display_offsets.h 
# для ассемблерного варианта драйвера дисплея
#
# Допускается свободное распространение.
# При любом способе распространения указание автора ОБЯЗАТЕЛЬНО.
# В случае внесения изменений и распространения модификаций указание первоначального автора ОБЯЗАТЕЛЬНО.
# Распространяется по типу "как есть", то есть использование осуществляется на свой страх и риск.
# Автор не предоставляет никаких гарантий.
#
# https://www.youtube.com/@VadRov
# https://dzen.ru/vadrov
# https://vk.com/vadrov
# https://t.me/vadrov_channel

# Заголовочный файл (путь с именем), в котором будем формировать смещения переменных структуры LCD_Handler 
header-file := ../Display/display_offsets.h

# Команды для исполнения (для них ниже будут, т.н., "правила")
all: display_offsets.s $(header-file)

# Параметры для компилятора и файлов драйвера, определяющие используемый МК
core := -mcpu=cortex-m4 \
		-std=gnu11 \
		-DSTM32F401xC \
		-DUSE_FULL_LL_DRIVER
		
# Пути поиска заголовочных файлов, необходимых компилятору для трансляции программы с C в ASM
# Это драйверы ядра, МК, "описателя" структуры LCD_Handler...
include := -I../Display \
		   -I../Core/Inc \
		   -I../Drivers/STM32F4xx_HAL_Driver/Inc \
		   -I../Drivers/CMSIS/Include \
		   -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include

# Макрос, определяющий во входном файле по заданному шаблону "#define name #value" все строки с именами 
# и значениями смещений переменных, и записывающий эти имена и смещения в выходной файл в виде строки 
# в формате макросов языка C (#define name value). Макрос формирует выходной файл по шаблону заголовочного
# файла языка С и добавляет пользовательские комментарии, указанные в теле макроса.
define create_h_from_s
	(set -e; \
	 echo "/*"; \
	 echo " *  Copyright (C)2023, VadRov, all right reserved."; \
	 echo " *"; \
	 echo " *  Display Driver v1.4+ (includes fast assembler subroutines)"; \
	 echo " *  Contains variable offsets in the LCD_Handler structure for use from an assembler program."; \
	 echo " *"; \
	 echo " *  This file was generated automatically."; \
	 echo " *"; \
	 echo " *  https://www.youtube.com/@VadRov"; \
	 echo " *  https://dzen.ru/vadrov"; \
	 echo " *  https://vk.com/vadrov"; \
	 echo " *  https://t.me/vadrov_channel"; \
	 echo " */"; \
	 echo ""; \
	 echo "#ifndef __DISPLAY_OFFSETS_H__"; \
	 echo "#define __DISPLAY_OFFSETS_H__"; \
	 echo ""; \
	 sed -n '{s:^#define \([^ ]*\) [\$$#]*\([-0-9]*\):#define \1 \2:p;}' $<; \
	 echo ""; \
	 echo "#endif /*__DISPLAY_OFFSETS_H__*/" ) > $@
endef

# Правила формирования заголовочного файла display_offsets.h
# Здесь в консоль выводится сообщение, что формируется файл display_offsets.h со смещениями переменных
# в структуре LCD_Handler, которые используются в программе на ассемблере.
# Вызывается макрос create_h_from_s, на входе которого будет файл display_offsets.s, а на выходе - 
# заголовочный файл display_offsets.h
$(header-file): display_offsets.s
	@echo "Creating a header file $@ for the LCD_Handler structure (used in an assembler program)."
	@$(create_h_from_s)

# Правила формирования файла display_offsets.s
# Вызывается компилятор (arm-none-eabi-gcc), в опции которому (-S) укажем, что хотим транслировать программу с С 
# в программу на ассемблере.
display_offsets.s: display_offsets.c
	arm-none-eabi-gcc -S $(core) $(include) ../Display/$<
	
display_offsets.c:
