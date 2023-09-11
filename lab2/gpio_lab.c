#include <stdio.h>
#include "pico/stdlib.h"

#define UART_ID uart0
#define BAUD_RATE 115200

#define UART_TX_PIN 16
#define UART_RX_PIN 17

#define BTN_PIN 15

int main() {
    char received_char;
    stdio_usb_init();

    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_set_pulls(BTN_PIN, true, false);

    // Give some time to init, then flush buffers
    sleep_ms(200);
    while (uart_is_readable(UART_ID)) {
        char discard_char = uart_getc(UART_ID);
    }
    sleep_ms(200);

    // If the transmittor is sending,
    // stop reading button state to prevent interrupts.
    int b_sending_data_stream = 0;
    int b_button_state = 0;

    // Determines the next character to send.
    char next_char_send = 'A';
    while (true)
    {
        // Just sent the last character...
        // '[' is the character after 'Z' in ASCII....
        if (next_char_send == ('Z' + 1)){
            b_sending_data_stream = 0;
            printf("\n");
            next_char_send = 'A';
        }

        // Currently not transmiting,
        // read button state.
        if (b_sending_data_stream == 0){
            b_button_state = gpio_get(BTN_PIN);
            b_sending_data_stream = 1; // Start sending again...
        }

        if (b_button_state){
            b_sending_data_stream = 0; // Send it once only...
            uart_putc_raw(UART_ID, '1');
        } else {
            uart_putc_raw(UART_ID, next_char_send);
        } 

        sleep_ms(200); // Give some time to recieve

        if (uart_is_readable(UART_ID))
        {
            received_char = uart_getc(UART_ID);
            if (received_char == '1') {
                printf("2\n"); // print '2' when '1' is received.
            } else {
                printf("%c", received_char + 32); // '+ 32' to convert to lowercase.
                next_char_send = next_char_send + 1; // Cycle to the next character...
            }
        }
        
        // Add some sleep time, to total idle time towards 1 second.
        // (Since 200ms wait to receieve input, 800ms to total to 1second...)
        sleep_ms(800);
    }
    return 0;
}
