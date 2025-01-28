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

    int led_state = 0;      // Initial state of the LED (OFF)
    int last_button_state = 1; // Button state in the previous iteration (released)

    while (1) {
        // Read the button state
        int button_state = mraa_gpio_read(gpiob);

        if (button_state == 0 && last_button_state == 1) {
            // Button was just pressed, toggle the LED
            led_state = !led_state;
            mraa_gpio_write(gpio, led_state);
            printf("Button Pressed, LED is now %s\n", led_state ? "ON" : "OFF");
        }

        // Update the last button state
        last_button_state = button_state;

        // Add a small delay to debounce the button (optional)
        usleep(50000); // 50ms delay
    }

    // Cleanup (unreachable in this loop)
    mraa_gpio_close(gpio);
    mraa_gpio_close(gpiob);

    return 0;
}

