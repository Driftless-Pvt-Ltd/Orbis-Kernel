/* ===========================================================================
 *  Orbis Kernel – Main Kernel Source (main.cpp)
 *  Author: Kap Petrov (@VigfallStudios)
 *  Description: Implements initialization code, logo
 *               display, and simple layered compositing.
 * ===========================================================================
 */

#include <stdint.h>
#include "../libkern/std/standard.h"
#include "../libkern/std/log.h"
#include "../libkern/font.h"
#include "../libui/coregraphics/cg.h"
#include "../libkern/mm/pmm.h"
#include "../libui/quartz/quartz.h"
#include "../libkern/ioframebuffer/iofb.h"

void panic(const char *msg)
{
    log_subsystem("kernel", LOG_ERROR, "*** Panic ***\n");

    fb_draw_rect(0, 0, FB_WIDTH, FB_HEIGHT, 0x2965);
    fb_drawstr_scaled(45, 10, "KERNEL PANIC EXCEPTION", 0xF800, 4);
    fb_drawstr_scaled(16, 570, msg, 0xFFFF, 2);

    fb_drawstr_scaled(120, 150, "YOU NEED TO RESTART YOUR COMPUTER", 0xFFFF, 2);
    fb_drawstr_scaled(120, 200, "NECESITAS REINICIAR TU COMPUTADORA", 0xFFFF, 2);
    fb_drawstr_scaled(120, 250, "SIE MUSSEN IHREN COMPUTER NEU STARTEN", 0xFFFF, 2);
    fb_drawstr_scaled(120, 300, "VAM NUZHNO PEREZAGRUZIT KOMPYUTER", 0xFFFF, 2);

    log_subsystem("panic", LOG_NONE, "hanging here...\n");

    while(1);
}

int bootDelay = 20000000;

extern "C" uintptr_t __phys_mem_start;
extern "C" uintptr_t __phys_mem_end;

extern "C" void el1_vector_table(void);

static inline uint64_t syscall(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2) {
    register uint64_t x0 asm("x0") = arg0;
    register uint64_t x1 asm("x1") = arg1;
    register uint64_t x2 asm("x2") = arg2;
    register uint64_t x8 asm("x8") = num;  // syscall number
    asm volatile("svc #0" : "+r"(x0) : "r"(x1), "r"(x2), "r"(x8) : "memory");
    return x0; // return value in x0
}

extern "C" void user_mode_entry(void)
{
    // This code runs in EL0 (user mode)
    volatile uint64_t *flag = (volatile uint64_t *)0x40414000; // write somewhere safe
    *flag = 0xDEADBEEF;

    while (1)
    {
        fb_draw_rect(0, 0, 50, 50, 0xF800); // red square
        for (volatile int i=0;i<500000;i++);
        fb_draw_rect(0, 0, 50, 50, 0x07E0); // green square
        for (volatile int i=0;i<500000;i++);
        syscall(SYSCALL_TEST, 0, 0, 0);
    }
}

typedef struct {
    uint64_t x0;
    uint64_t x1;
    uint64_t x2;
    uint64_t x3;
    uint64_t x4;
    uint64_t x5;
    uint64_t x6;
    uint64_t x7;
    uint64_t syscall_num; // x8
} syscall_regs_t;

extern "C" void syscall_handler(syscall_regs_t* regs)
{
    log_subsystem("SyscallHandler", LOG_INFO, "syscall triggered: ");

    print_hex(regs->syscall_num, 16);
    uart_puts("\n");

    uart_puts("Args: ");
    print_hex(regs->x0, 16); uart_puts(", ");
    print_hex(regs->x1, 16); uart_puts(", ");
    print_hex(regs->x2, 16); uart_puts("\n");

    regs->x0 = 0x1234ABCD;
}

extern "C" void enter_user_mode()
{
    static uint8_t el0_stack[4096];
    uintptr_t sp_el0 = (uintptr_t)el0_stack + sizeof(el0_stack);

    // Set EL0 stack, ELR_EL1, SPSR_EL1, then eret
    asm volatile(
        // Set EL0 stack pointer
        "msr sp_el0, %0\n"
        // Set the exception return address (user entry)
        "msr elr_el1, %1\n"
        // Set SPSR_EL1: EL0t, interrupts enabled
        "msr spsr_el1, %2\n"
        // Return to EL0
        "eret\n"
        :
        : "r"(sp_el0), "r"(user_mode_entry), "r"(0x0)
        : "memory"
    );
}

extern "C" int main()
{
    log_subsystem("kernel", LOG_NONE, "booting\n");
    log_subsystem("IOFramebuffer", LOG_WARN, "Framebuffer not fully initialized, using defaults.\n");

    uintptr_t fb_addr = (uintptr_t)&__fb_base;
    log_subsystem("IOFramebuffer", LOG_INFO, "Framebuffer base address found: ");
    uart_puts("0x");
    print_hex(fb_addr, 16);
    uart_puts("\n");

    int logo_x = (FB_WIDTH - LOGO_WIDTH * 4) / 2;
    int logo_y = (FB_HEIGHT - LOGO_HEIGHT* 4) / 2;
    fb_draw_logo_scaled(logo_x, logo_y, 0xFFFF, 4); // white

    // Draw loading bar under logo
    int bar_x = (FB_WIDTH - BAR_WIDTH) / 2;
    int bar_y = logo_y + LOGO_HEIGHT * 4 + 16; // 16 px spacing

    fb_draw_progress(bar_x, bar_y, BAR_WIDTH, BAR_HEIGHT, 10, 0x0000, 0xFFFF);
    for (volatile int i = 0; i < bootDelay; i++); // delay

    log_subsystem("PhysicalMemoryManager", LOG_INFO, "initializing\n");
    pmm_init((uintptr_t)&__phys_mem_start, (uintptr_t)&__phys_mem_end);

    fb_draw_progress(bar_x, bar_y, BAR_WIDTH, BAR_HEIGHT, 20, 0x0000, 0xFFFF);
    for (volatile int i = 0; i < bootDelay; i++);

    log_subsystem("PhysicalMemoryManager", LOG_INFO, "allocating\n");
    uintptr_t page = pmm_alloc_page();

    fb_draw_progress(bar_x, bar_y, BAR_WIDTH, BAR_HEIGHT, 50, 0x0000, 0xFFFF);
    for (volatile int i = 0; i < bootDelay; i++);

    log_subsystem("PhysicalMemoryManager", LOG_INFO, "freeing\n");
    pmm_free_page(page);

    fb_draw_progress(bar_x, bar_y, BAR_WIDTH, BAR_HEIGHT, 100, 0x0000, 0xFFFF);
    for (volatile int i = 0; i < bootDelay; i++);

    fb_draw_rect(0, 0, FB_WIDTH, FB_HEIGHT, 0x0000);
    for (volatile int i = 0; i < bootDelay; i++);

    log_subsystem("kernel", LOG_NONE, "entering EL0\n");
    asm volatile("msr vbar_el1, %0" :: "r"(el1_vector_table) : "memory");
    enter_user_mode();

    // Should never reach here
    panic("USER SWITCH FAILED");

    while(1)
    {
        //CompositorDrawDirty();
    }
}