//******************************************************************************
//FAT�ļ�ϵͳ��֧���ļ���д����Ŀ¼�ļ����ļ��н�������Ŀ¼�ļ����ļ��н������ļ�ɾ�����ļ�ϵͳ��ʽ�����ļ�����
//֧��FAT16��FAT32�ļ�ϵͳ�������ļ�ϵͳ���Ų�һ����Э��

//****************************************************************************** 


#include "mmc.h"
#include <stdio.h>
#include "FAT.h"
uint8 BUFFER[512];

uint16 BPB_BytesPerSec;
static uint8 BPB_SecPerClus;
uint16 BPB_RsvdSecCnt;
uint8 BPB_NumFATs;
uint16 BPB_RootEntCnt;
uint16 BPB_TotSec16;
uint16 BPB_FATSz16;
uint32 BPB_HiddSec;

uint32 BPB_FATSz32;
uint32 BPB_RootClus;
uint16 BPB_FSInfo;
uint8  FAT32_EN;                               //Ϊ0���ʾFAT16��Ϊ1���ʾFAT32

uint16 DirectoryCount = 0,FileCount = 0;       //��Ŀ¼���ļ��к��ļ�������

/***********************************************************************
�������ܣ���һ����������
�������ݣ�1������MMC/SD�����ȡ����
          2�������궨��ο�ͷ�ļ�
***********************************************************************/

void ReadBlock(uint32 LBA)
{
	mmcReadSector(LBA,BUFFER);
        return;
}

/***********************************************************************
�������ܣ�дһ����������
�������ݣ�1������MMC/SD����д�뺯��
          2�������궨��ο�ͷ�ļ�
***********************************************************************/

void WriteBlock(uint32 LBA)
{
	mmcWriteSector(LBA,BUFFER);
        return;
}

/***********************************************************************
�������ܣ��ֽ����ݸ���
�������ݣ����ݸ��ƣ����ֽڷ�ʽ��size�����С
***********************************************************************/

void CopyBytes(void* S, void* D, uint16 size)
{
	uint8 *s = S, *d = D;
	uint16 i;
	for(i = 0; i < size; i++)
		*d++ = *s++;
}

/***********************************************************************
�������ܣ������ݸ���
�������ݣ����ݸ��ƣ����ַ�ʽ��size�����С
***********************************************************************/

void CopyWord(void* S, void* D, uint16 size)
{
	uint16 *s = S, *d = D;
	uint16 i;
	for(i = 0; i < size; i++)
		*d++ = *s++;
}

/***********************************************************************
�������ܣ����ݱȽϺ���
�������ݣ�1���Ƚ������Ƿ���ȣ���ȷ���1������ȷ���0
          2������Ϊ��ʼ��ַ�����ݴ�С
***********************************************************************/

uint8 IsEqual(void* A, void* B, uint8 Size)
{
	uint8 i, *a = A, *b = B;
	for(i = 0; i < Size; i++)
		if(a[i] != b[i])
			return 0;              //����ȷ���0
	return 1;                              //��ȷ���1
}

/***********************************************************************
�������ܣ��������ֽ����
�������ݣ�����ȫΪ0��size�������0�Ĵ�С
***********************************************************************/

void EmptyBytes(void* D, uint16 size)
{
	uint16 i;
	uint8* data = (uint8*)D;
	for(i = 0; i < size; i++)
	{
		*data++ = 0;
	}
}
/***********************************************************************
�������ܣ�д��MMC/SD��FAT�ļ�ϵͳBPB(BIOS Parameter Block)���ݽṹ
�������ݣ�1��BPB���ݽṹ��ο�ͷ�ļ�����
          2��BPB���ݰ������̵��߼��ṹ��ɣ���������С���ش�С����Ϣ
***********************************************************************/
void FATInit(void)
{
	FAT_BPB* BPB = (FAT_BPB*)BUFFER;
		
	BPB->BPB_BytesPerSec = 0x0200;          //ÿ������512�ֽ�
	BPB->BPB_SecPerClus  = 0x20;            //ÿ����8������
	BPB->BPB_RsvdSecCnt  = 0x06;          //һ����������
	BPB->BPB_NumFATs     = 0x02;            //2��FAT��
	BPB->BPB_RootEntCnt  = 0x200;          //512Ŀ¼�����
	BPB->BPB_TotSec16    = 0x00;          //��������
	BPB->BPB_FATSz16     = 0xF1;            //һ��FAT����ռ��������
	BPB->BPB_HiddSec     = 0xFD;      //��FAT��ǰ���ص�������
	
	WriteBlock(253);                          //д������
}


/***********************************************************************
�������ܣ���ȡFAT16�ļ�ϵͳBPB(BIOS Parameter Block)���ݽṹ
�������ݣ�1��BPB���ݽṹ��ο�ͷ�ļ�����
          2��BPB���ݰ������̵��߼��ṹ��ɣ���������С���ش�С����Ϣ
***********************************************************************/

void ReadBPB(void)
{
	FAT_BPB* BPB = (FAT_BPB*)BUFFER;
	ReadBlock(0);
        
        ReadBlock(BUFFER[0x1C6]);                  //��ȡ��������BPB�����ݶ��뻺����
        
	BPB_BytesPerSec = BPB->BPB_BytesPerSec;   //��Щ��¼�����ݽṹ  
	BPB_SecPerClus = BPB->BPB_SecPerClus;     //ÿ������������ʽ��ʱ����ѡÿ���صĴ�С����2K��32K
	BPB_RsvdSecCnt = BPB->BPB_RsvdSecCnt;     //MMC/SD����ʽ���󣬱���Ϊ6������
	BPB_NumFATs = BPB->BPB_NumFATs;           //2��FAT�������һ��FAT���𻵣�����������һ��
	BPB_RootEntCnt = BPB->BPB_RootEntCnt;
	BPB_TotSec16 = BPB->BPB_TotSec16;         //���Ϊ0��BPB_FATSz16��¼��������
	BPB_FATSz16 = BPB->BPB_FATSz16;           //FATռ������������FAT32Ϊ0��FAT16��Ϊ0
	BPB_HiddSec = BPB->BPB_HiddSec;           //����������Ĭ��Ϊ0

}

