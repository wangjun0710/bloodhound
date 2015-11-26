/********************************************************************
//MSP430F149采集环境变量
//通过GPRS模块传送到服务器上
//预留继电器控制接口
//调试环境：EW430 V5.30
//作者：DH
//时间：2015.2.25
********************************************************************/

#include "stdlib.h"
#include <string.h>
#include "Config.h"
#include "bsp.h"
#include "gm8125.h"
//#include "i2c.h"
//#include "eeprom.h"
//#include "adxl345.h"
//#include "SPI.h"
//#include "mmc.h"
//#include "FAT.h"
#include "gprs.h"
 


#if DEBUG
#include "debug.h"
#endif

  
uchar Flag = 0;                      //标志位，操作是否成功
uchar Sec = 0;
unsigned char recievedata[6];

//SD operate
unsigned long cardSize = 0;


uchar send_ad8_flag = 0;
uchar send_io_flag = 0;
unsigned char settemp = 0xff;

//xieyi
#define Version1 1
#define Version2 0
unsigned char CMD[Command_Buf][Num_Of_Package]={0};
unsigned char CMD_num=0;
unsigned char Do_num=0;
unsigned char InputState = 0;          //输入状态查询状态
unsigned char InputTmp = 0;            //输入状态查询临时变量
unsigned char SendFlag = 0;           //发送数据允许标志位
unsigned char heart = 0;
unsigned char ConnectFlag = 0;
unsigned char LogFlag = 0;
unsigned char HeartRcv = 0;
unsigned char HeartCnt = 0;
unsigned char InteruptFlag = 0;
unsigned int countplus = 0;
unsigned int countplustmp = 0;
unsigned char counttemp[2] = {0};



unsigned char HeartBeat[Num_Of_Package]={0x60,0x00,0x02,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0xff};//heartbeat;
unsigned char LogOn [Num_Of_Package]={0x60,0x00,0x02,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0xff};//LogOn the tcp server;
unsigned char DataOut[Num_Of_Package]={0x60,0x00,0x02,0x00,0x00,0x0a,0x00,0x00,0x00,0x00,0xff};//传输检测数据
/****************包头1*设   备   号3**子从1*Type1**D   A     T     A4**end1****************/
/*Type:
  0x01---'空气温度
  0x02---'空气湿度
  0x03---'土壤温度
  0x04---'土壤湿度
  0x05---'光照强度
  0x06---'CO2浓度
  0x08---'查询输入口状态
  0x0a---'脉冲计数；
  0x10---'电量提示
  0x20---'请求连接
  0x30---'连接成功
  0x40---'心跳
  0x50---'复位
  0x60---'软件版本
*/
typedef struct 
{
  float max_data;
  unsigned char wan,qian,bai,shi,ge;
  unsigned char flag;
}RecordData;
RecordData X_data, Y_data, Z_data;

void WorkStateReverse(void)
{
     P2OUT ^= BIT1;
}



