#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "cacheutils.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MESSAGE_SIZE 200 
#define PORT 8000
#define THRESHOLD 275     // Timing threshold

int main(){
    
    char* received_msg = (char *)malloc(MESSAGE_SIZE);
    int received_msg_size = 0;

    int sock;
    struct sockaddr_in serv_addr;
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        perror("Socket creation error"); 
        exit(0); 
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0)
    {
        perror("Connect failed");
        exit(0);
    }

    int shm_fd = shm_open("flush_reload_shm", O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) { 
        perror("shm_open failed"); 
        exit(0); 
    }
   
    char *shared_mem;
    shared_mem = mmap(NULL, sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) { 
        perror("mmap failed"); 
        exit(0); 
    }

    while (1) {
        char byte = 0;
        for (int j = 0; j < 8; j++) {
            char sync_signal = 'S';
            send(sock, &sync_signal, sizeof(sync_signal), 0);  // Tell sender to send bit
            
            char ack;
            recv(sock, &ack, sizeof(ack), 0);  // Wait for sender’s encryption complete signal

            size_t time = rdtsc();  
            maccess((void*)shared_mem);  // Access shared memory location
            size_t delta = rdtsc() - time;  

            int bit = (delta < THRESHOLD) ? 1 : 0;  // If access is fast, it's a '1'
            byte |= (bit << j);

            send(sock, &sync_signal, sizeof(sync_signal), 0);  // Notify sender that decryption is complete
        }

        if (byte == '\0') break;  // Stop if null terminator is received
        received_msg[received_msg_size++] = byte;
    }

    close(sock);
    printf("Accuracy (%%): %f\n", check_accuracy(received_msg, received_msg_size) * 100);
    free(received_msg);
}
