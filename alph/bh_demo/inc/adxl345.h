#include <msp430x14x.h>
#include "i2c.h"


#define	SlaveAddress   0xA6	  //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
                              //ALT  ADDRESS���Žӵ�ʱ��ַΪ0xA6���ӵ�Դʱ��ַΪ0x3A
void Init_ADXL345(void);             //��ʼ��ADXL345
unsigned char  Single_Write_ADXL345(unsigned char REG_Address,unsigned char REG_data);   //����д������
unsigned char Single_Read_ADXL345(unsigned char REG_Address);                   //������ȡ�ڲ��Ĵ�������
//void  Multiple_Read_ADXL345(void);                                  //�����Ķ�ȡ�ڲ��Ĵ�������
void Multiple_read_ADXL345(void);
//------------------------------------
void ADXL345_SendByte(unsigned char dat);
unsigned char ADXL345_RecvByte();

extern unsigned char BUF[8];                         //�������ݻ�����      	
extern unsigned char ge,shi,bai,qian,wan;           //��ʾ����
extern int  dis_data;                       //����
extern unsigned char devid;
extern unsigned char err;