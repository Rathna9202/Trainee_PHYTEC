#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <mraa.h>
#include <mraa/uart.h>

// UART Configuration
#define UART_DEVICE "/dev/ttyS3"  // Adjust based on your UART device
#define UART_BAUDRATE 9600

// Define GPIO pins for LCD
#define LCD_RS_PIN    12   // Register Select pin
#define LCD_EN_PIN    13   // Enable pin
#define LCD_D4_PIN    43   // Data pin 4
#define LCD_D5_PIN    53   // Data pin 5
#define LCD_D6_PIN    52   // Data pin 6
#define LCD_D7_PIN    51   // Data pin 7

// GPIO contexts
mraa_gpio_context rs, en;
mraa_gpio_context d[4];

// Function prototypes for LCD
void LCD_Init();
void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_SendNibble(uint8_t nibble, uint8_t mode);
void LCD_WriteString(const char* str);
void LCD_Clear(void);
void delay_ms(int ms);

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

    // Initialize LCD
    rs = mraa_gpio_init(LCD_RS_PIN);
    en = mraa_gpio_init(LCD_EN_PIN);

    int data_pins[4] = {LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN};
    for (int i = 0; i < 4; i++) {
        d[i] = mraa_gpio_init(data_pins[i]);
        mraa_gpio_dir(d[i], MRAA_GPIO_OUT);
    }

    mraa_gpio_dir(rs, MRAA_GPIO_OUT);
    mraa_gpio_dir(en, MRAA_GPIO_OUT);

    LCD_Init();
    printf("LCD initialized successfully\n");

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

            // Display received data on the LCD
            LCD_Clear();
            LCD_WriteString(recv_buffer);
        } else {
            fprintf(stderr, "No data received over UART\n");
        }
    }

    // Cleanup
    mraa_uart_stop(uart);
    mraa_deinit();

    return 0;
}

// Function to initialize the LCD
void LCD_Init() {
    delay_ms(15); // Wait for the LCD to power up

    // Send initialization commands in 4-bit mode
    LCD_SendNibble(0x03, 0); // Function set (8-bit mode)
    delay_ms(5);
    LCD_SendNibble(0x03, 0);
    delay_ms(5);
    LCD_SendNibble(0x03, 0);
    delay_ms(1);
    LCD_SendNibble(0x02, 0); // Switch to 4-bit mode

    // Complete initialization with standard commands
    LCD_SendCommand(0x28); // Function set: 4-bit mode, 2-line display
    LCD_SendCommand(0x0C); // Display ON, cursor OFF, blink OFF
    LCD_SendCommand(0x06); // Entry mode set: Auto-increment, no shift
    LCD_Clear();           // Clear display
}

// Function to send a command to the LCD
void LCD_SendCommand(uint8_t cmd) {
    LCD_SendNibble(cmd >> 4, 0); // Send upper nibble (command mode)
    LCD_SendNibble(cmd & 0x0F, 0); // Send lower nibble
}

// Function to send data to the LCD
void LCD_SendData(uint8_t data) {
    LCD_SendNibble(data >> 4, 1); // Send upper nibble (data mode)
    LCD_SendNibble(data & 0x0F, 1); // Send lower nibble
}

// Helper function to send a nibble (4 bits)
void LCD_SendNibble(uint8_t nibble, uint8_t mode) {
    mraa_gpio_write(rs, mode); // Set RS pin for command/data mode

    // Set data pins (D4 to D7)
    for (int i = 0; i < 4; i++) {
        mraa_gpio_write(d[i], (nibble >> i) & 0x01);
    }

    mraa_gpio_write(en, 1); // Generate a high-to-low pulse on EN pin
    delay_ms(1);
    mraa_gpio_write(en, 0);

    delay_ms(1); // Wait for the LCD to process the command/data
}

// Function to write a string to the LCD
void LCD_WriteString(const char* str) {
    while (*str) {
        LCD_SendData(*str++);
    }
}

// Function to clear the LCD screen
void LCD_Clear(void) {
    LCD_SendCommand(0x01); // Clear command
    delay_ms(2); // Wait for the LCD to process the clear command
}

// Delay function (in milliseconds)
void delay_ms(int ms) {
    usleep(ms * 1000); // Delay in milliseconds
}

