#ifndef _SENDER_H_
#define _SENDER_H_

#include "list.h"

#define MSG_MAX_LEN 512

// Start background receive thread
void Sender_init(List* sendList, pthread_mutex_t sendListMutex ,const char* theirPort, const char *theirNodename);

// Should be called by producer (Keyboard)
void Sender_signalListNotEmpty();

// Stop background receive thread and cleanup
void Sender_shutdown(void);

#endif