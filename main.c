#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define MSG_MAX_LEN 1024
#define PORT 22110

int main(int argc, char *argv[])
{
  /////////////////////////
  // Server Socket
  ////////////////////////

  const char *myPort = argv[1];
  const char *remoteAddress = argv[2];
  const char *remotePort = argv[3];

  // Set up Structure to hold address information
  struct addrinfo hints;
  struct addrinfo *servinfo;
  struct sockaddr_storage servAddr;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE; // Automatically input IP address

  // Get Address Information of Host Machine

  int serverStatus = getaddrinfo(NULL, myPort, &hints, &servinfo); // IP parameter set to null since it is set to default
  if (serverStatus == 0)
  {
    printf("Server Address Works\n");
  }

  // Create Socket Descriptor
  int serverSocket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
  if (serverSocket != -1)
  {
    printf("Server Socket Creation Success\n");
  }

  // Associate socket to the specific address
  if (bind(serverSocket, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
  {
    perror("Server bind");
    close(serverSocket);
    freeaddrinfo(servinfo);
    exit(EXIT_FAILURE);
  }
  else
  {
    printf("Server Bind Success\n");
  }

  /////////////////////////
  // Client Socket
  ////////////////////////
  struct addrinfo hintsClient;
  struct addrinfo *servinfo2;

  memset(&hintsClient, 0, sizeof hintsClient);
  hintsClient.ai_family = AF_UNSPEC;
  hintsClient.ai_socktype = SOCK_DGRAM;
  hintsClient.ai_flags = 0;

  int clientStatus = getaddrinfo(remoteAddress, remotePort, &hintsClient, &servinfo2);
  if (clientStatus == 0)
  {
    printf("Client Address Works\n");
  }
  else
  {
    printf("Client getaddrinfo error");
  }
  int clientSocket = socket(servinfo2->ai_family, servinfo2->ai_socktype, servinfo2->ai_protocol);
  if (clientSocket != -1)
  {
    printf("Client Socket Creation Success\n");
  }
  else
  {
    printf("Client Socket Creation fail\n");
  }

  while (1)
  {

    char message[MSG_MAX_LEN];
    // SEND MESSAGE
    printf("Enter a message:");
    fgets(message, sizeof(message), stdin);

    int send_len = sendto(clientSocket, message, strlen(message), 0, servinfo2->ai_addr, servinfo2->ai_addrlen);
    if (send_len == -1)
    {
      perror("sendto error");
    }
    else
    {
      printf("Sent %d bytes\n", send_len);
    }

    // Receive data
    char serverBuffer[MSG_MAX_LEN];
    socklen_t servAddrLen = sizeof servAddr;

    int recv_len = recvfrom(serverSocket, serverBuffer, sizeof(serverBuffer), 0, (struct sockaddr *)&servAddr, &servAddrLen);
    if (recv_len == -1)
    {
      perror("recvfrom error");
    }

    serverBuffer[recv_len] = '\0'; // Null-terminate the received data

    printf("Received message from the server: %s\n", serverBuffer);
  }

  close(clientSocket);
  freeaddrinfo(servinfo2);
  close(serverSocket);
  freeaddrinfo(servinfo);
}