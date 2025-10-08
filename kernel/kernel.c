#include "print.h"
#include "idt.h"
#include "paging.h"
#include "ramfs.h"

__asm__("call main\n\t"  // jump to main always and hang
        "jmp $");

#define MAX_PROCESSES 10

typedef enum {
    PROC_RUNNING,
    PROC_READY,
    PROC_TERMINATED
} proc_status_t;

typedef struct {
    int pid;
    char* name;
    void (*entry_point)();
    proc_status_t status;
} process_t;

process_t process_table[MAX_PROCESSES];
int process_count = 0;
int current_index = 0;
int next_pid = 1;

// add a process
int add_process(char* name, void (*entry_point)()) {
    if (process_count >= MAX_PROCESSES) {
        print_system_call("Process table full\n", 19);
        return -1;
    }

    process_table[process_count].pid = next_pid++;
    process_table[process_count].name = name;
    process_table[process_count].entry_point = entry_point;
    process_table[process_count].status = PROC_READY;
    return process_count++;
}

// Mark process as terminated
void exit_process() {
    process_table[current_index].status = PROC_TERMINATED;

    // move last process into current slot
    if (current_index != process_count - 1) {
        process_table[current_index] = process_table[process_count - 1];
    }

    process_count--;
    if (current_index >= process_count) current_index = 0;
}

int fork_process() {
    if (process_count >= MAX_PROCESSES) {
        print_system_call("Fork failed: process table full\n", 33);
        return -1;
    }

    process_t* parent = &process_table[current_index];
    process_t* child = &process_table[process_count];

    child->pid = next_pid++;
    child->name = parent->name; // You could append "-child" if you wanted
    child->entry_point = parent->entry_point;
    child->status = PROC_READY;

    print_system_call("Forked process\n", 15);
    return child->pid;
}

// round-robin scheduler
void scheduler() {
    while (1) {
        if (process_count == 0) {
            print_system_call("No processes\n", 13);
            while (1);
        }

        process_t* proc = (void*)0;
        int start_index = current_index;

        do {
            current_index = (current_index + 1) % process_count;
            proc = &process_table[current_index];
        } while (proc->status != PROC_READY && current_index != start_index);

        if (proc->status != PROC_READY) {
            print_system_call("No runnable processes\n", 24);
            while (1);
        }

        proc->status = PROC_RUNNING;
        proc->entry_point();

        if (proc->status == PROC_RUNNING)
            proc->status = PROC_READY;  // if not exited, mark ready again
    }
}

void launchd()
{
    // initialize ramfs
    ramfs_init();
    ramfs_demo();

    exit_system_call();
}

void idle()
{
    // ...
}

void main()
{
    clear_screen();

    log_info("booting");
    idt_init();
    paging_init();

    log_debug("entering ring 3");
    #include "enter_user_mode.inc"

    add_process("launchd", launchd);
    add_process("idle", idle);

    scheduler();
    
    while(1);
}