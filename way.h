#ifndef NGRAPHICS_WAY_H
#define NGRAPHICS_WAY_H

#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

/*
wayland-scanner private-code  < /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml > xdg-shell.c
wayland-scanner client-header < /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml > xdg-shell.h
*/

typedef struct Way {
    int w, h;
    unsigned char *bitmap;
    int fd, size;
    int linesize;
    struct wl_shm_pool *wl_pool;
    struct wl_buffer *wl_buffer;
    struct wl_display *wl_display;
    struct wl_registry *wl_registry;
    struct wl_output *wl_output;
    struct wl_shm *wl_shm;
    struct wl_surface *wl_surface;
    struct wl_compositor *wl_compositor;
    struct wl_seat *wl_seat;
    struct xdg_wm_base *xdg_wm_base;
    struct xdg_surface *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;
    struct xkb_state *xkb_state;
    struct xkb_context *xkb_context;
    struct xkb_keymap *xkb_keymap;
    void (*handle_key)(int key);
} Way;

Way *way_init(void(*fn)(int));
void way_init_window(Way *, int w, int h);
void way_flush(Way *);
#define way_damage(way, x0, x1, y0, y1) wl_surface_damage_buffer(way->wl_surface, x0, x1, y0, y1)
#define way_set_title(way, title) xdg_toplevel_set_title(way->xdg_toplevel, title)
void way_loop(Way *);
void way_free(Way *);

#if defined(WAY_IMP) || defined(ALL_IMP)
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <util/estd.h>
//#include "xdg-shell.h"

static void wl_output_real(void *a, struct wl_output *b, int x, int y, int w, int h, int sp, const char *m, const char *mo, int t) {}
static void wl_output_geo(void *data, struct wl_output *wl_output, uint32_t flags, int w, int h, int rate) {
    Way *way = data;
    way->w = w;
    way->h = h;
}
static void wl_output_info(void *data, struct wl_output *wl_output) {}
static void wl_output_scale(void *data, struct wl_output *wl_output, int factor) {}
static void wl_output_name(void *data, struct wl_output *wl_output, const char *name) {}
static void wl_output_desc(void *data, struct wl_output *wl_output, const char *desc) {}

static const struct wl_output_listener wl_output_listener = {
    wl_output_real,
    wl_output_geo,
    wl_output_info,
    wl_output_scale,
    wl_output_name,
    wl_output_desc,
};

static void
xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, uint32_t serial) {
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static void
xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial) {
    Way *way = data;
    xdg_surface_ack_configure(xdg_surface, serial);
    wl_surface_commit(way->wl_surface);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure,
};

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping,
};

