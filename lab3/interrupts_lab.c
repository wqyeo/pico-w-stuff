#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico/types.h"

#define BTN_PIN 15

volatile uint32_t timer_seconds = 0;
volatile bool b_stopwatch_running = false;

// Next signal trigger has to be after 700ms, otherwise its ignored.
const uint32_t debounce_trigger = 700;
uint32_t last_interrupt_time = 0;

bool timer_callback(struct repeating_timer *timer) {
    if (b_stopwatch_running) {
        // Since timer runs on 1 second callback,
        // we can safely dictate that 1 second has passed...
        // (Ignoring micro-second differences due to execution delays, etc)
        timer_seconds++;
        printf("Timer: %u sec\n", timer_seconds);
    }

    return true;
}

void start_stopwatch(){
    b_stopwatch_running = true;
    printf("Stopwatch started!\n");
}

void stop_stopwatch() {
    b_stopwatch_running = false;
    printf("Stopwatch stopped!\n");
}

void gpio_callback(uint gpio, uint32_t events) {
    uint32_t time_now = to_ms_since_boot(get_absolute_time());

    // Ignore callback signals that happens too rapidly.
    // (Debouncing)
    if (time_now - last_interrupt_time > debounce_trigger) {
        if ((events & GPIO_IRQ_EDGE_FALL) && !b_stopwatch_running) {
            start_stopwatch();
        } else if ((events & GPIO_IRQ_EDGE_RISE) && b_stopwatch_running) {
            stop_stopwatch();
        }
        timer_seconds = 0;
    }
    last_interrupt_time = time_now;
}

int main() {
    stdio_usb_init();

    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);

    // Connect button PIN to ground to start timer,
    // disconnect to start.
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    
    struct repeating_timer timer;
    add_repeating_timer_ms(-1000, timer_callback, NULL, &timer);

    while (true);
}