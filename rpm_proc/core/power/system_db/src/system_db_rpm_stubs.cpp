/*============================================================================
  FILE:         system_db_rpm.c
 
  OVERVIEW:     Handle system db events on the RPM processor
 
  DEPENDENCIES: None
 
                Copyright (c) 2013 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

$Header: //components/rel/rpm.bf/2.1.1/core/power/system_db/src/system_db_rpm_stubs.cpp#1 $
============================================================================*/
#include "system_db.h"

// 0 -> zi.data, 192 -> runtime allocated data, 24 -> rw.data, 16 -> data used for RPM server services entry
static char reserve_systemDB_DATA[0 + 192 + 24 + 16] __attribute__((used));
// 740 -> text, -4 -> remaining text, 6 -> text lost from rpm_cpp due to removing systemDB
static const char reserve_systemDB_CODE[(740-4)+6] __attribute__((used)) = "systemDB-disabled";

/**
  * system_db_addop
  *
  * @brief Adds a read or write operation to the standby queue.
  *
  * Will draw free elements from the head of queues in the order of
  * free, op, standby.  This function will log the operation that has
  * been added to the queue.
  *
  * @param type: The command type of the element which defines the
  * number of times to execute, addr type, r/w type, and event type
  * @param buffer: uint32* buffer from the callback function.  Stores
  * the data elements for the op (address, number of reads/writes, etc.)
  */
void system_db_addop( uint32 type, uint32 *buffer )
{
}

/**
  * system_db_perform_op
  *
  * @brief performs a read/write operation and logs the outcome.
  *
  * @param op: Operation to perform
  */
void system_db_perform_op( SYSTEMDB_OP_QUEUE_ELEM *op )
{
}

void system_db_smd_cb( unsigned ee, void *data, unsigned len, void *context )
{
}

extern "C" {

void system_db_init()
{
}

/* Called in RPM to do operations at the event locations */
void system_db_statehandler(SYSTEMDB_EVENT_TYPE curr_event)
{
}

/**
  * systemdb_end_of_commands
  *
  * @brief moves the commands in the standby queue to the operation queue
  */
void system_db_end_of_commands( void )
{
}

/**
  * system_db_clearall
  *
  * @brief Clear the op and standby queues
  */
void system_db_clearall() 
{
}

} //for extern "C"
