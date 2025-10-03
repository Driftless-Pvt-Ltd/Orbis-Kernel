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

#define FB_WIDTH  800
#define FB_HEIGHT 600

void fb_putpixel(int x, int y, uint16_t color) {
    volatile uint16_t* fb = fb_ptr();
    fb[y * FB_WIDTH + x] = color;
}

void fb_drawchar(int x, int y, char c, uint16_t color) {
    if (c < 0 || c > 127) return; // only ASCII
    const uint8_t* glyph = font8x8_basic[(int)c];
    for (int row = 0; row < 8; row++) {
        uint8_t bits = glyph[row];
        for (int col = 0; col < 8; col++) {
            if (bits & (1 << (7 - col))) {
                fb_putpixel(x + col, y + row, color);
            }
        }
    }
}

void fb_drawstr(int x, int y, const char* s, uint16_t color) {
    while (*s) {
        fb_drawchar(x, y, *s++, color);
        x += 8; // move right by char width
    }
}

#define LOGO_WIDTH 16
#define LOGO_HEIGHT 16

// 1 = white pixel, 0 = black pixel
const uint16_t boot_logo[LOGO_HEIGHT][LOGO_WIDTH] = {
    {0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
    {0,1,1,0,0,1,1,1,1,1,0,0,1,1,0,0},
    {1,1,0,0,0,1,1,1,1,1,0,0,0,1,1,0},
    {1,1,0,0,0,1,1,1,1,1,0,0,0,1,1,0},
    {1,1,0,0,0,1,1,1,1,1,0,0,0,1,1,0},
    {1,1,0,0,0,0,1,1,1,1,0,0,0,1,1,0},
    {1,1,0,0,0,0,0,1,1,0,0,0,0,1,1,0},
    {1,1,0,0,0,0,0,1,1,0,0,0,0,1,1,0},
    {1,1,0,0,0,0,1,1,1,1,0,0,0,1,1,0},
    {1,1,0,0,0,1,1,1,1,1,0,0,0,1,1,0},
    {1,1,0,0,0,1,1,1,1,1,0,0,0,1,1,0},
    {0,1,1,0,0,1,1,1,1,1,0,0,1,1,0,0},
    {0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
    {0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};

void fb_draw_logo(int x0, int y0, uint16_t color) {
    for (int y = 0; y < LOGO_HEIGHT; y++) {
        for (int x = 0; x < LOGO_WIDTH; x++) {
            if (boot_logo[y][x])
                fb_putpixel(x0 + x, y0 + y, color);
        }
    }
}

void fb_draw_logo_scaled(int x0, int y0, uint16_t color, int scale) {
    for (int y = 0; y < LOGO_HEIGHT; y++) {
        for (int x = 0; x < LOGO_WIDTH; x++) {
            if (boot_logo[y][x]) {
                // Draw a scale x scale block
                for (int dy = 0; dy < scale; dy++) {
                    for (int dx = 0; dx < scale; dx++) {
                        fb_putpixel(x0 + x*scale + dx, y0 + y*scale + dy, color);
                    }
                }
            }
        }
    }
}

#define BAR_WIDTH 128
#define BAR_HEIGHT 8

void fb_draw_rect(int x0, int y0, int w, int h, uint16_t color) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            fb_putpixel(x0 + x, y0 + y, color);
        }
    }
}

void fb_draw_progress(int x0, int y0, int width, int height, int percent, uint16_t color_bg, uint16_t color_fg) {
    fb_draw_rect(x0, y0, width, height, color_bg); // background
    int filled = (width * percent) / 100;
    fb_draw_rect(x0, y0, filled, height, color_fg); // foreground
}

