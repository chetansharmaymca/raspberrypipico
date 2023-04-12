/*
 * File:
 * Author:
 * Comments:
 * Revision history:
 */

// This is a guard condition so that contents of this file are not included
// more than once.

/***  Display Digits
 *     C
 *   G   B
 *     F
 *   E   A
       D     **/
// #ifndef XC_HEADER_TEMPLATE_H
// #define	XC_HEADER_TEMPLATE_H
// define WR   RB1  //Pin 3 as read clock output
// define DATA RB0  //Pin 4 as Serial data output
// #define RB2 CS  //pin 4 as clock source
// #define RB2=1    digitalWrite(CS, HIGH)
// #define RB2=0    digitalWrite(CS, LOW)
// #define RB1=1    digitalWrite(WR, HIGH)
// #define RB1=0    digitalWrite(WR, LOW)
// #define RB0=1  digitalWrite(DATA, HIGH)
// #define RB0=0  digitalWrite(DATA, LOW) */

#ifndef SRC_LCD_H_
#define SRC_LCD_H_

#include <pico/stdlib.h>
#include <PIN_CONF.h>

#define sbi(x, y) (x |= (1 << y))
#define cbi(x, y) (x &= ~(1 << y))
#define uchar unsigned char
#define uint unsigned int

#define ComMode 0x52
#define RCosc 0x30
#define LCD_on 0x06
#define LCD_off 0x04
#define Sys_en 0x02
#define SYSDIS 0X00  // 0b1000 0000 0000
#define WDTDIS1 0X0A // 0b1000 0000 1010
#define CTRl_cmd 0x80
#define Data_cmd 0xa0

