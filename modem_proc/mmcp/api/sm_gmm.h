#ifndef SM_GMM_H
#define SM_GMM_H

/*===========================================================================
                        S M - G M M  Header File

DESCRIPTION

   Definition of Primitive message types between Session Management Entity
   and GPRS Mobility Management Entity (SM <-> GMM)

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000, 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
 
/*===========================================================================

            EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM5200/sm/vcs/sm_gmm.h_v   1.5   18 Jul 2002 15:43:34   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/sm_gmm.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
            
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/26/12   am      Back off timer code
04/18/06   ss       Added cause value to GMMSM_GPRS_RELEASE_IND
11/09/05   kvk     Added prototype for sm_check_pdp_procedure_pending()
08/22/05   kvk     Added gmmsm_establish_ind_T received from GMM with cause
06/30/05   kvk     Added boolean argument for  prototype sm_modify_rab()
03/10/05   ss      Added support for handling incoming PDP Context Status IE while Rau Accept or Service
                   accept based on Rel-4 Spec CR N1-010325
10/18/04   kvk     MT PDP support added
09/07/04   ab      Added gmmsm_npdu_list_ind_T received from GMM.  
09/02/04   kvk     Added new function prototype SM_PDP_MODIFY_RAB to trigger
                   context modification for streaming or conversational RAB.
06/17/04   kvk     Added extern prototype for sm_check_pdp_exist()
03/31/04   ks      Added extern prototype for sm_is_pdp_active() 
03/18/03   mks     Added datatype gmmsm_rat_change_req_T for GMMSM_RAT_CHANGE_REQ
                   primitive.
02/27/03   mks     Added support for PS data continuity feature - Added new 
                   data type gmmsm_rat_resume_cnf_T. 
01/16/03   ks      Added enum for GMMSM_CLOSE_SESSION_REQ and also defined
                   gmmsm_session_close_req_T primitive. 
09/30/02   ks      Renamed GMMSM_ERROR_IND with GMMSM_CON_RELEASE_IND and
                   GMMSM_RELEASE_IND with GMMSM_GPRS_RELEASE_IND primitives.
07/18/02   ks      Renamed FEATURE_GPRS with FEATURE_GSM_GPRS
07/01/02   ks      Added gmmsm_tlli_assignment_ind_T primitive for GPRS
                   support.
06/29/01   ks      Added GMMSM_ERROR_IND primitive for GMM error notification.
                   Also added the cause SIG_CONNECTION_RELEASE.
05/17/01   ks      Added message_len field in the gmmsm_unitdata_ind_T.  
01/26/01   ks      Created file.
===========================================================================*/

#include "sys_stru.h"
#include "sys_cnst.h"
#include "sys_type.h"
#include "rrcmmif.h"

/* ---------------------------------------------------------------------------
* Message Primitive Types between Session Management (SM) and the GMM.
* These are as defined in TR 24.007, table 9.5.1
* --------------------------------------------------------------------------*/
                              
/* Primitives from SM to GMM */

#define GMMSM_ESTABLISH_REQ          0x01
#define GMMSM_UNITDATA_REQ           0x02
#define GMMSM_CLOSE_SESSION_REQ      0x03
#define GMMSM_RAT_RESUME_CNF         0x04

#define GMMSM_FAST_DORMANCY_REQ      0x05

#define GMMSM_OK_TO_TUNEAWAY_IND     0x07

/* --------------------------------------------------------------------------
** The following are the Error causes between GMM and SM. 
** ------------------------------------------------------------------------*/
#define SIG_CONNECTION_RELEASE  0x20

#define SIG_CONNECTION_RELEASE_UE_INITIATED 0x21


/* Commands from GMM to SM */
/* These are defined in the SMTASK.H file */

/* --------------------------------------------------------------------------
** Data types used in the Primitives in GMMSM-SAP
** ------------------------------------------------------------------------*/
#define GMM_PDP_MBMS_CONTEXT_STATUS_LENGTH 0x12


