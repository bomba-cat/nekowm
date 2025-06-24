#include "headers/nekowm.h"

uint8_t nekowm_window_count = 0;
nekowm_window_t *nekowm_windows;

xcb_connection_t *connection;
xcb_screen_t *screen;

void nekowm_sigint_handler(int)
{
	running = 0;
}

int main(int argc, char** argv)
{
	int argr = nekowm_arguments(argc, argv); 
	if(argr > 0)
	{
		return argr;
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
