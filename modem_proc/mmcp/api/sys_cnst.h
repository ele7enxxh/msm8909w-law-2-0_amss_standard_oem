#ifndef sys_cnst_h
#define sys_cnst_h
/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/sys_cnst.h_v   1.18   07 May 2002 09:07:00   cdealy  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/sys_cnst.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
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
#include "nas_exp.h"
#include "cause.h"

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


#undef NULL





#define NULL 0
#define NULL_PTR     (( void *) 0)



/*
 * Macro Function Definitions
 */

#define SET_BIT(   mask, dest )  ((dest) |= (mask))
#define CLEAR_BIT( mask, dest )  ((dest) &= (~(mask)))
#define ARE_SET(   mask, dest )  (((dest) & (mask)) == (mask))


#define MAX_MESSAGE_SIZE             4096    /* biggest intertask message   */
#define N201_MAX                     23

/*
 * Define the clock frequency to operate in - if not defined, the build
 * will be for a 13Mhz clock
 */





/*
 * define maximum number of octets in an octet group in a CC information
 * element.
 */


/*
 * definitions for channel_mode_T;
 */

#define SIGNALLING_ONLY   0
#define SPEECH_V1         1
#define SPEECH_V2         33
#define SPEECH_V3         65
#define SPEECH_V5         130
#define DATA_9600         3
#define DATA_4800         11
#define DATA_2400         19
#define DATA_14400        15
/*
   Field channel_mode in MMCC_SYNC_IND is used for GSM only.
   So, when MM receives RRC_SYNC_IND, MM uses this value to fill inside MMCC_SYNC_IND
*/
#define DEFAULT_INVALID_CHANNEL_MODE  255

/*
 * definitions for channel_type_T;
 */

#define FULL_RATE_TRAFFIC 0
#define HALF_RATE_TRAFFIC 1
#define SDCCH_4           2
#define SDCCH_8           3



/*
 * Definations for UMTS Codecs
 */

typedef enum {
UMTS_CODEC_NONE,
UMTS_NB_AMR,
UMTS_WB_AMR,
UMTS_CODEC_MAX
}umts_codecs_list_T;






/*
 * Definitions for TchRate_T
 */



typedef enum
{
  UMTS_WCDMA_CS_VOICE_OVER_DCH,
  UMTS_WCDMA_CS_VOICE_OVER_HSPA,
  UMTS_WCDMA_CS_VOICE_OVER_INVALID
} umts_wcdma_cs_voice_bearer_enum_T;

typedef enum
{
  NV_NAS_COMPLIANCE_REL99,
  NV_NAS_COMPLIANCE_REL5,
  NV_NAS_COMPLIANCE_REL6,
  NV_NAS_COMPLIANCE_REL7,
  NV_NAS_COMPLIANCE_REL10,
  NV_NAS_COMPLIANCE_REL11,
  NV_NAS_COMPLIANCE_REL_MAX        /* FOR INTERNAL USE ONLY */
} ue_nas_rel_compliance_type;


#define MAX_NO_AVAILABLE_LAIS            30     /* Arbitrary Number           */

#define PLMN_SIZE                         3     /* no of bytes in PLMN        */
#define TMSI_SIZE                         4     /* no of bytes in TMSI        */
#define PTMSI_SIZE                TMSI_SIZE     /* no of bytes in P-TMSI      */
#define PTMSI_SIGNATURE_SIZE              3     /* no of bytes in P-TMSI signature */
#define LAI_SIZE                          5     /* no of bytes in LAI         */
#define RAI_SIZE                          6     /* no of bytes in RAI         */
#define LAC_SIZE                          2     /* no of bytes in LAC         */


/*
 * definitions for hold_auxiliary_state_T
 */


/*
 * definitions for mpty_auxiliary_state_T
 */


/*
 * Definitions for Mobile identities
 * Taken from GSM 04.08 Section 10.5.1.4
 * Encoded as a 3 bit value
 */

#define NO_IDENTITY                       0
#define IMSI                              1
#define IMEI                              2
#define IMEISV                            3
#define TMSI                              4
#ifdef FEATURE_NAS_REL11
#define PTMSI_RAI_PTMSI_SIG               5
#endif


/*
 * max lengths of the above identities
 */


/*
 * definitions relating to IE presence
 */

