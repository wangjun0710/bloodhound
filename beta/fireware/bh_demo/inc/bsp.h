#include    <msp430x14x.h>
#include "config.h"

void Clock_Init();
void WDT_Init();
void TimerA_init();
void Port_Init();
void UART0_Init(unsigned long baud);
void UART1_Init(unsigned long baud);
void Send_Byte_UART0(uchar data);
void Print_Str_UART0(uchar *s);
void Send_Byte_UART1(uchar data);
void Print_Str_UART1(uchar *s);
void ADC_Init(void);