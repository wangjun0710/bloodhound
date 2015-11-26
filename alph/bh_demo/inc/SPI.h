//**********************************************************************
//SD卡读写中SPI接口相关函数，主要是SPI口初始化以及字节数据传输函数
//调试环境：EW430 V5.30

//**********************************************************************
#include <msp430x14x.h>
#define uchar unsigned char
#define uint  unsigned int
#define ulong unsigned long


//**********************************************************************
//              函数声明
//**********************************************************************

void   spiInit(void);
uchar  spiSendByte(uchar data);
uint spiTransferWord(uint data);
