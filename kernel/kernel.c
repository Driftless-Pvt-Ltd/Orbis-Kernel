#include "print.h"
#include "idt.h"
#include "paging.h"

__asm__("call main\n\t"  // jump to main always and hang
        "jmp $");

void main () 
{
    clear_screen();
    idt_init();
    paging_init();

    #include "enter_user_mode.inc"

    print_system_call("abc",3);
    
    while(1);
}