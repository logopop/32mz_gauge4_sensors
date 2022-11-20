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
#include "nvram.h"
 

//*********************************************************************************************************************
// Globals
// 
//*********************************************************************************************************************
//

extern APP_DATA                        appData;
extern NVM_CONFIG                      sConfig;

//*********************************************************************************************************************
// Check content of NVRAM and current config, update as necessary
//
//*********************************************************************************************************************
//

void vNvram_State(void)
{
   unsigned char                 ucDebug[128]               = {0};
   unsigned char                 ucStatus[255]              = {0};
   DRV_NVM_COMMAND_STATUS        dCommandStatus;
   
   switch (appData.uiNvramState)
   {
      case 0:                          // Check if config object is valid, initialize with defaults if not
         if(*(int *)NVM_ADDRESS != NVM_MAGIC)
         {   
            //Prime config struct with default values
            sConfig.uiMagic = NVM_MAGIC;
            memcpy(sConfig.bCallsign, APRS_CALLSIGN, 8);
            memcpy(sConfig.bCall_ext, APRS_CALLSIGN_EXT, 4);
            memcpy(sConfig.bAprsSym, APRS_SYMBOL, 4);
            memcpy(sConfig.bAprsPath, APRS_PATH, 12);
            sConfig.bPower = APRS_POWER;
            sConfig.bPeriod = APRS_PERIOD;
            memcpy(sConfig.bTXfreq, APRS_TXFREQ, 8);
            memcpy(sConfig.bRXfreq, APRS_RXFREQ, 8);
            memcpy(sConfig.bBTname, BT_NAME, 16);
            memcpy(sConfig.bBTpin, BT_PIN, 4);

            memcpy(sConfig.bGsmPrimary, GSM_PRIMARY, 20);
            memcpy(sConfig.bGsmService, GSM_SERVICE, 20);

            memcpy(sConfig.bDnsServer, WIFI_DNS, 16);
            memcpy(sConfig.bCloudServer, WIFI_CLOUD, 40);

            sConfig.iXoff = 0;
            sConfig.iYoff = 0;  
            sConfig.cGyroThreshold = 0;
            sConfig.fBatteryLimit = BATT_LIMIT;
         }   
         else                          // Read config object from NVram
         {
            //vDebug("Valid, read!\0", D_GREEN, 1);   
            memcpy(&sConfig, (void *)NVM_ADDRESS, sizeof(sConfig));                                                          
         }   
         appData.uiNvramState = 10;
      break; 
      case 10:                         // Check if config object differs from content of NVram, write if not                     
         if (memcmp((void *)NVM_ADDRESS, &sConfig, sizeof(sConfig)))
         {             
            //vDebug("NVR differ!\0", D_RED, 1);   
            // Open NVR handle
            appData.hNvram = DRV_NVM_Open(0, DRV_IO_INTENT_READWRITE);         
            if (appData.hNvram)                                                
               appData.uiNvramState = 20;
            //else
              // vDebug("OpvDebugen fail!\0", D_RED, 1);  // kbdoo message
         } 
         else
            appData.uiNvramState = 80;
         
      break;                               
      case 20:                         // Erase NVram                     
         DRV_NVM_Erase(appData.hNvram, &appData.hNvmCmdHandle, 0, 1);
         
         if (appData.hNvmCmdHandle == DRV_NVM_COMMAND_HANDLE_INVALID)
         {   
            //vDebug("Erase fail!\0", D_RED, 1);  // kbdoo message                     
         }   
         else
            appData.uiNvramState = 30;
      break;                               
      case 30:                         // Get status                   
         dCommandStatus = DRV_NVM_CommandStatus(appData.hNvram, appData.hNvmCmdHandle);
         if (DRV_NVM_COMMAND_COMPLETED == dCommandStatus)
            appData.uiNvramState = 40;
      break;                               
      case 40:                         // Write config
         DRV_NVM_Write(appData.hNvram, &appData.hNvmCmdHandle, &sConfig, 0, 1);
         if (appData.hNvmCmdHandle == DRV_NVM_COMMAND_HANDLE_INVALID)
         {   
            //vDebug("Write fail!\0", D_RED, 1);  // kbdoo message
         }   
         else
         {
            appData.uiNvramState = 50;
         }   
      break;                               
      case 50:                         // Check if finished                   
         dCommandStatus = DRV_NVM_CommandStatus(appData.hNvram, appData.hNvmCmdHandle);    
         if (DRV_NVM_COMMAND_COMPLETED == dCommandStatus)
            appData.uiNvramState = 80;
      break;                               
      case 60:                     
                    
      break;   
      case 70:                     
                    
      break;   
      case 80:                  
         if (appData.hNvram)
         {   
            DRV_NVM_Close(appData.hNvram);
            appData.hNvram = 0;
         }   
         appData.uiNvramState = 10;    // Back to check if config has changed
      break;   
 
   }  
}

//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//






