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
#include "gps.h"

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

void vPPSReceiveEventHandler(void)
{
   //LED_BLUE = LED_ON;
 
   
   
}

//*********************************************************************************************************************
// Calculate Maidenhead locator
// 
//*********************************************************************************************************************
//

void vCalculateGrid()
{
   uint8_t                       cGrid[20]                  = {0};
   int                           o1;
   int                           o2;
   int                           o3;
   int                           a1;
   int                           a2;
   int                           a3;
   double                        remainder;
    
   // longitude
   remainder = appData.dGpsLonDec + 180.0;
   o1 = (int)(remainder / 20.0);
   remainder = remainder - (double)o1 * 20.0;
   o2 = (int)(remainder / 2.0);
   remainder = remainder - 2.0 * (double)o2;
   o3 = (int)(12.0 * remainder);

   // latitude
   remainder = appData.dGpsLatDec + 90.0;
   a1 = (int)(remainder / 10.0);
   remainder = remainder - (double)a1 * 10.0;
   a2 = (int)(remainder);
   remainder = remainder - (double)a2;
   a3 = (int)(24.0 * remainder);
   cGrid[0] = (char)o1 + 'A';
   cGrid[1] = (char)a1 + 'A';
   cGrid[2] = (char)o2 + '0';
   cGrid[3] = (char)a2 + '0';
   cGrid[4] = (char)o3 + 'a';
   cGrid[5] = (char)a3 + 'a';
   cGrid[6] = (char)0;
   
   if (appData.sGpsMode > 1)
      memcpy(appData.cMaidenhead, cGrid, 7);
   else
      memcpy(appData.cMaidenhead, "------\0", 7);
}   
   
//*********************************************************************************************************************
// Gps parse helper
// If pcTarget == 0, return scanned short from function
//*********************************************************************************************************************
//

short sParseGps(char * pcTarget, char * pcSource, short sField, short sSize)
{
   short                         sRc                        = 0;
   char                        * pP                         = pcSource;
   char                        * pQ                         = pcSource;
   short                         sCount                     = 0;           // Delimiter index
   int                           i                          = 0;
   int                           j                          = 0;
   char                          cTemp[10]                  = {0};
   char                          cTemp2[20]                  = {0};
   
   if (pcSource[0] != 'G')
      return sRc;
   do
   {
      if (*pP == ',')
         sCount++;
      pP++;
   } while (sCount < sField);

   if (pcTarget)
   {
      memset(pcTarget, 0x00, sSize + 1);
      strncpy(pcTarget, pP, sSize);
      if (strchr(pcTarget, ','))       // Invalid buffer
         memset(pcTarget, 0x00, sSize);
   }
   else
   {
      if (*pP != ',')
      {
         i = 0;
         memset(cTemp2, 0x00, 20);
         while (1)
         {
            cTemp2[i] = *(pP + i);
            if ((cTemp2[i] == ',') || (cTemp2[i] == '*') || (i > 18))
            {
               cTemp2[i] = 0x20;
               break;
            }
            i++;
         }
         if (cTemp2[0] == '0')
            cTemp2[0] = 0x20;
         sscanf(cTemp2, "%d", &sRc);
      }
      else
         sRc = 0;
   }
   return sRc;
}

//*********************************************************************************************************************
// Convert GPS position string to signed double
//
//*********************************************************************************************************************
//

double dGetDec(char * cPos)
{
   double                        dPos                       = 0;
   double                        dDeg                       = 0;
   double                        dMin                       = 0;
   char                          cTemp[20]                  = {0};
   char                        * pP                         = NULL;
   char                        * pQ                         = NULL;
   char                          cMins[10]                  = {0};
   char                          cDegrees[5]                = {0};

   // Check for valid input
   if (!cPos)
      return (0);
   if ((cPos[7] > 0x2f) && (cPos[7] < 0x3a))       // Input string should have a number here
   {
      strcpy(cTemp, cPos);
      pP = strchr(cTemp, '.');
      if (pP)
      {         
         pP -= 2;         
         strncpy(cMins, pP, 7);
         *pP = 0;        
         strcpy(cDegrees, cTemp);
                             
         sscanf(cDegrees + 1, "%f", &dDeg);
         sscanf(cMins, "%f", &dMin);
         dPos = dMin / 60.0;
         dPos += dDeg;
                                  
         if ((*cPos == 'W') || (*cPos == 'S'))
            dPos = -dPos;
      }     
   }
   return dPos;
}

