#include "headers/neko.h"

sig_atomic_t running = 1;
int *selected_stacks = NULL;
neko_monitor *monitors = NULL;
int monitor_count;
int stack_count;

void neko_die(const char *msg)
{
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

long neko_get_memory_usage()
{
  FILE *file = fopen("/proc/self/status", "r");
  if (!file)
  {
    neko_log("Unable to access `/proc/self/status`. This could also be the result of an modified "
             "or different Kernel.",
             WARNING);
    return -1;
  }

  char line[256];
  long memory = -1;

  while (fgets(line, sizeof(line), file))
  {
    if (strncmp(line, "VmRSS:", 6) == 0)
    {
      sscanf(line + 6, "%ld", &memory);
      break;
    }
  }

  fclose(file);
  return memory;
}

neko_command neko_get_arguments(const char *cmd)
{
  char *copy = strdup(cmd);
  char *token = strtok(copy, " ");
  char **array = malloc(50 * sizeof(char *));

  int i = 0;
  while (token != NULL && i < 50)
  {
    array[i++] = token;
    token = strtok(NULL, " ");
  }

  array[i] = NULL;

  neko_command command = {array, copy};
  return command;
}

void neko_spawn(const char *cmd)
{
  neko_log("Spawning process", INFO);
  if (fork() == 0)
  {
    if (fork() > 0)
    {
      exit(1);
    }

    setsid();
    neko_command command = neko_get_arguments(cmd);
    execvp(command.args[0], command.args);

    free(command.args);
    free(command.buffer);

    perror("execlp");
    exit(1);
  }
  wait(NULL);
}

void neko_setup_stacks(int stack_c)
{
  stacks = realloc(stacks, sizeof(neko_stack) * stack_c);
  stack_count = stack_c;
}

void neko_add_client(xcb_window_t window)
{
  neko_log("Adding Client", INFO);
  int curr_mon = neko_get_monitor_under_cursor();
  int selected_stack = selected_stacks[curr_mon];

  neko_stack *stack = &stacks[selected_stack];

  stack->clients = realloc(stack->clients, sizeof(neko_client) * (stack->client_count + 1));
  stack->clients[stack->client_count].window = window;
  stack->clients[stack->client_count].split =
      !stack->clients[(stack->client_count > 1) ? stack->client_count - 1 : stack->client_count]
           .split;
  stack->clients[stack->client_count].index = stack->client_count;
  stack->client_count++;

  neko_arrange();
  neko_log("Added Client", INFO);
  return;
}

void neko_remove_client(xcb_window_t window)
{
  neko_log("Removing Client", INFO);
  int curr_mon = neko_get_monitor_under_cursor();
  int selected_stack = selected_stacks[curr_mon];

  if (!stacks[selected_stack].client_count)
  {
    neko_log("No clients found", WARNING);
    return;
  }

  int j = 0;
  for (int i = 0; i < stacks[selected_stack].client_count; i++)
  {
    if (stacks[selected_stack].clients[i].window != window)
    {
      stacks[selected_stack].clients[j] = stacks[selected_stack].clients[i];
      stacks[selected_stack].clients[j].index = j;
      j++;
    }
  }
  stacks[selected_stack].client_count = j;
  stacks[selected_stack].clients = realloc(
      stacks[selected_stack].clients, sizeof(neko_client) * stacks[selected_stack].client_count);
  neko_arrange();
  neko_log("Removed Client", INFO);
  return;
}

void neko_update_monitors()
{
  neko_log("Updating Monitors", INFO);
  xcb_randr_get_screen_resources_current_cookie_t res_cookie =
      xcb_randr_get_screen_resources_current(connection, screen->root);
  xcb_randr_get_screen_resources_current_reply_t *res =
      xcb_randr_get_screen_resources_current_reply(connection, res_cookie, NULL);
  if (!res)
  {
    neko_log("Updating Monitors Failed", ERROR);
    return;
  }

  xcb_randr_crtc_t *crtcs = xcb_randr_get_screen_resources_current_crtcs(res);
  int num_crtcs = xcb_randr_get_screen_resources_current_crtcs_length(res);

  free(monitors);
  monitors = calloc(num_crtcs, sizeof(neko_monitor));
  monitor_count = 0;

  for (int i = 0; i < num_crtcs; ++i)
  {
    xcb_randr_get_crtc_info_cookie_t crtc_cookie =
        xcb_randr_get_crtc_info(connection, crtcs[i], res->config_timestamp);
    xcb_randr_get_crtc_info_reply_t *crtc =
        xcb_randr_get_crtc_info_reply(connection, crtc_cookie, NULL);
    if (!crtc)
    {
      continue;
    }

    if (crtc->num_outputs > 0 && crtc->width > 0 && crtc->height > 0)
    {
      monitors[monitor_count++] =
          (neko_monitor){.x = crtc->x, .y = crtc->y, .width = crtc->width, .height = crtc->height};
    }

    free(crtc);
  }

  free(res);
  neko_log("Updated Monitors", INFO);
}

void *neko_startup(void *data)
{
  UNUSED(data);
  for (unsigned long int i = 0; i < sizeof(startup) / sizeof(neko_startup_command); i++)
  {
    char buf[255];
    sprintf(buf, "Startup: %s, Delay: %d", startup[i].command, startup[i].delay);
    neko_log(buf, INFO);
    sleep(startup[i].delay);
    neko_spawn(startup[i].command);
  }
  return NULL;
}

void neko_startup_thread()
{
  pthread_t startup_thread;

  pthread_create(&startup_thread, NULL, neko_startup, NULL);
  pthread_detach(startup_thread);
  return;
}

void neko_setup()
{
  uint32_t values[] = {XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
                       XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |
                       XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_PROPERTY_CHANGE};
  xcb_change_window_attributes(connection, screen->root, XCB_CW_EVENT_MASK, values);
  xcb_flush(connection);

  neko_log_init();
  neko_init_socket();
  neko_grab_keybinds();
  neko_startup_thread();
  neko_update_monitors();
  neko_scan_message_thread();
  selected_stacks = calloc(monitor_count, sizeof(int));
  for (int i = 0; i < monitor_count; i++)
  {
#ifdef BAR
#if BAR_POSITION
    neko_execute_bar((neko_bar_args){monitors[i].x, 0, monitors[i].width, BAR_HEIGHT});
#else
    neko_execute_bar((neko_bar_args){monitors[i].x,
                                     monitors[i].height - BAR_HEIGHT - GAP + BAR_BORDER * 2,
                                     monitors[i].width, BAR_HEIGHT});
#endif
#endif
    selected_stacks[i] = i;
  }
  xcb_flush(connection);
  neko_log("Setup Sucess", INFO);
}

int neko_find_monitor_for_window(int wx, int wy)
{
  for (int i = 0; i < monitor_count; ++i)
  {
    if (wx >= monitors[i].x && wx < monitors[i].x + monitors[i].width && wy >= monitors[i].y &&
        wy < monitors[i].y + monitors[i].height)
    {
      return i;
    }
  }
  return 0;
}

int neko_get_monitor_under_cursor()
{
  xcb_query_pointer_cookie_t pointer_cookie = xcb_query_pointer(connection, screen->root);
  xcb_query_pointer_reply_t *pointer_reply =
      xcb_query_pointer_reply(connection, pointer_cookie, NULL);

  if (!pointer_reply)
  {
    neko_log("Failed to get pointer reply", ERROR);
    return 0;
  }

  int x = pointer_reply->root_x;
  int y = pointer_reply->root_y;

  for (int i = 0; i < monitor_count; ++i)
  {
    if (x >= monitors[i].x && x < monitors[i].x + monitors[i].width && y >= monitors[i].y &&
        y < monitors[i].y + monitors[i].height)
    {
      free(pointer_reply);
      return i;
    }
  }
  free(pointer_reply);
  return 0;
}

void neko_next_stack()
{
  int monitor = neko_get_monitor_under_cursor();

  neko_stack *stack = &stacks[selected_stacks[monitor]];
  neko_unmap_stack(stack);

  selected_stacks[monitor] = (selected_stacks[monitor] + 1) % stack_count;
  for (int i = 0; i < monitor_count; i++)
  {
    if (selected_stacks[monitor] == selected_stacks[i] && monitor != i)
    {
      selected_stacks[monitor] = (selected_stacks[monitor] + 1) % stack_count;
    }
  }
  stack = &stacks[selected_stacks[monitor]];
  neko_map_stack(stack);

  neko_arrange();
  return;
}

void neko_prev_stack()
{
  int monitor = neko_get_monitor_under_cursor();

  if (selected_stacks[monitor] == 0)
  {
    return;
  }

  neko_stack *stack = &stacks[selected_stacks[monitor]];
  neko_unmap_stack(stack);

  selected_stacks[monitor] = (selected_stacks[monitor] - 1) % stack_count;
  for (int i = 0; i < monitor_count; i++)
  {
    if (selected_stacks[monitor] == selected_stacks[i] && monitor != i &&
        selected_stacks[monitor] - 1 > -1)
    {
      selected_stacks[monitor] = (selected_stacks[monitor] - 1) % stack_count;
    }
  }

  stack = &stacks[selected_stacks[monitor]];
  neko_map_stack(stack);

  neko_arrange();
  return;
}

void neko_run()
{
  xcb_generic_event_t *ev;
  while (running)
  {
    ev = xcb_poll_for_event(connection);
    if (ev)
    {
      neko_handle_events(ev);
    }
    else
    {
      usleep(50000);
    }
  }
}

void neko_exit() { neko_cleanup(0); }

void neko_cleanup(int sig)
{
  neko_log("Received SigInt|SigTerm", SEVERE);
  UNUSED(sig);
  running = 0;
  if (neko_sock != -1)
  {
    close(neko_sock);
    unlink(SOCKET_PATH);
    neko_sock = -1;
  }
  if (log_file) fclose(log_file);
  xcb_disconnect(connection);
}
