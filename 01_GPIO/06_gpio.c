#include <mraa/gpio.h>
#include <unistd.h> // For usleep()
#include <stdio.h>  // For printf()

int main() {
    // Initialize GPIO pins
    mraa_gpio_context led1 = mraa_gpio_init(36);   // LED 1 pin
    mraa_gpio_context led2 = mraa_gpio_init(12);   // LED 2 pin
    mraa_gpio_context led3 = mraa_gpio_init(40);   // LED 3 pin
    mraa_gpio_context switch1 = mraa_gpio_init(13); // Switch pin

    if (led1 == NULL || led2 == NULL || led3 == NULL || switch1 == NULL) {
        fprintf(stderr, "Failed to initialize GPIO\n");
        return 1;
    }

    // Set LED pins as output
    mraa_gpio_dir(led1, MRAA_GPIO_OUT);
    mraa_gpio_dir(led2, MRAA_GPIO_OUT);
    mraa_gpio_dir(led3, MRAA_GPIO_OUT);

    // Set switch pin as input
    mraa_gpio_dir(switch1, MRAA_GPIO_IN);

    int press_count = 0;       // To count the number of switch presses
    int prev_state = 1;        // Store the previous state of the switch (1 = not pressed)

    while (1) {
        // Read the current state of the switch
        int current_state = mraa_gpio_read(switch1);

        if (current_state == 0 && prev_state == 1) {
            // Switch pressed (state changed from not pressed to pressed)
            press_count++;
            printf("Switch pressed %d times\n", press_count);

            // Turn on the corresponding LED based on the press count
            if (press_count == 1) {
                mraa_gpio_write(led1, 1); // Turn on LED 1
            } else if (press_count == 2) {
                mraa_gpio_write(led2, 1); // Turn on LED 2
            } else if (press_count == 3) {
                mraa_gpio_write(led3, 1); // Turn on LED 3
            }

            // Add a delay to avoid bouncing issues
            usleep(200000); // 200ms delay
        }

        if (current_state == 1 && prev_state == 0) {
            // Switch released (state changed from pressed to not pressed)
            // Turn off all LEDs
            mraa_gpio_write(led1, 0);
            mraa_gpio_write(led2, 0);
            mraa_gpio_write(led3, 0);

            printf("Switch released, LEDs OFF\n");

            // Reset the press count if all LEDs have been lit up
            if (press_count >= 3) {
                press_count = 0;
            }

            usleep(200000); // 200ms delay
        }

        // Update the previous state
        prev_state = current_state;

        // Add a small delay for debounce
        usleep(50000); // 50ms delay
    }

    // Cleanup (unreachable in this loop)
    mraa_gpio_close(led1);
    mraa_gpio_close(led2);
    mraa_gpio_close(led3);
    mraa_gpio_close(switch1);

    return 0;
}

