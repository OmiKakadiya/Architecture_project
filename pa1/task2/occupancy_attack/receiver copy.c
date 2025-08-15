#include <stdio.h>
#include "utils.h"
#include <string.h>
#include <arpa/inet.h>
#include "../../task1/cacheutils.h"
#include <stdlib.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>

int main()
{

    // Update these values accordingly
    char *received_msg = NULL;
    int received_msg_size = 0;
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    listen(server_fd, 1);
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_size);
    char buffer[1];
    int *arr = malloc(4 * 1024 * 1024*4);

    // while(1) {

    // memset(buffer, 0, 1);
    while(1){

    for(int i=0;i<1024*1024*4;++i){
        arr[i]=i+1;
    }

    int a;
    for (int j = 0; j < 1000; ++j)
    {
        a = 0;
        for (int i = 0; i < 4*1024 * 1024;)
        {
            a = arr[i];
            i = a + 29;
        }
    }
    // recv(cl`ient_fd, buffer, 1, 0);
    // if(buffer[1]=='1')

    int counter = 0;
    int p = clock();
    // printf("%d",p);
    int i = 0;
    while (clock() - p < 10000)
    {
        counter++;
        arr[i];
        i++;
        i = i % (1024 * 1024*4);
    }
    printf("%d", counter);
    send(client_fd, "1", 1, 0);
    // for (int i = 0; i < 1024 * 1024; ++i)
    //     flush(&arr[i]);

    recv(client_fd, buffer, 1, 0);
    int counter2 = 0;
    p = clock();
    i = 0;
    while (clock() - p < 10000)
    {
        counter2++;
        arr[i];
        i++;
        i = i % (1024 * 1024*4);
    }
    printf("\n%d\n", counter2);
    printf("%d\n",counter-counter2);
    printf("+++++++++++++++++++++++++++++++++++++++\n");

}
    //     i=0;
    // int *arr2 = malloc(4 * 1024 * 1024 * sizeof(int));

    // for(int j=0;j<4;++j) {
    //     i=0;
    //     while(i<4*1024*1024){
    //         arr2[i]=6;
    //         i+=16;
    //     }
    free(arr);
}
// usleep(100000);
// recv(client_fd,buffer,1,0);

// free(arr);
// DO NOT MODIFY THIS LINE
// printf("Accuracy (%%): %f\n", check_accuracy(received_msg, received_msg_size)*100);
// }
