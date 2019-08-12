/*============================================================================
  FILE:         system_db_rpm.h
 
  OVERVIEW:     Define functions used on the RPM for systemdb.
 
  DEPENDENCIES: None
 
                Copyright (c) 2012 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

$Header: //components/rel/rpm.bf/2.1.1/core/power/system_db/inc/system_db_rpm.h#1 $
$Date: 2015/02/25 $
============================================================================*/
#ifndef SYSTEMDBRPM_H
#define SYSTEMDBRPM_H
#include "system_db.h"
#include "DALStdDef.h"

//included in C files
#ifdef __cplusplus
extern "C" {
#endif

/**
  * system_db_init
  *
  * @brief initialize systemdb structures
  *
  * Allocates and initializes elements for the free queue and registers callback
  */
void system_db_init( void );

/**
  * system_db_statehandler
  *
  * @brief: Performs ops that match the current event
  *
  * Checks to see if the operation's event type matches the current event.  If
  * it does, then it will perform the op, and if the times to execute timer is
  * non-zero, decrement the times to execute counter.  If the operation has
  * been performed the number of times it's been specified, remove the
  * operation from the op queue.
  *
  * @param curr_event: Power state that the RPM is transitioning to.
  */
void system_db_statehandler( SYSTEMDB_EVENT_TYPE event );

/**
  * systemdb_end_of_commands
  *
  * @brief moves the commands in the standby queue to the operation queue
  */
void system_db_end_of_commands( void );

/**
  * system_db_clearall
  *
  * @brief Clear the op and standby queues
  */
void system_db_clearall( void );

#ifdef __cplusplus
}
#endif

#endif
