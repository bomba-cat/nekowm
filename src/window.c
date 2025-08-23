#include "headers/neko.h"
#include <xcb/xcb.h>
#include <xcb/xproto.h>

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
  xcb_intern_atom_cookie_t protocol_cookie = xcb_intern_atom(connection, 1, 12, "WM_PROTOCOLS");
  xcb_intern_atom_cookie_t delete_cookie = xcb_intern_atom(connection, 0, 16, "WM_DELETE_WINDOW");

  xcb_intern_atom_reply_t *protocol_reply = xcb_intern_atom_reply(connection, protocol_cookie, 0);
  xcb_intern_atom_reply_t *delete_reply = xcb_intern_atom_reply(connection, delete_cookie, 0);

  int curr_mon = neko_get_monitor_under_cursor();
  int selected_stack = selected_stacks[curr_mon];

  int focused_client = stacks[selected_stack].focused_client;
  xcb_window_t window = stacks[selected_stack].clients[focused_client].window;

  if (!protocol_reply || !delete_reply)
  {
    neko_log("protocol_reply or delete_reply failed, destroying window instead", WARNING);
    xcb_destroy_window(connection, window);
  }
  else
  {
    neko_log("Closing window", INFO);
    xcb_client_message_event_t event = {0};
    event.response_type = XCB_CLIENT_MESSAGE;
    event.format = 32;
    event.window = window;
    event.type = protocol_reply->atom;
    event.data.data32[0] = delete_reply->atom;
    event.data.data32[1] = XCB_CURRENT_TIME;

    xcb_send_event(connection, 0, window, XCB_EVENT_MASK_NO_EVENT, (char *)&event);
  }

  free(protocol_reply);
  free(delete_reply);
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

void neko_next_focus()
{
  int monitor = neko_get_monitor_under_cursor();
  int selected_stack = selected_stacks[monitor];

  neko_stack *stack = &stacks[selected_stack];

  stack->focused_client++;

  xcb_set_input_focus(connection, XCB_INPUT_FOCUS_POINTER_ROOT,
                      stack->clients[stack->focused_client].window, XCB_CURRENT_TIME);
  xcb_flush(connection);
}

void neko_prev_focus()
{
  int monitor = neko_get_monitor_under_cursor();
  int selected_stack = selected_stacks[monitor];

  neko_stack *stack = &stacks[selected_stack];

  stack->focused_client--;

  xcb_set_input_focus(connection, XCB_INPUT_FOCUS_POINTER_ROOT,
                      stack->clients[stack->focused_client].window, XCB_CURRENT_TIME);
  xcb_flush(connection);
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
      if (!attr)
      {
        free(attr);
        continue;
      }

      if (attr->map_state != XCB_MAP_STATE_VIEWABLE)
      {
        neko_log("Unviewable window", INFO);
        free(attr);
        continue;
      }

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
      free(attr);
    }
  }
  xcb_flush(connection);
  return;
}

void neko_unmap_stack(neko_stack *stack)
{
  for (int i = 0; i < stack->client_count; i++)
  {
    xcb_unmap_window(connection, stack->clients[i].window);
  }
  xcb_flush(connection);
  return;
}

void neko_map_stack(neko_stack *stack)
{
  for (int i = 0; i < stack->client_count; i++)
  {
    xcb_map_window(connection, stack->clients[i].window);
  }
  xcb_flush(connection);
  return;
}
