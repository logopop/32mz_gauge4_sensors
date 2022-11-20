//*********************************************************************************************************************
//
  

#ifndef _APP_H
#define _APP_H


//*********************************************************************************************************************
// Pinout VGA:
//
//  1 : GND          9 : Aprs Out      
//  2 : +12v         10: Aprs In
//  3 : Ignition     11: Extra relay
//  4 : Alarm        12: Radio relay
//  5 : GND          13: +12v
//  6 : CANH         14: GND
//  7 : CANL         15: GND
//  8 : GPS Out
//
//
//*********************************************************************************************************************
// 
//
//
//
// To do:
//
// Display:
// LED signal when new APRS station added
// LED signal when we hear ourselves repeated
// * SMS configurable startscreen
// * Improve APRS signal scale, too low now
// 
//
// Sensors:
// Command shortforms
// * SMS ignore space after 'jp'
// SMS settings: gyro pace and thresholds
// * Battery sense alarm after a time delay 
// Help screen SMS
// Power: duty cycle for 4G (N/A)
// * Turn on radio if off and receiving beacon command
// * Turn on radio after 4 minutes, independent of start up mode
// Send status that radio is on
// * Start communicating with radio only if it is on
// Aprs on? Send beacon on shutdown
//
// 
// LED signals:    (0)
//                 (1)   (2)
//  
// (0)   Red    : Altitude calibration starts
// (0)   Blue   : Altitude calibration done
// (0)   Purple : Geiger tick in subscreen
// (0)   Purple : NVram changed
// (0)   Green  : NVram being written
// (0)   Green  : Incoming SMS
// (0)   Various: Pitch/Roll alarms
// (0)   Various: Altitude change in position subscreen
//
// (1)   Blue   : In sub-menu
// (1)   Aquam. : In sub-sub setting
// (1)   Green  : Seen new APRS station
//
// (2)   Red    : Exception
// (2)   Blue   : No ham database record found
// (2)   Blue   : WiFi Connecting
// (2)   Green  : WiFi Connected
// (2)   White  : Incoming WiFi connection
// (2)   White  : Request sent 
// (2)   White  : Restart
//
//*********************************************************************************************************************
// SMS commands
// jp                   : Get standard reply
// jp a(prs)            : Turn on APRS radio
// jp b(eacon)          : Force beacon. Turn on radio first if necessary
// jp o(ff)             : Turn sensor board off if ignition is not on
// jp m<message>        : Set custom APRS message
// jp x1                : Accessory relay on
// jp x0                : Accessory relay off
// 
// When gauge is on:
// !+                   : Keyword to set a configuration element
// !+b11.8              : Set battery alarm threshold
// !+g100               : Gyro multiplier, 100 is neutral
// !+p100               : Pressure variation multiplier, 100 is neutral
// !+s4                 : Set startscreen < 14
// !+t+4746644580       : Set owner phone number
//
//
//
//*********************************************************************************************************************
//
 
#include       <xc.h>
#include       <p32xxxx.h>
#include	      <ctype.h>
#include       <stdio.h>
#include       <string.h>
#include       <stdlib.h>
#include       <math.h>
#include       <float.h> 
#include       <stdbool.h>
#include       <stddef.h>                     // Defines NULL
#include       "system_definitions.h"
#include       "system/common/sys_module.h"   // SYS function prototypes
#include       <peripheral/peripheral.h>
#include       <stddef.h>                     // Defines NULL
#include       <stdint.h>
 
#include       "peripheral/adc/plib_adc.h"    // plib ADC
#include       "peripheral/devcon/plib_devcon.h"
#include       "peripheral/reset/plib_reset.h"

#include       "og_hardware.h"
#include       "delays.h"
#include       "can.h"
#include       "display.h"
#include       "ssd1306_drv.h"

//*********************************************************************************************************************
// Defines
//
//*********************************************************************************************************************
//

#define                          PI                         3.14159265358979323846              // Pi

