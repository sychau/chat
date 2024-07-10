#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <pthread.h>
#include "list.h"

void Keyboard_init(List *sendList, pthread_mutex_t sendListMutex);
void Keyboard_shutdown();

#endif