#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>

#include "receiver.h"
#include "timings.h"

#define DYNAMIC_LEN 128
#define MSG_MAX_LEN 1024
#define PORT 22110

static pthread_mutex_t dynamicMsgMutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_t threadPID;
static int socketDescriptor;
static char* s_rxMessage;
static char* dynamicMessage;

void* receiveThread(void* unused) 
{
    // Dynamically allocate a message
    pthread_mutex_lock(&dynamicMsgMutex);
    {
        strcpy(dynamicMessage, "Dynamic!");
    }
    pthread_mutex_unlock(&dynamicMsgMutex);
    
    // Address
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(PORT);

    // Create the socket for UDP
    socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

    // Bind the socekt to the port (PORT) that we specify
    bind(socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));

    while(1) {
        // Get the data (blocking)
        // Will change sin (the address) to be the address of the client
        // Note: sin passes information in and out of call!
        struct sockaddr_in sinRemote;
        unsigned int sin_len = sizeof(sinRemote);
        char messageRx[MSG_MAX_LEN];
        printf("TEST %s", messageRx);
        recvfrom(socketDescriptor,
            messageRx, MSG_MAX_LEN, 0, 
            (struct sockaddr *) &sinRemote, &sin_len);

        // Do something amazing with the received message!
        pthread_mutex_lock(&dynamicMsgMutex);
        {
            printf("%s >> %s: %s", dynamicMessage, s_rxMessage, messageRx);
        }
        pthread_mutex_unlock(&dynamicMsgMutex);

        memset(messageRx, 0, sizeof(messageRx)); // Clear message after use
    }
    return NULL;
}

void Receiver_init(char* rxMessage)
{
    dynamicMessage = malloc(DYNAMIC_LEN);

    s_rxMessage = rxMessage;
    pthread_create(&threadPID, NULL, receiveThread, NULL);
}

void Receiver_changeDynamicMessage(char* newDynamic)
{
    pthread_mutex_lock(&dynamicMsgMutex);
    {
        strncpy(dynamicMessage, newDynamic, DYNAMIC_LEN);
    }
    pthread_mutex_unlock(&dynamicMsgMutex);
}

void Receiver_shutdown(void)
{
    // Cancel thread
    pthread_cancel(threadPID);

    // Waits for thread to finish
    pthread_join(threadPID, NULL);

    // Cleanup memory
    pthread_mutex_lock(&dynamicMsgMutex);
    {
        free(dynamicMessage);
    }
    pthread_mutex_unlock(&dynamicMsgMutex);
}
