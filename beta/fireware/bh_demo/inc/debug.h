
#include    <msp430x14x.h>
#include "config.h"
#include "gm8125.h"


#define DEBUG_PORT_NUM 3
#define DeBug_Buf_Size 500

void DeBug_Send_Byte(uchar data); 
void DeBug_Print_Str(uchar *s);
void DeBug_Print_HEX(unsigned char *scr,unsigned char num);
void DeBug_Print_NUM(unsigned int Capacity);
void DeBug_Put_in_Buf(unsigned char *s);
void DeBug_Put_out_Buf(void);

extern unsigned char DeBug_Info_Buf[50];
extern unsigned char DeBug_Print_Buf[DeBug_Buf_Size];