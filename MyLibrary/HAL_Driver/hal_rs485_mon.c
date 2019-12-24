#include <stdio.h>
#include "NuMicro.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "hal_api.h"


void RS485_Init(void)
{
    SYS_Init_RS485();
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Select UART function */
    UART1->FUNCSEL = UART_FUNCSEL_UART;
    /* Set UART line configuration */
    UART1->LINE = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
    /* Set UART Rx and RTS trigger level */
    UART1->FIFO = UART_FIFO_RFITL_14BYTES | UART_FIFO_RTSTRGLV_14BYTES;
    /* Set UART baud rate */
    UART1->BAUD = (UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HIRC, 115200));
    /* Set time-out interrupt comparator */
    UART1->TOUT = (UART1->TOUT & ~UART_TOUT_TOIC_Msk) | (0x40);
    NVIC_SetPriority(UART1_IRQn, 2);
    NVIC_EnableIRQ(UART1_IRQn);
    /* 0x0811 */
    UART1->INTEN = (UART_INTEN_TOCNTEN_Msk | UART_INTEN_RXTOIEN_Msk | UART_INTEN_RDAIEN_Msk);
}

/* please check "targetdev.h" for chip specifc define option */
void UART1_IRQHandler(void)
{
    /*----- Determine interrupt source -----*/
    uint32_t u32IntSrc = UART1->INTSTS;

    #if monAll
    if (monMode != 5) {
        monMode = 5; // RS485
    
        if (monRshorts == 0) {
            monRtail = 0;
            monRhead = 0;
        }
    }
    #endif

    if (u32IntSrc & 0x11) { //RDA FIFO interrupt & RDA timeout interrupt
        while ((UART1->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk) == 0) {	//RX fifo not empty
            monRbuf[monRtail++] = UART1->DAT;
            monRtail &= MONBUFMASK;
            monRshorts++;
        }
    }
}

void RS485_Monitor_printf(void)
{
    if (monRshorts) {
        uint16_t _monRshorts = monRshorts, i, data;
        for(i = 0; i < _monRshorts; i++) {
            if((monRhead & 0x0F) == 0) {
                printf("\n(RS485) ");
            }
            data = monRbuf[monRhead++];
            printf("%02X ", data);
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
