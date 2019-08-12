#ifndef cc_cc_h
#define cc_cc_h
/*===========================================================================
           COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/cc_cc.h_v   1.2   19 Mar 2001 17:42:52   tschwarz  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/inc/cc_cc.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/05   HS      Added support for FEATURE_CCBS


===========================================================================*/


/*
 * Local  Defines
 * --------------
 */

/* 
 * Message Types for cc to cc peer messages - These are as defined in
 * GSM 04.08
 */

#define ALERTING           0x01
#define CALL_CONFIRMED     0x08
#define CALL_PROCEEDING    0x02
#define CONNECT            0x07
#define CONNECT_ACK        0x0f
#define EMERGENCY_SETUP    0x0e
#define PROGRESS           0x03
#define SETUP              0x05

#define MODIFY             0x17
#define MODIFY_COMPLETE    0x1f
#define MODIFY_REJECT      0x13
#define USER_INFORMATION   0x10
#define HOLD               0x18
#define HOLD_ACK           0x19
#define HOLD_REJECT        0x1a
#define RETRIEVE           0x1c
#define RETRIEVE_ACK       0x1d
#define RETRIEVE_REJ       0x1e

#define DISCONNECT         0x25
#define RELEASE            0x2d
#define RELEASE_COMPLETE   0x2a

#define CONGESTION_CONTROL 0x39
#define NOTIFY             0x3e
#define STATUS_MSG         0x3d
#define STATUS_ENQUIRY     0x34
#define START_DTMF         0x35
#define STOP_DTMF          0x31
#define STOP_DTMF_ACK      0x32
#define START_DTMF_ACK     0x36
#define START_DTMF_REJ     0x37
#define FACILITY           0x3a
#define REGISTER           0x3b

#include "mmcp_variation.h"
#ifdef FEATURE_CCBS
#define CC_ESTABLISHMENT   0x04
#define CC_ESTABLISHMENT_CONFIRMED   0x06
#define RECALL             0x0b
#define START_CC           0x09
#endif /* FEATURE_CCBS */

/*
 * SMS Messages
 */

#define CP_DATA            0x01
#define CP_ACK             0x04
#define CP_ERROR           0x10

/*
 * General type definitions:
 *
 */

/*
 * Definitions of structures of messages
 */

/*********************************************************************
 * ALERTING MESSAGE
 * (Size limited to maximum allowed)
 *
 */

typedef struct alerting_msg_tag
{
   msg_head_T     msg_head;
   byte           data[ MAX_OCTETS_L3_MESSAGE - 2 ];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif

} alerting_msg_T;

/********************************************************************
 * CALL CONFIRMED MESSAGE
 */

typedef struct call_confirmed_msg_tag
{
   msg_head_T     msg_head;
   byte           data[ MAX_OCTETS_L3_MESSAGE - 2 ];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif

} call_confirmed_msg_T;

/********************************************************************
 *
 * CALL PROCEEDING MESSAGE
 */

typedef struct call_proceeding_msg_tag
{
   msg_head_T     msg_head;
   byte           data[ MAX_OCTETS_L3_MESSAGE - 2 ];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif

} call_proceeding_msg_T;


/********************************************************************
 * 
 * CONNECT MESSAGE
 */

typedef struct connect_msg_tag
{
   msg_head_T     msg_head;
   byte           data[ MAX_OCTETS_L3_MESSAGE - 2 ];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} connect_msg_T;


/********************************************************************
 * 
 * CONNECT ACK MESSAGE
 */

typedef struct connect_ack_msg_tag
{
   msg_head_T     msg_head;
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} connect_ack_msg_T;



/********************************************************************
 * 
 * EMERGENCY SETUP MESSAGE
 */

typedef struct emergency_setup_msg_tag
{
   msg_head_T     msg_head;
   byte           data[ MAX_OCTETS_L3_MESSAGE - 2 ];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} emergency_setup_msg_T;


/********************************************************************
 * 
 * PROGRESS MESSAGE
 */

typedef struct progress_msg_tag
{
   msg_head_T                 msg_head;
   byte           data[ MAX_OCTETS_L3_MESSAGE - 2 ];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} progress_msg_T;


/********************************************************************
 * 
 * SETUP MESSAGE - from TS GSM 04.08 section 9.3.23
 */

typedef struct setup_msg_tag
{
   msg_head_T                 msg_head;
   byte                       data[ MAX_OCTETS_L3_MESSAGE - 2];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} setup_msg_T;

