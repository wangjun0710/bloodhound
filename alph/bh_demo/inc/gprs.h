

#include <string.h>
#include "config.h"
#include "gm8125.h"

#define GPRS_PORT_NUM 4
#define GPRS_Recieve_Buf 100
#define Command_Buf 5
#define Num_Of_Package 11

void clr_Buf(void);
unsigned char hand(unsigned char *a);
void gprs_band_rate(void);
void gprs_check_china_mobile(void);
unsigned char gprs_connect_server(void);
void GPRS_Send_Byte(unsigned char data);
void GPRS_Print_Str(unsigned char *s);

extern unsigned char GsmRcv[GPRS_Recieve_Buf];
extern unsigned char GsmRcvCnt;