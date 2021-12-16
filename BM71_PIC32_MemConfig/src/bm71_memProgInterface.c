#include "device.h"
#include "definitions.h"                // SYS function prototypes
#include "bm71_memProgInterface.h"
#include "string.h"
#include "bm71_memProgCmds.h"
#include "bm71_AppModeInterface.h"
//#include "bm71_testFunctions.h"

static BM_PROGRAM_CMDPKT            pgmCmdPkt;
static uint8_t                      pgmDataCmdPkt[BM_PGM_PKT_LEN_MAX];
BM_PROGRAM_CMDPKT*                  program_cmdpkt = NULL;
static uint8_t                      pgmRspData[BM_PGM_PKT_LEN_MAX];
static uint16_t                     pgmDataIndex = 0;
static BM_PROGRAM_CMD_STATE         pgmCmdState = BM_PGM_HCI_EVENT;

event_t                             appEvent;
static uint8_t resp_event_id = INVALID_EVENT_ID;

//static uint16_t program_handle;

static BM71_UART_PGM_DATA_STRUCT uartMemPgmData;
volatile uint8_t BM71_uartMemPgmRxCount;

uint8_t outputNewLine1 = '\n';

void UART2_WriteNewLine(void)
{
    uint8_t outputNewLine = '\n';
    uint8_t outputCarrReturn = '\r';
    
    while (UART2_WriteIsBusy());
    UART2_Write(&outputNewLine,sizeof(outputNewLine));
    while (UART2_WriteIsBusy());
    UART2_Write(&outputCarrReturn,sizeof(outputCarrReturn));
    while (UART2_WriteIsBusy());
}

//Utility function to generate an Hex format number
void format_hex(uint8_t * str, uint16_t val)
{
    uint8_t x,y;

    for (x=0;x<4;x++)
    {
        y = val & 0xf;
        if ( y > 9 )
            str[3-x] =y +55;
        else
            str[3-x] =y +48;

        val = val >> 4;
    }
    str[4]=0;
}



void BM71_UART_MemPgmRxEventHandler(uintptr_t context)
{
    uartMemPgmData.rxBuffer[uartMemPgmData.rxHead++] = uartMemPgmData.rxData;
    UART3_Read((void*)&uartMemPgmData.rxData, 1);    // set up first read
    if(uartMemPgmData.rxHead >= RX_BUFFER_SIZE)
    {
        uartMemPgmData.rxHead = 0;
    }
    BM71_uartMemPgmRxCount++;
    GPIO_RA0_Set();       //This is LED3 which corresponds to pin P17 on PIM
}

void BM71_MemPgmInitialize(void){
    GPIO_RA0_Clear();
    GPIO_RA1_LED4_Clear();
    GPIO_PinInterruptCallbackRegister(GPIO_RD13_ButS4_PIN, PushButton_Handler, 0);
    GPIO_PinInterruptEnable(GPIO_RD13_ButS4_PIN);

    UART2_WriteCallbackRegister(Serial_UART2_TxEventHandler, 0);
    isUART2TxComplete = false;
    
    // Register an event handler with driver. This is done once
    //UART3_ReadCallbackRegister(BM71_UART_RxEventHandler, 0);
    UART3_ReadCallbackRegister(BM71_UART_MemPgmRxEventHandler, 0);
}


BM_PROGRAM_CMDPKT* BM_PROGRAM_Init()
{
    BM_PROGRAM_CMDPKT* programCmdPkt = &pgmCmdPkt;
    programCmdPkt->cmdPkt = pgmDataCmdPkt;
    programCmdPkt->length = 0;
    appEvent.event_id = BM_PROGRAM_HCI_ERROR;
    appEvent.event_msg.data = NULL;
    appEvent.event_msg.data_len = 0;
    return programCmdPkt;
}

