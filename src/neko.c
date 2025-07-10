#include "headers/neko.h"

xcb_connection_t *connection = NULL;
xcb_screen_t *screen = NULL;
int screen_count;

int main(int argc, char** argv)
{
  if(argc > 1 && (!strcmp("-v", argv[1]) || !strcmp("--version", argv[1])))
	{
		printf("NekoWM version 0.1, Copyright © 2025 bombacat, MIT License\n");
		return EXIT_SUCCESS;
	}

  if(argc == 2 && (atoi(argv[1])>0))
  {
    argc--;
    neko_setup_stacks(atoi(argv[1]));
  } else if(argc > 1)
  {
    neko_send_message(argc, argv);
  } else
  {
    neko_setup_stacks(10);
  }

  if (argc == 2 || (argc > 1 && (!strcmp("-h", argv[1]) || !strcmp("--help", argv[1]))))
  {
    printf("Usage: nekowm [-v | --version] [STACK_COUNT]\n\n"
           "Options:\n"
           "  -h, --help         Show this dialogue\n"
           "  -v, --version      Show version and exit\n"
           "  STACK_COUNT        Number of stacks to create [default: 10]\n");
    return EXIT_FAILURE;
  }

	connection = xcb_connect(NULL, &screen_count);
	if (xcb_connection_has_error(connection))
	{
		neko_die("Neko failed to connect to the X server");
	}

	const xcb_setup_t *setup = xcb_get_setup(connection);
	xcb_screen_iterator_t iterator = xcb_setup_roots_iterator(setup);
	for (int i = 0; i < screen_count; ++i)
	{
		xcb_screen_next(&iterator);
	}
	screen = iterator.data;

	signal(SIGINT, neko_cleanup);
	signal(SIGTERM, neko_cleanup);

	neko_setup();
	neko_run();

	xcb_disconnect(connection);

	free(connection);
	free(screen);
  for(int i = 0; i < 5; i++)
  {
	  free(stacks[i].clients);
  }
  free(stacks);
	free(keysyms);
	return EXIT_SUCCESS;
}
