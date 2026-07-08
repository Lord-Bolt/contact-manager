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
    {
        uint16_t chunk = (bytes[i] << 8) | bytes[i + 1];
        sum1 = (sum1 + chunk) % 65535;
        sum2 = (sum2 + sum1) % 65535;
        i += 2; // Move to next chunk
    }

    // Step 6: Handle leftover byte (if length is odd)
    if (i < length)
    {
        // We have one byte left (like 'e' in "abcde")
        uint16_t chunk = bytes[i] << 8;
        sum1 = (sum1 + chunk) % 65535;
        sum2 = (sum2 + sum1) % 65535;
    }

    // Step 7: Combine into 32-bit result
    uint32_t result = (sum2 << 16) | sum1;

    return result;
}

static bool write_contact(FILE *file, const Contact *contact)
{
    if (fwrite(contact->name, 1, MAX_NAME_LEN, file) != MAX_NAME_LEN)
    {
        return false;
    }

    if (fwrite(contact->phone, 1, MAX_PHONE_LEN, file) != MAX_PHONE_LEN)
    {
        return false;
    }

    if (fwrite(contact->email, 1, MAX_EMAIL_LEN, file) != MAX_EMAIL_LEN)
    {
        return false;
    }

    if (fwrite(&contact->id, sizeof(int), 1, file) != 1)
    {
        return false;
    }
    return true;
} // 323 Bytes - No padding

static uint32_t calculate_packed_checksum_stream(const ContactList *list)
{
    uint32_t sum1 = 0;
    uint32_t sum2 = 0;

    for (int i = 0; i < list->size; i++)
    {
        const Contact *c = &list->data[i];

        // Process in EXACT same order as file write
        fletcher32_update_stream(&sum1, &sum2, c->name, MAX_NAME_LEN);
        fletcher32_update_stream(&sum1, &sum2, c->phone, MAX_PHONE_LEN);
        fletcher32_update_stream(&sum1, &sum2, c->email, MAX_EMAIL_LEN);
        fletcher32_update_stream(&sum1, &sum2, &c->id, sizeof(int));
    }

    return (sum2 << 16) | sum1;
}

bool contact_file_save(const ContactList *list, const char *filename)
{
    if (list == NULL || filename == NULL)
    {
        printf("SAVE ERROR: NULL parameters\n");
        return false;
    }

    // Declare variables at top (C89 style)
    FILE *file = NULL;
    ContactFileHeader header = {0};
    bool success = false;

    // === STEP 1: Open file for binary writing ===
    file = fopen(filename, "wb");
    if (file == NULL)
    {
        printf("SAVE ERROR: Cannot open '%s' for writing\n", filename);
        goto cleanup;
    }

    // === STEP 2: Fill header structure ===
    header.magic = FILE_MAGIC_LRBT;       // "LRBT"
    header.version = FILE_FORMAT_VERSION; // 1
    header.contact_count = list->size;
    header.next_contact_id = next_contact_id; // GLOBAL - from contact_dynamic.c
    header.timestamp = time(NULL);            // Current Unix time
    header.header_size = sizeof(ContactFileHeader);
    header.contact_size = 323; // MANUAL PACKED SIZE: 50+15+254+4

    // === STEP 3: Calculate checksums ===
    // Data checksum (all packed contacts)
    header.data_checksum = calculate_packed_checksum_stream(list);
    header.header_checksum = fletcher32(
        &header.data_checksum,
        sizeof(ContactFileHeader) - offsetof(ContactFileHeader, data_checksum));

    // === STEP 4: Write header ===
    if (fwrite(&header, sizeof(header), 1, file) != 1)
    {
        printf("SAVE ERROR: Failed to write header\n");
        goto cleanup;
    }

    // === STEP 5: Write all contacts (MANUAL PACKING) ===
    for (int i = 0; i < list->size; i++)
    {
        if (!write_contact(file, &list->data[i]))
        {
            printf("SAVE ERROR: Failed to write contact %d\n", i);
            goto cleanup;
        }
    }

    // === STEP 6: Write footer ===
    uint32_t footer_magic = FILE_MAGIC_TRBL; // "TRBL"
    if (fwrite(&footer_magic, sizeof(footer_magic), 1, file) != 1)
    {
        printf("SAVE ERROR: Failed to write footer magic\n");
        goto cleanup;
    }

    // Write timestamp (should match header timestamp)
    if (fwrite(&header.timestamp, sizeof(time_t), 1, file) != 1)
    {
        printf("SAVE ERROR: Failed to write footer timestamp\n");
        goto cleanup;
    }

    // === STEP 7: Success! ===
    printf("SAVE SUCCESS: Saved %d contacts to '%s'\n",
           list->size, filename);
    printf("  File size: %ld bytes (header: %zu, data: %d, footer: %zu)\n",
           ftell(file), // Current position = total size
           sizeof(ContactFileHeader),
           list->size * 323, // 323 bytes per packed contact
           sizeof(uint32_t) + sizeof(time_t));

    success = true;

cleanup:
    if (file != NULL)
    {
        fclose(file);
    }
    return success;
}

