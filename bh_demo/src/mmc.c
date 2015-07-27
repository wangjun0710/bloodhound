//******************************************************************************
//MMC/SD��������SPIģʽ��MMC/SD����������ʶ��MMC/SD������
//MMC/SD��������MSP430F149��SPI�ӿڣ���������ͼ��ο�DM430-Aԭ��ͼ

//******************************************************************************  

#ifndef _MMCLIB_C
#define _MMCLIB_C

#include "mmc.h"                            //SD������ͷ�ļ�����Ҫ����CSƬѡ�źţ�����Ӳ������
#include "MSP430x14x.H"                     //������ͷ�ļ�


/***********************************************************************
                    �������������
***********************************************************************/
char mmc_buffer[512] = { 0 };               // Buffer for mmc i/o for data and registers

/***********************************************************************
�������ܣ�MMC/SD����ʼ��
�������ݣ�1������CSƬѡ��MOSI����74��ʱ������
          2��SPI����10��FF
***********************************************************************/
uchar initMMC (void)
{
  int i;
  
  initSPI();
  CS_HIGH();
  for(i=0;i<=9;i++)
    spiSendByte(0xff);

  return (mmc_GoIdle());
}

/***********************************************************************
�������ܣ�MMC/SD������ģʽ����ʼ��
�������ݣ�1����ʼ��MMC/SD��������SPIģʽ������MMC����SD��������CMD0
          2���ж�MMC������SD��
          3��SD��ѭ��10�η���CMD55+ACMD41��MMC������CMD1
***********************************************************************/
char mmc_GoIdle()
{
  char response=0x01;
  unsigned char i;
  unsigned char sdcard=0x00;               //SD����־λ��Ϊ0��ʾSD����Ϊ1��ʾΪMMC��
  CS_LOW();                                //ѡ��MMC/SD����Ƭѡ��Ч����������������
  mmcSendCmd(MMC_GO_IDLE_STATE,0,0x95);    //����CMD0��MMC/SD������ΪSPIģʽ��0x95ΪCRCֵ
  //Now wait for READY RESPONSE
  if(mmcGetResponse()!=0x01)
    return MMC_INIT_ERROR;

  while(response==0x01)
  {
    CS_HIGH();
    spiSendByte(0xff);
    CS_LOW();
  if(sdcard==0x00)                                //��ʶ���Ƿ�ΪSD��������SD������CMD55+ACMD41
  {
   for(i=0;i<10;i++)                              //ѭ��10�Σ����ΪSD����һ��2�μ�����ȷ����0x00
   {
    mmcSendCmd(SD_APP_COND,0x00,0xff);            //����SD���ж��������ɹ�����0x00����ΪSD��
    while(response=mmcGetResponse()!=0x01);       //����ֵΪ0x01�����ʾ���ڿ���״̬���ٷ�ACMD41����
    
    mmcSendCmd(SD_APP_OP_COND,0x00,0xff);         //����ACMD41����
    if(response=mmcGetResponse()==0x00)           //����ֵΪ0x00�����ʼ���ɹ�������ʹ��MMC����CMD1������ʼ��
     {
      CS_HIGH();
      spiSendByte(0xff);
      return (MMC_SUCCESS);                       //���س�ʼ���ɹ�����
     }
   }
   sdcard=0x01;                                   //Ϊ1��ʾSD��������Ч����ΪMMC����׼�����淢��CMD1����
  }
    response=0x01;                                //׼������MMC������CMD1
    mmcSendCmd(MMC_SEND_OP_COND,0x00,0xff);       //����MMC��CMD1����ɹ��򷵻�0x00
    response=mmcGetResponse();                    //����ֵΪ0x00�����ʾMMC����ʼ���ɹ�
  }
  
  CS_HIGH();                                      //MMC/SD��Ƭѡ��Ч
  spiSendByte(0xff);
  return (MMC_SUCCESS);                           //���س�ʼ���ɹ�����
}

