#ifndef CONTACT_DB_H
#define CONTACT_DB_H

#include "contact_dynamic.h"  // for Contact, ContactList
#include "sqlite3.h"

#define DB_FILENAME "contacts.db"

// Opens the database and ensures the contacts table exists.
// Returns SQLITE_OK on success, otherwise an error code.
int db_open(sqlite3 **db);

// Closes the database connection.
void db_close(sqlite3 *db);

// Creates the contacts table if it doesn't already exist.
int db_create_table(sqlite3 *db);

// Inserts a single contact. The id is assigned by SQLite.
// Returns SQLITE_OK on success.
int db_insert_contact(sqlite3 *db, const Contact *contact);

// Loads all contacts from the DB into the given ContactList.
// Returns the number of contacts loaded, or -1 on error.
int db_load_all_contacts(sqlite3 *db, ContactList *list);

// Searches for contacts by name (LIKE %pattern%).
// Returns a newly allocated ContactList (caller must free), or NULL on error.
ContactList *db_search_by_name(sqlite3 *db, const char *pattern);

ContactList *db_search_by_email(sqlite3 *db, const char *pattern);

ContactList *db_search_by_phone(sqlite3 *db, const char *pattern);

// Updates a contact by id.
// Returns SQLITE_OK on success.
int db_update_contact(sqlite3 *db, const Contact *contact);

// Deletes a contact by id.
// Returns SQLITE_OK on success.
int db_delete_contact(sqlite3 *db, int id);

#endif