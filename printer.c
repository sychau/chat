#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "list.h"
#include "printer.h"

static pthread_t s_printerThread;
static pthread_mutex_t s_recvListMutex;
static pthread_cond_t s_recvListNotEmptyCondVar = PTHREAD_COND_INITIALIZER;

static List *s_recvList = NULL;

void* printerRoutine(void* unused)
{
    while(1) {
        char *msg = NULL;
        pthread_mutex_lock(&s_recvListMutex);
        {   
            // If recvList is empty, sleep until new message received
            while (List_count(s_recvList) == 0) {
                pthread_cond_wait(&s_recvListNotEmptyCondVar, &s_recvListMutex);
            }
            // Now recvList has at least one item
            List_first(s_recvList);
            msg = List_remove(s_recvList);
        }
        pthread_mutex_unlock(&s_recvListMutex);

        assert(msg != NULL && "s_recvList should not be empty");

        // Printer message produced by receiver
        if (strcmp(msg, "!") != 0){
            printf(">other: %s\n", msg);
        }

        // Free message node from list
        free(msg);
    }
    return NULL;
}

void Printer_init(List *recvList, pthread_mutex_t recvListMutex)
{  
    // Assign static variables
    s_recvList = recvList;
    s_recvListMutex = recvListMutex;

    // Create thread and entry at printerRoutine
    pthread_create(&s_printerThread, NULL, printerRoutine, NULL);
}

/* Signal the recvList is not empty
   The printer thread will sleep if there is nothing to print
   Receiver thread will call this function to awake printer thread */
void Printer_signalListNotEmpty()
{
    pthread_mutex_lock(&s_recvListMutex);
    {
        pthread_cond_signal(&s_recvListNotEmptyCondVar);
    }
    pthread_mutex_unlock(&s_recvListMutex);
}

void Printer_shutdown()
{
    // Shutdown the thread
    pthread_cancel(s_printerThread);

    // Join main thread
    pthread_join(s_printerThread, NULL);
}