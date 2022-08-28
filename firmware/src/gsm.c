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
 
#include "gsm.h"
 

//*********************************************************************************************************************
// Globals
// 
//*********************************************************************************************************************
//
   
extern APP_DATA                        appData;
extern NVM_CONFIG                      sConfig;
 
//*********************************************************************************************************************
// Incoming SMS or Call
//  
//*********************************************************************************************************************
//

void vGsmRingEventHandler(void)
{
   strncpy(appData.uiDebugbuffer6, "RING!  ", 7);
   
   
   
}   

//*********************************************************************************************************************
// Interpret SMS message
//  
//*********************************************************************************************************************
// GSM commands  ('jp command <argument>')
//    aprs :   run APRS radio even if ignition is not on  
//    off :    send power off signal to shut down
//    beacon:  ask APRS to send beacon
//    accon:   accessory relay on
//    accoff:  accessory relay off
//

void vInterpretGsmMsg(void)
{
   uint8_t                       ucBuff[150]                 = {0};
   uint8_t                       ucTemp[50]                  = {0};
   
   //sprintf(appData.uiDebugbuffer6, "%s %d\0", appData.ucGsmSender, appData.iGsmMsgSize);
   //sprintf(appData.uiDebugbuffer7, "%s\0", appData.ucGsmMessage);     
   // Clear custom message     
   memset(appData.cCustomAprsMessage, 0x00, 30);
   
   if ((!memcmp(appData.ucGsmMessage, GSM_KEYWORD, 3)) || (appData.uiGsmPendingTx))
   {                  
      if (!appData.cGsmCommandHandled)
      {
         appData.cGsmCommandHandled = 1;
         
         // Run APRS even if ignition is not on
         if ((*(appData.ucGsmMessage + 3) == 'a') || (*(appData.ucGsmMessage + 3) == 'A'))
         {   
            vRelay_On(REL_RADIO);    
            appData.ulRadioOn = appData.ulLonglife;
            appData.ucNo_message = 1;
         }   
         // Turn device off if ignition is not on
         else if (((*(appData.ucGsmMessage + 3) == 'o') || (*(appData.ucGsmMessage + 3) == 'O')) && (IGNITION_SENSE))
         {   
            appData.cPoweroff_sms = 1;
            appData.ucNo_message = 1;           
         }   
         // Force APRS beacon
         if ((*(appData.ucGsmMessage + 3) == 'b') || (*(appData.ucGsmMessage + 3) == 'B'))
         {   
            vAprsBeacon();
            appData.ucNo_message = 1;            
         }   
         // Accessory relay on
         else if (!memcmp(appData.ucGsmMessage + 3, "x1", 2)) 
         {   
            vRelay_On(REL_ACC);   
            appData.ucNo_message = 1;            
         }   
         // Accessory relay off
         else if (!memcmp(appData.ucGsmMessage + 3, "x0", 2)) 
         {   
            vRelay_Off(REL_ACC);   
            appData.ucNo_message = 1;            
         }                      
         // Set custom APRS message [max 30 characters]
           if ((*(appData.ucGsmMessage + 3) == 'm') || (*(appData.ucGsmMessage + 3) == 'M'))
         {   
            strcpy(appData.cCustomAprsMessage, appData.ucGsmMessage + 4);                         
         }                      
      }   
      
      if (!appData.ucNo_message)
      {   
         if (((appData.sGpsMode > 1) && (abs(appData.dGpsLatDec) > 0.000)) || (appData.cPendingShutdown))   // Wait for GPS or pending shutdown
         {
            sprintf(ucBuff, "https://maps.google.com/maps?q=loc:%f\x2c%f ", appData.dGpsLatDec, appData.dGpsLonDec);
            sprintf(ucTemp, "%.1f km/h %.1f deg ", appData.fGpsSpeed, appData.fGpsBearing);
            strcat(ucBuff, ucTemp);
            sprintf(ucTemp, "%c%.2fv %.0fmA %.2fc %.2fmb %.1fnSh S:%d/%d %x", '\n', appData.dVoltage, appData.dCurrent, 
                    appData.dTemperature, appData.dPressure, appData.fRadLevel, appData.sGpsSatsUsed, appData.sGpsSatsSeen, appData.llRandom);
            strcat(ucBuff, ucTemp);        
            strncpy(appData.ucGsmTxMessage, ucBuff, 150);
            appData.uiGsmPendingTx = 0;

         }
         else     // Wait
         {
            appData.uiGsmPendingTx = 1;         
         }   
      }     
   }
}   

