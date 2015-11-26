#include <msp430x14x.h>
#include "i2c.h"


#define	SlaveAddress   0xA6	  //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
                              //ALT  ADDRESS引脚接地时地址为0xA6，接电源时地址为0x3A
void Init_ADXL345(void);             //初始化ADXL345
unsigned char  Single_Write_ADXL345(unsigned char REG_Address,unsigned char REG_data);   //单个写入数据
unsigned char Single_Read_ADXL345(unsigned char REG_Address);                   //单个读取内部寄存器数据
//void  Multiple_Read_ADXL345(void);                                  //连续的读取内部寄存器数据
void Multiple_read_ADXL345(void);
//------------------------------------
void ADXL345_SendByte(unsigned char dat);
unsigned char ADXL345_RecvByte();

extern unsigned char BUF[8];                         //接收数据缓存区      	
extern unsigned char ge,shi,bai,qian,wan;           //显示变量
extern int  dis_data;                       //变量
extern unsigned char devid;
extern unsigned char err;