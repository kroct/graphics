# graphics

These libraries provide bitmap access to directly to the framebuffer, and to x11/wayland windows.

## Examples

for the framebuffer
```
#define FB_IMP
#include "graphics/fb.h"

int
main(void) {
    int c;
    unsigned char *p;
    FB *fb;

    fb = fb_init();
    while((c = getchar()) != EOF) {
        switch(c) {
        case 'r':
            p = fb->bitmap;
            for(int i = 0; i < fb->h * fb->w; i++)
                *p++ = 0, *p++ = 0, *p++ = 0xff, *p++ = 0;
            break;
        case 'g':
            p = fb->bitmap;
            for(int i = 0; i < fb->h * fb->w; i++)
                *p++ = 0, *p++ = 0xff, *p++ = 0, *p++ = 0;
            break;
        case 'g':
            p = fb->bitmap;
            for(int i = 0; i < fb->h * fb->w; i++)
                *p++ = 0xff, *p++ = 0, *p++ = 0, *p++ = 0;
            break;
        case 'q':
            fb_free(fb);
            return 0;
            break;
        }
    }
    fb_free(fb);
    return 0;
}
```

for x11 (link with -lX11)
```
#define X11_IMP
#include "graphics/x11.h"
#include <stdlib.h>

int
main(void) {
    XEvent e;
    KeySym key;
    x11 *xorg = x11_init();
    x11_init_window(xorg, 100, 100);

    for(;;) {
        XNextEvent(xorg->display, &e);
        switch(e.type) {
        case Expose:
            x11_flush(xorg);
            break;
        case KeyPress:
            key = XkbKeycodeToKeysym(xorg->display, e.xkey.keycode, 0, 0);
            switch(key) {
            case XK_r:
                p = xorg->bitmap;
                for(int i = 0; i < xorg->h * xorg->w; i++)
                    *p++ = 0, *p++ = 0, *p++ = 0xff, *p++ = 0;
                break;
            case XK_g:
                p = xorg->bitmap;
                for(int i = 0; i < xorg->h * xorg->w; i++)
                    *p++ = 0, *p++ = 0xff, *p++ = 0, *p++ = 0;
                break;
            case XK_b:
                p = xorg->bitmap;
                for(int i = 0; i < xorg->h * xorg->w; i++)
                    *p++ = 0xff, *p++ = 0, *p++ = 0, *p++ = 0;
                break;
            case XK_q:
                x11_free(xorg);
                exit(0);
            default:
                break;
            }
            break;
        }
    }

}
```

for wayland
(NOTE:
you must generate extra files first with
```
wayland-scanner private-code  < /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml > xdg-shell.c
wayland-scanner client-header < /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml > xdg-shell.h
```
and link with xdg-shell.c and -lwayland-client)
```
#define WAY_IMP
#include "graphics/way.h"
#include <stdlib.h>

Way *way;

static int
input(int key) {
    switch(key) {
    case XK_r:
        p = way->bitmap;
        for(int i = 0; i < way->h * way->w; i++)
            *p++ = 0, *p++ = 0, *p++ = 0xff, *p++ = 0;
        break;
    case XK_g:
        p = way->bitmap;
        for(int i = 0; i < way->h * way->w; i++)
            *p++ = 0, *p++ = 0xff, *p++ = 0, *p++ = 0;
        break;
    case XK_b:
        p = way->bitmap;
        for(int i = 0; i < way->h * way->w; i++)
            *p++ = 0xff, *p++ = 0, *p++ = 0, *p++ = 0;
        break;
    case XK_q:
        way_free(way);
        exit(0);
    default:
        break;
    }
}

int
main(void) {
    way = way_init(input);
    way_init_window(way, 300, 300);
    way_loop();
}
```
