#include "headers/neko.h"

int neko_sock = -1;

static char valid[2][255] =
  {
    "--split-toggle",
    "--exit-neko"
  };

void neko_init_socket()
{
  neko_log("Initializing socket", INFO);
  struct sockaddr_un addr;
  neko_sock = socket(AF_UNIX, SOCK_DGRAM, 0);

  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, SOCKET_PATH);

  unlink(SOCKET_PATH);
  bind(neko_sock, (struct sockaddr*)&addr, sizeof(addr));
  neko_log("Socket Initialized", INFO);
}

int neko_send_message(int argc, char **argv)
{
  int sock = socket(AF_UNIX, SOCK_DGRAM, 0);

  struct sockaddr_un addr;
  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, SOCKET_PATH);

  char msg[256] = {0};
  for (int i = 1; i < argc; ++i) {
    strcat(msg, argv[i]);
    if (i < argc - 1) strcat(msg, " ");

    for(long unsigned int j = 0; j < sizeof(valid)/sizeof(char); j++)
    {
      if (!strcmp(msg, valid[j]))
      {
        goto passed;
      }
    }
    return 1;
  }

passed:
  sendto(sock, msg, strlen(msg) + 1, 0, (struct sockaddr*)&addr, sizeof(addr));

  close(sock);

  return 0;
}

void neko_scan_message()
{
  char buf[100];
  if (recvfrom(neko_sock, buf, sizeof(buf), MSG_DONTWAIT, NULL, NULL) > 0)
  {
    neko_log("Received message", INFO);
    /* TODO: Parse and execute */
  }
}
