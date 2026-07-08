#include "contact_db.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>   // for malloc / free

/* ------------------------------------------------------------------ */
/*  db_open                                                           */
/* ------------------------------------------------------------------ */
int db_open(sqlite3 **db) {
    int rc = sqlite3_open(DB_FILENAME, db);
    if (rc != SQLITE_OK){
        fprintf(stderr, "Error : %s\n", sqlite3_errmsg(*db));
        // BUGFIX: was sqlite_close (missing '3'); changed to sqlite3_close
        sqlite3_close(*db); 
        return rc;
    }
    
    rc = db_create_table(*db);
    if (rc != SQLITE_OK){
        fprintf(stderr, "Error : Failed to create 'contacts' table\n");
        // BUGFIX: same typo
        sqlite3_close(*db);
        return rc;
    }
    return SQLITE_OK; 
}

/* ------------------------------------------------------------------ */
/*  db_close                                                          */
/* ------------------------------------------------------------------ */
void db_close(sqlite3 *db) {
    if (db) {
        sqlite3_close(db);
    }
}

/* ------------------------------------------------------------------ */
/*  db_create_table                                                   */
/* ------------------------------------------------------------------ */
int db_create_table(sqlite3 *db) {
    const char *sql = 
        "CREATE TABLE IF NOT EXISTS contacts ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "phone TEXT NOT NULL, "
        "email TEXT NOT NULL"
        ");";
    char *err_msg = NULL;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK){
        fprintf(stderr, "Error : %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }
    return SQLITE_OK; 
}

/* ------------------------------------------------------------------ */
/*  db_insert_contact                                                 */
/* ------------------------------------------------------------------ */
int db_insert_contact(sqlite3 *db, const Contact *contact) {
    if (contact == NULL) {
        return SQLITE_ERROR;
    }
    const char *sql = "INSERT INTO contacts (name, phone, email) VALUES (?1, ?2, ?3);";
    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK){
        fprintf(stderr, "Error : %s\n", sqlite3_errmsg(db));
        return rc;
    }

    // BUGFIX: index order was wrong - phone should be 2, email 3
    sqlite3_bind_text(stmt, 1, contact->name,  -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, contact->phone, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, contact->email, -1, SQLITE_STATIC);

    // BUGFIX: was sqlite_step (missing '3')
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE){
        fprintf(stderr, "Error : %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? SQLITE_OK : rc;
}

/* ------------------------------------------------------------------ */
/*  db_load_all_contacts                                              */
/* ------------------------------------------------------------------ */
int db_load_all_contacts(sqlite3 *db, ContactList *list) {
    const char* sql = "SELECT id, name, phone, email FROM contacts ORDER BY id";
    sqlite3_stmt *stmt = NULL;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK){
        fprintf(stderr, "Error : %s\n", sqlite3_errmsg(db));
        // BUGFIX: return -1 on error, not the SQLite code
        return -1;
    }

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW){   // BUGFIX: sqlite_step -> sqlite3_step
        Contact contact;
        contact.id = sqlite3_column_int(stmt, 0);
        
        // BUGFIX: buffer sizes were swapped (phone/email)
        strncpy(contact.name,  (const char *)sqlite3_column_text(stmt, 1), MAX_NAME_LEN - 1);
        strncpy(contact.phone, (const char *)sqlite3_column_text(stmt, 2), MAX_PHONE_LEN - 1);
        strncpy(contact.email, (const char *)sqlite3_column_text(stmt, 3), MAX_EMAIL_LEN - 1);

        contact.name[MAX_NAME_LEN - 1]   = '\0';
        contact.phone[MAX_PHONE_LEN - 1] = '\0';
        contact.email[MAX_EMAIL_LEN - 1] = '\0';

        if (!contact_list_add(list, &contact)){
            fprintf(stderr, "Error : Failed to add contact (id - %d) to list\n", contact.id);
            break;
        }
        count++;
    }

    sqlite3_finalize(stmt);
    return count; 
}

