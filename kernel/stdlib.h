#ifndef LIBBY_H
#define LIBBY_H

#include "system_calls.h"

void *memset(void *dst, int value, unsigned int size);
void *memcpy(void *dst, const void *src, unsigned int size);
int strcmp(const char *a, const char *b);
unsigned int strlen(const char *str);
char *strcpy(char *dest, const char *src);

void puts(const char *str);
void exit(int code);
int fork();
void write(const char *name, uint32_t data);

#endif