all:
	aarch64-none-elf-gcc -nostdlib -ffreestanding -O2 -Wall -Wextra -I. -c startup/startup.S -o startup.o
	aarch64-none-elf-g++ -nostdlib -ffreestanding -fno-rtti -fno-exceptions -fno-threadsafe-statics -O2 -Wall -Wextra -I. -c startup/main.cpp -o main.o
	aarch64-none-elf-ld -T link.ld startup.o main.o -o os.elf

	truncate -s 0 /dev/shm/qemu_fb
	dd if=/dev/zero bs=1M count=64 of=/dev/shm/qemu_fb

	qemu-system-aarch64 -M virt -cpu cortex-a53 -nographic -serial mon:stdio -m 64M -kernel os.elf \
	  -object memory-backend-file,id=mem,mem-path=/dev/shm/qemu_fb,share=on,size=64M \
	  -numa node,memdev=mem &

	./viewer

	rm main.o os.elf startup.o