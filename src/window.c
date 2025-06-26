#include "headers/neko.h"

neko_client *nekos = NULL;
int neko_client_count = 0;

void neko_set_focus_color(xcb_window_t window, bool focus)
{
	if ((BORDER > 0) && (screen->root != window) && (0 != window))
	{
		uint32_t values[1];
		values[0] = focus ? FOCUSED : UNFOCUSED;
		xcb_change_window_attributes(connection, window, XCB_CW_BORDER_PIXEL, values);
		xcb_flush(connection);
	}
}

void neko_set_focus(xcb_drawable_t window)
{
	if ((window != 0) && (window != screen->root))
	{
		xcb_set_input_focus(connection, XCB_INPUT_FOCUS_POINTER_ROOT, window,
			XCB_CURRENT_TIME);
	}
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
		xcb_flush(connection);

		uint32_t vals[5];
		vals[0] = XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_FOCUS_CHANGE;
		xcb_change_window_attributes_checked(connection, client->window,
				XCB_CW_EVENT_MASK, vals);
		neko_set_focus(client->window);
	}
}
