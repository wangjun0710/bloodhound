/********************************************************************
//I2C操作相关函数声明
//调试环境：EW430 V5.30

********************************************************************/

#include    <msp430x14x.h>
#include "config.h"
/*I2C管脚定义*/
#define SCL_H P5OUT |= BIT0                       //SCL管脚置高
#define SCL_L P5OUT &= ~BIT0                      //SCL管脚置低
#define SDA_H P5OUT |= BIT1                       //SDA管脚置高
#define SDA_L P5OUT &= ~BIT1                     //SDA管脚置低

#define SDA_IN  P5DIR &= ~BIT1                    //SDA输入模式
#define SDA_OUT P5DIR |= BIT1                     //SDA输出模式
#define SDA_VAL P5IN&BIT1                         //SDA的位值读取

void delay(void);
void start(void);
void stop(void);
void mack(void);
void mnack(void);
unsigned char check(void);
void write1(void);
void write0(void);
void write1byte(unsigned char wdata);
unsigned char writeNbyte(unsigned char * outbuffer,unsigned char n);
unsigned char read1byte(void);
void readNbyte(unsigned char * inbuffer,unsigned char n);
