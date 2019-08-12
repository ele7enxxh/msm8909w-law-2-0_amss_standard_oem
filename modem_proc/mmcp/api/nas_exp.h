#ifndef nas_exp_h
#define nas_exp_h
/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/nas_exp.h_v   1.6   04 Feb 2002 11:55:02   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/nas_exp.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/08   rm      CMI defeaturisation
08/01/01   CD      Initial version
8/15/01    AB      Renamed all MMI references to CM.
08/21/01   CD      Moved the following from sys_type.h
                      cc_state_T
                      CNMServiceType_T
                      hold_auxiliary_state_T
                      mpty_auxiliary_state_T
                      channel_mode_T
                      lower_layer_cause_T
                      ss_state_T

08/21/01   CD      Moved the following items from sys_cnst.h
                    MAX_USER_USER_DATA_LENGTH
                    MAXNO_SMS_TRANSACTIONS
                    MAX_COMPONENTS_LENGTH
                    MAX_OCTETS_L3_MESSAGE
                    MAX_SS_PARAMETER_LENGTH
                    MAX_COMPONENTS
                    GSM_CAUSE_SIZE
08/23/01   TS      Moved the following items from sys_cnst.h
                    LLC_NORMAL_UNSPECIFIED
                    LLC_CONNNECTION_RELEASE
                    LLC_RADIO_LINK_FAIL
                    LLC_T3230_EXPIRY
                    LLC_AUTHENTICATION_FAILURE
                    LLC_CALL_REJECTED_BY_MM
                    LLC_NETWORK_REJECT_MM_CONN
                    LLC_CNM_SERVICE_REJECT_NORMAL
                    Moved the following items from sys_type.h
                    MAX_LENGTH_SMS_DATA

10/22/01   AB      Changed the ss_state_T data type to an enum type for 
                     data security and correctness.

11/01/01   AB      Moved cc_state_T and ss_state_T to cc_state.h.

01/25/02   CD      Moved LLC cause definitions back to sys_cnst.h
                   since CM/UI not longer needs them

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "cm_gw.h"             /* Defintions from NAS required by clients */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

typedef boolean     ReestablishmentSuccess_T;
typedef byte        TchRate_T;
typedef byte        Subchannel_T;/* value between 0 and 7 incl. */


typedef struct IMH_S
{
   byte message_set;
   byte message_id;
   byte message_len_lsb;
   byte message_len_msb;
} IMH_T;

typedef byte    connection_id_T;
typedef byte    CM_IEI_T;

typedef byte                      CNMServiceType_T;
typedef byte                      hold_auxiliary_state_T;
typedef byte                      mpty_auxiliary_state_T;
typedef byte                      channel_mode_T;
typedef byte                      lower_layer_cause_T;
typedef byte                      cc_state_T;

#define MAXNO_SMS_TRANSACTIONS                15
#define MAX_COMPONENTS_LENGTH                 220  /* derived from 24.008        */ 
#define MAX_OCTETS_L3_MESSAGE                 251  /* max size of L3 message     */
                                                   /* As defined in 04.06 sect. 5 */
#define MAX_OCTETS_L3_MESSAGE_PS_GRAT         400 /* RRC can accomodate maximum of 4096 bytes*/
#define MAX_OCTETS_L3_MESSAGE_PS_WRAT         1520 

#define MAX_LENGTH_SMS_DATA             MAX_OCTETS_L3_MESSAGE

#define MAX_SS_PARAMETER_LENGTH         260     /* for forwardingFeatureList  */
#define MAX_COMPONENTS                    4     /* Max no of components in    */
                                                /*        a facility message  */

#define GSM_CAUSE_SIZE                   32     /* size of cause IE           */

/* SMS Domain Selection 
 * This is a new data structure introduced in order for the NAS
 *  to communicate the domain on which SMS is tried, to WMS
 *  Domain name starts with 1 to facilitate an easy transition between CM and NAS
 *  CM casts this domain on to the sys_srv_domain_e_type enum in which CS domain had a value 1.
 */

typedef enum 
{
   MMCC_SMS_CS = 1,       /*SMS is tried on CS domain*/
   MMCC_SMS_PS,           /* SMS is tried on PS domain*/
   MMCC_SMS_CS_PS         /* SMS is tried on both CS and PS domain.*/
} mmcc_domain_name_T;


typedef enum
{
    NAS_VOTE_FOR_RESEL_MIN = -1,
    NAS_VOTE_FOR_RESEL_FALSE,
    NAS_VOTE_FOR_RESEL_TRUE,
    NAS_VOTE_FOR_RESEL_MAX
} nas_vote_for_reselection_e_type;


/*
 * definitions for hold_auxiliary_state_T
 */

#define HOLD_IDLE                         0
#define HOLD_REQUEST                      1
#define HOLD_CALL_HELD                    2
#define HOLD_RETRIEVE_REQUEST             3

/*
 * definitions for mpty_auxiliary_state_T
 */

#define MPTY_IDLE                         0
#define MPTY_REQUEST                      1
#define MPTY_CALL_IN_MPTY                 2
#define MPTY_SPLIT_REQUEST                3


/*
 * The numeric allocation matches the values specified in 
 * GSM 04.08 10.5.4.6 and should only be changed if the GSM 
 * spec changes. 
 */

#define CC_STATE_U0       0
#define CC_STATE_U0_1     2

#ifdef FEATURE_CCBS
#define CC_STATE_U0_2     34
#define CC_STATE_U0_3     35
#define CC_STATE_U0_4     36
#define CC_STATE_U0_5     37
#define CC_STATE_U0_6     38
#endif /* FEATURE_CCBS */
#define CC_STATE_U1       1
#define CC_STATE_U3       3
#define CC_STATE_U4       4
#define CC_STATE_U6       6
#define CC_STATE_U7       7
#define CC_STATE_U8       8
#define CC_STATE_U9       9
#define CC_STATE_U10      10
#define CC_STATE_U11      11
#define CC_STATE_U12      12
#define CC_STATE_U19      19
#define CC_STATE_U26      26
#define CC_STATE_U27      27

#endif /* nas_exp_h */

