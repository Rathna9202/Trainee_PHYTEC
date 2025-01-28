#include <stdio.h>
#include <unistd.h>
#include <mraa.h>

// Define GPIO pins for LCD (adjust GPIO pin numbers based on your setup)
#define LCD_RS_PIN    12   // Register Select pin
#define LCD_EN_PIN    13   // Enable pin
#define LCD_D4_PIN    43   // Data pin 4
#define LCD_D5_PIN    53   // Data pin 5
#define LCD_D6_PIN    52   // Data pin 6
#define LCD_D7_PIN    51   // Data pin 7

// GPIO contexts
mraa_gpio_context rs, en;
mraa_gpio_context d[4];

// Function prototypes
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

    // Initialize control pins (RS, EN)
    rs = mraa_gpio_init(LCD_RS_PIN);
    en = mraa_gpio_init(LCD_EN_PIN);

    if (!rs || !en) {
        fprintf(stderr, "Failed to initialize control pins\n");
        return 1;
    }

    // Set control pins to output
    mraa_gpio_dir(rs, MRAA_GPIO_OUT);
    mraa_gpio_dir(en, MRAA_GPIO_OUT);

    printf("Control pins initialized\n");

    // Initialize data pins (D4-D7) with correct mapping
    int data_pins[4] = {LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN};

    for (int i = 0; i < 4; i++) {
        d[i] = mraa_gpio_init(data_pins[i]);
        if (!d[i]) {
            fprintf(stderr, "Failed to initialize data pin D%d (GPIO %d)\n", i + 4, data_pins[i]);
            return 1;
        }
        mraa_gpio_dir(d[i], MRAA_GPIO_OUT);
    }

    printf("Data pins initialized\n");

    // Initialize the LCD
    printf("Initializing LCD...\n");
    LCD_Init();
    printf("LCD initialized\n");

    // Display a string on the LCD
    printf("Displaying message on LCD...\n");
    LCD_WriteString("Hello, RuggedBoard!");
    printf("Message displayed\n");

    // Infinite loop to keep the LCD displaying the string
    while (1) {
        usleep(500000); // Sleep for 0.5 seconds to avoid high CPU usage
    }

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
    // Set RS pin for command/data mode
    mraa_gpio_write(rs, mode);

    // Set data pins (D4 to D7)
    for (int i = 0; i < 4; i++) {
        mraa_gpio_write(d[i], (nibble >> i) & 0x01);
    }

    // Generate a high-to-low pulse on the EN pin
    mraa_gpio_write(en, 1);
    delay_ms(1); // Small delay to ensure the LCD registers the data
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
