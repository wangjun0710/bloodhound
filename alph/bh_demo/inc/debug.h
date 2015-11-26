
#include    <msp430x14x.h>
#include "config.h"
#include "gm8125.h"

extern unsigned char DeBug_Info_Buf[50];
#define DEBUG_PORT_NUM 3
void DeBug_Send_Byte(uchar data); 
void DeBug_Print_Str(uchar *s);
void DeBug_Print_HEX(unsigned char *scr,unsigned char num);