/***********************************************************************
�������ܣ�MMC/SD�������������Ӧֵ��ȡ
�������ݣ�1��SPIģʽ����Ӧֵ����1~8bytes���ο�SD���ֲ������Ӧֵ������
          2����Ӧֵ���λ����Ϊ0��������Ϊ�������
          3�������ӦֵΪFF�����ʾ������Ч��ʱ
***********************************************************************/
uchar mmcGetResponse(void)
{
  int i=0;
  char response;

  while(i<=64)
  {
    response=spiSendByte(0xff);
    if(response==0x00)break;
    if(response==0x01)break;
    i++;
  }
  return response;
}

/***********************************************************************
�������ܣ�MMC/SD�����Ͷ�д��������󷵻ص���Ӧֵ
�������ݣ�1��SPIģʽ����Ӧֵ����1~8bytes���ο�SD���ֲ������Ӧֵ������
          2����Ӧֵ���λ����Ϊ0��������Ϊ�������
          3����Ӧ֮ǰ������һֱΪ0xff
***********************************************************************/
uchar mmcGetXXResponse(const char resp)
{
  int i=0;
  char response;

  while(i<=1000)
  {
    response=spiSendByte(0xff);
    if(response==resp)break;
    i++;
  }
  return response;
}

/***********************************************************************
�������ܣ�MMC/SD����æ����
�������ݣ�1��SPIģʽ����Ӧֵ����1~8bytes���ο�SD���ֲ������Ӧֵ������
          2����Ӧֵ���λ����Ϊ0��������Ϊ�������
          3�������ӦֵΪFF�����ʾ������Ч��ʱ
          4���������ݿ�д���ʱ��д�����ݺ󶼻�����Ӧֵ�������˴������
***********************************************************************/
uchar mmcCheckBusy(void)
{

  int i=0;

  char response;
  char rvalue;
  while(i<=64)
  {
    response=spiSendByte(0xff);
    response &= 0x1f;
    switch(response)
    {
      case 0x05: rvalue=MMC_SUCCESS;break;      //��ʾMMC/SD����ȷ�ɹ���������
      case 0x0b: return(MMC_CRC_ERROR);         //��ʾCRCУ��������ݱ��ܾ�
      case 0x0d: return(MMC_WRITE_ERROR);       //��ʾMMC/SD��д�������ݱ��ܾ�
      default:
        rvalue = MMC_OTHER_ERROR;               //��������
        break;
    }
    if(rvalue==MMC_SUCCESS)break;
    i++;
  }
  i=0;
  do
  {
    response=spiSendByte(0xff);
    i++;
  }while(response==0);
  return response;
}

/***********************************************************************
�������ܣ�MMC/SD���������ݶ�ȡ
�������ݣ�1��������ȡ�ĵ�ַ�������Լ�����Ļ�����
		  2�����С�������ã���Ϊ512��������Ϊ��һ����׼������С
          3�����ݺ�����16λCRCֵ
***********************************************************************/
uchar mmcReadBlock(ulong address, uint count, uchar *pBuffer)
{
  unsigned long i = 0;
  uchar rvalue = MMC_RESPONSE_ERROR;

  if (mmcSetBlockLength (count) == MMC_SUCCESS)  //����MMC/SD���鳤��,ȫ�ֱ���BlockLength
  {

    CS_LOW ();

    mmcSendCmd (MMC_READ_SINGLE_BLOCK,address, 0xFF); //���Ͷ������������CMD17��ʵ�ʵ�ַ��������numSector*512
    if (mmcGetResponse() == 0x00)                         //����ֵΪ00����ʾ��������ɹ�����
    {
      if (mmcGetXXResponse(MMC_START_DATA_BLOCK_TOKEN) == MMC_START_DATA_BLOCK_TOKEN) //�������ݰ���ʼ��־��Ϊ0xfe
      {
        for (i = 0; i < count; i++)
          pBuffer[i] = spiSendByte(0xff);   // ���ն���������

        spiSendByte(0xff);                  //16λCRCֵ
        spiSendByte(0xff);                  //16λCRCֵ������������CRCֵ��������ԣ�����ʵ�����ݺ����16λCRCֵ
        rvalue = MMC_SUCCESS;               //���ճɹ������سɹ�����
      }
      else
      {
        rvalue = MMC_DATA_TOKEN_ERROR;      // ���û���յ����ݰ��Ŀ�ʼ��־���򷵻ش������
      }
    }
    else
    {
      rvalue = MMC_RESPONSE_ERROR;          //��������δ�ɹ����մ������
    }
  }
  else
  {
    rvalue = MMC_BLOCK_SET_ERROR;           //���ؿ��鳤������ʧ�ܴ������
  }
  CS_HIGH ();
  spiSendByte(0xff);
  return rvalue;                            //���ض�ȡ��������
}

