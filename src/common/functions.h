#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

#include <stdnoreturn.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Prints an error message in the standard error stream and exits the program.
 * The value of `errno` is also printed.
 *
 * Note: This function is implemented in main.c
 */
void panic(const char *s);

/**
 * Opens the log file and print a line in it.
 */
void print_log(const char *s, ...);

#ifdef __cplusplus
}
#endif
#endif