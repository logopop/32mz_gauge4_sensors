//*********************************************************************************************************************
//  
//  
//
//*********************************************************************************************************************
//
   
#include "app.h"
#include "temp_press.h"

//*********************************************************************************************************************
// Globals
// 
//*********************************************************************************************************************
//
   
extern APP_DATA                        appData;
extern NVM_CONFIG                      sConfig;

//*********************************************************************************************************************
// Bosch BMP280 conversion routines
// Returns temperature in DegC, double precision. Output value of ?51.23? equals 51.23 DegC. 
// t_fine carries fine temperature as global value BMP280_S32_t t_fine; 
//*********************************************************************************************************************
//
 
double bmp280_compensate_T_double(BMP280_S32_t adc_T) 
{  
   double var1;
   double var2;
   double T;
	  
   var1  = (((double)adc_T)/16384.0 - ((double)appData.dig_T1)/1024.0) * ((double)appData.dig_T2);   
   var2  = ((((double)adc_T)/131072.0 - ((double)appData.dig_T1)/8192.0) * (((double)adc_T)/131072.0 - ((double)appData.dig_T1)/8192.0)) * ((double)appData.dig_T3);
	appData.t_fine = (BMP280_S32_t)(var1 + var2);  
   T  = (var1 + var2) / 5120.0;  

   //sprintf(appData.uiDebugbuffer1, "V: %f %c", T , '\0');   
   
	return T; 
}  
  
// Returns pressure in Pa as double. Output value of ?96386.2? equals 96386.2 Pa = 963.862 hPa 	  
double bmp280_compensate_P_double(BMP280_S32_t adc_P) 
{  
   double var1;
   double var2;
   double p;
   
	var1 = ((double)appData.t_fine/2.0) - 64000.0;
	var2 = var1 * var1 * ((double)appData.dig_P6) / 32768.0;
	var2 = var2 + var1 * ((double)appData.dig_P5) * 2.0;
	var2 = (var2/4.0)+(((double)appData.dig_P4) * 65536.0);
	var1 = (((double)appData.dig_P3) * var1 * var1 / 524288.0 + ((double)appData.dig_P2) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0)*((double)appData.dig_P1);  
	if (var1 == 0.0)  
	{   
      return 0;   // avoid exception caused by division by zero  
	}
	p = 1048576.0 - (double)adc_P;
	p = (p - (var2 / 4096.0)) * 6250.0 / var1;
	var1 = ((double)appData.dig_P9) * p * p / 2147483648.0;
	var2 = p * ((double)appData.dig_P8) / 32768.0;
	p = p + (var1 + var2 + ((double)appData.dig_P7)) / 16.0;
	return p;
} 

//*********************************************************************************************************************
// vGet_temppress
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
   uint8_t                       i                          = {0};
   char                          cTemp[160]                 = {0};
   uint8_t                       uiTemp                     = 0x00;
   
   long                          lTemperature               = 0;     
   long                          lPressure                  = 0;
   
   double                        dTemperature               = 0.0;
   double                        dPressure                  = 0.0;
   
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
 
   appData.dTemperature = bmp280_compensate_T_double(lTemperature);   
   appData.dPressure = bmp280_compensate_P_double(lPressure) / 100.0;

   sprintf(appData.uiDebugbuffer7, "%.2fc %.2fmb%c", appData.dTemperature, appData.dPressure, '\0');   		 
}		 
  



