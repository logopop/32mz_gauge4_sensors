/*******************************************************************************
  USART driver static implementation of Byte model.

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usart_static_byte_model.c

  Summary:
    Source code for the USART driver static implementation of Byte model.

  Description:
    This file contains the source code for the static implementation of the
    USART driver Byte model.

  Remarks:
    Static interfaces incorporate the driver instance number within the names
    of the routines, eliminating the need for an object ID or object handle.

    Static single-open interfaces also eliminate the need for the open handle.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END
// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "system_config.h"
#include "system_definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************
extern DRV_USART_OBJ  gDrvUSART0Obj ;

// *****************************************************************************
// *****************************************************************************
// Section: Instance 0 static driver functions
// *****************************************************************************
// *****************************************************************************

uint8_t DRV_USART0_ReadByte(void)
{
    uint8_t readValue;
	
    /* Receive one byte */
    readValue = PLIB_USART_ReceiverByteReceive(USART_ID_2);

    return readValue;
}

void DRV_USART0_WriteByte(const uint8_t byte)
{
    /* Send one byte */
    PLIB_USART_TransmitterByteSend(USART_ID_2, byte);
    SYS_INT_SourceEnable(INT_SOURCE_USART_2_TRANSMIT);
}

unsigned int DRV_USART0_ReceiverBufferSizeGet(void)
{
    return 8;
}

unsigned int DRV_USART0_TransmitBufferSizeGet(void)
{
    return 8;
}

bool DRV_USART0_ReceiverBufferIsEmpty( void )
{
    /* Check the status of receiver buffer */
    return(!PLIB_USART_ReceiverDataIsAvailable(USART_ID_2));
}

bool DRV_USART0_TransmitBufferIsFull(void)
{
    /* Check the status of transmitter buffer */
    return(PLIB_USART_TransmitterBufferIsFull(USART_ID_2));
}

void DRV_USART0_ByteTransmitCallbackSet(const DRV_USART_BYTE_EVENT_HANDLER eventHandler)
{
        gDrvUSART0Obj.transmitCallback = eventHandler;
}

void DRV_USART0_ByteReceiveCallbackSet(const DRV_USART_BYTE_EVENT_HANDLER eventHandler)
{
        gDrvUSART0Obj.receiveCallback = eventHandler;
}

void DRV_USART0_ByteErrorCallbackSet(const DRV_USART_BYTE_EVENT_HANDLER eventHandler)
{
        gDrvUSART0Obj.errorCallback = eventHandler;
}

extern DRV_USART_OBJ  gDrvUSART1Obj ;

// *****************************************************************************
// *****************************************************************************
// Section: Instance 1 static driver functions
// *****************************************************************************
// *****************************************************************************

uint8_t DRV_USART1_ReadByte(void)
{
    uint8_t readValue;
	
    /* Receive one byte */
    readValue = PLIB_USART_ReceiverByteReceive(USART_ID_4);

    return readValue;
}

void DRV_USART1_WriteByte(const uint8_t byte)
{
    /* Send one byte */
    PLIB_USART_TransmitterByteSend(USART_ID_4, byte);
    SYS_INT_SourceEnable(INT_SOURCE_USART_4_TRANSMIT);
}

unsigned int DRV_USART1_ReceiverBufferSizeGet(void)
{
    return 8;
}

unsigned int DRV_USART1_TransmitBufferSizeGet(void)
{
    return 8;
}

bool DRV_USART1_ReceiverBufferIsEmpty( void )
{
    /* Check the status of receiver buffer */
    return(!PLIB_USART_ReceiverDataIsAvailable(USART_ID_4));
}

bool DRV_USART1_TransmitBufferIsFull(void)
{
    /* Check the status of transmitter buffer */
    return(PLIB_USART_TransmitterBufferIsFull(USART_ID_4));
}

void DRV_USART1_ByteTransmitCallbackSet(const DRV_USART_BYTE_EVENT_HANDLER eventHandler)
{
        gDrvUSART1Obj.transmitCallback = eventHandler;
}

void DRV_USART1_ByteReceiveCallbackSet(const DRV_USART_BYTE_EVENT_HANDLER eventHandler)
{
        gDrvUSART1Obj.receiveCallback = eventHandler;
}

void DRV_USART1_ByteErrorCallbackSet(const DRV_USART_BYTE_EVENT_HANDLER eventHandler)
{
        gDrvUSART1Obj.errorCallback = eventHandler;
}

extern DRV_USART_OBJ  gDrvUSART2Obj ;

// *****************************************************************************
// *****************************************************************************
// Section: Instance 2 static driver functions
// *****************************************************************************
// *****************************************************************************

uint8_t DRV_USART2_ReadByte(void)
{
    uint8_t readValue;
	
    /* Receive one byte */
    readValue = PLIB_USART_ReceiverByteReceive(USART_ID_1);

    return readValue;
}

void DRV_USART2_WriteByte(const uint8_t byte)
{
    /* Send one byte */
    PLIB_USART_TransmitterByteSend(USART_ID_1, byte);
    SYS_INT_SourceEnable(INT_SOURCE_USART_1_TRANSMIT);
}

unsigned int DRV_USART2_ReceiverBufferSizeGet(void)
{
    return 8;
}

unsigned int DRV_USART2_TransmitBufferSizeGet(void)
{
    return 8;
}

bool DRV_USART2_ReceiverBufferIsEmpty( void )
{
    /* Check the status of receiver buffer */
    return(!PLIB_USART_ReceiverDataIsAvailable(USART_ID_1));
}

bool DRV_USART2_TransmitBufferIsFull(void)
{
    /* Check the status of transmitter buffer */
    return(PLIB_USART_TransmitterBufferIsFull(USART_ID_1));
}

void DRV_USART2_ByteTransmitCallbackSet(const DRV_USART_BYTE_EVENT_HANDLER eventHandler)
{
        gDrvUSART2Obj.transmitCallback = eventHandler;
}

void DRV_USART2_ByteReceiveCallbackSet(const DRV_USART_BYTE_EVENT_HANDLER eventHandler)
{
        gDrvUSART2Obj.receiveCallback = eventHandler;
}

void DRV_USART2_ByteErrorCallbackSet(const DRV_USART_BYTE_EVENT_HANDLER eventHandler)
{
        gDrvUSART2Obj.errorCallback = eventHandler;
}

/*******************************************************************************
 End of File
*/
