add_executable(
    lab3 interrupts_lab.c
)

target_link_libraries(lab3 pico_stdlib)

# enable usb output, enable uart output
pico_enable_stdio_usb(lab3 1)
pico_enable_stdio_uart(lab3 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(lab3)