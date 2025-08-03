#include "headers/neko.h"

int neko_sock = -1;

static char valid[3][255] = {"--split-toggle", "--exit-neko", "--close-focused"};
#ifdef SOCKET
static void (*validfun[3])() = {neko_split_toggle, neko_exit, neko_close_window};
#endif

void neko_init_socket()
{
#ifdef SOCKET
  neko_log("Initializing socket", INFO);
  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  neko_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
  int flags = fcntl(neko_sock, F_GETFL, 0);
  fcntl(neko_sock, F_SETFL, flags | O_NONBLOCK);

  addr.sun_family = AF_UNIX;
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
  addr.sun_len = sizeof(struct sockaddr_un);
#endif
  strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
  addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';

  unlink(SOCKET_PATH);
  bind(neko_sock, (struct sockaddr *)&addr, sizeof(addr));
  neko_log("Socket Initialized", INFO);
#else
  return;
#endif
}

int neko_send_message(int argc, char **argv)
{
  int sock = socket(AF_UNIX, SOCK_DGRAM, 0);

  struct sockaddr_un addr;
  addr.sun_family = AF_UNIX;
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
  addr.sun_len = sizeof(struct sockaddr_un);
#endif
  strcpy(addr.sun_path, SOCKET_PATH);
  addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';

  char msg[256] = {0};
  for (int i = 1; i < argc; ++i)
  {
    strcat(msg, argv[i]);
    if (i < argc - 1) strcat(msg, " ");
  }

  for (long unsigned int j = 0; j < sizeof(valid) / sizeof(valid[0]); j++)
  {
    if (!strcmp(msg, valid[j]))
    {
      goto passed;
    }
  }
  return 1;

passed:
  if (sendto(sock, msg, strlen(msg) + 1, 0, (struct sockaddr *)&addr, sizeof(addr)) == -1)
  {
    perror("sendto");
    close(sock);
    return 1;
  }

  close(sock);

  return 0;
}

void neko_scan_message()
{
#ifdef SOCKET
  char buf[256];
  ssize_t bytes = recvfrom(neko_sock, buf, sizeof(buf) - 1, MSG_DONTWAIT, NULL, NULL);
  if (bytes == -1)
  {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
    {
      return;
    }
    else
    {
      perror("recvfrom");
      return;
    }
  }
  buf[bytes] = '\0';
  neko_log("Received message", INFO);
  neko_log(buf, INFO);

  for (long unsigned int j = 0; j < sizeof(valid) / sizeof(valid[0]); j++)
  {
    if (!strcmp(buf, valid[j]))
    {
      validfun[j]();
    }
  }
#else
  return;
#endif
}
