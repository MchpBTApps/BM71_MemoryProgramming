
#ifndef BM71_MEMPROGINTERFACE_H    /* Guard against multiple inclusion */
#define BM71_MEMPROGINTERFACE_H


/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
#include <stddef.h>
#include <stdbool.h>  
#include "definitions.h"                // SYS function prototypes
//#include "bm71_RxFifo.h"


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif



    /* ************************************************************************** */
    /* Section: Constants                                                         */
    /* ************************************************************************** */
#define BM_PGM_PKT_LEN_MAX  320 
#define BM_PGM_HCI_ACL_VAL 0x02
#define BM_PGM_HCI_EVENT_VAL    0x04
    
#define RX_BUFFER_SIZE      50
    
#define READ_UART_ERR_BYTE  0x99
#define INVALID_EVENT_ID	0xFF
    
    // *****************************************************************************
    // Section: Data Types
    // *****************************************************************************
enum
{
	INTERFACE_EVENT_WAITING,
	INTERFACE_EVENT_READY,
};

typedef enum
{
	DFU_WRITE_CONTINUE_START,
	DFU_WRITE_CONTINUE_PROCEED,
	DFU_WRITE_CONTINUE_STOP,
} dfu_write_continue_mode_t;

typedef struct
{
	uint8_t *data;
	uint32_t data_len;
}event_msg_t;

typedef struct
{
	event_msg_t event_msg;
	uint8_t event_id;
}event_t;

typedef enum
{
	BLE_EVENT_SUCCESS,
	BLE_EVENT_Q_OVERFLOW,
	BLE_EVENT_Q_EMPTY,
	BLE_EVENT_Q_INSUFFICIENT_MEMORY,
}event_status_t;

typedef enum
{
    BM71_UART_MEMPGM_STATE_INIT=0, 	/* Application's state machine's initial state. */
    BM71_UART_MEMPGM_STATE_TRANSMIT,
    BM71_UART_MEMPGM_STATE_RECEIVE,
    BM71_UART_MEMPGM_STATE_ERROR,	
	/* TODO: Define states used by the application state machine. */
} BM71_UART_MEMPGM_STATES_ENUM;
    
typedef struct
{    
    BM71_UART_MEMPGM_STATES_ENUM state;    /* The application's current state */
    uint8_t rxBuffer[RX_BUFFER_SIZE];    
    uint8_t rxHead;    
    uint8_t rxTail;
    volatile uint8_t rxData;
    /* TODO: Define any additional data used by the application. */
} BM71_UART_PGM_DATA_STRUCT;
    
typedef enum _bm_pgm_hci_index
{
    BM_PGM_HCI_EVENT_INDEX = 0x00,
    BM_PGM_HCI_EVENT_TYPE_INDEX,
    BM_PGM_HCI_EVENT_LEN_INDEX,
    BM_PGM_HCI_EVENT_DATA_INDEX,
} BM_PGM_HCI_INDEX;

typedef enum _bm_pgm_hci_acl_index
{
    BM_PGM_HCI_ACL_HANDLEL_INDEX = 0x01,
    BM_PGM_HCI_ACL_HANDLEH_INDEX,
    BM_PGM_HCI_ACL_LENL_INDEX,
    BM_PGM_HCI_ACL_LENH_INDEX,
    BM_PGM_HCI_ACL_DATA_INDEX,     
} BM_PGM_HCI_ACL_INDEX;

typedef enum {
    COMMAND_PKT_TYPE = 0x01,
    DATA_ACL_PKT_TYPE = 0x02,
    EVENT_PKT_TYPE = 0x04,
}HCI_PKT_TYPE;

typedef enum _bm_program_hci_event_id
{
	BM_PROGRAM_HCI_COMPLETE = 0x03,
	BM_PROGRAM_HCI_STATUS = 0x0F,
	BM_PROGRAM_HCI_NOCP = 0x13,
	BM_PROGRAM_HCI_ERROR = 0xFF
} BM_PROGRAM_HCI_EVENT_ID;

typedef struct __attribute__ ((packed)) _hci_command_pkt
{
    uint8_t type;
    uint16_t opcode;
    uint8_t length;
}HCI_COMMAND_PKT;

typedef struct __attribute__ ((packed)) _hci_data_pkt
{
    uint8_t type;
    uint16_t conn_handle;
    uint16_t length;
}HCI_DATA_PKT;

typedef struct __attribute__ ((packed)) _bm_program_connect_pkt
{
    HCI_COMMAND_PKT hci_command_pkt;
    uint8_t addr[6];
    uint16_t type;
    uint8_t mode;
    uint8_t rsvd;
    uint16_t offset;
    uint8_t role;
}BM_PROGRAM_CONNECT_PKT;

typedef struct __attribute__ ((packed)) _bm_program_disconnect_pkt
{
    HCI_COMMAND_PKT hci_command_pkt;
    uint16_t conn_handle;
    uint8_t reason;
}BM_PROGRAM_DISCONNECT_PKT;


