PART=EFM32GG232F1024

AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
OBJDUMP=arm-none-eabi-objdump

CFLAGS = \
	-g -O2 -Wall -std=gnu99 \
	-mthumb -mcpu=cortex-m3 \
	-D${PART} \
	-Iexternal/Device/SiliconLabs/EFM32GG/Include/ \
	-Iexternal/CMSIS/Include/ \
	-Iexternal/emlib/inc/ \
	-Iexternal/drivers \
	-Iexternal/fatfs/src/ \
	-Iexternal/lwip/src/include/ \
	-Iexternal/lwip/src/include/ipv4/ \
	-Iexternal/lwip/src/platform/ \
	-Iexternal/enc424j600/include/ \
	-Iexternal/enc424j600/src/ \
	-I.
LDFLAGS = -mthumb -mcpu=cortex-m3 -Wl,-Texternal/efm32gg.ld --specs=nosys.specs 
ASFLAGS =

.PHONY: default
default: asd_pre_1.bin asd_pre_1.hex

%.hex : %.elf
	${OBJCOPY} -O ihex $< $@

%.bin : %.elf
	${OBJCOPY} -O binary $< $@

%.dis : %.elf
	${OBJDUMP} -l -S -D $< > $@


asd_pre_1.elf: \
	$(patsubst %.c,%.o,$(wildcard *.c)) \
	$(patsubst %.c,%.o,$(wildcard external/emlib/src/*.c)) \
	$(patsubst %.c,%.o,$(wildcard external/drivers/*.c)) \
	$(patsubst %.c,%.o,$(wildcard external/fatfs/src/*.c)) \
	$(patsubst %.c,%.o,$(wildcard external/fatfs/src/option/unicode.c)) \
	$(patsubst %.c,%.o,$(wildcard external/lwip/src/core/*.c)) \
	$(patsubst %.c,%.o,$(wildcard external/lwip/src/core/ipv4/*.c)) \
	external/lwip/src/netif/etharp.o \
	$(patsubst %.c,%.o,$(wildcard external/lwip/src/platform/*.c)) \
	$(patsubst %.c,%.o,$(wildcard external/lwip/src/api/*.c)) \
	external/enc424j600/src/enc424j600_spi.o \
	external/Device/SiliconLabs/EFM32GG/Source/GCC/startup_efm32gg.o \
	external/Device/SiliconLabs/EFM32GG/Source/system_efm32gg.o
	${CC} ${LDFLAGS} $^ -o $@

%.o : %.c
	${CC} ${CFLAGS} -c $< -o $@

%.o : %.s
	${AS} ${ASFLAGS} -c $< -o $@

%.o : %.S
	${AS} ${ASFLAGS} -c $< -o $@

.PHONY: upload
upload: asd_pre_1.bin
	JLinkExe jlink.cmd

.PHONY: clean
clean:
	rm -rf *.o *.elf *.bin *.hex *.dis external/emlib/src/*.o external/drivers/*.o external/fatfs/src/*.o external/fatfs/src/option/*.o external/lwip/src/core/*.o external/lwip/src/core/ipv4/*.o external/lwip/src/netif/*.o external/lwip/src/platform/*.o external/lwip/src/api/*.o external/enc424j600/src/*.o external/Device/SiliconLabs/EFM32GG/Source/GCC/*.o external/Device/SiliconLabs/EFM32GG/Source/*.o
