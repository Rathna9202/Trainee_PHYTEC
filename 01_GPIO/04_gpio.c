#include <mraa/gpio.h>
#include <unistd.h> // For sleep()
#include <stdio.h>  // For printf()

int main() {
    // Initialize GPIO pins for LEDs
    mraa_gpio_context led1 = mraa_gpio_init(36); // LED 1 pin
    mraa_gpio_context led2 = mraa_gpio_init(12); // LED 2 pin
    mraa_gpio_context led3 = mraa_gpio_init(40); // LED 3 pin

    // Initialize GPIO pins for switches
    mraa_gpio_context switch1 = mraa_gpio_init(13); // Switch 1 pin
    mraa_gpio_context switch2 = mraa_gpio_init(37); // Switch 2 pin
    mraa_gpio_context switch3 = mraa_gpio_init(39); // Switch 3 pin

    if (led1 == NULL || led2 == NULL || led3 == NULL || 
        switch1 == NULL || switch2 == NULL || switch3 == NULL) {
        fprintf(stderr, "Failed to initialize GPIO\n");
        return 1;
    }

    // Set LED pins as output
    mraa_gpio_dir(led1, MRAA_GPIO_OUT);
    mraa_gpio_dir(led2, MRAA_GPIO_OUT);
    mraa_gpio_dir(led3, MRAA_GPIO_OUT);

    // Set switch pins as input
    mraa_gpio_dir(switch1, MRAA_GPIO_IN);
    mraa_gpio_dir(switch2, MRAA_GPIO_IN);
    mraa_gpio_dir(switch3, MRAA_GPIO_IN);

    while (1) {
        // Read the current state of each switch
        int state1 = mraa_gpio_read(switch1);
        int state2 = mraa_gpio_read(switch2);
        int state3 = mraa_gpio_read(switch3);

        // Default state: Turn off all LEDs
        mraa_gpio_write(led1, 0);
        mraa_gpio_write(led2, 0);
        mraa_gpio_write(led3, 0);

        // Check if switch 1 is pressed
        if (state1 == 0) {
            mraa_gpio_write(led1, 1); // Turn on LED 1
            printf("Switch 1 Pressed, LED 1 ON\n");
        }

        // Check if switch 2 is pressed
        if (state2 == 0) {
            mraa_gpio_write(led1, 1); // Turn on LED 1
            mraa_gpio_write(led2, 1); // Turn on LED 2
            printf("Switch 2 Pressed, LED 1 and LED 2 ON\n");
        }

        // Check if switch 3 is pressed
        if (state3 == 0) {
            mraa_gpio_write(led1, 1); // Turn on LED 1
            mraa_gpio_write(led2, 1); // Turn on LED 2
            mraa_gpio_write(led3, 1); // Turn on LED 3
            printf("Switch 3 Pressed, LED 1, LED 2, and LED 3 ON\n");
        }

        // Add a small delay to debounce the switches
        usleep(50000); // 50ms delay
    }

    // Cleanup (unreachable in this loop)
    mraa_gpio_close(led1);
    mraa_gpio_close(led2);
    mraa_gpio_close(led3);
    mraa_gpio_close(switch1);
    mraa_gpio_close(switch2);
    mraa_gpio_close(switch3);

    return 0;
}

