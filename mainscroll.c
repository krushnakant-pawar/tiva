/*
 * main.c
 *
 *  Created on: 04-Feb-2019
 *      Author: krushnakant
 */
//#include <stdint.h>
#include <stdio.h>

#include <stdint.h>
#include "inc/tm4c123gh6pm.h"

extern void PLL_Init(void);

extern void Init_SSI0(void);

extern void Init_LedMatrix(void);
extern void test_LedMatrix(void);

uint8_t bitmap[64];
uint8_t arr[64];
uint16_t bitmap2[68];
int index = 0;
int repeat = 0;

char str[][4] = { "ABCD", "IISC", "wxyz", "Dese" };
int main(void)
{
    char c;
    uint16_t data;
    PLL_Init();
    GPIOPortF_Init();
    Init_SSI0();
    Init_LedMatrix();
    //test_LedMatrix();
    //To get data from dictionary
    createBitmap();

    while (1)
    {
        scrollStr();

    }
}