bool contact_file_load(ContactList *list, const char *filename)
{
    if (list == NULL || filename == NULL)
    {
        printf("LOAD ERROR: NULL parameters\n");
        return false;
    }

    FILE *file = NULL;
    ContactFileHeader header = {0};
    uint32_t checksum_sum1 = 0;
    uint32_t checksum_sum2 = 0;
    bool success = false;

    // Open file
    file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("LOAD ERROR: Cannot open '%s'\n", filename);
        goto cleanup;
    }

    // Read header
    if (fread(&header, sizeof(header), 1, file) != 1)
    {
        printf("LOAD ERROR: Cannot read header\n");
        goto cleanup;
    }

    // 3. Verify magic "LRBT"
    if (header.magic != FILE_MAGIC_LRBT)
    {
        printf("LOAD ERROR: Not a LRBT file (got 0x%08X, expected 0x%08X)\n",
               header.magic, FILE_MAGIC_LRBT);
        goto cleanup;
    }

    // 4. Verify version
    if (header.version > FILE_FORMAT_VERSION)
    {
        printf("LOAD ERROR: File version %u is newer than supported %u\n",
               header.version, FILE_FORMAT_VERSION);
        goto cleanup;
    }

    // Verify sizes
    if (header.header_size != sizeof(ContactFileHeader))
    {
        printf("LOAD ERROR: Header size mismatch (got %u, expected %zu)\n",
               header.header_size, sizeof(ContactFileHeader));
        goto cleanup;
    }

    if (header.contact_size != 323)
    {
        printf("LOAD ERROR: Contact size mismatch (got %u, expected 323)\n",
               header.contact_size);
        goto cleanup;
    }

    printf("LOAD: Found %u contacts, next ID: %u\n",
           header.contact_count, header.next_contact_id);

    // Allocate memory
    contact_list_free(list);

    if (!contact_list_init(list, header.contact_count))
    {
        printf("LOAD ERROR: Memory allocation failed for %u contacts\n",
               header.contact_count);
        goto cleanup;
    }

    // Read contacts WITH CHECKSUM CALCULATION
    for (uint32_t i = 0; i < header.contact_count; i++)
    {
        Contact *contact = &list->data[i];

        // 7a. Name (50 byte)
        if (fread(contact->name, 1, MAX_NAME_LEN, file) != MAX_NAME_LEN)
        {
            printf("LOAD ERROR: Failed to read name for contact %u\n", i);
            goto cleanup;
        }
        contact->name[MAX_NAME_LEN - 1] = '\0';
        fletcher32_update_stream(&checksum_sum1, &checksum_sum2,
                                 contact->name, MAX_NAME_LEN);

        // 7b. Phone (15 bytes)
        if (fread(contact->phone, 1, MAX_PHONE_LEN, file) != MAX_PHONE_LEN)
        {
            printf("LOAD ERROR: Failed to read phone for contact %u\n", i);
            goto cleanup;
        }
        contact->phone[MAX_PHONE_LEN - 1] = '\0';
        fletcher32_update_stream(&checksum_sum1, &checksum_sum2,
                                 contact->phone, MAX_PHONE_LEN);

        // 7c. Email (254 bytes)
        if (fread(contact->email, 1, MAX_EMAIL_LEN, file) != MAX_EMAIL_LEN)
        {
            printf("LOAD ERROR: Failed to read email for contact %u\n", i);
            goto cleanup;
        }
        contact->email[MAX_EMAIL_LEN - 1] = '\0';
        fletcher32_update_stream(&checksum_sum1, &checksum_sum2,
                                 contact->email, MAX_EMAIL_LEN);

        // 7d. ID (4 bytes)
        if (fread(&contact->id, sizeof(int), 1, file) != 1)
        {
            printf("LOAD ERROR: Failed to read ID for contact %u\n", i);
            goto cleanup;
        }
        fletcher32_update_stream(&checksum_sum1, &checksum_sum2,
                                 &contact->id, sizeof(int));
    }

    // Checksum
    uint32_t calculated_checksum = (checksum_sum2 << 16) | checksum_sum1;

    if (calculated_checksum != header.data_checksum)
    {
        printf("LOAD ERROR: DATA CHECKSUM FAILED! File corrupted.\n");
        printf("  Expected: 0x%08X\n", header.data_checksum);
        printf("  Calculated: 0x%08X\n", calculated_checksum);
        printf("  Difference: 0x%08X\n",
               calculated_checksum ^ header.data_checksum);
        goto cleanup;
    }

    printf("CHECKSUM VERIFIED: 0x%08X\n", calculated_checksum);

    // Verify header checksum
    uint32_t header_checksum = fletcher32(
        &header.data_checksum,
        sizeof(ContactFileHeader) - offsetof(ContactFileHeader, data_checksum));

    if (header_checksum != header.header_checksum)
    {
        printf("LOAD ERROR: HEADER CHECKSUM FAILED! Header corrupted.\n");
        goto cleanup;
    }

    // Verify footer "TRBL"
    uint32_t footer_magic;
    time_t footer_timestamp;

    if (fread(&footer_magic, sizeof(footer_magic), 1, file) != 1)
    {
        printf("LOAD ERROR: Cannot read footer magic\n");
        goto cleanup;
    }

    if (footer_magic != FILE_MAGIC_TRBL)
    {
        printf("LOAD ERROR: Footer magic incorrect (got 0x%08X, expected 0x%08X)\n",
               footer_magic, FILE_MAGIC_TRBL);
        printf("         File may be truncated or corrupted\n");
        goto cleanup;
    }

    // Read timestamp
    if (fread(&footer_timestamp, sizeof(time_t), 1, file) == 1)
    {
        if (footer_timestamp != header.timestamp)
        {
            printf("WARNING: Footer timestamp doesn't match header\n");
            printf("         (Possible partial write, but data loaded)\n");
        }
    }

    // Update global next_contact_id
    extern int next_contact_id;
    next_contact_id = header.next_contact_id;

    list->size = header.contact_count;
    printf("LOAD SUCCESS: Loaded %u contacts, next ID: %u\n",
           header.contact_count, header.next_contact_id);

    success = true;

