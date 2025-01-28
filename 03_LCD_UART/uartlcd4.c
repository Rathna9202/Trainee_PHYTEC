#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <mraa.h>

// Define GPIO pins for LCD
#define LCD_RS_PIN 12
#define LCD_EN_PIN 13
#define LCD_D4_PIN 43
#define LCD_D5_PIN 53
#define LCD_D6_PIN 52
#define LCD_D7_PIN 51

#define port "/dev/ttyS3"

// GPIO contexts
mraa_gpio_context rs, en, d4, d5, d6, d7;
mraa_uart_context uart;

// Function prototypes
void LCD_Init();
void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_WriteString(const char *str);
void LCD_Clear(void);
void delay_ms(int ms);

int main() {
    // Initialize MRAA library
    if (mraa_init() != MRAA_SUCCESS) {
        fprintf(stderr, "Failed to initialize MRAA\n");
        return 1;
    }

    // Initialize LCD pins
    rs = mraa_gpio_init(LCD_RS_PIN);
    en = mraa_gpio_init(LCD_EN_PIN);
    d4 = mraa_gpio_init(LCD_D4_PIN);
    d5 = mraa_gpio_init(LCD_D5_PIN);
    d6 = mraa_gpio_init(LCD_D6_PIN);
    d7 = mraa_gpio_init(LCD_D7_PIN);

    if (!rs || !en || !d4 || !d5 || !d6 || !d7) {
        fprintf(stderr, "Failed to initialize LCD pins\n");
        return 1;
    }

    mraa_gpio_dir(rs, MRAA_GPIO_OUT);
    mraa_gpio_dir(en, MRAA_GPIO_OUT);
    mraa_gpio_dir(d4, MRAA_GPIO_OUT);
    mraa_gpio_dir(d5, MRAA_GPIO_OUT);
    mraa_gpio_dir(d6, MRAA_GPIO_OUT);
    mraa_gpio_dir(d7, MRAA_GPIO_OUT);

    // Initialize UART
    uart = mraa_uart_init_raw(port); // Use UART 0
    if (!uart) {
        fprintf(stderr, "Failed to initialize UART\n");
        return 1;
    }

    // Set UART parameters
    mraa_uart_set_baudrate(uart, 9600);
    mraa_uart_set_mode(uart, 8, MRAA_UART_PARITY_NONE, 1);
    mraa_uart_set_flowcontrol(uart, 0, 0);

    // Initialize the LCD
    LCD_Init();

    // Loopback message
    const char *message = "Hello Loopback UART!";
    char buffer[32] = {0};
    int len;

    while (1) {
        // Send data over UART
        mraa_uart_write(uart, message, strlen(message));

        // Wait for the message to loop back
        usleep(100000); // Wait for 100ms

        // Read the loopback data
        len = mraa_uart_read(uart, buffer, sizeof(buffer) - 1);

        if (len > 0) {
            buffer[len] = '\0'; // Null-terminate the received string
            LCD_Clear();
            LCD_WriteString(buffer);
        }

        usleep(1000000); // Wait 1 second before the next iteration
    }

    return 0;
}

// Function to initialize the LCD
void LCD_Init() {
    delay_ms(15);
    LCD_SendCommand(0x28); // 4-bit mode, 2 lines, 5x8 font
    delay_ms(5);
    LCD_SendCommand(0x0C); // Display ON, cursor OFF
    delay_ms(1);
    LCD_SendCommand(0x06); // Entry mode set: auto-increment, no shift
    delay_ms(1);
    LCD_Clear();
}

// Function to send a command to the LCD
void LCD_SendCommand(uint8_t cmd) {
    mraa_gpio_write(rs, 0); // RS = 0 for command
    LCD_Send(cmd);
}

// Function to send data to the LCD
void LCD_SendData(uint8_t data) {
    mraa_gpio_write(rs, 1); // RS = 1 for data
    LCD_Send(data);
}

// Helper function to send 4-bit data
void LCD_Send(uint8_t value) {
    // Send higher nibble
    mraa_gpio_write(d4, (value >> 4) & 1);
    mraa_gpio_write(d5, (value >> 5) & 1);
    mraa_gpio_write(d6, (value >> 6) & 1);
    mraa_gpio_write(d7, (value >> 7) & 1);
    mraa_gpio_write(en, 1);
    delay_ms(1);
    mraa_gpio_write(en, 0);

    // Send lower nibble
    mraa_gpio_write(d4, value & 1);
    mraa_gpio_write(d5, (value >> 1) & 1);
    mraa_gpio_write(d6, (value >> 2) & 1);
    mraa_gpio_write(d7, (value >> 3) & 1);
    mraa_gpio_write(en, 1);
    delay_ms(1);
    mraa_gpio_write(en, 0);

    delay_ms(2); // Wait for the LCD to process the command/data
}

// Function to write a string to the LCD
void LCD_WriteString(const char *str) {
    while (*str) {
        LCD_SendData(*str++);
    }
}

// Function to clear the LCD screen
void LCD_Clear(void) {
    LCD_SendCommand(0x01); // Clear display command
    delay_ms(2); // Wait for the LCD to process the clear command
}

// Delay function (in milliseconds)
void delay_ms(int ms) {
    usleep(ms * 1000);
}
