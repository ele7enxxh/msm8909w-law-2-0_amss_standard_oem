#ifndef RF_TASK_H
#define RF_TASK_H
/*!
  @file
  RF Task header file.

  @defgroup RF_TASK_header
  @ingroup RF_TASK
  @{
*/
/*===========================================================================

                 R F   T A S K    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations associated with RF task module.

REFERENCES
  None

Copyright (c) 2009 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/inc/rf_task.h#1 $
   
when         who     what, where, why
----------   ---     -------------------------------------------------------
01/16/13     aro     Renamed dispatch_handler to dispatch_handle
01/15/13     aro     Updated registration interface to pass dispatch handle
01/09/13     aro     Added query functions to get REX data and UMID
10/05/12     av      Expanded rf task architecture to include more tasks.  
07/06/12     kb      Portrd RCINIT changes from RFA3/main/1.0
05/28/2012   swb     Make SIG for FTM WCDMA enter/exit mode unique
04/27/2012   sar     Modified rf_init_complete_send_ind() and fixed CC Review defects. 
04/25/2012   sar     Added rf init complete indication for CM.
10/27/2011   gh      Added SIG for FTM WCDMA enter/exit complete
01/07/2010   bmg     Made header C++ compatible.
09/22/2010   mkv     Adding Dynamic watchdog support. 
08/26/2009   mkv     Integrating REX MSGR Interworking into RFA2. 
08/26/2009   ans     Removed header file to reduce coupling
07/21/2009   mkv     Added REX MSGR Interworking support. 
06/21/2009   ans     created
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfa_variation.h"
#include "comdef.h"
#include "msgr_types.h"
#include "rf_task_common_services.h"
#include "rf_dispatch.h"

/*===========================================================================
 
                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/
/*!
  @brief
  Featurization for dynamic watchdog support for RF Tasks. 

*/
#ifdef FEATURE_WDOG_DYNAMIC 
  #define FEATURE_RF_TASK_WDOG_DYNAMIC
#endif /*FEATURE_WDOG_DYNAMIC*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

rf_dispatch_handle_type *
rf_task_get_dispatch_handle
(
  void
);

void  rf_task(  dword parm );

/*!
  @brief
  RF Task function.

  @details
  This function can be called by any entity that requires to know whether RF task is running or not.

  @param void
 
*/

boolean
rf_task_dispatch_init
(
  msgr_client_t *msgr_client, 
  msgr_id_t mailbox 
);

/*!
  @brief
  Deinitialize dispatcher for RF Common Control commands processed through RF Task
*/

boolean
rf_task_dispatch_deinit
(
  msgr_client_t *msgr_client
);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef RF_TASK_H */

