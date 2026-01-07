/******************************************************************************
 * FILE: contact.c
 * DESCRIPTION: Contact management implementation
 * TODO: Implement all functions!
 * AUTHOR: Vedhashiva M T
 ******************************************************************************/

int next_contact_id = 1;

#include "contact.h"
#include "input.h" 
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>  

// ============================================================================
// CONTACT CREATION
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
// DISPLAY FUNCTIONS
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
// COMPARISON FUNCTIONS
// ============================================================================

bool contact_compare_name(const Contact *a, const Contact *b)
{
    // TODO: Compare by name (alphabetical)
    // Use strcmp()
    return strcmp(a->name, b->name) < 0;
}

bool contact_compare_id(const Contact *a, const Contact *b)
{
    // TODO: Compare by ID
    return a->id < b->id;
}

// ============================================================================
// VALIDATION FUNCTIONS
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