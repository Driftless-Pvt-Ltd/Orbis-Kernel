[bits 16]
switch_to_pm: ; switch to pm - protected mode
cli ; switch off interrupts
lgdt [gdt_descriptor] ; load global descriptor table

mov eax,cr0
or eax,0x1 ; swtich on first bit of cr0, which indicates protected mode
mov cr0,eax 

jmp CODE_KERNEL_SEG:init_pm ; far jump to flush pipeline

[bits 32]
init_pm:
mov ax,DATA_KERNEL_SEG
mov ds, ax ; all descriptors now point to data segment descriptor in gdt
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

mov ebp, 0x16000
mov esp, ebp

call BEGIN_PM