#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <mraa.h>
#include <mraa/uart.h>
#include <stdbool.h> 
int main() {
    char *portname = "/dev/ttyS3";
    mraa_uart_context uart;
    char input[100]; // Buffer for user input

    // Initialize UART
    uart = mraa_uart_init_raw(portname);
    if (uart == NULL) {
        fprintf(stderr, "Error initializing UART on %s\n", portname);
        return -1;
    }

    // Set UART parameters
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

    if (mraa_uart_set_flowcontrol(uart, false, false) != MRAA_SUCCESS) {
        fprintf(stderr, "Error setting flow control\n");
        mraa_uart_stop(uart);
        return -1;
    }

    printf("UART initialized successfully on %s\n", portname);

    while (1) {
        // Get input from user
        printf("Enter message: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0'; // Remove newline character

        // Transmit data
        if (mraa_uart_write(uart, input, strlen(input)) != strlen(input)) {
            fprintf(stderr, "Error writing to UART\n");
        } else {
            printf("Message sent: %s\n", input);
        }

        // Sleep for a short duration before the next iteration
        usleep(100000); // 100 ms
    }

    // Stop UART
    mraa_uart_stop(uart);

    return 0;
}

