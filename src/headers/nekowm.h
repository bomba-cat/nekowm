#ifndef NEKOWM_H
#define NEKOWM_H

#include <xcb/xcb.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

typedef struct
{
	int index;
	int width, height;
	int x, y;
	xcb_window_t window;
} nekowm_window_t;

extern uint8_t nekowm_window_count;
extern nekowm_window_t *nekowm_windows;

extern xcb_connection_t *connection;
extern xcb_screen_t *screen;
extern volatile sig_atomic_t running;

void nekowm_sigint_handler(int sig);

/* NekoWM Window */
void nekowm_map_window(xcb_window_t window);
void nekowm_destroy_window(xcb_window_t window);
void nekowm_show_window(nekowm_window_t window);
void nekowm_show_windows();

/* NekoWM Util */
int nekowm_arguments(int argc, char** argv);
void nekowm_setup();
void nekowm_run();
void nekowm_summon(const char *cmd[]);

#include "config.h"

#endif //!NEKOWM_H
