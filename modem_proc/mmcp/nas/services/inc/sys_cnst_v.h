#ifndef sys_cnst_v_h
#define sys_cnst_v_h
/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/sys_cnst.h_v   1.18   07 May 2002 09:07:00   cdealy  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/inc/sys_cnst_v.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/26/12   am      Back off timer code
07/30/08   rm      CMI defeaturisation
03/23/01   VT      Added MAX_RAB_ID & DEFAULT_RAB_ID
04/04/01   jca     Corrected Lint errors.
05/03/01   jca     Added PLMN_SIZE.
05/11/01   jca     Added more SIZE constants.
06/11/01   TS      Added GSM build support.
07/18/01   CD      Added MAXNO_INVOKE_IDS
08/14/01   AB      Renamed connection manager reference to CNM from CM.
08/15/01   AB      Renamed all MMI references to CM.
08/21/01   CD      Moved the following items to nas_exp.h
                    MAX_USER_USER_DATA_LENGTH
                    MAXNO_SMS_TRANSACTIONS
                    MAX_COMPONENTS_LENGTH
                    MAX_OCTETS_L3_MESSAGE
                    MAX_SS_PARAMETER_LENGTH
                    MAX_COMPONENTS
                    GSM_CAUSE_SIZE

                   Added nas_exp.h
08/23/01   TS      Moved several defines to nas_exp.h
                    LLC_NORMAL_UNSPECIFIED
                    LLC_CONNNECTION_RELEASE
                    LLC_RADIO_LINK_FAIL
                    LLC_T3230_EXPIRY
                    LLC_AUTHENTICATION_FAILURE
                    LLC_CALL_REJECTED_BY_MM
                    LLC_NETWORK_REJECT_MM_CONN
                    LLC_CNM_SERVICE_REJECT_NORMAL
                    MAX_LENGTH_SMS_DATA

01/25/02    CD     Moved the above definitions back in here
                   since CM/UI no longer needs them

04/19/02    CD     Added definitions for release types used in MMCNM_REL_REQ
                   and MMCNM_REL_IND

04/22/02    CD     Changed value of MAXNO_SMS_TRANSACTIONS form 4 to 7

05/07/02    CD     Changed definitions for CNM-MM release types due to
                   name collision with RR

09/19/02    CD     Added sync reasons to be included in the interface between
                   MM and MN/CNM
09/27/02    CD     Added reject_type_enum, and definitions for AS and MM internal
                   reject causes.
10/16/02    CD     Added reject cause for access class blocking
11/22/02    AB     Added the internal cause base value to the establish reject
                   cause value to differentiate the GSM cause value defined
                   in GSM 4.008 Table 10.86.
11/26/02    jca    Fixed LINT errors.
12/09/02    cd     Added to the sync reason enumeration list
12/10/02    ab     Increased the MAXNO_INVOKE_IDS to MAXNO_SS_TRANSACTIONS to 
                   accomodates SS call related SS transactions.
09/18/03    mks    Merged Kurt's changes to support silent redial feature -
                   Added new cause AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED.
08/12/04    ab     Added new MN CNM internal cause value, CNM_MM_REL_PENDING,
                   CNM_MN_NO_RESOURCES.
05/24/05    vdr    Added new AS_REJ_LOW_LEVEL_IMMED_RETRY reject cause
11/02/05    ab     Added values that is used for NV_FORCE_UE_MSCR_R99_I.
01/18/06    up     Updated ue_force_rel_mode_type to move NV_FORCE_UE_REL5 at
                   the end and also NV_REL_MODE_MAX is defined.
03/05/06    NR     Adding changes for WB_AMR
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#include "sys_cnst.h"


/*
 * Comment out to enable normal debug
 *
 *
 *
 */

/*#define NO_DEBUG_AT_ALL*/


/*
 * remove any pre-existing definition of NULL which is present in some
 * system header files
 */





#define FOREVER  for (;;)



