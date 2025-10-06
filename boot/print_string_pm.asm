[bits 32]

VIDEO_MEMORY equ 0xb8000 ; constant for mmio video(will always print on start sadly)
WHITE_ON_BLACK equ 0x0f ; constant for print color


print_string_pm: ; prints a null-terminated string pointed by EBX
pusha
mov edx, VIDEO_MEMORY
print_string_pm_loop:
mov al,[ebx]
mov ah,WHITE_ON_BLACK
xor al,0
je print_string_pm_end
mov word [edx],ax
add edx, 2
add ebx, 1
jmp print_string_pm_loop
print_string_pm_end:
popa
ret