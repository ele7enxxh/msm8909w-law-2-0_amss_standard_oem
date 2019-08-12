#ifndef RRCDATA_V_H
#define RRCDATA_V_H

/*===========================================================================
              R R C D A T A  H E A D E R  F I L E

DESCRIPTION
  This file contains data types and declarations associated with the
  RRC Data
  
Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/inc/rrcdata_v.h#1 $   

when       who     what, where, why
--------   ---     --------------------------------------------------------- 
07/05/16   sn      Changes for LTE U feature
05/13/16   gv      Made changes to increase the WCDMA scan timeout from 120s to 240s
05/11/16   sn      Changes for f3 logging reduction
05/11/16   nr      Added macro functions for WRRC_MSGx_HIGH
11/16/15   nr      Changes to fix error due to impilicit declaration of function
10/16/15   as      Added code to support LTA durting activation time
09/11/15   sr      Made changes for FR29345 EFS/NV Settings to be Read per Sub
07/01/15   sp      Made changes to save procedure substates for LTE commands and to skip logging frequent internal commands
04/16/15   sg      Made changes to add checks for pending CS RAB release in api rrc_ok_for_tuneaway
04/14/15   sp      Made changes to indicate Multi rab status to L1
02/09/15   sn      Changes to allow LTA even after PS RABs are released but not before
                   they are established
12/03/14   sp      Made changes to save one byte status in cmd_ptr to crash debug global
11/12/14   sp      Made changes to save procedure substates in crash debug info
11/11/14   sp      Made changes to increase the L2 ack timer value to 850 msec if EFACH_DRX is configured.
10/22/14   gv      Made changes to have connected mode feature set independent of TA status 
10/17/14   sr      CSFB call optimization changes
10/10/14   sr      Made changes to trigger tranistion to idle if TA duraitonis mroe than re-establishment timer values
10/06/14   gv      Made changes to follow same feature set in DSDA irrespective of number of active subs
09/30/14   sr      Made changes to send SCRI after CU confirm
09/16/14   sg      Made changes to fix compilation errors if REL 10
                   feature flag is disabled
09/03/14   sp      Made changes to save MCM/ASU Otas in crash debug info
08/04/14   vi      Made changes to avoid compiler warnings
08/04/14   sp      Made changes to fix compilation errors
07/24/14   sp      Made changes to skip saving RCR OTA in crash debug global if there is identity mismatch
06/30/14   vi      Made changes to get rid of wcdma_is_ue_in_dual_active for better clarity
06/18/14   as      Made changes to delay reselection while waiting for L2 ACK for PCH transition
04/11/14   vi      Made changes to disable FE_FACH capability in DSDS/DSDA mode.
04/10/14   sg      Made changes to correct the feauture flag used for rel10 spec CRs
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/14/14   sp      Made changes to init re-establish entity on Procedure cleanup
03/14/14   sp      Made changes to init re-establish entity on Procedure cleanup
02/03/14   as      Made changes to backup internal/external cmds for crash debugging
01/30/14   sn      WRRC changes for FR 15265
01/15/14   sp      Fixed compiler warnings
01/09/14   sp      Removed code under FEATURE_CS_VOICE_OVER_HSPA feature
11/08/13   ad      Made changes to indicate ho_type as part of codec info to CM
11/27/13   sp      Made changes to add nv support and to save time ticks for 
                   rr to/from rrc mm to/from rrc and to add msg_type for crash debug ota changes
11/13/13   sa      Mainlined the FEATURE_MVS_UNDER_GHDI
10/30/13   as      Made changes to add feature code for FEATURE_WCDMA_3C_HSDPA
10/29/13   as      Fixed compilation error
10/27/13   as      Made changes to backup OTAs for crash debugging
09/16/13   mp      Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
08/13/13   gv      Made changes to send RRC_RR_STOP_WCDMA_CNF with same AS ID as received in RRC_RR_STOP_WCDMA_REQ
08/26/13   as      Made changes to provide API to L2 to get CS + PS call status
07/31/13   vi      Mainlining the MODEM_STATISTICS feature.
06/28/13   gv      Made changes to buffer the MM and RR commands for crash debugging
06/20/13   sa      Made changes for Default config in CELL_FACH
05/20/13   sn      Changes to consider W CSG feature as disabled if there is 
                   no whitelist update from NAS
04/25/13   sg      Added code for REL9 under FEATURE_WCDMA_REL9
09/01/13   ad      Added featurization for MVS code
12/07/12   sn      CSG feature changes
12/04/12   ad      Made changes to send start of SRVCC indication to CM 
10/29/12   gv      Mainline the changes under FEATURE_PPAC
10/26/12   gv      Mainline the changes under FEATURE_3GPP_FAST_DORMANCY_ENHANCEMENT
10/26/12   gv      Corrected the changes under FEATURE_WCDMA_DIME_SW
10/23/12   pm      Made changes to assign pointer to NULL after de-allocation
10/18/12   gv      Replace FEATURE_DUAL_SIM_WCDMA_TUNEAWAY with FEATURE_DUAL_SIM
10/04/12   md      Made changes to detect RLC size change during state transitions via CELL PCH state.
10/04/12   pm      Removed unnecessary comments
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/26/12   as      Added code to support feature NB/WB handover support
03/28/12   zr      Adding feature definitions for Dime
05/10/12   md      Added code changes to support FEATURE_MAC_I
04/12/12   as      Added code to support feature NB/WB handover support
04/10/12   gv      Made changes to remove unused heap allocation and related unused code
03/12/12   rl      Made changes to fix the FEATURE_WCDMA_NEW_L2_INTERFACE porting issue.
02/03/12   mn       Added APIs to provide UL AMR and AMR-WB mode
01/12/12   gv      Added code to support FEATURE_PPAC
12/21/11   ad      Made changes to bring the mvs code outside of qchat feature
12/16/11   gv      Fixed compilation issue
12/12/11   gv      Added code to support free floating of tasks
12/09/11   ad      Added code to log WCDMA_RRC_PROTOCOL_ERRORS packet
11/30/11   md      Prototype of fun rrc_find_rab_for_rb_in_est_rabs has been moved
                   to Rrcdata_v.h
10/20/11   gv      Added function to send a rab-rel-ind after RRC_REL_IND for 
                   PS rabs
10/14/11   rl      Made changes for WCDMA Security Setting independent of NV Items.
09/07/11   gv      Added new prototype to support fast dormancy enhancement feature
08/30/11   ad      Made changes to handle default configuration 23
08/25/11   gv      Added code to support FEATURE_UIM_DISABLE_GSDI_INTERFACE
07/27/11   su      Added code changes to indicate the 64QAM configuration status
                   as part of RRC_HSPA_CALL_STATUS_IND.
07/28/11   ad      Made changes for CMI phase 4 
05/30/11   rl      CMI de-featurization
04/26/11   rl      Support for transition from TMC heap to Modem Heap in WCDMA.
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/21/11   as      Made changes to reject OTA if ordered code from OTA
                   is different from derived one
03/29/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/17/11   ad      RRC_CSP_GET_INT_MCC and RRC_CSP_GET_INT_MNC moved from 
                   rrcdata_v.h to rrcdata.h
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/02/11   ad      Fixed compilation warnings on 3.0 PL
03/02/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.92.50
02/28/11   ad      Fixed compilation warnings on 3.0 PL
02/24/11   ad      Moved rrc_return_lc_info_for_rab protoype to rrcdata_v.h
02/19/11   sk      Moved rrc_nv_rel_indicator to internal file. 
02/10/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.51
01/31/11   ad      Fixed compilation warnings
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/27/11   ad      Moving RRC_NV_REL_INDICATOR_REL7 to API folder
01/28/11   rl      Fixed compilation error
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/18/11   su      Fixed compilation error.
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50 
12/02/10   geg     Created file

===========================================================================*/

/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/

#include "wcdma_variation.h"
#include "rrcdata.h"
#include "rrcasn1.h"
#include "rrcasn1Def.h"
#include "l2ultask.h"
#include "wrrc_qsh_ext.h"
#include "qsh_ext.h"


/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/
#define INVALID_LOGICAL_CHANNEL_ID 0xFF
#include "rrccmd.h"
#include "mmtask.h"
#ifdef FEATURE_CMI
#include "mmtask_v.h"
#endif
#define RRC_DEFAULT_BIT_POS   0            /* Points to 0th position of
                                              bit string */

/* The following Bit values for December version of 25.331 specification. 
   If ASN1 for future revisions changes, then these values needs to be 
   updated accordingly */
#define INTEGRITY_CHECK_INFO_PRESENT_POS 0x00 
                                          /* Bit position for integrity 
                                             Check info present */
#define NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT 0x01
                                          /* Number of bits for integrity
                                                                                                         Check Info present */
#define NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO 0x24
                                          /* Number of bits for integrity
                                                                                                         Check Info */

#define NUM_OF_BITS_FOR_DL_DCCH_MSG_TYPE 0x05/* Number of bits for DL DCCH
                                             Message type  */
                                             
#define NUM_OF_BITS_FOR_DL_CCCH_MSG_TYPE 0x03/* Number of bits for DL CCCH
                                             Message type  */

#define NUM_OF_BITS_FOR_UL_DCCH_MSG_TYPE 0x05/* Number of bits for DL DCCH
                                             Message type  */
                                             
#define NUM_OF_BITS_FOR_UL_CCCH_MSG_TYPE 0x02/* Number of bits for DL CCCH
                                             Message type  */
extern boolean ps_rb_re_establish_indicator;

#ifdef FEATURE_INACTIVITY_HANDLING

#define INACTIVITY_TIMER_IN_MS 300000   /* Timer value for INACTIVITY_TIMER 
                                         * currently set at 5 minutes
                                         */

#endif /* FEATURE_INTACTIVITY_HANDLING */


  #ifdef __NON_DEMAND_PAGED_FUNCTION__ 
    #define __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ __NON_DEMAND_PAGED_FUNCTION__ 
  #else
    #define __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  #endif

  #ifdef __NON_DEMAND_PAGED_FUNCTION_END__
    #define __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ __NON_DEMAND_PAGED_FUNCTION_END__
  #else
    #define __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
  #endif
#define MAX_RRC_STACKS 1
#define WRRC_MAX_NUM_SUBS 2

#define WAS_ID_NON_DUAL_WCDMA (wrrc_as_id)

  /* Message Highs*/
#define WRRC_MSG0_HIGH(FMT) \
MSG(MSG_SSID_WCDMA_RRC, MSG_LEGACY_HIGH, FMT)

#define WRRC_MSG1_HIGH(FMT, A) \
MSG_1(MSG_SSID_WCDMA_RRC, MSG_LEGACY_HIGH, FMT, A)

#define WRRC_MSG2_HIGH(FMT, A, B) \
MSG_2(MSG_SSID_WCDMA_RRC, MSG_LEGACY_HIGH, FMT, A, B)

#define WRRC_MSG3_HIGH(FMT, A, B, C) \
  MSG_3(MSG_SSID_WCDMA_RRC, MSG_LEGACY_HIGH, FMT, A, B, C)
  
#define WRRC_MSG4_HIGH(FMT, A, B, C, D) \
  MSG_4(MSG_SSID_WCDMA_RRC, MSG_LEGACY_HIGH, FMT, A, B, C, D)
  
#define WRRC_MSG5_HIGH(FMT, A, B, C, D, E) \
  MSG_5(MSG_SSID_WCDMA_RRC, MSG_LEGACY_HIGH, FMT, A, B, C, D, E)
  
#define WRRC_MSG6_HIGH(FMT, A, B, C, D, E, F) \
  MSG_6(MSG_SSID_WCDMA_RRC, MSG_LEGACY_HIGH, FMT, A, B, C, D, E, F)
  
#define WRRC_MSG7_HIGH(FMT, A, B, C, D, E, F, G) \
  MSG_7(MSG_SSID_WCDMA_RRC, MSG_LEGACY_HIGH, FMT, A, B, C, D, E, F, G)
  
#define WRRC_MSG8_HIGH(FMT, A, B, C, D, E, F, G, H) \
  MSG_8(MSG_SSID_WCDMA_RRC, MSG_LEGACY_HIGH, FMT, A, B, C, D, E, F, G, H)
  
#define WRRC_MSG9_HIGH(FMT, A, B, C, D, E, F, G, H, I) \
  MSG_9(MSG_SSID_WCDMA_RRC, MSG_LEGACY_HIGH, FMT, A, B, C, D, E, F, G, H, I)

/* Message Mediums*/
#define WRRC_MSG0_MED(FMT) \
MSG(MSG_SSID_WCDMA_RRC, MSG_LEGACY_MED, FMT)

#define WRRC_MSG1_MED(FMT, A) \
MSG_1(MSG_SSID_WCDMA_RRC, MSG_LEGACY_MED, FMT, A)

#define WRRC_MSG2_MED(FMT, A, B) \
MSG_2(MSG_SSID_WCDMA_RRC, MSG_LEGACY_MED, FMT, A, B)

#define WRRC_MSG3_MED(FMT, A, B, C) \
MSG_3(MSG_SSID_WCDMA_RRC, MSG_LEGACY_MED, FMT, A, B, C)

#define WRRC_MSG4_MED(FMT, A, B, C, D) \
MSG_4(MSG_SSID_WCDMA_RRC, MSG_LEGACY_MED, FMT, A, B, C, D)

#define WRRC_MSG5_MED(FMT, A, B, C, D, E) \
MSG_5(MSG_SSID_WCDMA_RRC, MSG_LEGACY_MED, FMT, A, B, C, D, E)

#define WRRC_MSG6_MED(FMT, A, B, C, D, E, F) \
MSG_6(MSG_SSID_WCDMA_RRC, MSG_LEGACY_MED, FMT, A, B, C, D, E, F)

#define WRRC_MSG7_MED(FMT, A, B, C, D, E, F, G) \
MSG_7(MSG_SSID_WCDMA_RRC, MSG_LEGACY_MED, FMT, A, B, C, D, E, F, G)

#define WRRC_MSG8_MED(FMT, A, B, C, D, E, F, G, H) \
MSG_8(MSG_SSID_WCDMA_RRC, MSG_LEGACY_MED, FMT, A, B, C, D, E, F, G, H)

#define WRRC_MSG9_MED(FMT, A, B, C, D, E, F, G, H, I) \
MSG_9(MSG_SSID_WCDMA_RRC, MSG_LEGACY_MED, FMT, A, B, C, D, E, F, G, H, I)

#define WRRC_MSG0_ERROR(FMT) \
MSG(MSG_SSID_WCDMA_RRC, MSG_LEGACY_ERROR, FMT)

#define WRRC_MSG1_ERROR(FMT, A) \
MSG_1(MSG_SSID_WCDMA_RRC, MSG_LEGACY_ERROR, FMT, A)

#define WRRC_MSG2_ERROR(FMT, A, B) \
MSG_2(MSG_SSID_WCDMA_RRC, MSG_LEGACY_ERROR, FMT, A, B)

#define WRRC_MSG3_ERROR(FMT, A, B, C) \
MSG_3(MSG_SSID_WCDMA_RRC, MSG_LEGACY_ERROR, FMT, A, B, C)

extern boolean wcdma_rrc_systematic_logging_enabled_nv;

#define WRRC_MSG0_HIGH_OPT(FMT) \
do {\
if (wcdma_rrc_systematic_logging_enabled_nv == TRUE) \
MSG(MSG_SSID_WCDMA_RRC, MSG_LEGACY_HIGH, FMT); \
} while (0)

#define WRRC_MSG1_HIGH_OPT(FMT, A) \
do {\
if (wcdma_rrc_systematic_logging_enabled_nv == TRUE) \
MSG_1(MSG_SSID_WCDMA_RRC, MSG_LEGACY_HIGH, FMT, A); \
} while (0)

#define WRRC_MSG2_HIGH_OPT(FMT, A, B) \
do {\
if (wcdma_rrc_systematic_logging_enabled_nv == TRUE) \
MSG_2(MSG_SSID_WCDMA_RRC, MSG_LEGACY_HIGH, FMT, A, B); \
} while (0)

/* Error Fatals */
#define WRRC_ERR0_FATAL(FMT) \
ERR_FATAL(FMT, 0, 0, 0)

#define WRRC_ERR1_FATAL(FMT, A) \
ERR_FATAL(FMT, A, 0, 0)

#define WRRC_ERR2_FATAL(FMT, A, B) \
ERR_FATAL(FMT, A, B, 0)

#define WRRC_ERR3_FATAL(FMT, A, B, C) \
ERR_FATAL(FMT, A, B, C)

#define WRRC_LOG_ALLOC(A,B) \
log_alloc(A,B)

#define RRC_WAIT_TIME_FOR_L2_ACK_FOR_PCH_TRANSITION 500
#define RRC_WAIT_TIME_FOR_L2_ACK_FOR_PCH_TRANSITION_FOR_EDRX 850
/* This macros are meant for default configuration types that can received by UE 
through SIB16/Intersystem HO command to UTRAN */

#define RRC_DEF_CONFIG_0  0
#define RRC_DEF_CONFIG_1  1
#define RRC_DEF_CONFIG_3  3
#define RRC_DEF_CONFIG_4  4
#define RRC_DEF_CONFIG_6  6
#define RRC_DEF_CONFIG_7  7
#define RRC_DEF_CONFIG_8  8
#define RRC_DEF_CONFIG_9  9
#define RRC_DEF_CONFIG_10 10

#define RRC_DEF_CONFIG_11 11
#define RRC_DEF_CONFIG_12 12
#define RRC_DEF_CONFIG_13 13
#define  RRC_DEF_CONFIG_14 14

#define RRC_DEF_CONFIG_15  15
#define  RRC_DEF_CONFIG_16 16
#define RRC_DEF_CONFIG_17  17
#define RRC_DEF_CONFIG_18  18
#define RRC_DEF_CONFIG_19  19
#define RRC_DEF_CONFIG_20  20
#define  RRC_DEF_CONFIG_21 21
#define  RRC_DEF_CONFIG_22 22
#define  RRC_DEF_CONFIG_23 23
#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif
#define RRC_DEF_CONFIG_INVALID 0xFF

/* Defines for purging WM */
#define  MAX_RB0_SDUS_TOBE_PURGED 4
#define  MAX_SRB1_SDUS_TOBE_PURGED 15
/* Set num SDU’s to be purged to 0xFF, 
 *RLC will delete all the SDU’s which are in water mark 
 */
#define  MAX_SRB2_SDUS_TOBE_PURGED 0xFF

#define RRC_SRB5_SDU_LEN 1 /* For SRB#5, RLC size can utmost be 3/5/10 bits */

/* Minimum RSCP value conveyed to UI/CM during RL FAILURE */
#ifdef FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY
#error code not present
#else
#define MIN_RSCP_RL_FAILURE_OOS 240
#endif

/* Invalid Cell Id for Inter-RAT Measurements */
#define INVALID_CELL_ID   250

/* Global Flag which prevents BPLMN(If BPLMN request is received when UPDATE_SIBs
  was on going) to start till all the SIB_Change_ind commands are processed by RRC 
  procedures & L1*/
extern boolean sib_change_safe_guard;
  /* Given a MIB of a cell, finds out whether the Cell is shared or not.*/
#define RRC_IS_NETWORK_SHARED(mib_ptr)     \
(    \
  (mib_ptr->m.v690NonCriticalExtensionsPresent) &&    \
    (mib_ptr->v690NonCriticalExtensions.masterInformationBlock_v690ext.m.multiplePLMN_ListPresent)    \
)

extern wcdma_UI_event_info_type wcdma_UI_event_info;
extern uint16 wcdma_UI_event_bit_mask;

#define RRC_IS_WCDMA_UI_EVENT_ENABLED(event_bit_mask) wcdma_UI_event_bit_mask & event_bit_mask


#ifdef FEATURE_DUAL_SIM
#define RRCTMR_DEFAULT_WRM_LOCK_WAIT_TIME_IN_MS 120000
#define RRCTMR_WRM_LOCK_WAIT_TIMER_FOR_EST_REQ_IN_MS 30000
#define RRCTMR_DEFAULT_SEARCH_TIME_IN_MS 240000
#define RRCTMR_WRM_LOCK_WAIT_TIMER_FOR_INTERRAT_IN_MS 10000
#define W_QTA_DURATION 30
#endif
#ifdef FEATURE_WCDMA_DEFAULT_CFG_CELLFACH
#define RRC_DEF_CONFIG_0_DC_FACH 0
#define NUM_SRB_DC_CELLFACH 3
#define FACH_MAPPING_PRESENT 0x01
#define RACH_MAPPING_PRESENT 0x02
#define FACH_RACH_MAPPING_PRESENT 0x03