#define  OPTIONAL_IE                         0
#define  MANDATORY_IE                        1
#define  CONDITIONAL_IE                      2

/*
 * definitions of IE formats
 */


/*
** LANGUAGES
*/


#define BA_MAX_LEN                        32



typedef enum
{
   NORMAL_OPERATION                 = 0x00,
   NORMAL_OPERATION_AND_SF          = 0x01,
   MAINTENANCE                      = 0x02,
   CELL_TEST_OPERATION              = 0x04,
   TYPE_APPROVAL_OPERATIONS         = 0x80,
   TYPE_APPROVAL_OPERATIONS_AND_SF  = 0x81
}
ms_operation_mode_T;

/* Definitions at a lower level */

#define MAX_GSM_FRAME_NUMBER   2715648

typedef enum cn_domain_tag
{
   CS,
   PS,
   DONT_CARE_DOMAIN,
   SPARE_DOMAIN
} cn_domain_T;

/* Definitions for release req causes between CNM and MM */


/* Connection Management establishment rejection cause */
#define REJ_UNSPECIFIED                     (MS_INTERNAL_CAUSE_VALUE_BASE+0)

/* AS reject causes */
#define AS_REJ_RR_REL_IND                   (MS_INTERNAL_CAUSE_VALUE_BASE+1)
#define AS_REJ_RR_RANDOM_ACCESS_FAILURE     (MS_INTERNAL_CAUSE_VALUE_BASE+2)
#define AS_REJ_RRC_REL_IND                  (MS_INTERNAL_CAUSE_VALUE_BASE+3)
#define AS_REJ_RRC_CLOSE_SESSION_IND        (MS_INTERNAL_CAUSE_VALUE_BASE+4)
#define AS_REJ_RRC_OPEN_SESSION_FAILURE     (MS_INTERNAL_CAUSE_VALUE_BASE+5)
#define AS_REJ_LOW_LEVEL_FAIL               (MS_INTERNAL_CAUSE_VALUE_BASE+6)
#define AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED (MS_INTERNAL_CAUSE_VALUE_BASE+7)
#define AS_REJ_DEEP_FADE                    (MS_INTERNAL_CAUSE_VALUE_BASE+53)

/* MM reject causes */
#define MM_REJ_INVALID_SIM                  (MS_INTERNAL_CAUSE_VALUE_BASE+8)
#define MM_REJ_NO_SERVICE                   (MS_INTERNAL_CAUSE_VALUE_BASE+9)
#define MM_REJ_TIMER_T3230_EXP              (MS_INTERNAL_CAUSE_VALUE_BASE+10)
#define MM_REJ_NO_CELL_AVAILABLE            (MS_INTERNAL_CAUSE_VALUE_BASE+11)
#define MM_REJ_WRONG_STATE                  (MS_INTERNAL_CAUSE_VALUE_BASE+12)
#define MM_REJ_ACCESS_CLASS_BLOCKED         (MS_INTERNAL_CAUSE_VALUE_BASE+13)
#define MM_REJ_SIM_BUSY                           (MS_INTERNAL_CAUSE_VALUE_BASE+53)
#define MM_REJ_GW_NOT_AVAILABLE             (MS_INTERNAL_CAUSE_VALUE_BASE+57)

/* Definitions for release ind causes between MM  and CNM*/
#define ABORT_MSG_RECEIVED                  (MS_INTERNAL_CAUSE_VALUE_BASE+14)  
#define OTHER_CAUSE                         (MS_INTERNAL_CAUSE_VALUE_BASE+15)

/* CNM reject causes */
#define CNM_REJ_TIMER_T303_EXP              (MS_INTERNAL_CAUSE_VALUE_BASE+16)
#define CNM_REJ_NO_RESOURCES                (MS_INTERNAL_CAUSE_VALUE_BASE+17)
#define CNM_MM_REL_PENDING                  (MS_INTERNAL_CAUSE_VALUE_BASE+18)
#define CNM_INVALID_USER_DATA               (MS_INTERNAL_CAUSE_VALUE_BASE+19)


