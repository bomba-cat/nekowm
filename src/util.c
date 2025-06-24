#include "headers/neko.h"

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
	nekos = realloc(nekos, sizeof(neko_client) * (neko_client_count -1));
	nekos[neko_client_count].window = window;
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
}

void neko_run()
{
  xcb_generic_event_t *ev;
  neko_spawn(TERM);
  neko_spawn(TERM);
  while (running && (ev = xcb_wait_for_event(connection)))
	{
    switch (ev->response_type & ~0x80)
		{
      case XCB_MAP_REQUEST:
				{
        xcb_map_request_event_t *e = (xcb_map_request_event_t *)ev;
        xcb_map_window(connection, e->window);
        neko_add_client(e->window);
        break;
      }
      case XCB_DESTROY_NOTIFY:
				{
        xcb_destroy_notify_event_t *e = (xcb_destroy_notify_event_t *)ev;
        neko_remove_client(e->window);
        break;
      }
    }
    free(ev);
  }
}

void neko_cleanup(int)
{
	running = 0;
}
