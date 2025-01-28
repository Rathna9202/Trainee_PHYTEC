#include <mraa/gpio.h>
#include <unistd.h> // For usleep()
#include <stdio.h>  // For printf()

int main() {
    // Initialize GPIO pins
    mraa_gpio_context led1 = mraa_gpio_init(61);    // LED 1 pin
    mraa_gpio_context led2 = mraa_gpio_init(62);    // LED 2 pin
    mraa_gpio_context switch1 = mraa_gpio_init(35); // Switch pin

    if (led1 == NULL || led2 == NULL || switch1 == NULL) {
        fprintf(stderr, "Failed to initialize GPIO\n");
        return 1;
    }

    // Set LED pins as output
    mraa_gpio_dir(led1, MRAA_GPIO_OUT);
    mraa_gpio_dir(led2, MRAA_GPIO_OUT);

    // Set switch pin as input
    mraa_gpio_dir(switch1, MRAA_GPIO_IN);

    int press_count = 0;      // Counter for the number of switch presses
    int prev_state = 1;       // Store the previous state of the switch (1 = not pressed)

    // Start with LEDs OFF
    mraa_gpio_write(led1, 0);
    mraa_gpio_write(led2, 0);

    while (1) {
        // Read the current state of the switch
        int current_state = mraa_gpio_read(switch1);

        if (current_state == 0 && prev_state == 1) {
            // Switch pressed (state changed from not pressed to pressed)
            press_count++;
            printf("Switch pressed %d time(s)\n", press_count);

            // Toggle LED1 and LED2
            int led1_toggles = press_count;        // Number of toggles for LED1
            int led2_toggles = press_count * 3;    // Number of toggles for LED2

            // Toggle LED1
            for (int i = 0; i < led1_toggles; i++) {
                mraa_gpio_write(led1, 1);  // Turn LED1 ON
                usleep(500000);            // 500ms delay
                mraa_gpio_write(led1, 0);  // Turn LED1 OFF
                usleep(500000);            // 500ms delay
            }

            // Toggle LED2
            for (int i = 0; i < led2_toggles; i++) {
                mraa_gpio_write(led2, 1);  // Turn LED2 ON
                usleep(250000);            // 250ms delay
                mraa_gpio_write(led2, 0);  // Turn LED2 OFF
                usleep(250000);            // 250ms delay
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
    mraa_gpio_close(led1);
    mraa_gpio_close(led2);
    mraa_gpio_close(switch1);

    return 0;
}

