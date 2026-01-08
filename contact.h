/******************************************************************************
 * FILE: contact.h
 * DESCRIPTION: Contact structure and operations
 * RULE: One responsibility per function
 * AUTHOR: Vedhashiva M T
 ******************************************************************************/

// Version 1.0.0
#ifndef CONTACT_H
#define CONTACT_H

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

// ============================================================================
// CONSTANTS
// ============================================================================

#define MAX_NAME_LEN 50
#define MAX_PHONE_LEN 15
#define MAX_EMAIL_LEN 254
#define MAX_CONTACTS 100

// ============================================================================
// GLOBAL (for persistent ID)
// ============================================================================

extern int next_contact_id; // Defined in contact.c

// ============================================================================
// STRUCTURE
// ============================================================================

typedef struct
{
    char name[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
    char email[MAX_EMAIL_LEN];
    int id; // Unique identifier
} Contact;

// ============================================================================
// CORE FUNCTIONS
// ============================================================================

// Contact creation and validation
bool contact_create(Contact *contact, const char *name, const char *phone, const char *email);
bool contact_is_valid(const Contact *contact);

// Display
void contact_print(const Contact *contact);
void contact_print_header(void);
void contact_print_all(const Contact contacts[], int count);

// Comparison (for sorting/searching)
int contact_compare_id(const void *a, const void *b);
int contact_compare_name(const void *a, const void *b);

// Field validation
bool contact_validate_name(const char *name);
bool contact_validate_phone(const char *phone);
bool contact_validate_email(const char *email);

// Version 1.0.1
// SEARCH & HELPER FUNCTIONS

int contact_find_by_id(const Contact contacts[], int count, int id);
int contact_find_by_name(const Contact contacts[], int count,
                         const char *name, int results[]);
int contact_find_by_phone(const Contact contacts[], int count,
                          const char *phone, int results[]);
int contact_find_by_email(const Contact contacts[], int count,
                          const char *email, int results[]);

bool contact_name_matches(const Contact *contact, const char *search_term);
bool contact_phone_matches(const Contact *contact, const char *search_term);
bool contact_email_matches(const Contact *contact, const char *search_term);

bool contact_remove_by_index(Contact contacts[], int *count, int index);
void contact_shift_left(Contact contacts[], int *count, int start_index);
void extract_digits(char *dst, const char *src);

#endif // CONTACT_H