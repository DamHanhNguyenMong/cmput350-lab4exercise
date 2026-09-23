#include <cstddef>  // size_t
#include <cstdint>  // uint64_t
#include <iostream> // std::cout

#include <random>   // std::mt19937_64
#include <vector>   // std::vector

#include "timer.h"

constexpr size_t N = 4000;
constexpr size_t SIZE = N*N;

int main() { 
    // Seed your PRNG with 0 before starting to fill each container
    std::mt19937_64 rng(0); // start with seed 0

    // Allocate two contiguous arrays
    uint64_t* rowArray = new uint64_t[SIZE];
    uint64_t* colArray = new uint64_t[SIZE];

    // row wise
    for (size_t row = 0; row < N; ++row) {
        for (size_t col = 0; col < N; ++col) {
            rowArray[row * N + col] = rng();
        }
    }

    rng.seed(0); // re-seed

    // col wise
    for (size_t col = 0; col < N; ++col) {
        for (size_t row = 0; row < N; ++row) {
            colArray[row * N + col] = rng();
        }
    }

    Timer timer;

    // Sum the first array in row wise
    uint64_t rowArraySum = 0;

    timer.restart(); //start the timer
    for (size_t row = 0; row < N; ++row) {
        for (size_t col = 0; col < N; ++col) {
            rowArraySum += rowArray[row*N+col];
        }
    }
    // Stop the timer and retrieve the time
    uint64_t rowArrayTime = timer.click<Timer::Micros>();

    // Sum the second array in col wise
    uint64_t colArraySum = 0;
    timer.restart();

    for (size_t col = 0; col < N; ++col) {
        for (size_t row = 0; row < N; ++row) {
            colArraySum += colArray[row*N+col];
        }
    }
    uint64_t colArrayTime = timer.click<Timer::Micros>();

    std::cout << rowArrayTime << " " << rowArraySum << "\n";
    std::cout << colArrayTime << " " << colArraySum << "\n";

    // Free allocated memory.
    delete[] rowArray;
    delete[] colArray;
    
    return 0; 
}
