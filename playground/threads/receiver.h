#ifndef _RECEIVER_H_
#define _RECEIVER_H_

// Start background receive thread
void Receiver_init(char* rxMessage);

// Set the dynamic message
void Receiver_changeDynamicMessage(char* rxMessage);

// Stop background receive thread and cleanup
void Receiver_shutdown(void);

#endif