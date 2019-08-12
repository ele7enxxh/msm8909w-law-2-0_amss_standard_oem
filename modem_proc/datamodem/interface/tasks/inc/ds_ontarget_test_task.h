#ifndef DS_ONTARGET_TEST_TASK_H
#define DS_ONTARGET_TEST_TASK_H

/*==========================================================================*/ 
/*! 
  @file 
  ds_ontarget_test_task.h

  @brief
  This is the header file for DS_ONTARGET_TEST task. Notifications to the sockets and 
  network clients is provided asynchronously in DS_ONTARGET_TEST task.

  @details
  This file is not used by the clients directly. The main task loop is only
  exported to the task main controller (TMC). 

  @exported_functions
  ds_ontarget_test_task() - DS_ONTARGET_TEST task entry point and processing loop.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/inc/ds_ontarget_test_task.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who  what, where, why
  ---------- ---  ------------------------------------------------------------
  2012-09-02 aa   Created module

===========================================================================*/
#include "comdef.h"
#include "rex.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*!
  Task control block for DS_ONTARGET_TEST task 
*/
extern rex_tcb_type  ds_ontarget_test_tcb;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*=========================================================================*/
/*!
  @function 
  ds_ontarget_test_task

  @brief
  This function is the entry point and main processing loop for the Data
  Service(s) DS_ONTARGET_TEST task.

  @details
  This function can only called by the Task main controller to start the 
  DS_ONTARGET_TEST task processing loop. 

  @param[in]  dummy - Ignored.

  @dependencies
  None.

  @return
  Does not Return.
*/
/*=========================================================================*/
void ds_ontarget_test_task
(
  uint32 dummy
);


#ifdef __cplusplus
}
#endif



#endif /* DS_ONTARGET_TEST_TASK_H */


