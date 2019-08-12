/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR Cell Selection Module

GENERAL DESCRIPTION
   This module contains General RR definitions.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_defs.h_v   1.20   17 Jul 2002 11:50:34   rodgerc  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_defs.h#1 $   $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/17/11   sjw     Ignore block quality reports when DSDS is enabled in test mode
07/22/09   sjw     Added support for FEATURE_UMTS_VOICE_CIPHERING_IND
05/13/09   sjw     Start relocation of dedicated mode data to a single structure
05/06/09   sjw     Removed unused variable last_no_channels_used
05/06/09   sjw     Moved handover_details and assignment_details out of rr_data.c
05/05/09   sjw     Reworked interfaces between G2G and G2W handover code
05/01/09   sjw     Relocated invalidate_measurement_reports to rr_ded_meas_reporting.c
03/18/09   sjw     Relocated timer definitions to new rr_timer module
02/13/09   sjw     Reworked local storage for ciphering module data
Jun/24/04  VT      Added support for FEATURE_GERAN_STORED_LIST_CELL_SELECTION
May/26/04  DMB     Assigned values to rr_event_T enums and removed unused
05/07/04   gc      Enhanced Trace Buffer Code
04/29/04   gc      CR42000: Added Early Classmark Sending Restriction to sys_info
03/25/04   DMB     Events added/removed
03/24/04   DMB     Changes to RR and GRR main states, and GRR sub-states
03/05/04   gc      CR31210: Fixes to W2G HO Failure
01/07/04   gc      Added sys_info_mask to system_information_database_T
04/15/03   sbs     Added support for Location Services related APDU handling.
02/19/03   Rao     Added GSM Deep Sleep support.
02/18/03   JAC     Added rr_candidate_db_find_strongest and rr_candidate_db_sort_rxlev
02/14/03   RG      Added support for G2W blind Handover
02/07/03   he      Added REEST states and events.
01/27/03   JAC     Restoring of cell channel description when going idle
1/22/02    he      Added UTRAN_CLASSMARK_CHANGE
1/20/02    Rao     Changed RSCP from word to int.
12/12/02   JAC     Added another set of 5s C2 timers
12/03/02   Rao     Added additional State for camping on GPRS cell after W2G
                   Reselection.
11/21/02   Rao     Incorporating G2W Reselection Review Suggestions.
11/18/02   RG      Fixed bugs found when unit-testing GSM to WCDMA reselection.
11/08/02   he      Addded events and states for GPRS 2.0 support.
11/04/02   Rao     Added Vocoder control related changes.
                   Also WCDMA to GSM Ciphering related changes.
10/21/02   he      Added EV_GPRS_ABNORMAL_PS_PAGING.
10/28/02   Rao     Added more WCDMA to GSM Reselect Rej timout values.
10/29/02   Rao     Added InterRAT Reselection (GSM to WCDMA) Support.
08/30/02   Rao     Added InterRAT Reselection (WCDMA to GSM ) Support.
08/26/02   rmc     Renamed MULTIBAND_SUPPORT to FEATURE_GSM_MULTIBAND
08/15/02   Rao     Removed the extra Inter RAT HO Event.
08/07/02   he      Added last_tc field to system information database structure.
08/07/02   he      Added last_tc field to system information database structure.
08/09/02   JAC     Changes for cell selection optimizations
07/31/02   rmc     Support for centralized RR timer management
                   Removed all FEATURE_MULTIMODE_GSM and RR_LOCAL_SIMINFO flags
07/23/02   Rao     Added InterRAT ( WCDMA To GSM )  Handover support
07/15/02   rmc     Changed rr_PLMN_request_details to store gsm_only flag from MM
07/11/02   JAC     Added early camping support when collecting full SI in reselection
07/08/02   rmc     Removing CBQ2 from system information database
06/20/02   JAC     Added early camping support
06/20/02   Rao     Adding CRH to RR_C2_T Struct.
06/12/02   rmc     Removed include for mm_as_types.h, should be included from mm_rr.h,
                   featurized sys_PLMN_selection.h include
05/30/02   Rao     Getting rid of Path Loss timer - Reselection Optimizations
05/30/02   JAC     Removed reselection list structures added SI db changes
05/30/02   rmc     Changes to enable MULTIBAND_SUPPORT flag
05/28/02   rmc     Added include for "amssassert.h" so it is available in all RR files
05/23/02   rmc     Removed   EV_GSM_MODE_ACT_FAIL and EV_GSM_MODE_DEACT_FAIL events
03/12/02   rmc     Dual mode cell selection changes
02/25/02   rao     Added Dual Mode Inactivity Control Message & Event enums.
02/28/02   JAC     Added EV_RESELECTION_WAIT_CNF
02/22/02   JAC     Changed chosen_cell_list_T and system_information_database_T
02/07/02   JAC     Added EV_RESELECTION_START_IDLE, EV_CELL_RESELECTION_ABORT
02/04/02   rao     Added new RR local SIM info Structure.
01/30/02   RMC     Cell Reselection cleanup
12/14/01   rao     Added additional fields for Rescue Cell type.
12/11/00   TS      Initial Revision.
*============================================================================*/
#ifndef RR_DEFS_H
#define RR_DEFS_H

#include "geran_variation.h"
#include "customer.h"
#include "sys_cnst.h"
#include "rr_l1.h"
#include "rr_l1_g.h"                // for BSIC_T
#include "mm_rr.h"                // for MAX_FORBIDDEN_LAIS
#include "cause.h"
#include "sys.h"
#include "sys_v.h"                  // common types
#include "sys_plmn.h"             // for PLMN_id_T type and related functions
#include "sys_plmn_selection.h"   // for PLMN selection related types
#include "msg.h"
#include "rr_events.h"
#include "rr_lai.h"
#include "rr_timer.h"
#include "csn_apt.h"
#include "rr_l2.h"
#include "rr_multiband.h"
#include "rr_multiband_g.h"
#include <stddef.h>
#include "rr_log_types.h"
#include "amssassert.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#ifdef FEATURE_LTE
#include "lte_irat_types.h"
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS
#include "tdsrrc_rr_if.h"
#include "rr_tdsrrc_if.h"
#endif /*FEATURE_GSM_TDS*/

