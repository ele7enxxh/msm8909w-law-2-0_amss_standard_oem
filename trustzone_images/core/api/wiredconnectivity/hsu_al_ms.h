#ifndef _HSU_AL_MS_H_
#define _HSU_AL_MS_H_

/*=============================================================================

                High Speed USB Mass Storage Adaptation Layer

=============================================================================*/
/**
@file hsu_al_ms.h

Defines externalized data and functions for the mass storage adaptation layer.
  
  Externalized functions:
  - hsu_al_ms_get_num_devices
  - hsu_al_ms_task
*/

/*=============================================================================
  Copyright (c) 2006-2010 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

  EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
    $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_al_ms.h#1 $
    
    when     who  what, where, why
    -------- ---  ---------------------------------------------------------
    06/24/10  lo  (Tech Pubs) Edited/added Doxygen comments and markup.
=============================================================================*/

#include "rex.h"

/** @ingroup hsusb_qc_api
    @{
*/

/* Definitions related to the Mass Storage Adaptation Layer Task */

/** HS-USB Mass Storage task name.
*/
#define HSU_AL_MS_TASK_NAME "HSU AL MS TASK"

/** HS-USB Mass Storage task stack size.
*/
#define HSU_AL_MS_STACK_SIZE  (2048 / sizeof(rex_stack_word_type)) 

/** HS-USB Mass Storage task stack pointer.
*/
extern unsigned char          hsu_al_ms_stack[ ];

/** HS-USB Mass Storage task TCB.
*/
extern rex_tcb_type           hsu_al_ms_tcb;

/*=============================================================================
Typedefs
=============================================================================*/

/*=============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/

/*=============================================================================
FUNCTION    hsu_al_ms_get_num_devices
=============================================================================*/
/**
Returns the number of mass storage devices (i.e., the number of mass storage
USB interfaces).

@dependencies
None.

@sideeffects
None.

@return
The number of mass storage devices.
*/
uint8 hsu_al_ms_get_num_devices(void);

/*=============================================================================
FUNCTION    hsu_al_ms_task
=============================================================================*/
/**
The entry point for the mass storage adaptation layer task.

@param parm -- This parameter is not used (generic parameter for task entry
               procedure).

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_ms_task
(
  dword parm /* Not used */
);

/*=============================================================================
FUNCTION    hsu_al_ms_get_num_luns
=============================================================================*/
/**
Counts the total number of LUNs (Logical Unit Number). 

@dependencies
None.

@sideeffects
None.

@return
Number of LUNs.
*/
uint8 hsu_al_ms_get_num_luns(void);

/*=============================================================================
FUNCTION    hsu_al_ms_disable_polling
=============================================================================*/
/**
Disables the polling for media presence.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_ms_disable_polling(void);

/** @} */ /* endgroup hsusb_qc_api */

#endif /* _HSU_AL_MS_H_ */
