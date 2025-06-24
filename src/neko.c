#include "headers/neko.h"

xcb_connection_t *connection = NULL;
xcb_screen_t *screen = NULL;

int main(int argc, char** argv)
{
	if(argc == 2 && !strcmp("-v", argv[1]))
	{
		printf("NekoWM version 0.1 revision 1, Copyright © 2025 bombacat, MIT License\n");
		return EXIT_SUCCESS;
	}

	if(argc == 2)
	{
		printf("Usage: neko [-v]\n");
		return EXIT_FAILURE;
	}

	int scr;
	connection = xcb_connect(NULL, &scr);
	if (xcb_connection_has_error(connection))
	{
		neko_die("Neko failed to connect to the X server");
	}

	const xcb_setup_t *setup = xcb_get_setup(connection);
	xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
	for (int i = 0; i < scr; ++i)
	{
		xcb_screen_next(&it);
	}
	screen = it.data;

	signal(SIGINT, neko_cleanup);
	signal(SIGTERM, neko_cleanup);

	neko_setup();
	neko_run();

	xcb_disconnect(connection);
	return EXIT_SUCCESS;
}
