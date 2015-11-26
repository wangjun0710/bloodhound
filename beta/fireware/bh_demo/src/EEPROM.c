/********************************************************************
//EEPROM���ݶ�д��غ�����ͨ��I2C��AT24C16���ж�д��֧�ֵ��ֽںͶ��ֽ�
//���Ի�����EW430 V5.30


********************************************************************/


#include "eeprom.h"
/********************************************************************
�������ƣ�Write_1Byte
��    �ܣ���EEPROM��д��1���ֽڵ�����
��    ����Wdata--д�������
          Address--���ݵ�д���ַ
����ֵ  ��д������1--�ɹ���0--ʧ��
********************************************************************/
uchar Write_1Byte(uchar wdata,uchar Address)
{
	start();
	write1byte(EEPROMAddress);
	if(check()) 
        write1byte(Address);
	else 
        return 0;
	if(check()) 
        write1byte(wdata);
	else 
        return 0;
	if(check())	    stop();
	else            return 0;
    
        delay_ms(10);                           //�ȴ�EEPROM����ڲ�д��
	return 1; 
}

/********************************************************************
�������ƣ�Write_NByte
��    �ܣ���EEPROM��д��N���ֽڵ�����
��    ����outbuf--ָ��д�����ݴ���׵�ַ��ָ��
          n--���ݸ���������ܳ���8����ҳ��ַ
             ��������󳤶�
          Address--����д����׵�ַ
����ֵ  ��д������1--�ɹ���0--ʧ��
********************************************************************/
uchar Write_NByte(uchar * outbuf,uchar n,uchar Address)
{
	uchar  flag; 
    
	start();
	write1byte(EEPROMAddress);             //д��������ַ
	if(check() == 1) 
        write1byte(Address);               //д�������ֵ�ַ
	else 
        return 0;
	if(check()) 
        flag=writeNbyte(outbuf,n);
	else 
        return 0;
        delay_ms(10);                         //�ȴ�EEPROM����ڲ�д��
	if(flag)    return 1;
	else        return 0;

}

/********************************************************************
�������ƣ�Read_1Byte_currentaddress
��    �ܣ���EEPROM�ĵ�ǰ��ַ��ȡ1���ֽڵ�����
��    ������
����ֵ  ����ȡ������
********************************************************************/
uchar Read_1Byte_currentaddress(void)
{
	uchar temp;
    
	start();
	write1byte((EEPROMAddress|0x01));
	if(check())	
        temp = read1byte();
	else 
        return 0;
	mnack();
	stop();
	return temp;
}

/********************************************************************
�������ƣ�Read_NByte_currentaddress
��    �ܣ���EEPROM�ĵ�ǰ��ַ��ȡN���ֽڵ�����
��    ����readbuf--ָ�򱣴����ݵ�ַ��ָ��
          n--��ȡ���ݵĸ���
����ֵ  ����ȡ�����1--�ɹ���0--ʧ��
********************************************************************/
uchar Read_NByte_currentaddress(uchar * readbuf,uchar n)
{   
        start();
	write1byte((EEPROMAddress|0x01));
	if(check()) 
        readNbyte(readbuf,n);
	else 
        return 0;
    
	return  1;
}

/********************************************************************
�������ƣ�Read_1Byte_Randomaddress
��    �ܣ���EEPROM��ָ����ַ��ȡ1���ֽڵ�����
��    ����Address--���ݶ�ȡ�ĵ�ַ
����ֵ  ����ȡ������
********************************************************************/
uchar Read_1Byte_Randomaddress(uchar Address)
{
	uchar temp;
    
	start();
	write1byte(EEPROMAddress);
	if(check())	
        write1byte(Address);
	else 
        return 0;
	if(check()) 
	{
	  start();
	  write1byte((EEPROMAddress|0x01));
	}
	else    
        return 0;
	if(check())	
        temp = read1byte();
	else 
        return 0;

	mnack();
	stop();
	return temp;
}

/********************************************************************
�������ƣ�Read_NByte_Randomaddress
��    �ܣ���EEPROM��ָ����ַ��ȡN���ֽڵ�����
��    ����readbuf--ָ�򱣴����ݵ�ַ��ָ��
          n--��ȡ���ݵĸ���
          Address--���ݶ�ȡ���׵�ַ
����ֵ  ����ȡ�����1--�ɹ���0--ʧ��
********************************************************************/
uchar Read_NByte_Randomaddress(uchar * readbuf,uchar n,uchar Address)
{
	start();
	write1byte(EEPROMAddress);
	if(check())	
        write1byte(Address);
	else 
        return 0;
	if(check()) 
	{
	  start();
	  write1byte(EEPROMAddress|0x01);
	}
	else 
        return 0;
	if(check()) 
        readNbyte(readbuf,n);
	else 
        return 0;
	
	return 1;	
}

unsigned char eeprom_test(void)
{
  uchar WriteBuffer[] = {1,2,3,4};     //׼��д��EEPROM�����ݣ���4���ֽ�
  uchar ReadBuffer[4] = {0,0,0,0};     //���ݻ���������Ŷ�ȡ����������  
   unsigned char state = 1;
   if(Write_1Byte(0x11,0x00))          //�ڵ�ַ0x00��д��0x11
      _NOP();  
   else 
     state = 0;
  _NOP();                           //����з������������ڴ����öϵ�۲�flag
  
  if(Read_1Byte_Randomaddress(0x00) == 0x11)
      _NOP();  
   else 
     state = 0;
  _NOP();                             //��ȡһ���ֽ�
  
  if(Write_NByte(WriteBuffer,4,0x08)) //�ӵ�1ҳ�ĵ�0���ֽ�������д4������
  {
       _NOP();  
  }
  else
  {
     state = 0;
  }
  
  if(Read_NByte_Randomaddress(ReadBuffer,4,0x08))//�ӵ�1ҳ�ĵ�0���ֽ���������4������
  {
       _NOP();  
  }
  else
  {
     state = 0;
  }
  
  return state;
  
}