/***********************************************************************
�������ܣ�MMC/SD����������д��
�������ݣ�1��������ȡ�ĵ�ַ�������Լ�����Ļ�����
		  2�����С�������ã���Ϊ512��������Ϊ��һ����׼������С
          3�����ݺ�����16λCRCֵ
***********************************************************************/
uchar mmcWriteBlock(ulong address, uint count, uchar *pBuffer)
{
  unsigned long i = 0;
  uchar rvalue = MMC_RESPONSE_ERROR;         

  if (mmcSetBlockLength (count) == MMC_SUCCESS)    //����MMC/SD��ĳ��ȣ�ȫ�ֱ���BlockLength
  {
    CS_LOW ();									   //����Ƭѡ�źţ��������ѡ����Ҫ������Ƭѡ����
    mmcSendCmd (MMC_WRITE_BLOCK,address, 0xFF);//���͵���д��������CMD24����ַΪ����numSector*512

    if (mmcGetXXResponse(MMC_R1_RESPONSE) == MMC_R1_RESPONSE) //�յ�����ֵ�����MMC/SD���Ƿ���ȷ��������
    {
      spiSendByte(0xff);
      spiSendByte(0xfe);                           //����������ʼ��־0xfe����ʾ�����������ݰ���һ���鳤�ȵ�����

      for (i = 0; i < count; i++)                  //дһ���鳤�ȵ����ݵ�MMC/SD��
        spiSendByte(pBuffer[i]);            

      spiSendByte(0xff);                           //����16λCRCֵ�����Ǳ���ģ����Բ���Щ��ʵCRCֵ
      spiSendByte(0xff);
      mmcCheckBusy();                              //MMC/SD����æ�����ز����ο���Ӧ���������ﲻ��ȡ
      rvalue = MMC_SUCCESS;                        //�����ɹ�
    }
    else
    {
      rvalue = MMC_RESPONSE_ERROR;   	           //��������ʧ�ܣ����ش������
    }
  }
  else
  {
    rvalue = MMC_BLOCK_SET_ERROR;                  //���ÿ鳤��ʧ�ܣ����ش������
  }

  CS_HIGH ();                                      //����Ƭѡ���ͷ�SD��
  spiSendByte(0xff);                               //С��ʱ
  return rvalue;                                   //����״̬����ֵ
} 

/***********************************************************************
�������ܣ�MMC/SD������ͺ���
�������ݣ�1��������������Ϊ48λ��MMC/SD������
		  2��ʵ���������6λ��48λ�л���������Ĳ�������ͬ���������ͬ
***********************************************************************/
void mmcSendCmd (uchar cmd, ulong data, uchar crc)
{
  uchar frame[6];                                  //MMC/SD�������Ϊ48λ,6���ֽ�
  uchar temp;
  int i;
  frame[0]=(cmd|0x40);                             //�����ʽ����ߵ���λΪ01�����ڵ��ֽ���˵��"��"��0x40
  for(i=3;i>=0;i--)                                //4���ֽڳ��ȵĲ�������û�У���Ϊ��ַ����ͬ�����²�����ͬ
  	{
    	temp=(uchar)(data>>(8*i));
    	frame[4-i]=(temp);
  	}
  frame[5]=(crc);                                  //1���ֽڵ�CRCֵ
  for(i=0;i<6;i++)
    	spiSendByte(frame[i]);                     //��������
}

