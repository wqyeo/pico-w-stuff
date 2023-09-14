#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define BTN_PIN 15

uint64_t current_timer = 0;
uint64_t last_timestamp = 0;

bool repeating_timer_callback(struct repeating_timer *t) {
    uint64_t current_timestamp = time_us_64();
    uint64_t difference = current_timestamp - last_timestamp;

    current_timer += difference;

    // Convert it to actual seconds and print...
    printf("%i\n", current_timer / 1000000);

    last_timestamp = current_timestamp;
    return true;
}

void gpio_callback(uint gpio, uint32_t events) {
    current_timer = 0;
}

int main() {
    char received_char;
    stdio_usb_init();

    // Set interrupt on button. 
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    
    last_timestamp = time_us_64();
    
    // Set repeating timer...
    struct repeating_timer timer;
    add_repeating_timer_ms(-500, repeating_timer_callback, NULL, &timer);
    while (true);
}
