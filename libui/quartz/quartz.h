/* ===========================================================================
 *  Orbis Kernel – Quartz Compositor (quartz.h)
 *  Author: Kap Petrov (@VigfallStudios)
 *  Description: Provides a compositor.            
 *  License: None
 * ===========================================================================
 */

#ifndef QUARTZ_H
#define QUARTZ_H

#include <stdint.h>
#include "../../libkern/std/standard.h"

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

#endif