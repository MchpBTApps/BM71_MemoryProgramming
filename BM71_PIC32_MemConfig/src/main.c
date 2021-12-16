/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#define EVENT_BUFFER_LENGTH		1024

//#define FLASH_TOTAL_SIZE 0x40000
#define FLASH_TOTAL_SIZE 0x17FF
#define FLASH_CHUNK_SIZE 128
//#define FLASH_CHUNK_SIZE 0x80
#define FLASH_START_ADDRESS 0x00034000


#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include "bm71_AppModeInterface.h"
#include <string.h>
#include "bm71_modeConfig.h"
#include "bm71_memProgInterface.h"



dfu_status_t status = DFU_SUCCESS;
static uint16_t program_handle;

const uint8_t dfu_image[] = {
    #include "BM7x_array.txt"
//    #include "RN487x_128.txt"
//    #include "rn487x.txt"  
};

//uint8_t outputNewLine = '\n';
//uint8_t outputCarrReturn = '\r';
uint8_t start[] = "BM71 DFU Process Started";
uint8_t outputConnected[] = "Connected to Flash ";
uint8_t outputErased[] = "Erased previous content ";
uint8_t outputDisconnected1[] = "_Discon1";
uint8_t outputDisconnected2[] = "Disconnected from Flash ";
uint8_t outputWriteStart[] = "Started writing flash";
uint8_t outputWriteContinue[] = " Section written. ";
uint8_t outputWriteStop[] = " Finished write process";
uint8_t outputVerifyStart[] = "First write verified. ";
uint8_t outputVerifyContinue[] = " section verified. ";
uint8_t outputVerifyFail[] = " Failed section ";
uint8_t cmdFail[] = "Fail. \n";

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void ){
    
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    uint8_t str[32];
    uint16_t i=0;
    uint16_t cntr = 0;
    bool     rsdu = false;
    
    if (FLASH_TOTAL_SIZE%FLASH_CHUNK_SIZE == 0)
    {
        cntr = (FLASH_TOTAL_SIZE/FLASH_CHUNK_SIZE) - 1;
        rsdu = false;
    }
    else
    {
        cntr = FLASH_TOTAL_SIZE/FLASH_CHUNK_SIZE;
        rsdu = true;
    }
    //format_hex(str, i);
    //SetProgramMode(0);
    //BM_MODE_Set(BM_MODE_APPLICATION);

    BM71_MemPgmInitialize();
    UART2_Write(&start,sizeof(start));
    UART2_WriteNewLine();
    /*while (UART2_WriteIsBusy());
    UART2_Write(&outputNewLine,sizeof(outputNewLine));*/
    isUART2TxComplete = false;
    
    BM71_CommandDecodeInit();
    
    //BM71_SendCommand();
    //BM71_SendCmdPtr();
    //BM71_ProcessCommand();
    //BM71_SendCmd2Ptr();
    //BM71_SendCommand_DeviceName();
    //BM71_ProcessCommand();
    
    BM_MODE_Set(BM_MODE_PROGRAM);
    BM_PROGRAM_StartConnection();
    while (UART3_WriteIsBusy());
    if (bm71_memProg_event_process(EVENT_PKT_TYPE))
    {
        status = hci_response_status_report_parser(BM_PROGRAM_HCI_SUCCESS);
        if (bm71_memProg_event_process(EVENT_PKT_TYPE))
        {
            status = hci_response_status_report_parser(BM_PROGRAM_HCI_SUCCESS);
            program_handle = hci_response_status_handle_parser(BM_PROGRAM_HCI_SUCCESS);
            UART2_Write(&outputConnected,sizeof(outputConnected));
            UART2_WriteNewLine();
        }            
    }
    else
    {
        UART2_Write(&cmdFail,sizeof(cmdFail));
        UART2_WriteNewLine();
    }
    //while (UART2_WriteIsBusy());
    
    /*BM_PROGRAM_EraseMemory(program_handle, 0x00000000, 0);
    while (UART3_WriteIsBusy());
    
    if (bm71_memProg_event_process(EVENT_PKT_TYPE))
    {
        status = hci_response_status_report_parser(BM_PROGRAM_HCI_SUCCESS);
        if (bm71_memProg_event_process(EVENT_PKT_TYPE))
        {
            status = hci_response_status_report_parser(BM_PROGRAM_HCI_SUCCESS);
            UART2_Write(&outputErased,sizeof(outputErased));
            UART2_WriteNewLine();
        }            
    }
    else
    {
        UART2_Write(&cmdFail,sizeof(cmdFail));
        UART2_WriteNewLine();
    }
    //while (UART2_WriteIsBusy());*/
    
    
    //BM_PROGRAM_write_start(program_handle, 0x00000000, (uint8_t*)dfu_image, FLASH_CHUNK_SIZE, DFU_WRITE_CONTINUE_START, FLASH_TOTAL_SIZE);
    BM_PROGRAM_write_start(program_handle, 0x00034000, (uint8_t*)dfu_image, FLASH_CHUNK_SIZE, DFU_WRITE_CONTINUE_START, FLASH_TOTAL_SIZE);
    while (UART3_WriteIsBusy());
    
    if (bm71_memProg_event_process(EVENT_PKT_TYPE))
    {
        status = hci_response_status_report_parser(BM_PROGRAM_HCI_SUCCESS);
        if (bm71_memProg_event_process(EVENT_PKT_TYPE))
        {            
            UART2_Write(&outputWriteStart,sizeof(outputWriteStart));
            UART2_WriteNewLine();
        }            
    }
    else
    {
        UART2_Write(&cmdFail,sizeof(cmdFail));
        UART2_WriteNewLine();
    }
    //while (UART2_WriteIsBusy());
    
    //BM_PROGRAM_memory_read(program_handle, 0x00000000, (uint8_t*)dfu_image, FLASH_CHUNK_SIZE);
    BM_PROGRAM_memory_read(program_handle, 0x00034000, (uint8_t*)dfu_image, FLASH_CHUNK_SIZE);
    
    if (bm71_memProg_event_process(EVENT_PKT_TYPE))
    {
        
        uint8_t* buff;
        status = hci_response_status_report_parser(BM_PROGRAM_HCI_HANDLE);
        if (bm71_memProg_event_process(EVENT_PKT_TYPE))
        {
            status = hci_response_status_report_parser(BM_PROGRAM_HCI_SUCCESS);
            buff = hci_response_read_parser(BM_PROGRAM_HCI_SUCCESS);
            if(buff != NULL)
            {
                if(memcmp(buff, (uint8_t*)dfu_image, FLASH_CHUNK_SIZE) == 0)
                {
                   UART2_Write(&outputVerifyStart,sizeof(outputVerifyStart));
                }
                else
                {
                    UART2_Write(&cmdFail,sizeof(cmdFail));
                }
                UART2_WriteNewLine();
            }
        }
    }
    //cntr = 4;
    for (i=1; i<(cntr); i++)
    {
        //BM_PROGRAM_write_start(program_handle, 0x00000000, ((uint8_t*)dfu_image)+(FLASH_CHUNK_SIZE*i), FLASH_CHUNK_SIZE, DFU_WRITE_CONTINUE_PROCEED, 0x00000000);
        BM_PROGRAM_write_start(program_handle, 0x00034000, ((uint8_t*)dfu_image)+(FLASH_CHUNK_SIZE*i), FLASH_CHUNK_SIZE, DFU_WRITE_CONTINUE_PROCEED, FLASH_CHUNK_SIZE);
        while (UART3_WriteIsBusy());
        format_hex(str, i);
        UART2_Write(str,4);
        while (UART2_WriteIsBusy());
        
        if (bm71_memProg_event_process(EVENT_PKT_TYPE))
        {
            if (bm71_memProg_event_process(EVENT_PKT_TYPE))
            {
                UART2_Write(&outputWriteContinue,sizeof(outputWriteContinue));
            }            
        }
        else
        {
            UART2_Write(&cmdFail,sizeof(cmdFail));
        }
        while (UART2_WriteIsBusy());
        
        //BM_PROGRAM_memory_read(program_handle, (FLASH_CHUNK_SIZE*i), ((uint8_t*)dfu_image)+(FLASH_CHUNK_SIZE*i), FLASH_CHUNK_SIZE);

        BM_PROGRAM_memory_read(program_handle, (FLASH_START_ADDRESS +(FLASH_CHUNK_SIZE*i)), ((uint8_t*)dfu_image)+(FLASH_CHUNK_SIZE*i), FLASH_CHUNK_SIZE);
        if (bm71_memProg_event_process(EVENT_PKT_TYPE))
        {
            uint8_t* buff;
            status = hci_response_status_report_parser(BM_PROGRAM_HCI_HANDLE);
            if (bm71_memProg_event_process(EVENT_PKT_TYPE))
            {
                status = hci_response_status_report_parser(BM_PROGRAM_HCI_SUCCESS);
                buff = hci_response_read_parser(BM_PROGRAM_HCI_SUCCESS);
                if(buff != NULL)
                {
                    if(memcmp(buff, ((uint8_t*)dfu_image)+(FLASH_CHUNK_SIZE*i), FLASH_CHUNK_SIZE) == 0)
                    {
                       UART2_Write(&outputVerifyContinue,sizeof(outputVerifyContinue)); 
                    }
                    else
                    {
                        UART2_Write(&cmdFail,sizeof(cmdFail));
                    }
                    while (UART2_WriteIsBusy());
                }
            }
        }
    }
    
    if (rsdu)
    {
        //BM_PROGRAM_write_start(program_handle, 0x00000000, ((uint8_t*)dfu_image)+(FLASH_CHUNK_SIZE*i), FLASH_TOTAL_SIZE - (FLASH_CHUNK_SIZE*i), DFU_WRITE_CONTINUE_STOP, 0x00000000);
        BM_PROGRAM_write_start(program_handle, 0x00034000, ((uint8_t*)dfu_image)+(FLASH_CHUNK_SIZE*i), FLASH_TOTAL_SIZE - (FLASH_CHUNK_SIZE*i), DFU_WRITE_CONTINUE_STOP, 0x00034000);
        while (UART3_WriteIsBusy());
    }
    else
    {
        //BM_PROGRAM_write_start(program_handle, 0x00000000, ((uint8_t*)dfu_image)+(FLASH_CHUNK_SIZE*i), FLASH_CHUNK_SIZE, DFU_WRITE_CONTINUE_STOP, 0x00000000);
        BM_PROGRAM_write_start(program_handle, 0x00034000, ((uint8_t*)dfu_image)+(FLASH_CHUNK_SIZE*i), FLASH_CHUNK_SIZE, DFU_WRITE_CONTINUE_STOP, 0x00034000);
        while (UART3_WriteIsBusy());
    }
    
    if (bm71_memProg_event_process(EVENT_PKT_TYPE))
        {
            if (bm71_memProg_event_process(EVENT_PKT_TYPE))
            {
                UART2_Write(&outputWriteStop,sizeof(outputWriteStop));
            }            
        }
        else
        {
            UART2_Write(&cmdFail,sizeof(cmdFail));
        }
    UART2_WriteNewLine();
    //while (UART2_WriteIsBusy());

    
    BM_PROGRAM_EndConnection();
    while (UART3_WriteIsBusy());
    if (bm71_memProg_event_process(EVENT_PKT_TYPE))
    {
        if (bm71_memProg_event_process(EVENT_PKT_TYPE))
        {
            UART2_Write(&outputDisconnected2,sizeof(outputDisconnected2));
        }            
    }
    else
    {
        UART2_Write(&cmdFail,sizeof(cmdFail));
    }
    UART2_WriteNewLine();
    //while (UART2_WriteIsBusy());
    
    /*while ( true )
    {
    }*/

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

