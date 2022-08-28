//*********************************************************************************************************************
// ssd1306 driver
// 128x64
//
//*********************************************************************************************************************
//   
//
//*********************************************************************************************************************
//

#ifdef __XC32
    #include <xc.h>                                         // Defines special funciton registers, CP0 regs
#endif



//#include <plib.h>                                         // Include to use PIC32 peripheral libraries
 
#include       "app.h"
#include       "ssd1306_drv.h"

// Font
#include       "ssd1306_vga_font.h"                         // VGA
#include       "ssd1306_alps_font.h"                        // Alps
#include       "ssd1306_ital_font.h"                        // Italics


//*********************************************************************************************************************
//
//
//*********************************************************************************************************************
//

extern APP_DATA                        appData;
extern NVM_CONFIG                      sConfig;

//*********************************************************************************************************************
//  
//  
//*********************************************************************************************************************
//

void I2C_WriteCmdStream(const char * uiStream, unsigned int uiSize)
{
   appData.uiI2CTxbuffer[0] = 0x00;       // Multiple commands
   memcpy(appData.uiI2CTxbuffer + 1, uiStream, uiSize);
   vI2C_Write(DEV_DISPLAY, uiSize + 1);
}

//*********************************************************************************************************************
//  
//  
//*********************************************************************************************************************
//

void I2C_WriteDataStream(uint8_t * uiStream, unsigned int uiSize)
{
   appData.uiI2CTxbuffer[0] = 0x40;       // Multiple data
   memcpy(appData.uiI2CTxbuffer + 1, uiStream, uiSize);
   vI2C_Write(DEV_DISPLAY, uiSize + 1);
}

//*********************************************************************************************************************
//  
//  
//*********************************************************************************************************************
//

void I2C_WriteCommand(uint8_t ucByte)
{
   appData.uiI2CTxbuffer[0] = 0x80;       // Single command
   appData.uiI2CTxbuffer[1] = ucByte;     //     
   vI2C_Write(DEV_DISPLAY, 2);
}
 
//*********************************************************************************************************************
//  
//  
//*********************************************************************************************************************
//

void I2C_WriteData(uint8_t ucByte)
{
   appData.uiI2CTxbuffer[0] = 0xC0;       // Single data
   appData.uiI2CTxbuffer[1] = ucByte;     //     
   vI2C_Write(DEV_DISPLAY, 2);
}

//*********************************************************************************************************************
// Clear panel.
//
//*********************************************************************************************************************
//

uint8_t by1306_Clear(void)
{
   uint8_t                       bHome[]                    = {0x21, 0x00, 0x7f};
   uint8_t                       bHome2[]                   = {0x22, 0x00, 0x07};
   uint8_t                       bBuff[1024]                = {0};
   
   if (appData.ucDisplayPresent)
   {   
      I2C_WriteCmdStream(bHome, sizeof(bHome));
      I2C_WriteCmdStream(bHome2, sizeof(bHome2));
      I2C_WriteDataStream(bBuff, 1024); 
   }
   
   return RC_OK;
}
//*********************************************************************************************************************
// Reset panel area, home cursor.
//
//*********************************************************************************************************************
//

uint8_t by1306_Reset(void)
{
   uint8_t                       bHome[]                    = {0x21, 0x00, 0x7f};
   uint8_t                       bHome2[]                   = {0x22, 0x00, 0x07};
   
   if (appData.ucDisplayPresent)
   {   
      I2C_WriteCmdStream(bHome, sizeof(bHome));
      I2C_WriteCmdStream(bHome2, sizeof(bHome2));
   }
   
   return RC_OK;
}

//*********************************************************************************************************************
// Initialize SSD1306 chip. 
// Thank you to Alan Hawse, IOTexpert.com
//*********************************************************************************************************************
//

