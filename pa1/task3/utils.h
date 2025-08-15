#include <stdio.h>

#define LETTER_SIZE 168
#define IMAGE_SIZE 32287
#define MESSAGE_SIZE IMAGE_SIZE 

#define MSG_FILE "../task3/red_heart.jpg"
#define MAX_MSG_SIZE MESSAGE_SIZE+100

#define CACHE_LINE_SIZE 64
#define FILESIZE 1024 * 10
#define FILE_NAME "./invisibleink"
#define THRESHOLD 295


double check_accuracy(char*, int);