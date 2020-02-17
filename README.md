# Bare minimum ESP32

This is a very minimal ESP32 program, inspired by
[nosdk8266](https://github.com/cnlohr/nosdk8266). It is pure C with no SDK parts
left apart from one function declaration. Also see the related
[esp8266-minimal](https://github.com/aykevl/esp8266-minimal) project.

Code size on my version of the ESP32 toolchain (GCC 5.2.0):

| configuration    | text      | data | bss |
| ---------------- | --------- | ---- | --- |
| UART enabled     | 330 bytes | 0    | 0   |
| UART disabled    | 267 bytes | 0    | 0   |
| only timer + LED | 211 bytes | 0    | 0   |

You could trim this down even further with a delay loop instead of using a
timer.

## How to use

Simply run:

    make flash

I recommend [picocom](https://github.com/npat-efault/picocom) to see the serial
output:

    picocom --baud 115200 /dev/ttyUSB0

## Why?

To understand how the ESP32 works, outside of the
[ESP-IDF](https://github.com/espressif/esp-idf). This was my first step towards
porting the ESP32 to [TinyGo](https://tinygo.org/). It may be useful to other
people who would like to avoid the ESP-IDF, for reasons such as code size,
compilation speed, or porting a new language.

Also see the [esp8266-minimal](https://github.com/aykevl/esp8266-minimal)
project, which is very similar but for the ESP8266.

## License

Two files include code derived from ESP-IDF. These are `esp32.h` (register
names) and `esp32.ld` (libgcc function definitions). Because they describe an
API and not an implementation I belive they cannot be copyrighted or at least
fall under fair use, but I'm not sure so use your own judgment on that. If they
are copyrightable, they are under the Apache 2.0 license.

All other files can be considered public domain, which means you can use them
however you wish. If you want a bit more detail, you could consider them to be
licensed under the [Unlicense](https://unlicense.org/).
