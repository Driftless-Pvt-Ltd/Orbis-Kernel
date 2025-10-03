/* ===========================================================================
 *  Orbis Kernel – Logging System (log.h)
 *  Author: Kap Petrov (@VigfallStudios)
 *  Description: Manages kernel logging. Provides logging
 *               functions for debugging purposes.
 *  License: None
 * ===========================================================================
 */

#ifndef LOG_H
#define LOG_H

#include <stdint.h>

// PL011 UART0 base
#define UART0_BASE 0x09000000UL
#define UART0_DR   (*(volatile uint32_t*)(UART0_BASE + 0x00))
#define UART0_FR   (*(volatile uint32_t*)(UART0_BASE + 0x18))

// Send a single character
void uart_putc(char c) {
    while (UART0_FR & (1 << 5)) ; // wait if TX FIFO full
    UART0_DR = c;
}

// Send a null-terminated string
void uart_puts(const char* s) {
    while(*s) uart_putc(*s++);
}

static const char hexchars[] = "0123456789ABCDEF";

void print_hex(uint64_t val, int digits) {
    for (int i = (digits - 1) * 4; i >= 0; i -= 4) {
        uint8_t nibble = (val >> i) & 0xF;
        uart_putc(hexchars[nibble]);
    }
    uart_putc('\n');
}

enum LogLevel {
    LOG_NONE,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
};

static const char* log_level_str[] = {
    " ",
    "INFO",
    "WARN",
    "ERROR"
};

void uart_putnum(uint64_t val) {
    if (val == 0) { uart_putc('0'); return; }
    char buf[20];
    int i = 0;
    while (val) { buf[i++] = '0' + (val % 10); val /= 10; }
    for (int j=i-1; j>=0; j--) uart_putc(buf[j]);
}

static inline uint64_t read_cntpct(void) {
    uint64_t cnt;
    asm volatile("mrs %0, cntvct_el0" : "=r"(cnt));
    return cnt;
}

static inline uint64_t read_cntfrq(void) {
    uint64_t freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    return freq;
}

uint64_t millis() {
    static uint64_t freq = 0;
    if (!freq) freq = read_cntfrq();  // cache frequency
    uint64_t cnt = read_cntpct();
    return (cnt * 1000) / freq;       // ms
}

void uart_puttime(uint64_t ms) {
    // Print seconds as sssss.mmmmmm
    uint64_t sec = ms / 1000;
    uint64_t frac = (ms % 1000) * 1000; // microseconds
    // print sec
    uart_putnum(sec);
    uart_putc('.');
    // print frac with 6 digits, padded
    char buf[7];
    for (int i = 5; i >= 0; i--) {
        buf[i] = '0' + (frac % 10);
        frac /= 10;
    }
    buf[6] = 0;
    uart_puts(buf);
}

void log(LogLevel level, const char* msg) {
    uint64_t ts = millis();  // real milliseconds since boot
    uart_puttime(ts);
    uart_putc(' ');
    uart_putc('[');
    uart_puts(log_level_str[level]);
    uart_putc(']');
    uart_putc(' ');
    uart_puts(msg);
    uart_putc('\n');
}

void log_subsystem(const char* subsystem, LogLevel level, const char* msg) {
    uint64_t ts = millis();  // milliseconds since boot
    uart_puttime(ts);

    uart_puts("  <");
    uart_puts(subsystem);    // subsystem name
    uart_puts(">:");
    if (level != LOG_NONE)
    {
        uart_putc(' ');
        uart_putc('[');
        uart_puts(log_level_str[level]); // log level
        uart_putc(']');
    }
    uart_putc(' ');
    uart_puts(msg);
}

#endif