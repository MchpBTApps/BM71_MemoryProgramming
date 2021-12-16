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

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include "device.h"
#include "bm71_AppModeInterface.h"
#include "definitions.h"                // SYS function prototypes
#include "bm71_testFunctions.h"
#include "bm71_memProgInterface.h"



#define BM_PKT_LEN_MAX    0x01FF

#define BM_APPLICATION_MODE_CMD_ATTACH(x)	((BM_APPLICATION_CMD*)&appCmds[x-1])

static BM71_UART_DATA_STRUCT uartData;
volatile uint8_t BM71_uartRxCount;
volatile uint8_t BM71_txByteQueued; 

static uint8_t  CmdDecodedFlag;
static uint8_t  CmdBuffer[BM71_CMD_SIZE_MAX];

static BM71_RX_DECODE_MODE_ENUM  CmdDecodeState;
static uint8_t  CmdDecodeCmdLength;
static uint8_t  CmdDecodeChecksum;			
static uint8_t  CmdDecodeDataCnt;                    //temporary variable in decoding
static unsigned short CmdBufferPt;

// This array which consists of the structure with the opcode and the 
// associated length for each opcode command as it is used to build the packet.
static BM_APPLICATION_CMD bm70ApplicationCmds[] =
{
    {BM_LOCAL_INFORMATION_READ,     	0x00},
    {BM_RESET,                          0x00},
    {BM_STATUS_READ,                	0x00},
    {BM_ADC_READ,                       0x01},	
    {BM_SHUTDOWN,                       0x00},
    {BM_DEBUG,                          0xFF},
    {BM_NAME_READ,                  	0x00},
};

// This variable just stores the starting address of the bm70ApplicationCmds
// starting array and is used in the main code.
static BM_APPLICATION_CMD*          appCmds;

BM_APPLICATION_CMDPKT* application_cmdpkt = NULL;
//BM_APPLICATION_CMDPKT* application_cmdpkt;

uint8_t                             appDataCmdPkt[BM_PKT_LEN_MAX];
BM_APPLICATION_CMDPKT               appCmdPkt = {0, appDataCmdPkt};
//BM_APPLICATION_CMDPKT               appCmdPkt;

BM_APPLICATION_CMDPKT* BM_APPLICATION_Init()
{
    appCmdPkt.cmdPkt = appDataCmdPkt;
    appCmdPkt.length = 0;
    appCmds = bm70ApplicationCmds;
    return &appCmdPkt;
}

uint8_t calc_chksum_8bit(uint8_t* data, uint16_t length)
{
    uint32_t chksum = 0;
    uint8_t* payload = data;
    uint16_t i;
    for(i=0; i<length; i++)
        chksum += *payload++;
    chksum = ~chksum;
    chksum += 1;
    return (uint8_t)chksum;
}

static void BM_APPLICATION_CmdPkt(BM_APPLICATION_CMDPKT* applicationCmdPkt,BM_APPLICATION_CMD* applicationCmd, uint8_t* data )
{
	uint8_t* cmdPkt = applicationCmdPkt->cmdPkt;
	applicationCmdPkt->length = (5 + applicationCmd->length);

    //Command for Read status AA 00 01 03 FC
    *cmdPkt++ = 0xAA;
    *cmdPkt++ = (uint8_t)((applicationCmd->length + 1) >> 8);
    *cmdPkt++ = (uint8_t)(applicationCmd->length + 1);
    *cmdPkt++ = applicationCmd->opcode;
    *cmdPkt = calc_chksum_8bit(applicationCmdPkt->cmdPkt + 1, applicationCmd->length + 3);
}

void BM_APPLICATION_ReadStatus(BM_APPLICATION_CMDPKT* applicationCmdPkt)
{
    BM_APPLICATION_CmdPkt(applicationCmdPkt, BM_APPLICATION_MODE_CMD_ATTACH(BM_STATUS_READ), NULL);
}

void BM_APPLICATION_ReadName(BM_APPLICATION_CMDPKT* applicationCmdPkt)
{
    BM_APPLICATION_CmdPkt(applicationCmdPkt, BM_APPLICATION_MODE_CMD_ATTACH(BM_NAME_READ), NULL);
}


void SetProgramMode(int Mode)
{
    if (Mode)
    {
        GPIO_RG14_Clear();
    }
    else
    {
        GPIO_RG14_Set();
    }    
}

void PushButton_Handler(GPIO_PIN pin, uintptr_t context)
{
    if (GPIO_RD13_ButS4_Get() == 1)
    {
        GPIO_RG13_Set();
    }
    else
    {
        GPIO_RG13_Clear();
    }
}

