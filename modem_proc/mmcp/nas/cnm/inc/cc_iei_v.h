#ifndef cc_iei_v_h
#define cc_iei_v_h

/*===========================================================================
           COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited..

           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cc_iei.h_v   1.6   12 Jul 2002 17:10:02   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/inc/cc_iei_v.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
25-Jul-2001  VT    Added the following elements for CS Data in the struct, 
                     ie_bearer_capability_T: Octet6d & Octet6e

01-Apr-2002  VT    GSM EFR: Added Octet3a, Octet3b, & Octet3c in 
                    ie_bearer_capability_T.
                   Actually merged the above changes from rev 1.4.1.0 (branch)of this file.

07/02/02     CD    Added ie_cause_of_no_CLI_T structure

05/10/05     HS    Added support for FEATURE_CCBS

11/08/02     AB    Added new IEI for FEATURE_REL5

===========================================================================*/
#include "cc_iei.h"

/*
 * Local  Defines
 * --------------
 */

/*
 * type 1 information elements
 */

#define  IEI_SHIFT                  0x90      /* Mask 0xf0 */
#define  IEI_LOCKING_SHIFT          0x90
#define  IEI_NON_LOCKING_SHIFT      0x91      /* bit 4 indicates non locking */

/*
 * macro to use to check equivalence to IEI_SHIFT
 */

#define IS_IEI_SHIFT(val) (( (val)&0xf0 ) == IEI_SHIFT)


#define  IEI_REPEAT_INDICATOR       0xd0       /* mask 0xf0 */

/*
 * macro to use to check equivalence to IEI_REPEAT_INDICATOR
 */

#define IS_IEI_REPEAT_INDICATOR(val) (( (val)&0xf0 ) == IEI_REPEAT_INDICATOR)

#define  IEI_CONGESTION_CONTROL     0xb0       /* mask 0xf0 */

/*
 * note: the value 0xb0 was obtained from 04.08 v4.2.0. It is missing
 * (ie not defined) in 04.08 v4.9.0. This should be checked against a
 * later spec if it exists
 */

/*
 * macro to use to check equivalence to IEI_CONGESTION_CONTROL
 */

#define IS_IEI_CONGESTION_CONTROL(val) \
                           (( (val)&0xf0 ) == IEI_CONGESTION_CONTROL)

/*
 * type 2 information elements
 */

#define  IEI_MORE_DATA              0xa0
#define  IEI_CLIR_SUPPRESSION       0xa1
#define  IEI_CLIR_INVOCATION        0xa2
#define  IEI_REVERSE_CALL_SETUP_DIR 0xa3
#define  IEI_NW_INITIATED_SVC_UPGRADE_IND     0xa4
#define  IEI_IMMEDIDATE_MOD_IND               0xa5

/*
 * type 3 and 4 information elements
 */

#define  IEI_BEARER_CAPABILITY         0x04
#define  IEI_CAUSE                     0x08
#define  IEI_FACILITY                  0x1c
#define  IEI_PROGRESS_INDICATOR        0x1e
#define  IEI_AUXILIARY_STATES          0x24
#define  IEI_KEYPAD_FACILITY           0x2c
#define  IEI_STREAM_IDENTIFIER         0x2d
#define  IEI_SERVICE_CATEGORY          0x2e 
#define  IEI_NETWORK_CC_CAPABILITY     0x2f 
#define  IEI_SIGNAL                    0x34
#define  IEI_SUPPORTED_CODECS_LIST     0x40
#define  IEI_BACKUP_BEARER_CAPABILITY  0x41
#define  IEI_CONNECTED_NUMBER          0x4c
#define  IEI_CONNECTED_SUBADDRESS      0x4d
#define  IEI_CALLING_PARTY_BCD_NO      0x5c
#define  IEI_CALLING_PARTY_SUBAD       0x5d
#define  IEI_CALLED_PARTY_BCD_NO       0x5e
#define  IEI_CALLED_PARTY_SUBAD        0x6d
#define  IEI_REDIR_PARTY_BCD_NO        0x74
#define  IEI_REDIR_PARTY_SUBADDR       0x75
#define  IEI_LOW_LAYER_COMPATIB        0x7c
#define  IEI_HIGH_LAYER_COMPATIB       0x7d
#define  IEI_USER_USER                 0x7e
#define  IEI_SS_VERSION_INDICATOR      0x7f
#define  IEI_CALL_CONTROL_CAPABIL      0x15
#define  IEI_CALL_STATE                0x14     /* Value not part of GSM spec */
#define  IEI_NOTIFICATION_IND          0x27     /* Value not part of GSM spec */
#define  IEI_CAUSE_OF_NO_CLI           0x3a
#define  IEI_ALERTING_PATTERN          0x19      /* Table 9.70 Spec 24.008, One occurence as TLV Optional IE in Setup Message*/
//#ifdef FEATURE_CCBS
#define  IEI_ALLOWED_ACTIONS        0x7b
#define  IEI_RECALL_TYPE            0x11       /* Value not part of GSM spec */
#define  IEI_SETUP_CONTAINER        0x12       /* Value not part of GSM spec */
//#endif /* FEATURE_CCBS */
/*
 * SMS related information elements
 */