/***********************************************************************
�������ܣ���ȡFAT32�ļ�ϵͳBPB(BIOS Parameter Block)���ݽṹ
�������ݣ�1��BPB���ݽṹ��ο�ͷ�ļ�����
          2��BPB���ݰ������̵��߼��ṹ��ɣ���������С���ش�С����Ϣ
***********************************************************************/

void ReadBPB32(void)
{
	FAT32_BPB* BPB = (FAT32_BPB*)BUFFER;
	ReadBlock(253);                             //��ȡ��������BPB�����ݶ��뻺����
        
	BPB_BytesPerSec = BPB->BPB_BytesPerSec;   //��Щ��¼�����ݽṹ  
	BPB_SecPerClus = BPB->BPB_SecPerClus;     //ÿ������������ʽ��ʱ����ѡÿ���صĴ�С����2K��32K
	BPB_RsvdSecCnt = BPB->BPB_RsvdSecCnt;     //MMC/SD����ʽ���󣬱���Ϊ6������
	BPB_NumFATs = BPB->BPB_NumFATs;           //2��FAT�������һ��FAT���𻵣�����������һ��
	BPB_RootEntCnt = BPB->BPB_RootEntCnt;
	BPB_TotSec16 = BPB->BPB_TotSec16;         //���Ϊ0��BPB_FATSz16��¼��������
	BPB_FATSz16 = BPB->BPB_FATSz16;           //FATռ������������FAT32Ϊ0��FAT16��Ϊ0
	BPB_HiddSec = BPB->BPB_HiddSec;           //����������Ĭ��Ϊ0
        
        BPB_FATSz32 = BPB->BPB_FATSz32;
        BPB_RootClus =BPB->BPB_RootClus;
        BPB_FSInfo = BPB->BPB_FSInfo;
}



/***********************************************************************
�������ܣ���ȡFAT�������ţ�֧��FAT16/32
�������ݣ�1��FAT��������Ϊ��������+��������
          2����������Ĭ��Ϊ0
***********************************************************************/

uint32 FATStartSec(void)
{
    return BPB_RsvdSecCnt + BPB_HiddSec; //????????????????????????????
}

/***********************************************************************
�������ܣ���ȡ��Ŀ¼��ʼ�����ţ�֧��FAT32
�������ݣ�1����Ŀ¼�������ǹ̶���
          2����Ŀ¼����������FAT��֮��
***********************************************************************/

uint32 DirStartSec(void)
{
  uint32 FATSz;
  if(FAT32_EN==0)
    FATSz=BPB_FATSz16;
  else
    FATSz=BPB_FATSz32;
    return BPB_HiddSec+BPB_RsvdSecCnt + BPB_NumFATs * FATSz;             //��������+FAT������FATռ��������   ??????????????????????
}

/***********************************************************************
�������ܣ���ȡ��������ʼ�����ţ�֧��FAT32
�������ݣ�1���������ݿ�ʼ������
          2��FAT32ϵͳ��BPB_RootEntCnt=0
***********************************************************************/

uint32 DataStartSec(void)
{
	return (uint32)(DirStartSec() + (BPB_RootEntCnt * 32) / BPB_BytesPerSec); //����32����Ϊ��Ŀ¼��32λ�����ļ�
}

/***********************************************************************
�������ܣ���Ŀ¼ռ����������FAT16��Ч
�������ݣ�1�������Ŀ¼ռ��������
          2����Ŀ¼Ϊ����Ϊ32λ�ġ��ļ���
          3�����������ļ��������ļ�
***********************************************************************/

uint16 GetDirSecCount(void)
{
	return BPB_RootEntCnt * 32 / BPB_BytesPerSec;
}

/***********************************************************************
�������ܣ���Ŀ¼���ڵ�һ���صĴغţ�FAT32��Ч
�������ݣ�1��FAT32��Ŀ¼���ǲ�ȷ���ģ�FAT16Ϊ�̶�ֵ512
          2����Ŀ¼�����
***********************************************************************/
uint32 GetDirStartClus(void)
{
	return BPB_RootClus;
}

/***********************************************************************
�������ܣ���ȡһ�������Ĵص�������ʼ��ַ
�������ݣ�1������ؿ�ʼ������    
          2���������FAT������0��ƫ����
          3������ClusIDΪ�����غţ�Ϊ2^nֵ����2��4��8��16��
***********************************************************************/

uint32 ClusConvLBA(uint16 ClusID)
{
	return DataStartSec() + BPB_SecPerClus * (ClusID - 2);
}

/***********************************************************************
�������ܣ��ж��ļ�ϵͳ���ͣ�FAT16/FAT32�е�һ�֣�FAT12Ŀǰ����������
�������ݣ�1��ʵ�����ļ�ϵͳ���͵��ж�ֻ��Ψһ�ķ������жϴصĴ�С   
          2��FAT12�Ĵ������4084��FAT16���65524��FAT32����65524
          3��������ȫ��BPB��Ϣȷ�������Լ���
          4�����ǵ�FAT32�ļ�ϵͳ�����ڹ̶���FAT���������FATSz16=0
          5��ͨ��FATSz16�Ƿ�Ϊ0�����ж���FAT16����FAT32
***********************************************************************/
void FATSystemType(void)
{ 
  if(BPB_FATSz16!=0)
    FAT32_EN=0;                            //FAT16�ļ�ϵͳ
  else 
    FAT32_EN=1;                            //FAT32�ļ�ϵͳ
  
  //return FAT32_EN;
/*    FATSz=BPB_FATSz32;
  if(BPB_TotSec16!=0) 
    TotSec=BPB_TotSec16;
  else
    TotSec=BPB_TotSec32;
  DataSec=TotSec-(BPB_RsvdSecCnt+BPB_NumFATs*FATSz)+RootDirSectors);
  CountofClusters=Data / BPB_SecPerClus;  */
}

/***********************************************************************
�������ܣ���ȡ�ļ��������ָ�����һ��FAT��FAT1
�������ݣ�1��FAT��λ����������֮��һ����2��   
          2�������������ļ��أ�ÿ���ļ���Ӧһ��FAT��
          3��FAT16ϵͳ���Թ���2^16���ļ���
          4����¼�������й��ļ���α���ɢ�洢�ڲ�ͬ��������Ϣ
          5��ÿ���ļ���FAT��ռ2���ֽڣ�FAT32ϵͳ��4���ֽ�
***********************************************************************/