/* Number of macro entries */
#define MAX_ACQ_DB_MACRO_ENTRIES 15

#ifdef FEATURE_FEMTO_CSG
/* Number of CSG entries */
#define MAX_ACQ_DB_CSG_ENTRIES 35

/* Number of entries in temp ACQ DB which holds CSG cells during manual CSG search*/
#define  MAX_ACQ_DB_ENTRIES_TEMP 10

/* ACQ DB SIZE */
#define MAX_ACQ_DB_SIZE (MAX_ACQ_DB_CSG_ENTRIES + MAX_ACQ_DB_MACRO_ENTRIES)
#else

#define  MAX_ACQ_DB_SIZE MAX_ACQ_DB_MACRO_ENTRIES
#endif

extern uint8 default_cfg_ota_mapping[];
extern uint8 default_cfg_recfg_list_srb_present[];
extern uint8 rrcrbrc_default_config_0_dc_fach[];
extern uint8 rrcrbrc_default_config_0_len;
#endif

#ifdef FEATURE_DUAL_SIM
extern boolean rrc_allow_lta;
#endif

#define RRC_MAX_LTE_BAND_SUPPORT 64
/*===================================================================
MACRO       :   PRINT_PLMN_ID 

DESCRIPTION :   Prints PLMN I.D

DEPENDENCIES:   None

RETURN VALUE:   None.

SIDE EFFECTS:   None.

=====================================================================*/
#define     PRINT_PLMN_ID(plmn_id)\
\
  {\
    /*Print MMC here*/\
    MSG_HIGH("NWS:PLMN I.D mcc[0]=0x%x, mcc[1]=0x%x, mcc[2]=0x%x",\
                    plmn_id.mcc[0],plmn_id.mcc[1],plmn_id.mcc[2]);\
\
    /*Print MNC here*/\
    if(plmn_id.num_mnc_digits == 2)\
    {\
      /*Number of MNC digits is 2*/\
      MSG_HIGH("NWS:mnc[0]=0x%x, mnc[1]=0x%x",\
                            plmn_id.mnc[0],plmn_id.mnc[1],0);\
    }\
    else if(plmn_id.num_mnc_digits == 3)\
    {\
      /*Number of MNC digits is 3*/\
      MSG_HIGH("NWS:mnc[0]=0x%x, mnc[1]=0x%x, mnc[2]=0x%x",\
                         plmn_id.mnc[0],plmn_id.mnc[1],plmn_id.mnc[2]);\
    }\
    else\
    {\
     WRRC_MSG1_ERROR("NSW:Wrong number of MNC digits = %d",plmn_id.num_mnc_digits);\
    }\
  }

typedef enum {
  RRC_COMPLETE_CONFIG = 0,
  RRC_PREDEF_CONFIG   = 1,
  RRC_DEFAULT_CONFIG  = 2,
  RRC_INVALID_CONFIG  = 3 /* Internal use only */
} rrc_config_e_type;


/* Following two structures are used to get the ASN.1 decoding/encoding status when
there is any decoding or encoding failure */
typedef struct{
  int32  encode_status;      /* Encoding failure cause      */ 
  int32  pdu_num;            /*pdu on which encoding failed */
  void   *unencoded_pdu_ptr; /*un-encoded pdu ptr           */
} rrc_encode_info_type;

typedef struct{
  int32  decode_status;    /* Decoding failure cause       */
  int32  pdu_num;          /* pdu on which decoding failed */
  int32  pdu_size;         /* size of the encoded string   */
  void  *encoded_pdu_ptr;  /* ptr to encoded string        */
  int32  decode_length;    /* Decoded length incase pre-allocated
                              decode buffer is used */
} rrc_decode_info_type;

/* Followig two definitions map to the OSS error codes defined in 
asn1code.h file */
#define ASN1_CONSTRAINT_VIOLATED        17 /* constraint violation error occured */
#define ASN1_OUT_MEMORY                  8 /* memory-allocation error */

/* MACROs to be used in RRC to set global variable rrc_nv_rel_indicator */

#define RRC_NV_REL_INDICATOR_REL10 0x20

#define RRC_NV_REL_INDICATOR_REL9 0x10

#define RRC_NV_REL_INDICATOR_REL8 0x8
#define RRC_NV_REL_INDICATOR_REL6 0x2
#define RRC_NV_REL_INDICATOR_REL5 0x1
#define RRC_NV_REL_INDICATOR_REL99 0x0

extern uint8 rrc_nv_rel_indicator;

#ifdef FEATURE_MVS
/* Enum identifying the bit rates of AMR mode */
typedef enum rrc_amr_mode_bitrate_e {
  RRC_AMR_MODE_LOW_BITRATE  = 0,
  RRC_AMR_MODE_HIGH_BITRATE = 1,
  RRC_AMR_MODE_MAX
} rrc_amr_mode_bitrate_e_type;
#endif /* FEATURE_MVS */

#ifdef FEATURE_DUAL_SIM
extern boolean rrc_is_sim_id_valid;
extern sys_modem_as_id_e_type rrc_sim_id_recvd;
//#define WRM_DEF_RESOURCE_TYPE TRM_RXTX_BEST
extern boolean wcdma_is_ue_in_dual_standby;
extern boolean wcdma_is_ue_in_sxlte_mode;
extern boolean wcdma_look_for_concurrency_nv;
#endif

typedef enum
{
 RRC_PS_DATA_RATE_RAB_NOT_CONFIGURED,
 RRC_PS_DATA_RATE_NON_ZERO_CONFIGURED,
 RRC_PS_DATA_RATE_ZERO_CONFIGURED
}rrc_ps_data_rate_e_type;

/*===========================================================================

FUNCTION RRC_FREE

DESCRIPTION
  This function is called by the RRC Modules in order to free dynamic
  memory. This function simply calls mem_free by selecting proper heap.

DEPENDENCIES
  The memory manager must have been previously initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e683 */

#define rrc_free(ptr)\
  {\
     modem_mem_free(ptr, MODEM_MEM_CLIENT_WCDMA_RRC_CMD_CRIT);\
     ptr = NULL;\
  }


/*lint -restore */
/*===========================================================================

FUNCTION rrc_free_for_external_cmd

DESCRIPTION
  This function is called by the RRC Modules in order to free dynamic
  memory for external cmds. 
  This function simply calls mem_free by selecting proper heap.

DEPENDENCIES
  The memory manager must have been previously initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e683 */

#define rrc_free_for_external_cmd(ptr)\
  {\
     modem_mem_free(ptr, MODEM_MEM_CLIENT_WCDMA_RRC_CMD_CRIT);\
     ptr  = NULL;\
  }

/*lint -restore */

/*===========================================================================

FUNCTION RRC_MALLOC

DESCRIPTION
  This function is called by the RRC modules in order to allocate 
  dynamic memory. This function simply calls mem_malloc and selects suitable
  heap and returs pointer to the allocated memory.

DEPENDENCIES
  The memory manager must have been previously initialized.

RETURN VALUE
  A pointer to the allocated memory if the memory was successfully allocated,
  otherwise NULL.

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef __GNUC__
static inline void * rrc_malloc(size_t size)
#else
__inline void * rrc_malloc(size_t size)
#endif
{
  void * return_ptr =  NULL;

  return_ptr = 
     modem_mem_calloc(1,size,MODEM_MEM_CLIENT_WCDMA_RRC_CMD_CRIT);


  if(return_ptr == NULL)
  {
    ERR_FATAL("Failed to allocate memory for size 0x%x",size,0,0);
  }

  return(return_ptr);
}



#ifdef __GNUC__
  #define WCDMA_RRC_TASK_LOCK() REX_DISABLE_PREMPTION()

  #define WCDMA_RRC_TASK_FREE() REX_ENABLE_PREMPTION()

#else
  #define WCDMA_RRC_TASK_LOCK() TASKLOCK()
     
  #define WCDMA_RRC_TASK_FREE() TASKFREE()

#endif /* __GNUC__    is for Q6 modem */



/*===========================================================================
MACRO  VALID_RRC_STATE

DESCRIPTION
  Verify that passed value is a valid RRC state. "RRC_STATE_WILDCARD"
  is not considered valid for this macro.

PARAMETERS
  state  - member of rrc_state_e_type 

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
#define VALID_RRC_STATE(state) \
        ( (state == RRC_STATE_DISCONNECTED) || \
          (state == RRC_STATE_CONNECTING)   || \
          (state == RRC_STATE_CELL_FACH)    || \
          (state == RRC_STATE_CELL_DCH)     || \
          (state == RRC_STATE_CELL_PCH)     || \
          (state == RRC_STATE_URA_PCH) )


/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/


#ifdef FEATURE_VOC_AMR_WB
/* Define the types of codes receives in NAS-Syncronisation indicator in OTA msg */
typedef enum
{
  RRC_GSM_FR=0,
  RRC_GSM_HR=1,
  RRC_GSM_EFR=2,
  RRC_FR_AMR=3,
  RRC_HR_AMR=4,
  RRC_UMTS_AMR=5,
  RRC_UMTS_AMR_2=6,
  RRC_TDMA_EFR=7,
  RRC_PDC_EFR=8,
  RRC_FR_AMR_WB=9,
  RRC_UMTS_AMR_WB=10,
  RRC_OHR_AMR=11,
  RRC_OFR_AMR_WB=12,
  RRC_OHR_AMR_WB=13,
  RRC_RESERVED=0xFF
}rrc_codec_types;

#endif /*FEATURE_VOC_AMR_WB*/

/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/


/*This is the structure which contains the two DRX coefficients 
  value received in OTA*/
typedef struct{
  uint32 length;
  uint32 length2;
}utran_drx_cycle_length_coef_type;

/*This structure contains the DRX coefficients along with the 
  timer value for DRX CYCLE 2*/
typedef struct {
  utran_drx_cycle_length_coef_type utran_drx_cycle_length;
  uint32 drx_cycle2_timer_value;
}utran_drx_cycle_type;


/* Data structure to store UE Capability Update Requirement 
   received through RRC Connection Setup Message */
typedef struct {
  boolean  system_specific_cap_update_req_present;
                               /* This indicates whether System Specific 
                                  Capability Update Requirement is present
                                  or not */
  boolean  ue_radio_capability_update_requirement;
                               /* This indicates whether Capability Update
                                  is required or not */
  rrc_system_specific_cap_update_req_list_type system_specific_cap_update_req_list;  
                                  
} rrc_capability_update_requirement_type;


//add local type for n312 and n315 instead of using asn1 type.
typedef struct
{
  uint16 n312;
  uint16 n315;
}rrc_conn_timers_and_constants;

/*type to hold the byte array and the number of bytes in each of the byte
  array*/
typedef struct
{
  uint8 * default_config_ptr;                        /*ptr to default config
                                                      byte array          */    
  uint32   default_config_len;                        /*number of bytes in
                                                      each of the arrays  */

}rrc_default_config_info_type;

/* Data Structure to store the intra frequency reselection statistics */
typedef struct
{
  /* Intra frequency reselection success count since last power up */
  uint32 intra_freq_resel_success_cnt;
  /* Intra frequency reselection failure count due to N-BCCH set up failue since last power up */
  uint32 intra_freq_nbcch_setup_failure_cnt;
  /* Intra frequency reselection failure count due to SIB wait time out since last power up */
  uint32 intra_freq_sib_wait_timeout_cnt;
  /* Intra frequency reselection failure count due to transition failures since last power up */
  uint32 intra_freq_transition_fail_cnt;
  /* Intra frequency reselection failure count due to other causes since last power up */
  uint32 intra_freq_failed_other_causes_cnt;
}rrc_debug_intra_freq_resel_stats_type;

/* Data Structure to store the WTOG reselection statistics */
typedef struct
{
  /* WTOG cell reselection count since last power up */
  uint32 wtog_resel_cnt;
  /* WTOG cell reselection failure count since last power up */
  uint32 wtog_resel_failure_cnt;
}rrc_debug_wtog_resel_stats_type;

/* Data Structure to store the OOS related statistics */
typedef struct
{
  /* Number of times OOS is triggered in idle mode since last power up */
  uint32 oos_idle_cnt;
  /* Number of times OOS is triggered in connected mode since last power up */
  uint32 oos_connected_cnt;
}rrc_debug_oos_stats_type;

/* Data Structure to store RACH Transmission RRC Retry related statistics */
typedef struct
{
  /* Count which captures number of times RRC is successful in receiving RRC 
     procedure level response message from N/W for any RRC message sent on RACH. */
  uint32 succful_rrc_tran_retry_cnt ;
  /* Count which captures number of times RRC Retry Failed due L2 ACK IND 
     with cause failure in sending RRC message on RACH transport channel */
  uint32 rrc_retry_fail_due_to_cmac_status_ind_cnt;
  /* Counter is incremented when RRC exhausts with max time re-transmission
     before receiving RRC procedure level response message on RACH */
  uint32 rrc_retry_fail_due_to_others_cnt;
}rrc_debug_rach_transmission_stats_type;

extern rrc_ProtocolErrorIndicator rrc_protocol_error_indicator;

extern rrc_ProtocolErrorInformation  rrc_protocol_error_information;

extern unsigned short rrc_v_300;

extern unsigned short rrc_v_308;

extern unsigned short rrc_v_304;

extern unsigned short rrc_v_302;

extern rrc_establish_cause_e_type rrc_establishment_cause;

extern rrc_initial_ue_id_choice_e_type rrc_initial_ue_identity_choice;

extern rrc_initial_ue_id_type rrc_initial_ue_identity;

extern boolean rrc_protocol_error_reject;/* Indicates whether a severe 
                                            protocol error */
extern rrc_capability_update_requirement_type rrc_capability_update_requirement;

extern rrc_rel_cause_e_type rrc_release_cause;
           
/* Indicates if valid TMSI is present */
extern boolean                     rrc_tmsi_valid;
/* Indicates if valid PTMSI is present */
extern boolean                     rrc_ptmsi_valid;
/* Indicates if valid IMSI is present */
extern boolean                     rrc_imsi_valid;

extern rrc_tmsi_gsm_map_type       rrc_tmsi;

extern rrc_ptmsi_gsm_map_type      rrc_ptmsi;

extern rrc_imsi_gsm_map_type       rrc_imsi;

/* stores the access classes for the UE */
extern rrc_lo_access_class_e_type  rrc_lo_access_class;
extern rrc_hi_access_class_type    rrc_hi_access_class;

/* stores the admin data received in SIM INSERTED REQ */
extern byte rrc_rcvd_admin_data;

/*stores the current Conn Timers and constants.  updated in rrcdata.c file*/
extern rrc_UE_ConnTimersAndConstants rrc_ue_conn_timers_and_constants;
extern rrc_conn_timers_and_constants conn_timers_and_constants;

extern rrcllc_msg_failure_status_e_type rrc_msg_failure_cause;

extern boolean rrc_skip_dynamic_power_class;

#ifdef FEATURE_FEMTO_CSG
extern rrc_csg_white_list_type rrc_csg_white_list;
extern boolean is_whitelist_valid;
#endif
/* Stores RSSI value - Defined for RSSI enhancements */
#ifdef FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY
#error code not present
#else

extern uint8 rrc_rssi;
#endif

/* Stores timestamp (in CFN ) when a Downlink SDU is received */
extern  uint16  rrc_cfn_before_decode[MAX_NUM_OF_PROCEDURES];



/*constant definition for number of default configs. specified in
  13.7 of 25.331*/
  /* NUM_DEFAULT_CONFIGS has to be 13 for Rel-3 if all configurations are supported */
/* For release-6 default configs upto 31 has been introduced */
#define NUM_DEFAULT_CONFIGS 32

extern rrc_default_config_info_type default_config_info[];

extern uint8 default_config_0[];
extern uint8 default_config_0_len;

extern uint8 default_config_1[];
extern uint8 default_config_1_len;

extern uint8 default_config_3[];
extern uint8 default_config_3_len;

extern uint8 default_config_4[];
extern uint8 default_config_4_len;

extern uint8 default_config_6[];
extern uint8 default_config_6_len;

extern uint8 default_config_7[];
extern uint8 default_config_7_len;

extern uint8 default_config_8[];
extern uint8 default_config_8_len;

extern uint8 default_config_9[];
extern uint8 default_config_9_len;

extern uint8 default_config_10[];
extern uint8 default_config_10_len;

extern uint8 default_config_15[];
extern uint8 default_config_15_len;

extern uint8 default_config_18[];
extern uint8 default_config_18_len;

extern uint8 default_config_19[];
extern uint8 default_config_19_len;

extern uint8 default_config_20[];
extern uint8 default_config_20_len;

extern uint8 default_config_21[];
extern uint8 default_config_21_len;

extern uint8 default_config_16[];
extern uint8 default_config_16_len;

extern uint8 default_config_22[];
extern uint8 default_config_22_len;

extern uint8 default_config_17[];
extern uint8 default_config_17_len;
extern uint8 default_config_23[];
extern uint8 default_config_23_len;


extern uint8 default_config_11[];
extern uint32  default_config_11_len;

extern uint8 default_config_13[];
extern uint32  default_config_13_len;


extern uint8 default_config_12[];
extern uint32  default_config_12_len;

extern uint8 default_config_14[];
extern uint32 default_config_14_len;

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif

#ifdef FEATURE_VOC_AMR_WB
/* This contains the current Codec mode being used  */
extern mvs_mode_type rrc_active_codec;
/* Contains codec info received in the OTA msg, so that it can be sent to MAC in ordered config  */
extern mvs_mode_type rrc_ota_codec_mode_status;
#endif /*FEATURE_VOC_AMR_WB*/

#ifdef FEATURE_3GPP_CSFB
extern boolean rrc_csfb_call_status;
#ifdef FEATURE_WCDMA_CSFB_CALL_OPT
extern boolean rrc_csfb_reset_while_waiting_for_act_rsp;
#endif
#endif

#define INVALID_DL_AM_PDU_RLC_SIZE 0xffffffff
#define INVALID_TRCH_ID 0xFFFF

#define INVALID_DFLOW_ID 8
#define INVALID_QUEUE_ID 8
#define INVALID_PDU_INDEX_ID 8



/* HSDPA Plus Status Variable */
typedef enum
{
  HSDPA_PLUS_INACTIVE,
  HSDPA_PLUS_ACTIVE
} rrc_hsdpa_plus_status_enum_type;

/* HRNTI status */
typedef enum
{
  HRNTI_INVALID,
  HRNTI_VALID
} rrc_hsdpa_hrnti_enum_type;


typedef enum
{
  INTRA_FREQ_CEL_RESEL_SUCCESS = 0,
  INTRA_FREQ_CEL_RESEL_NBCCH_SETUP_FAILURE,
  INTRA_FREQ_CEL_RESEL_SIB_WAIT_TIMEOUT,
  INTRA_FREQ_CEL_RESEL_TRANSITION_FAILURE,
  INTRA_FREQ_CEL_RESEL_FAILED_OTHER_CAUSE,
  WTOG_CEL_RESEL_TRIGGERED,
  WTOG_CEL_RESEL_FAILURE,
  OOS_IN_IDLE_MODE,
  OOS_IN_CONNECTED_MODE,
  RACH_SUCCFUL_RRC_RETRY,
  RACH_RRC_RETRY_FAIL_DUE_TO_CMAC_STATUS_IND,
  RACH_RRC_RETRY_FAIL_DUE_TO_OTHER_CAUSE
} rrc_debug_stats_enum_type;


/*macros to return UE Connected Mode Constants.  There will be used frequently
 *hence a macro is defined rather than a function
 */
#define RRC_GET_N301()    rrc_ue_conn_timers_and_constants.n_301
#define RRC_GET_N302()    rrc_ue_conn_timers_and_constants.n_302
#define RRC_GET_N304()    rrc_ue_conn_timers_and_constants.n_304
#define RRC_GET_N310()    rrc_ue_conn_timers_and_constants.n_310
#define RRC_GET_N312()    conn_timers_and_constants.n312
#define RRC_GET_N313()    rrc_ue_conn_timers_and_constants.n_313
#define RRC_GET_N315()    conn_timers_and_constants.n315

