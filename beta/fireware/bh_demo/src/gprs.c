

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
      GPRS_Print_Str("AT\r\n");//����sim300������
      delay_ms(500);//��ʱ1��
      while(!hand("OK"))
      {
        GPRS_Print_Str("AT\r\n");//����sim300������
        delay_ms(500);
      }
      clr_Buf();
      
}
void gprs_check_china_mobile(void)
{
  
      GPRS_Print_Str("ATE0\r\n");
      delay_ms(500);
      while(!hand("CHINA"))//����Ƿ��յ� CHINA MOBILE ��������Ϣ������յ�֤����������������
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
    GPRS_Print_Str("AT+CLPORT=\"TCP\",\"2000\"\r\n");//����ָ��ָ�����ض˿�
    delay_ms(2000);
    clr_Buf();

    //GPRS_Print_Str("AT+CIPSTART=\"TCP\",\"61.155.65.45\",\"16000\"\r\n");//�����÷����� ������������IP���������˿ں�8086
   // GPRS_Print_Str("AT+CIPSTART=\"TCP\",\"112.74.14.169\",\"16000\"\r\n");//�����÷����� ������������IP���������˿ں�8086
    GPRS_Print_Str("AT+CIPSTART=\"TCP\",\"115.28.181.238\",\"16002\"\r\n");//���ܴ��ķ����� ������������IP���������˿ں�8086
    delay_ms(5000);   //һ��Ҫ���㹻��ʱ��
    if(hand("CONNECT OK") )//����Ƿ��յ� CONNECT OK ,��������ӳɹ�
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
             
      //�յ���������ATָ�������ATָ������0x0a 0x0d��β�ġ��������жϣ��ڽ��յĹ������Ƿ��յ�0x0a 0x0d
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