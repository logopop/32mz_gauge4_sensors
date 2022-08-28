
//*********************************************************************************************************************
// Common library  PIC32MX
//
//*********************************************************************************************************************
//

#ifndef _COMMON_H_
#define _COMMON_H_ 
 
//*********************************************************************************************************************
// NVRAM


//*********************************************************************************************************************
// NVRAM

#define                          NVM_PROGRAM_PAGE           0xBD07E000           // Start address
#define                          NVM_PAGE_SIZE              4096
#define                          NVM_MAGIC                  0xcafebabe           // Valid config header

// Other defines
#define                          BYTE                       unsigned char
#define                          MAX_BUF                    255
#define                          HALF_BUF                   128

// Debug colors
#define                          D_GREEN                    0x00
#define                          D_YELLOW                   0x01
#define                          D_RED                      0x02
#define                          D_PURPLE                   0x03
#define                          D_BLUE                     0x04


// Library function prototypes 
void vBeep(int);
void vDebug(char *, BYTE);
void vShortDelay(unsigned long);     // Delay Time (CoreTimer Ticks)
void vLongDelay(unsigned long);      // Delay Time (milliseconds at 200mhz)

void vSetRGBled(unsigned long *, unsigned char, unsigned char, unsigned char, unsigned char);
void vClearRGBled(unsigned long *, unsigned long);

//unsigned long ulGetBmpImage(SYS_FS_HANDLE, uint8_t *);

//*********************************************************************************************************************
// 
//
//*********************************************************************************************************************
//
  
  
  
  
  
  
#endif


