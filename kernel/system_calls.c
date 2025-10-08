#include "system_calls.h"

void exit_system_call()
{
	__asm__ volatile("push ecx"); // save ecx value
	__asm__ volatile("push ebx"); // save ebx value
	__asm__ volatile("push edx"); // save edx value

	__asm__ volatile("mov edx, 1"); // edx is syscall number
	__asm__ volatile("int 0x80"); // syscall

	__asm__ volatile("pop edx"); // return edx value
	__asm__ volatile("pop ebx"); // return ebx value
	__asm__ volatile("pop ecx"); // return ecx value
}

int fork_system_call()
{
	int ret;

	__asm__ volatile("push ecx");
	__asm__ volatile("push ebx");
	__asm__ volatile("push edx");

	__asm__ volatile("mov edx, 2");   // syscall number: fork
	__asm__ volatile("int 0x80");    // invoke syscall
	__asm__ volatile("mov %%eax, %0" : "=r"(ret));  // read return value

	__asm__ volatile("pop edx");
	__asm__ volatile("pop ebx");
	__asm__ volatile("pop ecx");

	return ret;
}

void write_system_call(char* name, uint32_t bytes)
{
    if(bytes>0)
    {
		__asm__ volatile("push ecx"); // save ecx value
		__asm__ volatile("push ebx"); // save ebx value
		__asm__ volatile("push edx"); // save edx value

		__asm__ volatile("mov ebx,%0"::"r" (name)); // ebx is argument 1
		__asm__ volatile("mov edx, 4"); // edx is syscall number
		__asm__ volatile("mov ecx,%0"::"r" (bytes)); // ecx is argument 2
		__asm__ volatile("int 0x80"); // syscall

		__asm__ volatile("pop edx"); // return edx value
		__asm__ volatile("pop ebx"); // return ebx value
		__asm__ volatile("pop ecx"); // return ecx value
    }
}