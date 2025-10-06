#include "system_call_interrupt.h"

#define SYS_PRINT 0

void system_call_handler(void* x)
{
    uint32_t system_call_number;
    uint32_t arg1;
    uint32_t arg2;
    __asm__ volatile ("cli");
    __asm__ volatile ("mov %0,edx": "=r" (system_call_number)); // EDX=syscall num
    __asm__ volatile ("mov %0,ecx": "=r" (arg2)); // ECX=arg2
    __asm__ volatile ("mov %0,ebx": "=r" (arg1)); // EBX=arg1
    switch(system_call_number)
    {
        case SYS_PRINT:
            print((const char *)arg1, arg2);
            break;

        default:
            log_error("unknown syscall number");
            break;
    }

}
