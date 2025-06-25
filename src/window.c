#include "headers/neko.h"

neko_client *nekos = NULL;
int neko_client_count = 0;

void neko_set_focus_color(neko_client client)
{
	return;
}

void neko_set_focus(neko_client *client, bool focused)
{
	client->focused = focused;
}

void neko_arrange()
{
	if (neko_client_count == 0)
	{
		return;
	}

	int window_width = screen->width_in_pixels / neko_client_count;
	for (int i = 0; i < neko_client_count; i++)
	{
		neko_client *client = &nekos[i];
		client->x = i * window_width + GAP;
		client->y = GAP;
		client->width = window_width - 2 * GAP;
		client->height = screen->height_in_pixels - 2 * GAP;

		uint32_t values[5] = { client->x, client->y, client->width, client->height, BORDER };
		uint32_t mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
			XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT |
			XCB_CONFIG_WINDOW_BORDER_WIDTH;
		xcb_configure_window(connection, client->window, mask, values);
	}
	xcb_flush(connection);
}
