#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <mraa.h>
#include <mraa/uart.h>

void receive_data(mraa_uart_context uart);

int main() {
    char *portname = "/dev/ttyS3";
    mraa_uart_context uart;

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

    // Start receiving data
    receive_data(uart);

    // Stop UART
    mraa_uart_stop(uart);

    return 0;
}

void receive_data(mraa_uart_context uart) {
    char buf[256];

    while (1) {
        int rdlen = mraa_uart_read(uart, buf, sizeof(buf));
        if (rdlen > 0) {
            buf[rdlen] = '\0'; // Null-terminate the received data
            printf("Received: %s\n", buf);
        } else if (rdlen < 0) {
            fprintf(stderr, "Error reading from UART\n");
        }
    }
}
