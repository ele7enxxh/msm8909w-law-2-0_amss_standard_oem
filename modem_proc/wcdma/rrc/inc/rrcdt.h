#ifndef RRCDT_H
#define RRCDT_H
/*===========================================================================
                  R R C  D I R E C T  T R A N S F E R

DESCRIPTION
  This header file contains function prototypes for five procedures that deal
  with direct transfer:
  Initial Direct Transfer, Uplink Direct Transfer, Downlink Direct Transfer,
  Signalling Connection Release, and Signalling Connection Release Request.

Copyright (c) 2001-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcdt.h_v   1.7   27 Jul 2001 17:32:46   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/inc/rrcdt.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/16   ad      Made changes to log important parms from OTAs
09/11/15   sr      Made changes for FR29345 EFS/NV Settings to be Read per Sub
06/09/14   sn      Changes to initialize SCRR if active while aborting dormancy
                   procedure
02/19/14   sn      Ignore CCO if PS domain release in progress
10/03/12   sr      Modified function prototype to support setting establishment cause
                   for IDT
12/13/11   sks     Added support for LTOW SRVCC.
10/04/11   gv      Modified function prototypes to support providing call termination
                   cause to NAS when RL failure happens
05/25/11   rl      Fixed the compilation error
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
04/23/09   kp      Bug fix to set udt substate to init when we delete all saved UDT's.
03/19/09   kp      Added support for handling clash between close_session_ind 
                   from CU due to local release of signalling connection & 
                   pending close_session_req from NAS.
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
09/12/08   ns      Added code for REl7 under FEATURE_WCDMA_REL7
10/23/07   kp      Added support for network sharing feature.Changes are under feature flag 
                   FEATURE NETWORK SHARING.
10/03/07   ns      Added the prototype of function rrcdt_check_if_emergency_call_initiated
09/11/06   kp      Added the prototype of function rrcscrr_is_cn_domain_open
08/28/06   kp      Added Bit masks which are needed for decoding Protocol discriminator
                   & Transaction I.D from NAS OTA message
09/19/05   ss      prototype name rrcidt_set_sent_by_CU changed to alllower case
09/12/05   ss      Added prototypes for rrcidt_check_if_est_cause_available and
                   rrcidt_set_sent_by_CU, these will be used by CU procedure also
                   when CU has to fill the establishment cause for Rel5
09/09/07   vm      Added prototypes in DT procedure to check if an UE is OOS when
                   an Emergency call is received.
06/30/05   Sh      Added prototype for the new funtion 
                   rrcdt_is_idt_waitingfor_l2ack which is called
                   by Cell update procedure to check if purging of RB3 is 
                   required.
02/10/04   kc      Added header definitions for API's to send open_sess_cnf and
                   for check to see if L2_ACK is rcvd. for open_sess_req.
12/10/03   kc      Added additional causes in sig_con_rel_cause_type to reject
                   sig_conn_rel if active rab's are present
04/24/03   rj      Added a prototype for function 
                   rrcscr_get_signalling_connection_status and also updated
                   copy right.
08/02/02   kc      Added fn. prototype to read NV item FAKE_SECURITY_ENABLED 
07/27/01   rj      Added new function prototype to reset Signalling 
                   Connections to release status.
05/14/01   rj      Added function rrcscr_release_signalling_connection to
                   provide interface for Radio Bearer Release procedure to
                   release a Signalling Connection if it exists. Added enum
                   type rrcscr_sig_conn_rel_status_e_type.
04/18/01   rj      Modified value for RRCDT_MAX_PAYLOAD to 4095.Updated some
                   comments.
04/16/01   rj      Added more description to the introduction.
04/06/01   rj      Replaced RRC_NEW_STATE_IND with RRC_STATE_CHANGE_IND.
03/09/01   rj      Separated event handlers so that future expandability is
                   easy.
03/02/01   rj      Modified value for RRCDT_MAX_PAYLOAD to 4096.
01/17/01   jrs     Created file.

===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */


#include "wcdma_variation.h"
#include "rrcmmif.h"
#include "rrclsmif.h"
#include "rrcdata_v.h"
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* This is the upper bound on the size of the payload in the Initial
 * Direct Transfer and Uplink Direct Transfer Messages. This value is
 * specified in the ASN.1.
 */
#define RRCDT_MAX_PAYLOAD 4095

