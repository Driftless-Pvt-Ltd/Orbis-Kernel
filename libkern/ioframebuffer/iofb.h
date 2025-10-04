/* ===========================================================================
 *  Orbis Kernel – IOFramebuffer (iofb.h)
 *  Author: Kap Petrov (@VigfallStudios)
 *  Description: Manages drawing to the frambuffer.
 *               Provides a driver for the display.
 *  License: None
 * ===========================================================================
 */

#ifndef IOFB_H
#define IOFB_H

#include <stdint.h>
#include "../std/standard.h"

#define FB_WIDTH  800
#define FB_HEIGHT 600

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

#endif