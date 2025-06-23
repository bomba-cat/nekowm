#include "headers/nekowm.h"

const char *term[] = { NEKOWM_TERM, NULL };
volatile sig_atomic_t running = 1;

int nekowm_arguments(int argc, char** argv)
{
	if(argc == 2 && !strcmp("-v", argv[1]))
	{
		printf("NekoWM version 0.1, Copyright © 2025 bombacat, MIT License\nNote: return code 254 is not an error\n");
		return 254;
	}
	if(argc != 1)
	{
		printf("NekoWM usage: nekowm [-v]\n");
		return 255;
	}
	return 0;
}

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

void nekowm_run()
{
  xcb_generic_event_t *event;

  while (running && (event = xcb_wait_for_event(connection)))
  {
    switch (event->response_type & ~0x80)
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
		if(nekowm_window_count == 0)
			nekowm_summon(term);

    free(event);
  }
}
