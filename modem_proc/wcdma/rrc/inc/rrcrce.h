#ifndef RRCRCE_H
#define RRCRCE_H
/*===========================================================================
              R R C R C E  H E A D E R  F I L E

DESCRIPTION
 The RRCRCE module consists of RRC CONNECTION ESTABLISHMENT PROCEDURE 
 implementation. The 3GPP TS 25.331 RRC Protocol specification is used as
 basis for implementation.
  
Copyright (c) 2001-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcrce.h_v   1.4   18 Sep 2001 17:35:06   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/inc/rrcrce.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/16   sp      Made changes to add call end metrics - FR#30856
05/09/16   sp      Made changes for Mav debug snapshot
09/11/15   sr      Made changes for FR29345 EFS/NV Settings to be Read per Sub
06/27/14   sn      Changes to prioritize MT call over reselection 
11/16/11   ad      Changed the macro for FEATURE_WCDMA_PAUSE_MTPL_BACKOFF_VOICE_CALL
10/31/11   sks     Added support for FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB.
07/29/11   ad      Added code for FEATURE_WCDMA_PAUSE_MTPL_BACKOFF_VOICE_CALL feature
03/29/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/28/11   sks     Made changes to trim RRC connection request msg if its size exceeds
                   allowed TM SDU size.
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
07/20/09   rm      Modified prototype for rrcrce_force_initialize
05/12/09   ss      Added declaration of function rrcrce_clear_num_retries_count()to fix warning
05/08/09   ss      Updated Copyright Information
23/10/08   gkg     Added prototype of functions rrcrce_send_conn_req_cause_event()
                   or rrcrce_send_conn_rel_cause_event()
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
05/16/08   pk      Added support for Rove-in to GAN. Changes are under the feature 
                   flag FEATURE_GAN.
04/03/08   gkg     Added signature for new function rrcrce_get_rbs_ptr() under 
                   feature flag FEATURE SRB5 SUPPORT.
11/23/07   pk      Added prototype for a new function rrcrce_check_for_rai_mismatch()
                   under the feature flag FEATURE_ABORT_ESTB_ON_RA_LA_MISMATCH.
03/22/07   kp      Added proto type for function rrcrce_get_rce_sub_state().
                   This function will return current rce sub state.  
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
06/05/06   sgk     Made changes to include rel5 non HSDPA functionality under 
                   FEATURE REL5 while retaining the HSDPA specifc processing 
                   under FEATURE HSDPA.
01/23/06   vm      BPLMN Optimization for Automatically re-starting Manual 
                   BPLMN search after completion of LAU/RAU updates & added
                   support for not rejecting Manual PLMN search request when RRC state 
                   is Connecting/FACH/DCH and the establishment cause is Registration. 
11/08/05   vm      Added support for WTOW and WTOG Background PLMN Search. 
                   Added prototype for new function rrcrce_is_rce_procedure_waiting_for_bplmn()
                   which is called by CSP before proceeding with WTOW/WTOG BPLMN search.
10/03/05   tkk     Added prototype "rrcrce_get_sib16_ptr" for getting default 
                   config info.
09/28/05   vm      Added support for redirection to another frequency + psc via
                   RRC Connection Setup message. Added new structure 
                   dl_ccch_msg_type.
04/05/05   da      Prototype for rrcrce_get_nv_items()
07/13/04   kc      Include header file to get defn. of rrcrce_status_e_type
07/07/04   kc      Defn. for API to peek into UE-Id before asn1 decoding of 
                   conn-setup message
05/10/04   vn      Prototype for rrcrce_force_initialize().
03/22/04   bu      Prototype for rrcrce_get_num_retries_count().
02/12/04   vk      The fn rrcrce_change_substate_to_rrcrce_initial() now gets
                   a bool indicating if abort ind needs to be send to MM
05/21/03   rj      Added prototype for an API to change the substate of RCE
                   to RRCRCE_INITIAL for Mode Change Controller
02/27/03   rj      Added prototype for an API to change the substate of RCE
                   procedure to RRCRCE_FINAL during G2W handover.
11/22/02   rj      Added a prototype for a function to know whether
                   RRC Connection Establishment procedure is active or
                   not.
09/18/01   rj      Added a prototype for the function that returns 
                   RRC Connection status.
02/08/01   rj      Updated comments to function rrcrce_init_procedure.
01/26/01   rj      Correct date in the edit hostory.
01/19/01   rj      Moved all declarations that are internal to RRCRCE module
                   to rrcrcei.h. Removed comments as per code review inputs.
                   Renamed rrcrce_init_rce_procedure function to  
                   rrcrce_init_procedure. Renamed rrc_rce_procedure function
                   to rrcrce_procedure_event_handler. Removed unwanted 
                   include files.
11/17/00   rj      Created file.

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/

#include "wcdma_variation.h"
#include "rrccmd_v.h"
#include "rrcrcei.h"
/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/

/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB

typedef enum
{
  CSFB_INVALID, /* RRC Connection is not established */
  CSFB_STARTED,
  CSFB_CS_RB_RELEASED /* Set when CS call released for a CSFB Connection */
}rrcrce_csfb_status_e_type;

