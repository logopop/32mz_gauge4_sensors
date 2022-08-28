//*********************************************************************************************************************
//  
//  
//
//*********************************************************************************************************************
//
//  
//      
//
//
//*********************************************************************************************************************
//
   
#include "app.h" 
#include "gyro.h"


//*********************************************************************************************************************
// Globals
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
   
void APP_GyroReceiveEventHandler(SYS_MODULE_INDEX index)    // SPI 3
{
   unsigned char ucTemp[20] = {0}; 
   unsigned char ucChar;
        
}   

//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//

uint8_t rcSpiReadwrite(uint8_t * uiSend, uint8_t * uiRec)
{
   uint8_t                       rcRc                       = RC_OK;
   DRV_SPI_BUFFER_HANDLE         bufferHandle;
   DRV_SPI_BUFFER_HANDLE         bufferHandle2; 
   
   memcpy(appData.ucBuff, uiSend, 2);
   
   GYRO_CE = 0;       
   //vShortDelay(300);
   bufferHandle = DRV_SPI_BufferAddWriteRead2(appData.drvGyroHandle, appData.ucBuff, 2, appData.ucBuff2, 2, NULL, NULL, &bufferHandle2);    
   if (bufferHandle2 != DRV_SPI_BUFFER_HANDLE_INVALID) 
   {   
      while (!(DRV_SPI_BUFFER_EVENT_COMPLETE & DRV_SPI_BufferStatus(bufferHandle2)))       
      {                  
      
      }
      memcpy(uiRec, appData.ucBuff2, 2); 
   }
   else
      rcRc = RC_ERROR;
   GYRO_CE = 1;  
   vShortDelay(100); 
   
   return rcRc; 
}

//*********************************************************************************************************************
//  Init SPI #4
//  
//*********************************************************************************************************************
//

void vGyro_Initialize(void)
{                                
   DRV_SPI_BUFFER_HANDLE         bufferHandle;
   DRV_SPI_BUFFER_HANDLE         bufferHandle2;   
   uint8_t                       ucBuff[2];
   
   appData.uiGyroUp = 0x00;
   appData.uiGyroptr = 0;
   
   // Disable chip
   GYRO_CE = 1;  
   
   appData.drvGyroHandle = DRV_SPI_Open(DRV_SPI_INDEX_0, DRV_IO_INTENT_READWRITE);         // Gyro
   if(DRV_HANDLE_INVALID == appData.drvGyroHandle) 
   {
      by1306_DrawText(0, 0, "Inval Gyro!\0", FONT_ALPS);
      while (1);  // kbdoo send message
   }
   
   // Init registers
   rcSpiReadwrite("\x6B\x86", ucBuff);    // PWR_MGMT_1
   rcSpiReadwrite("\x6A\x10", ucBuff);    // USER_CTRL
   rcSpiReadwrite("\x6C\x00", ucBuff);    // PWR_MGMT_2
   rcSpiReadwrite("\x68\x03", ucBuff);    // SIGNAL_PATH_RESET
   rcSpiReadwrite("\x1B\x03", ucBuff);    // GYRO_CONFIG
   rcSpiReadwrite("\x1E\x00", ucBuff);    // LP_ACCEL_ODR
   rcSpiReadwrite("\x19\x02", ucBuff);    // SMPLRT_DIV
   rcSpiReadwrite("\x1A\x00", ucBuff);    // CONFIG
   rcSpiReadwrite("\x1B\x00", ucBuff);    // GYRO_CONFIG
   rcSpiReadwrite("\x1C\x00", ucBuff);    // ACCEL_CONFIG
   rcSpiReadwrite("\x1D\x05", ucBuff);    // ACCEL_CONFIG2

   // Reset offsets
   //rcSpiReadwrite("\x77\x00", ucBuff);    // 
   //rcSpiReadwrite("\x78\x00", ucBuff);    // 
   //rcSpiReadwrite("\x7A\x00", ucBuff);    // 
   //rcSpiReadwrite("\x7B\x00", ucBuff);    // 
   
}

//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//

