#ifndef NEKO_H
#define NEKO_H

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>
#include <xcb/randr.h>
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xproto.h>

/* found this in xwm */
#define UNUSED(x) (void)(x)

typedef enum
{
  NEKO_VERTICAL,
  NEKO_HORIZONTAL,
} neko_split;

/* Log Flag */
typedef enum
{
  INFO = 0,
  WARNING = 1,
  ERROR = 2,
  SEVERE = 3,
} neko_log_flag;

typedef struct
{
  int x, y, width, height;
} neko_bar_args;

typedef struct
{
  int x, y, width, height;
} neko_monitor;

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
extern neko_monitor *monitors;
extern int monitor_count;
extern neko_stack *stacks;
extern int *selected_stacks;
extern xcb_key_symbols_t *keysyms;
extern sig_atomic_t running;
extern int neko_sock;
extern FILE *log_file;

/* log */
void neko_log_init();
void neko_log(char *message, neko_log_flag flag);

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
void neko_split_toggle();
void neko_close_window();
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
int neko_find_monitor_for_window(int wx, int wy);
int neko_get_monitor_under_cursor();
void neko_update_monitors();
void neko_setup();
void neko_run();

/* message */
void neko_init_socket();
void neko_scan_message();
int neko_send_message(int argc, char **argv);

/* bar */
void *neko_create_bar();
void neko_execute_bar();

/* main */
void neko_exit();
void neko_cleanup(int sig);

#endif //! NEKO_H
