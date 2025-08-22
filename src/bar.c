#include "headers/neko.h"

void *neko_create_bar(void *args)
{
  neko_log("Created bar thread", INFO);
  neko_bar_args *bar_args = (neko_bar_args *)args;

  if (!bar_args || bar_args->width == 0 || bar_args->height == 0)
  {
    neko_log("Invalid bar_args (zero size or null), Test2", ERROR);
    free(bar_args);
    return NULL;
  }

  xcb_connection_t *conn = xcb_connect(NULL, NULL);
  xcb_screen_t *scr = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;

  xcb_window_t window = xcb_generate_id(conn);

  uint32_t value_mask = XCB_CW_OVERRIDE_REDIRECT;
  uint32_t value_list[] = {1};

  /* TODO: Bar Border */
  xcb_create_window(conn, XCB_COPY_FROM_PARENT, window, scr->root, bar_args->x + BORDER,
                    bar_args->y + BORDER, bar_args->width - BORDER * 4,
                    bar_args->height - BORDER * 2, BORDER, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    scr->root_visual, value_mask, value_list);

  xcb_font_t font = xcb_generate_id(conn);
  xcb_open_font(conn, font, strlen("fixed"), "fixed");

  xcb_gcontext_t gc = xcb_generate_id(conn);
  uint32_t values[] = {scr->white_pixel, font};
  xcb_create_gc(conn, gc, window, XCB_GC_FOREGROUND | XCB_GC_FONT, values);

  xcb_map_window(conn, window);
  xcb_flush(conn);

  xcb_rectangle_t rect = {0, 0, bar_args->width, bar_args->height};
  char stack_msg[25];
  char memory_msg[255];
  char *msg = "Welcome to NekoWM!";

  xcb_generic_event_t *event;
  int running = 1;
  while (running)
  {
    int monitor = neko_get_monitor_under_cursor();
    snprintf(stack_msg, sizeof(stack_msg), "Stack number: %d", selected_stacks[monitor]);
    snprintf(memory_msg, sizeof(memory_msg), "NekoWM Memory Usage: %ld KB",
             neko_get_memory_usage());

    xcb_poly_fill_rectangle(conn, window, gc, 1, &rect);
    xcb_image_text_8(conn, strlen(msg), window, gc, 15, bar_args->height / 2, msg);
    xcb_image_text_8(conn, strlen(stack_msg), window, gc, bar_args->width / 2 - 50,
                     bar_args->height / 2, stack_msg);
    xcb_image_text_8(conn, strlen(memory_msg), window, gc, bar_args->width - 250,
                     bar_args->height / 2, memory_msg);
    xcb_flush(conn);

    while ((event = xcb_poll_for_event(conn)))
    {
      switch (event->response_type & ~0x80)
      {
      case XCB_KEY_PRESS:
        running = 0;
        break;
      }
      free(event);
    }

    usleep(50000);
  }

  xcb_close_font(conn, font);
  xcb_free_gc(conn, gc);
  xcb_disconnect(conn);
  neko_log("Exiting bar thread", WARNING);
  free(bar_args);
  return NULL;
}

void neko_execute_bar(neko_bar_args args)
{
  pthread_t bar_thread;

  if (args.width == 0 || args.height == 0)
  {
    neko_log("Invalid bar_args (zero size or null), Test1", ERROR);
  }

  neko_bar_args *heap_args = malloc(sizeof(neko_bar_args));
  *heap_args = args;
  pthread_create(&bar_thread, NULL, neko_create_bar, heap_args);
  pthread_detach(bar_thread);
}
