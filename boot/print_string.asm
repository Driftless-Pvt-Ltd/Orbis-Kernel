print_string: ; input : bx will have the address of the start of the string. must end with 0. prints the string.
pusha
mov ah,0x0e
mov al,[bx]
xor al,0
jne else
popa
ret
else:
int 0x10
inc bx
jmp print_string+1 ; +1 to jump over pusha