uint16 ReadFAT(uint16 Index)
{
	uint16 *RAM = (uint16*)BUFFER;
	ReadBlock(BPB_HiddSec+BPB_RsvdSecCnt + Index / 256);//?????????????
	return RAM[Index % 256];                //�������Ժ���Ƕ�Ӧ�����ڵ�ƫ������ռ2���ֽ�
}

uint32 Read32FAT(uint32 Index)
{
	uint32 *RAM = (uint32*)BUFFER;
	ReadBlock(BPB_HiddSec + BPB_RsvdSecCnt + Index / 128);
	return RAM[Index % 128];                //�������Ժ���Ƕ�Ӧ�����ڵ�ƫ������ռ4���ֽ�
}



/***********************************************************************
�������ܣ�д�ļ��������ָ�����һ��FAT��FAT1
�������ݣ�1��FAT��λ����������֮��һ����2��   
          2�������������ļ��أ�ÿ���ļ���Ӧһ��FAT��
          3��FAT16ϵͳ���Թ���2^16���ļ���
***********************************************************************/

void WriteFAT(uint16 Index, uint16 Value)
{
	uint16 *RAM = (uint16*)BUFFER;
	uint32 SecID;
	SecID = BPB_HiddSec + BPB_RsvdSecCnt + Index / 256;    //�����ţ���д��FATֵ����Ӧ����???????????????????????
	ReadBlock(SecID);
	RAM[Index % 256] = Value;                //�����ļ���ӦFAT����ֵ
	WriteBlock(SecID);
}

void Write32FAT(uint32 Index, uint32 Value)
{
        uint32 *RAM = (uint32*)BUFFER;
	uint32 SecID;
	SecID = BPB_HiddSec + BPB_RsvdSecCnt + Index / 128;    //�����ţ���д��FATֵ����Ӧ����
	ReadBlock(SecID);
	RAM[Index % 128] = Value;                //�����ļ���ӦFAT����ֵ
	WriteBlock(SecID);
}



/***********************************************************************
�������ܣ�д�ļ��������ָ����ڶ���FAT��FAT2
�������ݣ�1��FAT��λ����������֮��һ����2��   
          2�������������ļ��أ�ÿ���ļ���Ӧһ��FAT��
          3��FAT16ϵͳ���Թ���2^16���ļ���
***********************************************************************/

void WriteFAT2(uint16 Index, uint16 Value)
{
	uint16 *RAM = (uint16*)BUFFER;
	uint32 SecID;
	SecID = BPB_RsvdSecCnt + BPB_FATSz16 + Index / 256;   //�ڶ���FAT�������ڵ�һ��FAT������
	ReadBlock(SecID);
	RAM[Index % 256] = Value;
	WriteBlock(SecID);
}

void Write32FAT2(uint32 Index, uint32 Value)
{
        uint32 *RAM = (uint32*)BUFFER;
	uint32 SecID;
	SecID = BPB_RsvdSecCnt + BPB_FATSz16 + Index / 128;    //�����ţ���д��FATֵ����Ӧ����
	ReadBlock(SecID);
	RAM[Index % 128] = Value;                              //�����ļ���ӦFAT����ֵ
	WriteBlock(SecID);
}

/***********************************************************************
�������ܣ���ȡ��Ŀ¼�п���ʹ�õ�һ��
�������ݣ�1������ؿ�ʼ������    
          2���������FAT������0��ƫ����
          3������ClusIDΪ�����غţ�Ϊ2^nֵ����2��4��8��16��
          4��ʵ�������FileName[0]=0x00����0xE5������ʾΪ����ʹ��������岻һ��
          5��0x00��ʾ֮���ǿգ�û��Ŀ¼�����Բ��ٲ�ѯ��0xE5��ʾ��Ŀ¼��Ϊ�գ�����ʹ��
***********************************************************************/

uint16 GetEmptyDIR()
{
	uint16 DirSecCut, DirStart, i, m, ID = 0;
	DirSecCut = 32;               //GetDirSecCount();     //��Ŀ¼������������������FAT16��FAT32�����Ϊ512����Ŀ¼��
	DirStart = DirStartSec();                                            //��Ŀ¼��ʼ������ַ,FAT16Ϊ512
	for(i = 0; i < DirSecCut; i++)                                       //�����������ռ�����
	{
		ReadBlock(DirStart + i);                                     //����Ŀ¼����
		for(m = 0; m < 16; m++)
		{
			if((BUFFER[m * 32] == 0) || (BUFFER[m * 32] == 0xe5))//����Ϊ0x00�������0xe5���򣬸�Ŀ¼Ϊ32λ���ȵġ��ļ���
				return ID;                                   //���ؿ���ʹ������ID
			else
				ID++;
		}
	}
	return ID;                                                            //��ʾû�п��п���ʹ�õ���
}

/***********************************************************************
�������ܣ���ȡ��Ŀ¼�п���ʹ�õ�һ��
�������ݣ�1������ؿ�ʼ������    
          2��������Ը�Ŀ¼���ڿ�ʼ�ص�ƫ����
          3������ClusIDΪ�����غţ�Ϊ2^nֵ����2��4��8��16��
          4��ʵ�������FileName[0]=0x00����0xE5������ʾΪ����ʹ��������岻һ��
          5��0x00��ʾ֮���ǿգ�û��Ŀ¼�����Բ��ٲ�ѯ��0xE5��ʾ��Ŀ¼��Ϊ�գ�����ʹ��
***********************************************************************/

uint16 GetDirectoryEmptyDIR(uint16 ClusID)
{
	uint16 DirSecCut, DirStart, i, m, ID = 0;
	DirSecCut = BPB_SecPerClus*16;                                       //һ���ļ��е����ļ���Ŀ¼�һ��һ���ļ���ռһ�أ�������Ϊ��������ÿ����������
	DirStart = ClusConvLBA(ClusID);                                      //���ļ��е���ʼ��������ַ
	for(i = 0; i < DirSecCut; i++)                                       //�����������ռ�����
	{
		ReadBlock(DirStart + i);                                     //��Ŀ¼����
		for(m = 0; m < 16; m++)
		{
			if((BUFFER[m * 32] == 0) || (BUFFER[m * 32] == 0xe5))//����Ϊ0x00�������0xe5���򣬸�Ŀ¼Ϊ32λ���ȵġ��ļ���
				return ID;                                   //���ؿ���ʹ������ID����ƫ����
			else
				ID++;
		}
	}
	return ID;                                                            //��ʾû�п��п���ʹ�õ���
}

