#include <stdio.h>
#include "hal_api.h"

int main()
{
    SYS_Init_192MHZ();
    UART_Init(UART2, 921600);
    RS485_Init();

    while (1) {
        RS485_Monitor_printf();
    }
    // warning:  #111-D: statement is unreachable
    // return 0;
}
