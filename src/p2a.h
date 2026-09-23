#include <cassert>
#include <cstddef>
#include <cstdint>

inline uint64_t expand(uint64_t input, uint32_t scale) {
    assert(scale>=1); //Check if scale is larger than 1

    uint64_t result = 0;

    //Loop through each 64 bits
    for (size_t i = 0; i<64; ++i) {
        if (input & (1ULL << i)){ //mask to check if bit at position i is equal to 1
            uint64_t newPosition = static_cast<uint64_t>(i) * scale; //new position will be at i*scale
            if (newPosition < 64) { //If the expansion causes the result to be larger than 64 bits, we truncate the leftmost (most significant) bits.
                result |= (1ULL << newPosition); // puts a 1 at that position
            }
        } 
        
    }
    return result;
    
}

inline uint64_t morton3d(uint64_t x, uint64_t y, uint64_t z) {
    return expand(x, 3) | (expand(y, 3) << 1) | (expand(z, 3) << 2);
    // expand with scale of 3 eg: 0002
    // x goes 0, 3, 6, ...; y goes 1, 4, 7, ... => skip 1; z goes 2,5, 8, ... => skip 2
    // OR them to combine
}