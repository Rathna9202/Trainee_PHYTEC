#include <mraa/gpio.h>
#include <unistd.h> // For usleep()
#include <stdio.h>  // For printf()

int main() {
    // Initialize GPIO pins
    mraa_gpio_context led = mraa_gpio_init(61);    // LED pin
    mraa_gpio_context switch1 = mraa_gpio_init(35); // Switch pin

    if (led == NULL || switch1 == NULL) {
        fprintf(stderr, "Failed to initialize GPIO\n");
        return 1;
    }

    // Set LED pin as output
    mraa_gpio_dir(led, MRAA_GPIO_OUT);

    // Set switch pin as input
    mraa_gpio_dir(switch1, MRAA_GPIO_IN);

    int press_count = 0;      // Counter for the number of switch presses
    int prev_state = 1;       // Store the previous state of the switch (1 = not pressed)

    // Start with the LED OFF
    mraa_gpio_write(led, 0);

    while (1) {
        // Read the current state of the switch
        int current_state = mraa_gpio_read(switch1);

        if (current_state == 0 && prev_state == 1) {
            // Switch pressed (state changed from not pressed to pressed)
            press_count++;
            printf("Switch pressed %d time(s)\n", press_count);

            // Toggle the LED as many times as the press count
            for (int i = 0; i < press_count; i++) {
                mraa_gpio_write(led, 1);  // Turn LED ON
                usleep(500000);           // 500ms delay
                mraa_gpio_write(led, 0);  // Turn LED OFF
                usleep(500000);           // 500ms delay
            }

            // Add a delay to avoid bouncing issues
            usleep(200000); // 200ms delay
        }

        // Update the previous state
        prev_state = current_state;

        // Add a small delay for debounce
        usleep(50000); // 50ms delay
    }

    // Cleanup (unreachable in this loop)
    mraa_gpio_close(led);
    mraa_gpio_close(switch1);

    return 0;
}

