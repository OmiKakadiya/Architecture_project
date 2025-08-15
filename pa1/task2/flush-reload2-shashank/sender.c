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


#define CHANNEL_SYNC_TIMEMASK           0x000FFFFF
#define CHANNEL_SYNC_JITTER             0x0100
#define CHANNEL_DEFAULT_INTERVAL        1000000
#define OFFSET_SYNC_1  100
#define OFFSET_SYNC_2  200
#define OFFSET_DATA    300
#define THRESHOLD 295
#define SHMSIZE 1024

// size_t cc_sync() {
//     while((rdtsc() & CHANNEL_SYNC_TIMEMASK) > CHANNEL_SYNC_JITTER) {}
//     return rdtsc();
// }

char* initialize_shared_memory() {
    int shm_fd = open("./flush_reload_shm", O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) { 
        perror("shm_open failed"); 
        exit(1); 
    }
    if (ftruncate(shm_fd, sizeof(char)*SHMSIZE) == -1) { 
        perror("ftruncate failed"); 
        exit(1); 
    }
    char *shared_mem = mmap(NULL, sizeof(char)*SHMSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) { 
        perror("mmap failed"); 
        exit(1); 
    }
    return shared_mem;
}

void send_bit(int bit, char* shared_mem) {
    
    // size_t time=cc_sync();
    size_t start_time=rdtsc();
    if (bit) {
        // size_t s_time = rdtsc(); 
        // printf("rdtsc: %ld\n",s_time);
        while ((rdtsc() - start_time) < CHANNEL_DEFAULT_INTERVAL) {
            maccess((void*)shared_mem);
            // printf("B");
		}	
        // printf("1");
    } else {
        while ((rdtsc() - start_time) < CHANNEL_DEFAULT_INTERVAL) { //printf("A");
        }
        // printf("0");
    }
    
    // usleep(1000);
    // sched_yield();
}

void perform_handshake(char* shared_mem) {
    char* sync_addr_1 = shared_mem + OFFSET_SYNC_1;  //129
    char* sync_addr_2 = shared_mem + OFFSET_SYNC_2; //65

    // // Signal receiver by writing to Address 1
    // printf("Sender: Waiting for receiver...\n");
    // clock_t start_time = clock();
    // while ((clock() - start_time) < CLOCKS_PER_SEC*10) {
    //     maccess((void*)sync_addr_1);
    //     sched_yield();
    // }

    // // Wait for receiver to acknowledge using Address 2
    // printf("Sender: Waiting for receiver acknowledgment...\n");
    // while (1) {
    //     size_t s_time = rdtsc();
    //     maccess((void*)sync_addr_2);
    //     size_t delta = rdtsc() - s_time;

    //     if (delta < THRESHOLD) { 
    //         break;
    //     }
    //     flush((void*)sync_addr_2);
    //     sched_yield();

    // }
    // printf("Sender: Handshake complete. Starting data transmission...\n");
    int sync_counter = 0;
    while (sync_counter <= 200)
    {
        volatile char value;
        value = *(sync_addr_1);
        sync_counter++;
        // value = *(sync_addr_2);
        sched_yield();
    }
}

void send_message(const char* msg, int msg_size, char* shared_mem) { 
    printf("time: %lld\n", rdtsc());
    char* data_addr = shared_mem + OFFSET_DATA;
    for (int i = 0; i < msg_size; i++) {
        // printf("Byte: %d  Bits:", i+1);
        for (int j = 0; j < 8; j++) {
            send_bit((msg[i] >> j) & 1, data_addr);
        }
        // printf("\n");
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
    fclose(fp);
     clock_t start = clock();

    char* shared_mem = initialize_shared_memory();
    perform_handshake(shared_mem);
    send_message(msg, msg_size, shared_mem);
    clock_t end = clock();
    double time_taken = ((double)end - start) / CLOCKS_PER_SEC;
    printf("Message sent successfully\n");
    printf("Time taken to send the message: %f\n", time_taken);
    printf("Message size: %d\n", msg_size);
    printf("Bits per second: %f\n", msg_size * 8 / time_taken);
}