void BM_PROGRAM_Connect(BM_PROGRAM_CMDPKT* programCmdPkt)
{
    BM_PROGRAM_CONNECT_PKT* connect_pkt = (BM_PROGRAM_CONNECT_PKT*)(programCmdPkt->cmdPkt);
    connect_pkt->hci_command_pkt.type = COMMAND_PKT_TYPE;
    connect_pkt->hci_command_pkt.opcode = 0x0405;
    connect_pkt->hci_command_pkt.length = sizeof(BM_PROGRAM_CONNECT_PKT)-sizeof(HCI_COMMAND_PKT);
    memset(connect_pkt->addr, 0, 6);
    connect_pkt->type = 0;
    connect_pkt->mode = 0;
    connect_pkt->offset = 0;
    connect_pkt->role = 0;
    programCmdPkt->length = sizeof(BM_PROGRAM_CONNECT_PKT);
}

void BM_PROGRAM_Disconnect(BM_PROGRAM_CMDPKT* programCmdPkt)
{
    BM_PROGRAM_DISCONNECT_PKT* disconnect_pkt = (BM_PROGRAM_DISCONNECT_PKT*)(programCmdPkt->cmdPkt);
    disconnect_pkt->hci_command_pkt.type = COMMAND_PKT_TYPE;
    disconnect_pkt->hci_command_pkt.opcode = 0x0406;
    disconnect_pkt->hci_command_pkt.length = sizeof(BM_PROGRAM_DISCONNECT_PKT)-sizeof(HCI_COMMAND_PKT);
    disconnect_pkt->conn_handle = 0x0FFF;   //TBD: Update with received flash connection handle 
    disconnect_pkt->reason = 0;
	programCmdPkt->length = sizeof(BM_PROGRAM_DISCONNECT_PKT);
}

void BM_PROGRAM_Erase(BM_PROGRAM_CMDPKT* programCmdPkt, BM_PROGRAM_CMD* pgm_cmd)
{
    BM_PROGRAM_DATA_PKT* erase_pkt = (BM_PROGRAM_DATA_PKT*)(programCmdPkt->cmdPkt);
    erase_pkt->acl_data_pkt.type = DATA_ACL_PKT_TYPE;
    erase_pkt->acl_data_pkt.conn_handle = pgm_cmd->handle;//0x0FFF;    //fill
    erase_pkt->acl_data_pkt.length = sizeof(BM_PROGRAM_DATA_PKT)-sizeof(HCI_DATA_PKT);
    erase_pkt->pgm_cmd_id = 0x0112;
    erase_pkt->pgm_data_len = erase_pkt->acl_data_pkt.length-4; 
    erase_pkt->pgm_mem_type = 0x03;
    erase_pkt->pgm_mem_subtype = 0x00;
    erase_pkt->address = pgm_cmd->address;
    erase_pkt->size = pgm_cmd->length;
	programCmdPkt->length = sizeof(BM_PROGRAM_DATA_PKT);
}

void BM_PROGRAM_Write(BM_PROGRAM_CMDPKT* programCmdPkt, BM_PROGRAM_CMD* pgm_cmd)
{    
    BM_PROGRAM_DATA_PKT* write_pkt = (BM_PROGRAM_DATA_PKT*)(programCmdPkt->cmdPkt);
    write_pkt->acl_data_pkt.type = DATA_ACL_PKT_TYPE;
    write_pkt->acl_data_pkt.conn_handle = pgm_cmd->handle;//0x0FFF;    //fill
    write_pkt->acl_data_pkt.length = sizeof(BM_PROGRAM_DATA_PKT)-sizeof(HCI_DATA_PKT)+pgm_cmd->length;
    write_pkt->pgm_cmd_id = 0x0111;
    write_pkt->pgm_data_len = write_pkt->acl_data_pkt.length-4; 
    if (pgm_cmd->write_continue)
    {
        write_pkt->pgm_data_len |= 0x8000;
    }
    write_pkt->pgm_mem_type = 0x03;
    write_pkt->pgm_mem_subtype = 0x00;
    write_pkt->address = pgm_cmd->address;
    if (pgm_cmd->write_continue)
    {
        write_pkt->size = pgm_cmd->write_continue_length;
    }
    else
    {
        write_pkt->size = pgm_cmd->length;
    }
    memcpy(programCmdPkt->cmdPkt+sizeof(BM_PROGRAM_DATA_PKT), pgm_cmd->data, pgm_cmd->length);
    programCmdPkt->length = sizeof(BM_PROGRAM_DATA_PKT)+pgm_cmd->length;
}

