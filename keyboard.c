#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "list.h"
#include "printer.h"
#include "sender.h"

static pthread_t s_keyboardThread;
pthread_t g_receiverThread; /* Link to global receiver thread from receiver */
static pthread_mutex_t s_sendListMutex;

static char *s_msg = NULL;
static List *s_sendList;

static bool s_IQuit = false;

void* keyboardRoutine(void* unused)
{
    while(1) {
        s_msg = malloc(MSG_MAX_LEN); // sender should be responsible to free this
        fgets(s_msg, MSG_MAX_LEN, stdin); // block thread
        s_msg[strcspn(s_msg, "\n")] = 0;

        // Append to sendList and awake sender thread if it is sleeping
        pthread_mutex_lock(&s_sendListMutex);
        {
            List_append(s_sendList, s_msg);
            if (List_count(s_sendList) == 1) {
                Sender_signalListNotEmpty();
            }
        }
        pthread_mutex_unlock(&s_sendListMutex);

        // Check if user want to terminate program
        if(strcmp(s_msg, "!") == 0) {
            printf("Now ending the program...\n");
            s_IQuit = true;
            pthread_cancel(g_receiverThread); // trigger the program to shutdown
            break;
        }
    }
    return NULL;
}

void Keyboard_init(List *sendList, pthread_mutex_t sendListMutex)
{  
    // Assign static variables
    s_sendList = sendList;
    s_sendListMutex = sendListMutex;

    // Create thread and entry at keyboardRoutine
    pthread_create(&s_keyboardThread, NULL, keyboardRoutine, NULL);
}

void Keyboard_shutdown()
{
    // Shutdown the thread
    pthread_cancel(s_keyboardThread);

    // Join the main thread
    pthread_join(s_keyboardThread, NULL);

    // Clean up memory
    pthread_mutex_lock(&s_sendListMutex);
    {
        // free s_recvMsg only if they quit
        if (!s_IQuit && s_msg) {
            free(s_msg);
        }
        s_msg = NULL;
    }
    pthread_mutex_unlock(&s_sendListMutex);
}