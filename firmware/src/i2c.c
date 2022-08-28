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
#include "i2c.h"


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

void vError(void)
{
   int i = 0;
   
   for (i = 0; i < 6; i++)
   {
      LED_BLUE = LED_ON;
      vLongDelay(100);
      LED_BLUE = LED_OFF;
      vLongDelay(100);   
   }     
}

//*********************************************************************************************************************
//  
//  
//*********************************************************************************************************************
//

DRV_I2C_BUFFER_EVENT APP_Check_Transfer_Status(DRV_HANDLE drvOpenHandle, DRV_I2C_BUFFER_HANDLE drvBufferHandle)
{
   return (DRV_I2C_TransferStatusGet(drvOpenHandle, drvBufferHandle));
}

uint32_t APP_Number_Of_Bytes_Transferred(DRV_HANDLE drvOpenHandle, DRV_I2C_BUFFER_HANDLE drvBufferHandle)
{
   return (DRV_I2C_BytesTransferred(drvOpenHandle, drvBufferHandle));
}

void I2CMasterOpStatusCb ( DRV_I2C_BUFFER_EVENT event, DRV_I2C_BUFFER_HANDLE bufferHandle, uintptr_t context)
{
   switch(event)
   {
      case DRV_I2C_BUFFER_EVENT_COMPLETE:
      
      //this indicates that the I2C transaction has completed
      //DRV_I2C_BUFFER_EVENT_COMPLETE can be handled in the callback
      //or by checking for this event using the API DRV_I2C_BufferStatus
      /* include any callback event handling code here if needed */
      break;
      case DRV_I2C_BUFFER_EVENT_ERROR:
      //this indicates that the I2C transaction has completed
      //and a STOP condition has been asserted on the bus.
      //However the slave has NACKED either the address or data
      //byte.
      /* include any callback event handling code here if needed */
      default:
      break;
   }
}

//*********************************************************************************************************************
//  
//  
//*********************************************************************************************************************
//

void vI2C_Write(uint8_t uiDevice, unsigned int uiBytes)  
{  
   unsigned long                    ulEscape                = 0;
       
   if ((uiDevice == DEV_DISPLAY) && (!appData.ucDisplayPresent))
      return;
   
   if (appData.drvI2CHandle1 != DRV_HANDLE_INVALID)
   {             
      // Write
      if ((appData.appI2CWriteBufferHandle == DRV_I2C_BUFFER_HANDLE_INVALID) || 
         (APP_Check_Transfer_Status(appData.drvI2CHandle1, appData.appI2CWriteBufferHandle) == DRV_I2C_BUFFER_EVENT_COMPLETE) ||
         (APP_Check_Transfer_Status(appData.drvI2CHandle1, appData.appI2CWriteBufferHandle) == DRV_I2C_BUFFER_EVENT_ERROR))
      {
         appData.appI2CWriteBufferHandle = DRV_I2C_Transmit(appData.drvI2CHandle1, uiDevice, &appData.uiI2CTxbuffer[0], uiBytes, NULL);
      }
      // Wait for completion
      while (APP_Check_Transfer_Status(appData.drvI2CHandle1, appData.appI2CWriteBufferHandle) != DRV_I2C_BUFFER_EVENT_COMPLETE)
      {         
         vLongDelay(1);
         ulEscape++;
         if (ulEscape > 50)
         {   
            if (uiDevice == DEV_DISPLAY)  // Flag if we do not have a display
               appData.ucDisplayPresent = 0;
            break;
         }            
      }         
   }
   else
      vError();  
}


//*********************************************************************************************************************
//  
//  
//*********************************************************************************************************************
//

void vI2C_Read(uint8_t uiDevice, unsigned int uiBytes)  // Display
{  
   unsigned long                    ulEscape                = 0;
   
   if ((uiDevice == DEV_DISPLAY) && (!appData.ucDisplayPresent))
      return;
     
   if (appData.drvI2CHandle1 != DRV_HANDLE_INVALID)
   {             
      // Read
      if ((appData.appI2CReadBufferHandle == DRV_I2C_BUFFER_HANDLE_INVALID) || 
         (APP_Check_Transfer_Status(appData.drvI2CHandle1, appData.appI2CReadBufferHandle) == DRV_I2C_BUFFER_EVENT_COMPLETE) ||
         (APP_Check_Transfer_Status(appData.drvI2CHandle1, appData.appI2CReadBufferHandle) == DRV_I2C_BUFFER_EVENT_ERROR))
      {
         appData.appI2CReadBufferHandle = DRV_I2C_Receive(appData.drvI2CHandle1, uiDevice, &appData.uiI2CRxbuffer[0], uiBytes, NULL);
      }
      // Wait for completion
      while (APP_Check_Transfer_Status(appData.drvI2CHandle1, appData.appI2CReadBufferHandle) != DRV_I2C_BUFFER_EVENT_COMPLETE)
      {         
         vLongDelay(1);
         ulEscape++;
         if (ulEscape > 5000)
            break;         
      }         
   }
   else
      vError();  
} 

//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//

void vI2C_Init(void)
{      
   appData.drvI2CHandle1 = DRV_I2C_Open(DRV_I2C_INDEX_0, DRV_IO_INTENT_READWRITE);            
   DRV_I2C_BufferEventHandlerSet(appData.drvI2CHandle1, I2CMasterOpStatusCb, appData.i2cOpStatus);	  
}

//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//

 





