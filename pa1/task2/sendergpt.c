#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "utils.h"
#include "cacheutils.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define PORT 8000

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

    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed"); 
        exit(0); 
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) != 0) { 
        perror("bind failed"); 
        exit(0); 
    }

    if (listen(server_fd, 1) != 0) { 
        perror("listen"); 
        exit(0); 
    }

    printf("Waiting for receiver to connect...\n");
    if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) { 
        perror("accept failed"); 
        exit(0); 
    }

    int shm_fd = shm_open("flush_reload_shm", O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) { 
        perror("shm_open failed"); 
        exit(0); 
    }
    if (ftruncate(shm_fd, sizeof(char)) == -1) { 
        perror("ftruncate failed"); 
        exit(0); 
    }
    char *shared_mem;
    shared_mem = mmap(NULL, sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) { 
        perror("mmap failed"); 
        exit(0); 
    }

    for (int i = 0; i < msg_size; i++) {
        for (int j = 0; j < 8; j++) {
            char sync_signal;
            recv(client_fd, &sync_signal, sizeof(sync_signal), 0);  // Wait for receiver to be ready

            if ((msg[i] >> j) & 1) {
                maccess((void*)shared_mem);  // Bring into cache if bit = 1
            }

            send(client_fd, &sync_signal, sizeof(sync_signal), 0);  // Notify receiver that bit is encoded
            
            recv(client_fd, &sync_signal, sizeof(sync_signal), 0);  // Wait for receiver to complete decryption
            
            flush((void*)shared_mem);  // Flush memory location
        }
    }

    close(client_fd);
    close(server_fd);
    close(shm_fd);

    clock_t end = clock();
    double time_taken = ((double)end - start) / CLOCKS_PER_SEC;
    printf("Message sent successfully\n");
    printf("Time taken to send the message: %f\n", time_taken);
    printf("Message size: %d\n", msg_size);
    printf("Bits per second: %f\n", msg_size * 8 / time_taken);
}
