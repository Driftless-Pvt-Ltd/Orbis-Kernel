#ifndef common_definitions_header
#define common_definitions_header

// memory definitions
#define DIRECTORY_TABLE_MEMORY_LOCATION 0xd00000
#define PAGE_TABLES_MEMORY_LOCATION 0xe00000
#define KERNEL_STACK_MEMORY_LOCATION 0x50000000 // interrupts - defined in the tss
#define USER_STACK_MEMORY_LOCATION 0x20000000
#define MALLOC_MEMORY_LOCATION 0x60000000 // memory for malloc function.
#define KERNEL_FUNCTION_MEMORY_LOCATION 0xd000
#define MAIN_FUNCTION_STACK_MEMORY_LOCATION 0x16000 // defined in switch_to_pm.asm
#define SCREEN_MMIO_LOCATION 0xb8000

#define VGA_ADDRESS 0xA0000
#define VGA_MAX 0xF9FF
#define VGA_MAX_WIDTH 320
#define VGA_MAX_HEIGHT 200

enum vga_color {
  BLACK,
  BLUE,
  GREEN,
  CYAN,
  RED,
  MAGENTA,
  BROWN,
  GREY,
  DARK_GREY,
  BRIGHT_BLUE,
  BRIGHT_GREEN,
  BRIGHT_CYAN,
  BRIGHT_RED,
  BRIGHT_MAGENTA,
  YELLOW,
  WHITE,
};

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t color;
} screen_write_t;

/* Attribute Controller Registers */
#define	VGA_AC_INDEX 0x3C0
#define	VGA_AC_READ 0x3C1
#define	VGA_AC_WRITE 0x3C0

/* Miscellaneous Output */
#define	VGA_MISC_READ 0x3CC
#define	VGA_MISC_WRITE 0x3C2

/* Sequencer Registers */
#define VGA_SEQ_INDEX 0x3C4
#define VGA_SEQ_DATA 0x3C5

/* VGA Color Palette Registers */
#define	VGA_DAC_READ_INDEX 0x3C7
#define	VGA_DAC_WRITE_INDEX 0x3C8
#define	VGA_DAC_DATA 0x3C9

/* Graphics Controller Registers */
#define VGA_GC_INDEX 0x3CE
#define VGA_GC_DATA 0x3CF

/* CRT Controller Registers */
#define VGA_CRTC_INDEX 0x3D4
#define VGA_CRTC_DATA 0x3D5

/* General Control and Status Registers */
#define	VGA_INSTAT_READ 0x3DA

//task definitions
#define KERNEL_TASK_CODE 0
typedef enum task_status{  alive=1,  waiting=2,  dead=0  } task_status;

void exit_process();
int fork_process();

#endif
