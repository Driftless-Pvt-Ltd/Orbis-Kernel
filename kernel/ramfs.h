#ifndef RAMFS_H
#define RAMFS_H

#include "memory_manager.h"
#include "system_calls.h"
#include "common_defintions.h"
#include <stdint.h>

typedef struct ramfs_node {
    char name[64];
    uint8_t *data;
    uint32_t size;
    int is_dir;

    struct ramfs_node *parent;
    struct ramfs_node *children;
    struct ramfs_node *next;
} ramfs_node_t;

void ramfs_init();
void ramfs_demo();

#endif