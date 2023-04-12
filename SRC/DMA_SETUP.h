
#ifndef SRC_DMA_SETUP_H_
#define SRC_DMA_SETUP_H_

uint dma_busy = 0;
dma_channel_config cfg_ADC;      // configuration for dma_ADC_channel
dma_channel_config cfg_UART;     // configuration for dma_UART_TX_channel
dma_channel_config cfg_pwm;      // configuration for dma_PWM_channel
dma_channel_config cfg_UARTR;    // configuration for dma_UART_RX_channel
dma_channel_config cfg_UART1_TX; // configuration for dma_Uart 1
dma_channel_config cfg_CTRL;    //configuration for control channel

void dma_handler();
void dma_setup();

void dma_handler()
{
    if (dma_channel_get_irq0_status(dma_chan_UART))
    {
        dma_hw->ints0 = 1u << dma_chan_UART;
        dma_channel_set_read_addr(dma_chan_UART, &Uart_buf, true);
        dma_channel_set_read_addr(dma_chan_pwm, pwm_buf, true);
    }
    else if (dma_channel_get_irq0_status(dma_chan_ADC))
    {
        // dma_hw->ints0 = 1u << dma_chan_ADC;
        // // drain the fifo memory
        // adc_fifo_drain();
        // // make adc to stop
        // adc_run(false);

        // // rewrite dma address to start new conversion
        // dma_channel_set_write_addr(dma_chan_ADC, capture_buf, true);
        // dma_channel_set_read_addr(dma_chan_pwm, pwm_buf, true);
        // // restart the ADC
        // adc_run(true);
        BaseType_t checkIfYieldRequired;
        checkIfYieldRequired = xTaskResumeFromISR(taskhandle_dma_adc);
        portYIELD_FROM_ISR(checkIfYieldRequired);
    }
    else if (dma_channel_get_irq0_status(dma_chan_UARTR))
    {
        dma_hw->ints0 = 1u << dma_chan_UARTR;
        dma_channel_set_write_addr(dma_chan_UARTR, &CAL_UART, true);
        dma_channel_set_read_addr(dma_chan_pwm, pwm_buf, true);
        Cal_R = true;
    }
    else if (dma_channel_get_irq0_status(dma_chan_UART1_TX))
    {
        dma_hw->ints0 = 1u << dma_chan_UART1_TX;
        dma_channel_set_read_addr(dma_chan_UART1_TX, &Uart1_buf, true);
        dma_channel_set_read_addr(dma_chan_pwm, pwm_buf, true);
    }
}

