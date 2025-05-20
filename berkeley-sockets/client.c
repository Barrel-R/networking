#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 8450
#define BUFFER_SIZE 81

int main(int argc, char **argv) {
  struct sockaddr_in server_addr;
  char send_buffer[BUFFER_SIZE];
  char recv_buffer[BUFFER_SIZE];
  ssize_t n_sent, n_received;
  socklen_t server_addr_len;
  int sockfd;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <server_ip_address>\n", argv[0]);
    return 1;
  }

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  memset(&server_addr, 0, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(SERVER_PORT);

  if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
    perror("inet_pton failed");
    return 1;
  }

  printf("Enter lines of text to send to server: (%s:%d). Type 'exit' to quit\n", argv[1], SERVER_PORT);

  while (1) {
    printf("> ");

    if (fgets(send_buffer, BUFFER_SIZE, stdin) == NULL) {
      break;
    }

    n_sent = sendto(sockfd, send_buffer, strlen(send_buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (n_sent < 0) {
      perror("sendto failed");
      continue;
    }

    if (strcmp(send_buffer, "exit\n") == 0) {
      printf("Exiting client\n");
      break;
    }

    n_received = recvfrom(sockfd, recv_buffer, BUFFER_SIZE - 1, 0, NULL, NULL);

    if (n_received < 0) {
      perror("recvfrom failed");
      continue;
    }

    recv_buffer[n_received] = '\0';
    printf("Server replied: %s", recv_buffer);
  }

  close(sockfd);

  return 0;
}
