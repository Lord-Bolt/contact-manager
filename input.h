/******************************************************************************
 * FILE: input.h
 * DESCRIPTION: Input utilities with consistent error handling
 * AUTHOR: Vedhashiva M T
 * VERSION: 2.0.0 (Nuclear reset)
 * RULE: Return bool, output via pointers, no magic numbers
 ******************************************************************************/

#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

// ============================================================================
// CONSTANTS
// ============================================================================

#define BUFFER_SIZE 256

// ============================================================================
// CORE FUNCTIONS
// ============================================================================

// Buffer clearing
void nuke_buffer(void);

// String input
bool get_string(char *buffer, int size);
bool get_string_prompt(const char *prompt, char *buffer, int size);
void trim_string(char *str);
bool is_whitespace(const char *str);

// Character input
bool get_char(char *result);
bool get_char_prompt(const char *prompt, char *result);

// Integer input
bool get_int(int *result);
bool get_int_prompt(const char *prompt, int *result);
bool get_int_range(int min, int max, int *result);
bool get_int_range_prompt(const char *prompt, int min, int max, int *result);

// Floating point input
bool get_float(float *result);
bool get_float_prompt(const char *prompt, float *result);
bool get_float_range(float min, float max, float *result);
bool get_float_range_prompt(const char *prompt, float min, float max, float *result);

// Program control
void pause_program(const char *message);
void exit_program(void);

#endif // INPUT_H