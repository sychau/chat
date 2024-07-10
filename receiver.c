#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#include "receiver.h"
#include "printer.h"
#include "list.h"

typedef struct addrinfo addrinfo;
typedef struct sockaddr sockaddr;
typedef struct sockaddr_storage sockaddr_storage;

static int s_mySockfd;
static addrinfo *s_myAddrInfo;

static pthread_mutex_t s_recvListMutex;
static pthread_t s_receiverThread;
extern pthread_t g_receiverThread; /* Keyboard thread need this external global variable to shutdown the program */

static char* s_recvMsg = NULL;
static List* s_recvList = NULL;

static bool s_IQuit = true;

void* receiverRoutine(void* unused) 
{
    sockaddr_storage unusedRecvAddr; // unused addr, don't care where the message was send from because we already knew
    socklen_t unusedRecvAddrLen; // unused addr length

    // Initialize these to suppresss valgrind warning
    memset (&unusedRecvAddr, 0, sizeof unusedRecvAddr);
    memset (&unusedRecvAddrLen, 0, sizeof unusedRecvAddrLen);

    while(1) {
        // Listen for message
        s_recvMsg = malloc(sizeof(char) * MSG_MAX_LEN);
        int bytesRecv = recvfrom(s_mySockfd, s_recvMsg, MSG_MAX_LEN - 1, 0, // block thread
                        (sockaddr *) &unusedRecvAddr, &unusedRecvAddrLen);
                 
        if (bytesRecv == -1) {
            perror("recvfrom");
            exit(1);
        }

        // Null terminate received message
        int terminateIdx = (bytesRecv == MSG_MAX_LEN) ? MSG_MAX_LEN - 1 : bytesRecv;
        s_recvMsg[terminateIdx] = '\0';

        // Append to recvList and awake printer thread if it is sleeping
        pthread_mutex_lock(&s_recvListMutex);
        {
            List_append(s_recvList, s_recvMsg);
            if (List_count(s_recvList) == 1) {
                Printer_signalListNotEmpty(); // awake printer thread to print message
            }
        }
        pthread_mutex_unlock(&s_recvListMutex);

        // Check if remote machine send me a shutdown signal
        if(strcmp(s_recvMsg, "!") == 0) {
            printf("Other user has ended the program...\n");
            s_IQuit = false;
            pthread_cancel(s_receiverThread); // trigger shutdown in main thread
            break;
        }
    }
    return NULL;
}

void Receiver_init(List* recvList, pthread_mutex_t recvListMutex, const char* myPort)
{
    // Assign static variables
    s_recvList = recvList;
    s_recvListMutex = recvListMutex;
    
    // Get my address info
    addrinfo myHints;
    memset(&myHints, 0, sizeof myHints);
    myHints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    myHints.ai_socktype = SOCK_DGRAM; // UDP
    myHints.ai_flags = AI_PASSIVE; // use my IP

    addrinfo *myAddrInfoHead;
    int myRv = getaddrinfo(NULL, myPort, &myHints, &myAddrInfoHead);
    if (myRv != 0) {
        fprintf(stderr, "myAddrInfo getaddrinfo: %s\n", gai_strerror(myRv));
    }

    // Create socket and bind the socket to first valid address
    for(s_myAddrInfo = myAddrInfoHead; s_myAddrInfo != NULL; s_myAddrInfo = s_myAddrInfo->ai_next) {
        if ((s_mySockfd = socket(s_myAddrInfo->ai_family, s_myAddrInfo->ai_socktype, s_myAddrInfo->ai_protocol)) == -1) {
            perror("myAddrInfo: failed to return socket file descriptor");
            continue;
        }
        if (bind(s_mySockfd, s_myAddrInfo->ai_addr, s_myAddrInfo->ai_addrlen) == -1) {
            close(s_mySockfd);
            perror("myAddrInfo: failed to bind socket");
            continue;
        }
        break;
    }

    if (s_myAddrInfo == NULL) {
        fprintf(stderr, "myAddrInfo: failed to bind socket\n");
        exit(1);
    }

    // Create thread and entry at receiverRoutine
    pthread_create(&s_receiverThread, NULL, receiverRoutine, NULL);
    g_receiverThread = s_receiverThread; /* Allow Keyboard to shutdown receiver thread */
}

void Receiver_waitForShutdown()
{   
    /* Receiver or Keyboard will cancel receiver */

    // Waits for thread to finish
    pthread_join(s_receiverThread, NULL);

    // Cleanup memory, socket and data structure
    pthread_mutex_lock(&s_recvListMutex);
    {
        // free s_recvMsg only if I quit
        if (s_IQuit && s_recvMsg) { 
            free(s_recvMsg);
        }
        s_recvMsg = NULL;
    }
    pthread_mutex_unlock(&s_recvListMutex);
    close(s_mySockfd);
    freeaddrinfo(s_myAddrInfo);
}