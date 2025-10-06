#ifndef memory_manager_header
#define memory_manager_header

#include <stdint.h> //for type definitions

#include "common_defintions.h"
#include "print.h"

extern uint32_t total_bytes_allocated; // every request gets

void* malloc(uint32_t);
#endif