uint8_t by1306_Initialize(void)
{
   uint8_t                       ucBuff[250];                                   
   uint8_t                       i;
         
   const char initializeCmds[] =
   {
      //////// Fundamental Commands
      OLED_DISPLAYOFF,          // 0xAE Screen Off
      OLED_SETCONTRAST,         // 0x81 Set contrast control
      0x1F,                     // 0-FF ... default half way

      OLED_DISPLAYNORMAL,       // 0xA6, //Set normal display 

      //////// Scrolling Commands
      OLED_DEACTIVATE_SCROLL,   // Deactive scroll

      //////// Addressing Commands
      OLED_SETMEMORYMODE,       // 0x20, //Set memory address mode
      OLED_SETMEMORYMODE_HORIZONTAL,  // Page

      //////// Hardware Configuration Commands
      OLED_SEGREMAPINV,         // 0xA1, //Set segment re-map 
      OLED_SETMULTIPLEX,        // 0xA8 Set multiplex ratio
      0x3F,                     // Vertical Size - 1
      OLED_COMSCANDEC,          // 0xC0 Set COM output scan direction
      OLED_SETDISPLAYOFFSET,    // 0xD3 Set Display Offset
      0x00,                     //
      OLED_SETCOMPINS,          // 0xDA Set COM pins hardware configuration
      0x12,                     // Alternate com config & disable com left/right

      //////// Timing and Driving Settings
      OLED_SETDISPLAYCLOCKDIV,  // 0xD5 Set display oscillator frequency 0-0xF /clock divide ratio 0-0xF
      0x80,                     // Default value
      OLED_SETPRECHARGE,        // 0xD9 Set pre-changed period
      0x22,                     // Default 0x22
      OLED_SETVCOMDESELECT,     // 0xDB, //Set VCOMH Deselected level
      0x20,                     // Default 

      //////// Charge pump regulator
      OLED_CHARGEPUMP,          // 0x8D Set charge pump
      OLED_CHARGEPUMP_ON,       // 0x14 VCC generated by internal DC/DC circuit

      // Turn the screen back on...       
      OLED_DISPLAYALLONRESUME,  // 0xA4, //Set entire display on/off
      OLED_DISPLAYON,           // 0xAF  //Set display on
   };
   
   I2C_WriteCmdStream(initializeCmds, sizeof(initializeCmds));
  
   return RC_OK;
}

//*********************************************************************************************************************
// Set panel contrast, value 0-255.
//
//*********************************************************************************************************************
//

uint8_t by1306_SetContrast(uint8_t byContrast)
{
   if (appData.ucDisplayPresent)
   {   
      I2C_WriteCmd(0x81);                                        
      I2C_WriteCmd(byContrast);
   }
   
   return RC_OK;
}

//*********************************************************************************************************************
// Draw a pixel
// Arguments:
//  byX              : X position
//  byY              : Y position
//  byR, byG, byB    : RGB color
//  byAttrib         : Not used
//
//*********************************************************************************************************************
//

uint8_t by1306_DrawPixel(uint8_t byX, uint8_t byY, uint8_t byR, uint8_t byG, uint8_t byB, uint8_t byAttrib)
{
   if (appData.ucDisplayPresent)
   {   

      // Set column
      vCommand(0x15);
      vData(byX);
      vData(0x7F);
      // Set row
      vCommand(0x75);
      vData(byY);
      vData(0x7F);
      // Write to RAM
      vCommand(0x5C);
      by1306_WriteColors(byR, byG, byB);
   }
   
   return RC_OK;
}

//*********************************************************************************************************************
// Draw a line
// Generic Bresenham line implementation, or straight H or V loops
// Arguments:
//  byX1             : X position left
//  byY1             : Y position upper
//  byX2             : X position right
//  byY2             : Y position lower
//  byR, byG, byB    : RGB color
//  byAttrib         : Not used
//
//*********************************************************************************************************************
//

