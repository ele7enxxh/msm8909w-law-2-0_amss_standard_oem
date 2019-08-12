#ifndef mn_sms_h
#define mn_sms_h

/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

  EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_sms.h_v   1.11   17 Jun 2002 17:16:10   rgurumoo  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/mn_sms.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
5/21/01     CD     Commented out #defs for IEI_CP_USER_DATA and IEI_CP_CAUSE
                   as they were already defined in cc_iei.h

7/12/01     CD     Transferred data from mn.h

7/18/01     CD     Transferred external declaration of sm_rl_transaction_information
                   from mn_process_cnm_sms_msgs.c

                   Transferred external declaration of mo_rpdu_store from 
                   mn_process_smrl_msgs.c

                   Transferred external declaration of rl_mo_mr from mn_put_cm_sms_msgs.c

                   Removed definition of tp_mo_mr as is no longer needed in MN (see UASMS)

08/14/01    AB     Renamed connection manager references to CNM from CM.

08/16/01    AB     Renamed MMI references to CM.

08/21/01    CD     Moved external data declarations to mnglobal.h

9/27/01     CD     Deleted tp_message_ref from sm_rl_transaction_information_T
                   Deleted constant SM_RESOURCES_NOT_AVAILABLE

9/28/01     CD     Added definition for RP_User_data_IEI

10/04/01    CD     Added s_sms_to_idle stimulus for state machine 

1/29/02     RG     Changed the short_message_identifier parameter in the 
                   sm_rl_transaction_information_T to be a word instead of a byte. 
                   This is part of the changes required to support the UASMS to 
                   correlate the memory availability requests with the corresponding 
                   responses.

2/10/02    AB      Added the prefered domain flag, pref_rl_domain, in the 
                   sms_transaction_information_T  to differentiated the prefer 
                   radio link DOMAIN (CS or PS) where the SMS transactions 
                   will be transmitted.
                   
08/12/04   AB      Added support for SMS link control feature.

12/10/04   AB      Added a cp_ack_deferred flag for ATCOP CMMS with 
                       Multiple MO SMS.
04/09/07   RD      SMS optimization changes are incorporated
                       

===========================================================================*/

#include "mn_uasms.h"


/************************************
 * Global data for MN_SMS interface *
 ************************************/



typedef struct
{
   byte        rl_state;
   boolean     smma_transaction;
   boolean     retrans_flag;
   boolean     mtf_flag;
   byte        rl_message_ref;
   // Changed this from byte to word to be consistent
   // with the introduction of the "word short_message_identifier"
   // in the SM_RL_MEMORY_AVAILABLE_REQ_T.
   word        short_message_identifier;
   sys_modem_as_id_e_type     as_id;

}sm_rl_transaction_information_T;






/**************
 *  Constants *
 **************/


/* RP message headers */

#define  RP_DATA_MS   0
#define  RP_DATA_N    1
#define  RP_ACK_MS    2
#define  RP_ACK_N     3
#define  RP_ERROR_MS  4
#define  RP_ERROR_N   5
#define  RP_SMMA_MS   6




/* SM RL states  */

#define   IDLE     0
/* this is internal state where wms will be waiting for cp_ack for rp_data sent
for MT SMS */

/***********************
 * SMR data structures *
 ***********************/




/* return_data from MN_control_SMS_RL */

typedef    struct
{
   boolean     success;
   boolean     rel_req;
   boolean     sm_rl_report;
}control_sm_rl_return_data_T;



/************************************************************************
 *                                                                      *
 *                       MNSMS Message Identity values                  *
 *                                                                      *
 ************************************************************************
 * To be loaded into MessageId field of Message Header
 */




typedef struct rp_cause_tag
{
   byte        Iei;
   byte        Length;
   byte        cause_value;
   byte        diagnostics;
} rp_cause_T;

typedef struct cp_cause_tag
{
   byte        Iei;
   byte        cause_value;
} cp_cause_T;