/***********************************************************************
�������ܣ���ú��ļ�����Ӧ�ĸ�Ŀ¼���Լ���Ŀ¼��λ��
�������ݣ�1���Ƚ��ļ������   
          2��ÿ���ļ����ж�Ӧ��FAT��
          3��ID���׵�ַ�Ƕ�Ӧ�ĸ�Ŀ¼����׵�ַ������ΪDIR(��ͷ�ļ�fat.h)
***********************************************************************/

uint8 GetFileID(uint8 Name[11], DIR* ID)
{
	uint16 DirSecCut, DirStart, i, m,Index;
	DirSecCut = 32;                                  //GetDirSecCount();��Ŀ¼�������������ｫFAT16/32ͳһ��512����Ŀ¼��
	DirStart = DirStartSec();                       //��Ŀ¼��ʼ����
	for(i = 0; i < DirSecCut; i++)
	{
		ReadBlock(DirStart + i);                //��ȡ��������
		for(m = 0; m <16; m++)                  //һ������������16�����Ϊÿ����ĳ�����32λ
		{
			if(IsEqual(Name, &((DIR*)&BUFFER[m * 32])->FileName, 11))   //���бȽϣ���ȷ���index
			{
				*ID = *((DIR*)&BUFFER[m * 32]);   //IDʵ�ʾ��Ǹ�Ŀ¼��
                                Index = i*16+m;                   //FAT16��һ�������511��Ҳ����˵FAT16�¸�Ŀ¼����һ��Ϊ512��Ϊ��֤������
				return Index;                     //�ҵ���Ӧ��Ŀ¼��,����Ŀ¼��λ��.
			}
		}
	}
	return 0;                                         //û���ҵ���Ӧ��Ŀ¼��,����0.
}

uint8 GetFileID32(uint8 Name[11], DIR32* ID)
{
	uint16 DirSecCut, DirStart, i, m,Index;
	DirSecCut = 32;                                  //GetDirSecCount();��Ŀ¼�������������ｫFAT16/32ͳһ��512����Ŀ¼��
	DirStart = DirStartSec();                       //��Ŀ¼��ʼ����
	for(i = 0; i < DirSecCut; i++)
	{
		ReadBlock(DirStart + i);                //��ȡ��������
		for(m = 0; m <16; m++)                  //һ������������16�����Ϊÿ����ĳ�����32λ
		{
			if(IsEqual(Name, &((DIR32*)&BUFFER[m * 32])->FileName, 11))   //���бȽϣ���ȷ���1
			{
				*ID = *((DIR32*)&BUFFER[m * 32]);   //IDʵ�ʾ��Ǹ�Ŀ¼��
                                Index = i*16+m;                   //FAT16��һ�������511��Ҳ����˵FAT16�¸�Ŀ¼����һ��Ϊ512��Ϊ��֤������
				return Index;                     //�ҵ���Ӧ��Ŀ¼��,����Ŀ¼��λ��.
			}
		}
	}
	return 0;                                         //û���ҵ���Ӧ��Ŀ¼��,����0.
}

/***********************************************************************
�������ܣ���ȡһ���յ�FAT��
�������ݣ�1������׼��д��FAT��    
          2��ÿ���ļ����ж�Ӧ��FAT��
***********************************************************************/

uint16 GetNextFAT(void)
{
	uint16 FAT_Count, i;
	FAT_Count = BPB_FATSz16 * 256;                      //FAT����������һ��������256���FAT16һ������2���ֽ�
	for(i = 0; i < FAT_Count; i++)
	{
		if(ReadFAT(i) == 0)                         //Ϊ0��ʾ�գ�������Ϊ�´������ļ���FAT��
			return i;
	}
	return 0;
}

uint32 GetNext32FAT(void)
{
	uint32 FAT_Count, i;
	FAT_Count = BPB_FATSz32 * 128;                      //FAT����������һ��������128���FAT32һ������4���ֽ�
	for(i = 0; i < FAT_Count; i++)
	{
		if(Read32FAT(i) == 0)                         //Ϊ0��ʾ�գ�������Ϊ�´������ļ���FAT��
			return i;
	}
	return 0;
}

/***********************************************************************
�������ܣ���ȡ��Ŀ¼��ָ����
�������ݣ�1����ȡָ����ĸ�Ŀ¼����   
          2��һ����ĳ�����32λ������һ�����ļ���
***********************************************************************/

void ReadDIR(uint16 Index, DIR* Value)
{
	uint32 DirStart = DirStartSec();
	ReadBlock(DirStart + Index / 16);
	CopyBytes(&BUFFER[(Index % 16) * 32], Value, 32);
}


/***********************************************************************
�������ܣ�д��Ŀ¼��ָ����
�������ݣ�1���½��ļ�ʱ�����ҵ����ø�Ŀ¼������д��   
          2��һ����ĳ�����32λ������һ�����ļ���
***********************************************************************/

void WriteDIR(uint16 Index, DIR* Value)
{
	uint32 LBA = DirStartSec() + Index / 16;
	ReadBlock(LBA);
	CopyBytes(Value, &BUFFER[(Index % 16) * 32], 32);
	WriteBlock(LBA);
}

void WriteDIR32(uint16 Index, DIR32* Value)
{
	uint32 LBA = DirStartSec() + Index / 16;
	ReadBlock(LBA);
	CopyBytes(Value, &BUFFER[(Index % 16) * 32], 32);
	WriteBlock(LBA);
}

/***********************************************************************
�������ܣ�д��Ŀ¼�ġ�.���͡�..��������
�������ݣ�1��һ����2��32Bytes  
          2��һ����ĳ�����32λ������һ�����ļ���
          3����ʼ��ַ����Ŀ¼����ʼ�ص�ַ
***********************************************************************/

void WriteDirectoryDIR(uint16 ClusID, uint16 Index, DIR* Value)
{
	uint32 LBA = ClusConvLBA(ClusID);
	ReadBlock(LBA);
	CopyBytes(Value, &BUFFER[(Index % 16) * 32], 32);
	WriteBlock(LBA);
}