#define RRC_GET_T301()    rrc_ue_conn_timers_and_constants.t_301
#define RRC_GET_T302()    rrc_ue_conn_timers_and_constants.t_302
#define RRC_GET_T304()    rrc_ue_conn_timers_and_constants.t_304
#define RRC_GET_T305()    rrc_ue_conn_timers_and_constants.t_305
#define RRC_GET_T307()    rrc_ue_conn_timers_and_constants.t_307
#define RRC_GET_T308()    rrc_ue_conn_timers_and_constants.t_308
#define RRC_GET_T309()    rrc_ue_conn_timers_and_constants.t_309
#define RRC_GET_T310()    rrc_ue_conn_timers_and_constants.t_310
#define RRC_GET_T311()    rrc_ue_conn_timers_and_constants.t_311
#define RRC_GET_T312()    rrc_ue_conn_timers_and_constants.t_312
#define RRC_GET_T313()    rrc_ue_conn_timers_and_constants.t_313
#define RRC_GET_T314()    rrc_ue_conn_timers_and_constants.t_314
#define RRC_GET_T315()    rrc_ue_conn_timers_and_constants.t_315
#define RRC_GET_T316()    rrc_ue_conn_timers_and_constants.t_316
#define RRC_GET_T317()    rrc_ue_conn_timers_and_constants.t_317

/* RRC Statistics */

#define RRC_DEBUG_INCREMENT_STATS(field)    rrc_debug_stats(field)


/*======================================================

Definition of the Variable ESTABLISHED_RABS

  The following definitions and structures are used to
  define the variable ESTABLISHED_RABS
  as specified in 25.331 section 13.4

=======================================================*/
#define  RRC_INVALID_REEST_TMR 2 /* 0->T314, 1->T315 */

/* This structure is passed to get_amr_mapping info from procedures by
populating the rb id info */

typedef struct
{
  rrcrb_rab_e_type rab_type;      /* Type of RAB - PS, CS-DATA, CS-VOICE */ 
  uint32 num_rbs_for_rab;          /* Number of RBs mapped to this RAB     */
  rb_id_type  rb_id[MAX_RB_PER_RAB];
}rrc_rb_info_for_amr_type;

/* Variable to hold Signalling Radio Bearer information for the
"Established RAB's variable. The onle information needed here is
whether the Radio Bearer has been started or stopped                */
typedef struct
{
  rrc_rb_status_e_type srb_status; /* Status for each SRB           */
}rrc_est_srb_info_type;

/* The Variable "ESTABLISHED_RABS" as defined by the spec.          */
typedef struct
{
  rrc_est_rab_info_type rabs[MAX_RAB_TO_SETUP]; /* Info for each RAB */
  rrc_est_srb_info_type srbs[MAX_SRB_SETUP];    /* Info for each SRB */
}rrc_established_rabs_type;


#ifdef FEATURE_UMTS_PDCP

/* following structure is to keep track of the pdcp receive number.  This
   is to used when it needs to be included in a responce msg to UTRAN */
typedef struct 
{
  uint32 rb_id;     /* Specifies the rb_id */
  uint32 pdcp_sn;   /* Pdcp Sequence number of the rb */
}rrc_rb_with_pdcp_info_type;


/* Variable pdcp_sn_info as defined in spec */
typedef struct
{
  rrc_rb_with_pdcp_info_type rrc_rb_with_pdcp_info[MAX_RAB_TO_SETUP];
}rrc_pdcp_sn_info_type;

#endif /* FEATURE_UMTS_PDCP */

/* Structure used in converting a bitstring given by ASN.1 to a
uint32 usable internaly by RRC. Use this structure only for 
converting bitstrings less than 32 bits in length */
typedef union
{
  uint32 int_val;
  unsigned char char_val[4];
}rrc_int_char_converter_u_type;




/* Used to store the Message Variables for determining HSDPA Action & Bit Mask */
typedef struct
{
  boolean msg_params_present;
  /* If above present, then only one of the following is present */
  boolean hrnti_present;
  uint16 hrnti;
  boolean hspdsch_info_present;
  boolean rl_link_present;
  uint16 new_rl_link;
  boolean ifhho_involved;
  boolean intra_hho_involved;
  boolean hsdsch_info_present;
  boolean hs_dpcch_info_present;
  boolean beta_table_present;
  boolean hrnti_changed;
  boolean mac_hs_restore_tsns;

} rrc_hsdpa_msg_struct_type;

/* Strcuture with the procedure Id and RLC Size Change Status */
typedef struct
{
  rrc_proc_e_type proc_id;
  boolean is_rlc_size_changed;
  rb_id_type rb_id;
  rlc_reestablish_e_type direction;
} rlc_size_change_struct_type;

/* This structure is used to set the DL RLC AM PDU Size/DL RLC UM L1 Ind in RLC structure */
typedef struct
{
  uint8 no_of_rb;
  struct
  {
    rb_id_type rb_id;

    rrc_rlc_dl_entity_e_type rlc_dl_entity;
      union
    {
      struct 
      {
        boolean dl_rlc_am_pdu_size_present;
        uint32 dl_rlc_am_pdu_size; /* Specified in Reconfig Msg */
      } dl_rlc_am_pdu_info;
      struct
      {
        boolean dl_rlc_um_rlc_li_size_present;
        rrc_rlc_dl_um_li_e_type dl_rlc_um_rlc_l1_size; /* Specified in Reconfig Msg */
      } dl_rlc_um_pdu_info;
    } u;
    
  } rb_specified[MAX_RB];

} rlc_size_per_rb_struct_type;

/* This structure is used to set the UL  RLC AM/UM flexible PDU size parameters in RLC structure */

#ifdef FEATURE_MAC_I
typedef struct
{
  rlc_li_e_type      rrc_ul_li_size;           /* LI size, only for AM       */
  uint16             rrc_ul_flex_min_pdu_size; /* Minimum PDU size           */
  uint16             rrc_ul_flex_max_pdu_size; /* Minimum PDU size           */
} rlc_ul_flexi_size_info_type;

/* This structure is used to take the back up of configured RLC info before entering into the PCH state*/
typedef struct {
  
  uint8  nentity;                       /* Number of acknowledged mode     */
                                        /* entities to configure           */
  rlc_am_config_type am_info[UE_MAX_AM_ENTITY];
                                        /* Downlink transparent mode RLC   */
                                        /* configuration for each channel  */
  rlc_lc_action_type am_act[UE_MAX_AM_ENTITY];
                                        /* Action related information for  */
                                        /* RLC UL TM.                      */
} rrc_rlc_am_config_req_type;


#endif /* FEATURE_MAC_I */
/*Stores cell_id for each RL given in OTA msg*/
extern uint32  cell_id_per_rl[L1_MAX_RL]; 
#ifdef FEATURE_QSH_EVENT_METRIC

typedef enum{
  WRRC_CALL_END_TYPE_RRC_CONNECTION_RELEASE,
  WRRC_CALL_END_TYPE_RLF, 
  WRRC_CALL_END_TYPE_HO_FAIL, /* Handover failure */
  WRRC_CALL_END_TYPE_RRC_CONNECTION_EST_FAILURE,/* Used for RRC connection est failure (rach failure) */
  WRRC_CALL_END_TYPE_MAX
}wrrc_call_end_e_type;

typedef struct
{
  wrrc_call_end_e_type type;
  wrrc_call_end_cause_u_type cause;
}wrrc_call_end_data_log_type;

#endif
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*==========================================================================

FUNCTION rrc_translate_establishment_cause

DESCRIPTION
  This function maps the external(ASN1) RRC Connection Establishment cause
  for the internal RRC Connection Established cause received from the NAS 
  through RRC_EST_REQ/OPEN_SESSION_REQ primitive.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  rrc_translate_establishment_cause
(
   rrc_establish_cause_e_type est_cause_int,
   rrc_EstablishmentCause *est_cause_ext
   
);



/*===========================================================================

FUNCTION          RRC_TRANSLATE_ACCESS_CLASS_BARRED_LIST

DESCRIPTION       This function is responsible for translating the access 
                  class barred list from the ASN1 defined linked list format
                  to RRC internal array structure.
DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void rrc_translate_access_class_barred_list(rrc_AccessClassBarredList 
                                             *sib_access_class_barred_list_ptr,
                                            rrc_int_access_class_barred_list_type
                                             *access_class_barred_list_ptr);

/*===========================================================================

FUNCTION          RRC_TRANSLATE_PPAC_ACCESS_CLASS_BARRED_LIST

DESCRIPTION       This function is responsible for translating the access 
                  class barred list from the ASN1 defined linked list format
                  to RRC internal array structure.
DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void rrc_translate_ppac_access_class_barred_list
(
  rrc_LocationRegistrationAccessClassBarredList  *sib_access_class_barred_list_ptr,
  rrc_int_access_class_barred_list_type          *access_class_barred_list_ptr
);

/*===========================================================================

FUNCTION          RRC_TRANSLATE_GSM_MAP_PLMN_ID


DESCRIPTION       

  This function translates a GSM-MAP PLMN Id type defined by ASN.1 into
  the internal type used by RRC and RRC-MM interface. 

DEPENDENCIES

  Both the internal and external PLMN pointers must be initialized.
  If either one is NULL, the convertion will fail. MCC must have 3
  digits and MNC must have either 2 or 3 digits in the external
  PLMN type. Otherwise the convertion will fail.

RETURN VALUE
                  
  None

SIDE EFFECTS
                  
  None

===========================================================================*/
void rrc_translate_gsm_map_plmn_id
(
  rrc_PLMN_Identity  *ext_plmn,      /* Pointer to external PLMN type */
  rrc_plmn_identity_type  *int_plmn  /* Pointer to internal PLMN type */
);

/*===========================================================================

FUNCTION          rrc_translate_gsm_map_plmn_id_with_optional_mcc


DESCRIPTION       

  This function translates a GSM-MAP PLMN Id with optinal MCC type defined by ASN.1 into
  the internal type used by RRC and RRC-MM interface. And also indicates whether the MCC 
  of the externel PLMN type is present or not.

DEPENDENCIES

  Both the internal and external PLMN pointers must be initialized.
  If either one is NULL, the convertion will fail. MCC of external
  PLMN type is optional. Otherwise the convertion will fail.

RETURN VALUE
                  
  None

SIDE EFFECTS
                  
  None

===========================================================================*/
void rrc_translate_gsm_map_plmn_id_with_optional_mcc
(
  boolean *is_mcc_valid,
  rrc_PLMN_IdentityWithOptionalMCC_r6  *ext_plmn,      /* Pointer to external PLMN type */
  rrc_plmn_identity_type  *int_plmn  /* Pointer to internal PLMN type */
);

/*===========================================================================

FUNCTION          RRC_TRANSLATE_CELL_ID


DESCRIPTION       

  This function translates a Cell Id type defined by ASN.1 into
  the internal type used by RRC. 

DEPENDENCIES

  None
  
RETURN VALUE
                  
  The Cell_id converted to the internal type rrc_int_cell_id_type

SIDE EFFECTS
                  
  None

===========================================================================*/
rrc_int_cell_id_type rrc_translate_cell_id
(
  ASN1BitStr32  *ext_cell_id         /* Pointer to external PLMN type */
);

/*===========================================================================

FUNCTION          RRC_TRANSLATE_GSM_MAP_IMSI


DESCRIPTION       

  This function translates a GSM-MAP IMSI type defined by ASN.1 into
  the internal type used by RRC. 

DEPENDENCIES

  Both the internal and external IMSI pointers must be initialized.
  If either one is NULL, the convertion will fail.

RETURN VALUE
                  
  None

SIDE EFFECTS
                  
  None

===========================================================================*/
void rrc_translate_gsm_map_imsi
(
  rrc_IMSI_GSM_MAP *ext_imsi, /* Pointer to external IMSI type */
  rrc_imsi_gsm_map_type  *int_imsi   /* Pointer to internal IMSI type */
);

/*===========================================================================

FUNCTION          RRC_TRANSLATE_IMEI


DESCRIPTION       

  This function translates IMEI type defined by ASN.1 into
  the internal type used by RRC. 

DEPENDENCIES

  Both the internal and external IMEI pointers must be initialized.
  If either one is NULL, the convertion will fail.

RETURN VALUE
                  
  None

SIDE EFFECTS
                  
  None

===========================================================================*/
void rrc_translate_imei
(
  rrc_IMEI  *ext_imei,        /* Pointer to external IMEI type */
  rrc_imei_gsm_map_type  *int_imei   /* Pointer to internal IMEI type */
);

/*===========================================================================

FUNCTION   RRC_INITIALIZE_ESTABLISHED_RABS

DESCRIPTION

  This function initializes the "ESTABLISHED_RABS" variable.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_initialize_established_rabs( void );



#ifdef FEATURE_UMTS_PDCP
/*===========================================================================

FUNCTION   RRC_INITIALIZE_PDCP_SN_INFO

DESCRIPTION

  This function initializes the "PDCP_SN_INFO" variable.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_initialize_pdcp_sn_info(void);

/*===========================================================================

FUNCTION rrc_get_wcdma_rrc_version_nv

DESCRIPTION
  This  function gets the NV item NV_WCDMA_RRC_VERSION_I and stores it in 
  rrc global variable 'rrc_nv_rel_indicator'

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrc_get_wcdma_rrc_pdcp_disabled_nv
(
  sys_modem_as_id_e_type wrrc_as_id
);

#endif /* FEATURE_UMTS_PDCP */

/*===========================================================================

FUNCTION   RRC_ADD_RAB_TO_ESTABLISHED_RABS

DESCRIPTION

  This function adds a new RAB to the ESTABLISHED_RABS variable.
  The caller must verify that the RAB is not already in the
  variable by calling RRC_FIND_RAB_IN_ESTABLISHED_RABS.
        
DEPENDENCIES

  All the relevant information for the RAB must be included
  in the pointer that's passed in to this function.
  
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_add_rab_to_established_rabs
(
  rrc_est_rab_info_type *ptr /* Pointer to the RAB information */
);



/*===========================================================================

FUNCTION   RRC_FIND_IF_VOICE_RAB_EXISTS_IN_ESTABLISHED_RABS

DESCRIPTION

  This function checks if the given RAB id is present in the
  "ESTABLISHED_RABS" variable & belongs to CS domain. 
  If so, it returns TRUE.
  Else it returns FALSE.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
boolean
rrc_find_if_voice_rab_exists_in_established_rabs(rrc_rab_id_type rab_id /* Id of the received RAB */);

/*===========================================================================

FUNCTION   RRC_UPDATE_RB_INFO_TO_EXISTING_VOICE_RAB

DESCRIPTION

  This function checks if the given RAB id is present in the
  "ESTABLISHED_RABS" variable & belongs to CS domain. 
  If so, it updates the RB info.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_update_rb_info_to_existing_rab(rrc_est_rab_info_type *rbe_ptr);

/*===========================================================================

FUNCTION   RRC_FIND_RAB_IN_ESTABLISHED_RABS

DESCRIPTION

  This function checks if the given RAB id is present in the
  "ESTABLISHED_RABS" variable. If so, it returns RRC_RAB_FOUND.
  Else it returns RRC_RAB_NOT_FOUND.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_rab_search_e_type rrc_find_rab_in_established_rabs
(
  rrc_rab_id_type rab_id /* Id of the received RAB */
);
      



 /*===========================================================================

FUNCTION   FIND_RB_IN_EST_RABS

DESCRIPTION

  This function validates the rb_ids in the received RBR Message.
  For user plane RB ids it checks whether the rb to be released
  is present in the established rbs. If not this function returns FALSE.

DEPENDENCIES

  None.
 
RETURN VALUE

  Boolean 
          RRC_RB_FOUND     :rb-id exists in the ESTABLISHED_RAB variable.
          RRC_RB_NOT_FOUND :rb-id does not exist in the ESTABLISHED_RAB variable.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_rb_search_e_type rrc_find_rb_in_est_rabs
(
  rrc_RB_Identity rb_id
);

/*===========================================================================

FUNCTION   VALIDATE_RB_ID_IN_RECONFIG_LIST

DESCRIPTION

  This function validates the rb_ids in the received RB Reconfig Message.
  If signalling RB id is present it returns RRC_RB_PRESENT. For user plane RB ids
  and SRB4 it checks whether the rb to be reconfigured or affected is present in the 
  established rbs. If not this function returns RRC_RB_NOT_PRESENT.

DEPENDENCIES

  None.
 
RETURN VALUE

  rrc_rb_search_e_type 
          RRC_RB_PRESENT     :rb-id exists in the ESTABLISHED_RAB variable or
                              it is a signalling RB.
          RRC_RB_NOT_PRESENT :rb-id is not a signalling RB and it also
                              does not exist in the ESTABLISHED_RAB variable.

SIDE EFFECTS

  None.

===========================================================================*/

rrc_rb_search_e_type validate_rb_id_in_reconfig_list
(
  rrc_RB_Identity rb_id
);


/*===========================================================================

FUNCTION   VALIDATE_RB_ID_IN_RELEASE_LIST

DESCRIPTION

  This function validates the rb_ids in the received RB Release Message.
  If the release is for rb-id 0 through 3 then this returns RRC_RB_NOT_PRESENT
  because SRB0-SRB3 should not be in the release list. For user plane RB ids and RB4,
  it checks whether the rb to be released is present in the established rbs. 
  If not this function returns RRC_RB_NOT_PRESENT.

DEPENDENCIES

  None.
 
RETURN VALUE

  rrc_rb_search_e_type 
          RRC_RB_PRESENT     :rb-id exists in the ESTABLISHED_RAB variable or
                              it is a signalling RB.
          RRC_RB_NOT_PRESENT :rb-id is signalling RB <= SRB3 or 
                              rb-id is not a signalling RB and it also
                              does not exist in the ESTABLISHED_RAB variable.

SIDE EFFECTS

  None.

===========================================================================*/

rrc_rb_search_e_type validate_rb_id_in_release_list
(
  rrc_RB_Identity rb_id
);




/*===========================================================================

FUNCTION   RRC_REMOVE_RAB_FROM_ESTABLISHED_RABS

DESCRIPTION

  This function removes the given RAB id if it's present in the
  "ESTABLISHED_RABS" variable. 
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_remove_rab_from_established_rabs
(
  rrc_rab_id_type rab_id /* Id of the RAB */
);



/*===========================================================================

FUNCTION   RRC_REMOVE_RB_FROM_ESTABLISHED_RABS

DESCRIPTION

  This function removes the given RB id if it's present in the
  "ESTABLISHED_RABS" variable. If the removal of the RB causes
  a RAB to have no RBs mapped to it, the RAB will also be
  removed. 
        
DEPENDENCIES

  Pointer to hold the results must be initialized by the
  calling function.
 
RETURN VALUE

  None. (Results ptr is filled out by this function).

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_remove_rb_from_established_rabs
(
  uint32 rb_id,                  /* RB id to be removed */
  rrc_rb_remove_result_type *ptr /* Pointer for holding the results */
);



/*===========================================================================

FUNCTION   RRC_CLEAR_ESTABLISHED_RABS

DESCRIPTION

  This function clears the "ESTABLISHED_RABS" variable. All the RABS
  will be removed from the variable. This is to be used when the RRC
  connection is released or if all the RABs have to be released due to
  some failure condition.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_clear_established_rabs( void );


/*===========================================================================

FUNCTION   rrc_send_rab_rel_ind_on_ps_release

DESCRIPTION

  This function will send a rab-rel-ind which will follow immediately after 
  RRC_REL_IND for PS rabs.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_send_rab_rel_ind_on_ps_release(void);

/*===========================================================================

FUNCTION          RRC_TRANSLATE_URNTI


DESCRIPTION       

  This function translates U-RNTI type defined by ASN.1 into
  the internal type used by RRC. 

DEPENDENCIES

  Both the internal and external U-RNTI pointers must be initialized.
  If either one is NULL, the convertion will fail.

RETURN VALUE
                  
  None

SIDE EFFECTS
                  
  None

===========================================================================*/
void rrc_translate_urnti
(
  struct rrc_U_RNTI  *ext_urnti,     /* Pointer to external U-RNTI type */
  rrc_int_u_rnti_type  *int_urnti    /* Pointer to internal U-RNTI type */
); 