void Serial_UART2_TxEventHandler(uintptr_t context)
{
    isUART2TxComplete = true;
}

void BM71_UART_RxEventHandler(uintptr_t context)
{
    uartData.rxBuffer[uartData.rxHead++] = uartData.rxData;
    UART3_Read((void*)&uartData.rxData, 1);    // set up first read
    if(uartData.rxHead >= RX_BUFFER_SIZE)
    {
        uartData.rxHead = 0;
    }
    BM71_uartRxCount++;
    //GPIO_RA0_Set();       //This is LED3 which corresponds to pin P17 on PIM
}

uint8_t BM71_UART_Read(void)
{
    uint8_t readByte;

    if (BM71_uartRxCount == 0)
    {
        readByte = readErr;
    }
    else
    {
        readByte=uartData.rxBuffer[uartData.rxTail++];
        if(uartData.rxTail >= RX_BUFFER_SIZE)
        {
            uartData.rxTail = 0;
        }
        BM71_uartRxCount--;
        //GPIO_RA0_Clear();
    }
    
    return readByte;
}

void BM71_CommandDecodeInit( void )
{
    CmdDecodedFlag = 0;
    CmdDecodeState = BM71_RX_DECODE_CMD_SYNC_AA;
    while(BM71_uartRxCount)
    {
        BM71_UART_Read();     // flush buffer
        UART2_Write(&read11,sizeof(read11));
    }
    BM71_uartRxCount = 0;
}

void BM71_CommandResultHandler(void) {
    uint8_t current_byte;

    while (BM71_uartRxCount) {
        current_byte = BM71_UART_Read();
        //UART2_Write(&current_byte,sizeof(current_byte));

        switch (CmdDecodeState) {
            case BM71_RX_DECODE_CMD_SYNC_AA:
                if (current_byte == 0xaa)
                    CmdDecodeState = BM71_RX_DECODE_CMD_SYNC_00;
                break;

            case BM71_RX_DECODE_CMD_SYNC_00:
                if (current_byte == 0x00)
                    CmdDecodeState = BM71_RX_DECODE_CMD_LENGTH;
                else
                    CmdDecodeState = BM71_RX_DECODE_CMD_SYNC_AA;
                break;

            case BM71_RX_DECODE_CMD_LENGTH:
                CmdDecodedFlag = 0; //command receive flag clear
                CmdBufferPt = 0; //buffer reset for command parameter
                CmdDecodeCmdLength = current_byte;
                CmdDecodeChecksum = current_byte; //checksum calculation start!
                CmdDecodeDataCnt = current_byte; //save bytes number, use to check where is command end
                CmdDecodeState = BM71_RX_DECODE_CMD_DATA; //next state
                break;

            case BM71_RX_DECODE_CMD_DATA:
                CmdDecodeChecksum += current_byte;
                CmdDecodeDataCnt--;
                CmdBuffer[CmdBufferPt++] = current_byte;
                if (CmdDecodeDataCnt == 0) //no data remained?
                    CmdDecodeState = BM71_RX_DECODE_CMD_CHECKSUM; //yes, next mode: checksum
                break;

            case BM71_RX_DECODE_CMD_CHECKSUM:
                if ((uint8_t) (CmdDecodeChecksum + current_byte) == 0) {
                    CmdDecodedFlag = 1;
                } else {
                }
                CmdDecodeState = BM71_RX_DECODE_CMD_SYNC_AA;
                break;
            default:
                break;
        }

        if (CmdDecodedFlag) {
            //GPIO_RA1_LED4_Clear();
            break;
        }
    }
}

void BM71_CommandResponseParser(void)
{
    switch(CmdBuffer[0])
    {
        case BM71_COMMAND_COMPLETE:           // 0x80
            // CmdBuffer[1] is command that completed
            // CmdBuffer[2] is status (0 == success)
            // CmdBuffer[3-] are optional parameters is status (0 == success)
            switch (CmdBuffer[1])
            {
                case 0x07:
                    if (CmdBuffer[2] == 0x00)
                    {
                        UART2_Write(&readDevice,sizeof(readDevice));
                        while (UART2_WriteIsBusy());
                        UART2_Write(&CmdBuffer[3], sizeof(CmdBuffer[3]));
                        while (UART2_WriteIsBusy());
                    }
                    break;
                case 0x09:
                    UART2_Write(&currentStatus,sizeof(currentStatus));
                    while (UART2_WriteIsBusy());
                    break;
            }
            break;
            
        case BM71_STATUS_REPORT:              // 0x81
            // CmdBuffer[1] is status
            // 0x01 -- Scanning Mode
            // 0x02 -- Connecting Mode
            // 0x03 -- Standby Mode
            // 0x05 -- Broadcast Mode
            // 0x08 -- Transparent Service Enabled Mode
            // 0x09 -- Idle Mode
            // 0x0a -- Shutdown Mode
            // 0x0b -- Configure Mode
            // 0x0c -- BLE connected
            switch(CmdBuffer[1])
            { 
                case 0x09:
                    UART2_Write(&currentStatus,sizeof(currentStatus));
                    while (UART2_WriteIsBusy());
                    UART2_Write(&idleMode,sizeof(idleMode));
                    break;
            }
            break;
        default:
            UART2_Write(&failed,sizeof(failed));
            while (UART2_WriteIsBusy());
            break;
    }
}


