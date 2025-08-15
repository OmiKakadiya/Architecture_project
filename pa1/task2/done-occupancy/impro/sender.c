#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "utils.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "../task1/cacheutils.h"

#define MESSAGE_SIZE 200 // 8 bits (1 byte)
#define CACHE_LINE_SIZE 64
#define FILESIZE 1024 * 10
#define FILE_NAME "./invisibleink"
#define THRESHOLD 295
#define ARRAY_SIZE 3 * 1024 * 1024

// RAJ

void secret_protocol(char *message, int message_len)
{
    int fd = open(FILE_NAME, O_RDWR | O_CREAT, 0666);
    if (fd == -1)
    {
        perror("open failed");
        exit(1);
    }

    // Allocate shared memory (only 1 int is needed)
    ftruncate(fd, FILESIZE);
    char *shared_mem = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_mem == MAP_FAILED)
    {
        perror("mmap failed");
        exit(1);
    }
    printf("1 message size: %d\n", message_len);

    unsigned char buff[16];
    int j = 0;
    int counter = 0;
    int *arr = malloc(ARRAY_SIZE * sizeof(int));
    for (size_t i = 0; i < ARRAY_SIZE; i += 16)
    {
        arr[i] = i + 16;
    }

    for (size_t i = 0; i < 256; i += 16)
    {
        printf("arr[%zu] = %d\n", i, arr[i]);
    }
    // int *arr1 = malloc(ARRAY_SIZE * sizeof(int));
    // for (size_t i = 0; i < ARRAY_SIZE; i += 16)
    // {
    //     arr1[i] = i + 16;
    // }

    // for (size_t i = 0; i < 256; i += 16)
    // {
    //     printf("arr[%zu] = %d\n", i, arr1[i]);
    // }

    printf("here\n");
    printf("here\n");
    printf("counter: %d\n", counter);
    // while (j <= 40)
    // {
    //     volatile char value;
    //     value = *(shared_mem + 129);
    //     j++;
    //     value = *(shared_mem + 65);
    //     sched_yield();
    // }

    printf("counter: %d\n", counter);
    unsigned long long target_cycles = 200000000;
    int temp;
    printf("time: %lld\n", rdtsc());
    for (int i = 0; i < message_len * 8; ++i)
    // for (;;)
    {
        unsigned long long start = rdtsc();
        int j = 0;
        // size_t start_time = rdtsc();
        while ((rdtsc() - start) < target_cycles)
        {

            for (int k = 0; k < ARRAY_SIZE; k += 512)
            {
                temp = arr[k];
            }
            // if (message[i / 8] & (1 << (i % 8)))
            // {
            //     for (int k = 0; k < ARRAY_SIZE; k += 32)
            //     {
            //         temp = arr[k];
            //     }
            // }
            // else
            // {
            //     for (int k = 0; k < ARRAY_SIZE; k += 512)
            //     {
            //         temp = arr[k];
            //     }
            // }
        }
        if (i > 0 && i%8 == 0)
            usleep(1000);
        printf("index: i: %d, time: %lld\n", i, rdtsc());
        // printf("going to sleep\n");
        // sleep(5);
        // printf("woke up\n");
        // size_t end_time = rdtsc();
    }

    close(fd);
}

int main()
{

    // ********** DO NOT MODIFY THIS SECTION **********
    FILE *fp = fopen(MSG_FILE, "r");
    if (fp == NULL)
    {
        printf("Error opening file\n");
        return 1;
    }

    char msg[MAX_MSG_SIZE];
    int msg_size = 0;
    char c;
    while ((c = fgetc(fp)) != EOF)
    {
        msg[msg_size++] = c;
    }
    fclose(fp);

    clock_t start = clock();
    // **********************************************
    // ********** YOUR CODE STARTS HERE **********

    secret_protocol(msg, msg_size);

    // ********** YOUR CODE ENDS HERE **********
    // ********** DO NOT MODIFY THIS SECTION **********
    clock_t end = clock();
    double time_taken = ((double)end - start) / CLOCKS_PER_SEC;
    printf("Message sent successfully\n");
    printf("Time taken to send the message: %f\n", time_taken);
    printf("Message size: %d\n", msg_size);
    printf("Bits per second: %f\n", msg_size * 8 / time_taken);
    // **********************************************
}
