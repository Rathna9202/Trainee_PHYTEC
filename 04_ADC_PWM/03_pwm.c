#include <mraa/pwm.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    int pwm_pin = 72; // Correct PWM pin for the RuggedBoard
    float period_sec = 0.02; // 20ms period
    float duty_cycle = 0.5; // 50% duty cycle

    // Initialize PWM
    mraa_pwm_context pwm = mraa_pwm_init(pwm_pin);
    if (pwm == NULL) {
        printf("Failed to initialize PWM on pin %d\n", pwm_pin);
        return 1;
    }

    // Set PWM period
    if (mraa_pwm_period(pwm, period_sec) != MRAA_SUCCESS) {
        printf("Failed to set PWM period\n");
        return 1;
    }

    // Enable PWM
    if (mraa_pwm_enable(pwm, 1) != MRAA_SUCCESS) {
        printf("Failed to enable PWM\n");
        return 1;
    }

    // Generate PWM signal
    while (1) {
        mraa_pwm_write(pwm, duty_cycle); // Set duty cycle
        printf("PWM duty cycle: %.2f\n", duty_cycle);
        duty_cycle += 0.1; // Increment duty cycle
        if (duty_cycle > 1.0) {
            duty_cycle = 0.0; // Reset if it exceeds 100%
        }
        usleep(100000); // 100ms delay
    }

    // Close PWM (unreachable due to infinite loop)
    mraa_pwm_close(pwm);
    return 0;
}

