/*============================================================================
  FILE:         system_db_modem.h
 
  OVERVIEW:     Define functions used on the modem for systemdb.
 
  DEPENDENCIES: None
 
                Copyright (c) 2012 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

$Header: //components/rel/core.mpss/3.5.c12.3/power/system_db/inc/system_db_modem.h#1 $
$Date: 2016/12/13 $

============================================================================*/
#ifndef SYSTEMDBMODEM_H
#define SYSTEMDBMODEM_H
#include "system_db.h"
#include "DALStdDef.h"

/* initialize RPM transport */
void RPM_system_db_init( void );

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
void RPM_system_db_read( SYSTEMDB_EVENT_TYPE event_type, 
                         uint32 start_addr, 
                         unsigned int num, 
                         uint32 command_id, 
                         SYSTEMDB_ADDR_TYPE addr_type);

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

void RPM_system_db_write( SYSTEMDB_EVENT_TYPE event_type, 
                          uint32 start_addr, 
                          unsigned int num, 
                          uint32 data, 
                          uint32 mask, 
                          uint32 command_id, 
                          SYSTEMDB_ADDR_TYPE addr_type);

/* Clears all operations from the system DB queue */
void RPM_system_db_clearall( void );

/* Ends the command stream */
void RPM_system_db_end_of_commands( void );

#endif
