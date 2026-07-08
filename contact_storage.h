// contact_storage.h
#ifndef CONTACT_STORAGE_H
#define CONTACT_STORAGE_H

#include "contact_dynamic.h" // Contact, ContactList

// Initialize the storage subsystem (open database, etc.). Call once at startup.
int storage_init(void);

// Shutdown the storage subsystem (close database, etc.).
void storage_shutdown(void);

// Load all contacts into the given list. Returns number of contacts or -1 on error.
int storage_load_all(ContactList *list);

// Save (insert) a new contact. Returns 0 on success, non-zero on error.
int storage_save_contact(const Contact *contact);
int storage_save_all(const ContactList *list);

// Update an existing contact.
int storage_update_contact(const Contact *contact);

// Delete a contact by ID.
int storage_delete_contact(int id);

// Search functions...
ContactList *storage_search_by_name(const char *pattern);
ContactList *storage_search_by_email(const char *pattern);
ContactList *storage_search_by_phone(const char *pattern);

#endif