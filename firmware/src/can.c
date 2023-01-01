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
#include "can.h"
 
//*********************************************************************************************************************
// Globals
// 
//*********************************************************************************************************************
//
 
extern APP_DATA                        appData;
extern NVM_CONFIG                      sConfig;

//*********************************************************************************************************************
// Sequence buffer available
// Put into correct struct
//*********************************************************************************************************************
//

void vFetchData(void)
{
   switch(appData.ucCanType)
   {
      case CID_CONFIG_H:          
         memcpy(&sConfig, appData.pCanBuff, sizeof(NVM_CONFIG)); 
         
      break;
   }   
}   
   
//*********************************************************************************************************************
// CAN message received
// Called from ISR routine
//*********************************************************************************************************************
//

/*

 In system_interrupt.c
 
void __ISR(_CAN1_VECTOR, IPL1AUTO) _IntHandlerDrvCANInstance0(void)
{
   vCAN_MsgRecv();
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_CAN_1);
}

*/

void vCAN_MsgRecv(void)
{
   CAN_RX_MSG_BUFFER           * bpBuffer; 
   uint8_t                       uiTemp[17]                 = {0};
   uint8_t                       ucMessage[8]               = {0};
   unsigned int                  uiSize                     = 0;   
   uint8_t                       cChecksum                  = 0;
   unsigned int                  uiI                        = 0;
   unsigned int                  uiRemain                   = 0;
   unsigned int                  i                          = 0;
 
     
   if((PLIB_CAN_ModuleEventGet(CAN_ID_1) & (CAN_RX_EVENT)) != 0)
   {                
      bpBuffer = (CAN_RX_MSG_BUFFER *) PLIB_CAN_ReceivedMessageGet(CAN_ID_1, CAN_CHANNEL1); 
      memcpy(ucMessage, bpBuffer->data, 8);
      
      // Receive state
      if (appData.ucCanTrans == CST_SEQUENCE)         // We're in a sequence
      {         
         uiRemain = appData.uiCanSize - (appData.pucCanRXptr - appData.pCanBuff);            
         if (uiRemain > 8)    // Not last
         {   
            memcpy(appData.pucCanRXptr, ucMessage, 8);
            appData.pucCanRXptr += 8;
         }
         else     // We're done                  
         {
            memcpy(appData.pucCanRXptr, ucMessage, uiRemain);
            
            // Generate checksum         
            for (uiI = 0; uiI < appData.uiCanSize; uiI++)
               cChecksum ^= appData.pCanBuff[uiI];
      
            if (cChecksum == appData.cChecksum)
            {   
               // Move data into local structs
               vFetchData();
            }   
             
            appData.ucCanTrans = CST_SINGLE;
            free(appData.pCanBuff);                       
         }   
      }
      else                                            // Single message
      {           
         switch (bpBuffer->msgSID.sid)
         {        
            // Header of sequence?
            case CID_ALIVE_H: 
            case CID_CONFIG_H:   
               appData.ucCanType = bpBuffer->msgSID.sid;
               appData.cChecksum = ucMessage[0];
               memcpy(&appData.uiCanSize, (unsigned int *)(ucMessage + 1), 4);  // Get sequence size
               appData.uiCanTotal = appData.uiCanSize / 8;
               appData.cCanRemnant = appData.uiCanSize%8; 
               appData.pCanBuff = malloc(appData.uiCanSize + 8);
   
               if (appData.pCanBuff)
               {
                  appData.ucCanTrans = CST_SEQUENCE;
                  appData.pucCanRXptr = appData.pCanBuff;
                  
                  // Debug
                  //sprintf(appData.uiDebugbuffer4, "Id: %d  \0", bpBuffer->msgSID.sid);                                  
                  //sprintf(appData.uiDebugbuffer5, "%ul %ul \0", appData.pCanBuff, appData.pucCanRXptr);
                                                         
               }  
            break;   
            case CID_SCREEN:        // Two-byte screen notice
               appData.uiDisplayPrev = appData.uiDisplayScreen;
               appData.uiDisplayScreen = ucMessage[0];    
               appData.ucSubScreen = ucMessage[1];
            break;                                          
            case CID_RESET:         // Received a full reset
               vBeep(2);
               vRelay_Off(APRS_RELAY);
               vRelay_Off(EXTRA_RELAY);
               PWR_SHUTDOWN = 1;
               // If power not gone      
               vLongDelay(500);
               SYSKEY = 0x00000000; //write invalid key to force lock
               SYSKEY = 0xAA996655; //write key1 to SYSKEY
               SYSKEY = 0x556699AA; //write key2 to SYSKEY
               RSWRSTSET = 1;
               i = RSWRST;
               while (1);
            break;                                          
         }
      }
          
      appData.ulCanRXtotal++;    
     
      PLIB_CAN_ChannelUpdate(CAN_ID_1, CAN_CHANNEL1);       
   }
   //Clear CAN Interrupt flag     
   PLIB_CAN_ModuleEventClear(CAN_ID_1, CAN_RX_EVENT); 
}

