#include <mraa/gpio.h>
#include <unistd.h> // For sleep()
#include <stdio.h>  // For printf()

int main() {
    // Initialize GPIO pins
    mraa_gpio_context led = mraa_gpio_init(36);   // LED pin
    mraa_gpio_context switch1 = mraa_gpio_init(13); // Switch 1 pin
    mraa_gpio_context switch2 = mraa_gpio_init(37); // Switch 2 pin

    if (led == NULL || switch1 == NULL || switch2 == NULL) {
        fprintf(stderr, "Failed to initialize GPIO\n");
        return 1;
    }

    // Set LED pin as output
    mraa_gpio_dir(led, MRAA_GPIO_OUT);

    // Set switch pins as input
    mraa_gpio_dir(switch1, MRAA_GPIO_IN);
    mraa_gpio_dir(switch2, MRAA_GPIO_IN);

    while (1) {
        // Read the state of switches
        int state1 = mraa_gpio_read(switch1);
        int state2 = mraa_gpio_read(switch2);

        if (state1 == 0) {
            // If switch 1 is pressed, turn on the LED
            mraa_gpio_write(led, 1);
            printf("Switch 1 Pressed, LED ON\n");
        }

        if (state2 == 0) {
            // If switch 2 is pressed, turn off the LED
            mraa_gpio_write(led, 0);
            printf("Switch 2 Pressed, LED OFF\n");
        }

        // Add a small delay to debounce the switches
        usleep(50000); // 50ms delay
    }

    // Cleanup (unreachable in this loop)
    mraa_gpio_close(led);
    mraa_gpio_close(switch1);
    mraa_gpio_close(switch2);

    return 0;
}

