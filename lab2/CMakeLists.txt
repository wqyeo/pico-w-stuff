add_executable(
    lab2 gpio_lab.c
)

target_link_libraries(lab2 pico_stdlib)

# enable usb output, enable uart output
pico_enable_stdio_usb(lab2 1)
pico_enable_stdio_uart(lab2 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(lab2)