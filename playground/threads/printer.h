#ifndef _PRINTER_H_
#define _PRINTER_H

#include <pthread.h>

void Printer_init();
void Printer_signalNextChar();
void Printer_waitForShutdown();

#endif