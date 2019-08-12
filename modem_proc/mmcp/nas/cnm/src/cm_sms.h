#ifndef cm_sms_h
#define cm_sms_h

/*===========================================================================
           COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cm_sms.h_v   1.7   22 Apr 2002 09:47:12   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cm_sms.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/14/01   AB      Renamed connection manager references to CNM from CM.

09/17/01   CD      Added MMSMS_REST_CNF_event, MMSMS_ERROR_IND_event
                   corrected ifndef preprocessor

10/01/01   CD      Deleted MMSMS_ERROR_IND_event.  MMCNM_ERR_IND_event is used instead

04/19/02   CD      Replaced MMSMS_REEST_CNF_event by MMSMS_REEST_REJ_event
                   Added MMSMS_EST_REJ_event

                   Updated Copyright

08/01/02   CD      Removed MMCNM_ERR_IND_event

12/3/02    RG      Changed CP_DATA_RETRANSMIT_COUNT to 1.

07/13/05   HS      CNM_mt_cp_message and CNM_mt_cp_data_retransmit_counter
                   exanded to save CP data for all call ids

11/30/11   PM      Providing usage comment for an SMS event S_CP_ACK_FALSE_event
                   as part of CR 308048.
===========================================================================*/

#include "mmcp_variation.h"



/*
 * define structure for store which will contain cp_user_data details.
 * The data field contains the complete RPDU. There is no CP_USER_DATA IEI
 * nor length information present. The length of the rpdu is stored separately
 * in the length field of the structure
 */

typedef struct CNM_mo_cp_user_data_tag
{
   word        length;
   byte        data[MAX_LENGTH_SMS_DATA];
} CNM_cp_user_data_T;


#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)

extern   mmcnm_data_req_T        CNM_mo_cp_message_sim[MAX_AS_IDS];

#ifndef FEATURE_MODEM_HEAP
extern   mmcnm_data_req_T        CNM_mt_cp_message_sim[MAX_AS_IDS][MAXNO_SMS_TRANSACTIONS-8]; /* only for MT */
#else
extern   mmcnm_data_req_T       * CNM_mt_cp_message_sim[MAX_AS_IDS][MAXNO_SMS_TRANSACTIONS-8]; /* only for MT */
#endif
extern   byte                    CNM_mo_cp_data_retransmit_counter_sim[MAX_AS_IDS];
extern   byte                    CNM_mt_cp_data_retransmit_counter_sim[MAX_AS_IDS][MAXNO_SMS_TRANSACTIONS-8]; /* only for MT */
extern   CNM_cp_user_data_T      CNM_mo_cp_user_data_sim[MAX_AS_IDS];
extern   byte                    cp_cause_value_sim[MAX_AS_IDS];
extern   boolean cp_data_l2_ack_sim[MAX_AS_IDS];


#define CNM_mo_cp_message (CNM_mo_cp_message_sim[mn_dsda_as_id])
#define CNM_mt_cp_message (CNM_mt_cp_message_sim[mn_dsda_as_id])
#define CNM_mo_cp_data_retransmit_counter (CNM_mo_cp_data_retransmit_counter_sim[mn_dsda_as_id])
#define CNM_mt_cp_data_retransmit_counter (CNM_mt_cp_data_retransmit_counter_sim[mn_dsda_as_id])
#define CNM_mo_cp_user_data (CNM_mo_cp_user_data_sim[mn_dsda_as_id])
#define cp_cause_value (cp_cause_value_sim[mn_dsda_as_id])
#define cp_data_l2_ack (cp_data_l2_ack_sim[mn_dsda_as_id])


#else

extern   mmcnm_data_req_T        CNM_mo_cp_message;

#ifndef FEATURE_MODEM_HEAP
extern   mmcnm_data_req_T        CNM_mt_cp_message[MAXNO_SMS_TRANSACTIONS-8]; /* only for MT */
#else
extern   mmcnm_data_req_T       * CNM_mt_cp_message[MAXNO_SMS_TRANSACTIONS-8]; /* only for MT */
#endif
extern   byte                    CNM_mo_cp_data_retransmit_counter;
extern   byte                    CNM_mt_cp_data_retransmit_counter[MAXNO_SMS_TRANSACTIONS-8]; /* only for MT */
extern   CNM_cp_user_data_T      CNM_mo_cp_user_data;
extern   byte                    cp_cause_value;
extern   boolean cp_data_l2_ack;


#endif





/*
 * constants relating to sms
 */

#define CP_DATA_RETRANSMIT_COUNT       1

/*
 * Definitions of constants used to identify the sms trigger events
 */

/***************************USAGE *******************************
 *
 *                             S_CP_ACK_FALSE_event 
 *
 * This event is triggered when UE receives CP-ERROR in MT SMS transaction or
 * Network hasn't responded to UE's CP-DATA and timer in UE has expired.
 * 
 ****************************************************************/

#define NO_ACTION_event                     0
/*#define MMCNM_ERR_IND_event                          1*/
#define MMSMS_EST_CNF_event                 2
#define MMSMS_REL_IND_event                 3
#define MNSMS_EST_CNF_event                 4
#define MNSMS_EST_REQ_event                 5
#define SMS_TIMER_EXPIRY_event              6
#define C_ABORT_SMS_event                   7
#define S_CP_ACK_FALSE_event                8
#define S_CP_ACK_TRUE_event                 9
#define S_CP_DATA_event                     10
#define S_CP_ERROR_event                    11
#define S_MT_SMS_REQUEST_event              12
#define S_RP_MESSAGE_REQ_event              13
#define T_SMS_RELEASE_event                 14
#define MMSMS_REEST_REJ_event               15
#define MMSMS_EST_REJ_event                 16
#define CP_ACK_TRUE_NEW_CP_DATA_RXD_event   17


#endif
