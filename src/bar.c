#include "headers/neko.h"
#include <pthread.h>

xcb_rectangle_t rect;
char *msg = "Welcome to NekoWM!";
char stack_msg[25];
char memory_msg[255];
int monitor;
bool bar_kill;

void *neko_bar_info_fetcher(void *data)
{
  UNUSED(data);
  while (true)
  {
    snprintf(memory_msg, sizeof(memory_msg), "NekoWM Memory Usage: %ld KB",
             neko_get_memory_usage());
    sleep(5);
  }
}

void neko_bar_info_fetcher_thread()
{
  pthread_t fetcher_thread;

  pthread_create(&fetcher_thread, NULL, neko_bar_info_fetcher, NULL);
}

void draw(xcb_connection_t *conn, xcb_window_t window, xcb_gcontext_t gc, neko_bar_args *bar_args)
{
  monitor = neko_find_monitor_for_window(bar_args->x, bar_args->y);
  xcb_change_gc(conn, gc, XCB_GC_FOREGROUND, (uint32_t[]){BAR_COLOR});
  xcb_poly_fill_rectangle(conn, window, gc, 1, &rect);
  xcb_change_gc(conn, gc, XCB_GC_FOREGROUND | XCB_GC_BACKGROUND,
                (uint32_t[]){BAR_TEXT_COLOR, BAR_COLOR});

  snprintf(stack_msg, sizeof(stack_msg), "|%d|", selected_stacks[monitor]);
  xcb_image_text_8(conn, strlen(stack_msg), window, gc, bar_args->width / 2 - 14,
                   bar_args->height / 2, stack_msg);

  xcb_image_text_8(conn, strlen(memory_msg), window, gc, bar_args->width - 200,
                   bar_args->height / 2, memory_msg);

  xcb_image_text_8(conn, strlen(msg), window, gc, 15, bar_args->height / 2, msg);

  xcb_flush(conn);
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
  uint32_t values[] = {BAR_COLOR, font};
  xcb_create_gc(conn, gc, window, XCB_GC_FOREGROUND | XCB_GC_FONT, values);

  xcb_map_window(conn, window);
  xcb_flush(conn);

  uint32_t v_mask = XCB_CONFIG_WINDOW_BORDER_WIDTH | XCB_CW_BORDER_PIXEL;
  uint32_t v_list[] = {BAR_BORDER, BAR_BORDER_COLOR};
  xcb_change_window_attributes(conn, window, v_mask, v_list);
  xcb_flush(conn);

  rect.x = 0;
  rect.y = 0;
  rect.width = bar_args->width;
  rect.height = bar_args->height;

  xcb_generic_event_t *event;
  while (!bar_kill)
  {
    event = xcb_poll_for_event(conn);
    if (event)
    {
      switch (event->response_type & ~0x80)
      {
      case XCB_KEY_PRESS:
      {
        running = 0;
        break;
      }
      }
      free(event);
    }
    else
    {
      draw(conn, window, gc, bar_args);
    }
    usleep(100000);
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
}
