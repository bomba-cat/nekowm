#include "headers/nekowm.h"

uint8_t nekowm_window_count = 0;
nekowm_window_t *nekowm_windows;

const char *term[] = { NEKOWM_TERM, NULL };

xcb_connection_t *connection;
xcb_screen_t *screen;
volatile sig_atomic_t running = 1;

void nekowm_summon(const char *cmd[])
{
	if (fork() == 0)
	{
		if (fork() > 0)
		{
			exit(0);
		}

		setsid();
		execvp(cmd[0], (char *const *)cmd);
		perror("execvp failed");
		exit(1);
	}
}

void nekowm_sigint_handler(int sig)
{
	printf("NekoWM recieved %d", sig);
	running = 0;
}

void nekowm_setup()
{
	nekowm_windows = NULL;

	uint32_t mask = XCB_CW_EVENT_MASK;
	uint32_t values[] =
	{
		XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
		XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
		XCB_EVENT_MASK_ENTER_WINDOW |
		XCB_EVENT_MASK_LEAVE_WINDOW |
		XCB_EVENT_MASK_STRUCTURE_NOTIFY |
		XCB_EVENT_MASK_PROPERTY_CHANGE
	};

	xcb_change_window_attributes(connection, screen->root, mask, values);
	xcb_flush(connection);
}

void nekowm_map_window(xcb_window_t window)
{
	xcb_map_window(connection, window);
	xcb_flush(connection);

	nekowm_window_count++;
	nekowm_windows = realloc(nekowm_windows, sizeof(nekowm_window_t) * nekowm_window_count);
	nekowm_window_t newWindow =
	{
		.index = nekowm_window_count,
		.width = 0,
		.height = screen->height_in_pixels - (NEKOWM_WINDOW_SPACING*2),
		.x = 0,
		.y = NEKOWM_WINDOW_SPACING,
		.window = window,
	};
	nekowm_windows[nekowm_window_count-1] = newWindow;

	for (int i = 0; i < nekowm_window_count; i++)
	{
		nekowm_windows[i].x = (screen->width_in_pixels / nekowm_window_count) * i;
		nekowm_windows[i].width = screen->width_in_pixels / nekowm_window_count - (NEKOWM_WINDOW_SPACING * 2);
		
		if (nekowm_windows[i].x != 0)
		{
			nekowm_windows[i].x += NEKOWM_WINDOW_SPACING/2;
		} else
		{
			nekowm_windows[i].x += NEKOWM_WINDOW_SPACING;
		}
	}
}

void nekowm_destroy_window(xcb_window_t window)
{
	  int new_count = nekowm_window_count - 1;
    nekowm_window_t *new_windows = malloc(sizeof(nekowm_window_t) * new_count);

    int j = 0;
    for (int i = 0; i < nekowm_window_count; i++)
    {
        if (nekowm_windows[i].window == window)
				{
					continue;
				}
        new_windows[j++] = nekowm_windows[i];
    }

    free(nekowm_windows);
		nekowm_windows = new_windows;
    nekowm_window_count = new_count;
		//free(new_windows);
}

void nekowm_show_windows()
{
	for(int i = 0; i < nekowm_window_count; i++)
	{
		nekowm_show_window(nekowm_windows[i]);
	}
}

void nekowm_show_window(nekowm_window_t window)
{
	uint32_t values[4] = { window.x, window.y, window.width, window.height };
	uint16_t mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
									XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
	xcb_configure_window(connection, window.window, mask, values);

	xcb_configure_notify_event_t event =
	{
		.response_type = XCB_CONFIGURE_NOTIFY,
		.event = window.window,
		.window = window.window,
		.above_sibling = XCB_NONE,
		.x = values[0],
		.y = values[1],
		.width = values[2],
		.height = values[3],
		.border_width = 15,
		.override_redirect = 0
	};
	xcb_send_event(connection, 0, window.window, XCB_EVENT_MASK_STRUCTURE_NOTIFY, (char *)&event);

	xcb_flush(connection);
}

void nekowm_run()
{
	xcb_generic_event_t *event;

	while (running && (event = xcb_wait_for_event(connection)))
	{
		switch(event->response_type & ~0x80)
		{
			case XCB_MAP_REQUEST:
				{
					xcb_map_request_event_t *ev = (xcb_map_request_event_t *)event;
					nekowm_map_window(ev->window);
					nekowm_show_windows();
					break;
				}

			case XCB_DESTROY_NOTIFY:
				{
					xcb_destroy_notify_event_t *ev = (xcb_destroy_notify_event_t *)event;
					nekowm_destroy_window(ev->window);
					nekowm_show_windows();
					break;
				}

			default:
				break;
		}

		if (nekowm_window_count == 0)
			nekowm_summon(term);

		free(event);
	}
}

int main(int argc, char** argv)
{
	if(argc == 2 && !strcmp("-v", argv[1]))
	{
		printf("NekoWM version 0.1, Copyright © 2025 bombacat, MIT License\n");
		return 0;
	}
	if(argc != 1)
	{
		printf("NekoWM usage: nekowm [-v]\n");
		return 255;
	}

	int screen_number;
	connection = xcb_connect(NULL, &screen_number);
	if (xcb_connection_has_error(connection))
	{
		fprintf(stderr, "Neko failed to connect to X server\n");
		return 1;
	}

	const xcb_setup_t *setup = xcb_get_setup(connection);
	xcb_screen_iterator_t iterator = xcb_setup_roots_iterator(setup);
	for (int i = 0; i < screen_number; i++)
	{
		xcb_screen_next(&iterator);
	}
	screen = iterator.data;

	signal(SIGINT, nekowm_sigint_handler);

	nekowm_setup();
	nekowm_run();

	xcb_disconnect(connection);
	return 0;
}