void BM_PROGRAM_Write_Continue(BM_PROGRAM_CMDPKT* programCmdPkt, BM_PROGRAM_CMD* pgm_cmd)
{    
    BM_PROGRAM_DATA_ALT_PKT* write_pkt = (BM_PROGRAM_DATA_ALT_PKT*)(programCmdPkt->cmdPkt);
    write_pkt->acl_data_pkt.type = DATA_ACL_PKT_TYPE;
    write_pkt->acl_data_pkt.conn_handle = pgm_cmd->handle;//0x0FFF;    //fill
    write_pkt->acl_data_pkt.length = sizeof(BM_PROGRAM_DATA_ALT_PKT)-sizeof(HCI_DATA_PKT)+pgm_cmd->length;
    write_pkt->pgm_cmd_id = 0x0001;
    write_pkt->pgm_data_len = write_pkt->acl_data_pkt.length-4; 
    if (pgm_cmd->write_continue)
    {
        write_pkt->pgm_data_len |= 0x8000;
    }
    memcpy(programCmdPkt->cmdPkt+sizeof(BM_PROGRAM_DATA_ALT_PKT), pgm_cmd->data, pgm_cmd->length);
    programCmdPkt->length = sizeof(BM_PROGRAM_DATA_ALT_PKT)+pgm_cmd->length;
}

void BM_PROGRAM_Read(BM_PROGRAM_CMDPKT* programCmdPkt, BM_PROGRAM_CMD* pgm_cmd)
{  
    BM_PROGRAM_DATA_PKT* read_pkt = (BM_PROGRAM_DATA_PKT*)(programCmdPkt->cmdPkt);
    read_pkt->acl_data_pkt.type = DATA_ACL_PKT_TYPE;
    read_pkt->acl_data_pkt.conn_handle = pgm_cmd->handle;//0x0FFF;    //fill
    read_pkt->acl_data_pkt.length = sizeof(BM_PROGRAM_DATA_PKT)-sizeof(HCI_DATA_PKT);
    read_pkt->pgm_cmd_id = 0x0110;
    read_pkt->pgm_data_len = read_pkt->acl_data_pkt.length-4; 
    read_pkt->pgm_mem_type = 0x03;
    read_pkt->pgm_mem_subtype = 0x00;
    read_pkt->address = pgm_cmd->address;
    read_pkt->size = pgm_cmd->length;
    programCmdPkt->length = sizeof(BM_PROGRAM_DATA_PKT);
}

void BM_PROGRAM_StartConnection()
{
    program_cmdpkt = (BM_PROGRAM_CMDPKT*)BM_PROGRAM_Init();
    BM_PROGRAM_Connect(program_cmdpkt);
    UART3_Write(program_cmdpkt->cmdPkt, (uint32_t) program_cmdpkt->length);
    while (UART3_WriteIsBusy());
    UART3_Read((void*)&uartMemPgmData.rxData, 1);
}

void BM_PROGRAM_EndConnection()
{
    //program_cmdpkt = (BM_PROGRAM_CMDPKT*)BM_PROGRAM_Init();
    BM_PROGRAM_Disconnect(program_cmdpkt);
    UART3_Write(program_cmdpkt->cmdPkt, (uint32_t) program_cmdpkt->length);
    while (UART3_WriteIsBusy());
    UART3_Read((void*)&uartMemPgmData.rxData, 1);
}

void BM_PROGRAM_EraseMemory(uint16_t program_handle, uint32_t address, uint8_t length)
{
    BM_PROGRAM_CMD pgm_cmd;
    pgm_cmd.handle = program_handle;
    pgm_cmd.address = address;
    pgm_cmd.data = NULL;
    pgm_cmd.length = length;
    pgm_cmd.write_continue = 0;
    
    BM_PROGRAM_Erase(program_cmdpkt, &pgm_cmd);
	UART3_Write(program_cmdpkt->cmdPkt, (uint32_t) program_cmdpkt->length);
}

