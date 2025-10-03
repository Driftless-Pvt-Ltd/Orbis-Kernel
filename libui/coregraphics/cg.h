/* ===========================================================================
 *  Orbis Kernel – Core Graphics (cg.h)
 *  Author: Kap Petrov (@VigfallStudios)
 *  Description: Manages low-level drawing. Provides a low
 *               level API for drawing using Contexts.
 *  License: None
 * ===========================================================================
 */

#ifndef COREGRAPHICS_H
#define COREGRAPHICS_H

#include <stdint.h>
#include <stdlib.h>
#include "../../libkern/font.h"
#include "../../libkern/std/standard.h"

#define FB_WIDTH  800
#define FB_HEIGHT 600
#define MAX_CONTEXTS 4
#define MAX_CTX_WIDTH  800
#define MAX_CTX_HEIGHT 600

typedef struct {
    int width;
    int height;
    uint16_t buffer[MAX_CTX_HEIGHT][MAX_CTX_WIDTH];
    int active;
} CGContext;

// Static array of contexts
static CGContext contexts[MAX_CONTEXTS];

// Create a context (returns index)
int CGContextCreate(int width, int height) {
    if (width > MAX_CTX_WIDTH || height > MAX_CTX_HEIGHT)
    {
        log_subsystem("CoreGraphics", LOG_ERROR, "Context parameters passed exceed max resolution\n");
        return -1;
    }
    log_subsystem("CoreGraphics", LOG_NONE, "Creating context\n");
    for (int i = 0; i < MAX_CONTEXTS; i++) {
        if (!contexts[i].active) {
            contexts[i].width = width;
            contexts[i].height = height;
            contexts[i].active = 1;
            // Clear buffer
            for (int y = 0; y < height; y++)
                for (int x = 0; x < width; x++)
                    contexts[i].buffer[y][x] = 0x0000;
            log_subsystem("CoreGraphics", LOG_NONE, "Context creation successful\n");
            return i;
        }
    }
    log_subsystem("CoreGraphics", LOG_ERROR, "Max context limit surpassed\n");
    return -1; // no free context
}

// Set pixel in context
void CGContextSetPixel(int ctx_id, int x, int y, uint16_t color) {
    if (ctx_id < 0 || ctx_id >= MAX_CONTEXTS) return;
    CGContext* ctx = &contexts[ctx_id];
    if (!ctx->active) return;
    if (x < 0 || y < 0 || x >= ctx->width || y >= ctx->height) return;
    ctx->buffer[y][x] = color;
}

// Fill rectangle
void CGContextFillRect(int ctx_id, int x0, int y0, int w, int h, uint16_t color) {
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            CGContextSetPixel(ctx_id, x0 + x, y0 + y, color);
}

// Draw character
void CGContextDrawChar(int ctx_id, int x, int y, char c, uint16_t color) {
    if (c < 0 || c > 127) return;
    const uint8_t* glyph = font8x8_basic[(int)c];
    for (int row = 0; row < 8; row++) {
        uint8_t bits = glyph[row];
        for (int col = 0; col < 8; col++) {
            if (bits & (1 << (7 - col)))
                CGContextSetPixel(ctx_id, x + col, y + row, color);
        }
    }
}

// Draw string
void CGContextDrawString(int ctx_id, int x, int y, const char* str, uint16_t color) {
    while (*str) {
        CGContextDrawChar(ctx_id, x, y, *str++, color);
        x += 8;
    }
}

void CGContextBlitToFB(int ctx_id, int x0, int y0) {
    if (ctx_id < 0 || ctx_id >= MAX_CONTEXTS) return;
    CGContext* ctx = &contexts[ctx_id];
    if (!ctx->active) return;

    volatile uint16_t* fb = fb_ptr();
    for (int y = 0; y < ctx->height; y++) {
        for (int x = 0; x < ctx->width; x++) {
            int fb_x = x0 + x;
            int fb_y = y0 + y;
            if (fb_x >= 0 && fb_y >= 0 && fb_x < FB_WIDTH && fb_y < FB_HEIGHT)
                fb[fb_y * FB_WIDTH + fb_x] = ctx->buffer[y][x];
        }
    }
}

#endif