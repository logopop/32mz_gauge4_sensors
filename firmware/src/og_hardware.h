//*********************************************************************************************************************
// Hardware defines
// Sensor board
// Board v4.1 amd 5.0
//
//*********************************************************************************************************************
//  CAN
//  PIC32MZ
//
//*********************************************************************************************************************
//
//*********************************************************************************************************************
//

//*********************************************************************************************************************
// I2C device addresses
//
//*********************************************************************************************************************
//

#define                          DEV_DISPLAY                0x78              // SSD1306 display
#define                          DEV_GYRO                   0x68              // MPU6050 Gyro
#define                          DEV_PRESS                  0xEC              // BMP280 pressure sensor
#define                          DEV_CO2                    0x58              // SGP30 gas sensor
#define                          DEV_VOLTAGE                0x88              // INA219 Voltage current sensor

 
//*********************************************************************************************************************
// Display port defines
//
//*********************************************************************************************************************
//
/*
#define                          OLED_RESET                 LATFbits.LATF0    // !Reset 
#define                          OLED_DATA                  LATFbits.LATF1    // Data/!Command
#define                          OLED_RW                    LATDbits.LATD5    // Read/!Write
#define                          OLED_ENABLE                LATDbits.LATD4    // Enable
 */

// Board LEDs
#define                          LED_YELLOW                 LATEbits.LATE5    // Yellow board LED
#define                          LED_BLUE                   LATEbits.LATE6    // Blue board LED
#define                          LED_GREEN                  LATEbits.LATE4    // Green board LED
#define                          LED_RED                    LATEbits.LATE7    // Red board LED
#define                          LED_ON                     1                 // On
#define                          LED_OFF                    0                 // Off
 
// Defines speaker pin
#define                          SPEAKER                    LATBbits.LATB12   // 
// Geiger input
#define                          GEIGER                     PORTFbits.RF4     // Geiger pulse input  2
// GPS pulse per second
#define                          GPS_PPS                    PORTEbits.RE0     // GPS PPS  1
// Gyro Chip Enable
#define                          GYRO_CE                    LATDbits.LATD9    // CE
// Extra relay
#define                          EXTRA_RELAY                LATBbits.LATB10   // To Opto relay 1 out
// APRS power relay
#define                          APRS_RELAY                 LATBbits.LATB9    // To Opto relay 2 out
// Power controller shutdown
#define                          PWR_SHUTDOWN               LATBbits.LATB11   // To 18f - you can shut down now
// Pending power shutdown
#define                          PENDING_PWROFF             PORTEbits.RE1     // From 18f - About to power off
// Alarm sense input
#define                          ALARM_SENSE                PORTDbits.RD0     // To Opto in alarm
// Ignition sense input
#define                          IGNITION_SENSE             PORTDbits.RD1     // To Opto in ignition
// GSM DTR
#define                          GSM_DTR                    LATDbits.LATD4    // To GSM DTR  (optional)
// GSM RES
#define                          GSM_RESET                  LATDbits.LATD5    // To GSM RES  (optional)



//*********************************************************************************************************************
//  
//
//*********************************************************************************************************************
//
 
// Pinout  DB15
/*
   CANL                 7
   CANH                 6		
   Alarm                4
   Ignition             3
   +12v                 2
   +12v                 13
	GND                  1
   GND                  14
   GND                  15
   GPS out              8
   APRS Tx              9
   APRS Rx              10
   Extra Relay          12
   Radio Pwr Relay		11
   RE3                  5
 
*/