uint8_t by1306_DrawLine(uint8_t byX1, uint8_t byY1, uint8_t byX2, uint8_t byY2, uint8_t byR, uint8_t byG, uint8_t byB, uint8_t byAttrib)
{
   int                           iDx                        = 0;
   int                           iSx                        = 0;
   int                           iDy                        = 0;
   int                           iSy                        = 0;
   int                           err                        = 0;
   int                           iE2                        = 0;

   if (!appData.ucDisplayPresent)
      return RC_OK;

   if ((byX1 > 127) || (byX2 > 127) || (byY1 > 127) || (byY2 > 127))
      return RC_INVALID_PARAMETER;

   if (byX1 == byX2)             // Vertical line
   {
      // Set vertical scan
      vCommand(0xA0);
      vData(0x71);

      // Set coloumn
      vCommand(0x15);
      vData(byX1);
      vData(byX1);
      // Set row
      vCommand(0x75);
      vData(byY1);
      vData(byY2);
      // Write buffer data
      vCommand(0x5C);
      for (iSx = 0; iSx < (byY2 - byY1); iSx++)
         by1306_WriteColors(byR, byG, byB);

      // Reset scan
      vCommand(0xA0);
      vData(0x70); 
   }
   else if (byY1 == byY2)        // Horizontal line
   {
      // Set column
      vCommand(0x15);
      vData(byX1);
      vData(byX2);
      // Set row
      vCommand(0x75);
      vData(byY1);
      vData(byY2);
      vCommand(0x5C);

      // Fetch color bytes once only, for speed
      iSy = ui1351_GetColor(byR, byG, byB);
      for (iSx = byX1; iSx <= byX2; iSx++)
      {
         vData((uint8_t)((iSy & 0xFF00) >> 8));
         vData((uint8_t)(iSy & 0x00FF));
      }
   }
   else                          // Slanted - Bresenham
   {
      iDx = abs(byX2 - byX1);
      iSx = byX1 < byX2 ? 1 : -1;
      iDy = abs(byY2 - byY1);
      iSy = byY1 < byY2 ? 1 : -1;
      err = (iDx > iDy ? iDx : -iDy)/2;
      
      for(;;)
      {
         by1306_DrawPixel(byX1, byY1, byR, byG, byB, byAttrib);
         if (byX1 == byX2 && byY1 == byY2)
            break;
         iE2 = err;
         if (iE2 >-iDx)
         {
            err -= iDy;
            byX1 += iSx;
         }
         if (iE2 < iDy)
         {
            err += iDx;
            byY1 += iSy;
         }
      }
   }
   return RC_OK;
}

//*********************************************************************************************************************
// Squares, filles and outlined
// Arguments:
//  byX1             : X position left
//  byY1             : Y position upper
//  byX2             : X position right
//  byY2             : Y position lower
//  byR, byG, byB    : RGB color
//  byAttrib:
//    ATTR_FILLED    : Filled shape   
//
//*********************************************************************************************************************
//

uint8_t by1306_DrawSquare(uint8_t byX1, uint8_t byY1, uint8_t byX2, uint8_t byY2, uint8_t byR, uint8_t byG, uint8_t byB, uint8_t byAttrib)
{
   int                           i                          = 0;
   int                           j                          = 0;

   if (!appData.ucDisplayPresent)
      return RC_OK;

   if ((byX1 > 127) || (byX2 > 127) || (byY1 > 127) || (byY2 > 127))
      return RC_INVALID_PARAMETER;
   if ((byX1 > byX2) || (byY1 > byY2))
      return RC_INVALID_PARAMETER;

   if (byAttrib & ATTR_FILLED)            // Filled
   {
      // Set coloumn
      vCommand(0x15);
      vData(byX1);
      vData(byX2);
      // Set row
      vCommand(0x75);
      vData(byY1);
      vData(byY2);
      // Write to RAM
      i = (byX2 - byX1) * (byY2 - byY1);
      vCommand(0x5C);
      for (j = 0; j < i; j++)
         by1306_WriteColors(byR, byG, byB);
   }
   else                                   // Outlinesd
   {
      by1306_DrawLine(byX1, byY1, byX1, byY2, byR, byG, byB, byAttrib);
      by1306_DrawLine(byX1, byY1, byX2, byY1, byR, byG, byB, byAttrib);
      by1306_DrawLine(byX2, byY1, byX2, byY2, byR, byG, byB, byAttrib);
      by1306_DrawLine(byX1, byY2, byX2, byY2, byR, byG, byB, byAttrib);
   } 
   return RC_OK;
}

//*********************************************************************************************************************
// Circle
// Generic Bresenham line implementation
// Arguments:
//  byX              : X position
//  byY              : Y position
//  byRadius         : Radius
//  byR, byG, byB    : RGB color
//  byAttrib:
//    ATTR_FILLED    : Filled shape   
//
//*********************************************************************************************************************
//

