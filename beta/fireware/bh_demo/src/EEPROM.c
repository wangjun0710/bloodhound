/********************************************************************
//EEPROM数据读写相关函数，通过I2C对AT24C16进行读写，支持单字节和多字节
//调试环境：EW430 V5.30


********************************************************************/


#include "eeprom.h"
/********************************************************************
函数名称：Write_1Byte
功    能：向EEPROM中写入1个字节的数据
参    数：Wdata--写入的数据
          Address--数据的写入地址
返回值  ：写入结果：1--成功，0--失败
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
    
        delay_ms(10);                           //等待EEPROM完成内部写入
	return 1; 
}

/********************************************************************
函数名称：Write_NByte
功    能：向EEPROM中写入N个字节的数据
参    数：outbuf--指向写入数据存放首地址的指针
          n--数据个数，最大不能超过8，由页地址
             决定其最大长度
          Address--数据写入的首地址
返回值  ：写入结果：1--成功，0--失败
********************************************************************/
uchar Write_NByte(uchar * outbuf,uchar n,uchar Address)
{
	uchar  flag; 
    
	start();
	write1byte(EEPROMAddress);             //写入器件地址
	if(check() == 1) 
        write1byte(Address);               //写入数据字地址
	else 
        return 0;
	if(check()) 
        flag=writeNbyte(outbuf,n);
	else 
        return 0;
        delay_ms(10);                         //等待EEPROM完成内部写入
	if(flag)    return 1;
	else        return 0;

}

/********************************************************************
函数名称：Read_1Byte_currentaddress
功    能：从EEPROM的当前地址读取1个字节的数据
参    数：无
返回值  ：读取的数据
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
函数名称：Read_NByte_currentaddress
功    能：从EEPROM的当前地址读取N个字节的数据
参    数：readbuf--指向保存数据地址的指针
          n--读取数据的个数
返回值  ：读取结果：1--成功，0--失败
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
函数名称：Read_1Byte_Randomaddress
功    能：从EEPROM的指定地址读取1个字节的数据
参    数：Address--数据读取的地址
返回值  ：读取的数据
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
函数名称：Read_NByte_Randomaddress
功    能：从EEPROM的指定地址读取N个字节的数据
参    数：readbuf--指向保存数据地址的指针
          n--读取数据的个数
          Address--数据读取的首地址
返回值  ：读取结果：1--成功，0--失败
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
  uchar WriteBuffer[] = {1,2,3,4};     //准备写入EEPROM的数据，共4个字节
  uchar ReadBuffer[4] = {0,0,0,0};     //数据缓冲区，存放读取出来的数据  
   unsigned char state = 1;
   if(Write_1Byte(0x11,0x00))          //在地址0x00处写入0x11
      _NOP();  
   else 
     state = 0;
  _NOP();                           //如果有仿真器，可以在此设置断点观察flag
  
  if(Read_1Byte_Randomaddress(0x00) == 0x11)
      _NOP();  
   else 
     state = 0;
  _NOP();                             //读取一个字节
  
  if(Write_NByte(WriteBuffer,4,0x08)) //从第1页的第0个字节起连续写4个数据
  {
       _NOP();  
  }
  else
  {
     state = 0;
  }
  
  if(Read_NByte_Randomaddress(ReadBuffer,4,0x08))//从第1页的第0个字节起连续读4个数据
  {
       _NOP();  
  }
  else
  {
     state = 0;
  }
  
  return state;
  
}
