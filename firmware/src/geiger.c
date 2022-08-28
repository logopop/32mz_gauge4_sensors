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
#include "geiger.h"

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
   
void vGeigerCalculate()
{
   uint8_t                       i                          = 0;
   int                           iTemp                      = 0;  
   double                        fCalibrated                = 0.0;
      
   // Last 5 seconds
   appData.iCounts5 = appData.iCountArray[appData.uiGeigerPtr];
   
   // Last minute
   appData.iCountsMin = 0;
   if (appData.uiArrayFull || appData.uiGeigerPtr > 10)  // CPM
   {        
      if (appData.uiGeigerPtr > 10)
      {   
         for (i = (appData.uiGeigerPtr - 11); i <= appData.uiGeigerPtr; i++)
            appData.iCountsMin += appData.iCountArray[i];
      }
      else // Crossing boundary
      {
         for (i = 0; i <= appData.uiGeigerPtr; i++)
            appData.iCountsMin += appData.iCountArray[i];
         for (i = (GEIGER_ARRAYSIZE - (12 - appData.uiGeigerPtr)); i < GEIGER_ARRAYSIZE; i++)
            appData.iCountsMin += appData.iCountArray[i];                  
      }      
   }
   else   // Extrapolate CPM based on what we have
   {   
      appData.iCountsMin = 0;
      for (i = 0; i <= appData.uiGeigerPtr; i++)
         appData.iCountsMin += appData.iCountArray[i];
      if (appData.uiGeigerPtr > 11)
         appData.iCountsMin = (int)(appData.iCountsMin * (12.0 / (float)appData.uiGeigerPtr)); 
      else if (appData.uiGeigerPtr < 11)
         appData.iCountsMin = (int)(appData.iCountsMin * ((float)appData.uiGeigerPtr / 12.0)); 
   }   
      
   // Average CPM last 10 minutes
   if (appData.uiArrayFull)
   {   
      iTemp = 0;      
      for (i = 0; i < GEIGER_ARRAYSIZE; i++)
         iTemp += appData.iCountArray[i];
      appData.iAve10 = iTemp / 10;
      appData.fRad10 = (appData.iAve10 * (SIEVERT_RATE + fCalibrated));      
   }
   
   // MicroSievert
   fCalibrated = sConfig.iGeigerCal / 1000;
   appData.fRadLevel = (appData.iCountsMin * (SIEVERT_RATE + fCalibrated));      
}

//*********************************************************************************************************************
//  
//  
//*********************************************************************************************************************
//
   
void APP_GeigerTimerHandler(SYS_MODULE_INDEX index)    // Timer 4
{
   uint8_t ucBuff[17];
   
   appData.iCountArray[appData.uiGeigerPtr] = appData.iCounts;
   vGeigerCalculate(); 
   appData.uiGeigerPtr++;
   if (appData.uiGeigerPtr >= GEIGER_ARRAYSIZE)
   {   
      appData.uiGeigerPtr = 0;
      appData.uiArrayFull = 1;
   }
   appData.iCounts = 0;  
  
   appData.cNewGeigermsg = 1;                // Send message      
   
   // Debug
   sprintf(ucBuff, "G:%d %.1f  \0", appData.iCounts5, appData.fRadLevel);
   strncpy(appData.uiDebugbuffer4, ucBuff, 15);         
}   
   
//*********************************************************************************************************************
// Incoming detector pulse
// 
//*********************************************************************************************************************
//

void vGeigerReceiveEventHandler(void)   
{ 

   // Random
   appData.iRandomCnt = appData.ulLife ^ (appData.iCountArray[3] + appData.iCountArray[4] + appData.iCountArray[5]);
   appData.iRandomCnt ^= (appData.iX * appData.iY);

   // ***
   appData.iCounts++;
   
   /*
   if (sConfig.cConfig1 & CFG_GEIGER)
   {   
      SPEAKER = 0;
      vLongDelay(1);
      SPEAKER = 1;
   } 
   */
    
   if ((appData.uiDisplayScreen == SCREEN_GEIGER) && (sConfig.cConfig1 & CFG_GEIGER))     // Send tick message to VDU if on Geiger screen and on
      appData.uiTicks++;

   // Turn on indicator, set time delay
   LED_RED = LED_ON;
   appData.uiRledcount = appData.ulLife;   
}

//*********************************************************************************************************************
//  
//  
//*********************************************************************************************************************
//

void vGeiger_Initialize(void)
{
   uint8_t                       i                          = 0;
   
   // Initialize timer, array
   PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_TIMER_4);
   DRV_TMR0_Start(); // Start 
   appData.iCounts = 0;
   for (i = 0; i < GEIGER_ARRAYSIZE; i++)
      appData.iCountArray[i] = 0;
   
   appData.uiGeigerState = 0;  
   appData.iCounts5 = 0;  
   appData.iAve10 = 0;  
   appData.fRadLevel = 0;  
   appData.uiArrayFull = 0;
   appData.uiGeigerPtr = 0;      
}

//*********************************************************************************************************************
//  
//  
//*********************************************************************************************************************
//

void vSendGeigerMessage()
{  
   GEIGER_MSG sGeigerMsg;
   
   sGeigerMsg.iCounts = appData.iCounts;
   sGeigerMsg.iCounts5 = appData.iCounts5;
   sGeigerMsg.iCountsMin = appData.iCountsMin;
   sGeigerMsg.iAve10 = appData.iAve10;
   sGeigerMsg.fRadLevel = appData.fRadLevel;
   sGeigerMsg.fRad10 = appData.fRad10;

   vCanSendBuffer(&sGeigerMsg, sizeof(sGeigerMsg), CID_GEIGER_H);
}
         
//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//

void vGeiger_State()
{
      
	switch (appData.uiGpsState)
   {
      case 0:        // Start
         appData.uiGeigerState = 1;         
      break;
      case 1:                                    
         appData.uiGeigerState = 2;  
      break;                         
      case 2: 
         appData.uiGeigerState = 3;  
      break;           
      case 3: 
         appData.uiGeigerState = 4;  
      break;           
      case 4:     
         // Send geiger message
         if (appData.cNewGeigermsg)
         {            
            appData.cNewGeigermsg = 0;
            vSendGeigerMessage();
         }
         appData.uiGeigerState = 5;
      break;
      case 5: 
         appData.uiGeigerState = 6;  
      break;           
      case 6: 
         appData.uiGeigerState = 7;  
      break;           
      case 7: 
         appData.uiGeigerState = 8;  
      break;           
      case 8: 
         // Check for level alarm on timer startup
         if ((appData.ulLonglife > 60) && (appData.fRadLevel > 200) && (appData.uiStartupflag & STARTUP_TIMER))     
         {                       
            vSendSmsAlarm("RADIATION alarm!");
           
         }                  
         appData.uiGeigerState = 9;  
      break;           
      case 9: 
         appData.uiGeigerState = 10;  
      break;           
      case 10: 
         appData.uiGeigerState = 11;  
      break;           
      case 11: 
         appData.uiGeigerState = 12;  
      break;           
      case 12: 
         appData.uiGeigerState = 1;  
      break;           

          
   }   
}   














