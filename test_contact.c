/******************************************************************************
 * FILE: test_contact.c
 * DESCRIPTION: Unit tests for contact module
 * USAGE: gcc test_contact.c contact.c input.c -o test.exe && ./test.exe
 ******************************************************************************/

#include "contact.h"
#include <stdio.h>
#include <assert.h>

// Test counter
int tests_passed = 0;
int tests_failed = 0;

#define TEST(condition, description) \
    do { \
        if (condition) { \
            printf("✅ PASS: %s\n", description); \
            tests_passed++; \
        } else { \
            printf("❌ FAIL: %s\n", description); \
            tests_failed++; \
        } \
    } while(0)

void test_contact_find_by_id(void) {
    printf("\n=== Testing contact_find_by_id ===\n");
    
    // Setup test data
    Contact contacts[5] = {
        {.id = 1, .name = "Alice"},
        {.id = 2, .name = "Bob"},
        {.id = 3, .name = "Charlie"},
        {.id = 4, .name = "Diana"},
        {.id = 5, .name = "Eve"}
    };
    
    // Test 1: Find existing contact
    int result = contact_find_by_id(contacts, 5, 3);
    TEST(result == 2, "Find ID 3 (should return index 2)");
    
    // Test 2: Find first contact
    result = contact_find_by_id(contacts, 5, 1);
    TEST(result == 0, "Find ID 1 (first element)");
    
    // Test 3: Find last contact
    result = contact_find_by_id(contacts, 5, 5);
    TEST(result == 4, "Find ID 5 (last element)");
    
    // Test 4: Non-existent ID
    result = contact_find_by_id(contacts, 5, 99);
    TEST(result == -1, "Non-existent ID returns -1");
    
    // Test 5: Zero ID (your validation)
    result = contact_find_by_id(contacts, 5, 0);
    TEST(result == -1, "ID 0 returns -1 (your validation)");
    
    // Test 6: Negative ID
    result = contact_find_by_id(contacts, 5, -5);
    TEST(result == -1, "Negative ID returns -1");
    
    // Test 7: Empty array
    result = contact_find_by_id(contacts, 0, 1);
    TEST(result == -1, "Empty array returns -1");
    
    // Test 8: NULL array (shouldn't crash!)
    result = contact_find_by_id(NULL, 5, 1);
    TEST(result == -1, "NULL array returns -1 (doesn't crash)");
}

void test_contact_validation(void) {
    printf("\n=== Testing Validation Functions ===\n");
    
    // Test contact_validate_name
    TEST(contact_validate_name("John Doe") == true, "Valid name: John Doe");
    TEST(contact_validate_name("A") == false, "Too short name");
    TEST(contact_validate_name("") == false, "Empty name");
    TEST(contact_validate_name("   ") == false, "Whitespace only");
    
    // Test contact_validate_phone
    TEST(contact_validate_phone("555-1234") == true, "Valid phone: 555-1234");
    TEST(contact_validate_phone("abc") == false, "Invalid phone: letters");
    TEST(contact_validate_phone("123") == false, "Phone too short");
    
    // Test contact_validate_email
    TEST(contact_validate_email("test@example.com") == true, "Valid email");
    TEST(contact_validate_email("not-an-email") == false, "Invalid email");
}

void test_contact_creation(void) {
    printf("\n=== Testing Contact Creation ===\n");
    
    Contact c;
    
    // Test valid creation
    bool result = contact_create(&c, "John", "555-1234", "john@test.com");
    TEST(result == true, "Create valid contact");
    if (result) {
        TEST(c.id > 0, "Contact gets positive ID");
        TEST(strcmp(c.name, "John") == 0, "Name stored correctly");
    }
    
    // Test invalid creation
    result = contact_create(&c, "", "123", "bad-email");
    TEST(result == false, "Invalid contact rejected");
}

int main(void) {
    printf("========================================\n");
    printf("     CONTACT MODULE TEST SUITE\n");
    printf("========================================\n");
    
    // Run test suites
    test_contact_find_by_id();
    test_contact_validation();
    test_contact_creation();
    
    // Summary
    printf("\n========================================\n");
    printf("TEST SUMMARY:\n");
    printf("   Passed: %d\n", tests_passed);
    printf("   Failed: %d\n", tests_failed);
    printf("   Total:  %d\n", tests_passed + tests_failed);
    
    if (tests_failed == 0) {
        printf("\n ALL TESTS PASSED!\n");
        return 0;
    } else {
        printf("\n  Some tests failed\n");
        return 1;
    }
}
void test_matching_functions(void) {
    printf("\n=== Testing Matching Functions ===\n");
    
    Contact c = {.name="John Doe", .phone="(555) 123-4567", .email="JOHN@example.com"};
    
    // Test name matching
    TEST(contact_name_matches(&c, "john") == true, "Case-insensitive name match");
    TEST(contact_name_matches(&c, "doe") == true, "Partial name match");
    TEST(contact_name_matches(&c, "JO") == true, "Case-insensitive partial");
    TEST(contact_name_matches(&c, "smith") == false, "No match returns false");
    
    // Test phone matching
    TEST(contact_phone_matches(&c, "555") == true, "Phone digit extraction works");
    TEST(contact_phone_matches(&c, "1234567") == true, "Multiple digit match");
    TEST(contact_phone_matches(&c, "999") == false, "No phone match");
    
    // Test email matching
    TEST(contact_email_matches(&c, "john") == true, "Email partial match");
    TEST(contact_email_matches(&c, "EXAMPLE") == true, "Case-insensitive email");
    TEST(contact_email_matches(&c, "@") == true, "Email contains @");
    TEST(contact_email_matches(&c, "gmail") == false, "No email match");
}