#include "headers/neko.h"

xcb_connection_t *connection = NULL;
xcb_screen_t *screen = NULL;
int screen_count;
int stack_c = 10;

static void print_help(void)
{
  printf("Usage: nekowm [-v | --version] [STACK_COUNT]\n\n"
         "Options:\n"
         "  -h, --help         Show this dialogue\n"
         "  -v, --version      Show version and exit\n"
         "  --split-toggle     Split the toggle of the current selected window\n"
         "  --exit-neko        Kill the window manager\n"
         "  STACK_COUNT        Number of stacks to create [default: 10]\n");
}

int main(int argc, char **argv)
{
  if (argc > 1)
  {
    if (!strcmp("-v", argv[1]) || !strcmp("--version", argv[1]))
    {
      printf("NekoWM version 0.1, Copyright © 2025 bombacat, MIT License\n");
      return EXIT_SUCCESS;
    }
    else if (!strcmp("-h", argv[1]) || !strcmp("--help", argv[1]))
    {
      print_help();
      return EXIT_SUCCESS;
    }
    else if (atoi(argv[1]) > 0)
    {
      neko_setup_stacks(atoi(argv[1]));
      stack_c = atoi(argv[1]);
    }
    else
    {
      if (neko_send_message(argc, argv) != 0)
      {
        print_help();
        return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
    }
  }
  else
  {
    neko_setup_stacks(10);
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

  for (int i = 0; i < stack_c; i++)
  {
    free(stacks[i].clients);
  }
  free(stacks);
  if (keysyms) xcb_key_symbols_free(keysyms);
  return EXIT_SUCCESS;
}