//*********************************************************************************************************************
// Parse GSV sats in view
//
//*********************************************************************************************************************
//

void vParseGsv(void)
{
   uint8_t                       i                          = 0;    
   char                          cTemp[128]                 = {0};
   char                          cTemp2[128]                = {0};
   uint8_t                       j                          = 0;
  
   // sDataTemp  128 slot temporary array, only 32 is used for GPS
   
   if (appData.cGpsGsv[1] == 'P')      // Only handling GPS for now
   {               
      // Get line number
      sParseGps(cTemp, appData.cGpsGsv, 2, 1);
      appData.cGsvline = atoi(cTemp);
      
      if (appData.cGpsGsv[8] == '1')             // First sentence in cycle
      {   
         i = 0;
         appData.uiOffset = 0;        
         memset(appData.sDataTemp, 0x00, sizeof(SAT_DATA) * 128);    // Clear temp buffer
         sParseGps(cTemp, appData.cGpsGsv, 1, 1);
         appData.cGsvtotal = atoi(cTemp);
         appData.cGpsArrayReady = 0;
         // Get total number of sats seen
         sParseGps(cTemp, appData.cGpsGsv, 3, 2);
         appData.sGpsSatsSeen = atoi(cTemp);         
      }
                          
      // Parse into temp buffer
      for (j = 0; j < 4; j++)
      {   
         memset(cTemp, 0x00, 10);
         sParseGps(cTemp, appData.cGpsGsv, 4 + (j * 4), 2);
         appData.sDataTemp[appData.uiOffset].iSat = atoi(cTemp);
         memset(cTemp, 0x00, 10);
         sParseGps(cTemp, appData.cGpsGsv, 5 + (j * 4), 2);
         appData.sDataTemp[appData.uiOffset].iElevation = atoi(cTemp);       
         memset(cTemp, 0x00, 10);
         sParseGps(cTemp, appData.cGpsGsv, 6 + (j * 4), 3);
         appData.sDataTemp[appData.uiOffset].iAzimuth = atoi(cTemp);
         memset(cTemp, 0x00, 10);
         sParseGps(cTemp, appData.cGpsGsv, 7 + (j * 4), 2);
         if (cTemp[0] != '*')
            appData.sDataTemp[appData.uiOffset].iSignal = atoi(cTemp);
         else
            appData.sDataTemp[appData.uiOffset].iSignal = 0;
         appData.uiOffset++;         
      }   
      
      if (appData.cGsvline == appData.cGsvtotal)   // This was last line
      {
         memset(appData.sData, 0x00, 32);
         memcpy(appData.sData, appData.sDataTemp, appData.sGpsSatsSeen * sizeof(SAT_DATA));
         appData.cGpsArrayReady = 1;         
      }
   }
}   
   
//*********************************************************************************************************************
// Parse Gps buffers
//
//*********************************************************************************************************************
//

