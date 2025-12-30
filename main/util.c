#include "include/util.h"

// Some bit-twiddling to reverse an uint8
uint8_t reverse_u8(uint8_t value) {
    // Swap first 4 with the last 4
    value = (value >> 4) | (value << 4);
    // Swap pairs 0xCC => 11001100, 0x33 => 00110011
    value = ((value & 0xCC) >> 2) | ((value & 0x33) << 2);
    // Swap individual 0xAA => 10101010, 0x55 => 01010101
    value = ((value & 0xAA) >> 1) | ((value & 0x55) << 1);

    return value;
}