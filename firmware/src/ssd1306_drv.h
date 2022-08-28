//*********************************************************************************************************************
// ssd1306 driver
//
//
//*********************************************************************************************************************
//  PIC32MZ
//  128x64 OLED - SSD1306 Controller
//
//*********************************************************************************************************************
//
//  Color depth: 
//    R: Max 0x1F
//    G: Max 0x3F
//    B: Max 0x1F
//
//*********************************************************************************************************************
//

/*
#ifndef SSD1306_DRV_H
   #define	SSD1306_DRV_H
#endif
*/

//*********************************************************************************************************************
//  Argument defines
//
//*********************************************************************************************************************
//

// Attributes
#define                          ATTR_NONE                  0b0000000000000000
#define                          ATTR_FILLED                0b0000000000000001

// Fonts and sizes
#define                          FONT_VGA                   0x00
#define                          FONT_ALPS                  0x01
#define                          FONT_ITAL                  0x02

// Returns
#define                          RC_OK                      0x00
#define                          RC_ERROR                   0x01
#define                          RC_INVALID_PARAMETER       0x02


//*********************************************************************************************************************
//  Prototypes
//
//*********************************************************************************************************************
//

void vCommand(uint8_t);
void vData(uint8_t);
uint8_t cDataRead(void); 
uint8_t by1351_Reset(void);
uint8_t by1306_Initialize(void);
uint8_t by1351_WriteSprite(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t *);
uint8_t by1306_Clear(void);
uint8_t by1306_Reset(void);
uint8_t by1306_SetContrast(uint8_t);
uint8_t by1351_DrawText(uint8_t, uint8_t, uint8_t *, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
uint8_t by1351_WriteFrame(uint8_t *, unsigned int);
uint8_t by1351_Scroll(uint8_t);
unsigned int ui1351_GetColor(uint8_t, uint8_t, uint8_t);
uint8_t by1351_WriteColors(uint8_t, uint8_t, uint8_t);
uint8_t by1351_DrawPixel(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
uint8_t by1351_DrawLine(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
uint8_t by1351_DrawRectangle(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
uint8_t by1351_DrawCircle(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
uint8_t by1351_FadeIn(uint8_t);
uint8_t by1351_FadeOut(uint8_t);
uint8_t by1351_Gradient(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
uint8_t by1351_WriteString(uint8_t *, uint8_t, uint8_t, uint8_t);        
uint8_t by1351_DumpText(uint8_t *);
uint8_t cDataRead(void);