//*********************************************************************************************************************
// 
//*********************************************************************************************************************
//
   
void vSendSmsAlarm(char * cMessage)
{        
   uint8_t                       ucBuffer[128]              = {0}; 
   
   sprintf(ucBuffer, "%s%c %.2fv %.1f mA %.2fc %.2fmb %.1f nSh S:%d/%d%chttps://maps.google.com/maps?q=loc:%f\x2c%f", 
           cMessage, '\n', appData.dVoltage, appData.dCurrent, appData.dTemperature, appData.dPressure, appData.fRadLevel,
           appData.sGpsSatsUsed, appData.sGpsSatsSeen, '\n', appData.dGpsLatDec, appData.dGpsLonDec);           
   strncpy(appData.ucGsmTxMessage, ucBuffer, 120);
  
}
 
//*********************************************************************************************************************
// Send GSM message to VDU
//  
//*********************************************************************************************************************
//

void vSendGsmCan(void)
{
   uint8_t                       ucTemp[40]                 = {0}; 
   GSM_MSG                       sGsmMsg;
 
   sGsmMsg.iGsmMsgSize = appData.iGsmMsgSize;
   strncpy(sGsmMsg.ucGsmSender, appData.ucGsmSender, 20);
   strncpy(sGsmMsg.ucGsmMessage, appData.ucGsmMessage, 160);
   vCanSendBuffer((uint8_t *)&sGsmMsg, sizeof(sGsmMsg), CID_GSM_H);               
}   
 
//*********************************************************************************************************************
// Interpret GSM sentences, populate data fields.
//  
//*********************************************************************************************************************
//

