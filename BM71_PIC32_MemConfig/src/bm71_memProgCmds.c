#include "bm71_memProgInterface.h"
#include "string.h"
/*
//static BM_PROGRAM_CMDPKT            pgmCmdPkt;
//static uint8_t                      pgmDataCmdPkt[BM_PGM_PKT_LEN_MAX];
event_t                             appEvent;

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
*/
