/********************************************************************
//EEPROM操作相关函数声明

********************************************************************/
#include    <msp430x14x.h>
#include "config.h"
#include "i2c.h"
  
#define EEPROMAddress 0xa0                        //AT24C16的设备地址

unsigned char Write_1Byte(unsigned char wdata,unsigned char dataaddress);
unsigned char Write_NByte(unsigned char * outbuf,unsigned char n,unsigned char dataaddress);
unsigned char Read_1Byte_currentaddress(void);
unsigned char Read_NByte_currentaddress(unsigned char * readbuf,unsigned char n);
unsigned char Read_1Byte_Randomaddress(unsigned char dataaddress);
unsigned char Read_NByte_Randomaddress(unsigned char * readbuf,unsigned char n,unsigned char dataaddress);
unsigned char eeprom_test(void);