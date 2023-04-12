#ifndef SRC_timer_setup_H_
#define SRC_timer_setup_H_

void timer_setup();

bool repeating_timer_callback(struct repeating_timer *t) // defining a repating timer function to be used in main
{
    return true; // as it is a bool function we need to return true
}

int64_t alarm_callback_3(alarm_id_t id3, void *user_data)       //50ms ->alarm number 3 used for calibration write to Flash & K factor change
{
    if (first_run)                      //code if somting to perform only once during code run
    {
        // watchdog_enable(3000, true);
        first_run = false;
    }

/** code for update k Factor from push Button**/
    if (key_value == 25)        //detect if Key pin enabled before start
    {
        if (gpio_get(KEY_PIN))      //again detect key Pin enabled
        {
            if (gpio_get(K_FACP))   //incrase K factor 
            {
                k_factor_count++;
                if (k_factor_count > 10)
                {
                    if (k_factor < 200)     //Max limit for K factor -> 2 time (200%)
                    {
                        k_factor++;
                        LCD_PRINT(2, k_factor);
                    }
                    k_factor_count = 0;
                }
            }
            if (gpio_get(K_FACN))       //decrease K factor
            {
                k_factor_count++;
                if (k_factor_count > 10)
                {
                    if (k_factor > 1)       //Min limit for k factor -> 0.01 times (1%) 
                    {
                        k_factor--;
                        LCD_PRINT(2, k_factor);
                    }
                    k_factor_count = 0;
                }
            }
        }
    }

/** code for writing calibration data to Flash Memory**/
    if (Cal_R == true)      // detect if calibration data recived -> completed
    {
        if (CAL_UART[26] == 51387)      //check key provided is correct or not
        {
            for (size_t i = 1; i < 27; i++)     //change 16 bit calibration data array to 8 bit array before write to flash
            {
                BaseCal_8[2 * i - 2] = CAL_UART[i - 1] >> 8;
                BaseCal_8[2 * i - 1] = CAL_UART[i - 1] & 0xFF;
            }
                    /*change 16 bit total data array to 8 bit array before write to flash*/
            temp_total[0] = 0;
            temp_total[1] =CAL_UART[25] >> 8;
            temp_total[2] = CAL_UART[25] & 0xFF;
            temp_total[3] = CAL_UART[22];

            flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);          //erase the flash area where calibration to be stored
            flash_range_erase(FLASH_TARGET_OFFSET_T, FLASH_SECTOR_SIZE);        //erase the flash area where total to be stored

            flash_range_program(FLASH_TARGET_OFFSET, BaseCal_8, FLASH_PAGE_SIZE);       //write calibration valuse to flash
            flash_range_program(FLASH_TARGET_OFFSET_T, temp_total, FLASH_PAGE_SIZE);    //write total valuse to flash

            read_flash();                       //read flash again and update to ECU cal Array
            decimal();

            Cal_R = false;                      //set calibration bit false
            for (size_t i = 0; i < 27; i++)     //empty calibration recived array
            {
                CAL_UART[i] = 0;    
            }
        }
        else
        {
            for (size_t i = 0; i < 27; i++)     //empty calibration recived array
            {
                CAL_UART[i] = 0;
            }
            Cal_R = false;                      //set calibration bit false
        }
    }

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
    return (50000);
}

int64_t alarm_callback_2(alarm_id_t id2, void *user_data)   
{
    watchdog_update();
    uint64_t sum = 0;
    float temp_flow = FLOW() * factor_decimal;
    float temp_totel = TOTAL * 0.1;

    // avg_avg[count_avg] = adc_avg_temp;
    // count_avg++;
    // if(count_avg == avg_depth) count_avg =0;
    // use below to send ADC_Buffer to Uart//
    // sprintf(&Uart_buf[0][0], "%06.2f,",FLOW());
    // for(int i = 1;i <CAPTURE_DEPTH-1;i++){
    //     sprintf(&Uart_buf[i][0], "%08lu,",capture_buf[i-1]);
    // }
    // sprintf(&Uart_buf[CAPTURE_DEPTH-1][0], "%08lu\r\n",capture_buf[CAPTURE_DEPTH-2]);

    // use below to send normal calibration data//
    sprintf(&Uart_buf[0][0], "%08.2f,", temp_flow);     //sprintf(&Uart_buf[0][0], "%08.2f,", temp_flow);
    for (int i = 1; i < uart_depth - 1; i++)
    {
        sprintf(&Uart_buf[i][0], "%08lu,", ECU_CAL[i - 1]);     //sprintf(&Uart_buf[i][0], "%08lu,", ECU_CAL[i - 1]);
    }
    sprintf(&Uart_buf[uart_depth - 1][0], "%08.2f\r\n", ADC_AVG);

    sprintf(&Uart1_buf[0][0], "%08.2f,", temp_flow);     //sprintf(&Uart_buf[0][0], "%08.2f,", temp_flow);
    sprintf(&Uart1_buf[1][0], "%08lu,", k_factor);     //sprintf(&Uart_buf[0][0], "%08.2f,", temp_flow);
    sprintf(&Uart1_buf[2][0], "%08.1f\r\n", temp_totel);     //sprintf(&Uart_buf[0][0], "%08.2f,", temp_flow);

    return 1000;     //return 1000
}

int64_t alarm_callback_1(alarm_id_t id1, void *user_data)
{
    count++;
    // float sum = 0;
    led_value = 1 - led_value;    // toggle led value
    gpio_put(LED_PIN, led_value); // update LED value on GPIO
    // for (int i = 0; i < CAPTURE_DEPTH; i++)
    // {
    //     sum += capture_buf[i];
    // }
    // adc_avg_temp = (float)sum / CAPTURE_DEPTH;

    // ADC_LP[0] = 0;
    // ADC_LP[0] = ADC_LP[1];
    // ADC_LP[1] = 0;
    // ADC_LP[1] = (0.6 * ADC_LP[0]) + (0.4 * adc_avg_temp);
    if (((ADC_LP[1] - ADC_AVG) > 1.75) | ((ADC_LP[1] - ADC_AVG) < -1.75))
    ADC_AVG = ADC_LP[1];

    // for (int i = 0; i < avg_depth; ++i)
    // {
    //     sum += avg_avg[i];
    // }

    // ADC_AVG = sum/avg_depth;

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
        k_factor_count =0;
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

    return 1000000;
}

void timer_setup()
{
    add_alarm_in_ms(100, alarm_callback_1, NULL, false);
    add_alarm_in_ms(100, alarm_callback_2, NULL, false);
    add_alarm_in_ms(100, alarm_callback_3, NULL, false);
}



#endif