#include "system_call_interrupt.h"

//this is system_call_handel(int 0x80), edx register indicates which function the user wanted to do.
void system_call_handler(void* x)
{
    uint32_t system_call_number;
    char* char_pointer; // for printing sys call - pointer to string
    uint32_t string_length; // for printing sys call - length of string
    __asm__ volatile ("cli");
    __asm__ volatile ("mov %0,edx": "=r" (system_call_number)); // get the last edx, which indicates which system call it is
    switch(system_call_number)
    {
        case 1: // print interrupt
        	__asm__ volatile ("mov %0,ecx": "=r" (string_length)); // get the last ecx - which indicates string length
            __asm__ volatile ("mov %0,ebx": "=r" (char_pointer)); // get pointer of char to print
            print(char_pointer,string_length);
            break;


        default:
            break;
    }

}