#endif

typedef enum
{
  RRCRCE_RRC_CONN_NOT_ESTABLISHED, /* RRC Connection is not established */

  RRCRCE_RRC_CONN_ESTABLISHED      /* RRC Connection is established */
}rrcrce_rrc_conn_status_e_type;

/* Used to store the RRC Connection Setup message when UE receives
 * redirection information with freq+psc+crnti information and UTRAN
 * expects UE to directly enter Connected mode if destination state
 * is CELL_FACH
 */
typedef struct
{
  uint32 dl_sdu_length;   /* Length of downlink sdu in bytes */
  uint8 *dl_sdu;          /* Pointer to the Downlink SDU. */
  rrc_DL_CCCH_Message *decoded_msg; /* Pointer to the decoded msg */
  uint32 dl_sdu_type;     /* type of dl sdu */
}dl_ccch_msg_type;


/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION rrcrce_init_procedure

DESCRIPTION
  This function will initialize the RRC Connection Establishment procedure
  substate to RRCRCE_INITIAL Substate. This will called in the RRCTASK 
  modules along with other Init Procedures.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrcrce_init_procedure
(    
  void  
);
/*===========================================================================

FUNCTION rrcrce_procedure_event_handler

DESCRIPTION
  This is the entry point for RRC Connection Establishment procedure. All
  events for RCE come to this function from RRC Dispatcher. Depending on the
  UE state of RRC Connection Procedure, it calls the corresponding state 
  handler. These state handlers call substate handlers depending on the 
  the current substate of RRC Connection Establishment procedure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);


/*===========================================================================

FUNCTION rrcrce_get_nv_items

DESCRIPTION
  This  function checks NV item NV_GPRS_ANITE_GCF_I and stores it in local variable
  used to determine if 40s timer needs to be started before sending signalling
  connection release.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcrce_get_nv_items
(
  sys_modem_as_id_e_type wrrc_as_id
);

/*===========================================================================
FUNCTION rrcrce_is_abort_in_progress

DESCRIPTION
  This function returns if abort is in progress

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if abort in progress
  FALSE - otherwise

===========================================================================*/
boolean rrcrce_is_abort_in_progress();

/*===========================================================================
FUNCTION rrcrce_get_rrc_connection_status

DESCRIPTION
  This function looks into RRC Connection Establishment provcedure's 
  sub-states and returns the status of RRC Connection.

DEPENDENCIES
  None

RETURN VALUE
  rrcrce_rrc_conn_status_e_type: Returns RRCRCE_RRC_CONN_ESTABLISHED if RRC
  Connection Establishment procedure is in RRCRCE_FINAL substate. Otherwise
  it returns RRCRCE_RRC_CONN_NOT_ESTABLISHED.

SIDE EFFECTS
  None
===========================================================================*/
rrcrce_rrc_conn_status_e_type rrcrce_get_rrc_connection_status
(
  void                  
);

/*===========================================================================
FUNCTION rrcrce_is_rce_procedure_inactive

DESCRIPTION
  This function looks into RRC Connection Establishment provcedure's 
  sub-states and returns the status of RRC Connection Establishment
  Procedure.

DEPENDENCIES
  None

RETURN VALUE
  boolean: TRUE if RCE is still inactive. (Note - when RCE is waiting for 
           UE to camp, the function will still return TRUE) 
           FALSE when RCE is not in Initial substate and UE is camped on 
           a cell.

SIDE EFFECTS
  To be used only by CSP as the logic is designed for CSP's requirement.
===========================================================================*/
boolean  rrcrce_is_rce_procedure_inactive
(
  void
);

/*===========================================================================
FUNCTION rrcrce_change_substate_to_rrcrce_final

DESCRIPTION
  This function changes the substate of RRC Connection Establishment provcedure's 
  to RRCRCE_FINAL

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  To be used only during G2W Handover and should not be used for any other 
  purpose.
===========================================================================*/
void  rrcrce_change_substate_to_rrcrce_final
(
  void
);
/*===========================================================================
FUNCTION rrcrce_force_initialize

DESCRIPTION
  This function changes the substate of RRC Connection Establishment provcedure's 
  to RRCRCE_INITIAL and sends the required Indication/Response to MM depending
  on RRCRCE substate. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  To be used only during Mode Change and when an Out of Service Area is indicated
  to MM while in IDLE DISCONNECTED state.
  It should not be used for any other purpose.
===========================================================================*/

