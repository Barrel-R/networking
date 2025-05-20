#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8450
#define BUFFER_SIZE 81

int server_run() {
  struct sockaddr_in client_addr, server_addr;
  struct in_addr ipAddr;

  int sockfd;
  socklen_t client_addr_len;
  ssize_t n_received;
  char buffer[81];
  char ipStr[INET_ADDRSTRLEN];

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  if (sockfd < 0) {
    perror("socket creation failed");
    return -1;
  }

  memset(&server_addr, 0, sizeof(client_addr_len));

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(PORT);

  if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("bind failed");
    close(sockfd);
    return -1;
  }

  printf("UDP server listening on port %d\n", PORT);

  while (1) {
    client_addr_len = sizeof(client_addr);

    n_received = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                          (struct sockaddr *)&client_addr, &client_addr_len);

    if (n_received < 0) {
      perror("recvfrom failed");
    }

    buffer[n_received] = '\0';

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

    printf("Received %ld bytes from %s:%d: %s\n", n_received, client_ip,
           ntohs(client_addr.sin_port), buffer);

    for (int i = 0; i < n_received; i++) {
      if (buffer[i] >= 'a' && buffer[i] <= 'z') {
        buffer[i] = buffer[i] - ('a' - 'A');
      }
    }

    printf("Sending back: %s", buffer);

    if (sendto(sockfd, buffer, n_received, 0, (struct sockaddr *)&client_addr,
               client_addr_len) < 0) {
      perror("sendto failed");
    }
  }

  close(sockfd);

  return 0;
}

int main() {
  puts("Server starting");

  if (server_run() < 0) {
    puts("The server encountered an error and stopped.");
  }
  return 1;
}
