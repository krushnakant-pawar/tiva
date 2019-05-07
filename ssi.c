/*
 * ssi.c
 *
 *  Created on: 04-May-2019
 *      Author: krushnakant
 */
#include <stdio.h>
//#include "font.h"
//#include "inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "inc/tm4c123gh6pm.h"

extern uint8_t bitmap[64];
extern uint8_t arr[64];
extern uint16_t bitmap2[68];
extern int index;
extern int repeat;
extern uint8_t intensity;
extern char str[][4];


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


void delay_micro(unsigned int n)
{
    for (unsigned long i = 0; i <= 3 * n; i++)
        ;
}

void delayMs(int n)
{
    int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < 3180; j++)
        {
        }
}