dfu_status_t BM_PROGRAM_write_start(uint16_t program_handle, uint32_t address, uint8_t* data, uint8_t length, uint8_t continue_mode, uint32_t continue_length)
{    
    dfu_status_t status = DFU_SUCCESS;
    
    BM_PROGRAM_CMD pgm_cmd;
    pgm_cmd.handle = program_handle;
    //pgm_cmd.address = 0x00000000;
    pgm_cmd.address = address;
    pgm_cmd.data = data;
    pgm_cmd.length = length;
    pgm_cmd.write_continue = true;
    
    if (continue_mode == DFU_WRITE_CONTINUE_START)
    {
        pgm_cmd.address = address;
        pgm_cmd.write_continue_length = continue_length;
        BM_PROGRAM_Write(program_cmdpkt, &pgm_cmd);
    }
    else if (continue_mode == DFU_WRITE_CONTINUE_PROCEED)
    {
        BM_PROGRAM_Write_Continue(program_cmdpkt, &pgm_cmd);
    }
    else if (continue_mode == DFU_WRITE_CONTINUE_STOP)
    {
        pgm_cmd.write_continue = false;
        BM_PROGRAM_Write_Continue(program_cmdpkt, &pgm_cmd);
    }
    UART3_Write(program_cmdpkt->cmdPkt, (uint32_t) program_cmdpkt->length);
	/*status = interface_cmd_send_wait(program_cmdpkt->cmdPkt, (uint32_t) program_cmdpkt->length, EVENT_PKT_TYPE);
	if(DFU_SUCCESS == status)
	{
		//Check the status in status-report event 
		status = dfu_status_report_parser(BM_PROGRAM_HCI_HANDLE);
        
        interface_cmd_wait(DATA_ACL_PKT_TYPE);
        status = dfu_response_report_parser(BM_PROGRAM_HCI_SUCCESS);
	}*/
		
	return status;   
}


uint8_t BM71_UART_MemPgm_RxReg_Read(void)
{
    uint8_t readByte;

    if (BM71_uartMemPgmRxCount == 0)
    {
        readByte = READ_UART_ERR_BYTE;
    }
    else
    {
        readByte=uartMemPgmData.rxBuffer[uartMemPgmData.rxTail++];
        if(uartMemPgmData.rxTail >= RX_BUFFER_SIZE)
        {
            uartMemPgmData.rxTail = 0;
        }
        BM71_uartMemPgmRxCount--;
        GPIO_RA0_Clear();
    }
    
    return readByte;
}