void BM71_CommandDecodeMain( void )
{
	BM71_CommandResultHandler();
	if(CmdDecodedFlag)
	{
        UART2_Write(&success,sizeof(success));
	}
    while (UART2_WriteIsBusy());
}

void BM71_Initialize(void){
    GPIO_RA0_Clear();
    GPIO_RA1_LED4_Clear();
    GPIO_PinInterruptCallbackRegister(GPIO_RD13_ButS4_PIN, PushButton_Handler, 0);
    GPIO_PinInterruptEnable(GPIO_RD13_ButS4_PIN);

    UART2_WriteCallbackRegister(Serial_UART2_TxEventHandler, 0);
    isUART2TxComplete = false;
    
    // Register an event handler with driver. This is done once
    UART3_ReadCallbackRegister(BM71_UART_RxEventHandler, 0);
    //UART3_ReadCallbackRegister(BM71_UART_MemPgmRxEventHandler, 0);
}

void BM71_SendCmdPtr(void)
{
    application_cmdpkt = (BM_APPLICATION_CMDPKT*)BM_APPLICATION_Init();
    //BM_APPLICATION_CmdPkt(application_cmdpkt);
    BM_APPLICATION_ReadStatus(application_cmdpkt);
    UART3_Write(application_cmdpkt->cmdPkt,5);
    while (UART3_WriteIsBusy());
    UART3_Read((void*)&uartData.rxData, 1);
}

void BM71_SendCmd2Ptr(void)
{
    application_cmdpkt = (BM_APPLICATION_CMDPKT*)BM_APPLICATION_Init();
    //BM_APPLICATION_CmdPkt(application_cmdpkt);
    BM_APPLICATION_ReadName(application_cmdpkt);
    UART3_Write(application_cmdpkt->cmdPkt,5);
    while (UART3_WriteIsBusy());
    UART3_Read((void*)&uartData.rxData, 1);
}


void BM71_SendCommand(void){
    
    //Command to read device name: AA 00 01 07 F8
    //Command to read device status AA 00 01 03 FC
    cmdBuf[0] = 0xAA;
    cmdBuf[1] = 0x00;
    cmdBuf[2] = 0x01;
    cmdBuf[3] = 0x03;
    cmdBuf[4] = 0xFC;
    
    UART3_Write(&cmdBuf[0],1);
   
    while (UART3_WriteIsBusy());
    UART3_Write(&cmdBuf[1],1);
    while (UART3_WriteIsBusy());
    UART3_Write(&cmdBuf[2],1);
    while (UART3_WriteIsBusy());
    UART3_Write(&cmdBuf[3],1);
    while (UART3_WriteIsBusy());
    UART3_Write(&cmdBuf[4],1);
    while (UART3_WriteIsBusy());
    UART3_Read((void*)&uartData.rxData, 1); 
}

void BM71_ProcessCommand(void){
    while (!CmdDecodedFlag)
    {
        BM71_CommandDecodeMain();
    }
    if (CmdDecodedFlag)
    {
        BM71_CommandResponseParser();
        CmdDecodedFlag = 0;
    }
}

void BM71_SendCommand_DeviceName(void){
    
    //Command to read device name: AA 00 01 07 F8
    cmdBuf[0] = 0xAA;
    cmdBuf[1] = 0x00;
    cmdBuf[2] = 0x01;
    cmdBuf[3] = 0x07;
    cmdBuf[4] = 0xF8;
    
    UART3_Write(&cmdBuf[0],1);
    while (UART3_WriteIsBusy());
    UART3_Write(&cmdBuf[1],1);
    while (UART3_WriteIsBusy());
    UART3_Write(&cmdBuf[2],1);
    while (UART3_WriteIsBusy());
    UART3_Write(&cmdBuf[3],1);
    while (UART3_WriteIsBusy());
    UART3_Write(&cmdBuf[4],1);
    while (UART3_WriteIsBusy());
    UART3_Read((void*)&uartData.rxData, 1); 
}
    