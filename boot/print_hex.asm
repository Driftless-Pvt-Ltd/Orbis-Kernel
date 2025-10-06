print_hex: ;dx will have 16bit that we want to print. will print 2 ascii based on dl and dh.
pusha
mov ax,dx ;save dx on ax
and dx,0x00ff
mov [temp_string],dx
mov bx,temp_string
call print_string
mov dx,ax ; restore dx value
and dx,0xff00
shr dx,8
mov [temp_string],dx
mov bx,temp_string
call print_string
popa
ret

temp_string: dw 0x0000, 0