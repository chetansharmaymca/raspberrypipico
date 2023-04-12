
#ifndef SRC_PIN_CONF_H_
#define SRC_PIN_CONF_H_

#include <GB_VAR.h>

#define RB0 2 // pin for LCD
#define RB1 3 // pin for LCD
#define RB2 4 // pin for LCD
#define RB3 5 // pin for LCD

#define UART1_TX 8          //Pin for 2nd Uart
#define UART1_RX 9          //pin for 2nd Uart
#define PULLUP_UART 7       //pullup for uart

#define LED_PIN 27                                 // pin defined for LED
#define SUM_PIN 20                                 // pin define to display total
#define PWR_PIN 21                                  // Pin used to poer the SPST
#define KEY_PIN 19                                 // Key pin to Calibrate
#define K_FACP 18                                  // K factor +
#define K_FACN 17                                  // K factor -
#define AL_PIN 16                                  // Alarm Digital ON/OFF Pin
#define PWM_PIN 22                                 // PWM output for DAC
#define CAPTURE_CHANNEL 0                          // ADC capture Channel
#define CAPTURE_DEPTH 10                           // Number of ADC read in one go

void PIN_CONF();


void PIN_CONF()
{
    adc_gpio_init(26 + CAPTURE_CHANNEL);
    adc_init();
    adc_select_input(CAPTURE_CHANNEL);

    gpio_set_function(UART1_TX,GPIO_FUNC_UART);
    gpio_set_function(UART1_RX,GPIO_FUNC_UART);
    gpio_init(PULLUP_UART);
    gpio_set_dir(PULLUP_UART,GPIO_OUT);
    gpio_put(PULLUP_UART,true);

    gpio_init(RB0);
    gpio_init(RB1);
    gpio_init(RB2);
    gpio_init(RB3);
    gpio_set_dir(RB0, GPIO_OUT);
    gpio_set_dir(RB1, GPIO_OUT);
    gpio_set_dir(RB2, GPIO_OUT);
    gpio_set_dir(RB3, GPIO_OUT);
    gpio_put(RB3, true);

    gpio_init(SUM_PIN);
    gpio_set_dir(SUM_PIN, GPIO_IN);
    gpio_pull_down(SUM_PIN);

    gpio_init(KEY_PIN);
    gpio_set_dir(KEY_PIN, GPIO_IN);
    gpio_pull_down(KEY_PIN);

    gpio_init(K_FACP);
    gpio_set_dir(K_FACP, GPIO_IN);
    gpio_pull_down(K_FACP);

    gpio_init(K_FACN);
    gpio_set_dir(K_FACN, GPIO_IN);
    gpio_pull_down(K_FACN);

    gpio_init(PWR_PIN);
    gpio_set_dir(PWR_PIN, GPIO_OUT);
    gpio_put(PWR_PIN, true);

    gpio_init(AL_PIN);
    gpio_set_dir(AL_PIN, GPIO_OUT);

    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(PWM_PIN); // Find out which PWM slice is connected to PWM_PIN
    pwm_set_wrap(slice_num, 65535);             // Set period of 4 cycles (0 to 3 inclusive)
    pwm_set_gpio_level(PWM_PIN,11915);
    pwm_set_enabled(slice_num,true);

    gpio_init(LED_PIN);              // setting the LED Pin = 25 In GPIO
    gpio_set_dir(LED_PIN, GPIO_OUT); // setting LED PIN as OUTPUT in GPIO
}


#endif