/*
 * Define the clock frequency to operate in - if not defined, the build
 * will be for a 13Mhz clock
 */

#define CLOCK_26MHZ



/*
 * define maximum number of octets in an octet group in a CC information
 * element.
 */

#define  MAXNO_SUB_OCTETS              10

#define  MAXNO_CC_TRANSACTIONS         15
#define  MAXNO_SS_TRANSACTIONS         15
#define  MAXNO_INVOKE_IDS              15
#define  MAXNO_SMS_MO_TRANSACTIONS     7
#define  MAXNO_IES_IN_MESSAGE          25    /* max no of IEs in any L3 msg */

#define  MAX_NUM_SPEECH_BEARERS        16  /* 1-4 bits  */
#define  MAX_RAB_ID                    255 /* Used only in UMTS   */
#define  DEFAULT_RAB_ID                0   /* Used to reference
                                            * tch_user_connections_store in
                                            * GSM context as RAB_ID is only
                                            * applicable to UMTS */

#define MAX_TRANSIENT_TRIGGERS         3  /* max no of transient triggers */

/*
 * Definitions for CMServiceType
 */

#define  MO_CALL_ESTABLISHMENT               1
#define  EMERGENCY_CALL_ESTABLISHMENT        2
#define  SHORT_MESSAGE_SERVICE_ESTABLISHMENT 4
#define  SUPPLEMENTARY_SERVICE_ACTIVATION    8
#define  NO_CNM_SERVICE                      0xff

/* 
 * EMERGENCY Category:
 */
#define  EMERGENCY_CALL_POLICE               0x01
#define  EMERGENCY_CALL_AMBULANCE            0x02
#define  EMERGENCY_CALL_FIRE_BRIGADE         0x04
#define  EMERGENCY_CALL_MARINE_GUARD         0x08
#define  EMERGENCY_CALL_MOUNTAIN_RESCURE     0x10
#define  EMERGENCY_CALL_UNUSED_CATEGORY      0xF0

/*
 * The following are used between CC and MM to convey additional information
 * regarding channel requirements. This is needed by MM to be able to tel RR in
 * the RR_EST_REQ message, as RR needs to know for the channel request message
 */

#define MO_CALL_ESTABLISHMENT_FULL_SPEECH    10
#define MO_CALL_ESTABLISHMENT_HALF_SPEECH    11
#define MO_CALL_ESTABLISHMENT_DATA_9600      12
#define MO_CALL_ESTABLISHMENT_DATA_4800      13
#define MO_CALL_ESTABLISHMENT_DATA_2400      14
#define MO_CALL_ESTABLISHMENT_DEFAULT MO_CALL_ESTABLISHMENT_FULL_SPEECH

/*
 * definitions of lower layer cause values
 */
#define  LLC_NORMAL_UNSPECIFIED                 1
#define  LLC_CONNNECTION_RELEASE                2
#define  LLC_RADIO_LINK_FAIL                    3
#define  LLC_T3230_EXPIRY                       4
#define  LLC_AUTHENTICATION_FAILURE             5
#define  LLC_CALL_REJECTED_BY_MM                6
#define  LLC_NETWORK_REJECT_MM_CONN             7
#define  LLC_CNM_SERVICE_REJECT_NORMAL           8



/*
 * Definitions for TchRate_T
 */

#define  TCH_FULL_RATE                    1
#define  TCH_HALF_RATE                    0




/*
 * Definitions for BOOLEAN
 */

#ifndef FALSE
  #define FALSE                             0
#endif

#ifndef TRUE
  #define TRUE                              1
#endif


#define MAX_SS_VERSION_IND_LENGTH         1     /* for future expansion       */

                                                /* max no of data bytes in    */
                                                /*            CP_DATA message */
#ifdef HOST
#define RAM_SMS_STORAGE   TRUE                  /* ifndef = no RAM storage    */
#endif

