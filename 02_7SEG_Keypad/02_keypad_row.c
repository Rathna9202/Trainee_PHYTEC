#include <stdio.h>
#include <unistd.h>
#include <mraa.h>

// Define row and column GPIO pins
#define ROW1_PIN  12
#define ROW2_PIN  13
#define ROW3_PIN  36
#define ROW4_PIN  37

#define COL1_PIN  40
#define COL2_PIN  39
#define COL3_PIN  43

void init_keypad(mraa_gpio_context *row_pins, mraa_gpio_context *col_pins);
int scan_keypad(mraa_gpio_context *row_pins, mraa_gpio_context *col_pins);

int main() {
    mraa_gpio_context row_pins[4], col_pins[3];

    // Initialize keypad rows and columns
    init_keypad(row_pins, col_pins);

    while (1) {
        // Scan the keypad and print the corresponding row if a key is pressed
        int row = scan_keypad(row_pins, col_pins);
        if (row != -1) { // If a valid row is pressed
            printf("Key pressed in Row %d!\n", row);
        }
        usleep(100000); // Small delay for debounce
    }

    // Cleanup
    for (int i = 0; i < 4; i++) {
        mraa_gpio_close(row_pins[i]);
    }
    for (int i = 0; i < 3; i++) {
        mraa_gpio_close(col_pins[i]);
    }

    return 0;
}

void init_keypad(mraa_gpio_context *row_pins, mraa_gpio_context *col_pins) {
    // Initialize row pins (set as output)
    int row_pins_config[4] = {ROW1_PIN, ROW2_PIN, ROW3_PIN, ROW4_PIN};
    for (int i = 0; i < 4; i++) {
        row_pins[i] = mraa_gpio_init(row_pins_config[i]);
        if (row_pins[i] == NULL) {
            fprintf(stderr, "Error initializing GPIO for row %d\n", i);
            continue;
        }
        mraa_gpio_dir(row_pins[i], MRAA_GPIO_OUT);
        mraa_gpio_write(row_pins[i], 1); // Set all rows to HIGH initially
    }

    // Initialize column pins (set as input with pull-up resistors)
    int col_pins_config[3] = {COL1_PIN, COL2_PIN, COL3_PIN};
    for (int i = 0; i < 3; i++) {
        col_pins[i] = mraa_gpio_init(col_pins_config[i]);
        if (col_pins[i] == NULL) {
            fprintf(stderr, "Error initializing GPIO for column %d\n", i);
            continue;
        }
        mraa_gpio_dir(col_pins[i], MRAA_GPIO_IN);
        mraa_gpio_mode(col_pins[i], MRAA_GPIO_PULLUP); // Enable pull-up resistors
    }
}

int scan_keypad(mraa_gpio_context *row_pins, mraa_gpio_context *col_pins) {
    // Loop through each row
    for (int row = 0; row < 4; row++) {
        // Set all rows to HIGH
        for (int r = 0; r < 4; r++) {
            mraa_gpio_write(row_pins[r], 1);
        }

        // Set the current row to LOW
        mraa_gpio_write(row_pins[row], 0);

        // Check columns for a pressed key
        for (int col = 0; col < 3; col++) {
            if (mraa_gpio_read(col_pins[col]) == 0) { // Check if column is pulled LOW
                // Debounce delay
                usleep(50000);

                // Check again to confirm key press
                if (mraa_gpio_read(col_pins[col]) == 0) {
                    // Reset the current row to HIGH before returning
                    mraa_gpio_write(row_pins[row], 1);
                    return row + 1; // Return the row number (1-4)
                }
            }
        }
    }

    return -1; // No key pressed
}