void WriteDirectoryDIR32(uint16 Index, DIR32* Value)
{
	uint32 LBA = DirStartSec() + Index / 16;
	ReadBlock(LBA);
	CopyBytes(Value, &BUFFER[(Index % 16) * 32], 32);
	WriteBlock(LBA);
}

/***********************************************************************
�������ܣ��ļ��޸�����ת��Ϊ��Ӧ��ʽ����2010��07��01��ת��Ϊ0x3CE1
�������ݣ�1���޸����ڼ��㹫ʽ   
          2����������Ϊ2���ֽڳ���
          3�������ʵʱʱ��ʹ�ã��������Եõ�ʵʱ�ĸ�������
***********************************************************************/
uint16 GetFileCrtDate(uint16 Year, uint8 Month, uint8 Day)//�ꡢ�¡���
{
        uint16 FileCrtDate;
        FileCrtDate=(Year-1980)*512+Month*32+Day;    // 0~4λΪ��(1~31)��5��8λΪ��(1~12)��9��15λΪ��(0~127)�������1980��1��1�յ�ƫ����
        return FileCrtDate;
}

/***********************************************************************
�������ܣ��ļ��޸�ʱ��ת��Ϊ��Ӧ��ʽ
�������ݣ�1���޸�ʱ����㹫ʽ   
          2��ʱ������Ϊ2���ֽڳ���
          3�������ʵʱʱ��ʹ�ã��������Եõ�ʵʱ�ĸ���ʱ��
***********************************************************************/

uint16 GetFileCrtTime(uint8 Hour, uint8 Min, uint8 Sec)//ʱ���֡���
{
        uint16 FileCrtTime;
        FileCrtTime=Hour*2048+Min*32+Sec/2;       // 0~4λΪ�룬2��Ϊ1����λ(0~29)��5��10λΪ��(0~59)��11��15λΪʱ(0~23)
        return FileCrtTime;
}



/***********************************************************************
�������ܣ�����һ���ӿ��ļ�
�������ݣ�1���ļ������������ֻ��ߴ�д��ĸ��С��8λ���ÿո���չ��Ϊ�ո� 
          2���ļ���С���Ȳ�������,��Ϊ0;
          3���������ļ��Ĺ���ʵ�ʾ���д��Ŀ¼��FAT�������ﲻд�ļ��������
          4�������ļ���Ϊ������8���ֽڣ�Ҳ����8���ַ�
          5�����ļ���Ŀ¼д���丸Ŀ¼����ʼ�صĿհ�Ŀ¼��
***********************************************************************/
void CreateDirectoryFile(uint8 DirectoryName[11], uint8 FileName[11])  //���ļ��������½��ļ��������ļ���
{
	uint16 ClusID,ClusIDFile,Index;
        DIR FileDir;
        uint16 FileCrtDateTmp,FileCrtTimeTmp;                 //˫�ֽ�����       
        uint8 FileCrtDate[2],FileCrtTime[2];                  //���ֽ�ʱ����������
 
	FileDir.FilePosit.Start = GetNextFAT();               //�õ�һ���յ�FAT���������д��Ӧ�ļ���FAT��       
	ClusID = FileDir.FilePosit.Start;                     //�ļ��ؿ�ʼ�غţ��������ļ�����ʼ�ص�ַ
        ClusIDFile = FileDir.FilePosit.Start;                 //�ݴ��������Ա�����д��

	WriteFAT(ClusID, 0xffff);                             //д��Ӧ�ļ���FAT��1����FAT1,0xffff��ʾ�ļ�����
        WriteFAT2(ClusID, 0xffff);                            //д��Ӧ�ļ���FAT��2����FAT2��0xffff��ʾ�ļ�����     
                             
        Index = GetFileID(DirectoryName,&FileDir);            //�����õ��ļ��е����ԣ���Ҫ�ǻ����ʼ��λ�ã�����д�ļ���Ŀ¼��
        ReadDIR(Index, &FileDir);                             //��ȡ���ļ��е�Ŀ¼�ֻ��Ҫ����ʼ��
        
        FileCrtDateTmp=GetFileCrtDate(2010,7,2);              //�ļ��޸�����2010��7��2��
        FileCrtDate[0]=(uint8)FileCrtDateTmp&0x00ff;          //���ֽ���ȡ
        FileCrtDate[1]=(uint8)(FileCrtDateTmp>>8);            //���ֽ���ȡ       
        FileCrtTimeTmp=GetFileCrtTime(21,20,10);              //�ļ��޸�ʱ��21��20��10��
        FileCrtTime[0]=(uint8)FileCrtTimeTmp&0x00ff;          //���ֽ���ȡ
        FileCrtTime[1]=(uint8)(FileCrtTimeTmp>>8);            //���ֽ���ȡ      
        
        CopyBytes(FileName, &FileDir.FileName, 11);           //д���ļ���
        
        CopyBytes(FileCrtDate, &FileDir.FileCrtDate, 2);       //д���ļ���������
        CopyBytes(FileCrtTime, &FileDir.FileCrtTime, 2);       //д���ļ�����ʱ��
        
        FileDir.FileAttrib = 0x20;                             //�ļ�������䣬0x10Ϊ��Ŀ¼���ԣ����ļ���       
       
        ClusID = FileDir.FilePosit.Start;                      //���ļ��еĿ�ʼ�غţ����ļ�Ŀ¼��д���������
        FileDir.FilePosit.Start=ClusIDFile;                    //�½��ļ�����ʼ��
        WriteDirectoryDIR(ClusID,GetDirectoryEmptyDIR(ClusID),&FileDir);//��Ҫɨ�踸Ŀ¼����ʼ���пհ׿���д����Ŀ¼����
        
}


/***********************************************************************
�������ܣ��ڸ�Ŀ¼����һ�����ļ����ļ���
�������ݣ�1��FileName�ļ������������ֻ��ߴ�д��ĸ����չ��Ҳ�����   
          2���ļ���С���Ȳ�������,��Ϊ0;
          3�������ļ��Ĺ���ʵ�ʾ���д��Ŀ¼��FAT�������ﲻд�ļ��������
          4�������ļ���Ϊ������8���ֽڣ�Ҳ����8���ַ�
          5���ڶ�������Ϊ���ԣ�0x10Ϊ�ļ��У�0x20Ϊ�ļ�
          6�����ļ���ֻ�����ڸ�Ŀ¼�£����Ը���
          7����Ŀ¼��û��"."��".."��������Ŀ¼��Ҳ�����ļ��У�����
***********************************************************************/

