/*******************************************************************************
  BM71 Functions for BM71 module connected to UART

  Company:
    Microchip Technology Inc.

  File Name:
    bm71_interface.h

  Summary:
    Provides functions for the BM71 UART interface.

  Description:
    The BM71 module is a BLE module that provide BT functionality.

  Remarks:

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#ifndef BM71_INTERFACE_H
#define BM71_INTERFACE_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>
#include <stdbool.h>  
#include "definitions.h"                // SYS function prototypes
// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility

	extern "C" {

#endif

#define TX_BUFFER_SIZE      50
#define RX_BUFFER_SIZE      50
#define BM71_CMD_SIZE_MAX   200

uint8_t cmdBuf[64]; 
uint8_t response[20];
volatile bool isUART2TxComplete; 
//volatile bool BM71_awaitingResponse = true; 

// This enumerated datatype stores the list of opcode commands 
// available. This opcode will be referenced by the array named 
// bm70ApplicationCmds which stores the opcode values?
typedef enum _bm_application_cmd_opcode
{
	BM_LOCAL_INFORMATION_READ 	= 0x01,
    BM_RESET,
    BM_STATUS_READ,
    BM_ADC_READ,
    BM_SHUTDOWN             	= 0x05,
    BM_DEBUG,
    BM_NAME_READ,
    BM_NAME_WRITE,
} BM_APPLICATION_CMD_OPCODE;

//This is the structure that stores the opcode and the corresponding 
//length associated with the command when it is used to construct the whole 
// command packet.
typedef struct _bm_application_cmd
{
	BM_APPLICATION_CMD_OPCODE opcode;
	uint16_t length;
} BM_APPLICATION_CMD;

typedef enum
{
    BM71_UART_STATE_INIT=0, 	/* Application's state machine's initial state. */
    BM71_UART_STATE_TRANSMIT,
    BM71_UART_STATE_RECEIVE,
    BM71_UART_STATE_ERROR,	
	/* TODO: Define states used by the application state machine. */
} BM71_UART_STATES_ENUM;

typedef enum {
	BM71_RX_DECODE_CMD_SYNC_AA,
	BM71_RX_DECODE_CMD_SYNC_00,
	BM71_RX_DECODE_CMD_LENGTH,
	BM71_RX_DECODE_CMD_DATA,
	BM71_RX_DECODE_CMD_CHECKSUM
} BM71_RX_DECODE_MODE_ENUM;

typedef enum _bm71_application_event_opcode
{
	BM71_EVENT_NONE         = 0x00,
    BM71_COMMAND_COMPLETE   = 0x80,
    BM71_STATUS_REPORT      = 0x81,
    BM71_ERROR              = 0xFF
} BM71_APPLICATION_EVENT_OPCODE;

typedef struct
{    
    BM71_UART_STATES_ENUM state;    /* The application's current state */
    uint8_t rxBuffer[RX_BUFFER_SIZE];    
    uint8_t rxHead;    
    uint8_t rxTail;
    volatile uint8_t rxData;
    /* TODO: Define any additional data used by the application. */
} BM71_UART_DATA_STRUCT;


typedef struct _bm_application_cmdpkt
{
    uint16_t length;
    uint8_t* cmdPkt;
} BM_APPLICATION_CMDPKT;

uint8_t calc_chksum_8bit(uint8_t* data, uint16_t length);        
void SetProgramMode(int Mode);
void PushButton_Handler(GPIO_PIN pin, uintptr_t context);
void Serial_UART2_TxEventHandler(uintptr_t context);
void BM71_UART_RxEventHandler(uintptr_t context);
uint8_t BM71_UART_Read(void);
void BM71_CommandDecodeInit( void );
void BM71_CommandResultHandler(void);
void BM71_CommandResponseParser(void);
void BM71_CommandDecodeMain( void );
void BM71_Initialize(void);
void BM71_SendCmdPtr(void);
void BM71_SendCmd2Ptr(void);
void BM71_SendCommand(void);
void BM71_ProcessCommand(void);
void BM71_SendCommand_DeviceName(void);

#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif //PLIB_CLK_H

