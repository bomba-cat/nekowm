#include "headers/nekowm.h"

void nekowm_map_window(xcb_window_t window)
{
	xcb_map_window(connection, window);
	xcb_flush(connection);

	nekowm_window_count++;
	nekowm_windows = realloc(nekowm_windows, sizeof(nekowm_window_t) * nekowm_window_count);
	nekowm_window_t newWindow =
	{
		.index = nekowm_window_count,
		.width = 0,
		.height = screen->height_in_pixels - (NEKOWM_WINDOW_SPACING*2),
		.x = 0,
		.y = NEKOWM_WINDOW_SPACING,
		.window = window,
	};
	nekowm_windows[nekowm_window_count-1] = newWindow;

	for (int i = 0; i < nekowm_window_count; i++)
	{
		nekowm_windows[i].x = (screen->width_in_pixels / nekowm_window_count) * i;
		nekowm_windows[i].width = screen->width_in_pixels / nekowm_window_count - (NEKOWM_WINDOW_SPACING * 2);
		
		if (nekowm_windows[i].x != 0)
		{
			nekowm_windows[i].x += NEKOWM_WINDOW_SPACING/2;
		} else
		{
			nekowm_windows[i].x += NEKOWM_WINDOW_SPACING;
		}
	}
}

void nekowm_destroy_window(xcb_window_t window)
{
    int j = 0;
    for (int i = 0; i < nekowm_window_count; i++)
    {
        if (nekowm_windows[i].window == window)
					continue;
        nekowm_windows[j++] = nekowm_windows[i];
    }

    nekowm_window_count = j;
		nekowm_windows = realloc(nekowm_windows, sizeof(nekowm_window_t) * nekowm_window_count);
}

void nekowm_show_windows()
{
	for(int i = 0; i < nekowm_window_count; i++)
	{
		nekowm_show_window(nekowm_windows[i]);
	}
}

void nekowm_show_window(nekowm_window_t window)
{
	uint32_t values[4] = { window.x, window.y, window.width, window.height };
	uint16_t mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
									XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
	xcb_configure_window(connection, window.window, mask, values);

	xcb_configure_notify_event_t event =
	{
		.response_type = XCB_CONFIGURE_NOTIFY,
		.event = window.window,
		.window = window.window,
		.above_sibling = XCB_NONE,
		.x = values[0],
		.y = values[1],
		.width = values[2],
		.height = values[3],
		.border_width = 15,
		.override_redirect = 0
	};
	xcb_send_event(connection, 0, window.window, XCB_EVENT_MASK_STRUCTURE_NOTIFY, (char *)&event);

	xcb_flush(connection);
}
