#ifndef _PRINTER_H_
#define _PRINTER_H

#include <pthread.h>
#include "list.h"

void Printer_init(List *recvList, pthread_mutex_t recvListMutex);
void Printer_signalListNotEmpty();
void Printer_shutdown();

#endif