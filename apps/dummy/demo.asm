
bits 32

section .data
device_str db "device_tty", 0
msg db "Hello User Program!", 0

section .text
start:
    push ecx
    push ebx
    push edx

    mov ebx, device_str
    mov edx, 4
    mov ecx, msg

    int 0x80

    pop edx
    pop ebx
    pop ecx