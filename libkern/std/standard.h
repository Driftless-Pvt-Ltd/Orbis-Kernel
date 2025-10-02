#ifndef STAND_H
#define STAND_H

#include <stdint.h>

int strcmp(const char* s1, const char* s2) {
    while(*s1 && *s2) {
        if(*s1 != *s2) return (unsigned char)*s1 - (unsigned char)*s2;
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

extern "C" {
    extern char __fb_base; // provided by linker
}

static inline volatile uint16_t* fb_ptr() {
    return (volatile uint16_t*)&__fb_base;
}

#endif