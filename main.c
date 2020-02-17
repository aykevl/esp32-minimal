#include <stddef.h>
#include <stdint.h>
#include "esp32.h"

#define PRINT 1

extern uint32_t _sbss[0];
extern uint32_t _ebss[0];

#if PRINT
static const char *message = "Hello world!\r\n";
static const size_t message_len = 14;
#endif

void sleep_ns(uint64_t nanoseconds);

int main(void) {
    // Disable both watchdog timers that are enabled by default on startup.
    // Note that these watchdogs can be protected, but the ROM bootloader
    // doesn't seem to protect them.
    RTC->WDTCONFIG[0] = 0;
    TIMG0->WDTCONFIG[0] = 0;

    // Switch SoC clock source to PLL (instead of the default which is XTAl).
    // This switches the CPU (and APB) clock from 40MHz to 80MHz.
    // Bits:
    //   28-27: RTC_CNTL_SOC_CLK_SEL:       1 (default 0)
    //   14-12: RTC_CNTL_CK8M_DIV_SEL:      2 (default)
    //   9:     RTC_CNTL_DIG_CLK8M_D256_EN: 1 (default)
    //   5-4:   RTC_CNTL_CK8M_DIV:          1 (default)
    // The only real change made here is modifying RTC_CNTL_SOC_CLK_SEL, but
    // setting a fixed value produces smaller code.
    RTC->CLK_CONF = (1 << 27) | (2 << 12) | (1 << 9) | (4 << 1);

    // Switch CPU from 80MHz to 160MHz. This doesn't affect the APB clock,
    // which is still running at 80MHz.
    DPORT->CPU_PER_CONF = (1 << 0);

    // Clear the .bss section. The .data section has already been loaded by the
    // ROM bootloader.
    uint32_t *ptr = _sbss;
    while (ptr != _ebss) {
        *ptr = 0;
        ptr++;
    }

    // Configure UART baud rate.
    // The UART baud rate is (by default) derived from the APB clock, so will
    // be running at 80MHz.
#if PRINT
    UART0->CLKDIV = 80000000 / 115200;
#endif

    // Configure timer 0 in timer group 0. Bits:
    //   31:    EN:       enable the timer
    //   30:    INCREASE: count up every tick (as opposed to counting down)
    //   28-13: DIVIDER:  16-bit prescaler
    TIMG0->T[0].CONFIG = (1 << 31) | (1 << 30) | (2 << 13);

    // Set the timer counter value to 0.
    TIMG0->T[0].LOADLO = 0;
    TIMG0->T[0].LOADHI = 0;
    TIMG0->T[0].LOAD = 0; // value doesn't matter.

    // Enable output for GPIO2, which is connected to the blue LED on the
    // ESP-WROOM-32.
    GPIO->ENABLE_W1TS = 1 << 2;

    while (1) {
        GPIO->OUT_W1TS = 1 << 2; // LED on
#if PRINT
        for (int i = 0; i < message_len; i++) {
            UART0->FIFO = message[i];
        }
#endif

        sleep_ns(500000000);

        GPIO->OUT_W1TC = 1 << 2; // LED off
        sleep_ns(500000000);
    }
}

// timer_ticks returns the number of timer ticks passed since clock
// initialization.
uint64_t timer_ticks(void) {
    // First, update the LO and HI register pair by writing any value to the
    // register. This allows reading the pair atomically.
    TIMG0->T[0].UPDATE = 0;
    // Then read the two 32-bit parts of the timer.
    return (uint64_t)(TIMG0->T[0].LO) | ((uint64_t)(TIMG0->T[0].HI) << 32);
}

// sleep_ns busy-waits until the given number of nanoseconds have passed.
void sleep_ns(uint64_t nanoseconds) {
    // Calculate the number of ticks from the number of nanoseconds. At a 80MHz
    // APB clock, that's 25 nanoseconds per tick with a timer prescaler of 2:
    // 25 = 1e9 / (80MHz / 2)
    uint32_t ticks = nanoseconds / 25UL;
    uint64_t waitUntil = timer_ticks() + ticks;
    while (timer_ticks() < waitUntil) {
    }
}
