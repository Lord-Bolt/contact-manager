#include "contact_storage.h"
#include "contact_db.h"
#include "contact_dynamic.h"
#include <stdio.h>
#include <stdbool.h>

/* ------------------------------------------------------------------ */
/*  Internal database handle                                           */
/* ------------------------------------------------------------------ */
static sqlite3 *db = NULL; // single, module‑private database connection
static bool use_database = true;
/* ------------------------------------------------------------------ */
/*  storage_init                                                       */
/* ------------------------------------------------------------------ */
int storage_init(void)
{
    if (db_open(&db) != SQLITE_OK)
    {
        fprintf(stderr, "Storage Error : Could not open database");
        return -1;
    }
    return 0; // placeholder
}

/* ------------------------------------------------------------------ */
/*  storage_shutdown                                                   */
/* ------------------------------------------------------------------ */
void storage_shutdown(void)
{
    // TODO: call db_close(db)
    // Set db = NULL so we don't accidentally use it later
    db_close(db);
    db = NULL;
}

/* ------------------------------------------------------------------ */
/*  storage_load_all                                                   */
/* ------------------------------------------------------------------ */
int storage_load_all(ContactList *list)
{
    // TODO: call db_load_all_contacts(db, list)
    // Return the number of contacts loaded, or -1 on error
    return db_load_all_contacts(db, list);
}

/* ------------------------------------------------------------------ */
/*  storage_save_contact                                               */
/* ------------------------------------------------------------------ */
int storage_save_contact(const Contact *contact)
{
    // TODO: validate contact pointer (NULL check)
    // Call db_insert_contact(db, contact)
    // Return 0 on success, non-zero on error
    if (contact->name == NULL || !contact_validate_name(contact->name))
    {
        return -1;
    }
    if (contact->email == NULL || !contact_validate_email(contact->email))
    {
        return -1;
    }
    if (contact->phone == NULL || !contact_validate_phone(contact->phone))
    {
        return -1;
    }

    if (db_insert_contact(db, contact) != SQLITE_OK)
    {
        return -1;
    }

    return 0;
}

int storage_save_all(const ContactList *list)
{
    if (list == NULL || db == NULL)
        return -1;

    const char *sql =
        "INSERT OR REPLACE INTO contacts (id, name, phone, email) "
        "VALUES (?1, ?2, ?3, ?4);";
    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "storage_save_all prepare error: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    int saved = 0;
    for (int i = 0; i < list->size; i++)
    {
        const Contact *c = &list->data[i];
        sqlite3_bind_int(stmt, 1, c->id);
        sqlite3_bind_text(stmt, 2, c->name, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, c->phone, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, c->email, -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE)
        {
            saved++;
        }
        else
        {
            fprintf(stderr, "Failed to save contact ID %d: %s\n", c->id, sqlite3_errmsg(db));
        }
        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    return saved;
}

/* ------------------------------------------------------------------ */
/*  storage_update_contact                                             */
/* ------------------------------------------------------------------ */
int storage_update_contact(const Contact *contact)
{
    // TODO: validate contact pointer
    // Call db_update_contact(db, contact)
    // Return 0 on success, non-zero on error
    if (contact->name == NULL || !contact_validate_name(contact->name))
    {
        return -1;
    }
    if (contact->email == NULL || !contact_validate_email(contact->email))
    {
        return -1;
    }
    if (contact->phone == NULL || !contact_validate_phone(contact->phone))
    {
        return -1;
    }

    if (db_update_contact(db, contact) != SQLITE_OK)
    {
        return -1;
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/*  storage_delete_contact                                             */
/* ------------------------------------------------------------------ */
int storage_delete_contact(int id)
{
    // TODO: Call db_delete_contact(db, id)
    // Return 0 on success, non-zero on error
    if (db_delete_contact(db, id) != SQLITE_OK)
    {
        fprintf(stderr, "Storage Error : %s\n", sqlite3_errmsg(db));
        return -1;
    }
    return 0; // placeholder
}

/* ------------------------------------------------------------------ */
/*  storage_search_by_name                                             */
/* ------------------------------------------------------------------ */
ContactList *storage_search_by_name(const char *pattern)
{
    if (db == NULL || pattern == NULL)
        return NULL;
    return db_search_by_name(db, pattern);
}

/* ------------------------------------------------------------------ */
/*  storage_search_by_email                                            */
/* ------------------------------------------------------------------ */
ContactList *storage_search_by_email(const char *pattern)
{
    if (db == NULL || pattern == NULL)
        return NULL;
    return db_search_by_email(db, pattern);
}

/* ------------------------------------------------------------------ */
/*  storage_search_by_phone                                            */
/* ------------------------------------------------------------------ */
ContactList *storage_search_by_phone(const char *pattern)
{
    if (db == NULL || pattern == NULL)
        return NULL;
    return db_search_by_phone(db, pattern);
}
