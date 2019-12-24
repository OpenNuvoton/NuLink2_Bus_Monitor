#include <stdio.h>
#include "hal_api.h"

int main()
{
    SYS_Init_192MHZ();
    UART_Init(UART2, 921600);
    UI2C0_Monitor_Init(100000);

    while (1) {
        UI2C0_Monitor_printf();
    }
    // warning:  #111-D: statement is unreachable
    // return 0;
}
