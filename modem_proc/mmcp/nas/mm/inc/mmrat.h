#ifndef MMRAT_H
#define MMRAT_H
/*===========================================================================

        M M   I N T E R R A T   H A N D L E R   H E A D E R   F I L E

DESCRIPTION
  This file contains types, function prototypes and data declarations
  required for MM Inter-RAT handler.

Copyright (c) 2002,2003 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/inc/mmrat.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/01/04   ks      Added prototype for mm_handle_sm_resume_cnf() 
08/03/02   mks     Created file
08/10/02   mks     Added function prototypes for inter-RAT handover support
10/10/02   mks     Added support for GSM to WCDMA inter-RAT procedures
11/14/02   ks      Added support for Deep sleep procedure.
11/25/02   mks     Updated the function prototype mm_send_rr_stop_mode_req()
                   for fixing a compiler warning
11/26/02   jca     Added missing include and added PERFORCE header.
02/21/03   ks      Added parameter rr_mm_gsm_stop_reason_T in mm_send_rr_stop_mode_req.
02/27/03   mks     Added support for PS data continuity feature - added a new
                   enum mm_interrat_change_state_type and a new state variable 
                   mm_interrat_state.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "mmcp_variation.h"
#include "mm_v.h"
#include "rrcmmif.h"


/*===========================================================================

                            MACROS

===========================================================================*/


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*
** Inter-RAT status codes
*/
typedef enum
{
  MM_RAT_LO,
  MM_RAT_INITIAL,
  MM_RAT_STOP_GSM_IN_PROGRESS,
  MM_RAT_STOP_WCDMA_IN_PROGRESS,
  MM_RAT_STOP_TDSCDMA_IN_PROGRESS,
  MM_RAT_ABORT_IN_PROGRESS,
  MM_RAT_ABORT_PENDING,
  MM_RAT_STOP_GSM_PENDING_ABORT,
  MM_RAT_STOP_WCDMA_PENDING_ABORT,
  MM_RAT_HI
} mm_rat_handler_status_type;

/*
** Inter-RAT status codes
*/
typedef enum
{
  MM_INTERRAT_CHANGE_STATE_TYPE_LO,
  MM_UMTS_TO_GSM_INTERRAT_CHANGE_IN_PROGRESS,
  MM_GSM_TO_UMTS_INTERRAT_CHANGE_IN_PROGRESS,
#ifdef FEATURE_LTE
  MM_LTE_TO_UMTS_INTERRAT_CHANGE_IN_PROGRESS,
  MM_LTE_TO_GSM_INTERRAT_CHANGE_IN_PROGRESS,
#endif
  MM_INTERRAT_CHANGE_STATE_TYPE_HI
} mm_interrat_change_state_type;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

extern mm_interrat_change_state_type    mm_interrat_state_sim[MAX_NAS_STACKS] ;
#define  mm_interrat_state mm_interrat_state_sim[mm_as_id]
#define  mm_rat_handler_status (mm_rat_handler_status_sim[mm_as_id])
extern mm_rat_handler_status_type  mm_rat_handler_status_sim[MAX_NAS_STACKS];
#else
extern mm_interrat_change_state_type    mm_interrat_state ;
extern mm_rat_handler_status_type  mm_rat_handler_status;
#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/

/*
** Handler for inter-RAT transitions
*/
void mm_handle_inter_rat_change (mm_cmd_type *message);

/*
** Handler for ACTIVATION_IND primitive from RR/RRC
*/
void mm_handle_activation_ind (mm_cmd_type *message);

/*
** Handler for stop mode confirmation event from RR/RRC
*/
void mm_handle_stop_mode_cnf (mm_cmd_type *message);

/*
** Sends Activation response to RR
*/
void mm_send_rr_activation_rsp (sys_radio_access_tech_e_type last_active_rat);

/*
** Sends Activation response to RRC
*/
void mm_send_rrc_activation_rsp
(
  sys_radio_access_tech_e_type last_active_rat,
  sys_radio_access_tech_e_type rat
);

void mm_send_activation_rsp_if_needed (void);


/*
** Sends STOP mode request to RR
*/
/* Note: Deep sleep or Mode change reason in GSM is not supported yet */
void mm_send_rr_stop_mode_req (rr_mm_gsm_stop_reason_T);

/*
** Sends STOP mode request to RRC
*/
void mm_send_rrc_stop_mode_req
(
  rrc_stop_cause_e_type        reason,
  sys_radio_access_tech_e_type rat
);

/*
** This function sends activation response to RR/RRC when there is
** an input message on MM queue and MM is waiting for stop mode
** response from RR/RRC
*/
void mm_rat_handle_new_msg (mm_cmd_type *message);

/* This function is called whenever the GMMSM_RAT_RESUME_CNF is received */
void mm_handle_sm_resume_cnf(void);

void mmcoord_process_mmr_stop_mode_req( mm_cmd_type *message );

#endif /* #ifndef MMRAT_H */