#define                          MAX_BUF                    256
#define                          HALF_BUF                   128

// Relay flags
#define                          REL_RADIO                  0x0001
#define                          REL_ACC                    0x0002


// Temp preesure sensor
typedef                          signed long                BMP280_S32_t;  

//*********************************************************************************************************************
// APRS

#define                          APRS_ARRAYSIZE             50                   // Number of station cells
#define                          APRS_MAXAGE                20                   // Minutes before entry is cleared
#define                          APRS_CALLSIGN              "LA1PCA\0"           // Callsign
#define                          APRS_CALLSIGN_EXT          "-9\0"               //
#define                          APRS_SYMBOL                "/j"                 // Map symbol (Jeep)
#define                          APRS_PATH                  "WIDE2-2\0"          // Routing path
#define                          APRS_POWER                 1                    // Relative output power
#define                          APRS_PERIOD                61                   // Seconds between non-moving transmissions
#define                          APRS_TXFREQ                "144.800\0"          // Transmit frequency MHz
#define                          APRS_RXFREQ                "144.800\0"          // Receive frequency MHz

//*********************************************************************************************************************
// Bluetooth

#define                          BT_NAME                    "\"Jeep TJ\"\0"      // Device name
#define                          BT_PIN                     "2001"               // Device PIN
#define                          BT_ARRAYSIZE               20                   // Number of devices

//*********************************************************************************************************************
// Geiger

#define                          GEIGER_ARRAYSIZE           120                  // 10 minutes, 5 sec slots
#define                          SIEVERT_RATE               2.85714              // Nanosievert/h per pulse, 2 tubes  

//*********************************************************************************************************************
// WIFI

#define                          WIFI_DNS                   "8.8.8.8\0"          // DNS server              
#define                          WIFI_CLOUD                 "plan9.asuscomm.com\0"    // Cloud server            

//*********************************************************************************************************************
// GSM

#define                          GSM_KEYWORD                "jp\0"               // Keyword to submit commands              
#define                          GSM_PRIMARY                "+4746644580\0"      // Primary 'owner' telephone number
#define                          GSM_SERVICE                "+4792001000\0"      // Service center number
  
//*********************************************************************************************************************
// GPS

#define                          SAT_ARRAYSIZE              32                   // Only GPS sats 

//*********************************************************************************************************************
// NVRAM

//2048
//#define                          NVM_ADDRESS                0x9D1F0000           // Start address 
//512
#define                          NVM_ADDRESS                0x9D07e000           // Start address 
//#define                          NVM_PAGE_SIZE              2048                 // Defined in harmony

#define                          NVM_MAGIC                  0xcafebabe           // Valid config header

#define                          V_CALIBRATE                160.4                // AD voltage measurement factor

 
//*********************************************************************************************************************
// Config settings

// Configuration byte 1

#define                          CFG_OFF                    0b00000000           // All off
#define                          CFG_GEIGER                 0b00000001           // Sensor board geiger sound on
#define                          CFG_AL_AUDIO               0b00000010           // All alarms audio on
#define                          CFG_AL_VISUAL              0b00000100           // All alarms visuals on

// Configuration byte 2

// Startup flag
#define                          STARTUP_IGNITION           0b00000001           // Ignition
#define                          STARTUP_ALARM              0b00000010           // Alarm
#define                          STARTUP_TIMER              0b00000100           // Frequent startup on timer

#define                          BATT_LIMIT                 11.6                 // Battery alarm limit

//*********************************************************************************************************************
// cSignals flags from sensors to display, member of CID_ALIVE

#define                          SIG_APRS_NEW               0b00000001           // New APRS station seen
#define                          SIG_APRS_OWN               0b00000010           // Own call has been repeated

 
//*********************************************************************************************************************
// NVRAM configuration structure
//
//*********************************************************************************************************************
//