/* ------------------------------------------------------------------ */
/*  db_search_by_name                                                 */
/* ------------------------------------------------------------------ */
ContactList *db_search_by_name(sqlite3 *db, const char *pattern) {
    if (pattern == NULL) return NULL;
    
    char like_pattern[256];
    snprintf(like_pattern, sizeof(like_pattern), "%%%s%%", pattern);

    const char *sql = "SELECT id, name, phone, email FROM contacts WHERE name LIKE ?1 ORDER BY id";
    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK){
        fprintf(stderr, "Error : %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    sqlite3_bind_text(stmt, 1, like_pattern, -1, SQLITE_STATIC);

    ContactList *results = (ContactList *)malloc(sizeof(ContactList));
    if (results == NULL){
        sqlite3_finalize(stmt);
        return NULL;
    }
    contact_list_init(results, 10);

    while (sqlite3_step(stmt) == SQLITE_ROW){
        Contact contact;
        contact.id = sqlite3_column_int(stmt, 0);

        // BUGFIX: column order and buffer sizes fixed
        strncpy(contact.name,  (const char *)sqlite3_column_text(stmt, 1), MAX_NAME_LEN - 1);
        strncpy(contact.phone, (const char *)sqlite3_column_text(stmt, 2), MAX_PHONE_LEN - 1);
        strncpy(contact.email, (const char *)sqlite3_column_text(stmt, 3), MAX_EMAIL_LEN - 1);

        contact.name[MAX_NAME_LEN - 1]   = '\0';
        contact.phone[MAX_PHONE_LEN - 1] = '\0';
        contact.email[MAX_EMAIL_LEN - 1] = '\0';

        if (!contact_list_add(results, &contact)){
            fprintf(stderr, "Error : Failed to add contact (id - %d) to list\n", contact.id);
            contact_list_free(results);
            free(results);
            sqlite3_finalize(stmt);
            // BUGFIX: must return after cleanup to avoid using freed memory
            return NULL;
        }
    }
    sqlite3_finalize(stmt);
    return results; 
}

/* ------------------------------------------------------------------ */
/*  db_search_by_email                                                */
/* ------------------------------------------------------------------ */
ContactList *db_search_by_email(sqlite3 *db, const char *pattern) {
    if (pattern == NULL) return NULL;
    
    char like_pattern[256];
    snprintf(like_pattern, sizeof(like_pattern), "%%%s%%", pattern);

    // BUGFIX: SQL used ?2, must use ?1 if binding index 1
    const char *sql = "SELECT id, name, phone, email FROM contacts WHERE email LIKE ?1 ORDER BY id";
    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK){
        fprintf(stderr, "Error : %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    sqlite3_bind_text(stmt, 1, like_pattern, -1, SQLITE_STATIC);

    ContactList *results = (ContactList *)malloc(sizeof(ContactList));
    if (results == NULL){
        sqlite3_finalize(stmt);
        return NULL;
    }
    contact_list_init(results, 10);

    while (sqlite3_step(stmt) == SQLITE_ROW){
        Contact contact;
        contact.id = sqlite3_column_int(stmt, 0);

        strncpy(contact.name,  (const char *)sqlite3_column_text(stmt, 1), MAX_NAME_LEN - 1);
        strncpy(contact.phone, (const char *)sqlite3_column_text(stmt, 2), MAX_PHONE_LEN - 1);
        strncpy(contact.email, (const char *)sqlite3_column_text(stmt, 3), MAX_EMAIL_LEN - 1);

        contact.name[MAX_NAME_LEN - 1]   = '\0';
        contact.phone[MAX_PHONE_LEN - 1] = '\0';
        contact.email[MAX_EMAIL_LEN - 1] = '\0';

        if (!contact_list_add(results, &contact)){
            fprintf(stderr, "Error : Failed to add contact (id - %d) to list\n", contact.id);
            contact_list_free(results);
            free(results);
            sqlite3_finalize(stmt);
            return NULL;   // BUGFIX: missing return
        }
    }
    sqlite3_finalize(stmt);
    return results; 
}

/* ------------------------------------------------------------------ */
/*  db_search_by_phone                                                */
/* ------------------------------------------------------------------ */
ContactList *db_search_by_phone(sqlite3 *db, const char *pattern) {
    if (pattern == NULL) return NULL;
    
    char like_pattern[256];
    snprintf(like_pattern, sizeof(like_pattern), "%%%s%%", pattern);

    // BUGFIX: SQL used ?3, must be ?1 if binding index 1
    const char *sql = "SELECT id, name, phone, email FROM contacts WHERE phone LIKE ?1 ORDER BY id";
    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK){
        fprintf(stderr, "Error : %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    sqlite3_bind_text(stmt, 1, like_pattern, -1, SQLITE_STATIC);

    ContactList *results = (ContactList *)malloc(sizeof(ContactList));
    if (results == NULL){
        sqlite3_finalize(stmt);
        return NULL;
    }
    contact_list_init(results, 10);

    while (sqlite3_step(stmt) == SQLITE_ROW){
        Contact contact;
        contact.id = sqlite3_column_int(stmt, 0);

        strncpy(contact.name,  (const char *)sqlite3_column_text(stmt, 1), MAX_NAME_LEN - 1);
        strncpy(contact.phone, (const char *)sqlite3_column_text(stmt, 2), MAX_PHONE_LEN - 1);
        strncpy(contact.email, (const char *)sqlite3_column_text(stmt, 3), MAX_EMAIL_LEN - 1);

        contact.name[MAX_NAME_LEN - 1]   = '\0';
        contact.phone[MAX_PHONE_LEN - 1] = '\0';
        contact.email[MAX_EMAIL_LEN - 1] = '\0';

        if (!contact_list_add(results, &contact)){
            fprintf(stderr, "Error : Failed to add contact (id - %d) to list\n", contact.id);
            contact_list_free(results);
            free(results);
            sqlite3_finalize(stmt);
            return NULL;   // BUGFIX: missing return
        }
    }
    sqlite3_finalize(stmt);
    return results; 
}

/* ------------------------------------------------------------------ */
/*  db_update_contact                                                 */
/* ------------------------------------------------------------------ */
int db_update_contact(sqlite3 *db, const Contact *contact) {
    if (contact == NULL) return SQLITE_ERROR;
    
    const char *sql = "UPDATE contacts SET name=?1, phone=?2, email=?3 WHERE id=?4";
    sqlite3_stmt *stmt = NULL;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Update prepare error: %s\n", sqlite3_errmsg(db));
        return SQLITE_ERROR;
    }
    
    sqlite3_bind_text(stmt, 1, contact->name,  -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, contact->phone, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, contact->email, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt,  4, contact->id);
    
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Update error: %s\n", sqlite3_errmsg(db));
    }
    
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? SQLITE_OK : rc;
}

/* ------------------------------------------------------------------ */
/*  db_delete_contact                                                 */
/* ------------------------------------------------------------------ */
int db_delete_contact(sqlite3 *db, int id) {
    const char *sql = "DELETE FROM contacts WHERE id = ?1";
    sqlite3_stmt *stmt = NULL;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Delete prepare error: %s\n", sqlite3_errmsg(db));
        return SQLITE_ERROR;
    }
    
    sqlite3_bind_int(stmt, 1, id);
    
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Delete error: %s\n", sqlite3_errmsg(db));
    }
    
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? SQLITE_OK : rc;
}