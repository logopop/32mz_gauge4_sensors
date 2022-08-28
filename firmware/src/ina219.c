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
#include "gyro.h"


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

ina219_Initialize()
{    
	
	 // Initialize config
         appData.uiI2CTxbuffer[0] = 0x00;
         appData.uiI2CTxbuffer[1] = 0b00111110;
         appData.uiI2CTxbuffer[2] = 0b01100111;
         vI2C_Write(DEV_VOLTAGE, 3); 
         
         // Calibration
         appData.uiI2CTxbuffer[0] = 0x05;
         appData.uiI2CTxbuffer[1] = 0x00;
         appData.uiI2CTxbuffer[2] = 0x50;   // 1 ohm shunt
         vI2C_Write(DEV_VOLTAGE, 3); 
          
}	
	
//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//
 
ina219_Read() 
{ 
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
        
         sprintf(appData.uiDebugbuffer3, "V in: %.3f %c", ((lBusVoltage * 4.0) + (lShuntVoltage / 100.0)) / 1000.0,  '\0');   
         sprintf(appData.uiDebugbuffer4, "I: %.1f mA %c", (float)(lCurrent),  '\0');   
        
         
          
 
 
}


//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//




 
 
 
 