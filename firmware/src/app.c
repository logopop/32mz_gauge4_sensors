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

//*********************************************************************************************************************
// 
//  
//*********************************************************************************************************************
//
 
APP_DATA                            appData;
NVM_CONFIG                          sConfig;                      // Stored in non-volatile memory



	  
//*********************************************************************************************************************
// Change notification jump
//  
//*********************************************************************************************************************
//
 
void APP_ChangeReceiveEventHandler(void)   
{
   // Sort change flags
   if (GEIGER)
      vGeigerReceiveEventHandler();
 //  else if (ucSource == 1)   
 //     vPPSReceiveEventHandler();
   
} 
 
//*********************************************************************************************************************
// Timer 7 life callback, 1ms
//  
//*********************************************************************************************************************
//

void vTmr7_event(void)
{
   appData.ulLife++;
   if (appData.ulLife >= 1200000)     // Rollover after 20 minutes
   {   
      appData.ulLife = 0;
   
      
   }   
      
   // Poll for pending shutdown
   if ((PENDING_PWROFF) && (appData.ulLonglife > 20))
   {
      strcpy(appData.uiDebugbuffer0, "Shut down! \0");
      appData.cPendingShutdown = 1;
      if (!appData.cSalarm)
      {   
         appData.cSalarm = 1;
         vBeep(0);
      }   
   }   
   else
      appData.cPendingShutdown = 0;
      
   // Check timer events
   // LED indicator
  
   if ((appData.ulLife - appData.uiRledcount) > 20)  
      LED_RED = LED_OFF;
 
   if ((appData.ulLife - appData.uiYledcount) > 20)  
      LED_YELLOW = LED_OFF;

   if ((appData.ulLife - appData.uiBledcount) > 20) 
      LED_BLUE = LED_OFF;
   
   if ((appData.ulLife - appData.uiGledcount) > 20)  
      LED_GREEN = LED_OFF;
     
   if (!(appData.ulLife % 1000))
   {
      appData.ulLonglife++;
       
   }   
      
}   
 
//*********************************************************************************************************************
// Timers init
//  
//*********************************************************************************************************************
//

