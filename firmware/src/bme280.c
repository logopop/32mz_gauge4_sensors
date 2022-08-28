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
#include "bme280.h"


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
   
bme280_Initialize()
{
		// Oversampling setting
         appData.uiI2CTxbuffer[0] = 0xF4;
         appData.uiI2CTxbuffer[1] = 0x33;                    
         vI2C_Write(DEV_PRESS, 2);  
         appData.uiI2CTxbuffer[0] = 0xF5;
         appData.uiI2CTxbuffer[1] = 0x60;                    
         vI2C_Write(DEV_PRESS, 2);  
//       // Read compensation data  vI2C_Read(DEV_PRESS, 1);  
         appData.uiI2CTxbuffer[0] = 0x88;
         vI2C_Write(DEV_PRESS, 1);  
         vI2C_Read(DEV_PRESS, 24); 
           
         memcpy(&appData.dig_T1, appData.uiI2CRxbuffer, 2);
         memcpy(&appData.dig_T2, appData.uiI2CRxbuffer + 2, 2);
         memcpy(&appData.dig_T3, appData.uiI2CRxbuffer + 4, 2);
         memcpy(&appData.dig_P1, appData.uiI2CRxbuffer + 6, 2);
         memcpy(&appData.dig_P2, appData.uiI2CRxbuffer + 8, 2);
         memcpy(&appData.dig_P3, appData.uiI2CRxbuffer + 10, 2);
         memcpy(&appData.dig_P4, appData.uiI2CRxbuffer + 12, 2);
         memcpy(&appData.dig_P5, appData.uiI2CRxbuffer + 14, 2);
         memcpy(&appData.dig_P6, appData.uiI2CRxbuffer + 16, 2);
         memcpy(&appData.dig_P7, appData.uiI2CRxbuffer + 18, 2);
         memcpy(&appData.dig_P8, appData.uiI2CRxbuffer + 20, 2);
         memcpy(&appData.dig_P9, appData.uiI2CRxbuffer + 22, 2);
                           
	
	
	
}	
   
//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//
 
bme280_Read() 
{ 
 

         // Read measurement 
         appData.uiI2CTxbuffer[0] = 0xF7;
         vI2C_Write(DEV_PRESS, 1);  
         vI2C_Read(DEV_PRESS, 6);  
         
         // Temperature
         uiTemp = appData.uiI2CRxbuffer[3];
         appData.uiI2CRxbuffer[3] = appData.uiI2CRxbuffer[5];
         appData.uiI2CRxbuffer[5] = uiTemp; 
         memcpy(&lTemperature, appData.uiI2CRxbuffer + 3, 3);
         lTemperature = (lTemperature >> 4);

         // Pressure
         uiTemp = appData.uiI2CRxbuffer[0];
         appData.uiI2CRxbuffer[0] = appData.uiI2CRxbuffer[2];
         appData.uiI2CRxbuffer[2] = uiTemp; 
         memcpy(&lPressure, appData.uiI2CRxbuffer, 3);
         lPressure = (lPressure >> 4);
         
         // Test
         /*
         lTemperature = 519888;
         appData.dig_T1 = 27504;
         appData.dig_T2 = 26435;
         appData.dig_T3 = -1000;
         */
         
         dTemperature = bmp280_compensate_T_double(lTemperature);
         sprintf(appData.uiDebugbuffer1, "T:%.2f c %c", dTemperature, '\0');   
         
         dPressure = bmp280_compensate_P_double(lPressure);
         sprintf(appData.uiDebugbuffer2, "P:%.2f mb %c", dPressure / 100.0 , '\0');   
         
         appData.uiMainState = 5;      
		 
		 
}		 
		 
		 
//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//
















