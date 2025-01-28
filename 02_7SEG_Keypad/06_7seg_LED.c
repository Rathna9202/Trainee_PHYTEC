#include <stdio.h>
#include <unistd.h>
#include <mraa.h>

#define NUM_SEGMENTS 7
#define LED_PIN 61

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

#define NUM_ROWS 4
#define NUM_COLS 3

int row_pins[NUM_ROWS] = {12, 13, 36, 37};  // GPIO pins for rows
int col_pins[NUM_COLS] = {40, 39, 43};     // GPIO pins for columns

mraa_gpio_context row_gpio[NUM_ROWS];
mraa_gpio_context col_gpio[NUM_COLS];
mraa_gpio_context segments[NUM_SEGMENTS];
mraa_gpio_context led;

void init_keypad() {
    // Initialize row pins (set as input with pull-up)
    for (int i = 0; i < NUM_ROWS; i++) {
        row_gpio[i] = mraa_gpio_init(row_pins[i]);
        mraa_gpio_dir(row_gpio[i], MRAA_GPIO_IN);
        mraa_gpio_mode(row_gpio[i], MRAA_GPIO_PULLUP);
    }

    // Initialize column pins (set as output)
    for (int i = 0; i < NUM_COLS; i++) {
        col_gpio[i] = mraa_gpio_init(col_pins[i]);
        mraa_gpio_dir(col_gpio[i], MRAA_GPIO_OUT);
    }

    // Initialize LED
    led = mraa_gpio_init(LED_PIN);
    mraa_gpio_dir(led, MRAA_GPIO_OUT);
    mraa_gpio_write(led, 0); // Initially turn off the LED
}

int scan_keypad() {
    // Scan the keypad for key press
    for (int row = 0; row < NUM_ROWS; row++) {
        // Set all columns to LOW before setting the current row HIGH
        for (int col = 0; col < NUM_COLS; col++) {
            mraa_gpio_write(col_gpio[col], 0);
        }

        // Set the current row to LOW (active low row)
        mraa_gpio_write(row_gpio[row], 0);

        // Check each column for a pressed key
        for (int col = 0; col < NUM_COLS; col++) {
            if (mraa_gpio_read(col_gpio[col]) == 0) {
                // If a key is pressed, return the row index (1-4) and column index (1-3)
                return (row * NUM_COLS + col + 1);  // Return 1-12 corresponding to keys
            }
        }

        // Set the row back to HIGH
        mraa_gpio_write(row_gpio[row], 1);
    }
    return -1; // No key is pressed
}

void display_digit(int digit) {
    // Display the digit on the 7-segment display
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        mraa_gpio_write(segments[i], digit_map[digit][i]);
    }
}

void init_7seg() {
    int segment_pins[NUM_SEGMENTS] = {53, 52, 51, 48, 47, 46, 45};  // GPIO pins for 7-segment

    // Initialize GPIO for each segment and set them to LOW
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        segments[i] = mraa_gpio_init(segment_pins[i]);
        if (segments[i] == NULL) {
            fprintf(stderr, "Error initializing GPIO for segment %d\n", i);
            return;
        }
        mraa_gpio_dir(segments[i], MRAA_GPIO_OUT);
        mraa_gpio_write(segments[i], 0);  // Set initial state to LOW
    }
}

int main() {
    // Initialize the keypad and 7-segment display
    init_keypad();
    init_7seg();

    while (1) {
        int key = scan_keypad();  // Check for key press

        if (key != -1) {
            int digit = key - 1;  // Map key press to digit (0-9 for digits)
            if (digit >= 0 && digit <= 9) {
                printf("Key pressed: %d\n", digit);
                display_digit(digit);  // Display the pressed digit on the 7-segment display

                // Toggle the LED the number of times equal to the digit pressed
                for (int i = 0; i < digit; i++) {
                    mraa_gpio_write(led, 1);  // Turn LED ON
                    usleep(500000);           // Wait 500 ms
                    mraa_gpio_write(led, 0);  // Turn LED OFF
                    usleep(500000);           // Wait 500 ms
                }
            }
        } else {
            // If no key is pressed, ensure the LED is off
            mraa_gpio_write(led, 0);
            // Turn off the 7-segment display
            for (int i = 0; i < NUM_SEGMENTS; i++) {
                mraa_gpio_write(segments[i], 0);
            }
        }

        usleep(100000);  // Small delay to avoid constant scanning
    }

    // Cleanup
    for (int i = 0; i < NUM_ROWS; i++) {
        mraa_gpio_close(row_gpio[i]);
    }
    for (int i = 0; i < NUM_COLS; i++) {
        mraa_gpio_close(col_gpio[i]);
    }
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        mraa_gpio_close(segments[i]);
    }
    mraa_gpio_close(led);

    return 0;
}

