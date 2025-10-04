.global el1_vector_table
.type el1_vector_table, %function
.align 11          // 2KB alignment
el1_vector_table:

    // 0x000: Sync from EL0 using SP_EL0
    b el1_sync_from_el0
    // 0x080: IRQ from EL0
    b el1_other
    // 0x100: FIQ from EL0
    b el1_other
    // 0x180: SError from EL0
    b el1_other

    // 0x200: Sync from EL1t
    b el1_other
    // 0x280: IRQ from EL1t
    b el1_other
    // 0x300: FIQ from EL1t
    b el1_other
    // 0x380: SError from EL1t
    b el1_other

    // Padding to reach 2KB alignment
    .rept (0x800 - 0x400) / 4
    nop
    .endr

.global el1_sync_from_el0
.type el1_sync_from_el0, %function
el1_sync_from_el0:
    // Allocate stack space
    sub sp, sp, #9*8          // 9 registers x8 bytes
    stp x0, x1, [sp, #0]
    stp x2, x3, [sp, #16]
    stp x4, x5, [sp, #32]
    stp x6, x7, [sp, #48]
    str x8, [sp, #64]         // syscall number

    mov x0, sp                // pass pointer to saved registers
    bl syscall_handler

    // Restore registers
    ldp x0, x1, [sp, #0]
    ldp x2, x3, [sp, #16]
    ldp x4, x5, [sp, #32]
    ldp x6, x7, [sp, #48]
    ldr x8, [sp, #64]

    add sp, sp, #9*8
    eret

.global el1_other
el1_other:
    b el1_other
