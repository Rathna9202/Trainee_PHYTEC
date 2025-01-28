#include <stdio.h>
#include <unistd.h>
#include <mraa.h>

// Define the LCD's I2C address (run i2cdetect to confirm the address; typically 0x27 or 0x3F)
#define LCD_I2C_ADDR    0x27

// LCD Commands
#define LCD_CLEAR           0x01
#define LCD_HOME            0x02
#define LCD_ENTRY_MODE      0x06
#define LCD_DISPLAY_CTRL    0x0C
#define LCD_FUNCTION_SET    0x28
#define LCD_SET_CURSOR      0x80

mraa_i2c_context i2c;

// Function Prototypes
void LCD_Init();
void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_SendNibble(uint8_t nibble, uint8_t rs);
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

    // Initialize I2C communication
    i2c = mraa_i2c_init(0); // I2C bus 0 (update if using a different bus)
    if (i2c == NULL) {
        fprintf(stderr, "Failed to initialize I2C\n");
        return 1;
    }

    // Set the LCD's I2C address
    mraa_i2c_address(i2c, LCD_I2C_ADDR);
    printf("I2C initialized and LCD address set\n");

    // Initialize the LCD
    printf("Initializing LCD...\n");
    LCD_Init();
    printf("LCD initialized\n");

    // Display a message
    printf("Displaying message on LCD...\n");
    LCD_Clear();
    LCD_WriteString("Hello, Rugged!");
    printf("Message displayed\n");

    // Infinite loop to keep the program running
    while (1) {
        usleep(500000); // Sleep to avoid high CPU usage
    }

    return 0;
}

// Function to initialize the LCD
void LCD_Init() {
    delay_ms(50); // Wait for the LCD to power up

    // Send initialization commands
    LCD_SendNibble(0x30, 0); // Wake up sequence
    delay_ms(5);
    LCD_SendNibble(0x30, 0); // Wake up sequence
    delay_ms(1);
    LCD_SendNibble(0x30, 0); // Wake up sequence
    delay_ms(1);
    LCD_SendNibble(0x20, 0); // Set to 4-bit mode

    LCD_SendCommand(LCD_FUNCTION_SET);    // 4-bit mode, 2-line display
    LCD_SendCommand(LCD_DISPLAY_CTRL);    // Display ON, cursor OFF
    LCD_SendCommand(LCD_CLEAR);           // Clear display
    LCD_SendCommand(LCD_ENTRY_MODE);      // Auto-increment cursor
}

// Function to send a command to the LCD
void LCD_SendCommand(uint8_t cmd) {
    uint8_t highNibble = (cmd & 0xF0);  // Get the high nibble of the command
    uint8_t lowNibble = ((cmd << 4) & 0xF0);  // Get the low nibble by shifting left by 4 bits

    // Send the high nibble (command mode)
    LCD_SendNibble(highNibble, 0);  // RS=0 for command mode
    // Send the low nibble (command mode)
    LCD_SendNibble(lowNibble, 0);  // RS=0 for command mode
}

// Function to send data to the LCD
void LCD_SendData(uint8_t data) {
    uint8_t highNibble = (data & 0xF0);  // Get the high nibble of the data
    uint8_t lowNibble = ((data << 4) & 0xF0);  // Get the low nibble

    // Send the high nibble (data mode)
    LCD_SendNibble(highNibble, 1);  // RS=1 for data mode
    // Send the low nibble (data mode)
    LCD_SendNibble(lowNibble, 1);  // RS=1 for data mode
}

// Function to send a nibble (4 bits) to the LCD
void LCD_SendNibble(uint8_t nibble, uint8_t rs) {
    uint8_t data = nibble | (rs ? 0x01 : 0x00) | 0x08; // Set RS and backlight (BL=1)

    mraa_i2c_write_byte(i2c, data | 0x04); // Enable=1
    delay_ms(1);
    mraa_i2c_write_byte(i2c, data & ~0x04); // Enable=0
    delay_ms(1);
}

// Function to write a string to the LCD
void LCD_WriteString(const char* str) {
    while (*str) {
        LCD_SendData(*str++);
    }
}

// Function to clear the LCD screen
void LCD_Clear(void) {
    LCD_SendCommand(LCD_CLEAR); // Send clear command
    delay_ms(2); // Wait for the LCD to process the clear command
}

// Delay function (in milliseconds)
void delay_ms(int ms) {
    usleep(ms * 1000); // Convert milliseconds to microseconds
}
