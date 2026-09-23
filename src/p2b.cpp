#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream> // std::cout
#include <random>   // std::mt19937_64

#include "timer.h"
#include "p2a.h"

constexpr size_t X = 256;
constexpr size_t Y = 256;
constexpr size_t Z = 256;

constexpr size_t K = 4;
constexpr size_t OUT_X = X / K;
constexpr size_t OUT_Y = Y / K;
constexpr size_t OUT_Z = Z / K;

constexpr size_t INPUT_SIZE = X * Y * Z;
constexpr size_t OUTPUT_SIZE = OUT_X * OUT_Y * OUT_Z;
constexpr size_t KERNEL_SIZE = K * K * K;

// Index for the row-major array A
size_t rowMajorIndexA(size_t x, size_t y, size_t z) {
    return z * Y * X + y * X + x;
}
// Index for the 4x4x4 row-major kernel
size_t rowMajorIndexK(size_t x, size_t y, size_t z) {
    return z * K * K + y * K + x;
}
// Index for the 64x64x64 row-major convolution output
size_t rowMajorIndexConv(size_t x, size_t y, size_t z) {
    return z * OUT_Y * OUT_X + y * OUT_X + x;
}

int main() {
    // 2 arrays
    uint64_t* A = new uint64_t[INPUT_SIZE]; // normal row major 
    uint64_t* B = new uint64_t[INPUT_SIZE]; // morton order

    std::mt19937_64 rng(0);
    // traverse row major order
    for (size_t z = 0; z < Z; ++z) {
        for (size_t y = 0; y < Y; ++y) {
            for (size_t x = 0; x < X; ++x) {
                A[rowMajorIndexA(x, y, z)] = rng();
            }
        }
    }
    // traverse morton order
    for (size_t z = 0; z < Z; ++z) {
        for (size_t y = 0; y < Y; ++y) {
            for (size_t x = 0; x < X; ++x) {
                B[morton3d(x, y, z)] = A[rowMajorIndexA(x, y, z)];
            }
        }
    }

    //Kernel
    uint64_t Ka[KERNEL_SIZE]; // row major
    uint64_t Kb[KERNEL_SIZE]; // Morton

    //Populate kernel: K[z*K+y*K+x]=x+y+z
    for (size_t z = 0; z < K; ++z) {
        for (size_t y = 0; y < K; ++y) {
            for (size_t x = 0; x < K; ++x) {
                uint64_t value = x + y + z;
                Ka[rowMajorIndexK(x, y, z)] = value;
                Kb[morton3d(x, y, z)] = value;
            }
        }
    }

    // Output array
    uint64_t* outputA = new uint64_t[OUTPUT_SIZE];
    uint64_t* outputB = new uint64_t[OUTPUT_SIZE];


    Timer timer;
    timer.restart();

    for (size_t oz = 0; oz < OUT_Z; ++oz) {
        for (size_t oy = 0; oy < OUT_Y; ++oy) {
            for (size_t ox = 0; ox < OUT_X; ++ox) {

                uint64_t sum = 0;

                for (size_t kz = 0; kz < K; ++kz) {
                    for (size_t ky = 0; ky < K; ++ky) {
                        for (size_t kx = 0; kx < K; ++kx) {
                            // A position

                            size_t x = ox * K + kx;
                            size_t y = oy * K + ky;
                            size_t z = oz * K + kz;
                            sum += A[rowMajorIndexA(x, y, z)] * Ka[rowMajorIndexK(kx, ky, kz)];
                        }
                    }
                }
                // save sum
                outputA[rowMajorIndexConv(ox, oy, oz)] = sum;
            }
        }
    }
    uint64_t rowMajorTime = timer.click<Timer::Micros>();

    //Morton order convolution
    timer.restart();
    // Each 4x4x4 block contains 64 values
    // In Morton order, each block's 64 values are consecutive
    // Kb uses the same Morton ordering, so we can compute
    // the dot product by directly walking through both arrays

    for (size_t block = 0; block < OUTPUT_SIZE; ++block) {
        // Initialize sum
        uint64_t sum = 0;
        // Find the first value of this 4x4x4 block
        size_t blockStart = block * KERNEL_SIZE;
        for (size_t i = 0; i < KERNEL_SIZE; ++i) {
            sum += B[blockStart + i] * Kb[i];
        }
        outputB[block] = sum;
    }
    uint64_t mortonTime = timer.click<Timer::Micros>();

    // Check that both convolution results are identical
    for (size_t oz = 0; oz < OUT_Z; ++oz) {
        for (size_t oy = 0; oy < OUT_Y; ++oy) {
            for (size_t ox = 0; ox < OUT_X; ++ox) {

                size_t indexA = rowMajorIndexConv(ox, oy, oz);
                size_t indexB = morton3d(ox, oy, oz);
                assert(outputA[indexA] == outputB[indexB]);
            }
        }
    }
    // Print only the two required timings
    std::cout << rowMajorTime << "\n";
    std::cout << mortonTime << "\n";

    // Free all allocated memory.
    delete[] A;
    delete[] B;
    delete[] outputA;
    delete[] outputB;

    return 0; 
}
