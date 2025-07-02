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

/* found this in xwm */
#define UNUSED(x) (void)(x)

typedef enum
{
	NEKO_VERTICAL,
	NEKO_HORIZONTAL,
} neko_split;

typedef struct
{
	char **args;
	char *buffer;
} neko_command;

typedef struct
{
	uint16_t mod;
	xcb_keysym_t key;
	char *command;
} neko_keybind;

typedef struct
{
	int index;
	xcb_window_t window;
	int x, y, width, height;
	neko_split split;
} neko_client;

typedef struct
{
	int index;
	int focused_client;
  int client_count;
	neko_client *clients;
} neko_stack;

#include "config.h"

extern xcb_connection_t *connection;
extern xcb_screen_t *screen;
extern int screen_count;
extern neko_stack *stacks;
extern int selected_stack;
extern xcb_key_symbols_t *keysyms;
extern sig_atomic_t running;

/* keyboard */
void neko_grab_keybinds();
void neko_read_keybinds(xcb_key_press_event_t *e);

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
neko_command neko_get_arguments(const char *cmd);
void neko_spawn(const char *cmd);
void neko_setup_stacks(int stack_count);
void neko_add_client(xcb_window_t window);
void neko_remove_client(xcb_window_t window);
void neko_setup();
void neko_run();

/* main */
void neko_cleanup(int);

#endif //!NEKO_H
