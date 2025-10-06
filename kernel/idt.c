#include "idt.h"

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) // set specific interrupt in idt
{ 
    idt_entry_t* descriptor = &idt[vector];
 
    descriptor->isr_low        = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs      = 0x08; // offset kernel code selector in GDT
    descriptor->attributes     = flags;
    descriptor->isr_high       = (uint32_t)isr >> 16;
    descriptor->reserved       = 0;
}

void empty_int_func(void* x)
{
    __asm__ volatile ("cli");
    //print("im inside an interrupt",23);
}

void gpf_int_func(void* x)
{
    __asm__ volatile ("cli");
    print("GPF",3);
}

void idt_init() //init idt table
{
    initialize_pic();
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    for ( uint8_t i=0; i < IDT_MAX_DESCRIPTORS - 1; i++ ) // set every interrupt to empty_func
    {
        idt_set_descriptor(i,(void*)empty_int_func,IDT_FLAG_HW);
    }
    
    idt_set_descriptor(0x0d,(void*)gpf_int_func,IDT_FLAG_HW); // interrupt 14 GPF

    idt_set_descriptor(0x80,(void*)system_call_handler,IDT_FLAG_USER); // int 0x80 system call handler
    

    __asm__ volatile ("lidt %0" : : "m"(idtr)); // load the new IDT
    __asm__ volatile ("sti"); // set the interrupt flag
}

void initialize_pic()
{
    __asm__ volatile("push eax");

    __asm__ volatile("mov al, 0b00010001"); // icw1 - general
    __asm__ volatile("out 0x20, al"); // send to PIC
    __asm__ volatile("out 0xA0, al"); // send to secondary PIC

    __asm__ volatile("mov al, 0x20"); // icw2 - IVT number selector
    __asm__ volatile("out 0x21, al"); // send to PIC - IRQ 0..7<->0x20...0x27
    __asm__ volatile("mov al, 0x28"); // icw2 - IVT number selector
    __asm__ volatile("out 0xA1, al"); // send to secondary PIC - IRQ 8..15<->0x28...0x2f

    __asm__ volatile("mov al, 0x4"); // icw3 - which pin to connect the secondary PIC
    __asm__ volatile("out 0x21, al"); // 0x04 => 0100, second bit (IR line 2)
    __asm__ volatile("mov al, 0x2"); // icw3 - to secondary
    __asm__ volatile("out 0xA1, al");//010=> IR line 2
    
    __asm__ volatile("mov al, 1"); // icw4 - 80x86 mode
    __asm__ volatile("out 0x21, al"); // primary PIC
    __asm__ volatile("out 0xA1, al"); // secondary PIC

    __asm__ volatile("mov al, 0"); // null data registers
    __asm__ volatile("out 0x21, al"); 
    __asm__ volatile("out 0xA1, al"); 


    __asm__ volatile("pop eax");
}
