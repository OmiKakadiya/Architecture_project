#include<stdlib.h>
#include<stdio.h>
#include "./cacheutils.h"
    
#define ARRAY_SIZE 12 * 1024 * 1024

int main(int argc,char *argv[]) {
    int k=atoi(argv[1]);
    unsigned char buff[16];
    int j = 0;
    int *arr = malloc(ARRAY_SIZE * sizeof(int));
    for (size_t i = 0; i < ARRAY_SIZE; i++)
    {
        arr[i] = i + 20;
    }
    int temp;
    unsigned long long target_cycles = 1000000000;
    for(int z=0;z<100;++z)
    {
        // int j = 0;
        // size_t start_time = rdtsc();
        unsigned long long start = rdtsc();
        while ((rdtsc() - start) < target_cycles)
        {
            {
                for (size_t i = 0; i < ARRAY_SIZE; i += k)
                {
                    temp = arr[i];
                }
                // ++j;
            }
        }
    }
}
// }