typedef struct _Configuration
{
   unsigned int                  uiMagic;                         // Magic number - config exists (unsigned int - 0xcafebabe)
   // APRS
   unsigned char                 bCallsign[8];                    // APRS callsign
   unsigned char                 bCall_ext[4];                    // APRS callsign extension
   unsigned char                 bAprsSym[4];                     // APRS symbol
   unsigned char                 bAprsPath[12];                   // APRS path
   unsigned char                 bPower;                          // Relative TX power
   unsigned int                  bPeriod;                         // Standstill transmission frequency (minutes)
   unsigned char                 bTXfreq[8];                      // TX frequency ASCII
   unsigned char                 bRXfreq[8];                      // RX frequency ASCII
   // Bluetooth
   unsigned char                 bBTname[16];                     // BT device name
   unsigned char                 bBTpin[4];                       // BT pin
   // GSM
   unsigned char                 bGsmPrimary[20];                 // Primary owner phone number
   unsigned char                 bGsmService[20];                 // Cellphone SMS service number
   // WiFi
   unsigned char                 bDnsServer[16];                  // DNS server
   unsigned char                 bCloudServer[40];                // DNS name of cloud info server
   // Gyro level
   short                         iXoff;                           // Roll offset
   short                         iYoff;                           // Pitch offset
   int                           iDispBias;                       // Display contrast bias
   int                           iGeigerCal;                      // Geiger calibration (uSi factor)
   char                          cConfig1;                        // Config bits
   char                          cConfig2;                        // Config bits
   unsigned int                  uiConfig3;                       // Config value future use
   float                         fBatteryLimit;                   // Low level battery alarm limit
   //
   char                          cStartscreen;                    // Start screen  0x00 - 0x0d
   char                          cGyroThreshold;                  // Added alerts threshold bias
   char                          cPressmult;                      // Pressure change multiplier
   char                          cFuture1;                        // Future use
   char                          cFuture2;                        // 
      
} NVM_CONFIG;

//*********************************************************************************************************************
// APP data
//
//*********************************************************************************************************************
//

// GPS sat array
typedef struct
{
   int                              iSat;
   int                              iElevation;
   int                              iAzimuth;
   int                              iSignal;
} SAT_DATA;   

// APRS station array
typedef struct 
{
   uint8_t                          cLatitude[10];                // Latitude
   uint8_t                          cLongitude[11];               // Longitude
   short                            sAltitude;                    // Altitude
   double                           dDistance;                    // Distance from here
   double                           dBearing;                     // Bearing from here
   unsigned int                     uiTime;                       // Time of entry
   uint8_t                          cSymbol[4];                   // Symbol 0-FFFF
   uint8_t                          cCallsign[12];                // Callsign
   uint8_t                          cComment[120];                // Comment
} APRS_DATA;

// Bluetooth device array
typedef struct 
{
   uint8_t                          cName[20];                    // Name of device
   uint8_t                          cAddress[18];                 // Address of device
   uint8_t                          cRssi[4];                     // Signal -127..0
   uint8_t                          cStatus;                      //      
} BT_DATA;

// 216 CID_GSMS 
typedef struct
{
   uint8_t                          cGsmRegistered;               // 1: registered to network
   short                            iGsmSignal;                   // Signal strength  
   uint8_t                          ucGsmOperator[20];            // Operator  
   uint8_t                          cJamming;                     // Jamming status
   short                            iGsmMode;                     // GSM protocol mode (0-24) 
   uint8_t                          cGsmOnline;                   // Module is responding
   short                            iMisc1;                       // Future use
   short                            iMisc2;                       // Future use
} GSMS_MSG;

// 218 CID_GSM 
typedef struct
{
   short                            iGsmMsgSize;                  // Message size
   uint8_t                          ucGsmSender[20];              // Sender phone number
   uint8_t                          ucGsmMessage[160];            // Last message     
} GSM_MSG;

//*********************************************************************************************************************
//

