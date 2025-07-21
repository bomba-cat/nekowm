#include "headers/neko.h"

int neko_sock = -1;

void neko_init_socket()
{
  struct sockaddr_un addr;
  neko_sock = socket(AF_UNIX, SOCK_DGRAM, 0);

  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, SOCKET_PATH);

  unlink(SOCKET_PATH);
  bind(neko_sock, (struct sockaddr*)&addr, sizeof(addr));
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

    /* TODO: Check if valid, if not close and return 1 */
  }

  sendto(sock, msg, strlen(msg) + 1, 0, (struct sockaddr*)&addr, sizeof(addr));

  close(sock);

  return 0;
}

void neko_scan_message()
{
  char buf[100];
  if (recvfrom(neko_sock, buf, sizeof(buf), MSG_DONTWAIT, NULL, NULL) > 0) {
    printf("Received: %s\n", buf);
  }
}
