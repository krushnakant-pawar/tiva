/*
 * main.c
 *
 *  Created on: 04-Feb-2019
 *      Author: krushnakant
 */
//#include <stdint.h>
#include <stdio.h>
#include "font.h"
//#include "inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "inc/tm4c123gh6pm.h"

extern void PLL_Init(void);
extern void DMA_Init(void);
extern void DMA_Start(void);
void Init_SSI1(void);
void SSI1_Write(uint16_t data);
void delay_micro(unsigned int);
void delayMs(int n);
void Init_LedMatrix(void);
void test_LedMatrix(void);

uint16_t bitmap[64];
uint16_t bitmap2[68];
int index = 0;
uint8_t intensity = 0x00;
char str[][4] = { "IISC", "ABCD", "wxyz", "Tiva" };
int main(void)
{
    char c;
    uint16_t data;
    //    PLL_Init();
    GPIOPortF_Init();
    Init_SSI0();
    Init_LedMatrix();
    //test_LedMatrix();
    //To get data from dictionary
    createBitmap();

    uint16_t *inptr;

//    for (int i = 0; i < 20; i++)
//    {
//
//        inptr = (bitmap2 + i);
//
//        //data stream
//        data = *inptr;
//        inptr++;
//        SSI0_Write(data); /* write a character */
//
//    }

    while (1)
    {

        uint16_t * ptr;

        for (int i = 0; i < 64; i = i + 8)
        {

            ptr = (bitmap + i);

            //data stream
            data = *ptr;
            ptr++;
            SSI0_Write(data); /* write a character */

            data = *ptr;
            ptr++;
            SSI0_Write(data); /* write a character */

            data = *ptr;
            ptr++;
            SSI0_Write(data); /* write a character */

            data = *ptr;
            ptr++;
            SSI0_Write(data); /* write a character */

            delay_micro(1000);

            //zero stream
            data = *ptr;
            ptr++;

            SSI0_Write(data); /* write a character */
            data = *ptr;
            ptr++;

            SSI0_Write(data); /* write a character */
            data = *ptr;
            ptr++;
            SSI0_Write(data);

            data = *ptr;
            SSI0_Write(data); /* write a character */
            //  delay_micro(500);
        }

    }

}

void test_LedMatrix(void)
{

    uint16_t data;
    data = 0x0f01;
    SSI0_Write(data); /* write a character */
    data = 0x0f01;
    SSI0_Write(data); /* write a character */
    data = 0x0f01;
    SSI0_Write(data); /* write a character */
    data = 0x0f01;
    SSI0_Write(data); /* write a character */

    delay_micro(10000);

    data = 0x0f00;
    SSI0_Write(data); /* write a character */
    data = 0x0f00;
    SSI0_Write(data); /* write a character */
    data = 0x0f00;
    SSI0_Write(data); /* write a character */
    data = 0x0f00;
    SSI0_Write(data); /* write a character */
    delay_micro(1000000);
}

void Init_LedMatrix(void)
{
    uint16_t data;
    for (int i = 0; i < 16; i++)
    {
        data = i << 8;
        SSI0_Write(data);
    }

    data = 0b0000101100000111;
    SSI0_Write(data);
    // delay_micro(1000);
    data = 0b0000110000000001;
    SSI0_Write(data);
    //  delay_micro(100000);
    //max intensity
    data = 0x0a00 | intensity;
    SSI0_Write(data); /* write a character */
    // delay_micro(1000);
}

void SSI1_Write(uint16_t data)
{
    GPIO_PORTD_DATA_R &= ~0x02; /* assert SS low */

    while ((SSI1_SR_R & 2) == 0)
        ; /* wait until FIFO not full */
    SSI1_DR_R = data; /* transmit high byte */

    while (SSI1_SR_R & 0x10)
        ; /* wait until transmit complete */
    GPIO_PORTD_DATA_R |= 0x02; /* keep SS idle high */
    //  delay_micro(50);
}
void delay_micro(unsigned int n)
{
    for (unsigned long i = 0; i <= 3 * n; i++)
        ;
}

void SSI0_Write(uint16_t data)
{
    GPIO_PORTA_DATA_R &= ~0x08; /* assert SS low */

    while ((SSI0_SR_R & 2) == 0)
        ; /* wait until FIFO not full */
    SSI0_DR_R = data; /* transmit high byte */

    while (SSI0_SR_R & 0x10)
        ; /* wait until transmit complete */
    GPIO_PORTA_DATA_R |= 0x08; /* keep SS idle high */
    //  delay_micro(50);
}

void delayMs(int n)
{
    int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < 3180; j++)
        {
        }
}

//SSI1_SS : PD1
//SSI1_TX : PD3
//SSI1_RX :
//SSI1_CLK : PD0

