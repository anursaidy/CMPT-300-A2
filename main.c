#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "list.h"

#define MSG_MAX_LEN 1024

pthread_cond_t okToRecvCondVar = PTHREAD_COND_INITIALIZER;
pthread_mutex_t okToRecvMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t okToSendCondVar = PTHREAD_COND_INITIALIZER;
pthread_mutex_t okToSendMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_t inboundThread;
pthread_t outboundThread;
pthread_t receiveThreadPID;
pthread_t sendThreadPID;

static bool exitProgram = false;

struct threadData
{
  int socket;
  struct addrinfo *servinfo;
  List *givenList;
};

struct ListIo
{
  List *givenList;
};

void freeNodes(void* itemToFree){
  free(itemToFree);
};

// Keyboard Input Thread
void *getUserInputThread(void *data)
{
  struct ListIo *sendData = (struct ListIo *)data;

  while (1)
  {

    char message[MSG_MAX_LEN];
    // SEND MESSAGE

    fgets(message, sizeof(message), stdin);

    pthread_mutex_lock(&okToSendMutex);

    if(strcmp(message,"!\n") == 0){
      exitProgram = true;
    }

    if (List_append(sendData->givenList, message) == -1 && exitProgram)
    {
      printf("Error adding message to outbound list\n");
    }
    else
    {
      printf("Successfully added item to outbound list\n");
      pthread_cond_signal(&okToSendCondVar);
    }

    pthread_mutex_unlock(&okToSendMutex);

    if(exitProgram){
      printf("Terminating session\n");
      printf("Exiting user input thread\n");
      pthread_cancel(outboundThread);
      pthread_cancel(receiveThreadPID);
      pthread_cancel(inboundThread);
      pthread_cancel(pthread_self());
      // pthread_exit(NULL);
      // pthread_join(pthread_self(),NULL);
      break;
    }
  }
  return NULL;
}

// UDP Output Thread
void *sendToThread(void *data)
{
  struct threadData *sendData = (struct threadData *)data;

  while (1)
  {
    char message[MSG_MAX_LEN];

    pthread_mutex_lock(&okToSendMutex);

    while (List_count(sendData->givenList) == 0 && !exitProgram)
    {
      printf("Waiting for messages \n");
      pthread_cond_wait(&okToSendCondVar, &okToSendMutex);
    }

    void *messageToSend = List_curr(sendData->givenList);
    if (messageToSend != NULL && !exitProgram)
    {
      strcpy(message, (char *)messageToSend);
      int send_len = sendto(sendData->socket, message, strlen(message), 0, sendData->servinfo->ai_addr, sendData->servinfo->ai_addrlen);
      if (send_len == -1)
      {
        perror("sendto error");
      }
      else
      {
        printf("Sent %d bytes\n\n", send_len);
      }
      List_remove(sendData->givenList);
    }

    pthread_mutex_unlock(&okToSendMutex);

    if(exitProgram){
      printf("Exiting send to thread\n");
      pthread_cancel(pthread_self());
      break;
    }
  }

  return NULL;
}

// UDP Input Thread
void *receiveThread(void *data)
{
  struct threadData *receiveData = (struct threadData *)data;

  // Receive data
  struct sockaddr_storage servAddr;
  while (1 && !exitProgram)
  {
    char serverBuffer[MSG_MAX_LEN];
    socklen_t servAddrLen = sizeof servAddr;

    int recv_len = recvfrom(receiveData->socket, serverBuffer, sizeof(serverBuffer), 0, (struct sockaddr *)&servAddr, &servAddrLen);
    if (recv_len == -1)
    {
      perror("recvfrom error");
    }
    else
    {
      printf("Receive Success\n");
    }
    pthread_mutex_lock(&okToRecvMutex);

    serverBuffer[recv_len] = '\0'; // Null-terminate the received data

    int addStatus = List_append(receiveData->givenList, serverBuffer);
    if (addStatus == -1 && !exitProgram)
    {
      printf("Message has not been recieved!\n");
    }
    else
    {
      printf("Message recieved successfully\n");
    }

    pthread_cond_signal(&okToRecvCondVar);
    pthread_mutex_unlock(&okToRecvMutex);

    if(exitProgram){
      printf("Exiting recv thread \n");
    }

  }

  return NULL;
}

// Screen Output Thread
void *outputMessageThread(void *data)
{
  // TODO
  struct ListIo *inBoundList = (struct ListIo *)data;

  while (1)
  {
    char serverBuffer[MSG_MAX_LEN];

    pthread_mutex_lock(&okToRecvMutex);

    while (List_count(inBoundList->givenList) == 0 && !exitProgram)
    {
      pthread_cond_wait(&okToRecvCondVar, &okToRecvMutex);
    }

    void *messageToDisplay = List_curr(inBoundList->givenList);
    if (messageToDisplay != NULL && !exitProgram)
    {
      strcpy(serverBuffer, (char *)messageToDisplay);
      printf("Message: %s\n\n", serverBuffer);
    }
    List_remove(inBoundList->givenList);
    pthread_mutex_unlock(&okToRecvMutex);

    if(exitProgram){
      printf("Exiting output thread\n");
    }
  
  }
  return NULL;
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
  else
  {
    printf("Server Socket Creation Error\n");
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
  // pthread_t receiveThreadPID;
  // pthread_t sendThreadPID;
  struct ListIo threadSendData;
  struct threadData threadReceiveData;
  struct threadData threadSendToClient;
  struct ListIo threadDisplayData;

  /*********List and respective threads************/

  // pthread_t inboundThread;
  // pthread_t outboundThread;

  List *outBoundList = List_create();
  List *inBoundList = List_create();

  /************************************************/
  threadSendData.givenList = outBoundList;

  threadReceiveData.servinfo = servinfo;
  threadReceiveData.socket = serverSocket;
  threadReceiveData.givenList = inBoundList;

  threadSendToClient.servinfo = servinfo2;
  threadSendToClient.socket = clientSocket;
  threadSendToClient.givenList = outBoundList;

  threadDisplayData.givenList = inBoundList;

  int oldstate = PTHREAD_CANCEL_DEFERRED;

  printf("\nStart Messaging:\n");
  pthread_create(&outboundThread, NULL, sendToThread, (void *)&threadSendToClient);
  pthread_create(&sendThreadPID, NULL, getUserInputThread, (void *)&threadSendData);
  pthread_create(&receiveThreadPID, NULL, receiveThread, (void *)&threadReceiveData);
  pthread_create(&inboundThread, NULL, outputMessageThread, (void *)&threadDisplayData);

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate);

  pthread_join(sendThreadPID, NULL);
  pthread_join(receiveThreadPID, NULL);
  pthread_join(inboundThread, NULL);
  pthread_join(outboundThread, NULL);

  close(clientSocket);
  freeaddrinfo(servinfo2);
  close(serverSocket);
  freeaddrinfo(servinfo);

  pthread_mutex_destroy(&okToRecvMutex);
  pthread_cond_destroy(&okToRecvCondVar);

  pthread_mutex_destroy(&okToSendMutex);
  pthread_cond_destroy(&okToSendCondVar);
}
