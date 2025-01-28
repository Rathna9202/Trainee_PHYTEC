#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <mraa.h>
#include <mraa/uart.h>

// UART Configuration
#define UART_DEVICE "/dev/ttyS3"  // Adjust based on your UART device
#define UART_BAUDRATE 9600

int main() {
    // Initialize MRAA library
    if (mraa_init() != MRAA_SUCCESS) {
        fprintf(stderr, "Failed to initialize MRAA\n");
        return 1;
    }
    printf("MRAA initialized successfully\n");

    // Initialize UART
    mraa_uart_context uart = mraa_uart_init_raw(UART_DEVICE);
    if (!uart) {
        fprintf(stderr, "Failed to initialize UART\n");
        return 1;
    }

    // Configure UART settings
    if (mraa_uart_set_baudrate(uart, UART_BAUDRATE) != MRAA_SUCCESS) {
        fprintf(stderr, "Failed to set UART baudrate\n");
        mraa_uart_stop(uart);
        return 1;
    }
    mraa_uart_set_mode(uart, 8, MRAA_UART_PARITY_NONE, 1);  // 8N1 mode
    mraa_uart_set_flowcontrol(uart, 0, 0);                  // Disable flow control

    printf("UART initialized on %s with baudrate %d\n", UART_DEVICE, UART_BAUDRATE);

    // Main loop
    while (1) {
        char user_input[128];
        char recv_buffer[128];
        int bytes_written, bytes_read;

        // Prompt the user for input
        printf("Enter a message to send over UART (or type 'exit' to quit): ");
        if (fgets(user_input, sizeof(user_input), stdin) == NULL) {
            fprintf(stderr, "Error reading user input\n");
            continue;
        }

        // Remove the newline character from user input
        size_t len = strlen(user_input);
        if (len > 0 && user_input[len - 1] == '\n') {
            user_input[len - 1] = '\0';
        }

        // Exit the program if the user types "exit"
        if (strcmp(user_input, "exit") == 0) {
            printf("Exiting program...\n");
            break;
        }

        // Send the user input over UART
        bytes_written = mraa_uart_write(uart, user_input, strlen(user_input));
        if (bytes_written > 0) {
            printf("Data sent: %s (%d bytes)\n", user_input, bytes_written);
        } else {
            fprintf(stderr, "Failed to send data over UART\n");
            continue;
        }

        // Wait and receive the data
        usleep(500000);  // Wait for data to loop back (500ms)
        bytes_read = mraa_uart_read(uart, recv_buffer, sizeof(recv_buffer) - 1);
        if (bytes_read > 0) {
            recv_buffer[bytes_read] = '\0';  // Null-terminate the received string
            printf("Received data: %s\n", recv_buffer);
        } else {
            fprintf(stderr, "No data received over UART\n");
        }
    }

    // Cleanup
    mraa_uart_stop(uart);
    mraa_deinit();

    return 0;
}