/* --------------------------------------------------------------------------
 Enumeration that identifies the ps signal release request casue 
** ------------------------------------------------------------------------*/
typedef enum
{
  SM_PS_SIG_REL_REQ,
  SM_UE_PS_SIG_REL_REQ,
  SM_DS_PS_SIG_REL_REQ,
  SM_TS_DS_SIG_REL_REQ
} sm_ps_session_close_req_type;


/*-------------------------------------------------------------------
 ENUM: gmmsm_fast_dormancy_cnf_cause_e_type
 This enumerates the status of a fast dormancy Confirm from RRC
--------------------------------------------------------------------*/
typedef enum
{
  GMMSM_FAST_DORMANCY_SUCCESS_APP_NO_RETRY,
  GMMSM_FAST_DORMANCY_SUCCESS_APP_RETRY,
  GMMSM_FAST_DORMANCY_FAILURE_APP_RETRY
} gmmsm_fast_dormancy_cnf_cause_e_type;  


/**************************************************************************
          messages from SM to GMM , defined in table 9.5.1 in ts 24.007
**************************************************************************/                                                                  
/* --------------------------------------------------------------------------
** Primitive : GMMSM_ESTABLISH_REQ.
** ------------------------------------------------------------------------*/
typedef struct gmmsm_establish_req
{ 
  IMH_T         header;
  rrc_establish_cause_e_type cause;    /* Establish cause for RRC connection */

  sys_modem_as_id_e_type    as_id;

} gmmsm_establish_req_T;

/* --------------------------------------------------------------------------
** Primitive : GMMSM_RAT_RESUME_CNF.
** ------------------------------------------------------------------------*/
typedef struct gmmsm_rat_resume_cnf
{ 
  IMH_T         header;

  sys_modem_as_id_e_type    as_id;

} gmmsm_rat_resume_cnf_T;

/* --------------------------------------------------------------------------
** Primitive : GMMSM_SESSION_CLOSE_REQ.
** ------------------------------------------------------------------------*/
typedef struct gmmsm_session_close_req
{ 
  IMH_T         header;

  sm_ps_session_close_req_type cause;


  sys_modem_as_id_e_type    as_id;

} gmmsm_session_close_req_T;

/* --------------------------------------------------------------------------
** Primitive : GMMSM_UNITDATA_REQ.
** ------------------------------------------------------------------------*/
typedef struct gmmsm_unitdata_req
{ 
  IMH_T             header;

  byte *data;

  boolean           purgeQueueFlag;  /* an Indication ultimately passed to RLC to clean up the buffers:
                                      This Flag set to TRUE when
                                      a. Only One PDP exists and is in Deactivation process
                                      b. All the existing PDP are in Deactivation process
                                     */
  boolean          ps_vote_flag;  /* To indicate GMM to allow reselctions/HOs when RRC query;
                                     SM will block the reselctions/HOs till the RAB established */
  sys_modem_as_id_e_type    as_id;

} gmmsm_unitdata_req_T;

/* --------------------------------------------------------------------------
** Primitive : GMMSM_RAT_RESUME_CNF.
typedef struct gmmsm_rat_resume_cnf
{ 
  IMH_T             header;
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type    as_id;
#endif
} gmmsm_rat_resume_cnf_T;
** ------------------------------------------------------------------------*/

/**************************************************************************
            messages from GMM to SM , defined in table 9.5.1 in ts 24.007
**************************************************************************/

/* --------------------------------------------------------------------------
** Primitive : GMMSM_ESTABLISH_CNF
** ------------------------------------------------------------------------*/
/* --------------------------------------------------------------------------
** Note : This primitive doesn't have any fields, the cmd_id in the header 
          field of SM_CMD_TYPE (defined in smtask.h) is sufficient.
** ------------------------------------------------------------------------*/


