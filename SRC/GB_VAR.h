#ifndef SRC_GB_VAR_H_
#define SRC_GB_VAR_H_

#include <pico/stdlib.h>

/***Declaration of DMA functions**/

#define CAPTURE_CHANNEL 0                          // ADC capture Channel
#define CAPTURE_DEPTH 10                           // Number of ADC read in one go
#define avg_depth 15                                //number of avg of avg
#define uart_depth 24                              // Length of Uart send with Uart
#define uart1_depth 3                           // length of buffer for uart1 to send
#define FLASH_TARGET_OFFSET (1024 * 1024 )          // address to write calibration on flash
#define FLASH_TARGET_OFFSET_T (1024 * 1024 + 4096) // address to write total + k factor on flash
// Sample rate (Hz)
#define Fs 100.0
// ADC clock rate (unmutable!)
#define ADCCLK 48000000.0


TaskHandle_t taskhadle_main = NULL;
TaskHandle_t taskhadle_led = NULL;
TaskHandle_t taskhandle_dma_adc = NULL;

uint dma_chan_ADC = 0;                  //define dma channel 0 for ADC
uint dma_chan_UART = 1;                 //define dma channel 1 for UART TX
uint dma_chan_pwm = 2;                  //define dma channel 2 for pwm
uint dma_chan_UARTR = 3;                //define dma channel 3 for uart RX
uint dma_chan_UART1_TX = 4;             // define dma channel 4 uart1 TX
uint dma_chan_CTRL = 5;

uint16_t capture_buf[CAPTURE_DEPTH];    //buffer for ADC capture values   
float avg_avg[avg_depth];
char Uart_buf[uart_depth][9];           //char buffer for transmitting UART data
char Uart1_buf[uart1_depth][9];      // char buffer for transmitting Uart Data
uint16_t pwm_buf[1];                    //PWM buffer to be used by DMA

unsigned short * address_pointer = &capture_buf[0] ;

const uint8_t *flash_target_contents = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);       //address to read ECU calibration on flash
const uint8_t *flash_target_contents_T = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET_T);   //address to read Total & K factor

uint8_t decimal_p = 0;          //decimal position on LCD -> default 0 no decimal
uint8_t key_value = 0;          //key value for physical connection of Key Pin for setting K factor
int led_value = 0;              //setting LED value PIN 25 -> default 0 as OFF
uint16_t count = 0;             //counter to save total to flash once in 30 Min
uint8_t count_avg =0;           //counter for avg of avg
uint16_t PWM_count();           //function to calculate PWM value and return as float
float PWM_LP[2] = {0,0};        // time low pass for pwm output
bool first_run = true;          //to execute first time code run
uint slice_num;                 //Slice number of PWM to give its value
float ADC_AVG;                  //average value of capture buffer ->ADC valuse used for flow calculations
float ADC_LP[2] = {0,0};        // time low pass buffer for adc output
float adc_avg_temp;
float FLOW();                   //function to claculate Flow valuse and return flow as Float
float total_temp = 0;           //temperaory holder of total value -> used in toral calculations
float factor_unit = 1;          //factor based on flow unit -> m3, LPH etc -> used in total calculations
float factor_decimal = 1;       //factor based on decimal used -> used in total calculations
float decimal_corr_fac = 1;     //correcton pwm for decimal
uint32_t TOTAL = 0;             //final total value used to display total and saved periodically on flash
uint8_t sum_disp_count = 0;     //counter to display total value on screen
uint8_t sum_reset_count = 0;     //counter to reset total to 0
uint16_t BaseCal[22] = {0, 410, 819, 1229, 1638, 2048, 2458, 2867, 3277, 3686, 4096, 0, 410, 819, 1229, 1638, 2048, 2458, 2867, 3277, 3686, 4096};
uint8_t BaseCal_8[FLASH_PAGE_SIZE];     //8 bit array used to write the calibration in Flash
uint8_t temp_total[FLASH_PAGE_SIZE];    //8 bit array used to write total and k_factor on flash
uint8_t k_factor;                       // k factor for fianl flow calculation 
uint8_t k_factor_count = 0;             //counter for delay time to change the k factor using Push Button
uint16_t CAL_UART[27];                  //Uart buffer to recive the calibration data from dma
uint16_t ECU_CAL[27];                   //buffer to store the calibration values at every start of code
bool Cal_R = false;                     //bit to detect if calibration recived complete from dma

#endif