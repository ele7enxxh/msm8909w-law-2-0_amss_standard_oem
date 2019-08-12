/*============================================================================
  FILE:         system_db_modem.c
 
  OVERVIEW:     Implement calls to RPM side system_DB 
 
  DEPENDENCIES: None
 
                Copyright (c) 2012 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

$Header: //components/rel/core.mpss/3.5.c12.3/power/system_db/src/system_db_modem.c#1 $
$Date: 2016/12/13 $
============================================================================*/

#include "system_db.h"
#include "system_db_modem.h"
#include "smd_lite.h"
#include "rpm.h"
#include "rpm_wire.h"
#include "DALSys.h"
#include "DALStdDef.h"

#ifdef SYSDB_PC_SIM
#include <string.h>
#endif

/* Message key sentinel passed in with message. */
#define MSG__KEY 0x2367736d

void RPM_system_db_init( void )
{
  // placeholder in case we need an init function at a later date.
}

/*
 * System DB service API
 *
 * This is the interface to system db.  There is one read and one write function
 * This talks over RPM and adds a message to the queue management service in 
 * RPM and will read or write depending on what mode it is entering/exiting.
 *
 */

/* For the read:
 * @param event_type This is the event type for when the read will happen.  Check
 *                   system_db.h for the event types.  Immediate happens immediately and will
 *                   not be put into the queue.
 * @param start_addr Start address for the range of address to be read 
 * @param num        Number of 32 bit register reads to be performed, sequentially 
 *                   starting at start_addr 
 * @param command_id Command ID to be recorded with the read 
 * @param addr_type  Type of address being read GENERIC, SPMI. Used to determine if an 
 *                   SPMI operation or direct access 
 */
void RPM_system_db_read( SYSTEMDB_EVENT_TYPE	event_type,
                         uint32 start_addr, 
                         unsigned int num, 
                         uint32 command_id,
                         SYSTEMDB_ADDR_TYPE addr_type )
{
  uint32 read_data[SYSTEMDB_READ_SIZE] = { 0 };
  //Need an RPMMessageHeader b/c it is checked on the RPM
  RPMMessageHeader header;
  smdl_iovec_type header_dat, system_db_read_msg;
  //set the data;
  memset(&header, 0, sizeof(RPMMessageHeader));
  header.msg_id = MSG__KEY;
  header.data_len = sizeof(read_data);
  SET_OP_TYPE_COMMAND_ID( read_data[0], command_id );
  SET_OP_TYPE_RW( read_data[0], SYSTEMDB_OP_READ );
  SET_OP_TYPE_ADDR_TYPE( read_data[0], addr_type );
  SET_OP_TYPE_EVENT_TYPE( read_data[0], event_type );
  read_data[1] = start_addr;
  read_data[2] = num;
  //set up the smdl data to have the correct size and data ptrs
  system_db_read_msg.next = NULL;
  system_db_read_msg.length = sizeof(read_data);
  system_db_read_msg.buffer = read_data;
  header_dat.next = &system_db_read_msg;
  header_dat.length = sizeof(header);
  header_dat.buffer = &header;
  //send the message
  rpmclient_put( RPM_SYSTEMDB_SERVICE, &header_dat );
}

/* For the write:
 * @param event_type This is the event type for when the write will happen.  Check
 *                   system_db.h for the event types.  Immediate happens immediately and will
 *                   not be put into the queue.
 * @param start_addr Start address for the range of address to be written 
 * @param num        Number of 32 bit register writes to be performed, sequentially 
 *                   starting at start_addr 
 * @param data       Data to be written to the specified address 
 * @param mask       Mask to apply to data for writing, i.e. allows to differentiate what 
 *                   bits of the data should be applied and which 	are dont cares.
 * @param command_id Command ID to be recorded with the write 
 * @param addr_type  Type of address being read GENERIC, SPMI. Used to determine if an 
 *                   SPMI operation or direct access 
 */