void rrcrce_force_initialize
(
  boolean initiate_cell_selection
);

/*===========================================================================

FUNCTION rrcrce_get_num_retries_count

DESCRIPTION
  This function returns the count of number of times RRC connection request
  retries have been maxed out.

DEPENDENCIES
  None

RETURN VALUE
  uint8

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 rrcrce_get_num_retries_count(void);

/*===========================================================================

FUNCTION rrcrce_verify_ue_identity

DESCRIPTION
  This function checks whether UE identity in the downlink CCCH message is 
  same as UE initial identity.

DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type: This returns RRCRCE_SUCCESS if the match occurs. 
  Otherwise it returns RRCRCE_FAILURE_MINOR.

SIDE EFFECTS
  None
===========================================================================*/
rrcrce_status_e_type rrcrce_verify_ue_identity
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to the received 
                                                command */
);

/*===========================================================================
FUNCTION RRCRCE_IS_RCE_PROCEDURE_WAITING_FOR_BPLMN

DESCRIPTION
  This function looks into RRC Connection Establishment procedure's 
  sub-states and returns the status of RRC Connection Establishment
  Procedure.

DEPENDENCIES
  None

RETURN VALUE
  boolean: 
           TRUE  - if RCE is waiting for BPLMN search to be suspended/aborted.
           FALSE - otherwise
           
SIDE EFFECTS
  To be used only by CSP for BPLMN search as the logic is designed for CSP's requirement.
===========================================================================*/
boolean rrcrce_is_rce_procedure_waiting_for_bplmn(void);




/*===========================================================================

FUNCTION rrcrce_verify_ue_identity_r5

DESCRIPTION
  This function checks whether UE identity in the Release 5 downlink CCCH message is 
  same as UE initial identity.

DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type: This returns RRCRCE_SUCCESS if the match occurs. 
  Otherwise it returns RRCRCE_FAILURE_MINOR.

SIDE EFFECTS
  None
===========================================================================*/
rrcrce_status_e_type rrcrce_verify_ue_identity_later_than_r3
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to the received 
                                                command */
);


/*===========================================================================

FUNCTION   rrcrce_get_sib16_ptr

DESCRIPTION

  This function gets the Sib16 ptr either from the actual sib16 or the
  SIB16 version of Default Configuration
  
DEPENDENCIES

  None.
 
RETURN VALUE

  NONE

SIDE EFFECTS

  None.

===========================================================================*/
struct rrc_SysInfoType16 * rrcrce_get_sib16_ptr(void);

/*===========================================================================

FUNCTION   rrcrce_get_rb_setup_r6_ptr_for_dc

DESCRIPTION

  This function gets the default config ptr from RB setup r6 I.E.
  
DEPENDENCIES

  None.
 
RETURN VALUE

  rrc_RadioBearerSetup_r6_IEs pointer

SIDE EFFECTS

  None.

===========================================================================*/
struct rrc_RadioBearerSetup_r6_IEs * rrcrce_get_rb_setup_r6_ptr_for_dc
(
  void
);

/*===========================================================================

FUNCTION   rrcrce_is_dc_config_on_hspa

DESCRIPTION

  This function will indicate if DC will configure RB's/SRB's on HSPA channel's
  i.e. EUL in UL & HSDPA on DL. Currently only DC-17 does config SRB's on 
   HSPA channel's.
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE -If DC will configure RB's/SRB's on HSPA channel's
  FASLE - other wise
  
SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcrce_is_dc_config_on_hspa
(
  void
);

/*===========================================================================

FUNCTION   rrcrce_get_rbs_ptr

DESCRIPTION

  This function gets the RBS ptr either from the actual RBS or the
  RBS version of Default Configuration
  
DEPENDENCIES

  None.
 
RETURN VALUE

  rrc_DL_DCCH_Message pointer

SIDE EFFECTS

  None.

===========================================================================*/
struct rrc_DL_DCCH_Message * rrcrce_get_rbs_ptr
(
  void
);

/*===========================================================================
FUNCTION rrcrce_get_rrc_est_cause

DESCRIPTION
  Returns establishment cause

DEPENDENCIES
  None

RETURN VALUE
  Returns establishment cause

SIDE EFFECTS
  None
===========================================================================*/
rrc_establish_cause_e_type rrcrce_get_rrc_est_cause
(
  void
);
#ifdef FEATURE_WCDMA_MTPL_THERMAL_BACKOFF 
/*===========================================================================
FUNCTION rrcrce_set_rrc_est_cause_for_emergency_call

DESCRIPTION
  Sets the establishment cause to RRC_EST_EMERGENCY_CALL

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_set_rrc_est_cause_for_emergency_call
(
  void
);
#endif
/*===========================================================================
FUNCTION rrcrce_get_rce_sub_state()

DESCRIPTION
  This function lwill return current RRC Connection Establishment 
  procedure's sub state.
DEPENDENCIES
  None

RETURN VALUE
  return current rce sub state.
SIDE EFFECTS
none
===========================================================================*/
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
rrcrce_substate_e_type  rrcrce_get_rce_sub_state
(
void
);

