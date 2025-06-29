#include "headers/neko.h"

sig_atomic_t running = 1;

void neko_die(const char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

neko_command neko_get_arguments(const char *cmd)
{
	char *copy = strdup(cmd);
	char *token = strtok(copy, " ");
	char **array = malloc(50 * sizeof(char*));

	int i = 0;
	while(token != NULL && i < 50)
	{
		array[i++] = token;

		token = strtok(NULL, " ");
	}

	array[i] = NULL;

	neko_command command = { array, copy };
	return command;
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
		neko_command command = neko_get_arguments(cmd);
		execvp(command.args[0], command.args);

		free(command.args);
		free(command.buffer);

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

	neko_grab_keybinds();
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