#ifdef FEATURE_NAS_UDT_OOS
#error code not present
#endif
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

typedef enum
{
  RRCSCR_SIG_CONN_NOT_EXIST,        /* Indicates that Signalling Connection
                                       does not exist for the particular\
                                       CN Domain */
  RRCSCR_SIG_CONN_RELEASED,         /* Indicates that Signalling Connection
                                       is released successfully */
  RRCSCR_RAB_PRESENT_IN_SIG_DOMAIN, /* Indicates that a RAB is present in
                                       the signalling connection that is 
                                       being requested to be released*/
  RRCSCR_NO_RAB_PRESENT_IN_SIG_DOMAIN/* Indicates that RAB is not present in
                                       the signalling connection that is 
                                       being requested to be released*/
} rrcscr_sig_conn_rel_status_e_type;


typedef enum
{
  RRCDT_SUCCESS,                            /* Successful Status */      
  RRCDT_FAILURE_MINOR,                      /* Minor Failure Status */
  RRCDT_FAILURE_MAJOR                       /* Major Failure status */
} rrcdt_status_e_type;


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

FUNCTION rrcidt_check_if_est_cause_available

DESCRIPTION
  This function is called by CU as well as IDT procedure just before transmitting 
  a over the air message
  to see if establish cause needs to be set.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

boolean rrcidt_check_if_est_cause_available(rrc_proc_e_type proc_id, 
                             rrc_establish_cause_e_type *est_cause
                             ,rrc_call_type_in_est_req_e_type *call_type
                             );
/*===========================================================================

FUNCTION rrcidt_is_cs_call_initiate

DESCRIPTION
  This function is called by CU to confirm if CS call is being initiated.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE : If CS call initiated CU
  FALSE : Else.

===========================================================================*/

boolean rrcidt_is_cs_call_initiate(void );


/*===========================================================================

FUNCTION rrcidt_set_sent_by_cu

DESCRIPTION
  This function is called by CU to say that it has transmitted to the n/w, 
  helpful in case of priority of any cell update
  cause overwrites the cell update cause, uplink data transmission.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcidt_set_sent_by_cu( rrc_proc_e_type proc_id);



/*===========================================================================

FUNCTION rrcdt_init_procedure

DESCRIPTION
  This function initializes everything that the Direct Transfer procedure
  needs to have initialized.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcdt_init_procedure(void);
/*===========================================================================

FUNCTION rrcdt_is_cs_domain_open

DESCRIPTION
  This function returns whether session for CS domain is open or not.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - CS Domain is Open.
  FALSE - CS Domain isn't Open.

===========================================================================*/
boolean rrcdt_is_cs_domain_open(void);
/*===========================================================================

FUNCTION rrcdt_is_ps_domain_open

DESCRIPTION
  This function returns whether session for PS domain is open or not.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - PS Domain is Open.
  FALSE - PS Domain isn't Open.

===========================================================================*/

boolean rrcdt_is_ps_domain_open(void);

/*===========================================================================

FUNCTION rrcidt_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the initial direct transfer procedure.
  The following events are handled by this procedure:

    RRC_OPEN_SESSION_REQ Primitive (received from MM)
    RRC_STATE_CHANGE_IND Primitive( received from State Change Manager )
    RRC_CELL_UPDATE_COMPLETE_CNF Primitive (received from Cell Update Proc)

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcidt_procedure_event_handler(rrc_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION rrcudt_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the uplink direct transfer procedure.
  The following events are handled by this procedure:

    RRC_DATA_REQ primitive (received from MM)
    RRC_STATE_CHANGE_IND Primitive( received from State Change Manager )
    RRC_CELL_UPDATE_COMPLETE_CNF Primitive (received from Cell Update Proc)
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcudt_procedure_event_handler(rrc_cmd_type *cmd_ptr);
/*===========================================================================

FUNCTION rrcscrr_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the signalling connection release
  request procedure. The following events are handled by this procedure:

    RRC_CLOSE_SESSION_REQ primitive (received from MM)
    RRC_STATE_CHANGE_IND Primitive( received from State Change Manager )
    RRC_CELL_UPDATE_COMPLETE_CNF Primitive (received from Cell Update Proc)


DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcscrr_procedure_event_handler(rrc_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION rrcddt_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the downlink direct transfer
  procedure. The following events are handled by this procedure:

    Downlink Direct Transfer Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcddt_procedure_event_handler(rrc_cmd_type *cmd_ptr);
/*===========================================================================

FUNCTION rrcscr_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the signalling connection release
  procedure. The following events are handled by this procedure:

    Signalling Connection Release Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcscr_procedure_event_handler(rrc_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION rrcscr_release_signalling_connection

DESCRIPTION
  This function checks whether Signalling connection exist for the particular
  CN Domain and if it exits, then sends RRC_CLOSE_SESSION_IND to the MM. Also
  sets the local variable for signalling connection status to FALSE.


DEPENDENCIES
  None.

RETURN VALUE
  rrcscr_sig_conn_statue_e_type: it returns RRCSCR_SIG_CONN_RELEASED if there
  exits a signalling connection for the CN domain specified in the formal 
  parameter and if RRCSCR procedure successfully sends RRC_CLOSE_SESSION_IND
  to the MM. Otherwise it retuns RRCSCR_SIG_CONN_NOT_EXIST.

===========================================================================*/

