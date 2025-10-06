disk_load:
pusha
push dx
mov ah,0x02 ; BOIS read sector
mov al,dh ; read DH sector
mov ch,0x00 ; cylinder 0
mov dh,0x00 ; head 0
mov cl,0x02 ; second sector ( after boot)
int 0x13 ; BIOS interrupt
jc disk_error
pop dx
cmp dh,al
jne disk_error_2
popa
ret

disk_error:
mov bx, error_disk_message
call print_string
popa
ret
disk_error_2:
mov bx, error_disk_message_2
call print_string
popa
ret

error_disk_message: db 'bad sector!',0
error_disk_message_2: db 'bad sector_2!',0
