#include <stdio.h>
#include "NuMicro.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "hal_api.h"

uint32_t _CyclesPerUs      = 192; /* Cycles per micro second */

volatile uint16_t monRbuf[MONBUFSIZE + 32];
volatile uint16_t monRshorts = 0;
volatile uint16_t monRhead = 0;
volatile uint16_t monRtail = 0;
volatile uint8_t monMode = 2; // 2: I2C, 4: SPI, 5: RS485, 6: CAN


__weak uint32_t CLK_GetPLLClockFreq(void)
{
    return FREQ_192MHZ;
}

__weak uint32_t CLK_GetPCLK0Freq(void)
{
    return (FREQ_192MHZ / 2);
}

__weak uint32_t CLK_GetPCLK1Freq(void)
{
    return (FREQ_192MHZ / 2);
}

__weak void GPIO_SetMode(GPIO_T *port, uint32_t u32PinMask, uint32_t u32Mode)
{
    uint32_t i;

    for (i = 0ul; i < GPIO_PIN_MAX; i++) {
        if ((u32PinMask & (1ul << i)) == (1ul << i)) {
            port->MODE = (port->MODE & ~(0x3ul << (i << 1))) | (u32Mode << (i << 1));
        }
    }
}

void UART_Init(UART_T* uart, uint32_t u32baudrate)
{
    if (uart == (UART_T *)UART0) {
        SYS_Init_UART0();
    } else if (uart == (UART_T *)UART2) {
        SYS_Init_UART2();
    } else {
        return;
    }

    /* Init UART to 115200-8n1 for print message */
    uart->FUNCSEL = UART_FUNCSEL_UART;
    uart->LINE = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
    uart->FIFO = UART_FIFO_RFITL_1BYTE | UART_FIFO_RTSTRGLV_1BYTE;
    uart->BAUD = (UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HXT, u32baudrate));

    if (uart == (UART_T *)UART0) {
        printf("\n\n");
        printf("+-------------------------------------------------+\n");
        printf("|  M487 EVB Pin Configuration                     |\n");
        printf("|  UART0: TXD (134), RXD (135)   -- ICE VCOM      |\n");
        printf("|  I2C2: SCL (94), SDA (95)      -- I2C Master    |\n");
        printf("|  I2C0: SCL (63), SDA (64)      -- I2C Slave     |\n");
        printf("|  SPI0: SS (65), CLK (66), MISO (67), MOSI (68)  |\n");
        printf("+-------------------------------------------------+\n");
        printf("\n");
    } else {
        printf("\n\n");
        printf("+----------------------------------------------+\n");
        printf("|  Nu-Link2 Pin Configuration (CON6)           |\n");
        printf("|  UART2: TXD (1), RXD (2)                     |\n");
        printf("|  UI2C0: SCL (3), SDA (4)                     |\n");
        printf("|  SPI12: SS (5), CLK (6), MOSI (7), MISO (8)  |\n");
        printf("|  UART1: RS485_A (9), RS485_B (10)            |\n");
        printf("|  CAN1:  CAN_H (11), CAN_L (12)               |\n");
        printf("+----------------------------------------------+\n");
        printf("\n");
    }
}

#ifdef __cplusplus
}
#endif
