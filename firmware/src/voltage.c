//*********************************************************************************************************************
//  
//  
//
//*********************************************************************************************************************
//
   
#include "app.h"
#include "voltage.h"

//*********************************************************************************************************************
// Globals
// 
//*********************************************************************************************************************
//
   
extern APP_DATA                        appData;
extern NVM_CONFIG                      sConfig;


//*********************************************************************************************************************
// Initialize AD (not used)
//
//*********************************************************************************************************************
//

void vVoltage_Initialize(void)
{   
   // Calibration registers 
   ADC0CFG = DEVADC0;
   ADC1CFG = DEVADC1;
   ADC2CFG = DEVADC2;
   ADC3CFG = DEVADC3;
   ADC4CFG = DEVADC4;
   ADC7CFG = DEVADC7;

   // Reset ADCON1
   ADCCON1 = 0;  
   ADCCON1bits.SELRES = 3;                   // Resolution 
   ADCCON1bits.STRGSRC = 1;                  // Global software trigger
   ADCCON2bits.SAMC = 0b0000111111;          // ADC0 sampling time
   ADCCON2bits.ADCDIV = 0x7f;                // ADC0 clock frequency
   ADCCON2bits.CVDCPL = 7; 
   ADCCON1bits.AICPMPEN = 0; 
   ADCCON1bits.FRACT = 0; 
   CFGCONbits.IOANCPEN = 0; 
   
   
   ADCANCON = 0;
   ADCANCONbits.WKUPCLKCNT = 5;
   ADCCON3 = 0;
   ADCCON3bits.ADCSEL = 0;                   // Input clock source
   ADCCON3bits.CONCLKDIV = 1; 
   ADCCON3bits.VREFSEL = 0;                  // Reference v source

   ADC4TIMEbits.ADCDIV = 4;
   ADC4TIMEbits.SAMC = 100;  
   ADC4TIMEbits.SELRES = 3;     
 
   ADCIMCON1bits.SIGN12 = 0;                 // unsigned data
   ADCIMCON1bits.DIFF12= 0;                  // Single ended

   ADCGIRQEN1 = 0;                           // No interrupts
   ADCGIRQEN2 = 0;

   ADCCSS1 = 0; 
   ADCCSS2 = 0; 
   ADCCSS1bits.CSS4 = 1;                     // Class set for scan

   ADCCMPCON1 = 0;                           // No comparators
   ADCCMPCON2 = 0; 
   ADCCMPCON3 = 0;  
   ADCCMPCON4 = 0;
   ADCCMPCON5 = 0;
   ADCCMPCON6 = 0;

   ADCFLTR1 = 0;                             // No oversampling filters
   ADCFLTR2 = 0;
   ADCFLTR3 = 0;
   ADCFLTR4 = 0;
   ADCFLTR5 = 0;
   ADCFLTR6 = 0;

   // Trigger source for AN4
   ADCTRG2bits.TRGSRC4 = 1;

   ADCEIEN1 = 0; // No early interrupts
   ADCEIEN2 = 0;
   
   // ADC ON
   ADCCON1bits.ON = 1;
 
   // Wait for Vref stable
   while(!ADCCON2bits.BGVRRDY);  
   while(ADCCON2bits.REFFLT);  
   
   // Enable the clock
   ADCCON3bits.DIGEN4 = 1;    // Enable ADC4
   ADCANCONbits.ANEN4 = 1;    // Enable clock to ADC4
   while( !ADCANCONbits.WKRDY4 );   // Wait until ADC4 is ready
}   

//*********************************************************************************************************************
// vGet_voltage
// 
//*********************************************************************************************************************
//

void vGet_Voltage(void)  // INA219 sensor
{  
   uint8_t                       uiTemp                     = 0x00;
   signed long                   lShuntVoltage              = 0;
   signed long                   lBusVoltage                = 0;
   signed long                   lCurrent                   = 0;   
   double                        dShuntVoltage              = 0.0;
   double                        dBusVoltage                = 0.0;
   double                        dCurrent                   = 0.0;
   
   
   // Initialize config
   appData.uiI2CTxbuffer[0] = 0x00;
   appData.uiI2CTxbuffer[1] = 0b00111110;
   appData.uiI2CTxbuffer[2] = 0b01100111;
   vI2C_Write(DEV_VOLTAGE, 3); 

   // Calibration
    appData.uiI2CTxbuffer[0] = 0x05;
   appData.uiI2CTxbuffer[1] = 0x00;
   appData.uiI2CTxbuffer[2] = 0x2a;   // 2a - 1 ohm shunt
   vI2C_Write(DEV_VOLTAGE, 3); 

   // Current 
   appData.uiI2CTxbuffer[0] = 0x04;
   vI2C_Write(DEV_VOLTAGE, 1); 
   vI2C_Read(DEV_VOLTAGE, 2);  
   uiTemp = appData.uiI2CRxbuffer[0];
   appData.uiI2CRxbuffer[0] = appData.uiI2CRxbuffer[1];
   appData.uiI2CRxbuffer[1] = uiTemp; 
   memcpy(&lCurrent, appData.uiI2CRxbuffer, 2);       

   // Shunt voltage
   appData.uiI2CTxbuffer[0] = 0x01;
   vI2C_Write(DEV_VOLTAGE, 1); 
   vI2C_Read(DEV_VOLTAGE, 2);  
   uiTemp = appData.uiI2CRxbuffer[0];
   appData.uiI2CRxbuffer[0] = appData.uiI2CRxbuffer[1];
   appData.uiI2CRxbuffer[1] = uiTemp;                    
   memcpy(&lShuntVoltage, appData.uiI2CRxbuffer, 2);                           

   // Bus voltage
   appData.uiI2CTxbuffer[0] = 0x02;
   vI2C_Write(DEV_VOLTAGE, 1); 
   vI2C_Read(DEV_VOLTAGE, 2);  
   uiTemp = appData.uiI2CRxbuffer[0];
   appData.uiI2CRxbuffer[0] = appData.uiI2CRxbuffer[1];
   appData.uiI2CRxbuffer[1] = uiTemp;             

   memcpy(&lBusVoltage, appData.uiI2CRxbuffer, 2);                           
   lBusVoltage = (lBusVoltage >> 3);

   appData.dVoltage = ((lBusVoltage * 4.0) + (lShuntVoltage / 100.0)) / 1000.0;
   appData.dCurrent = (float)(lCurrent);        
   sprintf(appData.uiDebugbuffer6, "%.3fv %.1f mA%c", appData.dVoltage, appData.dCurrent, '\0');   
   //sprintf(appData.uiDebugbuffer4, "I: %.1f mA %c", (float)(lCurrent),  '\0');   
   
   // Send alarm
   if ((appData.ulLonglife > 50) && (!appData.cVoltalarm) && (appData.dVoltage < sConfig.fBatteryLimit))     
   {                       
      vSendSmsAlarm("BATTERY alarm!");
      appData.cVoltalarm = 1;
   }      


}