void CreateFile(uint8 FileName[11], uint8 FileAttrib)
{
	uint16 ClusID;
        DIR FileDir;
        uint8 DirectoryDIRFileName0[11]=".          ";
        uint8 DirectoryDIRFileName1[11]="..         ";
        
        uint16 FileCrtDateTmp,FileCrtTimeTmp;                 //˫�ֽ�����       
        uint8 FileCrtDate[2],FileCrtTime[2];                  //���ֽ�ʱ����������
        
        FileCrtDateTmp=GetFileCrtDate(2012,5,2);              //�ļ��޸�����2010��7��2��
        FileCrtDate[0]=(uint8)FileCrtDateTmp&0x00ff;          //���ֽ���ȡ
        FileCrtDate[1]=(uint8)(FileCrtDateTmp>>8);            //���ֽ���ȡ
        
        FileCrtTimeTmp=GetFileCrtTime(21,20,10);              //�ļ��޸�ʱ��21��20��10��
        FileCrtTime[0]=(uint8)FileCrtTimeTmp&0x00ff;          //���ֽ���ȡ
        FileCrtTime[1]=(uint8)(FileCrtTimeTmp>>8);            //���ֽ���ȡ
        
        
	//ClusNum = Size / (BPB_SecPerClus * 512) + 1;
	EmptyBytes(&FileDir, sizeof(DIR));                          //��ȫ�����0
        CopyBytes(FileName, &FileDir.FileName, 11);                 //д���ļ���
        
        FileDir.FileAttrib = FileAttrib;                            //�ļ�������䣬0x10��Ϊ�ļ��У�0x20��Ϊ�ļ�
        
        CopyBytes(FileCrtDate, &FileDir.FileCrtDate, 2);            //д���ļ���������
        CopyBytes(FileCrtTime, &FileDir.FileCrtTime, 2);            //д���ļ�����ʱ��
        
	//FileDir.FilePosit.Size = 0;
	FileDir.FilePosit.Start = GetNextFAT();                     //�õ�һ���յ�FAT���������д��Ӧ�ļ���FAT��
        
	ClusID = FileDir.FilePosit.Start;                           //�ļ��ؿ�ʼ�غ�

	WriteFAT(ClusID, 0xffff);                                   //д��Ӧ�ļ���FAT��1����FAT1,0xffff��ʾ�ļ�����
        WriteFAT2(ClusID, 0xffff);                                  //д��Ӧ�ļ���FAT��2����FAT2��0xffff��ʾ�ļ�����
	WriteDIR(GetEmptyDIR(), &FileDir);                          //д��Ŀ¼ָ���������������ʹ�����д��
        
    
  if(FileDir.FileAttrib==0x10)                                      //����ǽ����ļ��У���д��.����͡�..�
     {
        CopyBytes(DirectoryDIRFileName0, &FileDir.FileName, 11);    //д��".��Ŀ¼����"������Ϊ����Ŀ¼������
        WriteDirectoryDIR(ClusID,0,&FileDir);                       //д�룬��ַΪ�½�Ŀ¼����ʼ�ص�ַ��ǰ2��32Bytes������Ϊ��0��
        
        CopyBytes(DirectoryDIRFileName1, &FileDir.FileName, 11);    //д��"..��Ŀ¼����"������Ϊ�븸Ŀ¼��ϵ����ʼ�ص�ַΪ��Ŀ¼����ʼ�ص�ַ
        FileDir.FilePosit.Start=0x0000;                             //�ļ���λ�ڸ�Ŀ¼�£��ɸ���Ϊ�ļ��еĸ��ļ�����ʼ�ص�ַ
        WriteDirectoryDIR(ClusID,1,&FileDir);                       //д�룬��ַΪ�½�Ŀ¼����ʼ�ص�ַ��ǰ2��32Bytes������Ϊ��1��
     }
}

void FAT32CreateFile(uint8 FileName[11])
{
	uint32 ClusID;
        DIR32 FileDir;
        uint16 FileWrDataTmp,FileWrTimeTmp;                         //˫�ֽ�����       
        uint8 FileWrData[2],FileWrTime[2];                          //���ֽ�ʱ����������
        
        FileWrDataTmp=GetFileCrtDate(2012,5,2);                     //�ļ��޸�����2010��7��2��
        FileWrData[0]=(uint8)FileWrDataTmp&0x00ff;                  //���ֽ���ȡ
        FileWrData[1]=(uint8)(FileWrDataTmp>>8);                    //���ֽ���ȡ
        
        FileWrTimeTmp=GetFileCrtTime(21,20,10);                     //�ļ��޸�ʱ��21��20��10��
        FileWrTime[0]=(uint8)FileWrTimeTmp&0x00ff;                  //���ֽ���ȡ
        FileWrTime[1]=(uint8)(FileWrTimeTmp>>8);                    //���ֽ���ȡ
                
	//ClusNum = Size / (BPB_SecPerClus * 512) + 1;
	EmptyBytes(&FileDir, sizeof(DIR));                          //��ȫ�����0
        CopyBytes(FileName, &FileDir.FileName, 11);                 //д���ļ���
        
        FileDir.FileAttrib = 0x20;                                  //�ļ��������
        
        CopyBytes(FileWrData, &FileDir.DIR_WrtDate, 2);             //д���ļ��޸�����
        CopyBytes(FileWrTime, &FileDir.DIR_WrtTime, 2);             //д���ļ��޸�ʱ��
        
	//FileDir.FilePosit.Size = 0;
	ClusID = GetNext32FAT();                                    //�õ�һ���յ�FAT���������д��Ӧ�ļ���FAT��
        
	FileDir.DIR_FstClusHI=(uint16)(ClusID>>16);                 //�ļ���ʼ�ظ���
        FileDir.DIR_FstClusLo=(uint16)ClusID&0x0000ffff;            //�ļ��ؿ�ʼ�غŵ���

	Write32FAT(ClusID, 0xffffffff);                             //д��Ӧ�ļ���FAT��1����FAT1,0x0fffffff��ʾ�ļ�����
        Write32FAT2(ClusID, 0xffffffff);                            //д��Ӧ�ļ���FAT��2����FAT2,0x0fffffff��ʾ�ļ�����
	WriteDIR32(GetEmptyDIR(), &FileDir);                        //д��Ŀ¼ָ���������������ʹ�����д��
}

