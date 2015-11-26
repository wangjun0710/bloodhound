
#include "debug.h"

unsigned char DeBug_Info_Buf[50];
void DeBug_Send_Byte(unsigned char data)
{
  
   GM8125_Send_Byte_Mmoddle(DEBUG_PORT_NUM,data);
}

void DeBug_Print_Str(unsigned char *s)
{
    
    GM8125_Print_Str_Mmoddle(DEBUG_PORT_NUM,s);
}
/*
scr为要输出的16进制（数字）字符串 
num为要输出的16进制数目
*/
void DeBug_Print_HEX(unsigned char *scr,unsigned char num)
{
    unsigned char i;
    unsigned char tmp_h,tmp_l;
    unsigned char tmp;
    i = 0;
    tmp = 0;
    tmp = num << 1;
    while (i<tmp)
    {
      tmp_h = ((*scr) >> 4);
      if (tmp_h>9)
      {
        tmp_h = tmp_h + 55;
      }
      else
      {
         tmp_h = tmp_h + '0';
      }
       tmp_l = ((*scr) & 0x0F);
       if (tmp_l>9)
      {
        tmp_l = tmp_l + 55;
      }
      else
      {
         tmp_l = tmp_l + '0';
      }
      DeBug_Info_Buf[i] = tmp_h;
      DeBug_Info_Buf[i+1] = tmp_l;
      i = i+2;
      scr++;
    }
    DeBug_Info_Buf[tmp] = '\r';
    DeBug_Info_Buf[tmp + 1] = '\n';
    DeBug_Info_Buf[tmp + 2] = '\0';
  
    DeBug_Print_Str(DeBug_Info_Buf);
}