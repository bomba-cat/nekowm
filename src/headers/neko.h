#ifndef NEKO_H
#define NEKO_H

#include <xcb/xcb.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "config.h"

typedef struct
{
	xcb_window_t window;
	int x, y, width, height;
	bool split; /* true: l/r, false: u/d */
} neko_client;

extern xcb_connection_t *connection;
extern xcb_screen_t *screen;
extern neko_client *nekos;
extern int neko_client_count;
extern sig_atomic_t running;

/* event */
void neko_handle_events(xcb_generic_event_t *event);
void neko_handle_focus_in(xcb_generic_event_t *event);
void neko_handle_focus_out(xcb_generic_event_t *event);
void neko_handle_enter_notify(xcb_generic_event_t *event);
void neko_handle_destroy(xcb_generic_event_t *event);
void neko_handle_map(xcb_generic_event_t *event);

/* window */
void neko_set_focus_color(xcb_window_t window, bool focus);
void neko_set_focus(xcb_drawable_t window);
void neko_arrange();

/* util */
void neko_die(const char *msg);
void neko_spawn(const char *cmd);
void neko_add_client(xcb_window_t window);
void neko_remove_client(xcb_window_t window);
void neko_setup();
void neko_run();

/* main */
void neko_cleanup(int);

#endif //!NEKO_H
