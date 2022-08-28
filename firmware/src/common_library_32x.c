
//*********************************************************************************************************************
// Common library
//
//*********************************************************************************************************************
//
//     
//*********************************************************************************************************************
//
  
#include	      <ctype.h>
#include       <stdio.h>
#include       <string.h>
#include       <stdlib.h>
#include       <math.h>
#include       <float.h> 
#include       <stdbool.h>
#include       <stddef.h>                     // Defines NULL
#include       "system_definitions.h"
#include       "system/common/sys_module.h"   // SYS function prototypes
#include       <peripheral/peripheral.h>
#include       <stddef.h>                     // Defines NULL
#include       "app.h"

// Chip-specific
#include       <p32xxxx.h> 
#include       "common_library_32x.h"
#include       "ssd1351_drv.h"
#include       "og_hardware.h"
 
 
//*********************************************************************************************************************
// Convert two characters to one-byte numerical
//
//*********************************************************************************************************************
//

char cGetTupple(char * cBuff, int iOffset)
{
   int                           iVal                       = 0;
   char                          cTemp[3]                   = {0};

   strncpy(cTemp, cBuff + iOffset, 2);
   if (cTemp[0] == '0')
   {
      cTemp[0] = 0x20;
      if (cTemp[1] == '0')
         return 0;
   }
   sscanf(cTemp, "%d", &iVal);
   return (char)iVal;
}

//*********************************************************************************************************************
// Byte checksum routine
// 
//*********************************************************************************************************************
//

unsigned char ucChecksum(char * psString)
{
   unsigned char                 ucCheck                    = 0;    
   int                           i                          = 0;
   
   for (i = 0; i < strlen(psString); i++)
      ucCheck ^= psString[i];   
   return ucCheck;
}   
   
//*********************************************************************************************************************
// Check content of NVRAM and current config, update as necessary
//
//*********************************************************************************************************************
//

void vCheckandUpdateNvRam(void)
{
   unsigned int                  pagebuff[1024]             = {0};
   int                         * iPtr1                      = NULL;
   int                         * iPtr2                      = NULL;
   int                         * iPtr3                      = NULL;
   bool                          bWrite                     = 0;
   int                           i                          = 0;
   int                           j                          = 0;
    
   // Check if current config is valid, if not, prime with NVRAM (if valid) or default
   if(sConfig.uiMagic == NVM_MAGIC)
   {
      ClearWDT();
      // Current config is valid
      // Check if current config matches NVRAM, if not, write current to NVRAM
      if (memcmp((void *)NVM_PROGRAM_PAGE, &sConfig, sizeof(sConfig)))
      {
         // Different            
         NVMErasePage((void *)NVM_PROGRAM_PAGE);     
         if (NVMProgram((void *)(NVM_PROGRAM_PAGE), (const void *)&sConfig, sizeof(sConfig), (void*) pagebuff))
         {
            vDisplayString(0, 2, "NVRAM prog err!");
            vBeep(1);   // kbdoo message
            vLongDelay(2);
         }
      }
   }
   else
   {
      // Current config not valid, check if NVRAM is valid and read, or get defaults
      if(*(int *)NVM_PROGRAM_PAGE == NVM_MAGIC)
      {
         // NVRAM is valid, read it
         iPtr1 = (int *)NVM_PROGRAM_PAGE;
         iPtr2 = (int *)&sConfig;
         i = 0;
         do
         {
            i++;
            *iPtr2 = *iPtr1;
            iPtr1++;
            iPtr2++;
            ClearWDT();
         } while ((iPtr1 - (int *)NVM_PROGRAM_PAGE) < sizeof(sConfig));
         if (sConfig.uiMagic != (unsigned int)NVM_MAGIC)
         {
            vDisplayString(0, 2, "NVRAM read err!");
            vBeep(1);   // kbdoo message
            vLongDelay(2);
         }
    
      }
      else
      {
         // NVRAM not valid, load defaults
          
         // kbdoo
         
         
      }
   }
}
 

//*********************************************************************************************************************
// GetBmpImage
// Reads BMP image from open file handle, return BMP in given buffer. Function returns buffer size needed, also
// if no input buffer is given. Only 16 and 24 bit color depths are supported. Compression is not supported.
//*********************************************************************************************************************
//

// Header structure
#pragma pack(push, 1)
typedef struct _BMHDR
{
   // File Header
   short                         sType;                                             // "BM"
   unsigned long                 ulFsize;                                           // File size
   unsigned long                 ulReserve;
   unsigned long                 ulDataOffset;                                      // Data offset
   // Info Header
   unsigned long                 ulHdrsize;
   unsigned long                 ulHsize;
   unsigned long                 ulVsize;
   unsigned short                usPlanes;
   unsigned short                usBitcount;
   unsigned long                 ulCompression;
   unsigned long                 ulSizeImage;
   unsigned long                 ulXPelsPerMeter;
   unsigned long                 ulYPelsPerMeter;
   unsigned long                 ulClrUsed;
   unsigned long                 ulClrImportant;
} BMHDR;
#pragma pack(pop)
BMHDR sHeader;