static void keyboard_keymap(void *data, struct wl_keyboard *wl_keyboard, uint32_t format, int fd, uint32_t size) {
    Way *way = data;
    char *str;

    str = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    xkb_keymap_unref(way->xkb_keymap);
    way->xkb_keymap = xkb_keymap_new_from_string(way->xkb_context, str, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
    munmap(str, size);
    close(fd);
    xkb_state_unref(way->xkb_state);
	way->xkb_state = xkb_state_new(way->xkb_keymap);
}
static void keyboard_enter(void *data, struct wl_keyboard *k, uint32_t serial, struct wl_surface *surface, struct wl_array *keys) {}
static void keyboard_leave(void *data, struct wl_keyboard *k, uint32_t serial, struct wl_surface *surface) {}
static void keyboard_mod(void *data, struct wl_keyboard *k, uint32_t serial, uint32_t depress, uint32_t latch, uint32_t lock, uint32_t group) {
    Way *way = data;
    xkb_state_update_mask(way->xkb_state, depress, latch, lock, 0, 0, group);
}
static void keyboard_key(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state) {
    Way *way;
	if(state != WL_KEYBOARD_KEY_STATE_PRESSED)
        return;
    way = data;
    way->handle_key(xkb_keysym_to_utf32(xkb_state_key_get_one_sym(way->xkb_state, key+8)));
}
static void keyboard_repeat(void *data, struct wl_keyboard *k, int rate, int delay) {}
static struct wl_keyboard_listener wl_keyboard_listener = {
    keyboard_keymap,
    keyboard_enter,
    keyboard_leave,
    keyboard_key,
    keyboard_mod,
    keyboard_repeat,
};

static void wl_seat_cap(void *data, struct wl_seat *wl_seat, uint32_t cap) {
    if(cap & WL_SEAT_CAPABILITY_KEYBOARD) {
		struct wl_keyboard *keyboard = wl_seat_get_keyboard(wl_seat);
		wl_keyboard_add_listener(keyboard, &wl_keyboard_listener, data);
    }
}
static void wl_seat_name(void *data, struct wl_seat *wl_seat, const char *name) {}
static const struct wl_seat_listener wl_seat_listener = {
    wl_seat_cap,
    wl_seat_name,
};

static void
registry_global(void *data, struct wl_registry *wl_registry, uint32_t name, const char *interface, uint32_t version) {
    Way *way = data;
    if (strcmp(interface, wl_shm_interface.name) == 0) {
        way->wl_shm = wl_registry_bind(wl_registry, name, &wl_shm_interface, 1);
    } else if (strcmp(interface, wl_compositor_interface.name) == 0) {
        way->wl_compositor = wl_registry_bind(wl_registry, name, &wl_compositor_interface, 4);
    } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        way->xdg_wm_base = wl_registry_bind(wl_registry, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(way->xdg_wm_base, &xdg_wm_base_listener, way);
    } else if(strcmp(interface, wl_output_interface.name) == 0) {
        way->wl_output = wl_registry_bind(wl_registry, name, &wl_output_interface, version);
        wl_output_add_listener(way->wl_output, &wl_output_listener, way);
    } else if(strcmp(interface, wl_seat_interface.name) == 0) {
        way->wl_seat = wl_registry_bind(wl_registry, name, &wl_seat_interface, 1);
        wl_seat_add_listener(way->wl_seat, &wl_seat_listener, way);
    }
}
static void registry_global_remove(void *data, struct wl_registry *wl_registry, uint32_t name) {}
static const struct wl_registry_listener wl_registry_listener = {
    .global = registry_global,
    .global_remove = registry_global_remove,
};

Way *
way_init(void(*fn)(int)) {
    Way *way;
    char name[] = "/wl_shm-ofjdas";

    way = emalloc(sizeof(Way));
    way->xkb_keymap = NULL;
    way->xkb_state = NULL;
    way->xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    way->handle_key = fn;

    way->wl_display = wl_display_connect(NULL);
    way->wl_registry = wl_display_get_registry(way->wl_display);
    wl_registry_add_listener(way->wl_registry, &wl_registry_listener, way);
    wl_display_roundtrip(way->wl_display);

    way->wl_surface = wl_compositor_create_surface(way->wl_compositor);

    way->xdg_surface = xdg_wm_base_get_xdg_surface(way->xdg_wm_base, way->wl_surface);
    xdg_surface_add_listener(way->xdg_surface, &xdg_surface_listener, way);
    way->xdg_toplevel = xdg_surface_get_toplevel(way->xdg_surface);
    xdg_toplevel_set_title(way->xdg_toplevel, "program");
    wl_surface_commit(way->wl_surface);
    wl_display_roundtrip(way->wl_display);

    way->fd = shm_open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if(way->fd == -1) {
        free(way);
        return NULL;
    }
    return way;
}

void
way_init_window(Way *way, int w, int h) {
    way->w = w;
    way->h = h;
    way->linesize = way->w * 4;
    way->size = way->linesize * way->h;

    ftruncate(way->fd, way->size);
    way->bitmap = emmap(NULL, way->size, PROT_READ | PROT_WRITE, MAP_SHARED, way->fd, 0);
    way->wl_pool = wl_shm_create_pool(way->wl_shm, way->fd, way->size);
    way->wl_buffer = wl_shm_pool_create_buffer(way->wl_pool, 0, way->w, way->h, way->linesize, WL_SHM_FORMAT_XRGB8888);
    memset(way->bitmap, 0, way->size);
    wl_surface_attach(way->wl_surface, way->wl_buffer, 0, 0);
}

void
way_flush(Way *way) {
    wl_surface_attach(way->wl_surface, way->wl_buffer, 0, 0);
    wl_surface_commit(way->wl_surface);
    wl_display_dispatch(way->wl_display);
}

void
way_loop(Way *way) {
    for(;;) wl_display_dispatch(way->wl_display);
}

void
way_free(Way *way) {
    wl_shm_pool_destroy(way->wl_pool);
    close(way->fd);
    munmap(way->bitmap, way->size);
    free(way);
}

#endif
#endif
