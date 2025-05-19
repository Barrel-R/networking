#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int server() {
  struct sockaddr_in from, me;
  char linha[81];
  int err;

  int sock, addrlen = sizeof(me);

  sock = socket(AF_INET, SOCK_DGRAM, 0);

  bzero((char *)&me, addrlen);

  me.sin_family = AF_INET;
  me.sin_addr.s_addr = htonl(INADDR_ANY);
  me.sin_port = htons(8450);

  err = bind(sock, (struct sockaddr *)&me, addrlen);

  if (err == -1) {
    switch (errno) {
    case EADDRINUSE:
      puts("The address is already in use.");
      return err;
    default:
      perror("bind");
      return err;
    }
  }

  printf("Server is listening at %d", INADDR_ANY);

  do {
    ssize_t n = recvfrom(sock, linha, 81, 0, (struct sockaddr *)&from,
             (socklen_t *)&addrlen);

    if (n > 0) {
      linha[n] = '\0';
      printf("Received: %s", linha);
    }

    for (int i = 0; linha[i] != '\0'; i++) {
      if (linha[i] >= 'a' && linha[i] <= 'z') {
        linha[i] = linha[i] - ('a' - 'A');
      }
    }

    printf("New line: %s", linha);

  } while (strcmp(linha, "exit"));

  close(sock);

  return err;
}

int main() {
  puts("Server starting");

  int err;
  err = server();

  return err;
}
