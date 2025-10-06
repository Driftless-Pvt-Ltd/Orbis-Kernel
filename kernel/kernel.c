#include "print.h"
#include "idt.h"
#include "paging.h"

__asm__("call main\n\t"  // jump to main always and hang
        "jmp $");

#define MAX_TASKS 10

// Forward declarations of processes
void process1();
void process2();
void process3();

// Scheduler prototype
void scheduler();

// Task list and count
void (*processes[MAX_TASKS])();
int process_count = 0;

int current_process = 0;

// Add a new task to the scheduler
int add_task(void (*task)()) {
    if (process_count >= MAX_TASKS) {
        print_system_call("Task list full\n", 15);
        return -1; // fail
    }
    processes[process_count++] = task;
    return 0; // success
}

// Remove a task (by function pointer)
int remove_task(void (*task)()) {
    int found = -1;
    for (int i = 0; i < process_count; i++) {
        if (processes[i] == task) {
            found = i;
            break;
        }
    }
    if (found == -1) return -1; // not found

    // Shift remaining tasks left
    for (int i = found; i < process_count - 1; i++) {
        processes[i] = processes[i + 1];
    }
    process_count--;

    // Fix current_process index if needed
    if (current_process >= process_count) {
        current_process = 0;
    }

    return 0; // success
}

void scheduler()
{
    while (1)
    {
        if (process_count == 0) {
            print_system_call("No tasks to run\n", 16);
            while (1); // hang
        }

        int proc = current_process;
        current_process = (current_process + 1) % process_count;
        processes[proc]();
    }
}

void process1() {
    print_system_call("A", 1);
}

void process2() {
    print_system_call("B", 1);
}

void process3() {
    print_system_call("C", 1);
}

void main () 
{
    clear_screen();

    log_info("booting");
    idt_init();
    paging_init();

    log_debug("entering ring 3");
    #include "enter_user_mode.inc"

    add_task(process1);
    add_task(process2);
    add_task(process3);

    scheduler();
    
    while(1);
}