/*===========================================================================

FUNCTION          RRC_TRANSLATE_URNTI_SHORT


DESCRIPTION       

  This function translates U-RNTI Short type defined by ASN.1 into
  the internal type used by RRC. 

DEPENDENCIES

  Both the internal and external U-RNTI pointers must be initialized.
  If either one is NULL, the convertion will fail.

RETURN VALUE
                  
  None

SIDE EFFECTS
                  
  None

===========================================================================*/
void rrc_translate_urnti_short
(
  struct rrc_U_RNTI_Short  *ext_urnti,     /* Pointer to external U-RNTI type */
  rrc_int_u_rnti_type      *int_urnti    /* Pointer to internal U-RNTI type */
); 

/*===========================================================================

FUNCTION          RRC_TRANSLATE_CRNTI


DESCRIPTION       

  This function translates C-RNTI type defined by ASN.1 into
  the internal type used by RRC. 

DEPENDENCIES

  Both the internal and external C-RNTI pointers must be initialized.
  If either one is NULL, the convertion will fail.

RETURN VALUE
                  
  None

SIDE EFFECTS
                  
  None

===========================================================================*/
void rrc_translate_crnti
(
  ASN1BitStr32  *ext_crnti,         /* Pointer to external C-RNTI type */
  rrc_int_c_rnti_type  *int_crnti    /* Pointer to internal C-RNTI type */
); 
/*===========================================================================

FUNCTION          RRC_CONVERT_GSM_MAP_IMSI_INTO_INT_VALUE


DESCRIPTION       

  This function translates a GSM-MAP IMSI internal type  into
  an interger value type used by RRC LLC. 

DEPENDENCIES
  None

RETURN VALUE
                  
  uint64: returns an integer value of the IMSI

SIDE EFFECTS
                  
  None

===========================================================================*/
uint64 rrc_convert_gsm_map_imsi_into_int_value 
(
  rrc_imsi_gsm_map_type  int_imsi    /* Pointer to internal IMSI type */
);
/*===========================================================================

FUNCTION          RRC_CONVERT_PTMSI_MAP_INTO_INT_VALUE


DESCRIPTION

  This function translates a PTMSI internal type  into
  an interger value type used by RRC LLC.

  FROM 25.304
  For UE-Id types "TMSI and LAI (GSM-MAP)" or "P-TMSI and RAI (GSM-MAP)", only
  the TMSI or P-TMSI parts given as Bitstring(32) shall be used, and "Initial
  UE Identity" shall be interpreted as a binary representation of an integer
  number, where the first bit in the Bitstring represents the highest order bit.

DEPENDENCIES
  None

RETURN VALUE

  uint64: returns an integer value of the PTMSI

SIDE EFFECTS

  None

===========================================================================*/
uint64 rrc_convert_ptmsi_map_into_int_value
(
  rrc_ptmsi_gsm_map_type  int_ptmsi    /* Pointer to internal PTMSI type */
);
/*===========================================================================

FUNCTION          RRC_CONVERT_TMSI_MAP_INTO_INT_VALUE


DESCRIPTION       

  This function translates a TMSI internal type  into
  an interger value type used by RRC LLC. 
  
  FROM 25.304 
  For UE-Id types "TMSI and LAI (GSM-MAP)" or "P-TMSI and RAI (GSM-MAP)", only 
  the TMSI or P-TMSI parts given as Bitstring(32) shall be used, and "Initial 
  UE Identity" shall be interpreted as a binary representation of an integer 
  number, where the first bit in the Bitstring represents the highest order bit.  

DEPENDENCIES
  None

RETURN VALUE
                  
  uint64: returns an integer value of the TMSI

SIDE EFFECTS
                  
  None

===========================================================================*/
uint64 rrc_convert_tmsi_map_into_int_value 
(
  rrc_tmsi_gsm_map_type  int_tmsi    /* Pointer to internal TMSI type */
);
/*===========================================================================

FUNCTION          RRC_CONVERT_IMEI_INTO_INT_VALUE


DESCRIPTION       
  This function translates a IMEI internal type  into  an interger value.

DEPENDENCIES
  None

RETURN VALUE
                  
  uint64: returns an integer value of the IMEI

SIDE EFFECTS
                  
  None

===========================================================================*/
uint64 rrc_convert_imei_into_int_value 
(
  rrc_imei_gsm_map_type  int_imei    /* Local variable internal IMEI type */

);

/*===========================================================================

FUNCTION rrc_convert_initial_ue_identity_into_int_value

DESCRIPTION
  This function converts Initial UE Identity from internal format to integer value.


DEPENDENCIES
  None

RETURN VALUE
  uint64: returns an integer value of the Initial UE dentity

SIDE EFFECTS
  None
===========================================================================*/
uint64 rrc_convert_initial_ue_identity_into_int_value
(
  void
);


/*===========================================================================

FUNCTION rrc_log_protocol_error

DESCRIPTION
  This function logs the protocol error


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrc_log_protocol_error
(
  uint8 rrc_state,                /* The RRC state */
  uint8 rrc_procedure,            /* The RRC procedure */
  uint8 failure_cause,            /* The failure cause */
  uint8 protocol_error_cause      /* The protocol error cause */  
);

/*===========================================================================

FUNCTION rrc_log_map_protocol_cause

DESCRIPTION
  This function maps the asn1 cause to an internal macros

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 rrc_log_map_protocol_cause
(
  rrc_ProtocolErrorCause prot_err_cause
);

/*===========================================================================

FUNCTION rrc_log_map_procedure_name

DESCRIPTION
  This function maps RRC procedure name to Apex procedure name

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 rrc_log_map_procedure_name
(
  uint8 rrc_procedure            /* The RRC procedure */
);
/*===========================================================================

FUNCTION    rrc_get_dl_ccch_message_type 

DESCRIPTION
  This function looks into choice field and identifies the Message type and 
  then returns the index to to the Message Type.
  
DEPENDENCIES
  None

RETURN VALUE
  uint8 : Returns index to the Message type if successful. Otherwise it
          returns 0xFF.

SIDE EFFECTS
  None

===========================================================================*/
uint8 rrc_get_dl_ccch_message_type(
  uint8 *dl_sdu,                            /* Pointer received byte stream */
  uint32 dl_sdu_length                      /* Length of SDU received */
);
/*===========================================================================

FUNCTION    rrc_get_dl_dcch_message_type 

DESCRIPTION
  This function looks into choice field and identifies the Message type and 
  then returns the index to to the Message Type.
  
DEPENDENCIES
  None

RETURN VALUE
  uint8 : Returns index to the Message type if successful. Otherwise it
          returns 0xFF.
SIDE EFFECTS
  None

===========================================================================*/
uint8 rrc_get_dl_dcch_message_type(
  uint8 *dl_sdu,                            /* Pointer received byte stream */
  uint32 dl_sdu_length                      /* Length of SDU received */
);

/*===========================================================================

FUNCTION    rrc_get_ul_ccch_message_type 

DESCRIPTION
  This function looks into choice field and identifies the Message type and 
  then returns the index to to the Message Type.
  
DEPENDENCIES
  None

RETURN VALUE
  uint8 : Returns index to the Message type if successful. Otherwise it
          returns 0xFF.

SIDE EFFECTS
  None

===========================================================================*/
uint8 rrc_get_ul_ccch_message_type(
  uint8 *ul_sdu,                            /* Pointer received byte stream */
  uint32 ul_sdu_length                      /* Length of SDU received */
);
/*===========================================================================

FUNCTION    rrc_get_ul_dcch_message_type 

DESCRIPTION
  This function looks into choice field and identifies the Message type and 
  then returns the index to to the Message Type.
  
DEPENDENCIES
  None

RETURN VALUE
  uint8 : Returns index to the Message type if successful. Otherwise it
          returns 0xFF.
SIDE EFFECTS
  None

===========================================================================*/
uint8 rrc_get_ul_dcch_message_type(
  uint8 *ul_sdu,                            /* Pointer received byte stream */
  uint32 ul_sdu_length                      /* Length of SDU received */
);

/*===========================================================================

FUNCTION  RRC_RSSI_BER_TIMEOUT_HANDLER

DESCRIPTION

  This function is invoked when the RSSI&BER repoting timer expires. It calls
  to L1 to get RSSI and BER info. If the value of RSSI or BER is changed,
  the function will send a RRC-CM interface primitive to inform the changes.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

void rrc_rssi_ber_timeout_handler
( 
  void 
);


 /*===========================================================================
FUNCTION  rrc_bcch_and_nas_plmns_match

DESCRIPTION
  This function does a matching of BCCH and NAS supplied PLMNs. It further
  calls NAS supplied finctions to take care of the special case when NAS
  PLMN is a Home PLMN. This is bacause in that special case, a simple
  memcmp is not correct.

DEPENDENCIES
  Dependency on NAS supplied routines.

RETURN VALUE
  TRUE:  If NAS and BCCH PLMNs match.
  FALSE: If NAS and BCCH PLMNs do not match.

SIDE EFFECTS
  The two passed PLMNs have to be in the correct order. The first parameter
  is expected as the PLMN received on BCCH and the second one is NAS supplied
  PLMN.
  This utility is specifically for comparison between NAS and BCCH PLMNs
  and should NEVER be used for PLMN comparison between 2 NAS supplied PLMNs
  or 2 BCCH PLMNs. 
===========================================================================*/
boolean   rrc_bcch_and_nas_plmns_match
(
  rrc_plmn_identity_type  bcch_plmn,
  rrc_plmn_identity_type  nas_plmn
);


/*===========================================================================
FUNCTION  rrc_bcch_plmns_match

DESCRIPTION
  This function does a matching of two PLMNs obtained from BCCH.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE:  If BCCH PLMNs match.
  FALSE: If BCCH PLMNs do not match.

SIDE EFFECTS
  The two passed PLMNs have to be both from BCCH. For comparison between a
  NAS supplied PLMN and a BCCH PLMN, DO NOT call this function.
===========================================================================*/
boolean rrc_bcch_plmns_match(rrc_plmn_identity_type plmn1, rrc_plmn_identity_type plmn2);

/*===========================================================================
FUNCTION  rrc_update_ue_conn_timers_and_constants

DESCRIPTION
  This function updates rrc_ue_conn_timers_and_constants based on updates
  received in sib1 or Utran Mobility Info messages
  
DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  Updates the global variable rrc_ue_conn_timers_and_constants
===========================================================================*/
uecomdef_status_e_type rrc_update_ue_conn_timers_and_constants
(
  rrc_UE_ConnTimersAndConstants* ue_conn_timers_and_constants_ptr,
  rrc_UE_ConnTimersAndConstants_v3a0ext * ext_ptr,
  rrc_UE_ConnTimersAndConstants_v860ext *ue_conn_timers_and_constants_v860ext_ptr
);
/*===========================================================================
FUNCTION  rrc_update_ue_conn_timers_and_constants

DESCRIPTION
  This function Initializes rrc_ue_conn_timers_and_constants to default values
  as all parameters are mandatory default
  
DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  Initializes the global variable rrc_ue_conn_timers_and_constants with
  default values as all parameters are Mandatory Default
===========================================================================*/
void rrc_initialize_ue_conn_timers_and_constants
(
  void
);


/*====================================================================
FUNCTION: rrc_translate_n313

DESCRIPTION:
  This function translates the enum for timer N313 into an int16 value.

DEPENDENCIES:
  None

RETURN VALUE:
  uint16 value corresponding to the enum passed, 0 if invalid

SIDE EFFECTS:
====================================================================*/
uint16 rrc_translate_n313(rrc_N_313 val);


/*===========================================================================

FUNCTION RRC_CHECK_RBS_ASSOCIATED_WITH_RE_EST_TIMER

DESCRIPTION

  This function looks into the ESTABLISHED_RABS and checks if there are any
  RBs associated with the particular re-estblishment timer
  
DEPENDENCIES


RETURN VALUE
  Returns TRUE if there are any RBs associated with that particular 
  re_establishment timer. Otherwise it returns FALSE.
 

SIDE EFFECTS

  None

===========================================================================*/
boolean rrc_check_rbs_associated_with_re_est_timer
(    
  rrc_Re_EstablishmentTimer re_est_timer
);

/*===========================================================================

FUNCTION RRC_RELEASE_RBS_ASSOCIATED_WITH_RE_EST_TIMER

DESCRIPTION

  This function lloks into the ESTABLISHED_RABS and releases the RBs associated
  with the particular re-estblishment timer
  
DEPENDENCIES


RETURN VALUE
  Returns TRUE if the RBs associated with that particular re_establishment
  timer are released. Otherwise it returns FALSE.
 

SIDE EFFECTS
  Before releasing RBs assoicated with a re_establishment timer, a check needs
  to be performed whether there are any RABs present assocaited with that
  particular timer.

===========================================================================*/
boolean rrc_release_rbs_associated_with_re_est_timer
(    
  rrc_Re_EstablishmentTimer re_est_timer,
  rrc_proc_e_type rrc_proc
);

/*===========================================================================

FUNCTION RRC_CONVERT_T305_TO_MS

DESCRIPTION
  This function converts the rrc_T_305 enums to milliseconds

DEPENDENCIES
  None

RETURN VALUE
  uint32

SIDE EFFECTS
  None
===========================================================================*/
uint32 rrc_convert_t305_to_ms
(
  rrc_T_305 rrccu_t_305                    /* enum for Timer T305 */
);
/*===========================================================================

FUNCTION RRC_TRANSLATE_STATE_INDICATOR_TO_STATE

DESCRIPTION
  This function converts the Received RRC state indicator into internal
  RRC state format
  
DEPENDENCIES
  None

RETURN VALUE
  rrc_state_e_type : returns the RRC state
  
SIDE EFFECTS

  None

===========================================================================*/
rrc_state_e_type rrc_translate_state_indicator_to_state
(    
  rrc_RRC_StateIndicator state_indicator
);
/*===========================================================================

FUNCTION RRC_CHECK_RABS_ASSOCIATED_WITH_CN_DOMAIN

DESCRIPTION

  This function looks into the ESTABLISHED_RABS and checks if there are any
  RABs associated with the particular CN Domain
  
DEPENDENCIES


RETURN VALUE
  Returns TRUE if there are any RABs associated with that particular 
  CN Domain. Otherwise it returns FALSE.
 

SIDE EFFECTS

  None

===========================================================================*/
boolean rrc_check_rabs_associated_with_cn_domain
(    
  rrc_cn_domain_identity_e_type rrc_cn_domain_id
);


/*===========================================================================

FUNCTION   RRC_FIND_RAB_IN_ESTABLISHED_RABS

DESCRIPTION

  This function checks if the given RAB id is present in the
  "ESTABLISHED_RABS" variable. If so, it returns RRC_RAB_FOUND.
  Else it returns RRC_RAB_NOT_FOUND.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
rrc_established_rabs_for_a_domain_type rrc_get_rabs_in_established_rabs_for_given_domain
(
  rrc_cn_domain_identity_e_type cn_domain /* Id of the received RAB */
);


/*===========================================================================
FUNCTION   RRC_FIND_CN_DOMAIN_ID_FOR_RB_IN_EST_RABS

DESCRIPTION
  For user plane RB ids it checks whether the rb is present in the established rbs. 
  It returns the corresponding cn_domain

DEPENDENCIES
  None.
 
RETURN VALUE
  rrc_rab_id_type rab_id

SIDE EFFECTS
  None.
===========================================================================*/
boolean rrc_find_cn_domain_id_for_rb_in_est_rabs
(
  rrc_RB_Identity rb_id,
  rrc_cn_domain_identity_e_type *cn_domain_id
);

/*===========================================================================

FUNCTION          RRC_TRANSLATE_URA_ID


DESCRIPTION       

  This function translates URA_ID type defined by ASN.1 into
  the internal type used by RRC. 

DEPENDENCIES

  Both the internal and external URA_ID pointers must be initialized.
  If either one is NULL, the conversion will return .

RETURN VALUE
                  
  None

SIDE EFFECTS
                  
  None

===========================================================================*/
void rrc_translate_ura_id
(
  ASN1BitStr32  *ext_ura_id,          /* Pointer to external C-RNTI type */
  rrc_int_ura_id_type  *int_ura_id    /* Pointer to internal C-RNTI type */
);


/*===========================================================================

FUNCTION VALIDATE_FREQUENCY_INFO

DESCRIPTION

  This function validates the UARFCN values in the provided Frequency Info
  IE according to table 5.2 in 25.101.
  
DEPENDENCIES


RETURN VALUE
  Returns FAILURE if either UARFCN is invalid.
 

SIDE EFFECTS

  None

===========================================================================*/
uecomdef_status_e_type validate_frequency_info
(    
  rrc_FrequencyInfo *ie_ptr
);


/*===========================================================================

FUNCTION        RRCCSP_IS_MCC_AND_850_800_SUPPORTED_BAND_COMB_VALID()

DESCRIPTION     This function looks at the passed in UARFCN that belongs to
                the overlapping region of 850 and 800 bands, and determines 
                if the MCC and supported band combination is valid.

RETURN VALUE    Returns FAILURE if UARFCN and PLMN_ID combination is invalid.

SIDE EFFECTS    None

===========================================================================*/
boolean rrccsp_is_mcc_and_850_800_supported_band_comb_valid(
  uint32 uarfcn_dl, 
  rrc_plmn_identity_type plmn_id,
  w_uarfcn_source_e_type source
  , boolean check_and_update_ui_rf_mask
  );


/*===========================================================================

FUNCTION          RRC_CSP_SEND_INTERRAT_CELL_CHANGE_IND

DESCRIPTION       This function is responsible for filling up data structures
                  for INTERRAT_CELL_CHANGE_IND to MM sent by CSP only for GTOW
                  PCCO. This also allocates cmd buffer and sends the command.
                  It also frees allocated command and embedded buffers
                  when failure is detected before sending the command.

DEPENDENCIES

RETURN VALUE
                  None.
                  
SIDE EFFECTS
                  None.
===========================================================================*/
void rrc_csp_send_interrat_cell_change_ind
(
void
);


#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
/*===========================================================================

FUNCTION        validate_dl_arfcn()

DESCRIPTION     This function validates the DL UARFCN and PLMN_id
                combination.

RETURN VALUE    FAILURE if UARFCN and PLMN_id combination is invalid,
                SUCCESS otherwise
 
SIDE EFFECTS    None

===========================================================================*/
uecomdef_status_e_type validate_dl_arfcn
(    
  uint32                 uarfcn_dl,
  rrc_plmn_identity_type plmn_id,
  w_uarfcn_source_e_type source
  , boolean check_and_update_ui_rf_mask
);

#else

/*===========================================================================

FUNCTION        validate_dl_arfcn()

DESCRIPTION     This function validates the DL UARFCN values.
  
RETURN VALUE    FAILURE if UARFCN is invalid,
                SUCCESS otherwise
 
SIDE EFFECTS    None

===========================================================================*/
uecomdef_status_e_type validate_dl_arfcn
(    
  uint32                 uarfcn_dl
  , boolean check_and_update_ui_rf_mask
);
#endif /* FEATURE_MCC_AND_850_800_BAND_COMBINATION */


#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
/*===========================================================================

FUNCTION        validate_fdd_frequency()

DESCRIPTION     This function validates the UARFCN (in the provided
                rrc_FrequencyFDD IE) and the PLMN_ID combination.
                Notice that the plmn_id information is used only when
                the UARFCN is in the overlapping region of 850 and 800 bands.
               
RETURN VALUE    Returns FAILURE if UARFCN and plmn_id combination is invalid.

SIDE EFFECTS    None

===========================================================================*/
uecomdef_status_e_type validate_fdd_frequency
(    
  rrc_FrequencyInfoFDD *fdd_frequency,
  rrc_plmn_identity_type plmn_id
);

#else

/*===========================================================================

FUNCTION        validate_fdd_frequency()

DESCRIPTION     This function validates the UARFCN values in the provided
                rrc_FrequencyFDD IE according to table 5.2 in 25.101.
  
RETURN VALUE    Returns FAILURE if either UARFCN is invalid.
 
SIDE EFFECTS    None

===========================================================================*/
uecomdef_status_e_type validate_fdd_frequency
(    
  rrc_FrequencyInfoFDD *fdd_frequency
);

#endif /* FEATURE_MCC_AND_850_800_BAND_COMBINATION */

/*===========================================================================

FUNCTION        IS_FREQ_IN_OVERLAPPING_850_AND_800_BANDS

DESCRIPTION     This function determines if the UARFCN passed to it is in 
                the overlapping region of 850 and 800 bands or not.


RETURN VALUE    TRUE:  UARFCN is in the overlapping region
                FALSE: Otherwise

SIDE EFFECTS    

===========================================================================*/
boolean is_freq_in_overlapping_850_and_800_bands(uint32 freq);


