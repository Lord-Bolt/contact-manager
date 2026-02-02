// contact_file.c
#include "contact_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t fletcher32(const void *data, size_t length)
{
    // Step 1: Handle edge cases
    if (data == NULL || length == 0)
    {
        return 0; // No data = checksum 0
    }

    // Step 2: Cast to byte pointer (uint8_t = unsigned 8-bit = 1 byte)
    const uint8_t *bytes = (const uint8_t *)data;

    // Step 3: Initialize sums
    uint32_t sum1 = 0; // Sum of chunks
    uint32_t sum2 = 0; // Running sum of sum1

    // Step 4: Process 16-bit chunks (2 bytes at a time)
    size_t i = 0;
    while (i + 1 < length)
    { // While we have at least 2 bytes left
        // Combine two bytes into one 16-bit chunk
        // bytes[i] << 8 = shift first byte left 8 bits
        // | bytes[i+1] = OR with second byte
        // Example: bytes[0]=0x61, bytes[1]=0x62 → 0x6100 | 0x62 = 0x6162
        uint16_t chunk = (bytes[i] << 8) | bytes[i + 1];
        sum1 = (sum1 + chunk) % 65535;
        sum2 = (sum2 + sum1) % 65535;
        i += 2; // Move to next chunk
    }

    // Step 6: Handle leftover byte (if length is odd)
    if (i < length)
    {
        // We have one byte left (like 'e' in "abcde")
        // Pad it with 0 to make 16-bit: 0x65 → 0x6500
        uint16_t chunk = bytes[i] << 8; // Shift left, low byte stays 0
        sum1 = (sum1 + chunk) % 65535;
        sum2 = (sum2 + sum1) % 65535;
    }

    // Step 7: Combine into 32-bit result
    // sum2 << 16 = move sum2 to upper 16 bits
    // | sum1 = combine with sum1 in lower 16 bits
    uint32_t result = (sum2 << 16) | sum1;

    return result;
}

// TODO: Stub functions for now
bool contact_file_save(const ContactList *list, const char *filename)
{
    printf("SAVE: Not implemented yet\n");
    return false;
}

bool contact_file_load(ContactList *list, const char *filename)
{
    printf("LOAD: Not implemented yet\n");
    return false;
}