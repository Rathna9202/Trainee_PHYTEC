#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mraa.h>
#include <mraa/uart.h>

// Define LCD GPIO pins (adjust these based on your hardware setup)
#define LCD_RS_PIN    12
#define LCD_RW_PIN    48
#define LCD_EN_PIN    13
#define LCD_D0_PIN    36
#define LCD_D1_PIN    37
#define LCD_D2_PIN    40
#define LCD_D3_PIN    39
#define LCD_D4_PIN    43
#define LCD_D5_PIN    53
#define LCD_D6_PIN    52
#define LCD_D7_PIN    51

// UART configuration
#define UART_PORT "/dev/ttyS3"
#define UART_BAUDRATE 9600

// LCD GPIO contexts
mraa_gpio_context rs, rw, en;
mraa_gpio_context d[8];

// Function prototypes
void LCD_Init();
void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_WriteString(const char* str);
void LCD_Clear();
void delay_ms(int ms);

int main() {
    // Initialize MRAA
    if (mraa_init() != MRAA_SUCCESS) {
        fprintf(stderr, "Failed to initialize MRAA.\n");
        return 1;
    }

    // UART setup
    mraa_uart_context uart = mraa_uart_init_raw(UART_PORT);
    if (uart == NULL) {
        fprintf(stderr, "Failed to initialize UART on port %s.\n", UART_PORT);
        return 1;
    }

    if (mraa_uart_set_baudrate(uart, UART_BAUDRATE) != MRAA_SUCCESS) {
        fprintf(stderr, "Failed to set UART baudrate.\n");
        mraa_uart_stop(uart);
        return 1;
    }

    mraa_uart_set_mode(uart, 8, MRAA_UART_PARITY_NONE, 1);
    mraa_uart_set_flowcontrol(uart, 0, 0);
    printf("UART initialized on port %s with baudrate %d.\n", UART_PORT, UART_BAUDRATE);

    // LCD setup
    printf("Initializing LCD...\n");
    LCD_Init();
    printf("LCD initialized successfully.\n");

    // Buffer for UART data
    char rx_buffer[256];
    int rx_len;

    // Main loop
    while (1) {
        // Clear the buffer
        memset(rx_buffer, 0, sizeof(rx_buffer));

        // Read data from UART
        rx_len = mraa_uart_read(uart, rx_buffer, sizeof(rx_buffer) - 1); // Leave space for null terminator
        if (rx_len > 0) {
            rx_buffer[rx_len] = '\0'; // Null-terminate the received string
            printf("Received: %s\n", rx_buffer);

            // Display received data on LCD
            LCD_Clear();
            LCD_WriteString(rx_buffer);
        } else if (rx_len < 0) {
            fprintf(stderr, "Error reading from UART.\n");
        }

        usleep(500000); // Small delay to avoid busy-waiting
    }

    // Cleanup
    mraa_uart_stop(uart);
    mraa_deinit();
    return 0;
}

// LCD Initialization
void LCD_Init() {
    // Initialize control pins
    rs = mraa_gpio_init(LCD_RS_PIN);
    rw = mraa_gpio_init(LCD_RW_PIN);
    en = mraa_gpio_init(LCD_EN_PIN);
    mraa_gpio_dir(rs, MRAA_GPIO_OUT);
    mraa_gpio_dir(rw, MRAA_GPIO_OUT);
    mraa_gpio_dir(en, MRAA_GPIO_OUT);

    // Initialize data pins
    int data_pins[8] = {LCD_D0_PIN, LCD_D1_PIN, LCD_D2_PIN, LCD_D3_PIN,
                        LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN};
    for (int i = 0; i < 8; i++) {
        d[i] = mraa_gpio_init(data_pins[i]);
        mraa_gpio_dir(d[i], MRAA_GPIO_OUT);
    }

    // LCD initialization sequence
    delay_ms(15);
    LCD_SendCommand(0x38); // Function set: 8-bit mode, 2-line display
    delay_ms(5);
    LCD_SendCommand(0x0C); // Display ON, cursor OFF, blink OFF
    delay_ms(1);
    LCD_SendCommand(0x06); // Entry mode: auto-increment, no shift
    delay_ms(1);
    LCD_SendCommand(0x01); // Clear display
    delay_ms(2);
}

// Send a command to the LCD
void LCD_SendCommand(uint8_t cmd) {
    mraa_gpio_write(rs, 0); // Command mode
    mraa_gpio_write(rw, 0); // Write mode

    for (int i = 0; i < 8; i++) {
        mraa_gpio_write(d[i], (cmd >> i) & 0x01);
    }

    // Pulse enable pin
    mraa_gpio_write(en, 1);
    delay_ms(1);
    mraa_gpio_write(en, 0);
    delay_ms(1);
}

// Send data to the LCD
void LCD_SendData(uint8_t data) {
    mraa_gpio_write(rs, 1); // Data mode
    mraa_gpio_write(rw, 0); // Write mode

    for (int i = 0; i < 8; i++) {
        mraa_gpio_write(d[i], (data >> i) & 0x01);
    }

    // Pulse enable pin
    mraa_gpio_write(en, 1);
    delay_ms(1);
    mraa_gpio_write(en, 0);
    delay_ms(1);
}

// Write a string to the LCD
void LCD_WriteString(const char* str) {
    while (*str) {
        LCD_SendData(*str++);
    }
}

// Clear the LCD display
void LCD_Clear() {
    LCD_SendCommand(0x01); // Clear display command
    delay_ms(2);
}

// Millisecond delay function
void delay_ms(int ms) {
    usleep(ms * 1000);
}

