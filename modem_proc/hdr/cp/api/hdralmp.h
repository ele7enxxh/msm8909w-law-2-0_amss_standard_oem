#ifndef HDRALMP_H
#define HDRALMP_H

/*===========================================================================

          A I R   L I N K   M A N A G E M E N T   P R O T O C O L
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the Air Link Management Protocol (ALMP).

Copyright (c) 2000 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdralmp.h_v   1.13   05 Mar 2003 14:16:10   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdralmp.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/05/16   vko     FR 33944: Back port FR26160:3G back to LTE as fast as possible
09/14/15   vko     FR 29020: LTE Power Save Mode w/o coordination with network
07/24/15   vko     Skip earfcn for D2L reselection, if reselection failed with forbidden TA
                   reason
05/26/15   vko     DO should wait for D2L Abort response, if deactivate command is received
06/28/13   cnx     Abort D2L if AdviseUnlock rx'ed during D2L resel.
03/22/11   cnx     Set hdralmp.deactivate.deactivation_cmd_is_pending before enter 
                   INACTIVE state to make sure MMoC is informed of deactivation.08/10/09   wsh     Report RX PER data to CM  
08/10/09   wsh     Report RX PER data to CM  
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
06/30/04   dna     Delayed giving mmoc_rpt_prot_auto_deactd_ind() until           
                   HDRIND_LMAC_DEACTIVATE_DONE is received.  This changed     
                   hdralmp_deactivate(), hdralmp_cmd_params_union_type, and 
                   delayed when and where we give HDRIND_ALMP_EXIT_HDR_MODE.  
03/05/03   dna     Moved "connection_is_closing" to hdrcon.c
07/19/02   vas     Added function hdralmp_connection_is_closing()
01/21/02   sh      Added reselect is allowed query function and abort reason
                   to deactivate, reacquire and close connection commands
07/10/01   dna     Added prototype for hdralmp_reset()
05/08/01   dna     Re-naming for consistency.
03/29/01   dna     Added reacquire command to force ALMP to re-acquire
09/07/00   dna     Changed open_connection to have a reason 
02/02/00   dna     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "hdrhai.h"
#include "hdrind.h"
#include "dsm.h"
#include "prot.h"

#ifdef FEATURE_CMI
#include "prot_v.h"
#endif

#if defined(FEATURE_HDR_TO_LTE) || defined(FEATURE_HDR_TO_LTE_REDIRECTION)
#include "lte_rrc_irat_msg.h"
#endif


/* <EJECT> */ 
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRALMP_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRALMP.

DEPENDENCIES
  None
  
PARAMETERS
  msg - Pointer to the DSM item chain holding the message

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

extern void hdralmp_msg_cb( dsm_item_type *msg );