/* --------------------------------------------------------------------------
** Primitive : GMMSM_FAST_DORMANCY_CNF, used to indicate SIG Connectin Release.status
** ------------------------------------------------------------------------*/
typedef struct gmmsm_fast_dormancy_cnf
{
  gmmsm_fast_dormancy_cnf_cause_e_type      cause;    /* defined in sm_gmm.h */

  sys_modem_as_id_e_type    as_id;

} gmmsm_fast_dormancy_cnf_T;


/* --------------------------------------------------------------------------
** Primitive : GMMSM_CON_RELEASE_IND, used to indicate SIG Connectin Release.
** ------------------------------------------------------------------------*/
typedef struct gmmsm_con_release_ind
{
  cc_cause_T      cause;    /* defined in sys_type.h as byte */

  sys_modem_as_id_e_type    as_id;

} gmmsm_con_release_ind_T;

/* --------------------------------------------------------------------------
** Primitive : GMMSM_ESTABLISH_REJ.
** ------------------------------------------------------------------------*/
typedef struct gmmsm_establish_rej
{ 
  sm_cm_status_T      cause;    /* defined in sys_type.h as byte */

  sys_modem_as_id_e_type    as_id;

} gmmsm_establish_rej_T;

/* --------------------------------------------------------------------------
** Primitive : GMMSM_ESTABLISH_IND.
** ------------------------------------------------------------------------*/
typedef struct gmmsm_establish_ind
{ 
  sm_cm_status_T      cause;    /* defined in sys_type.h as byte */

  sys_modem_as_id_e_type    as_id;

} gmmsm_establish_ind_T;

/* --------------------------------------------------------------------------
** Primitive : GMMSM_GPRS_RELEASE_IND.
** ------------------------------------------------------------------------*/
typedef struct gmmsm_gprs_release_ind
{ 
  sm_cm_status_T      cause;    /* defined in sys_type.h as byte */

  sys_modem_as_id_e_type    as_id;

} gmmsm_gprs_release_ind_T;

/* --------------------------------------------------------------------------
** Primitive : GMMSM_UNITDATA_IND.
** ------------------------------------------------------------------------*/
typedef struct gmmsm_unitdata_ind
{ 
  uint16                    message_len;       /* Length of received SM-PDU */
 
  byte                     *data;

  sys_modem_as_id_e_type    as_id;

} gmmsm_unitdata_ind_T;

/* --------------------------------------------------------------------------
** Primitive : GMMSM_NDPU_LIST_IND.
** ------------------------------------------------------------------------*/
typedef struct gmmsm_npdu_list_ind
{ 
  boolean       nsapi_valid_list[11];  /* GMM_RECEIVE_NPDU_NUMBER_LIST_MAX_NSAPIS */
  byte          npdu_number_list[11];

  sys_modem_as_id_e_type    as_id;

} gmmsm_npdu_list_ind_T;


/* --------------------------------------------------------------------------
** Primitive : GMMSM_TLLI_ASSIGNMENT_IND.
** ------------------------------------------------------------------------*/

typedef struct gmmsm_tlli_assignment_ind
{
  uint32 tlli;

  sys_modem_as_id_e_type    as_id;

} gmmsm_tlli_assignment_ind_T ;


/* --------------------------------------------------------------------------
** Primitive : GMMSM_RAT_CHANGE_REQ.
** ------------------------------------------------------------------------*/
typedef struct gmmsm_rat_change_req
{
  IMH_T         header;
  sys_radio_access_tech_e_type rat;

  boolean abort_procedure;  


  sys_modem_as_id_e_type    as_id;

} gmmsm_rat_change_req_T;


/* --------------------------------------------------------------------------
** Primitive : GMMSM_RAT_RESUME_REQ.
** ------------------------------------------------------------------------*/
typedef struct gmmsm_rat_resume_req
{
  IMH_T         header;
  sys_modem_as_id_e_type    as_id;
} gmmsm_rat_resume_req_T;