/********************************************************************
 * 
 * REGISTER MESSAGE - Used for supplementary services
 */

typedef struct register_msg_tag
{
   msg_head_T                 msg_head;
   byte                       data[ MAX_OCTETS_L3_MESSAGE - 2];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif

} register_msg_T;

/********************************************************************
 * 
 * MODIFY MESSAGE
 */

typedef struct modify_msg_tag
{
   msg_head_T                 msg_head;
   byte                       data[ MAX_OCTETS_L3_MESSAGE - 2 ];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif

} modify_msg_T;


/********************************************************************
 * 
 * MODIFY COMPLETE MESSAGE
 */

typedef struct modify_complete_msg_tag
{
   msg_head_T                 msg_head;
   byte                       data[ MAX_OCTETS_L3_MESSAGE - 2 ];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif

}modify_complete_msg_T;


/********************************************************************
 * 
 * MODIFY REJECT MESSAGE
 */

typedef struct modify_reject_msg_tag
{
   msg_head_T                 msg_head;
   byte                       data[ MAX_OCTETS_L3_MESSAGE - 2 ];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif

}modify_reject_msg_T;


/********************************************************************
 * 
 * USER INFORMATION MESSAGE
 */

typedef struct user_information_msg_tag
{
   msg_head_T              msg_head;
   byte                       data[ MAX_OCTETS_L3_MESSAGE - 2 ];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} user_information_msg_T;


/********************************************************************
 * 
 * HOLD MESSAGE
 */

typedef struct hold_msg_tag
{
   msg_head_T              msg_head;
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} hold_msg_T;


/********************************************************************
 * 
 * HOLD ACK MESSAGE
 */

typedef struct hold_ack_msg_tag
{
   msg_head_T              msg_head;
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} hold_ack_msg_T;


/********************************************************************
 * 
 * HOLD REJECT MESSAGE
 */

typedef struct hold_reject_msg_tag
{
   msg_head_T              msg_head;
   byte                    data[ MAX_OCTETS_L3_MESSAGE - 2 ];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} hold_reject_msg_T;


/********************************************************************
 * 
 * RETRIEVE MESSAGE
 */

typedef struct retrieve_msg_tag
{
   msg_head_T              msg_head;
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} retrieve_msg_T;


/********************************************************************
 * 
 * RETRIEVE ACK MESSAGE
 */

typedef struct retrieve_ack_msg_tag
{
   msg_head_T              msg_head;
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} retrieve_ack_msg_T;


/********************************************************************
 * 
 * RETRIEVE REJECT MESSAGE
 */

typedef struct retrieve_reject_msg_tag
{
   msg_head_T              msg_head;
   byte                    data[ MAX_OCTETS_L3_MESSAGE - 2 ];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} retrieve_reject_msg_T;


/********************************************************************
 * 
 * DISCONNECT MESSAGE
 */

typedef struct disconnect_msg_tag
{
   msg_head_T           msg_head;
   byte                 data[ MAX_OCTETS_L3_MESSAGE - 2];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} disconnect_msg_T;


/********************************************************************
 * 
 * RELEASE MESSAGE
 */

typedef struct release_msg_tag
{
   msg_head_T           msg_head;
   byte                 data[ MAX_OCTETS_L3_MESSAGE - 2];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} release_msg_T;

/********************************************************************
 * 
 * RELEASE COMPLETE MESSAGE
 */

typedef struct release_complete_msg_tag
{
   msg_head_T           msg_head;
   byte                 data[ MAX_OCTETS_L3_MESSAGE - 2];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} release_complete_msg_T;


/********************************************************************
 * 
 * CONGESTION CONTROL MESSAGE
 */

typedef struct congestion_control_msg_tag
{
   msg_head_T              msg_head;
   byte                    data[ MAX_OCTETS_L3_MESSAGE - 2 ];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} congestion_control_msg_T;


/********************************************************************
 * 
 * NOTIFY MESSAGE
 */

typedef struct notify_msg_tag
{
   msg_head_T              msg_head;

   /*
    * TO BE COMPLETED - SS MESSAGE
    *
    * WARNING - NO IEI in notification indicator IE
    *
    */
#ifdef FEATURE_DUAL_SIM
   byte                    as_id;
#endif

} notify_msg_T;


/********************************************************************
 * 
 * STATUS MESSAGE
 */

