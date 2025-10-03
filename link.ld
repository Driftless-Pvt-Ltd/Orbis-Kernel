ENTRY(_start)

MEMORY
{
    RAM (rwx) : ORIGIN = 0x40000000, LENGTH = 64M
}

SECTIONS
{
    /* Start at beginning of RAM */
    . = ORIGIN(RAM);

    /* Kernel text/code */
    .text : {
        *(.text*)
    } > RAM

    /* Read-only data */
    .rodata : {
        *(.rodata*)
    } > RAM

    /* Initialized data */
    .data : {
        *(.data*)
    } > RAM

    /* Uninitialized data (bss) */
    .bss : {
        *(.bss*)
        *(COMMON)
    } > RAM

    /* Align to next 4K page */
    . = ALIGN(0x1000);

    /* Kernel end marker for PMM */
    _kernel_end = .;

    /* Framebuffer reservation (240*320*2 = 153600 bytes) */
    __fb_base = .;
    . = . + (240*320*2);
    __fb_end = .;

    /* Optionally reserve space for PMM bitmap */
    __pmm_bitmap_base = .;
    . = . + (1048576 / 8);  /* 1 bit per page, MAX_PAGES = 1M */
    __pmm_bitmap_end = .;

    /* Align to next 4K page again */
    . = ALIGN(0x1000);

    /* End of usable memory marker for PMM initialization */
    __phys_mem_start = .;
    __phys_mem_end   = ORIGIN(RAM) + LENGTH(RAM);
}