/*===========================================================================

FUNCTION HDRALMP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRALMP.

DEPENDENCIES
  None
 
PARAMETERS
  ind_name - Indication to process, is a global enum for all
                    possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

extern void hdralmp_ind_cb( hdrind_ind_name_enum_type ind_name,
                               void *ind_data );

/*===========================================================================

FUNCTION HDRALMP_ACTIVATE

DESCRIPTION
  Asynchronous command
  This function commands ALMP to activate and enter the initialization state.
  This function must be called when entering HDR mode of operation.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdralmp_activate( hdrhai_protocol_name_enum_type sender );

/*===========================================================================

FUNCTION HDRALMP_DEACTIVATE

DESCRIPTION
  This function commands HDRALMP to deactivate.  Currently there is no
  protocol that would need to call this function, since all events that
  cause the access terminal to leave HDR mode are handled by ALMP.
  This command is intended for use by a higher layer protocol.

DEPENDENCIES
  None

PARAMETERS
  reason - reason for deactivation
  explicit_mmoc_deactivate - Whether MMoC sent the HDRMC_PROT_DEACT_CMD_F
  trans_id - The MMoC Transaction ID of its deactivate command to HDRMC
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdralmp_deactivate
(
  hdrhai_abort_reason_enum_type  reason,
  boolean                        explicit_mmoc_deactivate,
  prot_trans_type                trans_id,
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRALMP_OPEN_CONNECTION

DESCRIPTION
  Asynchronous command
  This function commands ALMP to open a connection.

DEPENDENCIES
  None

PARAMETERS
  open_reason - The reason the connection is being opened
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdralmp_open_connection
( 
  hdrhai_open_reason_enum_type open_reason,
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRALMP_CLOSE_CONNECTION

DESCRIPTION
  Asynchronous command
  This function commands ALMP to close a connection.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdralmp_close_connection
( 
  hdrhai_abort_reason_enum_type reason,
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRALMP_REACQUIRE

DESCRIPTION
  This function commands ALMP to return to initialization state.  If a
  connection is open and in the process of being closed gracefully, ALMP 
  will not transition to initialization state until the connection close is 
  complete.  The AT will then proceed to attempt acquisition on whatever 
  channel is given by system determination.

DEPENDENCIES
  None

PARAMETERS
  reason - reason to reacquire
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdralmp_reacquire
(
  hdrhai_abort_reason_enum_type reason,
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRALMP_RESET

DESCRIPTION
  This function commands ALMP to reset itself.  When the reset is complete,
  ALMP is in idle state, no access attempt is in progress and no connection
  setup is being attempted.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdralmp_reset( hdrhai_protocol_name_enum_type sender );

/*===========================================================================

FUNCTION HDRALMP_CONNECTION_IS_OPEN

DESCRIPTION
  Boolean access function, can be called from any task to check if a 
  connection is open.  ALMP thinks a connection is open as long as it
  is in the connected state, which does not correspond directly with 
  when the RTCMAC is deactivated.

DEPENDENCIES
  None

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE if a connection is open, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern boolean hdralmp_connection_is_open
( 
  hdrhai_protocol_name_enum_type caller 
);

/*===========================================================================

FUNCTION HDRALMP_SYSTEM_IS_ACQUIRED

DESCRIPTION
  Boolean access function, can be called from any task to check if a 
  system is acquired.  ALMP thinks a system is acquired as long as it
  is in the idle or connected states.
  
DEPENDENCIES
  None

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE if a system is acquired, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern boolean hdralmp_system_is_acquired
( 
  hdrhai_protocol_name_enum_type caller 
);

/*===========================================================================

FUNCTION HDRALMP_RESELECT_IS_ALLOWED

DESCRIPTION
  Boolean access function, can be called from any task to check if system 
  reselection is allowed
  
DEPENDENCIES
  None

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE if a system is acquired, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern boolean hdralmp_reselect_is_allowed
( 
  hdrhai_protocol_name_enum_type caller 
);

/*===========================================================================

FUNCTION HDRALMP_POWERUP_INIT

DESCRIPTION
  This function initializes the ALMP protocol.
  
DEPENDENCIES
  This function must be called before ALMP protocol is used

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdralmp_powerup_init( void );

/*===========================================================================
FUNCTION HDRALMP_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the ALMP protocol
    
DEPENDENCIES
  None

PARAMETERS
  timer_id - Timer ID of the expired timer

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdralmp_process_timer
(
  uint32 timer_id
);

/*===========================================================================

FUNCTION HDRALMP_SET_DEACTIVATE_NOTIFIER

DESCRIPTION
  This function sets the flags to inform MMoC that HDR is deactivated.
  
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdralmp_set_deactivate_notifier( void );

#ifdef FEATURE_HDR_TO_LTE
/*===========================================================================

FUNCTION HDRALMP_SEND_RESELECT_ABORT_REQ_TO_LTE

DESCRIPTION
 This function sends a request to LTE RRC to abort reselection.
 
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdralmp_send_reselect_abort_req_to_lte( void );

/*===========================================================================

FUNCTION HDRALMP_SEND_RESELECT_REQ_TO_LTE

DESCRIPTION
 This function sends a request to LTE RRC to reselect to specified LTE
 freq and cell ID.
  
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_send_reselect_req_to_lte( void );


/*===========================================================================

FUNCTION hdralmp_hdr_to_lte_resel_abort_cmd_is_sent

DESCRIPTION
 This function returns the abort reselection cmd sent flag value
 
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  TRUE - if abort reselection command is sent to LTE
  FALSE - if the command is not sent/abort response recieved

SIDE EFFECTS
  None

===========================================================================*/
boolean hdralmp_hdr_to_lte_resel_abort_cmd_is_sent( void );

/*===========================================================================

FUNCTION hdralmp_clr_hdr_to_lte_resel_abort_cmd_is_sent

DESCRIPTION
 This function clears the abort reselection cmd sent flag value
 
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_clr_hdr_to_lte_resel_abort_cmd_is_sent( void );

#endif /* FEATURE_HDR_TO_LTE */

