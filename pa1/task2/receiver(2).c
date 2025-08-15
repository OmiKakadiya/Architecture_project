#include <stdio.h>
#include "utils.h"
#include "cacheutils.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MESSAGE_SIZE 200 
#define PORT 8018
#define THRESHOLD 1260    // Timing threshold


int main(){
    
    // Update these values accordingly
    char* received_msg = NULL;
    int received_msg_size = 0;

    received_msg = (char *)malloc(MAX_MSG_SIZE);
    if (received_msg == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    memset(received_msg, 0, MAX_MSG_SIZE);

    int sock;
    struct sockaddr_in serv_addr;
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        perror("Socket creation error"); 
        exit(EXIT_FAILURE); 
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0)
    {
        perror("Connect failed");
        exit(EXIT_FAILURE);
    }

    int shm_fd = shm_open("flush_reload_shm", O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) { 
        perror("shm_open failed"); 
        exit(EXIT_FAILURE); 
    }
   
    char *shared_mem;
    shared_mem = mmap(NULL, sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) { 
        perror("mmap failed"); 
        exit(EXIT_FAILURE); 
    }
    char buffer[20];
    for (int i = 0; i < 168; i++) {  
        char byte = 0;
        flush((void*)shared_mem);
        printf("Byte: %d ",i+1);
        for (int j = 0; j < 8; j++) {
            
            write(sock, "Send Bit", 9); // Tell sender to send bit

            read(sock, buffer, 20);  // Wait for sender’s encryption complete signal

            size_t s_time = rdtsc();  
            maccess((void*)shared_mem);  // Access shared memory location
            size_t e_time = rdtsc(); 
            size_t delta=e_time-s_time;
            // printf("delta: %ld  ",delta);

            int bit = ( delta < THRESHOLD) ? 1 : 0;
            printf("%d", bit);
            if (j%8==7){
                printf("  ");
            }
             // If access is fast, it's a '1'
            byte |= (bit << j);

            write(sock, "Bit Received", 13);  // Notify sender that decryption is complete
        }
        printf("\n");
        received_msg[received_msg_size++] = byte;
}

    close(sock);
    printf("Received Message: ");
    for (int i = 0; i < received_msg_size; i++) {
        printf("%c", received_msg[i]);
    }
    printf("\n");
    // DO NOT MODIFY THIS LINE
    printf("Accuracy (%%): %f\n", check_accuracy(received_msg, received_msg_size)*100);
}