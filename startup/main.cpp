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
    
    int ctx1 = CGContextCreate(200, 50);
    CGContextFillRect(ctx1, 0, 0, 200, 50, 0xF800); // red

    int ctx2 = CGContextCreate(100, 100);
    CGContextFillRect(ctx2, 0, 0, 100, 100, 0x001F); // blue

    int layer1 = LayerCreate(ctx1, 100, 50);
    int layer2 = LayerCreate(ctx2, 150, 100);

    while(1)
    {
        CompositorDrawDirty();
    }
}