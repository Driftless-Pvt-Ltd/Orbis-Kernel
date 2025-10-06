#include "system_calls.h"

void print_system_call(char* string, uint32_t bytes)
{
    if(bytes>0)
    {
		__asm__ volatile("push ecx"); // save ecx value
		__asm__ volatile("push ebx"); // save ebx value
		__asm__ volatile("push edx"); // save edx value

		__asm__ volatile("mov ebx,%0"::"r" (string)); // ebx is argument 1
		__asm__ volatile("mov edx, 0"); // edx is syscall number
		__asm__ volatile("mov ecx,%0"::"r" (bytes)); // ecx is argument 2
		__asm__ volatile("int 0x80"); // syscall

		__asm__ volatile("pop edx"); // return edx value
		__asm__ volatile("pop ebx"); // return ebx value
		__asm__ volatile("pop ecx"); // return ecx value
    }
}
