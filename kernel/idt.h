#include <stdint.h> //for type definitions

#ifndef idt_header
#define idt_header

#include "system_call_interrupt.h"
#include "print.h"

#define IDT_MAX_DESCRIPTORS 256
#define IDT_FLAG_HW 0x8E
#define IDT_FLAG_USER 0xEE

typedef struct 
{
	uint16_t    isr_low;      // The lower 16 bits of the ISR's address
	uint16_t    kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t     reserved;     // Set to zero
	uint8_t     attributes;   // Type and attributes;
	uint16_t    isr_high;     // The higher 16 bits of the ISR's address
} __attribute__((packed)) idt_entry_t;

typedef struct //IDTR structure
{ 
	uint16_t	limit;
	uint32_t	base;
} __attribute__((packed)) idtr_t;

__attribute__((aligned(0x10)))static idt_entry_t idt[256]; // Create an array of IDT entries; aligned for performance
static idtr_t idtr;

void empty_int_func(void*) __attribute__((interrupt)); // empty interrupt handler
void gpf_int_func(void*) __attribute__((interrupt)); // GPF interrupt handler

void idt_init(); //init idt with appropriate functions
void initialize_pic(); // function to initialize the PIC to send IRQ0...7 to 0x20 vector instead of 0x8.

#endif