//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//

void vCanTransmit(short uiAddress, uint8_t * cMessage, uint8_t uiBytes)
{    
   CAN_TX_MSG_BUFFER             * msgBuffer                = NULL;
   int                           status                     = CAN_TX_CHANNEL_TRANSMITTING;
   unsigned int                  uiWait                     = 0;  
  
   
   LED_GREEN = LED_ON;
   appData.uiGledcount = appData.ulLife;
   appData.ulCanTXtotal += uiBytes;     
   
   while ((status & CAN_TX_CHANNEL_TRANSMITTING) != 0)
   {   
      status = PLIB_CAN_TransmitChannelStatusGet(CAN_ID_1, CAN_CHANNEL0);
      uiWait++;
      if (uiWait > 60000)
         break;         
   }      
   if (uiWait < 60000)
   {
      uiWait = 0;
      while (msgBuffer == NULL)
      {   
         msgBuffer = PLIB_CAN_TransmitBufferGet(CAN_ID_1, CAN_CHANNEL0);
         uiWait++;
         if (uiWait > 60000)
            appData.uiCanUp = 0;
      }   
      if (uiWait < 60000)
      {  
         appData.uiCanUp = 1;
         memcpy(msgBuffer->data, cMessage, uiBytes);      
         msgBuffer->msgSID.sid = uiAddress;
         msgBuffer->msgEID.ide = 0;        
         msgBuffer->msgEID.data_length_code = uiBytes;
         PLIB_CAN_ChannelUpdate(CAN_ID_1, CAN_CHANNEL0);
         PLIB_CAN_TransmitChannelFlush(CAN_ID_1, CAN_CHANNEL0);     
      }   
   }       
}    
   
//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//

void vCan_Initialize(void)
{
    /* Switch the CAN module ON */
    PLIB_CAN_Enable(CAN_ID_1);

    /* Switch the CAN module to Configuration mode. Wait until the switch is complete */
    PLIB_CAN_OperationModeSelect(CAN_ID_1, CAN_CONFIGURATION_MODE);
    while(PLIB_CAN_OperationModeGet(CAN_ID_1) != CAN_CONFIGURATION_MODE);
    
    PLIB_CAN_PhaseSegment2LengthFreelyProgrammableEnable(CAN_ID_1);

    //Set the Baud rate to 500 kbps
    PLIB_CAN_PropagationTimeSegmentSet(CAN_ID_1, 1-1);
    PLIB_CAN_PhaseSegment1LengthSet(CAN_ID_1, 4-1);   
    PLIB_CAN_PhaseSegment2LengthSet(CAN_ID_1, 4-1);   
    PLIB_CAN_SyncJumpWidthSet(CAN_ID_1, 1-1);
    PLIB_CAN_BaudRatePrescaleSet(CAN_ID_1, 9);     

    PLIB_CAN_MemoryBufferAssign(CAN_ID_1, appData.can_message_buffer0);

    PLIB_CAN_ChannelForTransmitSet(CAN_ID_1, CAN_CHANNEL0, 8, CAN_TX_RTR_DISABLED, CAN_LOW_MEDIUM_PRIORITY);
    PLIB_CAN_ChannelForReceiveSet(CAN_ID_1, CAN_CHANNEL1, 1, CAN_RX_FULL_RECEIVE);

    // Filter and mask
    PLIB_CAN_FilterConfigure(CAN_ID_1, CAN_FILTER0, 0x7ff, CAN_SID);
    PLIB_CAN_FilterMaskConfigure(CAN_ID_1, CAN_FILTER_MASK0, 0x0, CAN_SID, CAN_FILTER_MASK_IDE_TYPE);
    PLIB_CAN_FilterToChannelLink(CAN_ID_1, CAN_FILTER0, CAN_FILTER_MASK0, CAN_CHANNEL1);
    PLIB_CAN_FilterEnable(CAN_ID_1, CAN_FILTER0);
    
    /* Setup CAN_ID_1 Interrupt */   
    PLIB_INT_VectorPrioritySet(INT_ID_0, INT_VECTOR_CAN1, INT_PRIORITY_LEVEL3);
    PLIB_INT_VectorSubPrioritySet(INT_ID_0, INT_VECTOR_CAN1, INT_SUBPRIORITY_LEVEL0);
    PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_CAN_1);
    
    PLIB_CAN_ChannelEventEnable(CAN_ID_1, CAN_CHANNEL1, CAN_RX_CHANNEL_NOT_EMPTY);
    PLIB_CAN_ModuleEventEnable(CAN_ID_1, CAN_RX_EVENT|CAN_RX_OVERFLOW_EVENT);
    
    // Normal
    PLIB_CAN_OperationModeSelect(CAN_ID_1, CAN_NORMAL_MODE);
    while(PLIB_CAN_OperationModeGet(CAN_ID_1) != CAN_NORMAL_MODE);
            
}   

