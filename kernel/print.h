#ifndef print_header
#define print_header

#include <stdint.h> //for type definitions

#include "common_defintions.h"

#define MAX_VIDEO_MEM 4000

extern char* g_video_memory;

typedef struct {
    char c;
    uint8_t color;
} TTY_Pixel;

#define TTY_WIDTH 80
#define TTY_HEIGHT 25

extern TTY_Pixel tty_buffer[TTY_HEIGHT][TTY_WIDTH];

void clear_screen();// called by system call interrupt handler ( int 0x80- aex = 1 )
void print(const char* word, int length);
void print_char(char c);
void print_hex(const char* bytes, int length);
void log_error(const char* msg);
void log_warn(const char* msg);
void log_info(const char* msg);
void log_debug(const char* msg);

#endif