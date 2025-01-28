#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mraa.h>
#include <mraa/uart.h>

#define RFID_UART_PORT "/dev/ttyS3"
#define RFID_UART_BAUDRATE 9600

int main() {
    mraa_init();
    char buffer[] = "Hello Mraa!";
    
    mraa_uart_context uart = mraa_uart_init_raw(RFID_UART_PORT);
    if (uart == NULL) {
        printf("Failed to initialize UART.\n");
        return 1;
    }

    if (mraa_uart_set_baudrate(uart, RFID_UART_BAUDRATE) != MRAA_SUCCESS) {
        printf("Failed to set UART baudrate.\n");
        mraa_uart_stop(uart);
        return 1;
    }

    mraa_uart_set_mode(uart, 8, MRAA_UART_PARITY_NONE, 1);
    mraa_uart_set_flowcontrol(uart, 0, 0);

    while (1) {
    	
    	
        /* send data through UART */
        mraa_uart_write(uart, buffer, sizeof(buffer));

        sleep(1);
        mraa_uart_read(uart, buffer, sizeof(buffer));
        
        printf("Received: %s\n", buffer);
  

        
    }

    mraa_uart_stop(uart);
    mraa_deinit();

    return 0;
}

