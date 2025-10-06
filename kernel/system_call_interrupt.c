#include "system_call_interrupt.h"
#include "common_defintions.h"

#define SYS_PRINT 0

// XNU System Calls
// Source: https://gist.github.com/BlueFalconHD/da39227ec22908c8e9d12e458a5155d4
#define SYS_EXIT 1
#define SYS_FORK 2

void system_call_handler(void* x)
{
    uint32_t system_call_number;
    uint32_t arg1;
    uint32_t arg2;
    uint32_t return_value = 0;

    // load arguments
    __asm__ volatile ("cli");
    __asm__ volatile ("mov %0,edx": "=r" (system_call_number)); // EDX=syscall num
    __asm__ volatile ("mov %0,ecx": "=r" (arg2)); // ECX=arg2
    __asm__ volatile ("mov %0,ebx": "=r" (arg1)); // EBX=arg1

    switch(system_call_number)
    {
        case SYS_PRINT:
            print((const char *)arg1, arg2);
            break;

        case SYS_EXIT:
            exit_process();
            break;

        case SYS_FORK:
            return_value = fork_process();
            break;

        default:
            log_error("unknown syscall number");
            break;
    }

    // Set return value to EAX
    __asm__ volatile ("mov %0, %%eax" : : "r"(return_value));
}
