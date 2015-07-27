/********************************************************************
//I2C������غ�������
//���Ի�����EW430 V5.30

********************************************************************/

#include    <msp430x14x.h>
#include "config.h"
/*I2C�ܽŶ���*/
#define SCL_H P5OUT |= BIT0                       //SCL�ܽ��ø�
#define SCL_L P5OUT &= ~BIT0                      //SCL�ܽ��õ�
#define SDA_H P5OUT |= BIT1                       //SDA�ܽ��ø�
#define SDA_L P5OUT &= ~BIT1                     //SDA�ܽ��õ�

#define SDA_IN  P5DIR &= ~BIT1                    //SDA����ģʽ
#define SDA_OUT P5DIR |= BIT1                     //SDA���ģʽ
#define SDA_VAL P5IN&BIT1                         //SDA��λֵ��ȡ

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
