#include "adxl345.h"

 unsigned char BUF[8];                         //�������ݻ�����      	
 unsigned char ge,shi,bai,qian,wan;           //��ʾ����
 int  dis_data;                       //����
  unsigned char devid;
  unsigned char err;


void ADXL345_Start()
{
    
   start();
}
/**************************************
ֹͣ�ź�
**************************************/
void ADXL345_Stop()
{
   stop();
}

/**************************************
��IIC���߷���һ���ֽ�����
**************************************/
void ADXL345_SendByte(unsigned char dat)
{
   write1byte(dat);
}

/**************************************
��IIC���߽���һ���ֽ�����
**************************************/
unsigned char ADXL345_RecvByte()
{

    unsigned char dat = 0;
    dat = read1byte();
    return dat;
}

//******���ֽ�д��*******************************************

unsigned char Single_Write_ADXL345(unsigned char REG_Address,unsigned char REG_data)
{
    ADXL345_Start();                  //��ʼ�ź�
    ADXL345_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�
    
    if(check()) 
    ADXL345_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf22ҳ 
    else
    return 0; 
    
    if(check()) 
    ADXL345_SendByte(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf22ҳ 
    else
    return 0; 
    
    if(check()) 
    ADXL345_Stop();                   //����ֹͣ�ź�
    else
    return 0; 
    
    delay_ms(1);                           //�ȴ�EEPROM����ڲ�д��
    return 1; 
}

//********���ֽڶ�ȡ*****************************************
unsigned char Single_Read_ADXL345(unsigned char REG_Address)
{  unsigned char REG_data;
    ADXL345_Start();                          //��ʼ�ź�
    ADXL345_SendByte(SlaveAddress);           //�����豸��ַ+д�ź�
    if(check()) 
    {
    ADXL345_SendByte(REG_Address);            //���ʹ洢��Ԫ��ַ����0��ʼ
    }
    else
    {
      err = 1;
    return 0;
    }
    
    if(check()) 
    {
       ADXL345_Start();                          //��ʼ�ź�
    }
     else
    {
      err = 1;
      return 0;
    }
    
    ADXL345_SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
     if(check()) 
     {
       REG_data=ADXL345_RecvByte();              //�����Ĵ�������
     }
     else
    {
      err = 1;
    return 0;
    }
    
    mnack();  
	ADXL345_Stop();                           //ֹͣ�ź�
    return REG_data; 
}
//*********************************************************
//
//��������ADXL345�ڲ����ٶ����ݣ���ַ��Χ0x32~0x37
//
//*********************************************************
void Multiple_read_ADXL345(void)
{   unsigned char i;
    ADXL345_Start();                          //��ʼ�ź�
    ADXL345_SendByte(SlaveAddress);           //�����豸��ַ+д�ź�
    if(check()) 
    {
        ADXL345_SendByte(0x32);                   //���ʹ洢��Ԫ��ַ����0x32��ʼ	
    }
    else
    {
        err = 1;
    }
    if(check()) 
    {
        ADXL345_Start();                          //��ʼ�ź�
    }
    else
    {
      err = 1;
    }
    ADXL345_SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
    if(check()) 
    {
    for (i=0; i<6; i++)                      //������ȡ6����ַ���ݣ��洢��BUF
    {
        BUF[i] = ADXL345_RecvByte();          //BUF[0]�洢0x32��ַ�е�����
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
    ADXL345_Stop();                          //ֹͣ�ź�
    delay_us(100);
}
 

//*****************************************************************

//��ʼ��ADXL345��������Ҫ��ο�pdf�����޸�************************
void Init_ADXL345()
{
   Single_Write_ADXL345(0x31,0x0B);   //������Χ,����16g��13λģʽ
   Single_Write_ADXL345(0x2C,0x0E);   //�����趨Ϊ12.5 �ο�pdf13ҳ
   Single_Write_ADXL345(0x2D,0x08);   //ѡ���Դģʽ   �ο�pdf24ҳ
   Single_Write_ADXL345(0x2E,0x80);   //ʹ�� DATA_READY �ж�
   Single_Write_ADXL345(0x1E,0x00);   //X ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
   Single_Write_ADXL345(0x1F,0x00);   //Y ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
   Single_Write_ADXL345(0x20,0x00);   //Z ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
}
/*
void ADXL345_Adjust(char *xval,char *yval,char *zval)
{
   unsigned char i;
   unsigned char ofsx,ofsy,ofsz;
   
   for (i=0; i<10; i++)
   {
      ADXL345_Start();                          //��ʼ�ź�
      ADXL345_SendByte(SlaveAddress);           //�����豸��ַ+д�ź�
      cack();
      ADXL345_SendByte(0x32);                   //���ʹ洢��Ԫ��ַ����0x32��ʼ	
      cack();
      ADXL345_Start();                          //��ʼ�ź�
      ADXL345_SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
      cack();
      for (i=0; i<6; i++)                      //������ȡ6����ַ���ݣ��洢��BUF
      {
        BUF[i] = ADXL345_RecvByte();          //BUF[0]�洢0x32��ַ�е�����
        if (i == 5)
        {
          mnack();
        }
        else
        {
          mack();
       }
      }
    ADXL345_Stop();                          //ֹͣ�ź�
    delayus(100);
    
   }
   Single_Write_ADXL345(0x1E,0x00);   //X ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
   Single_Write_ADXL345(0x1F,0x00);   //Y ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
   Single_Write_ADXL345(0x20,0x00);   //Z ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
}
*/