typedef struct status_msg_tag
{
   msg_head_T              msg_head;
   byte                    data[ MAX_OCTETS_L3_MESSAGE - 2 ];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} status_msg_T;



/********************************************************************
 * 
 * STATUS ENQUIRY MESSAGE
 */

typedef struct status_enquiry_msg_tag
{
   msg_head_T              msg_head;
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} status_enquiry_msg_T;


/********************************************************************
 * 
 * START DTMF MESSAGE
 */

typedef struct start_dtmf_msg_tag
{
   msg_head_T              msg_head;
   ie_keypad_facility_T    keypad_facility;
#ifdef FEATURE_DUAL_SIM
   byte                    as_id;
#endif
} start_dtmf_msg_T;


/********************************************************************
 * 
 * START DTMF ACK MESSAGE
 */

typedef struct start_dtmf_ack_msg_tag
{
   msg_head_T              msg_head;
   ie_keypad_facility_T    keypad_facility;
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} start_dtmf_ack_msg_T;

/********************************************************************
 * 
 * START DTMF REJ MESSAGE
 */

typedef struct start_dtmf_rej_msg_tag
{
   msg_head_T              msg_head;
   byte                    cause[ MAX_OCTETS_L3_MESSAGE - 2 ];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} start_dtmf_rej_msg_T;

/********************************************************************
 * 
 * STOP DTMF MESSAGE
 */

typedef struct stop_dtmf_msg_tag
{
   msg_head_T              msg_head;
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} stop_dtmf_msg_T;


/********************************************************************
 * 
 * STOP DTMF ACK MESSAGE
 */

typedef struct stop_dtmf_ack_msg_tag
{
   msg_head_T              msg_head;
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} stop_dtmf_ack_msg_T;

/********************************************************************
 * 
 * FACILITY MESSAGE
 */

typedef struct facility_msg_tag
{
   msg_head_T              msg_head;
   byte                    data[ MAX_OCTETS_L3_MESSAGE - 2];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} facility_msg_T;

/********************************************************************
 * 
 * CP_DATA MESSAGE
 */

typedef struct cp_data_msg_tag
{
  msg_head_T               msg_head;
  byte                     data[MAX_LENGTH_SMS_DATA - 2];
#ifdef FEATURE_DUAL_SIM
  byte           as_id;
#endif
} cp_data_msg_T;


/********************************************************************
 * 
 * CP_ACK MESSAGE
 */

typedef struct cp_ack_msg_tag
{
  msg_head_T               msg_head;
#ifdef FEATURE_DUAL_SIM
  byte           as_id;
#endif
} cp_ack_msg_T;


/********************************************************************
 * 
 * CP_ERROR MESSAGE
 */

typedef struct cp_error_msg_tag
{
  msg_head_T               msg_head;
  byte                     cp_cause;
#ifdef FEATURE_DUAL_SIM
  byte           as_id;
#endif
} cp_error_msg_T;

#ifdef FEATURE_CCBS
/*********************************************************************
 *
 * CC_ESTABLISHMENT MESSAGE
 */

typedef struct cc_establishment_msg_tag
{
   msg_head_T     msg_head;
   byte           data[ MAX_OCTETS_L3_MESSAGE - 2 ];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif

} cc_establishment_msg_T;

/*********************************************************************
 *
 * CC_ESTABLISHMENT_CONFIRMED MESSAGE
 */

typedef struct cc_establishment_confirmed_msg_tag
{
   msg_head_T     msg_head;
   byte           data[ 53 ]; /* data length =
                               *  Repeat Indicator     1 +
                               *  Bearer capability 1 10 +
                               *  Bearer capability 2  10 +
                               *  Cause               32 */
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} cc_establishment_confirmed_msg_T;


/*********************************************************************
 *
 * RECALL MESSAGE
 */

typedef struct recall_msg_tag
{
   msg_head_T     msg_head;
   byte           data[ MAX_OCTETS_L3_MESSAGE - 2 ];
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif

} recall_msg_T;

/*********************************************************************
 *
 * START_CC MESSAGE
 */

typedef struct start_cc_msg_tag
{
   msg_head_T     msg_head;
   byte           data[ 4 ]; /* data length =
                              *  CC Capabilities 4 */
#ifdef FEATURE_DUAL_SIM
   byte           as_id;
#endif
} start_cc_msg_T;
#endif /* FEATURE_CCBS */

#endif


