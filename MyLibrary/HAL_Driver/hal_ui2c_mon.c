#include "NuMicro.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define SCLOUT_ENABLE 0

#include "hal_usci_i2c.c"
#include "hal_api.h"

// Monitor (Slave)
void UI2C0_Monitor_Init(uint32_t u32ClkSpeed)
{
    SYS_Init_UI2C0();
    /* Open USCI_I2C0 and set clock to 100k */
    UI2C0_Open((FREQ_192MHZ / 2), u32ClkSpeed);
    /* Set USCI_I2C0 Slave Addresses */
    // UI2C_SetSlaveAddr(UI2C0, 0, 0xFF, UI2C_GCMODE_DISABLE);
    UI2C0->DEVADDR0  = 0xFF;
    UI2C0->PROTCTL  = (UI2C0->PROTCTL & ~UI2C_PROTCTL_GCFUNC_Msk) |UI2C_GCMODE_DISABLE;

    /* Enable Monitor Mode */
#if SCLOUT_ENABLE
    UI2C0->PROTCTL |= (UI2C_PROTCTL_MONEN_Msk | UI2C_PROTCTL_SCLOUTEN_Msk);
#else
    UI2C0->PROTCTL |= UI2C_PROTCTL_MONEN_Msk;
#endif

    // UI2C_SetSlaveAddrMask(UI2C0, 0, 0xFF);
    UI2C0->ADDRMSK0  = 0xFF;

    /* Enable UI2C0 protocol interrupt */
    UI2C_ENABLE_PROT_INT(UI2C0, (UI2C_PROTIEN_ACKIEN_Msk |
                                 UI2C_PROTIEN_NACKIEN_Msk |
                                 UI2C_PROTIEN_STORIEN_Msk |
                                 UI2C_PROTIEN_STARIEN_Msk));
    NVIC_EnableIRQ(USCI0_IRQn);
    UI2C_SET_CONTROL_REG(UI2C0, UI2C_CTL_AA);
}

void USCI0_IRQHandler(void)
{
    uint32_t u32Status;
    u32Status = (UI2C0->PROTSTS);
    monRshorts++;
#if monAll
    monMode = 2; // I2C
#endif
    if ((u32Status & UI2C_PROTSTS_STARIF_Msk) == UI2C_PROTSTS_STARIF_Msk) {
        UI2C_CLR_PROT_INT_FLAG(UI2C0, UI2C_PROTSTS_STARIF_Msk);
        monRbuf[monRtail++] = 0x5300; // 'S';
        monRtail &= MONBUFMASK;
        UI2C_SET_CONTROL_REG(UI2C0, (UI2C_CTL_PTRG | UI2C_CTL_AA));
    } else if ((u32Status & UI2C_PROTSTS_ACKIF_Msk) == UI2C_PROTSTS_ACKIF_Msk) {
        UI2C_CLR_PROT_INT_FLAG(UI2C0, UI2C_PROTSTS_ACKIF_Msk);
        monRbuf[monRtail++] = (0x4100 | UI2C_GET_DATA(UI2C0)); // 'A';
        monRtail &= MONBUFMASK;
        UI2C_SET_CONTROL_REG(UI2C0, (UI2C_CTL_PTRG | UI2C_CTL_AA));
    } else if ((u32Status & UI2C_PROTSTS_NACKIF_Msk) == UI2C_PROTSTS_NACKIF_Msk) {
        UI2C_CLR_PROT_INT_FLAG(UI2C0, UI2C_PROTSTS_NACKIF_Msk);
        monRbuf[monRtail++] = (0x4E00 | UI2C_GET_DATA(UI2C0)); // 'N';
        monRtail &= MONBUFMASK;
        UI2C_SET_CONTROL_REG(UI2C0, (UI2C_CTL_PTRG | UI2C_CTL_AA));
    } else if ((u32Status & UI2C_PROTSTS_STORIF_Msk) == UI2C_PROTSTS_STORIF_Msk) {
        UI2C_CLR_PROT_INT_FLAG(UI2C0, UI2C_PROTSTS_STORIF_Msk);
        monRbuf[monRtail++] = 0x5000; // 'P';
        monRtail &= MONBUFMASK;
        UI2C_SET_CONTROL_REG(UI2C0, (UI2C_CTL_PTRG | UI2C_CTL_AA));
    } else {
        monRshorts--;
    }
}

void UI2C0_Monitor_printf(void)
{
    if (monRshorts) {
        uint8_t ch;
        uint16_t _monRshorts = monRshorts, i, data;
        for(i = 0; i < _monRshorts; i++) {
            data = monRbuf[monRhead++];
            if (data == 0x5300) {
                // printf("\nS ");
                printf("\n(I2C) S ");
            } else 	if (data == 0x5000) {
                printf("P");
            } else {
                ch = (data >> 8);
                printf("%02X ", (data & 0xFF));
                // printf("%c ", ch);
                if(ch == 'N') {
                    printf("%c ", ch);
                }
            }
            monRhead &= MONBUFMASK;
        }

        __set_PRIMASK(1);
        monRshorts -= _monRshorts;
        __set_PRIMASK(0);
    }
}

#ifdef __cplusplus
}
#endif
