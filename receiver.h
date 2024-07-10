#ifndef _RECEIVER_H_
#define _RECEIVER_H_

#include "list.h"

#define MSG_MAX_LEN 512

// Start background receive thread
void Receiver_init(List* recvList, pthread_mutex_t recvListMutex, const char* myPort);

// Stop background receive thread and cleanup
void Receiver_waitForShutdown();

#endif