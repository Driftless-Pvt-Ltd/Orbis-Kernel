#include "system_call_interrupt.h"
#include "common_defintions.h"
#include "stdlib.h"
#include "ramfs.h"

#define SYS_EXIT 1
#define SYS_FORK 2
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6

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
        case SYS_EXIT:
            exit_process();
            break;

        case SYS_FORK:
            return_value = fork_process();
            break;

        case SYS_WRITE:
        {
            const char *name = (const char *)arg1;

            // check for specific devices
            if (strcmp(name, "device_screen") == 0)
            {
                log_error("write(): no screen initialized yet");
            }
            if (strcmp(name, "device_tty") == 0)
            {
                const char *string = (const char *)arg2;
                print(string, strlen(string));
            }
            else
            {
                uint8_t *data = (uint8_t*)arg2;
                ramfs_write_file(name, data, sizeof(data), 0);
            }
            break;
        }

        default:
            log_error("unknown syscall number");
            break;
    }

    // Set return value to EAX
    __asm__ volatile ("mov %0, %%eax" : : "r"(return_value));
}