bool BM_PROGRAM_ResponseCheck(uint8_t* data, uint8_t len)
{    
    while(len--)
    {
        switch(pgmCmdState)
        {
            case BM_PGM_HCI_EVENT:
                if(*data == BM_PGM_HCI_EVENT_VAL || *data == BM_PGM_HCI_ACL_VAL)
                {
                    pgmRspData[BM_PGM_HCI_EVENT_INDEX] = *data;
                    
                    if (*data == BM_PGM_HCI_EVENT_VAL)
                        pgmCmdState = BM_PGM_HCI_EVENT_TYPE;
                    else if (*data == BM_PGM_HCI_ACL_VAL)
                        pgmCmdState = BM_PGM_HCI_ACL_HANDLEL;
            		appEvent.event_id = BM_PROGRAM_HCI_ERROR;
                    appEvent.event_msg.data = NULL;
                    appEvent.event_msg.data_len = 0;
                    GPIO_RA1_LED4_Clear();
                }
                break;

            case BM_PGM_HCI_EVENT_TYPE:
                {
                    pgmRspData[BM_PGM_HCI_EVENT_TYPE_INDEX] = *data;
                    pgmCmdState = BM_PGM_HCI_EVENT_LEN;
                }
                break;

            case BM_PGM_HCI_EVENT_LEN:
                {
                    pgmRspData[BM_PGM_HCI_EVENT_LEN_INDEX] = *data;
                    pgmCmdState = BM_PGM_HCI_EVENT_DATA;
                }
                break;

            case BM_PGM_HCI_EVENT_DATA:
                pgmRspData[BM_PGM_HCI_EVENT_DATA_INDEX+pgmDataIndex] = *data;
                pgmDataIndex++;
                if(pgmDataIndex >= pgmRspData[BM_PGM_HCI_EVENT_LEN_INDEX])
                {
                    pgmDataIndex = 0;
                    pgmCmdState = BM_PGM_HCI_EVENT;
                    
                    if(pgmRspData[BM_PGM_HCI_EVENT_TYPE_INDEX] == BM_PROGRAM_HCI_COMPLETE || 
                       pgmRspData[BM_PGM_HCI_EVENT_TYPE_INDEX] == BM_PROGRAM_HCI_STATUS ||
                       pgmRspData[BM_PGM_HCI_EVENT_TYPE_INDEX] == BM_PROGRAM_HCI_NOCP)
                    {
						appEvent.event_id = pgmRspData[BM_PGM_HCI_EVENT_INDEX];
						appEvent.event_msg.data = &pgmRspData[BM_PGM_HCI_EVENT_DATA_INDEX];
						appEvent.event_msg.data_len = pgmRspData[BM_PGM_HCI_EVENT_LEN_INDEX];
	                }
                    else
                    {
						appEvent.event_id = BM_PROGRAM_HCI_ERROR;
						appEvent.event_msg.data = NULL;
						appEvent.event_msg.data_len = 0;
                    }
                    return true;                    
                }
                break;

            case BM_PGM_HCI_ACL_HANDLEL:
                {
                    pgmRspData[BM_PGM_HCI_ACL_HANDLEL_INDEX] = *data;
                    pgmCmdState = BM_PGM_HCI_ACL_HANDLEH;
                }
                break;
            
            case BM_PGM_HCI_ACL_HANDLEH:
                {
                    pgmRspData[BM_PGM_HCI_ACL_HANDLEH_INDEX] = *data;
                    pgmCmdState = BM_PGM_HCI_ACL_LENL;
                }
                break;
            
            case BM_PGM_HCI_ACL_LENL:
                {
                    pgmRspData[BM_PGM_HCI_ACL_LENL_INDEX] = *data;
                    pgmCmdState = BM_PGM_HCI_ACL_LENH;
                }
                break;
                
            case BM_PGM_HCI_ACL_LENH:
                {
                    pgmRspData[BM_PGM_HCI_ACL_LENH_INDEX] = *data;
                    pgmCmdState = BM_PGM_HCI_ACL_DATA;
                }
                break;
                
            case BM_PGM_HCI_ACL_DATA:
                {
                    pgmRspData[BM_PGM_HCI_ACL_DATA_INDEX+pgmDataIndex] = *data;
                    pgmDataIndex++;
                    if(pgmDataIndex >= (pgmRspData[BM_PGM_HCI_ACL_LENH_INDEX]<<8 | pgmRspData[BM_PGM_HCI_ACL_LENL_INDEX]))
                    {
                        pgmDataIndex = 0;
                        pgmCmdState = BM_PGM_HCI_EVENT;
                   
						appEvent.event_id = pgmRspData[BM_PGM_HCI_EVENT_INDEX];
						appEvent.event_msg.data = &pgmRspData[BM_PGM_HCI_ACL_DATA_INDEX];
						appEvent.event_msg.data_len = (pgmRspData[BM_PGM_HCI_ACL_LENH_INDEX]<<8 | pgmRspData[BM_PGM_HCI_ACL_LENL_INDEX]);
                     
						return true;
                    }
                }
                break;
                
            default:
                break;
        }
        if(len != 0)
            data++;
    }
	return false;
}

