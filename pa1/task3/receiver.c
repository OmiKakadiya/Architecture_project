#include <stdio.h>
#include "utils.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include "../task1/cacheutils.h"

// Shared memory and timing parameters
#define FILESIZE       (1024 * 10)
#define FILE_NAME      "./invisibleink"
#define THRESHOLD      295
#define TARGET_CYCLES  2000000

// Total number of encoded bytes we expect to receive.
// (This must match what the sender transmits. For example, if the original image size is N bytes,
// then the encoded message length is 2*N bytes since each byte is encoded into two codewords.)
#define MESSAGE_SIZE   40000  // Adjust as needed

// --- Hamming(7,4) Decoding ---
// For Hamming(7,4): Codeword bit positions (1-indexed):
// Positions 1,2,4 are parity bits and positions 3,5,6,7 hold data bits.
// This function assumes the 7-bit codeword is stored in the lower 7 bits of a byte.
unsigned char hamming_decode_nibble(unsigned char codeword) {
    // Extract bits into an array c[1..7]
    unsigned char c[8];
    for (int i = 1; i <= 7; i++) {
        c[i] = (codeword >> (i - 1)) & 1;
    }
    // Calculate parity checks (syndrome bits)
    unsigned char p1 = c[1] ^ c[3] ^ c[5] ^ c[7];
    unsigned char p2 = c[2] ^ c[3] ^ c[6] ^ c[7];
    unsigned char p3 = c[4] ^ c[5] ^ c[6] ^ c[7];
    int syndrome = (p3 << 2) | (p2 << 1) | p1; // value 0..7
    if (syndrome != 0 && syndrome <= 7) {
        // Flip the bit at the syndrome position to correct a single-bit error.
        codeword ^= (1 << (syndrome - 1));
    }
    // Extract the data bits from positions 3,5,6,7 (d0,d1,d2,d3)
    unsigned char d0 = (codeword >> (3 - 1)) & 1;
    unsigned char d1 = (codeword >> (5 - 1)) & 1;
    unsigned char d2 = (codeword >> (6 - 1)) & 1;
    unsigned char d3 = (codeword >> (7 - 1)) & 1;
    unsigned char nibble = (d3 << 3) | (d2 << 2) | (d1 << 1) | d0;
    return nibble;
}

// Decode the entire encoded message.
// Every original byte was encoded as two codewords (high nibble and low nibble).
void hamming_decode(unsigned char *encoded, int encoded_len, unsigned char **decoded, int *decoded_len) {
    int orig_len = encoded_len / 2;
    unsigned char *output = malloc(orig_len);
    if (!output) {
        perror("malloc failed");
        exit(1);
    }
    for (int i = 0; i < orig_len; i++) {
        unsigned char high_nibble = hamming_decode_nibble(encoded[2*i]);
        unsigned char low_nibble  = hamming_decode_nibble(encoded[2*i+1]);
        output[i] = (high_nibble << 4) | low_nibble;
    }
    *decoded = output;
    *decoded_len = orig_len;
}

// Receiver: uses flush+reload to receive bits for 'encoded_message_bytes' bytes.
void receive_bit_by_bit(unsigned char *received_msg, int encoded_message_bytes) {
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
    // Synchronization phase
    int j = 0, counter = 0;
    while (j <= 20) {
        size_t start_time = rdtsc();
        volatile char value = *(shared_mem + 65);
        size_t end_time = rdtsc();
        counter++;
        if (end_time - start_time < THRESHOLD)
            j++;
        flush((shared_mem + 65));
        sched_yield();
    }
    printf("Sync counter: %d\n", counter);
    
    // Receive the encoded bits
    for (int i = 0; i < encoded_message_bytes * 8; ++i) {   
        unsigned long long start = rdtsc();
        int hits = 0, misses = 0;
        while ((rdtsc() - start) < TARGET_CYCLES) {
            size_t t0 = rdtsc();
            volatile char value = *shared_mem;
            size_t t1 = rdtsc();
            if ((t1 - t0) < THRESHOLD)
                hits++;
            else
                misses++;
            flush(shared_mem);
        }
        int bit_position = i % 8;
        int byte_index   = i / 8;
        if (hits > misses)
            received_msg[byte_index] |= (1 << bit_position);
    }
    close(fd);
}

int main() {
    // Allocate buffer for the encoded message.
    unsigned char *received_encoded = malloc(MESSAGE_SIZE);
    if (!received_encoded) {
        perror("malloc failed");
        exit(1);
    }
    memset(received_encoded, 0, MESSAGE_SIZE);
    
    // Receive encoded bits over the covert channel.
    receive_bit_by_bit(received_encoded, MESSAGE_SIZE);
    
    // Now decode the received message.
    unsigned char *decoded;
    int decoded_len;
    hamming_decode(received_encoded, MESSAGE_SIZE, &decoded, &decoded_len);
    
    // Write the decoded data (image) to file.
    FILE *fp = fopen("received_image.jpg", "wb");
    if (fp == NULL) {
        perror("Error opening file for writing");
        exit(1);
    }
    fwrite(decoded, 1, decoded_len, fp);
    fclose(fp);
    
    printf("Received image saved as received_image.jpg (decoded length = %d bytes)\n", decoded_len);
    
    free(received_encoded);
    free(decoded);
    return 0;
}
