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


// #define USE_TINYTRAK

    
#include "app.h"
#include "aprs.h"

//*********************************************************************************************************************
// Globals
// 
//*********************************************************************************************************************
//
   
extern APP_DATA                        appData;
extern NVM_CONFIG                      sConfig;

//*********************************************************************************************************************
//  Send command buffer 
//*********************************************************************************************************************
//

void vAprs_sendcommand(char * ucBuff)
{
   unsigned int                  uiLength                   = 0;
   unsigned int                  i                          = 0;

   strcpy(appData.ucAprsTxbuff, ucBuff);
   uiLength = strlen(ucBuff);   

   if (appData.drvAprsHandle == DRV_HANDLE_INVALID)
   {      
   }
   else
   {        
      while (uiLength > 0)
      {   
         uiLength--;
         while (DRV_USART_TRANSFER_STATUS_TRANSMIT_FULL & DRV_USART_TransferStatus(appData.drvAprsHandle))
         {            
         }          
         PLIB_USART_TransmitterByteSend(USART_ID_4, appData.ucAprsTxbuff[i]);
         i++;
      } 
   }   
}


#ifdef USE_TINYTRAK
  
   appData.ucAprsTxbuff[1] = 0x02;
    
#endif

//*********************************************************************************************************************
// Decompress position
// 
//*********************************************************************************************************************
//**************************************************************************
// Format, compression type
/*
 In all cases the compressed format is a fixed 13-character field:
/YYYYXXXX$csT
where / is the Symbol Table Identifier
YYYY is the compressed latitude
XXXX is the compressed longitude
$ is the Symbol Code
cs is the compressed course/speed or
compressed pre-calculated radio range or
compressed altitude
T is the compression type indicator

/ : Symbol table
YYYY: Compressed lat
XXXX: Compressed long
$ : Symbol code
cs: course/speed, radio range, or altitude
T : compression type 
 
Bytes: 1 4 4 1 2 1

Compressed format can be used in place of lat/long position format anywhere
that ...ddmm.hhN/dddmm.hhW$xxxxxxx... occurs.
All bytes except for the / and $ are base-91 printable ASCII characters
(!..{). These are converted to numeric values by subtracting 33 from the
decimal ASCII character code. For example, # has an ASCII code of 35, and
represents a numeric value of 2 (i.e. 35-33).
Symbol The presence of the leading Symbol Table Identifier instead of a digit
indicates that this is a compressed Position Report and not a normal lat/long
report.
*/

/*
To decode a compressed lat/long, the reverse process is needed. That is, if
YYYY is represented as y1y2y3y4 and XXXX as x1x2x3x4, then:
Lat = 90 - ((y1-33) x 913 + (y2-33) x 912 + (y3-33) x 91 + y4-33) / 380926
Long = -180 + ((x1-33) x 913 + (x2-33) x 912 + (x3-33) x 91 + x4-33) / 190463
For example, if the compressed value of the longitude is <*e7 (as computed
above), the calculation becomes:
Long = -180 + (27 x 913 + 9 x 912 + 68 x 91 + 22) / 190463
= -180 + (20346417 + 74529 + 6188 + 22) / 190463
= -180 + 107.25
= -72.75 degrees

*/


