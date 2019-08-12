#ifndef DSRDUD_H
#define DSRDUD_H

#include "datamodem_variation.h"
#ifdef FEATURE_DS_RDUD

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               D S R D U D 

GENERAL DESCRIPTION
  This module contains the coding for RDUD (Reduced Dormancy followed by
  Unsolicited Data) module. It runs in DS task context. 
  This file has functions to handle RDUD related commands posted
  from DS module and internal RDUD State Machine  and static variables
  associated with this state machine. 


EXTERNALIZED FUNCTIONS
  These functions are defined in DSRDUD.H and are external to the  RDUD
  module

  ds_rdud_process_cmd()
    Processes RDUD command received by DS task

  FEATURE_DS_RDUD  - Supports RDUD ( Reduced Dormancy followed by Unsolicted
  Data) module in Hybrid mode

 Copyright (c) 2004-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/dsrdud.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/13/11    dvk    Merged support to enable/disable RDUD at runtime.
03/09/09    sa     AU level CMI modifications.
03/01/05    sk	   Removed ds.h and added dstask.h
01/28/04    kvd    Created Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "dstask_v.h"

/*===========================================================================
                        Variable Definitions
===========================================================================*/

#define DS_RDUD_DEFAULT_INACTIVITY_RESTORE_TIMEOUT  20000
#define DS_RDUD_INCOMING_DATA_RLP_TIMEOUT 5000

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION DSRDUD_INIT

DESCRIPTION
  This function performs power-on initialization of RDUD module.
  This includes initialization and registration of ds3g timers.

  This function must be called once at data services task startup.
  This could be called from different task context if RDUD module
  moves to a different task.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void  dsrdud_init
(
  void
);

/*===========================================================================

FUNCTION      DSRDUD_READ_NV

DESCRIPTION   This function is used to read the inactivity 
              restoration time and set the NV item   

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dsrdud_read_nv();

/*===========================================================================

FUNCTION DSRDUD_STATE_IS_ACTIVE

DESCRIPTION
  This function checks if the rdud is in active state. Returns TRUE if in 
  active state else returns false.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: rdud state is active
  FALSE: rdud state is not active
  
SIDE EFFECTS
  None
===========================================================================*/
boolean dsrdud_state_is_active
(
  void
);

/*===========================================================================
FUNCTION DS_PROCESS_RDUD_CMD

DESCRIPTION
  This module process RDUD cmds received by the DS task (possibly by different
  task if RDUD module moves to a different task in future); These cmds are for
  RDUD module to kick-in and force dormancy on 1X to move to EVDO, if needed.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_process_rdud_cmd
(
  ds_cmd_enum_type rdud_cmd
);

/*===========================================================================
FUNCTION      DSRDUD_IS_RDUD_ENABLED

DESCRIPTION   This function indicates if RDUD is enabled or not.

DEPENDENCIES  None.

RETURN VALUE  
  uint32 - CAM timer value in seconds

SIDE EFFECTS  None.
===========================================================================*/
boolean dsrdud_is_rdud_enabled
( 
  void 
);

/*===========================================================================
FUNCTION      DSRDUD_ENABLE_DISABLE_RDUD

DESCRIPTION   This function stores the RDUD enable value in an intermediate 
              variable if RDUD is not in IDLE state. This intermediate value 
              will be copied to "dsrdudi_enabled" when RDUD transitions to 
              IDLE state. If RDUD is not in IDLE state when this function is 
              called, the new RDUD enable value will be copied to 
              "dsrdudi_enabled" immediately.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dsrdud_enable_disable_rdud
( 
  boolean rdud_enable_val
);

#endif /* FEATURE_DS_RDUD */
#endif /* DSRDUD_H */