void vParseGpsBuffers(void)
{
   char                          cTemp[20]                  = {0};
    
   if (appData.cGpsRmc[0] == 'G')
   {   
      sParseGps(appData.cGpsTime, appData.cGpsRmc, 1, 6);
      sParseGps(appData.cGpsDate, appData.cGpsRmc, 9, 6);
      sParseGps(appData.cGpsLatitude, appData.cGpsRmc, 4, 1);
      sParseGps(appData.cGpsLatitude + 1, appData.cGpsRmc, 3, 9);
      sParseGps(appData.cGpsLongitude, appData.cGpsRmc, 6, 1);
      sParseGps(appData.cGpsLongitude + 1, appData.cGpsRmc, 5, 10);  
      appData.cGpsRmc[0] = 0;
   }   
   if (appData.cGpsVtg[0] == 'G')
   {        
      sParseGps(appData.cGpsSpeed, appData.cGpsVtg, 7, 4);
      if (strlen(appData.cGpsSpeed) < 2)
         strcpy(appData.cGpsSpeed, "0.0");
      sParseGps(appData.cGpsBearing, appData.cGpsVtg, 1, 3);
      if (strlen(appData.cGpsBearing) < 2)
         strcpy(appData.cGpsBearing, "0.0");      
      sscanf(appData.cGpsSpeed, "%lf", &appData.fGpsSpeed);
      sscanf(appData.cGpsBearing, "%lf", &appData.fGpsBearing);      
      appData.cGpsVtg[0] = 0;
   }
   if ((appData.cGpsGga[0] == 'G') && (strlen(appData.cGpsGga) > 60))
   {
      appData.sGpsSatsUsed = sParseGps(NULL, appData.cGpsGga, 7, 1);
      appData.dGpsAlt = sParseGps(NULL, appData.cGpsGga, 9, 1);      
      appData.cGpsGga[0] = 0;
   }  
   
   if (appData.cGpsGsa[0] == 'G')
   {   
      appData.sGpsMode = sParseGps(NULL, appData.cGpsGsa, 2, 1);
      sParseGps(cTemp, appData.cGpsGsa, 15, 4);
      sscanf(cTemp, "%lf", &appData.dGpsDop);      
      appData.cGpsGsa[0] = 0;
   }   
      
   if (appData.sGpsMode > 1)
   {
      memcpy(appData.cGpsLastTime, appData.cGpsTime, 6);
      memcpy(appData.cGpsLastDate, appData.cGpsDate, 6);
      if (appData.cPrevGpsMode < 2)
      {   
         appData.cPrevGpsMode = appData.sGpsMode;
         appData.ulGpsInitFixTime = appData.ulLonglife;
      }   
   }
   // Populate decimal position
   if (strlen(appData.cGpsLatitude))
      appData.dGpsLatDec = dGetDec(appData.cGpsLatitude);
   if (strlen(appData.cGpsLongitude))
      appData.dGpsLonDec = dGetDec(appData.cGpsLongitude); 
}

//*********************************************************************************************************************
// Parse GPS buffer
// P: GPS, SBAS   L: GLONASS   A: Galileo   B: Beidou   N: Combinations
//*********************************************************************************************************************
//

void vGpsFlush(void)
{
   if (!strncmp(appData.pcNmeaBuffer + 2, "GSA", 3))
   {   
      memset(appData.cGpsGsa, 0x00, HALF_BUF);
      strncpy(appData.cGpsGsa, appData.pcNmeaBuffer, appData.uiGpsRxptr);
   }   
   else if (!strncmp(appData.pcNmeaBuffer + 2, "GGA", 3))
   {
      memset(appData.cGpsGga, 0x00, HALF_BUF);      
      strncpy(appData.cGpsGga, appData.pcNmeaBuffer, appData.uiGpsRxptr);
   }
   else if (!strncmp(appData.pcNmeaBuffer + 2, "VTG", 3))
   {
      memset(appData.cGpsVtg, 0x00, HALF_BUF);
      strncpy(appData.cGpsVtg, appData.pcNmeaBuffer, appData.uiGpsRxptr);
   }
   else if (!strncmp(appData.pcNmeaBuffer + 2, "RMC", 3))
   {
      memset(appData.cGpsRmc, 0x00, HALF_BUF);    
      strncpy(appData.cGpsRmc, appData.pcNmeaBuffer, appData.uiGpsRxptr);
   }
   else if (!strncmp(appData.pcNmeaBuffer + 2, "GLL", 3))
   {
      memset(appData.cGpsGll, 0x00, HALF_BUF);      
      strncpy(appData.cGpsGll, appData.pcNmeaBuffer, appData.uiGpsRxptr);
   }
   else if (!strncmp(appData.pcNmeaBuffer + 2, "GSV", 3)) 
   {       
      memset(appData.cGpsGsv, 0x00, HALF_BUF);      
      strncpy(appData.cGpsGsv, appData.pcNmeaBuffer, appData.uiGpsRxptr);  
      vParseGsv();   // Parsed directly
   }
}   
  
