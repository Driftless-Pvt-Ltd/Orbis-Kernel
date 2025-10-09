#include "print.h"

char* g_video_memory = (char*)SCREEN_MMIO_LOCATION;
#define SCREEN_END (SCREEN_MMIO_LOCATION + 4000)

static inline uint8_t inb(uint16_t port) {
    uint8_t value;

    __asm__ volatile("push ecx");
    __asm__ volatile("push ebx");
    __asm__ volatile("push edx");

    __asm__ volatile("mov dx, %0" :: "r"(port));
    __asm__ volatile("in al, dx");
    __asm__ volatile("mov %0, al" : "=r"(value));

    __asm__ volatile("pop edx");
    __asm__ volatile("pop ebx");
    __asm__ volatile("pop ecx");

    return value;
}

// write a byte to an I/O port
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile("push ecx");
    __asm__ volatile("push ebx");
    __asm__ volatile("push edx");

    __asm__ volatile("mov dx, %0" :: "r"(port));
    __asm__ volatile("mov al, %0" :: "r"(value));
    __asm__ volatile("out dx, al");

    __asm__ volatile("pop edx");
    __asm__ volatile("pop ebx");
    __asm__ volatile("pop ecx");
}

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

#define COM1_PORT 0x3F8  // QEMU default serial port
#define COM1_LSR  0x3FD  // Line Status Register

static inline void serial_write_char(char c) {
    // wait for Transmitter Holding Register empty
    while (!(inb(COM1_LSR) & 0x20));
    outb(COM1_PORT, c);
}

void print_char(char c)
{
    if (c == '\n')
    {
        int offset = g_video_memory - (char*)SCREEN_MMIO_LOCATION;
        int next_line_offset = ((offset / SCREEN_LINE_SIZE) + 1) * SCREEN_LINE_SIZE;
        g_video_memory = (char*)SCREEN_MMIO_LOCATION + next_line_offset;

        if (g_video_memory >= (char*)SCREEN_END)
            clear_screen();

        serial_write_char('\r'); // carriage return for serial
        serial_write_char('\n'); // newline
        return;
    }

    if (g_video_memory >= (char*)SCREEN_END)
        clear_screen();

    *g_video_memory++ = c;
    *g_video_memory++ = 0x0f;

    serial_write_char(c); // also send to serial
}

void print(const char* word, int length)
{
    for (int i = 0; i < length; i++)
        print_char(word[i]);
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

// a global VGA buffer
static uint8_t* g_vga_buffer;

/*
See Intel® OpenSource HD Graphics PRM pdf file
for following defined data for each vga register
and its explaination
*/
static uint8_t seq_data[5] = {0x03, 0x01, 0x0F, 0x00, 0x0E};
static uint8_t crtc_data[25] = {0x5F, 0x4F, 0x50, 0x82, 
                              0x54, 0x80, 0xBF, 0x1F,
                              0x00, 0x41, 0x00, 0x00, 
                              0x00, 0x00, 0x00, 0x00,
                              0x9C, 0x0E, 0x8F, 0x28,	
                              0x40, 0x96, 0xB9, 0xA3,
                              0xFF};

static uint8_t gc_data[9] = {0x00, 0x00, 0x00, 0x00, 
                          0x00, 0x40, 0x05, 0x0F,
                          0xFF};

static uint8_t ac_data[21] = {0x00, 0x01, 0x02, 0x03,
                            0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0A, 0x0B,
                            0x0C, 0x0D, 0x0E, 0x0F,
                            0x41, 0x00, 0x0F, 0x00,
                            0x00};

void set_misc()
{
  outb(VGA_MISC_WRITE, 0x63);
}

void set_seq()
{
  // write sequence data to index of 0-4
  for(uint8_t index = 0; index < 5; index++){
    // first set index
    outb(VGA_SEQ_INDEX, index);
    // write data at that index
    outb(VGA_SEQ_DATA, seq_data[index]);
  }
}

void set_crtc()
{
  // write crtc data to index of 0-24
  for(uint8_t index = 0; index < 25; index++){
    outb(VGA_CRTC_INDEX, index);
	  outb(VGA_CRTC_DATA, crtc_data[index]);
  }
}

void set_gc()
{
  // write gc data to index of 0-8
  for(uint8_t index = 0; index < 9; index++){
    outb(VGA_GC_INDEX, index);
    outb(VGA_GC_DATA, gc_data[index]);
  }
}

void set_ac()
{
  uint8_t d;
  // write ac data to index of 0-20
  for(uint8_t index = 0; index < 21; index++){
    outb(VGA_AC_INDEX, index);
    outb(VGA_AC_WRITE, ac_data[index]);
  }
  d = inb(VGA_INSTAT_READ);
  outb(VGA_AC_INDEX, d | 0x20);
}

void clear_screenVGA()
{
  for(uint32_t index = 0; index < VGA_MAX; index++)
    g_vga_buffer[index] = 0;
}

void clear_color(uint8_t color)
{
  for(uint32_t index = 0; index < VGA_MAX; index++)
    g_vga_buffer[index] = color;
}

void init_vga()
{
  set_misc();
  set_seq();
  set_crtc();
  set_gc();
  set_ac();

  g_vga_buffer = (uint8_t*)VGA_ADDRESS;

  clear_screenVGA();
}

void putpixel_vga(uint16_t x, uint16_t y, uint8_t color)
{
  uint32_t index = 0;
  index = 320 * y + x;
  if(index < VGA_MAX)
    g_vga_buffer[index] = color;
}