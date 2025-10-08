#include "stdlib.h"

void *memset(void *dst, int value, unsigned int size) {
    unsigned char *ptr = (unsigned char *)dst;
    while (size--)
        *ptr++ = (unsigned char)value;
    return dst;
}

void *memcpy(void *dst, const void *src, unsigned int size) {
    unsigned char *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;
    while (size--)
        *d++ = *s++;
    return dst;
}

int strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(const unsigned char *)a - *(const unsigned char *)b;
}

unsigned int strlen(const char *str) {
    unsigned int len = 0;
    while (str[len])
        len++;
    return len;
}

char *strcpy(char *dest, const char *src) {
    char *ret = dest;
    while ((*dest++ = *src++));
    return ret;
}

void puts(const char *str)
{
    write("device_tty", str);
}

void exit(int code)
{
    // code not used yet..
    exit_system_call();
}

int fork()
{
    int res = fork_system_call();
    return res;
}

void write(const char *name, uint32_t data)
{
    write_system_call(name, data);
}