/**********************************************************************
�������ܣ�MMC/SD���鳤������
�������ݣ�1�����ÿ鳤�ȣ�����Ϊ2^n��С��һ��Ϊ512
		  2������RAMС�ĵ�Ƭ�������Զ���С�Ŀ鳤�ȣ�������ʡ���ݿռ�
***********************************************************************/
uchar mmcSetBlockLength (uint blocklength)
{

  CS_LOW ();                                       //ѡ��SD��Ƭѡ�ź�
  mmcSendCmd(MMC_SET_BLOCKLEN, blocklength, 0xFF); //�������ÿ鳤������CMD16
  if(mmcGetResponse()!=0x00)                       //����Ͳ��ɹ��������¸�λMMC/SD����Ȼ����������
  	{ 
  		initMMC();
    	mmcSendCmd(MMC_SET_BLOCKLEN, blocklength, 0xFF);
    	mmcGetResponse();
  	}

  CS_HIGH ();                                      //�ͷ�MMC/SD��
 
  spiSendByte(0xff);                               //8��ʱ�ӵ���ʱ

  return MMC_SUCCESS;                              //���ز����ɹ�����
} 

/**********************************************************************
�������ܣ���CSD����CID�Ĵ���ֵ
�������ݣ�1��CSD��CID�Ĵ�����¼�˿��������Ϣ
		  2������ͨ��CSD����ȡ��������
***********************************************************************/
uchar mmcReadRegister (uchar cmd_register, uchar length, uchar *pBuffer)
{
  uchar i = 0;
  uchar rvalue = MMC_TIMEOUT_ERROR;

  if (mmcSetBlockLength (length) == MMC_SUCCESS)   //���ÿ鳤��
  {
    CS_LOW ();

    mmcSendCmd(cmd_register, 0x00, 0xff);          //���ͶԼĴ�����������

    if (mmcGetResponse() == 0x00)                  //���سɹ�����
    {
      if (mmcGetXXResponse(MMC_START_DATA_BLOCK_TOKEN)==MMC_START_DATA_BLOCK_TOKEN) //������ʼ��־λ��0xfe
        for (i = 0; i < length; i++)
          pBuffer[i] = spiSendByte(0xff);          //��ȡ���ݣ����Ĵ�����ֵ
      spiSendByte(0xff);                           //16λCRCֵ�����Ǳ����
      spiSendByte(0xff);
      rvalue = MMC_SUCCESS;                        //���سɹ�����
    }
    else
      rvalue = MMC_RESPONSE_ERROR;                 //��������ɹ������ش������

    CS_HIGH ();                                    //�ͷ�MMC/SD��

    spiSendByte(0xff);			           //��ʱ8��ʱ������
  }
  CS_HIGH ();                                      //�ͷ�MMC/SD��
  return rvalue;                                   //����״ֵ̬
} 

/**********************************************************************
�������ܣ�MMC/SD��������ȡ
�������ݣ�1���Ȼ�ȡCSD�Ĵ�������
		  2�������������㹫ʽ��ȡ������С
		  3��������СΪ�û�ʹ�ÿռ��С��������������
***********************************************************************/
ulong MMC_ReadCardSize(void)
{

  ulong MMC_CardSize;
  uint i, j,b,response,mmc_C_SIZE=0;     

  uchar mmc_READ_BL_LEN=0;         		   //���С
  uchar mmc_C_SIZE_MULT=0;         		   //������С

  CS_LOW ();
  
  mmcSendCmd(MMC_READ_CSD, 0x00, 0xff);	          //���Ͷ�ȡCSD�Ĵ�������CMD9

  response = mmcGetResponse();

  b = spiSendByte(0xFF);                          //��ȡ����ֵ�����ݿ�ʼ��־����0xfe

  if( !response )
  {
    while (b != 0xfe) b = spiSendByte(0xff);      //�ȴ����յ�������ʼ��־λ

    for(j=5; j>0; j--)                            //ǰ5���ֽڵ����ݲ��ã�Bit[127:88]
      b = spiSendByte(0xff);

    b =spiSendByte(0xff);                         
    mmc_READ_BL_LEN = b & 0x0F;                   //Bit[83:80]��¼�˿��С���ο�CSD�Ĵ������˵��

    b = spiSendByte(0xff);

    mmc_C_SIZE = (b & 0x03) << 10;               //Bit[73:62]��¼��C_SIZE�Ĵ�С���ο�CSD�Ĵ���˵��
    b = spiSendByte(0xff);
    mmc_C_SIZE += b << 2;
    b = spiSendByte(0xff);
    mmc_C_SIZE += b >> 6;

    b = spiSendByte(0xff);


    mmc_C_SIZE_MULT = (b & 0x03) << 1;           //Bit[49:47]��¼�˳����Ĵ�С
    b = spiSendByte(0xff);
    mmc_C_SIZE_MULT += b >> 7;


    b = spiSendByte(0xff);                       //֮��������������޹�

    b = spiSendByte(0xff);

    b = spiSendByte(0xff);

    b = spiSendByte(0xff);

    b = spiSendByte(0xff);

  }

  for(j=4; j>0; j--)          			// �������ٱ���32��ʱ�����ڵ�SCK
    b = spiSendByte(0xff);  					 
                               					
  b = spiSendByte(0xff);
  CS_LOW ();

  MMC_CardSize = (mmc_C_SIZE + 1);              //���������㣬�ο��������㹫ʽ

  for(i = 2,j=mmc_C_SIZE_MULT+2; j>1; j--)      //������С���鳤�ȡ���������������������һ�㱣����Ϊ�ܴ�С��1%
    i <<= 1;
  MMC_CardSize *= i;

  for(i = 2,j=mmc_READ_BL_LEN; j>1; j--)
    i <<= 1;
  MMC_CardSize *= i;

  return (MMC_CardSize);                        //��������ֵ��С����λΪ�ֽ�

}
/*************************************************
�������ܣ����MMC/SD���Ƿ����
�������ݣ�1�����MMC/SD��δ���룬��6������ߵ�ƽ(Ե������)
          2����ģ���ݲ�֧��Ӳ�����
*************************************************/
char mmc_ping(void)
{
  if (!(P5IN & 0x01))
    return (MMC_SUCCESS);
  else
    return (MMC_INIT_ERROR);
}