const char num[] = {
    0x7D,
    0x60,
    0x3E,
    0x7A,
    0x63,
    0x5B,
    0x5F,
    0x70,
    0x7F,
    0x7B,
    0x77,
    0x4F,
    0x1D,
    0x0E,
    0x6E,
    0x1F,
    0x17,
    0x67,
    0x47,
    0x0D,
    0x46,
    0x75,
    0x37,
    0x06,
    0x0F,
    0x6D,
    0x02,
    0x00,
};
char dispnum[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void LCD_Init();
void LCD_CLR(void);
void LCD_BASE(void);
void LCD_PRINT(char p, uint32_t x);
void SendBit_1621(char sdata, char cnt);
void SendCmd_1621(char command);
void Write_1621(char addr, char sdata);
void HT1621_all_off(char num);
void HT1621_all_on(char num);
void displaydata(int p);
void delay1(unsigned long x);

void LCD_Init()
{

    SendCmd_1621(ComMode);
    SendCmd_1621(RCosc);
    SendCmd_1621(SYSDIS);
    SendCmd_1621(WDTDIS1);
    SendCmd_1621(Sys_en);
    SendCmd_1621(LCD_on);
    HT1621_all_on(16);
    sleep_ms(1);
    HT1621_all_off(16);
    sleep_ms(1);
    displaydata(1); // light on the first decimal point starting from the right side
    dispnum[5] = num[5];
    dispnum[4] = num[4];
    dispnum[3] = num[3];
    dispnum[2] = num[2];
    dispnum[1] = num[1];
    dispnum[0] = num[0];

    sbi(dispnum[5], 7);
    // cbi(dispnum[5],7);
    sbi(dispnum[4], 7);
    // cbi(dispnum[4],7);
    sbi(dispnum[3], 7);
    // cbi(dispnum[3],7);
    LCD_BASE();
    // delayed_by_ms(t , 2000);
    sleep_ms(2000);
    HT1621_all_off(16);
    sleep_ms(1);
}

void LCD_CLR(void)
{
    HT1621_all_off(16);
}

void LCD_PRINT(char p, uint32_t x)
{
    /// for printing number without dot put P =0, for dot at digit 1 put P=1 and same for p =2//
    HT1621_all_off(16);
    uint t1, t2, t3, t4, t5, a0, a1, a2, a3, a4, a5;
    t1 = x / 10;
    t2 = t1 / 10;
    t3 = t2 / 10;
    t4 = t3 / 10;
    t5 = t4 / 10;
    a0 = t5;
    a1 = (t4 - t5 * 10);
    a2 = (t3 - t4 * 10);
    a3 = (t2 - t3 * 10);
    a4 = (t1 - t2 * 10);
    a5 = (x - t1 * 10);
    // sleep_ms(1);
    if (p == 0)
    {
        Write_1621(0, num[a5]); // 0
        Write_1621(2, num[a4]);
        if (x < 100)
        {
            Write_1621(8, 0x00);
            Write_1621(6, 0x00);
            Write_1621(4, 0x00);
        }
        else if (x < 1000)
        {
            Write_1621(4, num[a3]); // 3
            Write_1621(6, 0x00);    // 3
            Write_1621(8, 0x00);
        }
        else if (x < 10000)
        {
            Write_1621(4, num[a3]); // 3
            Write_1621(6, num[a2]); // 3
            Write_1621(8, 0x00);
        }
        else
        {
            Write_1621(4, num[a3]); // 3
            Write_1621(6, num[a2]); // 3
            Write_1621(8, num[a1]);
            Write_1621(10, num[a0]);
        }
    }
    else if (p == 1)
    {
        Write_1621(0, (0x80 + num[a5])); // 0
        Write_1621(2, num[a4]);
        if (x < 100)
        {
            Write_1621(8, 0x00);
            Write_1621(6, 0x00);
            Write_1621(4, 0x00);
        }
        else if (x < 1000)
        {
            Write_1621(4, num[a3]); // 3
            Write_1621(6, 0x00);    // 3
            Write_1621(8, 0x00);
        }
        else if (x < 10000)
        {
            Write_1621(4, num[a3]); // 3
            Write_1621(6, num[a2]); // 3
            Write_1621(8, 0x00);
        }
        else
        {
            Write_1621(4, num[a3]); // 3
            Write_1621(6, num[a2]); // 3
            Write_1621(8, num[a1]);
            Write_1621(10, num[a0]);
        }
    }
    else if (p == 2)
    {
        Write_1621(0, num[a5]); // 0
        Write_1621(2, (0x80 + num[a4]));
        if (x < 1000)
        {
            Write_1621(4, num[a3]); // 3
            Write_1621(6, 0x00);    // 3
            Write_1621(8, 0x00);
        }
        else if (x < 10000)
        {
            Write_1621(4, num[a3]); // 3
            Write_1621(6, num[a2]); // 3
            Write_1621(8, 0x00);
        }
        else
        {
            Write_1621(4, num[a3]); // 3
            Write_1621(6, num[a2]); // 3
            Write_1621(8, num[a1]);
            Write_1621(10, num[a0]);
        }
    }
    else
    {
        Write_1621(0, (0x80 + num[a5])); // 3
        Write_1621(2, num[a4]);          // 3
        Write_1621(4, num[a3]);
        Write_1621(6, num[a2]); // 3
        Write_1621(8, num[a1]); // 3
        Write_1621(10, num[a0]);
    }
    //  Write_1621(2,0x80);  // for 2nd dot
    // sleep_ms(10);
}

void LCD_BASE(void)
{
    Write_1621(0, num[0]); // O
    Write_1621(2, 0x77);   // R
    Write_1621(4, 0x1D);   // C
    Write_1621(6, num[1]); // I
    Write_1621(8, 0x75);   // M
    Write_1621(10, 0x0D);  // L
}

void delay1(unsigned long x)
{
    for (; x > 0; x--)
        ;
}

void SendBit_1621(char sdata, char cnt)
{
    char i;
    for (i = 0; i < cnt; i++)
    {
        gpio_put(RB1, 0); // RB1 = 0;
        busy_wait_us(4);
        if (sdata & 0x80)
            gpio_put(RB0, 1); // RB0 = 1;
        else
            gpio_put(RB0, 0); // RB0 = 0;

        gpio_put(RB1, 1); // RB1 = 1;
        busy_wait_us(4);
        sdata <<= 1;
    }
}

void SendCmd_1621(char command)
{
    gpio_put(RB2, 0);
    SendBit_1621(0x80, 4);
    SendBit_1621(command, 8);
    gpio_put(RB2, 1);
}

void Write_1621(char addr, char sdata)
{
    addr <<= 2;
    gpio_put(RB2, 0);
    SendBit_1621(0xa0, 3);
    SendBit_1621(addr, 6);
    SendBit_1621(sdata, 8);
    gpio_put(RB2, 1);
}

void HT1621_all_off(char num)
{
    uchar i;
    uchar addr = 0;
    for (i = 0; i < num; i++)
    {
        Write_1621(addr, 0x00);
        addr += 2;
    }
}

void HT1621_all_on(char num)
{
    uchar i;
    uchar addr = 0;
    for (i = 0; i < num; i++)
    {
        Write_1621(addr, 0xff);
        addr += 2;
    }
}

void displaydata(int p)
{
    uchar i = 0;
    switch (p)
    {
    case 1:
        sbi(dispnum[0], 7);
        break;
    case 2:
        sbi(dispnum[1], 7);
        break;
    case 3:
        sbi(dispnum[2], 7);
        break;
    default:
        break;
    }
    for (i = 0; i <= 5; i++)
    {
        Write_1621(i * 2, dispnum[i]);
    }
}

#endif