/*  Test strings
LA1PCA-4>APTT4,LD1OE*,WIDE2-1:/160352h/06V!Q6(Tj!!_Testing 1-2-3
LE1CTN>U9US87,SAR1-1,LD1OE*,WIDE2-1:`&;:l!w[/"4/}NSOA 3.91V  05.1C
LA1KY-9>U9UU52,LD1OE*,WIDE2-2:`&P>l!g-/>"6:}^
LB0HI-9>APRS,LD1OE*,WIDE2-1:!5957.47N/01055.16E>000/000home
LA1PCA-4>APTT4,LD1OE*,WIDE2-1:/160452h/06V0Q6(Wj!!_Testing 1-2-3
LE1CTN>U9US87,SAR1-1,LD1OE*,WIDE2-1:`&;:l!w[/"4,}NSOA 3.91V  05.1C
LE1CTN>U9US87,SAR1-1,LD1OE,LD2HT-1,WIDE2*:`&;:l!w[/"4,}NSOA 3.91V  05.1C
LA1PCA-4>APTT4,LD1OE*,WIDE2-1:/160552h/06V3Q6(Uj!!_Testing 1-2-3
LA8FTA-7>APDR10-15,LD1OE*,WIDE2-1:!5958.57N\01055.93E-000/000/A=000839Skillebekk QTH
LA1HKA-9>APAT51,LD2KF,LD2HT-1,WIDE2*:!6003.49N/01051.25E(340/000/A=000869DMR og Anl
LA1PCA-4>APTT4,LD1OE*,WIDE2-1:/160653h/06V8Q6(cj!!_Testing 1-2-3
LA3JRA-7>APY878-1,LD3KS,LD1OE*,WIDE2-1:!5944.88N/00959.42E[Tom Portable AT-D878UV
LA1PCA-4>APTT4,LD1OE*,WIDE2-1:/160753h/06V/Q6(Uj!!_Testing 1-2-3
LA7DHA-8>U9UT71,LD1OE*,WIDE2-1:`&F#l bY/>"3r}
LA7DHA-8>U9UT71,LD1OE,LD2HT-1,WIDE2*:`&F#l bY/>"3r}
LE1CTN>U9US87,SAR1-1,LD1OE*,WIDE2-1:`&;:l!w[/"4/}NSOA 3.92V  05.1C
LA1UTX-7>VPPU98,LD1OE*,WIDE2-1:`')>l"m[/`"5o}_0
LA1UTX-7>VPPU98,LD2KF,LD2HT-1,WIDE2*:`')>l"m[/`"5o}_0
LD2KF>APNX03,LD2HT-1,WIDE2*:!6019.45N101208.30E#PHG3710 FILL-IN DIGI
LA1PCA-4>APTT4,LD1OE*,WIDE2-1:/160953h/06V6Q6(Sj!!_Testing 1-2-3
LA1PCA-4>APTT4,LD1OE,LD2HT-1,WIDE2*:/160953h/06V6Q6(Sj!!_Testing 1-2-3 

*/

//*********************************************************************************************************************
// dGpsDistance: Returns distance in kilometres between two positions
// 
//*********************************************************************************************************************
//**************************************************************************
// Decimal degrees to radians

double dDeg2rad(double deg) 
{
   return (deg * PI / 180);
}

//**************************************************************************
// Radians to decimal degrees

double dRad2deg(double rad) 
{
   return (rad * 180 / PI);
}

//**************************************************************************

double dGpsDistance(double lat1, double lon1, double lat2, double lon2) 
{
   double                        theta                      = 0;
   double                        dist                       = 0;
   
   theta = lon1 - lon2;
   dist = sin(dDeg2rad(lat1)) * sin(dDeg2rad(lat2)) + cos(dDeg2rad(lat1)) * cos(dDeg2rad(lat2)) * cos(dDeg2rad(theta));
   dist = acos(dist);
   dist = dRad2deg(dist);
   dist = dist * 111.18957696;    
    
   return (dist);
}

//*********************************************************************************************************************
// dPosToDbl
// Converts position string to double value, converting NMEA to decimal in the process
//*********************************************************************************************************************
//

double dPosToDbl(char * cValue)
{
   char                        * pP                         = 0;  
   double                        dResult                    = 0.0;
   double                        dTemp                      = 0.0;
   char                          cPre[10]                   = {0};         // Degree part incl. sign      
   char                          cPost[10]                  = {0};         // Decimal minutes      
   char                          cTemp[20]                  = {0};         // Temp      
     
   strncpy(cTemp, cValue, 12);
   pP = strchr(cTemp, '.');
   if (pP)
   {
      strncpy(cPre, cTemp, pP - cTemp - 2);
      pP -= 2;
      strncpy(cPost, pP, 10);
      pP = cPre;
      if ((cPre[0] == 'E') || (cPre[0] == 'N'))
         pP++;
      else
         cPre[0] = '-';

      sscanf(pP, "%lf", &dResult);
      sscanf(cPost, "%lf", &dTemp);
      dResult += (dTemp / 60.0);             
   }
   return(dResult);   
}   

//*********************************************************************************************************************
//
//
//*********************************************************************************************************************
//

double dGpsBearingf(double lat1, double lon1, double lat2, double lon2)
{
   double                        dBearing                   = 0.0;

   lat1 = lat1 * (PI / 180.0);
   lat2 = lat2 * (PI / 180.0);
   lon1 = lon1 * (PI / 180.0);
   lon2 = lon2 * (PI / 180.0);
        
   dBearing = atan2(sin(lon2 - lon1) * cos(lat2), (cos(lat1) * sin(lat2)) - (sin(lat1) * cos(lat2) * cos(lon2-lon1)));
   dBearing = dBearing * (180.0 / PI);
   dBearing = fmod((dBearing + 360.0), 360.0); 
   
   return (dBearing);
}


