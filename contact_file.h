
#ifndef CONTACT_FILE_H
#define CONTACT_FILE_H

#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include "contact_dynamic.h"

#define FILE_MAGIC_LRBT 0x4C524254 // "LRBT"
#define FILE_MAGIC_TRBL 0x5452424C // "TRBL"
#define FILE_FORMAT_VERSION 1

typedef struct
{
    uint32_t magic;   // "LRBT"
    uint32_t version; // 1
    uint32_t header_checksum;
    uint32_t data_checksum;
    uint32_t contact_count;
    uint32_t next_contact_id;
    time_t timestamp;
    uint32_t header_size;  // sizeof(ContactFileHeader)
    uint32_t contact_size; // sizeof(Contact)
    uint32_t reserved[4];
} ContactFileHeader;

// Function prototypes
bool contact_file_save(const ContactList *list, const char *filename);
bool contact_file_load(ContactList *list, const char *filename);
uint32_t fletcher32(const void *data, size_t length);
static void fletcher32_update_stream(uint32_t *sum1, uint32_t *sum2, const void *data, size_t length);
static bool write_contact(FILE *file, const Contact *contact);
static uint32_t calculate_packed_checksum_stream(const ContactList *list);
static bool read_contact(FILE *file, Contact *contact);
bool contact_file_validate(const char *filename);
#endif