rrcscr_sig_conn_rel_status_e_type rrcscr_release_signalling_connection
(
  rrc_cn_domain_identity_e_type cn_domain_id
);

/*===========================================================================

FUNCTION rrcscr_reset_signalling_connection_status

DESCRIPTION
  This function sets both CS and PS signalling Connections to release status .

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcscr_reset_signalling_connection_status
(
  void                    
);

/*===========================================================================

FUNCTION rrcdt_get_nv_items

DESCRIPTION
  This  function gets NV  item NV_RC_INTEGRITY_ENABLED  and
  NV_RRC_CIPHERING_ENABLED and stores it in local variables
  used to determine if Integrity/Ciphering are supported or
  not

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcdt_get_nv_items(sys_modem_as_id_e_type wrrc_as_id);
/*===========================================================================

FUNCTION rrcidt_is_idt_waiting_for_l2ack_per_domain

DESCRIPTION
  This function returns TRUE/FALSE depending if L2_ACK is pending for
  CS or PS domain which is passed as a formal parameter to the fn.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE:  The CN domain requested is waiting for L2_ACK
  FALSE: The CN domain requested is not waiting for L2_ACK 

===========================================================================*/

boolean rrcidt_is_idt_waiting_for_l2ack_per_domain
(
  rrc_cn_domain_identity_e_type cn_domain
);
/*===========================================================================

FUNCTION rrcidt_send_open_session_cnf  

DESCRIPTION
  This function builds the RRC_OPEN_SESSION_CNF command with parameters and 
  posts the same to the MM task.
DEPENDENCIES
  None.

RETURN VALUE
  rrcdt_status_e_type: It returns RRCDT_SUCCESS if it is successfully sends 
  the RRC_OPEN_SESSION_CNF command. Otherwise it returns RRCDT_FAILURE_MAJOR.

===========================================================================*/

void rrcidt_send_open_session_cnf
(
  rrc_open_session_status_e_type      session_status,  /* Success or failure
                                                          status*/
  rrc_cn_domain_identity_e_type       cn_domain_id     /* CN Domain Id */

);

/*===========================================================================


FUNCTION rrcdt_is_idt_waitingfor_l2ack

DESCRIPTION
  This function is used by other procedures like CU to get the state of idt

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
  
boolean rrcdt_is_idt_waitingfor_l2ack(void);

/*===========================================================================

FUNCTION is_ue_oos_and_em_call_attempted

DESCRIPTION
  This function takes UE to idle if UE is OOS (in Connected Mode) and an emergency call 
  is attempted
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: UE is OOS and EM call is attempted. 
  FALSE: otherwise

===========================================================================*/
boolean is_ue_oos_and_em_call_attempted 
(
  rrc_cmd_type    *cmd_ptr, 
  rrc_proc_e_type rrc_proc
);

/*===========================================================================

FUNCTION rrcscrr_is_cn_domain_open

DESCRIPTION
  This function will return the status of CN domain whether its open or closed
  It also checks to see if signalling connection relaese or
  close session request is in progress. This function is used by DDT
  to check whether the domain is open.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE : If the CN Domain is open.
  FALSE: If the CN Domain is closed.

===========================================================================*/
uint8 rrcscrr_is_cn_domain_open
(
rrc_cn_domain_identity_e_type cn_domain
);