/***********************************************************************
�������ܣ�ɾ���ļ�
�������ݣ�1��FileName�ļ������������ֻ��ߴ�д��ĸ����չ��Ҳ�����   
          2���������ļ�Ŀ¼�λ�ã���ȡ�Ƚ�ƥ��ķ���
          3���ļ����ĵ�һ���ֽ�д0xE5����ʾ��Ŀ¼�����ļ�
          4����FAT����д��Ŀ¼
***********************************************************************/

void DelFile(uint8 Name[11])
{
      DIR FileDir;
      uint16 ClusID,Index;
      Index = GetFileID(Name,&FileDir);                      //�����õ��ļ����ڸ�Ŀ¼��λ��
      FileDir.FileName.NAME[0] = 0xE5;                       //ɾ����Ŀ¼���ļ�
      ClusID = FileDir.FilePosit.Start;                      //�ļ����ڵĴغ�
      WriteFAT(ClusID,0x0000);                               //��ն�Ӧ��FAT��FAT1
      WriteFAT2(ClusID,0x0000);                              //ʹFAT2��FAT1����һ��
      WriteDIR(Index,&FileDir);                              //дָ����Ŀ¼��
      
}

void DelFile32(uint8 Name[11])
{
      DIR32 FileDir;
      uint32 ClusID;
      uint16 Index;
      Index = GetFileID32(Name,&FileDir);                      //�����õ��ļ����ڸ�Ŀ¼��λ��
      FileDir.FileName.NAME[0] = 0xE5;                       //ɾ����Ŀ¼���ļ�
      ClusID = ((uint32)FileDir.DIR_FstClusHI)<<16+(uint32)FileDir.DIR_FstClusLo;  //�ļ����ڵĴغ�
      Write32FAT(ClusID,0x00000000);                                //��ն�Ӧ��FAT��FAT1
      Write32FAT2(ClusID,0x00000000);                               //ʹFAT2��FAT1����һ��
      WriteDIR32(Index,&FileDir);                                   //дָ����Ŀ¼��
      
}

/***********************************************************************
�������ܣ������ļ������,ʹ��ͱ���һ��
�������ݣ�1����FAT��FAT1���Ƶ�FAT2���� 
          2��FATSz16��ʾFAT��ռ�õ���������BPB����ȡ��ֵ
          3������FAT��
***********************************************************************/

void CopyFAT(void)
{
	uint16 FATSz, RsvdSecCnt, i;
        if(FAT32_EN==0) 
          FATSz=BPB_FATSz16;
        else
          FATSz=BPB_FATSz32;
	RsvdSecCnt = BPB_HiddSec + BPB_RsvdSecCnt;
	for(i = 0; i < FATSz; i++)
	{
		ReadBlock(RsvdSecCnt + i);
		WriteBlock(RsvdSecCnt + FATSz + i);
	}
}

/***********************************************************************
�������ܣ�FAT16�����ļ������ݣ�֧�ֶ�д
�������ݣ�1��WriteΪ1��д������Ϊ0�Ƕ�������StartΪ��ʼ�ֽڣ�LengthΪ�������� 
          2��DataΪ�������ݣ���д�뻺����
          3����Ҫͬ������FAT������¼�ļ���ռ��Сλ��
***********************************************************************/

void OperateFile(uint8 Write ,uint8 Name[11], uint32 Start, uint32 Length, void* Data)
{
	uint8 *data = Data;
	uint16 BytePerClus, SecPerClus, ClusNum, ClusNext,ClusID,ClusIDValue ,m,Index;
	uint32 LBA, i,length;
	DIR FileDir;
	SecPerClus = BPB_SecPerClus;
	BytePerClus = BPB_SecPerClus * BPB_BytesPerSec;               //ÿ�ص��ֽ���
	Index = GetFileID(Name, &FileDir);                            //�ļ���Ŀ¼λ��0��511
        length = Length;

	//���㿪ʼλ�����ڴصĴغ�
	ClusNum = Start / BytePerClus;
	ClusID = FileDir.FilePosit.Start;

	for(i = 0; i < ClusNum; i++)
	{
                ClusIDValue = ReadFAT(ClusID);                        //��ȡ��Ӧ�ļ���FAT����0xFFFF��ʾ�ļ�����
		WriteFAT(ClusID, 0xffff);
                WriteFAT2(ClusID, 0xffff);
                if(ClusIDValue==0xffff)                               //���ClusIDValueֵ��Ϊ0xFFFF�������ı�ʾ��һ���ص�ַ
                  ClusNext = GetNextFAT();
                else
                  ClusNext = ClusIDValue; 
                
		WriteFAT(ClusID, ClusNext);
                WriteFAT2(ClusID, ClusNext);
		ClusID = ClusNext;
	}
        WriteFAT(ClusID, 0xffff);
        WriteFAT2(ClusID, 0xffff);

	//���㿪ʼλ��������������ƫ��
	i = (Start % BytePerClus) / 512;

	//���㿪ʼλ��������ƫ��
	m = (Start % BytePerClus) % 512;

	LBA = ClusConvLBA(ClusID) + i;


		for(i = 0; i < SecPerClus; i++)
		{
			if(Write)
				ReadBlock(LBA);
			else
				ReadBlock(LBA++);

			for(m = 0; m < 512; m++)
			{

				if(Write)
					BUFFER[m] = *data++;
				else
					*data++ = BUFFER[m];

				//�����ȡ��ɾ��˳�
				if(--Length == 0)
				{
					if(Write)
                                        {
                                                //��д����
						WriteBlock(LBA); 
                                               //��дĿ¼
                                                FileDir.FilePosit.Size = Start+length;
                                                WriteDIR(Index,&FileDir);
                                        }
					return;
				}
			}
			if(Write)
				WriteBlock(LBA++); //��д����,ָ������
		}
	}

/***********************************************************************
�������ܣ�FAT32�����ļ������ݣ�֧�ֶ�д
�������ݣ�1��WriteΪ1��д������Ϊ0�Ƕ�������StartΪ��ʼ�ֽڣ�LengthΪ�������� 
          2��DataΪ�������ݣ���д�뻺����
          3����Ҫͬ������FAT������¼�ļ���ռ��Сλ��
***********************************************************************/

