#ifndef NKM_GRAPHICS_X11_H
#define NKM_GRAPHICS_X11_H

#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct x11 {
    int w, h;
    int fd, screen;
    Display *display;
    Window window, root;
    XImage *image;
    unsigned char *bitmap;
    XFontStruct *font;
    GC gc;
    int linesize;
} x11;

x11 *x11_init();
void x11_init_window(x11 *, int w, int h);
void x11_flush(x11 *);
void x11_free(x11 *);

#if defined(X11_IMP) || defined(ALL_IMP)

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <util/estd.h>
#include <X11/keysym.h>

x11 *
x11_init(void) {
    x11 *d;
    Window root;
    int _, __, ___, ____;

    d = emalloc(sizeof(x11));
    d->display = XOpenDisplay(0);
    if(!d->display)
        die("nkm_x11", "XOpenDisplay");
    d->fd = ConnectionNumber(d->display);
    d->root = DefaultRootWindow(d->display);
    d->screen = DefaultScreen(d->display);
    XGetGeometry(d->display, d->root, &d->root, &_, &__, &d->w, &d->h, &___, &____);
    return d;
}

void
x11_init_window(x11 *d, int w, int h) {
    d->w = w;
    d->h = h;
    d->linesize = w * 4;
    d->window = XCreateSimpleWindow(d->display, d->root, 0, 0, d->w, d->h, 0, 0, BlackPixel(d->display, d->screen));
    XStoreName(d->display, d->window, "view");
    XSelectInput(d->display, d->window, StructureNotifyMask | KeyPressMask | ExposureMask);
    XMapWindow(d->display, d->window);
    d->gc = XCreateGC(d->display, d->window, 0, 0);
    XSetForeground(d->display, d->gc, 0xffffffff);
    XSetBackground(d->display, d->gc, 0x00000000);
    d->bitmap = emalloc(d->w * d->h * 4);
    d->image = XCreateImage(d->display, DefaultVisual(d->display, d->screen), 24, ZPixmap, 0, (char*)d->bitmap, d->w, d->h, 32, 0);
}

void
x11_flush(x11 *d) {
    XPutImage(d->display, d->window, d->gc, d->image, 0, 0, 0, 0, d->w, d->h);
}

void
x11_free(x11 *d) {
    XDestroyImage(d->image);
    XFreeGC(d->display, d->gc);
    XDestroyWindow(d->display, d->window);
    XCloseDisplay(d->display);
}

/*
int
main(void) {
    XEvent e;
    KeySym key;
    x11 *d;

    d = x11_init();
    x11_init_window(d, 300, 300);
    for(;;) {
        XNextEvent(d->display, &e);
        switch(e.type) {
        case Expose:
            x11_flush(d);
            break;
        case KeyPress:
            key = XkbKeycodeToKeysym(d->display, e.xkey.keycode, 0, 0);
            switch(key) {
            case XK_q:
                x11_free(d);
                exit(0);
            default:
                break;
            }
            break;
        }
    }

}
*/

#endif
#endif
