#include "headers/nekowm.h"

xcb_connection_t *connection;
xcb_screen_t *screen;
volatile sig_atomic_t running = 1;

void nekowm_sigint_handler(int sig)
{
	running = 0;
}

void nekowm_setup()
{
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
}

void nekowm_run()
{
	xcb_generic_event_t *event;

	while (running && (event = xcb_wait_for_event(connection)))
	{
		uint8_t type = event->response_type & ~0x80;

		switch(type)
		{
			case XCB_MAP_REQUEST:
				{
					xcb_map_request_event_t *ev = (xcb_map_request_event_t *)event;
					nekowm_map_window(ev->window);
					break;
				}

			case XCB_DESTROY_NOTIFY:
				break;

			default:
				break;
		}

		free(event);
	}
}

int main()
{
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
