ENTRY(_start)

MEMORY
{
    RAM (rwx) : ORIGIN = 0x40000000, LENGTH = 64M
}

SECTIONS
{
    . = ORIGIN(RAM);

    .text : {
        *(.text*)
    } > RAM

    .rodata : {
        *(.rodata*)
    } > RAM

    .data : {
        *(.data*)
    } > RAM

    .bss : {
        *(.bss*)
        *(COMMON)
    } > RAM

    /* Align to next 4K page */
    . = ALIGN(0x1000);

    /* Mark end of kernel */
    _kernel_end = .;

    /* Reserve framebuffer space (240*320*2 bytes) */
    __fb_base = .;
    . = . + (240 * 320 * 2);
    __fb_end = .;
}
