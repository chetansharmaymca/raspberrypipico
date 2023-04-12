#ifndef SRC_task_setup_H_
#define SRC_task_setup_H_

void led_task()
{
    while (true)
    {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        vTaskDelay(100);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        vTaskDelay(100);
    }
}

void dma_ADC_Task()
{
    while (true)
    {
        vTaskSuspend(NULL);
        dma_hw->ints0 = 1u << dma_chan_ADC;
        // drain the fifo memory
        adc_fifo_drain();
        // make adc to stop
        adc_run(false);
        vTaskDelay(pdMS_TO_TICKS(10));
        // rewrite dma address to start new conversion
        dma_channel_set_write_addr(dma_chan_ADC, capture_buf, true);
        dma_channel_set_read_addr(dma_chan_pwm, pwm_buf, true);
        // restart the ADC
        adc_run(true);
        
    }
}

void MSEC_task()
{
    while (true)
    {
        float sum = 0;
        for (int i = 0; i < CAPTURE_DEPTH; i++)
        {
            sum += capture_buf[i];
        }
        adc_avg_temp = (float)sum / CAPTURE_DEPTH;

        ADC_LP[0] = 0;
        ADC_LP[0] = ADC_LP[1];
        ADC_LP[1] = 0;
        ADC_LP[1] = (0.95 * ADC_LP[0]) + (0.05 * adc_avg_temp);

        pwm_buf[0] = PWM_count();

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void one_sec_task()
{
    // /* Block for 1000ms. */
    // const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;
    while (true)
    {
        count++;
        float sum = 0;
        led_value = 1 - led_value;    // toggle led value
        gpio_put(LED_PIN, led_value); // update LED value on GPIO
        if (((ADC_LP[1] - ADC_AVG) > 1.75) | ((ADC_LP[1] - ADC_AVG) < -1.75))
            ADC_AVG = ADC_LP[1];
        if (gpio_get(SUM_PIN))
        {
            sum_disp_count++;
            sum_reset_count++;
        }
        else if (sum_reset_count > 1 && sum_reset_count < 19)
        {
            sum_reset_count = 0;
            if (sum_disp_count > 1 && sum_disp_count < 3)
            {
                sum_disp_count = 0;
            }
        }

        if (count >= 1600)
        {
            write_SUM();
            k_factor_count = 0;
            count = 0;
        }

        FLOW_TOTAL();

        if (sum_reset_count >= 19)
        {
            TOTAL = 0;
            LCD_BASE();
            gpio_put(RB3, false);
            sum_disp_count = 0;
            sum_reset_count++;
            if (sum_reset_count >= 25)
            {
                sum_reset_count = 0;
                gpio_put(RB3, true);
                LCD_PRINT(decimal_p, 0);
            }
        }
        else if (sum_disp_count >= 3)
        {

            LCD_PRINT(1, TOTAL);
            write_SUM();
            sum_disp_count++;
            if (sum_disp_count >= 8)
            {
                sum_disp_count = 0;
            }
        }
        else
        {
            LCD_PRINT(decimal_p, (uint16_t)FLOW());
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// void fmin_task()
// {

//     /* Block for 30min. */
//     const TickType_t xDelay = 18000 / portTICK_PERIOD_MS;
//     while (true)
//     {
//         write_SUM();
//         k_factor_count = 0;

//         vTaskDelay(xDelay);
//     }
// }

void Print_task()
{

    // uint64_t sum = 0;
    // float temp_flow = FLOW() * factor_decimal;
    // float temp_totel = TOTAL * 0.1;

    while (true)
    {
        // sum++;
        // LCD_PRINT(0, sum);
        float temp_flow = FLOW() * factor_decimal;
        float temp_totel = TOTAL * 0.1;
        sprintf(&Uart_buf[0][0], "%08.2f,", temp_flow); // sprintf(&Uart_buf[0][0], "%08.2f,", temp_flow);
        for (int i = 1; i < uart_depth - 1; i++)
        {
            sprintf(&Uart_buf[i][0], "%08lu,", ECU_CAL[i - 1]); // sprintf(&Uart_buf[i][0], "%08lu,", ECU_CAL[i - 1]);
        }
        sprintf(&Uart_buf[uart_depth - 1][0], "%08.2f\r\n", ADC_AVG);

        sprintf(&Uart1_buf[0][0], "%08.2f,", temp_flow);     // sprintf(&Uart_buf[0][0], "%08.2f,", temp_flow);
        sprintf(&Uart1_buf[1][0], "%08lu,", k_factor);       // sprintf(&Uart_buf[0][0], "%08.2f,", temp_flow);
        sprintf(&Uart1_buf[2][0], "%08.1f\r\n", temp_totel); // sprintf(&Uart_buf[0][0], "%08.2f,", temp_flow);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

#endif