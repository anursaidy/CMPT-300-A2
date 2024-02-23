#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "List.h"

#define MSG_MAX_LEN 1024

/**
 * Send thread should lock its current mutex then send a signal for the recieve condition variable
 * Then after we unlock the current thread again
 * Brackets within will be our CS?
 * Vice versa for the recieve
*/

struct threadData{
  int socket;
  struct addrinfo *servinfo;
  List** givenList;
};

struct ListIo{
  List** givenList;
};

void *sendThread(void* data)
{
    struct ListIo* sendData = (struct ListIo*) data;

    while(1){
      char message[MSG_MAX_LEN];
      // SEND MESSAGE
    
      fgets(message, sizeof(message), stdin);
      if(List_append(&sendData->givenList,message) == -1){
        printf("Error adding message to outbound list\n");
      }
      else{
        printf("Successfully added item to outbound list\n");
      }

  }
    return NULL;
}

void *receiveThread(void* data)
{
  struct ListIo* receiveData = (struct ListIo*) data;

  // Receive data
 struct sockaddr_storage servAddr;
    while (1)
  {
    char serverBuffer[MSG_MAX_LEN];
    socklen_t servAddrLen = sizeof servAddr;

    int recv_len = recvfrom(receiveData->socket, serverBuffer, sizeof(serverBuffer), 0, (struct sockaddr *)&servAddr, &servAddrLen);
    if (recv_len == -1)
    {
      perror("recvfrom error");
    }

    serverBuffer[recv_len] = '\0'; // Null-terminate the received data

    // printf("Received message: %s\n", serverBuffer); // Receive data

    int addStatus = List_append(&recieveData->givenList,serverBuffer);
    if(addStatus == -1){
      printf("Message has not been recieved!\n");
    }
    else{
      printf("Message recieved successfully\n");
    }

  }

  return NULL;
}

void* sendToClient(void* data){
  struct threaddata* sendData = (struct threadData*) data;

  while(1){
    char message[MSG_MAX_LEN];

    void* messageToSend = List_trim(&sendData->givenList);

    if(messageToSend != NULL){
      message = (char*) messageToSend;
      int send_len = sendto(sendData->socket, message, strlen(message), 0, sendData->servinfo->ai_addr, sendData->servinfo->ai_addrlen);
      if (send_len == -1)
      {
        perror("sendto error");
      }
      else
      {
        printf("Sent %d bytes\n\n", send_len);
      }
    }
    else{
      printf("No messages to send\n");
    }
  }
  
}

void* displayMessage(void* data){
  //TODO
  struct ListIo* inBoundList = (struct ListIo*) data;

  while(1){
    char serverBuffer[MSG_MAX_LEN];
    void* messageToDisplay = List_trim(&sendData->givenList);
    if(messageToDisplay != NULL){
      serverBuffer = (char*) messageToDisplay;
      if (send_len == -1)
      {
        perror("sendto error");
      }
      else
      {
        printf("Message: %s\n\n", )serverBuffer;
      }
    }
    else{
      printf("No messages recieved\n");
    }
  }
}

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

  /////////////////////////
  // Send and Receive Data
  ////////////////////////
  pthread_t receiveThreadPID;
  pthread_t sendThreadPID;
  struct ListIo threadSendData;
  struct ListIo threadReceiveData;

  /*********List and respective threads************/

  pthread_t inboundThread;
  pthread_cond_t okToRecvCondVar = PTHREAD_COND_INITIALIZER;
  pthread_mutex_t okToRecvMutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_t outboundThread;
  pthread_cond_t okToDisplayCondVar = PTHREAD_COND_INITIALIZER;
  pthread_mutex_t okToDisplayMutex = PTHREAD_MUTEX_INITIALIZER;

  List* outBoundList = List_create();
  List* inBoundList = List_create();

  /************************************************/
  threadSendData.givenList = &outBoundList;

  threadReceiveData.givenList = &inBoundList;

  printf("\nStart Messaging:\n" );
  pthread_create(&sendThreadPID, NULL, sendThread, (void*)&threadSendData);
  pthread_create(&receiveThreadPID, NULL, receiveThread, (void*)&threadReceiveData);
  
  pthread_join(sendThreadPID, NULL);
  pthread_join(receiveThreadPID, NULL);
  pthread_join(inboundThread,NULL);
  pthread_join(outboundThread,NULL);
 

  close(clientSocket);
  freeaddrinfo(servinfo2);
  close(serverSocket);
  freeaddrinfo(servinfo);
}