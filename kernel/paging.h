#ifndef paging_header
#define paging_header
#include <stdint.h> //for type definitions
#include "print.h"
#include <stdbool.h> // for bool usage
#include "common_defintions.h"

#define PAGE_DIRECTORY_MASK 0b11111111110000000000000000000000
#define PAGE_TABLE_MASK 0b00000000001111111111000000000000
#define PAGE_OFFSET_MASK 0b00000000000000000000111111111111
#define PAGE_SIZE_BYTES 4096
#define PAGE_SIZE_4BYTES 1024

typedef struct{
    uint32_t present_flag:1;
    uint32_t read_write_flag:1;
    uint32_t user_kernel_mode_flag:1;
    uint32_t reserved_1:1;
    uint32_t reserved_2:1;
    uint32_t access_flag:1;
    uint32_t dirty_flag:1;
    uint32_t reserved_3:1;
    uint32_t reserved_4:1;
    uint32_t available_1:1;
    uint32_t available_2:1;
    uint32_t available_3:1;
    uint32_t frame:20;
} page_entry;

typedef struct{
    uint32_t present_flag:1;
    uint32_t read_write_flag:1;
    uint32_t user_kernel_mode_flag:1;
    uint32_t write_through_flag:1;
    uint32_t cache_flag:1;
    uint32_t access_flag:1;
    uint32_t reserved_1:1;
    uint32_t page_size_flag:1;
    uint32_t global_flag:1;
    uint32_t available_1:1;
    uint32_t available_2:1;
    uint32_t available_3:1;
    uint32_t frame:20;
} directory_entry;


extern page_entry* page_table; // size 1024*1024
extern directory_entry* directory_table; // size 1024

void create_page_entry(page_entry*,uint32_t);
void create_page_table(page_entry*,uint32_t);
void create_directory_entry(directory_entry*,uint32_t);
void create_directory_table(page_entry*,uint32_t);

void paging_init();
uint32_t get_physical_location(uint32_t);

#endif