void vGyro_State(void)
{         
   uint8_t                       rcRc                       = RC_OK; 
   uint8_t                       i                          = 0; 
   DRV_SPI_BUFFER_HANDLE         bufferHandle;
   DRV_SPI_BUFFER_HANDLE         bufferHandle2; 
   uint8_t                       uiBuffIn[2]                = {0};
   uint8_t                       uiBuffOut[2]               = {0};
   uint8_t                       uiTemp[2]                  = {0};
   uint8_t                       uiConv[2]                  = {0};
   uint8_t                       ucCbuff[8]                 = {0};   
   long                          iTemp                      = 0;
   uint8_t                       t1                         = 0; 
   uint8_t                       t2                         = 0; 
   uint8_t                       ucTemp[40]                 = {0};
  
	switch (appData.uiGyroState)
   {
      case 0:        // Start          
         appData.uiGyroState = 1;  
      break;
      case 1:        // ID device
         uiBuffIn[0] = (0x75 | 0x80);
         uiBuffIn[1] = 0x00;
         rcRc = rcSpiReadwrite(uiBuffIn, uiBuffOut);
         if (rcRc == RC_OK)
         {
            if (uiBuffOut[1] == 0x70)
               appData.uiGyroUp = 0x01;
            else
               appData.uiGyroUp = 0x00;
         }   
         else
            appData.uiGyroUp = 0x00;
         appData.uiGyroState = 2;
      break;      
      case 2:
         // Get temperature
         uiBuffIn[0] = (0x41 | 0x80);
         uiBuffIn[1] = 0x00;
         rcRc = rcSpiReadwrite(uiBuffIn, uiBuffOut);
         if (rcRc == RC_OK)
            uiConv[0] = uiBuffOut[1];
         uiBuffIn[0] = (0x42 | 0x80);
         uiBuffIn[1] = 0x00;
         rcRc = rcSpiReadwrite(uiBuffIn, uiBuffOut);
         if (rcRc == RC_OK)
            uiConv[1] = uiBuffOut[1];
         appData.iGyroTemp = uiConv[1] | uiConv[0] << 8;         
         appData.uiGyroState = 3;
      break;
      case 3:         
         // Get X-axis
         uiBuffIn[0] = (0x3b | 0x80);
         uiBuffIn[1] = 0x00;
         rcRc = rcSpiReadwrite(uiBuffIn, uiBuffOut);
         if (rcRc == RC_OK)
            uiConv[0] = uiBuffOut[1];
         uiBuffIn[0] = (0x3c | 0x80);
         uiBuffIn[1] = 0x00;
         rcRc = rcSpiReadwrite(uiBuffIn, uiBuffOut);
         if (rcRc == RC_OK)
            uiConv[1] = uiBuffOut[1];
         appData.iXacc[appData.uiGyroptr] = uiConv[1] | uiConv[0] << 8;    
                  
         iTemp = 0;
         for (i = 0; i < 10; i++)
            iTemp += appData.iXacc[i];
         appData.iX = (iTemp / 10);// + appData.iXoff;
              
         // Get Y-axis
         uiBuffIn[0] = (0x3d | 0x80);
         uiBuffIn[1] = 0x00;
         rcRc = rcSpiReadwrite(uiBuffIn, uiBuffOut);
         if (rcRc == RC_OK)
            uiConv[0] = uiBuffOut[1];
          
         uiBuffIn[0] = (0x3e | 0x80);
         uiBuffIn[1] = 0x00;
         rcRc = rcSpiReadwrite(uiBuffIn, uiBuffOut);
         if (rcRc == RC_OK)
            uiConv[1] = uiBuffOut[1];
 
         appData.iYacc[appData.uiGyroptr] = uiConv[1] | uiConv[0] << 8;
         
         iTemp = 0;
         for (i = 0; i < 10; i++)
            iTemp += appData.iYacc[i];
         appData.iY = (iTemp / 10);// + appData.iYoff;
         
         if (appData.uiGyroptr == 9)
            appData.uiGyroState = 2;      // Get temp             
         else
            appData.uiGyroState = 3;
         
         sprintf(ucTemp, "Gyro %d %d    ", appData.iX, appData.iY);
         memcpy(appData.uiDebugbuffer1, ucTemp, 17);
         appData.uiDebugbuffer1[16] = 0;
         memcpy(ucCbuff, &appData.iY, 2);       // iX
         memcpy(ucCbuff + 2, &appData.iX, 2);   // iY
         vCanTransmit(CID_GYRO, ucCbuff, 4);
         
         appData.uiGyroptr++;
         if (appData.uiGyroptr > 9)
            appData.uiGyroptr = 0;
      break;
      case 5:  // Not used
         // Get Z-axis
         uiBuffIn[0] = (0x3f | 0x80);
         uiBuffIn[1] = 0x00;
         rcRc = rcSpiReadwrite(uiBuffIn, uiBuffOut);
         if (rcRc == RC_OK)
            uiConv[0] = uiBuffOut[1];
         uiBuffIn[0] = (0x40 | 0x80);
         uiBuffIn[1] = 0x00;
         rcRc = rcSpiReadwrite(uiBuffIn, uiBuffOut);
         if (rcRc == RC_OK)
            uiConv[1] = uiBuffOut[1];
         appData.iZ = uiConv[1] | uiConv[0] << 8;                  
         appData.uiGyroState = 2;         
      break;   
      case 6:   
	
         // Done
         
      break;   
	}
	 
}

//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//
