typedef struct rp_data_mo_tag
{
   byte        message_type;
   byte        message_reference;
   byte        data[252];
} rp_data_mo_T;

typedef struct rp_data_mt_tag
{
   byte        rp_priority_indicator;
   byte        message_reference;
   byte        data[252];
} rp_data_mt_T;

typedef struct rp_smma_tag
{
   byte        message_type;
   byte        message_reference;
} rp_smma_T;

typedef struct rp_ack_tag
{
   byte        message_type;
   byte        message_reference;
} rp_ack_T;

typedef struct rp_error_tag
{
   byte        message_type;
   byte        message_reference;
   rp_cause_T  rp_cause;
   byte        rp_user_data[240];
} rp_error_T;

typedef union rpdu_tag
{
   rp_data_mo_T      rp_data_mo;
   rp_data_mt_T      rp_data_mt;
   rp_smma_T         rp_smma;
   rp_ack_T          rp_ack;
   rp_error_T        rp_error;
   byte              data[255];

} rpdu_T;

/************************************************************************
 *                                                                      *
 *                       MN to SMS PRIMITIVES                            *
 *                       -------------------                            *
 *                                                                      *
 ************************************************************************
 */

/************************************************************************
 *                                                                      *
 *                       MNSMS_EST_REQ                                  *
 *                                                                      *
 ************************************************************************
 */

typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type   as_id;
   connection_id_T    connection_id;
   byte               rpdu_len;
   rpdu_T             rpdu;
} MNSMS_EST_REQ_T;

/************************************************************************
 *                                                                      *
 *                       MNSMS_DATA_REQ                                 *
 *                                                                      *
 ************************************************************************
 */

typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type   as_id;
   connection_id_T    connection_id;
   byte               rpdu_len;
   rpdu_T             rpdu;
} MNSMS_DATA_REQ_T;

/************************************************************************
 *                                                                      *
 *                       MNSMS_ABORT_REQ                                *
 *                                                                      *
 ************************************************************************
 */

typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type   as_id;
   connection_id_T    connection_id;
   cp_cause_T         cp_cause;
} MNSMS_ABORT_REQ_T;



/************************************************************************
 *                                                                      *
 *                       MNSMS_REL_REQ                                  *
 *                                                                      *
 ************************************************************************
 */

typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type   as_id;
   connection_id_T    connection_id;
   cp_cause_T         cp_cause;
} MNSMS_REL_REQ_T;



/************************************************************************
 *                                                                      *
 *                       SMS to MN PRIMITIVES                           *
 *                       -------------------                            *
 *                                                                      *
 ************************************************************************
*/

/************************************************************************
 *                                                                      *
 *                       MNSMS_DATA_IND                                 *
 *                                                                      *
 ************************************************************************
 */

typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type   as_id;
   connection_id_T    connection_id;
   rpdu_T             rpdu;
} MNSMS_DATA_IND_T;

/************************************************************************
 *                                                                      *
 *                       MNSMS_EST_IND                                  *
 *                                                                      *
 ************************************************************************
 */

typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type   as_id;
   connection_id_T    connection_id;
   nas_msg_transport_type_e_type    sms_transport_type;
   rpdu_T             rpdu;
} MNSMS_EST_IND_T;

/************************************************************************
 *                                                                      *
 *                       MNSMS_ERROR_IND                                *
 *                                                                      *
 ************************************************************************
 */

typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type   as_id;
   connection_id_T    connection_id;
   cp_cause_T         cp_cause;
   byte               lower_layer_cause;
   word               lower_layer_cause_value;
   boolean           permanent_error;
   mmcc_domain_name_T   domain;
   sms_ims_retry_interval_type_T  retry_interval;  

} MNSMS_ERROR_IND_T;

/************************************************************************
 *                                                                      *
 *                     SMS Link Control                                 *
 *                                                                      *
 ************************************************************************
 */

#endif


