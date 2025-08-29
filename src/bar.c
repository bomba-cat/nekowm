#include "headers/neko.h"

xcb_rectangle_t rect;
char stack_msg[25];
char memory_msg[255];
char *msg = "Welcome to NekoWM!";

void draw(xcb_connection_t *conn, xcb_window_t window, xcb_gcontext_t gc, neko_bar_args *bar_args)
{
  int monitor = neko_find_monitor_for_window(bar_args->x, bar_args->y);
  xcb_change_gc(conn, gc, XCB_GC_FOREGROUND, (uint32_t[]){BAR_COLOR});
  xcb_poly_fill_rectangle(conn, window, gc, 1, &rect);
  xcb_change_gc(conn, gc, XCB_GC_FOREGROUND, (uint32_t[]){0xFFFFFF});

  snprintf(stack_msg, sizeof(stack_msg), "Stack number: %d", selected_stacks[monitor]);
  uint8_t *stack_items = malloc(strlen(stack_msg) + 2);
  stack_items[0] = 0;
  stack_items[1] = strlen(stack_msg);
  memcpy(stack_items + 2, stack_msg, strlen(stack_msg));
  xcb_poly_text_8(conn, window, gc, bar_args->width / 2 - 50, bar_args->y / 2, sizeof(stack_items),
                  stack_items);

  snprintf(memory_msg, sizeof(memory_msg), "NekoWM Memory Usage: %ld KB", neko_get_memory_usage());
  uint8_t *memory_items = malloc(strlen(memory_msg) + 2);
  memory_items[0] = 0;
  memory_items[1] = strlen(memory_msg);
  memcpy(memory_items + 2, memory_msg, strlen(memory_msg));
  xcb_poly_text_8(conn, window, gc, bar_args->width - 250, bar_args->y / 2, sizeof(memory_items),
                  memory_items);

  uint8_t *items = malloc(strlen(msg) + 2);
  items[0] = 0;
  items[1] = strlen(msg);
  memcpy(items + 2, msg, strlen(msg));
  xcb_poly_text_8(conn, window, gc, 15, bar_args->height / 2, strlen(msg), items);

  xcb_flush(conn);
  free(stack_items);
  free(memory_items);
  free(items);
}

void *neko_create_bar(void *args)
{
  xcb_connection_t *conn;
  xcb_screen_t *scr;
  xcb_window_t window;
  xcb_gcontext_t gc;

  neko_log("Created bar thread", INFO);
  neko_bar_args *bar_args = (neko_bar_args *)args;

  if (!bar_args || bar_args->width == 0 || bar_args->height == 0)
  {
    neko_log("Invalid bar_args (zero size or null), Test2", ERROR);
    free(bar_args);
    return NULL;
  }

  conn = xcb_connect(NULL, NULL);
  scr = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;

  window = xcb_generate_id(conn);

  uint32_t value_mask = XCB_CW_OVERRIDE_REDIRECT;
  uint32_t value_list[] = {1};

  /* TODO: Bar Border */
  xcb_create_window(conn, XCB_COPY_FROM_PARENT, window, scr->root, bar_args->x + BORDER,
                    bar_args->y + BORDER, bar_args->width - BORDER * 4,
                    bar_args->height - BORDER * 2, BORDER, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    scr->root_visual, value_mask, value_list);

  xcb_font_t font = xcb_generate_id(conn);
  xcb_open_font(conn, font, strlen("fixed"), "fixed");

  gc = xcb_generate_id(conn);
  uint32_t values[] = {(uint32_t)BAR_COLOR, font};
  xcb_create_gc(conn, gc, window, XCB_GC_FOREGROUND | XCB_GC_FONT, values);

  xcb_map_window(conn, window);
  xcb_flush(conn);

  rect.x = 0;
  rect.y = 0;
  rect.width = bar_args->width;
  rect.height = bar_args->height;

  xcb_generic_event_t *event;
  int running = 1;
  while (running)
  {
    event = xcb_poll_for_event(conn);
    if (event)
    {
      switch (event->response_type & ~0x80)
      {
      case XCB_KEY_PRESS:
        running = 0;
        break;
      }
      free(event);
    }
    else
    {
      draw(conn, window, gc, bar_args);
    }
    usleep(5000000);
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
