
//*********************************************************************************************************************
// Change notifications
//
//*********************************************************************************************************************
//

#ifndef _CAN_H_
#define _CAN_H_ 
  
 
//*********************************************************************************************************************
// CAN message ID defines
//    1 -  99  Housekeeping
//  100 - 199  Gauge -> Sensors 
//  200 - 299  Sensors -> Gauge
//
//*********************************************************************************************************************
//

// Receive state
#define                       CST_SINGLE                 000            // Receiving single messages
#define                       CST_SEQUENCE               001            // Sequence of protocol

// Housekeeping
#define                       CID_ALIVE_H                001            // Keep-alive message header
#define                       CID_ALIVE_B                002            // Keep-alive message body

// Gauge -> Sensors
#define                       CID_CONFIG_H               101            // Shared configuration header
#define                       CID_CONFIG_B               102            // Shared configuration body
#define                       CID_SCREEN                 103            // VDU sends screen number to sensors
#define                       CID_RESET                  104            // VDU sends reset command to sensors

// Sensors -> Gauge
#define                       CID_STATUS                 200            // C8 General status

#define                       CID_GEIGER_H               201            // C9 Geiger data header
#define                       CID_GEIGER_B               202            // CA Geiger data body
#define                       CID_TICK                   203            // CB Geiger tick 

#define                       CID_GPS_SATS_H             210            // D2 Satellites in view header
#define                       CID_GPS_SATS_B             211            // D3 Satellites in view body
#define                       CID_GPS_POS_H              212            // D4 Position header
#define                       CID_GPS_POS_B              213            // D5 Position body

#define                       CID_GYRO                   214            // D6 Gyro attitude header
 
#define                       CID_GSMS_H                 216            // GSM signal carrier header
#define                       CID_GSMS_B                 217            // GSM signal carrier body
#define                       CID_GSM_H                  218            // GSM SMS message header
#define                       CID_GSM_B                  219            // GSM SMS message body

#define                       CID_APRS_H                 220            // DC APRS station data header
#define                       CID_APRS_B                 221            // DD APRS station data body

#define                       CID_BT_H                   222            // BT data header
#define                       CID_BT_B                   223            // BT data body


/*
char, signed char                      8     1
unsigned char                          8     1
short, signed short                    16    2
unsigned short                         16    2
int, signed int, long, signed long     32    4
unsigned int, unsigned long            32    4
long long, signed long long            64    8
unsigned long long                     64    8

float                                  32    4
double                                 32    4
long double                            64    8
 
*/

// Protocol structures

// 001 CID_ALIVE
typedef struct
{
   // Time, temp, random
   uint8_t                          cTime[6];
   uint8_t                          cDate[6];
   double                           dGsmTemp;
   short                            iGyroTemp;
   float                            fVoltage;
   long long                        llRandom;
   // Power/Pressure data (v4)
   double                           dTemperature;
   double                           dPressure;
   double                           dVoltage;
   double                           dCurrent;
   uint8_t                          cSignals;
} LIFE_MSG;   

// 101 Config from gauge to sensors
typedef struct
{
   uint8_t                          cCfgBits;                     // Switch bits
   uint8_t                          cRadioPwr;                    // Radio power level
   uint8_t                          cFrequency[8];                // Radio frequency 'ttt.ttt rrr.rrr'
   uint8_t                          cCallsign[8];                 // Callsign
   uint8_t                          cAprsPath[18];                // APRS path
   
} CONFIG_MSG;

// 201 CID_GEIGER
typedef struct
{
   int                              iCounts;                      // Counts 
   int                              iCounts5;                     // Counts last 5 seconds 
   int                              iCountsMin;                   // Calculated counts per minute
   int                              iAve10;                       // Average CPM last 10 minutes
   float                            fRadLevel;                    // Radiation level uSi
   float                            fRad10;                       // Radiation level uSi average over 10 minutes
} GEIGER_MSG;

// 212 CID_GPS_POS
typedef struct
{
   double                           dGpsLatDec;                   // Decimal latitude
   double                           dGpsLonDec;                   // Decimal longitude   
   double                           fGpsSpeed;                    // Speed
   double                           fGpsBearing;                  // Bearing   
   short                            sGpsSatsUsed;                 // Sats used
   short                            sGpsSatsSeen;                 // GPS sats currently seen
   double                           dGpsDop;                      // Current DOP
   double                           dGpsAlt;                      // Altitude
   short                            dGpsMode;                     // Gps mode   
   uint8_t                          cMaidenhead[8];               // Maidenhead grid locator
} GPS_POS_MSG;

  
#endif