/*==========================================================
FUNCTION  : get_rx_window_size()

DESCRIPTION : 
  Return the Window Size for receiving.

DEPENDENCIES: 
  None.

RETURN VALUE:
  Window size for receiving for RLC.

SIDE EFFECTS:
  None.
============================================================*/
uint16 get_rx_window_size
(
  rrc_ReceivingWindowSize rrc_rx_window_size
);

/*===========================================================================

FUNCTION   rrc_is_voice_rab_present

DESCRIPTION

  This function indicates whether any voice RAB is present or not in est_rabs database.
  This function is going to be used by MAC/RRC to verify whether Voice RAB is present 
  or not.
        
DEPENDENCIES

None

RETURN VALUE

  TRUE: if voice RAB exist in est_rabs database
  FALSE: if voice RAB doesnt exist in est_rabs database

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_is_voice_rab_present (void);

/*===========================================================================

FUNCTION   rrc_is_cs_data_rab_present

DESCRIPTION

  This function indicates whether any cs data RAB is present or not in est_rabs database.
  This function is going to be used by RRC to verify whether cs data RAB is present 
  or not.
  This function has been modified such that it returns TRUE only if RRCRB_CS_DATA_RAB is of DCH type.        
DEPENDENCIES

None

RETURN VALUE

  TRUE: if cs data RAB exist in est_rabs database
  FALSE: if cs data RAB doesnt exist in est_rabs database

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_is_cs_data_rab_present (void);

/*===========================================================================

FUNCTION   rrc_get_rb_id_list_associated_with_voice_rab

DESCRIPTION

  This function gives the list of RB ids associated with a Voice RAB.
        
DEPENDENCIES

  None.

ARGUMENTS
  *num_rb: number of rb associated with Voice RAB, set to 0 if Voice RAB is not established.
                set 'num_rb' to NULL if this function needs to be called to know whether AMR is 
                established or not.
  rb_id_list: list of rbIds associated with Voice RAB

RETURN VALUE

  boolean: TRUE if Voice RAB is established elase FALSE.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_get_rb_id_list_associated_with_voice_rab 
(
  uint32 *num_rb,
  uint32 rb_id_list[MAX_RB_PER_RAB]
);

/*===========================================================================

FUNCTION   rrc_get_rab_type_for_given_rb_id 

DESCRIPTION

  This function returns the RAB type of a RAB associated with a given RB id. This function
  returns RRCRB_UNKNOWN_RAB if there is no RAB exist for a given RB id.
        
DEPENDENCIES

  None.

ARGUMENTS
  rbId: rb Id

RETURN VALUE

  rrcrb_rab_e_type: rab type

SIDE EFFECTS

  None.

===========================================================================*/
rrcrb_rab_e_type rrc_get_rab_type_for_given_rb_id 
(
  uint32 rb_id
);

/*===========================================================================

FUNCTION   rrc_get_rab_type_for_given_rab_id 

DESCRIPTION

  This function returns the RAB type of a given RAB id. This function returns
  RRCRB_UNKNOWN_RAB if RAB with a given RAB id is not found.
        
DEPENDENCIES

  None.

ARGUMENTS
  rab_Id: RAB Id

RETURN VALUE

  rrcrb_rab_e_type: rab type

SIDE EFFECTS

  None.

===========================================================================*/
rrcrb_rab_e_type rrc_get_rab_type_for_given_rab_id 
(
  uint32 rab_id
);

/*===========================================================================

FUNCTION   rrc_get_idx_in_rrc_est_rabs_for_rab_id

DESCRIPTION

  This function returns the index in rrc_est_rabs for a given rab_id.
        
ARGUMENTS

RETURN VALUE

SIDE EFFECTS

  None.

===========================================================================*/
uint8 rrc_get_idx_in_rrc_est_rabs_for_rab_id
(
  rrc_rab_id_type rab_id
);

/*===========================================================================

FUNCTION   rrc_is_voice_rab_present_for_given_reest_timer 

DESCRIPTION

  This function returns whether voice RAB is present for a given re-establishment timer.
  This function also returns the rab-id of voice RAB it present.
        
DEPENDENCIES

  None.

ARGUMENTS
  re_est_timer: timer for which voice RAB needs to be determined
  *rab_id_ptr: returns RAB id if voice RAB with a given re-establishment timer is present,
                     else returns RRC_INVALID_RAB_ID.
                     Specify this argument as NULL if rab_id is not needed.

RETURN VALUE

  boolean: TRUE if voice RAB with a given re-establishment timer found otherwise FALSE.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_is_voice_rab_present_for_given_reest_timer 
(
  rrc_Re_EstablishmentTimer re_est_timer,
  rrc_rab_id_type *rab_id_ptr
);

/*===========================================================================

FUNCTION   rrc_update_rab_type_in_rab_info

DESCRIPTION
 This function is updates the rab type in 'rrc_est_rab_info_type' structure
 using information stored in ASN1 'rrc_RAB_InformationSetupList'.
        
DEPENDENCIES

  There is no error checking done in this function. Its assumed that ASN1
  structure 'rrc_RAB_InformationSetup' is already validated.

ARGUMENTS:

  rrc_RAB_InformationSetup *: pointer to ASN1 structure,
  rrc_est_rab_info_type *: pointer to internal RRC est_rab structure in which this 
                                       function will fill appropriate rab_type.

RETURN VALUE

rrcrb_rab_e_type: returns the rab_type which is filled in est_rab_info

SIDE EFFECTS

  None.

===========================================================================*/
rrcrb_rab_e_type rrc_update_rab_type_in_rab_info
(
  rrc_RAB_InformationSetup *rab_info_setup_ptr,
  rrc_est_rab_info_type *est_rab_info_ptr 
);

/*===========================================================================

FUNCTION   rrc_update_rab_type_in_rab_info_r5

DESCRIPTION
 This function is updates the rab type in 'rrc_est_rab_info_type' structure
 using information stored in ASN1 'rrc_RAB_InformationSetupList'.
        
DEPENDENCIES

  There is no error checking done in this function. Its assumed that ASN1
  structure 'rrc_RAB_InformationSetup' is already validated.

ARGUMENTS:

  rrc_RAB_InformationSetup *: pointer to ASN1 structure,
  rrc_est_rab_info_type *: pointer to internal RRC est_rab structure in which this 
                                       function will fill appropriate rab_type.

RETURN VALUE

rrcrb_rab_e_type: returns the rab_type which is filled in est_rab_info

SIDE EFFECTS

  None.

===========================================================================*/
rrcrb_rab_e_type rrc_update_rab_type_in_rab_info_r5
(
  rrc_RAB_InformationSetup_r5 *rab_info_setup_ptr,
  rrc_est_rab_info_type *est_rab_info_ptr 
);



/*===========================================================================

FUNCTION RRC_HSDPA_INITIAL_VALIDATION

DESCRIPTION

  This function does the initial level validation of HSDPA.
  
DEPENDENCIES


RETURN VALUE
  Success/Failure
 

SIDE EFFECTS

  None

===========================================================================*/
uecomdef_status_e_type rrc_hsdpa_initial_validation
(    
  rrc_RRC_StateIndicator rrc_state_from_msg,
  OSS_UINT32 rrc_mode_Specific_Info_tdd_chosen
);

/*===========================================================================

FUNCTION RRC_HSDPA_INITIAL_VALIDATION_R6

DESCRIPTION

  This function does the initial level validation of HSDPA.
  
DEPENDENCIES


RETURN VALUE
  Success/Failure
 

SIDE EFFECTS

  None

===========================================================================*/
uecomdef_status_e_type rrc_hsdpa_initial_validation_r6
(    
  rrc_RRC_StateIndicator rrc_state_from_msg,
  rrc_DL_HSPDSCH_Information_r6* hspdsch_info_ptr
);
/*===========================================================================

FUNCTION   rrc_update_rab_type_in_rab_info_r6

DESCRIPTION
 This function is updates the rab type in 'rrc_est_rab_info_type' structure
 using information stored in ASN1 'rrc_RAB_InformationSetupList'.
        
DEPENDENCIES

  There is no error checking done in this function. Its assumed that ASN1
  structure 'rrc_RAB_InformationSetup' is already validated.

ARGUMENTS:

  rrc_RAB_InformationSetup *: pointer to ASN1 structure,
  rrc_est_rab_info_type *: pointer to internal RRC est_rab structure in which this 
                                       function will fill appropriate rab_type.

RETURN VALUE

rrcrb_rab_e_type: returns the rab_type which is filled in est_rab_info

SIDE EFFECTS

  None.

===========================================================================*/
rrcrb_rab_e_type rrc_update_rab_type_in_rab_info_r6
(
  rrc_RAB_InformationSetup_r6 *rab_info_setup_ptr,
  rrc_est_rab_info_type *est_rab_info_ptr 
);


/*===========================================================================
FUNCTION   rrc_is_ps_domain_rab_present

DESCRIPTION
  This function returns TRUE if any PS RAB is present in EST_RABs.
        
DEPENDENCIES
  None

ARGUMENTS:
  None

RETURN VALUE
  TRUE - PS RAB is present.
  FALSE - PS RAB isn't present.

SIDE EFFECTS
  None.
===========================================================================*/
boolean rrc_is_ps_domain_rab_present
(
  void
);

/*===========================================================================

FUNCTION   rrc_update_rab_type_in_rab_info_r7

DESCRIPTION
 This function is updates the rab type in 'rrc_est_rab_info_type' structure
 using information stored in ASN1 'rrc_RAB_InformationSetupList'.
        
DEPENDENCIES

  There is no error checking done in this function. Its assumed that ASN1
  structure 'rrc_RAB_InformationSetup' is already validated.

ARGUMENTS:

  rrc_RAB_InformationSetup *: pointer to ASN1 structure,
  rrc_est_rab_info_type *: pointer to internal RRC est_rab structure in which this 
                                       function will fill appropriate rab_type.

RETURN VALUE

rrcrb_rab_e_type: returns the rab_type which is filled in est_rab_info

SIDE EFFECTS

  None.

===========================================================================*/
rrcrb_rab_e_type rrc_update_rab_type_in_rab_info_r7
(
  rrc_RAB_InformationSetup_r7 *rab_info_setup_ptr,
  rrc_est_rab_info_type *est_rab_info_ptr 
);

/*===========================================================================

FUNCTION          RRC_TRANSLATE_ERNTI


DESCRIPTION       

  This function translates Primary and Secondary ERNTI type defined by ASN.1 
  into the internal type used by RRC. 

DEPENDENCIES

  Both the internal and external Primary ERNTI pointers must be initialized.
  If either one is NULL, the convertion will fail.

RETURN VALUE
                  
  Success/Failure

SIDE EFFECTS
                  
  None

===========================================================================*/
uecomdef_status_e_type rrc_translate_ernti
(
  ASN1BitStr32  *ext_ernti,          /* Pointer to external ERNTI type */
  uint16  *int_ernti    /* Pointer to internal ERNTI type */
);


/*===========================================================================

FUNCTION   rrc_indicate_layer1_next_config_is_for_voice

DESCRIPTION

  This function will indicate layer1 that next configuration is for AMR by calling some 
  feature specific call back function provided by layer1.
        
DEPENDENCIES

  None
 
RETURN VALUE

  None
SIDE EFFECTS

  None.

===========================================================================*/
void rrc_indicate_layer1_next_config_is_for_voice (void);

/*===========================================================================

FUNCTION   rrc_indicate_layer1_next_config_will_release_voice_rab

DESCRIPTION

  This function will indicate layer1 that next configuration to physical layer
  will release AMR. This is done by calling some feature specific call back
  function provided by layer1.
        
DEPENDENCIES

  None
 
RETURN VALUE

  None
SIDE EFFECTS

  None.

===========================================================================*/
void rrc_indicate_layer1_next_config_will_release_voice_rab (void);

/*===========================================================================

FUNCTION   rrc_is_given_rb_rel_list_releases_all_rb_for_voice_rab

DESCRIPTION

  This function determines whether the given 'rrc_RB_InformationReleaseList'
  will result in release of voice call or not.
        
DEPENDENCIES

  None
 
RETURN VALUE

  TRUE: If voice RAB is not present in est_rabs or if rb_release_list will result
        in releasing all RBs associated with RAB.
  FALSE: If voice RAB still exist after processing rb_release_list.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_is_given_rb_rel_list_releases_all_rb_for_voice_rab 
(
  rrc_RB_InformationReleaseList *rb_release_list_ptr
);



/*===========================================================================

FUNCTION   rrc_get_hsdpa_cell_capability_indicator

DESCRIPTION

  This function will indicate if the current cell supports HSDPA.
        
DEPENDENCIES

  None
 
RETURN VALUE

  None
SIDE EFFECTS

  None.

===========================================================================*/
extern sys_hs_ind_e_type rrc_get_3g_cell_capability_indicator (void);

/*===========================================================================

FUNCTION          RRC_SEND_3G_CELL_CAPABILITY_INFORMATION

DESCRIPTION       This function sends 3G cell related info to MM

DEPENDENCIES
                  None
                  
RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void rrc_send_3g_cell_capability_information(sys_hs_ind_e_type hsdpa_hsupa_indication);
#ifdef FEATURE_HSPA_CALL_STATUS_IND
/*===========================================================================

FUNCTION          rrc_send_hspa_call_status_info

DESCRIPTION       This function sends 3G cell related info to MM

DEPENDENCIES
                  None
                  
RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void rrc_send_hspa_call_status_info
(
  void
);
#endif /* FEATURE_HSPA_CALL_STATUS_IND */


/*===========================================================================

FUNCTION rrc_initialize_rlc_size_change

DESCRIPTION

  This function initializes the RLC Size Change structure
  
DEPENDENCIES


RETURN VALUE
  None
 

SIDE EFFECTS

  None

===========================================================================*/
void rrc_initialize_rlc_size_change
(
  void
);

/*====================================================================
FUNCTION: rrc_set_rlc_size_change_status()

DESCRIPTION:
  This function sets the RLC PDU Size per RB in the global
  rlc size change structure

DEPENDENCIES:
  None

RETURN VALUE:
  RLC Size

SIDE EFFECTS:
  None.
====================================================================*/
void rrc_set_rlc_size_change_status(rrc_proc_e_type proc_id, boolean rlc_size_change_status,
                                       rrc_RB_Identity rb_id, rlc_reestablish_e_type direction);

/*====================================================================
FUNCTION: rrc_get_rlc_size_change_status()

DESCRIPTION:
  This function gets the RLC Size Change Status based on the Procedure Id

DEPENDENCIES:
  None

RETURN VALUE:
  RLC Size

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrc_get_rlc_size_change_status_for_procedure_id(rrc_proc_e_type proc_id);


/*===========================================================================

FUNCTION rrc_initialize_rlc_size_per_rb

DESCRIPTION

  This function initializes the RLC Size per RB in the global structure
  rlc_size_per_rb
  
DEPENDENCIES


RETURN VALUE
  None
 

SIDE EFFECTS

  None

===========================================================================*/

void rrc_initialize_rlc_size_per_rb
(
  void
);

/*===========================================================================

FUNCTION          RRC_TRANSLATE_HRNTI


DESCRIPTION       

  This function translates HRNTI type defined by ASN.1 into
  the internal type used by RRC. 

DEPENDENCIES

  Both the internal and external HRNTI pointers must be initialized.
  If either one is NULL, the convertion will fail.

RETURN VALUE
                  
  Success/Failure

SIDE EFFECTS
                  
  None

===========================================================================*/
uecomdef_status_e_type rrc_translate_hrnti
(
  ASN1BitStr32  *ext_hrnti,          /* Pointer to external HRNTI type */
  uint16  *int_hrnti    /* Pointer to internal HRNTI type */
);



/*===========================================================================

FUNCTION RRC_SET_HSDPA_ACTION

DESCRIPTION

  This function sets the HSDPA Action Variable.
  
DEPENDENCIES


RETURN VALUE
  None
 

SIDE EFFECTS

  None

===========================================================================*/

void rrc_set_hsdpa_action
(
  hsdpa_action_enum_type action
);



/*==========================================================
FUNCTION  : rrc_get_hsdpa_action()

DESCRIPTION : Gets the HSDPA Action variable

DEPENDENCIES: None.

RETURN VALUE: Action

SIDE EFFECTS: None.
============================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
hsdpa_action_enum_type rrc_get_hsdpa_action
(
  void
);


/*===========================================================================

FUNCTION RRC_SET_HSDPA_STATUS

DESCRIPTION

  This function sets the HSDPA Status Variable.
  
DEPENDENCIES


RETURN VALUE
  None
 

SIDE EFFECTS

  None

===========================================================================*/

void rrc_set_hsdpa_status
(
  rrc_hsdpa_status_enum_type status
);



/*===========================================================================

FUNCTION rrc_get_dl_am_rlc_pdu_size

DESCRIPTION

  This function finds the DL AM RLC PDU Size as per the RB specified from the 
  global structure rlc_size_per_rb
  
DEPENDENCIES


RETURN VALUE
  DL AM RLC PDU Size if found else INVALID_DL_AM_PDU_RLC_SIZE
 

SIDE EFFECTS

  None

===========================================================================*/

uint32 rrc_get_dl_am_rlc_pdu_size
(
  rrc_RB_Identity rb_id
);


/*===========================================================================

FUNCTION rrc_get_dl_um_li

DESCRIPTION

  This function finds the DL UM LI as per the RB specified from the 
  global structure rlc_size_per_rb
  
DEPENDENCIES


RETURN VALUE
  DL UM LI Size if found else INVALID_DL_AM_PDU_RLC_SIZE
 

SIDE EFFECTS

  None

===========================================================================*/

rrc_rlc_dl_um_li_e_type rrc_get_dl_um_li
(
  rrc_RB_Identity rb_id
);

/*===========================================================================

FUNCTION rrc_set_dl_am_rlc_pdu_size

DESCRIPTION

  This function sets the DL AM RLC PDU Size as per the RB specified in the 
  global structure rlc_size_per_rb
  
DEPENDENCIES


RETURN VALUE
  DL AM RLC PDU Size if found else INVALID_DL_AM_PDU_RLC_SIZE
 

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrc_set_dl_am_rlc_pdu_size
(
  rrc_RB_Identity rb_id,
  uint32 dl_rlc_am_pdu_size
);


/*===========================================================================

FUNCTION rrc_set_dl_um_li

DESCRIPTION

  This function sets the DL UM LI indication as per the RB specified in the 
  global structure rlc_size_per_rb
  
DEPENDENCIES


RETURN VALUE
  Success/Failure
 

SIDE EFFECTS

  None

===========================================================================*/

uecomdef_status_e_type rrc_set_dl_um_li
(
  rrc_RB_Identity rb_id,
  rrc_DL_UM_RLC_LI_size dl_rlc_um_li
);

/*===========================================================================
FUNCTION   is_ctch_present_in_serving_cell

DESCRIPTION
  This function returns TRUE if CTCH is present on current cell otherwise
  returns false.
 
DEPENDENCIES
  None.
 
RETURN VALUE
  boolean

SIDE EFFECTS
  None.
===========================================================================*/
boolean is_ctch_present_in_serving_cell(rrc_state_e_type next_rrc_state);


/*===========================================================================

FUNCTION rrc_get_lac_id 

DESCRIPTION
  
This function returns LAC id for the serving cell.
  
DEPENDENCIES
  None

RETURN VALUE
  SUCCESS or FAILURE
  
SIDE EFFECTS

  None

===========================================================================*/
uecomdef_status_e_type rrc_get_lac_id
(
  rrc_lai_type *lai
);

/*====================================================================
FUNCTION: rrc_update_constants_n312_and_n315_r5

DESCRIPTION:
  This function updates the global constants for N312 & N315 for R5.
  
DEPENDENCIES:
  None

RETURN VALUE:
  uint16 value corresponding to the enum passed, 0 if invalid

SIDE EFFECTS:
====================================================================*/
uint16 rrc_update_constants_n312_and_n315_r5
(
rrc_UE_ConnTimersAndConstants_r5 *conn_timers
);

