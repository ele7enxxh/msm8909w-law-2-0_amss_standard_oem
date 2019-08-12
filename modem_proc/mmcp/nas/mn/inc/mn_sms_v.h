#ifndef mn_sms_v_h
#define mn_sms_v_h

/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

  EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_sms.h_v   1.11   17 Jun 2002 17:16:10   rgurumoo  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/inc/mn_sms_v.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

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

#include "mn_sms.h"
#include "mn_uasms_v.h"


/************************************
 * Global data for MN_SMS interface *
 ************************************/

typedef  byte    sms_state_T;

typedef struct sms_transaction_information_tag
{
   sms_state_T                sms_state;
   boolean                    release_pending;
   byte                       pref_rl_domain;  /* prefered RL for CS/PS domain flag */
} sms_transaction_information_T;




/* Store of MO relay data */

#define  RPDU_SIZE   255

typedef struct {
   byte            rpdu[RPDU_SIZE];
   byte            rpdu_length;
   connection_id_T connection_id;
} mo_rpdu_store_T;

typedef byte rl_mo_mr_T;


/**************
 *  Constants *
 **************/

/* RP IEIs */
#define RP_USER_DATA_IEI    0x41

/* SM RL states  */

#define   WAIT_FOR_RP_ACK   1
#define   WAIT_TO_SEND_RP_ACK    2
#define   WAIT_FOR_RETRANS_TIMER   4
#define   WAIT_FOR_CP_ACK 5
/* this is internal state where wms will be waiting for cp_ack for rp_data sent
for MT SMS */

/***********************
 * SMR data structures *
 ***********************/

/* SM RL control stimulae */

enum
{
   s_mo_sms_req,
   s_mt_sms_request,
   s_sms_ack,
   s_sms_per_error,
   s_cnm_error,
   s_sms_temp_error,
   sms_TRAM_timer_expiry,
   sms_TR1M_timer_expiry,
   sms_TR2M_timer_expiry,
   s_mo_smma_req,
   s_mo_abort_req,
   s_report_req,
   s_mo_smma_abort_req,
   s_sms_to_idle,
   s_cnm_ack
};

/* return_data from MN_control_SMS_RL */




/************************************************************************
 *                                                                      *
 *                       MNSMS Message Identity values                  *
 *                                                                      *
 ************************************************************************
 * To be loaded into MessageId field of Message Header
 */

enum 
{

/* MN to SMS primitives */

   MNSMS_EST_REQ          = 0x00,
   MNSMS_DATA_REQ,
   MNSMS_ABORT_REQ,
   MNSMS_REL_REQ,

/* SMS to MN Primitives */

   MNSMS_EST_IND       = 0x80,
   MNSMS_DATA_IND,
   MNSMS_ERROR_IND,
   MNSMS_CPACK_IND
};

/*
 * Definitions of CP cause values
 */

#define CP_CAUSE_NETWORK_FAILURE                         17
#define CP_CAUSE_CONGESTION                              22
#define CP_CAUSE_INVALID_TI                              81
#define CP_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE          95
#define CP_CAUSE_INVALID_MANDATORY_INFORMATION           96
#define CP_CAUSE_MSG_TYPE_NON_EXIST_OR_NOT_IMPLEMENTED   97
#define CP_CAUSE_MSG_NOT_COMPATIBLE_WITH_SMS_STATE       98
#define CP_CAUSE_IE_NON_EXIST_OR_NOT_IMPLEMENTED         99
#define CP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED              111

/*
 * Definition of SMS IEIs
 */
/*  these are defined in cc_iei.h as part of the cnm code
#define IEI_CP_USER_DATA   0x01
#define IEI_CP_CAUSE       0x02
*/





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



/************************************************************************
 *                                                                      *
 *                       MNSMS_DATA_REQ                                 *
 *                                                                      *
 ************************************************************************
 */


/************************************************************************
 *                                                                      *
 *                       MNSMS_ABORT_REQ                                *
 *                                                                      *
 ************************************************************************
 */




/************************************************************************
 *                                                                      *
 *                       MNSMS_REL_REQ                                  *
 *                                                                      *
 ************************************************************************
 */




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


/************************************************************************
 *                                                                      *
 *                       MNSMS_EST_IND                                  *
 *                                                                      *
 ************************************************************************
 */


/************************************************************************
 *                                                                      *
 *                       MNSMS_ERROR_IND                                *
 *                                                                      *
 ************************************************************************
 */



/************************************************************************
 *                                                                      *
 *                     SMS Link Control                                 *
 *                                                                      *
 ************************************************************************
 */

typedef struct {
   connection_id_T            connection_id;
   byte                       cause;
   byte                       duration;  /* holding duration (seconds) */
   sms_link_control_mode_T    mode;
   sms_link_control_status_T  status;
   boolean                    cp_ack_deferred;  /* for more SMS messages */
} sms_link_control_T;

typedef struct 
{
   IMH_T              message_header;
   sys_modem_as_id_e_type   as_id;
   connection_id_T    connection_id;
   byte     error_status;
   byte     cause_value;
} MNSMS_CPACK_IND_T;

#endif


