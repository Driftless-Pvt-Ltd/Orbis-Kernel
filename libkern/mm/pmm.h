/* ===========================================================================
 *  Orbis Kernel – Physical Memory Manager (pmm.h)
 *  Author: Kap Petrov (@VigfallStudios)
 *  Description: Manages physical memory pages. Provides alloc/free
 *               interfaces for page-level memory management.
 *  License: None
 * ===========================================================================
 */

#ifndef PMM_H
#define PMM_H

#include <stdint.h>

#define PAGE_SIZE       4096ULL        // 4 KB pages
#define MAX_PAGES       1048576ULL     // example: 4 GB RAM / 4 KB pages
#define BITMAP_SIZE     (MAX_PAGES / 8)

// --------------------
// Physical Memory Bitmap
// --------------------
static uint8_t phys_bitmap[BITMAP_SIZE];

// --------------------
// Bitmap Helpers
// --------------------
static inline void bitmap_set(uint64_t page, int val) {
    if (val)
        phys_bitmap[page / 8] |=  (1 << (page % 8));
    else
        phys_bitmap[page / 8] &= ~(1 << (page % 8));
}

static inline int bitmap_get(uint64_t page) {
    return (phys_bitmap[page / 8] >> (page % 8)) & 1;
}

// --------------------
// PMM Initialization
// --------------------
// mem_start and mem_end are physical addresses of usable memory
static inline void pmm_init(uintptr_t mem_start, uintptr_t mem_end) {
    // Mark all pages as used initially
    for (uint64_t i = 0; i < MAX_PAGES; i++)
        bitmap_set(i, 1);

    // Mark usable pages as free
    uint64_t start_page = mem_start / PAGE_SIZE;
    uint64_t end_page   = mem_end   / PAGE_SIZE;

    for (uint64_t i = start_page; i < end_page && i < MAX_PAGES; i++)
        bitmap_set(i, 0);
}

// --------------------
// Allocate a single physical page
// Returns physical address, 0 if out of memory
// --------------------
static inline uintptr_t pmm_alloc_page(void) {
    for (uint64_t i = 0; i < MAX_PAGES; i++) {
        if (!bitmap_get(i)) {
            bitmap_set(i, 1);
            return i * PAGE_SIZE;
        }
    }
    return 0;
}

// --------------------
// Free a previously allocated page
// --------------------
static inline void pmm_free_page(uintptr_t addr) {
    uint64_t page = addr / PAGE_SIZE;
    if (page < MAX_PAGES)
        bitmap_set(page, 0);
}

#endif