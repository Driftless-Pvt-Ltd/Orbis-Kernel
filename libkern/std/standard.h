/* ===========================================================================
 *  Orbis Kernel – Standard Global Header (standard.h)
 *  Author: Kap Petrov (@VigfallStudios)
 *  Description: Provides standard functions and global
 *               variables for every source file.
 *  License: None
 * ===========================================================================
 */


#ifndef STAND_H
#define STAND_H

#include <stdint.h>

/*
    XNU syscall list was obtained from this gist
    https://gist.github.com/BlueFalconHD/da39227ec22908c8e9d12e458a5155d4
*/
#define SYSCALL_EXIT 1
#define SYSCALL_FORK 2
#define SYSCALL_READ 3
#define SYSCALL_WRITE 4

typedef struct {
    uintptr_t ptr;
} user_addr_t;

typedef char* caddr_t;

static inline void* user_ptr(user_addr_t ua) { return (void*)ua.ptr; }
static inline user_addr_t make_user_addr(void* p) { return (user_addr_t){ .ptr = (uintptr_t)p }; }
static inline caddr_t to_caddr(void* p) { return (caddr_t)p; }

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