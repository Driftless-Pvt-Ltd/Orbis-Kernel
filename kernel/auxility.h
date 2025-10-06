#ifndef auxility_header
#define auxility_header

#include "print.h"
#include <stdint.h> //for type definitions

#define PRINT_ESP() \
uint32_t my_esp;\
__asm__ volatile ("mov %0, esp":"=r" (my_esp));\
 print_hex(&my_esp,4);

#define PRINT_EBP() \
uint32_t my_ebp;\
__asm__ volatile ("mov %0, ebp":"=r" (my_ebp));\
 print_hex(&my_ebp,4);

#define PRINT_EIP()\
uint32_t my_eip;\
__asm__ volatile ("Call $+5");\
__asm__ volatile ("pop %0":"=r" (my_eip));\
print_hex(&my_eip,4);

#endif