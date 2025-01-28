#include <mraa/aio.h>
#include <mraa/pwm.h>
#include <stdio.h>
#include <unistd.h>

#define POTENTIOMETER_PIN 6   // Analog pin connected to the potentiometer
#define LED_PWM_PIN 72        // PWM pin connected to the LED
#define MAX_ADC_VALUE 1023.0  // Maximum ADC value for the potentiometer

int main() {
    // Initialize MRAA
    mraa_init();
    printf("MRAA Version: %s\n", mraa_get_version());

    // Initialize analog input (potentiometer)
    mraa_aio_context potentiometer = mraa_aio_init(POTENTIOMETER_PIN);
    if (potentiometer == NULL) {
        fprintf(stderr, "Error initializing analog input on pin %d\n", POTENTIOMETER_PIN);
        return -1;
    }

    // Initialize PWM output (LED)
    mraa_pwm_context led_pwm = mraa_pwm_init(LED_PWM_PIN);
    if (led_pwm == NULL) {
        fprintf(stderr, "Error initializing PWM on pin %d\n", LED_PWM_PIN);
        return -1;
    }

    // Set PWM period and enable it
    if (mraa_pwm_period(led_pwm, 0.02) != MRAA_SUCCESS) { // 20ms period
        fprintf(stderr, "Failed to set PWM period\n");
        return -1;
    }
    if (mraa_pwm_enable(led_pwm, 1) != MRAA_SUCCESS) {
        fprintf(stderr, "Failed to enable PWM\n");
        return -1;
    }

    printf("Adjust the potentiometer to change the LED intensity.\n");

    while (1) {
        // Read the potentiometer value (0 to MAX_ADC_VALUE)
        int pot_value = mraa_aio_read(potentiometer);
        if (pot_value < 0) {
            fprintf(stderr, "Error reading analog value\n");
            continue;
        }

        // Map the potentiometer value to a duty cycle (0.0 to 1.0)
        float duty_cycle = pot_value / MAX_ADC_VALUE;

        // Set the PWM duty cycle for the LED
        mraa_pwm_write(led_pwm, duty_cycle);

        // Print the current duty cycle
        printf("Potentiometer Value: %d, Duty Cycle: %.2f\n", pot_value, duty_cycle);

        usleep(100000); // Delay for 100ms
    }

    // Clean up
    mraa_aio_close(potentiometer);
    mraa_pwm_close(led_pwm);

    return 0;
}