/*===========================================================================

FUNCTION rrcdt_check_if_emergency_call_initiated

DESCRIPTION
  This function checks whether emergency call has been initiated for Initial Direct Transfer and Uplink Direct Transfer

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Emergency call is initiated
  FALSE: Otherwise

===========================================================================*/
boolean rrcdt_check_if_emergency_call_initiated(void);

/*==========================================================================

FUNCTION rrcdt_fill_plmn_id_in_idt

DESCRIPTION
  This function will fill's selected PLMN I.D in
  IDT message.
  .
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcdt_fill_plmn_id_in_idt
(
  /* PLMN I.D to be sent over the air */
  rrc_PLMN_Identity *plmn_id_msg_ie,
  /*CN Domain I.Dfor which PLMN I.D to be filled*/
  rrc_cn_domain_identity_e_type cn_id
);


/*==========================================================================

FUNCTION rrcdt_is_dt_initiated_cu

DESCRIPTION
  .
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean rrcdt_is_dt_initiated_cu
(
  void
);

/*===========================================================================

FUNCTION rrcscr_release_signalling_connection

DESCRIPTION
  This function checks whether Signalling connection exist for the particular
  CN Domain and if it exits, then sends RRC_CLOSE_SESSION_IND/CNF to the MM. Also
  sets the local variable for signalling connection status to FALSE.


DEPENDENCIES
  None.

RETURN VALUE
  rrcscr_sig_conn_statue_e_type: it returns RRCSCR_SIG_CONN_RELEASED if there
  exits a signalling connection for the CN domain specified in the formal 
  parameter and if RRCSCR procedure successfully sends RRC_CLOSE_SESSION_IND
  to the MM. Otherwise it retuns RRCSCR_SIG_CONN_NOT_EXIST.

===========================================================================*/
rrcscr_sig_conn_rel_status_e_type rrcscr_local_release_of_signalling_connection
(
  rrc_cn_domain_identity_e_type cn_domain_id,
  rrc_session_close_cause_e_type cause
);

/*===========================================================================

FUNCTION rrcscrr_delete_close_session_req

DESCRIPTION
  This function deletes the Close Session req data from the static variable
  for the specified CN domain

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcscrr_delete_close_session_req
(
  rrc_cn_domain_identity_e_type cn_domain_id
);

/*===========================================================================

FUNCTION rrcscr_send_close_session_ind  

DESCRIPTION
  This function builds the RRC_CLOSE_SESSION_IND command with parameters and 
  posts the same to the MM task.
DEPENDENCIES
  None.

RETURN VALUE
  rrcdt_status_e_type: It returns RRCDT_SUCCESS if it is successfully sends 
  the RRC_CLOSE_SESSION_IND command. Otherwise it returns RRCDT_FAILURE_MAJOR.

===========================================================================*/

rrcdt_status_e_type rrcscr_send_close_session_ind
(
  rrc_cn_domain_identity_e_type       cn_domain_id,     /* CN Domain Id */
  rrc_session_close_cause_e_type      cause

);

/*===========================================================================

FUNCTION rrcscrr_send_sig_conn_release_cnf  

DESCRIPTION
  This function builds the RRC_SIG_CONN_RELEASE_CNF command with parameters and 
  posts the same to the MM task.
DEPENDENCIES
  None.

RETURN VALUE
  rrcdt_status_e_type: It returns RRCDT_SUCCESS if it is successfully sends 
  the RRC_SIG_CONN_RELEASE_CNF command. Otherwise it returns RRCDT_FAILURE_MAJOR.

===========================================================================*/

void rrcscrr_send_sig_conn_release_cnf
(
  uecomdef_status_e_type              status,  /* Success or failure status*/
  rrc_proc_e_type rrc_proc                     /* Dest rrc proc */
);

