#include <FreeRTOS.h>
#include <task.h>

#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <hardware/uart.h>
#include <hardware/adc.h>
#include <hardware/pwm.h>
#include <hardware/flash.h>
#include <hardware/watchdog.h>
#include <hardware/timer.h>
#include <hardware/irq.h>
#include <hardware/dma.h>
#include <pico/cyw43_arch.h>
#include <LCD.h>
#include <GB_VAR.h>
#include <flow_setup.h>
#include <flash_setup.h>
#include <PIN_CONF.h>
#include <DMA_SETUP.h>
#include <timer_setup.h>
#include <task_setup.h>
#include <queue.h>

// #include <hardware/spi.h>
// #include <hardware/clocks.h>
// #include <hardware/pio.h>
// #include "blink.pio.h"

static QueueHandle_t xQueue = NULL;


void main_task()
{

    xTaskCreate(led_task, "LED_Task", 256, NULL, 1, &taskhadle_led);
    xTaskCreate(dma_ADC_Task, "dma_adc", 256, NULL, 1, &taskhandle_dma_adc);
    // xTaskCreate(one_sec_task, "one_sec_task", 256, NULL, 1, NULL);
    // xTaskCreate(Print_task, "uart_print", 256, NULL, 1, NULL);
    xTaskCreate(MSEC_task, "50ms_task", 256, NULL, 1, NULL);
    // // xTaskCreate(fmin_task, "minute_task", 256, NULL, 1, NULL);
    // vTaskStartScheduler();
    // irq_set_enabled(DMA_IRQ_0, true);
    // dma_handler();
    // dma_start_channel_mask(1u << dma_chan_ADC);
    vTaskDelete(taskhadle_main);
}

void vLaunch(void)
{
    xTaskCreate(main_task, "Main_task", 256, NULL, 1, &taskhadle_main);

    vTaskStartScheduler();
}

int main()
{
    stdio_init_all();

    if (cyw43_arch_init())
    {
        printf("failed to initialise\n");
        return -1;
    }

    uart_init(uart0, 9600); // initialising Uart 0
    uart_set_format(uart0, 8, 1, UART_PARITY_NONE);

    uart_init(uart1, 9600);                         // initialise uart 1
    uart_set_format(uart1, 8, 1, UART_PARITY_NONE); // configure uart1

    PIN_CONF();

    sleep_ms(500);

    LCD_Init();

    read_flash();
    decimal();

    dma_setup();

    // struct repeating_timer timer1; // creating timer1 as repeating timer

    // add_repeating_timer_ms(500, repeating_timer_callback, NULL, &timer1); // add repaeating timer MS function (every 500 ms) with timer1

    // timer_setup();

    // while (1)
    // {
    //     tight_loop_contents(); // no action in while loop only other function to work
    // }

    vLaunch();
}