#define  IEI_CP_USER_DATA           0x01
#define  IEI_CP_CAUSE               0x02

/*
 * structures of information elements:
 */

/*
 * shift IE
 */

typedef struct ie_shift_tag
{
   byte  shift_data;
} ie_shift_T;

#define  CODESET_5      0x05
#define  CODESET_6      0x06
#define  CODESET_7      0x07

#define  NON_LOCKING_SHIFT_FLAG 0x08

/***********************************************************************/
 




/* bit pattern for bearer capability values */

/* OCTET 3 bit values */

/* bits 7 and 6 */

#define BC_HALF_RATE       0x00     
#define BC_FULL_RATE       0x20
#define BC_DUAL_HALF_PREF  0x40
#define BC_DUAL_FULL_PREF  0x60

/* bit 5 always 0 */

/* bit 4 */

#define BC_CIRCUIT_MODE    0x00
#define BC_PACKET_MODE     0x80

/* bits 3,2 & 1 */

#define BC_SPEECH          0x00
#define BC_UNREST_DIGITAL  0x01
#define BC_3_1KHZ_AUDIO    0x02
#define BC_FAX_GROUP_3     0x03
#define BC_ALT_SPEECH_FAX3 0x07

/* OCTET 4 bit values */

/* bits 6 & 5 - structure */

#define BC_SERVICE_DATA_UNIT_INTEG  0x00
#define BC_UNSTRUCTURED             0x30

/* bit 4 - duplex mode */

#define BC_HALF_DUPLEX     0x00
#define BC_FULL_DUPLEX     0x08

/* bit 3 - configuration  always 0 */

/* bit 2 - NIRR */

#define BC_NO_MEANING      0x00
#define BC_TO_4_8KBS       0x02






/* 
 * definitions of progress description
 */

#define PROG_DESC_NOT_END2END_PLMN  0x01
#define PROG_DESC_DEST_NOT_PLMN     0x02
#define PROG_DESC_ORIG_NOT_PLMN     0x03
#define PROG_DESC_CALL_RET_TO_PLMN  0x04
#define PROG_DESC_INBAND_INFO_AVAIL 0x08
#define PROG_DESC_END2END_PLMN      0x20







#define SIGVAL_DIAL_TONE_ON            0x00
#define SIGVAL_RINGBACK_TONE_ON        0x01
#define SIGVAL_INTERCEPT_TONE_ON       0x02
#define SIGVAL_NET_CONGEST_TONE_ON     0x03
#define SIGVAL_BUSY_TONE_ON            0x04
#define SIGVAL_CONFIRM_TONE_ON         0x05
#define SIGVAL_ANSWER_TONE_ON          0x06
#define SIGVAL_CALL_WAITING_TONE_ON    0x07
#define SIGVAL_OFF_HOOK_WARN_TONE_ON   0x08
#define SIGVAL_TONES_OFF               0x3f
#define SIGVAL_ALERTING_OFF            0x4f




#define PRESIND_ALLOWED                0x00
#define PRESING_RESTRICTED             0x20
#define PRESING_UNAVAILABLE            0x40

#define SCRN_USER_PROV_NOT_SCREENED    0x00
#define SCRN_USER_PROV_VER_PASSED      0x01
#define SCRN_USER_PROV_VER_FAILED      0x02
#define SCRN_NETWORK_PROVIDED          0x03




#define  SUBADD_TYPE_NSAP        0x00  /* also used in callig party subad */
#define  SUBADD_TYPE_USER        0x20

#define  EVEN_NO_ADDRESS_SIGNALS 0x00
#define  ODD_NO_ADDRESS_SIGNALS  0x80

















/*
* Values of the notification indicator from TS GSM 04.08 section 10.5.4.20
*/

#define User_suspended     0x00
#define User_resumed       0x01
#define Bearer_change      0x02
#define User_alerted    0x03

/*
 * user-user ie
 */



#define  UU_PD_USER_SPECIFIED       0x00
#define  UU_PD_OSI_HI_LAYER         0x01
#define  UU_PD_X224                 0x02
#define  UU_PD_IA5_CHARS            0x04
#define  UU_PD_V_120_RATE_ADAPTION  0x07
#define  UU_PD_Q931_USER_NWK_CC     0x08




/*
 * Congestion control IE
 */

typedef struct ie_congestion_control_tag
{
   byte  IeiLevel;     /* Iei + congestion level in bottom 4 bits */
} ie_congestion_control_T;

#define  CONGESTION_RECEIVER_READY        0x00
#define  CONGESTION_RECEIVER_NOT_READY    0x0f



#define  EMERGENCY_CATEGORY_MASK          0x7f


#endif
