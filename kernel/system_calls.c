#include "system_calls.h"

// preparing the registers for the int 0x80 interrupt
void print_system_call(char* string, uint32_t bytes)
{
    if(bytes>0)
    {
		__asm__ volatile("push ecx"); // save ecx value
		__asm__ volatile("push ebx"); // save ebx value
		__asm__ volatile("push edx"); // save edx value


		__asm__ volatile("mov ebx,%0"::"r" (string)); // put the next char location inside ebx
		__asm__ volatile("mov edx,1"); // eax= 1 means print interrupt
		__asm__ volatile("mov ecx,%0"::"r" (bytes)); // put the string length inside ecx
		__asm__ volatile("int 0x80"); // call system call interrupt

		__asm__ volatile("pop edx"); // return edx value
		__asm__ volatile("pop ebx"); // return ebx value
		__asm__ volatile("pop ecx"); // return ecx value
    }
}
