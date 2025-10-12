#ifndef RESOLUTION_H
#define RESOLUTION_H

// Global variables to hold the resolution width and height.
extern int resolution_width;
extern int resolution_height;

// Fixed aspect ratio and resolution bounds
#define WIDTH_HEIGHT_RATIO 0.75f
#define MIN_WIDTH 640
#define MAX_WIDTH 3840

/**
 * @brief Parses command-line arguments to set the resolution.
 *
 * Looks for "--resolution" or "-r" followed by a width value.
 * Sets resolution_width and resolution_height if valid.
 * Ensures width is within MIN_WIDTH and MAX_WIDTH bounds.
 *
 * @param argc Argument count from main.
 * @param argv Argument vector from main.
 */
void checkResolution(int argc, char *argv[]);

#endif