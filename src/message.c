#include "headers/neko.h"

void neko_send_message(int argc, char **argv)
{
  int sock = socket(AF_UNIX, SOCK_DGRAM, 0);

  struct sockaddr_un client = {0}, server = {0};
  client.sun_family = AF_UNIX;
  server.sun_family = AF_UNIX;
  snprintf(client.sun_path, sizeof(client.sun_path), SOCKET_PATH);
  strcpy(server.sun_path, SOCKET_PATH);

  unlink(client.sun_path);
  bind(sock, (struct sockaddr*)&client, sizeof(client));

  char msg[256] = {0};
  for (int i = 1; i < argc; ++i)
  {
    strcat(msg, argv[i]);
    if (i < argc - 1) strcat(msg, " ");
  }

  sendto(sock, msg, strlen(msg), 0, (struct sockaddr*)&server, sizeof(server));

  char buf[256];
  int n = recvfrom(sock, buf, sizeof(buf), 0, NULL, NULL);
  if(n > 0) write(1, buf, n);

  close(sock);
  unlink(client.sun_path);
}
