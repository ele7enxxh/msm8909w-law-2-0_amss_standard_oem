/*===========================================================================
                       COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

                       EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnmglobal.c_v   1.7   22 Apr 2002 10:24:12   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnmglobal.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
23-Mar-2001  VT      Created

23-May-2001  VT      Removed some unwanted include(s)

05-Jun-2001  VT      Added q_type mn_cm_cmd_q (as it is extern now in mn_cc.h)

08/06/01     CD      Rearranged header files

08/14/01     AB      Renamed connection manager references to CNM from CM.

27-Sep-2001  VT      Added the declaration for the following global data (after
                       making it extern in cnm_init_data.c) :
                          rex_timer_type mn_cnm_sleep_timer;

04/19/02     CD      Deleted mm_connections_active definition and added 
                     mm_rel_req_sent flag

07/13/05     HS      CNM_mt_cp_message and CNM_mt_cp_data_retransmit_counter
                     exanded to save CP data for all call ids

07/14/06     NR      Adding changes for SMS optimization CR 93099 
===========================================================================*/
/*===========================================================================
                          MN_CNM Interface Library Functions

DESCRIPTION

 This module contains the MN_CNM global variables and structures. 
 
   
EXTERNALIZED FUNCTIONS

  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "queue.h"
#include "task.h"
#include "environ.h"
#include "mn.h"
#include "cmlib.h"
#include "cnm.h"
#include "cm_sms.h"


q_type                    mn_cnm_cmd_q;

/* Internal high priority CMD Q for MN_CNM task */
q_type                    mn_cnm_internal_cmd_q;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)

#ifndef FEATURE_MODEM_HEAP
cc_transaction_information_T   
                          cc_transaction_information_sim[MAX_AS_IDS][MAXNO_CC_TRANSACTIONS];
#endif
cc_transaction_information_T *  
                          cc_transaction_information_p_sim[MAX_AS_IDS][MAXNO_CC_TRANSACTIONS];
sms_transaction_information_T   
                       sms_transaction_information_sim[MAX_AS_IDS][MAXNO_SMS_TRANSACTIONS];
ss_transaction_information_T   
                       ss_transaction_information_sim[MAX_AS_IDS][MAXNO_SS_TRANSACTIONS];

cnm_request_id_T          mm_connection_pending_sim[MAX_AS_IDS];
boolean                   mm_link_reestablishing_sim[MAX_AS_IDS];
boolean                   mm_rel_req_sent_sim[MAX_AS_IDS];

cc_state_T                last_state_sim[MAX_AS_IDS]; 
byte                      cc_progress_information_sim[MAX_AS_IDS][MAXNO_CC_TRANSACTIONS];
timer_store_T             timer_store_sim[MAX_AS_IDS][MAX_CNM_TIMER];
byte                      timer_store_pointer_sim[MAX_AS_IDS];
srvcc_transient_triggers_T  buffer_transient_triggers_sim[MAX_AS_IDS][MAX_TRANSIENT_TRIGGERS];

/* TCH assignment */
mmcc_sync_ind_T           CNM_mmcc_sync_ind_sim[MAX_AS_IDS] = {{{0, 0, 0, 0}, 0, 0, 0, 0, (byte)WCDMA_RAB_RELEASED},
                                                               {{0, 0, 0, 0}, 0, 0, 0, 0, (byte)WCDMA_RAB_RELEASED}};
 

mmcnm_data_req_T         CNM_mo_cp_message_sim[MAX_AS_IDS];
#ifndef FEATURE_MODEM_HEAP
mmcnm_data_req_T         CNM_mt_cp_message_sim[MAX_AS_IDS][MAXNO_SMS_TRANSACTIONS-8]; /* only for MT */
#else
mmcnm_data_req_T        * CNM_mt_cp_message_sim[MAX_AS_IDS][MAXNO_SMS_TRANSACTIONS-8]; /* only for MT */
#endif

byte                     CNM_mo_cp_data_retransmit_counter_sim[MAX_AS_IDS];
byte                     CNM_mt_cp_data_retransmit_counter_sim[MAX_AS_IDS][MAXNO_SMS_TRANSACTIONS-8]; /* only for MT */
CNM_cp_user_data_T       CNM_mo_cp_user_data_sim[MAX_AS_IDS];
byte                     cp_cause_value_sim[MAX_AS_IDS];
boolean cp_data_l2_ack_sim[MAX_AS_IDS];

#else

#ifndef FEATURE_MODEM_HEAP
cc_transaction_information_T   
                          cc_transaction_information[MAXNO_CC_TRANSACTIONS];
#endif
cc_transaction_information_T *  
                          cc_transaction_information_p[MAXNO_CC_TRANSACTIONS];
sms_transaction_information_T   
                       sms_transaction_information[MAXNO_SMS_TRANSACTIONS];
ss_transaction_information_T   
                       ss_transaction_information[MAXNO_SS_TRANSACTIONS];
cnm_request_id_T          mm_connection_pending;

boolean                   mm_link_reestablishing;

/* ** mm_rel_req_sent ** 
 * This variable flags whether CNM waits for MMCNM_REL_CNF in response to CNM's MMCNM_REL_REQ message to MM.
 * Whenever CNM sends MMCNM_REL_REQ, CNM does following as well
 * starts timer CNM_MM_REL_TIMER
 * resets mn_as_id to NONE in DSDS.
 * */
boolean                   mm_rel_req_sent;

/* 
 * state prior to processing a msg 
 */

cc_state_T                last_state; 

byte                      cc_progress_information[MAXNO_CC_TRANSACTIONS];

timer_store_T             timer_store[MAX_CNM_TIMER];
byte                      timer_store_pointer;

/*buffer_transient_triggers**
 * After SRVCC handover, NAS will do some transient procedure like hold req, CONNECT etc based on the handover_complete_ind and context response*
 * NAS will do the transient procedure only after receiving, handover_complete_ind with trigger=0(means, MM Rx_es LTE_deact_cnf) even if it receives the call context response
 */

srvcc_transient_triggers_T    buffer_transient_triggers[MAX_TRANSIENT_TRIGGERS];


/* TCH assignment */
mmcc_sync_ind_T           CNM_mmcc_sync_ind = {{0, 0, 0, 0}, 0, 0, 0, 0, (byte)WCDMA_RAB_RELEASED};
 

mmcnm_data_req_T         CNM_mo_cp_message;
#ifndef FEATURE_MODEM_HEAP
mmcnm_data_req_T         CNM_mt_cp_message[MAXNO_SMS_TRANSACTIONS-8]; /* only for MT */
#else
mmcnm_data_req_T        * CNM_mt_cp_message[MAXNO_SMS_TRANSACTIONS-8]; /* only for MT */
#endif

byte                     CNM_mo_cp_data_retransmit_counter;
byte                     CNM_mt_cp_data_retransmit_counter[MAXNO_SMS_TRANSACTIONS-8]; /* only for MT */
CNM_cp_user_data_T       CNM_mo_cp_user_data;
byte                     cp_cause_value;




boolean cp_data_l2_ack;






#endif



