void vInterpretGsm(uint8_t * ucBuff)
{
   uint8_t                       ucTemp[40]                 = {0}; 
   uint8_t                       ucTemp2[220]               = {0}; 
   uint8_t                     * pPtr1                      = 0;
   uint8_t                     * pPtr2                      = 0;
   uint8_t                       i                          = 0;   
   uint8_t                       uiArraySize                = 0; 
  
   //***** SMS ******************** 
   
   // Registration status
   if (!memcmp(ucBuff, "CREG:", 5))                   
   {
      strncpy(ucTemp, ucBuff + 6, 4);
      appData.cGsmRegistered = ucTemp[2]; 
   }      
   // Signal
   else if (!memcmp(ucBuff, "CSQ:", 4))                   
   {
      strncpy(ucTemp, ucBuff + 3, 5);
      sscanf(ucTemp, ": %hu,", &appData.iGsmSignal);       
   }   
   // Device got reset
   else if (!memcmp(ucBuff, "CPIN:", 5))                   
   {
      // Restart state machine
      appData.uiGsmState = 0;
   }   
   // Operator
   else if (!memcmp(ucBuff, "COPS:", 5))                   
   {
      pPtr1 = strchr(ucBuff, 0x22);
      if (pPtr1)
      {
         pPtr1++;
         pPtr2 = strchr(pPtr1, 0x22);
         *pPtr2 = 0x00;
         strncpy(appData.ucGsmOperator, pPtr1, 19);         
      }
      else
         strcpy(appData.ucGsmOperator, "No operator\0");         
          
   }      
   
   else if (!memcmp(ucBuff, "CNSMOD:", 7))                   
   {
      pPtr1 = strchr(ucBuff, 0x2c);
      if (pPtr1)    
      {   
         strncpy(ucTemp, pPtr1 + 1, 2);
         ucTemp[2] = 0;
         sscanf(ucTemp, "%hu", &appData.iGsmMode);   
      }              
      //sprintf(appData.uiDebugbuffer0, "M: %d %c", appData.iGsmMode, '\0');
   } 
   
    
   // Jamming status
   else if (!memcmp(ucBuff, "SJDR:", 5))   
   {      
      memcpy(ucTemp, ucBuff + 6, 8);
      ucTemp[8] = 0;
            
      if (strstr(ucTemp, "NO JAMM"))
      {
         appData.cJamming = 1;
      }
      else if (strstr(ucTemp, "JAMMING"))
      {
         appData.cJamming = 2;         
      }
      else if (strstr(ucTemp, "INTERFE"))
      {
         appData.cJamming = 3;
      } 
      else if (ucTemp[0] == 0x30)   // '0' 
      {
         appData.cJamming = 1;
      }        
      else if (ucTemp[0] > 0x30)    // '0' 
      {
         appData.cJamming = 2;
      }                      
   }
      
   // Read SMS message
   else if (!memcmp(ucBuff, "CMGR:", 5))                   
   {                              
      appData.uiGsmGotMsg = 1;
      pPtr1 = strchr(ucBuff, 0x2c);
      pPtr2 = strchr(pPtr1 + 1, 0x2c);    
      memset(appData.ucGsmSender, 0x00, 20);
      strncpy(appData.ucGsmSender, pPtr1 + 2, pPtr2 - pPtr1 - 3);       
      pPtr2 = strchr(ucBuff, 0x03);
      if (pPtr2)
      {         
         memset(appData.ucGsmMessage, 0x00, 160);
         appData.iGsmMsgSize = strlen(pPtr2 + 1);
         strncpy(appData.ucGsmMessage, pPtr2 + 1, appData.iGsmMsgSize);
      }          
      vSendGsmCan();                      // Send message to VDU
      appData.cGsmCommandHandled = 0;     // Flag any commands for handling
      appData.ucNo_message = 0;
      vInterpretGsmMsg(); 
   }   
   else if (!memcmp(ucBuff, "CMTE:", 5))                   
   {            
      pPtr1 = strchr(ucBuff, 0x2c);
      if (pPtr1)    
      {   
         strncpy(appData.ucGsmTemp, pPtr1 + 1, 6);
         sscanf(appData.ucGsmTemp, "%lf", &appData.dGsmTemp);   
      }   
   }            
   else if (!memcmp(ucBuff, "CMTI:", 5))   // Get SMS index                
   {            
      pPtr1 = strchr(ucBuff, 0x2c);
      if (pPtr1)    
      {   
         strncpy(ucTemp, pPtr1 + 1, 3);
         sscanf(ucTemp, "%hu", &appData.iSmsIndex);   
      }   
     
   }
   
   //***** Bluetooth ************** 
   else if (!memcmp(ucBuff, "BTSTATUS:", 9))                   
   {                  
      pPtr1 = strchr(ucBuff, 0x3a);
      if (pPtr1)    
         sscanf(pPtr1 + 1, " %hu", &appData.iGsmBtStatus);      
      if (appData.iGsmBtStatus == 5)
         appData.ucGsmBtScan = 0;
   }   
   if (!memcmp(ucBuff, "BTSCAN:", 7))   
   {         
      if (strlen(ucBuff) > 15)  // Discovered a device
      {  
         pPtr1 = strchr(ucBuff, 0x2c);
         if (pPtr1)                                                           // Node number
         {
            sscanf(pPtr1 + 1, "%d", &i);          
 
            //sprintf(appData.uiDebugbuffer6, "-> '%d' \0", i);

            if (i < BT_ARRAYSIZE)
            {   
               pPtr2 = strchr(pPtr1 + 1, 0x22);                                  // Name
               if (pPtr2)
               {   
                  pPtr1 = strchr(pPtr2 + 1, 0x22);                               // End name      
                  if (pPtr1)
                  {                     
                     strncpy(appData.sBtData[i-1].cName, pPtr2 + 1, pPtr1 - pPtr2 - 1);                   
                     pPtr2 = strchr(pPtr1 + 1, 0x2c);
                     if (pPtr2)
                     {   
                        memcpy(appData.sBtData[i-1].cAddress, pPtr1 + 2, 17);                      
                        strncpy(appData.sBtData[i-1].cRssi, pPtr2 + 1, 4);   

                        //sprintf(appData.uiDebugbuffer6, "%s \0", appData.sBtData[0].cName);
                        //sprintf(appData.uiDebugbuffer7, "%s \0", appData.sBtData[0].cAddress);                                                                             
                     }   
                  }   
               }      
            }
         }                                       
      }   
   }   
}

//*********************************************************************************************************************
// Receive a character from the GSM. Call to interpret if complete sentence. 
// 
//*********************************************************************************************************************
//

