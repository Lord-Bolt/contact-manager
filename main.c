/******************************************************************************
 * FILE: main.c
 * DESCRIPTION: Contact Manager - Main program with menu
 * AUTHOR: Vedhashiva M T
 * RULE: Use input.h for ALL user input!
 ******************************************************************************/

#include "contact.h"
#include "input.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
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
void save_contacts_to_file(const char *filename);
void load_contacts_from_file(const char *filename);
void display_search_results(const Contact contacts[], const int indices[], int count);

// UI functions
void show_menu(void);
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
        if (!get_int_range_prompt("Enter choice (1-8): ", 1, 8, &choice))
        {
            printf("\nInvalid input! Please enter a number 1-8.\n");
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
            clear_screen();
            break;
        case 8:
            printf("\nExiting Contact Manager...\n");
            break;
        default:
            printf("Invalid choice!\n");
            break;
        }

    } while (choice != 8);

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
// DONE
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
                printf("<No Sort>\n`");
            }
        }
    }
    contact_print_all(temp_contacts, contact_count);
    printf("\nTotal: %d contact(s)\n", contact_count);
    pause_program("\nPress Enter to return to menu...");
    return;
}
// DONE
void search_contacts(void)
{
    printf("\n=== SEARCH CONTACTS ===\n");

    // show_search_menu(); // Removed as requested
    int choice;
    int result;                      // Not single int for readability //
    int found_indices[MAX_CONTACTS]; // CHANGED: renamed from found_count
    int found_index;                 // Learnt the hard way that total number != index

    // FIXED: Clearer prompt without show_search_menu()
    if (!get_int_range_prompt("\n1 - Search By ID\n2 - Search By Name\n3 - Search By Phone\n4 - Search By E-mail\n5 - Quit\nEnter Choice: ", 1, 5, &choice))
    {
        printf("Invalid Choice Has Been Entered. Returning to Main Menu.\n");
        pause_program(NULL);
        return;
    }

    switch (choice)
    {
    case 1: // Search by id
    {
        int id;
        if (!get_int_range_prompt("Enter ID : ", 1, 100, &id))
        {
            printf("Invalid ID Has Been Entered. Returning to Main Menu.\n");
            pause_program(NULL);
            return;
        }

        found_index = contact_find_by_id(contacts, contact_count, id);
        if (found_index == -1)
        {
            printf("No such Contact with ID : %d exists within the directory.\n", id);
            break;
        }

        printf("Found 1 Contact(s)\n");            // FIXED: Added newline
        printf("Contact with ID : \'%d\':\n", id); // FIXED: Added newline
        contact_print(&contacts[found_index]);
        break;
    }

    case 2: // Search by name
    {
        char name[MAX_NAME_LEN];
        if (!get_string_prompt("Enter Name : ", name, sizeof(name)) || is_whitespace(name))
        {
            printf("Invalid Name Has Been Entered. Returning to Main Menu.\n");
            pause_program(NULL);
            return;
        }

        result = contact_find_by_name(contacts, contact_count, name, found_indices); // CHANGED: found_count → found_indices

        // FIXED: Check for 0 results, not -1 (unless -1 means error in your implementation)
        if (result == 0) // CHANGED: result == 0 means "found nothing"
        {
            printf("No such Contact with Name : %s exists within the directory.\n", name);
            break;
        }
        else if (result == -1) // -1 means ERROR (not just "not found")
        {
            printf("Search error occurred. Returning To Main Menu\n");
            break;
        }

        printf("Found %d Contact(s)\n", result);        // FIXED: Added newline
        printf("Contacts with Name : \'%s\':\n", name); // FIXED: Added newline
        for (int i = 0; i < result; i++)
        {
            contact_print(&contacts[found_indices[i]]); // CHANGED: found_count → found_indices
        }
        break;
    }

    case 3: // Search by phone
    {
        char phone[MAX_PHONE_LEN];
        if (!get_string_prompt("Enter Phone : ", phone, sizeof(phone)) || is_whitespace(phone))
        {
            printf("Invalid Phone Has Been Entered. Returning To Main Menu.\n");
            pause_program(NULL);
            return;
        }

        result = contact_find_by_phone(contacts, contact_count, phone, found_indices); // CHANGED: found_count → found_indices

        // FIXED: Same logic as name search
        if (result == 0)
        {
            printf("No such Contact with Phone : %s exists within the directory.\n", phone);
            break;
        }
        else if (result == -1)
        {
            printf("Search error occurred. Returning To Main Menu\n");
            break;
        }

        printf("Found %d Contact(s)\n", result);          // FIXED: Added newline
        printf("Contacts with Phone : \'%s\':\n", phone); // FIXED: Added newline
        for (int i = 0; i < result; i++)
        {
            contact_print(&contacts[found_indices[i]]); // CHANGED: found_count → found_indices
        }
        break;
    }

    case 4: // Search by e-mail
    {
        char email[MAX_EMAIL_LEN];
        if (!get_string_prompt("Enter E-mail : ", email, sizeof(email)) || is_whitespace(email))
        {
            printf("Invalid E-mail Has Been Entered. Returning To Main Menu.\n"); // FIXED: Added newline
            pause_program(NULL);
            return;
        }

        result = contact_find_by_email(contacts, contact_count, email, found_indices); // CHANGED: found_count → found_indices

        // FIXED: Same logic
        if (result == 0)
        {
            printf("No such Contact with E-Mail : %s exists within the directory.\n", email);
            break;
        }
        else if (result == -1)
        {
            printf("Search error occurred. Returning To Main Menu\n");
            break;
        }

        printf("Found %d Contact(s)\n", result);           // FIXED: Added newline
        printf("Contacts with E-mail : \'%s\':\n", email); // FIXED: Added newline
        for (int i = 0; i < result; i++)
        {
            contact_print(&contacts[found_indices[i]]); // CHANGED: found_count → found_indices
        }
        break;
    }

    case 5: // Quit
        printf("Returning to main menu...\n");
        return;
    }

    pause_program("\nPress Enter to return to menu...");
}
// DONE
void delete_contact(void)
{
    printf("\n=== DELETE CONTACT ===\n"); // Not show all Contacts Because It's Too Much

    int id_to_delete;

    if (!get_int_range_prompt("\nEnter ID To Delete : ", 1, 100, &id_to_delete))
    {
        printf("Invalid ID Has Been Entered. Returning To Main Menu.\n");
        pause_program(NULL);
        return;
    }

    int index = contact_find_by_id(contacts, contact_count, id_to_delete);
    if (index == -1)
    {
        printf("Contact with ID %d not found.\n", id_to_delete);
        pause_program("Press Enter to continue...");
        return;
    }

    // TODO: Show contact details for confirmation
    printf("Delete this contact?\n");
    contact_print_header(); // For header
    contact_print(&contacts[index]);
    printf("\n"); // For Spacing

    // TODO: Get confirmation (Y/N) using get_char_prompt()
    char choice;
    if (!get_char_prompt("\nAre You Sure? (Y/N) : ", &choice))
    {
        printf("Invalid Character Has Been Entered. Directory Is Being Left Unchanged.\nReturning To Main Menu.\n");
        pause_program(NULL);
        return;
    }

    // TODO: If confirmed:
    // - Shift all contacts after index left by one
    // - Decrement contact_count
    if (tolower(choice) == 'y') // ctype.h has BEEN included
    {
        if (!contact_remove_by_index(contacts, &contact_count, index))
        {
            printf("Failure To Execute. Directory Is Being Left Unchanged.\nReturning To Main Menu.\n");
            pause_program(NULL);
            return;
        }
        printf("Contact Deletion Executed Successfully.\nTotal Contacts Remaining In Directory : %d\n", contact_count);
    }
    else if (tolower(choice) == 'n')
    {
        printf("Deletion Cancelled By User. Directory Is Being Left Unchanged.\n");
    }
    else
    {
        printf("Invalid Character Has Been Entered. Please Enter Only Y/N.\nReturning To Main Menu.\n");
        pause_program(NULL); // To keep consistency of same formatting
        return;
    }

    pause_program("\nPress Enter to return to menu...");
    return;
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
    int id_to_find;

    if (!get_int_range_prompt("\nEnter ID To Modify : ", 1, 100, &id_to_find))
    {
        printf("Invalid ID Has Been Entered. Returning To Main Menu.\n");
        pause_program(NULL);
        return;
    }

    int index = contact_find_by_id(contacts, contact_count, id_to_find);
    if (index == -1)
    {
        printf("Contact with ID %d not found.\n", id_to_find);
        pause_program("Press Enter to continue...");
        return;
    }

    // TODO: Show contact details for confirmation
    printf("Modify this contact?\n");
    contact_print_header(); // For header
    contact_print(&contacts[index]);
    printf("\n"); // For Spacing

    // TODO: Get confirmation (Y/N) using get_char_prompt()
    char choice;
    if (!get_char_prompt("\nAre You Sure? (Y/N) : ", &choice))
    {
        printf("Invalid Character Has Been Entered. Directory Is Being Left Unchanged.\nReturning To Main Menu.\n");
        pause_program(NULL);
        return;
    }

    if (tolower(choice) == 'y')
    {
        int c;
        if (get_int_range_prompt("Which Field To Modify?\n1 - Name\n2 - Phone\n3 - E-mail\nEnter Your Choice : ", 1, 3, &c))
        {

            // Declare ALL variables BEFORE switch:
            char new_name[MAX_NAME_LEN], old_name[MAX_NAME_LEN];
            char new_phone[MAX_PHONE_LEN], old_phone[MAX_PHONE_LEN];
            char new_email[MAX_EMAIL_LEN], old_email[MAX_EMAIL_LEN];
            switch (c)
            {
            case 1:
                if (!get_string_prompt("Enter new name: ", new_name, sizeof(new_name)) ||
                    is_whitespace(new_name) ||
                    !contact_validate_name(new_name))
                {
                    printf("Invalid Name Entered. Update cancelled.\nReturning To Main Menu.\n");
                    break;
                }

                // Store old for comparison
                strcpy(old_name, contacts[index].name);

                // Update
                strncpy(contacts[index].name, new_name, MAX_NAME_LEN - 1);
                contacts[index].name[MAX_NAME_LEN - 1] = '\0';

                // Show results
                printf("\nContact Updated Successfully. (Field Updated : Name)\n");
                printf("OLD: %s\n", old_name);
                printf("NEW: %s\n", contacts[index].name);
                break;

            case 2:
                if (!get_string_prompt("Enter new phone: ", new_phone, sizeof(new_phone)) ||
                    is_whitespace(new_phone) ||
                    !contact_validate_phone(new_phone))
                {
                    printf("Invalid Phone Entered. Update cancelled.\nReturning To Main Menu.\n");
                    break;
                }

                // Store old for comparison
                strcpy(old_phone, contacts[index].phone);

                // Update
                strncpy(contacts[index].phone, new_phone, MAX_PHONE_LEN - 1);
                contacts[index].phone[MAX_PHONE_LEN - 1] = '\0';

                // Show results
                printf("\nContact Updated Successfully. (Field Updated : Phone)\n");
                printf("OLD: %s\n", old_phone);
                printf("NEW: %s\n", contacts[index].phone);
                break;

            case 3:
                if (!get_string_prompt("Enter new e-mail: ", new_email, sizeof(new_email)) ||
                    is_whitespace(new_email) ||
                    !contact_validate_email(new_email))
                {
                    printf("Invalid E-mail Entered. Update cancelled.\nReturning To Main Menu.\n");
                    break;
                }

                // Store old for comparison
                strcpy(old_email, contacts[index].email);

                // Update
                strncpy(contacts[index].email, new_email, MAX_EMAIL_LEN - 1);
                contacts[index].email[MAX_EMAIL_LEN - 1] = '\0';

                // Show results
                printf("\nContact Updated Successfully. (Field Updated : Email)\n");
                printf("OLD: %s\n", old_email);
                printf("NEW: %s\n", contacts[index].email);
                break;

            default:
                printf("Invalid choice. Update cancelled.\nReturning To Main Menu.");
                break;
            }
        }
    }

    else if (tolower(choice) == 'n')
    {
        printf("Updation Cancelled By User. Directory Is Being Left Unchanged.\n");
    }

    else
    {
        printf("Invalid Character Has Been Entered. Please Enter Only Y/N.\nReturning To Main Menu.\n");
        pause_program(NULL); // To keep consistency of same formatting
        return;
    }

    pause_program("\nPress Enter to return to menu...");
    return;
}

void clear_screen(void)
{
    // TODO: Platform-specific screen clear
    // Windows: system("cls");
    // Linux/Mac: system("clear");
    // Or print many newlines as fallback
    printf("Clear Screen Not Implemented Yet - Upcoming In New Version");
    return;
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

void display_search_results(const Contact contacts[], const int indices[], int count)
{
    if (count == 0)
    {
        printf("No matches found.\n");
        return;
    }

    printf("Found %d match(es):\n", count);
    contact_print_header();

    // Create temporary array of matching contacts
    Contact matches[count];
    for (int i = 0; i < count; i++)
    {
        matches[i] = contacts[indices[i]];
    }

    contact_print_all(matches, count);
}

void show_menu(void)
{
    printf("\n=== MAIN MENU ===\n");
    printf("1. Add Contact\n");
    printf("2. List All Contacts\n");
    printf("3. Search Contact\n");
    printf("4. Delete Contact\n");
    printf("5. Edit Contact\n");
    printf("6. Save to File (Coming Soon)\n");
    printf("7. Clear Screen (Coming Soon)\n");
    printf("8. Exit\n");
}