/*===========================================================================

FUNCTION rrcscr_indicate_close_session

DESCRIPTION
  This function will check & delete any pending CLOSE_SESSION_REQ for a  domain.
  based on scrr substae it will send either close_session_cnf/ind or sig_conn_rel 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If IDT is buffered for a particular CN domain.
  FALSE:Otherwise.
===========================================================================*/
rrcdt_status_e_type rrcscr_indicate_close_session
(
  rrc_cn_domain_identity_e_type       cn_domain_id,     /* CN Domain Id */
  rrc_session_close_cause_e_type      cause
);
/*===========================================================================

FUNCTION rrcudt_any_saved_data_req_for_cn_domain

DESCRIPTION
  This function will give status of any saved data req for a given CN domain

DEPENDENCIES
  None.

RETURN VALUE
  TRUE:- If any dat req saved of in process for a CN domain
  FALSE:- otherwise

===========================================================================*/
boolean rrcudt_any_saved_data_req_for_cn_domain
(
  rrc_cn_domain_identity_e_type cn_domain_id
);
/*===========================================================================

FUNCTION rrcudt_delete_all_saved_data_req

DESCRIPTION
  This function deletes the Data Req cmd from the static variable
  for the specified CN domain

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcudt_delete_all_saved_data_req
(
  rrc_cn_domain_identity_e_type cn_domain_id
);


/*===========================================================================

FUNCTION rrcidt_update_proc_id  

DESCRIPTION
  This function will be called by RCE to update the variable 
  last_proc_sent so that IDT cause can be skipped from filling
  for the first time

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcidt_update_proc_id(void);

/*===========================================================================

FUNCTION   rrcidt_is_idt_active

DESCRIPTION
  This function checks if idt for the given domain is in progress
DEPENDENCIES
  None.

RETURN VALUE
  TRUE indicates that it is active, FALSE otherwise.

===========================================================================*/

boolean rrcidt_is_idt_active
(
   rrc_cn_domain_identity_e_type domain_id
);

/*===========================================================================

FUNCTION rrcdt_update_scrr_to_handle_dormancy

DESCRIPTION
  This function handles RRC_CLOSE_SESSION_REQ and/or RRC_SIG_CONN_RELEASE_REQ in case 
  RRC_FAST_DORMANCY_REQ is active. 
  It updates the RRC_CLOSE_SESSION_REQ and/or RRC_SIG_CONN_RELEASE_REQ database
  and aborts RRC_FAST_DORMANCY_REQ.

  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Emergency call is initiated
  FALSE: Otherwise

===========================================================================*/
boolean rrcdt_update_scrr_to_handle_dormancy
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
);

#ifdef FEATURE_LTE_TO_WCDMA
/*===========================================================================

FUNCTION rrcdt_check_n_send_sync_ind_for_ps_to_fake_security

DESCRIPTION
  This function sends SYNC_IND for PS domain for integrity
  protection
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Emergency call is initiated
  FALSE: Otherwise

===========================================================================*/
void rrcdt_check_and_send_sync_ind_for_ps_to_fake_security( void );
#endif

/*===========================================================================

FUNCTION rrcscrr_is_conn_rel_in_progress

DESCRIPTION
  Returns if SCRR for passed domain is in progress
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if SCRR in progress else FALSE

===========================================================================*/
boolean rrcscrr_is_conn_rel_in_progress(rrc_cn_domain_identity_e_type cn_domain_id);

/*===========================================================================

FUNCTION rrcscrr_abort_ps_rel_proc

DESCRIPTION
  Initializes SCRR procedure

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcscrr_abort_ps_rel_proc(void);
/*===========================================================================

FUNCTION rrc_qsh_log_scri_params

DESCRIPTION
  Initializes SCRR procedure

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrc_qsh_log_scri_params(uint8 scri_cause, uint8 domain_id);

/*===========================================================================

FUNCTION rrc_qsh_log_scrr_params

DESCRIPTION
  Initializes SCRR procedure

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrc_qsh_log_scrr_params(uint8 domain_id);

/*===========================================================================

FUNCTION rrc_qsh_log_idt_params

DESCRIPTION
  Initializes SCRR procedure

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrc_qsh_log_idt_params(uint8 est_cause, uint8 domain_id);

/*===========================================================================

FUNCTION rrc_qsh_log_conn_setup_compl_params

DESCRIPTION
  Initializes SCRR procedure

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrc_qsh_log_conn_setup_compl_params(uint16 cont_length);

/*===========================================================================

FUNCTION rrc_qsh_log_conn_rej_params

DESCRIPTION
  Initializes SCRR procedure

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrc_qsh_log_conn_rej_params(rrc_log_rrc_conn_rej_ota *conn_rej_params);

/*===========================================================================

FUNCTION rrc_qsh_log_conn_req_params

DESCRIPTION
  Initializes SCRR procedure

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrc_qsh_log_conn_req_params(rrc_log_rrc_conn_req_ota *conn_req_params);

#endif /* RRCDT_H */

