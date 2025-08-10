#include "headers/neko.h"
#include <xcb/xcb.h>
#include <xcb/xproto.h>

void *neko_create_bar(void *args)
{
  neko_bar_args bar_args = (neko_bar_args)&args;
  xcb_connection_t *conn = xcb_connect(NULL, NULL);
  xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;

  xcb_window_t window = xcb_generate_id(conn);

  uint32_t value_mask = XCB_CW_OVERRIDE_REDIRECT;
  uint32_t value_list[] = {1};

  /* TODO: Bar Border */
  xcb_create_window(conn, XCB_COPY_FROM_PARENT, window, screen->root, bar_args.x, bar_args.y,
                    bar_args.width, bar_args.height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    screen->root_visual, value_mask, value_list);

  xcb_gcontext_t gc = xcb_generate_id(conn);
  uint32_t gc_values[] = {screen->white_pixel, 0};
  xcb_create_gc(conn, gc, window, XCB_GC_FOREGROUND | XCB_GC_LINE_WIDTH, gc_values);

  xcb_rectangle_t rect = {0, 0, bar_args.width, bar_args.height};
  xcb_poly_fill_rectangle(conn, window, gc, 1, &rect);

  xcb_flush(conn);

  xcb_generic_event_t *event;
  while ((event = xcb_wait_for_event(conn)))
  {
    switch (event->response_type & ~0x80)
    {
      free(event);
      xcb_disconnect(conn);
      break;
    }
    free(event);
  }

  xcb_disconnect(conn);
  return NULL;
}

void neko_execute_bar(neko_bar_args args)
{
  pthread_t bar_thread;

  pthread_create(&bar_thread, NULL, neko_create_bar, &args);
}
