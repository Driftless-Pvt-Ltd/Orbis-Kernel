all:
	# nasm -f bin main.asm -o demo.bin
	gcc -m32 -masm=intel -fno-pie -ffreestanding -c -mgeneral-regs-only -o demo.o -c demo.c
	ld -m elf_i386 -o demo.bin -Ttext 0x1000 --oformat binary demo.o