#define RR_ASSERT_WARN(xx_exp) \
  if (!(xx_exp)) \
  { \
    MSG_GERAN_ERROR_0("RR Assertion " #xx_exp " failed"); \
  }

#define RR_ASSERT_RETURN_VOID(xx_exp) \
  if (!(xx_exp)) \
  { \
    MSG_GERAN_ERROR_0("RR Assertion " #xx_exp " failed - exit"); \
    return; \
  }

#define RR_ASSERT_RETURN_VALUE(xx_exp, xx_val) \
  if (!(xx_exp)) \
  { \
    MSG_GERAN_ERROR_0("RR Assertion " #xx_exp " failed - exit"); \
    return (xx_val); \
  }

#define RR_ASSERT_FATAL(xx_exp) \
  if (!(xx_exp)) \
  { \
    ERR_FATAL("RR Assertion " #xx_exp " failed", 0, 0, 0); \
  }

#define RR_NULL_CHECK_FATAL(pp)             \
  if (pp == NULL)                           \
  {                                         \
    ERR_FATAL(#pp" == NULL",0,0,0);         \
  }

#define RR_NULL_CHECK_RETURN_VOID(pp)    \
  if (pp == NULL)                        \
  {                                      \
    MSG_GERAN_ERROR_0(#pp" == NULL");    \
    return;                              \
  }

#define RR_NULL_CHECK_RETURN_PARAM(pp,rr)   \
  if (pp == NULL)                           \
  {                                         \
    MSG_GERAN_ERROR_0(#pp" == NULL");       \
    return rr;                              \
  }

#define  RR_ZERO_STRUCTURE(s) memset((void *)&s, 0, sizeof(s));
#define  RR_ZERO_ARRAY(a)     memset((void *)a,  0, sizeof(a));

#define RR_ASSERT_FATAL_INVALID_GAS_ID(xx)   \
  if (xx > GERAN_ACCESS_STRATUM_ID_MAX)      \
  {                                          \
    ERR_FATAL(#xx" = 0x%x",(int)xx, 0, 0);   \
  }

#define RR_AS_ID_TO_GAS_ID geran_map_nas_id_to_gas_id(as_id)
#define RR_GAS_ID_TO_AS_ID geran_map_gas_id_to_nas_id(gas_id)

#define RR_DEFAULT_SERVING_BAND_REPORTING               0x03
#define RR_DEFAULT_REPORT_TYPE                          REPORT_TYPE_N0RMAL

/* Default values for parameters in E-UTRAN Measurement Parameters / GPRS
E-UTRAN Measurement Parameters. */
#define RR_DEFAULT_XXX_REPORTING_THRESHOLD              0x00
#define RR_DEFAULT_XXX_REPORTING_OFFSET                 0x00
#define RR_DEFAULT_FDD_REPORTING_THRESHOLD_2            0x00
#define RR_DEFAULT_EUTRAN_FDD_REPORTING_THRESHOLD_2     0x00
#define RR_DEFAULT_EUTRAN_TDD_REPORTING_THRESHOLD_2     0x00
#define RR_DEFAULT_EUTRAN_FDD_MEASUREMENT_REPORT_OFFSET 0x00
#define RR_DEFAULT_EUTRAN_TDD_MEASUREMENT_REPORT_OFFSET 0x00
#define RR_DEFAULT_REPORTING_GRANULARITY                0x00
#define RR_DEFAULT_QSEARCH_C                            0x0f /*!< Search Never */
#define RR_DEFAULT_QSEARCH_C_EUTRAN                     0x0f /*!< Search Never */
#define RR_DEFAULT_QSEARCH_P_EUTRAN                     0x0f /*!< Search Never */
#define RR_DEFAULT_MEASUREMENT_CONTROL_EUTRAN           (TRUE)

/*********************************************/
/* definition of the Protocol Discriminators */
/*********************************************/

#define RADIO_RESOURCES_SHORT_PD 0x00

#define RADIO_RESSOURCES     0x06
#define MOBILITY_MANAGEMENT  0x05
#define CALL_CONTROL         0x03
#define SMS_MESSAGES         0x09
#define NON_CALL_RELATED_SS  0x0B
#define TEST_MANAGEMENT      0x0F
#ifdef FEATURE_GSM_DTM
#define GTTP                 0x04
#endif /* FEATURE_GSM_DTM */

/* NUMBER_OF_MT_RR_MESSAGES is the defined size of message_list, which contains the definition */
/* of all the OTA messages which can be received into RR.                                      */
/* If other FEATURE_ switches add messages which can be received in rr_initialise_msg_list(),  */
/* then this maximum size must be increased to accomodate.                                     */
#define NUMBER_OF_MT_RR_MESSAGES 40

/*
 * define the number of IE descriptors we need to handle.
 */
#define MAXNO_RR_IE_DESCS 166

/**************************************/
/* definition of the RR message types */
/* according to 04.08 10.4            */
/**************************************/

#define ADDITIONAL_ASSIGNMENT             0x3B
#define IMMEDIATE_ASSIGNMENT              0x3F
#define IMMEDIATE_ASSIGNMENT_EXTENDED     0x39
#define IMMEDIATE_ASSIGNMENT_REJECT       0x3A

#define CIPHERING_MODE_COMMAND            0x35
#define CIPHERING_MODE_COMPLETE           0x32

#define ASSIGNMENT_COMMAND                0x2E
#define ASSIGNMENT_COMPLETE               0x29
#define ASSIGNMENT_FAILURE                0x2F
#define HANDOVER_COMMAND                  0x2B
#define HANDOVER_COMPLETE                 0x2C
#define HANDOVER_FAILURE                  0x28
#define PHYSICAL_INFORMATION              0x2D

#define CHANNEL_RELEASE                   0x0D
#define PARTIAL_RELEASE                   0x0A
#define PARTIAL_RELEASE_COMPLETE          0x0F

#define PAGING_REQUEST_TYPE_1             0x21
#define PAGING_REQUEST_TYPE_2             0x22
#define PAGING_REQUEST_TYPE_3             0x24
#define PAGING_RESPONSE                   0x27

#define SYSTEM_INFORMATION_TYPE_8         0x18
#define SYSTEM_INFORMATION_TYPE_1         0x19
#define SYSTEM_INFORMATION_TYPE_2         0x1A
#define SYSTEM_INFORMATION_TYPE_3         0x1B
#define SYSTEM_INFORMATION_TYPE_4         0x1C
#define SYSTEM_INFORMATION_TYPE_5         0x1D
#define SYSTEM_INFORMATION_TYPE_6         0x1E
#define SYSTEM_INFORMATION_TYPE_7         0x1F

#define SYSTEM_INFORMATION_TYPE_2_BIS     0x02
#define SYSTEM_INFORMATION_TYPE_5_BIS     0x05

#define SYSTEM_INFORMATION_TYPE_2_TER     0x03

#define SYSTEM_INFORMATION_TYPE_2_QUATER  0x07

/* Note:
Measurement Information has a short header format and the message type is specified as 0x05 which
clashes with the message type of SYSTEM_INFORMATION_TYPE_5_BIS.

Defining MEASUREMENT_INFORMATION_TYPE to a unique (unused in RR) value that will be used interally
for RR processing.
*/

/* Table 10.4.2/3GPP TS 04.18: uses short RR PD and short header format */
#define SYSTEM_INFORMATION_TYPE_10        0x00

#define ENHANCED_MEASUREMENT_REPORT       0x04
#define MEASUREMENT_INFORMATION           0x05
#define MEASUREMENT_INFORMATION_TYPE      0x80
#define UNKNOWN_SHORT_HEADER_MESSAGE_TYPE 0x8F

#define SYSTEM_INFORMATION_TYPE_5_TER     0x06
#define SYSTEM_INFORMATION_TYPE_13        0x00

#define CHANNEL_MODE_MODIFY               0x10
#define RR_STATUS                         0x12
#define CHANNEL_MODE_MODIFY_ACKNOWLEDGE   0x17
#define FREQUENCY_REDEFINITION            0x14
#define MEASUREMENT_REPORT                0x15
#define CLASSMARK_CHANGE                  0x16
#define CLASSMARK_ENQUIRY                 0x13
#define UTRAN_CLASSMARK_CHANGE            0x60

#define CLOSE_TCH_LOOP_CMD                0x00
#define OPEN_LOOP_CMD                     0x06
#define TEST_INTERFACE                    0x14
#define ACT_EMMI_CMD                      0x0C
#define DEACT_EMMI_CMD                    0x10
#define CLOSE_TCH_LOOP_ACK                0x01
#define ACT_EMMI_ACK                      0x0D

/* the following test mode message types are defined in 3GPP 44.014 */

#ifdef FEATURE_CGPS_UMTS_CP_GSM
#define RR_RESET_UE_POSITIONING_INFO      0x26
#endif /* FEATURE_CGPS_UMTS_CP_GSM */

/* end of test mode message types */
#define OPEN_LOOP_CMD_ACK_LEN             3
#define CLOSE_TCH_LOOP_ACK_LEN            2
#define OPEN_LOOP_CMD_ACK                 0x81

#define GPRS_SUSPENSION                       0x34
#define APPLICATION_INFORMATION               0x38
#define INTERSYSTEM_TO_UTRAN_HANDOVER_COMMAND 0x63
#define DTM_ASSIGNMENT_FAILURE                0x48
#define DTM_REJECT                            0x49
#define DTM_REQUEST                           0x4a
#define PACKET_ASSIGNMENT                     0x4b
#define DTM_ASSIGNMENT_COMMAND                0x4c
#define DTM_INFORMATION                       0x4d
#define PACKET_NOTIFICATION                   0x4e
#define GPRS_INFORMATION                      0x00  /* see 44.018 10.4.3 GTTP messages */

/**
 * Default values for various measurement parameters.
 */
#define DEFAULT_FDD_GPRS_QOFFSET       8
#define DEFAULT_FDD_QOFFSET            8
#define DEFAULT_FDD_QMIN               7
#define DEFAULT_QSEARCH_I             15
#define DEFAULT_QSEARCH_P             15
#define DEFAULT_TDD_GPRS_QOFFSET       5
#define DEFAULT_TDD_QOFFSET            5

/* Min and max values for BS_PA_MFRMS */
#define BS_PA_MFRMS_MIN 2
#define BS_PA_MFRMS_MAX 9

/**
 * Defines for invalid PLMN ID, etc.
 */
#define INVALID_BSIC_CC  0xFF
#define INVALID_PLMN_ID  0xFF
#define INVALID_PLMN_CC  0xFF
#define INVALID_LA_BYTE  0xFF
#define INVALID_LAC_WORD (((word)INVALID_LA_BYTE << 8) | ((word)INVALID_LA_BYTE))

#define INVALID_UARFCN 0xFFFFFFFF
#define INVALID_EARFCN 0xFFFFFFFF

/* Defined in 3GPP 45.008 */
#define RR_DEFAULT_UTRAN_QRXLEVMIN 0

/* Specifications do not define a default value for THRESH_UTRAN_HIGH. */
#define RR_DEFAULT_THRESH_UTRAN_HIGH 0

/* Defined in 3GPP 45.008 as THRESH_UTRAN_HIGH, but no default is specified
for that value. */
#define RR_DEFAULT_THRESH_UTRAN_LOW 0

/* Defined in 3GPP 45.008 */
#define RR_DEFAULT_EUTRAN_QRXLEVMIN 0

/* Specifications do not define a default value for THRESH_EUTRAN_HIGH. */
#define RR_DEFAULT_THRESH_EUTRAN_HIGH 0

/* Defined in 3GPP 45.008 as THRESH_EUTRAN_HIGH, but no default is specified
for that value. */
#define RR_DEFAULT_THRESH_EUTRAN_LOW 0

/* Defined in 3GPP 44.018. */
#define RR_DEFAULT_LTE_MEASUREMENT_BANDWIDTH 0

#define MAX_RR_EUTRAN_NEIGHBOUR_LIST_SIZE 16 

/* Defined in 3GPP 45.008, the default T-reselection value is 5 seconds. */
#define RR_DEFAULT_T_RESELECTION_TIMEOUT 5000

typedef enum
{
  RR_PLMN_SELECT_REQ_SEARCH = 0x0,
  RR_PLMN_LIST_REQ_IMME_SEARCH = 0x1,
  RR_PLMN_LIST_REQ_BG_SEARCH = 0X2
} rr_freq_search_reason_T;

/* The RR cause values are defined in services\utils\cause.h */
typedef byte RR_cause_T;

/******************************************/
/* Defines the complement of channel type */
/* that we need for the channel_needed    */
/* Information Element                    */
/******************************************/

#define SDCCH_NEEDED                      4
#define FULL_RATE_OR_HALF_RATE_TRAFFIC    5
#define ANY_CHANNEL                       6

/**********************************************/
/* Defines the handover sychronisation values */
/* Defined in rr_l1.h                         */
/**********************************************/

/**********************************************/
/* Masks                                      */
/**********************************************/

#define X_MASK              0x01  /* 0000 0001 */
#define BAZY_MASK           0x3E  /* 0011 1110 */

/**********************************************/
/* Masks for measurement reports received     */
/**********************************************/
#define RR_MASK_GSM_IDLE_SERV_MEAS         0x0001U
#define RR_MASK_GSM_IDLE_SURROUND_MEAS     0x0002U
#define RR_MASK_WCDMA_IDLE_SURROUND_MEAS   0x0004U

#define RR_MASK_IDLE_MEAS_COMPLETE         0x0007U

#define RR_CIPHERKEY_LEN   0x08
#define RR_TMSI_LEN        0x4
#define RR_IMSI_LEN        0x9
#define RR_PTMSI_LEN       0x4
#define RR_PTMSI_SIG_LEN   0x7
#define RR_BCCH_LIST_LEN   0x16
#define RR_ACC_CLASS_LEN   0x2

#define RR_MAX_ARFCN       1024

#ifdef FEATURE_CGPS_UMTS_CP_GSM
#define RR_APDU_COMMAND_OR_FINAL_RESP      0x0
#define RR_APDU_NOT_COMMAND_OR_FINAL_RESP  0x4
#define RR_APDU_FIRST_FLAG                 0x2
#define RR_APDU_LAST_FLAG                  0x1
#endif /* FEATURE_CGPS_UMTS_CP_GSM */

/* Information Element Indentifiers (IEIs) */
#define BA_RANGE_IEI                  0x73
#define GPRS_RESUMPTION_IEI           0xC0
#define CMM_MULTIRATE_CONFIG_IEI      0x03 /* for Channel Mode Modify    */
#define DTM_MULTIRATE_CONFIG_IEI      0x17 /* for DTM Assignment Command */
#define CELL_SELECTION_INDICATOR_IEI  0x77  /* in CHANNEL RELEASE */
#define INDIVIDUAL_PRIORITIES_IEI     0x7c

/* Optional IEIs for the HANDOVER COMMAND */
/* See 3GPP 44.018 Table 9.1.15.1         */
#define FREQ_SHORT_LIST_AFTER_TIME_IEI     0x02
#define FREQUENCY_LIST_AFTER_TIME_IEI      0x05
#define CELL_CHANNEL_DESCRIPTION_IEI       0x62
#define MODE_OF_FIRST_CHANNEL_IEI          0x63
#define SECOND_CHAN_DESC_AFTER_TIME_IEI    0x64
#define MODE_OF_SECOND_CHANNEL_IEI         0x66
#define FREQ_CHAN_SEQUENCE_AFTER_TIME_IEI  0x69
#define MOBILE_ALLOC_AFTER_TIME_IEI        0x72
#define STARTING_TIME_IEI                  0x7C
#define REAL_TIME_DIFFERENCE_IEI           0x7B
#define TIMING_ADVANCE_IEI                 0x7D
#define FREQ_SHORT_LIST_BEFORE_TIME_IEI    0x12
#define FREQUENCY_LIST_BEFORE_TIME_IEI     0x19
#define FIRST_CHAN_DESC_BEFORE_TIME_IEI    0x1C
#define SECOND_CHAN_DESC_BEFORE_TIME_IEI   0x1D
#define FREQ_CHAN_SEQUENCE_BEFORE_TIME_IEI 0x1E
#define MOBILE_ALLOC_BEFORE_TIME_IEI       0x21
#define CIPHER_MODE_SETTING_IEI            0x90
#define HO_MULTIRATE_CONFIG_IEI            0x03
#define MOBILE_TIME_DIFFERENCE_IEI         0x77 /* sent in HANDOVER COMPLETE */

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
#define EDTM_CS_RELEASE_IEI  0xA0
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

/* General purpose call-back function type (gas_id parameter, returns void) */
typedef void (*rr_cb_fn_t)(gas_id_t);

#define CALL_RR_CB_FN(rr_cb_fn,gas_id) {if (rr_cb_fn != NULL) (*rr_cb_fn)(gas_id);}

/* H_PRIO - 2 bit field, 0 = rule disabled, 1 = 5 dB, 2 = 4 dB, 3 = 3 dB */
#define RR_CONVERT_H_PRIO_TO_DB(h_prio) (6 - h_prio)

#ifdef FEATURE_VAMOS_II
  #define SPEECH_V1_VAMOS2 193
  #define SPEECH_V2_VAMOS2 194
  #define SPEECH_V3_VAMOS2 195
  #define SPEECH_V5_VAMOS2 197
#endif /*FEATURE_VAMOS_II*/


typedef enum
{
  T,
  TLV,
  TV,
  V,
  LV
} format_T;

typedef struct ie_descriptor
{
  byte                 IEI;
  byte                 IE_length;
  format_T             IE_format;
  byte                 presence;
  byte                 *start_IE;
  struct ie_descriptor *next_IE;
} ie_descriptor_T;

typedef struct
{
  byte               message_type;
  byte               imperative_length;
  l2_channel_type_T  l2_channel;
  byte               *start_l3_msg;  
  uint8              l3_msg_len;     
  ie_descriptor_T   *first_IE;
} message_list_T;

typedef struct
{
  LAI_T   lai; /*23.003 4.3.1 location area identifier*/
  uint16  ci;  /*23.003 4.3.1 cell id*/
} rr_csi_cgi_t;

/* Main states of RR (rr_control) */
/* N.B. This numbering should be preserved for F3/logging purposes */
typedef enum
{
  RR_INACTIVE                             =  0,
  RR_GOING_ACTIVE                         =  1,
  RR_GOING_INACTIVE                       =  2,
  RR_CELL_SELECTION                       =  3,
  RR_PLMN_LIST_SEARCH                     =  4,
  RR_IDLE                                 =  5,  /* No longer used */
  RR_CELL_RESELECTION                     =  6,  /* No longer used */
  RR_CONNECTION_PENDING                   =  7,
  RR_CELL_REESTABLISH                     =  8,
  RR_DATA_TRANSFER                        =  9,
  RR_NO_CHANNELS                          = 10,
  RR_CONNECTION_RELEASE                   = 11,
  RR_EARLY_CAMPED_WAIT_FOR_SI             = 12,
  RR_W2G_INTERRAT_HANDOVER_PROGRESS       = 13,
  RR_W2G_INTERRAT_RESELECTION_PROGRESS    = 14, /* No longer used */
  RR_W2G_INTERRAT_CC_ORDER_PROGRESS       = 15,
  RR_G2W_INTERRAT_RESELECTION_PROGRESS    = 16, /* No longer used */
  RR_WAIT_FOR_EARLY_PSCAN                 = 17,
  RR_GRR                                  = 18,
  RR_G2W_INTERRAT_HANDOVER_PROGRESS       = 19,
  RR_W2G_SERVICE_REDIRECTION_IN_PROGRESS  = 21,
  RR_RESET                                = 22,


#ifdef FEATURE_FEMTO_GSM_NL
  RR_FEMTO                                = 29,
#endif /* FEATURE_FEMTO_GSM_NL */

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
  RR_X2G_RESEL                            = 30,
  RR_X2G_RESEL_ABORTED                    = 31,
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

  RR_X2G_REDIR                            = 32,

#if defined(FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS_G2T_REDIRECTION)
  RR_G2X_REDIR                            = 33,
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE || FEATURE_LTE || FEATURE_GSM_TDS_G2T_REDIRECTION*/

#ifdef FEATURE_LTE_TO_GSM_CGI
  RR_X2G_CGI                              = 34,
#endif /* FEATURE_LTE_TO_GSM_CGI */

  RR_X2G_CCO_FAILED                       = 35,
  RR_X2G_CCO_ABORTED                      = 36,
  RR_X2G_CCO_FAILED_ABORTED               = 37,

  // A cell is selected (i.e. GL1 is tuned to the BCCH) but SI acquisition is needed.
  RR_CELL_SELECTED_ACQUIRE_SI             = 40,

  RR_STATE_MAX

} rr_state_T;

/************************************************/
/* GRR main states                              */
/************************************************/
typedef enum
{
  GRR_NULL,
  GRR_ACQUIRE,
  GRR_CAMPED,
  GRR_CONNECTION_PENDING,
  GRR_CELL_RESELECTION,
  GRR_CELL_RESELECTION_G2W,
#ifdef FEATURE_GPRS_PS_HANDOVER
  GRR_PSHO,
#endif /* FEATURE_GPRS_PS_HANDOVER */
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  GRR_X2G_CCO_FAILED,
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
  GRR_STARTING_L1_IDLE,
  GRR_CAMPED_ENABLING_PS_ACCESS,
  GRR_CAMPED_DISABLING_PS_ACCESS,
  GRR_CAMPED_MSC_CHANGE,
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  GRR_X2G_CCO_ABORTED,
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
  GRR_SGLTE_SWITCHING_TO_GSM_ONLY,
  GRR_SGLTE_X2G_RESEL,
  GRR_STATE_MAX
}grr_state_t;

/*********************************/
/* GPRS NC & EXT meas FSM states */
/*********************************/
typedef enum
{
  NC_MEAS_NULL,
  NC_MEAS_ACQUIRE,
  NC_MEAS_SEND,
  NC_MEAS_SUSPEND
} nc_meas_state_T;

typedef enum
{
  EXT_MEAS_NULL,
  EXT_MEAS_ACQUIRE,
  EXT_MEAS_SEND,
  EXT_MEAS_MAC_WAIT
} ext_meas_state_T;

typedef enum
{
  RR_CP_INACTIVE,
  RR_CP_SENDING_RANDOM_ACCESS,
  RR_CP_MAX_RETRIES_ATTEMPTED,
  RR_CP_AWAITING_PHYSICAL_LINK,
  RR_CP_AWAITING_LINK_ESTABLISHMENT,
  RR_CP_LINK_ESTABLISHED,
  RR_CP_ASSIGNMENT_REJECTION_RECEIVED,
  RR_CP_DISCONNECT_PHYSICAL_LINK,
  RR_CP_IA_ABORTED,
  RR_CP_PCCO_FAILED,
  RR_CP_T3146_EXPIRED,
  RR_CP_T3146_EXPIRED_AFTER_REJECT,
  RR_CP_STATE_COUNT
} rr_con_est_state_T;

typedef enum
{
  NO_SAPI3_LINK,
  SAPI3_LINK_PENDING,
  SAPI3_LINK_ESTABLISHED
} SAPI3_link_state_T;

typedef enum
{
  RR_G2W_HO_INACTIVE,
  RR_G2W_HO_AWAITING_L2_SUSPENSION,
  RR_G2W_HO_GOING_INACTIVE,
  RR_G2W_HO_AWAITING_RRC_RESPONSE,
  RR_G2W_HO_GOING_ACTIVE,
  RR_G2W_HO_AWAITING_RRC_ABORT_COMPLETION,
  RR_G2W_HO_AWAITING_COMPLETION
} rr_g2w_ho_state_T;

typedef enum
{
  WAIT_FOR_LIST_REQUEST,
  WAIT_FOR_ALTERNATE_CONFIRMATION,
  WAIT_FOR_LIST_CONFIRMATION,
  WAIT_FOR_LAI_INFORMATION
} list_state_T;

#ifdef FEATURE_CGPS_UMTS_CP_GSM
typedef enum
{
  RR_APDU_ID_RRLP
} rr_apdu_id_T;
#endif /* FEATURE_CGPS_UMTS_CP_GSM */

/* the reselection PSCAN cell list size can be max BA list length + 1 */
#define RR_MAX_RES_PSCAN_CELL_LIST_SIZE (BA_MAX_LEN + 1)

/**
 * List of cells that are being tried for a reselection power scan
 */
typedef struct
{
  /**
   * Number of entries in the list (0 if empty)
   */
  int   no_of_entries;

  /**
   * Set to TRUE the first time the power scan list is populated
   */
  boolean valid_data;

  /**
   * Up to MAX ARFCN values.
   */
  ARFCN_T  cell_list[RR_MAX_RES_PSCAN_CELL_LIST_SIZE];
} rr_reselection_pscan_cell_list_T;

typedef struct
{
  byte no_of_entries;
  LAI_T location_area_identification[MAX_NO_AVAILABLE_LAIS];
} LAI_list_T;

// list of frequencies (ARFCN)
typedef struct
{
  byte no_of_entries;
  word BCCH_ARFCN[BA_MAX_LEN];
} BCCH_allocation_T;

// the reason the RR is performing a cell selection
typedef enum
{
  // RR is performing an autonomous cell selection
  RR_AUTONOMOUS_CELL_SELECTION,

  // RR is performing a PLMN selection
  RR_PLMN_SELECTION,

  RR_PERFORM_REEST,

  // RR is performing a W2G Service Redirection procedure
  RR_W2G_SERVICE_REDIRECTION,

  // RR is performing a W2G Service Redirection for an Emergency Call
  RR_W2G_SERVICE_REDIRECTION_EMERGENCY_CALL

} rr_search_reason_T;

// Stores information regarding the PLMN that has been requested by the
// MM, and other cell selection parameters currently being used by the RR
typedef struct
{
  // PLMN_id of the requested PLMN (stored internally in LAI IE format).
  // Only applicable while NOT in any cell selection mode
  PLMN_id_T requested_PLMN_id;

  // PLMN_id of the equivalent PLMN (stored internally in LAI IE format).
  PLMN_id_T equivalent_PLMN_id;

  // set to true if a frequency list is being used during cell selection search
  boolean valid_frequency_list;

  // list of frequencies being used during cell selection search
  BCCH_allocation_T BCCH_allocation;

  // the reason the RR is performing the current cell selection operation
  rr_search_reason_T search_reason;

  // indicates whether an immediate or background search was requested
  mm_rr_list_search_mode_T search_mode;

  //saves last measurement done so that when we can calculate C1 when we get the full SI again
  byte RXLEV_average;

  /* requested service domain in the last request from MM */
  sys_srv_domain_e_type          service_domain;

  /* keep the network select mode sent down by MM */
  sys_network_selection_mode_e_type network_select_mode;

  sys_rat_pri_list_info_s_type rat_pri_list_info;

  rr_mm_service_req_cause_e_type service_req_cause;

  /* an EHPLMN list being used during cell selection search if it TRUE */
  boolean is_ehplmn_to_be_camped;
} rr_PLMN_request_details_T;

typedef rr_plmn_list_req_T rr_PLMN_list_req_info_T;

typedef struct
{
  boolean PLMN_id_valid;
  byte    PLMN_id[3];
} required_PLMN_T;

typedef struct
{
  boolean valid_channel;
  int C1;
  word BCCH_ARFCN;
  byte RXLEV_average;
} medium_priority_channel_T;


typedef enum
{
  NORMAL_RESELECTION,
  OTHER_RESELECTION_REASON
} reselection_reason_T;

typedef struct
{
  BSIC_T BSIC;
  ARFCN_T ARFCN;
} cell_description_T;

typedef struct nas_message_entry
{
  struct nas_message_entry *next_message;
  rr_data_req_T        nas_message;
} rr_w2g_nas_message_entry_T;

typedef enum
{
  RR_SI2TER_UNKNOWN,
  RR_SI2TER_NOT_EXPECTED,
  RR_SI2TER_EXPECTED
} rr_si2ter_expected_T;

typedef enum
{
  REPORT_TYPE_ENHANCED,
  REPORT_TYPE_N0RMAL,
  REPORT_TYPE_UNKNOWN
}report_type_T;

/**
 * nc_meas_params_mods_t
 *
 * Superset of all NC Measurement Parameters that may be received in
 * any of:
 *
 * - SI2quater;
 * - SI13
 * - PSI5;
 * - Packet Measurement Order;
 * - Packet Cell Change Order;
 */
typedef struct
{
  boolean valid;
  uint8 network_control_order;
  boolean nc_period_valid;
  uint8 nc_non_drx_period;
  uint8 nc_reporting_period_i;
  uint8 nc_reporting_period_t;

} nc_meas_params_mods_t;

/**
 * gprs_meas_params_mods_t
 *
 * Superset of all GPRS Measurement Parameters that may be received in
 * any of:
 *
 * - SI2quater;
 * - PSI5;
 * - Packet Measurement Order;
 * - Packet Cell Change Order;
 */
typedef struct
{
  boolean valid;

  boolean pmo_ind_valid;
  uint8 pmo_ind;

  boolean report_type_valid;
  report_type_T report_type;

  boolean reporting_rate_valid;
  uint8 reporting_rate;

  boolean invalid_bsic_reporting_valid;
  uint8 invalid_bsic_reporting;

  boolean multiband_reporting_valid;
  uint8 multiband_reporting;

  boolean serving_band_reporting_valid;
  uint8 serving_band_reporting;

  boolean scale_ord_valid;
  uint8 scale_ord;

  boolean reporting_900_valid;
  uint8 reporting_900_offset;
  uint8 reporting_900_threshold;

  boolean reporting_1800_valid;
  uint8 reporting_1800_offset;
  uint8 reporting_1800_threshold;

  boolean reporting_450_valid;
  uint8 reporting_450_offset;
  uint8 reporting_450_threshold;

  boolean reporting_1900_valid;
  uint8 reporting_1900_offset;
  uint8 reporting_1900_threshold;

  boolean reporting_850_valid;
  uint8 reporting_850_offset;
  uint8 reporting_850_threshold;
} gprs_meas_params_mods_t;

/**
 * gsm_3g_meas_params_mods_t
 *
 * Superset of all GSM 3G Measurement Parameters that may be received in
 * any of:
 *
 * - SI2quater;
 * - SI2ter;
 * - PSI3quater;
 * - Packet Measurement Order;
 * - Packet Cell Change Order;
 */
typedef struct
{
  boolean valid;

  boolean _3G_BA_IND_valid;
  uint8 _3G_BA_IND;

  boolean Qsearch_I_valid;
  uint8 Qsearch_I;

  boolean Qsearch_C_Initial_valid;
  uint8 Qsearch_C_Initial;

  boolean FDD_info_valid;
  uint8 FDD_Qoffset;
  uint8 FDD_REP_QUANT;
  uint8 FDD_MULTIRAT_REPORTING;

  boolean FDD_Qmin_valid;
  uint8 FDD_Qmin; /* where does this come from? */

  boolean TDD_info_valid;
  uint8 TDD_Qoffset;
  uint8 TDD_MULTIRAT_REPORTING;

  boolean FDD_additional_MPs_valid;
  uint8 FDD_Qmin_offset;
  uint8 FDD_RSCPmin;
} gsm_3g_meas_params_mods_t;

/**
 * gprs_3g_meas_params_mods_t
 *
 * Superset of all GPRS 3G Measurement Parameters that may be received in
 * any of:
 *
 * - SI2quater;
 * - PSI5;
 * - Packet Measurement Order;
 * - Packet Cell Change Order;
 */
typedef struct
{
  boolean valid;

  boolean Qsearch_P_valid;
  uint8 Qsearch_P;

  boolean _3G_SEARCH_PRIO_valid;
  uint8 _3G_SEARCH_PRIO;

  boolean FDD_valid;
  uint8 FDD_GPRS_Qoffset;
  uint8 FDD_Qmin;

  boolean TDD_valid;
  uint8 TDD_GPRS_Qoffset;

  boolean FDD_reporting_valid;
  uint8 FDD_REP_QUANT;
  uint8 FDD_MULTIRAT_REPORTING;

  boolean FDD_off_thresh_valid;
  uint8 FDD_REPORTING_OFFSET;
  uint8 FDD_REPORTING_THRESHOLD;

  boolean TDD_reporting_valid;
  uint8 TDD_MULTIRAT_REPORTING;

  boolean TDD_off_thresh_valid;
  uint8 TDD_REPORTING_OFFSET;
  uint8 TDD_REPORTING_THRESHOLD;

  boolean CDMA2000_reporting_valid;
  uint8 CDMA2000_MULTIRAT_REPORTING;

  boolean CDMA2000_off_thresh_valid;
  uint8 CDMA2000_REPORTING_OFFSET;
  uint8 CDMA2000_REPORTING_THRESHOLD;

  boolean FDD_additional_MPs_valid;
  byte      FDD_Qmin_offset; /* default is 0 : 0db */
  byte      FDD_RSCPmin;       /* default is 7 : -101dbm */
  
} gprs_3g_meas_params_mods_t;


#define MAX_GPRS_BSIC_DESCRIPTION_ENTRIES_PER_INSTANCE  22   /* limited by max length of SI2q rest octects */
#define MAX_BSIC_PER_FREQUENCY   MAX_GPRS_BSIC_DESCRIPTION_ENTRIES_PER_INSTANCE

typedef struct
{
  byte size;
  byte bsic[MAX_BSIC_PER_FREQUENCY];
} gprs_bsic_description_instance_entry_t;

typedef struct
{
  byte ba_index_start_bsic;
  byte entry_count;
  gprs_bsic_description_instance_entry_t entries[MAX_GPRS_BSIC_DESCRIPTION_ENTRIES_PER_INSTANCE];
} gprs_bsic_description_instance_t;


/**
 * Constants that indicate whether SI messages are required
 * or not (or it is not yet known whether required or not).
 * Mostly used for multiband BA list construction
 */
typedef enum
{
  /**
   * It is not yet known whether the SI is required or not
   */
  RR_SI_REQUIRE_UNKNOWN = 0,

  /**
   * SI is required (RR should decode it)
   */
  RR_SI_REQUIRED        = 1,

  /**
   * SI is not required (RR should not wait for it since it may
   * not be sent by the network)
   */
  RR_SI_NOT_REQUIRED    = 2
} rr_si_require_T;

typedef enum
{
  RR_SI_OUT_OF_DATE,
  RR_SI_HANDOVER,
  RR_SI_INCONSISTENT,
  RR_SI_LAC_CHANGED
} rr_invalidate_si_reason_t;

typedef enum
{
  RR_CA_NOT_RECEIVED,
  RR_CA_RECEIVED,
  RR_CA_RECEIVED_BUT_INVALID
} rr_ca_status_t;

typedef struct
{
  byte no_of_entries;
  LAI_T lai[MAX_FORBIDDEN_LAIS];
} forbidden_lai_list_T;

typedef enum
{
  NORMAL_CALLS_ALLOWED,
  EMERGENCY_CALLS_ALLOWED,
  ALL_CALLS_ALLOWED,
  NO_CALLS_ALLOWED
} access_result_T;

typedef struct
{
  boolean BA_IND;
  byte    no_of_entries;
  ARFCN_T BCCH_ARFCN[BA_MAX_LEN];
} BCCH_frequency_list_T;

/**
 * Indicates the type/priority of the cell
 */
typedef enum
{
  RR_LOW_PRIORITY_CELL,
  RR_HIGH_PRIORITY_CELL,
  RR_CELL_BARRED
} rr_cell_type_T;

typedef struct
{
  byte no_of_entries;
  byte MA_C[CA_MAX_LEN];
} mobile_allocation_T;

typedef struct
{
  boolean PWRC;
  byte    DTX;
  byte    RADIO_LINK_TIMEOUT;
} cell_options_T;

typedef struct
{
  byte    max_retrans;
  byte    tx_integer;
  boolean REESTABLISHMENT_ALLOWED;
  boolean CELL_BAR_ACCESS;
  byte    access_control[2];
} RACH_control_parameters_T;

// Control Channel Description information
typedef struct
{
  boolean ATT;
  byte    BS_AG_BLKS_RES;
  byte    BS_CC_CHANS;
  byte    BS_CCCH_SDCCH_COMB;
  byte    BS_PA_MFRMS;
  byte    T3212_timeout;
  /**
   * MSC Revision Number. 0-R'98 or earlier. 1-R'99 or later.
   */
  byte    MSCR;
} control_channel_description_T;

/**
 * Contains information used for cell selection/reselection purposes
 */
typedef struct
{
  byte              RXLEV_average;
  int               C1;
  int               C2;
  boolean           measurements_valid;
  rr_timer_id_T     five_second_timer_id;
} reselection_info_T;

typedef struct
{
  byte    CELL_RESELECT_HYSTERESIS;
  byte    MS_TXPWR_MAX_CCH;
  byte    RXLEV_ACCESS_MIN;
  boolean NECI;
  boolean ACS;
  byte    CELL_RESELECT_PARAM_IND;
  byte    CELL_BAR_QUALIFY;
  byte    CELL_RESELECT_OFFSET;
  byte    TEMPORARY_OFFSET;
  byte    PENALTY_TIME;
  boolean power_offset_valid;
  byte    POWER_OFFSET;
} cell_selection_parameters_T;


/* SI2quater_POSITION values */
typedef enum
{
  SI2Q_POSITION_UNKNOWN,
  SI2Q_POSITION_NONE,
  SI2Q_POSITION_BCCH_Norm,
  SI2Q_POSITION_BCCH_Ext
}two_quater_position_T;

/*
  Max utran cells that will be possible within one instance of an MI or SI2quater
  assuming only 1 frequency (with zero's for others; wherever possible) = 11 cells.
  Adding a 12th cell will cause the byte stream to exceeds the 20 octet limit on the
  rest octets. Set the value of MAX_UTRAN_CELLS_PER_INSTANCE to 16 to be on the
  safer side.
*/
#define MAX_INSTANCES       16
#define MAX_UTRAN_CELLS_PER_INSTANCE  16
#define MAX_UTRAN_CELLS_PER_FREQUENCY 32
#define MAX_UTRAN_FREQ_WITH_CELLS      4
#define MAX_UTRAN_FREQ_ONLY           MAX_UTRAN_FREQ
#define MAX_UTRAN_FREQ                MAX_RR_MPH_FREQ_UPDATE_LIST
#define MAX_UTRAN_TDD_FREQ            MAX_UTRAN_TDD_MPH_FREQ_UPDATE_LIST
#define MAX_UTRAN_CELLS               MAX_RR_MPH_CELL_UPDATE_LIST
#define MAX_MI_SI2Q_INSTANCES         MAX_SI2QUATER_INSTANCES

#define SOURCE_SI2_QUATER 0
#define SOURCE_MEAS_INFO  (1-SOURCE_SI2_QUATER)
#define SOURCE_UNKNOWN    0xFF

#define REPORT_ON_3G_CELLS 31
#define MAX_MEAS_REP_SLOTS 6
#define QSEARCH_I_ALWAYS 7

/* Max number of W measurements that can be stored in ded mode.
 * 3gpp says we can get upto 96 W cells in 3G neighbour cell list,
 * but we have choosen 32 based on real network data. */
#define MAX_WCDMA_STORED_DED_MEASUREMENTS 32

#define MAX_MI_BSIC 128
#define MAX_REP_PRIO_BITS 4

#define RR_NUM_SACCH_BLOCKS 10

/* The 3G dedicated cell list */
typedef wcdma_reselection_freq_list_T wcdma_freq_only_list_T;

typedef enum
{
  FDD_REP_QUANT_RSCP,
  FDD_REP_QUANT_Ec_No
} fdd_rep_quant_T;

typedef enum
{
  FREQ_SC,
  FREQ_ONLY_REPORTED,
  FREQ_ONLY_UNREPORTED
} wcdma_freq_T;

/* parameters common to SI2quater and MI */
typedef struct
{
  boolean         BA_IND;
  boolean         BA_IND_3G;
  boolean         MP_CHANGE_MARK;
  byte            INDEX;
  byte            COUNT;
  boolean         WCDMA_3G_NC_Desc_PI;
  boolean         UTRAN_FDD_Desc_PI;
  boolean         Index_Start_3G_PI;
  byte            Index_Start_3G; /* Set to 0 if the parameter was not present */
  boolean         Absolute_Index_Start_EMR_PI;
  byte            Absolute_Index_Start_EMR;
  byte            num_repeated_FDD_info;
  uint8           utran_tdd_bandwidth;
  byte            num_repeated_TDD_info;
  boolean         UTRAN_TDD_Desc_PI;
} SI2q_MI_params_T;


#define CCN_BITMAP_LENGTH 16

typedef struct
{
  uint8 number_cells;
  uint8 ccn_supported[CCN_BITMAP_LENGTH];
} gprs_ccn_support_desc_t;


typedef struct
{
  boolean valid_flag;
  boolean ccn_active;
  boolean ccn_support_flag;
  csn_ccn_support_desc_t ccn_supported_desc;
} pmo_pcco_nacc_params_t;


typedef struct
{
  boolean Frequency_Scrolling;
  byte BSIC;
} MI_BSIC_T;

typedef struct
{
  boolean BA_Index_Start_BSIC_PI;
  byte BA_Index_Start_BSIC;
  byte BSIC;
  byte Number_Remaining_BSIC;
  MI_BSIC_T mi_bsic[MAX_MI_BSIC];
} BSIC_Desc_T;

#if defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)
typedef enum
{
  RR_RESEL_PRIORITY_UNDEFINED = -1,
  RR_RESEL_PRIORITY_0,
  RR_RESEL_PRIORITY_1,
  RR_RESEL_PRIORITY_2,
  RR_RESEL_PRIORITY_3,
  RR_RESEL_PRIORITY_4,
  RR_RESEL_PRIORITY_5,
  RR_RESEL_PRIORITY_6,
  RR_RESEL_PRIORITY_7,
  RR_RESEL_PRIORITY_MAX
} rr_resel_priority_e;

#endif /*FEATURE_LTE || FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE
typedef struct
{
  /* The PLMN on which the Individual Priorities were received. */
  sys_plmn_id_s_type                 plmn;

  /* Default GERAN priority is used if available.  This differs from default
  UTRAN / default EUTRAN priorities, which are only used if a frequency-
  specific value is not available.  Default GERAN priority always takes
  precedence of frequency-specific values. */
  boolean                            default_geran_priority_valid;
  rr_resel_priority_e                default_geran_priority;

  /* Default UTRAN priority is used if a priority for a specific UTRAN
  frequency is not available.  Frequency-specific priority should be used if
  available. */
  boolean                            default_utran_priority_valid;
  rr_resel_priority_e                default_utran_priority;

  /* Default EUTRAN priority is used if a priority for a specific EUTRAN
  frequency is not available.  Frequency-specific priority should be used if
  available. */
  boolean                            default_eutran_priority_valid;
  rr_resel_priority_e                default_eutran_priority;

  lte_irat_dedicated_priority_list_s dedicated_priority_list;
} rr_individual_priorities_t;
#endif /* FEATURE_LTE */

/**
  @brief 3G Frequency List.
*/
#define RR_UTRAN_FREQUENCY_LIST_SIZE 32

typedef enum
{
  RR_UTRAN_FREQUENCY_LIST_ENTRY_TYPE_UNDEFINED,
  RR_UTRAN_FREQUENCY_LIST_ENTRY_TYPE_FDD,
  RR_UTRAN_FREQUENCY_LIST_ENTRY_TYPE_TDD,
  RR_UTRAN_FREQUENCY_LIST_ENTRY_TYPE_MAX
} rr_utran_frequency_list_entry_type_e;

typedef struct
{
  rr_utran_frequency_list_entry_type_e type;
  uint16 uarfcn;
} rr_utran_frequency_list_entry_t;

typedef struct
{
  rr_utran_frequency_list_entry_t entries[RR_UTRAN_FREQUENCY_LIST_SIZE];
} rr_utran_frequency_list_t;

 
/* Bitmap of 504 entries. */
#define RR_PCID_GROUP_BITMAP_LEN 63

/* The maximum valid PCID */
#define RR_LTE_PCID_MIN 0U
#define RR_LTE_PCID_MAX 503U

/*! Utility macro for setting a presence bit in the PCID group */
#define RR_PCID_GROUP_IE_SET(bitmap, pcid) \
  ( (bitmap)[(pcid)/8] |= (1<<((pcid) % 8)))

/*! Utility macro for checking a presence bit in the PCID group */
#define RR_PCID_GROUP_IE_GET(bitmap, pcid) \
  (( (bitmap)[(pcid)/8] & (1<<((pcid) % 8))) ? TRUE : FALSE )

typedef struct
{
  /* There are 504 possible PCIDs */
  uint8 bitmap[RR_PCID_GROUP_BITMAP_LEN];
} rr_pcid_group_ie_t;

typedef struct
{
  lte_earfcn_t  earfcn;
  uint8   measurement_bandwidth;
} rr_lte_frequency_t;

#ifdef FEATURE_LTE
typedef struct
{
  rr_lte_frequency_t  frequency;
  rr_resel_priority_e priority;
  uint8               thresh_high;
  uint8               thresh_low;
  uint8               qrxlevmin;
  rr_pcid_group_ie_t  not_allowed_cells;
  boolean             measurement_control; 
} rr_lte_neighbour_list_entry_t;
#endif /* FEATURE_LTE */


typedef enum
{
  RR_EUTRAN_REPORTING_LENGTH_6_BIT,
  RR_EUTRAN_REPORTING_LENGTH_3_BIT
} rr_eutran_reporting_length_e;

#define MAX_EUTRAN_NEIGHBOUR_CELLS_EARFCNS 16 
typedef struct
{
  uint8              eutran_frequency_list_count;
  rr_lte_frequency_t eutran_frequency_list[MAX_EUTRAN_NEIGHBOUR_CELLS_EARFCNS];

  boolean            eutran_priority_valid;
  uint8              eutran_priority;

  uint8              thresh_eutran_high;

  boolean            thresh_eutran_low_valid;
  uint8              thresh_eutran_low;

  boolean            eutran_qrxlevmin_valid;
  uint8              eutran_qrxlevmin;

} rr_si2quater_ro_eutran_neighbour_cells_t;
 
typedef rr_si2quater_ro_eutran_neighbour_cells_t rr_eutran_neighbour_cells_t;

typedef struct
{
  uint8 length;
  uint8 pattern;
  uint8 sense;
} rr_pcid_pattern_t;

#define MAX_NOT_ALLOWED_CELLS_PCID_COUNT         16
#define MAX_NOT_ALLOWED_CELLS_PCID_PATTERN_COUNT 16
typedef struct
{
  uint8  pcid_count;
  uint16 pcid[MAX_NOT_ALLOWED_CELLS_PCID_COUNT];

  boolean pcid_bitmap_group_valid;
  uint8   pcid_bitmap_group;

  uint8             pcid_pattern_count;
  rr_pcid_pattern_t pcid_patterns[MAX_NOT_ALLOWED_CELLS_PCID_PATTERN_COUNT];

} rr_si2quater_ro_not_allowed_cells_t;

#define MAX_EUTRAN_NOT_ALLOWED_CELLS_EUTRAN_FREQUENCY_INDICES 8
typedef struct
{
  rr_pcid_group_ie_t not_allowed_cells;
  uint8              eutran_frequency_indices_count;
  uint8              eutran_frequency_indices[MAX_EUTRAN_NOT_ALLOWED_CELLS_EUTRAN_FREQUENCY_INDICES];
} rr_si2quater_ro_eutran_not_allowed_cells_t;

#define MAX_PCID_TO_TA_MAPPING_PCID_TO_TA_MAPPING_GROUPS 8
typedef struct
{
  uint8 pcid_to_ta_mapping_group_count;
  rr_pcid_group_ie_t pcid_to_ta_mapping_group[MAX_PCID_TO_TA_MAPPING_PCID_TO_TA_MAPPING_GROUPS];
} rr_si2quater_ro_pcid_to_ta_mapping_t;

#define MAX_EUTRAN_PCID_TO_TA_MAPPING_EUTRAN_FREQUENCY_INDICES 8
typedef struct
{
  rr_si2quater_ro_pcid_to_ta_mapping_t pcid_to_ta_mapping;
  uint8 eutran_frequency_indices_count;
  uint8 eutran_frequency_indices[MAX_EUTRAN_PCID_TO_TA_MAPPING_EUTRAN_FREQUENCY_INDICES];
} rr_si2quater_ro_eutran_pcid_to_ta_mapping_t;

#define MAX_REPEATED_EUTRAN_NEIGHBOUR_CELLS    16
#define MAX_REPEATED_EUTRAN_NOT_ALLOWED_CELLS  16
#define MAX_REPEATED_EUTRAN_PCID_TO_TA_MAPPING 16

#ifdef FEATURE_LTE
typedef struct
{
  uint32                                        count;
  rr_lte_neighbour_list_entry_t                 entries[MAX_RR_EUTRAN_NEIGHBOUR_LIST_SIZE];
  uint8                                         eutran_pcid_to_ta_mapping_count;
  rr_si2quater_ro_eutran_pcid_to_ta_mapping_t   eutran_pcid_to_ta_mapping[MAX_REPEATED_EUTRAN_PCID_TO_TA_MAPPING];
} rr_lte_neighbour_list_t;
#endif /* FEATURE_LTE */

typedef enum
{
	RR_EUTRAN_REP_QUANT_RSRP,
	RR_EUTRAN_REP_QUANT_RSRQ
} rr_eutran_rep_quant_e;

 
typedef struct
{
  uint8 reporting_threshold; /*< Only provided when 6-bit reporting is enabled, default value (used when 3-bit reporting is enabled) is zero. */
  uint8 reporting_threshold_2;
  uint8 measurement_report_offset; /*!< Only used when 3-bit reporting is enabled, default value is zero. */
  uint8 reporting_offset;
} rr_eutran_meas_params_per_mode_t;

typedef struct
{
  uint8                 qsearch_c;
  rr_eutran_rep_quant_e rep_quant;
  uint8                 multirat_reporting;

  rr_eutran_meas_params_per_mode_t fdd;
  rr_eutran_meas_params_per_mode_t tdd;

  uint8   reporting_granularity; /*!< Only used when 3-bit reporting is enabled, default value is zero. */

} rr_eutran_meas_params_t;

/**
  @defgroup eutran_gprs_measurement_parameters E-UTRAN GPRS Measurement Parameters Description
  @{
*/

/**
  @ingroup eutran_gprs_measurement_parameters
*/
typedef struct
{
  uint8 reporting_threshold;
  uint8 reporting_threshold_2;
  uint8 reporting_offset;
} rr_gprs_eutran_meas_params_per_mode_t;

/**
  @ingroup eutran_gprs_measurement_parameters
*/
typedef struct
{
  uint8 qsearch_p;
  uint8 rep_quant;
  uint8 multirat_reporting;

  rr_gprs_eutran_meas_params_per_mode_t fdd;
  rr_gprs_eutran_meas_params_per_mode_t tdd;

} rr_gprs_eutran_meas_params_t;
/** @} */


typedef struct
{
  boolean eutran_ccn_active;
  boolean eutran_start;
  boolean eutran_stop;

  /* E-UTRAN Measurement Parameters Description */
  rr_eutran_meas_params_t                               * eutran_meas_params;

  /* GPRS E-UTRAN Measurement Parameters Description */
  rr_gprs_eutran_meas_params_t                          * gprs_eutran_meas_params;

  uint8                                                   eutran_neighbour_cells_count;
  rr_si2quater_ro_eutran_neighbour_cells_t              * eutran_neighbour_cells[MAX_REPEATED_EUTRAN_NEIGHBOUR_CELLS];

  uint8                                                   eutran_not_allowed_cells_count;
  rr_si2quater_ro_eutran_not_allowed_cells_t            * eutran_not_allowed_cells[MAX_REPEATED_EUTRAN_NOT_ALLOWED_CELLS];

  uint8                                                   eutran_pcid_to_ta_mapping_count;
  rr_si2quater_ro_eutran_pcid_to_ta_mapping_t           * eutran_pcid_to_ta_mapping[MAX_REPEATED_EUTRAN_PCID_TO_TA_MAPPING];
} rr_si2quater_ro_eutran_params_desc_t;

typedef struct
{
  csn_gsm_priority_params_ie_t         * serving_cell_priority_params;
  uint8                                  utran_start;
  uint8                                  utran_stop;
  csn_utran_priority_params_ie_t       * utran_priority_params;
  rr_si2quater_ro_eutran_params_desc_t                * eutran_params_desc;
} rr_si2quater_ro_priority_and_eutran_params_desc_t;

typedef struct
{
  void * null;
} rr_si2quater_ro_utran_csg_desc_t;

typedef struct
{
  void * null;
} rr_si2quater_ro_eutran_csg_desc_t;

/*rel 9 decode*/
#define  MAX_EUTRAN_FREQUENCY_INDEX_COUNT 8

  typedef struct
  {
    uint8 eutran_frequency_index_count;  /*(3 bit field)*/

    uint8 eutran_frequency_index[MAX_EUTRAN_FREQUENCY_INDEX_COUNT];

    boolean RSRP_valid;                /*reselection based on target cell RSRP*/
    uint8   eutran_qmin;               /*(4 bit field)*/

    boolean RSRQ_valid;                /*reselection based on target cell RSRQ*/
    uint8 thresh_eutran_high;           /*(5 bit field)*/

    boolean thresh_eutran_low_q_valid;
    uint8 thresh_eutran_low_q;         /*(5 bit field)*/

    boolean eutran_qqualmin_valid;
    uint8 eutran_qqualmin;             /*(4 bit field)*/

    boolean eutran_rsrpmin_valid;
    uint8 eutran_rsrpmin;              /*(5 bit field)*/

  }
  rr_si2quater_ro_enh_cell_resel_par_des_t;


  typedef struct
  {
    boolean utran_csg_fdd_reporting_threshold_valid;
    uint8   utran_csg_fdd_reporting_threshold;        /*(3 bit field)*/
    uint8   utran_csg_fdd_reporting_threshold_2;      /*(6 bit field)*/

    boolean utran_csg_tdd_reporting_threshold_valid;
    uint8   utran_csg_tdd_reporting_threshold;        /*(3 bit field)*/
  } 
  rr_si2quater_ro_utran_csg_cells_reporting_des_t;

  typedef struct
  {
    boolean eutran_csg_fdd_reporting_threshold_valid;
    uint8   eutran_csg_fdd_reporting_threshold;        /*(3 bit field)*/
    uint8   eutran_csg_fdd_reporting_threshold_2;      /*(6 bit field)*/

    boolean eutran_csg_tdd_reporting_threshold_valid;
    uint8   eutran_csg_tdd_reporting_threshold;        /*(3 bit field)*/
    uint8   eutran_csg_tdd_reporting_threshold_2;      /*(6 bit field)*/
  } 
  rr_si2quater_ro_eutran_csg_cells_reporting_des_t;

  typedef struct
  {
    boolean                                           utran_csg_cells_reporting_des_vaild;
    rr_si2quater_ro_utran_csg_cells_reporting_des_t   utran_csg_cells_reporting_des;

    boolean                                           eutran_csg_cells_reporting_des_vaild;
    rr_si2quater_ro_eutran_csg_cells_reporting_des_t  eutran_csg_cells_reporting_des;
  }
  rr_si2quater_ro_csg_cell_reporting_des_t;

/* parameters from SI2quater only */
typedef struct
{
  boolean         WCDMA_3G_MP_Desc_PI;
  byte            QSearch_I;
  byte            QSearch_C_Initial;
  boolean         FDD_Info_PI;
  byte            FDD_Qmin;
  byte            FDD_Qoffset; /* default is 0db which is 8 */
  byte            FDD_MULTIRAT_REPORTING;
  fdd_rep_quant_T FDD_REP_QUANT;
  boolean         MP_Desc_PI;
  report_type_T   REPORT_TYPE; /* default is normal */
  byte            SERVING_BAND_REPORTING;

  nc_meas_params_mods_t              nc_meas_params;
  gprs_meas_params_mods_t            gprs_meas_params;
  gprs_3g_meas_params_mods_t         gprs_3g_meas_params;
#ifdef FEATURE_LTE
  gprs_bsic_description_instance_t * gprs_bsic_description;
#else
  gprs_bsic_description_instance_t   gprs_bsic_description;
#endif /* FEATURE_LTE */

  boolean                   ccn_support_flag;
  csn_ccn_support_desc_t    ccn_support_desc;

  boolean   FDD_additional_MPs_PI;
  byte      FDD_Qmin_offset; /* default is 0 : 0db */
  byte      FDD_RSCPmin;     /* default is 7 : -101dbm */

  boolean   FDD_additional_MPs_2_PI;
  boolean   fdd_reporting_threshold_2_valid;
  byte      FDD_REPORTING_THRESHOLD_2;

  boolean   three_g_ccn_active_valid;
  byte      three_g_ccn_active;

  /* Additions in Rel-7 */
  boolean   gsm_700_reporting_params_valid;
  uint8     gsm_700_reporting_offset;
  uint8     gsm_700_reporting_threshold;

  boolean   gsm_810_reporting_params_valid;
  uint8     gsm_810_reporting_offset;
  uint8     gsm_810_reporting_threshold;

  /* Additions in Rel-8 */
  rr_si2quater_ro_priority_and_eutran_params_desc_t * priority_and_eutran_params_desc;
  rr_si2quater_ro_utran_csg_desc_t                  * utran_csg_desc;
  rr_si2quater_ro_eutran_csg_desc_t                 * eutran_csg_desc;

  boolean   TDD_Info_PI;
  byte      TDD_Qoffset; /* default is -90db which is 5 */
  byte      TDD_MULTIRAT_REPORTING;

/*rel 9 decode*/
  boolean enh_cell_resel_par_des_present;
  rr_si2quater_ro_enh_cell_resel_par_des_t   enh_cell_resel_par_des;

  boolean csg_cell_reporting_des_present;
  rr_si2quater_ro_csg_cell_reporting_des_t   csg_cell_reporting_des;



/*rel 10 decode*/
  boolean int_pwr_red_present;
  byte    int_pwr_red;
  boolean nc2_csg_pccn_permitted;

} SI2q_params_T;


typedef struct
{
  uint8           qsearch_c;

  fdd_rep_quant_T fdd_rep_quant;
  uint8           fdd_multirat_reporting;
  uint8           fdd_reporting_offset;
  uint8           fdd_reporting_threshold;
  uint8           fdd_reporting_threshold_2;

  uint8           tdd_multirat_reporting;
  uint8           tdd_reporting_offset;
  uint8           tdd_reporting_threshold;

} rr_utran_meas_params_t;

typedef struct
{
	rr_utran_meas_params_t meas_params;
} rr_dedicated_mode_utran_params_t;


typedef struct
{
  byte MULTIBAND_REPORTING_PI;
  byte MULTIBAND_REPORTING;
  byte SERVING_BAND_REPORTING_PI;
  byte SERVING_BAND_REPORTING;
  byte SCALE_ORD;
  byte REPORTING_900_PI;
  byte REPORTING_OFFSET_900;
  byte REPORTING_THRESHOLD_900;
  byte REPORTING_1800_PI;
  byte REPORTING_OFFSET_1800;
  byte REPORTING_THRESHOLD_1800;
  byte REPORTING_400_PI;
  byte REPORTING_OFFSET_400;
  byte REPORTING_THRESHOLD_400;
  byte REPORTING_1900_PI;
  byte REPORTING_OFFSET_1900;
  byte REPORTING_THRESHOLD_1900;
  byte REPORTING_850_PI;
  byte REPORTING_OFFSET_850;
  byte REPORTING_THRESHOLD_850;
} MP_Desc_T;


typedef struct
{
  boolean default_meas_control_valid;
  uint8   default_meas_control;

  /* Each bit position indicates if the corresponding bit in
  measurement_control_utran_bitmap is valid or not. */
  uint32  validity_bitmap;

  /* Measurement_Control_UTRAN */
  uint32  value_bitmap;

} rr_utran_meas_control_params_t;

typedef struct
{
  boolean                        utran_start;
  boolean                        utran_stop;
  boolean                        utran_meas_control_params_valid;
  rr_utran_meas_control_params_t utran_meas_control_params;

} rr_mi_utran_suppl_params_t;

typedef struct
{
  boolean default_meas_control_valid;
  uint8   default_meas_control;

  uint8   validity_bitmap;
  uint8   value_bitmap;

} rr_eutran_meas_control_params_t;

#define RR_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS 8U

typedef struct
{
  boolean eutran_start;
  boolean eutran_stop;

  boolean                                    meas_params_valid;
  rr_eutran_meas_params_t                    meas_params;

  uint32                                     neighbour_cells_count;
  rr_lte_frequency_t                         neighbour_cells[RR_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS];

  uint32                                     not_allowed_cells_count;
  rr_si2quater_ro_eutran_not_allowed_cells_t not_allowed_cells[RR_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS];

  boolean                                    meas_control_params_valid;
  rr_eutran_meas_control_params_t            meas_control_params;

} rr_mi_eutran_params_t;

typedef struct
{
  rr_lte_frequency_t frequency;
  rr_pcid_group_ie_t not_allowed_cells;
  boolean            measurement_control;

} rr_dedicated_mode_eutran_neighbour_list_entry_t;

#define RR_EUTRAN_NEIGHBOUR_LIST_LENGTH 8UL

typedef struct
{
  uint32                                          count;
  rr_dedicated_mode_eutran_neighbour_list_entry_t entries[RR_EUTRAN_NEIGHBOUR_LIST_LENGTH];
} rr_dedicated_mode_eutran_neighbour_list_t;

typedef struct
{
  rr_eutran_meas_params_t                   meas_params;
  rr_dedicated_mode_eutran_neighbour_list_t neighbour_list;
} rr_dedicated_mode_eutran_params_t;


/* The following structure holds the List of individual EARFCN and there associated LTE Physical Frequency */
typedef struct 
{
  uint8 count;
  uint32 earfcn[MAX_EUTRAN_NEIGHBOUR_CELLS_EARFCNS];
  uint16 lte_phy_freq[MAX_EUTRAN_NEIGHBOUR_CELLS_EARFCNS];
 
} rr_lte_freq_info_list_t;

/* The following structure holds the List of individual UARFCN and there associated UTRA Physical Frequency */
typedef struct 
{
  uint8 count;
  uint16 uarfcn[MAX_UTRAN_FREQ];
  uint16 wcdma_phy_freq[MAX_UTRAN_FREQ];
  
} rr_wcdma_freq_info_list_t;


/* Parameters from MI only */
typedef struct
{
  boolean PWRC;
  report_type_T   REPORT_TYPE; /* default value is normal(1) */
  boolean REPORTING_RATE;
  boolean INVALID_BSIC_REPORTING;

  boolean MI_3G_MP_Desc_PI;
  byte QSearch_C;
  fdd_rep_quant_T FDD_REP_QUANT;
  byte FDD_MULTIRAT_REPORTING_PI;
  byte FDD_MULTIRAT_REPORTING;
  byte FDD_REP_PI;
  byte FDD_REP_OFFSET;
  byte FDD_REP_THRESHOLD;

  boolean MI_3G_WAIT_PI;
  byte MI_3G_WAIT;
  boolean REPORT_PRIORITY_Desc_PI;
  uint32 REP_PRIORITY[MAX_REP_PRIO_BITS]; /* need 96 + 8 bits to store this */

  boolean BSIC_Desc_PI;
  BSIC_Desc_T BSIC_Desc;

  boolean MP_Desc_PI;
  MP_Desc_T MP_Desc;

  /* Additions in Release 5 */
  boolean fdd_reporting_threshold_2_valid;
  uint8   fdd_reporting_threshold_2;

  byte TDD_MULTIRAT_REPORTING_PI;
  byte TDD_MULTIRAT_REPORTING;
  byte TDD_REP_PI;
  byte TDD_REP_OFFSET;
  byte TDD_REP_THRESHOLD;

  rr_mi_utran_suppl_params_t utran_suppl_params;
  rr_mi_eutran_params_t      eutran_params;

} MI_params_T;


typedef struct
{
  word uarfcn;

  /* decode, sort and store the received cells per frequency per instance into this array */
  word decoded_and_sorted_cell_values[MAX_UTRAN_CELLS_PER_INSTANCE];

  byte TDD_Indic0;

  byte NR_OF_TDD_CELLS;

  /* For NR_OF_TDD_CELLS=0 or (>20) set num_cells=0; */
  byte num_cells;
} TDD_Info_T;

typedef struct
{
  word uarfcn;

  /* decode, sort and store the received cells per frequency per instance into this array */
  word decoded_and_sorted_cell_values[MAX_UTRAN_CELLS_PER_INSTANCE];

  byte FDD_Indic0;

  byte NR_OF_FDD_CELLS;

  /* For NR_OF_FDD_CELLS=0 or (17 to 31) set num_cells=0; */
  byte num_cells;
} FDD_Info_T;

typedef enum
{
  SI2Q_INSTANCE,
  MI_INSTANCE
}si2q_mi_instance_e_type;

typedef struct
{
  boolean valid_data; /* indicates whether this instance is filled with valid data */

  FDD_Info_T FDD_Info[MAX_UTRAN_FREQ];

  /* Information common across all instances of a set */
  SI2q_MI_params_T common_params;

  union{
    SI2q_params_T si2q_params;
    MI_params_T   mi_params;
  }si2q_mi;

  /* The changed value is used to track instances changed during one set
  of instances. It is initialized to 0 at the beginning and at every
  time a set of instances of SI2quater of MI have been received. */
  boolean changed;  /* indicates whether changed */

  TDD_Info_T TDD_Info[MAX_UTRAN_FREQ];

} SI2q_MI_instance_T;

typedef struct
{
  byte num_instances; /* current number of instances in the instance array */
  si2q_mi_instance_e_type si2q_or_mi;
  SI2q_MI_instance_T      instance[MAX_INSTANCES];
} instances_T;

typedef struct
{
  boolean                 valid_data;
  boolean                 SI2ter_MP_CHANGE_MARK;
  boolean                 SI2ter_3G_CHANGE_MARK;
  byte                    SI2ter_INDEX;
  byte                    SI2ter_COUNT;
  boolean                 WCDMA_3G_MP_Desc_PI;
  byte                    Qsearch_I;
  boolean                 WCDMA_3G_MP_FDD_Params_PI;
  byte                    FDD_Qoffset;
  byte                    FDD_Qmin;
  word                    FDD_INFO_PI;
  word                    FDD_arfcn;

  gprs_3g_meas_params_mods_t gprs_3g_meas_params;

  boolean   FDD_additional_MPs_valid;
  byte      FDD_Qmin_offset; /* default is 0 : 0db */
  byte      FDD_RSCPmin;       /* default is 7 : -101dbm */

  /* The changed value is used to track instances changed during one set of
  instances. It is initialized to 0 at the beginning and at every time a set
  of instances of SI2quater of MI have been received.
  */
  boolean                 changed;  /* indicates whether changed */

  boolean                 WCDMA_3G_MP_TDD_Params_PI;
  byte                    TDD_Qoffset;
  word                    TDD_INFO_PI;
  word                    TDD_arfcn;
  boolean                 TDD_BANDWIDTH_PI;
  byte                    TDD_BANDWIDTH;
} si2ter_rest_T;

#define RR_NUM_OCTETS_IN_EXT_BCCH_FREQ_LIST_SI2TER 16
#define MAX_SI2TER_INSTANCES                        8

typedef struct
{
  /* Buffer for the 16 mandatory Extended BCCH Frequency list octets in SI2ter */
  byte              first_ncell_desc_2[RR_NUM_OCTETS_IN_EXT_BCCH_FREQ_LIST_SI2TER];

  /* current number of instances in the instance array */
  byte              num_instances;

  /* instance information */
  si2ter_rest_T     instance[MAX_SI2TER_INSTANCES];
} si2ter_instances_T;

typedef struct
{
  boolean                 valid_data;
  wcdma_freq_only_list_T  wcdma_freq_only_list;
  byte                    Qsearch_I;
  byte                    FDD_Qoffset;
  byte                    FDD_Qmin;

  boolean                 FDD_additional_MPs_valid; /* need to know if it is valid */
  byte                    FDD_Qmin_offset;
  byte                    FDD_RSCPmin;

  wcdma_freq_only_list_T  tds_freq_only_list;
  byte                    TDD_Qoffset;
  boolean                 TDD_BANDWIDTH_valid;
  byte                    TDD_BANDWIDTH;
} wcdma_idle_neighbor_list_si2ter_T;

typedef enum
{
  RR_UTRAN_NONE,
  RR_UTRAN_FDD,
  RR_UTRAN_TDD
} rr_utran_rat_type_t;

typedef struct
{
  rr_utran_rat_type_t rat_type;
  word                uarfcn;
} rr_utran_arfcn_t;

/*Max size is 9, this is from choice of UTRAN TDD = 9, UTRAN FDD = 8*/
#define MAX_UTRAN_FREQ_UPDATE_LIST MAX_UTRAN_TDD_MPH_FREQ_UPDATE_LIST

typedef struct
{
  rr_utran_arfcn_t uarfcn[MAX_UTRAN_FREQ_UPDATE_LIST];
  byte num_of_frequencies;
} rr_utran_reselection_freq_list_T;

/*Struct to store frequencies with a scrambling code*/
typedef struct
{
   rr_utran_arfcn_t  uarfcn;
   word              cell_code; /*TDD- Cell Parameter ID (9bits), FDD- Scrambling Code (10bits)*/
   boolean           diversity;
   uint8             sync_case_tstd; /*TDD only*/
} rr_utran_cell_T;

typedef struct
{
  byte num_of_cells;
  rr_utran_cell_T  cell_list [MAX_RR_MPH_CELL_UPDATE_LIST];
} rr_utran_cell_list_T;

/* The 3G reselection list */
typedef struct
{
  /* indicates whether the information stored in this structure is valid */
  boolean                               valid_data;
  SI2q_MI_params_T                      common_params;
  SI2q_params_T                         si2q_params;

  /* 3G "stand alone" neighboring frequency list */
  rr_utran_reselection_freq_list_T      wcdma_freq_only_list;

  /* 3G neighboring cell list */
  rr_utran_cell_list_T                  wcdma_cell_list;

  boolean                               fdd_info;
  boolean                               tdd_info;
  uint8                                 Qsearch_P;
  uint8                                 FDD_GPRS_Qoffset;
  uint8                                 TDD_GPRS_Qoffset;

} utran_idle_neighbor_list_T;

typedef utran_idle_neighbor_list_T wcdma_neighbor_list_T;

typedef struct
{
  /* indicates whether the information stored in this structure is valid */
  boolean                               valid_data;
  SI2q_MI_params_T                      common_params;
  MI_params_T                           mi_params;

  /* 3G "stand alone" neighboring frequency list */
  rr_utran_reselection_freq_list_T      wcdma_freq_only_list;

  /* 3G neighboring cell list */
  rr_utran_cell_list_T                  wcdma_cell_list;

} wcdma_ded_neighbor_list_T;

/* 3G Neighbor cell list used to maintain the indices that are used for reporting
3G cells in the measurement report */

typedef struct
{
  boolean valid_data;

  /* there could be upto 8 frequency only entries in this table when constructed from the MI instances */
  wcdma_freq_T frequency_only;

  /* Wcdma cell */
  wcdma_cell_T cell;

  boolean MI_REP_PRIORITY;

  rr_utran_rat_type_t rat_type;
} wcdma_neighbor_cell_list_T;

/* Storage for dedicated measurements is defined below */
typedef struct
{
  /* Received Signal Strength */
  int16 RSSI;

  /* Wcdma cell */
  wcdma_cell_T cell;

  /* Received Signal Strength */
  int16 Ec_Nox2;

  /* Received Signal Strength (in dBm) */
  int16 RSCP;

  /* PN Offset of the WCDMA cell */
  word PN_Offset;

  /* Set to N everytime L1 returns a measurement report on this cell and decrement
  every SACCH block. When it reaches 0 set "known" to FALSE */
  int8 PN_position_validity_count;

} wcdma_ded_measurements_T;

typedef struct
{
  byte num_entries;
  wcdma_ded_measurements_T meas[MAX_WCDMA_STORED_DED_MEASUREMENTS];
} surr_wcdma_ded_measurements_T;

/*!
  @brief Enumeration of RATs that GERAN interworks with.  Automatic numbering
  is used in this enumeration to ensure that RR_RAT_MAX always equals the number
  of supported RATs (including GSM).  This may be used throughout the inter-RAT
  code in RR.
*/
typedef enum
{
  RR_RAT_UNDEFINED = -1,
  RR_RAT_GSM,
#ifdef FEATURE_WCDMA
  RR_RAT_WCDMA,
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
  RR_RAT_LTE,
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS
  RR_RAT_UTRAN_TDD,
#endif /*FEATURE_GSM_TDS*/
  RR_RAT_MAX
} rr_rat_e;

#ifdef FEATURE_GSM_TDS
typedef struct
{
  word    uarfcn;
  /*!< UARFCN of the cell. */

  uint16 cell_parameter_id;
  /*!< Cell Parameter ID of the cell. */

  boolean diversity;
  /*!< Diversity value for the cell. */

  int16  rscp;
  /*!< RSCP of the cell. */

  uint16 ts0_pos;
  /*!< TS0 position of the strongest cell in the UARFCN. All cells with the same UARFCN
       will have identical TS0 positions*/ 
} tds_ded_measurements_t;

typedef union
{
  tds_ded_measurements_t utran_tdd;
} rr_ded_irat_measurement_data_u;

typedef struct
{
  rr_rat_e                       rat;
  rr_ded_irat_measurement_data_u data;
} rr_ded_irat_measurement_t;

typedef struct
{
  // No of entries in the surrounding cell data base
  byte                  no_of_entries;
  // cell entries
  rr_ded_irat_measurement_t entries[MAX_IRAT_STORED_DED_MEASUREMENTS];
} rr_ded_irat_measurements_t;
#endif /*FEATURE_GSM_TDS*/


#ifdef FEATURE_WCDMA

typedef struct
{
  byte NUM_CELLS;
  rr_3g_report_t rr_3g_report[MAX_WCDMA_STORED_DED_MEASUREMENTS];
} rr_log_3g_meas_report_T;

#endif /* FEATURE_WCDMA */

#define MAX_GSM_NCELLS 96
#define GSM_BSIC_DESCRIPTION_MAX_BSICS_PER_ENTRY 8

typedef struct
{
  byte                        num_of_bsics;
  BSIC_T                      bsic[GSM_BSIC_DESCRIPTION_MAX_BSICS_PER_ENTRY];
} gsm_bsic_description_entry_T;

typedef struct
{
  gsm_bsic_description_entry_T bsics[MAX_GSM_NCELLS];
} rr_gsm_bsic_description_T;

typedef enum
{
  SI_TYPE_NONE = 0,
  SI_TYPE_SI_1,
  SI_TYPE_SI_2,
  SI_TYPE_SI_2_BIS,
  SI_TYPE_SI_2_TER,
  SI_TYPE_SI_2_TER_REST,
  SI_TYPE_SI_2_QUATER,
  SI_TYPE_SI_3,
  SI_TYPE_SI_4,
  SI_TYPE_SI_5,
  SI_TYPE_SI_5_BIS,
  SI_TYPE_SI_5_TER,
  SI_TYPE_SI_6,
  SI_TYPE_SI_7,
  SI_TYPE_SI_8,
  SI_TYPE_SI_13,
  SI_TYPE_MAX_COUNT

} rr_si_type_t;

//#define SI_TYPE_TO_MASK(x)    (1U << ((uint16)x-1))
#define SI_TYPE_TO_MASK(x) \
  ( ((x > SI_TYPE_NONE) && (x < SI_TYPE_MAX_COUNT)) ? 1U << ((uint16)x-1) : MASK_SI_NONE )

/* bit masks to indicate messages received */

#define MASK_SI_NONE          0
#define MASK_SI_1             SI_TYPE_TO_MASK(SI_TYPE_SI_1)
#define MASK_SI_2             SI_TYPE_TO_MASK(SI_TYPE_SI_2)
#define MASK_SI_2_BIS         SI_TYPE_TO_MASK(SI_TYPE_SI_2_BIS)
#define MASK_SI_2_TER         SI_TYPE_TO_MASK(SI_TYPE_SI_2_TER)
#define MASK_SI_2_TER_REST    SI_TYPE_TO_MASK(SI_TYPE_SI_2_TER_REST)
#define MASK_SI_2_QUATER      SI_TYPE_TO_MASK(SI_TYPE_SI_2_QUATER)
#define MASK_SI_3             SI_TYPE_TO_MASK(SI_TYPE_SI_3)
#define MASK_SI_4             SI_TYPE_TO_MASK(SI_TYPE_SI_4)
#define MASK_SI_5             SI_TYPE_TO_MASK(SI_TYPE_SI_5)
#define MASK_SI_5_BIS         SI_TYPE_TO_MASK(SI_TYPE_SI_5_BIS)
#define MASK_SI_5_TER         SI_TYPE_TO_MASK(SI_TYPE_SI_5_TER)
#define MASK_SI_6             SI_TYPE_TO_MASK(SI_TYPE_SI_6)
#define MASK_SI_7             SI_TYPE_TO_MASK(SI_TYPE_SI_7)
#define MASK_SI_8             SI_TYPE_TO_MASK(SI_TYPE_SI_8)
#define MASK_SI_13            SI_TYPE_TO_MASK(SI_TYPE_SI_13)

#define MASK_SI_ALL  ( MASK_SI_1         |  \
                       MASK_SI_2         |  \
                       MASK_SI_2_BIS     |  \
                       MASK_SI_2_TER     |  \
                       MASK_SI_2_QUATER  |  \
                       MASK_SI_3         |  \
                       MASK_SI_4         |  \
                       MASK_SI_13 )

/**
  @brief Dedicated mode Measurement Parameters, received in SI2quater &
         Measurement Information.
*/
typedef struct
{
  uint8         pwrc;
  report_type_T report_type;
  uint8         reporting_rate;
  boolean       invalid_bsic_reporting;
  uint8         serving_band_reporting;
  uint8         multiband_reporting;

  /*
  Indication of the offset, which shall be used for the reported RXLEV
  values.
  0 = +0 dB, 1 = + 10 dB, 2 = automatic
  Default value = 0 dB.
  */
  uint8         scale_ord;



  uint8         reporting_threshold_850;
  uint8         reporting_offset_850;
  uint8         reporting_threshold_900;
  uint8         reporting_offset_900;
  uint8         reporting_threshold_1800;
  uint8         reporting_offset_1800;
  uint8         reporting_threshold_1900;
  uint8         reporting_offset_1900;

} rr_gsm_meas_params_t;

typedef struct
{
	rr_gsm_meas_params_t meas_params;
} rr_dedicated_mode_gsm_params_t;

typedef struct
{
  rr_dedicated_mode_gsm_params_t    gsm_params;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
  rr_dedicated_mode_utran_params_t  utran_params;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
  rr_dedicated_mode_eutran_params_t eutran_params;
#endif /* FEATURE_LTE */

} rr_dedicated_mode_params_t;

typedef struct
{
  /**
   * true if the RR is camped on the serving cell described by the database
   */
  boolean                           camped_on_flag;

  /**
   * set to true if all the idle SI required to fully camp on the cell has been read
   */
  boolean                           valid_data_flag;

  /**
   * Reason code set when SI is invalidate (e.g. HANDOVER)
   */
  rr_invalidate_si_reason_t         invalidate_si_reason;

  /**
   * if TRUE, a RR_EV_GSM_SI_RECEIVED event is generated when all
   * the SI needed to camp on the cell is received
   */
  boolean                           generate_si_received_event;

  /**
   * true if all the dedicated SI has been read
   */
  boolean                           valid_dedicated_data_flag;

  /**
   * bit mask indicating which SI messages have been received by the RR for the serving cell
   */
  word                              update_message_received;

  /**
   * the ARFCN of the cell
   */
  ARFCN_T                           BCCH_ARFCN;

  /**
   * the BSIC (Base Station Identification Code) for the cell
   */
  BSIC_T                            BSIC;

  /**
   * set to RR_CA_RECEIVED if valid Cell Allocation (CA) values for the cell have been received
   */
  rr_ca_status_t                    valid_CA_received;


  /**
   * Decoded Cell channel description IE information
   */
  cell_channel_description_T        cell_channel_description;

  /**
   * Cell channel description that was received in SI1.
   * We will copy this value over to the cell channel
   * description if we go back to idle on the same serving cell
   */
  cell_channel_description_T        SI_cell_channel_description;


  /**
   * Decoded RACH Control Parameters IE information
   */
  RACH_control_parameters_T         RACH_control_parameters;

  /**
   * TRUE once the complete IDLE BA list has been received
   */
  boolean                           complete_idle_BA_list_received;

  /**
   * TRUE once the complete IDLE 3G neighbor list has been received
   */
  boolean                           complete_idle_3G_neighbor_list_received;

  boolean                           complete_idle_3G_neighbor_list_received_si2ter;

  /**
   * Indicates whether SI2bis should be decoded for this cell and
   * used to construct the complete BA list
   */
  rr_si_require_T                   decode_SI2bis;

  /**
   * if TRUE, SI2ter should be decoded for this cell and used
   * to generate the complete BA list
   */
  rr_si_require_T                   decode_SI2ter;

  /**
   * the complete idle GSM BA list for the cell (combines SI2, and
   * optionally SI2bis, SI2ter)
   */
  BCCH_frequency_list_T             complete_BA_list;

  /**
   * Only BA list received from SI2
   */
  BCCH_frequency_list_T             SI2_BA_list;

  /**
   * Only BA list received from SI2bis
   */
  BCCH_frequency_list_T             SI2bis_BA_list;

  /**
   * Only BA list received from SI2ter
   */
  BCCH_frequency_list_T             SI2ter_BA_list;

  /**
   * Set to true if the complete dedicated BA list has been read
   */
  boolean                           complete_BA_sacch_received;

  /**
   * Indicates whether SI5bis should be decoded for this cell and
   * used to construct the complete BA list
   */
  rr_si_require_T                   decode_SI5bis;

  /**
   * if TRUE, SI5ter should be decoded for this cell and used
   * to generate the complete BA list
   */
  rr_si_require_T                   decode_SI5ter;

  /**
   * the complete SACCH BSIC list that should be associated with the
   * the complete_BA_sacch index wise
   */
  rr_gsm_bsic_description_T         gsm_bsic_description;

  byte                              NCC_permitted;
  word                              cell_identity;
  boolean                           cell_identity_valid_flag;
  boolean                           ecsc;
  rr_si2ter_expected_T              two_ti;

  /* the SI2quater_POSITION received in the SI3 Rest octets */
  two_quater_position_T            two_quater_position;

  LAI_T                             location_area_identification;
  control_channel_description_T     control_channel_description;
  boolean                           control_channel_description_valid;
  cell_options_T                    cell_options;

  /**
   * Indicates whether the RADIO_LINK_TIMEOUT value in the cell
   * is valid or not. If not valid, the S_counter needs to be
   * updated when the new RLT value is received.
   */
  boolean                           RLT_valid;

  cell_selection_parameters_T       cell_selection_parameters;
  boolean                           cbch_channel_information_valid;
  channel_information_T             cbch_channel_information;
  mobile_allocation_T               cbch_mobile_allocation;

  byte                              multiband_reporting;

  /**
   * contains measurement info related to reselection
   */
  reselection_info_T                reselection_info;

  /**
   * TRUE if the cell is only suitable because MS
   * is in cell test mode of operation FALSE otherwise
   */
  boolean                           cell_test_operation_mode;

  /**
   * the BA list received from SI2 in encoded form (used for BCCH allocation)
   */
  byte                              BCCH_allocation[MM_SIM_EFBCCH_SIZE];

  /**
   * Set to TRUE if the GRPS Indicator is present in the system information,
   * FALSE otherwise.
   */
  boolean                           gprs_indicator;

  /**
   * TC value of the last system information message received (used to
   * detect wrap around)
   */
  byte                              last_tc;

  /* 3G neighbor list */
  utran_idle_neighbor_list_T              utran_neighbor_list;
  /* storage for trk_lo_adj received in both idle and dedicated measurements
  from L1; trk_lo_adj is independent of whether the UE is in idle or
  dedicated mode */
  word              trk_lo_adj;

  wcdma_idle_neighbor_list_si2ter_T       wcdma_neighbor_list_si2ter;

  wcdma_ded_neighbor_list_T               wcdma_ded_neighbor_list;

  /* Early Classmark Change Restriction */
  boolean                           ecsr;

  /* R99 band indicator parameter carried by SI1, SI6, PSI1
     0 - DCS_1800, 1 - PCS_1900 */
  si_psi_band_ind_T      band_indicator;

#ifdef FEATURE_LTE
  rr_lte_neighbour_list_t          lte_neighbour_list;

  /*! 3G Frequency List, see 3GPP 44.018 section 3.4.1.2.1.7c */
  rr_utran_frequency_list_t        utran_frequency_list;
#endif /* FEATURE_LTE */

  rr_dedicated_mode_params_t     * dedicated_mode_params;

} system_information_database_T;

typedef struct
{
  boolean     valid_data_flag;
  word        BCCH_ARFCN;
  LAI_T       lai;
  byte        RXLEV_average;
  BSIC_T      BSIC;
  boolean     cell_bar_access;
} alternate_info_T;

/* stores information for the surrounding (neighbor) cells */
typedef struct
{
  /* set to TRUE when valid SI data has been read */
  boolean                       valid_data_flag;

  /* ARFCN (channel number) for the entry */
  ARFCN_T                       BCCH_ARFCN;

  /* Base Station Identification Code (BSIC) for the entry */
  BSIC_T                        BSIC;
  /* indicates which SI messages have been received (bitmask) */
  word                          update_message_received;

  /* system information parameters required for neighbor cells */
  RACH_control_parameters_T     RACH_control_parameters;
  cell_selection_parameters_T   cell_selection_parameters;
  LAI_T                         location_area_identification;

  /***********************************************************/
  /* Control channel information is needed for early camping */
  /* Arrives in SI3 but it may be invalid when we make our   */
  /* reselection decision                                    */
  /***********************************************************/
  control_channel_description_T control_channel_description;
  boolean                       control_channel_description_valid;

  /* set to TRUE if event should be generated when all SI for this entry has been read */
  boolean                       si_expected;

  /* band associated with the ARFCN for the entry */

  boolean cell_id_valid;
  uint32  cell_id; /*cell id, range 0..0xffff*/
} information_T;

typedef enum
{
  NO_LINK = 0,
  SAPI0_LINK,
  SAPI0_AND_SAPI3_LINK,
  SAPI0_LINK_IN_PROGRESS
} link_status_T;

typedef struct
{
  byte              no_of_entries;
  information_T     info[MAX_NUMBER_OF_BEST_CELLS];
} surrounding_system_information_base_T;

typedef struct
{
  ARFCN_T BCCH_ARFCN;
  byte RXLEV_average;
  byte BSIC;
} measurement_T;

typedef struct
{
  byte              no_of_entries;
  measurement_T     measurement[MAX_NUMBER_OF_SURROUND_CELLS];
} surrounding_measurement_T;

typedef struct
{
  byte        access_reference;
  word        frame_number;
  word        egprs_access_reference;
} request_references_T;

/*! Define the maximum number of forbidden cells that RR will track.  If this
value is changed then the corresponding timer IDs must be changed in
rr_timers.h. */
#define RR_MAX_NUMBER_OF_FORBIDDEN_CELLS 7

/* Cells barred due to authentication or authentication and ciphering failure */
#define RR_MAX_NUM_OF_BARRED_CELLS 6
#define RR_INVALID_REMAINING_TIME  0xffff

/**
 * information for a forbidden cell
 */
typedef struct
{
  /**
   * TRUE if the entry is valid, FALSE otherwise
   */
  boolean       valid_entry;

  /**
   * ARFCN of the cell
   */
  ARFCN_T      ARFCN;

  /**
   * forbidden cell timer id associated with the cell
   */
  rr_timer_id_T timer_id;
} rr_forbidden_cell_list_info_T;

typedef struct
{
  rr_forbidden_cell_list_info_T forbidden_cells[RR_MAX_NUMBER_OF_FORBIDDEN_CELLS];
} rr_forbidden_cell_list_T;


/* Details of barred cells due to authentication or authentication and
   ciphering failures */

typedef struct
{
  ARFCN_T arfcn;            /* arfcn of the cell */
  BSIC_T bsic;              /* bsic of the cell */
  uint32 remaining_time;    /* time (in milli seconds) cell to be kept barred */
}rr_barred_cell_info_T;



/* The following Struct is the RR local Sim info Struct and its
   info will be populated & updated based on the info given by
   MM.
   cipher_key           Cipher Key
   cipher_key_seq       Cipher Key sequence
   rr_tmsi              TMSI
   rr_imsi              IMSI
   ptmsi                PTMSI
   rr_ms_opcode         MS Operational mode
   and all the other flags represent if the corresponding info is
   valid and updated or not */

// Values for performing reselection in cell reselection params log packets
// Also used in cell reselection state machine to keep track of the type of
// reselection being preformed
typedef enum
{
  RR_RESELECTION_NOT_PERFORMED = 0,
  RR_PERFORMING_C2_RESELECTION,
  RR_PERFORMING_C1_RESELECTION,
  RR_PERFORMING_WCDMA_RESELECTION,
  RR_PERFORMING_IMMEDIATE_RESELECTION
} rr_performing_reselection_enum_T;


/********* Cell selection power scan types *************/

typedef struct
{
  rr_l1_pscan_results_T pscan_results;

  rr_l1_band_bitmap_T bands_searched;
} rr_pscan_results_db_T;

/***** RR available plmn database types ****************/

typedef struct
{
  rr_l1_freq_power_T      meas;
  rr_cell_type_T          priority;
  boolean                 GPRS_indicator;
  LAI_T                   lai;
} rr_plmn_cell_info_T;

typedef enum
{
  NO_CHECK,
  CHECK_FORBIDDEN_LAI_LIST,
  CHECK_REJECT_LAI_LIST
}rr_forbidden_lai_check_option_e;


/********************************/
/* Global Variable Declarations */
/********************************/

/******************************************************************/
/* maximum size of request references array. Only the last 3 need */
/* to be matched against immediate assignment messages            */
/******************************************************************/
#define RR_REQ_REF_ARRAY_SIZE 3

/* reason why a reselection is being performed */
extern rr_performing_reselection_enum_T reselection_type;

#define MAX_NAS_MESSAGE_BUFFER_SIZE 20

typedef struct
{
  SAPI_T            SAPI;
  byte              l3_message_length;
  byte              layer3_message[MAX_OCTETS_L3_MESSAGE];
} rr_nas_message_buffer_T;

typedef struct
{

  BCCH_frequency_list_T             SIbis_measurement_report_BA_list;
  BCCH_frequency_list_T             SIter_measurement_report_BA_list;
  cell_options_T                    cell_options;
  reselection_info_T                reselection_info;
  best_six_info_T                   best_six_info;

} usat_system_information_database_T;

extern void qsort_arfcn(
  ARFCN_T *arfcn,
  size_t num,
  const gas_id_t gas_id
);

/* used to collect information from network */
extern instances_T *rr_get_instances_ptr(si2q_mi_instance_e_type, const gas_id_t gas_id);
si2ter_instances_T *rr_get_SI2ter_instances_ptr(const gas_id_t gas_id);

extern void rr_data_discard_data(const gas_id_t gas_id);

typedef enum
{
  /* RR is out of service. */
  RR_SERVICE_STATE_OUT_OF_SERVICE,

  /* RR is performing autonomous cell selection. */
  RR_SERVICE_STATE_CELL_SELECTION,

  /* RR is performing cell selection in response to RR_PLMN_SELECT_REQ. */
  RR_SERVICE_STATE_PLMN_SELECTION,

  /* RR is in service. */
  RR_SERVICE_STATE_IN_SERVICE,

  /* RR is in dedicated mode. */
  RR_SERVICE_STATE_DEDICATED,

  RR_SERVICE_STATE_MAX
} rr_service_state_e;

#ifdef FEATURE_LTE

extern rr_individual_priorities_t ** rr_individual_priorities(const gas_id_t gas_id);

#endif /* FEATURE_LTE */


#define MAX_GSM_CELLS_IN_CELL_SELECTION_INDICATOR  32

typedef struct
{
  byte        num_of_cells;
  ARFCN_T     arfcn[MAX_GSM_CELLS_IN_CELL_SELECTION_INDICATOR];
  BSIC_T      bsic[MAX_GSM_CELLS_IN_CELL_SELECTION_INDICATOR];
}rr_gsm_target_info_type;

typedef struct
{
  byte        dummy;
}rr_tdd_target_info_type;

#ifdef FEATURE_LTE

typedef struct
{
  lte_earfcn_t       earfcn;
  boolean            measurement_bandwidth_valid;
  uint8              measurement_bandwidth;
  boolean            not_allowed_cells_valid;
  rr_pcid_group_ie_t not_allowed_cells;
  boolean            target_pcid_valid;
  uint16             target_pcid;
} rr_csi_eutran_description_t;

#define MAX_CSI_EUTRAN_DESCRIPTIONS_COUNT 8

typedef struct
{
  /* @brief TRUE if the redirection is a blind redirection, FALSE otherwise. */
  boolean                     blind_redir;
  uint32                      eutran_descriptions_count;
  rr_csi_eutran_description_t eutran_descriptions[MAX_CSI_EUTRAN_DESCRIPTIONS_COUNT];
#ifdef FEATURE_SGLTE
  boolean                     redirection_for_sglte;
#endif /*FEATURE_SGLTE*/
} rr_lte_target_info_type;

#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS_G2T_REDIRECTION
  typedef struct
  {
    tdsrrc_target_info_type target;
#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
    boolean pseudo_fr_active;
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/
  } rr_g2t_redir_info_t;
#endif /*FEATURE_GSM_TDS_G2T_REDIRECTION*/

typedef union
{
  rr_gsm_target_info_type     gsm_target;
#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE
  rr_rrc_fdd_target_info_type fdd_target;
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */
#ifdef FEATURE_GSM_TDS_G2T_REDIRECTION
  rr_g2t_redir_info_t         tdd_target;
#else
  rr_tdd_target_info_type     tdd_target; /*dummy*/
#endif /*FEATURE_GSM_TDS_G2T_REDIRECTION*/
#ifdef FEATURE_LTE
  rr_lte_target_info_type     lte_target;
#endif /* FEATURE_LTE */
}rr_cell_selection_target_u;

typedef enum
{
  GSM_TARGET,
  FDD_TARGET,
  TDD_TARGET,
  LTE_TARGET,
  UNKNOWN_TARGET
}rr_cell_selection_target_e;

typedef struct
{
  boolean                    valid;
  rr_cell_selection_target_e type;
  rr_cell_selection_target_u target;
} rr_chan_rel_cell_selection_indicator_t;

extern void rr_init_cell_selection_indicator(const gas_id_t gas_id);


typedef enum
{
  PRIO_LOWEST = 0,
  PRIO_PLMN_SELECT = 20,
  PRIO_PLMN_SEARCH_AUTO = 30,
  PRIO_CS_CALL_LAU = 40,
  PRIO_DL_UNIT_DATA_IND = 50,
  PRIO_CS_CALL_PAGE_RES = 60,
  PRIO_PS_CALL_PAGE_RES = 70,
  PRIO_PS_CALL = 80,
  PRIO_CS_CALL_NORMAL = 90,
  PRIO_PLMN_SEARCH_MANUAL = 100,
  PRIO_GSM_MODE_DEACTIVATION = 110,
  PRIO_SHUTDOWN  = 120
}rr_procedure_priority_e;

#define RR_CHANNEL_DESC_IE_LENGTH 3

typedef struct
{
  channel_information_T channel_info;
  channel_mode_T        channel_mode;
  byte                  channel_desc[RR_CHANNEL_DESC_IE_LENGTH];
} channel_mode_modify_T;

#define FREQ_REDEF_CCD_IE_SIZE 16

typedef struct
{
  channel_information_T channel_info;
  boolean               ccd_present;
  byte                  ccd_ie[FREQ_REDEF_CCD_IE_SIZE];
  word                  starting_time;
} frequency_redefinition_T;

typedef struct
{
  byte current_signal_strength;
  byte current_signal_quality;
} rr_measured_signal_data_t;

typedef struct
{
  BCCH_frequency_list_T             complete_BA_sacch;
  BCCH_frequency_list_T             SI5_BA_list; /* Only BA list received in SI5*/
  BCCH_frequency_list_T             SI5bis_BA_list; /* Only BA list received in SI5bis */
  BCCH_frequency_list_T             SI5ter_BA_list; /* Only BA list received in SI5ter */
  BCCH_frequency_list_T             SI5_SI5bis_measurement_report_BA_list; /* BA list being used for measurement reports (SI5/SI5bis) indexing */
  BCCH_frequency_list_T             SI5ter_measurement_report_BA_list; /* BA list being used for measurement report (SI5ter) indexing */
} rr_ba_lists_t;

typedef struct
{
  wcdma_neighbor_cell_list_T wcdma_neighbor_cell_list[MAX_UTRAN_CELLS + MAX_UTRAN_FREQ];
} wcdma_ncell_list_data_t;

#ifdef FEATURE_WCDMA
typedef struct
{
  uint16  uarfcn;
  uint16  scrambling_code;
  uint16  pn_offset;
  boolean diversity;
} rr_wcdma_cell_t;
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
typedef struct
{
  uint16  uarfcn;
  uint16  cpid;
  boolean diversity;
} rr_utran_tdd_cell_t;
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE
typedef struct
{
  lte_earfcn_t earfcn;
  uint16 pcid;
} rr_lte_cell_t;
#endif /* FEATURE_LTE */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
typedef union
{
#ifdef FEATURE_WCDMA
  rr_wcdma_cell_t wcdma;
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
  rr_lte_cell_t   lte;
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS
  rr_utran_tdd_cell_t utran_tdd;
#endif /* FEATURE_GSM_TDS */
} rr_irat_cell_u;

typedef struct
{
  rr_rat_e       rat;
  rr_irat_cell_u data;
} rr_irat_cell_t;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

typedef enum
{
  RR_CCO_STATUS_UNDEFINED,
  RR_CCO_STATUS_SUCCESS,
  RR_CCO_STATUS_FAILURE_CCO_TIMER_EXPIRY,
  RR_CCO_STATUS_FAILURE_NO_RESPONSE,
  RR_CCO_STATUS_FAILURE_IA_REJECT,
  RR_CCO_STATUS_FAILURE_CONTENTION_RESOLUTION_FAILURE,
  RR_CCO_STATUS_FAILURE_DSF,
  RR_CCO_STATUS_ABORTED_HIGH_PRIORITY_REQUEST,
  RR_CCO_STATUS_REJECTED,
  RR_CCO_STATUS_MAX
} rr_cco_status_e;

typedef enum
{
  RR_NORMAL_SEARCH_ORDER
} rr_search_order_e;

#ifdef FEATURE_LTE

typedef struct
{
  boolean                         default_utran_priority_valid;
  uint8                           default_utran_priority;

  uint32                          num_utra_fdd_freq;
  lte_irat_reselection_priority_s utra_fdd_freq_list[LTE_IRAT_MAX_UTRAN_FREQ];

  uint32                          num_utra_tdd_freq;
  lte_irat_reselection_priority_s utra_tdd_freq_list[LTE_IRAT_MAX_UTRAN_FREQ];

} rr_utran_individual_priority_parameters_description_t;

typedef struct
{
  boolean                         default_eutran_priority_valid;
  uint8                           default_eutran_priority;

  uint32                          num_eutra_freq;
  lte_irat_reselection_priority_s eutra_freq_list[LTE_IRAT_MAX_EUTRAN_FREQ];

} rr_eutran_individual_priority_parameters_description_t;


typedef struct
{
  uint8                                                    geran_priority;
  rr_utran_individual_priority_parameters_description_t  * utran_individual_priority_parameters_description;
  rr_eutran_individual_priority_parameters_description_t * eutran_individual_priority_parameters_description;
  boolean                                                  t3230_timeout_value_valid;
  uint8                                                    t3230_timeout_value;
} rr_individual_priorities_ie_t;


#ifdef FEATURE_SGLTE

typedef struct
{
  sys_plmn_id_s_type                requested_PLMN;

  boolean                           dedicated_priorities_valid;

  lte_irat_dedicated_priority_info_s dedicated_priority_info;

} sglte_l2g_resel_req_ded_pri_info_t;

#endif /* FEATURE_SGLTE */

#endif /* FEATURE_LTE */

/* ------------------------------------ General functions --------------------------------------- */

/****************************************************************************
 *   Function name:  rr_get_message_list_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer the message list array.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  message_list_T *
 *   --------
 *
 ****************************************************************************/
message_list_T *rr_get_message_list_ptr(const gas_id_t gas_id);

/****************************************************************************
 *   Function name:  rr_elements_in_message_list
 *   --------------------------
 *
 *   Description: Returns number of elements in the message list array.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  uint16
 *   --------
 *
 ****************************************************************************/
uint16 rr_elements_in_message_list(const gas_id_t gas_id);

/****************************************************************************
 *   Function name:  rr_get_PLMN_request_details_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to rr_PLMN_request_details.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  rr_PLMN_request_details_T *
 *   --------
 *
 ****************************************************************************/
rr_PLMN_request_details_T *rr_get_PLMN_request_details_ptr(const gas_id_t gas_id);

/*!
 * \brief Initialisation function for the PLMN select request information
 * 
 * \param gas_id 
 */
extern void rr_init_plmn_select_req_info(const gas_id_t gas_id);

/****************************************************************************
 *   Function name:  rr_get_plmn_list_req_info_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to rr_plmn_list_req_info.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  rr_PLMN_request_details_T *
 *   --------
 *
 ****************************************************************************/
rr_PLMN_list_req_info_T *rr_get_plmn_list_req_info_ptr(const gas_id_t gas_id);

/*!
 * \brief Initialisation function for the PLMN list request information
 * 
 * \param gas_id 
 */
extern void rr_init_plmn_list_req_info(const gas_id_t gas_id);

/**
  @brief Accessor for rr_l1_gsm_stop_reason.

  Retrieve the current value of rr_l1_gsm_stop_reason.

  @return the current value of rr_l1_gsm_stop_reason.
*/
extern rr_l1_gsm_stop_reason_T rr_l1_gsm_stop_reason_get(const gas_id_t gas_id);

/**
  @brief Mutator for rr_l1_gsm_stop_reason.

  Set the current value of rr_l1_gsm_stop_reason.

  @param gsm_stop_reason The new value of rr_l1_gsm_stop_reason.
*/
extern void rr_l1_gsm_stop_reason_set(rr_l1_gsm_stop_reason_T gsm_stop_reason, const gas_id_t gas_id);


/**
  @brief Accessor for rr_l1_gsm_deact_reason.

  Retrieve the current value of rr_l1_gsm_deact_reason.

  @return the current value of rr_l1_gsm_deact_reason.
*/
extern rr_l1_gsm_deact_reason_T rr_l1_gsm_deact_reason_get(const gas_id_t gas_id);

/**
  @brief Mutator for rr_l1_gsm_deact_reason.

  Set the current value of rr_l1_gsm_deact_reason.

  @param gsm_deact_reason The new value of rr_l1_gsm_deact_reason.
*/
extern void rr_l1_gsm_deact_reason_set(rr_l1_gsm_deact_reason_T gsm_deact_reason, const gas_id_t gas_id);

/****************************************************************************
 *   Function name:  rr_get_wcdma_ncell_list_data_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to wcdma_ncell_list_data.
 *
 *   Parameters: 
 *   -----------
 *
 *   Returns:  wcdma_ncell_list_data_t *
 *   --------
 *
 ****************************************************************************/
wcdma_ncell_list_data_t *rr_get_wcdma_ncell_list_data_ptr(const gas_id_t gas_id);

/****************************************************************************
 *   Function name:  rr_get_wcdma_ded_meas_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to wcdma_ded_meas_ptr.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  surr_wcdma_ded_measurements_T *
 *   --------
 *
 ****************************************************************************/
surr_wcdma_ded_measurements_T *rr_get_wcdma_ded_meas_ptr(const gas_id_t gas_id);

#ifdef FEATURE_GSM_TDS
/****************************************************************************
 *   Function name:  rr_get_tds_ded_meas_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to tds_ded_meas_ptr.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  rr_ded_irat_measurements_t *
 *   --------
 *
 ****************************************************************************/
rr_ded_irat_measurements_t *rr_get_tds_ded_meas_ptr(const gas_id_t gas_id);
#endif /*FEATURE_GSM_TDS*/

/****************************************************************************
 *   Function name:  rr_get_measured_signal_data_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to rr_measured_signal_data.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  rr_measured_signal_data_t *
 *   --------
 *
 ****************************************************************************/
rr_measured_signal_data_t *rr_get_measured_signal_data_ptr(const gas_id_t gas_id);

/****************************************************************************
 *   Function name:  rr_get_max_message_index_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to max_message_index.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  byte *
 *   --------
 *
 ****************************************************************************/
byte *rr_get_max_message_index_ptr(const gas_id_t gas_id);


/****************************************************************************
 *   Function name:  rr_get_cell_select_indicator_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to rr_cell_select_indicator.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  rr_chan_rel_cell_selection_indicator_t *
 *   --------
 *
 ****************************************************************************/
rr_chan_rel_cell_selection_indicator_t *rr_get_cell_select_indicator_ptr(const gas_id_t gas_id);


/****************************************************************************
 *   Function name:  rr_get_surrounding_measurement_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to surrounding_measurement.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  surrounding_measurement_T *
 *   --------
 *
 ****************************************************************************/
surrounding_measurement_T *rr_get_surrounding_measurement_ptr(const gas_id_t gas_id);

/****************************************************************************
 *   Function name:  rr_get_nas_message_buffer_ptr_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to rr_nas_message_buffer.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  rr_nas_message_buffer_T *
 *   --------
 *
 ****************************************************************************/
rr_nas_message_buffer_T **rr_get_nas_message_buffer_ptr_ptr(const gas_id_t gas_id);

/****************************************************************************
 *   Function name:  rr_get_usat_system_information_database_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to usat_system_information_database.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  usat_system_information_database_T *
 *   --------
 *
 ****************************************************************************/
usat_system_information_database_T *rr_get_usat_system_information_database_ptr(const gas_id_t gas_id);


/****************************************************************************
 *   Function name:  rr_get_nc_meas_params_mods_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to nc_meas_params_mods.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  nc_meas_params_mods_t *
 *   --------
 *
 ****************************************************************************/
nc_meas_params_mods_t *rr_get_nc_meas_params_mods_ptr(const gas_id_t gas_id);


/****************************************************************************
 *   Function name:  rr_get_gprs_meas_params_mods_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to gprs_meas_params_mods.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  gprs_meas_params_mods_t *
 *   --------
 *
 ****************************************************************************/
gprs_meas_params_mods_t *rr_get_gprs_meas_params_mods_ptr(const gas_id_t gas_id);

/****************************************************************************
 *   Function name:  rr_get_final_pmo_pcco_nacc_params_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to final_pmo_pcco_nacc_params.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  pmo_pcco_nacc_params_t *
 *   --------
 *
 ****************************************************************************/
pmo_pcco_nacc_params_t *rr_get_final_pmo_pcco_nacc_params_ptr(const gas_id_t gas_id);

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
/****************************************************************************
 *   Function name:  rr_get_gprs_3g_meas_params_mods_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to gprs_3g_meas_params_mods.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  gprs_3g_meas_params_mods_t *
 *   --------
 *
 ****************************************************************************/
gprs_3g_meas_params_mods_t *rr_get_gprs_3g_meas_params_mods_ptr(const gas_id_t gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

/****************************************************************************
 *   Function name:  rr_get_nc_meas_params_pcco_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to nc_meas_params_pcco.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  nc_meas_params_pcco_ptr *
 *   --------
 *
 ****************************************************************************/
nc_meas_params_mods_t *rr_get_nc_meas_params_pcco_ptr(const gas_id_t gas_id);

/****************************************************************************
 *   Function name:  rr_get_gprs_meas_params_pcco_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to gprs_meas_params_pcco.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  gprs_meas_params_mods_t *
 *   --------
 *
 ****************************************************************************/
gprs_meas_params_mods_t *rr_get_gprs_meas_params_pcco_ptr(const gas_id_t gas_id);

/****************************************************************************
 *   Function name:  rr_get_pmo_pcco_nacc_params_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to pmo_pcco_nacc_params.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  pmo_pcco_nacc_params_t *
 *   --------
 *
 ****************************************************************************/
pmo_pcco_nacc_params_t *rr_get_pmo_pcco_nacc_params_ptr(const gas_id_t gas_id);


#ifdef FEATURE_WCDMA
/****************************************************************************
 *   Function name:  rr_get_gprs_3g_meas_params_pcco_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to gprs_3g_meas_params_pcco.
 *
 *   Parameters:
 *   -----------
 *
 *   Returns:  gprs_3g_meas_params_mods_t *
 *   --------
 *
 ****************************************************************************/
gprs_3g_meas_params_mods_t *rr_get_gprs_3g_meas_params_pcco_ptr(const gas_id_t gas_id);
#endif /*FEATURE_WCDMA*/

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */


typedef struct
{
  nc_meas_params_mods_t          nc_meas_params;
  gprs_meas_params_mods_t        gprs_meas_params;
  pmo_pcco_nacc_params_t         nacc_params;
  gprs_3g_meas_params_mods_t     gprs_3g_meas_params;
  boolean                        serving_cell_priority_params_valid;
  csn_gsm_priority_params_ie_t   serving_cell_priority_params;
  boolean                        utran_priority_params_valid;
  csn_utran_priority_params_ie_t utran_priority_params;
  boolean                        eutran_params_valid;
  csn_eutran_params_ie_t         eutran_params;
} rr_params_mods_t;

extern rr_params_mods_t ** rr_params_mods(const gas_id_t gas_id);
extern rr_params_mods_t ** rr_pcco_params_mods(const gas_id_t gas_id);

typedef struct
{
  boolean                        ccn_active;

  rr_eutran_meas_params_t        meas_params;

  rr_gprs_eutran_meas_params_t   gprs_meas_params;

  uint8                          neighbour_cells_count;
  rr_eutran_neighbour_cells_t    neighbour_cells[RR_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS];

  uint8                          not_allowed_cells_count;
  csn_eutran_not_allowed_cells_t not_allowed_cells[CSN_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS];

} rr_eutran_params_t;


#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)

/* Types used for X2G NACC */

#define RR_X2G_SYSTEM_INFORMATION_MESSAGES_MAX 10

typedef struct
{
  uint32                 count;
  geran_grr_si_message_t messages[RR_X2G_SYSTEM_INFORMATION_MESSAGES_MAX];
} rr_x2g_system_information_t;

#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

#ifdef FEATURE_LTE 

/*! Define the maximum number of depriorized  cells that RR will track.*/
#define RR_MAX_NUMBER_OF_DEPRIOTIZED_CELLS 10

typedef struct
{
  ARFCN_T  ARFCN;
  inter_task_lai_T    lte_lai;
} rr_depriotized_cell_info_T;

typedef struct
{
  uint8    index;
  uint8    num_of_cell;
  rr_depriotized_cell_info_T depriotized_cell_info[RR_MAX_NUMBER_OF_DEPRIOTIZED_CELLS];
} rr_depriotized_gsm_cell_list_T;

#endif /*FEATURE_LTE*/

#endif /* #ifndef RR_DEFS_H */