void Operate32File(uint8 Write ,uint8 Name[11], uint32 Start, uint32 Length, void* Data)
{
	uint8 *data = Data;
	uint16 BytePerClus, SecPerClus, ClusNum ,m,Index;
        uint32 ClusID,ClusIDValue, ClusNext;
	uint32 LBA, i,length;
	DIR32 FileDir;
	SecPerClus = BPB_SecPerClus;
	BytePerClus = BPB_SecPerClus * BPB_BytesPerSec;               //ÿ�ص��ֽ���
	Index = GetFileID32(Name, &FileDir);                            //�ļ���Ŀ¼λ��0��511
        length = Length;

	//���㿪ʼλ�����ڴصĴغ�
	ClusNum = Start / BytePerClus;
        ClusID=((uint32)FileDir.DIR_FstClusHI<<16)+(uint32)FileDir.DIR_FstClusLo;
  	for(i = 0; i < ClusNum; i++)
	{
                ClusIDValue = Read32FAT(ClusID);                        //��ȡ��Ӧ�ļ���FAT����0xFFFF��ʾ�ļ�����
		Write32FAT(ClusID, 0x0fffffff);
                Write32FAT2(ClusID, 0x0fffffff);
                if(ClusIDValue==0x0fffffff)                               //���ClusIDValueֵ��Ϊ0xFFFF�������ı�ʾ��һ���ص�ַ
                  ClusNext = GetNext32FAT();
                else
                  ClusNext = ClusIDValue; 
                
		Write32FAT(ClusID, ClusNext);
                Write32FAT2(ClusID, ClusNext);
		ClusID = ClusNext;
	}
        Write32FAT(ClusID, 0x0fffffff);
        Write32FAT2(ClusID, 0x0fffffff);

	//���㿪ʼλ��������������ƫ��
	i = (Start % BytePerClus) / 512;

	//���㿪ʼλ��������ƫ��
	m = (Start % BytePerClus) % 512;

	LBA = ClusConvLBA(ClusID) + i;


		for(i = 0; i < SecPerClus; i++)
		{
			if(Write)
				ReadBlock(LBA);
			else
				ReadBlock(LBA++);

			for(m = 0; m < 512; m++)
			{

				if(Write)
					BUFFER[m] = *data++;
				else
					*data++ = BUFFER[m];

				//�����ȡ��ɾ��˳�
				if(--Length == 0)
				{
					if(Write)
                                        {
                                                //��д����
						WriteBlock(LBA); 
                                               //��дĿ¼
                                                FileDir.DIR_FileSize = Start+length;
                                                WriteDIR32(Index,&FileDir);
                                        }
					return;
				}
			}
			if(Write)
				WriteBlock(LBA++); //��д����,ָ������
		}
	}

/***********************************************************************
�������ܣ���ʽ������
�������ݣ�1���������������ϵĸ�ʽ����ֻ�������Ŀ¼
          2��ʵ���ļ���������Ȼ����
***********************************************************************/

void FormatCard(void)
{
    uint16 i,DirStart,DirSecNum,FatStart,FatSecNum;
   
    //ReadBPB();

    for(i = 0; i < 512;i++ )BUFFER[i]=0;                  //�����Ŀ¼����������0
    DirSecNum = GetDirSecCount();                          //FAT16�ĸ�Ŀ¼����λ512��ռ32��������FAT32��Ŀ¼��������
    DirStart = DirStartSec();                              //��ȡ��Ŀ¼��ʼ��ַ
    for(i=0;i<DirSecNum;i++)                               //��������������0
    {
      WriteBlock(DirStart+i);
    }
 

    FatStart = BPB_HiddSec + BPB_RsvdSecCnt;              //���FAT��
    FatSecNum = BPB_FATSz16;                              //FAT����ռ��������С
    for(i=0;i<FatSecNum;i++)
    {
      WriteBlock(FatStart+i);
    }
    WriteFAT(0,0xfff8);                                   //FAT���ĵ�һ���̶ֹ�Ϊ0xfff8
    WriteFAT(1,0xffff);                                   //FAT���ĵڶ����̶ֹ�Ϊ0xffff

    CopyFAT();                                            //����FAT����������FAT1һ��
    

}
       
/***********************************************************************
�������ܣ���Ŀ¼���ļ��к��ļ���������
�������ݣ�1��������Ŀ¼���Ŀ¼�����Լ����� 
          2�������ж��ļ����ͣ����ļ��С�txt�ļ���BMP�ļ���MP3�ļ���
          3�������ļ�����
***********************************************************************/  
void FileCountSearch(void)
{/*
	uint16 BytePerClus, SecPerClus, ClusNum, ClusNext,ClusID,ClusIDValue ,m,Index;
	uint32 LBA, i;
	DIR FileDir;
	SecPerClus = BPB_SecPerClus;
	BytePerClus = BPB_SecPerClus * BPB_BytesPerSec;               //ÿ�ص��ֽ���
	Index = GetFileID(Name, &FileDir);                            //�ļ���Ŀ¼λ��0��511
  
      */  
        uint16 DirSecCut, DirStart, i, m;//, Count = 0;
	DirSecCut = 32;                                               //��Ŀ¼������������������FAT16��FAT32�����Ϊ512����Ŀ¼��
	DirStart = DirStartSec();                                     //��Ŀ¼��ʼ������ַ,FAT16Ϊ512
        DirectoryCount = 0,FileCount = 0;
	for(i = 0; i < DirSecCut; i++)                                //�����������ռ�����
	{
		ReadBlock(DirStart + i);                              //����Ŀ¼����
		for(m = 0; m < 16; m++)
		{
			if((BUFFER[m * 32] != 0) && (BUFFER[m * 32] != 0xe5))//����Ϊ0x00�������0xe5���򣬸�Ŀ¼Ϊ32λ���ȵġ��ļ���
                        {
                          if(BUFFER[m * 32+11] == 0x10)               //0x10Ϊ�ļ���
                            DirectoryCount++;
                          if(BUFFER[m * 32+11] == 0x20)               //0x20Ϊ�ļ�
                            FileCount++;
                        //  else
                            //Count=0;
			}
		}
	}
}