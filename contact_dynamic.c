/******************************************************************************
 * FILE: contact.c
 * DESCRIPTION: Contact management implementation
 * AUTHOR: Vedhashiva M T
 * VERSION: 1.0.1
 ******************************************************************************/

#include "contact_dynamic.h"
#include "input.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h> // For malloc/realloc/free

// ============================================================================
// GLOBAL VARIABLES - FOR DYNAMIC
// ============================================================================
int next_contact_id = 1;
ContactList contact_list = {NULL, 0, 0};

// ============================================================================
// DYNAMIC ARRAY IMPLEMENTATION
// ============================================================================
bool contact_list_init(ContactList *list, int initial_capacity)
{
    if (list == NULL)
    {
        return false;
    }

    if (initial_capacity <= 0)
    {
        initial_capacity = 4; // Handle Invalid capacity
    }

    // Allocate Memory
    list->data = malloc(initial_capacity * sizeof(Contact));
    if (list->data == NULL)
    {
        return false; // Memory Did Not Initialize
    }

    list->size = 0;
    list->capacity = initial_capacity;
    return true; // Successful
}

bool contact_list_ensure_capacity(ContactList *list, int min_capacity)
{
    if (list == NULL)
    {
        return false;
    }

    // Check for if capacity is already enough -> early return
    if (min_capacity <= list->capacity)
    {
        return true;
    }

    // calc new capacity -> Atleast Double
    int new_capacity = list->capacity * 2;
    if (new_capacity < min_capacity)
    {
        new_capacity = min_capacity; // Double ensuring
    }

    // VERY IMPORTANT -> temp variable for realloc -> prevents mem loss
    Contact *new_data = realloc(list->data, new_capacity * sizeof(Contact));
    if (new_data == NULL)
    {
        return false; // Fail
    }

    // Success
    list->data = new_data;
    list->capacity = new_capacity;
    return true;
}

void contact_list_free(ContactList *list)
{
    if (list == NULL)
    {
        return;
    }

    free(list->data);
    list->data = NULL;
    list->capacity = 0;
    list->size = 0;
}

bool contact_list_add(ContactList *list, const Contact *contact)
{
    if (list == NULL || contact == NULL)
    {
        return false;
    }

    if (list->size >= list->capacity)
    { // Ensuring Enough Size
        if (!contact_list_ensure_capacity(list, list->capacity + 1))
        {
            return false; // Out of Memory -> will cause mem leak if followed through
        }
    }

    list->data[list->size] = *contact; // list->size is index
    list->size++;                      // Index increment
    return true;                       // Sucess
}

bool contact_list_remove_by_id(ContactList *list, int id)
{
    if (list == NULL)
    {
        return false;
    }

    int index = contact_find_by_id_in_list(list, id);
    if (index == -1)
    {
        return false;
    }

    // Shift Everything after removed contact by 1
    for (int i = index; i < list->size - 1; i++)
    {
        list->data[i] = list->data[i + 1];
    }

    list->size--; // Decrement
    return true;
}

bool contact_list_remove_by_index(ContactList* list, int index) {
    if (list == NULL || index < 0 || index >= list->size) 
        return false;
    
    for (int i = index; i < list->size - 1; i++) {
        list->data[i] = list->data[i + 1];
    }
    
    list->size--;
    return true;
}
// ============================================================================
// CONTACT CREATION - DONE
// ============================================================================

bool contact_create(Contact *contact, const char *name, const char *phone, const char *email)
{
    if (contact == NULL || name == NULL || phone == NULL || email == NULL)
    {
        return false;
    }

    if (contact_validate_name(name) && contact_validate_email(email) && contact_validate_phone(phone))
    {

        strncpy(contact->name, name, MAX_NAME_LEN - 1);
        contact->name[MAX_NAME_LEN - 1] = '\0';

        strncpy(contact->email, email, MAX_EMAIL_LEN - 1);
        contact->email[MAX_EMAIL_LEN - 1] = '\0';

        strncpy(contact->phone, phone, MAX_PHONE_LEN - 1);
        contact->phone[MAX_PHONE_LEN - 1] = '\0';

        contact->id = next_contact_id;
        next_contact_id++;
        return true;
    }
    return false;
}

bool contact_is_valid(const Contact *contact)
{
    if (contact == NULL)
    {
        return false; // Always NuLL check
    }

    if (contact_validate_email(contact->email) && contact_validate_name(contact->name) && contact_validate_phone(contact->phone))
    {
        return true;
    }
    return false;
}