/* --------------------------------------------------------------------------
** Primitive : GMMSM_CON_RELEASE_REJ.
** ------------------------------------------------------------------------*/
typedef struct gmmsm_con_release_rej
{
  IMH_T         header;
  sys_modem_as_id_e_type    as_id;
} gmmsm_con_release_rej_T;



/* --------------------------------------------------------------------------
** Primitive : GMMSM_PDP_MBMS_STATUS_IND .
** ------------------------------------------------------------------------*/
typedef struct
{
  boolean pdp_status_length;
  boolean mbms_status_length;
  byte value[GMM_PDP_MBMS_CONTEXT_STATUS_LENGTH];

  sys_modem_as_id_e_type    as_id;

}gmmsm_pdp_mbms_context_status_T;

/* --------------------------------------------------------------------------
** Primitive : GMMSM_STATUS_IND .
** ------------------------------------------------------------------------*/
typedef struct
{
  sys_modem_as_id_e_type    as_id;

}gmmsm_status_ind_T;


/* --------------------------------------------------------------------------
** Primitive : GMMSM_ESTABLISH_CNF.
** ------------------------------------------------------------------------*/
typedef struct gmmsm_establish_cnf
{ 
  sys_modem_as_id_e_type    as_id;
  boolean                   is_pdp_retransmit_req;
} gmmsm_establish_cnf_T;

/* --------------------------------------------------------------------------
** Primitive : GMMSM_PS_FOP_IND.
** ------------------------------------------------------------------------*/
typedef struct gmmsm_ps_fop_ind
{ 
  sys_modem_as_id_e_type    as_id;
} gmmsm_ps_fop_ind_T;


/* --------------------------------------------------------------------------
** Primitive : GMMSM_OK_TO_TUNEAWAY, used to indicate if its ok to tuneaway
** ------------------------------------------------------------------------*/
typedef struct gmmsm_ok_to_tuneaway_ind
{
  IMH_T                     header;

  boolean                   ok_to_tuneaway;

  sys_modem_as_id_e_type    as_id;

} gmmsm_ok_to_tuneaway_ind_T;


/* --------------------------------------------------------------------------
** Primitive : GMMSM_RAT_RESUME_REQ.
** ------------------------------------------------------------------------*/
/* --------------------------------------------------------------------------
** Note : This primitive doesn't have any fields, the cmd_id in the header 
          field of SM_CMD_TYPE (defined in smtask.h) is sufficient.
** ------------------------------------------------------------------------*/

/* --------------------------------------------------------------------------
** Primitive : SM_CHECK_PDP_EXIST, returns TRUE if any PDP context exists
** ------------------------------------------------------------------------*/
extern boolean sm_check_pdp_exist(void);


/* --------------------------------------------------------------------------
** Primitive : SM_CHECK_PDP_OBJECT_EXIST, returns TRUE if any PDP context exists
** ------------------------------------------------------------------------*/
extern boolean sm_check_pdp_object_exist(void);


/* --------------------------------------------------------------------------
** Primitive : SM_CHECK_PDP_PROCEDURE_PENDING, 
             returns TRUE if any PDP procedure is pending, FALSE otherwise
** ------------------------------------------------------------------------*/
extern boolean sm_check_pdp_procedure_pending(void);

/* --------------------------------------------------------------------------
** Function   : SM_MODIFY_RAB, 
** Description: Checks any RAB with traffic class streaming or conversational 
                is in dormant state and triggers RAB reestablsihment
** ------------------------------------------------------------------------*/
extern void sm_modify_rab(boolean);


typedef struct gmmsm_isr_status_ind
{
  boolean isr_status;     
  sys_modem_as_id_e_type as_id;
}gmmsm_isr_status_ind_t;

typedef struct gmmsm_unblock_all_apn
{ 
   sys_modem_as_id_e_type    as_id;
} gmmsm_unblock_all_apn_T;

typedef struct gmmsm_multimode_subs_chgd_req_s
{ 
   sys_modem_as_id_e_type    as_id;
}gmmsm_multimode_subs_chgd_req_T;


#endif

