#include <stdio.h>
#include "./utils.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "./cacheutils.h"


#define ARRAY_SIZE 12 * 1024 * 1024

int main() {
    unsigned long long target_cycles = 1000000000;
    int c=0;
    double avg=0;
    int temp;
    int *arr = malloc(ARRAY_SIZE * sizeof(int));
    for (size_t i = 0; i < ARRAY_SIZE; i++)
    {                    
        arr[i] = i + 20; 
    }
    for(ssize_t i=0;i<ARRAY_SIZE;i+=20){
        temp=arr[i];
    }
    for (int z=0;z<100;++z)
    {
        size_t start_time1 = rdtsc();
        int counter = 0;
        unsigned long long start = rdtsc();
        do
        {
            counter++;
            for (size_t i = 0; i < ARRAY_SIZE; i += 16)
            {
                temp = arr[i];
            }
        } while ((rdtsc() - start) < target_cycles);
        printf("Counter : %d\n",counter);
        if(counter<100){
            c++;
            avg+=counter;
        }
    }
    printf("average : %f",avg/c);

}