/*===========================================================================

FUNCTION rrc_print_supported_asn1_rel_version

DESCRIPTION
  This  function prints the ASN1 release version supported by the UE as per 
  NV item "NV_WCDMA_RRC_VERSION_I" i.e. global variable 'rrc_nv_rel_indicator'

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrc_print_supported_asn1_rel_version(void);

/*===========================================================================

FUNCTION rrc_get_wcdma_rrc_version_nv

DESCRIPTION
  This  function gets the NV item NV_WCDMA_RRC_VERSION_I and stores it in 
  rrc global variable 'rrc_nv_rel_indicator'

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrc_get_wcdma_rrc_version_nv
(
  sys_modem_as_id_e_type wrrc_as_id
);

#ifdef FEATURE_MVS
/*===========================================================================

FUNCTION rrc_get_amr_mode_bitrate

DESCRIPTION
  This  function takes amr mode as argument & returns the AMR rate category

DEPENDENCIES
  None.

RETURN VALUE
  rrc_amr_mode_bitrate_e_type.

===========================================================================*/
rrc_amr_mode_bitrate_e_type rrc_get_amr_mode_bitrate(mvs_amr_mode_type amr_mode);

/*===========================================================================

FUNCTION rrc_get_dl_amr_mode_given_rbs

DESCRIPTION
  This  function operates on amr_rb_info global var & returns previous & new
  DL AMR modes. If changed, returns that the mode has changed(TRUE).

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

===========================================================================*/
boolean rrc_get_dl_amr_mode_given_rbs
(
  mvs_amr_mode_type *prev_mode, 
  mvs_amr_mode_type *new_mode,
  rrc_rb_info_for_amr_type *amr_rb_info
);
#endif /* FEATURE_MVS */

/*===========================================================================

FUNCTION rrc_check_if_wcdma_can_be_stopped

DESCRIPTION
  This  function is called by L1 to check if WCDMA stack can be stopped ie PNR
  (Point of no return) has not been reached. If PNR has not been reached,
  the function calls  cm function to post a special stop to RRC and returns
  TRUE to L1. 

  If PNR has been reached the function returns FALSE.

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

===========================================================================*/
boolean rrc_check_if_wcdma_can_be_stopped
(
  void
);

#ifdef FEATURE_MVS
#ifdef FEATURE_VOC_AMR_WB
#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
/*===========================================================================
FUNCTION rrc_get_ordered_codec_status

DESCRIPTION
  This  function returns that the ordred codec mode status

DEPENDENCIES
  None.

RETURN VALUE
  ordered codec status

===========================================================================*/
boolean rrc_get_ordered_codec_status
(
  void
);

/*===========================================================================

FUNCTION rrc_set_ordered_codec_status

DESCRIPTION
  This  function sets the ordered codec status
DEPENDENCIES
  None.

RETURN VALUE
 None

===========================================================================*/
void rrc_set_ordered_codec_status(boolean);
#endif
/*===========================================================================

FUNCTION rrc_get_current_codec_mode

DESCRIPTION
  This  function returns that the Current codec mode type that is set 

DEPENDENCIES
  None.

RETURN VALUE
  mvs_mode_type: rteurns the  Current codec mode type that is set 

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
mvs_mode_type rrc_get_current_codec_mode
(
  void
);

/*===========================================================================

FUNCTION rrc_set_current_codec_mode

DESCRIPTION
  This  function sets the current codec mode with value configured in MVS
DEPENDENCIES
  None.

RETURN VALUE
 None

===========================================================================*/
void rrc_set_current_codec_mode
(
  mvs_mode_type new_codec_mode
);

/*===========================================================================

FUNCTION  rrc_translate_nas_sync_indicator

DESCRIPTION This function translates a Nas-Syncronisation-indicator defined by
 ASN.1 into the internal type used by RRC. 

DEPENDENCIES

  The algorithm used assumes that the processor is Little Endian.
  
RETURN VALUE
                  
  Nas-Syncronisation-indicator converted to the internal type rrc_codec_types


===========================================================================*/
mvs_mode_type rrc_translate_nas_sync_indicator
(
  ASN1BitStr32  *nas_sync_indicator         /* Pointer to external PLMN type */
);

/*===========================================================================

FUNCTION  rrc_set_ordered_codec_mode

DESCRIPTION This function sets the codec mode ordered in the ota messages

DEPENDENCIES
  None
  
RETURN VALUE          
  None

===========================================================================*/
void rrc_set_ordered_codec_mode
(
  mvs_mode_type nas_sync_ind
);

/*===========================================================================

FUNCTION  rrc_get_ordered_codec_mode

DESCRIPTION This function sets the codec mode ordered in the ota messages

DEPENDENCIES
  None
  
RETURN VALUE          
  None

===========================================================================*/
mvs_mode_type rrc_get_ordered_codec_mode
(
  void
);

#endif /*FEATURE_VOC_AMR_WB*/
#endif
/*===========================================================================
FUNCTION   rrc_find_any_rab_already_established_rab_type

DESCRIPTION

  This function checks any RAB is already present in
  "ESTABLISHED_RABS" variable
  If so, it returns TRUE.
  It will also return the Rab Type and the CN Domain used by the 
   established Rab
  Else it returns FALSE.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  boolean. TRUE: If a RAB already exists
     else returns FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean 
rrc_find_any_rab_already_established_rab_type
(
  rrc_cn_domain_identity_e_type *rab_domain,
  rrcrb_rab_e_type *rab_type
);

/*===========================================================================

FUNCTION  rrc_debug_stats

DESCRIPTION This function increments the debug statistics

DEPENDENCIES
  None
  
RETURN VALUE
  None

===========================================================================*/
void rrc_debug_stats
(
  rrc_debug_stats_enum_type rrc_debug_cause
);

/*===========================================================================

FUNCTION  rrc_debug_clear_stats

DESCRIPTION This function clears all the debug statistics

DEPENDENCIES
  None
  
RETURN VALUE
  None

===========================================================================*/
void rrc_debug_clear_stats
(
  void
);

/*===========================================================================

FUNCTION  rrc_debug_get_cel_resel_success_cnt

DESCRIPTION   This function gives the intra frequency cell reselection success count

DEPENDENCIES
  None
  
RETURN VALUE  Intra frequency reselection success count since last power up

===========================================================================*/
uint32 rrc_debug_get_cel_resel_success_cnt
(
  void
);

/*===========================================================================

FUNCTION  rrc_debug_get_cel_resel_nbcch_set_up_fail_cnt

DESCRIPTION  This function gives the intra frequency cell reselection fail count due to 
 N-BCCH set up failure.

DEPENDENCIES
  None
  
RETURN  Intra frequency reselection failure count due to N-BCCH set up failue since last power up

===========================================================================*/
uint32 rrc_debug_get_cel_resel_nbcch_set_up_fail_cnt
(
  void
);

/*===========================================================================

FUNCTION  rrc_debug_get_cel_resel_sib_wait_timeout_cnt

DESCRIPTION  This function gives the intra frequency cell reselection fail count due to 
 SIB wait time out.

DEPENDENCIES
  None
  
RETURN   Intra frequency reselection failure count due to SIB wait time out since last power up

===========================================================================*/
uint32 rrc_debug_get_cel_resel_sib_wait_timeout_cnt
(
  void
);

/*===========================================================================

FUNCTION  rrc_debug_get_cel_resel_transition_fail_cnt

DESCRIPTION  This function gives the intra frequency cell reselection fail count due to 
 transition failure(s-criteria failure).

DEPENDENCIES
  None
  
RETURN  Intra frequency reselection failure count due to transition failures since last power up 

===========================================================================*/
uint32 rrc_debug_get_cel_resel_transition_fail_cnt
(
  void
);

/*===========================================================================

FUNCTION  rrc_debug_get_cel_resel_failed_other_causes_cnt

DESCRIPTION  This function gives the intra frequency cell reselection fail count due to 
 the causes other than n-bcch setup failure, sib wait time out and transition failure.

DEPENDENCIES
  None
  
RETURN  Intra frequency reselection failure count due to other causes since last power up

===========================================================================*/
uint32 rrc_debug_get_cel_resel_failed_other_causes_cnt
(
  void
);

/*===========================================================================

FUNCTION  rrc_debug_get_wtog_cel_resel_success_cnt

DESCRIPTION  This function gives the number of times the WTOG cell reselection is triggered 

DEPENDENCIES
  None
  
RETURN  WTOG cell reselection count since last power up

===========================================================================*/
uint32 rrc_debug_get_wtog_cel_resel_cnt
(
  void
);

/*===========================================================================

FUNCTION  rrc_debug_get_wtog_cel_resel_fail_cnt

DESCRIPTION  This function gives the WTOG cell reselection fail count.

DEPENDENCIES
  None
  
RETURN  WTOG cell reselection failure count since last power up

===========================================================================*/
uint32 rrc_debug_get_wtog_cel_resel_fail_cnt
(
  void
);

/*===========================================================================

FUNCTION  rrc_debug_get_oos_idle_mode_cnt

DESCRIPTION  This function gives how many times the OOS is triggered in idle mode.

DEPENDENCIES
  None
  
RETURN  Number of times OOS is triggered in idle mode since last power up

===========================================================================*/
uint32 rrc_debug_get_oos_idle_mode_cnt
(
  void
);

/*===========================================================================

FUNCTION  rrc_debug_get_oos_connected_mode_cnt

DESCRIPTION This function gives how many times the OOS is triggered in connected mode.

DEPENDENCIES
  None
  
RETURN  Number of times OOS is triggered in connected mode since last power up

===========================================================================*/
uint32 rrc_debug_get_oos_connected_mode_cnt
(
  void
);
/*===========================================================================

FUNCTION  rrc_debug_get_succful_rrc_trans_retry_cnt

DESCRIPTION This function gives counter value which is  incremented 
when RRC is successful in receiving RRC procedure level response
message from N/W for any RRC message sent on common channel i.e. RACH. 
.
DEPENDENCIES
  None
  
RETURN  Number of times RRC is successful in receiving RRC procedure level response
message from N/W since last power up

===========================================================================*/
uint32 rrc_debug_get_succful_rrc_trans_retry_cnt
(
  void
);
/*===========================================================================

FUNCTION  rrc_debug_get_rrc_retry_fail_due_to_l2_ack_ind_cnt

FUNCTION  rrc_debug_get_rrc_retry_fail_due_to_l2_ack_ind_cnt

DESCRIPTION This function gives counter value which is incremented 
when MAC hits max re-try while RACHing & sends CMAC_STATUS_IND
with cause as hit MAX re-try.
.
DEPENDENCIES
  None
  
RETURN  Number of times max RACH transmission hit by MAC.
===========================================================================*/
uint32 rrc_debug_get_rrc_retry_fail_due_to_l2_ack_ind_cnt
(
  void
);
/*===========================================================================

FUNCTION  rrc_debug_get_rrc_retry_fail_due_to_others_cnt

DESCRIPTION This function gives counter value which is incremented 
when RRC retransmit message Bcoz of receiving L2 ACK IND with cause
failure for previously attempted transmission of RRC message on RACH 
.
DEPENDENCIES
  None
  
RETURN   Number of times RRC transmission or retry fail due to L2 ACK IND.
===========================================================================*/
uint32 rrc_debug_get_rrc_retry_fail_due_to_others_cnt
(
  void
);

/*===========================================================================

FUNCTION  rrc_extract_rac_from_sib_info

DESCRIPTION This function extracts RAC from NAS CN Domain System Information List.

DEPENDENCIES
  None
  
RETURN VALUE          
  TRUE: RAC is found
  FALSE: RAC is not found

===========================================================================*/
boolean  rrc_extract_rac_from_sib_info
(
  rrc_rai_type *rai,
  rrc_CN_DomainSysInfoList  *cn_domain_sys_info_list_ptr
);
#ifdef FEATURE_SCUDIF_ENHANCEMENT
#error code not present
#endif /* FEATURE_SCUDIF_ENHANCEMENT */

/*===========================================================================

FUNCTION   rrc_is_nw_shared

DESCRIPTION

 This function will indicate if this N/W is shared or not.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_is_nw_shared
(
  rrc_MasterInformationBlock *mib_ptr
);
/*===========================================================================

FUNCTION rrc_return_hsdpa_category_value

DESCRIPTION
   RRC inform L1 about the HS cat to be used
  
DEPENDENCIES
  None.
  
RETURN VALUE
  HS cat to be used.
  
SIDE EFFECTS
  None.

===========================================================================*/
uint8 rrc_return_hsdpa_category_value
(
  mac_hs_e_type machs_or_ehs
);

#ifdef FEATURE_MVS
#if defined(FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING) 
/*===========================================================================

FUNCTION rrc_register_mvs_callbacks_to_l1

DESCRIPTION
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rrc_register_mvs_callback_to_l1
(
  mvssup_wcdma_blk_size_cb_set_type * amr_cb_set
);
#endif /*FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING*/
#endif
/*===========================================================================

FUNCTION rrc_get_hsdpa_plus_status

DESCRIPTION

  This function returns HSDPA PLUS Status.
  This id determined if MAC-EHS is active or not.
  
DEPENDENCIES


RETURN VALUE
  HSDPA PLUS status of type rrc_hsdpa_plus_status_enum_type
 

SIDE EFFECTS

  None

===========================================================================*/

rrc_hsdpa_plus_status_enum_type rrc_get_hsdpa_plus_status
(
  void
);

/*===========================================================================

FUNCTION   rrc_update_rab_type_in_rab_info_r8

DESCRIPTION
 This function is updates the rab type in 'rrc_est_rab_info_type' structure
 using information stored in ASN1 'rrc_RAB_InformationSetupList'.
        
DEPENDENCIES

  There is no error checking done in this function. Its assumed that ASN1
  structure 'rrc_RAB_InformationSetup' is already validated.

ARGUMENTS:

  rrc_RAB_InformationSetup *: pointer to ASN1 structure,
  rrc_est_rab_info_type *: pointer to internal RRC est_rab structure in which this 
                                       function will fill appropriate rab_type.

RETURN VALUE

rrcrb_rab_e_type: returns the rab_type which is filled in est_rab_info

SIDE EFFECTS

  None.

===========================================================================*/
rrcrb_rab_e_type rrc_update_rab_type_in_rab_info_r8
(
  rrc_RAB_InformationSetup_r8 *rab_info_setup_ptr,
  rrc_est_rab_info_type *est_rab_info_ptr 
);

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION RRC_SET_AS_ID

DESCRIPTION           This function sets the AS ID
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rrc_set_as_id
(
  sys_modem_as_id_e_type as_id
);

/*===========================================================================

FUNCTION RRC_GET_AS_ID

DESCRIPTION           This function returns as_id received from NAS if received
                             If not received, returns SYS_MODEM_AS_ID_1
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
sys_modem_as_id_e_type rrc_get_as_id
(
  void
);
/*===========================================================================

FUNCTION  rrc_check_send_change_priority

DESCRIPTION

  This function sends change priority request to WRM if necessary.
    
DEPENDENCIES
 This functon is called during addition , modification and deletion of RABs
  None.
 
RETURN VALUE

  Lock status.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_check_send_change_priority
(
  void
);

/*===========================================================================

FUNCTION  rrc_ok_for_tune_away

DESCRIPTION

  This function checks whether UE allows tune away or not
    
DEPENDENCIES
  None.
 
RETURN VALUE

 None
 
SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_ok_for_tune_away(void);

/*===========================================================================

FUNCTION  rrc_change_priority_on_tuneaway_disable

DESCRIPTION

  This change priority on disabling tuneaway

DEPENDENCIES
  None.
 
RETURN VALUE

  Lock status.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_change_priority_on_tuneaway_disable
(
  void
);

/*===========================================================================
 FUNCTION     rrc_get_max_ta_duration

 DESCRIPTION
             This function calculates the maximum possible TA duration after which 
             RRC connection can be released. This is calculated based on re-establishment timer (T314/T315)
             and out of synch timers (N313/T313)

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   uint32.

 SIDE EFFECTS
   None

===========================================================================*/
uint32 rrc_get_max_ta_duration(void);
#endif
#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
/*===========================================================================
FUNCTION rrc_register_with_msgr_for_client_and_register_umids

DESCRIPTION
  This function rregisters RRC with MSGR and registers for UMIDs it receives
  
DEPENDENCIES
  None
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrc_register_with_msgr_for_client_and_register_umids
(
  void
);
#endif


/*============================================================================
FUNCTION: rrcllc_get_64QAM_status()

DESCRIPTION:
This function checks whether 64QAM is configured or not.

DEPENDENCIES:
  None.

RETURN VALUE:
If 64QAM is configured it returns TRUE else it returns false.

SIDE EFFECTS:
=============================================================================*/

boolean rrcllc_get_64QAM_status
(
  void
);


/*===========================================================================

FUNCTION rrc_malloc_interlayer_cmd

DESCRIPTION
  This function is called by the RRC modules in order to allocate 
  dynamic memory.
  If FEATURE_WCDMA_USE_MODEM_HEAP
  is defined , we allocate from the modem heap within sub layers 
  of UMTS, else we allocate the tmc heap.

DEPENDENCIES

RETURN VALUE
  A pointer to the allocated memory if the memory was successfully allocated,
  otherwise NULL.

SIDE EFFECTS
  None.

===========================================================================*/



void* rrc_malloc_interlayer_cmd
(
  size_t size
);


/*===========================================================================

FUNCTION rrc_free_interlayer_cmd

DESCRIPTION
  This function is called by the RRC Modules in order to free dynamic
  memory.
  If FEATURE_WCDMA_USE_MODEM_HEAP
  is defined , we free from the modem heap within sub layers 
  of UMTS else we free the tmc heap.

DEPENDENCIES
  The memory manager must have been previously initialized.

RETURN VALUE
  A pointer to the allocated memory if the memory was successfully allocated,
  otherwise NULL.

SIDE EFFECTS
  None.

===========================================================================*/


void rrc_free_interlayer_cmd
(
  void *ptr
);

#ifdef FEATURE_MVS
/*============================================================================
FUNCTION: rrc_mvssup_wcdma_set_ul_channel()

DESCRIPTION:
This function sets the ul channel parameters

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrc_mvssup_wcdma_set_ul_channel
(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_wcdma_chan_class_type chan_class
);
/*============================================================================
FUNCTION: rrc_mvssup_wcdma_set_dl_channel()

DESCRIPTION:
This function sets the dl channel parameters

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
void rrc_mvssup_wcdma_set_dl_channel
(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_wcdma_chan_class_type chan_class
#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
  , mvssup_wcdma_blk_size_cb_set_type *blk_size_cb_set
#endif  /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */

);
/*============================================================================
FUNCTION: rrc_mvssup_amr_set_scr_mode()

DESCRIPTION:
This function sets the scr mode.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
void rrc_mvssup_amr_set_scr_mode
(
  mvs_scr_mode_type scr_mode
);

/*============================================================================
FUNCTION: rrc_mvssup_amr_set_amr_mode()

DESCRIPTION:
This function sets the amr mode.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
void rrc_mvssup_amr_set_amr_mode
(
  mvs_amr_mode_type amr_ul_mode
);

#if defined(FEATURE_VOC_AMR_WB)
/*============================================================================
FUNCTION: rrc_mvssup_amr_set_awb_mode()

DESCRIPTION:
This function sets the awb mode.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
void rrc_mvssup_amr_set_awb_mode
(
#ifdef FEATURE_MVS 
  mvs_amr_mode_type amr_ul_mode
#endif  
);
#endif

/*============================================================================
FUNCTION: rrc_mvssup_get_ul_amr_mode()

DESCRIPTION:
This function gets the UL frame mode for AMR mode

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
mvs_amr_mode_type rrc_mvssup_get_ul_amr_mode
(
  void 
);
/*============================================================================
FUNCTION: rrc_mvssup_get_ul_amb_mode()

DESCRIPTION:
This function gets the UL frame mode for AMR WB mode

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
mvs_amr_mode_type rrc_mvssup_get_ul_awb_mode
(
  void 
);
#endif
/*===========================================================================

FUNCTION   rrc_check_psrab_data_is_present

DESCRIPTION
  This function check for whether the WM size corresponding
  any of the rb_id is zero or not.

DEPENDENCIES
  None.

RETURN VALUE
  FALSE, if WM_size for all the rb_id is 0.
  TRUE, if WM_size for all the rb_id is non-zero. 

SIDE EFFECTS
  None.
===========================================================================*/
boolean rrc_check_psrab_data_is_present
(
  void
);


#ifdef FEATURE_WCDMA_SECURITY_SETTINGS

/*===========================================================================

FUNCTION rrc_qxdm_integrity_enable_diagcmd

DESCRIPTION
 This function handles setting the integrity flag enable/disable.

DEPENDENCIES
  None.

RETURN VALUE
  uint8

===========================================================================*/

