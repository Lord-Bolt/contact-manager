/******************************************************************************
 * FILE: main.c
 * DESCRIPTION: Contact Manager - Main program with menu
 * AUTHOR: Vedhashiva M T
 * RULE: Use input.h for ALL user input!
 ******************************************************************************/

#include "contact.h"
#include "input.h" // YOUR LIBRARY!
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// ============================================================================
// GLOBAL DATA
// ============================================================================

Contact contacts[MAX_CONTACTS];
int contact_count = 0;

// ============================================================================
// FUNCTION PROTOTYPES
// ============================================================================

// Core operations
void add_contact(void);
void list_contacts(void);
void search_contacts(void);
void delete_contact(void);
void edit_contact(void);

// Helper functions
void find_contact_by_name(const char *name, int *indices, int *found_count);
void find_contact_by_phone(const char *phone, int *indices, int *found_count);
int find_contact_by_id(int id);
void save_contacts_to_file(const char *filename);
void load_contacts_from_file(const char *filename);

// UI functions
void show_menu(void);
void show_search_menu(void);
void show_contact_details(int index);
void clear_screen(void);

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main(void)
{
    printf("=== CONTACT MANAGER v1.0 ===\n");
    printf("Built with your input.h library!\n");

    // TODO: Load contacts from file (optional)
    // load_contacts_from_file("contacts.dat");

    int choice;

    do
    {
        show_menu();

        // Use YOUR library - consistent error handling!
        if (!get_int_range_prompt("Enter choice (1-7): ", 1, 7, &choice))
        {
            printf("\nInvalid input! Please enter a number 1-7.\n");
            pause_program("Press Enter to continue...");
            continue;
        }

        // Clear screen for better UX (optional)
        // clear_screen();

        switch (choice)
        {
        case 1:
            add_contact();
            break;
        case 2:
            list_contacts();
            break;
        case 3:
            search_contacts();
            break;
        case 4:
            delete_contact();
            break;
        case 5:
            edit_contact();
            break;
        case 6: // TODO: Save to file
            printf("Save feature coming soon!\n");
            pause_program("Press Enter to continue...");
            break;
        case 7:
            printf("\nExiting Contact Manager...\n");
            break;
        default:
            printf("Invalid choice!\n");
            break;
        }

    } while (choice != 7);

    // TODO: Save before exit
    // save_contacts_to_file("contacts.dat");

    pause_program("Press Enter to exit completely...");
    return 0;
}

// ============================================================================
// CORE OPERATIONS (IMPLEMENT THESE!)
// ============================================================================

