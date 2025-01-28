#include <stdio.h>
#include <unistd.h>
#include <mraa.h>

#define NUM_SEGMENTS 7

// Segment mapping for digits 0-9
const int digit_map[10][NUM_SEGMENTS] = {
    {0, 0, 0, 0, 0, 0, 1}, // 0
    {1, 0, 0, 1, 1, 1, 1}, // 1
    {0, 0, 1, 0, 0, 1, 0}, // 2
    {0, 0, 0, 0, 1, 1, 0}, // 3
    {1, 0, 0, 1, 1, 0, 0}, // 4
    {0, 1, 0, 0, 1, 0, 0}, // 5
    {0, 1, 0, 0, 0, 0, 0}, // 6
    {0, 0, 0, 1, 1, 1, 1}, // 7
    {0, 0, 0, 0, 0, 0, 0}, // 8
    {0, 0, 0, 0, 1, 0, 0}  // 9
};

int main() {
    int segment_pins[NUM_SEGMENTS] = {12, 13, 36, 37, 40, 39, 43}; // Update these pins as per your setup
    mraa_gpio_context segments[NUM_SEGMENTS];

    // Initialize GPIO for each segment
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        segments[i] = mraa_gpio_init(segment_pins[i]);
        if (segments[i] == NULL) {
            fprintf(stderr, "Error initializing GPIO for segment %d\n", i);
            return -1;
        }
        mraa_gpio_dir(segments[i], MRAA_GPIO_OUT);
    }

    // Display digits 0-9
    for (int digit = 0; digit <= 9; digit++) {
        printf("Displaying digit: %d\n", digit);

        // Set segment values based on digit map
        for (int i = 0; i < NUM_SEGMENTS; i++) {
            mraa_gpio_write(segments[i], digit_map[digit][i]);
        }

        sleep(1); // Display each digit for 1 second
    }

    // Turn off all segments after displaying
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        mraa_gpio_write(segments[i], 0);
    }

    // Cleanup
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        mraa_gpio_close(segments[i]);
    }

    return 0;
}
