#include "NuMicro.h"
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include "hal_api.h"


/**
  * @brief  CAN0_IRQ Handler.
  * @param  None.
  * @return None.
  */
void CAN1_IRQHandler(void)
{
    if (CAN1->IIDR == 0x00008000) {     /* Check Status Interrupt Flag (Error status Int and Status change Int) */
        /**************************/
        /* Status Change interrupt*/
        /**************************/
        if (CAN1->STATUS & CAN_STATUS_RXOK_Msk) {
            CAN1->STATUS &= ~CAN_STATUS_RXOK_Msk;   /* Clear Rx Ok status*/
            // CAN_BasicReceiveMsg
            CAN1->IF[1].CMASK = CAN_IF_CMASK_ARB_Msk
                                | CAN_IF_CMASK_CONTROL_Msk
                                | CAN_IF_CMASK_DATAA_Msk
                                | CAN_IF_CMASK_DATAB_Msk;
            monMode = 6; // CAN
            monRtail &= MONBUFMASK;
            monRbuf[monRtail++] = CAN1->IF[1].ARB2;
            monRbuf[monRtail++] = CAN1->IF[1].ARB1;
            monRbuf[monRtail++] = CAN1->IF[1].MCON;
            monRbuf[monRtail++] = CAN1->IF[1].DAT_A1;
            monRbuf[monRtail++] = CAN1->IF[1].DAT_A2;
            monRbuf[monRtail++] = CAN1->IF[1].DAT_B1;
            monRbuf[monRtail++] = CAN1->IF[1].DAT_B2;
            monRtail &= MONBUFMASK;
            monRshorts += 7;
        }
    }
}

void CAN_Init(void)
{
    SYS_Init_CAN1();
    CAN_Open(CAN1, 500000, CAN_BASIC_MODE);
    // Set Silent Mode
    CAN1->TEST |= (0x1 << 3);
    CAN_EnableInt(CAN1, (CAN_CON_IE_Msk | CAN_CON_SIE_Msk | CAN_CON_EIE_Msk));
    NVIC_SetPriority(CAN1_IRQn, (1 << __NVIC_PRIO_BITS) - 2);
    NVIC_EnableIRQ(CAN1_IRQn);
}

void CAN_Monitor_printf(void)
{
    if (monRshorts) {
        uint32_t Id;
        uint16_t data, DLC;
        monRhead &= MONBUFMASK;
        data = monRbuf[monRhead++]; // ARB2
        Id = monRbuf[monRhead++]; // ARB1
        DLC = monRbuf[monRhead++] & 0x000F; // MCON

        if ((data & CAN_IF_ARB2_XTD_Msk) == 0ul) {
            /* standard ID (11 Bit) */
            Id = (data >> 2) & 0x07FFul;
            printf("\n(CAN) STD(%3X), DLC(%d), DATA:", Id, DLC);
        } else {
            /* extended ID (29 Bit) */
            Id |= (data & 0x1FFFul) << 16;
            printf("\n(CAN) EXT(%8X), DLC(%d), DATA:", Id, DLC);
        }

        printf(" %04X", monRbuf[monRhead++]);
        printf(" %04X", monRbuf[monRhead++]);
        printf(" %04X", monRbuf[monRhead++]);
        printf(" %04X", monRbuf[monRhead++]);
        monRhead &= MONBUFMASK;
        __set_PRIMASK(1);
        monRshorts -= 7;
        __set_PRIMASK(0);
    }
}


#ifdef __cplusplus
}
#endif