void APP_GsmReceiveEventHandler(SYS_MODULE_INDEX index)    // GPS USART 2
{
   unsigned char ucTemp[20] = {0}; 
   unsigned char ucChar;
        
   ucChar = DRV_USART_ReadByte(appData.drvGsmHandle);    
   // Check if command finished
   appData.ucGsmLast[0] = appData.ucGsmLast[1];
   appData.ucGsmLast[1] = appData.ucGsmLast[2];
   appData.ucGsmLast[2] = appData.ucGsmLast[3];
   appData.ucGsmLast[3] = appData.ucGsmLast[4];
   appData.ucGsmLast[4] = appData.ucGsmLast[5];
   appData.ucGsmLast[5] = ucChar;
   if (!memcmp(appData.ucGsmLast, "\r\nOK\r\n", 6)) 
   {   
      appData.uiGsmPending = 0;
      appData.cGsmInited = 1;
      appData.cGsmOnline = 1;      
   }   
   else if (!memcmp(appData.ucGsmLast, "RROR\r\n", 6))     
   {   
      appData.uiGsmPending = 0;
      appData.cGsmInited = 1;
   }   
   else if (!memcmp(appData.ucGsmLast, "eady\r\n", 6))     
   {   
      appData.uiGsmState = 2;   // Restart queries
      appData.uiGsmPending = 0;
      appData.cGsmInited = 1;
   }   
   
   // Manage incoming character
   if (!appData.uiGsmSync)       // Waiting for sync 0x0d
   {
      if (ucChar == '+')
      {
         memset(appData.ucGsmRxbuff, 0x00, 256);
         appData.uiGsmSync = 1;
         appData.uiGsmRxptr = 0;                  
      }   
      else   
      {                  
      }   
   }
   else
   { 
      if (appData.uiGsmRxptr < 256)
      {   
         appData.ucGsmRxbuff[appData.uiGsmRxptr] = ucChar;              
         if (appData.ucGsmRxbuff[appData.uiGsmRxptr] == 0x0d)     // End of sentence
         {
            if (!appData.uiGsmDouble)
            {   
               appData.ucGsmRxbuff[appData.uiGsmRxptr] = 0x00;    // Null out 0x0d
               vInterpretGsm(appData.ucGsmRxbuff);
               appData.uiGsmSync = 0;
            }
            else
            {   
               appData.ucGsmRxbuff[appData.uiGsmRxptr] = 0x5f;    // Underscore
               appData.uiGsmRxptr++;
            }   
         }
         else if ((appData.ucGsmRxbuff[appData.uiGsmRxptr] == 0x0a) && (appData.ucGsmRxbuff[appData.uiGsmRxptr - 1] == 0x5f))    // Start of second line of text
         {
            appData.ucGsmRxbuff[appData.uiGsmRxptr] = 0x03;    // Start of text 
            appData.uiGsmRxptr++;
            appData.uiGsmDouble = 0;
         }   
         else
            appData.uiGsmRxptr++;
      }
      else
         appData.uiGsmSync = 0;
   }   
}

//*********************************************************************************************************************
//  
//*********************************************************************************************************************
//
 
void vGsm_Initialize (void)
{         
   appData.drvGsmHandle = DRV_USART_Open(DRV_USART_INDEX_2, DRV_IO_INTENT_READWRITE);
   if(DRV_HANDLE_INVALID == appData.drvGsmHandle) 
   {
      by1306_DrawText(0, 0, "Inval Gsm!\0", FONT_ALPS); 
      
   }

   // Register UART callback
   DRV_USART_ByteReceiveCallbackSet(DRV_USART_INDEX_2, APP_GsmReceiveEventHandler);
   
   appData.uiGsmRxptr = 0;  
   appData.uiGsmSync = 0;     // Wait for sync
   appData.uiGsmOldstate = 0;       
   appData.uiGsmPendingTx = 0;    
   appData.cGsmCommandHandled = 1;              // Reset if new incoming message
   memset(appData.ucGsmMessage, 0x00, 160);
   memset(appData.ucGsmTxMessage, 0x00, 160);
   appData.iSmsIndex = 0;
   
}

//*********************************************************************************************************************
//  Send command buffer 
//*********************************************************************************************************************
//

void vGsm_sendcommand(char * ucBuff, uint8_t uiDouble)
{
   unsigned int                  uiLength                   = 0;
   unsigned int                  i                          = 0;
   uint8_t                       ucTemp[240];
   
   if (uiDouble == 2)   // Raw string
   {
      strcpy(appData.ucGsmTxbuff, ucBuff);
      uiLength = strlen(ucBuff);     
   }
   else
   {   
      sprintf(appData.ucGsmTxbuff, "AT%s\r\n", ucBuff);
      uiLength = strlen(ucBuff) + 4;   
   }
   
   if (appData.drvGsmHandle == DRV_HANDLE_INVALID)
   {      
   }
   else
   {  
      if (uiDouble == 1)
         appData.uiGsmDouble = 1;
      while (uiLength > 0)
      {   
         uiLength--;
         while (DRV_USART_TRANSFER_STATUS_TRANSMIT_FULL & DRV_USART_TransferStatus(appData.drvGsmHandle))
         {            
         }          
         PLIB_USART_TransmitterByteSend(USART_ID_1, appData.ucGsmTxbuff[i]);
         i++;
         vLongDelay(1);
      } 
   }   
}