// ============================================================================
// DISPLAY FUNCTIONS - DONE
// ============================================================================

void contact_print(const Contact *contact)
{
    // TODO: Print contact in nice format
    // Example: "ID: 1 | Name: John Doe | Phone: 555-1234 | Email: john@email.com"
    printf("\n| %-4d | %-20s | %-15s | %-30s |",
           contact->id,     // Left-aligned, 4 chars wide
           contact->name,   // Left-aligned, 20 chars wide
           contact->phone,  // Left-aligned, 15 chars wide
           contact->email); // Left-aligned, 30 chars wide
    printf("\n|------|----------------------|-----------------|--------------------------------|");
}

void contact_print_header(void)
{
    printf("\n|------|----------------------|-----------------|--------------------------------|");
    printf("\n|  ID  |        Name          |     Phone      |            Email              |");
    printf("\n|------|----------------------|-----------------|--------------------------------|");
}

void contact_print_all(const Contact contacts[], int count)
{
    if (count == 0)
    {
        printf("No contacts found.\n");
        return;
    }

    contact_print_header();
    for (int i = 0; i < count; i++)
    {
        contact_print(&contacts[i]);
    }
    printf("\n"); // Final newline
}

// ============================================================================
// COMPARISON FUNCTIONS - DONE
// ============================================================================

int contact_compare_id(const void *a, const void *b)
{
    const Contact *contact_a = (const Contact *)a;
    const Contact *contact_b = (const Contact *)b;
    return contact_a->id - contact_b->id; // Ascending ID
}

int contact_compare_name(const void *a, const void *b)
{
    const Contact *contact_a = (const Contact *)a;
    const Contact *contact_b = (const Contact *)b;
    return strcmp(contact_a->name, contact_b->name); // Alphabetical
}

// ============================================================================
// VALIDATION FUNCTIONS - DONE
// ============================================================================

bool contact_validate_name(const char *name)
{
    // Basic checks
    if (name == NULL || is_whitespace(name))
        return false;

    size_t len = strlen(name);
    if (len < 2 || len >= MAX_NAME_LEN)
        return false;

    int has_seen_letter = 0;

    // Loop through string
    for (size_t i = 0; name[i] != '\0'; i++)
    {
        unsigned char c = name[i]; // unsigned for isalpha()

        if (isalpha(c))
        {
            has_seen_letter = 1;
        }
        else if (isdigit(c))
        {
            // Allow digits only after we've seen a letter
            // This allows "John123" but not "123John"
            if (!has_seen_letter)
            {
                return false;
            }
        }
        else if (c == ' ' || c == '-' || c == '.' || c == '\'')
        {
            // Allowed punctuation
        }
        else
        {
            // Any other character is invalid
            return false;
        }
    }

    return (has_seen_letter == 1);
}

bool contact_validate_phone(const char *phone)
{ // TODO: Improve this!
    // Allow: +, (, ), -, spaces
    // Reject: letters, too many non-digits
    // Format checking: (555) 123-4567 vs 555-123-4567

    if (!phone || is_whitespace(phone))
    {
        return false; // Checking for NULL and EMPTY STRING of space
    }

    int digit_count = 0, plus_count = 0, parenthesis_count = 0;
    for (int i = 0; phone[i]; i++)
    {
        unsigned char c = phone[i];
        if (isdigit(c))
        {
            digit_count++; // Counting Digits
        }

        else if (c == '+')
        {
            plus_count++;
            if (i != 0 || plus_count > 1)
            { // Plus can only be at the start and only one '+' is allowed
                return false;
            }
        }

        else if (c == '(' || c == ')')
        {
            if (c == '(')
            {
                parenthesis_count++;
            }
            else
            {
                parenthesis_count--;
                if (c == ')' && phone[i + 1] != '\0' && phone[i + 1] != ' ' && phone[i + 1] != '-' && !isdigit(phone[i + 1]))
                {
                    // Only reject if next character is invalid
                    return false;
                }
            }

            if (parenthesis_count < 0)
            { // No ')' appearing before '('
                return false;
            }
        }

        else if (c == '-' || c == ' ' || c == '.')
        {
            if (i == 0 || phone[i + 1] == '\0')
            { // Can't start or end with separator
                return false;
            }

            // no two separators in a row
            if (phone[i + 1] == '-' || phone[i + 1] == ' ' || phone[i + 1] == '.')
            {
                return false;
            }
        }
    }

    if (parenthesis_count != 0)
    {
        return false;
    }

    return (digit_count >= 7 && digit_count <= 15);
}