#define AS_REJ_LOW_LEVEL_IMMED_RETRY        (MS_INTERNAL_CAUSE_VALUE_BASE+20)
#define AS_REJ_LRRC_LOWER_LAYER_FAILURE     (MS_INTERNAL_CAUSE_VALUE_BASE+21)
#define AS_REJ_ABORT_RADIO_UNAVAILABLE      (MS_INTERNAL_CAUSE_VALUE_BASE+22)

/* EMM reject causes*/
#define EMM_REJ_TIMER_T3417_EXT_EXP    (MS_INTERNAL_CAUSE_VALUE_BASE+23)
#define EMM_REJ_TIMER_T3417_EXP    (MS_INTERNAL_CAUSE_VALUE_BASE+24)
#define EMM_REJ_SERVICE_REQ_FAILURE_LTE_NW_REJECT         (MS_INTERNAL_CAUSE_VALUE_BASE+49)
#define EMM_REJ_SERVICE_REQ_FAILURE_CS_DOMAIN_NOT_AVAILABLE        (MS_INTERNAL_CAUSE_VALUE_BASE+52)
#define EMM_REJ_SERVICE_REQ_FAILURE_SIM_BUSY        (MS_INTERNAL_CAUSE_VALUE_BASE+53)

/* AS - LTE RRC related failure causes*/
#define AS_REJ_LRRC_UL_DATA_CNF_FAILURE_TXN              (MS_INTERNAL_CAUSE_VALUE_BASE+25)
#define AS_REJ_LRRC_UL_DATA_CNF_FAILURE_HO               (MS_INTERNAL_CAUSE_VALUE_BASE+26)
#define AS_REJ_LRRC_UL_DATA_CNF_FAILURE_CONN_REL         (MS_INTERNAL_CAUSE_VALUE_BASE+27)
#define AS_REJ_LRRC_UL_DATA_CNF_FAILURE_RLF              (MS_INTERNAL_CAUSE_VALUE_BASE+28)
#define AS_REJ_LRRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN    (MS_INTERNAL_CAUSE_VALUE_BASE+29)
#define AS_REJ_LRRC_CONN_EST_SUCCESS                     (MS_INTERNAL_CAUSE_VALUE_BASE+30)
#define AS_REJ_LRRC_CONN_EST_FAILURE                     (MS_INTERNAL_CAUSE_VALUE_BASE+31)
#define AS_REJ_LRRC_CONN_EST_FAILURE_ABORTED             (MS_INTERNAL_CAUSE_VALUE_BASE+32)
#define AS_REJ_LRRC_CONN_EST_FAILURE_ACCESS_BARRED       (MS_INTERNAL_CAUSE_VALUE_BASE+33)
#define AS_REJ_LRRC_CONN_EST_FAILURE_CELL_RESEL          (MS_INTERNAL_CAUSE_VALUE_BASE+34)
#define AS_REJ_LRRC_CONN_EST_FAILURE_CONFIG_FAILURE      (MS_INTERNAL_CAUSE_VALUE_BASE+35)
#define AS_REJ_LRRC_CONN_EST_FAILURE_TIMER_EXPIRED       (MS_INTERNAL_CAUSE_VALUE_BASE+36)
#define AS_REJ_LRRC_CONN_EST_FAILURE_LINK_FAILURE        (MS_INTERNAL_CAUSE_VALUE_BASE+37)
#define AS_REJ_LRRC_CONN_EST_FAILURE_NOT_CAMPED          (MS_INTERNAL_CAUSE_VALUE_BASE+38)
#define AS_REJ_LRRC_CONN_EST_FAILURE_SI_FAILURE          (MS_INTERNAL_CAUSE_VALUE_BASE+39)
#define AS_REJ_LRRC_CONN_EST_FAILURE_CONN_REJECT         (MS_INTERNAL_CAUSE_VALUE_BASE+40)
#define AS_REJ_LRRC_CONN_REL_NORMAL                      (MS_INTERNAL_CAUSE_VALUE_BASE+41)
#define AS_REJ_LRRC_CONN_REL_RLF                         (MS_INTERNAL_CAUSE_VALUE_BASE+42)
#define AS_REJ_LRRC_CONN_REL_CRE_FAILURE                 (MS_INTERNAL_CAUSE_VALUE_BASE+43)
#define AS_REJ_LRRC_CONN_REL_OOS_DURING_CRE              (MS_INTERNAL_CAUSE_VALUE_BASE+44)
#define AS_REJ_LRRC_CONN_REL_ABORTED                     (MS_INTERNAL_CAUSE_VALUE_BASE+45)
#define AS_REJ_LRRC_CONN_REL_SIB_READ_ERROR              (MS_INTERNAL_CAUSE_VALUE_BASE+46)
#define AS_REJ_LRRC_CONN_REL_ABORTED_IRAT_SUCCESS        (MS_INTERNAL_CAUSE_VALUE_BASE+47)
#define AS_REJ_LRRC_RADIO_LINK_FAILURE                   (MS_INTERNAL_CAUSE_VALUE_BASE+48)
#define AS_REJ_DETACH_WITH_REATTACH_LTE_NW_DETACH        (MS_INTERNAL_CAUSE_VALUE_BASE+50)
#define AS_REJ_DETACH_WITH_OUT_REATTACH_LTE_NW_DETACH    (MS_INTERNAL_CAUSE_VALUE_BASE+51)
#define AS_REJ_LRRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB       (MS_INTERNAL_CAUSE_VALUE_BASE+52)