//*********************************************************************************************************************
// 
//*********************************************************************************************************************
//
 
void vSendBluetoothArray()
{
   unsigned int                  uiArraySize                = 0; 

   // Send list of Bluetooth devices seen to VDU
   if ((appData.uiDisplayScreen != SCREEN_GEIGER) || (!(sConfig.cConfig1 & CFG_GEIGER)))  // Not when sending ticks
   { 
      uiArraySize = sizeof(BT_DATA) * BT_ARRAYSIZE;
      appData.pcBtArray = malloc(uiArraySize);
      if (appData.pcBtArray != NULL)
      {   
         memcpy(appData.pcBtArray, appData.sBtData, uiArraySize);                  
         vCanSendBuffer(appData.pcBtArray, uiArraySize, CID_BT_H);         
         free(appData.pcBtArray);            
      }                  
   }   
}
   
//*********************************************************************************************************************
// 
//*********************************************************************************************************************
//
 
void vGsmSendMessage(void)
{
   uint8_t                       ucBuffer[256]              = {0}; 
   
   sprintf(ucBuffer, "AT+CMGS=\x22%s\x22\x0d\0", sConfig.bGsmPrimary);
   vGsm_sendcommand(ucBuffer, 2);
   strcpy(ucBuffer, appData.ucGsmTxMessage);
   vLongDelay(50);
   vGsm_sendcommand(ucBuffer, 2);
   vLongDelay(20);
   vGsm_sendcommand("\x1a\r\n\0", 2);
}   

//*********************************************************************************************************************
// 
//*********************************************************************************************************************
//

