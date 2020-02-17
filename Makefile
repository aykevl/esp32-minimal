all: build/firmware.elf

CC      = xtensa-esp32-elf-gcc
CFLAGS  = -Os -g -Wall -Werror -nostdlib -mlongcalls -std=c11 -flto
LDFLAGS = -Wl,--gc-sections -T esp32.ld

flash: build/firmware.elf
	esptool.py --chip=esp32 elf2image build/firmware.elf
	esptool.py --chip=esp32 --port /dev/ttyUSB0 write_flash 0x1000 build/firmware.bin -ff 80m -fm dout

clean:
	rm -rf build

build/firmware.elf: main.c esp32.h esp32.ld
	@mkdir -p build
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ main.c
	size $@
