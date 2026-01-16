/******************************************************************************
 * FILE: contact_dynamic.h
 * DESCRIPTION: Contact structure and operations - DYNAMIC VERSION
 * RULE: One responsibility per function
 * AUTHOR: Vedhashiva M T
 ******************************************************************************/

// Version 1.2.0 - DYNAMIC ARRAYS
#ifndef CONTACT_DYNAMIC_H
#define CONTACT_DYNAMIC_H

#include <stdbool.h>

// ============================================================================
// CONSTANTS
// ============================================================================

#define MAX_NAME_LEN 50
#define MAX_PHONE_LEN 15
#define MAX_EMAIL_LEN 254
// NO MAX_CONTACTS! Dynamic grows as needed.

// ============================================================================
// STRUCTURES
// ============================================================================

typedef struct
{
    char name[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
    char email[MAX_EMAIL_LEN];
    int id;
} Contact;

typedef struct
{
    Contact *data; // Dynamic array
    int size;      // Contacts stored
    int capacity;  // Memory allocated
} ContactList;

// ============================================================================
// GLOBALS
// ============================================================================

extern ContactList contact_list; // Main dynamic list
extern int next_contact_id;

// ============================================================================
// DYNAMIC ARRAY MANAGEMENT (NEW)
// ============================================================================

// Lifecycle
bool contact_list_init(ContactList *list, int initial_capacity);
bool contact_list_ensure_capacity(ContactList *list, int min_capacity);
void contact_list_free(ContactList *list);

// CRUD for ContactList
bool contact_list_add(ContactList *list, const Contact *contact);
bool contact_list_remove_by_id(ContactList *list, int id);
bool contact_list_update_by_id(ContactList *list, int id, const Contact *updates);
bool contact_list_remove_by_index(ContactList *list, int index);

// Search in ContactList (NEW)
int contact_find_by_id_in_list(const ContactList *list, int id);
int contact_find_by_name_in_list(const ContactList *list, const char *name, int results[]);
int contact_find_by_phone_in_list(const ContactList *list, const char *phone, int results[]);
int contact_find_by_email_in_list(const ContactList *list, const char *email, int results[]);

// ============================================================================
// CORE FUNCTIONS (UNCHANGED from static)
// ============================================================================

// Contact creation and validation
bool contact_create(Contact *contact, const char *name, const char *phone, const char *email);
bool contact_is_valid(const Contact *contact);

// Display
void contact_print(const Contact *contact);
void contact_print_header(void);
void contact_print_all(const Contact contacts[], int count); // UNCHANGED

// Comparison (for sorting)
int contact_compare_id(const void *a, const void *b);
int contact_compare_name(const void *a, const void *b);

// Field validation
bool contact_validate_name(const char *name);
bool contact_validate_phone(const char *phone);
bool contact_validate_email(const char *email);

// ============================================================================
// HELPER FUNCTIONS (UNCHANGED)
// ============================================================================

bool contact_name_matches(const Contact *contact, const char *search_term);
bool contact_phone_matches(const Contact *contact, const char *search_term);
bool contact_email_matches(const Contact *contact, const char *search_term);
void extract_digits(char *dst, const char *src);

// ============================================================================
// LEGACY FUNCTIONS (Keep for now, phase out later)
// ============================================================================

// These work on raw arrays (for transition period)
int contact_find_by_id(const Contact contacts[], int count, int id);
int contact_find_by_name(const Contact contacts[], int count, const char *name, int results[]);
int contact_find_by_phone(const Contact contacts[], int count, const char *phone, int results[]);
int contact_find_by_email(const Contact contacts[], int count, const char *email, int results[]);
bool contact_remove_by_index(Contact contacts[], int *count, int index);
void contact_shift_left(Contact contacts[], int *count, int start_index);

#endif // CONTACT_DYNAMIC_H