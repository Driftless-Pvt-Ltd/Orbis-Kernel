#include "ramfs.h"
#include "stdlib.h"

static ramfs_node_t *ramfs_root = 0;

// helper to zero new memory since we got no calloc
static void *zalloc(uint32_t sz) {
    void *p = malloc(sz);
    if (p) memset(p, 0, sz);
    return p;
}

// dumb realloc substitute (malloc + copy)
static void *ralloc(void *old, uint32_t old_sz, uint32_t new_sz) {
    void *p = malloc(new_sz);
    if (!p) return 0;
    if (old) {
        memcpy(p, old, old_sz);
        // no free() yet? if you got it, free(old);
    }
    return p;
}

void ramfs_init() {
    ramfs_root = zalloc(sizeof(ramfs_node_t));
    strcpy(ramfs_root->name, "/");
    ramfs_root->is_dir = 1;
    print_system_call("ramfs: initialized\n", 20);
}

ramfs_node_t *ramfs_create(ramfs_node_t *parent, const char *name, int is_dir) {
    if (!parent || !parent->is_dir) return 0;

    ramfs_node_t *node = zalloc(sizeof(ramfs_node_t));
    strcpy(node->name, name);
    node->is_dir = is_dir;
    node->parent = parent;

    node->next = parent->children;
    parent->children = node;

    return node;
}

ramfs_node_t *ramfs_find(ramfs_node_t *parent, const char *name) {
    for (ramfs_node_t *n = parent->children; n; n = n->next)
        if (strcmp(n->name, name) == 0)
            return n;
    return 0;
}

int ramfs_write(ramfs_node_t *file, const void *buf, uint32_t len, uint32_t offset) {
    if (!file || file->is_dir) return -1;

    if (offset + len > file->size) {
        file->data = ralloc(file->data, file->size, offset + len);
        file->size = offset + len;
    }

    memcpy(file->data + offset, buf, len);
    return len;
}

int ramfs_read(ramfs_node_t *file, void *buf, uint32_t len, uint32_t offset) {
    if (!file || file->is_dir) return -1;
    if (offset >= file->size) return 0;
    if (offset + len > file->size) len = file->size - offset;
    memcpy(buf, file->data + offset, len);
    return len;
}

void ramfs_demo() {
    ramfs_node_t *etc = ramfs_create(ramfs_root, "etc", 1);
    ramfs_node_t *file = ramfs_create(etc, "config.txt", 0);

    const char *msg = "hello from ramfs!\n";
    ramfs_write(file, msg, strlen(msg), 0);

    char buf[64];
    int n = ramfs_read(file, buf, sizeof(buf) - 1, 0);
    buf[n] = 0;

    print_system_call(buf, n);
}