#include <stdio.h>
#include "hal_api.h"

int main()
{
    SYS_Init_192MHZ();
    UART_Init(UART2, 921600);
    UI2C0_Monitor_Init(100000);
    SPI12_Monitor_Init();
    RS485_Init();
    CAN_Init();

    while (1) {
        if (monMode == 2) {
            UI2C0_Monitor_printf();
        } else if (monMode == 4) {
            SPI12_Monitor_printf();
        } else if (monMode == 5) {
            RS485_Monitor_printf();
        } else if (monMode == 6) {
            CAN_Monitor_printf();
        }
    }
    // warning:  #111-D: statement is unreachable
    // return 0;
}