BYTE bBuff[54] = {0};

//*********************************************************************************************************************


unsigned long ulGetBmpImage(SYS_FS_HANDLE hBMPFile, uint8_t * pbImage)
{
   unsigned char                 ucBuff[128]                = {0};
   unsigned long                 ulImagesize                = 0;
   unsigned char               * pucBuffer                  = NULL;     // Temporary processing buffer
   int                           i                          = 0;     
   int                           iSrc                       = 0;     
   int                           iDst                       = 0;     
   int                           iLine                      = 0;     
   unsigned char                 ucR                        = 0; 
   unsigned char                 ucG                        = 0; 
   unsigned char                 ucB                        = 0; 
   unsigned char                 ucH                        = 0; 
   unsigned char                 ucL                        = 0; 
   unsigned char                 ucTemp                     = 0; 
   
  
   if (hBMPFile == NULL)
      return RC_INVALID_PARAMETER;
       
   // Set filepointer to beginning
   if (SYS_FS_FileSeek(hBMPFile, 0, SYS_FS_SEEK_SET) == -1) 
      return RC_ERROR;
      
   // Read header
   if (SYS_FS_FileRead(hBMPFile, (void *)&sHeader, sizeof(BMHDR) ) == -1)
      return RC_ERROR;

   ulImagesize = sHeader.ulHsize * sHeader.ulVsize * 2;          // Supports 16 or 24 bit pixel depth, no pallette
   iLine = sHeader.ulVsize - 1;
   
   if ((sHeader.ulCompression) || (sHeader.usBitcount < 16))
      return RC_ERROR;   
   // Return size if needed
   if (!pbImage)        // No buffer, return resulting buffer size needed for raw image
      return (ulImagesize);
   // Read rest of input image
   pucBuffer = (unsigned char *)malloc(sHeader.ulFsize);  // Includes header size, not necessary
   if (!pucBuffer)
      return RC_ERROR;
   if (SYS_FS_FileRead(hBMPFile, (void *)pucBuffer, sHeader.ulFsize - sizeof(BMHDR)) == -1)
   {
      free(pucBuffer);
      return RC_ERROR;
   }   
   
   // Move image into provided buffer, convert as necessary
   if(sHeader.usBitcount == 24)        // Map from 3 to two bytes pr. pixel
   {
      vDebug("Start.\0", D_GREEN);
      for (iLine = sHeader.ulVsize - 1; iLine >= 0; iLine--)
      {
         iDst = iLine * sHeader.ulHsize * 2;
         for (i = 0; i < sHeader.ulHsize; i ++)
         {   
            // Values stored 'backwards'
            ucR = *(pucBuffer + iSrc);
            ucG = *(pucBuffer + iSrc + 1);
            ucB = *(pucBuffer + iSrc + 2);
 
            // Reduce color depth by throwing away LSBs. 
            // R4 R3 R2 R1 R0 G5 G4 G3 G2 G1 G0 B4 B3 B2 B1 B0
            ucTemp = ucG;
            ucH = (ucR & 0xF8);        
            ucH |= (ucTemp >> 5); 
            ucL = (ucB >> 3);
            ucTemp = (ucG << 3);         
            ucTemp &= 0xE0;
            ucL |= ucTemp;

            *(pbImage + iDst + (i*2)) = ucH;
            *(pbImage + iDst + (i*2) + 1) = ucL;                                                                                 
             
            iSrc += 3;
         }   
      }
   }
   free(pucBuffer);   
   return (ulImagesize);
}
 
//*********************************************************************************************************************
// Debug to display 
// 117 is last line, with 2 lines to spare
//*********************************************************************************************************************
//

void vDebug(char * cString, BYTE byColor)
{  
   switch(byColor)
   {
      case D_GREEN:
         by1351_WriteString((char *)cString, 0x00, 0x3f, 0x00);
      break;   
      case D_YELLOW:
         by1351_WriteString((char *)cString, 0x1f, 0x3f, 0x00);
      break;   
      case D_RED:
         by1351_WriteString((char *)cString, 0x1f, 0x00, 0x00);
      break;   
      case D_PURPLE:
         by1351_WriteString((char *)cString, 0x1f, 0x00, 0x1f);
      break;   
      case D_BLUE:
         by1351_WriteString((char *)cString, 0x00, 0x00, 0x1f);
      break;      
   }   
}    
 
//*********************************************************************************************************************
// Beep
//
//*********************************************************************************************************************
//

