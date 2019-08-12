#ifndef HDRAMPI_H
#define HDRAMPI_H

/*===========================================================================

            A D D R E S S   M A N A G E M E N T   P R O T O C O L
                    I N T E R N A L   D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for regional functions exported by AMP,
  and any types, macros, or variables used in the different files of AMP.

Copyright (c) 2000 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrampi.h_v   1.12   06 Mar 2003 09:15:48   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrampi.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/11/12   cnx     Handle IRMI when AT is doing RestorePriorSession.
11/08/11   cnx     Support Optimized Handoff phase 3.
06/10/10   pba     IRAT Measurement feature.
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/24/09   wsh     CMI: updated included headers due to header split
12/23/08   pba     Support for Quick Idle Protocol
04/17/08   wsh     DCM optimization: not closing session unnecessarily when
                   moving across subnet
04/03/08   wsh     Merged fix for waiting UATIAssign onto 2H07
05/29/07   mpa     Added update NAI comand.
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
08/26/04   mpa     Added Update ESN Command.
03/05/03   mpa     Changed timers to use REX callbacks instead of signals
03/27/02   mpa     Removed hdramp_dual_addr_timer
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
05/08/01   nmn     Beautification
05/02/01   nmn     removed NV data structure
12/11/00   nmn     Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "hdramp.h"
#include "bit.h"
#include "dsmbit.h"

#include "hdraddr.h"


/* <EJECT> */ 
/*===========================================================================

                                AMP COMMANDS

===========================================================================*/

/* Command type */
typedef enum
{
  HDRAMP_ACTIVATE_CMD    = 0,     /* Connection is Active */
  HDRAMP_DEACTIVATE_CMD  = 1,     /* Deactivate the Protocol */
  HDRAMP_UPDATE_UATI_CMD = 2,     /* Update UATI Command */
  HDRAMP_UPDATE_ESN_CMD  = 3,     /* Update ESN Command */
  HDRAMP_UPDATE_NAI_CMD  = 4      /* Update NAI Command */
#ifdef FEATURE_LTE_TO_HDR_OH
  , HDRAMP_SEND_IRMI_CMD = 5      /* Send IRMI command */
#endif /* FEATURE_LTE_TO_HDR_OH */

} hdramp_cmd_name_enum_type;

typedef struct
{
  hdramp_cmd_name_enum_type name; /* Name of the command */
  hdrhai_protocol_name_enum_type sender; /* protocol giving command, 
                                            for debug */
//  hdramp_cmd_params_union_type params; /* Common command fields */
} hdramp_cmd_type;

typedef enum
{
  HDRAMP_N_ADDR_TIMER,          /* n address timer*/
  HDRAMP_UATI_ASSIGNMENT_TIMER  /* timer for receiving uati assignment msg */

#ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
  /* Sanity timer to hold off ACMAC while waiting for UATIAssignment */
  , HDRAMP_UATI_ASSIGNMENT_SLP_HOLDOFF_TIMER  
#endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */

#ifdef FEATURE_LTE_TO_HDR_OH
  , HDRAMP_IRMA_TIMER           /* Timer for receiving IRMA */
#endif /* FEATURE_LTE_TO_HDR_OH */

} hdramp_timer_name_enum_type;

/* <EJECT> */ 
/*===========================================================================

                             AMP PROCESS FUNCTIONS

===========================================================================*/


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_PROCESS_CMD

DESCRIPTION
  This function processes commands given to AMP.

DEPENDENCIES
  None

PARAMETERS
  amp_cmd_ptr - Pointer to the command for AMP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdramp_process_cmd
(
  hdramp_cmd_type * amp_cmd_ptr
);


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_PROCESS_IND

DESCRIPTION
  This function processes indications given to AMP.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for AMP to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdramp_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_PROCESS_MSG

DESCRIPTION
  This function processes messages for AMP.

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
extern void hdramp_process_msg
( 
  dsm_item_type * item_ptr, 
  hdrhai_protocol_instance_enum_type protocol_instance
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_PROCESS_TIMER

DESCRIPTION
  Callback function that gets called when a timer for hdramp expires

DEPENDENCIES
  None

PARAMETERS
  timer_id - Timer ID of the expired timer

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdramp_process_timer
( 
  uint32 timer_id
);

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRAMP_TIMER_CB

DESCRIPTION
  This function is the callback for all amp state protocol timers.
  This function is called by REX when the timer expires, and runs
  in the context of the task that defined the timer (HDRMC), however
  it runs as an independent thread and thus must queue an event for HDRMC.

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern void hdramp_timer_cb
(
  uint32 timer_id
);

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRAMP_STOP_UATI_ASSIGNMENT_TIMER

DESCRIPTION
  This function stops the uati assignment timer if it was running
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
extern void hdramp_stop_uati_assignment_timer( void );

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_RESET_DATA

DESCRIPTION
  This helper function resets all address data and local data/flags

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdramp_reset_data(void);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_ENTER_SETUP

DESCRIPTION
  This function sets data upon entering setup state

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Changes state.

===========================================================================*/
extern void hdramp_enter_setup ( void );

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_SEND_UATI_REQ_MSG

DESCRIPTION
  This helper function prepares and sends a UATI Request Message.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdramp_send_uati_req_msg ( void );

/*=============================================================================
FUNCTION HDRAMP_UPDATE_ESN

DESCRIPTION
  This function sends an ESN update command.
  
DEPENDENCIES
  None.

PARAMETERS
  sender - Protocol sending the command

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
extern void hdramp_update_esn
(
  hdrhai_protocol_name_enum_type sender
);

/*=============================================================================
FUNCTION HDRAMP_UPDATE_NAI

DESCRIPTION
  This function sends an NAI update command.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
extern void hdramp_update_nai
(
  hdrhai_protocol_name_enum_type sender
);

#if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
/*===========================================================================

FUNCTION HDRAMP_GET_DCM_REACQ_DIFF_SUBNET

DESCRIPTION
  Check if AT has reacq-ed during DCM mode

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - AT has reacq-ed during DCM
  FALSE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdramp_get_dcm_reacq_diff_subnet( void );

#endif /* FEATURE_DEDICATED_C2K_MODE ||| FEATURE_IRAT_PILOT_MEAS */

/*===========================================================================

FUNCTION HDRAMP_UATI_MATCH_SUBNET

DESCRIPTION
  Check if the current UATI address matches the subnet from OVHD MSG
  This function should only be called when OVHD INFO is current.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - UATI is of the same subnet of the serving AN
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdramp_uati_match_subnet( void );
#endif               /* HDRAMPI_H */
