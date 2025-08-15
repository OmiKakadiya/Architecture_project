#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "utils.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include "../task1/cacheutils.h"

// Shared memory and timing parameters
// #define FILESIZE       (1024 * 10)
// #define FILE_NAME      "./invisibleink"
// #define THRESHOLD      295
#define TARGET_CYCLES  2000000

// File to be sent (an image)
// #define MSG_FILE       "image.jpg"

// --- Hamming(7,4) Encoding ---
// Given a 4-bit nibble, encode it into a 7-bit codeword stored in the lower 7 bits of a byte.
unsigned char hamming_encode_nibble(unsigned char nibble) {
    // Extract data bits d0-d3 (with d3 as the most significant bit)
    unsigned char d0 = nibble & 1;
    unsigned char d1 = (nibble >> 1) & 1;
    unsigned char d2 = (nibble >> 2) & 1;
    unsigned char d3 = (nibble >> 3) & 1;
    // Compute parity bits:
    // p1 covers positions 1,3,5,7: p1 = d0 ^ d1 ^ d3
    unsigned char p1 = d0 ^ d1 ^ d3;
    // p2 covers positions 2,3,6,7: p2 = d0 ^ d2 ^ d3
    unsigned char p2 = d0 ^ d2 ^ d3;
    // p3 covers positions 4,5,6,7: p3 = d1 ^ d2 ^ d3
    unsigned char p3 = d1 ^ d2 ^ d3;
    // Construct the 7-bit codeword.
    // Positions (1-indexed): 1: p1, 2: p2, 3: d0, 4: p3, 5: d1, 6: d2, 7: d3.
    unsigned char codeword = 0;
    codeword |= (p1 & 1) << (1 - 1);
    codeword |= (p2 & 1) << (2 - 1);
    codeword |= (d0 & 1) << (3 - 1);
    codeword |= (p3 & 1) << (4 - 1);
    codeword |= (d1 & 1) << (5 - 1);
    codeword |= (d2 & 1) << (6 - 1);
    codeword |= (d3 & 1) << (7 - 1);
    return codeword;
}

// Encode the entire message using Hamming(7,4).
// Each original byte is split into two nibbles; each nibble is encoded into one codeword (1 byte).
// Thus, the encoded message length will be 2 * (original message length) bytes.
void hamming_encode(unsigned char *input, int input_len, unsigned char **output, int *output_len) {
    int enc_len = input_len * 2;
    unsigned char *enc = malloc(enc_len);
    if (!enc) {
        perror("malloc failed");
        exit(1);
    }
    for (int i = 0; i < input_len; i++) {
        unsigned char byte = input[i];
        unsigned char high_nibble = (byte >> 4) & 0x0F;
        unsigned char low_nibble  = byte & 0x0F;
        enc[2*i]     = hamming_encode_nibble(high_nibble);
        enc[2*i + 1] = hamming_encode_nibble(low_nibble);
    }
    *output = enc;
    *output_len = enc_len;
}

// The secret protocol: send the encoded message via flush+reload.
void secret_protocol(unsigned char *message, int message_len) {
    int fd = open(FILE_NAME, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open failed");
        exit(1);
    }
    ftruncate(fd, FILESIZE);
    char *shared_mem = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }
    printf("Sending encoded message of size: %d bytes\n", message_len);
    
    // Synchronization phase
    int j = 0, counter = 0;
    while (j <= 40) {
        volatile char value;
        value = *(shared_mem + 129);
        j++;
        value = *(shared_mem + 65);
        sched_yield();
    }
    printf("Sync complete, counter: %d\n", counter);
    
    printf("time: %lld\n", rdtsc());
    // Transmit each bit of the encoded message.
    for (int i = 0; i < message_len * 8; ++i) {
        unsigned long long start = rdtsc();
        while ((rdtsc() - start) < TARGET_CYCLES) {
            if (message[i / 8] & (1 << (i % 8))) {
                volatile char value = *shared_mem;
            }
        }
    }
    usleep(1);
    close(fd);
}

int main() {
    // Open the image file.
    FILE *fp = fopen(MSG_FILE, "rb");
    if (fp == NULL) {
        printf("Error opening file %s\n", MSG_FILE);
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);
    printf("File %s size: %ld bytes\n", MSG_FILE, file_size);
    
    unsigned char *msg = malloc(file_size);
    if (!msg) {
        perror("malloc failed");
        fclose(fp);
        return 1;
    }
    size_t msg_size = fread(msg, 1, file_size, fp);
    if (msg_size != file_size) {
        printf("Warning: expected to read %ld bytes, but read %zu bytes\n", file_size, msg_size);
    }
    fclose(fp);
    
    // Encode the message using Hamming(7,4)
    unsigned char *encoded;
    int encoded_len;
    hamming_encode(msg, msg_size, &encoded, &encoded_len);
    printf("Encoded message size: %d bytes\n", encoded_len);
    
    clock_t start = clock();
    secret_protocol(encoded, encoded_len);
    clock_t end = clock();
    double time_taken = ((double)end - start) / CLOCKS_PER_SEC;
    printf("Message sent successfully\n");
    printf("Time taken to send the message: %f seconds\n", time_taken);
    printf("Original message size: %zu bytes\n", msg_size);
    printf("Bits per second: %f\n", msg_size * 8 / time_taken);
    
    free(msg);
    free(encoded);
    return 0;
}
