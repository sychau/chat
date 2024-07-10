#include <stdio.h>
#include <stdlib.h>
#include "receiver.h"

int main(int argc, char** args)
{
    // Start modules
    Receiver_init("A voice can be heard in the distance");   

    // Wait for user input
    printf("Enter something to kill the receive thread\n");
    char x;
    scanf("%c", &x);

    // Shutdown modules
    Receiver_shutdown();

    printf("Done\n");

    return 0;
}