void dma_setup()
{
    adc_fifo_setup(
        true,  // Write each completed conversion to the sample FIFO
        true,  // Enable DMA data request (DREQ)
        1,     // DREQ (and IRQ) asserted when at least 1 sample present
        false, // We won't see the ERR bit because of 8 bit reads; disable.
        false  // Shift each sample to 8 bits when pushing to FIFO
    );

    // Divisor of 0 -> full speed. Free-running capture with the divider is
    // equivalent to pressing the ADC_CS_START_ONCE button once per `div + 1`
    // cycles (div not necessarily an integer). Each conversion takes 96
    // cycles, so in general you want a divider of 0 (hold down the button
    // continuously) or > 95 (take samples less frequently than 96 cycle
    // intervals). This is all timed by the 48 MHz ADC clock.
    adc_set_clkdiv(ADCCLK/Fs); // 960

    // printf("Arming DMA\n");
    sleep_ms(1000);
    // Set up the DMA to start transferring data as soon as it appears in FIFO
    // dma_chan = dma_claim_unused_channel(true);
    cfg_ADC = dma_channel_get_default_config(dma_chan_ADC);
    cfg_UART = dma_channel_get_default_config(dma_chan_UART);
    cfg_UARTR = dma_channel_get_default_config(dma_chan_UARTR);
    cfg_pwm = dma_channel_get_default_config(dma_chan_pwm);
    cfg_UART1_TX = dma_channel_get_default_config(dma_chan_UART1_TX);
    cfg_CTRL = dma_channel_get_default_config(dma_chan_CTRL);

    // configure for ADC DMA///
    //  Reading from constant address, writing to incrementing byte addresses
    channel_config_set_transfer_data_size(&cfg_ADC, DMA_SIZE_16);
    channel_config_set_read_increment(&cfg_ADC, false);
    channel_config_set_write_increment(&cfg_ADC, true);
    channel_config_set_dreq(&cfg_ADC, DREQ_ADC);
    // channel_config_set_chain_to(&cfg_ADC, dma_chan_CTRL); 

    adc_fifo_drain();
    adc_run(false);
    dma_channel_configure(dma_chan_ADC, &cfg_ADC,
                          capture_buf,   // dst
                          &adc_hw->fifo, // src
                          CAPTURE_DEPTH, // transfer count
                          true        // start immediately
    );
    // dma_start_channel_mask(1u << sample_chan) ;
    adc_run(true);
    dma_channel_wait_for_finish_blocking(dma_chan_ADC);
    dma_channel_set_irq0_enabled(dma_chan_ADC, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);
    dma_handler();

    // CONTROL CHANNEL
    // channel_config_set_transfer_data_size(&cfg_CTRL, DMA_SIZE_32);      // 32-bit txfers
    // channel_config_set_read_increment(&cfg_CTRL, false);                // no read incrementing
    // channel_config_set_write_increment(&cfg_CTRL, false);               // no write incrementing
    // channel_config_set_chain_to(&cfg_CTRL, dma_chan_ADC);                // chain to sample chan

    // dma_channel_configure(
    //     dma_chan_CTRL,                         // Channel to be configured
    //     &cfg_CTRL,                                // The configuration we just created
    //     &dma_hw->ch[dma_chan_ADC].write_addr,  // Write address (channel 0 read address)
    //     &address_pointer,                   // Read address (POINTER TO AN ADDRESS)
    //     1,                                  // Number of transfers, in this case each is 4 byte
    //     true                               // Don't start immediately.
    // );

    // dma_handler();
    // configure for UART TX//
    // channel_config_set_transfer_data_size(&cfg_UART, DMA_SIZE_8);
    // channel_config_set_read_increment(&cfg_UART, true);
    // channel_config_set_write_increment(&cfg_UART, false);
    // channel_config_set_dreq(&cfg_UART, DREQ_UART0_TX);
    // dma_channel_configure(dma_chan_UART, &cfg_UART,
    //                       &uart0_hw->dr,
    //                       &Uart_buf,
    //                       uart_depth * 9,
    //                       true);

    // dma_channel_wait_for_finish_blocking(dma_chan_UART);
    // dma_channel_set_irq0_enabled(dma_chan_UART, true);
    // irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    // irq_set_enabled(DMA_IRQ_0, true);
    // dma_handler();
    // // configure for UART RX//
    // channel_config_set_transfer_data_size(&cfg_UARTR, DMA_SIZE_8);
    // channel_config_set_read_increment(&cfg_UARTR, false);
    // channel_config_set_write_increment(&cfg_UARTR, true);
    // channel_config_set_dreq(&cfg_UARTR, DREQ_UART0_RX);
    // dma_channel_configure(dma_chan_UARTR, &cfg_UARTR,
    //                       &CAL_UART,
    //                       &uart0_hw->dr,
    //                       54,
    //                       true);

    // // dma_channel_wait_for_finish_blocking(dma_chan_UARTR);
    // dma_channel_set_irq0_enabled(dma_chan_UARTR, true);
    // irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    // irq_set_enabled(DMA_IRQ_0, true);
    // dma_handler();

    // // configure for UART1_TX//
    // channel_config_set_transfer_data_size(&cfg_UART1_TX, DMA_SIZE_8);
    // channel_config_set_read_increment(&cfg_UART1_TX, true);
    // channel_config_set_write_increment(&cfg_UART1_TX, false);
    // channel_config_set_dreq(&cfg_UART1_TX, DREQ_UART1_TX);
    // dma_channel_configure(dma_chan_UART1_TX, &cfg_UART1_TX,
    //                       &uart1_hw->dr,
    //                       &Uart1_buf,
    //                       uart1_depth * 9,
    //                       true);

    // dma_channel_wait_for_finish_blocking(dma_chan_UART1_TX);
    // dma_channel_set_irq0_enabled(dma_chan_UART1_TX, true);
    // irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    // irq_set_enabled(DMA_IRQ_0, true);
    // dma_handler();

    // // dma config for PWM
    // channel_config_set_transfer_data_size(&cfg_pwm, DMA_SIZE_16);
    // channel_config_set_read_increment(&cfg_pwm, true);
    // channel_config_set_write_increment(&cfg_pwm, false);
    // channel_config_set_dreq(&cfg_pwm, DREQ_PWM_WRAP0 + slice_num);
    // pwm_set_enabled(slice_num, false);
    // dma_channel_configure(dma_chan_pwm, &cfg_pwm,
    //                       &pwm_hw->slice[slice_num].cc,
    //                       pwm_buf,
    //                       1,
    //                       true);

    // pwm_set_enabled(slice_num, true);
}

#endif