void vBeep(int iSound)
{
   int         i = 0;
   int         j = 0;
   int         k = 0;
   int         iTemp = 0;
   
   switch (iSound)
   {
      case 0:                             // Error           
         for (i = 0; i < 50; i++)
         {
            SPEAKER = 1;   
            vLongDelay(5);
            SPEAKER = 0;   
            vLongDelay(5);
         }     
      break;
      case 1:                             // Message
         for (i = 0; i < 140; i++)
         {
            SPEAKER = 1;   
        		vShortDelay(36000);
            SPEAKER = 0;   
        		vShortDelay(36000);               
         }
         for (i = 0; i < 135; i++)
         {
            SPEAKER = 1;   
        		vShortDelay(43000);
            SPEAKER = 0;   
        		vShortDelay(43000);               
         }
      break;
      case 2:                             // Alarm
         for (k = 0; k < 5; k++)
         {      
            j = 60;
            for (i = 0; i < 60; i++)
            {
               SPEAKER = 1;   
        		   vShortDelay(i*1000);
               SPEAKER = 0;   
        		   vShortDelay(j*1000);
        		   j--;
        		}
            j = 60;
            for (i = 20; i < 60; i++)
            {
               SPEAKER = 1;   
        		   vShortDelay((i*1000) - 2000);
               SPEAKER = 0;   
        		   vShortDelay((j*1000) - 2000);
        		   j--;
        		}        		        	        		                
         }
      break;
      case 3:                             // Tick
         for (i = 0; i < 55; i++)
         {
            SPEAKER = 1;   
        		vShortDelay(4500);
            SPEAKER = 0;   
        		vShortDelay(4500);               
         }
      break;                                              
      case 4:                             // Dit-dit-dit
         for (k = 0; k < 3; k++)
         {                 
            for (i = 0; i < 60; i++)
            {
               SPEAKER = 1;   
        		   vShortDelay(15500);
               SPEAKER = 0;   
        		   vShortDelay(15500);        		   
        		}
        		vLongDelay(60);          
         }
      break;
       
   }   
}

//*********************************************************************************************************************
// Send RGB value to a WS2812 string
// BBRRGG
//*********************************************************************************************************************
//

void vSetRGBled(unsigned long ulRGB[LED_SERIAL_NUM], unsigned char ucNum, unsigned char ucR, unsigned char ucG, unsigned char ucB)
{
   unsigned char                 i                          = 0;
   unsigned char                 j                          = 0;
   unsigned long                 ulColor                    = 0;
   unsigned long                 ulTemp                     = 0;
   unsigned long                 k                          = 0;
   unsigned long                 l                          = 0;
   
   unsigned char ucTemp[20] = {0}; 
     
   // Address out-of-bounds?
   if (ucNum > LED_SERIAL_NUM)
      return;
   
   // Insert value in array   
   
   for (k = 0; k < 8; k++)
   {   
      ulColor |= (ucB & 0x01);        
      ulColor = ulColor << 1;
      ucB = ucB >> 1;
   }   
 
   for (k = 0; k < 8; k++)
   {
      ulColor |= (ucR & 0x01);
      ulColor = ulColor << 1;
      ucR = ucR >> 1;
   }   

   for (k = 0; k < 8; k++)
   {   
      ulColor |= (ucG & 0x01);
      if (k < 7)
         ulColor = ulColor << 1;
      ucG = ucG >> 1;
   }   

   /* 
   sprintf(ucTemp, "%08x ", ulColor);
   vDebug(ucTemp, 0);
   */
   
   ulRGB[ucNum] = ulColor; 
   
   // Update entire array  
   // Reset
 
   LED_SERIAL = 0;                             
   vShortDelay(6000);
                 
   SYS_INT_Disable();
   
   for (i = 0; i < LED_SERIAL_NUM; i++)                        // Iterate all LEDs in string
   {         
      ulTemp = ulRGB[i];         
      // Clock out 24 bits for one LED
 
      for (j = 0; j < 24; j++)
      {   
         if (ulTemp & 1)   // One
         {              
            LED_SERIAL = 1;
            vShortDelay(72);
            LED_SERIAL = 0;   
            vShortDelay(34);            
         }   
         else              // Zero
         {          
            LED_SERIAL = 1;
            vShortDelay(34);               
            LED_SERIAL = 0;
            vShortDelay(72);
         }    
         ulTemp = ulTemp >> 1;
      }    
   }      
   vShortDelay(6000);
   LED_SERIAL = 1;  
   SYS_INT_Enable();  
}   

//*********************************************************************************************************************
// 
//*********************************************************************************************************************
//

void vClearRGBled(unsigned long ulRGB[LED_SERIAL_NUM], unsigned long ulColor)
{
   unsigned char                 i                          = 0;

   for (i = 0; i < LED_SERIAL_NUM; i++)
      vSetRGBled(ulRGB, i, 0, 0, 0);
}










