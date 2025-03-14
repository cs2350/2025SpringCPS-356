//
//  main.cpp
//  memoryWalk
//
//  Created by wsucatslabs on 3/9/22.
//

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>

static constexpr uint64_t BIGBUFFERSIZE{1UL << 28};

// Demonstrate difference in performance between reads from sequential addresess
// vs. the same number of reads from non-sequential sddresses.
//
// Benchmark approach:
// - Fill a large buffer with indexes.
// - In a loop, read each index from the buffer and use the read index as
// the next index to read.
//
// If the buffer with indexes contains sequential indexes, there the read loop
// will read at sequential indexes. If the buffer with indexes contains random
// order indexes, then the read loop will read at non sequential indexes.
// Filling the buffer with indexes may take different amounts of time based on
// sequential vs. not sequential indexes, but the read loop code is exactly the
// same code reardless of the way the buffer was filled.
int main(int argc, const char *argv[]) {
  uint64_t *bigBuffer{nullptr};

  // Allocate buffer big enough that it won't all fit in RAM
  bigBuffer = (uint64_t *)malloc(BIGBUFFERSIZE * sizeof(uint64_t));

  if (argc > 1) {
    // Fill the boffer twice in both cases so that the difference in timing
    // from calling random() vs incrementing an index does not change total
    // run time.
    for (uint64_t i = 0; i < BIGBUFFERSIZE; ++i) {
      bigBuffer[i] = (i + 1) % BIGBUFFERSIZE;
    }
    std::cout << "Using non-sequential addresses\n";
    for (uint64_t i = 0; i < BIGBUFFERSIZE; ++i) {
      bigBuffer[i] = (((uint64_t)random()) * random()) % BIGBUFFERSIZE;
    }
  } else {
    for (uint64_t i = 0; i < BIGBUFFERSIZE; ++i) {
      bigBuffer[i] = (((uint64_t)random()) * random()) % BIGBUFFERSIZE;
    }
    std::cout << "Using sequential addresses\n";
    for (uint64_t i = 0; i < BIGBUFFERSIZE; ++i) {
      bigBuffer[i] = (i + 1) % BIGBUFFERSIZE;
    }
  }

  // The following code is identical no matter how bigBuffer was filled
  std::cout << "Starting reads\n";
  volatile uint64_t sum{0};

  for (uint32_t i = 0; i < 16; ++i) {
    uint64_t nextIndex{0};
    for (uint64_t i = 0; i < BIGBUFFERSIZE; ++i) {
      nextIndex = bigBuffer[nextIndex];
      sum += nextIndex;
    }
  }

  return 0;
}
