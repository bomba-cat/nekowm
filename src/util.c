#include "headers/neko.h"

xcb_key_symbols_t *keysyms = NULL;
sig_atomic_t running = 1;

void neko_die(const char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

void neko_spawn(const char *cmd)
{
	if(fork() == 0)
	{
		if (fork() > 0)
		{
			exit(1);
		}

		setsid();
		execlp(cmd, cmd, NULL);
		perror("execlp");
		exit(1);
	}
	wait(NULL);
}

void neko_add_client(xcb_window_t window)
{
	nekos = realloc(nekos, sizeof(neko_client) * (neko_client_count + 1));
	nekos[neko_client_count].window = window;
	nekos[neko_client_count].split = !nekos[(neko_client_count > 1) ? neko_client_count-1 : neko_client_count].split;
	neko_client_count++;
	neko_arrange();
}

void neko_remove_client(xcb_window_t window)
{
	int j = 0;
	for (int i = 0; i < neko_client_count; i++)
	{
		if(nekos[i].window != window)
		{
			nekos[j++] = nekos[i];
		}
	}
	neko_client_count = j;
	nekos = realloc(nekos, sizeof(neko_client) * neko_client_count);
	neko_arrange();
}

void neko_setup()
{
	uint32_t values[] =
	{
    XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
    XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |
    XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_PROPERTY_CHANGE
  };
  xcb_change_window_attributes(connection, screen->root, XCB_CW_EVENT_MASK, values);
  xcb_flush(connection);

	keysyms = xcb_key_symbols_alloc(connection);

	xcb_keycode_t *keycodes = xcb_key_symbols_get_keycode(keysyms, TERM_KEY);
	if (keycodes)
	{
		for (xcb_keycode_t *code = keycodes; *code != 0; ++code)
		{
			xcb_grab_key(connection, 1, screen->root,	MOD, *code,	XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
		}
		free(keycodes);
	}

	keycodes = xcb_key_symbols_get_keycode(keysyms, LAUNCHER_KEY);
	if (keycodes)
	{
		for (xcb_keycode_t *code = keycodes; *code != 0; ++code)
		{
			xcb_grab_key(connection, 1, screen->root,	MOD, *code,	XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
		}
		free(keycodes);
	}
	xcb_flush(connection);
}

void neko_run()
{
  xcb_generic_event_t *ev;
  while (running && (ev = xcb_wait_for_event(connection)))
	{
    neko_handle_events(ev);
  }
}

void neko_cleanup(int)
{
	running = 0;
}