cleanup:
    if (file != NULL)
    {
        fclose(file);
    }

    if (!success)
    {
        contact_list_free(list);
    }

    return success;
}

bool contact_file_validate(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
        return false;

    ContactFileHeader header;
    bool valid = false;

    if (fread(&header, sizeof(header), 1, file) == 1)
    {
        if (header.magic == FILE_MAGIC_LRBT &&
            header.version <= FILE_FORMAT_VERSION &&
            header.header_size == sizeof(ContactFileHeader))
        {
            valid = true;
        }
    }

    fclose(file);
    return valid;
}

static void fletcher32_update_stream(uint32_t *sum1, uint32_t *sum2, const void *data, size_t length)
{
    const uint8_t *bytes = (const uint8_t *)data;

    for (size_t i = 0; i < length; i++)
    {
        *sum1 = (*sum1 + bytes[i]) % 65535;
        *sum2 = (*sum2 + *sum1) % 65535;
    }
}

bool rotate_backups(void)
{
    FILE *fp;
    fp = fopen("contacts.dat.bak3", "rb");
    if (fp != NULL)
    {
        fclose(fp);
        remove("contacts.dat.bak3");
    }

    fp = fopen("contacts.dat.bak2", "rb");
    if (fp != NULL)
    {
        fclose(fp);
        rename("contacts.dat.bak2", "contacts.dat.bak3");
    }

    fp = fopen("contacts.dat.bak1", "rb");
    if (fp != NULL)
    {
        fclose(fp);
        rename("contacts.dat.bak1", "contacts.dat.bak2");
    }

    fp = fopen("contacts.dat", "rb");
    if (fp != NULL)
    {
        fclose(fp);
        rename("contacts.dat", "contacts.dat.bak1");
    }
    fp = NULL;
    return true;
}

bool contact_file_load_backup(ContactList *list)
{
    const char *files[] = {
        "contacts.dat",
        "contacts.dat.bak1",
        "contacts.dat.bak2",
        "contacts.dat.bak3"};

    for (int i = 0; i < 4; i++)
    {
        if (contact_file_load(list, files[i]))
        {
            printf("Successfully loaded from %s\n", files[i]);
            return true;
        }
    }

    printf("All backups exhausted. No valid contacts file found.\n");
    return false;
}

bool contact_file_save_backup(const ContactList *list)
{
    rotate_backups();
    return contact_file_save(list, "contacts.dat");
}