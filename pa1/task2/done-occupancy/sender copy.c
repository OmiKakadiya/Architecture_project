#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "utils.h"
#include "../../task1/cacheutils.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>

// #define PREAMBLE "10101011"  // Synchronization pattern
#define CHANNEL_DEFAULT_INTERVAL 500000000
#define OCCUPANCY_ARRAY_SIZE 10 * 1024 * 1024 

void send_bit(int bit, int* arr) {
    
    size_t start_time=rdtsc();
    int access;
    // bit=0;
    if (bit) {
        // size_t s_time = rdtsc(); 
        // printf("rdtsc: %ld\n",s_time);
        while ((rdtsc() - start_time) < CHANNEL_DEFAULT_INTERVAL) {
            for (size_t i = 0; i < OCCUPANCY_ARRAY_SIZE; i += 200)
                {
                    access = arr[i];
                }
		}	
        printf("1");
    } else {
        while ((rdtsc() - start_time) < CHANNEL_DEFAULT_INTERVAL) { 
            for (size_t i = 0; i < OCCUPANCY_ARRAY_SIZE; i += 400)
                {
                    access = arr[i];
                }
        }
        printf("0");
    }
    // usleep(1000);
    // sched_yield();
}

// void perform_handshake(int* arr) {
//     printf("Sending preamble...\n");
//     for (int i = 0; i < 8; i++) {
//         send_bit(PREAMBLE[i] - '0', arr);
//     }
//     printf("Preamble sent!\n");
// }

void send_message(const char* msg, int msg_size) { 
    int *array = malloc(OCCUPANCY_ARRAY_SIZE * sizeof(int));
    for (size_t i = 0; i < OCCUPANCY_ARRAY_SIZE; i += 100)
    {
        array[i] = (i* 6) + 45; 
    }
    // perform_handshake(array);
    // printf("time: %lld\n", rdtsc());
    for (int i = 0; i <= msg_size; i++) {
        printf("Byte: %d  Bits:", i+1);
        for (int j = 0; j < 8; j++) {
            send_bit((msg[i] >> j) & 1,array);
        }
        printf("\n");
    }
}

int main(){
    FILE *fp = fopen(MSG_FILE, "r");
    if(fp == NULL){
        printf("Error opening file\n");
        return 1;
    }

    char msg[MAX_MSG_SIZE];
    int msg_size = 0;
    char c;
    while((c = fgetc(fp)) != EOF){
        msg[msg_size++] = c;
    }
    msg[msg_size] = '\0';
    fclose(fp);
    clock_t start = clock();
    send_message(msg, msg_size);
    clock_t end = clock();
    double time_taken = ((double)end - start) / CLOCKS_PER_SEC;
    printf("Message sent successfully\n");
    printf("Time taken to send the message: %f\n", time_taken);
    printf("Message size: %d\n", msg_size);
    printf("Bits per second: %f\n", msg_size * 8 / time_taken);
}