uint8_t by1306_DrawCircle(uint8_t byX, uint8_t byY, uint8_t byRadius, uint8_t byR, uint8_t byG, uint8_t byB, uint8_t byAttrib)
{
   int                           x                          = 0;
   int                           y                          = 0;
   int                           iToggle                    = 0;

   if (!appData.ucDisplayPresent)
      return RC_OK;
   
   x = byRadius;
   y = 0;
   iToggle = 1 - x;

   while(x >= y)
   {
      if (byAttrib & ATTR_FILLED)
      {
         by1306_DrawLine(-y + byX, -x + byY, y + byX, -x + byY, byR, byG, byB, byAttrib);
         by1306_DrawLine( -x + byX, -y + byY, x + byX, -y + byY, byR, byG, byB, byAttrib);
         by1306_DrawLine(-x + byX, y + byY, x + byX, y + byY, byR, byG, byB, byAttrib);
         by1306_DrawLine(-y + byX, x + byY,y + byX, x + byY, byR, byG, byB, byAttrib);
      }
      else
      {
         by1306_DrawPixel(y + byX, -x + byY, byR, byG, byB, byAttrib);                  // 1
         by1306_DrawPixel(-y + byX, -x + byY, byR, byG, byB, byAttrib);                 // 8
         by1306_DrawPixel(x + byX, -y + byY, byR, byG, byB, byAttrib);                  // 2
         by1306_DrawPixel(-x + byX, -y + byY, byR, byG, byB, byAttrib);                 // 7
         by1306_DrawPixel(x + byX, y + byY, byR, byG, byB, byAttrib);                   // 3
         by1306_DrawPixel(-x + byX, y + byY, byR, byG, byB, byAttrib);                  // 6
         by1306_DrawPixel(y + byX, x + byY, byR, byG, byB, byAttrib);                   // 4
         by1306_DrawPixel(-y + byX, x + byY, byR, byG, byB, byAttrib);                  // 5
      }
      y++;
      if (iToggle <= 0)
         iToggle += 2 * y + 1;
      else
      {
         x--;
         iToggle += 2 * (y - x) + 1;
      }
   }

   return RC_OK;
}

//*********************************************************************************************************************
// Draw text string.
// Arguments:
//  byX              : X position (upper left of character)
//  byY              : Page (line) 0-7
//  pcString         : Zero terminated string
//  byFont:          : Font name define
//
//*********************************************************************************************************************
//

uint8_t by1306_DrawText(uint8_t byX, uint8_t byY, char * pcString, uint8_t byFont)
{
   int                           iLen                       = 0;
   int                           i                          = 0;
   int                           j                          = 0;   
   int                           byWidth                    = 8;           // Character font width
   uint8_t                      *pFont                      = NULL;
   uint8_t                      *pP                         = NULL;
   
   if (!appData.ucDisplayPresent)
      return RC_OK;   
   
   if ((byX > 0x7F) || (byY > 0x07))
      return RC_INVALID_PARAMETER;     
   
   // Set font pointer
   if (byFont == FONT_VGA)
      pFont = font_vga;
   else if (byFont == FONT_ALPS)
      pFont = font_alps;
   else if (byFont == FONT_ITAL)
      pFont = font_ital;
   else
      return RC_INVALID_PARAMETER;

   iLen = strlen(pcString);
   
   // Set column
   I2C_WriteCommand(0x21);
   I2C_WriteCommand(byX);
   I2C_WriteCommand(0x7f);
   // Set page
   I2C_WriteCommand(0x22);
   I2C_WriteCommand(byY);
   I2C_WriteCommand(0x07);
   
   // Traverse string
   for (i = 0; i < iLen; i++) 
   {      
      // Set pointer to desired font character position
      pP = pFont + (pcString[i] * 8);
      
      //Traverse 8 bytes of character data
      for (j = 0; j < 8; j++)
      {         
         I2C_WriteData(*pP);                  
         pP++;
      }    
   }   
   return RC_OK;
}

//*********************************************************************************************************************
//
//
//*********************************************************************************************************************
//
 