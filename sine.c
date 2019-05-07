// ***** 2. Global Declarations Section *****
unsigned char D;
// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void); // Enable interrupts

// ***** 3. Subroutines Section *****

/*
 we are going to send a two byte command to CC.
 for doing so we should use ssi interface.

 STEPS FOR SENDING A COMMAND DESCRIBES BELOW:
 first we configure the cs to be zero.
 then we wait for 10 uS.
 then we send the command.
 again we wait for 10 uS.
 now we configure the cs to be high again.
 */

//SSI initialization
void SSI_Init(void)
{ //1. Enable the SSI module using the RCGCSSI register (see page 346).
    SYSCTL_RCGCSSI_R = (1<<2);
    //2. Enable the clock to the appropriate GPIO module via the RCGCGPIO register (see page 340).

    //To find out which GPIO port to enable, refer to Table 23-5 on page 1351.

    SYSCTL_RCGCGPIO_R = (1<<1); //3. Set the GPIO AFSEL bits for the appropriate pins (see page 671). To determine which GPIOs to //configure, see Table 23-4 on page 1344. GPIO_PORTB_AFSEL_R |= (1<<4)|(1<<6)|(1<<7)|(1<<5); //4. Configure the PMCn fields in the GPIOPCTL register to assign the SSI signals to the appropriate //pins. See page 688 and Table 23-5 on page 1351. GPIO_PORTB_PCTL_R |= (2<<16)|(2<<20)|(2<<24)|(2<<28); //PMC4,5,6,7 //5. Program the GPIODEN register to enable the pin's digital function. In addition, the drive strength, //drain select and pull-up/pull-down functions must be configured. Refer to “General-Purpose //Input/Outputs (GPIOs)” on page 649 for more information. //Note: Pull-ups can be used to avoid unnecessary toggles on the SSI pins, which can take the //slave to a wrong state. In addition, if the SSIClk signal is programmed to steady state //High through the SPO bit in the SSICR0 register, then software must also configure the //GPIO port pin corresponding to the SSInClk signal as a pull-up in the GPIO Pull-Up //Select (GPIOPUR) register. GPIO_PORTB_DEN_R |= (1<<4)|(1<<5)|(1<<6)|(1<<7);

//For each of the frame formats, the SSI is configured using the following steps:

//1. Ensure that the SSE bit in the SSICR1 register is clear before making any configuration changes. SSI2_CR1_R &= ~(1<<1); //2. Select whether the SSI is a master or slave: //a. For master operations, set the SSICR1 register to 0x0000.0000. //b. For slave mode (output enabled), set the SSICR1 register to 0x0000.0004. //c. For slave mode (output disabled), set the SSICR1 register to 0x0000.000C. SSI2_CR1_R &= 0x00000000; //3. Configure the SSI clock source by writing to the SSICC register. SSI2_CC_R = 0X00; //4. Configure the clock prescale divisor by writing the SSICPSR register. SSI2_CPSR_R = 16; //the frequency of clk is 1 MHz. //5. Write the SSICR0 register with the following configuration: //¦ Serial clock rate (SCR) //¦ Desired clock phase/polarity, if using Freescale SPI mode (SPH and SPO) //¦ The protocol mode: Freescale SPI, TI SSF, MICROWIRE (FRF) //¦ The data size (DSS) SSI2_CR0_R = (0X07<<0);

//6. Optionally, configure the SSI module for µDMA use with the following steps: //a. Configure a µDMA for SSI use. See “Micro Direct Memory Access (µDMA)” on page 585 for //more information. //b. Enable the SSI Module's TX FIFO or RX FIFO by setting the TXDMAE or RXDMAE bit in the //SSIDMACTL register.

//7. Enable the SSI by setting the SSE bit in the SSICR1 register. SSI2_CR1_R &= (1<<1);
}

//Function for write in SPI
void WriteSPI(unsigned char data)
{
    SSI2_DR_R = data;
    while ((SSI2_SR_R) * (1 << 0) == 0)
        ;
}

//Create a delay for 10 uS
void Delay(void)
{
    unsigned long volatile time;
    time = 10; // 10 micro sec while(time){ time--; }
} // first data point is wrong, the other 49 will be correct
unsigned long Time[50];
// you must leave the Data array defined exactly as it is
unsigned long Data[50];
unsigned long in;
int main(void)
{
    unsigned long i, last, now;
    TExaS_Init(SW_PIN_PF40, LED_PIN_PF1); // activate grader and set system clock to 16 MHz SSI_Init();

    EnableInterrupts();

    WriteSPI(0x30); //SRES command Delay(); WriteSPI(0x3D); //SNOP command Delay();

    while (1)
    {
        WriteSPI(0x30); //SRES command Delay(); WriteSPI(0x3D); //SNOP command Delay();

    }
}
