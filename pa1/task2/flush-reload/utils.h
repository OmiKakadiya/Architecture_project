#include <stdio.h>

#define MSG_FILE "msg.txt"
#define MAX_MSG_SIZE 500
#define CHANNEL_DEFAULT_INTERVAL        0x00008000
#define CHANNEL_SYNC_TIMEMASK           0x000FFFFF
#define CHANNEL_SYNC_JITTER             0x0100

double check_accuracy(char*, int);