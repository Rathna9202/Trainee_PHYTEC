#include <stdio.h>
#include <mraa.h>
#include <unistd.h>

#define DIGITAL_PIN 12   // Pin connected to KY-037 DO (Digital Output)
#define ANALOG_PIN 6    // Pin connected to KY-037 AO (Analog Output)

int main() {
    // Initialize MRAA
    mraa_init();
    printf("MRAA Version: %s\n", mraa_get_version());

    // Initialize the digital input for DO
    mraa_gpio_context digital_pin;
    digital_pin = mraa_gpio_init(DIGITAL_PIN);
    if (digital_pin == NULL) {
        fprintf(stderr, "Error initializing GPIO %d\n", DIGITAL_PIN);
        return -1;
    }
    mraa_gpio_dir(digital_pin, MRAA_GPIO_IN);

    // Initialize the analog input for AO
    mraa_aio_context analog_pin;
    analog_pin = mraa_aio_init(ANALOG_PIN);
    if (analog_pin == NULL) {
        fprintf(stderr, "Error initializing AIO %d\n", ANALOG_PIN);
        return -1;
    }

    while (1) {
        // Read digital output
        int sound_detected = mraa_gpio_read(digital_pin);
        if (sound_detected == 1) {
            printf("Sound detected! (Digital Output HIGH)\n");
        } else {
            printf("No sound detected. (Digital Output LOW)\n");
        }

        // Read analog output
        int analog_value = mraa_aio_read(analog_pin);
        if (analog_value < 0) {
            fprintf(stderr, "Error reading analog value\n");
        } else {
            printf("Analog Sound Level: %d\n", analog_value);
        }

        usleep(500000); // Delay for 500 ms
    }

    // Clean up
    mraa_gpio_close(digital_pin);
    mraa_aio_close(analog_pin);

    return 0;
}