//*********************************************************************************************************************
// Receive a character from the GPS. Call to interpret if complete sentence. 
// 
//*********************************************************************************************************************
//

void APP_GpsReceiveEventHandler(SYS_MODULE_INDEX index)    // GPS USART 2 
{
   unsigned char ucTemp[20] = {0}; 
   unsigned char ucChar;
      
   ucChar = DRV_USART_ReadByte(appData.drvGpsHandle);  
   if (!appData.uiGpsSync)       // Waiting for sync 0x0a
   {
      if (ucChar == 0x0a)
      {
         memset(appData.pcNmeaBuffer, 0x00, 256);
         appData.uiGpsSync = 1;
         appData.uiGpsRxptr = 0;
      }   
      else   
      {
      }   
   }
   else
   { 
      if (appData.uiGpsRxptr < 256)
      {   
         if (ucChar != '$')
         {
            appData.pcNmeaBuffer[appData.uiGpsRxptr] = ucChar;      

            if (appData.pcNmeaBuffer[appData.uiGpsRxptr] == 0x0d)  // End of sentence
            {
               appData.pcNmeaBuffer[appData.uiGpsRxptr] = 0x00;
               vGpsFlush();                   
               appData.uiGpsRxptr = 0;
               appData.uiGpsSync = 0;
            }
            else
               appData.uiGpsRxptr++;
         }   
      }
      else
         appData.uiGpsSync = 0;
   }   
}
   
//*********************************************************************************************************************
//  
//*********************************************************************************************************************
//
 
void vGps_Initialize(void)
{  
   appData.drvGpsHandle = DRV_USART_Open(DRV_USART_INDEX_0, DRV_IO_INTENT_READWRITE | DRV_IO_INTENT_NONBLOCKING);
   if(DRV_HANDLE_INVALID == appData.drvGpsHandle) 
   {
      // kbdoo message                  
   }
   else
   {
   }        
   
   // Register UART callback
   DRV_USART_ByteReceiveCallbackSet(DRV_USART_INDEX_0, APP_GpsReceiveEventHandler);
   appData.uiGpsRxptr = 0;  
   appData.uiGpsSync = 0;     // Wait for sync
   appData.uiGpsState = 0;
}

//*********************************************************************************************************************
//  Send command buffer (not used, not sending commands to GPS currently)
//*********************************************************************************************************************
//

void vGps_sendcommand(char * ucBuff)
{
   unsigned int                  uiLength                   = 0;
   unsigned int                  i                          = 0;
      
   sprintf(appData.ucGpsTxbuff, "AT%s\r\n", ucBuff);
   uiLength = strlen(ucBuff);
   uiLength += 4;
   
  /*
   if (appData.drvUsartHandle == DRV_HANDLE_INVALID)
   {   
      vDebug("---Oh no!", 2); kbdoo
      vLongDelay(200);
      vLongDelay(200);
   }
   */
   
   //uiLength--;
   /*
   while (uiLength > 0)
   {   
      if(!(DRV_USART_TRANSFER_STATUS_TRANSMIT_FULL & DRV_USART_TransferStatus(appData.drvGpsHandle)))
      {           
         //DRV_USART_WriteByte(appData.drvUsartHandle, appData.ucWtxbuff[i]);         
         PLIB_USART_TransmitterByteSend(USART_ID_6, appData.ucGpsTxbuff[i]);
         
         i++;
         uiLength--;
      }
   } 
    */
}

//*********************************************************************************************************************
// 
//*********************************************************************************************************************
//

