#include <stdio.h>
#include "NuMicro.h"
#include "hal_api.h"

int main()
{
    SYS_Init_192MHZ();
    NL2_UART_Init();
    CAN_Init();

    while (1) {
        CAN_Monitor_printf();
    };
}