//*********************************************************************************************************************
// Write GPS position
//
//*********************************************************************************************************************
//

void vWriteGpsPosition(void)
{
   unsigned char                 cBuff[40]                  = {0};
   unsigned char                 cBuff2[40]                 = {0};
   
   memcpy(cBuff, appData.cGpsLatitude + 1, 9);
   cBuff[7] = appData.cGpsLatitude[0];
   cBuff[8] = 0x20;
   memcpy(cBuff + 9, appData.cGpsLongitude + 1, 10);
   cBuff[17] = appData.cGpsLongitude[0];
   cBuff[18] = 0;
   
   sprintf(cBuff2, "POSITION %s\x0d\x0a\0", cBuff);
   vAprs_sendcommand(cBuff2);
   vLongDelay(100);
}
   
//*********************************************************************************************************************
// Write Aprs status
//
//*********************************************************************************************************************
//

char cWriteAprsStatus(void)
{
   unsigned char                 ucEnters[]                 = {"\x0d\x0a\0"};
   unsigned char                 cBuff[120]                 = {0};
   unsigned char                 cWaitloop                  = 0;
   char                          cReturn                    = 0;

   while (appData.uiAprsSync) // Wait for incoming to be finished
      vLongDelay(10);

   
   // Check if activity
   for (cWaitloop = 0; cWaitloop < 30; cWaitloop++)    // 3 seconds
   {
      appData.bAprsGot = 0;
      vLongDelay(100);           // 100mS
      if (!appData.bAprsGot)     // No more characters came in
      {
         cWaitloop = 30;   // Exit
      }
   }
   if (!appData.bAprsGot)
   {      
      vAprs_sendcommand(ucEnters);
      vLongDelay(20);
      sprintf(cBuff, "COMMENT %s\x0d\x0a\0", appData.cAprsStatus);
      vAprs_sendcommand(cBuff);
      vLongDelay(150);
      cReturn = 0;
   }
   else
      cReturn = 1;
   return(cReturn);
}

//*********************************************************************************************************************
// Initialize Aprs module
//
//*********************************************************************************************************************
//

