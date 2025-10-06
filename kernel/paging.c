#include "paging.h"
page_entry* page_table = (page_entry*)PAGE_TABLES_MEMORY_LOCATION; // page tables location
directory_entry* directory_table = (directory_entry*)DIRECTORY_TABLE_MEMORY_LOCATION; // directory table location



void create_page_entry(page_entry* page_entry_to_allocate,uint32_t physical_memory)
{
    if(physical_memory % 0x1000 == 0)
    {
        uint32_t memory_to_frame = (uint32_t)(((uint32_t)(physical_memory)>>12));
        page_entry_to_allocate->frame = memory_to_frame;
        page_entry_to_allocate->present_flag = 1;
        page_entry_to_allocate->read_write_flag = 1;
        page_entry_to_allocate->user_kernel_mode_flag = 1;
        page_entry_to_allocate->access_flag = 0;
        page_entry_to_allocate->dirty_flag = 0;
    }
}
void create_page_table(page_entry* page_table_to_allocate,uint32_t physical_memory)
{
    if(physical_memory % 0x1000 == 0)
    {
        for(int i=0;i<1024;++i)
        {
            uint32_t current_page_memory_location = (uint32_t)(page_table_to_allocate)+i*4;
            create_page_entry((page_entry*)current_page_memory_location,physical_memory+i*0x1000);
        }
    }
}

void create_directory_entry(directory_entry* directory_entry_to_allocate,uint32_t page_table_memory)
{
    if(page_table_memory % 0x1000 == 0)
    {
        uint32_t memory_to_frame = (uint32_t)(((uint32_t)(page_table_memory)>>12));
        directory_entry_to_allocate->frame = memory_to_frame;
        directory_entry_to_allocate->present_flag = 1;
        directory_entry_to_allocate->read_write_flag = 1;
        directory_entry_to_allocate->user_kernel_mode_flag = 1;
        directory_entry_to_allocate->write_through_flag = 0;
        directory_entry_to_allocate->cache_flag = 0;
        directory_entry_to_allocate->access_flag = 0;
        directory_entry_to_allocate->page_size_flag = 0;
    }
}

void create_directory_table(page_entry* directory_table_to_allocate,uint32_t page_table_memory)
{
    if(page_table_memory % 0x1000 == 0)
    {
        for(int i=0;i<1024;++i)
        {
            uint32_t current_directory_memory_location = (uint32_t)(directory_table_to_allocate)+i*4;
            uint32_t current_page_table_memory = page_table_memory+i*0x1000;
            create_directory_entry((directory_entry*)current_directory_memory_location,current_page_table_memory);
        }
    }
}


void paging_init()
{
    __asm__ volatile("cli");

    log_debug("initializing page tables");

    for(int i =0 ;i<1024;++i) // create 1024 on 1024 page tables for whole 4GB memory
    {
        uint32_t current_page_memory_location= (uint32_t)(page_table)+i*0x1000;
        uint32_t current_physical_memory_location= (uint32_t)(0x400000*i);
        create_page_table((page_entry*)current_page_memory_location,current_physical_memory_location);
    }

    log_debug("creating the dir table");
    create_directory_table(directory_table,page_table); // create the mighty directory table

    log_debug("loading page directory");
    // loads the page directory address onto the CR3 register, where the MMU will find it
    __asm__ volatile("mov cr3, %0\n\t"
    : 
    : "r" (directory_table));

    log_debug("enabling paging");
    // enabling paging (changing 32th bit of cr0)
    __asm__ volatile("mov eax, cr0");
    __asm__ volatile("or eax, 0x80000000");
    __asm__ volatile("mov cr0, eax");
    __asm__ volatile("sti");
}

uint32_t get_physical_location(uint32_t paged_memory)
{
    uint32_t directory_offset, page_offset, offset_in_page, directory_original_content, table_original_content, directory_to_real_page_memory,table_to_real_memory,directory_to_real_page_table_memory;
    directory_offset = (paged_memory & PAGE_DIRECTORY_MASK) >> 22; // get offset of directory ( which pagetable to pick )
    page_offset = (paged_memory & PAGE_TABLE_MASK) >> 12; // get offset of page table ( which page entry to pick )
    offset_in_page = (paged_memory & PAGE_OFFSET_MASK); // the offset insdie the page itself.

    uint32_t directory_original_content_place = (uint32_t)(directory_table)+directory_offset*4; // get the directory entry in the offset location.
    directory_original_content = *((uint32_t*)(directory_original_content_place)); // get its content.
    directory_to_real_page_table_memory = directory_original_content&0xfffff000; // get the memory location of the apropriate page table


    directory_to_real_page_memory=directory_to_real_page_table_memory + 4*page_offset; // get the memory location of the appropriate page table entry location
    table_original_content = *((uint32_t*)(directory_to_real_page_memory)); // get its content
    table_to_real_memory = ((table_original_content>>12) * 0x1000); // get the appropriate 4kb allignem memory the page entry is pointing to

    return (uint32_t)(table_to_real_memory + offset_in_page); // add the offset and return the sum.
}
