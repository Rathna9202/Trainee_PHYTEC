#include <stdio.h>
#include <unistd.h>
#include <mraa.h>

// Define GPIO pins for LCD (adjust GPIO pin numbers based on your setup)
#define LCD_RS_PIN    12   // Register Select pin
#define LCD_RW_PIN    48   // Read/Write pin (set to ground for write mode)
#define LCD_EN_PIN    13   // Enable pin
#define LCD_D0_PIN    36   // Data pin 0
#define LCD_D1_PIN    37   // Data pin 1
#define LCD_D2_PIN    40   // Data pin 2
#define LCD_D3_PIN    39   // Data pin 3
#define LCD_D4_PIN    43   // Data pin 4
#define LCD_D5_PIN    53   // Data pin 5
#define LCD_D6_PIN    52   // Data pin 6
#define LCD_D7_PIN    51   // Data pin 7

// GPIO contexts
mraa_gpio_context rs, rw, en;
mraa_gpio_context d[8];

// Function prototypes
void LCD_Init();
void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_Send(uint8_t value, uint8_t mode);
void LCD_WriteString(const char* str);
void LCD_Clear(void);
void delay_ms(int ms);
void LCD_ScrollMessage(const char* str, int delay_ms);

// Main function
int main() {
    // Initialize MRAA library
    if (mraa_init() != MRAA_SUCCESS) {
        fprintf(stderr, "Failed to initialize MRAA\n");
        return 1;
    }

    printf("MRAA initialized successfully\n");

    // Initialize control pins (RS, RW, EN)
    rs = mraa_gpio_init(LCD_RS_PIN);
    rw = mraa_gpio_init(LCD_RW_PIN);
    en = mraa_gpio_init(LCD_EN_PIN);

    if (!rs || !rw || !en) {
        fprintf(stderr, "Failed to initialize control pins\n");
        return 1;
    }

    // Set control pins to output
    mraa_gpio_dir(rs, MRAA_GPIO_OUT);
    mraa_gpio_dir(rw, MRAA_GPIO_OUT);
    mraa_gpio_dir(en, MRAA_GPIO_OUT);

    printf("Control pins initialized\n");

    // Initialize data pins (D0-D7)
    int data_pins[8] = {LCD_D0_PIN, LCD_D1_PIN, LCD_D2_PIN, LCD_D3_PIN, 
                        LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN};

    for (int i = 0; i < 8; i++) {
        d[i] = mraa_gpio_init(data_pins[i]);
        if (!d[i]) {
            fprintf(stderr, "Failed to initialize data pin D%d (GPIO %d)\n", i, data_pins[i]);
            return 1;
        }
        mraa_gpio_dir(d[i], MRAA_GPIO_OUT);
    }

    printf("Data pins initialized\n");

    // Initialize the LCD
    printf("Initializing LCD...\n");
    LCD_Init();
    printf("LCD initialized\n");

    // Scroll a message on the LCD
    printf("Scrolling message on LCD...\n");
    LCD_ScrollMessage("Hello, RuggedBoard!", 500); // 500 ms delay between shifts
    printf("Message scrolled\n");

    // Infinite loop to keep the program running
    while (1) {
        usleep(500000); // Sleep for 0.5 seconds to avoid high CPU usage
    }

    return 0;
}

// Function to initialize the LCD
void LCD_Init() {
    delay_ms(15); // Wait for the LCD to power up
    LCD_SendCommand(0x38); // Function set: 8-bit mode, 2-line display
    printf("Command 0x38 sent\n");
    delay_ms(5);
    LCD_SendCommand(0x0C); // Display ON, cursor OFF, blink OFF
    printf("Command 0x0C sent\n");
    delay_ms(1);
    LCD_SendCommand(0x06); // Entry mode set: Auto-increment, no shift
    printf("Command 0x06 sent\n");
    delay_ms(1);
    LCD_SendCommand(0x01); // Clear display
    printf("Command 0x01 sent (Clear Display)\n");
    delay_ms(2); // Wait for the LCD to process the clear display command
}

// Function to send a command to the LCD
void LCD_SendCommand(uint8_t cmd) {
    printf("Sending command: 0x%02X\n", cmd);
    LCD_Send(cmd, 0); // 0 indicates command mode
}

// Function to send data to the LCD
void LCD_SendData(uint8_t data) {
    printf("Sending data: 0x%02X (%c)\n", data, data);
    LCD_Send(data, 1); // 1 indicates data mode
}

// Helper function to send either command or data
void LCD_Send(uint8_t value, uint8_t mode) {
    // Set RS pin for command/data mode
    mraa_gpio_write(rs, mode);

    // Set RW pin to low (write mode)
    mraa_gpio_write(rw, 0);

    // Set data pins (D0 to D7)
    for (int i = 0; i < 8; i++) {
        mraa_gpio_write(d[i], (value >> i) & 0x01);
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

// Function to scroll the message on the LCD
void LCD_ScrollMessage(const char* str, int delay_ms) {
    int len = 0;
    while (str[len] != '\0') len++; // Find the length of the string
    
    // Display the string and scroll
    for (int i = 0; i < len + 16; i++) { // Loop through for scrolling
        LCD_Clear(); // Clear the display for each scroll
        if (i < len) {
            LCD_WriteString(&str[i]);
        } else {
            LCD_WriteString("                "); // Blank spaces to scroll
        }
        usleep(delay_ms * 1000); // Wait before next shift
    }
}

// Delay function (in milliseconds)
void delay_ms(int ms) {
    usleep(ms * 1000); // Delay in milliseconds
}