typedef struct _bm_program_cmdpkt
{
    uint8_t length;
    uint8_t* cmdPkt;
} BM_PROGRAM_CMDPKT;

typedef struct __attribute__ ((packed)) _bm_program_data_pkt
{
    HCI_DATA_PKT acl_data_pkt;
    uint16_t pgm_cmd_id;
    uint16_t pgm_data_len;
    uint8_t pgm_mem_type;
    uint8_t pgm_mem_subtype;
    uint32_t address;
    uint32_t size;
}BM_PROGRAM_DATA_PKT;

typedef struct __attribute__ ((packed)) _bm_program_data_alt_pkt
{
    HCI_DATA_PKT acl_data_pkt;
    uint16_t pgm_cmd_id;
    uint16_t pgm_data_len;
}BM_PROGRAM_DATA_ALT_PKT;

typedef enum _bm_program_cmd_state
{
	BM_PGM_HCI_EVENT = 0x00,
    BM_PGM_HCI_EVENT_TYPE,
    BM_PGM_HCI_EVENT_LEN,
    BM_PGM_HCI_EVENT_DATA,
            
    BM_PGM_HCI_ACL_HANDLEH,
    BM_PGM_HCI_ACL_HANDLEL,
    BM_PGM_HCI_ACL_LENH,
    BM_PGM_HCI_ACL_LENL,
    BM_PGM_HCI_ACL_DATA,
} BM_PROGRAM_CMD_STATE;

typedef struct _bm_program_cmd
{
    uint16_t handle;
    uint32_t address;
    uint8_t* data;
    uint8_t length;
    bool    write_continue;
    uint32_t write_continue_length; 
} BM_PROGRAM_CMD;

typedef enum
{
	//Success
	DFU_SUCCESS  = 0x00,
	DFU_FAIL
} dfu_status_t;

typedef enum _bm_program_hci_event_status
{
    BM_PROGRAM_HCI_SUCCESS = 0x00,
    BM_PROGRAM_HCI_HANDLE = 0x01,        
	BM_PROGRAM_HCI_FAIL
} BM_PROGRAM_HCI_EVENT_STATUS;

    // *****************************************************************************
    // Section: Interface Functions
    // *****************************************************************************

void UART2_WriteNewLine(void);
void format_hex(uint8_t * str, uint16_t val);
void BM71_UART_MemPgmRxEventHandler(uintptr_t context);
void BM71_MemPgmInitialize(void);
/*BM_PROGRAM_CMDPKT* BM_PROGRAM_Init();
void BM_PROGRAM_Connect(BM_PROGRAM_CMDPKT* programCmdPkt);*/

void BM_PROGRAM_Connect(BM_PROGRAM_CMDPKT* programCmdPkt);
void BM_PROGRAM_Disconnect(BM_PROGRAM_CMDPKT* programCmdPkt);
void BM_PROGRAM_Erase(BM_PROGRAM_CMDPKT* programCmdPkt, BM_PROGRAM_CMD* pgm_cmd);
void BM_PROGRAM_Write(BM_PROGRAM_CMDPKT* programCmdPkt, BM_PROGRAM_CMD* pgm_cmd);
void BM_PROGRAM_Write_Continue(BM_PROGRAM_CMDPKT* programCmdPkt, BM_PROGRAM_CMD* pgm_cmd);
void BM_PROGRAM_Read(BM_PROGRAM_CMDPKT* programCmdPkt, BM_PROGRAM_CMD* pgm_cmd);


void BM_PROGRAM_StartConnection();
void BM_PROGRAM_EndConnection();
//void BM_PROGRAM_EraseMemory(uint16_t program_handle, uint32_t address, uint8_t length);
dfu_status_t BM_PROGRAM_write_start(uint16_t program_handle, uint32_t address, uint8_t* data, uint8_t length, uint8_t continue_mode, uint32_t continue_length);
uint8_t BM71_UART_MemPgm_RxReg_Read(void);
bool BM_PROGRAM_ResponseCheck(uint8_t* data, uint8_t len);
uint8_t bm71_uartRx_process_data(void);
bool is_waiting_for_this_response(uint8_t event_id);
bool bm71_memProg_event_process(uint8_t event_id);
dfu_status_t hci_response_status_report_parser(BM_PROGRAM_HCI_EVENT_STATUS status_id);
uint16_t hci_response_status_handle_parser(BM_PROGRAM_HCI_EVENT_STATUS status_id);
uint8_t* hci_response_read_parser(BM_PROGRAM_HCI_EVENT_STATUS status_id);
dfu_status_t BM_PROGRAM_memory_read(uint16_t program_handle, uint32_t address, uint8_t* data, uint8_t length);
void BM_PROGRAM_EraseMemory(uint16_t program_handle, uint32_t address, uint8_t length);
    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
