/******************************************************************************
 * FILE: input.c
 * DESCRIPTION: Implementation of clean input utilities
 * RULE: Consistent, predictable, no surprises
 ******************************************************************************/

#include "input.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// ============================================================================
// BUFFER CLEARING
// ============================================================================

void nuke_buffer(void)
{
    int c;
    // Read and discard characters until newline or EOF
    // This clears any leftover input in stdin
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

// ============================================================================
// STRING FUNCTIONS
// ============================================================================

bool get_string(char *buffer, int size)
{
    // Safety check: buffer must exist and have space for at least 1 char + null
    if (buffer == NULL || size <= 1)
        return false;

    // fgets reads at most (size-1) characters from stdin
    // Returns NULL on error or EOF
    if (fgets(buffer, size, stdin) == NULL)
        return false;

    // Find newline character and replace it with null terminator
    // strcspn returns index of first occurrence of "\n" in buffer
    buffer[strcspn(buffer, "\n")] = '\0';

    // If buffer is full (size-1 characters read), clear leftover input
    // This prevents next input operation from reading the leftover newline
    if (strlen(buffer) == (size_t)(size - 1))
        nuke_buffer();

    return true;
}

bool get_string_prompt(const char *prompt, char *buffer, int size)
{
    // Display prompt if provided
    if (prompt != NULL)
        printf("%s", prompt);

    // Call the actual string input function
    return get_string(buffer, size);
}

void trim_string(char *str)
{
    // Handle null or empty strings
    if (str == NULL || str[0] == '\0')
        return;

    int start = 0;
    int end = strlen(str) - 1;

    // Find first non-whitespace character from left
    while (isspace(str[start]))
        start++;

    // Find first non-whitespace character from right
    while (end >= start && isspace(str[end]))
        end--;

    if (start > 0)
    {
        // Shift characters left to remove leading whitespace
        for (int i = start; i <= end; i++)
            str[i - start] = str[i];
        // Add null terminator at new end position
        str[end - start + 1] = '\0';
    }
    else
    {
        // No leading whitespace, just truncate at end position
        str[end + 1] = '\0';
    }
}

bool is_whitespace(const char *str)
{
    // Null pointer is considered whitespace (empty)
    if (str == NULL)
        return true;

    // Check each character - if any non-whitespace found, return false
    while (*str != '\0')
    {
        if (!isspace(*str))
            return false;
        str++;
    }

    // All characters were whitespace
    return true;
}

// ============================================================================
// CHARACTER INPUT
// ============================================================================

bool get_char(char *result)
{
    // Must have a place to store the result
    if (result == NULL)
        return false;

    char buffer[100]; // Small buffer for single character input

    // Get string input (allows us to use our existing validation)
    if (!get_string(buffer, sizeof(buffer)))
        return false;

    // Empty string is valid (returns null character)
    if (buffer[0] == '\0')
    {
        *result = '\0';
        return true;
    }

    // Check if first character is printable (not control character)
    if (!isprint(buffer[0]))
        return false;

    // Success - store the character
    *result = buffer[0];
    return true;
}

bool get_char_prompt(const char *prompt, char *result)
{
    // Display prompt if provided
    if (prompt != NULL)
        printf("%s", prompt);

    // Call the actual character input function
    return get_char(result);
}

// ============================================================================
// INTEGER INPUT
// ============================================================================

bool get_int(int *result)
{
    // Must have a place to store the result
    if (result == NULL)
        return false;

    char buffer[BUFFER_SIZE];
    int value;
    char extra; // Used to detect trailing garbage after number

    // Get string input first
    if (!get_string(buffer, sizeof(buffer)))
        return false;

    // sscanf returns number of successfully matched items
    // "%d %c" pattern: read integer, then optional character
    // If returns 1: successfully read integer, no trailing garbage
    // If returns 2: integer followed by non-whitespace (invalid)
    // If returns 0: no integer found
    if (sscanf(buffer, "%d %c", &value, &extra) != 1)
        return false;

    // Success - store the integer
    *result = value;
    return true;
}

bool get_int_prompt(const char *prompt, int *result)
{
    // Display prompt if provided
    if (prompt != NULL)
        printf("%s", prompt);

    // Call the actual integer input function
    return get_int(result);
}

bool get_int_range(int min, int max, int *result)
{
    // First get an integer
    if (!get_int(result))
        return false;

    // Check if it's within specified range
    // Return true only if validation passes
    return (*result >= min && *result <= max);
}

bool get_int_range_prompt(const char *prompt, int min, int max, int *result)
{
    // Display prompt if provided
    if (prompt != NULL)
        printf("%s", prompt);

    // Call the range-validated integer function
    return get_int_range(min, max, result);
}

// ============================================================================
// FLOATING POINT INPUT
// ============================================================================

bool get_float(float *result)
{
    // Same pattern as get_int but for floats
    if (result == NULL)
        return false;

    char buffer[BUFFER_SIZE];
    float value;
    char extra;

    if (!get_string(buffer, sizeof(buffer)))
        return false;

    // %f reads a float, %c detects trailing garbage
    if (sscanf(buffer, "%f %c", &value, &extra) != 1)
        return false;

    *result = value;
    return true;
}

bool get_float_prompt(const char *prompt, float *result)
{
    if (prompt != NULL)
        printf("%s", prompt);

    return get_float(result);
}

bool get_float_range(float min, float max, float *result)
{
    if (!get_float(result))
        return false;

    return (*result >= min && *result <= max);
}

bool get_float_range_prompt(const char *prompt, float min, float max, float *result)
{
    if (prompt != NULL)
        printf("%s", prompt);

    return get_float_range(min, max, result);
}

// ============================================================================
// PROGRAM CONTROL
// ============================================================================

void pause_program(const char *message)
{
    // Default message if none provided
    if (message == NULL)
        message = "Press Enter to continue...";

    printf("\n%s", message);
    fflush(stdout); // Ensure prompt appears immediately

    // Wait for Enter key
    nuke_buffer();
}

void exit_program(void)
{
    // Display exit message
    printf("\n========================================\n");
    printf("            PROGRAM FINISHED\n");
    printf("========================================\n\n");

    // Wait for user confirmation before exiting
    pause_program("Press Enter to exit...");

    // Exit program with success code
    exit(0);
}