/*
void vVoltage_State(void)
{  
   // AD curve multiplier table 
   //                 
   // Test unit
   //                           6       7       8       9       9,5     10      10.5    11      11.5    12      12.5    13      13.5    14      14.5    15      16      17      18      19      20
   //unsigned long ulLut[21] = {1045,   1229,   1402,   1571,   1648,   1723,   1793,   1857,   1915,   1973,   2023,   2071,   2115,   2154,   2191,   2220,   2283,   2337,   2384,   2425,   2466};
   //float         fMult[21] = {174.17, 175.57, 175.25, 174.56, 173.47, 172.30, 170.76, 168.82, 166.52, 164.42, 161.84, 159.31, 156.67, 153.86, 151.10, 148.00, 142.69, 137.47, 132.44, 127.63, 123.30};   
   // 
   // Jeep
   //                         6       7       8       9       9,5     10      10.5    11      11.5    12      12.5    13      13.5    14      14.5    15      16      17      18      19      20
   unsigned long ulLut[21] = {966,    1150,   1346,   1520,   1613,   1690,   1764,   1832,   1896,   1949,   2006,   2042,   2098,   2136,   2176,   2210,   2275,   2327,   2378,   2422,   2461};
   float         fMult[21] = {161.00, 164.29, 168.25, 168.89, 169.79, 169.00, 168.00, 166.55, 164.87, 162.42, 160.48, 157.08, 155.41, 152.57, 150.06, 147.33, 142.19, 136.88, 132.11, 127.47, 123.05};   
  
   unsigned int                  i                          = 0;
   char                          cTemp[120]                 = {0};
   float                         fFract1                    = 0;
   float                         fFract2                    = 0;
  
   switch(appData.uiVoltageState)
   {
      case 0:        // Idle
         appData.uiVoltageState = 1;
         appData.ulADvalue = 0;
      break; 
      case 1:        // Start sample
         // Start conversion
         ADCCON3bits.GSWTRG = 1;
         appData.uiVoltageState = 2;         
      break;
      case 2:        // Wait for result 
         if (ADCDSTAT1bits.ARDY4)        
            appData.uiVoltageState = 3;            
         else
         {             
            appData.uiVoltageCount++;
            if (appData.uiVoltageCount > 10)  
            {  
               vVoltage_Initialize();
               appData.uiVoltageState = 0;
            }   
         }           
      break;
      case 3:        
         appData.fAdAccu += ADCDATA4;
         
         appData.cAdAve++;
         if (appData.cAdAve > 29)
            appData.uiVoltageState = 4;
         else
            appData.uiVoltageState = 0;         
         appData.uiVoltageCount = 0;        
      break;              
      case 4:                       // Calculate voltage
         appData.cAdAve = 0;
   
         // Translate AD value to a voltage value         
         appData.ulADvalue = appData.fAdAccu / 30;         
         for (i = 0; i < 21; i++)
         {   
            if (appData.ulADvalue < ulLut[i]) 
               break;
         }                     
         fFract1 =  ((float)(appData.ulADvalue - ulLut[i - 1])) / (float)(ulLut[i] - ulLut[i - 1]);   // Fraction of segment         
         fFract2 = ((fMult[i] - fMult[i - 1]) * fFract1) + fMult[i - 1];
                  
         if (i)
            appData.fVoltage = appData.ulADvalue / fFract2;  
         else
            appData.fVoltage = appData.ulADvalue / fMult[i];  
         // Display
         sprintf(appData.uiDebugbuffer6, " %.2fv %lu %d %c", appData.fVoltage, appData.ulADvalue, sConfig.iGeigerCal, '\0');                        
         appData.cAdAvailable = 1;
         appData.fAdAccu = 0;
         appData.uiVoltageState = 5;
      break;                  
      case 5:
         if ((appData.ulLonglife > 50) && (!appData.cVoltalarm) && (appData.fVoltage < 11.2))     
         {                       
            vSendSmsAlarm("BATTERY alarm!");
            appData.cVoltalarm = 1;
         }               
         appData.uiVoltageState = 0;      
      break;
      default:
         appData.uiVoltageState = 0;   
    }
}   
 
*/


