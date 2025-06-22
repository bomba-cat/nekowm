#ifndef NEKOWM_H
#define NEKOWM_H

#include <xcb/xcb.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

extern xcb_connection_t *connection;
extern xcb_screen_t *screen;

void nekowm_sigint_handler(int sig);
void nekowm_map_window(xcb_window_t window);
void nekowm_setup();
void nekowm_run();

#endif //!NEKOWM_H
