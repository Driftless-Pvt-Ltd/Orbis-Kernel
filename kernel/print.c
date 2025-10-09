#include "print.h"

char* g_video_memory = (char*)SCREEN_MMIO_LOCATION;
#define SCREEN_END (SCREEN_MMIO_LOCATION + 4000)

void clear_screen()
{
    int i;
    for(i = SCREEN_MMIO_LOCATION; i < SCREEN_END;)
    {
        *((char*)i++) = 0x00;
        *((char*)i++) = 0x00;
    }
    g_video_memory = (char*)SCREEN_MMIO_LOCATION;
}

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define SCREEN_LINE_SIZE (SCREEN_WIDTH * 2)  // 160 bytes per line

void print_char(char c)
{
    if (c == '\n')
    {
        // Move g_video_memory to next line start
        int offset = g_video_memory - (char*)SCREEN_MMIO_LOCATION;
        int next_line_offset = ((offset / SCREEN_LINE_SIZE) + 1) * SCREEN_LINE_SIZE;
        g_video_memory = (char*)SCREEN_MMIO_LOCATION + next_line_offset;

        if (g_video_memory >= (char*)SCREEN_END)
        {
            clear_screen();
        }
        return;
    }

    if (g_video_memory >= (char*)SCREEN_END)
    {
        clear_screen();
    }

    *g_video_memory++ = c;      // write character
    *g_video_memory++ = 0x0f;   // white on black
}

void print(const char* word, int length)
{
    int i;
    for(i = 0; i < length; i++)
    {
        if(word[i] == '\n')
        {
            // Move g_video_memory to next line start
            // Calculate current position relative to SCREEN_MMIO_LOCATION
            int offset = g_video_memory - (char*)SCREEN_MMIO_LOCATION;
            // Compute next line start (round up to next multiple of SCREEN_LINE_SIZE)
            int next_line_offset = ((offset / SCREEN_LINE_SIZE) + 1) * SCREEN_LINE_SIZE;
            g_video_memory = (char*)SCREEN_MMIO_LOCATION + next_line_offset;

            if(g_video_memory >= (char*)SCREEN_END)
            {
                clear_screen();
            }
            continue;
        }

        if(g_video_memory >= (char*)SCREEN_END)
        {
            clear_screen();
        }

        *g_video_memory++ = word[i];
        *g_video_memory++ = 0x0f; // white on black
    }
}

void delete_char()
{
    if(g_video_memory > (char*)SCREEN_MMIO_LOCATION)
    {
        g_video_memory--;
        *g_video_memory = 0x00;
        g_video_memory--;
        *g_video_memory = 0x00;
    }
}

char hex_chars[] = "0123456789ABCDEF";

void print_byte_hex(char byte)
{
    char buf[2];
    buf[0] = hex_chars[(byte >> 4) & 0xF];
    buf[1] = hex_chars[byte & 0xF];
    print(buf, 2);
}

void print_hex(const char* bytes, int length)
{
    int i;
    if(length == 4)
    {
        // print in little-endian (reverse)
        for(i = length - 1; i >= 0; i--)
            print_byte_hex(bytes[i]);
    }
    else
    {
        for(i = 0; i < length; i++)
            print_byte_hex(bytes[i]);
    }
}

#define LOG_MODULE "KERN"

void log_error(const char* msg)
{
    print_string("<ERR>: ");
    print_string(LOG_MODULE);
    print_string(": ");
    print_string(msg);
    print_string("\n");
}

void log_warn(const char* msg)
{
    print_string("<WARN>: ");
    print_string(LOG_MODULE);
    print_string(": ");
    print_string(msg);
    print_string("\n");
}

void log_info(const char* msg)
{
    print_string("<INFO>: ");
    print_string(LOG_MODULE);
    print_string(": ");
    print_string(msg);
    print_string("\n");
}

void log_debug(const char* msg)
{
    print_string("<DEBUG>: ");
    print_string(LOG_MODULE);
    print_string(": ");
    print_string(msg);
    print_string("\n");
}

void print_string(const char* str)
{
    int len = string_length(str);
    print((char*)str, len);
}

int string_length(const char* str)
{
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

TTY_Pixel tty_buffer[TTY_HEIGHT][TTY_WIDTH];

#define COLOR_BLACK 0x0
#define COLOR_BLUE 0x1
#define COLOR_GREEN 0x2
#define COLOR_CYAN 0x3
#define COLOR_RED 0x4
#define COLOR_MAGENTA 0x5
#define COLOR_BROWN 0x6
#define COLOR_LIGHT_GRAY 0x7
#define COLOR_DARK_GRAY 0x8
#define COLOR_LIGHT_BLUE 0x9
#define COLOR_LIGHT_GREEN 0xA
#define COLOR_LIGHT_CYAN 0xB
#define COLOR_LIGHT_RED 0xC
#define COLOR_LIGHT_MAGENTA 0xD
#define COLOR_YELLOW 0xE
#define COLOR_WHITE 0xF

// Draw a “pixel” (space with color) at x,y
void tty_draw_pixel(int x, int y, uint8_t color) {
    if (x < 0 || x >= TTY_WIDTH || y < 0 || y >= TTY_HEIGHT)
        return;
    tty_buffer[y][x].c = ' ';
    tty_buffer[y][x].color = color;
}

// Flush framebuffer to VGA memory
void tty_flush() {
    char* video = (char*)SCREEN_MMIO_LOCATION;
    for (int y = 0; y < TTY_HEIGHT; y++) {
        for (int x = 0; x < TTY_WIDTH; x++) {
            *video++ = tty_buffer[y][x].c;
            *video++ = tty_buffer[y][x].color;
        }
    }
}

// Clear framebuffer
void tty_clear() {
    for (int y = 0; y < TTY_HEIGHT; y++) {
        for (int x = 0; x < TTY_WIDTH; x++) {
            tty_buffer[y][x].c = ' ';
            tty_buffer[y][x].color = COLOR_BLACK | (COLOR_LIGHT_GRAY << 4);
        }
    }
    tty_flush();
}

// Demo rainbow screen
void tty_demo() {
    tty_clear();
    for (int y = 0; y < TTY_HEIGHT; y++) {
        for (int x = 0; x < TTY_WIDTH; x++) {
            uint8_t fg = (x + y) % 16;           // foreground color
            uint8_t bg = (15 - (x + y) % 16);    // background color
            tty_draw_pixel(x, y, fg | (bg << 4));
        }
    }
    tty_flush();
}