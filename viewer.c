// viewer_realtime.c
#include <SDL2/SDL.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define FB_WIDTH 800
#define FB_HEIGHT 600
#define GUEST_RAM_BASE 0x40000000ULL

int main() {
    unsigned long long fb_phys = 0x00000000403AD000ULL;
    if (fb_phys < GUEST_RAM_BASE) {
        fprintf(stderr, "fb_phys (0x%llx) < guest RAM base (0x%llx)\n",
                fb_phys, (unsigned long long)GUEST_RAM_BASE);
        return 1;
    }

    off_t fb_offset = fb_phys - GUEST_RAM_BASE;
    size_t fb_bytes = FB_WIDTH * FB_HEIGHT * 2;

    long page = sysconf(_SC_PAGE_SIZE);
    off_t map_off = (fb_offset / page) * page;
    off_t inpage_off = fb_offset - map_off;
    size_t map_len = inpage_off + fb_bytes;

    int fd = open("/dev/shm/qemu_fb", O_RDONLY);
    if (fd < 0) { perror("open"); return 1; }

    void *map = mmap(NULL, map_len, PROT_READ, MAP_SHARED, fd, map_off);
    if (map == MAP_FAILED) { perror("mmap"); close(fd); return 1; }

    uint16_t *fb = (uint16_t *)((uint8_t*)map + inpage_off);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        munmap(map, map_len); close(fd);
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("QEMU FB",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        FB_WIDTH, FB_HEIGHT, 0);

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *tex = SDL_CreateTexture(ren,
        SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING,
        FB_WIDTH, FB_HEIGHT);

    SDL_Event ev;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = 0;
        }

        // Update texture from framebuffer
        SDL_UpdateTexture(tex, NULL, fb, FB_WIDTH * 2);

        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);

        // Optional: yield CPU briefly
        SDL_Delay(1);
    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    munmap(map, map_len);
    close(fd);

    return 0;
}
