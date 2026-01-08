/******************************************************************************
 * FILE: contact.c
 * DESCRIPTION: Contact management implementation
 * AUTHOR: Vedhashiva M T
 * VERSION: 1.0.1
 ******************************************************************************/

// Version 1.0.0
int next_contact_id = 1;

#include "contact.h"
#include "input.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

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
            // Could add more: c == ',' for "Smith, Jr."?
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
{ // 254 because of RFC rules states that emails can only have max 254 chars
    if (!email || strlen(email) > 254)
        return false;

    // Find @
    char *at = strchr(email, '@');
    // Invalid for - No @, Multiple @, @ at start
    if (!at || at == email || strchr(at + 1, '@'))
        return false;

    // Split into local and domain
    // before @ -> local part, after @ -> domain
    size_t local_len = at - email; // pointer arithmetic
    size_t domain_len = strlen(at + 1);

    // Validate local part (before @)
    // Rules : Can contain numbers, Letters and spec chars
    for (size_t i = 0; i < local_len; i++)
    {
        unsigned char c = email[i];
        // Allowed -> 0-9, a-z(both cases), and these spec chars
        if (!isalnum(c) && strchr(".!#$%&'*+/=?^_`{|}~-", c) == NULL)
        {
            return false;
        }
        // Additional: Can't start or end with dot, can't have consecutive dots
        if (c == '.' && (i == 0 || i == local_len - 1 || email[i + 1] == '.'))
        {
            return false;
        }
    }

    // Validate domain part (after @)
    for (size_t i = 0; i < domain_len; i++)
    {
        unsigned char c = at[1 + i]; // at+1 is start of domain
        if (!isalnum(c) && c != '-' && c != '.')
        {
            return false;
        }
        // Domain rules: no leading/trailing hyphens or dots
        if (c == '.' || c == '-')
        {
            if (i == 0 || i == domain_len - 1)
                return false;
            if (c == '.' && at[1 + i + 1] == '.')
                return false; // No consecutive dots
        }
    }

    // Check TLD
    char *dot = strrchr(at + 1, '.');
    if (!dot || dot == at + 1 || strlen(dot + 1) < 2)
        return false;

    // TLD should be letters only (no digits/hyphens)
    for (int i = 0; dot[i + 1]; i++)
    {
        if (!isalpha(dot[i + 1]))
            return false;
    }

    return true;
}

// Version 1.0.1
// ============================================================================
// SEARCH HELPER FUNCTIONS - DONE
// ============================================================================

int contact_find_by_id(const Contact contacts[], int count, int id)
{
    // TODO: Loop through contacts
    // TODO: Compare each contact's id with parameter id
    // TODO: Return index if found
    // TODO: Return -1 if not found
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
    // TODO: Initialize found_name_count = 0
    // TODO: Loop through all contacts
    // TODO: For each contact, check if name matches (use contact_name_matches)
    // TODO: If matches, store index in results[found_name_count] and increment
    // TODO: Return found_count
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
    // TODO: Similar to find_by_name but for phone
    // TODO: Consider normalizing phone numbers first
    // TODO: Use found_phone_count for comparison
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
    // TODO: Similar to find_by_name but for email
    // TODO: Use contact_email_matches for comparison
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