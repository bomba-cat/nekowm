#ifndef NEKO_H
#define NEKO_H

#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "config.h"

/* found this in xwm */
#define UNUSED(x) (void)(x)

typedef enum
{
	NEKO_VERTICAL,
	NEKO_HORIZONTAL,
} neko_split;

typedef struct
{
	xcb_window_t window;
	int x, y, width, height;
	neko_split split;
} neko_client;

extern xcb_connection_t *connection;
extern xcb_screen_t *screen;
extern neko_client *nekos;
extern int neko_client_count;
extern xcb_key_symbols_t *keysyms;
extern sig_atomic_t running;

/* keyboard */


/* event */
void neko_handle_events(xcb_generic_event_t *event);
void neko_handle_focus_in(xcb_generic_event_t *event);
void neko_handle_focus_out(xcb_generic_event_t *event);
void neko_handle_enter_notify(xcb_generic_event_t *event);
void neko_handle_destroy(xcb_generic_event_t *event);
void neko_handle_map(xcb_generic_event_t *event);
void neko_handle_key_press(xcb_generic_event_t *event);

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