void vGsm_State()
{    
   static unsigned char          i                          = 0; 
   unsigned char                 ucDebug[512]               = {0};
   uint8_t                       ucTemp[128]                = {0};
   
   // State debug display
   if (appData.uiDisplayState > 4)
   {   
      sprintf(ucTemp, "%d %d %c", appData.uiGsmState, appData.iGsmMode, '\0');
      by1306_DrawText(80, 4, ucTemp, FONT_VGA);   
   }   
      
   if (appData.cGsmStarted)
   {   
      // Normal
      if (appData.uiGsmOldstate != appData.uiGsmState)
      {   
         appData.uiGsmOldstate = appData.uiGsmState;
         appData.uiGsmLifeTimeout = appData.ulLonglife;
      }   
      else     // Check for timeout
      {                                  
         if (appData.ulLonglife - appData.uiGsmLifeTimeout > 10)      
         {    
            vBeep(3);
            sprintf(ucTemp,  "[Error] %d \0", appData.uiGsmState); 
            by1306_DrawText(0, 0, ucTemp, FONT_ALPS); 
            appData.uiGsmLifeTimeout = appData.ulLonglife;
            appData.uiGsmState = 2;    // Restart   
         }
      }
   }
   
   switch (appData.uiGsmState)
   {
      case 0:             
         if ((appData.ulLonglife - appData.ulGsmStartDelay) > 15)  // delay before start
         {        
            appData.uiGsmState = 2;
            appData.cGsmStarted = 1;   
            appData.uiGsmLifeTimeout = appData.ulLife;
         }
      break;                               
      case 2:        // Start      
         appData.uiGsmPending = 1;
         vGsm_sendcommand("\r\n\r\n", 0);
         appData.uiGsmState = 10;                         
      break;
      case 10:                     
         if (!appData.uiGsmPending)
            appData.uiGsmState = 12;  
      break;               
      case 12:          
         appData.uiGsmPending = 1;
         vGsm_sendcommand("&F0", 0);    
         appData.uiGsmState = 14;
      break;   
      case 14:                     
         if (!appData.uiGsmPending)
            appData.uiGsmState = 16;  
      break;               
      case 16:          
         appData.uiGsmPending = 1;
         vGsm_sendcommand("E0", 0);    
         appData.uiGsmState = 40;
      break;                     
      case 40:                     
         if (!appData.uiGsmPending)
            appData.uiGsmState = 50;  
      break;               
      case 50:          
         appData.uiGsmPending = 1;
         vGsm_sendcommand("+CFUN=1", 0);    
         appData.uiGsmState = 60;  
      break;               
      case 60:                     
         if (!appData.uiGsmPending)
            appData.uiGsmState = 62;  
      break;                     
      case 62:             
         appData.uiGsmPending = 1;
         vGsm_sendcommand("+CSCLK=1", 0);       // Turn off UART when sleeping                         
         appData.uiGsmState = 64;  
      break;               
      case 64:                     
         if (!appData.uiGsmPending)
            appData.uiGsmState = 70;  
      break;                     
      case 70:                     
         appData.uiGsmPending = 1;
         vGsm_sendcommand("+CMGF=1", 0);        // SMS text format                 
         appData.uiGsmState = 80;  
      break;                         
      case 80:                     
         if (!appData.uiGsmPending)      
            appData.uiGsmState = 90;  
      break;                     
      case 90: 
         appData.uiGsmPending = 1;
         vGsm_sendcommand("+SJDR=1,1", 0);      // Jamming detection on                      
         appData.uiGsmState = 100;  
      break;           
      case 100:                                   
         if (!appData.uiGsmPending)
            appData.uiGsmState = 160;           // Skip bluetooth  160
      break;                      
       
      case 110: 
         appData.uiGsmPending = 1;
         vGsm_sendcommand("+BTPOWER=1", 0);     // Bluetooth on             
         
         appData.uiGsmState = 120;  
      break;           
      case 120:                                   
         if (!appData.uiGsmPending)
            appData.uiGsmState = 130;  
      break;                               
      case 130:                                   
                  
         appData.uiGsmState = 140;  
      break;                               
      case 140: 
         appData.uiGsmPending = 1;
         sprintf(ucTemp, "+BTHOST=%s", BT_NAME);
         vGsm_sendcommand(ucTemp, 0);           // Bluetooth name    
         appData.uiGsmState = 150;  
      break;           
                 
      case 150:                           
          if (!appData.uiGsmPending)
            appData.uiGsmState = 160;//16 
         
      break;
      case 160: 
         appData.uiGsmPending = 1;
         vGsm_sendcommand("&W", 0);     // Store config            
         
         appData.uiGsmState = 170;  
      break;           
      case 170:                                   
         if (!appData.uiGsmPending)
            appData.uiGsmState = 240;  
      break;                               
       
      case 180:                     
         appData.uiGsmPending = 1;
         vGsm_sendcommand("+BTVIS=1", 0);    // Bluetooth visible    
         appData.uiGsmState = 190;  
      break;                               
      case 190:                              
         if (!appData.uiGsmPending)
            appData.uiGsmState = 240;    
      case 200: 
         //appData.uiGsmPending = 1;
         //vGsm_sendcommand("+CMTE=1", 0);    // Enable temperature checking
         appData.uiGsmState = 240;  
      break;           
      
      //****** Main loop ***************************************
      
      case 240:                           
         if (!appData.uiGsmPending)
            appData.uiGsmState = 250;  
          
         // Display
         strncpy(ucTemp, appData.ucGsmOperator, 12);
         memset(appData.uiDebugbuffer0, 0x00, 16);
         if (strlen(appData.ucGsmOperator))
            sprintf(appData.uiDebugbuffer0, "%s %d\0", ucTemp, appData.iGsmSignal);             
      break;                               
      case 250:                     
      
         appData.uiGsmState = 260;  
      break;                               
      case 260:                                
         appData.uiGsmPending = 1;
         vGsm_sendcommand("+CREG?", 0);         // Network registration status  
         appData.uiGsmState = 280;  
      break;           
      case 280:                           
         if (!appData.uiGsmPending)
            appData.uiGsmState = 300;  
      break;                               
      case 300:                           
         appData.uiGsmState = 320;  
      break;                               
      case 320: 
         appData.ulGsmTime = appData.ulLonglife;
         //appData.uiGsmPending = 1;
         sprintf(ucTemp, "+CMGR=%d%c", appData.iSmsIndex, '\0');         
         vGsm_sendcommand(ucTemp, 1);           // Read first SMS
         appData.uiGsmState = 340;  
      break;     
      case 340:                           
         //if (!appData.uiGsmPending)
         if ((appData.ulLonglife - appData.ulGsmTime) > 2)
            appData.uiGsmState = 360;  
      break;                               
      case 360: 
         if (appData.uiGsmGotMsg)
         {  
            appData.uiGsmPending = 1;
            vGsm_sendcommand("+CMGD=0,4", 0);   // Delete all messages  
            appData.uiGsmGotMsg = 0;
            appData.uiGsmGotMsgRdy = 1;
         }
         appData.uiGsmState = 380;  
      break;     
      case 380:                     
         if (appData.uiGsmGotMsgRdy)
         {  
            if (!appData.uiGsmPending)   
            {   
               appData.uiGsmState = 400;
               appData.uiGsmGotMsgRdy = 0;
               if ((appData.cPoweroff_sms) && (IGNITION_SENSE))                  
               {
                  PWR_SHUTDOWN = 1;
               }   
               else
                  appData.cPoweroff_sms = 0;    // Poweroff overridden by ignition
            }   
         } 
         else
            appData.uiGsmState = 400;
      break;                               
      case 400: // Pending send?              
         if ((strlen(appData.ucGsmTxMessage)) && (appData.cGsmRegistered))
         {              
            appData.uiGsmGotMsgRdy = 1;
            appData.uiGsmPending = 1;
            vGsmSendMessage();
            appData.ucGsmTxMessage[0] = 0;
         }   
         appData.uiGsmState = 420;  
      break;     
      case 420:                     
         if (appData.uiGsmGotMsgRdy)
         {  
            if (!appData.uiGsmPending)   
            {   
               appData.uiGsmState = 440;  
               appData.uiGsmGotMsgRdy = 0;
            }   
         }   
         else
            appData.uiGsmState = 440;  
      break;                               
      case 440: 
         appData.uiGsmPending = 1;
         vGsm_sendcommand("+CSQ", 0);           // Signal report
         appData.uiGsmState = 460;  
      break;     
      case 460:                     
         if (!appData.uiGsmPending)   
            appData.uiGsmState = 480;    
      break;                               
      case 480: 
         //appData.uiGsmPending = 1;
         //vGsm_sendcommand("+CMTE?", 0);         // Get temperature  
         appData.uiGsmState = 500;  
      break;     
      case 500:                     
         if (!appData.uiGsmPending)   
            appData.uiGsmState = 520;  
      break;                               
      case 520:           
         appData.uiGsmPending = 1;
         vGsm_sendcommand("+COPS?", 0);         // Operator           
         appData.uiGsmState = 540;  
      break;     
      case 540:                     
         if (!appData.uiGsmPending)   
            appData.uiGsmState = 560;  
      break;                               
      case 560: 
         if (appData.uiGsmPendingTx)
            vInterpretGsmMsg();
         appData.uiGsmState = 700;     // Skip BT   
      break;
      case 580: 
         //appData.uiGsmPending = 1;        
         if (!appData.ucGsmBtScan)
         {   
            vGsm_sendcommand("+BTSCAN=1,20", 0);         // Bluetooth scan                    
            appData.ucGsmBtScan = 1;
         }                 
         appData.uiGsmState = 600;  
      break;     
      case 600:                     
         vGsm_sendcommand("+BTSTATUS?", 0);    
         appData.uiGsmState = 700;  
      break;                            

      case 700:
         appData.uiGsmPending = 1;
         vGsm_sendcommand("+SJDR?", 0);                  // Poll for jamming
         appData.uiGsmState = 702;  
      break;    
      case 702:                     
         if (!appData.uiGsmPending)   
            appData.uiGsmState = 710;  
      break;     
      
      case 710:                     
         appData.uiGsmPending = 1;
         vGsm_sendcommand("+CNSMOD?", 0);               
         appData.uiGsmState = 720;  
      break;                         
      case 720:                     
         if (!appData.uiGsmPending)      
            appData.uiGsmState = 722;  
      break;               
          
      case 722:           
         appData.uiGsmPending = 1;
         vGsm_sendcommand("+CEREG?", 0);                 // Extended mode information           
         appData.uiGsmState = 724;  
      break; 
      
      case 724:                     
         if (!appData.uiGsmPending)   
            appData.uiGsmState = 999;  
      break;                     
            
      case 999: 
         // Send bluetooth device array
         // vSendBluetoothArray();
         appData.uiGsmState = 240;  
      break;                
   }   
}

//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//










