#include <stdio.h>
#include <unistd.h>
#include <mraa.h>

// Define GPIO pins for 7-segment (a-g)
#define SEG_A_PIN 53
#define SEG_B_PIN 52
#define SEG_C_PIN 51
#define SEG_D_PIN 48
#define SEG_E_PIN 47
#define SEG_F_PIN 46
#define SEG_G_PIN 45

// Define GPIO pins for keypad rows and columns
#define ROW1_PIN  12
#define ROW2_PIN  13
#define ROW3_PIN  36
#define ROW4_PIN  37

#define COL1_PIN  40
#define COL2_PIN  39
#define COL3_PIN  43

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

void init_7seg(mraa_gpio_context *seg_pins);
void init_keypad(mraa_gpio_context *row_pins, mraa_gpio_context *col_pins);
int scan_keypad(mraa_gpio_context *row_pins, mraa_gpio_context *col_pins);
void display_digit(int digit, mraa_gpio_context *seg_pins);
void turn_off_7seg(mraa_gpio_context *seg_pins);

int main() {
    mraa_gpio_context seg_pins[NUM_SEGMENTS];  // Array to hold 7-segment GPIO pins
    mraa_gpio_context row_pins[4], col_pins[3];  // Array for keypad pins

    // Initialize 7-segment display pins
    init_7seg(seg_pins);

    // Initialize keypad rows and columns
    init_keypad(row_pins, col_pins);

    int last_key = -1; // To store the last pressed key

    while (1) {
        // Scan the keypad and get the pressed key
        int key = scan_keypad(row_pins, col_pins);
        
        if (key != -1 && key != last_key) {  // If key is pressed and it's different from last key
            last_key = key;  // Update the last pressed key
            printf("Key pressed: %d\n", key);

            // If * or # is pressed, don't display anything (null)
            if (key == 10 || key == -1) {
                turn_off_7seg(seg_pins);  // Turn off 7-segment display
            } else {
                // Display the corresponding digit on the 7-segment display
                display_digit(key, seg_pins);
            }

            // Wait for key release (debounce)
            while (scan_keypad(row_pins, col_pins) != -1) {
                usleep(50000); // Delay to wait for key release
            }
        } else {
            // If no key is pressed or key is released, turn off the 7-segment display
            turn_off_7seg(seg_pins);
        }

        usleep(100000); // Small delay for debounce
    }

    // Cleanup 7-segment and keypad GPIO pins
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        mraa_gpio_close(seg_pins[i]);
    }

    for (int i = 0; i < 4; i++) {
        mraa_gpio_close(row_pins[i]);
    }

    for (int i = 0; i < 3; i++) {
        mraa_gpio_close(col_pins[i]);
    }

    return 0;
}

void init_7seg(mraa_gpio_context *seg_pins) {
    // Initialize the 7-segment display GPIO pins (a-g)
    int seg_pins_config[7] = {SEG_A_PIN, SEG_B_PIN, SEG_C_PIN, SEG_D_PIN, SEG_E_PIN, SEG_F_PIN, SEG_G_PIN};
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        seg_pins[i] = mraa_gpio_init(seg_pins_config[i]);
        if (seg_pins[i] == NULL) {
            fprintf(stderr, "Error initializing GPIO for segment %d\n", i);
            continue;
        }
        mraa_gpio_dir(seg_pins[i], MRAA_GPIO_OUT);
	mraa_gpio_mode(seg_pins[i], MRAA_GPIO_PULLUP);
    }
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
        mraa_gpio_write(row_pins[i], 0); // Set all rows to HIGH initially
	mraa_gpio_mode(row_pins[i], MRAA_GPIO_PULLUP);
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
        mraa_gpio_mode(col_pins[i], MRAA_GPIO_PULLUP); // Enable pull-up resistors for columns
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
                    // Return corresponding key
                    if (row == 0) return col + 1;  // 1, 2, 3
                    if (row == 1) return col + 4;  // 4, 5, 6
                    if (row == 2) return col + 7;  // 7, 8, 9
                    if (row == 3) return (col == 0) ? 0 : 0;  // 0, * (null as 10), # (null)
                }
            }
        }
    }

    return -1; // No key pressed
}

void display_digit(int digit, mraa_gpio_context *seg_pins) {
    // Display the corresponding digit on the 7-segment display using the updated digit_map
    if (digit >= 0 && digit <= 9) {
        for (int i = 0; i < NUM_SEGMENTS; i++) {
            mraa_gpio_write(seg_pins[i], digit_map[digit][i]);
        }
    }
}

void turn_off_7seg(mraa_gpio_context *seg_pins) {
    // Turn off all 7-segment display segments (a-g)
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        mraa_gpio_write(seg_pins[i], 1);
    }
}

