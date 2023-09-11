#include <stdio.h>
#include "pico/stdlib.h"

#define UART_ID uart0
#define BAUD_RATE 115200

#define UART_TX_PIN 16
#define UART_RX_PIN 17

#define BTN_PIN 15

int main() {
    char receivedChar;
    stdio_init_all();

    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_set_pulls(BTN_PIN, true, false);
    
    // If the transmittor is sending,
    // stop reading button state to prevent interrupts.
    int isSendingFlag = 0;
    int pseudoButtonState = 0;

    // Determines the next character to send.
    char nextCharSend = 'A';
    while (true)
    {
        // Just sent the last character...
        // '[' is the character after 'Z' in ASCII....
        if (nextCharSend == '['){
            isSendingFlag = 0;
            printf("\n");
            nextCharSend = 'A';
        }

        // Currently not transmiting,
        // read button state.
        if (isSendingFlag == 0){
            pseudoButtonState = gpio_get(BTN_PIN);
            isSendingFlag = 1; // Start sending again...
        }

        if (pseudoButtonState){
            uart_putc_raw(UART_ID, '1');
            isSendingFlag = 0; // Send it once only...
        } else {
            uart_putc_raw(UART_ID, nextCharSend);
            nextCharSend = nextCharSend + 1; // Cycle to the next character...
        } 

        if (uart_is_readable(UART_ID))
        {
            receivedChar = uart_getc(UART_ID);
            if (receivedChar == '1') {
                printf("2\n"); // print '2' when '1' is received.
            } else {
                printf("%c", receivedChar + 32); // '+ 32' to convert to lowercase.
            }
        }
        
        sleep_ms(1000);
    }
    return 0;
}