unsigned char mmc_test(void)
{
    unsigned int BlockLength=512;                    //MMC/SD�����С��CSD�Ĵ�����¼�˿���������Ĵ�С��һ��Ϊ512
    unsigned long  numSector=20;	                 //��SD����д�����ݵĵ�ַ��Ϊ������������һ������Ϊ512�ֽڵ�SD��ʵ�ʵ�ַΪnumSector*512�ֽ�
    unsigned char buffer1[512];              //���ݻ�����
    unsigned char buffer2[512];              //���ݻ�����
    unsigned char success=0;
    unsigned int i;
    unsigned char status = 1;
    unsigned int timeout = 0;

   while (status != 0)                             //������ط���ֵ�����ʾ����SD���������³�ʼ��                                           
  {
      status = initMMC();
      timeout++;
      if (timeout == 50)                            //��ʼ��50��MMC/SD���������Ȼ���ش�����룬���ʼ��ʧ�ܣ�������û��SD������
      {
        //DeBug_Print_Str("Init Failure\r\n");  
        //while(1);                                   //���˳������Ǹ�λ�������ϵ�
        return 0;
      }
  }   
   buffer1[0]='A';
  for(i=1;i<512;i++)
  {
     buffer1[i]=buffer1[0];   	                  //�����������������������ݻ�����buffer1
  }
 
    mmcWriteSector(numSector, buffer1);             //��ָ��������ַ(numSector*512)д������
    
    mmcReadSector(numSector, buffer2);	          //��ȡ�ղ�д��ռ�����ݣ��ж��Ƿ�д��ɹ�
    
   for(i=0;i<BlockLength;i++)                       //�ж϶�д�Ƿ�ɹ���������д�������Ƿ�һ��
  {
    if(buffer1[i]!=buffer2[i])                     //���ݶ�дʧ��
    {
  
      success=0;
      break;
    }
    else 
    {
     
      success=1;
    }
  }
  return success;
}

#ifdef withDMA
#ifdef __IAR_SYSTEMS_ICC__
#if __VER__ < 200
interrupt[DACDMA_VECTOR] void DMA_isr(void)
#else
#pragma vector = DACDMA_VECTOR
__interrupt void DMA_isr(void)
#endif
#endif

#ifdef __CROSSWORKS__
void DMA_isr(void)   __interrupt[DACDMA_VECTOR]
#endif

#ifdef __TI_COMPILER_VERSION__
__interrupt void DMA_isr(void);
DMA_ISR(DMA_isr)
__interrupt void DMA_isr(void)
#endif
{
  DMA0CTL &= ~(DMAIFG);
  LPM3_EXIT;
}
#endif

#endif /* _MMCLIB_C */
