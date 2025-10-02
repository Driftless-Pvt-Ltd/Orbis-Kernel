.global _start
_start:
    // Set up stack pointer
    ldr x0, =stack_top
    mov sp, x0

    bl main

hang:
    b hang

.section .bss
    .space 0x1000
stack_bottom:
stack_top:
