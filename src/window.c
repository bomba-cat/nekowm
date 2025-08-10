#include "headers/neko.h"

neko_stack *stacks = NULL;

void neko_split_toggle()
{
  int curr_mon = neko_get_monitor_under_cursor();
  int selected_stack = selected_stacks[curr_mon];

  int focused_client = stacks[selected_stack].focused_client;
  stacks[selected_stack].clients[focused_client].split =
      !stacks[selected_stack].clients[focused_client].split;
  neko_arrange();
}

void neko_close_window()
{
  /* TODO:
   * Close window properly by trying with a cookie first
   * and if it doesn't handle that we force close it
   * */

  int curr_mon = neko_get_monitor_under_cursor();
  int selected_stack = selected_stacks[curr_mon];

  int focused_client = stacks[selected_stack].focused_client;
  xcb_window_t window = stacks[selected_stack].clients[focused_client].window;
  xcb_destroy_window(connection, window);
  neko_arrange();
}

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
  int curr_mon = neko_get_monitor_under_cursor();
  int selected_stack = selected_stacks[curr_mon];

  if ((window != 0) && (window != screen->root))
  {
    neko_stack *stack = &stacks[selected_stack];

    for (int i = 0; i < stack->client_count; i++)
    {
      if (stack->clients[i].window == window)
      {
        stack->focused_client = stack->clients[i].index;
      }
    }

    xcb_set_input_focus(connection, XCB_INPUT_FOCUS_POINTER_ROOT, window, XCB_CURRENT_TIME);
    xcb_flush(connection);
  }
}

void neko_arrange()
{
  for (int m = 0; m < monitor_count; m++)
  {
    int selected_stack = selected_stacks[m];

    if (stacks[selected_stack].client_count == 0)
    {
      continue;
    }

    int x = monitors[m].x, y;
    int w = monitors[m].width, h;
#ifdef BAR
    if (BAR_POSITION)
    {
      h = monitors[m].height - BAR_HEIGHT;
      y = monitors[m].y + BAR_HEIGHT;
    }
    else
    {
      h = monitors[m].height - BAR_HEIGHT;
      y = monitors[m].y;
    }
#else
    h = monitors[m].height;
    y = monitors[m].y;
#endif

    for (int i = 0; i < stacks[selected_stack].client_count; i++)
    {
      neko_client *client = &stacks[selected_stack].clients[i];

      xcb_get_window_attributes_cookie_t cookie =
          xcb_get_window_attributes(connection, client->window);
      xcb_get_window_attributes_reply_t *attr =
          xcb_get_window_attributes_reply(connection, cookie, NULL);

      if (attr->override_redirect)
      {
        // xcb_map_window(connection, client->window);
        free(attr);
        continue;
      }

      client->x = x + GAP;
      client->y = y + GAP;

      if (client->split == NEKO_HORIZONTAL)
      {
        client->width = (w / 2) - 2 * GAP - 2 * BORDER;
        client->height = h - 2 * GAP - 2 * BORDER;
        if (i == stacks[selected_stack].client_count - 1 && i > 0)
        {
          client->width = stacks[selected_stack].clients[i - 1].width;
        }
        else if (stacks[selected_stack].client_count == 1)
        {
          client->width = w - 2 * GAP - 2 * BORDER;
        }

        x += w / 2;
        w /= 2;
      }
      else
      {
        client->width = w - 2 * GAP - 2 * BORDER;
        client->height = (h / 2) - 2 * GAP - 2 * BORDER;
        if (i == stacks[selected_stack].client_count - 1 && i > 0)
        {
          client->height = stacks[selected_stack].clients[i - 1].height;
        }
        else if (stacks[selected_stack].client_count == 1)
        {
          client->height = h - 2 * GAP - 2 * BORDER;
        }

        y += h / 2;
        h /= 2;
      }
      uint32_t values[5] = {client->x, client->y, client->width, client->height, BORDER};
      uint32_t mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH |
                      XCB_CONFIG_WINDOW_HEIGHT | XCB_CONFIG_WINDOW_BORDER_WIDTH;
      xcb_configure_window(connection, client->window, mask, values);
    }
  }
  xcb_flush(connection);
}
