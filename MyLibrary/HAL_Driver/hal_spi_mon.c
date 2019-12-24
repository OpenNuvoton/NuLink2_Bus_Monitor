#include "NuMicro.h"
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include "hal_api.h"

void SPI12_Monitor_Init(void)
{
    SYS_Init_SPI12();
    /* Configure as a slave, clock idle low, 32-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    /* Configure SPI as a low level active device. */
    /* Disable I2S mode */
    SPI1->I2SCTL &= ~SPI_I2SCTL_I2SEN_Msk;
    SPI2->I2SCTL &= ~SPI_I2SCTL_I2SEN_Msk;
    /* Default setting: slave selection signal is low level active. */
    SPI1->SSCTL = SPI_SS_ACTIVE_LOW;
    SPI2->SSCTL = SPI_SS_ACTIVE_LOW;
    /* Default setting: MSB first, disable unit transfer interrupt, SP_CYCLE = 0. */
    SPI1->CTL = SPI_SLAVE | ((8 & 0x1F) << SPI_CTL_DWIDTH_Pos) | (SPI_MODE_0) | SPI_CTL_SPIEN_Msk;
    SPI2->CTL = SPI_SLAVE | ((8 & 0x1F) << SPI_CTL_DWIDTH_Pos) | (SPI_MODE_1) | SPI_CTL_SPIEN_Msk;
    /* Set DIVIDER = 0 */
    SPI1->CLKDIV = 0U;
    SPI2->CLKDIV = 0U;
    /* Set TX FIFO threshold and enable FIFO mode. */
    SPI1->FIFOCTL = (SPI1->FIFOCTL & ~(SPI_FIFOCTL_TXTH_Msk | SPI_FIFOCTL_RXTH_Msk)) |
                    (4 << SPI_FIFOCTL_TXTH_Pos) |
                    (4 << SPI_FIFOCTL_RXTH_Pos);
    SPI2->FIFOCTL = (SPI2->FIFOCTL & ~(SPI_FIFOCTL_TXTH_Msk | SPI_FIFOCTL_RXTH_Msk)) |
                    (4 << SPI_FIFOCTL_TXTH_Pos) |
                    (4 << SPI_FIFOCTL_RXTH_Pos);
    /* Enable slave selection signal active interrupt flag */
    SPI1->SSCTL |= SPI_SSCTL_SSACTIEN_Msk;
    // SPI2->SSCTL |= SPI_SSCTL_SSACTIEN_Msk;
    // SPI_WRITE_TX(spi, 0xFFFFFFFF);    /* Dummy Write to prevent TX under run */
    NVIC_EnableIRQ(SPI1_IRQn);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  SPI IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void SPI1_IRQHandler(void)
{
    if (SPI1->STATUS & SPI_STATUS_SSACTIF_Msk) {
        SPI1->STATUS |= SPI_STATUS_SSACTIF_Msk;
        SPI1->FIFOCTL |= SPI_FIFOCTL_RXFBCLR_Msk;
        SPI2->FIFOCTL |= SPI_FIFOCTL_RXFBCLR_Msk;
        #if monAll
        if (monMode != 4) {
            monMode = 4; // SPI

            if (monRshorts == 0) {
                monRtail = 0;
                monRhead = 0;
            }
        }
        #endif
        SysTick->LOAD = 1000 * CyclesPerUs;
        SysTick->VAL   = (0x00);
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

        // Active
        while (!(SPI1->STATUS & SPI_STATUS_SSINAIF_Msk)) {
            /* Check RX EMPTY flag */
            if ((SPI_GET_RX_FIFO_EMPTY_FLAG(SPI1) == 0)
                && (SPI_GET_RX_FIFO_EMPTY_FLAG(SPI2) == 0)) {
                monRbuf[monRtail++] = (SPI_READ_RX(SPI1) | (SPI_READ_RX(SPI2) << 8));
                monRtail &= MONBUFMASK;
                monRshorts++;
                /* Disable SysTick counter */
                SysTick->CTRL = 0UL;
                SysTick->LOAD = 1000 * CyclesPerUs;
                SysTick->VAL   = (0x00);
                SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
            }

            if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) {
                /* Disable SysTick counter */
                SysTick->CTRL = 0UL;
                break;
            }
        }

        if (SPI1->STATUS & SPI_STATUS_SSINAIF_Msk) {
            SPI1->STATUS |= SPI_STATUS_SSINAIF_Msk;
        }
        /* Disable SysTick counter */
        SysTick->CTRL = 0UL;
    } else {
    }
}

void SPI12_Monitor_printf(void)
{
    if (monRshorts) {
        uint16_t _monRshorts = monRshorts, i, data;
        for(i = 0; i < _monRshorts; i++) {
            if((monRhead & 0x0F) == 0) {
                printf("\n(SPI) ");
            }
            data = monRbuf[monRhead++];
            printf("%04X ", data);
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
