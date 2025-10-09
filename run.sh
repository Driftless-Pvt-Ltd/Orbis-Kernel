#!/bin/bash
find . -name "*.o" -type f -delete #clean
find . -name "*.bin" -type f -delete
find . -name "*.tmp" -type f -delete

cd ./apps/dummy
make
xxd -i demo.bin > demo_bin.h
cp -f demo_bin.h ../../kernel/demo_bin.h
rm demo_bin.h

cd ../../kernel
CFiles=`find . -name "*.c" -type f` #get all c files

cd ../boot #compile asm files
nasm main.asm -f bin -o ../boot_sect.bin

cd ../kernel #compile kernel files
for eachfile in $CFiles
do
    gcc -m32 -masm=intel -fno-pie -ffreestanding -c -mgeneral-regs-only $eachfile -o $eachfile.o
done

OFiles=`find . -name "*.o" -type f` #get all o files
OFiles=${OFiles/"./kernel.c.o"/}
OFiles="./kernel.c.o ${OFiles}" #force kernel.c.o to be first

ld -m elf_i386 -o ../kernel.bin -Ttext 0xd000 $OFiles --oformat binary #link

cd ..
cat boot_sect.bin kernel.bin > os-image # make image
truncate -s 16M os-image #force the image to have enouh bytes, so the read-disk instruction will not overflow.

qemu-system-x86_64 -monitor stdio -D ./log.txt -d int -m 4000 os-image

find . -name "*.o" -type f -delete #clean
find . -name "*.bin" -type f -delete
find . -name "*.tmp" -type f -delete
find . -name "log.txt" -type f -delete
