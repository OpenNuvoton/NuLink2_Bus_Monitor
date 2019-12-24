#ifndef __HAL_API_H__
#define __HAL_API_H__

#include <stdio.h>
#include "NuMicro.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "hal_sys_init.h"

#ifdef EXPORT_LIB
#define printf(...)
#define EP_HID_IN  EPJ
#define EP_HID_OUT EPK
#else
#define __set_PRIMASK(...)
#define EP_HID_IN  EPA
#define EP_HID_OUT EPB

#endif

extern uint32_t _CyclesPerUs;         /*!< Cycles per micro second              */

// hal_hsusbd.c
//void _EP_HID_IN_Handler(uint32_t u32Ep, uint8_t *pu8Buf);  /* Interrupt IN handler */
void _EP_HID_IN_Handler(uint32_t u32Ep, uint8_t *pu8Buf, uint32_t u32rLen);  /* Interrupt IN handler */
void _EP_HID_OUT_Handler(uint32_t u32Ep, uint8_t *pu8Buf);  /* Interrupt OUT handler */

// hal_spi.c
void SPI1_Init(uint32_t Pclk0);
uint32_t SPI1_Write(uint32_t *buf, uint32_t len);
uint32_t SPI1_Read(uint32_t *buf, uint32_t len);

// hal_sys.c
void UART_Init(UART_T* uart, uint32_t u32baudrate); // for printf
void PrintfBufByte(uint8_t *Buf, uint32_t len);
void PrintfBufWord(uint32_t *Buf, uint32_t len);

// hal_usci_i2c.c
void UI2C0_Init(uint32_t Pclk0, uint32_t u32BusClock);
uint32_t UI2C_WriteMultiBytes(UI2C_T *ui2c, uint8_t u8SlaveAddr, uint8_t *data, uint32_t u32wLen);
uint32_t UI2C_ReadMultiBytes(UI2C_T *ui2c, uint8_t u8SlaveAddr, uint8_t *rdata, uint32_t u32rLen);

// hal_timer.c
void TIMER3_Init(void);

// hal_rs485.c
void RS485_Init(void);
void RS485_WriteMultiBytes(uint8_t *data);
// hal_rs485_mon.c
void RS485_Init(void);
void RS485_Monitor_printf(void);

// hal_can_isp.c
extern volatile uint8_t u8CAN_PackageFlag;
extern STR_CANMSG_T rrMsg;
void CAN_Init(void);
int32_t CAN_Package_Tx(CAN_T *tCAN, uint8_t *data);

#define monAll (1)
extern volatile uint8_t monMode; // 2: I2C, 4: SPI, 5: RS485, 6: CAN

// Buffer for Monitor mode
#define MONBUFSIZE 0x10000
#define MONBUFMASK (MONBUFSIZE - 1)

extern volatile uint16_t monRbuf[];
extern volatile uint16_t monRshorts;
extern volatile uint16_t monRhead;
extern volatile uint16_t monRtail;

// hal_i2c_mon.c
void UI2C0_Monitor_Init(uint32_t u32ClkSpeed);
void UI2C0_Monitor_printf(void);

// hal_spi_mon.c
void SPI12_Monitor_Init(void);
void SPI12_Monitor_printf(void);

// hal_can_mon.c
void CAN_Init(void);
void CAN_Monitor_printf(void);


__STATIC_INLINE void NL2_UART_Init(void)
{
    UART_Init(UART2, 921600);
}

#ifdef __cplusplus
}
#endif

#endif // #ifndef __HAL_API_H__
