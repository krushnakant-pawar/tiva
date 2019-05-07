#include <stdint.h>
#include "inc/tm4c123gh6pm.h"

uint32_t ucControlTable[256] __attribute__ ((aligned(1024)));

#define CH11 (11*4)
#define CH11ALT (11*4+128)
#define BIT11 0x00000800
extern uint16_t bitmap2[64];
void DMA_Init(void)
{

    int i;
    volatile uint32_t delay;
    for (i = 0; i < 256; i++)
    {
        ucControlTable[i] = 0;
    }
    SYSCTL_RCGCDMA_R = 0x01;    // uDMA Module Run Mode Clock Gating Control
    delay = SYSCTL_RCGCDMA_R;   // allow time to finish

    UDMA_CFG_R = 0x01;          // MASTEN Controller Master Enable
    UDMA_CTLBASE_R = (uint32_t) ucControlTable; //aligned to 1024 boundary
    UDMA_CHMAP1_R = (UDMA_CHMAP1_R & 0xFFFF0FFF) | 0x00001000;  // SSI1 TX

    UDMA_PRIOCLR_R = BIT11;     // high priority
    UDMA_ALTCLR_R = BIT11;      // use primary control
    UDMA_USEBURSTCLR_R = BIT11; // responds to both burst and single requests
    UDMA_REQMASKCLR_R = BIT11; // allow the uDMA controller to recognize requests for this channel
   // printf("DMA_INIT\n");
}

uint16_t *SourcePt;       // last address of the source buffer, incremented by 2
volatile uint32_t *DestinationPt;
uint32_t Count = 4;
uint32_t increment = 0;

void static setRegular(void)
{
    ucControlTable[CH11] = (uint32_t) SourcePt;        // first and last address
    ucControlTable[CH11 + 1] = (uint32_t) DestinationPt; // last address (when DSTINC is 0x3)
    ucControlTable[CH11 + 2] = 0xD5000001 + ((Count - 1) << 4); // DMA Channel Control Word (DMACHCTL)

}

void DMA_Start()
{
    // printf("DMA_START\n");

    SourcePt = bitmap2 + increment + Count - 1;  // last address of source buffer
    //  printf("DMA_START\n");
    DestinationPt = &SSI1_DR_R;

    //  printf("DMA_START\n");
    setRegular();
    //  printf("DMA_START\n");
    UDMA_ENASET_R |= BIT11;  // DMA Channel 11 is enabled
    // SSI1_CR1_R |= 2; /* enable SSI1 */
 //   printf("DMA_START\n");
    increment += 4;

}

void SSI1_Handler(void)
{ // interrupts after each block is transferred

//printf("SSI1_CR1_R: %x\tSSI1_RIS_R: %x\n",SSI1_CR1_R,SSI1_RIS_R);


    if ((ucControlTable[CH11 + 2] & 0x0007) == 0)
    {     // regular buffer complete
      //  printf("SSI_HANDLER\n");

        if(increment == 67)
            increment = 0;
        SourcePt = bitmap2 + increment + Count - 1;  // last address of source buffer
        ucControlTable[CH11] = (uint32_t) SourcePt;    // first and last address
        ucControlTable[CH11 + 1] = (uint32_t) DestinationPt; // last address (when DSTINC is 0x3)
        ucControlTable[CH11 + 2] = 0xD5000001 + ((Count - 1) << 4); // DMA Channel Control Word (DMACHCTL)
        increment += 4;
        // rebuild channel control structure
    }

    uint32_t status = UDMA_CHIS_R;
   // printf("status: %x\n", status);
    status &= 0x00000800;
    if (status)
    {
        UDMA_CHIS_R = (UDMA_CHIS_R & (~0x00000800));
      //  printf("transfer complete\n");
    }
 //   Init_SSI1();
    delay_micro(10000);

}

// private function used to reprogram alternate channel control structure
void static setAlternate(void)
{
    ucControlTable[CH11ALT] = (uint32_t) SourcePt;     // first and last address
    ucControlTable[CH11ALT + 1] = (uint32_t) DestinationPt; // last address (when DSTINC is 0x3)
    ucControlTable[CH11ALT + 2] = 0xD5000003 + ((Count - 1) << 4); // DMA Channel Control Word (DMACHCTL)

}
