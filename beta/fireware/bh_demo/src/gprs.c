

#include "gprs.h"

unsigned char GsmRcv[GPRS_Recieve_Buf] = {0};
unsigned char GsmRcvCnt = 0;
extern unsigned char ConnectFlag;
extern unsigned char LogFlag;

unsigned char hand(unsigned char *a)
{
    if(strstr(GsmRcv,a)!= NULL)
        return 1;
	else
	return 0;
}
void clr_Buf(void)
{
    unsigned char k;
	for(k=0;k<GPRS_Recieve_Buf;k++)
	{
		GsmRcv[k]=0;
	}
    GsmRcvCnt=0;
}
void GPRS_Send_Byte(unsigned char data)
{
  
   GM8125_Send_Byte_Mmoddle(GPRS_PORT_NUM,data);
}

void GPRS_Print_Str(unsigned char *s)
{
    
    GM8125_Print_Str_Mmoddle(GPRS_PORT_NUM,s);
}
void gprs_band_rate(void)
{
      GPRS_Print_Str("AT\r\n");//设置sim300波特率
      delay_ms(500);//延时1秒
      while(!hand("OK"))
      {
        GPRS_Print_Str("AT\r\n");//设置sim300波特率
        delay_ms(500);
      }
      clr_Buf();
      
}
void gprs_check_china_mobile(void)
{
  
      GPRS_Print_Str("ATE0\r\n");
      delay_ms(500);
      while(!hand("CHINA"))//检测是否收到 CHINA MOBILE 服务商信息。如果收到证明是连接上网络了
      {
          GPRS_Print_Str("AT+COPS?\r\n");

          delay_ms(500);
      }
      clr_Buf();
}
unsigned char gprs_connect_server(void)
{
    
    GPRS_Print_Str("AT+CIPCLOSE\r\n");
    delay_ms(1000);
    clr_Buf();
    GPRS_Print_Str("AT+CIPSHUT\r\n");	
    delay_ms(1000);
    clr_Buf();
    GPRS_Print_Str("AT+CLPORT=\"TCP\",\"2000\"\r\n");//发送指令指定本地端口
    delay_ms(2000);
    clr_Buf();

    //GPRS_Print_Str("AT+CIPSTART=\"TCP\",\"61.155.65.45\",\"16000\"\r\n");//测试用服务器 建立服务器的IP，服务器端口号8086
   // GPRS_Print_Str("AT+CIPSTART=\"TCP\",\"112.74.14.169\",\"16000\"\r\n");//测试用服务器 建立服务器的IP，服务器端口号8086
    GPRS_Print_Str("AT+CIPSTART=\"TCP\",\"115.28.181.238\",\"16002\"\r\n");//陈总处的服务器 建立服务器的IP，服务器端口号8086
    delay_ms(5000);   //一定要等足够长时间
    if(hand("CONNECT OK") )//检测是否收到 CONNECT OK ,如果这连接成功
    {
      clr_Buf();
      return 1;
    }
    else
    {
      clr_Buf();
      return 0;
    }
    
}
void gprs_buf_review(unsigned char data)
{
  
/*
             if (GsmRcvCnt >= (GPRS_Recieve_Buf-1))
             {
                  unsigned char k;
                  for(k=0;k<(GPRS_Recieve_Buf-1);k++)
                  {
                      GsmRcv[k]=GsmRcv[k+1];
                  }
                  GsmRcv[GPRS_Recieve_Buf-1] = data;
                  GsmRcvCnt = GPRS_Recieve_Buf-1;
             }
             else
             {
               */
                   GsmRcv[GsmRcvCnt] = data;
                   GsmRcvCnt++;
           //  }
             
      //收到了完整的AT指令，完整的AT指令是以0x0a 0x0d结尾的。故作此判断，在接收的过程中是否收到0x0a 0x0d
      if (GsmRcvCnt >= 2)
      {
         if( (GsmRcv[GsmRcvCnt-2] == 0x0d) && (GsmRcv[GsmRcvCnt-1] == 0x0a) )
         {
            if((hand("CLOSED")) || (hand("ERROR")))
            {
              ConnectFlag=0;
              LogFlag=0;
              clr_Buf();
#if DEBUG
          DeBug_Print_Str("GPRS recieved the closed or error command,CPU will retry logon\r\n");
#endif
            }
         }
      }
      
    if (GsmRcvCnt >= GPRS_Recieve_Buf)
   {
        clr_Buf();
   }
      
}