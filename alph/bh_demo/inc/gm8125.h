
#include    <msp430x14x.h>
#include "config.h"


#define GM8125_RST_0 P4OUT &= ~BIT7
#define GM8125_RST_1 P4OUT |= BIT7

#define READ_PIN P4IN
#define PIN_MASK 0x70
#define BITNUM 4


#define SRADD2_0  P4OUT &= ~BIT6
#define SRADD2_1 P4OUT |= BIT6
#define SRADD1_0  P4OUT &= ~BIT5
#define SRADD1_1 P4OUT |= BIT5
#define SRADD0_0  P4OUT &= ~BIT4
#define SRADD0_1 P4OUT |= BIT4

#define STADD2_0  P4OUT &= ~BIT3
#define STADD2_1 P4OUT |= BIT3
#define STADD1_0  P4OUT &= ~BIT2
#define STADD1_1 P4OUT |= BIT2
#define STADD0_0  P4OUT &= ~BIT1
#define STADD0_1 P4OUT |= BIT1
#define GM8125_MS_0 P4OUT &= ~BIT0
#define GM8125_MS_1 P4OUT |= BIT0

#define GM8125_S_MODLE 1
#define GM8125_M_MODLE 0

void GM8125_Init(unsigned char gm8125_modle);
void GM8125_Send_Byte_Smoddle(unsigned char channel,unsigned char data);
void GM8125_Print_Str_Smoddle(unsigned char channel,unsigned char *s);
void GM8125_Recieve_Byte_Smoddle(unsigned char channel,unsigned char data);

void GM8125_Send_Byte_Mmoddle(unsigned char channel,unsigned char data);
void GM8125_Print_Str_Mmoddle(unsigned char channel,unsigned char *s);
void GM8125_Recieve_Byte_Mmoddle(unsigned char data);

extern void Send_Byte_UART1(uchar data);
extern unsigned char recievedata[6];
extern void UART1_Init(unsigned long baud);