#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "timings.h"
#include "printer.h"
#include "signaller.h"

// return true if it computed the average OK
// average written into pAvgOut
static bool getAverage(double *pData, int size, double *pAvgOut)
{
    if (size == 0) {
        return false;
    }
    double sum = 0;
    for (int i = 0; i < size; i++){
        sum += *(pData + i);
    }
    // Store value into output parameter
    *pAvgOut = sum/size;
    return true;
}

int main(int argc, char** args)
{
    // Synchronization
    
    double myData[] = {1.0, 2.0};
    double myAverage = 0;
    if (getAverage(myData, 2, &myAverage)){
        printf("Average us : %f\n", myAverage);
    } else {
        printf("Could not compute average\n");
    }

    Printer_init();
    Signaller_init();

    // Cleanup
    Printer_waitForShutdown();
    Signaller_waitForShutdown();

    printf("\nDONE\n");
    
    return 0;
}