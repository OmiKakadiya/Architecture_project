#include <stdio.h>
#include "utils.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


#define MESSAGE_SIZE 168 // 8 bits (1 byte)
#define CACHE_LINE_SIZE 64
#define THRESHOLD 295
#define FILESIZE 1024 * 10
#define FILE_NAME "./invisibleink"
#define ARRAY_SIZE 3 * 1024 * 1024

// SIMRAN
void receive_bit_by_bit(char *received_msg, int message_len)
{
    int fd = open(FILE_NAME, O_RDWR | O_CREAT, 0666);
    if (fd == -1)
    {
        perror("open failed");
        exit(1);
    }
    ftruncate(fd, FILESIZE);
    char *shared_mem = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_mem == MAP_FAILED)
    {
        perror("mmap failed");
        exit(1);
    }
    int j = 0;
    int counter = 0;
    int temp;
    int hits = 0, misses = 0;
    int *arr = malloc(ARRAY_SIZE * sizeof(int));
    for (size_t i = 0; i < ARRAY_SIZE; i += 16)
    {                    
        arr[i] = i + 16; 
    }

    for (size_t i = 0; i < 256; i += 16)
    {
        printf("arr[%zu] = %d\n", i, arr[i]);
    }

    printf("counter: %d\n", counter);
    unsigned long long target_cycles = 500000000;

    printf("time: %lld\n", rdtsc());
    int i = 0;
    // for (int i = 0; i < message_len * 8; ++i)
    int false_positive = 0;
    for (;;)
    {
        unsigned long long start = rdtsc();

        j = 0;
        counter = 0;
        
        do
        {
            counter++;

            // int temp = buff[j];

            for (int k = 0; k < ARRAY_SIZE; k += 16)
            {                    
                temp = arr[k]; 
            }
        } while ((rdtsc() - start) < target_cycles);
        printf("counter: %d\n", counter);
        unsigned long long start1 = rdtsc();
        int bit_position = i % 8;
        int byte_index = i / 8;
        if (counter < 235) {
            if (counter < 190) {
                received_msg[byte_index] |= (1 << bit_position);
            }
            i++;
        } else {
            if (i % 8 != 0) {
                i=0;
            }
        }
        if (i%8 == 0)
            printf("-------------received: %c\n", received_msg[byte_index]);
        unsigned long long end1 = rdtsc();
        printf("del: %lld\n", end1-start1);
        // printf("index: i: %d, time: %lld\n", i, rdtsc());
    }
    close(fd);
}

int main()
{

    // Update these values accordingly
    char *received_msg = NULL;
    int received_msg_size = 0;

    received_msg = (char *)malloc(MESSAGE_SIZE + 1);
    memset(received_msg, 0, MESSAGE_SIZE + 1);

    char *received_msg_temp = (char *)malloc(MESSAGE_SIZE + 1);
    memset(received_msg_temp, 0, MESSAGE_SIZE + 1);

    receive_bit_by_bit(received_msg_temp, MESSAGE_SIZE);

    printf("Received message: \n");
    for (int i = 0; i < MESSAGE_SIZE + 1; i++)
    {
        printf("%c", received_msg_temp[i]);
        if (received_msg_temp[i] != '\0')
        {
            received_msg[i] = received_msg_temp[i];
        }
        else
        {
            received_msg[i] = '_';
        }
    }
    printf("Received message: %s\n", received_msg);

    // DO NOT MODIFY THIS LINE
    printf("Accuracy (%%): %f\n", check_accuracy(received_msg, strlen(received_msg)) * 100);
}
