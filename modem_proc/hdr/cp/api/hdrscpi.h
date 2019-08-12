#ifndef HDRSCPI_H
#define HDRSCPI_H

/*===========================================================================

         S E S S I O N   C O N F I G U R A T I O N   P R O T O C O L
                   I N T E R N A L   D E F I N I T I O N S

DESCRIPTION
  This file contains the declarations for regional functions exported by SCP,
  and any types, macros, or variables used in the different files of SCP.

Copyright (c) 2000 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrscpi.h_v   1.17   14 Mar 2003 11:14:04   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrscpi.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/19/12   cnx     Resolved TCB compiler errors for Dime. 
10/30/09   pba     Added ehrpd support.
06/20/06   pba     Removed redundant AN auth status maintained by SCP.
03/12/03   mpa     Use new session close enum in deactivate cmd type
05/13/02   nmw     Limit attempts to retry configuration before failing session
02/05/02   vas     Changed AN authentication status from boolean to enum. 
12/03/01   vas     Added new command to set HDR AN authentication status.
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
07/10/01   nmn     changed AN Init State Timer from 80ms to ms and name to match
01/02/01   nmn     Initial Revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "hdrscp.h"
#include "bit.h"
#include "dsmbit.h"
#include "hdrhai.h"
#include "hdrmc_v.h"
#include "rex.h"

/* <EJECT> */ 
/*--------------------------------------------------------------------------
                              SCP TIMERS

        AN Initiated State Timer and Configuration Response Timer
--------------------------------------------------------------------------*/

/* AN Init State timer */
extern rex_timer_type hdrscp_an_init_state_timer;
/* Config Response timer */
extern rex_timer_type hdrscp_config_timer;

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Address of the TCB for the task in which this protocol runs */
#define HDRSCP_TASK_PTR hdrmc_tcb_ptr

/* AN Init State Timer - 30 seconds */
#define HDRSCP_AN_INIT_STATE_TIME 0x7530

/* <EJECT> */ 
/*===========================================================================

                                SCP COMMANDS

===========================================================================*/

/* Command type */
typedef enum
{
  HDRSCP_ACTIVATE_CMD,   /* Connection is Active */
  HDRSCP_DEACTIVATE_CMD,  /* Deactivate the Protocol */
  HDRSCP_UPDATE_EHRPD_CREDENTIALS_CMD /* Update eHRPD credentials */
} hdrscp_cmd_name_enum_type;

typedef struct
{
  hdrhai_session_close_reason_enum_type reason;
} hdrscp_deactivate_cmd_type;

typedef union
{
  hdrscp_deactivate_cmd_type deactivate;
} hdrscp_cmd_params_union_type;

typedef struct
{
  hdrscp_cmd_name_enum_type name; /* Name of the command */
  hdrhai_protocol_name_enum_type sender; /* protocol giving command, for debug */
  hdrscp_cmd_params_union_type params; /* Common command fields */
} hdrscp_cmd_type;

/*===========================================================================

                                SCP PROCESS FUNCTIONS

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_RESET_CON_FAIL_RETRY_COUNT

DESCRIPTION
  This helper function resets the Connection Failure Retry Counter

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  hdrscp.con_fail_retry_count value is changed to 0.

===========================================================================*/
extern void hdrscp_reset_con_fail_retry_count (void);
 
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_OPEN_TO_AT_INIT_STATE

DESCRIPTION
  This helper function clears the config_requested flag upon transition from
    Open State to AT Init State

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrscp_open_to_at_init_state (void);
 
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_AN_INIT_TO_OPEN_STATE

DESCRIPTION
  This helper function resets the previous and negotiated data during a
    transition from AN Init State to Open State

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrscp_an_init_to_open_state (void);
 
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_ENTER_OPEN_STATE

DESCRIPTION
  This helper function resets the negotiation status, sets the session
    status, and saves the new session status into NV, upon entering 
    Open state

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrscp_enter_open_state (void);
 
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_CMD

DESCRIPTION
  This function processes commands given to SCP.

DEPENDENCIES
  None

PARAMETERS
  scp_cmd_ptr - Pointer to the command for SCP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrscp_process_cmd
(
  hdrscp_cmd_type * scp_cmd_ptr
);


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_IND

DESCRIPTION
  This function processes indications given to SCP.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for SCP to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrscp_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
);
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_MSG

DESCRIPTION
  This function processes messages for SCP.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrscp_process_msg
( 
  dsm_item_type * item_ptr, 
  hdrhai_protocol_instance_enum_type protocol_instance
);

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRSCP_PROCESS_SIG

DESCRIPTION
  This function processes signals for SCP.
  
DEPENDENCIES
  Only one signal is sent at a time, so only one bit of the sig input 
  parameter is set.  If multiple signals need to be delivered, they must
  be delivered by multiple calls.

PARAMETERS
  sig - The signal that belongs to this protocol and was set.  This is not
        a signal mask.  Only one bit of the value is set
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrscp_process_sig
( 
  rex_sigs_type sig 
);

#endif               /* HDRSCPI_H */