void Init_SSI0(void)
{

    volatile uint32_t delay;
    SYSCTL_RCGCSSI_R |= 0x01;  // activate SSI0
    SYSCTL_RCGCGPIO_R |= 0x01; // activate port A
    delay = SYSCTL_RCGC2_R;               // allow time to finish activating
    GPIO_PORTA_AFSEL_R |= 0x2C;           // enable alt funct on PA2,3,5
    GPIO_PORTA_DEN_R |= 0x2C;             // enable digital I/O on PA2,3,5
                                          // configure PA2,3,5 as SSI
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFF0F00FF) + 0x00202200;
    GPIO_PORTA_AMSEL_R = 0;               // disable analog functionality on PA
    SSI0_CR1_R &= ~SSI_CR1_SSE;           // disable SSI
    SSI0_CR1_R &= ~SSI_CR1_MS;            // master mode
                                          // clock divider for 8 MHz SSIClk (assumes 80 MHz PLL)
    SSI0_CPSR_R = (SSI0_CPSR_R & ~SSI_CPSR_CPSDVSR_M) + 10;
    SSI0_CR0_R &= ~(SSI_CR0_SCR_M |       // SCR = 0 (8 Mbps data rate)
            SSI_CR0_SPH |         // SPH = 0
            SSI_CR0_SPO);         // SPO = 0
                                  // FRF = Freescale format
    SSI0_CR0_R = (SSI0_CR0_R & ~SSI_CR0_FRF_M) + SSI_CR0_FRF_MOTO;
    // DSS = 16-bit data
    SSI0_CR0_R = (SSI0_CR0_R & ~SSI_CR0_DSS_M) + SSI_CR0_DSS_16;
    //SSI0_DR_R = data;                     // load 'data' into transmit FIFO
    SSI0_CR1_R |= SSI_CR1_SSE;            // enable SSI

}

void Init_SSI11(void)
{
    EnableInterrupts();
    SYSCTL_RCGCSSI_R |= 2; /* enable clock to SSI1 */
    SYSCTL_RCGCGPIO_R |= 8; /* enable clock to GPIOD for SSI1 */
    SYSCTL_RCGCGPIO_R |= 0x20; /* enable clock to GPIOF for slave select */

    /* configure PORTD 3, 0 for SSI1 clock and Tx */
    GPIO_PORTD_AMSEL_R &= ~0x0b; /* disable analog for these pins */
    GPIO_PORTD_DEN_R |= 0x0b; /* and make them digital */
    GPIO_PORTD_AFSEL_R |= 0x0b; /* enable alternate function */
    GPIO_PORTD_PCTL_R &= ~0x0000F0fF; /* assign pins to SSI1 */
    GPIO_PORTD_PCTL_R |= 0x00002022; /* assign pins to SSI1 */

    //REMOVE IF NOT WORKING
    GPIO_PORTD_PUR_R = 0x01; //To enable pull for PD0 SSI1_CLK

//    /* configure PORTF 2 for slave select */
//    GPIO_PORTF_DEN_R |= 0x08; /* make the pin digital */
//    GPIO_PORTF_DIR_R |= 0x08; /* make the pin output */
//    GPIO_PORTF_DATA_R |= 0x08; /* keep SS idle high */

    /* SPI Master, POL = 0, PHA = 0, clock = 4 MHz, 16 bit data */
    SSI1_CR1_R = 0; /* disable SSI and make it master */
    SSI1_CC_R = 0; /* use system clock */
    SSI1_CPSR_R = 2; /* prescaler divided by 2 */
    // SSI1_CR0_R = 0x0007; /* 8 MHz SSI clock, SPI mode, 8 bit data */
    SSI1_CR0_R = 0x000f; /* 8 MHz SSI clock, SPI mode, 16 bit data */
    SSI1_CR1_R |= 2; /* enable SSI1 */

}

/*********** DisableInterrupts ***************
 *
 * disable interrupts
 *
 * inputs:  none
 * outputs: none
 */

void DisableInterrupts(void)
{
    __asm ("CPSID  I\n");
}

/*********** EnableInterrupts ***************
 *
 * emable interrupts
 *
 * inputs:  none
 * outputs: none
 */

void EnableInterrupts(void)
{
    __asm ("CPSIE  I\n");
}

/*********** WaitForInterrupt ************************
 *
 * go to low power mode while waiting for the next interrupt
 *
 * inputs:  none
 * outputs: none
 */

void WaitForInterrupt(void)
{
    __asm ("WFI\n");
}