/* Cause used in MMCNM_REL_IND to indicate failure of ONLY PS SMS 
 * for example: Failure of SMS started in LTE due to CSFB.
 * */
#define PS_SMS_FAIL                         (MS_INTERNAL_CAUSE_VALUE_BASE+52)

/* Cause used when RLF happends during call in disconnecting state*/
#define RLF_DURING_CC_DISCONNECT            (MS_INTERNAL_CAUSE_VALUE_BASE+54)

/* cause used when RRC connection has been released with the cause “deep fade”*/
#define RLF_DEEP_FADE_CC_DISCONNECT  (MS_INTERNAL_CAUSE_VALUE_BASE+55)

#define EMM_REJ_TEMP_STATE                  (MS_INTERNAL_CAUSE_VALUE_BASE+56)

/*=========================================================================
* ================== 1XCP CSFB reject causes===============================
* =========================================================================
*/

/*!< Malformed UHDM/ECAM  */
#define ONEX_REJ_1XCSFB_HO_FAIL_MSG_INVAILD              (MS_INTERNAL_CAUSE_VALUE_BASE+1)

/*!< Unexpected UHDM/ECAM */ 
#define ONEX_REJ_1XCSFB_HO_FAIL_MSG_IGNORE               (MS_INTERNAL_CAUSE_VALUE_BASE+2)

/*!< Acquisition on native 1X failed */
#define ONEX_REJ_1XCSFB_HO_FAIL_ACQ_FAIL                 (MS_INTERNAL_CAUSE_VALUE_BASE+3)

/*!< Network initiated call end */
#define ONEX_REJ_1XCSFB_HO_FAIL_CALL_REL_REL_ORDER       (MS_INTERNAL_CAUSE_VALUE_BASE+4)

/*!< Reorder */
#define ONEX_REJ_1XCSFB_HO_FAIL_CALL_REL_REORDER         (MS_INTERNAL_CAUSE_VALUE_BASE+5)

/*!< Intercept Order */
#define ONEX_REJ_1XCSFB_HO_FAIL_CALL_REL_INTERCEPT_ORDER (MS_INTERNAL_CAUSE_VALUE_BASE+6)

/*!< Network initiated call Release and no reason is given */ 
#define ONEX_REJ_1XCSFB_HO_FAIL_CALL_REL_NORMAL          (MS_INTERNAL_CAUSE_VALUE_BASE+7)

/*!< Network initiated call Release because SO is not supported */
#define ONEX_REJ_1XCSFB_HO_FAIL_CALL_REL_SO_REJ          (MS_INTERNAL_CAUSE_VALUE_BASE+8)

/*!< Network initiated call because of SPC error in OTASP */
#define ONEX_REJ_1XCSFB_HO_FAIL_CALL_REL_OTASP_SPC_ERR   (MS_INTERNAL_CAUSE_VALUE_BASE+9)


/* --------------------------------------------------------
** Enum type for UE standard specification revision level 
** indicator, which is control by NV items 
** 
**   NV_FORCE_UE_SGSNR_R99_I                       = 1030
**   NV_FORCE_UE_MSCR_R99_I                        = 1031
** ----------------------------------------------------- */

#endif /* sys_cnst_h */
