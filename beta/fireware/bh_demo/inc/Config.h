#include <msp430x14x.h>

#define CPU_F ((double)8000000) 
#define uchar unsigned char
#define uint  unsigned int
#define ulong unsigned long
#define WORD unsigned short
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0)) 
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0)) 

#define TRUE    1
#define FALSE   0

#define EEPROM_USE 
#define FAT_USE 
#define DEBUG 1