// DONE
void add_contact(void)
{

    if (contact_count >= MAX_CONTACTS)
    {
        printf("Directory is Full. Cannot Add Any More Contacts. Please Delete Contacts If You Wish To Add More.");
        pause_program("\nPress Enter to return to menu...");
        return;
    }

    printf("\n=== ADD NEW CONTACT ===\n");

    Contact new_contact;
    char name[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
    char email[MAX_EMAIL_LEN];

    // TODO: Get name using get_string_prompt()
    // TODO: Validate with contact_validate_name()
    // TODO: If invalid, show error and return
    if (!get_string_prompt("Name : ", name, MAX_NAME_LEN))
    {
        printf("Failed to read Name Entered. Please Try Again.\n");
        pause_program("\nPress Enter to return to menu...");
        return;
    }
    if (!contact_validate_name(name))
    {
        printf("\nInvalid Name Entered. Please Try Again.");
        printf("\nName must be 2-50 characters, must contain letters, and should not contain any special characters.");
        printf("\nYou entered: '%s'\n", name);
        pause_program("\nPress Enter to return to menu...");
        return;
    }

    // TODO: Get phone using get_string_prompt()
    // TODO: Validate with contact_validate_phone()
    if (!get_string_prompt("Phone Number : ", phone, MAX_PHONE_LEN))
    {
        printf("Failed to read Phone Number. Please Try Again\n");
        pause_program("\nPress Enter to return to menu...");
        return;
    }
    if (!contact_validate_phone(phone))
    {
        printf("\nInvalid Phone Number Entered. Please Try Again.");
        printf("\nPhone Number must be between 7 - 15 characters, should have full parenthesis, should not contain any letters and only one '+' is allowed and only at the starting of the respective Phone Number");
        printf("\nYou entered: '%s'\n", phone);
        pause_program("\nPress Enter to return to menu...");
        return;
    }

    // TODO: Get email using get_string_prompt()
    // TODO: Validate with contact_validate_email()
    if (!get_string_prompt("E-mail : ", email, MAX_EMAIL_LEN))
    {
        printf("Failed to read E-mail. Please Try Again.\n");
        pause_program("\nPress Enter to return to menu...");
        return;
    }
    if (!contact_validate_email(email))
    {
        printf("\nInvalid E-mail Entered. Please Try Again.");
        printf("\nE-mail must follow RFC 5321 Regulations - Should not have multiple @, multiple '.', follow-up '.' or @");
        printf("\nYou entered: '%s'\n", email);
        pause_program("\nPress Enter to return to menu...");
        return;
    }

    // TODO: Create contact with contact_create()
    // TODO: Add to contacts array if successful
    bool result = contact_create(&new_contact, name, phone, email);
    if (!result)
    {
        printf("\nContact Could Not Be Added Successfully. Please Try Again.\n");
        pause_program("\nPress Enter to return to menu...");
        return;
    }

    // Adding contact
    contacts[contact_count] = new_contact;
    contact_count++;
    printf("\nContact '%s' Added Successfully To Directory.", name);
    printf("\nCurrent Total Number of Contacts In Directory : %d\n", contact_count);
    pause_program("\nPress Enter to return to menu...");
}

void list_contacts(void)
{
    int choice;
    Contact temp_contacts[MAX_CONTACTS];
    memcpy(temp_contacts, contacts, sizeof(Contact) * contact_count); // Make Temp Array To Store Contacts

    printf("\n=== ALL CONTACTS (%d) ===\n", contact_count);

    if (contact_count == 0)
    {
        printf("No contacts found.\n");
    }
    else
    {
        // TODO: Optional - Add sorting choice
        // printf("Sort by: 1) ID 2) Name\n");
        // Get choice, sort array (temporarily) if needed
        if (contact_count > 1)
        {
            printf("1. ID (ascending)\n");
            printf("2. Name (alphabetical)\n");
            printf("3. No sorting (default)\n");

            if (!get_int_range_prompt("Enter choice : ", 1, 3, &choice))
            {
                printf("Invalid Input Has Been Entered. Printing In Default Mode.\n");
                choice = 3;
            }
            if (choice == 1)
            { // Sort by ID
                qsort(temp_contacts, contact_count, sizeof(Contact), contact_compare_id);
                printf("<Sorted by ID>\n");
            }
            else if (choice == 2)
            { // Sort by Name
                qsort(temp_contacts, contact_count, sizeof(Contact), contact_compare_name);
                printf("<Sorted by Name>\n");
            }
            else
            { // No sorting
                printf("<No Sort>\n");
            }
        }
    }
    contact_print_all(temp_contacts, contact_count);
    printf("\nTotal: %d contact(s)\n", contact_count);
    pause_program("\nPress Enter to return to menu...");
    return;
}

void search_contacts(void)
{
    printf("\n=== SEARCH CONTACTS ===\n");

    show_search_menu();
    int choice;

    // TODO: Get search choice (1-3, 4 to cancel)

    switch (choice)
    {
    case 1: // Search by name
        // TODO: Get search term
        // TODO: Call find_contact_by_name()
        // TODO: Display results
        break;

    case 2: // Search by phone
        // TODO: Get phone number
        // TODO: Call find_contact_by_phone()
        // TODO: Display results
        break;

    case 3: // Search by ID
        // TODO: Get ID
        // TODO: Call find_contact_by_id()
        // TODO: Show contact if found
        break;

    case 4: // Cancel
        printf("Search cancelled.\n");
        return;
    }

    pause_program("\nPress Enter to return to menu...");
}

void delete_contact(void)
{
    printf("\n=== DELETE CONTACT ===\n");

    // TODO: Show all contacts first (optional)

    int id_to_delete;
    // TODO: Get ID to delete using get_int_prompt()

    int index = find_contact_by_id(id_to_delete);
    if (index == -1)
    {
        printf("Contact with ID %d not found.\n", id_to_delete);
        pause_program("Press Enter to continue...");
        return;
    }

    // TODO: Show contact details for confirmation
    printf("Delete this contact?\n");
    contact_print(&contacts[index]);

    // TODO: Get confirmation (Y/N) using get_char_prompt()

    // TODO: If confirmed:
    // - Shift all contacts after index left by one
    // - Decrement contact_count
    // - Show success message

    pause_program("\nPress Enter to return to menu...");
}

void edit_contact(void)
{
    printf("\n=== EDIT CONTACT ===\n");

    // TODO: Similar to delete, but allow editing fields
    // 1. Find contact by ID
    // 2. Show current values
    // 3. For each field (name, phone, email):
    //    - Ask if user wants to edit
    //    - Get new value if yes
    //    - Validate new value
    // 4. Update contact if any changes
    // 5. Show before/after

    printf("Edit feature coming soon!\n");
    pause_program("Press Enter to return to menu...");
}

// ============================================================================
// HELPER FUNCTIONS (IMPLEMENT THESE TOO!)
// ============================================================================

void find_contact_by_name(const char *name, int *indices, int *found_count)
{
    // TODO: Search contacts array for name (case-insensitive)
    // Use strstr() or your own logic
    // Store matching indices in indices[] array
    // Set *found_count to number of matches
}

void find_contact_by_phone(const char *phone, int *indices, int *found_count)
{
    // TODO: Similar to name search but for phone
    // Might want to normalize phone numbers first
    // (remove spaces, dashes, parentheses for comparison)
}

int find_contact_by_id(int id)
{
    // TODO: Linear search through contacts
    // Return index if found, -1 if not found
    return -1;
}

void show_menu(void)
{
    printf("\n=== MAIN MENU ===\n");
    printf("1. Add Contact\n");
    printf("2. List All Contacts\n");
    printf("3. Search Contact\n");
    printf("4. Delete Contact\n");
    printf("5. Edit Contact\n");
    printf("6. Save to File\n");
    printf("7. Exit\n");
}

void show_search_menu(void)
{
    printf("Search by:\n");
    printf("1. Name\n");
    printf("2. Phone Number\n");
    printf("3. ID\n");
    printf("4. Cancel\n");
}

void clear_screen(void)
{
    // TODO: Platform-specific screen clear
    // Windows: system("cls");
    // Linux/Mac: system("clear");
    // Or print many newlines as fallback
}

/*
// ============================================================================
// FILE I/O (ADVANCED - OPTIONAL FOR NOW)
// ============================================================================

void save_contacts_to_file(const char *filename) {
    // TODO: Open file for writing (binary or text)
    // TODO: Write contact_count first
    // TODO: Write each contact
    // TODO: Handle errors
}

void load_contacts_from_file(const char *filename) {
    // TODO: Open file for reading
    // TODO: Read contact_count
    // TODO: Read each contact into array
    // TODO: Update next_contact_id based on loaded data
    // TODO: Handle errors (file not found, corrupt, etc.)
}*/