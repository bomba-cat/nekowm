#include "headers/neko.h"

void neko_handle_key_press(xcb_generic_event_t *event)
{
  if (!keysyms)
  {
    return;
  }

  xcb_key_press_event_t *e = (xcb_key_press_event_t *)event;
  neko_read_keybinds(e);
}

void neko_handle_focus_in(xcb_generic_event_t *event)
{
  xcb_focus_in_event_t *e = (xcb_focus_in_event_t *)event;
  neko_set_focus_color(e->event, true);
}

void neko_handle_focus_out(xcb_generic_event_t *event)
{
  xcb_focus_in_event_t *e = (xcb_focus_in_event_t *)event;
  neko_set_focus_color(e->event, false);
}

void neko_handle_enter_notify(xcb_generic_event_t *event)
{
  xcb_enter_notify_event_t *e = ( xcb_enter_notify_event_t *)event;
	neko_set_focus(e->event);
}

void neko_handle_map(xcb_generic_event_t *event)
{
  xcb_map_request_event_t *e = (xcb_map_request_event_t *)event;
  xcb_map_window(connection, e->window);
  
  uint32_t mask = XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_FOCUS_CHANGE;
  xcb_change_window_attributes(connection, e->window, XCB_CW_EVENT_MASK, &mask);
  
  neko_add_client(e->window);
}

void neko_handle_destroy(xcb_generic_event_t *event)
{
  xcb_destroy_notify_event_t *e = (xcb_destroy_notify_event_t *)event;
  neko_remove_client(e->window);
}

void neko_handle_events(xcb_generic_event_t *event)
{
	switch (event->response_type & ~0x80)
		{
      case XCB_MAP_REQUEST:
				{
          neko_handle_map(event);
          break;
        }
      case XCB_DESTROY_NOTIFY:
				{
          neko_handle_destroy(event);
          break;
        }
      case XCB_FOCUS_IN:
        {
          neko_handle_focus_in(event);
          break;
        }
      case XCB_FOCUS_OUT:
        {
          neko_handle_focus_out(event);
          break;
        }
      case XCB_ENTER_NOTIFY:
        {
          neko_handle_enter_notify(event);
          break;
        }
      case XCB_KEY_PRESS:
        {
          neko_handle_key_press(event);
          break;
        }
    }
    free(event);
}
