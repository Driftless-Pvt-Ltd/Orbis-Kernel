__asm__ volatile ("cli");
__asm__ volatile ("mov ax, 0x2B");// Load the index of our TSS structure - The index is
                     // 0x28, as it is the 5th selector and each is 8 bytes
                     // long, but we set the bottom two bits (making 0x2B)
                     // so that it has an RPL of 3, not zero.
__asm__ volatile ("ltr ax"); //Load 0x2B into the task state register.
__asm__ volatile ("mov ax, 0x23");	// user mode data selector is 0x20 (GDT entry 3). Also sets RPL to 3
__asm__ volatile ("mov ds, ax");
__asm__ volatile ("mov es, ax");
__asm__ volatile ("mov fs, ax");
__asm__ volatile ("mov gs, ax");
__asm__ volatile ("push 0x23");		// SS, notice it uses same selector as above
__asm__ volatile ("push esp");		// ESP
__asm__ volatile ("pushfd");			// EFLAGS
__asm__ volatile ("pop eax"); // Get EFLAGS back into EAX. The only way to read EFLAGS is to pushf then pop.
__asm__ volatile ("or eax, 0x200"); // Set the IF flag ( to enable interrupts after IRET)
__asm__ volatile ("push eax"); // Push the new EFLAGS value back onto the stack.
__asm__ volatile ("push 0x1b");		// CS, user mode code selector is 0x18. With RPL 3 this is 0x1b
__asm__ volatile ("lea eax, [a]");		// EIP first
__asm__ volatile ("push eax");
__asm__ volatile ("iretd"); //force user mode
__asm__ volatile ("a:");
__asm__ volatile ("add esp, 4"); // fix stack