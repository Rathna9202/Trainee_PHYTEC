#include <mraa/gpio.h>
#include <unistd.h> // For sleep()
#include <stdio.h>  // For printf()

int main() {
    // Initialize GPIO pins
    mraa_gpio_context gpio;  // LED pin
    mraa_gpio_context gpiob; // Button pin

    gpio = mraa_gpio_init(12);  // GPIO pin for the LED
    gpiob = mraa_gpio_init(13); // GPIO pin for the button

    if (gpio == NULL || gpiob == NULL) {
        fprintf(stderr, "Failed to initialize GPIO\n");
        return 1;
    }

    // Set LED pin as output and button pin as input
    mraa_gpio_dir(gpio, MRAA_GPIO_OUT);
    mraa_gpio_dir(gpiob, MRAA_GPIO_IN);

    while (1) {
        // Read the button state
        int button = mraa_gpio_read(gpiob);

        if (button == 0) {
            // Turn LED on when button is pressed
            mraa_gpio_write(gpio, 1);
	    printf("Button Pressed\n");
        } else {
            // Turn LED off when button is released
            mraa_gpio_write(gpio, 0);
	    printf("Button not Pressed\n");
        }

        // Add a small delay to debounce the button (optional)
        usleep(50000); // 50ms delay
    }

    // Cleanup (unreachable in this loop)
    mraa_gpio_close(gpio);
    mraa_gpio_close(gpiob);

    return 0;
}