bool contact_validate_email(const char *email)
{
    if (email == NULL)
        return false;

    // RFC 5321: max 320 chars total (64 local + 1 @ + 255 domain)
    size_t len = strlen(email);
    if (len == 0 || len > 320)
        return false;

    // Must contain exactly one @
    char *at = strchr(email, '@');
    if (at == NULL)
        return false;

    // Check for multiple @
    if (strchr(at + 1, '@') != NULL)
        return false;

    // Extract local part (before @)
    size_t local_len = at - email;
    if (local_len == 0 || local_len > 64)
        return false; // RFC 5321: 64 octets max

    // Extract domain part (after @)
    size_t domain_len = len - local_len - 1;
    if (domain_len == 0 || domain_len > 255)
        return false; // RFC 1035: 255 chars max

    // Basic character validation (simplified for now)
    for (size_t i = 0; i < local_len; i++)
    {
        char c = email[i];
        if (!isalnum(c) && c != '.' && c != '!' && c != '#' && c != '$' &&
            c != '%' && c != '&' && c != '\'' && c != '*' && c != '+' &&
            c != '-' && c != '/' && c != '=' && c != '?' && c != '^' &&
            c != '_' && c != '`' && c != '{' && c != '|' && c != '}' && c != '~')
        {
            return false;
        }
    }

    // Domain validation (simplified)
    const char *domain = at + 1;
    if (domain[0] == '.' || domain[domain_len - 1] == '.')
        return false;
    if (strstr(domain, "..") != NULL)
        return false;

    // Must contain at least one dot in domain
    if (strchr(domain, '.') == NULL)
        return false;

    return true;
}

// Version 1.0.1
// ============================================================================
// SEARCH HELPER FUNCTIONS - DONE
// ============================================================================

// FOR STATIC -> keeping for compatibility
int contact_find_by_id(const Contact contacts[], int count, int id)
{
    if (contacts == NULL || count < 1)
    {
        return -1;
    }

    for (int i = 0; i < count; i++)
    {
        if (contacts[i].id == id)
        {
            return i;
        }
    }

    return -1;
}

int contact_find_by_name(const Contact contacts[], int count, const char *name, int results[])
{
    int found_name_count = 0;

    if (name == NULL || contacts == NULL || results == NULL)
    {
        return -1;
    }

    if (name[0] == '\0') // Must come after NULL check
    {
        return 0;
    }

    for (int i = 0; i < count; i++)
    {
        if (contact_name_matches(&contacts[i], name))
        {
            results[found_name_count++] = i;
        }
    }

    return found_name_count;
}

int contact_find_by_phone(const Contact contacts[], int count, const char *phone, int results[])
{
    int found_phone_count = 0;

    if (phone == NULL || contacts == NULL || results == NULL)
    {
        return -1;
    }

    if (phone[0] == '\0')
    {
        return 0;
    }
    for (int i = 0; i < count; i++)
    {
        if (contact_phone_matches(&contacts[i], phone))
        {
            results[found_phone_count++] = i;
        }
    }

    return found_phone_count;
}

int contact_find_by_email(const Contact contacts[], int count, const char *email, int results[])
{
    int found_email_count = 0;

    if (email == NULL || contacts == NULL || results == NULL)
    {
        return -1;
    }

    if (email[0] == '\0')
    {
        return 0;
    }

    for (int i = 0; i < count; i++)
    {
        if (contact_email_matches(&contacts[i], email))
        {
            results[found_email_count++] = i;
        }
    }

    return found_email_count;
}

// FOR DYNAMIC -> New
int contact_find_by_id_in_list(const ContactList* list, int id)
{
    if (list == NULL)
    {
        return -1;
    }
    for (int i = 0; i < list->size; i++)
    {
        if (list->data[i].id == id)
            return i;
    }
    return -1;
}