void fb_drawchar_scaled(int x0, int y0, char c, uint16_t color, int scale) {
    if (c < 0 || c > 127) return; // only ASCII
    const uint8_t* glyph = font8x8_basic[(int)c];
    for (int row = 0; row < 8; row++) {
        uint8_t bits = glyph[row];
        for (int col = 0; col < 8; col++) {
            if (bits & (1 << (7 - col))) {
                // Draw a scale x scale block
                for (int dy = 0; dy < scale; dy++) {
                    for (int dx = 0; dx < scale; dx++) {
                        fb_putpixel(x0 + col*scale + dx, y0 + row*scale + dy, color);
                    }
                }
            }
        }
    }
}

void fb_drawstr_scaled(int x, int y, const char* s, uint16_t color, int scale) {
    while (*s) {
        fb_drawchar_scaled(x, y, *s++, color, scale);
        x += 8 * scale; // move right by scaled char width
    }
}

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

#define MAX_LAYERS 8

typedef struct {
    int ctx_id;   // CoreGraphics context index
    int x, y;     // position on screen
    int active;
} Layer;

static Layer layers[MAX_LAYERS];

int LayerCreate(int ctx_id, int x, int y) {
    log_subsystem("QuartzCompositor", LOG_NONE, "Creating layer\n");
    for (int i = 0; i < MAX_LAYERS; i++) {
        if (!layers[i].active) {
            layers[i].ctx_id = ctx_id;
            layers[i].x = x;
            layers[i].y = y;
            layers[i].active = 1;
            return i;
        }
        log_subsystem("QuartzCompositor", LOG_INFO, "Created layer\n");
    }
    log_subsystem("QuartzCompositor", LOG_ERROR, "Failed to create layer: no layers free\n");
    return -1; // no free layer
}

void LayerSetPos(int layer_id, int x, int y) {
    if (layer_id < 0 || layer_id >= MAX_LAYERS) return;
    if (!layers[layer_id].active) return;
    layers[layer_id].x = x;
    layers[layer_id].y = y;
}

void LayerRemove(int layer_id) {
    if (layer_id < 0 || layer_id >= MAX_LAYERS) return;
    log_subsystem("QuartzCompositor", LOG_NONE, "Removing layer\n");
    layers[layer_id].active = 0;
}

void CompositorDraw() {
    volatile uint16_t* fb = fb_ptr();

    // Clear framebuffer first
    for (int y = 0; y < FB_HEIGHT; y++)
        for (int x = 0; x < FB_WIDTH; x++)
            fb[y * FB_WIDTH + x] = 0x0000;

    // Draw layers in order
    for (int i = 0; i < MAX_LAYERS; i++) {
        if (!layers[i].active) continue;
        CGContext* ctx = &contexts[layers[i].ctx_id];
        for (int y = 0; y < ctx->height; y++) {
            for (int x = 0; x < ctx->width; x++) {
                int fb_x = layers[i].x + x;
                int fb_y = layers[i].y + y;
                if (fb_x >= 0 && fb_y >= 0 && fb_x < FB_WIDTH && fb_y < FB_HEIGHT) {
                    fb[fb_y * FB_WIDTH + fb_x] = ctx->buffer[y][x];
                }
            }
        }
    }
}

void CompositorDrawDirty() {
    volatile uint16_t* fb = fb_ptr();

    // Draw layers in order
    for (int i = 0; i < MAX_LAYERS; i++) {
        if (!layers[i].active) continue;
        CGContext* ctx = &contexts[layers[i].ctx_id];

        for (int y = 0; y < ctx->height; y++) {
            for (int x = 0; x < ctx->width; x++) {
                int fb_x = layers[i].x + x;
                int fb_y = layers[i].y + y;

                if (fb_x < 0 || fb_y < 0 || fb_x >= FB_WIDTH || fb_y >= FB_HEIGHT)
                    continue;

                uint16_t pixel = ctx->buffer[y][x];
                volatile uint16_t* fb_pixel = &fb[fb_y * FB_WIDTH + fb_x];

                if (*fb_pixel != pixel) {
                    *fb_pixel = pixel;
                }
            }
        }
    }
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