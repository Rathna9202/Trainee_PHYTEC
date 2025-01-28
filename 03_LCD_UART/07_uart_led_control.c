#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <mraa.h>
#include <mraa/uart.h>

#define LED_PIN 61

void control_led(mraa_gpio_context led, mraa_uart_context uart);

int main() {
    char *portname = "/dev/ttyS3";
    mraa_uart_context uart;
    mraa_gpio_context led;

    // Initialize UART
    uart = mraa_uart_init_raw(portname);
    if (uart == NULL) {
        fprintf(stderr, "Error initializing UART on %s\n", portname);
        return -1;
    }

    if (mraa_uart_set_baudrate(uart, 115200) != MRAA_SUCCESS) {
        fprintf(stderr, "Error setting baud rate\n");
        mraa_uart_stop(uart);
        return -1;
    }

    if (mraa_uart_set_mode(uart, 8, MRAA_UART_PARITY_NONE, 1) != MRAA_SUCCESS) {
        fprintf(stderr, "Error setting UART mode\n");
        mraa_uart_stop(uart);
        return -1;
    }

    printf("UART initialized successfully on %s\n", portname);

    // Initialize GPIO for LED
    led = mraa_gpio_init(LED_PIN);
    if (led == NULL) {
        fprintf(stderr, "Error initializing GPIO for LED\n");
        mraa_uart_stop(uart);
        return -1;
    }

    mraa_gpio_dir(led, MRAA_GPIO_OUT);

    // Start controlling the LED
    control_led(led, uart);

    // Stop UART and GPIO (this is unreachable in the current design)
    mraa_uart_stop(uart);
    mraa_gpio_close(led);

    return 0;
}

void control_led(mraa_gpio_context led, mraa_uart_context uart) {
    char buf[1];

    while (1) {
        printf("Waiting for UART data...\n");
        int rdlen = mraa_uart_read(uart, buf, sizeof(buf));
        if (rdlen > 0) {
            buf[rdlen] = '\0'; // Null-terminate the received data

            printf("Received: %c\n", buf[0]);

            // Echo data back
            int wrlen = mraa_uart_write(uart, buf, rdlen);
            if (wrlen != rdlen) {
                fprintf(stderr, "Error writing to UART\n");
            }

            // Control the LED
            if (buf[0] == 'F' || buf[0] == 'f') {
                mraa_gpio_write(led, 1);
                printf("LED turned OFF\n");
            } else if (buf[0] == 'N' || buf[0] == 'n') {
                mraa_gpio_write(led, 0);
                printf("LED turned ON\n");
            } else {
                printf("Invalid input: %c\n", buf[0]);
            }
        } else if (rdlen < 0) {
            fprintf(stderr, "Error reading from UART\n");
        }

        usleep(100000); // Small delay to avoid overloading the CPU
    }
}