#ifdef FEATURE_LTE_TO_HDR
/*===========================================================================

FUNCTION HDRALMP_START_GET_CGI_TIMER

DESCRIPTION
  This function stops the LTE to HDR get CGI timer if it is not 
  active

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_start_get_cgi_timer ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_STOP_GET_CGI_TIMER

DESCRIPTION
  This function stops the LTE to HDR get CGI timer

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_stop_get_cgi_timer(void);
#endif /* FEATURE_LTE_TO_HDR */

#ifdef FEATURE_HDR_TO_LTE
/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_HDR_TO_LTE_RESEL_IS_IN_PROGRESS

DESCRIPTION
  This function returns TRUE or FALSE based on whether HDR to LTE reselection
  is in progress or not.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if D2L reselection is in progress, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdralmp_hdr_to_lte_resel_is_in_progress( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_HDR_TO_LTE_RESEL_CMD_IS_SENT

DESCRIPTION
  This function returns TRUE or FALSE based on whether HDR to LTE reselection
  command is sent to LTE or not.
 
  HDRCP moves to SUSPEND state and deactivates HDRSRCH once D2L reselection
  starts. Once HDRSRCH is deactivated, HDRCP sends D2L reselection command
  to LTE. This functions can tell whether HDRCP is still waiting HDRSRCH to
  be deactivated.

DEPENDENCIES
  Need to call hdralmp_hdr_to_lte_resel_is_in_progress() first to ensure that
  D2L is in progress.

PARAMETERS
  None

RETURN VALUE
  TRUE if D2L reselection command is sent to LTE, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdralmp_hdr_to_lte_resel_cmd_is_sent( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_SET_PENDING_ABORT_RESEL

DESCRIPTION
  This function sets the value of pending_abort_resel flag.

DEPENDENCIES
  DO to LTE reselection should be already in progress. HDR SRCH has been asked
  to be deactivated but reselection command is not sent to LTE yet.

PARAMETERS
  Boolean value of disired pending_abort_resel flag.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_set_pending_abort_resel
(
  boolean value
);

/*===========================================================================

FUNCTION HDRALMP_GET_RESELECT_EARFCN

DESCRIPTION
 This function gets the current earfcn on which D2L reseleciton is attempted.
  
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  earfcn - on which D2L is in progress

SIDE EFFECTS
  None

===========================================================================*/
lte_earfcn_t hdralmp_get_reselect_earfcn( void );

#endif /* FEATURE_HDR_TO_LTE */

/*===========================================================================

FUNCTION HDRALMP_PROCESS_PSM_REQ

DESCRIPTION
  This function process the PSM READY REQUEST from CM.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdralmp_process_psm_req(void);

/*===========================================================================

FUNCTION hdralmp_send_psm_ready_ind_to_cm

DESCRIPTION
  This function sends PSM ready indication to CM.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdralmp_send_psm_ready_ind_to_cm( void );

#ifdef FEATURE_HDR_TO_LTE_REDIRECTION 
/*===========================================================================

FUNCTION HDRALMP_CLEAR_HDR_TO_LTE_REDIR

DESCRIPTION
  This function clears the info related to hdr to lte redirection.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_clear_hdr_to_lte_redir( void );

/*===========================================================================

FUNCTION HDRALMP_UPDATE_EARFCN_PLMNID_INFO

DESCRIPTION
  This function receives the earfcn and plmn info.

DEPENDENCIES
  None

PARAMETERS
  lte_rrc_acq_db_earfcn_rsp_s containing the return value of earfcn and plmn 
  info

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_update_earfcn_plmnid_info
(
   lte_rrc_acq_db_earfcn_rsp_s *lte_rrc_acq_db_earfcn_rsp_s
);

/*===========================================================================

FUNCTION HDRALMP_HDR_TO_LTE_REDIR_IS_IN_PROGRESS

DESCRIPTION
  This function returns TRUE or FALSE based on whether HDR to LTE redirection
  is in progress or not.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if D2L redirection is in progress, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdralmp_hdr_to_lte_redir_is_in_progress( void );

/*===========================================================================

FUNCTION HDRALMP_HDR_TO_LTE_REDIR_CMD_IS_SENT

DESCRIPTION
  This function returns TRUE or FALSE based on whether HDR to LTE redirection
  command is sent to LTE or not.
 
  HDRCP moves to SUSPEND state and deactivates HDRSRCH once D2L redirection
  starts. Once HDRSRCH is deactivated, HDRCP sends D2L redirection command
  to LTE. This functions can tell whether HDRCP is still waiting HDRSRCH to
  be deactivated.

DEPENDENCIES
  Need to call hdralmp_hdr_to_lte_redir_is_in_progress() first to ensure that
  D2L is in progress.

PARAMETERS
  None

RETURN VALUE
  TRUE if D2L redirection command is sent to LTE, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdralmp_hdr_to_lte_redir_cmd_is_sent( void );

/*===========================================================================

FUNCTION HDRALMP_SET_PENDING_ABORT_REDIR

DESCRIPTION
  This function sets the value of pending_abort_redir flag.

DEPENDENCIES
  DO to LTE redirection should be already in progress. HDR SRCH has been asked
  to be deactivated but reselection command is not sent to LTE yet.

PARAMETERS
  Boolean value of disired pending_abort_redir flag.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_set_pending_abort_redir
(
  boolean value
);


/*=============================================================================

FUNCTION HDRALMP_CHECK_OK_TO_REDIR_TO_LTE

DESCRIPTION
  This function checks if it is ok to redirect to LTE. 

DEPENDENCIES
  None.

PARAMETERS
  d2l_rat_pri_list - the RAT priority list to be given in redirection command
                           if D2L redirection is allowed.
  supported_band_index - LTE supported index, for retrieving band info 
RETURN VALUE
  TRUE is ok to redirect to LTE. Otherwise FALSE.

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdralmp_check_ok_to_redir_to_lte
(
  sys_rat_pri_list_info_s_type *d2l_rat_pri_list,
  uint16 *supported_band_index
);

/*===========================================================================

FUNCTION HDRALMP_SEND_REDIRECT_ABORT_REQ_TO_LTE

DESCRIPTION
 This function sends a request to LTE RRC to abort redirection.
 
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  hdralmp_send_redirect_abort_req_to_lte(void);

/*===========================================================================

FUNCTION HDRALMP_CLR_HDR_TO_LTE_REDIR_ABORT_CMD_IS_SENT

DESCRIPTION
 This function clears the abort redirection cmd sent flag value
 
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_clr_hdr_to_lte_redir_abort_cmd_is_sent( void );

/*===========================================================================

FUNCTION HDRALMP_SEND_MEASUREMENT_REQ

DESCRIPTION
  This function sends measurement request to SRCH with list of EARFCN, for
  UE initiated redirection.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None


===========================================================================*/

void hdralmp_send_measurement_req( void );

/*===========================================================================

FUNCTION HDRALMP_CHECK_AN_INITIATED_REDIR_ALLOWED

DESCRIPTION
  This function checks if AN initiated D2L redirection is allowed or not by UE.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  TRUE - if supported
  FALSE - if not supported

SIDE EFFECTS
  None


===========================================================================*/

boolean hdralmp_check_an_initiated_redir_allowed( void );

/*=============================================================================

FUNCTION HDRALMP_SET_HDR_TO_LTE_REDIR_MEAS_IN_PROGRESS

DESCRIPTION
  This function sets if LTE measurements for redirection are in progress (or) not

DEPENDENCIES
  None.

PARAMETERS
  value - value to be set TRUE (or) FALSE

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdralmp_set_hdr_to_lte_redir_meas_in_progress
(
   boolean value
);

/*=============================================================================

FUNCTION HDRALMP_HDR_TO_LTE_REDIR_MEAS_IN_PROGRESS

DESCRIPTION
  This function checks if LTE measurements shall be triggered (or) in progress

DEPENDENCIES
  None.

PARAMETERS
None.

RETURN VALUE
  TRUE is LTE meas started. Otherwise FALSE.

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdralmp_hdr_to_lte_redir_meas_in_progress(void);

/*===========================================================================

FUNCTION hdralmp_send_acq_db_earfcn_req_to_lte

DESCRIPTION
 This function sends a request to LTE RRC to redirect to list of LTE
 freq and cell ID.
  
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdralmp_send_acq_db_earfcn_req_to_lte(void);

#endif /* FEATURE_HDR_TO_LTE_REDIRECTION */


#endif               /* HDRALMP_H */
