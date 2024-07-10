/*
CMPT300 Assignment 2 2024-02-23
CMPT 300 X1 Groups
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
Henry Chau, syc54
Ryan Taban, rat2
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

#include "list.h"
#include "receiver.h"
#include "sender.h"
#include "printer.h"
#include "keyboard.h"

#define SOCKETERROR (-1)
#define MAXBUFLEN 1024

typedef struct addrinfo addrinfo;
typedef struct sockaddr sockaddr;
typedef struct sockaddr_storage sockaddr_storage;

// A helper function used by List_free to free string
void freeString (void* pString) {
    free(pString);
}

// Program entry point
int main (int argc, char *argv[]) {
    // Handle input arguments
    if (argc != 4) {
        perror("Usage: ./s-talk [my port number] [remote machine name] [remote port number]");
        exit(1);
    }
    const char *myPort = argv[1];
    const char *theirNodename = argv[2];
    const char *theirPort = argv[3];

    // Init necessary variables
    List* sendList = List_create();
    List* recvList = List_create();
    pthread_mutex_t recvListMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t sendListMutex = PTHREAD_MUTEX_INITIALIZER;

    printf("You are now chatting with %s on port %s, enter \"!\" to quit...\n", theirNodename, theirPort);

    // Start multithreading
    Receiver_init(recvList, recvListMutex, myPort); // receiver is producer of recvList
    Printer_init(recvList, recvListMutex); // printer is consumer of recvList
    Sender_init(sendList, sendListMutex, theirPort, theirNodename); // sender is consumer of sendList
    Keyboard_init(sendList, sendListMutex); // keyboard is producer of sendList

    // Main thread will wait for receiver thread to join then continue shutdown other thread
    Receiver_waitForShutdown();
    Printer_shutdown();
    Sender_shutdown();
    Keyboard_shutdown();

    // Clean up list
    List_free(sendList, freeString);
    List_free(recvList, freeString);
}