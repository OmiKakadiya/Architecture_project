#include <iostream>
#include <x86intrin.h>  // For __rdtsc()
#include <cstdlib>

#define ITERATIONS 1000000

void measure_latency(size_t array_size) {
    volatile int *arr = new int[array_size / sizeof(int)]; // Allocate memory
    unsigned long long start, end;
    
    size_t index = 0;
    double total_cycles = 0;

    // Run multiple iterations for accuracy
    for (int i = 0; i < ITERATIONS; i++) {
        index = (index + 16) % (array_size / sizeof(int)); // Small stride to prevent cache optimizations
        start = __rdtsc(); // Start timing
        arr[index]++; // Access memory
        end = __rdtsc(); // End timing
        total_cycles += (end - start);
    }

    delete[] arr;
    
    double avg_cycles = total_cycles / ITERATIONS;
    std::cout << "Latency for size " << (array_size / 1024) << " KB: " << avg_cycles << " cycles" << std::endl;
}

int main() {
    std::cout << "Measuring Cache & DRAM Latency...\n";

    measure_latency(32 * 1024);    // L1 Cache (~32 KB)
    measure_latency(1536 * 1024);   // L2 Cache (~512 KB)
    measure_latency(9 * 1024 * 1024); // L3 Cache (~8 MB)
    measure_latency(128 * 1024 * 1024); // DRAM (~128 MB)

    return 0;
}