//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//

void vCanSendBuffer(uint8_t * pcData, unsigned int uiSize, unsigned short sMessage)
{
   uint8_t                       ucBytes[8]                 = {0};
   uint8_t                       cChecksum                  = 0;
   uint8_t                       cRemnant                   = 0;
   unsigned int                  uiI                        = 0;
   unsigned int                  uiLoops                    = 0;  
   
   // Generate checksum
   for (uiI = 0; uiI < uiSize; uiI++)
      cChecksum ^= pcData[uiI];
      
   // Send transmission header
   ucBytes[0] = cChecksum; 
   memcpy(ucBytes + 1, (uint8_t *)&uiSize, 4);   
   vCanTransmit(sMessage, ucBytes, 5);
   
   uiLoops = uiSize / 8;
   cRemnant = uiSize%8; 
       
   for (uiI = 0; uiI < uiLoops; uiI++)
   {       
      memcpy(ucBytes, pcData + (uiI * 8), 8);
      vCanTransmit(sMessage + 1, ucBytes, 8);
   }
   if (cRemnant)
   {             
      memcpy(ucBytes, pcData + (uiI * 8), cRemnant);
      vCanTransmit(sMessage + 1, ucBytes, cRemnant);
   }   
}
            
//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//

void vSendGsmSignal(void)
{        
   GSMS_MSG                      sGsmMsg;
      
   memset((void *)&sGsmMsg, 0x00, sizeof(GSMS_MSG));
   appData.cSigCount = 0; 
   sGsmMsg.cGsmRegistered = appData.cGsmRegistered;
   sGsmMsg.iGsmSignal = appData.iGsmSignal;
   strncpy(sGsmMsg.ucGsmOperator, appData.ucGsmOperator, 20);
   sGsmMsg.cJamming = appData.cJamming;
   sGsmMsg.iGsmMode = appData.iGsmMode;
   sGsmMsg.cGsmOnline = appData.cGsmOnline;  
   vCanSendBuffer((uint8_t *)&sGsmMsg, sizeof(sGsmMsg), CID_GSMS_H);    
}            
            
//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//

void vSendAlive(void)
{        
   LIFE_MSG                      sLifeMsg;
   
   memset((void *)&sLifeMsg, 0x00, sizeof(LIFE_MSG));
   memcpy(sLifeMsg.cTime, appData.cGpsTime, 6);
   memcpy(sLifeMsg.cDate, appData.cGpsDate, 6);           
   sLifeMsg.dGsmTemp = appData.dGsmTemp;
   sLifeMsg.iGyroTemp = appData.iGyroTemp;
   sLifeMsg.fVoltage = appData.fVoltage;
   sLifeMsg.llRandom = appData.llRandom; 

   // Dedicated sensor values
   sLifeMsg.dTemperature = appData.dTemperature;
   sLifeMsg.dPressure = appData.dPressure;
   sLifeMsg.dVoltage = appData.dVoltage;
   sLifeMsg.dCurrent = appData.dCurrent;
   if (appData.cNewseen)
   {
      appData.cNewseen = 0;   
      sLifeMsg.cSignals |= SIG_APRS_NEW;
   }   
   if (appData.cOwnseen)
   {
      appData.cOwnseen = 0;   
      sLifeMsg.cSignals |= SIG_APRS_OWN;
   }   
   
   vCanSendBuffer((uint8_t *)&sLifeMsg, sizeof(sLifeMsg), CID_ALIVE_H); 
   sLifeMsg.cSignals = 0;
}
      
//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//

void vCan_State()
{             
   LIFE_MSG                      sLifeMsg;
   uint8_t                       ucBytes[8]                 = {0};
   uint8_t                       ucBuff[20]                 = {0};
   
  	switch (appData.uiCanState)
   {
      case 0:        // Start
         
         appData.uiCanState = 1;         
      break;
      case 1:
         
         appData.uiCanState = 2;  
      break;                         
      case 2:
         sprintf(ucBuff, "TX:%lu RX:%lu%c", appData.ulCanTXtotal, appData.ulCanRXtotal, '\0');
         strncpy(appData.uiDebugbuffer5, ucBuff, 16);
         
         appData.uiCanState = 3;  
      break;           
      case 3:
         
         appData.uiCanState = 4;  
      break;           
      case 4: 
         if (appData.cSigCount > 6)
         {        
            vSendGsmSignal();
         }       
         else
            appData.cSigCount++;
         appData.uiCanState = 5;  
      break;           
      case 5:      
         if (appData.cLifeCount > 5) 
         {
            vSendAlive();
            appData.cLifeCount = 0;
         }
         else
            appData.cLifeCount++;
         appData.uiCanState = 1;         
      break;           
   }   
}

//*********************************************************************************************************************
// 
// 
//*********************************************************************************************************************
//
















