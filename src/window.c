#include "headers/neko.h"

neko_stack *stacks = NULL;
int selected_stack = 0;

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
		xcb_set_input_focus(connection, XCB_INPUT_FOCUS_POINTER_ROOT, window, XCB_CURRENT_TIME);
		xcb_flush(connection);
	}
}

void neko_arrange()
{
	if (stacks[selected_stack].client_count == 0)
	{
		return;
	}

	int x = 0, y = 0;
	int w = screen->width_in_pixels, h = screen->height_in_pixels;

	for (int i = 0; i < stacks[selected_stack].client_count; i++)
	{
		neko_client *client = &stacks[selected_stack].clients[i];

		client->x = x + GAP;
		client->y = y + GAP;

		if (client->split == NEKO_HORIZONTAL)
		{
			client->width = (w / 2) - 2 * GAP - 2 * BORDER;
			client->height = h - 2 * GAP - 2 * BORDER;
			if (i == stacks[selected_stack].client_count-1 && i > 0)
			{
				client->width = stacks[selected_stack].clients[i-1].width;
			} else if(stacks[selected_stack].client_count == 1)
			{
				client->width = w - 2 * GAP - 2 * BORDER;
			}

			x += w / 2;
			w /= 2;
		} else
		{
			client->width = w - 2 * GAP - 2 * BORDER;
			client->height = (h / 2) - 2 * GAP - 2 * BORDER;
			if (i == stacks[selected_stack].client_count-1 && i > 0)
			{
				client->height = stacks[selected_stack].clients[i-1].height;
			} else if(stacks[selected_stack].client_count == 1)
			{
				client->height = h - 2 * GAP - 2 * BORDER;
			}

			y += h / 2;
			h /= 2;
		}
		uint32_t values[5] = { client->x, client->y, client->width, client->height, BORDER };
		uint32_t mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
			XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT |
			XCB_CONFIG_WINDOW_BORDER_WIDTH;
		xcb_configure_window(connection, client->window, mask, values);
	}
	xcb_flush(connection);
}
