#ifndef DEBUG_H
#define DEBUG_H

#include <stdbool.h>

// Global flag indicating if the game is running in debug mode.
extern bool is_debug_mode;

/**
 * @brief Parses command-line arguments to check for the debug flag.
 *
 * This function scans the provided arguments for the presence of "--debug".
 * If found, it sets the global is_debug_mode flag to true.
 *
 * @param argc The count of command-line arguments.
 * @param argv The array of command-line argument strings.
 */
void checkDebugFlag(int argc, char *argv[]);

#endif