uint8 rrc_qxdm_integrity_enable_diagcmd
(
  uint8 enable
);

/*===========================================================================

FUNCTION rrc_qxdm_ciphering_enable_diagcmd

DESCRIPTION
 This function handles setting the ciphering flag enable/disable.

DEPENDENCIES
  None.

RETURN VALUE
  uint8

===========================================================================*/

uint8 rrc_qxdm_ciphering_enable_diagcmd
(
  uint8 enable
);

/*===========================================================================

FUNCTION rrc_qxdm_fakesecurity_enable_diagcmd

DESCRIPTION
 This function handles setting the fake security flag enable/disable.

DEPENDENCIES
  None.

RETURN VALUE
  uint8

===========================================================================*/
uint8 rrc_qxdm_fakesecurity_enable_diagcmd
(
  uint8 enable
);

#endif
#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
/*===========================================================================

FUNCTION rrc_send_hho_cmd_to_cm

DESCRIPTION


  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrc_send_hho_cmd_to_cm(sys_codec_ho_cmd cmd,sys_ho_type_e_type ho_type);

/*===========================================================================

FUNCTION rrc_send_codec_info_to_cm

DESCRIPTION


  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
 void rrc_send_codec_info_to_cm(mvs_mode_type );
/*===========================================================================

FUNCTION rrc_send_codec_info_on_activation_timer_expiry

DESCRIPTION
  This function sends codec info to CM on activation timer expiry for L1

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrc_send_codec_info_on_activation_timer_expiry(void);
#endif

/*===========================================================================
FUNCTION rrc_qsh_commit_data

DESCRIPTION
  This function commits data to QSH and 


DEPENDENCIES
  None


RETURN VALUE
  None


SIDE EFFECTS
  None

===========================================================================*/

void rrc_qsh_commit_data(uint8 metric_idx,qsh_client_metric_log_reason_e reason);


typedef struct
{
  void* data_ptr;
  qsh_client_metric_context_id_t metric_context_id;
}rrc_qsh_metric_db_type;


/*===========================================================================
FUNCTION wrrc_qsh_cb

DESCRIPTION
  This function is called by QSH module to collect events/metrics/dumps


DEPENDENCIES
  None


RETURN VALUE
  None


SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
void wrrc_qsh_cb(qsh_client_cb_params_s* qsh_cb_params_ptr);

#define RRC_CRASH_DEBUG_EXTERNAL_CMD_CNT 25
#define RRC_CRASH_DEBUG_INTERNAL_CMD_CNT 100
#define RRC_CRASH_DEBUG_LTE_IRAT_CMD_CNT 10
#define RRC_CRASH_DEBUG_MM_CMD_CNT 30
#define RRC_INVALID_PROC_SUBSTATE 255

typedef struct
{
 int64   timeticks; 
 rrc_cmd_e_type cmd_id;
 uint8 proc_id;
 uint8 substate;
 uint8 status;
}rrc_debug_cmd_type;

typedef enum
{
  RRC_SDU_TYPE_NONE,
  RRC_CCCH_DL_SDU,
  RRC_CCCH_UL_SDU,
  RRC_DCCH_DL_SDU,
  RRC_DCCH_UL_SDU,
  RRC_HO_TO_UTRAN
}rrc_crash_debug_ota_pdu_type;

typedef enum
{
  L1_PHY,
  L2_UL,
  L2_DL
}rrc_lower_layer_e_type;

typedef struct
{
  int64   timeticks;
  uint8 *rrc_crash_debug_otas;
  uint16 rrc_ota_length;
  rrc_crash_debug_ota_pdu_type sdu_type; 
  uint8 proc_id;
  uint8 substate;
}rrc_crash_debug_ota;

typedef struct
{
  /*Index maintains the next buf to be filled*/
  rrc_crash_debug_ota ota_message[RRC_CRASH_DEBUG_EXTERNAL_CMD_CNT];
  /* Have a separate buffer for measurment control message and ASUs */
  rrc_crash_debug_ota meas_asu_message[RRC_CRASH_DEBUG_EXTERNAL_CMD_CNT];
  uint8 rrc_crash_debug_index;
  uint8 meas_asu_index;
}rrc_crash_debug_ota_msg_type;
typedef struct
{
  int64 timeticks;
  rrc_to_mm_sink_e_type cmd_id;
}rrc_to_mm_debug_cmd_type;

typedef struct
{
  int64 timeticks;
  rrc_rr_cmd_e_type cmd_id;
  uint8 proc_id;
  uint8 substate;
}rrc_rr_debug_cmd_type;


typedef struct
{
  rrc_debug_cmd_type mm_to_rrc_debug_cmds[RRC_CRASH_DEBUG_MM_CMD_CNT];
  rrc_to_mm_debug_cmd_type rrc_to_mm_debug_cmds[RRC_CRASH_DEBUG_MM_CMD_CNT];
  uint8 mm_to_rrc_debug_index;
  uint8 rrc_to_mm_debug_index;
}rrc_crash_debug_mm_cmd_type;

typedef struct
{
  rrc_rr_debug_cmd_type rr_to_rrc_debug_cmds[RRC_CRASH_DEBUG_EXTERNAL_CMD_CNT];
  rrc_rr_debug_cmd_type rrc_to_rr_debug_cmds[RRC_CRASH_DEBUG_EXTERNAL_CMD_CNT];
  uint8 rr_to_rrc_debug_index;
  uint8 rrc_to_rr_debug_index;
}rrc_crash_debug_rr_cmd_type;

typedef struct
{
  rrc_debug_cmd_type int_cmd[RRC_CRASH_DEBUG_INTERNAL_CMD_CNT];
  /*Index maintains the next buf to be filled*/
  uint8 rrc_crash_debug_index;
}rrc_crash_debug_int_cmd_type;
typedef struct
{
  int64 timeticks;
  l1_cmd_enum_type cmd_id;
}rrc_to_l1_cmd_type;

typedef struct
{
  rrc_debug_cmd_type l1_to_rrc_debug_cmds[RRC_CRASH_DEBUG_EXTERNAL_CMD_CNT];
  rrc_to_l1_cmd_type rrc_to_l1_debug_cmds[RRC_CRASH_DEBUG_EXTERNAL_CMD_CNT];
  uint8 l1_to_rrc_debug_index;
  uint8 rrc_to_l1_debug_index;
}rrc_crash_debug_l1_cmd_type;
typedef struct
{
  int64 timeticks;
  rrc_lower_layer_e_type dest;
  union
  {
    wcdma_l2_dl_cmd_enum_type l2_dl_cmd;
    wcdma_l2_ul_cmd_enum_type l2_ul_cmd;
  }cmds;
}rrc_crash_debug_rrc_to_l2_cmd_type;

typedef struct
{
  rrc_debug_cmd_type l2_to_rrc_debug_cmds[RRC_CRASH_DEBUG_EXTERNAL_CMD_CNT];
  rrc_crash_debug_rrc_to_l2_cmd_type rrc_to_l2_debug_cmds[RRC_CRASH_DEBUG_EXTERNAL_CMD_CNT];
  uint8 l2_to_rrc_debug_index;
  uint8 rrc_to_l2_debug_index;
}rrc_crash_debug_l2_cmd_type;
typedef struct
{
  int64 timeticks;
  uint32 cmd_id;
  uint8 proc_id;
  uint8 substate;
}lte_to_rrc_cmds_type;
typedef struct
{
  int64 timeticks;
  uint32 cmd_id;
}rrc_to_lte_cmds_type;

typedef struct
{
  lte_to_rrc_cmds_type lte_to_rrc_debug_cmds[RRC_CRASH_DEBUG_LTE_IRAT_CMD_CNT]; 
  rrc_to_lte_cmds_type rrc_to_lte_debug_cmds[RRC_CRASH_DEBUG_LTE_IRAT_CMD_CNT];
  uint8 lte_to_rrc_debug_index;
  uint8 rrc_to_lte_debug_index;
}rrc_crash_debug_lte_cmd_type;

typedef struct
{
  int64 timeticks;
  rrc_lower_layer_e_type dest;
  union
  {
    l1_cmd_enum_type l1_llc_cmd;
    wcdma_l2_ul_cmd_enum_type l2_llc_ul_cmd;
    wcdma_l2_dl_cmd_enum_type l2_llc_dl_cmd;
  }cmds;
  uint8 substate;
}rrc_llc_cmd_type;
typedef struct
{
  rrc_llc_cmd_type rrc_llc_cmds[RRC_CRASH_DEBUG_EXTERNAL_CMD_CNT];
  uint8 index;
}rrc_crash_debug_llc_cmd_type;

typedef enum
{
  INVALID_CMD,
  MM_TO_RRC_CMD,
  RR_TO_RRC_CMD,
  INT_CMD,
  L1_TO_RRC_CMD,
  L2_TO_RRC_CMD,
  LTE_TO_RRC_CMD,
}rrc_crash_debug_cmd_dest_e_type; /* Used to save the procedure and its substate to the respective structures */


typedef struct 
{
  /*Index maintains the next */
  rrc_crash_debug_mm_cmd_type rrc_crash_debug_mm_cmds;
  rrc_crash_debug_rr_cmd_type rrc_crash_debug_rr_cmds;
  rrc_crash_debug_ota_msg_type rrc_crash_debug_ota;
  rrc_crash_debug_int_cmd_type rrc_crash_debug_int_cmds;
  rrc_crash_debug_l1_cmd_type rrc_crash_debug_l1_cmds;
  rrc_crash_debug_l2_cmd_type rrc_crash_debug_l2_cmds;
  rrc_crash_debug_lte_cmd_type rrc_crash_debug_lte_cmds;
  rrc_crash_debug_llc_cmd_type rrc_crash_debug_llc_cmds;
  rrc_crash_debug_cmd_dest_e_type rrc_crash_debug_cmd_dest;
}rrc_crash_debug_info_type;


/*===========================================================================
 FUNCTION     rrc_crash_debug_set_rrc_to_mm_cmd_id

 DESCRIPTION
   This functions sets the command sent from RRC to MM into the
   crash debug info

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   None.

 SIDE EFFECTS

===========================================================================*/
void rrc_crash_debug_set_rrc_to_mm_cmd_id(uint32 cmd_id);

/*===========================================================================
FUNCTION     rrc_crash_debug_set_rrc_to_rr_cmd_id

DESCRIPTION
   This functions sets the command sent from RRC to RR into the
   crash debug info

DEPENDENCIES
   None.

PARAMETERS
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None

===========================================================================*/
void rrc_crash_debug_set_rrc_to_rr_cmd_id(uint32 cmd_id);

/*===========================================================================
FUNCTION     rrc_crash_debug_set_mm_to_rrc_cmd_id
  
DESCRIPTION
   This functions sets the command sent from MM to RRC into the
   crash debug info

DEPENDENCIES
   None.

PARAMETERS
   None.
  
RETURN VALUE
   None.
  
SIDE EFFECTS
   None
  ===========================================================================*/
void rrc_crash_debug_set_mm_to_rrc_cmd_id(rrc_cmd_type *cmd_ptr);

/*===========================================================================
 FUNCTION     rrc_crash_debug_set_rr_to_rrc_cmd_id

 DESCRIPTION
   This functions sets the command sent from RR to RRC into the
   crash debug info

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   None.

 SIDE EFFECTS
   None

===========================================================================*/
void rrc_crash_debug_set_rr_to_rrc_cmd_id(rrc_rr_cmd_e_type cmd_id);
/*===========================================================================
 FUNCTION     rrc_send_rrc_message_to_mm

 DESCRIPTION
   This functions sends the RRC message to MM command queue

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   None.

 SIDE EFFECTS
   None

===========================================================================*/