/*
void conversion(RecordData data,uint temp_data)  
{  
    data.wan=temp_data/10000+0x30 ;
    temp_data=temp_data%10000;   //取余运算
    data.qian=temp_data/1000+0x30 ;
    temp_data=temp_data%1000;    //取余运算
    data.bai=temp_data/100+0x30   ;
    temp_data=temp_data%100;     //取余运算
    data.shi=temp_data/10+0x30    ;
    temp_data=temp_data%10;      //取余运算
    data.ge=temp_data+0x30 ; 	
}
*/
/*
void get_xyz(RecordData *Xdata,RecordData *Ydata,RecordData *Zdata)
{
    float x_tmp,y_tmp,z_tmp;
    unsigned char xflag_tmp,yflag_tmp,zflag_tmp;
    
     dis_data=(BUF[1]<<8)+BUF[0];  //合成数据   
     if(dis_data<0)
     {
	 dis_data = -dis_data;
         xflag_tmp = 1;
     }
     else
     {
         xflag_tmp = 0;
     }
    x_tmp=(float)dis_data*3.9;  //计算数据和显示,查考ADXL345快速入门第4页

    
    dis_data=(BUF[3]<<8)+BUF[2];  //合成数据   
     if(dis_data<0)
     {
	 dis_data=-dis_data;
         yflag_tmp = 1;
      }
     else
     {
         yflag_tmp = 0;
     }
     
    y_tmp=(float)dis_data*3.9;  //计算数据和显示,查考ADXL345快速入门第4页

    dis_data=(BUF[5]<<8)+BUF[4];  //合成数据   
     if(dis_data<0)
     {
	 dis_data=-dis_data;
         zflag_tmp = 1;
      }
     else
     {
         zflag_tmp = 0;
     }
     
    z_tmp=(float)dis_data*3.9;  //计算数据和显示,查考ADXL345快速入门第4页


    X_data.max_data = x_tmp;
    Y_data.max_data = y_tmp;
    Z_data.max_data = z_tmp;
    X_data.flag = xflag_tmp;
    Y_data.flag = yflag_tmp;
    Z_data.flag = zflag_tmp;
    
    
    conversion(Xdata,x_tmp);
    delay_ms(2);
    conversion(Ydata,y_tmp);
    delay_ms(2);
    conversion(Zdata,z_tmp);
    delay_ms(2);
*/
    /*
}
void ProcessingData(void)
{
  

  
}
void Dis_ADXL345_Data(RecordData *data)
{
   unsigned char dis_tmp[7];
   
   dis_tmp[0] = (*data).qian;
    dis_tmp[1] = '.';
     dis_tmp[2] = (*data).bai;
   dis_tmp[3] = (*data).shi;
   dis_tmp[4] = '\r';
   dis_tmp[5] = '\n';
   dis_tmp[6] = '\0';
   
   DeBug_Print_Str(dis_tmp);	
  
}
*/
/*
unsigned char ADXL345_Work(RecordData *Xdata,RecordData *Ydata,RecordData *Zdata)
{
      devid=Single_Read_ADXL345(0X00);	//读出的数据为0XE5,表示正确
      if(devid!=0XE5)
      {		
         
          return FALSE;
      }
      else
      {			
             
                err = 0;
                Multiple_read_ADXL345();       	//连续读出数据，存储在BUF中  
                if (err == 1)
                {
                     
                    return FALSE;
                }
                else
                {
                    get_xyz(Xdata,Ydata,Zdata);                      //原5次采样取最大的数值
                    Dis_ADXL345_Data(Xdata);
                    return TRUE;
                }
            
      }  
  
}
*/
//***********************************************************************
//            SD卡容量处理及显示函数
//***********************************************************************
/*
void SDsizeDisplay(ulong Capacity)
{
  static uchar SizeQ,SizeB,SizeS,SizeG;
  ulong Size;
  unsigned char temp;
  DeBug_Print_Str("SD Size:");          //显示SD卡容量
  Size=Capacity/1024/1024;
  SizeQ=Size/1000;
  SizeB=(Size%1000)/100;
  SizeS=(Size%1000)%100/10;
  SizeG=(Size%1000)%100%10;
   temp = SizeQ+'0';
  DeBug_Send_Byte(temp); 
   temp = SizeB+'0';
  DeBug_Send_Byte(temp); 
   temp = SizeS+'0';
    DeBug_Send_Byte(temp); 
   temp = SizeG+'0';
     DeBug_Send_Byte(temp);  
   DeBug_Print_Str("MB\r\n"); 
}

void SD_File_Create(void)
{

  ReadBPB();

  FormatCard();

  FATSystemType();                          //MMC/SD卡文件系统类型判断，FAT32_EN==1则为FAT32文件系统
  if(FAT32_EN==1) ReadBPB32();
  FileCountSearch();                        //搜索文件夹及文件数量

  DelFile("DATAFILETXT");   

  //FileCountSearch();                      //搜索文件夹及文件数量
  if(FAT32_EN==0) 
  {
    //FAT文件系统下在根目录下创建文件或文件夹，0x10为文件夹，0x20为文件
    CreateFile("DATAFILETXT", 0x20); 
    DeBug_Print_Str("create success ..!\r\n"); 
    
  } 
  else
  {
     FAT32CreateFile("DATAFILETXT");
     DeBug_Print_Str("create fail ..!\r\n"); 
  }
}
*/
void Logon() //发送登录包给服务器
{
  unsigned char m;
  GPRS_Print_Str("AT+CIPSEND\r\n");
  delay_ms(1000);
  for(m=0;m<Num_Of_Package;m++)
   GPRS_Send_Byte(LogOn[m]);
  delay_ms(1000);
  GPRS_Send_Byte(0x1a);//以0x1a结束
  delay_ms(1000);//延时20秒	
#if DEBUG
          DeBug_Print_Str("GPRS has been sending logon meassage the server:\r\n");
        DeBug_Print_HEX(LogOn,Num_Of_Package);
#endif

}
void Heart()  //发送心跳包给服务器
{
  unsigned char m;
  heart=0;
  if(!HeartRcv)   //若连续3次未收到心跳则跳出重新登陆
  {
     HeartCnt++;
     if(HeartCnt >= 3)
     {
       HeartCnt=0;
       LogFlag=0;
     }
  }
  else
  {
     HeartRcv=0;
    //LogFlag=1;
  }
  GPRS_Print_Str("AT+CIPSEND\r\n");
  delay_ms(1000);
  for(m=0;m<Num_Of_Package;m++)
      GPRS_Send_Byte(HeartBeat[m]);
  delay_ms(1000);
  GPRS_Send_Byte(0x1a);//以0x1a结束
  delay_ms(1000);;//延时2秒	
#if DEBUG
  DeBug_Print_Str("GPRS has been sending heart meassage to the server:\r\n"); 
  DeBug_Print_HEX(HeartBeat,Num_Of_Package);
#endif
}
void SendData()
{
  unsigned char m;
  if(SendFlag)
  {
    SendFlag=0;
    GPRS_Print_Str("AT+CIPSEND\r\n");
    delay_ms(1000);
    for(m=0;m<Num_Of_Package;m++)
      GPRS_Send_Byte(DataOut[m]);
    delay_ms(1000);
    GPRS_Send_Byte(0x1a);//以0x1a结束
                            
    delay_ms(1000);;//延时20秒	
#if DEBUG
    DeBug_Print_Str("GPRS is sending  meassage to the server:\r\n");
    DeBug_Print_HEX(DataOut,Num_Of_Package);
#endif
    //clr_Buf();
    DataOut[6]=0;
    DataOut[7]=0;
    DataOut[8]=0;
    DataOut[9]=0;
  }
}
void DoCommand()
{
  unsigned char i,j,command;
  
  if(CMD[0][1]==0x00&&CMD[0][2]==0x01&&CMD[0][4]==0x01) //1\2为设备号,4为主机命令标示
  {
    command=CMD[0][5];
  }
  else
  {
    command=0xFF;   //wrong command
  }
  DataOut[5]=command;
  
  switch(command)
  {
    case 0x01:
      DataOut[8]=0;
      DataOut[9]=0;
      SendFlag=1;
      break;
    case 0x02:
      DataOut[8]=0;
      DataOut[9]=0;
      SendFlag=1;
      break;
    case 0x05:
      
      break;
    case 0x06:
      
      break;
    case 0x07:
      
      break;
    case 0x08:
      /*
      InputState = P2IN & 0xFF;
      delay_ms(10);
      InputTmp = P2IN & 0xFF;
      if (InputTmp == InputState)
      {
      DataOut[8]=0x00;
      DataOut[9]=InputState;  //bit7-》bit0，电平为高时，相应位读取为0；
      }
      else
      {
      DataOut[8]=0xff;
      DataOut[9]=0xff;    //电平波动，读取电平两次不稳
      }
      InputState = 0;
      InputTmp = 0;
      SendFlag=1;
      */
      break;
    case 0x09:
      //P1OUT=CMD[0][9];
      delay_ms(10);
      break;
    case 0x0a:
     
      /*
     countplustmp = countplus; 
     counttemp[0] = (countplustmp && 0xff00)>>8;
     countplustmp = countplus; 
     counttemp[1] = (countplustmp && 0x00ff);
      DataOut[8]=counttemp[0];
      DataOut[9]=counttemp[1];
      SendFlag=1;
      */
      break;
    case 0x40:
      HeartRcv=1;
      break;
    case 0x30:
      if(!LogFlag)
      {
        LogFlag=1;
        heart=0;
        SendFlag=0;
      }
      break;
    case 0x50:
      
      _DINT();
      GPRS_Print_Str("AT+CIPCLOSE=1\r\n");
      delay_ms(1000);
      GPRS_Print_Str("AT+CIPSHUT\r\n");
      delay_ms(1000);
      _EINT();
      _NOP();
      ((void  (*)())RESET_VECTOR)();
      
      break;
    case 0x60:
    
      DataOut[8]=Version1;
      DataOut[9]=Version2;
      SendFlag=1;
      break;

    case 0xff:
      SendFlag=1;
      break;
    default:
      break;
  }
  //设置buf轮换
  for(j=0;j<CMD_num;j++)
  {
     for(i=0;i<Num_Of_Package;i++)
     {
       CMD[j][i]=CMD[j+1][i];
     }
  }
  for (i=0; i<Num_Of_Package; i++)
  {
    CMD[CMD_num][i]=0;
  }
  
  command=0;
}
void DoRcv(void)
{

  unsigned char i,j;

  if(GsmRcvCnt!=0)
  {

    for (i=0;i<(GsmRcvCnt-Num_Of_Package+1);i++)  //在收到的所有数据中，提取服务器指令，最多提取CMD_num条
    {
       if ((GsmRcv[i] == 0x60)&& (GsmRcv[i+Num_Of_Package-1] == 0xff))
       {
         for(j=0;j<Num_Of_Package;j++)
         {
           CMD[CMD_num][j]=GsmRcv[j+i];
         }
         DeBug_Print_Str("GPRS revieved a new comand form server:\r\n");
         DeBug_Print_HEX(CMD[CMD_num],Num_Of_Package);
         CMD_num++;

       }
    }
    clr_Buf();
  }

  if(CMD_num!=0)
  {
    DoCommand();
    CMD_num--;
  }
}
unsigned int AD_DATA[8] = {0};
//*********************************************************************
//读取AD转换值
//**********************************************************************/
void ad_read(void)
{
  ADC12CTL0 |= ENC + ADC12SC;           //开启转换
  ADC12CTL0 &= ~ADC12SC;          //清零
  while((ADC12CTL0&0x01)==1);           //如果ADC忙，则等待，否则读取ADC转换数值
  //Flag = 1 ;
  AD_DATA[0] = ADC12MEM0;                     //读取ADC转换值
  AD_DATA[1] = ADC12MEM1;
  AD_DATA[2] = ADC12MEM2;
   AD_DATA[3] = ADC12MEM3;
    AD_DATA[4] = ADC12MEM4;
     AD_DATA[5] = ADC12MEM5;
      AD_DATA[6] = ADC12MEM6;
  AD_DATA[7] = ADC12MEM7;
}
//***********************************************************************
//			主程序
//***********************************************************************
void main(void)
{
  unsigned char qq;
 // unsigned char adxl_state;
  //unsigned char mmc_state;
  //  unsigned char eeprom_state;
   unsigned char i;
   unsigned int temp;
  for (qq=0;qq<0xf0;qq++)
  {
  _NOP();
  }
  WDT_Init();                                   //看门狗设置
 //  _DINT();
  Clock_Init();                                 //系统时钟设置
  delay_ms(100);                                //延时100ms
  Port_Init();                                  //系统初始化，设置IO口属性
  ADC_Init();
  GM8125_Init(0);
  delay_ms(100); 
  DeBug_Print_Str("System Start!\r\n");  
  TimerA_init();
 
  /*
  mmc_state = mmc_test();
  if ( mmc_state == 1)
  {
   DeBug_Print_Str("MMC Verify Success!\r\n");  
   cardSize =  MMC_ReadCardSize();
   SDsizeDisplay(cardSize); 
   SD_File_Create();
  }
  else
  {
      DeBug_Print_Str("MMC Verify Failure!\r\n");  
  }
  */
    //延时100ms
 _EINT(); 
  /*
    eeprom_state = eeprom_test();
   if ( eeprom_state == 1)
  {
     DeBug_Print_Str("EEPROM Verify Success!\r\n");  

  }
  else
  {
      DeBug_Print_Str("EEPROM Verify Failure!\r\n");  
  }
  */

     /*
      
      Init_ADXL345();                 	//初始化ADXL345
      devid=Single_Read_ADXL345(0X00);	//读出的数据为0XE5,表示正确
      if(devid!=0XE5)
      {				
          DeBug_Print_Str("No ADXL345\r\n");	
      }
      else
      {			
      	  DeBug_Print_Str("ADXL345 init Success!\r\n");	    
      } 
      */
#if 0 
  while(1)
  {
   ad_read();
   delay_ms(10);
   DeBug_Print_NUM(AD_DATA[0]);
   delay_ms(10);
   DeBug_Print_NUM(AD_DATA[1]);
   delay_ms(10);
   DeBug_Print_NUM(AD_DATA[2]);
   delay_ms(10);
   DeBug_Print_NUM(AD_DATA[3]);
   delay_ms(10);
   DeBug_Print_NUM(AD_DATA[4]);
   delay_ms(10);
   DeBug_Print_NUM(AD_DATA[5]);
   delay_ms(10);
   DeBug_Print_NUM(AD_DATA[6]);
   delay_ms(10);
   DeBug_Print_NUM(AD_DATA[7]);
   delay_ms(1000);
  }
#endif
  while(1)
  {
    /*
     adxl_state = ADXL345_Work(&X_data,&Y_data,&Z_data);
     if (adxl_state == FALSE)
     {
         DeBug_Print_Str("Adxl345 no work!\r\n");	
     }
     
     delay_ms(5);
     ProcessingData();
     delay_ms(5);
     */

    while (0 == ConnectFlag)
    {
         DeBug_Put_in_Buf("debug test1");///
         gprs_band_rate();
         DeBug_Print_Str("GPRS BandRate Set OK!\r\n"); 
         delay_ms(1000);
         gprs_check_china_mobile();
         DeBug_Print_Str("GPRS Check China Mobile OK!\r\n"); 
         delay_ms(1000);
         ConnectFlag = gprs_connect_server();//while (connect ok)
         if (1 == ConnectFlag)
         {
           DeBug_Print_Str("GPRS Connect Server is OK!\r\n"); 
           LogFlag=0;   //登上服务器后将logon标志置零
         }
         else
         {
            DeBug_Print_Str("GPRS Connect Server Fail!\r\n"); 
         }
         DeBug_Put_in_Buf("debug test2");///
         DeBug_Put_out_Buf();
         delay_ms(500);  
    }
    
     while(1 == ConnectFlag)
    {
        DeBug_Put_in_Buf("debug test1");///
      ad_read();
      DoRcv();
      if((Sec%5==0)&&(!LogFlag))  //5s一次登录包，因为3s第一个登录包被回应时，单片机还未处理好，第二个包就发出去了
      {
        Logon();

      }
      if(heart&&LogFlag)
      {
        heart=0;
        Heart();

      }
        DeBug_Put_in_Buf("debug test2");///
      if(LogFlag)
      {
         //SendData();
          if (send_ad8_flag == 1)
         {   
            
            for (i=0; i<8; i++)
            {
              SendFlag = 1;
              if ((settemp & 0x01) > 0)
              {
                  temp = AD_DATA[i];
                  DataOut[5] =  0x0b;
                  DataOut[9]=(uchar)(temp & 0x00ff);
                  DataOut[8]=(uchar)((temp & 0xff00)>>8);
                  DataOut[7]= i;
                  SendData(); 
              }
               settemp = settemp >> 1;
            }
             settemp = 0xff;
             send_ad8_flag = 0;
         }
         if (send_io_flag == 1)
         {
              SendFlag = 1;
              
              countplustmp = countplus; 
              counttemp[0] = (countplustmp & 0xff00)>>8;
              countplustmp = countplus; 
              counttemp[1] = (countplustmp & 0x00ff);
              countplus = 0;
              DataOut[5]=0x0a;
              DataOut[8]=counttemp[0];
              DataOut[9]=counttemp[1];
              SendData(); 
              send_io_flag = 0;
         }
        
      }
      DeBug_Put_in_Buf("debug test3");///
      DeBug_Put_out_Buf();
      delay_ms(500);  
    }  
    
      //delay_ms(1000);  

  }

}


