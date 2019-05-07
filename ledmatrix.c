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

#define SPEED 2
#define INTENSITY 0x0f

extern void PLL_Init(void);
void Init_SSI1(void);
void delay_micro(unsigned int);
void delayMs(int n);
void Init_LedMatrix(void);
void test_LedMatrix(void);

extern uint8_t bitmap[64];
extern uint8_t arr[64];
extern uint16_t bitmap2[68];
extern int index;
extern int repeat;
extern char str[][4];


uint8_t intensity = INTENSITY;
void scrollStr(void)
{
    char c;

    while (1)
    {
        uint8_t array[8];
        uint16_t k,data;
        for (int i = 0; i < 64; i++)
            bitmap[i] = arr[i];
        for (int e = 0; e < 32; e++)
        {
            for (int j = 0; j < 8; j++)
            {

                for (int i = 0; i < 8; i++)
                {
                    int tmp = (j << 3) + i; //multiply by 8
                    array[i] = bitmap[tmp];
                }

                int l;

                unsigned char savedCarry, carry;
                l = 4;

                // this will cause the array bit shift to be circular
                // by taking the "carry" bit (lowest bit) from the last element of the array
                // this "carry" bit will be used on the first element in the `for` loop

                carry = array[l - 1] & 1;

                // loop trought the array applying bit shift

                for (int i = 0; i < 4; i++)
                {
                    savedCarry = array[i] & 1;        // save the lowest bit
                    array[i] = array[i] >> 1;           // right shift
                    if (carry)
                    {
                        array[i] |= (unsigned char) 0x80; // the lowest bit of the previuos element becomes the highest bit of the current one
                    }
                    carry = savedCarry; // the lowest bit of this element will become the highest of the next one
                }

                // array[0] = array[0] & 0x7f;

                for (int i = 0; i < 8; i++)
                {
                    int tmp = (j << 3) + i; //multiply by 8
                    bitmap[tmp] = array[i];

                }
            }

            for (int l = 0; l < SPEED; l++)
            {

                k = 0;
                for (int i = 4; i < 64; i = i + 8)
                {

                    uint16_t msb = ((8 - k) << 8);
                    //data stream
                    data = msb | bitmap[i];
                    SSI0_Write(data); /* write a character */

                    data = msb | (bitmap[i + 1]);
                    SSI0_Write(data); /* write a character */

                    data = msb | bitmap[i + 2];
                    SSI0_Write(data); /* write a character */

                    data = msb | bitmap[i + 3];
                    SSI0_Write(data); /* write a character */

                    delay_micro(2500);

                    //zero stream
                    data = msb;

                    SSI0_Write(data); /* write a character */

                    SSI0_Write(data); /* write a character */

                    SSI0_Write(data);

                    SSI0_Write(data); /* write a character */
                    //  delay_micro(500);

                    k++;
                }

                k = 0;
                for (int i = 0; i < 64; i = i + 8)
                {

                    uint16_t msb = ((8 - k) << 8);
                    //data stream
                    data = msb | bitmap[i];
                    SSI0_Write(data); /* write a character */

                    data = msb | (bitmap[i + 1]);
                    SSI0_Write(data); /* write a character */

                    data = msb | bitmap[i + 2];
                    SSI0_Write(data); /* write a character */

                    data = msb | bitmap[i + 3];
                    SSI0_Write(data); /* write a character */

                    delay_micro(2500);

                    //zero stream
                    data = msb;

                    SSI0_Write(data); /* write a character */

                    SSI0_Write(data); /* write a character */

                    SSI0_Write(data);

                    SSI0_Write(data); /* write a character */
                    //  delay_micro(500);

                    k++;
                }

            }

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
        arr[u] = bitmap[u] = buf1[i]; //lsb led
        arr[u + 1] = bitmap[u + 1] = buf2[i]; //1st led
        arr[u + 2] = bitmap[u + 2] = buf3[i]; //2nd led
        arr[u + 3] = bitmap[u + 3] = buf4[i]; //msb led

        //  delay_micro(1000);

        //To pass stream of zeros
        arr[u + 4] = bitmap[u + 4] = msb;
        arr[u + 5] = bitmap[u + 5] = msb;
        arr[u + 6] = bitmap[u + 6] = msb;
        arr[u + 7] = bitmap[u + 7] = msb;

        u = u + 8;
    }
//    for (u = 0; u < 64; u = u + 4)
//    {
//        printf("0x%x,\t0x%x,\t0x%x,\t0x%x,\n", bitmap[u], bitmap[u + 1],
//               bitmap[u + 2], bitmap[u + 3]);
//    }

}