uint8_t bm71_uartRx_process_data(void)
{
	//uint8_t byte;
	bool result;
    uint8_t current_byte;
	
	while (BM71_uartMemPgmRxCount)
	{
		current_byte = BM71_UART_MemPgm_RxReg_Read();
        result = BM_PROGRAM_ResponseCheck(&current_byte,1);
		if (result)
		{
			return INTERFACE_EVENT_READY;
		}
	}
	return INTERFACE_EVENT_WAITING;
}

bool is_waiting_for_this_response(uint8_t event_id)
{
	if((event_id == resp_event_id) && (INVALID_EVENT_ID != resp_event_id))
    //if(event_id == EVENT_PKT_TYPE)
	{
		return true;
	}
	
	return false;
}
bool bm71_memProg_event_process(uint8_t event_id)
{
	bool is_event_ready = bm71_uartRx_process_data();
	bool is_event_received = false;
	resp_event_id = EVENT_PKT_TYPE;
    do 
    {
        if(INTERFACE_EVENT_READY == is_event_ready)
        {
            if((event_id == resp_event_id) && (INVALID_EVENT_ID != resp_event_id))
            //if(is_waiting_for_this_response(appEvent.event_id))
            {
                is_event_received = true;
                GPIO_RA1_LED4_Set();
            }
        }
        else
        {
            is_event_received = false;
            appEvent.event_msg.data_len--;
            is_event_ready = bm71_uartRx_process_data();
        }
    } while (false == is_event_received); /* Is expected event received? or timeout happened*/

	return is_event_received;
}

//This was defined as 'static' in main DFU code. But I had to remove this. 
dfu_status_t hci_response_status_report_parser(BM_PROGRAM_HCI_EVENT_STATUS status_id)
{
	event_t *cmd_frame = (event_t *) &appEvent;
	
	if(status_id == cmd_frame->event_msg.data[0])
	{
		return DFU_SUCCESS;
	}
	
	return DFU_FAIL;
}

//This was defined as 'static' in main DFU code. But I had to remove this. 
uint16_t hci_response_status_handle_parser(BM_PROGRAM_HCI_EVENT_STATUS status_id)
{
	event_t *cmd_frame = (event_t *) &appEvent;
	
	if(status_id == cmd_frame->event_msg.data[0])
	{
		return ((cmd_frame->event_msg.data[2]<<8) | cmd_frame->event_msg.data[1]);
	}
	
	return 0;
}

uint8_t* hci_response_read_parser(BM_PROGRAM_HCI_EVENT_STATUS status_id)
{
	event_t *cmd_frame = (event_t *) &appEvent;
	
	if(status_id == (cmd_frame->event_msg.data[4] | cmd_frame->event_msg.data[5]<<8))
	{
		return &(cmd_frame->event_msg.data[6]);
	}
	
	return NULL;
}

dfu_status_t BM_PROGRAM_memory_read(uint16_t program_handle, uint32_t address, uint8_t* data, uint8_t length)
{
    dfu_status_t status = DFU_SUCCESS;
    
    BM_PROGRAM_CMD pgm_cmd;
    pgm_cmd.handle = program_handle;
    pgm_cmd.address = address;
    pgm_cmd.data = data;
    pgm_cmd.length = length;
    
    BM_PROGRAM_Read(program_cmdpkt, &pgm_cmd);
    UART3_Write(program_cmdpkt->cmdPkt, (uint32_t) program_cmdpkt->length);
    while (UART3_WriteIsBusy());
    UART3_Read((void*)&uartMemPgmData.rxData, 1);
	/*status = interface_cmd_send_wait(program_cmdpkt->cmdPkt, (uint32_t) program_cmdpkt->length, EVENT_PKT_TYPE);
	if(DFU_SUCCESS == status)
	{
        uint8_t* buff; 
	
		status = dfu_status_report_parser(BM_PROGRAM_HCI_HANDLE);
        
        interface_cmd_wait(DATA_ACL_PKT_TYPE);
        status = dfu_response_report_parser(BM_PROGRAM_HCI_SUCCESS);
        
        buff = dfu_response_read_parser(BM_PROGRAM_HCI_SUCCESS);
        if(buff != NULL)
        {
            memcpy(data, buff, length);
        }
	}*/
		
	return status;   
}