#define N_SHORT_MESSAGES                  4     /* Number of short messages   */
                                                /* that can be held in MS RAM */
                                                /* if RAM_SMS_STORAGE defined */

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
 * definitions for call_ss_operation_T
 */

#define  CSSO_BUILD_MPTY                  0
#define  CSSO_SPLIT_MPTY                  1
#define  CSSO_RETRIEVE_MPTY               2
#define  CSSO_HOLD_MPTY                   3

/*
 * Definitions for Mobile identities
 * Taken from GSM 04.08 Section 10.5.1.4
 * Encoded as a 3 bit value
 */


/*
 * max lengths of the above identities
 */

#define MAX_IMEI_LEN                      8   /* bytes  */
#define MAX_IMEI_DIGITS                   15  /* digits */

/*
 * definitions relating to IE presence
 */


/*
 * definitions of IE formats
 */

#define  FORMAT_T                         0
#define  FORMAT_V                         1
#define  FORMAT_TV                        2
#define  FORMAT_LV                        3
#define  FORMAT_TLV                       4

/*
** LANGUAGES
*/




typedef unsigned char gsm_language_T;
enum
{
   GSM_GERMAN            = 0,
   GSM_ENGLISH           = 1,
   GSM_ITALIAN           = 2,
   GSM_FRENCH            = 3,
   GSM_SPANISH           = 4,
   GSM_DUTCH             = 5,
   GSM_SWEDISH           = 6,
   GSM_DANISH            = 7,
   GSM_PORTUGUESE        = 8,
   GSM_FINNISH           = 9,
   GSM_NORWEGIAN         = 10,
   GSM_GREEK             = 11,
   GSM_TURKISH           = 12,
   GSM_LANGUAGE_13       = 13,
   GSM_LANGUAGE_14       = 14,
   GSM_UNKNOWN_LANGUAGE  = 15
};


/* Definitions at a lower level */




/* Definitions for release req causes between CNM and MM */

#define   CNM_ESTABLISHMENT_IN_PROGRESS  0
#define   CNM_NORMAL_RELEASE             2

/* Connection Management establishment rejection cause */


/* Definitions for release ind causes between MM  and CNM*/



/* --------------------------------------------------------
** Enum type for UE standard specification revision level 
** indicator, which is control by NV items 
** 
**   NV_FORCE_UE_SGSNR_R99_I                       = 1030
**   NV_FORCE_UE_MSCR_R99_I                        = 1031
** ----------------------------------------------------- */
typedef enum
{
  NV_FORCE_UE_REL97,
  NV_FORCE_UE_REL99,
  NV_FORCE_UE_DYNAMIC,
  NV_REL_MODE_MAX        /* FOR INTERNAL USE ONLY */
} ue_force_rel_mode_type;

/* Defination for maximum stacks in REG and MM */
#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
#define MAX_NAS_STACKS 3
#elif defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
#define MAX_NAS_STACKS 2
#endif 

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
#define IS_NOT_VALID_SUB_ID(x) (((sys_modem_as_id_e_type)x < SYS_MODEM_AS_ID_1) || ((sys_modem_as_id_e_type)x >= SYS_MODEM_AS_ID_1 + MAX_AS_IDS))
#define IS_NOT_VALID_STACK_ID(x) (((sys_modem_as_id_e_type)x < SYS_MODEM_AS_ID_1) || ((sys_modem_as_id_e_type)x >= SYS_MODEM_AS_ID_1+ MAX_NAS_STACKS))
#endif 

#if defined FEATURE_TRIPLE_SIM
#define IS_NOT_VALID_AS_ID(x) ((x < SYS_MODEM_AS_ID_1) || (x > SYS_MODEM_AS_ID_3))
#elif defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
#define IS_NOT_VALID_AS_ID(x) ((x != SYS_MODEM_AS_ID_1) && (x != SYS_MODEM_AS_ID_2))
#endif
#endif /* sys_cnst_h */
