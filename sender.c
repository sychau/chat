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
#include <assert.h>

#include "receiver.h"
#include "list.h"

typedef struct addrinfo addrinfo;
typedef struct sockaddr sockaddr;
typedef struct sockaddr_storage sockaddr_storage;

static int s_theirSockfd;
static addrinfo *s_theirAddrInfo;

static pthread_mutex_t s_sendListMutex;
static pthread_t s_senderThread;
static pthread_cond_t s_sendListNotEmptyCondVar = PTHREAD_COND_INITIALIZER;

List* s_sendList;

void* senderRoutine(void* unused) 
{
    while(1) {
        char *msg = NULL;
        pthread_mutex_lock(&s_sendListMutex);
        {
            // If sendList is empty, sleep until new message received
            while (List_count(s_sendList) == 0) {
                pthread_cond_wait(&s_sendListNotEmptyCondVar, &s_sendListMutex);
            }
            // Now sendList has at least one item
            List_first(s_sendList);
            msg = List_remove(s_sendList);
        }
        pthread_mutex_unlock(&s_sendListMutex);

        assert(msg != NULL && "s_sendList should not be empty");

        // Send message to remote machine
        int bytesSent = sendto(s_theirSockfd, msg, strlen(msg) + 1, 0,
            s_theirAddrInfo->ai_addr, s_theirAddrInfo->ai_addrlen);

        if (bytesSent == -1) {
            perror("sendto");
            exit(1);
        }

        // Free message node from sendList
        free(msg);
    }
    return NULL;
}

void Sender_init(List* sendList, pthread_mutex_t sendListMutex ,const char* theirPort, const char *theirNodename)
{
    // Assign static variables
    s_sendList = sendList;
    s_sendListMutex = sendListMutex;

    // Get their addressi info 
    addrinfo theirHints;
    memset(&theirHints, 0, sizeof theirHints);
    theirHints.ai_family = AF_UNSPEC;
    theirHints.ai_socktype = SOCK_DGRAM;

    addrinfo *theirAddrInfoHead;
    int theirRv = getaddrinfo(theirNodename, theirPort, &theirHints, &theirAddrInfoHead);
    if (theirRv != 0) {
        fprintf(stderr, "theirAddrInfo getaddrinfo: %s\n", gai_strerror(theirRv));
    }

    // Create socket 
    for(s_theirAddrInfo = theirAddrInfoHead; s_theirAddrInfo != NULL; s_theirAddrInfo = s_theirAddrInfo->ai_next) {
        if ((s_theirSockfd = socket(s_theirAddrInfo->ai_family, s_theirAddrInfo->ai_socktype, s_theirAddrInfo->ai_protocol)) == -1) {
            perror("theirAddrInfo: failed to return socket file descriptor");
            continue;
        }
        // No binding needed for theirSockfd
        break;
    }

    if (s_theirAddrInfo == NULL) {
        fprintf(stderr, "theirAddrInfo: failed to create socket\n");
        exit(1);
    }

    // Create thread and entry at senderRoutine
    pthread_create(&s_senderThread, NULL, senderRoutine, NULL);
}

/* Signal the sendList is not empty
   The sender thread will sleep if there is nothing to send
   Keyboard thread will call this function to awake sender thread */
void Sender_signalListNotEmpty() {
    pthread_mutex_lock(&s_sendListMutex);
    {
        pthread_cond_signal(&s_sendListNotEmptyCondVar);
    }
    pthread_mutex_unlock(&s_sendListMutex);
}

void Sender_shutdown(void)
{
    // Shutdown the thread
    pthread_cancel(s_senderThread);

    // Join the main thread
    pthread_join(s_senderThread, NULL);

    // Clean up for socket and data structure
    close(s_theirSockfd);
    freeaddrinfo(s_theirAddrInfo);
}