void vAprsInit(void)
{
   #define                       RDELAY                     150        
   unsigned char                 ucEnters[]                 = {"\x0d\x0a\0"};
   unsigned short                usI                        = 0;
   unsigned char                 cBuff[30]                  = {0};

   vAprs_sendcommand(ucEnters);
   vLongDelay(200);
   appData.uiAprsSync = 0;       // Starting without capturing sentence
   

   for (usI = 0; usI < APRS_ARRAYSIZE; usI++)
   {
      memset(appData.tAprsEntry[usI].cCallsign, 0x00, 12);
      memset(appData.tAprsEntry[usI].cLatitude, 0x00, 10);
      memset(appData.tAprsEntry[usI].cLongitude, 0x00, 11);
      memset(appData.tAprsEntry[usI].cComment, 0x00, 120);
      memset(appData.tAprsEntry[usI].cSymbol, 0x00, 4);
      appData.tAprsEntry[usI].dDistance = 0.0;
      appData.tAprsEntry[usI].dBearing = 0.0;
      appData.tAprsEntry[usI].sAltitude = 0;
      appData.tAprsEntry[usI].uiTime = 0;
   }
   
   appData.cAprsBeaconInit = 0;
   appData.cAprsBeaconCount = 0;
   appData.cAprsBeaconTimeout = 0;

   // Initialize module  
   //sprintf(cBuff, "RESET%s", ucEnters);
  // vAprs_sendcommand(cBuff);
  // vLongDelay(RDELAY);
   
   sprintf(cBuff, "%s", ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);
                 
   // Use profile 2 temporary
   sprintf(cBuff, "PROFILE 2%s", ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);
   // Write to profile 1
   sprintf(cBuff, "CONFIG 1%s", ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);
   sprintf(cBuff, "MYCALL %s%s%s", sConfig.bCallsign, sConfig.bCall_ext, ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);
   // Nice
   sprintf(cBuff, "NICE 2%s", ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);
   sprintf(cBuff, "ALTITUDE off%s", ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);
   sprintf(cBuff, "GPSDATA off%s", ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);
   sprintf(cBuff, "MONITOR on%s", ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);
   sprintf(cBuff, "COMPRESS off%s", ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);
   sprintf(cBuff, "STATUS 0%s", ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);
   sprintf(cBuff, "SETFREQ %s %s%s", sConfig.bTXfreq, sConfig.bRXfreq, ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);
   sprintf(cBuff, "SETPOWER %c%s", sConfig.bPower, ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);   
   sprintf(cBuff, "ALT off%s", ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);
   sprintf(cBuff, "TEL on%s", ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);
   sprintf(cBuff, "AUTH +%s%s", sConfig.bCallsign, ucEnters);   
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);
   sprintf(cBuff, "PATH %s%s", sConfig.bAprsPath, ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);
   sprintf(cBuff, "INT %d%s", sConfig.bPeriod, ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);
   sprintf(cBuff, "SMARTBCON 5 80 200 30 30%s", ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);
   sprintf(cBuff, "SYMBOL %s%s", sConfig.bAprsSym, ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);
   // Only transmit if GPS is valid
   sprintf(cBuff, "TXNOFIX off%s", ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(RDELAY);
   
   // Use profile 1
   sprintf(cBuff, "PROFILE 1%s", ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(200);

   // Save current config state
   appData.bPrevPower = sConfig.bPower;
   appData.bPrevPeriod = sConfig.bPeriod;
   memcpy(appData.bPrevAprsPath, sConfig.bAprsPath, 12);
   memcpy(appData.bPrevTXfreq, sConfig.bTXfreq, 8);
   memcpy(appData.bPrevRXfreq, sConfig.bRXfreq, 8);
   
}

//*********************************************************************************************************************
// Transmit beacon
//
//*********************************************************************************************************************
//

void vAprsBeacon(void)
{
   unsigned char                 ucEnters[]                 = {"\x0d\x0a\0"};
   unsigned short                usI                        = 0;
   unsigned char                 cBuff[30]                  = {0};

   vAprs_sendcommand(ucEnters);
   vLongDelay(200);
   appData.uiAprsSync = 0;        

   // Force beacon transmit
   LED_YELLOW = LED_ON;
   appData.uiYledcount = appData.ulLife;

   sprintf(cBuff, "BEACON%s", ucEnters);
   vAprs_sendcommand(cBuff);
   vLongDelay(200);
}

//*********************************************************************************************************************
// Remove cells older than APRS_MAXAGE
//
//*********************************************************************************************************************
//

vPruneAprsArray(void)
{
   unsigned int                  uiTime                     = 0;           // Current GPS time
   unsigned short                usI                        = 0;

   sscanf(appData.cGpsTime, "%d", &uiTime);
   for (usI = 0; usI < APRS_ARRAYSIZE; usI++)
   {
      if (!(appData.tAprsEntry[usI].uiTime))
         sscanf(appData.cGpsTime, "%d", &appData.tAprsEntry[usI].uiTime);
      else
      {   
         if ((uiTime - appData.tAprsEntry[usI].uiTime) > (APRS_MAXAGE * 60)) 
         {
            memset(appData.tAprsEntry[usI].cCallsign, 0x00, 12);
            memset(appData.tAprsEntry[usI].cLatitude, 0x00, 10);
            memset(appData.tAprsEntry[usI].cLongitude, 0x00, 11);
            memset(appData.tAprsEntry[usI].cComment, 0x00, 120);
            memset(appData.tAprsEntry[usI].cSymbol, 0x00, 4);
            appData.tAprsEntry[usI].sAltitude = 0;
            appData.tAprsEntry[usI].uiTime = 0;
            appData.tAprsEntry[usI].dDistance = 0.0;
            appData.tAprsEntry[usI].dBearing = 0.0;
         }
      }   
   }
}

//*********************************************************************************************************************
// Locate existing or free cell
//
//*********************************************************************************************************************
//

unsigned short usLocateCell(char * cCallsign)
{
   short                         sCount                     = 0;           // Delimiter index
   unsigned short                usI                        = 0;
   unsigned short                usFree                     = 0;
   unsigned short                usFound                    = 0;
   unsigned short                usFound2                   = 0;
   unsigned short                usFound3                   = 0;
   unsigned int                  uiTime                     = 0;           // Oldest time

   for (usI = 0; usI < APRS_ARRAYSIZE; usI++)
   {
      // Save first free if any
      if ((!strlen(appData.tAprsEntry[usI].cCallsign)) && (!usFound))
      {
         usFree = usI;
         usFound = 1;
      }
      if (!memcmp(appData.tAprsEntry[usI].cCallsign, cCallsign, strlen(cCallsign)))
      {
         if (strlen(appData.tAprsEntry[usI].cCallsign) == strlen(cCallsign))
         {
            usFound2 = usI;
         }
      }
      if ((appData.tAprsEntry[usI].uiTime) && (!uiTime))   // We have a time, and it is the first, it is the oldest per def.
         uiTime = appData.tAprsEntry[usI].uiTime;
      else if ((appData.tAprsEntry[usI].uiTime) && (uiTime))
      {
         if (appData.tAprsEntry[usI].uiTime < uiTime)
         {
            uiTime = appData.tAprsEntry[usI].uiTime;
            usFound3 = usI;                        // Current cell of oldest entry
         }
      }
   }

   // If found existing cell
   if (usFound2)
      usFree = usFound2;
   else if (!usFound)          // If no existing or free, overwrite oldest
   {
      usFree = usFound3;
   }

   if (!usFound2)              // Signal new entry
      appData.cNewseen = 1;
    
   return (usFree);
}

//*********************************************************************************************************************
// Aprs parse helper
//
//*********************************************************************************************************************
//

short vParseAprs(char * pcTarget, char * pcSource, short sField, short sSize)
{
   short                         sRc                        = 0;
   char                        * pP                         = pcSource;
   char                        * pQ;
   short                         sCount                     = 0;           // Delimiter index

   if ((pcSource[0] != 'G') && (pcSource[0] != 'P'))
      return sRc;
   do
   {
      if (*pP == ',')
         sCount++;
      pP++;
   } while (sCount < sField);

   if (pcTarget)
   {
      strncpy(pcTarget, pP, sSize);
      pQ = strchr(pcTarget, ',');
      if (pQ)
         *pQ = 0;
   }
   else
   {
      sscanf(pP, "%d", &sRc);
   }
   return sRc;
}

//*********************************************************************************************************************
// Parse Aprs buffers
//
//*********************************************************************************************************************
//

void vParseAprsBuffers(void)
{
   // Local members before inserting into array
   char                          cCallsign[12]              = {0};         // Callsign
   char                          cLatitude[10]              = {0};         // Latitude
   char                          cLongitude[11]             = {0};         // Longitude
   short                         sAltitude                  = 0;           // Altitude
   char                          cSymbol[4]                 = {0};         // Symbol 0-FFFF
   char                          cComment[120]              = {0};         // Comment
   double                        dLat1                      = 0;
   double                        dLon1                      = 0;
   double                        dLat2                      = 0;
   double                        dLon2                      = 0;
   double                        dDistance                  = 0;
   double                        dBearing                   = 0;
   char                        * pP;
   unsigned short                usI                        = 0;

   char                          cTemp[30]                  = {0};


   if (!appData.uiAprsSync)   // Parse when not actively capturing
   {
      if ((*appData.cAprsRmw) && (*appData.cAprsWpl))
      {
         // Parse fields
         vParseAprs(cLatitude, appData.cAprsWpl, 2, 1);
         vParseAprs(cLatitude+1, appData.cAprsWpl, 1, 7);
         vParseAprs(cLongitude, appData.cAprsWpl, 4, 1);
         vParseAprs(cLongitude+1, appData.cAprsWpl, 3, 8);
         vParseAprs(cCallsign, appData.cAprsWpl, 5, 12);
         sAltitude = vParseAprs(NULL, appData.cAprsRmw, 2, 4);
         vParseAprs(cSymbol, appData.cAprsRmw, 3, 4);
         vParseAprs(cComment, appData.cAprsRmw, 4, 120);
         cComment[119] = 0;
         pP = strstr(cComment, "%)");
         if (pP)
         {
            usI = 0;
            while ((*pP != '(') && (usI < 4))
            {
               pP--;
               usI++;
            }
            if (*pP == '(')
               *pP = 0;
         }
         // Empty buffers
         *appData.cAprsRmw = 0;
         *appData.cAprsWpl = 0;

         // Signal if own callsign
         if (!strncmp(cCallsign, APRS_CALLSIGN, strlen(APRS_CALLSIGN)))
            appData.cOwnseen = 1;
         
         // Insert into array cell
         if (strlen(cCallsign))
         {
            usI = usLocateCell(cCallsign);   // Will return existing, free, or oldest cell
            strncpy(appData.tAprsEntry[usI].cCallsign, cCallsign, 12);
            strncpy(appData.tAprsEntry[usI].cLatitude, cLatitude, 10);
            strncpy(appData.tAprsEntry[usI].cLongitude, cLongitude, 11);
            appData.tAprsEntry[usI].sAltitude = sAltitude;
            strncpy(appData.tAprsEntry[usI].cSymbol, cSymbol, 4);
            strncpy(appData.tAprsEntry[usI].cComment, cComment, 120);
            sscanf(appData.cGpsTime, "%d", &appData.tAprsEntry[usI].uiTime);

            if (appData.sGpsMode > 1)
            {
               // Insert initial calculated distance and bearing
               dLat1 = dPosToDbl(appData.cGpsLatitude);
               dLon1 = dPosToDbl(appData.cGpsLongitude);
               dLat2 = dPosToDbl(appData.tAprsEntry[usI].cLatitude);
               dLon2 = dPosToDbl(appData.tAprsEntry[usI].cLongitude);
               dDistance = dGpsDistance(dLat1, dLon1, dLat2, dLon2);
               dBearing = dGpsBearingf(dLat1, dLon1, dLat2, dLon2);
               appData.tAprsEntry[usI].dDistance = dDistance;
               appData.tAprsEntry[usI].dBearing = dBearing;
            }
         }
         // Prune cells older than APRS_MAXAGE
         vPruneAprsArray();         
      }
   }
}

//*********************************************************************************************************************
// Calculate and store distance and bearing between us and this station
//
//*********************************************************************************************************************
//

void vUpdateDistance(void)
{
   double                        dLat1                      = 0;
   double                        dLon1                      = 0;
   double                        dLat2                      = 0;
   double                        dLon2                      = 0;
   unsigned short                usI                        = 0;
   //char                          cTemp[35]                  = {0};         // Temp

   dLat1 = dPosToDbl(appData.cGpsLatitude);
   dLon1 = dPosToDbl(appData.cGpsLongitude);
   for (usI = 0; usI < APRS_ARRAYSIZE; usI++)
   {
      if (strlen(appData.tAprsEntry[usI].cCallsign) > 2)
      {
         dLat2 = dPosToDbl(appData.tAprsEntry[usI].cLatitude);
         dLon2 = dPosToDbl(appData.tAprsEntry[usI].cLongitude);
         appData.tAprsEntry[usI].dDistance = dGpsDistance(dLat1, dLon1, dLat2, dLon2);
         appData.tAprsEntry[usI].dBearing = dGpsBearingf(dLat1, dLon1, dLat2, dLon2);
      }
   }
}

//*********************************************************************************************************************
//
//
//*********************************************************************************************************************
//


 
//*********************************************************************************************************************
// Receive a character. Call to interpret if complete sentence. 
// 
//*********************************************************************************************************************
//

void APP_AprsReceiveEventHandler(SYS_MODULE_INDEX index)    // APRS USART 4
{
   unsigned char ucTemp[20] = {0}; 
   unsigned char ucChar;

          
   ucChar = DRV_USART_ReadByte(appData.drvAprsHandle);    
   // Check for module
   appData.cIncoming[5] = appData.cIncoming[4];
   appData.cIncoming[4] = appData.cIncoming[3];
   appData.cIncoming[3] = appData.cIncoming[2];
   appData.cIncoming[2] = appData.cIncoming[1];
   appData.cIncoming[1] = appData.cIncoming[0];
   appData.cIncoming[0] = ucChar;
   // Check status
   //if (!memcmp(appData.cIncoming, "Arge", 4))
   
   
   if (!appData.uiAprsSync)       // Waiting for sync 0x0a
   {    
      if (ucChar =='$')
      {
         memset(appData.ucAprsRxbuff, 0x00, 512);
         appData.uiAprsSync = 1;
         appData.uiAprsRxptr = 0; 
         LED_BLUE = LED_ON;         
         appData.uiBledcount = appData.ulLife;
      }   
      else   
      {          
         
      }   
   }
   else
   { 
      if (appData.uiAprsRxptr < 512)
      {   
         appData.ucAprsRxbuff[appData.uiAprsRxptr] = ucChar;      
        
         if (appData.ucAprsRxbuff[appData.uiAprsRxptr] == '*')  // End of sentence
         {
            appData.ucAprsRxbuff[appData.uiAprsRxptr] = 0x00;             
            appData.uiAprsSync = 0;
                              
            if (!strncmp(appData.ucAprsRxbuff, "GPWPL", 5))
            {   
               strcpy(appData.cAprsWpl, appData.ucAprsRxbuff);
            }
            else if (!strncmp(appData.ucAprsRxbuff, "PGRMW", 5))     
            {
               strcpy(appData.cAprsRmw, appData.ucAprsRxbuff);                                  
            }                                
         }
         else
            appData.uiAprsRxptr++;
      }
      else
         appData.uiAprsSync = 0;
   }   
}

//*********************************************************************************************************************
//  
//*********************************************************************************************************************
//
 
void vSendAprsStationArray(void)
{
   unsigned int                  uiArraySize                = 0; 

   if ((appData.uiDisplayScreen != SCREEN_GEIGER) || (!(sConfig.cConfig1 & CFG_GEIGER)))  // Not when sending ticks
   {
      uiArraySize = sizeof(APRS_DATA) * APRS_ARRAYSIZE;
      appData.pcArray = malloc(uiArraySize);
      if (appData.pcArray != NULL)
      {   
         memcpy(appData.pcArray, appData.tAprsEntry, uiArraySize);                  
         vCanSendBuffer(appData.pcArray, uiArraySize, CID_APRS_H);         
      }
      free(appData.pcArray);
   } 
   appData.uiAprsTxCount = 0;   
}
         
//*********************************************************************************************************************
//  
//*********************************************************************************************************************
//
 
void vAprs_Initialize (void)
{       
   appData.drvAprsHandle = DRV_USART_Open(DRV_USART_INDEX_1, DRV_IO_INTENT_READWRITE);
   if (DRV_HANDLE_INVALID == appData.drvAprsHandle) 
   {
      //kbdoo message
      while (1);
   }

   // Register UART callback
   DRV_USART_ByteReceiveCallbackSet(DRV_USART_INDEX_1, APP_AprsReceiveEventHandler);
   
   appData.uiAprsRxptr = 0;  
   appData.uiAprsSync = 0;     // Wait for sync
}

//*********************************************************************************************************************
// 
//*********************************************************************************************************************
//

void vAprs_State()
{         
   static unsigned char          i                          = 0; 
   unsigned char                 ucDebug[128]               = {0};
   unsigned char                 cBuff[128]                 = {0};
   unsigned char                 ucStatus[255]              = {0};
   unsigned char                 ucStartup[10]              = {0};
   unsigned char                 ucEnters[]                 = {"\x0d\x0a\0"};

   
   switch (appData.uiAprsState)
   {
      case 0:     
         if (appData.ulRadioOn)
         {   
            if (appData.ulLonglife > 10)
            {
               vAprsInit();       
               appData.uiAprsState = 2;                       
            }   
         }   
      break;                               

      case 2:        // Start
          
         appData.uiAprsState = 3;         
      break;
      case 3:        // Initial beacon?         
         if ((appData.cAprsBeaconData) && (!(appData.cAprsBeaconInit)) && ((appData.ulLonglife - appData.ulGpsInitFixTime) > 10))
         {                                
            appData.cAprsBeaconInit = 1;          
            vAprsBeacon();            
         }                      
         else if ((appData.ulLonglife > 100) && (!appData.cAprsBeaconTimeout) && (!appData.cAprsBeaconInit))  // Timeout beacon
         {
            appData.cAprsBeaconTimeout = 1;   
            vAprsBeacon();            
         }   
    
         appData.uiAprsState = 4;         
      break;
      case 4:                     
         vParseAprsBuffers();  
          
         appData.uiAprsState = 6;
      break;                               
      case 6:         
         if (appData.cAprsPosCount > 4)
         {
            //vWriteGpsPosition();    // Write GPS position to tracker kbdoo switch off if supplied directly
            appData.cAprsPosCount = 0;
         }   
         else
            appData.cAprsPosCount++;
         
         appData.uiAprsState = 8;  
      break;                         
      case 8:              
         appData.cAprsBeaconCount++;
         if (appData.cAprsBeaconCount > 50)
         {               
            // Custom message?
            if (strlen(appData.cCustomAprsMessage) > 1)  // Send custom message
            {
               strcpy(appData.cAprsStatus, appData.cCustomAprsMessage);
               cWriteAprsStatus();
               memset(appData.cCustomAprsMessage, 0x00, 30);
               appData.ulMsgTime = appData.ulLonglife;
               vAprsBeacon();     
            }            
            else
            {   
               strncpy(ucStartup, "N", 2);
               if (appData.uiStartupflag & STARTUP_TIMER)
                  strncpy(ucStartup, "T", 2);
               if (appData.uiStartupflag & STARTUP_ALARM)
                  strncpy(ucStartup, "A", 2);

               // Don't have registered status
               if (appData.cGsmRegistered == 0)
                  appData.cGsmRegistered =  '-';

               if (appData.sGpsMode > 1)
               {   
                  sprintf(ucStatus, "%.0fmA %.1fkmh %.1fdg %dcpm %.1fnSh %.1fc %.1fmb %s [%sz]", 
                       appData.dCurrent, appData.fGpsSpeed, appData.fGpsBearing, appData.iCountsMin, appData.fRadLevel, 
                       appData.dTemperature, appData.dPressure, ucStartup, appData.cGpsTime);                           
               }
               else
               {
                  sprintf(ucStatus, "%.0fmA %.1fkmh %.1fdg %dcpm %.1fnSh %.1fc %.1fmb %s", 
                       appData.dCurrent, appData.fGpsSpeed, appData.fGpsBearing, appData.iCountsMin, appData.fRadLevel, 
                       appData.dTemperature, appData.dPressure, ucStartup);                                                         
               }                        

               if (memcmp(ucStatus, appData.cAprsStatus, strlen(ucStatus))) 
               {                     
                  if ((appData.ulMsgTime) && ((appData.ulLonglife - appData.ulMsgTime) < 300))  // 5 minutes
                  {
                     // Custom message exists, don't overwrite for t seconds
                  }
                  else
                  {   
                     appData.ulMsgTime = 0;
                     strcpy(appData.cAprsStatus, ucStatus);
                     cWriteAprsStatus();

                     if (appData.sGpsMode > 1)
                     {                     
                        appData.cAprsBeaconData = 1;
                     } 
                  }   
               }
               appData.cAprsBeaconCount = 0;
            }
         }   
         appData.uiAprsState = 10;  
      break;       
      case 10:                     
         // Send station array over CAN
         if (appData.uiAprsTxCount > 20)
         {   
            vSendAprsStationArray();
         }   
         else
            appData.uiAprsTxCount++;
         appData.uiAprsState = 12;
      break;    
      case 12:                     
         // Write changed settings   
         if (appData.bPrevPower != sConfig.bPower)
         {
            appData.bPrevPower = sConfig.bPower;
            sprintf(cBuff, "SETPOWER %c%s", sConfig.bPower, ucEnters);
            vAprs_sendcommand(cBuff);
         }   
         
         if (appData.bPrevPeriod != sConfig.bPeriod)
         {
            appData.bPrevPeriod = sConfig.bPeriod;
            sprintf(cBuff, "INT %d%s", sConfig.bPeriod, ucEnters);
            vAprs_sendcommand(cBuff);
         }   
         
         if (memcmp(appData.bPrevAprsPath, sConfig.bAprsPath, 12))
         {
            memcpy(appData.bPrevAprsPath, sConfig.bAprsPath, 12);
            sprintf(cBuff, "PATH %s%s", sConfig.bAprsPath, ucEnters) ;
            vAprs_sendcommand(cBuff);
         }   
         
         if (memcmp(appData.bPrevTXfreq, sConfig.bTXfreq, 8))
         {
            memcpy(appData.bPrevTXfreq, sConfig.bTXfreq, 8);
            sprintf(cBuff, "SETFREQ %s %s%s", sConfig.bTXfreq, sConfig.bRXfreq, ucEnters);
            vAprs_sendcommand(cBuff);
         }   
         
         if (memcmp(appData.bPrevRXfreq, sConfig.bRXfreq, 8))
         {
            memcpy(appData.bPrevRXfreq, sConfig.bRXfreq, 8);
            sprintf(cBuff, "SETFREQ %s %s%s", sConfig.bTXfreq, sConfig.bRXfreq, ucEnters);
            vAprs_sendcommand(cBuff);
         }                     
         appData.uiAprsState = 20;
      break;    
      case 20:
         // Display
        // sprintf(appData.uiDebugbuffer7, "%s %.1f", appData.tAprsEntry[0].cCallsign, appData.tAprsEntry[0].dDistance);                 
         appData.uiAprsState = 2;
      break;     
   }   
}

//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//










