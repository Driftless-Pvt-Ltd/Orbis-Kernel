void _start()
{
        __asm__ volatile("push ecx"); // save ecx value
        __asm__ volatile("push ebx"); // save ebx value
        __asm__ volatile("push edx"); // save edx value

        __asm__ volatile("mov edx, 7"); // edx is syscall number
        __asm__ volatile("int 0x80"); // syscall

        __asm__ volatile("pop edx"); // return edx value
        __asm__ volatile("pop ebx"); // return ebx value
        __asm__ volatile("pop ecx"); // return ecx value

        __asm__ volatile("push ecx"); // save ecx value
        __asm__ volatile("push ebx"); // save ebx value
        __asm__ volatile("push edx"); // save edx value

        __asm__ volatile("mov edx, 1"); // edx is syscall number
        __asm__ volatile("int 0x80"); // syscall

        __asm__ volatile("pop edx"); // return edx value
        __asm__ volatile("pop ebx"); // return ebx value
        __asm__ volatile("pop ecx"); // return ecx value
}