int contact_find_by_name_in_list(const ContactList* list, const char *name, int results[])
{
    int found_name_count = 0;

    if (name == NULL || list == NULL || results == NULL)
    {
        return -1;
    }

    if (name[0] == '\0') // Must come after NULL check
    {
        return 0;
    }

    for (int i = 0; i < list->size; i++)
    {
        if (contact_name_matches(&list->data[i], name))
        {
            results[found_name_count++] = i;
        }
    }

    return found_name_count;
}

int contact_find_by_phone_in_list(const ContactList* list, const char *phone, int results[])
{
    int found_phone_count = 0;

    if (phone == NULL || list == NULL || results == NULL)
    {
        return -1;
    }

    if (phone[0] == '\0')
    {
        return 0;
    }
    for (int i = 0; i < list->size; i++)
    {
        if (contact_phone_matches(&list->data[i], phone))
        {
            results[found_phone_count++] = i;
        }
    }

    return found_phone_count;
}

int contact_find_by_email_in_list(const ContactList* list, const char *email, int results[])
{
    int found_email_count = 0;

    if (email == NULL || list == NULL || results == NULL)
    {
        return -1;
    }

    if (email[0] == '\0')
    {
        return 0;
    }

    for (int i = 0; i < list->size; i++)
    {
        if (contact_email_matches(&list->data[i], email))
        {
            results[found_email_count++] = i;
        }
    }

    return found_email_count;
}

// ============================================================================
// MATCHING FUNCTIONS  - DONE
// ============================================================================

bool contact_name_matches(const Contact *contact, const char *search_term)
{
    // TODO: Case-insensitive partial match
    // TODO: Use strstr() with tolower()
    // TODO: Return true if search_term is found in contact->name
    if (search_term[0] == '\0')
    {
        return false;
    }

    const char *name = contact->name;
    const char *search = search_term;

    // Basically strcasestr
    for (int i = 0; name[i]; i++)
    {
        int j = 0;
        while (name[i + j] && search[j] && tolower(name[i + j]) == tolower(search[j]))
        { // i+j Might look weird but it's to find chars when they're in the middle of a name
            j++;
        }

        if (!search[j])
        {
            return true;
        }
    }

    return false;
}

bool contact_phone_matches(const Contact *c, const char *search)
{
    char norm_phone[20], norm_search[20]; // MAX_PHONE_LEN is only 15, taking 20 just to be safe
    extract_digits(norm_phone, c->phone);
    extract_digits(norm_search, search); // using extract_digits
    return strstr(norm_phone, norm_search) != NULL;
}

bool contact_email_matches(const Contact *contact, const char *search_term)
{
    // TODO: Case-insensitive email matching
    // TODO: strcasestr() or similar
    // TODO: Return true if search_term found in contact->email

    if (search_term[0] == '\0')
    {
        return false;
    }

    const char *email = contact->email;
    const char *search = search_term;

    // Basically strcasestr
    for (int i = 0; email[i]; i++)
    {
        int j = 0;
        while (email[i + j] && search[j] && tolower(email[i + j]) == tolower(search[j]))
        { // i+j Might look weird but it's to find chars when they're in the middle of a name
            j++;
        }

        if (!search[j])
        {
            return true;
        }
    }

    return false;
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

bool contact_remove_by_index(Contact contacts[], int *count, int index)
{
    // TODO: Validate index is within 0 <= index < *count
    // TODO: If invalid, return false
    // TODO: Call contact_shift_left to remove the contact
    // TODO: Decrement *count
    // TODO: Return true on success
    if (contacts == NULL || count == NULL || *count < 0 || index < 0 || index >= *count)
    {
        return false;
    }

    contact_shift_left(contacts, count, index);
    return true;
}

void contact_shift_left(Contact contacts[], int *count, int start_index)
{
    // TODO: Shift all contacts left starting from start_index
    // TODO: Loop: contacts[i] = contacts[i+1] for i = start_index to *count-2
    // TODO: This "removes" the contact at start_index by overwriting it
    if (contacts == NULL || count == NULL || *count <= 0 || start_index < 0 || start_index >= *count)
    {
        return;
    }

    for (int i = start_index; i < *count - 1; i++)
    {
        contacts[i] = contacts[i + 1];
    }
    (*count)--;
}

void extract_digits(char *dst, const char *src)
{
    int d = 0;
    for (int i = 0; src[i] && d < 19; i++)
    { // 19 for 20-char buffer
        if (isdigit((unsigned char)src[i]))
        {
            dst[d++] = src[i];
        }
    }
    dst[d] = '\0';
}