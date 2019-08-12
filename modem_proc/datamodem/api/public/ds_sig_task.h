#ifndef DS_SIG_TASK_H
#define DS_SIG_TASK_H

/*==========================================================================*/ 
/*! 
  @file 
  ds_sig_task.h

  @brief
  This is the header file for DS_SIG task. Notifications to the sockets and 
  network clients is provided asynchronously in DS_SIG task.

  @details
  This file is not used by the clients directly. The main task loop is only
  exported to the task main controller (TMC). 

  @exported_functions
  ds_sig_task() - DS_SIG task entry point and processing loop.

  @notes
  The only user of this file is the AMSS task framework. Clients should
  not include this file. 

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ds_sig_task.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who  what, where, why
  ---------- ---  ------------------------------------------------------------
  2009-08-17 hm   Created module.

===========================================================================*/
#include "comdef.h"
#include "rex.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
  DOG data structures. 
---------------------------------------------------------------------------*/
extern uint32           dog_ds_sig_rpt_var;          /*! Dog report ID     */
extern uint32           dog_ds_sig_rpt_time_var;     /*! Report Period     */
extern rex_timer_type   ds_sig_dog_rpt_timer;        /*! Watchdog Timer    */

/*!
  Task control block for DS_SIG task 
*/
extern rex_tcb_type  ds_sig_tcb;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*=========================================================================*/
/*!
  @function 
  ds_sig_task

  @brief
  This function is the entry point and main processing loop for the Data
  Service(s) DS_SIG task.

  @details
  This function can only called by the Task main controller to start the 
  DS_SIG task processing loop. 

  @param[in]  dummy - Ignored.

  @dependencies
  None.

  @return
  Does not Return.
*/
/*=========================================================================*/
void ds_sig_task
(
  uint32 dummy
);



#endif /* DS_SIG_TASK_H */