void vTimersInit(void)
{   
   // Fast pace timer init (Timer 5)
   // Timer clock is 100 MHz
   T5CON = 0x0000;
   TMR5 = 0;
   T7CONbits.TCKPS = 0b101;         // Prescaler /32
   
   IFS1bits.T7IF = 0;               // Clear interrupt flag
   IPC8bits.T7IP = 1;               // Interrupt priority
   IPC8bits.T7IS = 0;               // Sub-priority
   IEC1bits.T7IE = 1;               // Enable Timer
   PR7 = 0x0c35;                    // 3125   1ms   
   T7CONbits.ON = 1;                // Module enabled
  
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
   
   // Return if alarms are turned off
   if (sConfig.cConfig1 & CFG_AL_AUDIO)
   {      
      switch (iSound)
      {
         case 0:                             // Error           
            for (i = 0; i < 3; i++)
            {
               SPEAKER = 1;   
               vLongDelay(3);
               SPEAKER = 0;   
               vLongDelay(3);
            }     
         break;
         case 1:                             // Start
            for (i = 0; i < 10; i++)
            {
               SPEAKER = 1;   
               vShortDelay(26000);
               SPEAKER = 0;   
               vShortDelay(26000);
            }     
         break;
         case 2:      
            for (i = 0; i < 13; i++)
            {
               SPEAKER = 1;   
               vShortDelay(22000);
               SPEAKER = 0;   
               vShortDelay(22000);
            }     
         break;
         case 3:
            for (i = 0; i < 20; i++)
            {
               SPEAKER = 1;   
               vShortDelay(12000);
               SPEAKER = 0;   
               vShortDelay(12000);
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
   SPEAKER = 0;
}

//*********************************************************************************************************************
// Turn relay on using tris to avoid unknown states on startup
//  
//*********************************************************************************************************************
//

void vRelay_On(uint8_t uiRelay)
{
   if (uiRelay == REL_RADIO)
   {
      TRISBbits.TRISB10 = 0;             // Output
      LATBbits.LATB10 = 1;      
   }
   else if (uiRelay == REL_ACC)
   {
      TRISBbits.TRISB9 = 0;              // Output
      LATBbits.LATB9 = 1;                  
   }   
}

//*********************************************************************************************************************
// Turn relay off using tris to avoid unknown states on startup
//  
//*********************************************************************************************************************
//

void vRelay_Off(uint8_t uiRelay)
{
   if (uiRelay == REL_RADIO)
   {
      LATBbits.LATB10 = 0;      
      TRISBbits.TRISB10 = 1;              // Input
   }
   else if (uiRelay == REL_ACC)
   {
      LATBbits.LATB9 = 0;                  
      TRISBbits.TRISB9 = 1;              // Input
   }   
}

//*********************************************************************************************************************
// 
//  
//*********************************************************************************************************************
//

void APP_Initialize (void)
{
   uint8_t                      i                           = 0;
   
   // Variables   
   PWR_SHUTDOWN = 0;
   LED_BLUE = LED_OFF;
   LED_GREEN = LED_OFF;
   LED_YELLOW = LED_OFF;
   LED_RED = LED_OFF;
   
   appData.uiGyroState = 0;
   appData.uiDisplayState = 0;
   appData.uiAprsState = 0;
   appData.uiGpsState = 0;
   appData.uiGsmState = 0;
   appData.uiCanState = 0;
   appData.ulLife = 0;
   appData.uiMainState = 0;
   appData.uiCanUp = 0;
   appData.ucDisplayPresent = 1;    // Default: we have a display
   appData.ulRadioOn = 0;
    
   // GSM
   GSM_DTR = 1;
   GSM_RESET = 1;
   
   // Capture input trigger lines (ignition, alarm)
   appData.uiStartupflag = 0x00;
   if (!IGNITION_SENSE)
      appData.uiStartupflag = STARTUP_IGNITION;
   if ((!ALARM_SENSE) || (PENDING_PWROFF))       // PWROFF line indicates alarm on startup
   {   
      appData.uiStartupflag |= STARTUP_ALARM;
      vBeep(0);
   }   
      
   if (!appData.uiStartupflag)
      appData.uiStartupflag = STARTUP_TIMER;
            
   // Init indicator

   LED_YELLOW = LED_ON;
   vLongDelay(30);
   LED_BLUE = LED_ON;
   vLongDelay(30);
   LED_RED = LED_ON;
   vLongDelay(30);
   LED_GREEN = LED_ON;
   vLongDelay(30);
   LED_YELLOW = LED_OFF;
   vLongDelay(30);
   LED_BLUE = LED_OFF;
   vLongDelay(30);
   LED_RED = LED_OFF;
   vLongDelay(30);
   LED_GREEN = LED_OFF;
   
    
   // Initialize boot timers
   appData.ulLife = 0;
   appData.cGsmStarted = 0;
   appData.ulGsmStartDelay = appData.ulLonglife;
   
   // Initialize I2C #1 
   vI2C_Init();

   // Initialize display
   by1306_Initialize();  
     
   vGps_Initialize();
   vGeiger_Initialize();
   vGsm_Initialize();       
   vGyro_Initialize();
   vAprs_Initialize();
   vCan_Initialize();
   vVoltage_Initialize();   
   
   // Initialize BMP280 temperature pressure sensor
   bme280_Initialize();
      
   // Initialize RNG
   RNGCONbits.CONT = 1;
   RNGCONbits.PLEN = 0xd4;   
   RNGCONbits.PRNGEN = 1;
   RNGCONbits.TRNGEN = 1;
  
   // Enable all interrupts
   SYS_INT_Enable();
   vTimersInit();
   
   // Fencing test
   memset(appData.cFence, 0x00, 64);   
   appData.cPendingShutdown = 0;   
      
}

//*********************************************************************************************************************
// Send geiger tick backlog 
//
//*********************************************************************************************************************
//

void vGeigerTicks(void)
{
   uint8_t                       uiBuff[6]                  = {0};

   while (appData.uiTicks)
   {   
      vCanTransmit(CID_TICK, uiBuff, 1);          
      appData.uiTicks--;
   }   
}

//*********************************************************************************************************************
// 
//
//*********************************************************************************************************************
//

void APP_Tasks(void)
{
   uint8_t                       *pFont                     = NULL;
   uint8_t                       ucBuff[20]                 = {0};
   uint8_t                       i                          = {0};
   char                          cTemp[160]                 = {0};
  
   // Read, load, or store config
   vNvram_State(); 
   
   vGeigerTicks();        
     
   if (appData.uiDisplayScreen == SCREEN_GYRO)         
      vGyro_State();
   vGeigerTicks();        
   
   //if (appData.ucDisplayPresent)
   vDisplay_State(); 
    
   if (appData.uiDisplayScreen == SCREEN_GYRO)         
      vGyro_State();
   vGeigerTicks();        
    
   vGps_State();
   
   vGyro_State();
   vGeigerTicks();        
     
//   by1306_DrawText(0, 1, ucBuff, FONT_VGA);
  
   vGsm_State();
   vGeigerTicks();        
   
   if (appData.uiDisplayScreen == SCREEN_GYRO)         
      vGyro_State();

   vGeiger_State();
   vGeigerTicks();        
   
   //if (appData.uiCanUp)
   vCan_State();
   if (appData.uiDisplayScreen == SCREEN_GYRO)         
      vGyro_State();
   vGeigerTicks();
   
   if (appData.ulRadioOn)
   {   
      if ((appData.ulLonglife - appData.ulRadioOn) > 3)  // Start comms after 1 seconds on
         vAprs_State();
   }
   vGeigerTicks();        
   
   //vVoltage_State();  This is old AD based routine, replaced by INA219
   vGeigerTicks();                    
     
     
   // Main app state ************************
   switch (appData.uiMainState)
   {
      case 0:        // Start
         if (appData.uiDisplayScreen == SCREEN_GYRO)         
            vGyro_State();
         appData.uiMainState = 1;         
      break;
      case 1:          
                    
         appData.uiMainState = 2;  
      break;                         
      case 2: 
         
         
         if (appData.uiDisplayScreen == SCREEN_GYRO)         
            vGyro_State();
         appData.uiMainState = 3;  
      break;              
      case 3: 
         // Send SMS alarm if alarm caused startup
         if ((appData.ulLonglife > 60) && (!appData.cCaralarm) && (appData.uiStartupflag & STARTUP_ALARM))     
         {                                   
            vSendSmsAlarm("ALARM Car!");
            appData.cCaralarm = 1;
         }               
         appData.uiMainState = 4;  
      break;              
      case 4: 
         // Emphasize CAN messages depending on VDU screen                
         switch(appData.uiDisplayScreen)
         {
            case SCREEN_SPEED:
            case SCREEN_POSITION:
               vSendGpsPosition();   
               if (appData.ucSubScreen)
                  vSendAlive();
            break;
//            case SCREEN_GEIGER:         Only every 5 seconds
//               vSendGeigerMessage();                    
            break;
            case SCREEN_APRSRADAR:
            case SCREEN_APRSLIST:
               vSendAprsStationArray();
            break;
            case SCREEN_SATS:
               if (appData.cGpsArrayReady)
                  vSendGpsView();  
            break;   
            case SCREEN_SENSORS: 
               vSendAlive();
            break;
         }   
          
         appData.uiMainState = 5;                             
      break;              
      case 5: 
         if (appData.uiDisplayScreen == SCREEN_GYRO)         
            vGyro_State();              
                            
         appData.uiMainState = 6;  
      break;                    
      case 6: 
         if (appData.uiDisplayScreen == SCREEN_GYRO)         
            vGyro_State();   
         
         // Read INA219 current and voltage
         vGet_Voltage();
         // Read BMP280 temperature and pressure
         bme280_Read(); 
                  
         appData.uiMainState = 7;  
      break;                    
      case 7: 
         if (appData.uiDisplayScreen == SCREEN_GYRO)         
            vGyro_State();         
                             
         appData.uiMainState = 8;  
      break; 
      case 8:
         if ((appData.ulLonglife > 10) && (!appData.ulRadioOn))    // Delay radio start by 10 seconds
         {   
            // Start Radio if normal startup (ignition), or ignition is on
            if ((appData.uiStartupflag & STARTUP_IGNITION) || (!IGNITION_SENSE))
            {   
               vRelay_On(REL_RADIO);
               appData.ulRadioOn = appData.ulLonglife;              
            }   
         }
         // Fallback - start radio regardless of startup cause after 4 minutes
         if ((appData.ulLonglife > 240) && (!appData.ulRadioOn)) 
         {
            vRelay_On(REL_RADIO);
            appData.ulRadioOn = appData.ulLonglife; 
         }   
            
         appData.uiMainState = 1;  
      break; 
   }
   vGeigerTicks();        
   
   // Boundscheck sat array
   for (i = 0; i < 64; i++)
   {
      if (appData.cFence[i])
      {
         strncpy(appData.uiDebugbuffer7, appData.cFence, 16);
         memset(appData.cFence, 0x00, 64);
         i = 64;        
      }      
   }
   
    
   //strncpy(appData.uiDebugbuffer7, appData.cIncoming, 6);
 
   // Calculate random number   
   appData.llRandom = appData.iRandomCnt ^ (RNGNUMGEN1 * RNGNUMGEN2);
   vGeigerTicks();        
                
}

 
//*********************************************************************************************************************
// 
 


