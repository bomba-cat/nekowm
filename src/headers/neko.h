#ifndef NEKO_H
#define NEKO_H

#include <xcb/xcb.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "config.h"

typedef struct
{
	xcb_window_t window;
	int x, y, width, height;
} neko_client;

extern xcb_connection_t *connection;
extern xcb_screen_t *screen;
extern neko_client *nekos;
extern int neko_client_count;
extern sig_atomic_t running;

void neko_die(const char *msg);
void neko_spawn(const char *cmd);
void neko_arrange();
void neko_add_client(xcb_window_t window);
void neko_remove_client(xcb_window_t window);
void neko_setup();
void neko_run();
void neko_cleanup(int);

#endif //!NEKO_H
