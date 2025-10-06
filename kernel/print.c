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