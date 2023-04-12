add_executable(${PROJECT_NAME}
                main.c
                )

target_link_libraries(${PROJECT_NAME}
                    pico_stdlib
                    FreeRTOS
                    hardware_adc
                    hardware_dma
                    hardware_pwm
                    hardware_flash
                    hardware_spi
                    hardware_pio
                    pico_cyw43_arch_none
                    )

target_include_directories(${PROJECT_NAME} PRIVATE
                    ${CMAKE_CURRENT_LIST_DIR}
                     )

# create map/bin/hex file etc.
pico_add_extra_outputs(${PROJECT_NAME})

# disable usb output, enable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)