void createBitmap()
{
    int j = 8;
    int k = 16;
    int l = 24;
    int m = 24;
    unsigned char buf1[8], buf2[8], buf3[8], buf4[8];
    j = str[index][3] - 'A';
    if (j > 26)
    {
        j = str[index][3] - 'a';
        for (int t = 0; t < 8; t++)
        {
            buf1[t] = eng_small[j][t];
        }
    }
    else
    {
        for (int t = 0; t < 8; t++)
        {
            buf1[t] = eng_cap[j][t];
        }
    }
    k = str[index][2] - 'A';
    if (k > 26)
    {
        k = str[index][2] - 'a';
        for (int t = 0; t < 8; t++)
        {
            buf2[t] = eng_small[k][t];
        }
    }
    else
    {
        for (int t = 0; t < 8; t++)
        {
            buf2[t] = eng_cap[k][t];
        }
    }
    l = str[index][1] - 'A';
    if (l > 26)
    {
        l = str[index][1] - 'a';
        for (int t = 0; t < 8; t++)
        {
            buf3[t] = eng_small[l][t];
        }
    }
    else
    {
        for (int t = 0; t < 8; t++)
        {
            buf3[t] = eng_cap[l][t];
        }
    }
    m = str[index][0] - 'A';
    if (m > 26)
    {
        m = str[index][0] - 'a';
        for (int t = 0; t < 8; t++)
        {
            buf4[t] = eng_small[m][t];
        }
    }
    else
    {
        for (int t = 0; t < 8; t++)
        {
            buf4[t] = eng_cap[m][t];
        }
    }

    //Got data from dictionary

    //create bitmap
    int u = 0;
    for (int i = 0; i < 8; i++)
    {

        uint16_t msb = ((8 - i) << 8);

        //to pass data for each led display
        bitmap[u] = msb | buf1[i]; //lsb led
        bitmap[u + 1] = msb | buf2[i]; //1st led
        bitmap[u + 2] = msb | buf3[i]; //2nd led
        bitmap[u + 3] = msb | buf4[i]; //msb led

        //  delay_micro(1000);

        //To pass stream of zeros
        bitmap[u + 4] = msb;
        bitmap[u + 5] = msb;
        bitmap[u + 6] = msb;
        bitmap[u + 7] = msb;

        u = u + 8;
    }

    bitmap2[16] = 0x0b07;
    bitmap2[17] = 0x0c01;
    bitmap2[18] = 0x0a05;
    bitmap2[19] = 0x0000;
    for (int i = 0; i < 16; i++)
    {
        bitmap2[i] = i << 8;
    }
}

void GPIOPortF_Init(void)
{

    SYSCTL_RCGC2_R |= 0x00000020; /* 1) activate clock for PortF */
    GPIO_PORTF_LOCK_R = 0x4C4F434B; /* 2) unlock GPIO PortF */
    GPIO_PORTF_CR_R = 0x1F; /* allow changes to PF4-0 */ //make port F configurable
    GPIO_PORTF_AMSEL_R = 0x00; /* 3) disable analog on PF */
    GPIO_PORTF_PCTL_R = 0x00000000; /* 4) PCTL GPIO on PF4-0 */
    GPIO_PORTF_DIR_R = 0x0E; /* 5) PF4,PF0 in, PF3-1 out */
    GPIO_PORTF_AFSEL_R = 0x00; /* 6) disable alt funct on PF7-0 */
    GPIO_PORTF_PUR_R = 0x11; /* enable pull-up on PF0 and PF4 */
    GPIO_PORTF_DEN_R = 0x1F; /* 7) enable digital I/O on PF4-0 */

    GPIO_PORTF_IS_R &= ~0x11; /*  PF4 is edge-sensitive */ //0 for edge sensitive
    GPIO_PORTF_IBE_R &= ~0x11; /*  PF4 is not both edges */ //
    GPIO_PORTF_IEV_R &= ~0x11; /*  PF4 falling edge event */ //0 for falling edge
    GPIO_PORTF_ICR_R = 0x11; /*  Clear flag4 */
    GPIO_PORTF_IM_R |= 0x11; /*  arm interrupt on PF4 */ //unmask interrupt
    NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF1FFFFF) | 0x00A00000; /*  priority 5 */
    NVIC_EN0_R = 0x40000000; /*  Enable interrupt 30 in NVIC */

    EnableInterrupts(); /* Enable global Interrupt flag (I) */
}

void GPIOPortF_Handler(void)
{
    volatile int readback;

    readback = GPIO_PORTF_RIS_R;
    if ((readback & 0x00000010) == 0x10)
    {
        index++;
        if (index == 4)
        {
            index = 0;
        }
        createBitmap();
    }
    else if ((readback & 0x00000001) == 0x01)
    {
        intensity += 5;
        uint16_t data = 0x0a00 | intensity;
        if (intensity == 0xf)
            intensity = 0;
       // SSI0_Write(data); /* write a character */
        Init_LedMatrix();
    }

    GPIO_PORTF_ICR_R = 0x11; /* clear PF4 int */

    GPIO_PORTF_DATA_R ^= (1 << 2); /* toggle Blue LED */

    readback = GPIO_PORTF_ICR_R; /* a read to force clearing of interrupt flag */
}
