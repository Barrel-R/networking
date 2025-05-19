#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv) {
  struct sockaddr_in to;
  in_addr_t addr = inet_addr(argv[1]);
  char linha[81];
  int err;

  if (argv[1] == NULL) {
    puts("You must inform the address.");
    return -1;
  } else {
    printf("Address: %s\n", argv[1]);
  }

  int sock, addrlen = sizeof(to);

  sock = socket(AF_INET, SOCK_DGRAM, 0);

  bzero((char *)&to, sizeof(to));

  to.sin_family = AF_INET;
  to.sin_addr.s_addr = addr;
  to.sin_port = htons(8450);

  err = bind(sock, (struct sockaddr *)&to, addrlen);

  if (err == -1) {
    perror("bind");
    return err;
  }

  do {
    fgets(linha, 81, stdin);
    sendto(sock, linha, strlen(linha) + 1, 0, (struct sockaddr *)&to, sizeof(to));
  } while (strcmp(linha, "exit\n") != 0);

  close(sock);

  return 0;
}
