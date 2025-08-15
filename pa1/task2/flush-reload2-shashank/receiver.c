#include <stdio.h>
#include "utils.h"
#include "../../task1/cacheutils.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>

#define MESSAGE_SIZE 169 
#define THRESHOLD 295
#define CHANNEL_SYNC_TIMEMASK           0x000FFFFF
#define CHANNEL_SYNC_JITTER             0x0100
#define CHANNEL_DEFAULT_INTERVAL        1000000
#define OFFSET_SYNC_1  100
#define OFFSET_SYNC_2  200
#define OFFSET_DATA    300
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

int receive_bit(char* shared_mem) {
    
    int misses = 0;
	int hits = 0;
    
    // size_t time=cc_sync();
    size_t start_time=rdtsc();
    while ((rdtsc() - start_time) < CHANNEL_DEFAULT_INTERVAL) {
		size_t s_time = rdtsc();  
        maccess((void*)shared_mem);
        size_t e_time = rdtsc(); 
        size_t delta = e_time - s_time;
        // printf("delta: %ld\n",delta);

		// Count if it's a miss or hit depending on latency
		if (delta > THRESHOLD) {
			misses++;
		} else {
			hits++;
		}
        flush((void*)shared_mem);
        // sched_yield();
	}
    // printf("miss: %d\n",misses);
    // printf("hit: %d\n",hits);

	return (hits>=misses) ? 1 : 0;
}

void perform_handshake(char* shared_mem) {
    char* sync_addr_1 = shared_mem + OFFSET_SYNC_1;
    char* sync_addr_2 = shared_mem + OFFSET_SYNC_2;
    // flush((void*)sync_addr_1);
    // // sched_yield();
    // int counter=0;
    // printf("Receiver: Waiting for sender...\n");
    // while (1) {
    //     if (receive_bit(sync_addr_1)) {
    //         counter++;
    //         if (counter > 100) break;
    //     }
    // }

    // printf("Receiver: Sender detected! Sending acknowledgment...\n");
    // size_t start_time = rdtsc();
    // while ((rdtsc() - start_time) < CHANNEL_DEFAULT_INTERVAL * 10) {
    //     maccess((void*)sync_addr_2);
    // }
    // printf("Receiver: Handshake complete. Receiving data...\n");
    int sync_counter = 0;
    while (sync_counter <= 100)
    {
        size_t start_time = rdtsc();
        volatile char value;
        value = *(sync_addr_1);
        size_t end_time = rdtsc();
        
        if (end_time - start_time < THRESHOLD)
        {
        sync_counter++;
        }
        flush((sync_addr_1));

        sched_yield();
    }

}

void receive_message(char* received_msg, char* shared_mem, int* received_msg_size) {
    
    char* data_addr = shared_mem + OFFSET_DATA;
    usleep(50);
    printf("time: %lld\n", rdtsc());
    for (int i = 0; i < 168; i++) {  
        char byte = 0;
        flush((void*)shared_mem);
        // printf("Byte: %d ", i+1);
        for (int j = 0; j < 8; j++) {
            int bit = receive_bit(data_addr);
            // printf("%d", bit);
            if (j % 8 == 7){
                // printf("  ");
            }
            byte |= (bit << j);
            // usleep(1000);
        }
        // printf("\n");
        received_msg[(*received_msg_size)++] = byte;
    }
}

int main(){
    char* received_msg = (char *)malloc(MESSAGE_SIZE);
    if (received_msg == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    memset(received_msg, 0, MESSAGE_SIZE);
    int received_msg_size = 0;

    char* shared_mem = initialize_shared_memory();

    perform_handshake(shared_mem);

    receive_message(received_msg, shared_mem, &received_msg_size);

    printf("Received Message: ");
    for (int i = 0; i < received_msg_size; i++) {
        printf("%c", received_msg[i]);
    }
    printf("\n");
    printf("Accuracy (%%): %f\n", check_accuracy(received_msg, received_msg_size) * 100);
}
