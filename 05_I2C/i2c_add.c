#include <stdio.h>
#include <mraa.h>

void scan_i2c_bus(mraa_i2c_context i2c) {
    for (int addr = 3; addr < 0x78; addr++) {
        if (mraa_i2c_address(i2c, addr) == MRAA_SUCCESS) {
            printf("I2C device found at address 0x%02X\n", addr);
        }
    }
}

int main() {
    mraa_init();
    mraa_i2c_context i2c = mraa_i2c_init(0);  // Initialize I2C bus 0
    if (i2c == NULL) {
        printf("Failed to initialize I2C bus\n");
        return 1;
    }
    scan_i2c_bus(i2c);
    mraa_i2c_stop(i2c);
    return 0;
}