//*************************************************************************
//               处理来自串口1的接收中断
//*************************************************************************

#pragma vector=UART1RX_VECTOR

__interrupt void UART1_RX_ISR(void) 
{
  unsigned char data;

  data = RXBUF1;
  GM8125_Recieve_Byte_Mmoddle(data);
  

}
//********************************************************
//定时器A中断
//********************************************************
#pragma vector = TIMERA1_VECTOR
__interrupt void Timer_A(void)
{

  switch(TAIV)                                  //需要判断中断的类型
  {
    case 2:break;
    case 4:break;
    case 10:
      //count++;
     Sec++;
     WorkStateReverse();
      break;                       
  }
  if (Sec == 60)
  {
    /*
      countplustmp = countplus; 
      counttemp[0] = (countplustmp & 0xff00)>>8;
      countplustmp = countplus; 
      counttemp[1] = (countplustmp & 0x00ff);
      
      DataOut[5]=0x0a;
      DataOut[8]=counttemp[0];
      DataOut[9]=counttemp[1];
      
      SendFlag = 1;
      Sec = 0;
      countplus = 0;
    */
    send_ad8_flag = 1;
    send_io_flag = 1;
    Sec = 0;
  }

}

#pragma vector=PORT1_VECTOR
__interrupt void  PORT1_ISR(void)
{

    if(P1IFG & 0x01)  //P1^0 interrupt
  {
    P1IFG=0;

    countplus++;
  }

}