void vSendGpsPosition(void)
{
   GPS_POS_MSG sGpsPosMsg;
   
   //memset((void *)&sGpsPosMsg, 0x00, sizeof(GPS_POS_MSG));
   
   sGpsPosMsg.dGpsLatDec = appData.dGpsLatDec;                // Decimal latitude
   sGpsPosMsg.dGpsLonDec = appData.dGpsLonDec;                // Decimal longitude   
   sGpsPosMsg.fGpsSpeed = appData.fGpsSpeed;                  // Speed
   sGpsPosMsg.fGpsBearing = appData.fGpsBearing;              // Bearing   
   sGpsPosMsg.sGpsSatsUsed = appData.sGpsSatsUsed;            // Sats used
   sGpsPosMsg.sGpsSatsSeen = appData.sGpsSatsSeen;            // GPS sats currently seen
   sGpsPosMsg.dGpsDop = appData.dGpsDop;                      // Current DOP
   sGpsPosMsg.dGpsAlt = appData.dGpsAlt;                      // Altitude
   sGpsPosMsg.dGpsMode = appData.sGpsMode;                    // Gps mode    
   memcpy(sGpsPosMsg.cMaidenhead, appData.cMaidenhead, 8);    // Maidenhead grid locator    
   vCanSendBuffer(&sGpsPosMsg, sizeof(sGpsPosMsg), CID_GPS_POS_H);   
}         

//*********************************************************************************************************************
// 
//*********************************************************************************************************************
//
        
void vSendGpsView(void)
{
   unsigned int                  uiArraySize                = 0; 
    
   if ((appData.uiDisplayScreen != SCREEN_GEIGER) || (!(sConfig.cConfig1 & CFG_GEIGER)))  // Not when sending ticks
   {   
      uiArraySize = sizeof(SAT_DATA) * SAT_ARRAYSIZE;
      appData.pcSatArray = malloc(uiArraySize);
      if (appData.pcSatArray != NULL)
      {   
         memcpy(appData.pcSatArray, appData.sData, uiArraySize);                  
         vCanSendBuffer(appData.pcSatArray, uiArraySize, CID_GPS_SATS_H);         
         free(appData.pcSatArray);   
      }                           
   }   
   appData.cGpsSendCount = 0;
}

//*********************************************************************************************************************
// 
//*********************************************************************************************************************
//

void vGps_State()
{          
   uint8_t                       cI                         = 0;
   long                          lAccu                      = 0;
   uint8_t cTemp[30];
      
  	switch (appData.uiGpsState)
   {
      case 0:        // Start
         appData.uiGpsState = 1;         
      break;
      case 1:       
         // Dump to display 
         memset(appData.uiDebugbuffer2, 0x00, 17);
         memset(appData.uiDebugbuffer3, 0x00, 17);
         if (appData.sGpsMode > 1)     // We have a fix
         {   
            sprintf(appData.uiDebugbuffer2, "%f %.1fm%c", appData.dGpsLatDec, appData.dGpsAlt, '\0');
            sprintf(appData.uiDebugbuffer3, "%f %.2f%c", appData.dGpsLonDec, appData.dGpsDop, '\0');
         }
         else
         {
            // No fix, give sat data
            sprintf(appData.uiDebugbuffer2, "Gp:%d Gl:%d %c", appData.sGpsSatsSeen, appData.sGlonassSatsSeen, '\0');
            sprintf(appData.uiDebugbuffer3, "Ga:%d Be:%d %c", appData.sGalileoSatsSeen, appData.sBeidouSatsSeen, '\0');                        
         }   
         appData.uiGpsState = 4;  
      break;                         
      case 4: 
         vParseGpsBuffers();                          
         appData.uiGpsState = 5;  
      break;           
      case 5: 
         vCalculateGrid();                          
         appData.uiGpsState = 6;  
      break;           
      case 6:          
         vSendGpsPosition();         
         appData.uiGpsState = 7;  
      break;        
      case 7: 
         // Send satellites in view                      
         if (appData.cGpsSendCount > 20)
         {               
            if (appData.cGpsArrayReady)
               vSendGpsView();
            
            // Get signal strength index, display
            for (cI = 0; cI < SAT_ARRAYSIZE; cI++)            
            lAccu += appData.sData[cI].iSignal;               
            //sprintf(appData.uiDebugbuffer7, "Sg: %d \0", 
            //   lAccu / (appData.sGpsSatsSeen + appData.sGlonassSatsSeen + appData.sGalileoSatsSeen + appData.sBeidouSatsSeen));                                        
         }
         else
            appData.cGpsSendCount++;
         
         appData.uiGpsState = 1;  
      break;           
   }   
}

//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//










