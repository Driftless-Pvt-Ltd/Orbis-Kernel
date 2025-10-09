#include "memory_manager.h"
uint32_t total_bytes_allocated = 0;
typedef struct {
    uint32_t length;
    uint32_t active;
} Block;
void* malloc(uint32_t length)
{
    total_bytes_allocated+=length;
    return (void*)((uint32_t)(MALLOC_MEMORY_LOCATION) + (uint32_t)((total_bytes_allocated-length)));
}
void* free(void* Location)
{
    Block* freemyboy = ((Block*)Location);
    freemyboy->active = 1;
}