typedef struct
{
   //********************************************************************
   // Main App
   uint8_t                          uiMainState;       
   uint8_t                          uiStartupflag;                // Flags boot reason (ignition, alarm)
   uint8_t                          cPendingShutdown;             // True 15 sec before shutdown
   
   // NVRAM data
   uint8_t                          uiNvramState;
   DRV_HANDLE                       hNvram;                       // Handle to NVRAM driver   
   DRV_HANDLE                       hNvmCmdHandle;                // NVM command handle
   unsigned long                    ulLonglife;                   // Sequence timer counter 1 sec  
   uint8_t                          cVoltalarm;                   // True if voltalarm sent
   uint8_t                          cCaralarm;                    // True if caralarm sent
   
   //********************************************************************
   // I2C
   DRV_HANDLE                       drvI2CHandle1; 
   uintptr_t                        appI2CWriteBufferHandle;
   uintptr_t                        appI2CReadBufferHandle;    
   uint8_t                          uiI2CRxbuffer[32];
   uint8_t                          uiI2CTxbuffer[8192];   
   uintptr_t                        i2cOpStatus;
   uint8_t                          uiDisplayState;
   uint8_t                          uiDisplayDelay;
   uint8_t                          ucDisplayPresent;
   // Message from VDU
   uint8_t                          uiDisplayPrev;                // Previous visible screen      
   uint8_t                          uiDisplayScreen;              // Current visible screen 
   uint8_t                          ucSubScreen;                  // True if in subscreen
   unsigned int                     uiLogoTime;                   // Time delay counter for intro display   
   
   // Debug buffers
   uint8_t                          uiDebugbuffer0[17];
   uint8_t                          uiDebugbuffer1[17];
   uint8_t                          uiDebugbuffer2[17];
   uint8_t                          uiDebugbuffer3[17];
   uint8_t                          uiDebugbuffer4[17];
   uint8_t                          uiDebugbuffer5[17];
   uint8_t                          uiDebugbuffer6[17];
   uint8_t                          uiDebugbuffer7[17];
   
   uint8_t                          uiPrevbuffer0[17];
   uint8_t                          uiPrevbuffer1[17];
   uint8_t                          uiPrevbuffer2[17];
   uint8_t                          uiPrevbuffer3[17];
   uint8_t                          uiPrevbuffer4[17];
   uint8_t                          uiPrevbuffer5[17];
   uint8_t                          uiPrevbuffer6[17];
   uint8_t                          uiPrevbuffer7[17];
       
   //********************************************************************
   // Random
 
   int                              iRandomCnt;
   long long                        llRandom;
   
   //********************************************************************
   // Voltage AD  
  
   unsigned long                    ulADvalue;                    // AD return
   uint8_t                          uiVoltageState;               // 0: idle 1: Start sample 2: Wait         
   uint8_t                          uiVoltageCount;               // Reset safety
   float                            fVoltage;                     // Input voltage
   uint8_t                          cAdAve;                       // Average counter
   float                            fAdAccu;                      // Averaging accumulator
   uint8_t                          cAdAvailable;                 // AD is available
   
   //********************************************************************
   // Dedicated sensors  
  
   double                           dTemperature;
   double                           dPressure;
   double                           dVoltage;
   double                           dCurrent;
   
   //********************************************************************
   // Gyro
   
   uint8_t                          uiGyroState; 
   uint8_t                          uiGyroUp; 
   DRV_HANDLE                       drvGyroHandle;
   uint8_t                          ucBuff[20];
   uint8_t                          ucBuff2[20];
   DRV_SPI_BUFFER_HANDLE            bufferHandle2;
   uint8_t                          uiGyroptr;
   // Data
   short                            iGyroTemp;
   short                            iX;
   short                            iY;
   short                            iZ;
   short                            iXacc[20];
   short                            iYacc[20];
   short                            iXoff;
   short                            iYoff;
    
   //********************************************************************
   // Geiger
   
   uint8_t                          uiGeigerState;
   int                              iCounts;                      // Counts 
   int                              iCounts5;                     // Counts last 5 seconds 
   int                              iCountsMin;                   // Calculated counts per minute
   int                              iAve10;                       // Average CPM last 10 minutes
   float                            fRadLevel;                    // Radiation level uSi
   float                            fRad10;                       // Radiation level uSi 10 min average
   int                              iCountArray[GEIGER_ARRAYSIZE];// Array of 5-second counts
   uint8_t                          uiGeigerPtr;                  // Pointer to slots
   uint8_t                          uiArrayFull;                  // 1 when pointer rolls over
   uint8_t                          uiGeigerTick;                 // 1 when audio tick on      
   uint8_t                          cNewGeigermsg;                // Message ready
   uint8_t                          uiTicks;                      // Ticks to VDU
    
   //********************************************************************
   // CAN
   
   uint8_t                          uiCanState;
   uint8_t                          uiCanUp;                      // 1 if bus available
   uint8_t                          ucCanTXbuff[10];              // CAN TX buffer
   uint8_t                          ucCanRXbuff[10];              // CAN RX buffer
   uint8_t                       *  pucCanTXbuff;                 // Complete TX buffer
   uint8_t                       *  pucCanTXptr;                  // TX pos pointer
   unsigned int                     ucCanCurrent;                 // Current package number, header is 0
   unsigned int                     ucCanTotal;                   // Total number of packages in addition to header
   unsigned long                    ulCanTXtotal;                 // Total packages sent
   unsigned long                    ulCanRXtotal;                 // Total packages received
   
   // Protocol
   uint8_t                       *  pucCanRXbuff;                 // Complete RX buffer
   uint8_t                       *  pucCanRXptr;                  // RX pos pointer
   uint8_t                       *  pCanBuff;                     // Pointer to sequence buffer
   uint8_t                          ucCanTrans;                   // Single message or part of sequence
   uint8_t                          ucCanType;                    // Sequence message type
   unsigned int                     u1CanCurrent;                 // Current package number, header is 0
   unsigned int                     uiCanTotal;                   // Total number of packages in addition to header
   uint8_t                          cCanRemnant;                  // Bytes in last package
   unsigned int                     uiCanSize;                    // Size of transmission, bytes
   uint8_t                          cChecksum;                    // Expected checksum of received sequence
   
   uint8_t                          can_message_buffer0[512];
   uint8_t                          cCanSendScreen;               // VDU screen changed, change message cadence
   
   //********************************************************************
   // GPS 
   
   uint8_t                          uiGpsState;       
   DRV_HANDLE                       drvGpsHandle;
   uint8_t                          ucGpsTxbuff[64];
   unsigned int                     uiGpsRxptr;
   uint8_t                          uiGpsSync;                    // 0: wait for sync 1: Synced
   uint8_t                          uiGsvPtr;                     // Sat in view pointer
   uint8_t                          cGpsTime[7];
   uint8_t                          cGpsDate[7];
   uint8_t                          cGpsLatitude[10];             // N/Sddmm.mmmm
   uint8_t                          cGpsLongitude[11];            // E/Wdddmm.mmmm
   double                           dGpsLatDec;                   // Decimal latitude
   double                           dGpsLonDec;                   // Decimal longitude
   uint8_t                          cGpsSpeed[5];                 // xxx.x km/h
   uint8_t                          cGpsBearing[5];               // xxx.x degrees
   double                           fGpsSpeed;                    // Speed
   double                           fGpsBearing;                  // Bearing   
   uint8_t                          cGpsLastTime[6];              // Last fix time
   uint8_t                          cGpsLastDate[6];              // Last fix date
   short                            sGpsMode;                     // 1: No fix  2:2D  3:3D
   short                            cPrevGpsMode;                 // Previous mode
   unsigned long                    ulGpsInitFixTime;             // Longlife time of first fix
   short                            sGpsSatsUsed;                 // Sats used
   short                            sGpsSatsSeen;                 // GPS sats currently seen
   short                            sGalileoSatsSeen;             // Galileo sats currently seen
   short                            sGlonassSatsSeen;             // Glonass sats currently seen
   short                            sBeidouSatsSeen;              // Beidou sats currently seen
   double                           dGpsDop;                      // Current DOP
   double                           dGpsAlt;                      // Altitude
   uint8_t                          cMaidenhead[8];               // Maidenhead grid locator
   uint8_t                          cGpsSendCount;                // Counter to trigger satellite in view CAN msg 
   uint8_t                          pcNmeaBuffer[MAX_BUF];   
   uint8_t                          cGpsComm;                     // 1:traffic 2:parsed
   uint8_t                        * pcSatArray;                   // CAN sats pointer
   uint8_t                          cGpsGga[HALF_BUF];
   uint8_t                          cGpsGsa[HALF_BUF];
   uint8_t                          cGpsVtg[HALF_BUF];
   uint8_t                          cGpsRmc[HALF_BUF];
   uint8_t                          cGpsGll[HALF_BUF];
   uint8_t                          cGpsGsv[HALF_BUF];
   SAT_DATA                         sData[32];                    // GPS sats only, sopied from sDataTemp
   SAT_DATA                         sDataTemp[128];               // Temporary sats data to be filles
   uint8_t                          cGsvline;                     // GSV sentence number
   uint8_t                          cGsvtotal;                    // Total GSV sentences
   uint8_t                          uiOffset;                     // Current element offset in GSV array
   
   uint8_t                          cGpsArrayReady;               // Array is ready for transmit 
   
   uint8_t                          cFence[64];
           
           
   //********************************************************************
   // GSM 
   
   short                            uiGsmState;   
   uint8_t                          uiGsmOldstate;   
   
   DRV_HANDLE                       drvGsmHandle;
   uint8_t                          ucGsmTxbuff[256];
   uint8_t                          ucGsmRxbuff[256];
   uint8_t                          ucGsmLast[6];                 // Last 4 chars to look for complete command reply   
   uint8_t                          uiGsmRxptr;
   uint8_t                          uiGsmSync;                    // 0: wait for sync 1: Synced
   uint8_t                          uiGsmDouble;                  // 1: expect cr/lf in reply
   uint8_t                          uiGsmGotMsg;                  // Got message, can be deleted
   uint8_t                          uiGsmPending;                 // 1: request is not finished
   uint8_t                          uiGsmGotMsgRdy;               // 1 if previous transaction took place
   uint8_t                          uiGsmPendingTx;               // 1 if TX message waiting on GPS  
   uint8_t                          ucNo_message;                 // 1 when incoming command causes no reply
   short                            iSmsIndex;                    // SMS message index   
   uint8_t                        * pcBtArray;                    // CAN BT devices pointer
          
   // Data
   uint8_t                          cGsmRegistered;               // 1: registered to network
   short                            iGsmMode;                     // GSM protocol mode (0 - 24)
   short                            iGsmSignal;                   // Signal strength
   uint8_t                          ucGsmOperator[20];            // Operator   
   short                            iGsmMsgSize;                  // Message size
   uint8_t                          ucGsmSender[20];              // Sender phone number
   uint8_t                          ucGsmMessage[160];            // Last message  
   uint8_t                          ucGsmTxMessage[160];          // Send message. Text here is sent and nulled. 
   uint8_t                          ucGsmTemp[10];                // Module temperature 
   double                           dGsmTemp;                     // Numerical temperature
   uint8_t                          cSigCount;                    // CAN msg signal report counter
   uint8_t                          cLifeCount;                   // CAN msg life report counter
   unsigned int                     uiGsmLifeTimeout;             // Timeout life data
   unsigned long                    ulGsmStartDelay;              // Boot time delay
   uint8_t                          cGsmStarted;                  // Started flag   
   uint8_t                          cGsmInited;                   // We have received a keyword from GSM   
   uint8_t                          cGsmCommand[10];              // Command string   
   uint8_t                          cGsmCommandHandled;           // True when handled, reset when message deleted   
   uint8_t                          cPoweroff_sms;                // Pending shutdown command after SMS is deleted
   
   
   uint8_t                          ucGsmBtScan;                  // 1: scanning 
   short                            iGsmBtStatus;                 // 
   uint8_t                          cGsmOnline;                   // Module is available
   BT_DATA                          sBtData[BT_ARRAYSIZE];        // BT devices
   uint8_t                          cJamming;                     // 1: No jamming, 2: jamming, 3: interference
   unsigned long                    ulGsmTime;                    // Time based state delays
   
   //********************************************************************
   // APRS 
   
   uint8_t                          uiAprsState;       
   uint8_t                          uiAprsTxCount;       
   DRV_HANDLE                       drvAprsHandle;
   uint8_t                          ucAprsTxbuff[256];
   uint8_t                          ucAprsRxbuff[512];
   unsigned int                     uiAprsRxptr;
   uint8_t                          uiAprsSync;                   // 0: wait for sync 1: Synced
   uint8_t                          cAprsRmw[MAX_BUF];
   uint8_t                          cAprsWpl[MAX_BUF];
   uint8_t                          cAprsStatus[MAX_BUF];
   uint8_t                          bAprsGot;         
   uint8_t                          ulRadioOn;                    // True when radio is switched on (longlife val)
   APRS_DATA                        tAprsEntry[APRS_ARRAYSIZE];   // Array
   uint8_t                          cAprsBeaconInit;
   uint8_t                          cAprsBeaconData;              // True if datastring sent to radio
   uint8_t                          cAprsBeaconTimeout;           // Send after 100 sec anyway
   uint8_t                          cAprsBeaconCount;  
   uint8_t                          cAprsPosCount;
   uint8_t                          cAprsOnline;                  // Module is available
   uint8_t                          cIncoming[6];                 // Incoming shift buffer
   uint8_t                          cCustomAprsMessage[30];       // Custom APRS telemetry message from SMS
   unsigned long                    ulMsgTime;                    // Time custom message was created
   uint8_t                        * pcArray;
   uint8_t                          cNewseen;                     // Seen a new station
   uint8_t                          cOwnseen;                     // Seen our own call repeated
 
   unsigned char                    bPrevPower;
   unsigned int                     bPrevPeriod;
   unsigned char                    bPrevAprsPath[12];
   unsigned char                    bPrevTXfreq[8];
   unsigned char                    bPrevRXfreq[8];
 
   unsigned int                     uiRledcount;                
   unsigned int                     uiGledcount;                
   unsigned int                     uiBledcount;                
   unsigned int                     uiYledcount;                
 
      
   //********************************************************************
   // Misc
   unsigned long                    ulLife;
   uint8_t                          cSalarm;                      // Shutdown alarm sounded
   
   //********************************************************************
   // Pressure temperature sensor data from I2C
   unsigned short                   dig_T1;
   short                            dig_T2;
   short                            dig_T3;
   unsigned short                   dig_P1;
   short                            dig_P2;
   short                            dig_P3;
   short                            dig_P4;
   short                            dig_P5;
   short                            dig_P6;
   short                            dig_P7;
   short                            dig_P8;
   short                            dig_P9;
   BMP280_S32_t                     t_fine;   
    
} APP_DATA;


 
void APP_Initialize (void);
void APP_Tasks(void);
void vBeep(int);
double bmp280_compensate_T_double(long);
double bmp280_compensate_P_double(long);