/*===========================================================================
FUNCTION rrcrce_check_for_rai_mismatch()

DESCRIPTION
  This function compares the camped on LAI/RAI with the one given in buffered EST_REQ to decide
  on whether to proceed with EST_REQ or reject it.
DEPENDENCIES
  None

RETURN VALUE
  TRUE: Mismatch and reject the buffered EST_REQ.
SIDE EFFECTS
none
===========================================================================*/
boolean rrcrce_check_for_rai_mismatch
(
rrc_plmn_identity_type camped_plmn,
rrc_SysInfoType1 *sib1_ptr
);

/*===========================================================================
FUNCTION RRC_RCE_GET_EST_CN_DOMAIN_ID

DESCRIPTION
  Returns NAS sent CN Domain id for connection establishment.

DEPENDENCIES
  None

RETURN VALUE
  Returns establishment CN Domain ID.

SIDE EFFECTS
  None
===========================================================================*/

rrc_cn_domain_identity_e_type rrc_rce_get_est_cn_domain_id(void);

/*===========================================================================
FUNCTION    rrcrce_send_conn_rel_cause_event 

DESCRIPTION
  This function will log the event named EVENT_WCDMA_CONN_REL_CAUSE to diag task.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcrce_send_conn_rel_cause_event
(
  uint8 conn_rel_cause
);

/*===========================================================================
FUNCTION    rrcrce_send_conn_req_cause_event 

DESCRIPTION
  This function will log the event named EVENT_WCDMA_CONN_REQ_CAUSE to diag task.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_conn_req_cause_event
(
  uint8 conn_req_cause
);


/*===========================================================================
FUNCTION rrcrce_clear_num_retries_count()

DESCRIPTION
  This function will clear rrcrce_num_retries_count value
DEPENDENCIES
  None

RETURN VALUE
none
SIDE EFFECTS
none
===========================================================================*/
void rrcrce_clear_num_retries_count
(
  void
);

/*===========================================================================

FUNCTION rrcrce_trim_conn_request_msg_ul_tm_tb_size

DESCRIPTION
  This function trimms connection request message.


DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcrce_trim_conn_request_msg_ul_tm_tb_size
(
  rrc_UL_CCCH_Message *msg_ptr,
  uint32 pdu_length
);
#ifdef FEATURE_QSH_EVENT_METRIC
/*===========================================================================

FUNCTION rrcrce_qsh_log_connection_est_failure

DESCRIPTION
  This function logs connection establishmnet failure

  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrcrce_qsh_log_connection_est_failure(void);
#endif

#endif /* RRCRCE_H */

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
/*===========================================================================

FUNCTION rrcrce_post_internal_csfb_abort_cmd

DESCRIPTION
  This function posts internal command to trigger RRC connection.
   abort when the CSFB CS call ends.


DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcrce_post_internal_csfb_abort_cmd(void);
#endif

/*===========================================================================

FUNCTION rrcrce_abort_resel

DESCRIPTION
  This function lets SIB know whether cell reselection needs to be aborted or not


DEPENDENCIES
  None.

RETURN VALUE
  boolean.

===========================================================================*/
boolean rrcrce_abort_resel
(
  rrc_plmn_identity_type plmn_id, 
  rrc_SysInfoType1* sib1_ptr,
  rrc_freq_type freq,
  rrc_scr_code_type scr_code
);

/*===========================================================================

FUNCTION rrcrce_ignore_resel

DESCRIPTION
  Tells whether cell reselection needs to be ignored or not


DEPENDENCIES
  None.

RETURN VALUE
  boolean.

===========================================================================*/
boolean rrcrce_ignore_resel
(
  rrc_freq_type freq,
  rrc_scr_code_type scr_code
);
/*===========================================================================

FUNCTION rrcrce_get_mm_abort_cause

DESCRIPTION
Returns current abort cause

DEPENDENCIES
  None.

RETURN VALUE
  rrc_abort_cause_e_type.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
rrc_abort_cause_e_type rrcrce_get_mm_abort_cause(void);
/*===========================================================================

FUNCTION rrcrce_return_abort_ind_sent_status

DESCRIPTION
Returns abort indication sent

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
boolean rrcrce_return_abort_ind_sent_status(void);