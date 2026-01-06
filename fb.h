#ifndef M_GRAPHICS_H
#define M_GRAPHICS_H

typedef struct FB {
    unsigned char *bitmap;
    int fd;
    int w, h;
    int linesize;
} FB;

struct FB *fb_init(void);
void fb_free(struct FB *);
void fb_flush(struct FB *);

#if defined(FB_IMP) || defined(ALL_IMP)

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <util/estd.h>

static void
terminal_instant(int n) {
    static struct termios oldterm;
    if(n) {
        struct termios term;
        tcgetattr(0, &term);
        oldterm = term;
        term.c_lflag &= !(ICANON | ECHO);
        tcsetattr(0, TCSAFLUSH, &term);
        printf("\033[2J" "\033[?25l");
        fflush(stdout);
    } else {
        tcsetattr(0, 0, &oldterm);
        printf("\033[?25h" "\033[H");
        fflush(stdout);
    }
}

struct FB *
fb_init(void) {
    int ret;
    struct FB *fb;
    struct fb_var_screeninfo fbinfo;

    fb = emalloc(sizeof(struct FB));
    fb->fd = eopen("/dev/fb0", O_RDWR);
    ret = ioctl(fb->fd, FBIOGET_VSCREENINFO, &fbinfo);
    if(ret == -1)
        die("fb_init", "Error initilizing framebuffer");
    fb->w = fbinfo.xres;
    fb->h = fbinfo.yres;
    fb->linesize = fb->w * 4;
    fb->bitmap = emmap(NULL, fb->linesize * fb->h, PROT_READ|PROT_WRITE, MAP_SHARED, fb->fd, 0);
    terminal_instant(1);
    return fb;
}

void
fb_free(struct FB *fb) {
    munmap(fb->bitmap, fb->linesize * fb->h);
    close(fb->fd);
    terminal_instant(0);
    free(fb);
}

void
fb_flush(struct FB *fb) {
    // hack
    write(fb->fd, "\0", 1);
}

#endif
#endif
