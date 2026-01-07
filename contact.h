/******************************************************************************
 * FILE: contact.h
 * DESCRIPTION: Contact structure and operations
 * RULE: One responsibility per function
 * AUTHOR: Vedhashiva M T
 ******************************************************************************/

#ifndef CONTACT_H
#define CONTACT_H

#include <stdbool.h>

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

extern int next_contact_id;  // Defined in contact.c


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
bool contact_compare_name(const Contact *a, const Contact *b);
bool contact_compare_id(const Contact *a, const Contact *b);

// Field validation
bool contact_validate_name(const char *name);
bool contact_validate_phone(const char *phone);
bool contact_validate_email(const char *email);

#endif // CONTACT_H