void rrc_send_rrc_message_to_mm(mm_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION          rrc_query_mrab_status


DESCRIPTION

  This function checks if CS voice and PS RAB is present 

DEPENDENCIES

  None

RETURN VALUE

Boolean

SIDE EFFECTS

  None

===========================================================================*/
boolean rrc_query_mrab_status
(
  void
);
/*===========================================================================
   FUNCTION     rrc_crash_debug_free_ota_msgs
  
   DESCRIPTION
        This funtion free the memory allocated for all the OTA messages
  
   DEPENDENCIES
     None.
  
   PARAMETERS
     None.
  
   RETURN VALUE
     None.
  
   SIDE EFFECTS
  
  ===========================================================================*/
void rrc_crash_debug_free_ota_msgs(void);
/*===========================================================================
   FUNCTION     rrc_crash_debug_save_ota
  
   DESCRIPTION
         This function saves the pased OTA into global. Before saving if already another OTA is present
       that has to be freed.
  
   DEPENDENCIES
     None.
  
   PARAMETERS
     OTA pointer, OTA lengh, OTA type
  
   RETURN VALUE
     None.
  
   SIDE EFFECTS
  
  ===========================================================================*/
void rrc_crash_debug_save_ota(uint8* sdu, uint16 sdu_length, rrc_crash_debug_ota_pdu_type pdu_type,uint8 msg_type,rrc_proc_e_type proc_id);
/*===========================================================================
   FUNCTION     rrc_crash_debug_save_l1_to_rrc_cmds
  
   DESCRIPTION
       This funtion save l1 to rrc commands in rrc_crash_debug_info
  
   DEPENDENCIES
     None.
  
   PARAMETERS
     rrc_cmd_e_type.
  
   RETURN VALUE
     None.
  
   SIDE EFFECTS
  
  ===========================================================================*/
void rrc_crash_debug_save_l1_to_rrc_cmds(rrc_cmd_type *cmd_ptr);
/*===========================================================================
   FUNCTION     rrc_crash_debug_save_l2_to_rrc_cmds
  
   DESCRIPTION
       This funtion save l2 to rrc commands in rrc_crash_debug_info
  
   DEPENDENCIES
     None.
  
   PARAMETERS
     rrc_cmd_e_type.
  
   RETURN VALUE
     None.
  
   SIDE EFFECTS
  
  ===========================================================================*/
void rrc_crash_debug_save_l2_to_rrc_cmds(rrc_cmd_type *cmd_ptr);
/*===========================================================================
   FUNCTION     rrc_crash_debug_save_internal_cmds
  
   DESCRIPTION
       This funtion saves rrc internal commands in rrc_crash_debug_info
  
   DEPENDENCIES
     None.
  
   PARAMETERS
     rrc_cmd_type.
  
   RETURN VALUE
     None.
  
   SIDE EFFECTS
  
  ===========================================================================*/
void rrc_crash_debug_save_internal_cmds(rrc_cmd_type *cmd_ptr);
/*===========================================================================
   FUNCTION     rrc_crash_debug_save_rrc_to_l2_ul_cmds
  
   DESCRIPTION
       This funtion saves rrc to l2 commands in rrc_crash_debug_info
  
   DEPENDENCIES
     None.
  
   PARAMETERS
     wcdma_l2_ul_cmd_enum_type.
  
   RETURN VALUE
     None.
  
   SIDE EFFECTS
  
  ===========================================================================*/
void rrc_crash_debug_save_rrc_to_l2_ul_cmds(wcdma_l2_ul_cmd_enum_type cmd_id);
/*===========================================================================
   FUNCTION     rrc_crash_debug_save_rrc_to_l2_ul_cmds
  
   DESCRIPTION
       This funtion saves rrc to l2 commands in rrc_crash_debug_info
  
   DEPENDENCIES
     None.
  
   PARAMETERS
     wcdma_l2_dl_cmd_enum_type.
  
   RETURN VALUE
     None.
  
   SIDE EFFECTS
  
  ===========================================================================*/
void rrc_crash_debug_save_rrc_to_l2_dl_cmds(wcdma_l2_dl_cmd_enum_type cmd_id);
/*===========================================================================
   FUNCTION     rrc_crash_debug_save_rrc_to_l1_cmds
  
   DESCRIPTION
       This funtion saves rrc to l1 commands in rrc_crash_debug_info
  
   DEPENDENCIES
     None.
  
   PARAMETERS
     l1_cmd_enum_type.
  
   RETURN VALUE
     None.
  
   SIDE EFFECTS
  
  ===========================================================================*/
void rrc_crash_debug_save_rrc_to_l1_cmds(l1_cmd_enum_type cmd_id);
/*===========================================================================
   FUNCTION     rrc_crash_debug_save_lte_to_rrc_cmds
  
   DESCRIPTION
       This funtion saves lte to rrc commands in rrc_crash_debug_info
  
   DEPENDENCIES
     None.
  
   PARAMETERS
     uint32.
  
   RETURN VALUE
     None.
  
   SIDE EFFECTS
  
  ===========================================================================*/
void rrc_crash_debug_save_lte_to_rrc_cmds(uint32 cmd_id);
/*===========================================================================
   FUNCTION     rrc_crash_debug_save_rrc_to_lte_cmds
  
   DESCRIPTION
       This funtion saves rrc to lte commands in rrc_crash_debug_info
  
   DEPENDENCIES
     None.
  
   PARAMETERS
     uint32.
  
   RETURN VALUE
     None.
  
   SIDE EFFECTS
  
  ===========================================================================*/
void rrc_crash_debug_save_rrc_to_lte_cmds(uint32 cmd_id);
/*===========================================================================
   FUNCTION     rrc_crash_debug_save_l1_llc_cmds
  
   DESCRIPTION
       This funtion saves l1 llc commands in rrc_crash_debug_info
  
   DEPENDENCIES
     None.
  
   PARAMETERS
     l1_cmd_enum_type.
  
   RETURN VALUE
     None.
  
   SIDE EFFECTS
  
  ===========================================================================*/
void rrc_crash_debug_save_l1_llc_cmds(l1_cmd_enum_type cmd_id);
/*===========================================================================
   FUNCTION     rrc_crash_debug_save_l2_llc_ul_cmds
  
   DESCRIPTION
       This funtion saves l2 llc commands in rrc_crash_debug_info
  
   DEPENDENCIES
     None.
  
   PARAMETERS
     wcdma_l2_ul_cmd_enum_type.
  
   RETURN VALUE
     None.
  
   SIDE EFFECTS
  
  ===========================================================================*/
void rrc_crash_debug_save_l2_llc_ul_cmds(wcdma_l2_ul_cmd_enum_type cmd_id);
/*===========================================================================
   FUNCTION     rrc_crash_debug_save_l2_llc_dl_cmds
  
   DESCRIPTION
       This funtion saves l2 llc commands in rrc_crash_debug_info
  
   DEPENDENCIES
     None.
  
   PARAMETERS
     wcdma_l2_dl_cmd_enum_type.
  
   RETURN VALUE
     None.
  
   SIDE EFFECTS
  
  ===========================================================================*/
void rrc_crash_debug_save_l2_llc_dl_cmds(wcdma_l2_dl_cmd_enum_type cmd_id);
/*===========================================================================
   FUNCTION     rrc_send_l1_cmd
  
   DESCRIPTION
       This is a wrapper funtion to send l1 commands and to save the command id to crash
debug info.
  
   DEPENDENCIES
     None.
  
   PARAMETERS
     l1_ext_cmd_type.
  
   RETURN VALUE
     None.
  
   SIDE EFFECTS
  
  ===========================================================================*/
void rrc_send_l1_cmd(l1_ext_cmd_type *cmd_ptr);
/*===========================================================================
   FUNCTION     rrc_send_l2_ul_cmd
  
   DESCRIPTION
       This is a wrapper funtion to send l2 ul commands and to save the command id to crash
debug info.
  
   DEPENDENCIES
     None.
  
   PARAMETERS
     wcdma_l2_ul_cmd_type.
  
   RETURN VALUE
     None.
  
   SIDE EFFECTS
  
  ===========================================================================*/
void rrc_send_l2_ul_cmd(wcdma_l2_ul_cmd_type *cmd_ptr);
/*===========================================================================
   FUNCTION     rrc_send_l2_dl_cmd
  
   DESCRIPTION
       This is a wrapper funtion to send l2 dl commands and to save the command id to crash
debug info.
  
   DEPENDENCIES
     None.
  
   PARAMETERS
     wcdma_l2_dl_cmd_type.
  
   RETURN VALUE
     None.
  
   SIDE EFFECTS
  
  ===========================================================================*/
void rrc_send_l2_dl_cmd(wcdma_l2_dl_cmd_type *cmd_ptr);
/*====================================================================

   FUNCTION     rrc_crash_debug_remove_previous_ota
  
   DESCRIPTION
       This function deletes the previously saved OTA. This is used to remove saved OTAs in crash debug
   global. 
  
   DEPENDENCIES
     None.
  
   PARAMETERS
    None
  
   RETURN VALUE
     None.
  
   SIDE EFFECTS
  ===========================================================================*/
void rrc_crash_debug_remove_previous_ota(void);

/*====================================================================
FUNCTION: rrc_check_and_init_rrc_reestab_entity()

DESCRIPTION:
 This function clears out cheks and clears reestab_entity in OC and CC

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrc_check_and_init_rrc_reestab_entity(rrc_proc_e_type proc_id);

/*===========================================================================
   FUNCTION     rrc_check_if_any_proc_waiting_for_l2_ack_for_pch_transition
  
   DESCRIPTION
     This function checks if any proceudre is waiting for L2 ACK for PCH transition
  
   DEPENDENCIES
     None.
  
   PARAMETERS

  
   RETURN VALUE
     TRUE  - If any proceudre is waiting for L2 ACK
     FALSE - not waiting for L2 ACK
  
   SIDE EFFECTS
  
  ===========================================================================*/
boolean rrc_check_if_any_proc_waiting_for_l2_ack_for_pch_transition
(
  rrc_proc_e_type *proc_id_ptr
);

#ifdef FEATURE_DUAL_SIM
/*===========================================================================
 FUNCTION     rrc_get_concurrency_nv()

 DESCRIPTION

 DEPENDENCIES
   None

 PARAMETERS
   None

 RETURN VALUE
   None

 SIDE EFFECTS

===========================================================================*/
uint16 rrc_get_concurrency_nv(void);

#endif /*FEATURE_DUAL_SIM*/





/*===========================================================================
   FUNCTION     rrc_check_if_any_proc_waiting_for_l2_ack_for_pch_transition
  
   DESCRIPTION
     This function checks if any proceudre is waiting for L2 ACK for PCH transition
  
   DEPENDENCIES
     None.
  
   PARAMETERS

  
   RETURN VALUE
     TRUE  - If any proceudre is waiting for L2 ACK
     FALSE - not waiting for L2 ACK
  
   SIDE EFFECTS
  
  ===========================================================================*/
boolean rrc_check_if_proc_waiting_for_l2_ack_for_pch_transition();
/*===========================================================================
   FUNCTION     rrc_crash_debug_log_proc_substates
  
   DESCRIPTION
       This funtion saves history of procedure substates
  
   DEPENDENCIES
     None.
  
   PARAMETERS
     rrc_proc_e_type, substate, cmd_ptr
  
   RETURN VALUE
     None.
  
   SIDE EFFECTS
  
  ===========================================================================*/
void rrc_crash_debug_log_proc_substates(rrc_proc_e_type proc_id, uint8 substate, rrc_cmd_type *cmd_ptr);

/*===========================================================================
   FUNCTION     rrc_crash_debug_save_proc_substates
  
   DESCRIPTION
       This funtion saves history of procedure substates
  
   DEPENDENCIES
     None.
  
   PARAMETERS
     rrc_proc_e_type.
  
   RETURN VALUE
     None.
  
   SIDE EFFECTS
  
  ===========================================================================*/
  void rrc_crash_debug_save_proc_substates(rrc_proc_e_type proc_id, uint8 substate, rrc_cmd_e_type cmd_id);
/*===========================================================================
   FUNCTION     rrc_get_current_substate_for_proc
  
   DESCRIPTION
       This function returns the current substate of a given procedure
debug info.
  
   DEPENDENCIES
     None.
  
   PARAMETERS
     rrc_proc_e_type.
  
   RETURN VALUE
   uint8 
  
   SIDE EFFECTS
  
  ===========================================================================*/

uint8 rrc_get_current_substate_for_proc(rrc_proc_e_type proc_id);
/*===========================================================================

FUNCTION rrcrce_get_t300_from_serv_cell_db

DESCRIPTION
  This function reads the serving cell DB and returns the T300 value from the
  UE Timers and Constants in idle mode field

DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type : Returns RRCRCE_SUCCESS If it successful retrieves
  T300 from the SIB database. Otherwise it returns RRCRCE_FAILURE_MAJOR.

SIDE EFFECTS
  None
  
===========================================================================*/

typedef enum
{
  RRCRCE_SUCCESS,               /* Successful Status */
  RRCRCE_FAILURE_MAJOR,         /* Major Failure status */
  RRCRCE_FAILURE_MINOR          /* Minor Failure Status */
}rrcrce_status_e_type;

rrcrce_status_e_type rrcrce_get_t300_from_serv_cell_db
(
  rrc_T_300 *rrcrce_t_300                    /* Pointer to T300 */ 
);

/*===========================================================================
FUNCTION wrrc_qsh_log_resel_cel_sel_params

DESCRIPTION
  This function logs RLF OOS RRC connection reject parameters

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
void wrrc_qsh_log_rrc_to_rr_params(uint32 msg_id);
/*===========================================================================
FUNCTION wrrc_qsh_log_int_cmd_params

DESCRIPTION
  This function logs internal command parameters

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
void wrrc_qsh_log_int_cmd_params(rrc_cmd_type* cmd_ptr);
/*===========================================================================
FUNCTION wrrc_qsh_log_l1_rrc_cmd_params

DESCRIPTION
  This function logs internal command parameters

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
void wrrc_qsh_log_l1_rrc_cmd_params(rrc_cmd_type* cmd_ptr);
/*===========================================================================
FUNCTION wrrc_qsh_log_oos_stats

DESCRIPTION
  This function logs oos stats

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
void wrrc_qsh_log_oos_stats(void);
/*===========================================================================
FUNCTION wrrc_qsh_log_rlf_stats

DESCRIPTION
  This function logs rlf stats

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
void wrrc_qsh_log_rlf_stats(void);
/*===========================================================================
FUNCTION wrrc_qsh_log_conn_rej_stats

DESCRIPTION
  This function logs conn reject stats

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
void wrrc_qsh_log_conn_rej_stats(void);

/*===========================================================================
FUNCTION wrrc_qsh_log_rrc_state_stats

DESCRIPTION
  This function logs conn reject stats

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
void wrrc_qsh_log_rrc_state_stats(rrc_state_e_type new_state);

/*===========================================================================
FUNCTION wrrc_qsh_log_mm_rrc_cmd_params

DESCRIPTION
  This function mm to rrc commands

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
void wrrc_qsh_log_mm_rrc_cmd_params(rrc_cmd_type* cmd_ptr);
/*===========================================================================
FUNCTION wrrc_qsh_log_rr_rrc_cmd_params

DESCRIPTION
  This function rr to rrc commands

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
void wrrc_qsh_log_rr_rrc_cmd_params(rrc_rr_cmd_type* cmd_ptr);
/*===========================================================================
FUNCTION wrrc_qsh_log_lte_rrc_cmd_params

DESCRIPTION
  This function lte to rrc commands

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
void wrrc_qsh_log_lte_rrc_cmd_params(wcdma_rrc_lte_irat_cmd_type* cmd_ptr);
/*===========================================================================
FUNCTION wrrc_qsh_update_serving_cell_metrics

DESCRIPTION
  This function serving cell metrics

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
void wrrc_qsh_update_serving_cell_metrics();
/*===========================================================================
FUNCTION rrc_update_cell_id_in_serving_cell_metrics

DESCRIPTION
  This function updates cell_ID to QSH if there is any change

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/

void rrc_update_cell_id_in_serving_cell_metrics(uint32 cell_id);
/*===========================================================================
FUNCTION wrrc_qsh_log_aset_params

DESCRIPTION
  This function logs ASET size parameters

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
void wrrc_qsh_log_aset_params(wrrc_qsh_int_action_e_type action);
#ifdef FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION wrrc_qsh_dump_cu_debug

DESCRIPTION
  This function dumps cu globals

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
__attribute__((section(".uncompressible.text")))
size_t wrrc_qsh_dump_cu_debug(uint8* dump_addr, size_t size);

/*===========================================================================
FUNCTION wrrc_qsh_dump_nv_vals

DESCRIPTION
  This function dumps nv values

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
__attribute__((section(".uncompressible.text")))
size_t wrrc_qsh_dump_nv_vals(uint8* dump_addr, size_t size );

/*===========================================================================
FUNCTION wrrc_qsh_dump_rb_proc_debug

DESCRIPTION
  This function dumps rb proc globals

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
__attribute__((section(".uncompressible.text")))

size_t wrrc_qsh_dump_rb_proc_debug(uint8* dump_addr, size_t size);
#endif

/*===========================================================================
  
  FUNCTION rrcdt_return_idt_substate
  
  DESCRIPTION
  return idt substate
  
  DEPENDENCIES
  
  RETURN VALUE
  rrcidt_substate_e_type
  
  ===========================================================================*/
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
  uint8 rrcdt_return_idt_substate(void);
  /*===========================================================================
  
  FUNCTION rrcdt_return_udt_substate
  
  DESCRIPTION
  return udt substate
  
  DEPENDENCIES
  
  RETURN VALUE
  rrcudt_substate_e_type
  
  ===========================================================================*/
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
  uint8 rrcdt_return_udt_substate(void);
  /*===========================================================================
  
  FUNCTION rrcdt_return_scrr_substate
  
  DESCRIPTION
  return scrr substate
  
  DEPENDENCIES
  
  RETURN VALUE
  rrcscrr_substate_e_type
  
  ===========================================================================*/
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
  uint8 rrcdt_return_scrr_substate(void);
  
  /*===========================================================================
  
  FUNCTION rrcdt_return_cs_domain_open
  
  DESCRIPTION
  return true if CS is open
  
  DEPENDENCIES
  
  RETURN VALUE
  uint8
  
  ===========================================================================*/
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
  uint8 rrcdt_return_cs_domain_open(void);
  /*===========================================================================
FUNCTION rrcrcr_return_substate

DESCRIPTION
returns rcr substate 
DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  rrcrcr_substate_e_type
===========================================================================*/       
#ifdef FEATURE_QSH_DUMP
    __attribute__((section(".uncompressible.text")))
#endif
uint8 rrcrcr_return_substate(void);
/*===========================================================================

FUNCTION  rrclbt_return_test_mode

DESCRIPTION
returns test mode
    
DEPENDENCIES

  None.
 
RETURN VALUE
  byte
  
SIDE EFFECTS

  None.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
byte rrclbt_return_test_mode(void)  ;

/*===========================================================================

FUNCTION  rrclbt_return_lbt_substate

DESCRIPTION
returns lbt substate
    
DEPENDENCIES

  None.
 
RETURN VALUE
  rrclbt_substates_e_type
  
SIDE EFFECTS

  None.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
uint8 rrclbt_return_lbt_substate(void);

#if defined(FEATURE_QSH_EVENT_METRIC) || defined(FEATURE_QSH_DUMP)
/*===========================================================================
FUNCTION wrrc_qsh_dump_llc_debug

DESCRIPTION
  This function dumps llc globals

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
size_t wrrc_qsh_dump_llc_debug(uint8* dump_addr, size_t size);
/*===========================================================================
FUNCTION wrrc_qsh_log_cell_resel_stats

DESCRIPTION
  This function logs cell sel/resel fail stats

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
void wrrc_qsh_log_cell_resel_stats(wrrc_mobility_rat_enum_type resel_type, wrrc_qsh_int_action_e_type action);

/*===========================================================================
FUNCTION wrrc_qsh_log_cell_resel_stats

DESCRIPTION
This function logs cell sel/resel fail stats

DEPENDENCIES
None


RETURN VALUE
None


SIDE EFFECTS
None

===========================================================================*/
void wrrc_qsh_log_iho_stats(wrrc_mobility_rat_enum_type rat_type, wrrc_qsh_int_action_e_type action);
/*===========================================================================
FUNCTION wrrc_qsh_log_timers_and_const_params

DESCRIPTION
  This function logs timer parameters

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
void wrrc_qsh_log_timers_and_const_params(wrrc_qsh_int_action_e_type action);
/*===========================================================================
FUNCTION rrc_process_qsh_cmd

DESCRIPTION
  This function is updates metric/collects dumps/notifies events


DEPENDENCIES
  None


RETURN VALUE
  None


SIDE EFFECTS
  None

===========================================================================*/


void rrc_process_qsh_cmd(qsh_client_cb_params_s* rrc_qsh_cmd);


/*===========================================================================
FUNCTION wrrc_qsh_log_resel_cel_sel_params

DESCRIPTION
  This function logs RLF OOS RRC connection reject parameters

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
void wrrc_qsh_log_rlf_oos_reject_params(wrrc_rlf_oos_con_reject_enum_type update_type, wrrc_qsh_int_action_e_type action);
/*===========================================================================
FUNCTION wrrc_qsh_get_max_size_for_dump
  
DESCRIPTION
  This function is called at rrc task init to get max dump size


DEPENDENCIES
  None


RETURN VALUE
  None


SIDE EFFECTS
  None

===========================================================================*/

uint32 wrrc_qsh_get_max_size_for_dump(void);
#endif
#ifdef FEATURE_QSH_EVENT_METRIC
/*===========================================================================
FUNCTION wrrc_qsh_log_call_end_data

DESCRIPTION
  This function logs call end data params to QSH


DEPENDENCIES
  None


RETURN VALUE
  None


SIDE EFFECTS
  None

===========================================================================*/

void wrrc_qsh_log_conn_end_info(wrrc_qsh_int_action_e_type action,
                                wrrc_call_end_data_log_type *call_end_ptr);
/*===========================================================================
FUNCTION wrrc_qsh_log_mrab_status

DESCRIPTION
  This function logs mrab status

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/

void wrrc_qsh_log_mrab_status(wrrc_qsh_int_action_e_type action);
/*===========================================================================

FUNCTION  RRCRBR_INIT_PROCEDURE

DESCRIPTION
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
uint8 rrcrbr_get_num_rbs_to_release(void);
/*===========================================================================

FUNCTION  rrcrbr_get_num_rbs_to_recfg

DESCRIPTION
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

uint8 rrcrbr_get_num_rbs_to_recfg(void);
/*===========================================================================

FUNCTION  rrcrbr_get_srns_relocation

DESCRIPTION
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

uint8 rrcrbr_get_srns_relocation(void);


/*===========================================================================

FUNCTION  

DESCRIPTION
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

uint8 rrcrbrc_get_num_rbs_reccfg(void);
/*===========================================================================

FUNCTION  

DESCRIPTION
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

uint8 rrcrbrc_get_srns_relocation(void);


/*===========================================================================

FUNCTION  

DESCRIPTION
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

uint8 rrcpcr_get_srns_relocation(void);

/*===========================================================================

FUNCTION  

DESCRIPTION
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

uint8 rrctcr_get_srns_relocation(void);
#endif
/*====================================================================
FUNCTION: rrc_rbe_get_ps_mrabs

DESCRIPTION:
  This function is called during the CPHY_SETUP processing and the return
  value signifies whether multiple PDP is configured or not (more than 2)

DEPENDENCIES:
  NONE

RETURN VALUE:
  Return value is TRUE, more than 2 PDP configured else FALSE
  established.

SIDE EFFECTS:
  None.
====================================================================*/

boolean rrc_rbe_get_ps_mrabs(void);
/*====================================================================
FUNCTION: rrcrbr_is_rb_being_released

DESCRIPTION:
  This function is called during the CPHY_SETUP processing and the return
  value signifies whether multiple PDP is configured or not (more than 2)

DEPENDENCIES:
  NONE

RETURN VALUE:
  Return value is TRUE, more than 2 PDP configured else FALSE
  established.

SIDE EFFECTS:
  None.
====================================================================*/

boolean rrcrbr_is_rb_being_released(uint8 rb_id);
/*===========================================================================

FUNCTION   rrc_is_cs_voice_rab_release_pending

DESCRIPTION
      
DEPENDENCIES

None

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

boolean rrc_is_cs_voice_rab_release_pending();
/*===========================================================================

FUNCTION   rrc_is_cs_data_rab_release_pending

DESCRIPTION
      
DEPENDENCIES

None

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

boolean rrc_is_cs_data_rab_release_pending();
/*===========================================================================

FUNCTION rrc_is_dl_ps_zero_tb_size_configured

DESCRIPTION

  This function fills in the transport block size per PS RAB

  
DEPENDENCIES


RETURN VALUE
  Returns TRUE if PS RAB id doesn't find the size.  Otherwise returns FALSE.
 

SIDE EFFECTS

  None

===========================================================================*/
rrc_ps_data_rate_e_type rrc_is_dl_ps_zero_tb_size_configured_for_all_rabs
(
void
);
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
/*===========================================================================
   FUNCTION     rrcdata_buffer_ota_during_activation_time
  
   DESCRIPTION
       This function is used to buffer the ota messages received during activation time
       It backup OTA message in ecoded format. Heap memory is allcoated for OTA message.
   DEPENDENCIES
     None.
  
   PARAMETERS
     rrc_cmd_type
  
   RETURN VALUE
     void 
  
   SIDE EFFECTS
  
  ===========================================================================*/

void rrcdata_buffer_ota_during_activation_time(
  rrc_cmd_type *cmd_ptr,                      
  rrc_proc_e_type procedure

);
/*===========================================================================

FUNCTION rrcdata_process_buffered_ota_message

DESCRIPTION

  This function process the backedup message and corresponding procedure entry function is called.
  It also takes care of creating DL SDU message as every procedures expected the same.

  
DEPENDENCIES


RETURN VALUE
None
 

SIDE EFFECTS

  None

===========================================================================*/

void rrcdata_process_buffered_ota_message(
  rrc_cmd_type *cmd_ptr                       
);

/*============================================================================
FUNCTION: rrcrbcommon_set_failure_cause()

DESCRIPTION:
  This function sets failure cause if procedure is failed because of CU gettign triggered
DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/

void rrcrbcommon_set_failure_cause(void);

/*===========================================================================

FUNCTION rrcdata_post_buffer_ota_after_activation_time_expiry

DESCRIPTION

  This function posts cmd which triggers buffered OTA processing

  
DEPENDENCIES


RETURN VALUE
None
 

SIDE EFFECTS

  None

===========================================================================*/

void rrcdata_post_buffer_ota_after_activation_time_expiry(
void
);
/*===========================================================================

FUNCTION rrcdata_reset_buffed_ota_info

DESCRIPTION

  This function backup OTA message in ecoded format. Heap memory is allcoated for saving OTA message.

  
DEPENDENCIES


RETURN VALUE
None
 

SIDE EFFECTS

  None

===========================================================================*/

void rrcdata_reset_buffed_ota_info(
  void
);
/*===========================================================================

FUNCTION rrcdata_check_buffer_ota_during_activatin_time_nv_support

DESCRIPTION

  This function returns buffer_ota_during_activation nv value

  
DEPENDENCIES


RETURN VALUE
None
 

SIDE EFFECTS

  None

===========================================================================*/

boolean rrcdata_check_buffer_ota_during_activation_time_nv_support(void);

/*===========================================================================

FUNCTION rrcdata_check_buffer_ota_during_activation_time_feature_support

DESCRIPTION

  This function returns TRUE if DUAL SIM is configured.

  
DEPENDENCIES


RETURN VALUE
None
 

SIDE EFFECTS

  None

===========================================================================*/

boolean rrcdata_check_buffer_ota_during_activation_time_feature_support(void);


/*===========================================================================

FUNCTION   rrcasu_procedure_event_handler

DESCRIPTION
      
DEPENDENCIES

None

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void rrcasu_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);
/*===========================================================================

FUNCTION rrcdata_check_if_ota_is_buffered

DESCRIPTION

  This function returns TRUE if OTA is buffered to be proceseed after 
activatin time

  
DEPENDENCIES


RETURN VALUE
None
 

SIDE EFFECTS

  None

===========================================================================*/

boolean rrcdata_check_if_ota_is_buffered(void);

#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/

/*===========================================================================
 FUNCTION     rrc_log_nv_status

 DESCRIPTION
   This functions logs the status of all connected mode NVs

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   None.

 SIDE EFFECTS
   None

===========================================================================*/
void rrc_log_nv_status();
#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_WCDMA
/*===========================================================================
FUNCTION rrc_qsh_update_metric_database

DESCRIPTION
  This function is updates metric database if W subscription is switched from slot1 to slot 2 or viceversa


DEPENDENCIES
  None


RETURN VALUE
  None


SIDE EFFECTS
  None

===========================================================================*/
void rrc_qsh_update_metric_database(sys_modem_as_id_e_type wrrc_as_id);
#endif
#endif
#endif /* RRCDATA_V_H */
