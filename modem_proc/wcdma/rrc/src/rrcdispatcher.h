#ifndef RRCDISPATCHER_H
#define RRCDISPATCHER_H
/*===========================================================================
                        R R C  D I S P A T C H E R  H E A D E R

DESCRIPTION

  This module is the header file for the RRC Dispatcher module.
	
Copyright (c) 2000, 2002-2003, 2007-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcdispatcher.h_v   1.2   13 May 2002 17:16:30   krishnan  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcdispatcher.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/15   vi      Changes for force DRX and skip idlemode measurements based on QMI request 
03/25/13   sn      Changes to replace timer based dog reporting with signal based reporting
06/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.99.50
06/02/11   ad      Added MMAL changes to return the trch type for the OTA
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/19/11   ad      Changes to add callback function for MMAL support
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
08/18/10   ss      Added SIB19 logging code.
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
05/14/09   sks     Made changes to suppress compilation errors for RRC UTF.
05/08/09   ss      Updated Copyright Information
05/13/02   ram     Fixed incorrect prototype for rrc_event_wait()
05/10/02   bu      Added prototype for rrc_event_wait().
10/31/00   ram     Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "wcdma_variation.h"
#include "rrcsibproci.h"
#include "rrccmd.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */



/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */



/* =======================================================================
**                          Macro Definitions
** ======================================================================= */





/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================

FUNCTION  RRC_DISPATCHER

DESCRIPTION
 
  The RRC_DISPATCHER is the central wait-loop for the RRC task. This
  function constantly waits for events to be received. When events are
  received, they are dispatched to the appropriate function to be processed.
  
  This function is the only wait-loop in the RRC task. No other procedures
  can perform a local wait loop. 
 
  The set of events that the RRC task waits for is determined by RRC's
  event dispatch table. This table must be created before rrc_dispatcher is
  called. Regardless of the current state of RRC, the set of events that
  the rrc_dispatcher is waiting for is the same.
 
DEPENDENCIES

  Event dispatch table must be created before this function is called.  
   
RETURN VALUE

  None. Note that this function does not return unless the RRC task is being
  shut down. (*** This is TBD ***).
  
SIDE EFFECTS

  None.

===========================================================================*/

extern void rrc_dispatcher ( void );

/*===========================================================================
FUNCTION RRC_EVENT_SIG

DESCRIPTION
  Wait on the specified signal.  Handle signals as they come in. Only time
  critical watchdog and other signals are acknowledged.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECT
  Some signals are ignored till we're done with the signal

===========================================================================*/
void rrc_event_wait( rex_sigs_type sig);

/*===========================================================================

FUNCTION  RRC_DOG_REPORT_AND_CLEAR_SIG

DESCRIPTION
 
  This function reports to dog and clears sig.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/
void rrc_dog_report_and_clear_sig(void);

/*===========================================================================

FUNCTION  RRC_DISPATCH_INTERNAL_COMMANDS

DESCRIPTION
 
  This function dispatches all commands received in RRC's internal
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/

void rrc_dispatch_internal_commands( void );

/*===========================================================================

FUNCTION  RRC_DISPATCH_L1_COMMANDS

DESCRIPTION
 
  This function dispatches all commands received in RRC's l1
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/

void rrc_dispatch_l1_commands( void );

/*===========================================================================

FUNCTION  RRC_DISPATCH_MAC_COMMANDS

DESCRIPTION
 
  This function dispatches all commands received in RRC's mac
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/

void rrc_dispatch_mac_commands( void );

/*===========================================================================

FUNCTION  RRC_DISPATCH_RLC_COMMANDS

DESCRIPTION
 
  This function dispatches all commands received in RRC's rlc
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/

void rrc_dispatch_rlc_commands( void );

/*===========================================================================

FUNCTION  RRC_DISPATCH_MM_COMMANDS

DESCRIPTION
 
  This function dispatches all commands received in RRC's mm
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/

void rrc_dispatch_mm_commands( void );

/*===========================================================================

FUNCTION  RRC_DISPATCH_RR_COMMANDS

DESCRIPTION
 
  This function dispatches all commands received in RRC's RR
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/
void rrc_dispatch_rr_commands( void );


/*===========================================================================

FUNCTION  RRC_DISPATCH_QMI_COMMANDS

DESCRIPTION
 
  This function dispatches all commands received in RRC's QMI
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/
void rrc_dispatch_qmi_commands();

/*===========================================================================

FUNCTION    rrc_log_dl_sig_message_for_ext_sibs

DESCRIPTION
    This function will log the downlink encoded OTA messages into QXDM for extension sibS.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrc_log_dl_sig_message_for_ext_sibs
(
  rrc_SIB_type sib_type,
  sib_events_index_e_type index
);

#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
/*===========================================================================

FUNCTION  RRC_DISPATCH_LTE_RRC_COMMANDS

DESCRIPTION
 
  This function dispatches all commands received in RRC's LTE RRC
  command queue. This function dequeues each command in the command queue
  until the command queue is empty. For each command, it looks at
  the command id and passes the command to the appropriate procedure
  for processing. The command is not freed by this function. Freeing the
  command is the responsibility of the procedure that processes the
  command.
   
DEPENDENCIES

  None.
   
RETURN VALUE

  None. 
  
SIDE EFFECTS

  None.

===========================================================================*/
void rrc_dispatch_lte_rrc_commands( void );
#endif
/*===========================================================================

FUNCTION    rrc_get_dl_asn1_message_type

DESCRIPTION
  This function determines the type of the message received and returns this
  information as a byte suitable for reporting in a QXDM event.  This value
  is *not* the same as the ASN.1 based message type values returned by other
  similar functions.  See the rrclogging.h file for details.
  
DEPENDENCIES
  None

RETURN VALUE
  uint8 : Returns the message type if successful. Otherwise it returns 0xff.
  
SIDE EFFECTS
  None

===========================================================================*/
 uint8 rrc_get_dl_asn1_message_type(
  rrc_cmd_type              *cmd_ptr /* To hold received command */
);
#endif /* RRCDISPATCHER_H */

