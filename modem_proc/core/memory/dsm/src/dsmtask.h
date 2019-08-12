#ifndef DSMTASK_H 
#define DSMTASK_H

/*===========================================================================

                                  D S M T A S K . H

DESCRIPTION
  This file contains types and declarations associated with the DMSS Data
  Service Memory pool and services.

Copyright (c) 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
  $Header: //components/rel/core.mpss/3.5.c12.3/memory/dsm/src/dsmtask.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/07/2013 sko   New DSM feature, FEATURE_DSM_AUGMENT
===========================================================================*/

#define     DSM_TASK_POOL_AUGMENT_REQ 0x1
#define     DSM_TASK_POOL_AUGMENT_CLEANUP_REQ 0x2
#define     DSM_TASK_CLEANUP_TIMER_SIG 0x4

/*===========================================================================
FUNCTION dsm_task_main()

DESCRIPTION
  DSM Task entry point

DEPENDENCIES
  None

PARAMETERS
 

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsm_task_main

(

  /* Unused parameter - required to match rex_task_func_type prototype */

  dword                           ignored    /*lint -esym(725,ignored) */

);
#endif /* DSMTASK_H */