void RPM_system_db_write( SYSTEMDB_EVENT_TYPE	event_type,
                          uint32 start_addr, 
                          unsigned int num, 
                          uint32 data, 
                          uint32 mask, 
                          uint32 command_id,
                          SYSTEMDB_ADDR_TYPE addr_type )
{
  uint32 write_data[SYSTEMDB_WRITE_SIZE] = { 0 };
  //Need an RPMMessageHeader b/c it is checked on the RPM
  RPMMessageHeader header;
  smdl_iovec_type header_dat, system_db_write_msg;
  //set the data;
  memset(&header, 0, sizeof(RPMMessageHeader));
  header.msg_id = MSG__KEY;
  header.data_len = sizeof(write_data);
  SET_OP_TYPE_COMMAND_ID( write_data[0], command_id );
  SET_OP_TYPE_RW( write_data[0], SYSTEMDB_OP_WRITE );
  SET_OP_TYPE_ADDR_TYPE( write_data[0], addr_type );
  SET_OP_TYPE_EVENT_TYPE( write_data[0], event_type );
  write_data[1] = start_addr;
  write_data[2] = num;
  if ( addr_type == SYSTEMDB_PMIC )
  {
    data &= 0xFF;
    mask &= 0xFF;
    write_data[3] = data<<24 | data<<16 | data<<8 | data;
    write_data[4] = mask<<24 | mask<<16 | mask<<8 | mask;
  }
  else
  {
    write_data[3] = data;
    write_data[4] = mask;
  }
  //set up the smdl data to have the correct size and data ptrs
  system_db_write_msg.next = NULL;
  system_db_write_msg.length = sizeof(write_data);
  system_db_write_msg.buffer = write_data;
  header_dat.next = &system_db_write_msg;
  header_dat.length = sizeof(header);
  header_dat.buffer = &header;
  //send the message
  rpmclient_put( RPM_SYSTEMDB_SERVICE, &header_dat );
}

/* Clears all operations from the system DB queue */
void RPM_system_db_clearall( void )
{
  uint32 clearall_data[SYSTEMDB_CLEARALL_SIZE];
  //Need an RPMMessageHeader b/c it is checked on the RPM
  RPMMessageHeader header;
  smdl_iovec_type header_dat, system_db_clearall_msg;
  //set up the smdl data to have the correct size and data ptrs
  memset(&header, 0, sizeof(RPMMessageHeader));
  header.msg_id = MSG__KEY;
  header.data_len = sizeof(clearall_data);
  //set type to sentinel value
  clearall_data[0] = 0xFFFFFFFF;
  //set up the smdl data to have the correct size and data ptrs
  system_db_clearall_msg.next = NULL;
  system_db_clearall_msg.length = sizeof(clearall_data);
  system_db_clearall_msg.buffer = clearall_data;
  header_dat.next = &system_db_clearall_msg;
  header_dat.length = sizeof(header);
  header_dat.buffer = &header;
  rpmclient_put( RPM_SYSTEMDB_SERVICE, &header_dat );
}

/* Sends command to move commands from standby queue to op queue */
void RPM_system_db_end_of_commands( void )
{
  uint32 endofcmds_data[SYSTEMDB_ENDOFCMDS_SIZE];
  //Need an RPMMessageHeader b/c it is checked on the RPM
  RPMMessageHeader header;
  smdl_iovec_type header_dat, system_db_endofcmds_msg;
  //set up the smdl data to have the correct size and data ptrs
  memset(&header, 0, sizeof(RPMMessageHeader));
  header.msg_id = MSG__KEY;
  header.data_len = sizeof(endofcmds_data);
  //set type to sentinel value
  endofcmds_data[0] = 0xF0F0F0F0;
  //set up the smdl data to have the correct size and data ptrs
  system_db_endofcmds_msg.next = NULL;
  system_db_endofcmds_msg.length = sizeof(endofcmds_data);
  system_db_endofcmds_msg.buffer = endofcmds_data;
  header_dat.next = &system_db_endofcmds_msg;
  header_dat.length = sizeof(header);
  header_dat.buffer = &header;
  rpmclient_put( RPM_SYSTEMDB_SERVICE, &header_dat );
}