#endif 
 

// system_interrupts backup
/*
 
void __ISR(_I2C3_MASTER_VECTOR, ipl1AUTO) _IntHandlerDrvI2CMasterInstance0(void)
{
    DRV_I2C_Tasks(sysObj.drvI2C0);
}


void __ISR(_I2C3_BUS_VECTOR, ipl1AUTO) _IntHandlerDrvI2CErrorInstance0(void)
{
    SYS_ASSERT(false, "I2C Driver Instance 0 Error");
}

  
void __ISR(_UART2_TX_VECTOR, ipl1AUTO) _IntHandlerDrvUsartTransmitInstance0(void)
{
    DRV_USART_TasksTransmit(sysObj.drvUsart0);
}
void __ISR(_UART2_RX_VECTOR, ipl1AUTO) _IntHandlerDrvUsartReceiveInstance0(void)
{
    DRV_USART_TasksReceive(sysObj.drvUsart0);
}
void __ISR(_UART2_FAULT_VECTOR, ipl1AUTO) _IntHandlerDrvUsartErrorInstance0(void)
{
    DRV_USART_TasksError(sysObj.drvUsart0);
}
 
 

 
void __ISR(_UART4_TX_VECTOR, ipl1AUTO) _IntHandlerDrvUsartTransmitInstance1(void)
{
    DRV_USART_TasksTransmit(sysObj.drvUsart1);
}
void __ISR(_UART4_RX_VECTOR, ipl1AUTO) _IntHandlerDrvUsartReceiveInstance1(void)
{
    DRV_USART_TasksReceive(sysObj.drvUsart1);
}
void __ISR(_UART4_FAULT_VECTOR, ipl1AUTO) _IntHandlerDrvUsartErrorInstance1(void)
{
    DRV_USART_TasksError(sysObj.drvUsart1);
}

 

 
void __ISR(_UART1_TX_VECTOR, ipl1AUTO) _IntHandlerDrvUsartTransmitInstance2(void)
{
    DRV_USART_TasksTransmit(sysObj.drvUsart2);
}
void __ISR(_UART1_RX_VECTOR, ipl1AUTO) _IntHandlerDrvUsartReceiveInstance2(void)
{
    DRV_USART_TasksReceive(sysObj.drvUsart2);
}
void __ISR(_UART1_FAULT_VECTOR, ipl1AUTO) _IntHandlerDrvUsartErrorInstance2(void)
{
    DRV_USART_TasksError(sysObj.drvUsart2);
}
 
    void __ISR(_CHANGE_NOTICE_E_VECTOR, ipl1AUTO) _IntHandlerChangeNotification_PortE(void)
{
    APP_ChangeReceiveEventHandler();   // kbdoo keep     
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_CHANGE_NOTICE_E);
}
    void __ISR(_CHANGE_NOTICE_F_VECTOR, ipl1AUTO) _IntHandlerChangeNotification_PortF(void)
{
    APP_ChangeReceiveEventHandler();   // kbdoo keep   
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_CHANGE_NOTICE_F);
}

 
 
void __ISR(_TIMER_5_VECTOR, ipl1AUTO) IntHandlerDrvTmrInstance0(void)
{
   APP_GeigerTimerHandler();   // kbdoo keep
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_5);
}
 

void __ISR(_TIMER_7_VECTOR, ipl1AUTO) IntHandlerDrvTmrInstance1(void)
{
   vTmr7_event();  // kbdoo keep
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_7);
}
 

void __ISR(_SPI4_RX_VECTOR, ipl1AUTO) _IntHandlerSPIRxInstance0(void)
{
    DRV_SPI_Tasks(sysObj.spiObjectIdx0);
}
void __ISR(_SPI4_TX_VECTOR, ipl1AUTO) _IntHandlerSPITxInstance0(void)
{
    DRV_SPI_Tasks(sysObj.spiObjectIdx0);
}
void __ISR(_SPI4_FAULT_VECTOR, ipl1AUTO) _IntHandlerSPIFaultInstance0(void)
{
    DRV_SPI_Tasks(sysObj.spiObjectIdx0);
}

// kbdoo keep
void __ISR(_CAN1_VECTOR, IPL3AUTO) _IntHandlerDrvCANInstance0(void)
{
   vCAN_MsgRecv();
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_CAN_1);
}




/*******************************************************************************
 End of File
*/
