#ifndef SRC_flash_setup_H_
#define SRC_flash_setup_H_

void write_SUM();  // writing sum values to Flash
void read_flash(); // read flash memory data
void burn_flash();

void write_SUM()
{
    temp_total[0] = (TOTAL >> 16) & 0xFF;
    temp_total[1] = (TOTAL >> 8) & 0xFF;
    temp_total[2] = TOTAL & 0xFF;
    temp_total[3] = k_factor;

    flash_range_erase(FLASH_TARGET_OFFSET_T, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET_T, temp_total, FLASH_PAGE_SIZE);
}

void read_flash()
{
    for (int i = 0; i < 28; i++)
    {
        ECU_CAL[i - 1] = (*(uint8_t *)(flash_target_contents + 2 * i - 2) << 8) | (*(uint8_t *)(flash_target_contents + 2 * i - 1));
    }
    TOTAL = (*(uint8_t *)(flash_target_contents_T) << 16) | (*(uint8_t *)(flash_target_contents_T + 1) << 8) | (*(uint8_t *)(flash_target_contents_T + 2));
    k_factor = (*(uint8_t *)(flash_target_contents_T + 3));
}

void burn_flash()
{
    /** code for writing calibration data to Flash Memory**/
    if (Cal_R == true) // detect if calibration data recived -> completed
    {
        if (CAL_UART[26] == 51387) // check key provided is correct or not
        {
            for (size_t i = 1; i < 27; i++) // change 16 bit calibration data array to 8 bit array before write to flash
            {
                BaseCal_8[2 * i - 2] = CAL_UART[i - 1] >> 8;
                BaseCal_8[2 * i - 1] = CAL_UART[i - 1] & 0xFF;
            }
            /*change 16 bit total data array to 8 bit array before write to flash*/
            temp_total[0] = 0;
            temp_total[1] = CAL_UART[25] >> 8;
            temp_total[2] = CAL_UART[25] & 0xFF;
            temp_total[3] = CAL_UART[22];

            flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);   // erase the flash area where calibration to be stored
            flash_range_erase(FLASH_TARGET_OFFSET_T, FLASH_SECTOR_SIZE); // erase the flash area where total to be stored

            flash_range_program(FLASH_TARGET_OFFSET, BaseCal_8, FLASH_PAGE_SIZE);    // write calibration valuse to flash
            flash_range_program(FLASH_TARGET_OFFSET_T, temp_total, FLASH_PAGE_SIZE); // write total valuse to flash

            read_flash(); // read flash again and update to ECU cal Array
            decimal();

            Cal_R = false;                  // set calibration bit false
            for (size_t i = 0; i < 27; i++) // empty calibration recived array
            {
                CAL_UART[i] = 0;
            }
        }
        else
        {
            for (size_t i = 0; i < 27; i++) // empty calibration recived array
            {
                CAL_UART[i] = 0;
            }
            Cal_R = false; // set calibration bit false
        }
    }
}

#endif