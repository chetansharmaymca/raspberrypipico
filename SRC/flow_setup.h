#ifndef SRC_flow_setup_H_
#define SRC_flow_setup_H_


void FLOW_TOTAL();          //total flow calculation based on various units
void decimal();             //function to calculate the decimal and unit based factor information
uint16_t PWM_count();
float FLOW();

void decimal()
{
    if (ECU_CAL[24] == 1)   //if decimal data from calibration
    {
        decimal_p = 2;      // 2 digit dcimal at display
        factor_decimal = 0.01;  //actual flow devide factor
        decimal_corr_fac = 10;
    }
    else
    {
        decimal_corr_fac = 1;
        if (ECU_CAL[21] > 6500)
        {
            decimal_p = 0;
            factor_decimal =1;
        }
        else if (ECU_CAL[21] > 999)
        {
            decimal_p = 1;
            factor_decimal = 0.1;
        }
        else
        {
            decimal_p = 2;
            factor_decimal = 0.01;
        }
    }
    switch (ECU_CAL[23])        //flow factor for unit of flow
    {
    case 0:
        factor_unit = 0.001; // for LPH     
        break;
    case 1:
        factor_unit = 1; // for m3/h
        break;
    case 2:
        factor_unit = 0.06; // for LPM
        break;
    case 3:
        factor_unit = 1.699; // for CFM
        break;
    }
}

uint16_t PWM_count()
{
    uint16_t pwm_temp;
    float pwm_ratio = FLOW()*factor_decimal*decimal_corr_fac / ECU_CAL[21];

    pwm_temp = 46650* pwm_ratio + 11250;

    PWM_LP[0] = 0;
    PWM_LP[0] = PWM_LP[1];
    PWM_LP[1] = 0;
    PWM_LP[1] = (0.95 * PWM_LP[0]) + (0.05 * pwm_temp);

    return ((uint16_t)PWM_LP[1]);
}

void FLOW_TOTAL()
{
    total_temp = total_temp + ( FLOW()*factor_unit*factor_decimal * 0.000278);

    if (total_temp >= 0.1)
    {
        TOTAL = TOTAL + total_temp * 10;
        total_temp = 0;
    }
    if (TOTAL > 999999)
    {
        TOTAL = 0;
        temp_total[0] = (TOTAL >> 16) & 0xFF;
        temp_total[1] = (TOTAL >> 8) & 0xFF;
        temp_total[2] = TOTAL & 0xFF;
        temp_total[3] = k_factor;
        flash_range_erase(FLASH_TARGET_OFFSET_T, FLASH_SECTOR_SIZE);
        flash_range_program(FLASH_TARGET_OFFSET_T, temp_total, FLASH_PAGE_SIZE);
    }
}

float FLOW()
{
    float f, k = 0.000;
    float f3 = 0.000, f2 = 0.000, f1 = 0.000, f4 = 0.000, f5 = 0.000, temp = 0.000, temp1 = 0.000;
    k = (float)k_factor/100;
    unsigned int x[11];
    unsigned int y[11];
    for (int i = 0; i < 11; i++)
    {
        x[i] = ECU_CAL[i];
        y[i] = ECU_CAL[11 + i];
    }
    // this function is using table for flow calculation//
    if (ADC_AVG < x[0])
    {
        temp1 = 0;
    }
    else if (ADC_AVG < x[10])
    {
        for (uint8_t i = 0; i < 11; i++)
        {
            if (ADC_AVG >= x[i])
            {
                if (ADC_AVG < x[i + 1])
                {
                    f2 = (x[i + 1] - x[i]);
                    f3 = (y[i + 1] - y[i]) / f2;
                    f1 = f3 * ADC_AVG;
                    f4 = y[i + 1];
                    f5 = f3 * x[i + 1];
                    temp1 = f1 + f4 - f5;
                }
            }
        }
    }
    else if (ADC_AVG >= x[10])
    {
        temp1 = y[10];
    }

    if (ECU_CAL[24] == 1)
    {
        temp = temp1 * 10;
    }
    else
    {
        switch (decimal_p)
        {
        case 0:
            temp = temp1;
            break;
        case 1:
            temp = temp1 * 10;
            break;
        case 2:
            temp = temp1 * 100;
            break;
        }
    }

    f = temp * k;

    return (f);
}

#endif