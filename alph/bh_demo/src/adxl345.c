#include "adxl345.h"

 unsigned char BUF[8];                         //接收数据缓存区      	
 unsigned char ge,shi,bai,qian,wan;           //显示变量
 int  dis_data;                       //变量
  unsigned char devid;
  unsigned char err;


void ADXL345_Start()
{
    
   start();
}
/**************************************
停止信号
**************************************/
void ADXL345_Stop()
{
   stop();
}

/**************************************
向IIC总线发送一个字节数据
**************************************/
void ADXL345_SendByte(unsigned char dat)
{
   write1byte(dat);
}

/**************************************
从IIC总线接收一个字节数据
**************************************/
unsigned char ADXL345_RecvByte()
{

    unsigned char dat = 0;
    dat = read1byte();
    return dat;
}

//******单字节写入*******************************************

unsigned char Single_Write_ADXL345(unsigned char REG_Address,unsigned char REG_data)
{
    ADXL345_Start();                  //起始信号
    ADXL345_SendByte(SlaveAddress);   //发送设备地址+写信号
    
    if(check()) 
    ADXL345_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf22页 
    else
    return 0; 
    
    if(check()) 
    ADXL345_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf22页 
    else
    return 0; 
    
    if(check()) 
    ADXL345_Stop();                   //发送停止信号
    else
    return 0; 
    
    delay_ms(1);                           //等待EEPROM完成内部写入
    return 1; 
}

//********单字节读取*****************************************
unsigned char Single_Read_ADXL345(unsigned char REG_Address)
{  unsigned char REG_data;
    ADXL345_Start();                          //起始信号
    ADXL345_SendByte(SlaveAddress);           //发送设备地址+写信号
    if(check()) 
    {
    ADXL345_SendByte(REG_Address);            //发送存储单元地址，从0开始
    }
    else
    {
      err = 1;
    return 0;
    }
    
    if(check()) 
    {
       ADXL345_Start();                          //起始信号
    }
     else
    {
      err = 1;
      return 0;
    }
    
    ADXL345_SendByte(SlaveAddress+1);         //发送设备地址+读信号
     if(check()) 
     {
       REG_data=ADXL345_RecvByte();              //读出寄存器数据
     }
     else
    {
      err = 1;
    return 0;
    }
    
    mnack();  
	ADXL345_Stop();                           //停止信号
    return REG_data; 
}
//*********************************************************
//
//连续读出ADXL345内部加速度数据，地址范围0x32~0x37
//
//*********************************************************
void Multiple_read_ADXL345(void)
{   unsigned char i;
    ADXL345_Start();                          //起始信号
    ADXL345_SendByte(SlaveAddress);           //发送设备地址+写信号
    if(check()) 
    {
        ADXL345_SendByte(0x32);                   //发送存储单元地址，从0x32开始	
    }
    else
    {
        err = 1;
    }
    if(check()) 
    {
        ADXL345_Start();                          //起始信号
    }
    else
    {
      err = 1;
    }
    ADXL345_SendByte(SlaveAddress+1);         //发送设备地址+读信号
    if(check()) 
    {
    for (i=0; i<6; i++)                      //连续读取6个地址数据，存储中BUF
    {
        BUF[i] = ADXL345_RecvByte();          //BUF[0]存储0x32地址中的数据
        if (i == 5)
        {
          mnack();
        }
        else
        {
          mack();
       }
   }
    }
    else
    {
      err = 1;
    }
    ADXL345_Stop();                          //停止信号
    delay_us(100);
}
 

//*****************************************************************

//初始化ADXL345，根据需要请参考pdf进行修改************************
void Init_ADXL345()
{
   Single_Write_ADXL345(0x31,0x0B);   //测量范围,正负16g，13位模式
   Single_Write_ADXL345(0x2C,0x0E);   //速率设定为12.5 参考pdf13页
   Single_Write_ADXL345(0x2D,0x08);   //选择电源模式   参考pdf24页
   Single_Write_ADXL345(0x2E,0x80);   //使能 DATA_READY 中断
   Single_Write_ADXL345(0x1E,0x00);   //X 偏移量 根据测试传感器的状态写入pdf29页
   Single_Write_ADXL345(0x1F,0x00);   //Y 偏移量 根据测试传感器的状态写入pdf29页
   Single_Write_ADXL345(0x20,0x00);   //Z 偏移量 根据测试传感器的状态写入pdf29页
}
/*
void ADXL345_Adjust(char *xval,char *yval,char *zval)
{
   unsigned char i;
   unsigned char ofsx,ofsy,ofsz;
   
   for (i=0; i<10; i++)
   {
      ADXL345_Start();                          //起始信号
      ADXL345_SendByte(SlaveAddress);           //发送设备地址+写信号
      cack();
      ADXL345_SendByte(0x32);                   //发送存储单元地址，从0x32开始	
      cack();
      ADXL345_Start();                          //起始信号
      ADXL345_SendByte(SlaveAddress+1);         //发送设备地址+读信号
      cack();
      for (i=0; i<6; i++)                      //连续读取6个地址数据，存储中BUF
      {
        BUF[i] = ADXL345_RecvByte();          //BUF[0]存储0x32地址中的数据
        if (i == 5)
        {
          mnack();
        }
        else
        {
          mack();
       }
      }
    ADXL345_Stop();                          //停止信号
    delayus(100);
    
   }
   Single_Write_ADXL345(0x1E,0x00);   //X 偏移量 根据测试传感器的状态写入pdf29页
   Single_Write_ADXL345(0x1F,0x00);   //Y 偏移量 根据测试传感器的状态写入pdf29页
   Single_Write_ADXL345(0x20,0x00);   //Z 偏移量 根据测试传感器的状态写入pdf29页
}
*/


