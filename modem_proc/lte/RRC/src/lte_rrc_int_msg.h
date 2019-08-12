/*!
  @file
  lte_rrc_int_msg.h

  @brief
  LTE RRC internal interfaces

  @detail

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_int_msg.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/10/12   ap      Added RRC_TMRI_CTLR_DEADLOCK and its UMID definition
04/03/12   ap      Removed SIB2 wait timer related MACROs
03/16/12   sk      Added support for L to G CCO
03/09/12   kp      Added code to indicate l2w_psho_enabled status to W-RRC
02/02/12   sk      Interface support for L to G CCO/NACC
10/07/10   np      W2L Blind redir support
07/08/11   ae      Add lte_security.h
05/31/11   np      W2L PSHO basic interfaces
03/07/11   mm      Changes for supporting CMAS
02/07/11   mm      Added UMID definitions for IRAT CGI
02/01/11   np      Interface for internal IRAT redirection
01/13/11   mm      Added new conn mode failure cause for full buffer
12/16/10   mm      Added a new config type for IRAT HO prep request
12/10/10   mm      Added definitions for LTE_RRC_SRB2_RESUMED_INDI
10/19/10   mm      Added LTE_RRC_IRAT_FROM_LTE_TO_W_MOBILITY_INDI
10/18/10   np      New destinatio scenarios for L->W/1x HO/CSFB 
10/07/10   np      Interface for e1xCSFB MobilityFromEutra
09/13/10   mm      Defined new types for dedicated RLF timers for Rel 9
09/10/10   np      Add WCDMA SIB Containers in Conn Rel for CSFB
08/25/10   np      Move lte_rrc_redir_rat_type_e to lte_rrc_ext_msg.h to be visible to NAS
08/04/10   amit    Added lte_rrc_sib_read_error_cause_e to sib_read_error 
08/04/10   np      Add bar time to lte_rrc_irat_to_lte_resel_cnfi_s
06/21/10   mm      Added LTE_RRC_IRAT_FROM_LTE_TO_G_MOBILITY_INDI
06/11/10   mm      Changed lte_rrc_llc_susp_cfg_s structure
06/09/10   mm      Added new internal UMID type for updating serving cell in CSP
06/07/10   sk      Added new internal UMID for CCO request to GSM
05/19/10   np      Depricate LTE_RRC_IRAT_ABORT_CAUSE_MANUAL_PLMN_SRCH 
                   and LTE_RRC_IRAT_FROM_LTE_RESEL_FAILURE_CAUSE_ABORTED 
                   in favor of Revoke Mechanism
05/10/10   mm      Added definitions for LTE_RRC_T321_TMRI
05/07/10   amit    Added LTE_RRC_DLM_PROCESSED_INDI 
04/25/10   sureshs Added suspend-resume cause for IRAT BPLMN
04/20/10   amit    Added LTE_RRC_PERIODIC_SIB8_TMRI 
04/16/10   np      Increase LTE_RRC_MAX_BLIST_CELLS_PER_FREQ to 16
04/14/10   sureshs Added enums for revoke utility function
03/03/10   np      Timers for IRAT to LTE Redir
02/22/10   sk      Added measurement bandwidth field
02/22/10   amit    Moved SIB bitmasks to lte_rrc_sibi.h
02/18/10   np      Removed fail_cause from IRAT to LTE Redir Failed
02/18/10   sureshs Added IRAT to LTE as camping reason
02/17/10   np      Define lte_rrc_nas_info_s for internal RRC use
02/15/10   np      Define new irat abort causes (service req, manual PLMN srch)
02/11/10   np      Ability to override cell bar time for IRAT Resel Fail
02/10/10   np      W Stub Capabilities payload support
02/08/10   np      Added G/W --> LTE redirection - RRC internal msg support, part 2
01/26/10   sk      Added G/W --> LTE redirection - RRC internal msg support
01/19/10   mm      Added dedicated reselection priority structure for WCDMA and GSM
01/18/10   dd      Added internal Capabilities msgs structs and UMIDs to/from G/W
01/14/10   np      Moved To LTE failures to external file    
01/14/10   amit    Removed SIB8 related fields from lte_rrc_sib_updated_indi_s
01/11/10   amit    Modified sib bitmasks to disable MIB reading under feature 
                   FEATURE_LTE_RRC_DISABLE_MIB_READING
01/08/10   sk      Added failure cause for "From LTE" reselection
12/22/09   sureshs Added msg hdr to lte_rrc_irat_to_lte_resel_reqi_s
12/21/09   np      Define LTE_RRC_IRAT_UTRA_RESEL_WAIT_TIME_INDI
12/18/09   sk      Upated 'to lte resel reqi' struct from IRAT MGR to CSP
12/16/09   sureshs Added enum for forbidden CSG ID in IRAT TO LTE failure cause
12/15/09   sk      Updated abort reqi for From/To LTE 
                   Updated lte_rrc_irat_to_lte_failure_cause_type_e
                   Updated lte_rrc_camped_reason_e
12/11/09   sk      Changed resel status type in 
                   lte_rrc_irat_to_lte_resel_cnfi_s
12/10/09   sk      Added G/W --> LTE reselection support
                   Renamed lte_rrc_irat_resel_reqi_s to 
                   lte_rrc_irat_from_lte_resel_reqi_s
                   Renamed lte_rrc_irat_redir_reqi_s to 
                   lte_rrc_irat_from_lte_redir_reqi_s
12/04/09   sureshs Added reselection enum for lte_rrc_dest_scenario_e
12/04/09   amit    Added sib8_si_win_bnd and sib8_age to sib_updated_ind
12/03/09   sk      Moved lte_rrc_suspend_type_e to lte_rrc_controlleri.h
12/02/09   sk      Removed dest_state from mode change request
12/01/09   sureshs Added dest_scenario field in mode change request
12/01/09   sureshs Modified comments for Mode Change Request fields
11/30/09   sureshs Added comments for Mode Change Request fields
11/24/09   sk      Updated enum lte_rrc_irat_abort_cause_type_e
11/18/09   sk      Updated enum lte_rrc_irat_abort_cause_type_e
11/18/09   fhu     replaced lte_cphy_sap.h by lte_cphy_msg.h
11/17/09   amit    Added LTE_RRC_UNSOLICITED_SIB_INDI
11/17/09   amit    Updated sib_read_error_indi (added csg_indication and 
                   intra_freq_resel_allowed)
11/12/09   sureshs Removed conn_rel_msg_ptr from Connection Released indication
11/05/09   amit    Added SIB bitmasks
11/04/09   sk      Added LTE to G/W Reselection/Redirection messages
10/28/09   np      Added UMID, failed struct for LTE -> 1X/DO reselection
10/15/09   sureshs Changed candidate_cell field in 
                    lte_rrc_cell_resel_started_indi_s to target cell info
10/14/09   sk      Added UMID, request struct for LTE -> 1X/DO reselection
10/14/09   amit    Added LTE_RRC_MIB_SIB1_BITMASK
10/07/09   sureshs Added suspend_resume_cause to Mode Change Request
10/06/09   sk      Added Abort cause enum for Inter-RAT From LTE Abort from CSP
10/02/09   sk      Removed unused UMID name
10/01/09   amit    Updated lte_rrc_get_sibs_cause_e
09/28/09   sk      Defined specific internal UMID for DO redirection 
09/23/09   sureshs Added mode to Mode Change Confirm
09/09/21   np      Define 1x RAT Manager UMIDs
09/09/09   sureshs Renamed suspend type enum values
08/25/09   sk      Added CSP-IRAT manager interface related data types for 
                   inter-RAT From LTE Redirection
08/20/09   vatsac  Added redirection related info for IRAT redirection
08/17/09   sureshs Added field to Mode Change Request to indicate suspend type
08/03/09   mm      Replaced LTE_RRC_HANDOVER_INDI with an external indication
                   in lte_ind_msg.h
07/21/09   dd      Added SMC_FAIL timer, SMC_FAIL reason for Conn Mode Failure, 
                   pdcp_hdr field in lte_rrc_mh_dlm_s
06/12/09   amit    Replaced sib_read_error_cause with sibs_rcvd bitmask
06/09/09   sureshs Added RLF & Redir as camping reason for camped indication
06/09/09   amit    Added IP check failure cause to lte_rrc_conn_mode_failure_cause_e
06/09/09   mm      RLF 2.0 changes, updated cfg_reqi interface for suspending RBs,
                   added UMID for T311 expiry
06/08/09   sureshs Added SIB read failure error to Conn Mode Failure Indi
05/27/09   mm      Changed UMID CONFIG_FAILURE_INDI to CONN_MODE_FAILURE_INDI
05/21/09   amit    Added sib read error cause INVALID_MOD_PRD_LEN
05/15/09   sureshs Added LTE_RRC_INITIATE_CON_RELEASE_INDI
05/11/09   sk      Added support for new meas pending message
04/22/09   vatsac  Moved lte_rrc_redir_info_s to lte_rrc_ext_msg.h
02/06/09   sureshs Added message pointer field to conn released ind
03/08/09   amit    Update SIB bitmasks
03/02/09   amit    Removed sib_timeout_indi
03/02/09   amit    Added sib_read_error_indi & removed 
                   sib_modification_started_indi
02/06/09   sureshs Added cooments to not change timer enum values
02/06/09   da      Added CRE indications
01/21/09   sureshs Added release reason to Connection Released indication
01/08/09   amit    Added macro for SRB0 & NULL_SFN
01/07/09   ask     Created a def to cover all measurement related SIBs
12/09/08   sureshs Added more fields in Conn Released indication
12/08/08   sureshs Added Cell Reselection canceled indication, and reselection
                   as reason for camping
12/04/08   mm      Added HANDOVER_STARTED_INDI and modified HANDOVER_COMPLETED_INDI
                   to include target cell information
11/26/08   sureshs Added camped reason to Camped indication
11/23/08   amit    Added failure cause to ul_msg_cnf and removed the resend 
                   field from ul_msg_req
11/17/08   ask     Adding support for pending queue
11/10/08   amit    Replaced initiate_cell_sel_reqi with sib_timeout_indi
11/05/08   amit    Removed sibs_needed bitmask from get_sibs_req
11/04/08   amit    Replaced cell_info with cell_id in sib_updated_ind
11/04/08   amit    Added connected to idle causes to get_sibs_cause
11/04/08   sureshs Added redir info to Conn Released Ind
11/03/08   sureshs Removed abort cause from SIB abort request
10/29/08   sureshs Removed conn_rel_msg_ptr from Conn Rel Ind
10/21/08   amit    Modified SIB bitmasks to remove lint warnings
10/21/08   da      Added macros LTE_RRC_DCCH_SRB1 and LTE_RRC_DCCH_SRB2
10/16/08   amit    Included lte_cphy_sap.h, using bitmasks defined there for 
                   SIB bitmasks
10/09/08   da      Added support for RRC llc deadlock timer
10/08/08   amit    Added pdu_num to mh_dlm_s, changed uint16 to 
                   lte_rrc_sib_bitmask_t
09/30/08   vr      Added Cell camping status enum
09/24/08   amit    Renamed SIB bitmasks, added ETWS as get_sibs cause, added a 
                   timer UMID for SIB, renamed 6hr_tmr to 3hr_tmr
09/15/08   amit    Added UMIDs LTE_RRC_CDMA2000_CSFB_PARAMETERS_REQUEST/RESPONSE
                   Removed LTE_RRC_SYSTEM_INFORMATION_DLM
09/10/08   sureshs Added cell_barring_is_reqd field to conn released ind
09/09/08   sureshs Changed name to trans_id_ptr in CFG req
09/09/08   da      Updated LTE_RRC_CFG_REQI
09/09/08   sureshs Added pointer field to conn released indication
09/08/08   am      Added enums for MH and SIB, and sib bitmasks
09/05/08   sureshs Added connection release timer
09/03/08   ask     Removed MIB and SIB1 UMIDs
09/02/08   sureshs Added MAX field to dest_state enum
08/27/08   vr      Added fields to camped_ind and resel_started_ind
08/06/08   da      Added structure definition for LTE_RRC_CFG_REQI and 
                   LTE_RRC_CFG_CNFI
07/11/08   dd      Initial Version
===========================================================================*/

#ifndef LTE_RRC_INT_MSG_H
#define LTE_RRC_INT_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <msgr.h>
#include <msgr_lte.h>
#include <lte_as.h>
#include <lte_cphy_msg.h>
#include <lte_ml1_ext_api.h>
#include <lte_rrc_ext_msg.h>
#include <lte_rrc_irat_msg.h>
#include <lte.h>
#include <lte_assert.h>
#include <lte_rrc_cfg.h>
#include <hdrcp_msg.h>
#include <wcdma_rrc_msg.h>
#include <tds_rrc_msg.h>
#include <geran_grr_msg.h>
#include <lte_security.h>
#include <dsm_pool.h>
#include <onex_cp_msg.h>
#include <lte_tlb.h>
#include <cm_msgr_msg.h>
#include <lte_rrc_osys_asn1.h>
#include <lte_l2_int_msg.h>
#ifdef FEATURE_LTE_ELS_ENABLED
#include <lte_rrc_osys_els_asn1.h>
#endif
#include <lrrc_qsh_ext.h>
#include "lte_rrc_qsh.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define LTE_RRC_DISPATCHER_EXTERNAL_MB_DEPTH          10
#define LTE_RRC_DISPATCHER_EXTERNALOTA_MB_DEPTH    10
#define LTE_RRC_DISPATCHER_INTERNAL_MB_DEPTH           20
#define LTE_RRC_DISPATCHER_PENDING_MB_DEPTH            10

#define LTE_RRC_DISPATCHER_EXTERNALOTA_MB_PRIO        1
#define LTE_RRC_DISPATCHER_EXTERNAL_MB_PRIO              2
#define LTE_RRC_DISPATCHER_PENDING_MB_PRIO                3
#define LTE_RRC_DISPATCHER_INTERNAL_MB_PRIO               4

/*! @brief LTE_RRC_MAX_CFG_INFO - Indicates the max number of req in one LTE_CFG_REQI
*/
#define LTE_RRC_MAX_CFG_INFO 2 

/*! Max number of UTRA cells for which System Info can be provided as redir assistance */
#define LTE_RRC_REDIR_UTRA_MAX_CELL_LIST (16)

/*! Max number of GERAN cells for which System Info can be provided as redir assistance */
#define LTE_RRC_REDIR_GERAN_MAX_CELL_LIST (32)

/*! Max number of GERAN SIB containers provided as redir assistance and NACC */
#define LTE_RRC_REDIR_GERAN_MAX_SIB_CONTAINER_LIST (10)

/*! Max EUTRA Cells that we can blacklist per frequency */
#define LTE_RRC_MAX_BLIST_CELLS_PER_FREQ     (16)

#define LTE_RRC_MAX_INTFREQ_NEIGHBORS     (32)

/*! @brief Macros for remaining free memory in DSM small item pool
*/
#define LTE_RRC_REM_SMALL_ITEM_POOL_SIZE_IN_BYTES (DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL) \
                                                   * DSM_POOL_ITEM_SIZE(DSM_DS_SMALL_ITEM_POOL))

/*! @brief DSM Pool to use
*/
#define LTE_RRC_DSM_POOL(size) ((size <= LTE_RRC_REM_SMALL_ITEM_POOL_SIZE_IN_BYTES) ? \
																	(DSM_DS_SMALL_ITEM_POOL) : (DSM_DS_LARGE_ITEM_POOL))

/*! @brief NULL SFN, i.e. indicates invalid or unused value
*/
#define LTE_RRC_NULL_SFN LTE_CPHY_NULL_SFN

/*! @brief Bitmask for MIB
*/
#define LTE_RRC_MIB_BITMASK   ((lte_rrc_sib_bitmask_t)LTE_CPHY_SIB_MASK_BIT_MIB)

/*! @brief Bitmask for SIB1
*/
#define LTE_RRC_SIB1_BITMASK  ((lte_rrc_sib_bitmask_t)LTE_CPHY_SIB_MASK_BIT_SIB1)

/*! @brief Bitmask for SIB2
*/
#define LTE_RRC_SIB2_BITMASK  ((lte_rrc_sib_bitmask_t)LTE_CPHY_SIB_MASK_BIT_SIB2)

/*! @brief Bitmask for SIB3
*/
#define LTE_RRC_SIB3_BITMASK  ((lte_rrc_sib_bitmask_t)LTE_CPHY_SIB_MASK_BIT_SIB3)

/*! @brief Bitmask for SIB4
*/
#define LTE_RRC_SIB4_BITMASK  ((lte_rrc_sib_bitmask_t)LTE_CPHY_SIB_MASK_BIT_SIB4)

/*! @brief Bitmask for SIB5
*/
#define LTE_RRC_SIB5_BITMASK  ((lte_rrc_sib_bitmask_t)LTE_CPHY_SIB_MASK_BIT_SIB5)

/*! @brief Bitmask for SIB6
*/
#define LTE_RRC_SIB6_BITMASK  ((lte_rrc_sib_bitmask_t)LTE_CPHY_SIB_MASK_BIT_SIB6)

/*! @brief Bitmask for SIB7
*/
#define LTE_RRC_SIB7_BITMASK  ((lte_rrc_sib_bitmask_t)LTE_CPHY_SIB_MASK_BIT_SIB7)

/*! @brief Bitmask for SIB8
*/
#define LTE_RRC_SIB8_BITMASK  ((lte_rrc_sib_bitmask_t)LTE_CPHY_SIB_MASK_BIT_SIB8)

/*! @brief Bitmask for SIB9
*/
#define LTE_RRC_SIB9_BITMASK  ((lte_rrc_sib_bitmask_t)LTE_CPHY_SIB_MASK_BIT_SIB9)

/*! @brief Bitmask for SIB10
*/
#define LTE_RRC_SIB10_BITMASK  ((lte_rrc_sib_bitmask_t)LTE_CPHY_SIB_MASK_BIT_SIB10)

/*! @brief Bitmask for SIB11
*/
#define LTE_RRC_SIB11_BITMASK  ((lte_rrc_sib_bitmask_t)LTE_CPHY_SIB_MASK_BIT_SIB11)

/*! @brief Bitmask for SIB12
*/
#define LTE_RRC_SIB12_BITMASK  ((lte_rrc_sib_bitmask_t)LTE_CPHY_SIB_MASK_BIT_SIB12)

/*! @brief Bitmask for SIB13
*/
#define LTE_RRC_SIB13_BITMASK  ((lte_rrc_sib_bitmask_t)LTE_CPHY_SIB_MASK_BIT_SIB13)

/*! @brief Bitmask for SIB14
*/
#define LTE_RRC_SIB14_BITMASK  ((lte_rrc_sib_bitmask_t)LTE_CPHY_SIB_MASK_BIT_SIB14)

/*! @brief Bitmask for SIB15
*/
#define LTE_RRC_SIB15_BITMASK  ((lte_rrc_sib_bitmask_t)LTE_CPHY_SIB_MASK_BIT_SIB15)

/*! @brief Bitmask for SIB16
*/
#define LTE_RRC_SIB16_BITMASK  ((lte_rrc_sib_bitmask_t)LTE_CPHY_SIB_MASK_BIT_SIB16)

/*! @brief Maximum length of an encoded SI
*/
#define LTE_RRC_MAX_SI_LENGTH 128


/*! @brief Maximum length of an Unsolicited SI

The physical layer imposes a limit to the maximum size a SIB can take. When 
DCI format 1C is used the maximum allowed by the physical layer is 1736 bits (
217 bytes) while for format 1A the limit is 2216 bits (277 bytes), see TS 36.
212 [22] and TS 36.213 [23]

*/
#define LTE_RRC_MAX_UNSOLICITED_SI_LENGTH 300

/*! @brief Default value of T310 timer
*/
#define LTE_RRC_DEFAULT_T310_VALUE 1000

/*! @brief macro for SRB0
*/
#define LTE_RRC_CCCH_SRB0 0

/*! @brief macro for SRB1
*/
#define LTE_RRC_DCCH_SRB1 1

/*! @brief macro for SRB2
*/
#define LTE_RRC_DCCH_SRB2 2

/*! @brief macro for max number of messages that can be pended in RRC
*/
#define LTE_RRC_PENDQ_MAX_QUEUE_LEN     10

/*! @brief Initialized value for LLC Trans id, to be set by other RRC modules
  before sending CFG_REQI to LLC
*/
#define LTE_RRC_INVALID_TRANS_ID 0xFF

/*! @brief Initialized value for index corresponding to band in multiBandInfoList
*/
#define LTE_RRC_INVALID_MFBI_INDEX 0xFF

/*<! Max number of deprioritized LTE freqs, set to same as Max num of EUTRA freqs
  that can be sent/received for IRAT */
#define LTE_RRC_MAX_NUM_DEPRI_FREQS LTE_IRAT_MAX_EUTRAN_FREQ 

/*! @brief Macro for SEC_TO_MS
*/
#define LTE_RRC_SEC_TO_MS 1000

/*! @brief Macro for MIN_TO_SEC
*/
#define LTE_RRC_MIN_TO_SEC 60

/*! @brief Value to indicate Invalid scell ID valid range SCellIndex-r10   ::=   INTEGER   (1 .  . 7 ). 
*/
#define LTE_RRC_INVALID_SCELL_ID 0x00

#define LTE_RRC_MIN_DELAY_TOLERANT_TIME 0
#define LTE_RRC_MAX_DELAY_TOLERANT_TIME 1800
#ifdef FEATURE_LTE_ELS_ENABLED
/*! @brief Value: To Indicate ELS signature (2nd and 3rd byte)
*/
#define LTE_RRC_ELS_SIGNATURE_2ND_BYTE  0x45
#define LTE_RRC_ELS_SIGNATURE_3RD_BYTE  0x92
#endif

/*! @brief Maximum number of entries in SIB1 PLMN list
*/
#define LTE_RRC_MAX_SIB1_PLMN_LIST_SIZE 6

#define LTE_RRC_MDT_AREA_CFG_MAX_NUM_CGI 32
#define LTE_RRC_MDT_AREA_CFG_MAX_NUM_TAC 8
#define LTE_RRC_MDT_TRACE_SESSION_REF_NUM_BITS 2
#define LTE_RRC_MDT_TRACE_ID_NUM_BITS 3

#define LTE_RRC_MAX_CA_BANDS_IN_BAND_COMBO 5

/*! @brief Number of SIBs (excluding MIB)
*/
#define LTE_RRC_SIB_NUM_OF_SIBS 16

/* Type for SIB bitmask */
typedef uint32 lte_rrc_sib_bitmask_t;

/* Type for Timer Struct*/
typedef struct lte_rrc_timer_struct
{
  timer_type timer_type_inst;

}lte_rrc_timer_type;

/* Type for Timer CB*/
typedef unsigned long    lte_rrc_timer_cb_data_type;


/*! @brief Enumeration of Cap IEs that need EFS support
*/
typedef enum
{
  /* Disable IEs Bitmask */
  LTE_RRC_CAP_TM8_TDD = 1, 
  LTE_RRC_CAP_INTRE_FREQ_GAPLESS_MEAS_SUPPORT_FDD = 2, 
  LTE_RRC_CAP_DISABLE_IE_OTDOA_RSTD_TDD = 3,
  LTE_RRC_CAP_DISABLE_IE_OTDOA_RSTD_FDD = 4,
  LTE_RRC_CAP_DISABLE_IE_MAX = 64, 
  /* Enable IEs Bitmask, Should start with 65 */
  LTE_RRC_CAP_NON_CA_BAND_SUPPORT = 65, 
  LTE_RRC_CAP_INTRE_FREQ_GAPLESS_MEAS_SUPPORT_TDD = 66,
  LTE_RRC_CAP_ENABLE_IE_UTRA_PROXIMITY_IND = 67, 
  LTE_RRC_CAP_ENABLE_IE_EUTRA_PROXIMITY_IND = 68, 
  LTE_RRC_CAP_ENABLE_IE_SSF9 = 69,
  LTE_RRC_CAP_ENABLE_IE_3DL_TDD_CA = 70,
  LTE_RRC_CAP_ENABLE_IE_UL_64_QAM = 80,
  LTE_RRC_CAP_ENABLE_IE_MAX = 128
} lte_rrc_cap_ies_e;


/*! @brief LTE_RRC_PROC_E - Specifies the RRC Procedures
*/
typedef enum
{ 
  LTE_RRC_PROC_CSP, /*!< 0 Cell Selection Procedure */
  LTE_RRC_PROC_SIB, /*!< 1 RRC SIB Procedure */
  LTE_RRC_PROC_LLC, /*!< 2 RRC Lower Layer Controller Procedure */
  LTE_RRC_PROC_CEP, /*!< 3 RRC Connection Establishement Procedure */
  LTE_RRC_PROC_CONFIG, /*!< 4 RRC Connection Reconfiguration Procedure */
  LTE_RRC_PROC_PG, /*!< 5 RRC Paging Procedure */
  LTE_RRC_PROC_CRP, /*!< 6 RRC Connection Release Procedure */
  LTE_RRC_PROC_MH, /*!< 7 RRC Message Handler Procedure */
  LTE_RRC_PROC_MEAS, /*!< 8 RRC Measurement Procedure */
  LTE_RRC_PROC_SEC, /*!< 9 RRC Security Procedure */
  LTE_RRC_PROC_CRE, /*!< 10 RRC Connection Re-establishment Procedure */
  LTE_RRC_PROC_CONTROLLER, /*!< 11 RRC Controller Procedure */
  LTE_RRC_PROC_IRAT, /*!< 12 IRAT Procedures */
  LTE_RRC_PROC_ESMGR, /*!< 13 EMBMS Manager */
  LTE_RRC_PROC_EMP, /*!< 14 EMBMS procedure */
  LTE_RRC_PROC_UEINFO, /*!< 15 UEInfo procedure */
  LTE_RRC_PROC_MDT, /*!< 15 MDT procedure */
  LTE_RRC_PROC_PLMN, /*!< 16 PLMN procedure */
  LTE_RRC_PROC_MAX /*!< MAX - for initialization and range check purpose */
} lte_rrc_proc_e;

/*! @brief Enumeration for cell type
*/
typedef enum
{ 
  LTE_RRC_CELL_TYPE_NONE = 0, /*!< 0 Cell Type NONE */
  LTE_RRC_CELL_TYPE_PCELL,     /*!< 0 Cell Type NONE */
  LTE_RRC_CELL_TYPE_SCELL     /*!< 0 Cell Type NONE */  
} lte_rrc_cell_type_e;


/*! @brief Enumeration for mode field in Mode Change Request
*/
typedef enum
{
  LTE_RRC_MODE_CHANGE_ACTIVATE, /*!< 0 */
  LTE_RRC_MODE_CHANGE_DEACTIVATE, /*!< 1 */
  LTE_RRC_MODE_CHANGE_SUSPEND, /*!< 2 */
  LTE_RRC_MODE_CHANGE_RESUME, /*!< 3 */
  LTE_RRC_MODE_CHANGE_MAX /*!< 4 */
} lte_rrc_mode_change_e;

/*! @brief Enumeration for destination scenario field in Mode Change Request for
    activate and suspend mode change options
*/
typedef enum
{
  LTE_RRC_ACTIVATED_DUE_TO_NAS_REQUEST, /*!< 0 */
  LTE_RRC_ACTIVATED_DUE_TO_IRAT_TO_LTE, /*!< 1 */
  LTE_RRC_SUSPENDED_DUE_TO_RLF, /*!< 2 */

  LTE_RRC_SUSPENDED_DUE_TO_CAMPED_PLMN_SRCH_FROM_LTE, /*!< 3 */
  LTE_RRC_SUSPENDED_DUE_TO_IDLE_NOT_CAMPED_PLMN_SRCH_FROM_LTE, /*!< 4 */
  LTE_RRC_SUSPENDED_DUE_TO_RTT_FROM_LTE_TO_DO, /*!< 5 */
  LTE_RRC_SUSPENDED_DUE_TO_CGI_REQ_FROM_LTE, /*!< 6 */

  LTE_RRC_SUSPENDED_DUE_TO_RESEL_FROM_LTE_TO_1X, /*!< 7 */
  LTE_RRC_SUSPENDED_DUE_TO_RESEL_FROM_LTE_TO_DO, /*!< 8 */
  LTE_RRC_SUSPENDED_DUE_TO_RESEL_FROM_LTE_TO_W, /*!< 9 */
  LTE_RRC_SUSPENDED_DUE_TO_RESEL_FROM_LTE_TO_G, /*!< 10 */
  LTE_RRC_SUSPENDED_DUE_TO_RESEL_FROM_LTE_TO_TDS, /*!< 11*/

  LTE_RRC_SUSPENDED_DUE_TO_PSHO_FROM_LTE_TO_HDR, /*! 12 */
  LTE_RRC_SUSPENDED_DUE_TO_PSHO_FROM_LTE_TO_W, /*!< 13 */
  LTE_RRC_SUSPENDED_DUE_TO_PSHO_FROM_LTE_TO_TDS, /*!< 14*/
  LTE_RRC_SUSPENDED_DUE_TO_CCO_FROM_LTE_TO_G, /*!< 15 */
  LTE_RRC_SUSPENDED_DUE_TO_HO_FROM_LTE_TO_G, /*!< 16*/
  LTE_RRC_SUSPENDED_DUE_TO_ECSFB_FROM_LTE_TO_1X, /*!< 17 */
  LTE_RRC_SUSPENDED_DUE_TO_SRVCC_FROM_LTE_TO_1X, /*!< 18 */

  LTE_RRC_SUSPENDED_DUE_TO_REDIR_FROM_LTE_TO_G, /*!< 19 */
  LTE_RRC_SUSPENDED_DUE_TO_REDIR_FROM_LTE_TO_W, /*!< 20*/
  LTE_RRC_SUSPENDED_DUE_TO_REDIR_FROM_LTE_TO_TDS, /*!< 21*/
  LTE_RRC_SUSPENDED_DUE_TO_REDIR_FROM_LTE_TO_1X, /*!< 22*/
  LTE_RRC_SUSPENDED_DUE_TO_REDIR_FROM_LTE_TO_DO, /*!< 23*/

  /* Following new causes added for X2L TRM exchange and reserve */
  LTE_RRC_ACTIVATED_DUE_TO_PSHO_FROM_W_TO_LTE, /*!< 24 */
  LTE_RRC_ACTIVATED_DUE_TO_REDIR_FROM_W_TO_LTE, /*!< 25 */
  LTE_RRC_ACTIVATED_DUE_TO_PSHO_FROM_TDS_TO_LTE, /*!< 26 */
  LTE_RRC_ACTIVATED_DUE_TO_REDIR_FROM_TDS_TO_LTE, /*!< 27 */
  LTE_RRC_ACTIVATED_DUE_TO_RESEL_FROM_TDS_TO_LTE, /*!< 28 */
  LTE_RRC_ACTIVATED_DUE_TO_REDIR_FROM_G_TO_LTE, /*!< 29 */
  LTE_RRC_ACTIVATED_DUE_TO_RESEL_FROM_G_TO_LTE, /*!< 30 */

  /*No TRM exchange, only for SSIM*/
  LTE_RRC_ACTIVATED_DUE_TO_RESEL_FROM_1X_TO_LTE, /*!< 31 */
  LTE_RRC_ACTIVATED_DUE_TO_REDIR_FROM_DO_TO_LTE, /*!< 32 */

  LTE_RRC_DEACTIVATED_DUE_TO_PSHO_FROM_W_TO_LTE_ENDED, /*!< 33 */
  LTE_RRC_DEACTIVATED_DUE_TO_REDIR_FROM_W_TO_LTE_ENDED, /*!< 34 */
  LTE_RRC_DEACTIVATED_DUE_TO_PSHO_FROM_TDS_TO_LTE_ENDED, /*!< 35 */
  LTE_RRC_DEACTIVATED_DUE_TO_REDIR_FROM_G_TO_LTE_ENDED, /*!< 36 */
  LTE_RRC_DEACTIVATED_DUE_TO_RESEL_FROM_G_TO_LTE_ENDED, /*!< 37 */
  LTE_RRC_DEACTIVATED_DUE_TO_REDIR_FROM_TDS_TO_LTE_ENDED, /*!< 38 */
  LTE_RRC_DEACTIVATED_DUE_TO_RESEL_FROM_TDS_TO_LTE_ENDED, /*!< 39 */
} lte_rrc_dest_scenario_e;

/*! @brief LTE_RRC_GET_SIBS_CAUSE_E - Indicates the cause/trigger of get_sibs_req
*/
typedef enum
{
  LTE_RRC_GET_SIBS_CAUSE_CELL_SEL,  /*!< 0 Cell Selection */
  LTE_RRC_GET_SIBS_CAUSE_CELL_RESEL,  /*!< 1 Cell Reselection */
  LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE,  /*!< 2 Paged for SIB update */
  LTE_RRC_GET_SIBS_CAUSE_3HR_TMR, /*!< 3 3hr timer expiry */
  LTE_RRC_GET_SIBS_CAUSE_HO_COMPLETE, /*!< 4 Handover Complete */
  LTE_RRC_GET_SIBS_CAUSE_ETWS, /*!< 5 ETWS */
  LTE_RRC_GET_SIBS_CAUSE_CONN_TO_IDLE_SAME_CELL, /*!< 6 Going from Connected to Idle on the same cell */
  LTE_RRC_GET_SIBS_CAUSE_NEXT_MOD_PRD_VALUE_TAG_CHECK, /*!< 7 All SIBs read, but SIB1 to be read in next mod prd to compare value tag */
  LTE_RRC_GET_SIBS_CAUSE_RESEL_FAIL, /*!< 8 Reselection failed, going back to old serving cell */
  LTE_RRC_GET_SIBS_CAUSE_SIB8_TIMING_INFO, /*!< 9 Read SIB8 to get timing info */
  LTE_RRC_GET_SIBS_CAUSE_CMAS, /*!< 10 CMAS */
  LTE_RRC_GET_SIBS_CAUSE_EMBMS, /*!< 11 EMBMS */
  LTE_RRC_GET_SIBS_CAUSE_SIB16_TIMING_INFO, /*!< 12 Read SIB16 to get UTC timing info */
  LTE_RRC_GET_SIBS_CAUSE_RF_RECOVERY, /*!< 13 After RF_recovery re-reading SIB1 to confirm value-tag */
  LTE_RRC_GET_SIBS_CAUSE_DCCH_SIB1, /*!< 14 DCCH  SIB1 signalling*/
  LTE_RRC_GET_SIBS_CAUSE_EAB, /*!< 18 EAB */
  LTE_RRC_GET_SIBS_CAUSE_MAX  /*!< MAX Invalid */
} lte_rrc_get_sibs_cause_e;

/*! @brief LTE_RRC_CFG_TYPE_E - Indicates the configuration type
*/
typedef enum
{
  LTE_RRC_CFG_DED,             /*!< 0 CONFIG due to dec OTA msg - CONFIG and CEP */
  LTE_RRC_CFG_SRV_COMMON,      /*!< 1 Used for updating phy layer with common info of serving cell  */
  LTE_RRC_CFG_CONN_REL,        /*!< 2 Configuration due to RRC Conn Release - No data to fill in union */
  LTE_RRC_CFG_ABORT_ML1,       /*!< 3 Used for sending LTE_ML1_ABORT_REQ - No date to fill in union */
  LTE_RRC_CFG_SEC,             /*!< 4 Used for performing security config of PDCP 
                                    (via PDCPUL_CFG_REQ and PDCPDL_CFG_REQ*/
  LTE_RRC_CFG_RLF,             /*!< 5 Used for aborting ML1 and suspending all RBs except SRB0 */
  LTE_RRC_CFG_DEACTIVATE_FS,   /*!< 6 Used for releasing all L1 and L2 resources */
  LTE_RRC_CFG_IRAT_HO,         /*!< 7 Used for doing the preparatory steps for IRAT HO */
  LTE_RRC_CFG_EMBMS,           /*!< 8 Used for eMBMS releated configuration */
  LTE_RRC_CFG_BAND_SCAN,       /*!< 9 Used for CSP to send band scan request to ML1*/
  LTE_RRC_CFG_SYSTEM_SCAN,     /*!< 10 Used for CSP to send system scan request to ML1*/
  LTE_RRC_CFG_ACQ,         /*!< 11 Used for CSP to send acquisition request to ML1*/
  #ifdef FEATURE_LTE_ELS_ENABLED
  LTE_RRC_CFG_ELS,             /*!< 12 CONFIG due to dec ELS OTA msg - CONFIG*/
  #endif
  LTE_RRC_CFG_MAX              /*!< MAX invalid - for initialization purpose */
} lte_rrc_cfg_type_e;

/*! @brief LTE_RRC_CFG_FAILURE_TYPE_E - Indicates the configuration Failure type
*/
typedef enum
{
  LTE_RRC_CFG_FAILURE_NONE = 0,             /*!< 0 CONFIG FAILURE - NONE*/
  LTE_RRC_CFG_FAILURE_NORMAL,              /*!< 1 CONFIG FAILURE - NORMAL*/
  LTE_RRC_CFG_FAILURE_PHY_ABORT,         /*!< 2 CONFIG FAILURE - PHY ABORT*/
  LTE_RRC_CFG_FAILURE_CONN_REL,           /*!< 3 CONFIG FAILURE - CONN REL*/
  LTE_RRC_CFG_FAILURE_CPHY_STOP          /*!< 4 CONFIG FAILURE - CPHY STOP*/
} lte_rrc_cfg_failure_type_e;

/*! 
  This is an enumeration of all RNTIs used by RRC.
  */
typedef enum
{
  LTE_RRC_C_RNTI, /*!< 0 RNTI to decode dedicated UE data in connected mode */
  LTE_RRC_SI_RNTI_TYPE, /*!< 1 RNTI to identify System Information  */
  LTE_RRC_P_RNTI_TYPE,  /*!< 2 RNTI used to identify paging information */
  LTE_RRC_RNTI_MAX /*!< 3 MAX - for initialization and range check purpose */
} lte_rrc_rnti_type_e;


/*! @brief LTE_CHAN_CFG_ACTION_E - Specifies action for a channel
*/
typedef enum
{
  LTE_RRC_CFG_ACTION_SETUP_ENABLE, /*!< 0 setup CHAN or enable RNTI */
  LTE_RRC_CFG_ACTION_TEARDOWN_DISABLE, /*!< 1 teardown CHAN or disable RNTI */
  LTE_RRC_CFG_ACTION_MAX /*!< 2 MAX - for initialization and range check purpose */
} lte_rrc_cfg_action_e;

/*! @brief LTE_RRC_UL_MSG_STATUS_E - Status of a message sent on uplink
*/
typedef enum
{
  LTE_RRC_UL_MSG_ACK,  /*!< 0 Ack */
  LTE_RRC_UL_MSG_NACK,  /*!< 1 Nack */
  LTE_RRC_UL_MSG_MAYBE,  /*!< 2 May be */
  LTE_RRC_UL_MSG_STATUS_MAX  /*!< 3 Invalid */
} lte_rrc_ul_msg_status_e;

/*! @brief LTE_RRC_GET_SIBS_STATUS_E - Status sent with get_sibs_cnf
*/
typedef enum
{
  LTE_RRC_GET_SIBS_SUCCESS,  /*!< 0 Success */
  LTE_RRC_GET_SIBS_FAILURE,  /*!< 1 Failure */
  LTE_RRC_GET_SIBS_BANDCHG_SUCCESS, /*!< 2 Sib retrieved new dl overlap frequency */
  LTE_RRC_GET_SIBS_STATUS_MAX  /*!< 3 Invalid */
} lte_rrc_get_sibs_status_e;

/*! @brief Enum to represent camping status
*/
typedef enum
{
  /*! Not camped */
  LTE_RRC_STATUS_NOT_CAMPED,          
  /*! Camped on suitable cell, all calls are allowed */
  LTE_RRC_STATUS_CAMPED_SUITABLE,     
  /*! Camped on Acceptable cell, only emergency calls are allowed */
  LTE_RRC_STATUS_CAMPED_ACCEPTABLE,   
  /*! MAX value */
  LTE_RRC_STATUS_MAX
} lte_rrc_camped_status_e;

/*! @brief Enum to represent UL msg failure (nack/maybe) cause
*/
typedef enum
{
  LTE_RRC_UL_MSG_FAIL_CAUSE_MAX_TX,  /*!< 0 Maximum number of transmissions */
  LTE_RRC_UL_MSG_FAIL_CAUSE_HO,  /*!< 1 In a HO */
  LTE_RRC_UL_MSG_FAIL_CAUSE_RLF,  /*!< 2 In a RLF */
  LTE_RRC_UL_MSG_FAIL_CAUSE_CONN_RELEASE,  /*!< 3 Connection being released */
  LTE_RRC_UL_MSG_FAIL_CAUSE_MAX  /*!< 4 Invalid */
} lte_rrc_ul_msg_fail_cause_e;

/*! @brief Enum to represent camped reason in Camped indication
*/
typedef enum
{
  LTE_RRC_CAMPED_CELL_SELECTION,               /*!< 0 */
  LTE_RRC_CAMPED_CONN_TO_IDLE,                 /*!< 1 */
  LTE_RRC_CAMPED_CONN_EST_FAILED,              /*!< 2 */
  LTE_RRC_CAMPED_CELL_RESELECTION,             /*!< 3 */
  LTE_RRC_CAMPED_CONN_TO_IDLE_INTRA_LTE_REDIR, /*!< 4 */
  LTE_RRC_CAMPED_CELL_SELECTION_RLF,           /*!< 5 */
  LTE_RRC_CAMPED_INTER_RAT_TO_LTE_RESEL,       /*!< 6*/
  LTE_RRC_CAMPED_INTER_RAT_TO_LTE_REDIR,       /*!< 7*/
} lte_rrc_camped_reason_e;

/*! @brief Enumeration for revoke priority
*/
typedef enum
{
  LTE_RRC_REVOKE_PRIORITY_NOW,      /*!< 0 */
  LTE_RRC_REVOKE_PRIORITY_NOT_REQD, /*!< 1 */
} lte_rrc_revoke_priority_e;

/*! @brief Enumeration for sib read error cause
*/
typedef enum
{
  LTE_RRC_SIB_READ_ERROR_CAUSE_SIB_TIMEOUT,      /*!< 0 */
  LTE_RRC_SIB_READ_ERROR_CAUSE_INVALID_SIBS,     /*!< 1 */
  LTE_RRC_SIB_READ_ERROR_CAUSE_MIB_CHANGED       /*!< 2 */
} lte_rrc_sib_read_error_cause_e;

/*! @brief Enumeration describing state of ML1, either its running normally, 
    or if its stopped.
*/
typedef enum
{

  /*!< ML1 successfully resumed/started */
  LTE_RRC_ML1_STATUS_NORMAL                          = 0,

  /*!< ML1 not resumed */
  LTE_RRC_ML1_STATUS_NOT_RESUMED                     = 1,

}lte_rrc_ml1_status_type_e;


/*! @brief Enumeration of different causes for CRE failure cause
*/
typedef enum
{

  /*! Reestablishment not triggered since security is inactive */
  LTE_RRC_CRE_FAILURE_CAUSE_SEC_NOT_ACTIVE = 0,

  /*! Reestablishment not triggered because of flow control shutdown from RF */
  LTE_RRC_CRE_FAILURE_CAUSE_FC_SHUTDOWN,

  /*! Reestablishment not triggered because of PDCPUL Conn release */
  LTE_RRC_CRE_FAILURE_CAUSE_PDCPUL_CONN_REL,
  
  /*! Reestablishment not triggered because of SIB read error */
  LTE_RRC_CRE_FAILURE_CAUSE_SIB_READ_ERROR,

  /*! Reestablishment failed because of T311 expiry */
  LTE_RRC_CRE_FAILURE_CAUSE_T311_EXPIRY,

  /*! Reestablishment failed because of T301 expiry */
  LTE_RRC_CRE_FAILURE_CAUSE_T301_EXPIRY,

  /*! Reestablishment failed because of ReestablishmentReject */
  LTE_RRC_CRE_FAILURE_CAUSE_REEST_REJECT,

  /*! Reestablishment failed because of ReestablishmentReject */
  LTE_RRC_CRE_FAILURE_CAUSE_REEST_INVALID_CRNTI,

  /*! Reestablishment failed because of Reestablishment validation failure */
  LTE_RRC_CRE_FAILURE_CAUSE_REEST_VALIDATION_FAILURE,

  /*! Reestablishment failed because of SRB1 resume failure */
  LTE_RRC_CRE_FAILURE_CAUSE_SRB1_RESUME_FAILURE,

  /*! Reestablishment failed because of CPHY resets */  
  LTE_RRC_CRE_FAILURE_CAUSE_CPHY_RESET,

  /*! Reestablishment failed because of RF unavailability*/  
  LTE_RRC_CRE_FAILURE_CAUSE_CPHY_RF_UNAVAILABLE,

  /*! Reestablishment not triggered because of DSDS LTA Local release*/
  LTE_RRC_CRE_FAILURE_CAUSE_MAC_DSDS_LTA_CONN_REL,

  /*! Max value */
  LTE_RRC_CRE_FAILURE_CAUSE_MAX

} lte_rrc_cre_failure_cause_e;


/*! @brief Enumeration for RB action
*/
typedef enum
{
  LTE_RRC_EMBMS_RB_ACTION_NONE,        /*!< 0 - Used just for initialization */
  LTE_RRC_EMBMS_RB_ACTION_CFG,         /*!< 1 - Used to setup or modify an MRB */
  LTE_RRC_EMBMS_RB_ACTION_RELEASE,     /*!< 2 - Used to Release an MRB */
  LTE_RRC_EMBMS_RB_ACTION_RELEASE_ALL, /*!< 3 - Used to release all MRBs */
} lte_rrc_embms_rb_action_e;

/*! @brief Enumeration for MCCH config type
*/
typedef enum
{
  LTE_RRC_EMBMS_MCCH_CFG_ALL,             /*!< 0 - Config all MCCH parameters from SIB13 */
  LTE_RRC_EMBMS_MCCH_CFG_MONITOR_PATTERN, /*!< 1 - Reconfig only monitor pattern for MCCH */
} lte_rrc_embms_mcch_cfg_type_e;

typedef uint8 lte_rrc_sib_db_key_t;

/*! @brief Structure for cell info
*/
typedef struct
{
  lte_rrc_cell_id_s cell_id;
  lte_rrc_sib_db_key_t sib_db_key;
} lte_rrc_cell_info_s;

/*! @brief Structure for dedicated RLF timers maintained by RRC
*/
typedef struct
{
  uint32 t301;  /*!< Timer T301 value */
  uint32 t311;  /*!< Timer T311 value */

} lte_rrc_rlf_tmr_s;

/*! @brief Structure for dedicated RLF timers
*/
typedef struct
{
  /*! If dedicated RLF timers and constants are enabled */
  boolean enable;

  /*! If default values are used for RLF timers and constants */
  boolean default_values_used;

  /*! Dedicated RLF timers and constant params that are passed to ML1 */
  lte_cphy_rlf_tmr_and_const_dedicated_param_s cphy_tmr_and_const;

  /*! Dedicated RLF timers that are maintained by RRC */
  lte_rrc_rlf_tmr_s rrc_tmr_params;

} lte_rrc_dedicated_rlf_timers_s;

/*! @brief Suspend/resume cause enum
*/
typedef enum
{
  LTE_RRC_RF_STATUS_NONE,
  LTE_RRC_RF_AVAILABLE,           /*!< RF Available */
  LTE_RRC_RF_UNAVAILABLE,         /*!< RF Unavailable */
}lte_rrc_rf_status_e;

/*! @brief tune away cause enum
*/
typedef enum
{
  LTE_RRC_RF_TUNEAWAY_NONE,
  LTE_RRC_RF_TUNEAWAY_DSDS,           /*!< RF Available */
  LTE_RRC_RF_TUNEAWAY_SLTE,         /*!< RF Unavailable */
}lte_rrc_rf_tune_away_e;


/*! @brief Rf Status Ind
*/
typedef struct
{
  /*! The time at which this indication is received */
  lte_ref_time_s                    ref_time; 

  /*! Status, if RF is available or not */
  lte_rrc_rf_status_e              rf_status; 

  /*! Status, if RF is available or not */
  lte_rrc_rf_tune_away_e ta_type;

}lte_rrc_rf_status_info_s;


/*! @brief Enumeration for msg type
*/
typedef enum
{
  LTE_RRC_UL_MSG_TYPE_NONE = 0,        /*!< 0 - Used by all modules */
  LTE_RRC_UL_MSG_TYPE_MDT,             /*!< 1 - Used by prebuilt MDT */
  #ifdef FEATURE_LTE_ELS_ENABLED
  LTE_RRC_UL_MSG_TYPE_ELS              /*!< 2 - Used for ELS UL message */
  #endif
} lte_rrc_ul_msg_type_e;

/*! @brief Data structure for Send UL Msg Request 
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  void *ul_msg_ptr; /*!< Message to be sent */
  lte_rb_id_t rb_id; /*!< Radio bearer id */
  boolean ack_needed; /*!< Ack needed or not*/
  uint8 *mu_id; /*!< Message unit identifier - for book keeping */
  uint8 *encode_status; /*!< Set by MH - 0 means SUCCESS */
  lte_rrc_ul_msg_type_e msg_type; /*!< UL msg type */
} lte_rrc_send_ul_msg_reqi_s;

/*! @brief
    Parameters required to configure Scell's for Carrier Aggregation
*/
typedef struct
{
  /*! Indicates number of entries in scell_id_info_list*/
  uint8 num_scell;
  /*! Scell's active for Carrier Aggregation*/
  lte_rrc_cell_id_s scell_id_info[LTE_CPHY_MAX_S_CELL];

} lte_rrc_ca_scell_list_s;

/*! @brief Data structure for Mode Change Request 
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  /* mandatory field */
  lte_rrc_mode_change_e mode; /*!< Requested mode */
  /* fields below are only looked at in specific cases and are don't care for
     the rest */
  lte_rrc_dest_scenario_e dest_scenario; /*!< Requested destination scenario if 
          mode = LTE_RRC_MODE_CHANGE_ACTIVATE or LTE_RRC_MODE_CHANGE_SUSPEND */

  sys_lte_band_mask_e_type  lte_band_pref; //Populated only if Mode change is for activate
} lte_rrc_mode_change_reqi_s;

/*! @brief Data structure for Get SIBs request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_cell_id_s cell_id; /*!< Cell identity */
  lte_rrc_proc_e req_proc; /*!< Requesting procedure */
  lte_rrc_get_sibs_cause_e cause; /*!< Cause/trigger for the request */
  boolean cnf_needed; /*!< CNF needed or not */
  lte_sfn_s sfn; /*!< (To be used by Paging only) SFN in which paging message
                         was received */
} lte_rrc_get_sibs_reqi_s;

/*! @brief Data structure for SIBs abort request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_cell_id_s cell_id; /*!< Cell identity */
} lte_rrc_sib_abort_reqi_s;

/*! @brief Data structure for dedicated configuration info
*/
typedef struct
{
  void* msg_ptr; /*!< ASN.1 decoded msg pointer to CONFIG or CEP  */
} lte_rrc_llc_ded_cfg_s;

/*! @brief Data structure for QSH CB INDI
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  qsh_client_cb_params_s cb_params; /*!< Call Back parameters*/
} lte_rrc_qsh_cb_ind_s;

#ifdef FEATURE_LTE_ELS_ENABLED
/*! @brief Data structure for ELS configuration info
*/
typedef struct
{
  void* msg_ptr; /*!< ASN.1 ELS decoded msg pointer from Config  */
} lte_rrc_llc_els_cfg_s;
#endif

/*! @brief Enumeration for cell selection cause
*/
typedef enum
{
  LTE_RRC_INITIATE_CELL_SEL_RL_FAILURE,     /*!< 0 */
  LTE_RRC_INITIATE_CELL_SEL_RLF_RF_UNAVAIL, /*!< 1 */
  LTE_RRC_INITIATE_CELL_SEL_MAX             /*!< 2 */
} lte_rrc_initiate_cell_sel_cause_e;

/*! @brief Enmueration for barring indication 
*/
typedef enum
{
  LTE_RRC_NO_BARRING, /*!< 0*/
  LTE_RRC_SHORT_DURATION_BARRING, /*! 1*/
  LTE_RRC_LONG_DURATION_BARRING /*! 2*/
}lte_rrc_barring_type_e;

/*! @brief Data structure for Initiate Cell Selection request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_rrc_initiate_cell_sel_cause_e cause; /*!< Reason for initiating cell selection */
  lte_rrc_barring_type_e bar_type;/*! Decide whether or not to bar cell for a certian duration*/
} lte_rrc_initiate_cell_sel_reqi_s;

/*! @brief Data structure for Cell Selection Abort request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} lte_rrc_csp_cell_select_abort_reqi_s;

/*! @brief Data structure for Start Internal Cell Selection request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} lte_rrc_start_internal_cell_selection_reqi_s;

/*! @brief Data structure for PLMN Search Abort request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} lte_rrc_plmn_search_abort_reqi_s;

/*! @brief Enumeration of nB values (needed to identity paging group)
*/
typedef enum
{
  LTE_RRC_NB_4T,    /*!< 0 */
  LTE_RRC_NB_2T,    /*!< 1 */
  LTE_RRC_NB_1T,    /*!< 2 */
  LTE_RRC_NB_T2,    /*!< 3 */
  LTE_RRC_NB_T4,    /*!< 4 */
  LTE_RRC_NB_T8,    /*!< 5 */
  LTE_RRC_NB_T16,   /*!< 6 */
  LTE_RRC_NB_T32,   /*!< 7 */

  LTE_RRC_NB_MAX
} lte_rrc_nb_e;

/*! @brief Enumeration for Config failure reasons
*/
typedef enum
{
  LTE_RRC_CONN_MODE_FAILURE_CAUSE_HO,           /*!< 0 - Handover failure due to RACH*/
  LTE_RRC_CONN_MODE_FAILURE_CAUSE_INVALID_CFG,      /*!< 1 - Invalid configuration */
  LTE_RRC_CONN_MODE_FAILURE_CAUSE_SIB_READ_FAILURE, /*!< 2 - SIB read failure */
  LTE_RRC_CONN_MODE_FAILURE_CAUSE_IP_CHECK_FAILURE,  /*!< 3 - IP check failure */
  LTE_RRC_CONN_MODE_FAILURE_CAUSE_SMC_FAILURE,   /*!< 4 - SMC Failure*/
  LTE_RRC_CONN_MODE_FAILURE_CAUSE_FULL_BUFFER,  /*!< 5 - MH UL MSG buffer is full */
  LTE_RRC_CONN_MODE_FAILURE_CAUSE_RESUME_FAILURE, /*!< 5 - MH UL MSG buffer is full */
  LTE_RRC_CONN_MODE_FAILURE_CAUSE_MIB_CHG, /*!< 6 - mib info changed */
  #ifdef FEATURE_LTE_ELS_ENABLED
  LTE_RRC_CONN_MODE_FAILURE_CAUSE_ELS, /*!< 7 - RLF Due to ELS Invalid Config*/
  #endif
  LTE_RRC_CONN_MODE_FAILURE_CAUSE_OTHER,        /*!< 8 - Other Failures */
  LTE_RRC_CONN_MODE_FAILURE_CAUSE_MAX          /*!< 9 Invalid */
} lte_rrc_conn_mode_failure_cause_e;

/*! @brief enumeration to identify the cause of Connection Reestablishment 
    While adding new cause please make sure that it is added after  
    LTE_RRC_CRE_CAUSE_RLF_RF_UNAVAILABLE
*/
typedef enum
{
  LTE_RRC_CRE_CAUSE_CFG_FAILURE,   /*!< 0 - Reconfig Failure from Config module */
  LTE_RRC_CRE_CAUSE_HO_FAILURE,   /*!< 1 - HO Failure from Config module */
  LTE_RRC_CRE_CAUSE_RLF,         /*!<  2 - T310 expiry from CPHY */
  LTE_RRC_CRE_CAUSE_RACH_PROBLEM, /*!< 3 - RACH problem indication from MAC */
  LTE_RRC_CRE_CAUSE_MAX_RETRX,    /*!< 4 - Maximum Retransmissions indication from RLC */
  LTE_RRC_CRE_CAUSE_IP_CHECK_FAILURE, /*!< 5 - IP Check Failure from MH module */
  LTE_RRC_CRE_CAUSE_SIB_READ_FAILURE, /*!< 6 - SIB Read Failure from CSP module */
  LTE_RRC_CRE_CAUSE_OTHER_FAILURE, /*!< 7 - Any other Failure causing RLF */
  LTE_RRC_CRE_CAUSE_MAX,          /*!< 8 - Invalid */
  LTE_RRC_CRE_CAUSE_MIB_CHG, /*!< 9 - mib info changed in Connected Mode */
  LTE_RRC_CRE_CAUSE_RLF_RF_UNAVAILABLE, /*!< 10 - DSDS RL failure due to RF unavailability */
  LTE_RRC_CRE_CAUSE_PDCPUL_RL_FAILURE, /* !< 11 - PDCPUL RL Fail Ind from L2 */
  LTE_RRC_CRE_CAUSE_L2_RL_FAILURE, /* !< 12 -RL Fail Ind from L2 */
  LTE_RRC_CRE_CAUSE_ELS /* !< 13 -RLF due to ELS*/

} lte_rrc_cre_cause_e;

/*! @brief Enumeration for HO failure reasons
*/
typedef enum
{
  LTE_RRC_HO_FAILURE_CAUSE_NONE,    /*!< 0 - Cause not applicable */
  LTE_RRC_HO_FAILURE_CAUSE_INVALID_CFG,    /*!< 1 - Invalid configuration  */
  LTE_RRC_HO_FAILURE_CAUSE_CPHY,         /*!< 2 - Handover failure due to ML1 */
  LTE_RRC_HO_FAILURE_CAUSE_RACH,    /*!< 3 - Handover failure due to RACH */
  LTE_RRC_HO_FAILURE_CAUSE_RACH_MEAS,  /*!< 4 - Handover with Meas, failure due to RACH */
  LTE_RRC_HO_FAILURE_CAUSE_IRAT,      /*!< 5 - Handover with Meas, failure due to RACH */
  LTE_RRC_HO_FAILURE_CAUSE_MAX          /*!< 6 Invalid */
} lte_rrc_ho_failure_cause_e;

/*! @brief Enumeration for RRC OOS Indi reasons
*/
typedef enum
{
  LTE_RRC_OOS_IND_CAUSE_IM3_BACKOFF,    /*!< 0 - OOS due to IM3 backoff */
  LTE_RRC_OOS_IND_CAUSE_MIB_CHG,    /*!< 1 - OOS due to MIB change in idle mode  */
  LTE_RRC_OOS_IND_CAUSE_CONN_CONTROL,    /*!< 2 - OOS due to Conn Control criteria successful */
  LTE_RRC_OOS_IND_CAUSE_MAX         /*!< 3 Invalid */
} lte_rrc_oos_ind_cause_e;

/*! @brief Data structure for dedicated configuration info
*/
typedef struct
{
  boolean sib1_update; /*!< If set to true, need to update L1 with S-criteria */
  boolean sib2_update;  /*!< If set to true, need to update L1 with common Chan config */
  boolean default_cfg;  /*!< If set to true, need to update L1 with dedicated default cfg */
} lte_rrc_llc_common_cfg_s;


/*! @brief Data structure for Security configuration info
*/
typedef struct
{
  uint8 key_index_rrc_int;  /*!< Key index for the RRC integrity Key*/
  uint8 *key_rrc_int_ptr;/*!< RRC integrity Key ptr*/
  uint8 key_index_rrc_enc; /*!< Key index for the RRC ciphering key*/
  uint8 *key_rrc_enc_ptr; /*!< RRC ciphering key*/
  uint8 key_index_up_enc;/*!< Key index for User plane ciphering key*/  
  uint8 *key_up_enc_ptr;/*!< User plane ciphering key ptr*/
  lte_cipher_algo_e cipher_algo;/*!< Ciphering Algo*/
  lte_integrity_algo_e integrity_algo;/*!< Integrity Algo*/
}lte_rrc_sec_cfg_s;

/*! @brief Data structure for suspension configuration info
*/
typedef struct
{
  boolean revert_l2_config; /*!< if L2 needs to be reveted to old config */
} lte_rrc_llc_susp_cfg_s;

/*! @brief Data structure for release configuration info
*/
typedef struct
{
  lte_rrc_ml1_status_type_e ml1_status; /*!< ml1 state */
} lte_rrc_llc_rel_cfg_s;

/*! @brief Data structure for MCCH configuration
*/
typedef struct
{
  /*!< Action for the MRB - CFG or RELEASE */
  lte_rrc_embms_rb_action_e  mrb_action; 

  /*!< MBSFN area id */
  lte_mbsfn_area_id_t        mbsfn_area_id; 

  /*! MCCH config type */
  lte_rrc_embms_mcch_cfg_type_e cfg_type; 

  /*! Monitor pattern */
  lte_cphy_mbsfn_area_monitor_pattern_e  monitor_pattern;

} lte_rrc_llc_mcch_cfg_s;

/*! @brief Data structure for MTCH configuration
*/
typedef struct
{
  /*!< Action for the MRB - CFG or RELEASE */
  lte_rrc_embms_rb_action_e  mrb_action; 

  /*!< MBSFN area id */
  lte_mbsfn_area_id_t        mbsfn_area_id; 

  /*!< pmch id - *** VALID IF RB_ID IS MTCH TYPE *** */
  lte_pmch_id_t              pmch_id; 

  /*!< LC id - *** VALID IF RB_ID IS MTCH TYPE *** */
  lte_lc_id_t                lc_id; 

  /*!< Session info - *** VALID IF RB_ID IS MTCH TYPE *** */
  lte_rrc_embms_session_info_s session_info; 

} lte_rrc_llc_mtch_cfg_s;


/*! @brief Data structure for eMBMS RB configuration
*/
typedef struct
{
  /*!< MRB id = LTE_EMBMS_MCCH_RB_ID is for MCCH, 
    all alse is MTCH (LTE_MIN_MTCH_RB_ID to LTE_MAX_MTCH_RB_ID) */
  lte_rb_id_t                mrb_id; 

  union {
    lte_rrc_llc_mcch_cfg_s  mcch_cfg; /*!< MCCH config params */
    lte_rrc_llc_mtch_cfg_s  mtch_cfg; /*!< MTCH config params*/
  }mrb_cfg_u;

} lte_rrc_llc_mrb_cfg_s;

/*! @brief Data structure for eMBMS configuration
*/
typedef struct
{

  boolean release_all_mrb;  /*!< Release all MCCH and MTCH, 
                                used for disable - Kill all scenarios*/

  uint8 num_mcch; /*!< number of MCCHs in cfg */

  lte_rrc_llc_mrb_cfg_s mcch_cfg[LTE_CPHY_MAX_MCCH_CFG];
                                  /*!< MCCH config for active MCCHs */

  uint8 num_mtch; /*!< number of MTCHs in cfg */

  lte_rrc_llc_mrb_cfg_s mtch_cfg[LTE_EMBMS_MAX_ACTIVE_SESSIONS];
                                  /*!< MTCH config for active MTCHs */

} lte_rrc_llc_embms_cfg_s;

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct
{
  lte_rrc_cfg_type_e  cfg_type;  /*!< This will indicate the configuration type */
  union {
    lte_rrc_llc_ded_cfg_s  ded_cfg; /*!< Can be used for recfg & CEP, includes msg_ptr */
    lte_rrc_llc_common_cfg_s  common_cfg; /*!< Can be used to update common info sib1/sib2/drx params*/
    lte_rrc_llc_susp_cfg_s susp_cfg; /*!< Can be used to indicate the cause for RLF */
    lte_rrc_llc_rel_cfg_s  rel_cfg; /*!< Can be used to indicate if ML1 status is resumed or not */
    lte_rrc_llc_embms_cfg_s embms_cfg; /*!< Used for eMBMS configurations */
    lte_cphy_band_scan_req_s band_scan_cfg; /*!< can be used for sending band scan request to ML1*/
    lte_cphy_system_scan_req_s system_scan_cfg; /*!< can be used for sending system scan request to ML1*/
    lte_cphy_acq_req_s acq_req_cfg; /*!< can be used for sending acquisitoon request to ML1*/
    #ifdef FEATURE_LTE_ELS_ENABLED
    lte_rrc_llc_els_cfg_s  els_cfg; /*!< Can be used for ELS recfg, includes ELS msg_ptr */
    #endif
  }cfg_u;
} lte_rrc_cfg_info_s;

#define LTE_RRC_MAX_CDMA2000_BANDS 16
#define LTE_RRC_MAX_CDMA2000_FREQUENCIES_PER_BAND 16
#define LTE_RRC_MAX_CDMA2000_CELLS_PER_FREQUENCY 16

/*! @brief Data structure for HRPD/1xRTT neighbors' info
*/
typedef struct
{
  uint8 band_count; /*!< Num neighboring bands*/
  struct
  {
    sys_band_class_e_type band_class; /*!< Band Class */
    uint8 freq_count; /*!< Num frequencies per band*/
    struct
    {
      uint16 freq; /*!< Frequency - 0..2047 */
      uint8 cell_count; /*!< Num cell per frequency */
      struct
      {
        uint16 id; /*!< Cell Id - 0..maxPNOffset(511) */
      } cell[LTE_RRC_MAX_CDMA2000_CELLS_PER_FREQUENCY];
    } frequency[LTE_RRC_MAX_CDMA2000_FREQUENCIES_PER_BAND];
  } band[LTE_RRC_MAX_CDMA2000_BANDS];
} lte_rrc_cdma2000_neighbors_s;

#define LTE_RRC_MAX_UTRA_FREQUENCIES 16
#define LTE_RRC_MAX_UTRA_TDD_FREQUENCIES 6
#define LTE_RRC_MAX_UTRA_TDD_CELL_INFO_LIST_SIZE 16

/*! @brief Data structure for FDD/TDD UTRA neighbors' info
*/
typedef struct
{
  uint8 freq_count; /*!< Num frequencies present */
  uint16 freq[LTE_RRC_MAX_UTRA_FREQUENCIES]; /*!< Frequency - 0..16383 */
} lte_rrc_utra_neighbors_s;

/*! @brief Used for rel 10 L to TDS Redir
*/
typedef lte_rrc_utra_neighbors_s lte_rrc_redir_freq_list_from_nw;

#define LTE_RRC_MAX_GERAN_FREQUENCIES 32

/*! @brief Data structure for GERAN neighbors' info
*/
typedef struct
{
  uint8 freq_count; /*!< Num frequencies present */
  uint16 freq[LTE_RRC_MAX_GERAN_FREQUENCIES]; /*!< Frequency - 0..1023 */
} lte_rrc_geran_neighbors_s;

/*! @brief Data structure for optional redirection info
*/
typedef struct
{
  union
  {
    lte_rrc_cdma2000_neighbors_s cdma2000_neighbors; /*!< HRPD/1xRTT neighbors' info*/
    lte_rrc_utra_neighbors_s utra_neighbors; /*!< FDD/TDD UTRA neighbors' info*/
    lte_rrc_geran_neighbors_s geran_neighbors; /*!< GERAN neighbors' info*/
  } rat_u;
} lte_rrc_redir_opt_info_s;

/*! @brief lte_rrc_conn_rel_ver_e - 
  Specifies Release Version of the Connection Release msg
*/
typedef enum
{ 
  LTE_RRC_CONN_REL_VER_9,  /*!< 0 Release 9 */
  LTE_RRC_CONN_REL_VER_10, /*!< 1 Release 10 */
} lte_rrc_conn_rel_ver_e;

/*! @brief Data structure for optional redirection IRAT Cell PCI and corresponding SIBs info

    NOTE: sib_container_ptr is expected to be freed by the receiver.
*/
typedef struct
{
  uint16    pci;                /*!< FDD/TDD UTRA Cell Id */
  uint16    carrier_freq_r10;   /*!< UTRA cell frequency */
  uint32    sib_container_len; /*!< Length of the SIB Container */
  uint8     *sib_container_ptr; /*!< Contents of the SIB Container */
} lte_rrc_redir_utra_cell_info_s;

/*! @brief Data structure for optional redirection UTRAN Cell SIBs info list
*/
typedef struct
{
  uint8                               cell_count; /*!< Num cells present */
  lte_rrc_redir_utra_cell_info_s      cell_info[LTE_RRC_REDIR_UTRA_MAX_CELL_LIST]; /*!< Cell Info */
} lte_rrc_redir_utra_cell_info_list_s;

/*! @brief Data structure for optional redirection GERAN SIB container info 
*/
typedef struct
{
  uint8    sib_container_len; /*!< Length of the SIB Container */
  uint8    *sib_container_ptr; /*!< Contents of the SIB Container */
}lte_rrc_redir_geran_sib_container_s;

/*! @brief Data structure for optional redirection GERAN SIB container list
*/
typedef struct
{
  uint8                                sib_container_count; /*!< Num sib containers present */
  lte_rrc_redir_geran_sib_container_s  sib_container[LTE_RRC_REDIR_GERAN_MAX_SIB_CONTAINER_LIST]; /*!< SIB Container */
} lte_rrc_redir_geran_sib_container_list_s;

typedef lte_rrc_redir_geran_sib_container_list_s lte_rrc_geran_sib_container_list_s;

/*! @brief Data structure for optional redirection GERAN Cell PCI, ARFCN, BAND and corresponding SIBs info

    NOTE: sib_container_ptr is expected to be freed by the receiver.
*/
typedef struct
{
  uint8     bcc;   /*!< BCC */
  uint8     ncc;
  uint16    arfcn; /*!< ARFCN */
  uint8     band_ind;
  lte_rrc_redir_geran_sib_container_list_s geran_cell_sib_container_list;
} lte_rrc_redir_geran_cell_info_s;

/*! @brief Data structure for optional redirection GERAN Cell SIBs info list
*/
typedef struct
{
  uint8                            cell_count; /*!< Num cells present */
  lte_rrc_redir_geran_cell_info_s  cell_info[LTE_RRC_REDIR_GERAN_MAX_CELL_LIST]; /*!< Cell Info */
} lte_rrc_redir_geran_cell_info_list_s;

/*! @brief Data structure for optional redirection IRAT Cell SIBs list
*/
typedef struct
{
  union
  {
    lte_rrc_redir_utra_cell_info_list_s     utra_cell_info_list;  /*!< FDD/TDD UTRA cell sib list*/
    lte_rrc_redir_geran_cell_info_list_s    geran_cell_info_list; /*! GERAN Cell SIB list */
  } rat_u;
} lte_rrc_redir_cell_info_list_s;

/*! @brief Data structure for redirection info
*/
typedef struct
{
  lte_rrc_redir_rat_type_e rat_type; /*!< RAT Type */
  lte_rrc_conn_rel_ver_e   conn_rel_ver;  /*!< Enum to store Connection Release version */
  sys_band_class_e_type band; /*!< Band Indicator */
  uint32 dl_arfcn; /*!< DL Frequency */

  /*!< HRPD/1XRTT/UTRA/GERAN neighbors info as selected in rat_type */
  lte_rrc_redir_opt_info_s *opt_info_ptr;

  /* Includes number of redir frequencies from Sib7 for internal redir */
  uint8 geran_internal_redir_num_freq;

  /*!< Optional R9 SIB Containers for CSFB present */
  boolean                         cell_info_list_is_present;  
  /*!< List of cells and their SIB Containers for the RAT */
  lte_rrc_redir_cell_info_list_s  cell_info_list;
  /*!< List of redirected freq for the RAT */
  lte_rrc_redir_freq_list_from_nw *redir_freq_list_ptr;

  /* Information for CSFB Call Setup optimization*/
  lte_rrc_global_cell_id_t global_cell_id;
  lte_rrc_plmn_s redir_plmn;
} lte_rrc_redir_info_s;

/*! @brief Data structure for Config request 
*/
typedef struct
{
  msgr_hdr_s msg_hdr;       /*!< Message router header */
  lte_rrc_proc_e proc;      /*!< Procedure ID of the initiating module */
  uint32 *trans_id_ptr;     /*!< transaction ID to pair the CNF_CNFI with CNF_REQI. */ 
                            /*!< IMPORTANT NOTE: This has to be set to 0xFF by
                              all RRC modules before sending the request to LLC.
                              Else, the CFG_REQI will not be handled correctly */
  boolean cnfi_required;    /*!< Field to indicate if CFG_CNFI is needed by Proc */
  uint32 num_cfg;           /*!< Number of cfg info */
  lte_rrc_cfg_info_s cfg_info[LTE_RRC_MAX_CFG_INFO]; /*!< Payload required for configurations */
} lte_rrc_cfg_reqi_s;

/*! @brief Data structure for pend Config request 
*/
typedef struct
{
  msgr_hdr_s msg_hdr;           /*!< Message router header */
  lte_rrc_cfg_reqi_s cfg_reqi;  /*!< cfg_reqi payload when pended */
  uint32 trans_id;              /*!< transaction ID to pair the CNF_CNFI with CNF_REQI. */ 
} lte_rrc_pended_cfg_reqi_s;


/*! @brief Enumeration of different causes for meas config
*/
typedef enum
{
  /*! Normal connection reconfiguration scenario */
  LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL = 0,

  /*! Start of a handover */
  LTE_RRC_MEAS_CFG_REQI_CAUSE_HANDOVER,

  /*! Revert meas config to old, e.g. in case of handover failure */
  LTE_RRC_MEAS_CFG_REQI_CAUSE_REVERT_CONFIG,

  /*! Reapply current meas config, e.g. in case of RLF */
  LTE_RRC_MEAS_CFG_REQI_CAUSE_REAPPLY_CONFIG,

  /*! Start autonomous meas id removal and Increased monitoring computation */
  LTE_RRC_MEAS_CFG_REQI_SCELL_ADD_REMOVAL_CONFIG,

  /*! Max value */
  LTE_RRC_MEAS_CFG_REQI_CAUSE_MAX,

} lte_rrc_meas_cfg_reqi_cause_e;

/*! @brief Enumeration for Inter-RAT From-LTE Abort Causes
*/
typedef enum
{

  /*!< Initialization value or abort cause not applicable */
  LTE_RRC_IRAT_ABORT_CAUSE_NONE                               = 0,

  /*!< High Priority Unknown Request, Abort IRAT, 
    resume LTE stack */
  LTE_RRC_IRAT_ABORT_CAUSE_UNKNOWN                            = 1,

  /*!< High Priority Service Request, Abort IRAT, 
    resume LTE stack */
  LTE_RRC_IRAT_ABORT_CAUSE_SVC_REQ                            = 2,

  /*!< High Priority Deactivate, Powerdown, Abort IRAT, 
    no need to resume LTE stack */
  LTE_RRC_IRAT_ABORT_CAUSE_DEACTIVATED                        = 3,

  /*!< Aborting W PLMN srch */
  LTE_RRC_IRAT_ABORT_CAUSE_W_PLMN_SRCH                        = 4,

  /*!< Aborting G PLMN srch */
  LTE_RRC_IRAT_ABORT_CAUSE_G_PLMN_SRCH                        = 5,

  /*!< Aborting due to CGI timer expiry */
  LTE_RRC_IRAT_ABORT_CAUSE_CGI_TMR_EXPIRY                     = 6,

  /*!< Aborting due to CDRX end */
  LTE_RRC_IRAT_ABORT_CAUSE_CDRX_END                           = 7,

  /*!< Aborting due to ML1_REVOKE */
  LTE_RRC_IRAT_ABORT_CAUSE_ML1_REVOKE                         = 8,

  /*!< Connection Abort */
  LTE_RRC_IRAT_ABORT_CAUSE_CONN_ABORT                         = 9,

    /*!< Aborting TDS PLMN srch */
  LTE_RRC_IRAT_ABORT_CAUSE_TDS_PLMN_SRCH                      = 10,
  
    /*!< Aborting G PLMN srch */
  LTE_RRC_IRAT_ABORT_CAUSE_INACTIVE_G_PLMN_SRCH               = 11,

  /*!< Abort due to L2G SRVCC SGLTE success */
  LTE_RRC_IRAT_ABORT_CAUSE_SGLTE_SRVCC_TO_G_SUCCESS           = 12,
  
}lte_rrc_irat_abort_cause_type_e;

/*! @brief Enumeration for Inter-RAT Internal Redir type
*/
typedef enum
{

  /*!< Inter-RAT SIB neighbor info available */
  LTE_RRC_IRAT_FROM_LTE_TYPE_SIB                              = 0,

  /*!< Inter-RAT SIB neighbor info not available, target RAT should try full scan */
  LTE_RRC_IRAT_FROM_LTE_TYPE_ACQDB                            = 1,

}lte_rrc_irat_from_lte_internal_redir_type_e;

/*! @brief Enumeration for eMBMS OOS cause
*/
typedef enum
{
  /*!< Unicast OOS, all TMGIs will be affected */
  LTE_RRC_EMP_WARN_REASON_OOS_UNICAST                   = 0,

  /*!< Multicast OOS, TMGIs in one MBSFN area alone will be affected */
  LTE_RRC_EMP_WARN_REASON_OOS_MULTICAST                 = 1,

  /*!< OOS recovery has happened */
  LTE_RRC_EMP_WARN_REASON_OOS_RECOVERED                 = 2,

  /*!< Warning reason Not applicable */
  LTE_RRC_EMP_WARN_REASON_NONE                          = 3,

}lte_rrc_emp_warn_reason_e;

/*! @brief Enumeration for LTE deprioritization type
*/
typedef enum
{
  /*!< Deprioritize serving freq */
  LTE_RRC_DEPRI_SERVING_FREQ = 0,

  /*!< Deprioritize all LTE freqs */
  LTE_RRC_DEPRI_ALL          = 1,

  /*!< Deprioritize list of freqs */
  LTE_RRC_DEPRI_FREQ_LIST   = 2,

  /*!< Deprioritize None */
  LTE_RRC_DEPRI_NONE         = 3, 

}lte_depri_type_e;

/*! @brief Enumeration for LTE deprioritization type
*/
typedef enum
{
  /*!< 5 MIN */
  LTE_RRC_DEPRI_TMR_5_MIN  = 5,

  /*!< 10 MIN */
  LTE_RRC_DEPRI_TMR_10_MIN = 10,

  /*!< 15 MIN */
  LTE_RRC_DEPRI_TMR_15_MIN = 15,

  /*!< 30 MIN */
  LTE_RRC_DEPRI_TMR_30_MIN = 30,

  /*!< Deprioritize until deprioritization clear req is received */
  LTE_RRC_DEPRI_TMR_NONE,

}lte_depri_timer_e;


/*! @brief Enumeration for LTE deprioritization cause
*/
typedef enum
{
  /*!< Deprioritize by network*/
  LTE_RRC_DEPRI_CAUSE_NETWORK =        0,

  /*!< L-RRC internal deprioritization */
  LTE_RRC_DEPRI_CAUSE_LIMITED_MODE_IMS          = 1,

  /*!< Deprioritize None */
  LTE_RRC_DEPRI_CAUSE_NONE              = 2, 

}lte_rrc_depri_cause_e;

/*! @brief Enumeration for X2L redirection mode
*/
typedef enum
{
  /*!< Blind redirection intiated by UE*/
  LTE_RRC_IRAT_FROM_IRAT_MODE_REDIR_BLIND       = 0,

  /*!Full Redirection initiated by N/W*/
  LTE_RRC_IRAT_FROM_IRAT_MODE_REDIR_NW          = 1,

  /*!List Redirection initiated by N/W*/
  LTE_RRC_IRAT_FROM_IRAT_MODE_REDIR_LIST        = 2, 

}lte_rrc_irat_from_irat_redir_mode_e;

/*! @brief Enumeration for ML1 Stop status
*/
typedef enum
{
  LTE_CPHY_STOPPED          = 0,
  LTE_CPHY_ACTIVE           = 1,
  LTE_CPHY_PENDING_STOP_CNF = 2,
}lte_rrc_ctlr_cphy_stop_status;

/*! @brief Data structure for Measurement Config request
*/
typedef struct
{
  msgr_hdr_s msg_hdr;                   /*!< Message router header */
  void *dl_dcch_ptr;                    /*!< Ptr to the reconfig dlm message */
  lte_rrc_cell_id_s cell_id;            /*!< Cell info */
  lte_rrc_meas_cfg_reqi_cause_e cause;  /*!< Meas config cause */

} lte_rrc_meas_cfg_reqi_s;

/*! @brief Data structure for Inter-RAT From LTE Redirection request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; 
  lte_rrc_redir_info_s redir_info;
}lte_rrc_irat_from_lte_redir_reqi_s;

/*! @brief Data structure for Inter-RAT From LTE (Internal) Redirection 
 *         request.
 *
 *         This is used if Redir to target RAT fails and we want to try
 *         another RAT before declaring OOS to upper layers.
 *         
 *         This is sent directly between the IRAT managers.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; 
  lte_rrc_irat_from_lte_internal_redir_type_e  redir_type;
  uint8 num_geran_internal_redir_freq;
}lte_rrc_irat_from_lte_internal_redir_reqi_s;

/*! @brief Data structure for Inter-RAT From LTE Abort request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; 
  lte_rrc_irat_abort_cause_type_e abort_cause;  /* From-LTE Inter-RAT Abort cause */
  lte_rrc_proc_e proc;      /*!< Procedure ID of the initiating module */
}lte_rrc_irat_from_lte_abort_reqi_s;

/*! @brief Data structure for Inter-RAT To LTE Resel Abort request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; 
}lte_rrc_irat_to_lte_resel_abort_reqi_s;

/*! @brief Data structure for Inter-RAT To LTE Redir Abort request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; 
}lte_rrc_irat_to_lte_redir_abort_reqi_s;

/*! @brief Data structure for Inter-RAT To LTE PSHO Abort request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; 
}lte_rrc_irat_to_lte_psho_abort_reqi_s;

/*! @brief Data structure for Inter-RAT From LTE reselection request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; 
  lte_cphy_resel_target_cell_info_s resel_info;
  boolean csg_resel;
}lte_rrc_irat_from_lte_resel_reqi_s;

/*! @brief Data Structure for 
    Internal Capabilities Request Message from Capabilities to DO Inter-RAT  
 */

typedef struct
{
  msgr_hdr_struct_type           msg_hdr;

  sys_band_mask_type             cdma_band_preference;
  lte_irat_capabilities_s        rat_capabilities;
} lte_rrc_irat_from_lte_to_do_capabilities_reqi_s;

/*! @brief Data Structure for 
    Internal Capabilities Request Message from Capabilities to W Inter-RAT mgr 
 */

typedef struct
{
  msgr_hdr_struct_type           msg_hdr;

  sys_band_mask_type             wcdma_band_preference;
  sys_lte_band_mask_e_type       lte_band_preference;
  sys_band_mask_type             gsm_band_preference;
  lte_irat_capabilities_s        rat_capabilities;

  boolean                        compressed_mode_required;

  /*L2W PSHO feature enabled flag, this will help W-RRC know if it needs to carry START-PS or not in capability*/
  boolean                        l2w_psho_enabled; 

} lte_rrc_irat_from_lte_to_w_capabilities_reqi_s;

/*! @brief Data Structure for 
    Internal Capabilities Request Message from Capabilities to TDS Inter-RAT mgr 
 */

typedef struct
{
  msgr_hdr_struct_type           msg_hdr;

  sys_band_mask_type             tdscdma_band_preference;
  sys_lte_band_mask_e_type       lte_band_preference;
  sys_band_mask_type             gsm_band_preference;
  lte_irat_capabilities_s        rat_capabilities;

  boolean                        compressed_mode_required;
  /*L2T PSHO feature enabled flag, this will help TDSCDMA know if it needs to carry START-PS or not in capability*/
  boolean                        l2t_psho_enabled; 

} lte_rrc_irat_from_lte_to_tds_capabilities_reqi_s;

/*! @brief Data Structure for 
    Internal Capabilities Request Message from GERAN CS 
    Capabilities to GERAN  Inter-RAT mgr  
 */

typedef struct
{
  msgr_hdr_struct_type           msg_hdr;

  sys_band_mask_type             geran_band_preference;
  lte_irat_capabilities_s        rat_capabilities;
} lte_rrc_irat_from_lte_to_g_cs_capabilities_reqi_s;


/*! @brief Data Structure for 
    Internal Capabilities Request Message from GERAN PS 
    Capabilities to GERAN  Inter-RAT mgr  
 */

typedef struct
{
  msgr_hdr_struct_type           msg_hdr;

  sys_band_mask_type             geran_band_preference;
  lte_irat_capabilities_s        rat_capabilities;
} lte_rrc_irat_from_lte_to_g_ps_capabilities_reqi_s;


/*! @brief Data Structure for 
    Internal CCO Request Message from Config to "To-G" RAT MGR
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr;

  /* CCO payload */
  void                           *msg_ptr;

}lte_rrc_irat_from_lte_to_g_cco_reqi_s;


/*! @brief Data structure for Inter-RAT To LTE reselection request 
    from inter-RAT MGR to CSP
*/
typedef struct
{
  msgr_hdr_s msg_hdr;

  sys_rat_pri_list_info_s_type rat_pri_list; /*!< RAT priority list */

  /*! TRUE if req_plmn field is present, FALSE otherwise */
  boolean req_plmn_info_is_valid; 

  lte_rrc_plmn_s req_plmn; /*!< Requested PLMN */

  /*! Enhanced requested PLMN for eHRPD to LTE  reselection */
  lte_rrc_plmn_list_s enh_req_plmn_list_info;
  
  /*! network select mode */
  sys_network_selection_mode_e_type network_select_mode;

  /*! Frequency of the cell to which reselection needs to occur */
  lte_earfcn_t dl_cell_frequency;

  /*! cell id to which reselection needs to occur */
  lte_phy_cell_id_t phy_cell_id;

  /*! NAS Requested CSG Id */
  sys_csg_id_type csg_id;

  /*!< Serving Cell Info */
  source_cell_info_type serving_cell_info;

  /* CSG reselection */
  boolean csg_resel;
}lte_rrc_irat_to_lte_resel_reqi_s;


/*! @brief Data structure for Inter-RAT To LTE PSHO request 
    from inter-RAT MGR to Config
*/
typedef struct
{
  msgr_hdr_s msg_hdr;
  void *dl_msg_ptr; /*!< Pointer to decoded DL msg containing the RRC Reconfiguration message */

}lte_rrc_irat_to_lte_psho_reqi_s;

/*! @brief IRAT to LTE Redirected EARFCN frequency 
 *         along with blacklisted cells and PCI per frequency.
 */
typedef struct
{
  /* Measurement bandwidth - Default value is LTE_BW_UNKNOWN */
  lte_bandwidth_e       meas_bandwidth;

  /*! Actual LTE Redirected frequency */
  lte_earfcn_t          earfcn;

  /*! (Optional) Blacklisted cells for that frequency (if any) */
  uint8                 blacklisted_cells_count;
  lte_phy_cell_id_t     blacklisted_cells[LTE_RRC_MAX_BLIST_CELLS_PER_FREQ];

  /*! (Optional) If PCI is provided, (applicable to GSM/HDR IRAT only)*/
  uint8                 pci_count; /*Count should be 1 for GSM, and >= 1 for HDR if included*/
  lte_phy_cell_id_t     pci_list[LTE_RRC_IRAT_MAX_X2L_REDIR_PCI_PER_FREQ];

} lte_rrc_irat_to_lte_redir_earfcn_info_s;

/*!
    @brief

    Internal RRC definition of common NAS related info needed for 
    IRAT operations like redir to LTE needed for camping.

    Has the internal RRC definition of PLMN.

    Also see lte_rrc_irat_nas_info_s
*/
typedef struct
{
  /*! RAT priority list */
  sys_rat_pri_list_info_s_type        rat_pri_list;

  /*! TRUE if req_plmn field is present, FALSE otherwise */
  boolean                             req_plmn_info_is_valid; 

  /*! Requested plmn (Internal RRC Type) */
  lte_rrc_plmn_s                      req_plmn;
  
  /*! Network select mode */
  sys_network_selection_mode_e_type   network_select_mode;

  /*! NAS Requested CSG Id */
  sys_csg_id_type                     csg_id;

} lte_rrc_nas_info_s;

/*! @brief Data structure for Inter-RAT To LTE redirection request 
    from inter-RAT MGR to CSP, to search *only* on listed EARFCNs
    provided in the InterRAT Redirection Message.
*/
typedef struct
{
  msgr_hdr_s                                msg_hdr; /* Message header */

  lte_rrc_redir_rat_type_e                  source_rat; /*!< RAT Type */

  /*! NAS related info needed for camping on LTE */
  lte_rrc_nas_info_s                        nas_info;

  /*! Enhanced requested PLMN for eHRPD to LTE  redirection */
  lte_rrc_plmn_list_s                       enh_req_plmn_list_info;

  /*! Num frequencies present */
  uint8                                     earfcn_count;
  /*!< Composite of LTE Frequency with black listed cells , PCI info etc */
  lte_rrc_irat_to_lte_redir_earfcn_info_s   earfcn_info[LTE_RRC_IRAT_MAX_REDIR_FREQ];

  /*! Source cell info for CSG fingerprinting */
  source_cell_info_type                     serving_cell_info;

}lte_rrc_irat_to_lte_redir_list_reqi_s;

/*! @brief Data structure for Inter-RAT To LTE redirection request 
    from inter-RAT MGR to CSP for searching all possible frequencies
    giving special priority to those stored in Acq. database and neighbors
*/
typedef struct
{
  msgr_hdr_s                                msg_hdr; /* Message header */

  lte_rrc_redir_rat_type_e                  source_rat; /*!< RAT Type */

  /*! NAS related info needed for camping on LTE */
  lte_rrc_nas_info_s                        nas_info;

  /*! Enhanced requested PLMN for eHRPD to LTE  redirection */
  lte_rrc_plmn_list_s                       enh_req_plmn_list_info;

  /*! LTE Neighbor List that GERAN/WCDMA RRC gets from SIBs */
  lte_rrc_irat_nbr_earfcn_list_s            nbr_earfcn_list;

  /*! Source cell info for CSG fingerprinting */
  source_cell_info_type                     serving_cell_info;

  /*! Num prioritized bands sent from IRAT during re diection */
  uint8                                     irat_redir_num_prioritized_bands;

  /*! Current mode of X2L redirection */
  lte_rrc_irat_from_irat_redir_mode_e      mode;

   /*! Prioritized band list sent from IRAT during re direction */
  sys_sband_lte_e_type                      irat_redir_band_priority_list[LTE_RRC_IRAT_MAX_REDIR_FREQ];

}lte_rrc_irat_to_lte_redir_full_reqi_s;

/*! @brief Data Structure for ML1 Revoke Reqi
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr;
} lte_rrc_ml1_revoke_reqi_s;

/*! @brief Data Structure for requesting CGI info for a W cell
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr; /*!< Message header */

  uint16                         uarfcn;  /*!< UARFCN in the target RAT */

  uint16                         psc; /*!< PSC of the UTRA cell for which CGI is requested */

  uint16                         time_avail_for_cgi; /*! Time available for IRAT CGI reading (in ms) */

} lte_rrc_irat_from_lte_to_w_get_cgi_reqi_s;

/*! @brief Data Structure for aborting IRAT CGI info acquistion in W
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr;

} lte_rrc_irat_from_lte_to_w_abort_cgi_reqi_s;

/*! @brief Data Structure for requesting CGI info for a TDS cell
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr; /*!< Message header */

  uint16                         uarfcn;  /*!< UARFCN in the target RAT */

  uint16                         psc; /*!< PSC of the UTRA cell for which CGI is requested */

  uint16                         time_avail_for_cgi; /*! Time available for IRAT CGI reading (in ms) */

} lte_rrc_irat_from_lte_to_tds_get_cgi_reqi_s;

/*! @brief Data Structure for aborting IRAT CGI info acquistion in tds
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr;

} lte_rrc_irat_from_lte_to_tds_abort_cgi_reqi_s;

/*! @brief Data Structure for requesting CGI info for a G cell
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr; /*!< Message header */

  uint8                          num_arfcns; /*!< Num of ARFCNs passed */

  uint16                         arfcns[LTE_CPHY_MEAS_MAX_GERAN_FREQS_IN_GROUP];  
                                                /*!< ARFCNs in the target RAT */

  geran_grr_band_ind_e           band_ind;  /*!< Indicates whether the passed ARFCN 
                                                 belongs to 1800 or 1900 band if it 
                                                 lies in the 'overlap' range 512 - 810. */

  lte_cphy_geran_phy_cell_id_s   geran_cell_id; /*!< Cell id of the G cell for which CGI is requested*/

  uint16                         time_avail_for_cgi; /*! Time available for IRAT CGI reading (in ms) */


  geran_grr_lte_cgi_rsp_info_t   last_cgi_rsp; /*! Last CGI rsp received from G */

} lte_rrc_irat_from_lte_to_g_get_cgi_reqi_s;

/*! @brief Data Structure for aborting IRAT CGI info acquistion in G
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr;

} lte_rrc_irat_from_lte_to_g_abort_cgi_reqi_s;

/*! @brief Data Structure for requesting CGI info for a DO cell
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr; /*!< Message header */

  sys_channel_type               channel; /*!< DO band and channel number */

  lte_phy_cell_id_t              report_cgi_cell_id; /*!< PN Offset for which CGI needs to be reported */

  uint16                         time_avail_for_cgi; /*! Time available for IRAT CGI reading (in ms) */

  uint8                          search_window_size; /*! DO search_window_size */

} lte_rrc_irat_from_lte_to_do_get_cgi_reqi_s;

/*! @brief Data Structure for aborting IRAT CGI info acquistion in DO
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr;

} lte_rrc_irat_from_lte_to_do_abort_cgi_reqi_s;

/*! @brief Data Structure for requesting CGI info for a 1x cell
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr; /*!< Message header */

  sys_channel_type               channel; /*!< 1x band and channel number */

  lte_phy_cell_id_t              report_cgi_cell_id; /*!< PN Offset for which CGI needs to be reported */

  uint16                         time_avail_for_cgi; /*! Time available for IRAT CGI reading (in ms) */

} lte_rrc_irat_from_lte_to_1x_get_cgi_reqi_s;

/*! @brief Data Structure for aborting IRAT CGI info acquistion in 1x
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr;

} lte_rrc_irat_from_lte_to_1x_abort_cgi_reqi_s;

/*! @brief Data Structure for activating a TMGI in EMP module
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr;

  lte_rrc_embms_session_info_s   session_info; /*!< Session info of the service to be activated */

  boolean                        loopback_mode; /*!< Flag when EMBMS is operating 
                                                          in Loopback test mode C*/
  lte_mbsfn_area_id_t tlb_area_id; /*!MBSFN Area ID for which Loopback mode C
                                  is needed*/

  lte_pmch_id_t tlb_pmch_id;   /*!< PMCH id */
  lte_lc_id_t tlb_lc_id;       /*!< LC ID */

} lte_rrc_esmgr_act_tmgi_reqi_s;

/*! @brief Data Structure for deactivating a TMGI in EMP module
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr;

  lte_rrc_embms_session_info_s   session_info; /*!< Session info of the service to be deactivated */

  boolean                        deact_all_tmgi;  /*!< If all active TMGIs should be deactivated */

} lte_rrc_esmgr_deact_tmgi_reqi_s;


/*! @brief Data Structure for activating a TMGI and deactivating a TMGI in one shot in EMP module
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr;

  lte_rrc_embms_session_info_s   act_session_info; /*!< Session info of the service to be activated */

  lte_rrc_embms_session_info_s   deact_session_info; /*!< Session info of the service to be deactivated */

} lte_rrc_esmgr_act_deact_tmgi_reqi_s;

/*! @brief Data Structure for getting the available services from EMP module
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr;

} lte_rrc_esmgr_avail_tmgi_list_reqi_s;

/*! @brief Enumeration of REQI id nums
*/
typedef enum
{
  /*Message Handler*/
  RRC_REQI_SEND_UL_MSG = 0x00,
  /*RRC Controller*/
  RRC_REQI_MODE_CHANGE = 0x01,
  /*SIB*/
  RRC_REQI_GET_SIBS = 0x02,
  RRC_REQI_SIB_ABORT = 0x03,
  /*CEP*/
  /*CRP*/
  /*CSP*/
  RRC_REQI_INITIATE_CELL_SEL = 0x04,
  RRC_REQI_CSP_CELL_SELECT_ABORT =0x05,
  /*Config*/
  /*LLC*/
  RRC_REQI_CFG = 0x06,
  /*Measurement*/
  RRC_REQI_MEAS_CFG = 0x07,

  /* Inter-RAT from LTE */
  RRC_REQI_IRAT_FROM_LTE_ABORT = 0x08,

  RRC_REQI_IRAT_FROM_LTE_TO_1X_REDIR = 0x09,
  RRC_REQI_IRAT_FROM_LTE_TO_1X_RESEL = 0x0A,

  RRC_REQI_IRAT_FROM_LTE_TO_DO_REDIR = 0x0B,
  RRC_REQI_IRAT_FROM_LTE_TO_DO_RESEL = 0x0C,

  RRC_REQI_IRAT_FROM_LTE_TO_G_REDIR  = 0x0D,
  RRC_REQI_IRAT_FROM_LTE_TO_G_RESEL  = 0x0E,

  RRC_REQI_IRAT_FROM_LTE_TO_W_RESEL  = 0x0F,
  RRC_REQI_IRAT_FROM_LTE_TO_W_REDIR  = 0x10,

  RRC_REQI_IRAT_FROM_LTE_TO_DO_CAPABILITIES = 0x11,
  RRC_REQI_IRAT_FROM_LTE_TO_W_CAPABILITIES = 0x12,
  RRC_REQI_IRAT_FROM_LTE_TO_G_CS_CAPABILITIES = 0x13,
  RRC_REQI_IRAT_FROM_LTE_TO_G_PS_CAPABILITIES = 0x14,

  /* Inter-RAT to LTE */
  RRC_REQI_IRAT_TO_LTE_RESEL        = 0x15,
  RRC_REQI_IRAT_TO_LTE_RESEL_ABORT  = 0x16,
  RRC_REQI_IRAT_TO_LTE_REDIR_LIST   = 0x17,
  RRC_REQI_IRAT_TO_LTE_REDIR_FULL    = 0x18,
  RRC_REQI_IRAT_TO_LTE_REDIR_ABORT   = 0x19,

  /* PLMN_Search */
  RRC_REQI_PLMN_SEARCH_ABORT        =0x1A,

  /* IRAT PLMN Search */
  RRC_REQI_IRAT_TO_LTE_FROM_G_PLMN_SRCH         = 0x1B,
  RRC_REQI_IRAT_TO_LTE_FROM_G_ABORT_PLMN_SRCH   = 0x1C,
  RRC_REQI_IRAT_TO_LTE_FROM_W_PLMN_SRCH         = 0x1D,
  RRC_REQI_IRAT_TO_LTE_FROM_W_ABORT_PLMN_SRCH   = 0x1E,
  RRC_REQI_IRAT_FROM_LTE_TO_G_PLMN_SRCH         = 0x1F,
  RRC_REQI_IRAT_FROM_LTE_TO_W_PLMN_SRCH         = 0x20,

  RRC_REQI_ML1_REVOKE =0x21,

  /* Inter-RAT CCO to G */
  RRC_REQI_IRAT_FROM_LTE_TO_G_CCO = 0x22,

  RRC_REQI_IRAT_FROM_LTE_TO_G_INTERNAL_REDIR = 0x23,

  RRC_REQI_IRAT_FROM_LTE_TO_W_GET_CGI         = 0x24,
  RRC_REQI_IRAT_FROM_LTE_TO_W_ABORT_CGI       = 0x25,
  RRC_REQI_IRAT_FROM_LTE_TO_G_GET_CGI         = 0x26,
  RRC_REQI_IRAT_FROM_LTE_TO_G_ABORT_CGI       = 0x27,

  RRC_REQI_IRAT_TO_LTE_PSHO                   = 0x28,
  RRC_REQI_IRAT_TO_LTE_PSHO_ABORT             = 0x29,
  RRC_REQI_ESMGR_ACT_TMGI                      = 0x2A,
  RRC_REQI_ESMGR_DEACT_TMGI                    = 0x2B,
  RRC_REQI_ESMGR_AVAIL_TMGI_LIST               = 0x2C,
  /* LLC */
  RRC_REQI_PENDED_CFG                          = 0x2D,
  RRC_REQI_START_INTERNAL_CELL_SELECTION       = 0x2E,

  RRC_REQI_IRAT_FROM_LTE_TO_TDS_CAPABILITIES   = 0x2F,
  RRC_REQI_IRAT_FROM_LTE_TO_TDS_GET_CGI         = 0x30,
  RRC_REQI_IRAT_FROM_LTE_TO_TDS_ABORT_CGI       = 0x31,
  RRC_REQI_IRAT_FROM_LTE_TO_TDS_PLMN_SRCH         = 0x32,
  RRC_REQI_IRAT_FROM_LTE_TO_TDS_RESEL  = 0x33,
  RRC_REQI_IRAT_FROM_LTE_TO_TDS_REDIR  = 0x34,
  RRC_REQI_IRAT_TO_LTE_FROM_TDS_PLMN_SRCH         = 0x35,
  RRC_REQI_IRAT_TO_LTE_FROM_TDS_ABORT_PLMN_SRCH   = 0x36,
  RRC_REQI_IRAT_FROM_LTE_TO_DO_GET_CGI         = 0x37,
  RRC_REQI_IRAT_FROM_LTE_TO_DO_ABORT_CGI       = 0x38,
  RRC_REQI_IRAT_FROM_LTE_TO_1X_GET_CGI         = 0x39,
  RRC_REQI_IRAT_FROM_LTE_TO_1X_ABORT_CGI       = 0x3A,

  /* eMBMS - contd */
  RRC_REQI_ESMGR_ACT_DEACT_TMGI                   = 0x3B,

  /* IRAT PLMN Search - contd*/
  RRC_REQI_IRAT_TO_LTE_FROM_G_SUSPEND_PLMN_SRCH   = 0x3C,
  RRC_REQI_IRAT_TO_LTE_FROM_W_SUSPEND_PLMN_SRCH   = 0x3D,
  RRC_REQI_IRAT_TO_LTE_FROM_TDS_SUSPEND_PLMN_SRCH = 0x3E,

  RRC_REQI_IRAT_FROM_LTE_TO_G_GET_PLMN_PRTL_SRCH = 0x3F,

  RRC_REQI_IRAT_FROM_LTE_TO_W_GET_PLMN_PRTL_SRCH = 0x40,

  RRC_REQI_IRAT_FROM_LTE_TO_TDS_GET_PLMN_PRTL_SRCH = 0x41,

  /* IRAT PLMN Search - contd*/
  RRC_REQI_IRAT_TO_LTE_FROM_G_GET_PLMN_PRTL_SRCH   = 0x42,
  RRC_REQI_IRAT_TO_LTE_FROM_W_GET_PLMN_PRTL_SRCH   = 0x43,
  RRC_REQI_IRAT_TO_LTE_FROM_TDS_GET_PLMN_PRTL_SRCH = 0x44,

  /* Misc */
  RRC_REQI_SIB_EXTND_TMR                           = 0x45,

  RRC_REQI_MAX     /*!< Max value */
}lte_rrc_reqi_type_e;

/*! @brief Enumeration for Inter-RAT From-LTE Reselection Failure Causes
*/
typedef enum
{
  LTE_RRC_IRAT_FROM_LTE_RESEL_FAILURE_CAUSE_UNKNOWN        = 0,
  LTE_RRC_IRAT_FROM_LTE_RESEL_FAILURE_CAUSE_CELL_BARRED    = 1,       
  LTE_RRC_IRAT_FROM_LTE_RESEL_FAILURE_CAUSE_FREQ_BARRED    = 2,
  LTE_RRC_IRAT_FROM_LTE_RESEL_FAILURE_CAUSE_FORBIDDEN_TA   = 3,
  LTE_RRC_IRAT_FROM_LTE_RESEL_FAILURE_CAUSE_FORBIDDEN_CSG_ID = 4,
  LTE_RRC_IRAT_FROM_LTE_RESEL_FAILURE_CAUSE_PLMN_MISMATCH = 5,
  LTE_RRC_IRAT_FROM_LTE_RESEL_FAILURE_CAUSE_BSIC_MISMATCH = 6
}lte_rrc_irat_from_lte_failure_cause_type_e;


/*! @brief Internal requests that various RRC procedures receive 
*/
enum
{
  /*Internal Requests*/

  LTE_RRC_REQI_FIRST = MSGR_UMID_BASE(MSGR_LTE_RRC, MSGR_TYPE_REQI),

  /*Message Handler*/

  MSGR_DEFINE_UMID(LTE, RRC, REQI, SEND_UL_MSG, RRC_REQI_SEND_UL_MSG, 
                   lte_rrc_send_ul_msg_reqi_s),

  /*RRC Controller*/
  MSGR_DEFINE_UMID(LTE, RRC, REQI, MODE_CHANGE, RRC_REQI_MODE_CHANGE, 
                   lte_rrc_mode_change_reqi_s),

  /*SIB*/ 

  MSGR_DEFINE_UMID(LTE, RRC, REQI, GET_SIBS, RRC_REQI_GET_SIBS, 
                   lte_rrc_get_sibs_reqi_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQI, SIB_ABORT, RRC_REQI_SIB_ABORT, 
                   lte_rrc_sib_abort_reqi_s),

  /*CEP*/

  /*CRP*/

  /*CSP*/ 
   MSGR_DEFINE_UMID(LTE, RRC, REQI, INITIATE_CELL_SEL, RRC_REQI_INITIATE_CELL_SEL, 
                  lte_rrc_initiate_cell_sel_reqi_s),

   MSGR_DEFINE_UMID(LTE, RRC, REQI, CSP_CELL_SELECT_ABORT, RRC_REQI_CSP_CELL_SELECT_ABORT, 
                    lte_rrc_csp_cell_select_abort_reqi_s),

   MSGR_DEFINE_UMID(LTE, RRC, REQI, START_INTERNAL_CELL_SELECTION, RRC_REQI_START_INTERNAL_CELL_SELECTION, 
                    lte_rrc_start_internal_cell_selection_reqi_s),

  /*Config*/

  /*LLC*/
  MSGR_DEFINE_UMID(LTE, RRC, REQI, CFG, RRC_REQI_CFG, lte_rrc_cfg_reqi_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQI, PENDED_CFG, RRC_REQI_PENDED_CFG, lte_rrc_pended_cfg_reqi_s),

  /*Measurement*/
  MSGR_DEFINE_UMID(LTE, RRC, REQI, MEAS_CFG, RRC_REQI_MEAS_CFG, 
                   lte_rrc_meas_cfg_reqi_s),

  /* Inter-RAT : From LTE Redirection/Reselection Abort */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_ABORT, RRC_REQI_IRAT_FROM_LTE_ABORT, 
                   lte_rrc_irat_from_lte_abort_reqi_s ),

  /* Inter-RAT :  LTE to DO Redirection */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_DO_REDIR, RRC_REQI_IRAT_FROM_LTE_TO_DO_REDIR, 
                   lte_rrc_irat_from_lte_redir_reqi_s),

  /* Inter-RAT :  LTE to 1X Redirection */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_1X_REDIR, RRC_REQI_IRAT_FROM_LTE_TO_1X_REDIR, 
                   lte_rrc_irat_from_lte_redir_reqi_s),

  /* Inter-RAT :  LTE to G Redirection */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_G_REDIR, RRC_REQI_IRAT_FROM_LTE_TO_G_REDIR, 
                   lte_rrc_irat_from_lte_redir_reqi_s),

  /* Inter-RAT :  LTE to W Redirection */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_W_REDIR, RRC_REQI_IRAT_FROM_LTE_TO_W_REDIR, 
                   lte_rrc_irat_from_lte_redir_reqi_s),
  
  /* Inter-RAT :  LTE to TDS Redirection */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_TDS_REDIR, RRC_REQI_IRAT_FROM_LTE_TO_TDS_REDIR, 
                   lte_rrc_irat_from_lte_redir_reqi_s),

  /* Inter-RAT :  LTE to DO Reselection */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_DO_RESEL, RRC_REQI_IRAT_FROM_LTE_TO_DO_RESEL, 
                   lte_rrc_irat_from_lte_resel_reqi_s),

  /* Inter-RAT :  LTE to 1X Reselection */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_1X_RESEL, RRC_REQI_IRAT_FROM_LTE_TO_1X_RESEL, 
                   lte_rrc_irat_from_lte_resel_reqi_s),

  /* Inter-RAT :  LTE to G Reselection */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_G_RESEL, RRC_REQI_IRAT_FROM_LTE_TO_G_RESEL, 
                   lte_rrc_irat_from_lte_resel_reqi_s),

  /* Inter-RAT :  LTE to W Reselection */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_W_RESEL, RRC_REQI_IRAT_FROM_LTE_TO_W_RESEL, 
                   lte_rrc_irat_from_lte_resel_reqi_s),

  /* Inter-RAT :  LTE to TDS Reselection */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_TDS_RESEL, RRC_REQI_IRAT_FROM_LTE_TO_TDS_RESEL, 
                   lte_rrc_irat_from_lte_resel_reqi_s),

  /* Inter-RAT :  G/W to LTE Reselection */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_TO_LTE_RESEL, RRC_REQI_IRAT_TO_LTE_RESEL, 
                   lte_rrc_irat_to_lte_resel_reqi_s), 

  /* Inter-RAT :  G/W to LTE Reselection Abort */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_TO_LTE_RESEL_ABORT, RRC_REQI_IRAT_TO_LTE_RESEL_ABORT, 
                   lte_rrc_irat_to_lte_resel_abort_reqi_s), 

   /* Inter-RAT :  LTE to DO Capabilities Req (Internal)*/
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_DO_CAPABILITIES, 
                   RRC_REQI_IRAT_FROM_LTE_TO_DO_CAPABILITIES, 
                   lte_rrc_irat_from_lte_to_do_capabilities_reqi_s), 

  /* Inter-RAT :  LTE to W Capabilities Req (Internal)*/
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_W_CAPABILITIES, 
                   RRC_REQI_IRAT_FROM_LTE_TO_W_CAPABILITIES, 
                   lte_rrc_irat_from_lte_to_w_capabilities_reqi_s),

    /* Inter-RAT :  LTE to TDS Capabilities Req (Internal)*/
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_TDS_CAPABILITIES, 
                   RRC_REQI_IRAT_FROM_LTE_TO_TDS_CAPABILITIES, 
                   lte_rrc_irat_from_lte_to_tds_capabilities_reqi_s), 

  /* Inter-RAT :  LTE to GERAN-CS Capabilities Req (Internal)*/
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_G_CS_CAPABILITIES, 
                   RRC_REQI_IRAT_FROM_LTE_TO_G_CS_CAPABILITIES, 
                   lte_rrc_irat_from_lte_to_g_cs_capabilities_reqi_s), 

   /* Inter-RAT :  LTE to GERAN-PS Capabilities Req (Internal)*/
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_G_PS_CAPABILITIES, 
                   RRC_REQI_IRAT_FROM_LTE_TO_G_PS_CAPABILITIES, 
                   lte_rrc_irat_from_lte_to_g_ps_capabilities_reqi_s), 

  /* Inter-RAT :  G/W to LTE Redirection List Request */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_TO_LTE_REDIR_LIST, RRC_REQI_IRAT_TO_LTE_REDIR_LIST, 
                   lte_rrc_irat_to_lte_redir_list_reqi_s), 

  /* Inter-RAT :  G/W to LTE Redirection Full Request */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_TO_LTE_REDIR_FULL, RRC_REQI_IRAT_TO_LTE_REDIR_FULL, 
                   lte_rrc_irat_to_lte_redir_full_reqi_s), 

  /* Inter-RAT :  G/W to LTE Redirection Abort */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_TO_LTE_REDIR_ABORT, RRC_REQI_IRAT_TO_LTE_REDIR_ABORT, 
                   lte_rrc_irat_to_lte_redir_abort_reqi_s), 

  /* PLMN_Search */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, PLMN_SEARCH_ABORT, RRC_REQI_PLMN_SEARCH_ABORT, 
                    lte_rrc_plmn_search_abort_reqi_s),

  /* Inter-RAT : G to LTE PLMN_Search */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_TO_LTE_FROM_G_PLMN_SRCH, RRC_REQI_IRAT_TO_LTE_FROM_G_PLMN_SRCH, 
                    lte_irat_plmn_srch_req_s),

  /* Inter-RAT : G to LTE PLMN_Search Abort */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_TO_LTE_FROM_G_ABORT_PLMN_SRCH, RRC_REQI_IRAT_TO_LTE_FROM_G_ABORT_PLMN_SRCH, 
                    lte_irat_abort_plmn_srch_req_s),

  /* Inter-RAT : G to LTE PLMN_Search Suspend */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_TO_LTE_FROM_G_SUSPEND_PLMN_SRCH, RRC_REQI_IRAT_TO_LTE_FROM_G_SUSPEND_PLMN_SRCH, 
                    lte_irat_suspend_plmn_srch_req_s),

  /* Inter-RAT : G to LTE PLMN_Search Get Partial results */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_TO_LTE_FROM_G_GET_PLMN_PRTL_SRCH, RRC_REQI_IRAT_TO_LTE_FROM_G_GET_PLMN_PRTL_SRCH, 
                    lte_irat_get_plmn_prtl_results_req_s),

  /* Inter-RAT : W to LTE PLMN_Search */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_TO_LTE_FROM_W_PLMN_SRCH, RRC_REQI_IRAT_TO_LTE_FROM_W_PLMN_SRCH, 
                    lte_irat_plmn_srch_req_s),

  /* Inter-RAT : W to LTE PLMN_Search Abort */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_TO_LTE_FROM_W_ABORT_PLMN_SRCH, RRC_REQI_IRAT_TO_LTE_FROM_W_ABORT_PLMN_SRCH, 
                    lte_irat_abort_plmn_srch_req_s),

  /* Inter-RAT : W to LTE PLMN_Search Suspend */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_TO_LTE_FROM_W_SUSPEND_PLMN_SRCH, RRC_REQI_IRAT_TO_LTE_FROM_W_SUSPEND_PLMN_SRCH, 
                    lte_irat_suspend_plmn_srch_req_s),

  /* Inter-RAT : W to LTE PLMN_Search Get Partial results */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_TO_LTE_FROM_W_GET_PLMN_PRTL_SRCH, RRC_REQI_IRAT_TO_LTE_FROM_W_GET_PLMN_PRTL_SRCH, 
                    lte_irat_get_plmn_prtl_results_req_s),

  /* Inter-RAT : TDS to LTE PLMN_Search */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_TO_LTE_FROM_TDS_PLMN_SRCH, RRC_REQI_IRAT_TO_LTE_FROM_TDS_PLMN_SRCH, 
                    lte_irat_plmn_srch_req_s),

  /* Inter-RAT : TDS to LTE PLMN_Search Abort */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_TO_LTE_FROM_TDS_ABORT_PLMN_SRCH, RRC_REQI_IRAT_TO_LTE_FROM_TDS_ABORT_PLMN_SRCH, 
                    lte_irat_abort_plmn_srch_req_s),

  /* Inter-RAT : TDS to LTE PLMN_Search Suspend */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_TO_LTE_FROM_TDS_SUSPEND_PLMN_SRCH, RRC_REQI_IRAT_TO_LTE_FROM_TDS_SUSPEND_PLMN_SRCH, 
                    lte_irat_suspend_plmn_srch_req_s),

  /* Inter-RAT : TDS to LTE PLMN_Search Get Partial results */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_TO_LTE_FROM_TDS_GET_PLMN_PRTL_SRCH, RRC_REQI_IRAT_TO_LTE_FROM_TDS_GET_PLMN_PRTL_SRCH, 
                    lte_irat_get_plmn_prtl_results_req_s),

  /* Inter-RAT : LTE to G PLMN_Search */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_G_PLMN_SRCH, RRC_REQI_IRAT_FROM_LTE_TO_G_PLMN_SRCH, 
                    lte_irat_plmn_srch_req_s),

  /* Inter-RAT : LTE to W PLMN_Search */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_W_PLMN_SRCH, RRC_REQI_IRAT_FROM_LTE_TO_W_PLMN_SRCH, 
                    lte_irat_plmn_srch_req_s),

  /* Inter-RAT : LTE to TDS PLMN_Search */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_TDS_PLMN_SRCH, RRC_REQI_IRAT_FROM_LTE_TO_TDS_PLMN_SRCH, 
                    lte_irat_plmn_srch_req_s),

    /* Inter-RAT : LTE to G GET_PLMN_PRTL */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_G_GET_PLMN_PRTL_SRCH, RRC_REQI_IRAT_FROM_LTE_TO_G_GET_PLMN_PRTL_SRCH, 
                    lte_irat_get_plmn_prtl_results_req_s),

  /* Inter-RAT : LTE to W GET_PLMN_PRTL */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_W_GET_PLMN_PRTL_SRCH, RRC_REQI_IRAT_FROM_LTE_TO_W_GET_PLMN_PRTL_SRCH, 
                    lte_irat_get_plmn_prtl_results_req_s),

  /* Inter-RAT : LTE to TDS GET_PLMN_PRTL */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_TDS_GET_PLMN_PRTL_SRCH, RRC_REQI_IRAT_FROM_LTE_TO_TDS_GET_PLMN_PRTL_SRCH, 
                    lte_irat_get_plmn_prtl_results_req_s),

   MSGR_DEFINE_UMID(LTE, RRC, REQI, ML1_REVOKE, RRC_REQI_ML1_REVOKE, 
                    lte_rrc_ml1_revoke_reqi_s),

  /* Cell Change Order to GSM */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_G_CCO, 
                   RRC_REQI_IRAT_FROM_LTE_TO_G_CCO, 
                   lte_rrc_irat_from_lte_to_g_cco_reqi_s), 

  /* Inter-RAT :  LTE to G (Internal) Redirection */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_G_INTERNAL_REDIR, RRC_REQI_IRAT_FROM_LTE_TO_G_INTERNAL_REDIR, 
                   lte_rrc_irat_from_lte_internal_redir_reqi_s),

  /* Inter-RAT : LTE to W GET_CGI_REQI */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_W_GET_CGI, RRC_REQI_IRAT_FROM_LTE_TO_W_GET_CGI, 
                    lte_rrc_irat_from_lte_to_w_get_cgi_reqi_s),

  /* Inter-RAT : LTE to W ABORT_CGI_REQI */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_W_ABORT_CGI, RRC_REQI_IRAT_FROM_LTE_TO_W_ABORT_CGI, 
                    lte_rrc_irat_from_lte_to_w_abort_cgi_reqi_s),

  /* Inter-RAT : LTE to TDS GET_CGI_REQI */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_TDS_GET_CGI, RRC_REQI_IRAT_FROM_LTE_TO_TDS_GET_CGI, 
                    lte_rrc_irat_from_lte_to_tds_get_cgi_reqi_s),

  /* Inter-RAT : LTE to TDS ABORT_CGI_REQI */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_TDS_ABORT_CGI, RRC_REQI_IRAT_FROM_LTE_TO_TDS_ABORT_CGI, 
                    lte_rrc_irat_from_lte_to_tds_abort_cgi_reqi_s),

  /* Inter-RAT : LTE to G GET_CGI_REQI */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_G_GET_CGI, RRC_REQI_IRAT_FROM_LTE_TO_G_GET_CGI, 
                    lte_rrc_irat_from_lte_to_g_get_cgi_reqi_s),

  /* Inter-RAT : LTE to G ABORT_CGI_REQI */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_G_ABORT_CGI, RRC_REQI_IRAT_FROM_LTE_TO_G_ABORT_CGI, 
                    lte_rrc_irat_from_lte_to_g_abort_cgi_reqi_s),

  /* Inter-RAT : LTE to DO GET_CGI_REQI */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_DO_GET_CGI, RRC_REQI_IRAT_FROM_LTE_TO_DO_GET_CGI, 
                    lte_rrc_irat_from_lte_to_do_get_cgi_reqi_s),

  /* Inter-RAT : LTE to DO ABORT_CGI_REQI */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_DO_ABORT_CGI, RRC_REQI_IRAT_FROM_LTE_TO_DO_ABORT_CGI, 
                    lte_rrc_irat_from_lte_to_do_abort_cgi_reqi_s),

  /* Inter-RAT : LTE to 1X GET_CGI_REQI */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_1X_GET_CGI, RRC_REQI_IRAT_FROM_LTE_TO_1X_GET_CGI, 
                    lte_rrc_irat_from_lte_to_1x_get_cgi_reqi_s),

  /* Inter-RAT : LTE to 1X ABORT_CGI_REQI */
   MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_FROM_LTE_TO_1X_ABORT_CGI, RRC_REQI_IRAT_FROM_LTE_TO_1X_ABORT_CGI, 
                    lte_rrc_irat_from_lte_to_1x_abort_cgi_reqi_s),

  /* Inter-RAT :  to LTE PSHO */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_TO_LTE_PSHO, RRC_REQI_IRAT_TO_LTE_PSHO, 
                   lte_rrc_irat_to_lte_psho_reqi_s), 

  /* Inter-RAT :  to LTE PSHO Abort */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, IRAT_TO_LTE_PSHO_ABORT, RRC_REQI_IRAT_TO_LTE_PSHO_ABORT, 
                   lte_rrc_irat_to_lte_psho_abort_reqi_s), 

  /* eMBMS */
  MSGR_DEFINE_UMID(LTE, RRC, REQI, ESMGR_ACT_TMGI, RRC_REQI_ESMGR_ACT_TMGI, 
                   lte_rrc_esmgr_act_tmgi_reqi_s), 

  MSGR_DEFINE_UMID(LTE, RRC, REQI, ESMGR_DEACT_TMGI, RRC_REQI_ESMGR_DEACT_TMGI, 
                   lte_rrc_esmgr_deact_tmgi_reqi_s), 

  MSGR_DEFINE_UMID(LTE, RRC, REQI, ESMGR_AVAIL_TMGI_LIST, RRC_REQI_ESMGR_AVAIL_TMGI_LIST, 
                   lte_rrc_esmgr_avail_tmgi_list_reqi_s), 

  MSGR_DEFINE_UMID(LTE, RRC, REQI, ESMGR_ACT_DEACT_TMGI, RRC_REQI_ESMGR_ACT_DEACT_TMGI, 
                   lte_rrc_esmgr_act_deact_tmgi_reqi_s), 

  MSGR_DEFINE_UMID(LTE, RRC, REQI, SIB_EXTND_TMR, RRC_REQI_SIB_EXTND_TMR, 
                   lte_rrc_sib_extnd_tmr_reqi_s), 
  
  LTE_RRC_REQI_MAX,
  LTE_RRC_REQI_LAST = LTE_RRC_REQI_MAX - 1
};

/*! @brief Data structure for UL msg Confirmation
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  uint8 mu_id; /*!< Message unit identifier */
  lte_rrc_ul_msg_status_e status; /*!< Response (ack/nack/maybe) */
  lte_rrc_ul_msg_fail_cause_e fail_cause; /*!< Failure cause (used by DT in 
                                               case of nack/maybe) */
} lte_rrc_ul_msg_cnfi_s;

/*! @brief Data structure for Mode Change Confirmation 
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  boolean status; /*!< TRUE = success, FALSE = failure */
  lte_rrc_mode_change_e mode; /*!< Mode in the request */
} lte_rrc_mode_change_cnfi_s;

/*! @brief Data structure for storing overlapping frequencies populated from sib1
*/
typedef struct
{
  uint8 num_freqs;/*!< number of overlapping frequencies*/
  lte_earfcn_t new_dl_freq[LTE_CPHY_MAX_OVERLAP_BANDS]; /*!< list of converted EARFCN of overlapping bands*/
} lte_rrc_earfcn_list;

/*! @brief Data structure for Get SIBS Confirmation 
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_get_sibs_status_e status; /*!< Status */
  lte_rrc_cell_info_s cell_info; /*!< Cell info */
  lte_rrc_earfcn_list new_dl_freq_list; /*!< List of converted EARFCNs of overlapping bands*/
} lte_rrc_get_sibs_cnfi_s;

/*! @brief Data structure for SIB1 Avail Indi 
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router Header */
  lte_rrc_cell_id_s cell_id; /*!< Cell Id */
  lte_rrc_osys_SystemInformationBlockType1 *sib1_ptr; /*!< Sib1 decoded ptr for populating parameters in service Indi */
}lte_rrc_sib1_avail_indi_s;

/*! @brief Data structure for SEC Active Indi
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router Header */
}lte_rrc_sec_active_indi_s;

/*! @brief Data structure for Config Confirmation 
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_rrc_proc_e proc; /*!< Procedure ID for which this REQI is meant for */
	lte_rrc_cfg_type_e cfg_type;  /*!< This will indicate the configuration type */
  uint32 trans_id;  /*!< transaction ID to pair the CNF_CNFI with CNF_REQI. */     
  IxErrnoType status; /*!< status field to indicate the status of cfg req */
  lte_rrc_cfg_failure_type_e failure_type; /*!< Failure type applicable when status != E_SUCCESS */
} lte_rrc_cfg_cnfi_s;

/*! @brief Data structure for Measurement Config Confirmation 
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_errno_e status; /*!< status field to indicate status of meas cfg req */

} lte_rrc_meas_cfg_cnfi_s;

/*! @brief Data structure for Inter-RAT To LTE Reselection Confirmation 
    from CSP to Inter-RAT MGR
*/
typedef struct
{
  msgr_hdr_s  msg_hdr;  /*!< Message router header */
  lte_errno_e status;   /*!< status field to indicate status of reselection */
  /* Cause of reselection failure. 
     Applicable only if status field is not E_SUCCESS */
  lte_rrc_irat_to_lte_failure_cause_type_e fail_cause; 

  /*! For certain failures, time ( in Sec ) when the same cell should be considered i.e cell barring time */
  uint32                                   cell_bar_time;
}lte_rrc_irat_to_lte_resel_cnfi_s;

/*! @brief Data structure for Inter-RAT To LTE Reselection Abort Confirmation 
    from CSP to Inter-RAT MGR
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
}lte_rrc_irat_to_lte_resel_abort_cnfi_s;

/*! @brief Data structure for Inter-RAT To LTE PSHO Confirmation 
    from Config to Inter-RAT MGR
*/
typedef struct
{
  msgr_hdr_s  msg_hdr;  /*!< Message router header */

  /*! Did HO succeed or fail */
  lte_rrc_irat_to_lte_psho_status_type_e          ho_status;

  /*! failure cause only if HO failed */
  lte_rrc_irat_to_lte_psho_failure_cause_type_e   failure_cause;

}lte_rrc_irat_to_lte_psho_cnfi_s;

/*! @brief Data structure for Inter-RAT To LTE PSHO Abort Confirmation 
    from Config to Inter-RAT MGR
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
}lte_rrc_irat_to_lte_psho_abort_cnfi_s;

/*! @brief Data structure for Inter-RAT To LTE Redirection Confirmation 
    from CSP to Inter-RAT MGRs

    No fail_cause because this message implies that no suitable cell
    was found in LTE
*/
typedef struct
{
  msgr_hdr_s  msg_hdr;  /*!< Message router header */

  lte_errno_e status;   /*!< Status of redirection */
  lte_rrc_irat_to_lte_redir_failure_cause_type_e cause;

}lte_rrc_irat_to_lte_redir_cnfi_s;

/*! @brief Data structure for Inter-RAT To LTE Redirection Abort Confirmation 
    from CSP to Inter-RAT MGR
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
}lte_rrc_irat_to_lte_redir_abort_cnfi_s;


/*! @brief Internal Message for DO IRAT mgr to LTE Capabilities Response Confirmation  
*/
typedef struct
{
  msgr_hdr_struct_type msg_hdr;

  uint32                capabilities_container_length;
  uint8                 capabilities_container[HDRCP_MAX_CAP_CONTAINER_LENGTH];
} lte_rrc_irat_from_do_to_lte_capabilities_cnfi_s;

/*! @brief Internal Message for W IRAT mgr to LTE Capabilities Response Confirmation  
*/
typedef struct
{
  msgr_hdr_struct_type msg_hdr;

  uint32                capabilities_container_length;
  uint8                 capabilities_container[WCDMA_MAX_CAP_CONTAINER_LENGTH];
} lte_rrc_irat_from_w_to_lte_capabilities_cnfi_s;

/*! @brief Internal Message for TDS IRAT mgr to LTE Capabilities Response Confirmation  
*/
typedef struct
{
  msgr_hdr_struct_type msg_hdr;

  uint32                capabilities_container_length;
  uint8                 capabilities_container[TDSCDMA_MAX_CAP_CONTAINER_LENGTH];
} lte_rrc_irat_from_tds_to_lte_capabilities_cnfi_s;

/*! @brief Internal Message for from-G IRAT mgr to LTE 
GERAN CS Capabilities Response Confirmation  
*/
typedef struct
{
  msgr_hdr_struct_type msg_hdr;

  uint32                capabilities_container_length;
  uint8                 capabilities_container[GERAN_CS_MAX_CAP_CONTAINER_LENGTH];
} lte_rrc_irat_from_g_cs_to_lte_capabilities_cnfi_s;

/*! @brief Internal Message for from-G IRAT mgr to LTE 
GERAN PS Capabilities Response Confirmation  
*/
typedef struct
{
  msgr_hdr_struct_type msg_hdr;

  uint32                capabilities_container_length;
  uint8                 capabilities_container[GERAN_PS_MAX_CAP_CONTAINER_LENGTH];
  /*! Indicates if DTM is enabled*/
  boolean               geran_dtm_is_supported;  
} lte_rrc_irat_from_g_ps_to_lte_capabilities_cnfi_s;



/*! @brief Data structure for Cell Selection Abort Confirmation 
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

} lte_rrc_csp_cell_select_abort_cnfi_s;

/*! @brief Data structure for IRAT From LTE Abort Confirmation 

    If a connected mode procedure is in progress and gets aborted and
    ML1 was suspended, status is used to inform if ML1 succeeded in
    resuming or not
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_rrc_ml1_status_type_e status;
  lte_rrc_proc_e proc;      /*!< Procedure ID of the Destination module */
} lte_rrc_irat_from_lte_abort_cnfi_s;

/*! @brief Data structure for ML1 Revoke Confirmation 
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

} lte_rrc_ml1_revoke_cnfi_s;

/*! @brief Data Structure for IRAT CGI Get Confirmation from W
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr; /*!< Message router header */

  uint32                         uarfcn; /*!< UARFCN of the cell for which CGI info was requested */

  uint32                         psc;  /*!< PSC of the cell for which CGI info was requested */

  boolean                        cgi_info_included; /*!< TRUE, if CGI acquisition suceeded 
                                                    and CGI info is included, FALSE otherwise */

  wcdma_rrc_lte_cgi_info_type    cgi_info; /*!< CGI information for the requested cell */

} lte_rrc_irat_from_lte_to_w_get_cgi_cnfi_s;

/*! @brief Data Structure for IRAT CGI Get Confirmation from W
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr; /*!< Message router header */

  uint32                         uarfcn; /*!< UARFCN of the cell for which CGI info was requested */

  uint32                         psc;  /*!< PSC of the cell for which CGI info was requested */

  boolean                        cgi_info_included; /*!< TRUE, if CGI acquisition suceeded 
                                                    and CGI info is included, FALSE otherwise */

  tds_rrc_lte_cgi_info_type    cgi_info; /*!< CGI information for the requested cell */

} lte_rrc_irat_from_lte_to_tds_get_cgi_cnfi_s;


/*! @brief Data Structure for IRAT CGI Get Confirmation from G
 */
typedef struct
{
  msgr_hdr_struct_type            msg_hdr; /*!< Message router header */

  uint16                          arfcn;  /*!< ARFCN of the cell in which the CGI cell was found */

  geran_grr_band_ind_e            band_ind;  /*!< @brief Indicates whether the ARFCN belongs to 1800 
                                              or 1900 band if it lies in the 'overlap' range 512 - 810. */

  lte_cphy_geran_phy_cell_id_s    cell_id;  /*!< Cell id of the cell for which CGI info was requested */ 

  geran_grr_lte_cgi_rsp_info_t    cgi_rsp; /*!< CGI response received from G for the requested cell */

  boolean                         rssi_valid; /*! RSSI is valid or not */

  uint8                           rssi; /*! RSSI (RXLEV), range is 0 - 63 */

} lte_rrc_irat_from_lte_to_g_get_cgi_cnfi_s;

/*! @brief Data Structure for IRAT CGI Abort Confirmation from G
 */
typedef struct
{
  msgr_hdr_struct_type            msg_hdr; /*!< Message router header */

} lte_rrc_irat_from_lte_to_g_abort_cgi_cnfi_s;

/*! @brief Data Structure for IRAT CGI Get Confirmation from DO
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr; /*!< Message router header */

  sys_channel_type               channel; /*!< DO band and channel number */

  lte_phy_cell_id_t              report_cgi_cell_id; /*!< PN Offset for which CGI needs to be reported */

  int16                          pilot_strength;    /*!< Pilot strength based on Measurements on the PN (0..63),
                                                         a valid value should always be included in the response,
                                                         in -0.5dB */

  boolean                        cgi_info_included; /*!< TRUE, if CGI acquisition suceeded 
                                                    and CGI info is included, FALSE otherwise */

  hdraddr_type                   cgi_info; /*!< CGI information for the requested cell */

} lte_rrc_irat_from_lte_to_do_get_cgi_cnfi_s;

/*! @brief Data Structure for IRAT CGI Get Confirmation from 1x
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr; /*!< Message router header */

  sys_channel_type               channel; /*!< DO band and channel number */

  lte_phy_cell_id_t              report_cgi_cell_id; /*!< PN Offset for which CGI needs to be reported */

  uint8                          pilot_strength; /*! Pilot strength based on Measurements on the PN (0..63), a valid value 
                                                                                            should always be included in the response */  

  boolean                        cgi_info_included;  /*! TRUE if CGI acquisition succeeded, FALSE otherwise */

  onex_cgi_info_t                cgi_info;   /*! CGI info in 1x*/ 

} lte_rrc_irat_from_lte_to_1x_get_cgi_cnfi_s;

/*! @brief Data Structure for CGI Success Indication to CSG
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  source_cell_info_type   target_cell_info; /*Target cell info */

}lte_rrc_cgi_success_indi_s;

/*! @brief Data Structure for the response for LTE_EMBMS_ACT_TMGI_REQI from EMP module
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr;      /*!< Message router header */

  lte_rrc_embms_session_info_s   session_info; /*!< Session info of the service to be activated */

  lte_rb_id_t                    mrb_id;       /*!< MRB ID for the activated service */

  lte_embms_status_e             act_status;   /*!< Status of the activation */

  lte_mbsfn_area_id_t            mbsfn_area_id;   /*!< MBSFN area id */

} lte_rrc_esmgr_act_tmgi_cnfi_s;

/*! @brief Data Structure for the response for LTE_EMBMS_DEACT_TMGI_REQI from EMP module
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr;      /*!< Message router header */

  lte_rrc_embms_session_info_s   session_info; /*!< Session info of the service to be activated */

  lte_rb_id_t                    mrb_id;       /*!< MRB ID for the activated service */

  lte_embms_status_e             deact_status;   /*!< Status of the deactivation */

  boolean                        deact_all_tmgi;  /*!< If all active TMGIs were deactivated */

} lte_rrc_esmgr_deact_tmgi_cnfi_s;


/*! @brief Data Structure for the response for LTE_EMBMS_ACT_DEACT_TMGI_REQI from EMP module
 */
typedef struct
{
  msgr_hdr_struct_type           msg_hdr;      /*!< Message router header */

  lte_rrc_embms_session_info_s   act_session_info; /*!< Session info of the service to be activated */

  lte_rb_id_t                    act_mrb_id;       /*!< MRB ID for the activated service */

  lte_embms_status_e             act_status;       /*!< Status of the activation */

  lte_rrc_embms_session_info_s   deact_session_info; /*!< Session info of the service to be deactivated */

  lte_rb_id_t                    deact_mrb_id;       /*!< MRB ID for the activated service */

  lte_embms_status_e             deact_status;       /*!< Status of the deactivation */

  lte_mbsfn_area_id_t            act_mbsfn_area_id; /*!< Area ID where service got activated*/

} lte_rrc_esmgr_act_deact_tmgi_cnfi_s;

/*! @brief Data Structure for available services list. 
  */

typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  msgr_attach_s dsm_attach; /*!< dsm attachment containing the List of available TMGIs,
                                 containing maximum of LTE_EMBMS_MAX_AVAIL_SESSIONS */

  uint8 num_tmgi_avail ; /*!< Number of TMGI avail  */

  lte_rrc_embms_session_info_s avail_tmgi_list[LTE_EMBMS_MAX_AVAIL_SESSIONS]; /*!< List of TMGI */

} lte_rrc_esmgr_avail_tmgi_list_indi_s;


/*! @brief Message for CSG Search Confirmation
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_plmn_search_status_e status; /*!< Status of search */

  detected_csg_cell_list_type *detected_csg_cell_list_ptr; /*!< Detected CSG cells */

  detected_macro_cell_list_type *detected_cell_list_ptr;  /*!< Detected Macro cells for fingerprinting */

  uint8 num_no_fp_rat_list; /*!< Number of RATs with no FP info */

  sys_sys_mode_e_type no_fp_rat_list[SYS_SYS_MODE_MAX]; /*!< List of RATs with no FP info */

} lte_rrc_csg_asf_search_cnfi_s;


/*! @brief Message for PLMN Search Confirmation
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_plmn_search_status_e status; /*!< Status of search */

  detected_macro_cell_list_type *detected_cell_list_ptr;  /*!< Detected Macro cells for fingerprinting */

} lte_rrc_plmn_search_cnfi_s;

/*! @brief PLMN Search Cnfi 
*/
typedef struct
{
  /*!< message header type*/
  msgr_hdr_s msg_hdr;

  /*!< List of higher priority PLMNs that were found on 3G */
  sys_detailed_plmn_list_s_type *available_plmn_list_ptr;

  /*!< Indicates if the BPLMN search is done or not */
  boolean srch_done;

  /*!< Indicates response is for partial plmn results request*/
  boolean partial_srch;

  /*!< Indicates if the highest priority PLMN was found or not */
  boolean hplmn_found;

  /*!< Detected Frequencies */
  detected_frequency_list_type *detected_frequency_list_ptr;

  /*!< Detected CSG cells */
  detected_csg_cell_list_type *detected_csg_cell_list_ptr;

  /*!< Detected Macro cells for fingerprinting */
  detected_macro_cell_list_type *detected_macro_cell_list_ptr;

  /*!< TRUE if the ASF search failed due to lack of FP info with the serving cell,
       FALSE otherwise. If TRUE, srch_done should also be TRUE */
  boolean no_fp_info;
} lte_irat_plmn_srch_cnfi_s;




/*! @brief Enumeration of CNFI id nums
*/
typedef enum
{
  /*Message Handler*/
  RRC_CNFI_CSFB_PARAMETERS_REQUEST_CDMA2000 = 0,
  RRC_CNFI_MEASUREMENT_REPORT = 0x01,
  RRC_CNFI_RRC_RECONFIGURATION_COMPLETE = 0x02,
  RRC_CNFI_RRC_REESTABLISHMENT_COMPLETE = 0x03,
  RRC_CNFI_RRC_CONNECTION_SETUP_COMPLETE = 0x04,
  RRC_CNFI_SECURITY_MODE_COMPLETE = 0x05,
  RRC_CNFI_SECURITY_MODE_FAILURE = 0x06,
  RRC_CNFI_UE_CAPABILITY_INFORMATION = 0x07,
  RRC_CNFI_UL_HANDOVER_PREPARATION_TRANSFER = 0x08,
  RRC_CNFI_UL_INFORMATION_TRANSFER = 0x09,
  RRC_CNFI_COUNTER_CHECK_RESPONSE = 0x0a,
  RRC_CNFI_UE_INFORMATION_RESPONSE = 0x0b,
  RRC_CNFI_PROXIMITY_INDICATION = 0x0c,
  RRC_CNFI_RN_RECONFIGURATION_COMPLETE = 0x0d,
  RRC_CNFI_MBMS_COUNTING_RESPONSE = 0x0e,
  RRC_CNFI_INTER_FREQ_RSTD_MEASUREMENT_INDICATION = 0x0f,
  RRC_CNFI_UE_ASSISTANCE_INFORMATION = 0x10,
  RRC_CNFI_IN_DEVICE_COEX_INDICATION = 0x11,
  RRC_CNFI_MBMS_INTEREST_INDICATION = 0x12,
  /*RRC Controller*/
  RRC_CNFI_MODE_CHANGE = 0x13,
  /*SIB*/
  RRC_CNFI_GET_SIBS = 0x14,
  /*CEP*/
  /*Paging*/
  /*CRP*/
  /*CSP*/
  RRC_CNFI_CSP_CELL_SELECT_ABORT = 0x15,
  /*Config*/
  /*LLC*/
  RRC_CNFI_CFG = 0x16,
  /*Measurement*/
  RRC_CNFI_MEAS_CFG = 0x17,
  /* Inter-RAT */
  RRC_CNFI_IRAT_TO_LTE_RESEL        = 0x18,
  RRC_CNFI_IRAT_TO_LTE_RESEL_ABORT  = 0x19,
  RRC_CNFI_IRAT_FROM_DO_TO_LTE_CAPABILITIES = 0x1a,
  RRC_CNFI_IRAT_FROM_W_TO_LTE_CAPABILITIES = 0x1b,
  RRC_CNFI_IRAT_FROM_G_CS_TO_LTE_CAPABILITIES = 0x1c,
  RRC_CNFI_IRAT_FROM_G_PS_TO_LTE_CAPABILITIES = 0x1d,
  RRC_CNFI_IRAT_TO_LTE_REDIR        = 0x1e,
  RRC_CNFI_IRAT_TO_LTE_REDIR_ABORT  = 0x1f,

  RRC_CNFI_IRAT_TO_LTE_FROM_G_PLMN_SRCH        = 0x20,
  RRC_CNFI_IRAT_TO_LTE_FROM_G_ABORT_PLMN_SRCH  = 0x21,
  RRC_CNFI_IRAT_TO_LTE_FROM_W_PLMN_SRCH        = 0x22,
  RRC_CNFI_IRAT_TO_LTE_FROM_W_ABORT_PLMN_SRCH  = 0x23,
  RRC_CNFI_IRAT_FROM_LTE_TO_G_PLMN_SRCH        = 0x24,
  RRC_CNFI_IRAT_FROM_LTE_TO_W_PLMN_SRCH        = 0x25,

  RRC_CNFI_IRAT_FROM_LTE_ABORT = 0x26,

  RRC_CNFI_ML1_REVOKE = 0x27,

  RRC_CNFI_IRAT_FROM_LTE_TO_W_GET_CGI        = 0x28,
  RRC_CNFI_IRAT_FROM_LTE_TO_W_ABORT_CGI      = 0x29,
  RRC_CNFI_IRAT_FROM_LTE_TO_G_GET_CGI        = 0x2a,
  RRC_CNFI_IRAT_FROM_LTE_TO_G_ABORT_CGI      = 0x2b,
  RRC_CNFI_IRAT_FROM_LTE_TO_DO_GET_CGI       = 0x2c,
  RRC_CNFI_IRAT_FROM_LTE_TO_1X_GET_CGI       = 0x2d,

  RRC_CNFI_IRAT_TO_LTE_PSHO                  = 0x2e,
  RRC_CNFI_IRAT_TO_LTE_PSHO_ABORT            = 0x2f,

  RRC_CNFI_ESMGR_ACT_TMGI                 = 0x30,
  RRC_CNFI_ESMGR_DEACT_TMGI               = 0x31,

  RRC_CNFI_IRAT_FROM_LTE_TO_TDS_PLMN_SRCH   = 0x32,
  RRC_CNFI_IRAT_FROM_LTE_TO_TDS_GET_CGI        = 0x33,
  RRC_CNFI_IRAT_FROM_LTE_TO_TDS_ABORT_CGI      = 0x34,
  RRC_CNFI_IRAT_FROM_TDS_TO_LTE_CAPABILITIES     = 0x35,
  RRC_CNFI_IRAT_TO_LTE_FROM_TDS_PLMN_SRCH        = 0x36,
  RRC_CNFI_IRAT_TO_LTE_FROM_TDS_ABORT_PLMN_SRCH  = 0x37,

  RRC_CNFI_ESMGR_ACT_DEACT_TMGI               = 0x38,
  RRC_CNFI_CSG_ASF_SEARCH  = 0x39,

  RRC_CNFI_PLMN_SEARCH        = 0x3a,
  #ifdef FEATURE_LTE_ELS_ENABLED
  RRC_CNFI_ELS_MEASUREMENT_REPORT = 0x3b,
  RRC_CNFI_ELS_RRC_RECONFIGURATION_COMPLETE = 0x3c,
  RRC_CNFI_ELS_UE_CAPABILITY_INFORMATION = 0x3d,
  RRC_CNFI_ELS_UE_INDICATION = 0x3e,
  #endif

  RRC_CNFI_MAX     /*!< Max value */
}lte_rrc_cnfi_type_e;



/*! @brief Internal confirmations that various RRC procedures send
*/
enum
{
  /*Internal Confirmations */

  LTE_RRC_CNFI_FIRST = MSGR_UMID_BASE(MSGR_LTE_RRC, MSGR_TYPE_CNFI),

  /*Message Handler*/

  MSGR_DEFINE_UMID(LTE, RRC, CNFI, CSFB_PARAMETERS_REQUEST_CDMA2000, 
                   RRC_CNFI_CSFB_PARAMETERS_REQUEST_CDMA2000, 
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, MEASUREMENT_REPORT, 
                   RRC_CNFI_MEASUREMENT_REPORT, 
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, RRC_CONNECTION_RECONFIGURATION_COMPLETE, 
                   RRC_CNFI_RRC_RECONFIGURATION_COMPLETE, 
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, RRC_CONNECTION_REESTABLISHMENT_COMPLETE,
                   RRC_CNFI_RRC_REESTABLISHMENT_COMPLETE, 
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, RRC_CONNECTION_SETUP_COMPLETE,
                    RRC_CNFI_RRC_CONNECTION_SETUP_COMPLETE, 
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, SECURITY_MODE_COMPLETE,
                   RRC_CNFI_SECURITY_MODE_COMPLETE, 
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, SECURITY_MODE_FAILURE, 
                   RRC_CNFI_SECURITY_MODE_FAILURE, 
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, UE_CAPABILITY_INFORMATION,
                   RRC_CNFI_UE_CAPABILITY_INFORMATION, 
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, UL_HANDOVER_PREPARATION_TRANSFER,
                   RRC_CNFI_UL_HANDOVER_PREPARATION_TRANSFER,
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, UL_INFORMATION_TRANSFER,
                   RRC_CNFI_UL_INFORMATION_TRANSFER, 
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, COUNTER_CHECK_RESPONSE,
                   RRC_CNFI_COUNTER_CHECK_RESPONSE, 
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, UE_INFORMATION_RESPONSE,
                   RRC_CNFI_UE_INFORMATION_RESPONSE, 
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, PROXIMITY_INDICATION,
                   RRC_CNFI_PROXIMITY_INDICATION, 
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, RN_RECONFIGURATION_COMPLETE,
                   RRC_CNFI_RN_RECONFIGURATION_COMPLETE, 
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, MBMS_COUNTING_RESPONSE,
                   RRC_CNFI_MBMS_COUNTING_RESPONSE, 
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, INTER_FREQ_RSTD_MEASUREMENT_INDICATION,
                   RRC_CNFI_INTER_FREQ_RSTD_MEASUREMENT_INDICATION, 
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, UE_ASSISTANCE_INFORMATION,
                   RRC_CNFI_UE_ASSISTANCE_INFORMATION, 
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IN_DEVICE_COEX_INDICATION,
                   RRC_CNFI_IN_DEVICE_COEX_INDICATION, 
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, MBMS_INTEREST_INDICATION,
                   RRC_CNFI_MBMS_INTEREST_INDICATION, 
                   lte_rrc_ul_msg_cnfi_s),
  #ifdef FEATURE_LTE_ELS_ENABLED
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, ELS_MEASUREMENT_REPORT, 
                   RRC_CNFI_ELS_MEASUREMENT_REPORT, 
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, ELS_RRC_CONNECTION_RECONFIGURATION_COMPLETE, 
                   RRC_CNFI_ELS_RRC_RECONFIGURATION_COMPLETE, 
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, ELS_UE_CAPABILITY_INFORMATION,
                   RRC_CNFI_ELS_UE_CAPABILITY_INFORMATION, 
                   lte_rrc_ul_msg_cnfi_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, ELS_UE_INDICATION,
                   RRC_CNFI_ELS_UE_INDICATION, 
                   lte_rrc_ul_msg_cnfi_s),
  #endif

  /*RRCController*/
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, MODE_CHANGE,
                   RRC_CNFI_MODE_CHANGE, 
                   lte_rrc_mode_change_cnfi_s),
  /*SIB*/ 
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, GET_SIBS,
                   RRC_CNFI_GET_SIBS, 
                   lte_rrc_get_sibs_cnfi_s),
  /*CEP*/
  /*Paging*/
  /*CRP*/
  /*CSP*/
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, CSP_CELL_SELECT_ABORT,
                   RRC_CNFI_CSP_CELL_SELECT_ABORT,
                   lte_rrc_csp_cell_select_abort_cnfi_s),

  /*Config*/

  /*LLC*/
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, CFG, 
                   RRC_CNFI_CFG, lte_rrc_cfg_cnfi_s),

  /*Measurement*/
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, MEAS_CFG, 
                   RRC_CNFI_MEAS_CFG, lte_rrc_meas_cfg_cnfi_s),

  /* Inter-RAT :  G/W to LTE Reselection */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_TO_LTE_RESEL, RRC_CNFI_IRAT_TO_LTE_RESEL, 
                   lte_rrc_irat_to_lte_resel_cnfi_s), 

  /* Inter-RAT :  G/W to LTE Reselection Abort */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_TO_LTE_RESEL_ABORT, RRC_CNFI_IRAT_TO_LTE_RESEL_ABORT, 
                   lte_rrc_irat_to_lte_resel_abort_cnfi_s), 

  /* Inter-RAT : DO to LTE CAPABILITIES CNF (Internal) */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_FROM_DO_TO_LTE_CAPABILITIES, 
                   RRC_CNFI_IRAT_FROM_DO_TO_LTE_CAPABILITIES, 
                   lte_rrc_irat_from_do_to_lte_capabilities_cnfi_s), 

  /* Inter-RAT : W to LTE CAPABILITIES CNF (Internal) */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_FROM_W_TO_LTE_CAPABILITIES, 
                   RRC_CNFI_IRAT_FROM_W_TO_LTE_CAPABILITIES, 
                   lte_rrc_irat_from_w_to_lte_capabilities_cnfi_s),

  /* Inter-RAT : TDS to LTE CAPABILITIES CNF (Internal) */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_FROM_TDS_TO_LTE_CAPABILITIES, 
                   RRC_CNFI_IRAT_FROM_TDS_TO_LTE_CAPABILITIES, 
                   lte_rrc_irat_from_tds_to_lte_capabilities_cnfi_s),

  /* Inter-RAT : GERAN CS to LTE CAPABILITIES CNF (Internal) */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_FROM_G_CS_TO_LTE_CAPABILITIES, 
                   RRC_CNFI_IRAT_FROM_G_CS_TO_LTE_CAPABILITIES, 
                   lte_rrc_irat_from_g_cs_to_lte_capabilities_cnfi_s), 

  /* Inter-RAT : GERAN PS to LTE CAPABILITIES CNF (Internal) */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_FROM_G_PS_TO_LTE_CAPABILITIES, 
                   RRC_CNFI_IRAT_FROM_G_PS_TO_LTE_CAPABILITIES, 
                   lte_rrc_irat_from_g_ps_to_lte_capabilities_cnfi_s), 

  /* Inter-RAT :  G/W to LTE Redirection */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_TO_LTE_REDIR, RRC_CNFI_IRAT_TO_LTE_REDIR, 
                   lte_rrc_irat_to_lte_redir_cnfi_s), 

  /* Inter-RAT :  G/W to LTE Redirection Abort */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_TO_LTE_REDIR_ABORT, RRC_CNFI_IRAT_TO_LTE_REDIR_ABORT, 
                   lte_rrc_irat_to_lte_redir_abort_cnfi_s), 

  /* Inter-RAT :  G to LTE PLMN Search CNF */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_TO_LTE_FROM_G_PLMN_SRCH, RRC_CNFI_IRAT_TO_LTE_FROM_G_PLMN_SRCH,
                   lte_irat_plmn_srch_cnfi_s), 

  /* Inter-RAT :  G to LTE PLMN Search Abort CNF */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_TO_LTE_FROM_G_ABORT_PLMN_SRCH, RRC_CNFI_IRAT_TO_LTE_FROM_G_ABORT_PLMN_SRCH,
                   lte_irat_abort_plmn_srch_rsp_s), 

  /* Inter-RAT :  W to LTE PLMN Search CNF */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_TO_LTE_FROM_W_PLMN_SRCH, RRC_CNFI_IRAT_TO_LTE_FROM_W_PLMN_SRCH,
                   lte_irat_plmn_srch_cnfi_s), 

  /* Inter-RAT :  W to LTE PLMN Search Abort CNF */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_TO_LTE_FROM_W_ABORT_PLMN_SRCH, RRC_CNFI_IRAT_TO_LTE_FROM_W_ABORT_PLMN_SRCH,
                   lte_irat_abort_plmn_srch_rsp_s),

  /* Inter-RAT :  TDS to LTE PLMN Search CNF */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_TO_LTE_FROM_TDS_PLMN_SRCH, RRC_CNFI_IRAT_TO_LTE_FROM_TDS_PLMN_SRCH,
                   lte_irat_plmn_srch_cnfi_s), 

  /* Inter-RAT :  TDS to LTE PLMN Search Abort CNF */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_TO_LTE_FROM_TDS_ABORT_PLMN_SRCH, RRC_CNFI_IRAT_TO_LTE_FROM_TDS_ABORT_PLMN_SRCH,
                   lte_irat_abort_plmn_srch_rsp_s),

  /* Inter-RAT :  LTE to G PLMN Search CNF */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_FROM_LTE_TO_G_PLMN_SRCH, RRC_CNFI_IRAT_FROM_LTE_TO_G_PLMN_SRCH,
                   lte_irat_plmn_srch_cnfi_s), 

  /* Inter-RAT :  LTE from W PLMN Search CNF */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_FROM_LTE_TO_W_PLMN_SRCH, RRC_CNFI_IRAT_FROM_LTE_TO_W_PLMN_SRCH,
                   lte_irat_plmn_srch_cnfi_s),

  /* Inter-RAT :  LTE from TDS PLMN Search CNF */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_FROM_LTE_TO_TDS_PLMN_SRCH, RRC_CNFI_IRAT_FROM_LTE_TO_TDS_PLMN_SRCH,
                   lte_irat_plmn_srch_cnfi_s),

  /* Inter-RAT :  From LTE PLMN Search Abort CNF */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_FROM_LTE_ABORT, RRC_CNFI_IRAT_FROM_LTE_ABORT,
                   lte_rrc_irat_from_lte_abort_cnfi_s),

  MSGR_DEFINE_UMID(LTE, RRC, CNFI, ML1_REVOKE, RRC_CNFI_ML1_REVOKE,
                   lte_rrc_ml1_revoke_cnfi_s),

  /* Inter-RAT :  From LTE to W get CGI CNFI */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_FROM_LTE_TO_W_GET_CGI, RRC_CNFI_IRAT_FROM_LTE_TO_W_GET_CGI,
                   lte_rrc_irat_from_lte_to_w_get_cgi_cnfi_s),

  /* Inter-RAT :  From LTE to TDS get CGI CNFI */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_FROM_LTE_TO_TDS_GET_CGI, RRC_CNFI_IRAT_FROM_LTE_TO_TDS_GET_CGI,
                   lte_rrc_irat_from_lte_to_tds_get_cgi_cnfi_s),

  /* Inter-RAT :  From LTE to G get CGI CNFI */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_FROM_LTE_TO_G_GET_CGI, RRC_CNFI_IRAT_FROM_LTE_TO_G_GET_CGI,
                   lte_rrc_irat_from_lte_to_g_get_cgi_cnfi_s), 

  /* Inter-RAT :  From LTE to G abort CGI CNFI */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_FROM_LTE_TO_G_ABORT_CGI, RRC_CNFI_IRAT_FROM_LTE_TO_G_ABORT_CGI,
                   lte_rrc_irat_from_lte_to_g_abort_cgi_cnfi_s), 

  /* Inter-RAT :  From LTE to DO get CGI CNFI */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_FROM_LTE_TO_DO_GET_CGI, RRC_CNFI_IRAT_FROM_LTE_TO_DO_GET_CGI,
                   lte_rrc_irat_from_lte_to_do_get_cgi_cnfi_s), 

  /* Inter-RAT :  From LTE to 1X get CGI CNFI */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_FROM_LTE_TO_1X_GET_CGI, RRC_CNFI_IRAT_FROM_LTE_TO_1X_GET_CGI,
                   lte_rrc_irat_from_lte_to_1x_get_cgi_cnfi_s), 

  /* Inter-RAT :  to LTE PSHO */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_TO_LTE_PSHO, RRC_CNFI_IRAT_TO_LTE_PSHO, 
                   lte_rrc_irat_to_lte_psho_cnfi_s), 

  /* Inter-RAT :  to LTE PSHO Abort */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, IRAT_TO_LTE_PSHO_ABORT, RRC_CNFI_IRAT_TO_LTE_PSHO_ABORT, 
                   lte_rrc_irat_to_lte_psho_abort_cnfi_s), 


  /* eMBMS */
  MSGR_DEFINE_UMID(LTE, RRC, CNFI, ESMGR_ACT_TMGI, RRC_CNFI_ESMGR_ACT_TMGI, 
                   lte_rrc_esmgr_act_tmgi_cnfi_s), 

  MSGR_DEFINE_UMID(LTE, RRC, CNFI, ESMGR_DEACT_TMGI, RRC_CNFI_ESMGR_DEACT_TMGI, 
                   lte_rrc_esmgr_deact_tmgi_cnfi_s),

  MSGR_DEFINE_UMID(LTE, RRC, CNFI, CSG_ASF_SEARCH, RRC_CNFI_CSG_ASF_SEARCH, 
                   lte_rrc_csg_asf_search_cnfi_s),

  MSGR_DEFINE_UMID(LTE, RRC, CNFI, ESMGR_ACT_DEACT_TMGI, RRC_CNFI_ESMGR_ACT_DEACT_TMGI, 
                   lte_rrc_esmgr_act_deact_tmgi_cnfi_s),

  MSGR_DEFINE_UMID(LTE, RRC, CNFI, PLMN_SEARCH, RRC_CNFI_PLMN_SEARCH, 
                   lte_rrc_plmn_search_cnfi_s),

  LTE_RRC_CNFI_MAX ,
  LTE_RRC_CNFI_LAST = LTE_RRC_CNFI_MAX - 1
};

/*! @brief Data structure for SMC failure indication;
*/  
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
}lte_rrc_smc_failure_indi_s;

/*! @brief Data structure for Inter-RAT From LTE Redirection Failed indication;
*/  
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
}lte_rrc_irat_from_lte_redir_failed_indi_s;

/*! @brief Data structure for Inter-RAT From LTE Reselection Success indication;
*/  
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  /*Carries W cell information which is passed in resel request*/
  source_cell_info_type       w_cell_info;

  /*Carries L cell  information*/
  source_cell_info_type       lte_cell_info;
}lte_rrc_irat_from_lte_resel_success_indi_s;

/*! @brief Data structure for Inter-RAT From LTE PLMN Srch Resume Failed indication;
*/  
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
}lte_rrc_irat_from_lte_plmn_srch_resume_failed_indi_s;


/*! @brief Data structure for Inter-RAT From LTE Reselection Failed indication;
*/  
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_rrc_irat_from_lte_failure_cause_type_e fail_cause; /*!< Failure reason */
  boolean     override_bar_time; /*!< If cell bar time needs to be used from Target RAT */
  uint32      bar_time; /*!< Value in sec of bar time if it is to be overridden */
}lte_rrc_irat_from_lte_resel_failed_indi_s;

/*! @brief Data structure for PLMN Search suspend ind
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} lte_irat_plmn_srch_suspend_indi_s;

/*! @brief Data structure for Stopped indication;
*/  
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} lte_rrc_stopped_indi_s;

#ifdef FEATURE_LTE_ELS_ENABLED
/*! @brief Data structure for Autonomous ELS UE Cap info indication;
*/  
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} lte_rrc_auto_els_ue_cap_info_indi_s;
#endif


/*! @brief Data structure for unsolicited SIB indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_cell_id_s cell_id; /*!< Cell id */
  uint8 encoded_si[LTE_RRC_MAX_UNSOLICITED_SI_LENGTH]; /*!< ASN1 Encoded SI */
  uint32 encoded_si_size; /*!< Size of ASN1 encoded SI */
} lte_rrc_unsolicited_sib_indi_s;

/*! @brief Data structure for SIB Updated indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_sib_bitmask_t sibs_changed_bitmask; /*!< Bitmask indicating sibs changed */
  lte_rrc_sib_bitmask_t sibs_scheduled_bitmask; /*!< Bitmask indicating sibs scheduled; used by meas */
  lte_rrc_cell_id_s cell_id;
} lte_rrc_sib_updated_indi_s;

/*! @brief Data structure for SIB Updated indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_sib_bitmask_t sibs_not_read_bitmask; /*!< Bitmask indicating sibs required but not read */
  lte_rrc_sib_bitmask_t sibs_scheduled_bitmask; /*!< Bitmask indicating sibs scheduled*/
  lte_rrc_cell_id_s cell_id;
} lte_rrc_sib_not_rcvd_indi_s;

/*! @brief Data structure for SIB read error indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_rrc_cell_id_s cell_id;
  lte_rrc_sib_read_error_cause_e cause; /*!< Cause of the error */
  lte_rrc_sib_bitmask_t sibs_rcvd_curr_bitmask; /*!< SIBs received in curr mod prd */
  lte_rrc_sib_bitmask_t sibs_not_rcvd_curr_bitmask; /*!< SIBs not received in curr mod prd,
                                                      to be looked only for 
                                                      LTE_RRC_SIB_READ_ERROR_CAUSE_SIB_TIMEOUT cause */
  boolean csg_indication; /*!< csg_Indication to be used for cell/freq barring */
  boolean intra_freq_resel_allowed; /*!< intraFreqReselection to be used for cell/freq barring */
} lte_rrc_sib_read_error_indi_s;

/*! @brief Data structure for Connection Establishment Started indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} lte_rrc_conn_establishment_started_indi_s;

/*! @brief Data structure for Connection Establishment Failure indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} lte_rrc_conn_establishment_failure_indi_s;

/*! @brief Data structure for Stop Cell Reselection indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} lte_rrc_stop_cell_resel_indi_s;
 /*! @brief Data structure for Stop Cell Reselection indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  boolean status;
} lte_rrc_proceed_with_resel_indi_s;

/*! @brief Data structure for Connection Release Started indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} lte_rrc_conn_rel_started_indi_s;

/*! @brief Enumeration of Source Tech. RRC internal DS
*/
typedef enum
{
  LTE_RRC_TRM_PRIORITY_REQ_FROM_NAS,               /*!< 0 */
  LTE_RRC_TRM_PRIORITY_REQ_FROM_DS,                /*!< 1 */
  LTE_RRC_TRM_PRIORITY_REQ_FROM_CEP,               /*!< 2 */
  LTE_RRC_TRM_PRIORITY_REQ_FROM_CRP,               /*!< 3 */
  LTE_RRC_TRM_PRIORITY_REQ_FROM_CSP,               /*!< 4 */
  LTE_RRC_TRM_PRIORITY_REQ_FROM_CONFIG,            /*!< 5 */
  LTE_RRC_TRM_PRIORITY_REQ_FROM_IRAT_FROM_G_MGR,   /*!< 6 */  
  LTE_RRC_TRM_PRIORITY_REQ_FROM_IRAT_FROM_W_MGR,   /*!< 7  */ 
  LTE_RRC_TRM_PRIORITY_REQ_FROM_IRAT_FROM_TDS_MGR, /*!< 8 */
  LTE_RRC_TRM_PRIORITY_REQ_FROM_IRAT_FROM_1X_MGR,  /*!< 9 */
  LTE_RRC_TRM_PRIORITY_REQ_FROM_CM,                /*!< 10 */ 
  LTE_RRC_TRM_PRIORITY_REQ_FROM_IRAT_FROM_DO_MGR,  /*!< 11 */
  LTE_RRC_TRM_PRIORITY_REQ_MAX                     /*!< 12 */    
} lte_rrc_trm_client_id_e;

/*! @brief Data structure for TRM Priority change indication
*/
typedef struct
{
  msgr_hdr_s               msg_hdr;         /*!< Message router header */
  lte_rrc_trm_client_id_e  trm_client;      /*!< Client requesting for TRM priority change */
  lte_trm_priority_e       trm_pri;         /*!< TRM priority */
  boolean                  send_pri_to_ml1; /*!< TRUE - internal bookkeeping | FALSE - internal + send down to ML1 */
} lte_rrc_trm_priority_change_indi_s;

/*! @brief Data structure for Stop Connection Released indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_conn_rel_reason_e rel_reason; /*!< Release reason */
  boolean cell_barring_is_reqd; /*!< Is cell barring required by CSP */
  lte_phy_cell_id_t phy_cell_id;  /*!< Physical cell id to be barred */
  lte_earfcn_t earfcn;  /*!< Frequency of cell to be barred */
  boolean redir_info_is_present; /*!< Is redirection info present? */
  lte_rrc_redir_info_s redir_info; /*!< Redirection info
                                        - only valid if rel_reason is 
                                        LTE_RRC_CONN_REL_REDIRECTED*/
  lte_rrc_ml1_status_type_e ml1_status; /*!< ml1 state */
  boolean is_acq_needed;  
  lte_cphy_conn_release_acq_cell_info_s acq_cell_info;
} lte_rrc_conn_released_indi_s;


/*! @brief Data structure for Not Camped indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} lte_rrc_not_camped_indi_s;


/*! @brief Data structure for Not Camped indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_rrc_cell_id_s camped_cell;  /*!< Cell Id of the new serving cell */ 
  lte_rrc_camped_reason_e reason; /*!< Camped reason */
} lte_rrc_camped_indi_s;

/*! @brief Data structure for each frequency entry
*/
typedef struct
{
  /*! DL Center frequency */
  lte_earfcn_t dl_earfcn;
	/*! System Bandwidth */
	lte_bandwidth_e bandwidth;
} lte_earfcn_bw;

/*! @brief Data structure for connmode interfreq neighbors indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  uint8 num_frequencies;
  lte_earfcn_bw interFreqNeighborList[LTE_RRC_MAX_INTFREQ_NEIGHBORS];
} lte_rrc_interfreq_list_update_indi_s;


/*! @brief Data structure for Connected indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} lte_rrc_connected_indi_s;


/*! @brief Data structure for Cell Reselction Started indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_rrc_cell_id_s camped_cell;  /*!< Cell Id of the current serving cell */ 
  lte_cphy_resel_target_cell_info_s target_cell_info;  /*!< Target cell info */ 
  source_cell_info_type serving_cell_info; /*!< Serving cell info for ASF fingerprinting */
} lte_rrc_cell_resel_started_indi_s;

/*! @brief Data structure for Cell Reselction Canceled indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} lte_rrc_cell_resel_canceled_indi_s;

/*! @brief Data structure for Handover Started indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

} lte_rrc_handover_started_indi_s;

/*! @brief Data structure for Mobility from EUTRA Started indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} lte_rrc_irat_from_lte_mobility_started_indi_s;

/*! @brief Data structure for Connected mode failure indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_rrc_conn_mode_failure_cause_e conn_mode_failure_cause; /*!< Conn Mode Failure cause */
  lte_rrc_ho_failure_cause_e ho_failure_cause; /*!< HO Failure cause, applicable only
                                                    when conn_mode_failure_cause is HO */
  lte_rrc_cell_info_s ho_source_cell_info;   /*! HO Source cell Info */                                                
} lte_rrc_conn_mode_failure_indi_s;

/*! @brief Data structure for CRE Started indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} lte_rrc_cre_started_indi_s;

/*! @brief Data structure for CRE Completed indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} lte_rrc_cre_completed_indi_s;

/*! @brief Data structure for Initiate Connection Release indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_rrc_conn_rel_reason_e rel_reason; /*!< Reason */
} lte_rrc_initiate_conn_rel_indi_s;


/*! @brief Data structure for priority list passed from RRC Conn release or IRAT mgr
*/
typedef struct
{
  msgr_hdr_s msg_hdr;                          /*!< Message router header */
  uint64 t_320_ms;                             /*!< T320 value in milliseconds */
  boolean t_320_is_present;                    /*!< TRUE if Timer T320 has been provided, FALSE otherwise */
  lte_irat_dedicated_priority_list_s freq_list;      /*!< list of frequencies and their priority */
	sys_plmn_id_s_type plmn;                     /*!< PLMN ID associated with dedicated priorities */
  lte_rrc_proc_e source_proc;                  /*!< Source procedure that sends this indication */
 
} lte_rrc_ded_priority_list_indi_s;             

/*! @brief Data structure for serving cell changed indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_rrc_cell_id_s serving_cell_id;  /*!< serving cell id */

  /*! Bandwidth of the target cell */
  lte_bandwidth_e cell_bandwidth;

  /*! Number of Tx antennas */
  uint8 num_tx_antennas;

} lte_rrc_serving_cell_changed_indi_s;

/*! @brief Data structure for serving cell changed indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  
  boolean mbsfn_cfg_valid;  /*!< MBSFN cfg status */

} lte_rrc_llc_mbsfn_cfg_chng_indi_s;


/*! @brief Data structure for reselection completed indication passed from IRAT Mgr to CSP
*/
typedef struct
{
  msgr_hdr_s msg_hdr;       /*!< Message router header */
}lte_rrc_irat_to_lte_resel_completed_indi_s;

/*! @brief Data structure for PSHO successful indication sent by IRAT Mgr on getting a success from LLC/Config.  This triggers RRC to move to connected.
*/
typedef struct
{
  msgr_hdr_s msg_hdr;       /*!< Message router header */

}lte_rrc_irat_to_lte_psho_success_indi_s;

/*! @brief Data structure for PSHO completed indication sent by IRAT Mgr on getting successful Activation Response from NAS
*/
typedef struct
{
  msgr_hdr_s msg_hdr;       /*!< Message router header */
}lte_rrc_irat_to_lte_psho_completed_indi_s;

/*! @brief Data structure for dlm processed indication passed to MH
*/
typedef struct
{
  msgr_hdr_s msg_hdr;       /*!< Message router header */
}lte_rrc_dlm_processed_indi_s;

/*! @brief Data structure for passing Wait Time until which cell resel to UTRA 
    need to be disabled.The range of wait time is 0..15 seconds.  For wait time of 0, 
    UE behaviour is not specified.
*/
typedef struct
{
  msgr_hdr_s msg_hdr;                          /*!< Message router header */
  uint64 t_wait_time_ms;                       /*!< Value in milliseconds */
}lte_rrc_irat_utra_resel_wait_time_indi_s;

/*! @brief Data structure for passing Wait Time until which cell resel to UTRA  TDD
    need to be disabled.The range of wait time is 0..15 seconds.  For wait time of 0, 
    UE behaviour is not specified.
*/
typedef struct
{
  msgr_hdr_s msg_hdr;                          /*!< Message router header */
  uint64 t_wait_time_ms;                       /*!< Value in milliseconds */
}lte_rrc_irat_utra_tdd_resel_wait_time_indi_s;

/*! @brief Data structure for redirection completed indication passed from IRAT Mgr to CSP
*/
typedef struct
{
  msgr_hdr_s msg_hdr;      /*!< Message router header */
}lte_rrc_irat_to_lte_redir_completed_indi_s;

/*! @brief Data structure for sending Mobility from EUTRA to G message to IRAT Mgr */
typedef struct
{
  msgr_hdr_s msg_hdr;      /*!< Message router header */
  void *dl_msg_ptr; /*!< Pointer to decoded DL msg containing the mobility from
                      EUTRA command for CCO or handover */
}lte_rrc_irat_from_lte_to_g_mobility_indi_s;

//CEP opt
/*! @brief Data structure for sending RRC OOS Indi due to CEP opt/MIB change */
typedef struct
{
  msgr_hdr_s msg_hdr;      /*!< Message router header */
  lte_rrc_oos_ind_cause_e oos_ind_cause;      /*!< Message router header */
}lte_rrc_oos_indi_s;

/*! @brief Data structure for sending Mobility from EUTRA to 1x message to IRAT Mgr */
typedef struct
{
  msgr_hdr_s msg_hdr;      /*!< Message router header */
  void *dl_msg_ptr; /*!< Pointer to decoded DL msg containing the mobility from
                      EUTRA command for handover */

}lte_rrc_irat_from_lte_to_1x_mobility_indi_s;

/*! @brief Data structure for sending Mobility from EUTRA to HDR message to IRAT Mgr */
typedef struct
{
  msgr_hdr_s msg_hdr;      /*!< Message router header */
  void *dl_msg_ptr; /*!< Pointer to decoded DL msg containing the mobility from
                      EUTRA command for handover */

}lte_rrc_irat_from_lte_to_hdr_mobility_indi_s;

/*! @brief Data structure for sending Mobility from EUTRA to W message to IRAT Mgr */
typedef struct
{
  msgr_hdr_s msg_hdr;      /*!< Message router header */
  void *dl_msg_ptr; /*!< Pointer to decoded DL msg containing the mobility from
                      EUTRA command for handover */
}lte_rrc_irat_from_lte_to_w_mobility_indi_s;

/*! @brief Data structure for sending Mobility from EUTRA to TDS message to IRAT Mgr */
typedef struct
{
  msgr_hdr_s msg_hdr;      /*!< Message router header */
  void *dl_msg_ptr; /*!< Pointer to decoded DL msg containing the mobility from
                      EUTRA command for handover */
}lte_rrc_irat_from_lte_to_tds_mobility_indi_s;

/*! @brief Data structure for sending tracking area info after camping on new cell */
typedef struct
{
  msgr_hdr_s msg_hdr;      /*!< Message router header */
  lte_rrc_cell_info_s cell_info; /*!< Cell info */
  uint16 tac; /*!< Tracking area code of the cell */
  lte_rrc_plmn_s plmn; /*!< PLMN area code of the cell */
}lte_rrc_new_cell_info_indi_s;

/*! @brief Data structure for SRB2 resumed indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} lte_rrc_srb2_resumed_indi_s;

/*! @brief Data structure for sending Pre-Registration info for HRPD to IRAT Mgr */
typedef struct
{
  msgr_hdr_s msg_hdr;      /*!< Message router header */

  void *dl_msg_ptr; /*!< Pointer to decoded DL msg containing DL DCCH message for 
                          RRC Reconfiguration */

  boolean prereg_with_handover; /*!< If PreRegInfo came with the handover msg */

} lte_rrc_irat_hrpd_pre_reg_info_indi_s;

/**/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_irat_rat_type_e                       source_rat; /*!< RAT Type */

  /*! T304 value (in ms) contained in RRCConnectionReconfiguration */
  uint32                                    t_304_value_ms;

  /*! NAS related info needed for camping on LTE */
  lte_rrc_nas_info_s                        nas_info;

 /*!< Serving cell info for ASF fingerprinting */
  source_cell_info_type                     serving_cell_info;

 /*! UTRA cell info for too early handover */
  utra_serving_cell_info_type                 utra_cell_info;

} lte_rrc_irat_to_lte_psho_started_indi_s;

/*! @brief Data structure for key generation for IRAT to LTE PSHO indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} lte_rrc_irat_to_lte_psho_key_gen_indi_s;


/*! @brief Data Structure for deactivation indication.  contains list of 
   TMGIs deactivated and the reason.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_embms_deact_reason_e reason; /*!< eMBMS deactivation reason */

  uint8 num_tmgi_deactivated; /*!< Number of deactivated tmgi in deact_info_list */

  lte_rrc_embms_bearer_info_s deact_info_list[LTE_EMBMS_MAX_ACTIVE_SESSIONS]; /*!< List of deact_info */

} lte_rrc_emp_deact_tmgi_indi_s;

/*! @brief Data Structure for OOS warning indication. Contains list of 
   TMGIs impacted by OOS and the cause.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_emp_warn_reason_e reason; /*!< warning reason */

  uint8 num_tmgi_affected; /*!< Number of TMGIs affected by this warning indi */

  lte_rrc_embms_bearer_info_s affected_info_list[LTE_EMBMS_MAX_ACTIVE_SESSIONS]; 
                                                /*! List of affected TMGIs/sessions */

} lte_rrc_emp_oos_warn_indi_s;

/*! @brief Data Structure for pended service indication. Contains service status.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_svc_status_e svc_status; /*!< Service status */
} lte_rrc_emp_pended_service_indi_s;

/*! RRC eMBMS bearer action type enumeration - log packet only */
typedef enum
{
  LTE_RRC_EMP_EMBMS_BEARER_ACTION_ACTIVATE,
  LTE_RRC_EMP_EMBMS_BEARER_ACTION_DEACTIVATE,
  LTE_RRC_EMP_EMBMS_BEARER_ACTION_MODIFY,
  LTE_RRC_EMP_EMBMS_BEARER_ACTION_MAX
}lte_rrc_emp_embms_bearer_action_e;

/*!< Struct that stores bearer info - log packet only */
typedef struct
{
  lte_mbsfn_area_id_t mbsfn_area_id; 
  lte_rb_id_t mrb_id;
  lte_pmch_id_t pmch_id;
  lte_lc_id_t lc_id;
  lte_rrc_embms_session_info_s session_info;
}lte_rrc_emp_embms_bearer_info_s;

/*!< Struct that stores active services - log packet only */
typedef struct
{
  uint8 num_active_svc;
  lte_rrc_emp_embms_bearer_info_s active_svc_list[LTE_MAX_ACTIVE_MRB];
}lte_rrc_emp_embms_active_service_s;

/*!< Struct that stores modified services - log packet only */
typedef struct
{
  lte_rrc_emp_embms_bearer_action_e action;
  uint8 num_modified_svc;
  lte_rrc_embms_session_info_s modified_svc_list[LTE_MAX_ACTIVE_MRB];
}lte_rrc_emp_embms_modified_service_s;

/*!< Struct that stores affected session info because of OOS - log packet only */
typedef struct
{
  lte_rb_id_t mrb_id;
  lte_rrc_embms_session_info_s session_info;
}lte_rrc_emp_embms_affected_session_info_list_s;

/*!< Struct that stores affected MBSFN area ID because of OOS - log packet only */
typedef struct
{
  lte_mbsfn_area_id_t mbsfn_area_id;
}lte_rrc_emp_embms_affected_mbsfn_areas_s;

/*!< Struct that stores eMBMS OOS warning indication info - log packet only */
typedef struct
{
  lte_rrc_emp_warn_reason_e reason;
  uint8 num_affected_sessions;
  lte_rrc_emp_embms_affected_session_info_list_s affected_info_list[LTE_MAX_ACTIVE_MRB];
  uint8 num_affected_areas;
  lte_rrc_emp_embms_affected_mbsfn_areas_s affected_areas[LTE_EMBMS_MAX_MBSFN_AREA];
}lte_rrc_emp_embms_oos_warning_ind_info_s;


/*! @brief Data Structure for CSG Config Ind.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_cphy_meas_csg_info_s w_csg_info; /*!< W CSG Cells Info */

  lte_cphy_meas_lte_csg_info_s l_csg_info; /*!< L CSG Cells Info */
} lte_rrc_csg_cfg_indi_s;

/*! @brief Data structure for RAT priority update indication;
*/  
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */  
} lte_rrc_rat_priority_update_indi_s;

/*! @brief Structure to hold LTE Deprioritization info
*/
typedef struct
{
  /*! Flag to indicate if T325 is running */
  boolean t325_running;

  /*! Flag to indicate if all LTE freqs are deprioritized */
  boolean depri_all_freq;

  /*! Number of LTE freqs that are deprioritized. 
  Currently used for limited mode IMS related deprioritization */
  uint8 num_depri_freq;

  /*! Set of LTE freqs that are deprioritized */
  lte_earfcn_t depri_freqs[LTE_RRC_MAX_NUM_DEPRI_FREQS];

} lte_rrc_lte_depri_info_s;

/*! @brief Structure to hold OTA Deprioritization info and 
     Deprioritization RPLMN and EPLMN info
*/
typedef struct
{
  /*! Deprio PLMN list*/
  lte_rrc_plmn_list_s depri_plmn_list;

  /*! LTE Deprioritization info */
  lte_rrc_lte_depri_info_s lte_depri_info;

} lte_rrc_lte_ota_depri_info_s;

/*! @brief Data Structure for Depri Freq Ind.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_depri_type_e lte_depri_type; /*!< LTE deprioritization type */
  
  lte_rrc_depri_cause_e cause;/*! deprioritization cause */

  lte_depri_timer_e depri_tmr_val; /*!< LTE deprioritization timer value */

  /*! Number of LTE freqs that are deprioritized */
  uint8 num_depri_freq;

  /*! Set of LTE freqs that are deprioritized */
  lte_earfcn_t depri_freqs[LTE_RRC_MAX_NUM_DEPRI_FREQS];

} lte_rrc_depri_freq_indi_s;

/*! @brief Data Structure for clear Depri Freq Ind.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  
  lte_rrc_depri_cause_e cause;/*! deprioritization cause */
} lte_rrc_clear_depri_freq_indi_s;


/*! @brief Data Structure for LTE Depri Freq Ind.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_lte_depri_info_s depri_freq_info; /*!< LTE deprioritization info */

} lte_rrc_irat_lte_depri_indi_s;

/*! @brief Data Structure for Resel priority change indi for eMBMS.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

} lte_rrc_esmgr_resel_prio_change_indi_s;

/*! @brief Data Structure for Proximity Config Ind.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  /*!< TRUE, if Proximity Indi for EUTRA configured
            FALSE, if Proximity Indi for EUTRA disabled */
  boolean eutra_configured;

  /*!< TRUE, if Proximity Indi for UTRA configured
            FALSE, if Proximity Indi for UTRA disabled */
  boolean utra_configured;
  
} lte_rrc_prox_cfg_indi_s;

/*! @brief Data Structure for MBSFN SNR Ind.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  /*! MBSFN SNR info */
  lte_rrc_mbsfn_snr_info_s mbsfn_snr_info;
  
} lte_rrc_mbsfn_snr_indi_s;

/* Type for Global Cell id */
typedef uint16 lte_rrc_tac_t;

/*! @brief Data structure for logged mdt duration
*/
typedef enum {
   LTE_RRC_MDT_DURATION_10_MIN = 600000,
   LTE_RRC_MDT_DURATION_20_MIN = 1200000,
   LTE_RRC_MDT_DURATION_40_MIN = 2400000,
   LTE_RRC_MDT_DURATION_60_MIN = 3600000,
   LTE_RRC_MDT_DURATION_90_MIN = 5400000,
   LTE_RRC_MDT_DURATION_120_MIN = 7200000,
   LTE_RRC_MDT_DURATION_SPARE  
} lte_rrc_mdt_duration_e;

/*! @brief enum for area configuration type
*/
typedef enum {
  LTE_RRC_MDT_AREA_CFG_TYPE_NONE = 0,
  LTE_RRC_MDT_AREA_CFG_TYPE_CGI,
  LTE_RRC_MDT_AREA_CFG_TYPE_TAC,
} lte_rrc_mdt_area_cfg_type_e;

/*! @brief enum for requested Log Meas report type
*/
typedef enum {
  LTE_RRC_MDT_RPT_NONE = 0,
  LTE_RRC_MDT_PPREBUILD_UL,
  LTE_RRC_MDT_UL_ALL_RPT,
  LTE_RRC_MDT_UL_FIRST_RPT,
} lte_rrc_mdt_rpt_type_e;

/*! @brief Data structure for MDT CGI
*/
typedef struct
{
  /*!< Selected PLMN - req PLMN or its equivalent */
  lte_rrc_plmn_s plmn;

  /*!< Global Cell Id */
  lte_rrc_global_cell_id_t cell_identity;
  
} lte_rrc_mdt_cgi_r10_s;

/*! @brief Data structure for MDT CGI list
*/
typedef struct
{
  /*!< Number of cell global id */
  uint8 num_cgi;

  /*!< Cell Global id */
  lte_rrc_mdt_cgi_r10_s cgi[LTE_RRC_MDT_AREA_CFG_MAX_NUM_CGI];
  
} lte_rrc_mdt_cgi_list_r10_s;

/*! @brief Data structure for MDT TAC list
*/
typedef struct
{
  /*!< Number of TAC */
  uint8 num_tac;

  /*!< TAC List */
  lte_rrc_tac_t tac[LTE_RRC_MDT_AREA_CFG_MAX_NUM_TAC];
  
} lte_rrc_mdt_tac_list_r10_s;

/*! @brief Data structure for area cfg info union
*/
typedef union
{
  /*!< Cell Global id List */
  lte_rrc_mdt_cgi_list_r10_s cgi_list;

  /*!< TAC List */
  lte_rrc_mdt_tac_list_r10_s tac_list;
} lte_rrc_mdt_area_cfg_info_r10_u;


/*! @brief Data structure for area cfg
*/
typedef struct
{
  /*!< Cell Global id List */
  lte_rrc_mdt_area_cfg_type_e area_cfg_type;

  /*!< area cfg union */
  lte_rrc_mdt_area_cfg_info_r10_u area_cfg_info;
  
} lte_rrc_mdt_area_cfg_r10_s;

/*! @brief Data structure for Trace reference
*/
typedef struct
{
  /*!< Selected PLMN - req PLMN or its equivalent */
  lte_rrc_plmn_s selected_plmn;

  /*!< Trace ID R10 */
  uint8 trace_id[3];
  
}lte_rrc_mdt_trace_reference_r10_s;

/*! @brief Data structure for Logged MDT Cfg
*/
typedef struct
{
  /*!< TraceReference R-10 */
  lte_rrc_mdt_trace_reference_r10_s trace_ref;
  
  /*!< Parameter Trace Recording Session Reference: See TS 32.422 [58] */
  uint8  trace_session_ref[2];

  /*!< Parameter Trace Collection Entity Id: See TS 32.422 [58] */
  uint8  tce_id_r10;

  /*!< Indicates the absolute time in the current cell */
  lte_rrc_osys_AbsoluteTimeInfo_r10 absoluteTimeInfo_r10;

  /*!< boolean to indicate whether Area config is present, since its NEED OR */
  boolean area_cfg_present;
  
  /*!< Indicates area configuration */
  lte_rrc_mdt_area_cfg_r10_s area_cfg;

  /*!< Duration of MDT session */
  lte_rrc_mdt_duration_e logging_duration;

  /*!< Logging interval while MDT session is active */
  lte_cphy_ueinfo_mdt_logging_interval_e logging_interval;
  /*!< R11 PLMN identity list received in LoggedMeasConfig */ 
  lte_rrc_plmn_list_s plmn_identity_list;

  /*!< plmn_Identity_perTAC_List_r11 received in LoggedMeasConfig */ 
  lte_rrc_plmn_list_s plmn_identity_perTAC_list;

  /*!< Indicates whether LoggedMeasConfig OTA has R11 AreaCfg IE*/
  boolean area_config_r11_present;
} lte_rrc_mdt_cfg_r10_s;

/*! @brief Enum to represent timer extension request reason
*/
typedef enum
{
  LTE_RRC_SIB_EXTND_TMR_AUTO_GAP,              /*!< 0 */
  LTE_RRC_SIB_EXTND_TMR_MAX = 0xFF,            /*!< 255 */
} lte_rrc_sib_extnd_tmr_reason_e;

/*! @brief Data structure for SIB extnd timer reqi
*/
typedef struct
{
  /*!< Message router header */
  msgr_hdr_s msg_hdr;

  /*!< SIB timer extension reason */
  lte_rrc_sib_extnd_tmr_reason_e reason;

  /*!< Duration to extend the SIB timers */
  uint64 duration;

} lte_rrc_sib_extnd_tmr_reqi_s;

/*! @brief Enumeration of INDI id nums
*/
typedef enum
{
  /*RRC Controller*/
  RRC_INDI_STOPPED = 0x0,
  /*SIB*/
  RRC_INDI_UNSOLICITED_SIB = 0x01,
  RRC_INDI_SIB_UPDATED = 0x02,
  RRC_INDI_SIB_READ_ERROR = 0x03,
  /*CEP*/
  RRC_INDI_CONN_ESTABLISHMENT_STARTED = 0x04,
  RRC_INDI_CONNECTED = 0x05,
  RRC_INDI_CONN_ESTABLISHMENT_FAILURE = 0x06,
  RRC_INDI_STOP_CELL_RESEL = 0x07,
  RRC_INDI_PROCEED_WITH_RESEL = 0x08,
  /*Paging*/
  /*CRP*/
  RRC_INDI_CONN_REL_STARTED = 0x09,
  RRC_INDI_CONN_RELEASED = 0x0a,
  /*CSP*/
  RRC_INDI_NOT_CAMPED = 0x0b,
  RRC_INDI_CAMPED = 0x0c,
  RRC_INDI_CELL_RESEL_STARTED = 0x0d,
  RRC_INDI_CELL_RESEL_CANCELED = 0x0e,
  /*Config*/
  RRC_INDI_CONN_MODE_FAILURE = 0x0f,
  /*Dispatcher*/
  RRC_INDI_PENDING_MSG = 0x10,
  /*Config*/
  RRC_INDI_HANDOVER_STARTED = 0x11,
  /*CRE*/
  RRC_INDI_CRE_STARTED = 0x12,
  RRC_INDI_CRE_COMPLETED = 0x13,
  RRC_INDI_INITIATE_CONN_REL = 0x14,
  /*Measurement*/
  RRC_INDI_MEAS_PENDING_SIB_UPDATED = 0x15,
  /*Security*/
  RRC_INDI_SMC_FAILURE = 0x16,
  /* Inter RAT : From LTE Redirection / Reselection */
  RRC_INDI_IRAT_FROM_LTE_REDIR_FAILED = 0x17,
  RRC_INDI_IRAT_FROM_LTE_RESEL_FAILED = 0x18,
  /* Inter RAT : To LTE Reselection */
  RRC_INDI_IRAT_TO_LTE_RESEL_COMPLETED = 0x19,
  /* Dedicated reselection priority list */
  RRC_INDI_DED_PRIORITY_LIST = 0x1A,
  /* IRAT disabling of Cell Resel to UTRA */
  RRC_INDI_IRAT_UTRA_RESEL_WAIT_TIME = 0x1B,
  /* Inter RAT : To LTE Redirection */
  RRC_INDI_IRAT_TO_LTE_REDIR_COMPLETED = 0x1C,
  /* Inter RAT : From LTE PLMN SRCH */
  RRC_INDI_IRAT_FROM_LTE_PLMN_SRCH_RESUME_FAILED = 0x1D,
  /* Inter RAT : From LTE Mobility */
  RRC_INDI_IRAT_FROM_LTE_TO_G_MOBILITY = 0x1E,
  /*Message Handler*/
  RRC_INDI_DLM_PROCESSED = 0x1F,
  /*LLC*/
  RRC_INDI_SERVING_CELL_CHANGED = 0x20,
  /*CEP*/
  RRC_INDI_OOS = 0x21,
  /* Inter RAT : From LTE to 1x Mobility */
  RRC_INDI_IRAT_FROM_LTE_TO_1X_MOBILITY = 0x22,
  /* CEP - new cell info containing tac\plmn */
  RRC_INDI_NEW_CELL_INFO = 0x23,
  /* Inter RAT : From LTE to W Mobility */
  RRC_INDI_IRAT_FROM_LTE_TO_W_MOBILITY = 0x24,
  RRC_INDI_SRB2_RESUMED = 0x25,
  RRC_INDI_IRAT_HRPD_PRE_REG_INFO = 0x26,

  /* Inter RAT: To LTE PSHO */
  RRC_INDI_IRAT_TO_LTE_PSHO_STARTED   = 0x27,
  RRC_INDI_IRAT_TO_LTE_PSHO_KEY_GEN   = 0x28,
  RRC_INDI_IRAT_TO_LTE_PSHO_SUCCESS   = 0x29,
  RRC_INDI_IRAT_TO_LTE_PSHO_COMPLETED = 0x2A,

  /* eMBMS */
  RRC_INDI_EMP_DEACT_TMGI = 0x2B,

  /* Inter RAT : From LTE to HDR Mobility */
  RRC_INDI_IRAT_FROM_LTE_TO_HDR_MOBILITY = 0x2C,

  /* eMBMS */
  RRC_INDI_EMP_OOS_WARN = 0x2D,
  RRC_INDI_EMP_PENDED_SERVICE = 0x2E,
  RRC_INDI_ESMGR_AVAIL_TMGI_LIST = 0x2F,

    /* Inter RAT : From LTE to TDS Mobility */
  RRC_INDI_IRAT_FROM_LTE_TO_TDS_MOBILITY = 0x30,

  RRC_INDI_IRAT_UTRA_TDD_RESEL_WAIT_TIME = 0x31,

  RRC_INDI_CSG_CFG = 0x32,

  RRC_INDI_IRAT_FROM_LTE_TO_G_PLMN_SRCH_SUSPEND = 0x33,

  RRC_INDI_IRAT_FROM_LTE_TO_W_PLMN_SRCH_SUSPEND = 0x34,

  RRC_INDI_IRAT_FROM_LTE_TO_TDS_PLMN_SRCH_SUSPEND = 0x35,

  RRC_INDI_RAT_PRIORITY_UPDATE = 0x36,
  
  RRC_INDI_INTERFREQ_LIST_UPDATE = 0x37,

  RRC_INDI_LLC_MBSFN_CFG_CHANGED = 0x38,

  RRC_INDI_DEPRI_FREQ = 0x39,

  RRC_INDI_IRAT_LTE_DEPRI = 0x3A,

  RRC_INDI_ESMGR_RESEL_PRIO_CHANGE = 0x3B,

  RRC_INDI_CGI_SUCCESS = 0x3C,

  RRC_INDI_TRM_PRIORITY_CHANGE = 0x3D,

  RRC_INDI_IRAT_FROM_LTE_RESEL_SUCCESS = 0x3E,

  RRC_INDI_PROX_CFG = 0x3F,

  RRC_INDI_SIB_NOT_RCVD = 0x40,

  RRC_INDI_IRAT_FROM_LTE_MOBILITY_STARTED = 0x41,

  RRC_INDI_CLEAR_DEPRI_FREQ = 0x42,

  RRC_INDI_MBSFN_SNR = 0x43,

  RRC_INDI_NV_REFRESH = 0x44,

  RRC_INDI_LOC_INFO = 0x45, 

  RRC_INDI_SIB1_AVAIL = 0x46,

  RRC_INDI_SEC_ACTIVE = 0x47,

  #ifdef FEATURE_LTE_ELS_ENABLED
  RRC_INDI_AUTO_ELS_UE_CAP_INFO = 0x48,
  #endif

  RRC_INDI_QSH_CB = 0x49,

  RRC_INDI_MAX     /*!< Max value */
}lte_rrc_indi_type_e;

/*! @brief Enumeration for location service timer type
*/
typedef enum
{ 
  LTE_RRC_LOC_SERVICES_TMR_TRPT = 0,
  LTE_RRC_LOC_SERVICES_TMR_TGPS,     
  LTE_RRC_LOC_SERVICES_TMR_IGPS,
  LTE_RRC_LOC_SERVICES_TMR_TGNSS,
} lte_rrc_loc_services_tmr_type_e;

/*! @brief Enumeration for location service client type
*/
typedef enum
{ 
  LTE_RRC_LOC_SERVICES_CLIENT_NONE = 0,
  LTE_RRC_LOC_SERVICES_CLIENT_MDT = 0x01,     
  LTE_RRC_LOC_SERVICES_CLIENT_MEAS = 0x02,
  LTE_RRC_LOC_SERVICES_CLIENT_RLF_RPT = 0x04,
} lte_rrc_loc_services_client_type_e;

/*! @brief Enumeration for received info type
*/
typedef enum
{ 
  LTE_RRC_LOC_SERVICES_INFO_BAP = 0x01,     
  LTE_RRC_LOC_SERVICES_INFO_POS_RPT = 0x02,
} lte_rrc_loc_services_info_type_e;

/*! @brief Data Structure for RRC Loc Service Info Ind.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  /*! Received report type from GPS */
  lte_rrc_loc_services_info_type_e rpt_type;
  /* Payload */
  void *rpt_ptr;


} lte_rrc_loc_services_info_indi_s;


/*! @brief Data Structure for RRC NV Refresh Ind.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  boolean reference_valid;

  /*! Reference id */
  uint32 reference_id;

  /*! Subscription id to refresh NVs */
  int8 sub_id;

} lte_rrc_nv_refresh_indi_s;

/*! @brief Internal indications that various RRC procedures throw
*/
enum
{
  /*Internal Indications */

  LTE_RRC_INDI_FIRST = MSGR_UMID_BASE(MSGR_LTE_RRC, MSGR_TYPE_INDI),

  /*RRC Controller*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, STOPPED,
                   RRC_INDI_STOPPED,
                   lte_rrc_stopped_indi_s),

  /*SIB*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, UNSOLICITED_SIB, 
                   RRC_INDI_UNSOLICITED_SIB,
                   lte_rrc_unsolicited_sib_indi_s),

  MSGR_DEFINE_UMID(LTE, RRC, INDI, SIB_UPDATED, 
                   RRC_INDI_SIB_UPDATED,
                   lte_rrc_sib_updated_indi_s),

  MSGR_DEFINE_UMID(LTE, RRC, INDI, SIB_READ_ERROR, 
                   RRC_INDI_SIB_READ_ERROR,
                   lte_rrc_sib_read_error_indi_s),

  /*CEP*/
         
  MSGR_DEFINE_UMID(LTE, RRC, INDI, CONN_ESTABLISHMENT_STARTED, 
                   RRC_INDI_CONN_ESTABLISHMENT_STARTED,
                   lte_rrc_conn_establishment_started_indi_s),
  MSGR_DEFINE_UMID(LTE, RRC, INDI, CONNECTED, 
                   RRC_INDI_CONNECTED,
                   lte_rrc_connected_indi_s),
  MSGR_DEFINE_UMID(LTE, RRC, INDI, CONN_ESTABLISHMENT_FAILURE, 
                   RRC_INDI_CONN_ESTABLISHMENT_FAILURE,
                   lte_rrc_conn_establishment_failure_indi_s),
  MSGR_DEFINE_UMID(LTE, RRC, INDI, STOP_CELL_RESEL, 
                   RRC_INDI_STOP_CELL_RESEL,
                   lte_rrc_stop_cell_resel_indi_s),
  MSGR_DEFINE_UMID(LTE, RRC, INDI, PROCEED_WITH_RESEL, 
                   RRC_INDI_PROCEED_WITH_RESEL,
                   lte_rrc_proceed_with_resel_indi_s),
  MSGR_DEFINE_UMID(LTE, RRC, INDI, OOS, 
                   RRC_INDI_OOS,
                   lte_rrc_oos_indi_s),
  /*Paging*/


  /*CRP*/

  MSGR_DEFINE_UMID(LTE, RRC, INDI, CONN_REL_STARTED, 
                   RRC_INDI_CONN_REL_STARTED,
                   lte_rrc_conn_rel_started_indi_s),
                                    
  MSGR_DEFINE_UMID(LTE, RRC, INDI, CONN_RELEASED, 
                   RRC_INDI_CONN_RELEASED,
                   lte_rrc_conn_released_indi_s),
 /*CSP*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, NOT_CAMPED, 
                   RRC_INDI_NOT_CAMPED,
                   lte_rrc_not_camped_indi_s),

  MSGR_DEFINE_UMID(LTE, RRC, INDI, CAMPED, 
                   RRC_INDI_CAMPED,
                   lte_rrc_camped_indi_s),

  MSGR_DEFINE_UMID(LTE, RRC, INDI, CELL_RESEL_STARTED, 
                   RRC_INDI_CELL_RESEL_STARTED,
                   lte_rrc_cell_resel_started_indi_s),

  MSGR_DEFINE_UMID(LTE, RRC, INDI, CELL_RESEL_CANCELED, 
                   RRC_INDI_CELL_RESEL_CANCELED,
                   lte_rrc_cell_resel_canceled_indi_s),
  /*Config*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, HANDOVER_STARTED, 
                   RRC_INDI_HANDOVER_STARTED,
                   lte_rrc_handover_started_indi_s),

  MSGR_DEFINE_UMID(LTE, RRC, INDI, CONN_MODE_FAILURE, 
                   RRC_INDI_CONN_MODE_FAILURE,
                   lte_rrc_conn_mode_failure_indi_s),

  MSGR_DEFINE_UMID(LTE, RRC, INDI, SRB2_RESUMED, 
                   RRC_INDI_SRB2_RESUMED,
                   lte_rrc_srb2_resumed_indi_s),

  /* CRE */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, CRE_STARTED, 
                   RRC_INDI_CRE_STARTED,
                   lte_rrc_cre_started_indi_s),

  MSGR_DEFINE_UMID(LTE, RRC, INDI, CRE_COMPLETED, 
                   RRC_INDI_CRE_COMPLETED,
                   lte_rrc_cre_completed_indi_s),

  MSGR_DEFINE_UMID(LTE, RRC, INDI, INITIATE_CONN_REL, 
                   RRC_INDI_INITIATE_CONN_REL,
                   lte_rrc_initiate_conn_rel_indi_s),
  /*LLC*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, SERVING_CELL_CHANGED, 
                   RRC_INDI_SERVING_CELL_CHANGED,
                   lte_rrc_serving_cell_changed_indi_s),

  MSGR_DEFINE_UMID(LTE, RRC, INDI, LLC_MBSFN_CFG_CHANGED, 
                   RRC_INDI_LLC_MBSFN_CFG_CHANGED,
                   lte_rrc_llc_mbsfn_cfg_chng_indi_s),

  /*Dispatcher*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, PENDING_MSG, 
                   RRC_INDI_PENDING_MSG,
                   none),

  /*Measurement*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, MEAS_PENDING_SIB_UPDATED, RRC_INDI_MEAS_PENDING_SIB_UPDATED, 
                   lte_rrc_sib_updated_indi_s),

  /*Security*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, SMC_FAILURE, RRC_INDI_SMC_FAILURE, 
                   lte_rrc_smc_failure_indi_s),

  /* Inter-RAT */ 
  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_FROM_LTE_REDIR_FAILED, RRC_INDI_IRAT_FROM_LTE_REDIR_FAILED, 
                   lte_rrc_irat_from_lte_redir_failed_indi_s),

  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_FROM_LTE_RESEL_SUCCESS, RRC_INDI_IRAT_FROM_LTE_RESEL_SUCCESS, 
                   lte_rrc_irat_from_lte_resel_success_indi_s),

  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_FROM_LTE_RESEL_FAILED, RRC_INDI_IRAT_FROM_LTE_RESEL_FAILED, 
                   lte_rrc_irat_from_lte_resel_failed_indi_s),

  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_FROM_LTE_PLMN_SRCH_RESUME_FAILED, RRC_INDI_IRAT_FROM_LTE_PLMN_SRCH_RESUME_FAILED, 
                   lte_rrc_irat_from_lte_plmn_srch_resume_failed_indi_s),

  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_FROM_LTE_MOBILITY_STARTED, 
                   RRC_INDI_IRAT_FROM_LTE_MOBILITY_STARTED,
                   lte_rrc_irat_from_lte_mobility_started_indi_s),

  /* Dedicated reselection priority indi*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, DED_PRIORITY_LIST, RRC_INDI_DED_PRIORITY_LIST, 
                   lte_rrc_ded_priority_list_indi_s),

  /* To LTE Reselection completed indi*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_TO_LTE_RESEL_COMPLETED, RRC_INDI_IRAT_TO_LTE_RESEL_COMPLETED, 
                   lte_rrc_irat_to_lte_resel_completed_indi_s),

  /* (IRAT) Disabling of cell resel to UTRA till Wait Time expiry */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_UTRA_RESEL_WAIT_TIME, RRC_INDI_IRAT_UTRA_RESEL_WAIT_TIME, 
                   lte_rrc_irat_utra_resel_wait_time_indi_s),

  /* (IRAT) Disabling of cell resel to UTRA till Wait Time expiry */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_UTRA_TDD_RESEL_WAIT_TIME, RRC_INDI_IRAT_UTRA_TDD_RESEL_WAIT_TIME, 
                   lte_rrc_irat_utra_resel_tdd_wait_time_indi_s),

  /* To LTE Redirection completed indi*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_TO_LTE_REDIR_COMPLETED, RRC_INDI_IRAT_TO_LTE_REDIR_COMPLETED, 
                   lte_rrc_irat_to_lte_redir_completed_indi_s),

  /*Message Handler*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, DLM_PROCESSED,
                   RRC_INDI_DLM_PROCESSED,
                   lte_rrc_dlm_processed_indi_s),

  /* From LTE to 1x Mobility indication */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_FROM_LTE_TO_1X_MOBILITY, RRC_INDI_IRAT_FROM_LTE_TO_1X_MOBILITY, 
                   lte_rrc_irat_from_lte_to_1x_mobility_indi_s),

    /* From LTE to HDR Mobility indication */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_FROM_LTE_TO_HDR_MOBILITY, RRC_INDI_IRAT_FROM_LTE_TO_HDR_MOBILITY, 
                   lte_rrc_irat_from_lte_to_hdr_mobility_indi_s),

  /* From LTE to G Mobility indication */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_FROM_LTE_TO_G_MOBILITY, RRC_INDI_IRAT_FROM_LTE_TO_G_MOBILITY, 
                   lte_rrc_irat_from_lte_to_g_mobility_indi_s),

  /* From LTE to W Mobility indication */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_FROM_LTE_TO_W_MOBILITY, RRC_INDI_IRAT_FROM_LTE_TO_W_MOBILITY, 
                   lte_rrc_irat_from_lte_to_w_mobility_indi_s),

  /* From LTE to TDS Mobility indication */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_FROM_LTE_TO_TDS_MOBILITY, RRC_INDI_IRAT_FROM_LTE_TO_TDS_MOBILITY, 
                   lte_rrc_irat_from_lte_to_tds_mobility_indi_s),

  /* Contains information of new cell */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, NEW_CELL_INFO, RRC_INDI_NEW_CELL_INFO,
                   lte_rrc_new_cell_info_indi_s),

  /* HRPD PreRegistration info indi for IRAT Mgr */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_HRPD_PRE_REG_INFO, RRC_INDI_IRAT_HRPD_PRE_REG_INFO,
                   lte_rrc_irat_hrpd_pre_reg_info_indi_s),

  /* IRAT Handover to LTE started indication */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_TO_LTE_PSHO_STARTED, RRC_INDI_IRAT_TO_LTE_PSHO_STARTED, 
                   lte_rrc_irat_to_lte_psho_started_indi_s),

  /* IRAT Handover to LTE keys generated indication */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_TO_LTE_PSHO_KEY_GEN, RRC_INDI_IRAT_TO_LTE_PSHO_KEY_GEN, 
                   lte_rrc_irat_to_lte_psho_key_gen_indi_s),

  /* IRAT Handover Success indication */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_TO_LTE_PSHO_SUCCESS, RRC_INDI_IRAT_TO_LTE_PSHO_SUCCESS, 
                   lte_rrc_irat_to_lte_psho_success_indi_s),

  /* IRAT Handover completed indication */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_TO_LTE_PSHO_COMPLETED, RRC_INDI_IRAT_TO_LTE_PSHO_COMPLETED, 
                   lte_rrc_irat_to_lte_psho_completed_indi_s),
  /* EMP TMGI diactivation indication */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, EMP_DEACT_TMGI, RRC_INDI_EMP_DEACT_TMGI, 
                   lte_rrc_emp_deact_tmgi_indi_s),

  /* EMP OOS warning indication */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, EMP_OOS_WARN, RRC_INDI_EMP_OOS_WARN, 
                   lte_rrc_emp_oos_warn_indi_s),

  /* EMP pended service indication */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, EMP_PENDED_SERVICE, RRC_INDI_EMP_PENDED_SERVICE, 
                   lte_rrc_emp_pended_service_indi_s),
  
  /* ESMGR available TMGI list indication */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, ESMGR_AVAIL_TMGI_LIST, RRC_INDI_ESMGR_AVAIL_TMGI_LIST, 
                   lte_rrc_esmgr_avail_tmgi_list_indi_s), 

  /* Contains information of new cell */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, CSG_CFG, RRC_INDI_CSG_CFG,
                   lte_rrc_csg_cfg_indi_s),

  /* Inter-RAT : From LTE to G PLMN Search suspend indication*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_FROM_LTE_TO_G_PLMN_SRCH_SUSPEND, RRC_INDI_IRAT_FROM_LTE_TO_G_PLMN_SRCH_SUSPEND, 
                   lte_irat_plmn_srch_suspend_indi_s),

  /* Inter-RAT : From LTE to W PLMN Search suspend indication */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_FROM_LTE_TO_W_PLMN_SRCH_SUSPEND, RRC_INDI_IRAT_FROM_LTE_TO_W_PLMN_SRCH_SUSPEND, 
                   lte_irat_plmn_srch_suspend_indi_s),
  /* RAT priority update Ind Nw mode reset from limited to auto*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, RAT_PRIORITY_UPDATE,
                   RRC_INDI_RAT_PRIORITY_UPDATE,
                   lte_rrc_rat_priority_update_indi_s),

  /* Inter-RAT : From LTE to TDS PLMN Search suspend indication*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_FROM_LTE_TO_TDS_PLMN_SRCH_SUSPEND, RRC_INDI_IRAT_FROM_LTE_TO_TDS_PLMN_SRCH_SUSPEND, 
                   lte_irat_plmn_srch_suspend_indi_s),

  /* Connectedmode interfrequency neighbor list indication*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, INTERFREQ_LIST_UPDATE, 
                   RRC_INDI_INTERFREQ_LIST_UPDATE,
                   lte_rrc_interfreq_list_update_indi_s), 

  /* LTE deprioritization indication*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, DEPRI_FREQ, RRC_INDI_DEPRI_FREQ, 
                   lte_rrc_depri_freq_indi_s), 

  /* LTE clear deprioritization indication*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, CLEAR_DEPRI_FREQ, RRC_INDI_CLEAR_DEPRI_FREQ, 
                   lte_rrc_clear_depri_freq_indi_s), 

  /* LTE deprioritization indication*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, IRAT_LTE_DEPRI, RRC_INDI_IRAT_LTE_DEPRI, 
                   lte_rrc_irat_lte_depri_indi_s),

  /* eMBMS priority change indication*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, ESMGR_RESEL_PRIO_CHANGE, RRC_INDI_ESMGR_RESEL_PRIO_CHANGE, 
                   lte_rrc_esmgr_resel_prio_change_indi_s), 

  /*Sibs NOT rcvd Indication*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, SIB_NOT_RCVD, 
                   RRC_INDI_SIB_NOT_RCVD,
                   lte_rrc_sib_not_rcvd_indi_s),
  /* LTE Proximity config indication*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, PROX_CFG, RRC_INDI_PROX_CFG, 
                   lte_rrc_prox_cfg_indi_s), 

  /* LTE CGI Success indication*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, CGI_SUCCESS, RRC_INDI_CGI_SUCCESS, 
                   lte_rrc_cgi_success_indi_s), 

  /* TRM priority change indication*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, TRM_PRIORITY_CHANGE, RRC_INDI_TRM_PRIORITY_CHANGE, 
                   lte_rrc_trm_priority_change_indi_s),  

  /* LTE RRC MBSFN SNR indication*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, MBSFN_SNR, RRC_INDI_MBSFN_SNR, 
                   lte_rrc_mbsfn_snr_indi_s),  

  /* LTE RRC NV Refresh indication by RRC MISC */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, NV_REFRESH, RRC_INDI_NV_REFRESH, 
                   lte_rrc_nv_refresh_indi_s),  

  /* LTE RRC Loc services info sent by loc_services.c */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, LOC_SERVICES_INFO, RRC_INDI_LOC_INFO, 
                   lte_rrc_loc_services_info_indi_s),  

  #ifdef FEATURE_LTE_ELS_ENABLED
  MSGR_DEFINE_UMID(LTE, RRC, INDI, AUTO_ELS_UE_CAP_INFO,
                   RRC_INDI_AUTO_ELS_UE_CAP_INFO,
                   lte_rrc_auto_els_ue_cap_info_indi_s),
  #endif

  /*LTE RRC Sib1 avail indi sent by lte_rrc_sib.c*/
  MSGR_DEFINE_UMID(LTE, RRC, INDI, SIB1_AVAIL, RRC_INDI_SIB1_AVAIL,
                   lte_rrc_sib1_avail_indi_s),
                  
  MSGR_DEFINE_UMID(LTE, RRC, INDI, SEC_ACTIVE, RRC_INDI_SEC_ACTIVE,
                   lte_rrc_sec_active_indi_s),

    /* LTE RRC QSH CB INDI sent by qsh.c */
  MSGR_DEFINE_UMID(LTE, RRC, INDI, QSH_CB, RRC_INDI_QSH_CB, 
                                lte_rrc_qsh_cb_ind_s), 

  LTE_RRC_INDI_MAX,
  LTE_RRC_INDI_LAST = LTE_RRC_INDI_MAX - 1
};

/*! @brief Data structure for DL message indications
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  void *dl_msg_ptr; /*!< Pointer to decoded DL msg */
  lte_rrc_cell_id_s cell_id; /*!< Cell identity */
  lte_sfn_s sfn; /*!< SFN on which the msg was received */
  int pdu_num; /*!< PDU number of the message; to be used by dispatcher to free 
                    the decoded msg*/
  /*Security related data... specifically for SMC*/
  uint8 pdcp_hdr;/*!< PDCP PDU hdr*/
  uint32 count; /*!< PDCP DL count*/
  uint8 mac_i[LTE_MACI_LEN]; /*!< MAC_I */
} lte_rrc_mh_dlm_s;

/*! @brief Enumeration of DLM id nums
*/
typedef enum
{
  /*Message Handler*/
  RRC_DLM_RRC_CONNECTION_REESTABLISHMENT = 0x0,
  RRC_DLM_RRC_CONNECTION_REESTABLISHMENT_REJECT = 0x01,
  RRC_DLM_RRC_CONNECTION_REJECT = 0x02,
  RRC_DLM_RRC_CONNECTION_SETUP = 0x03,
  RRC_DLM_CSFB_PARAMETERS_RESPONSE_CDMA2000 = 0x04,
  RRC_DLM_DL_INFORMATION_TRANSFER = 0x05,
  RRC_DLM_HANDOVER_FROM_EUTRA_PREPARATION_REQUEST = 0x06,
  RRC_DLM_MOBILITY_FROM_EUTRA_COMMAND = 0x07,
  RRC_DLM_RRC_CONNECTION_RECONFIGURATION = 0x08,
  RRC_DLM_RRC_CONNECTION_RELEASE = 0x09,
  RRC_DLM_SECURITY_MODE_COMMAND = 0x0a,
  RRC_DLM_UE_CAPABILITY_ENQUIRY = 0x0b,
  RRC_DLM_COUNTER_CHECK = 0x0c,
  RRC_DLM_REL9_UE_INFORMATION_REQUEST = 0x0d,
  LTE_DLM_REL10_LOGGED_MEAS_CFG_REQUEST = 0x0e,
  RRC_DLM_REL9_PROXIMITY_INDICATION = 0x0f,
  RRC_DLM_PAGING = 0x10,
  #ifdef FEATURE_LTE_ELS_ENABLED
  RRC_DLM_ELS_UE_CAPABILITY_ENQUIRY = 0x11,
  RRC_DLM_ELS_RRC_CONNECTION_RECONFIGURATION = 0x12,
  #endif
  /*RRC Controller*/
  /*SIB*/
  /*CEP*/
  /*Paging*/
  /*CRP*/
  /*CSP*/
  /*Config*/
  /*LLC*/
  /*Measurement*/
  RRC_DLM_MAX     /*!< Max value */
}lte_rrc_dlm_type_e;

 /*! @brief Downlink Message indications from Message Handler 
*/
enum
{
  /*Downlink Message Indications */

  LTE_RRC_DLM_FIRST = MSGR_UMID_BASE(MSGR_LTE_RRC, MSGR_TYPE_DLM),

  /*Message Handler*/

  MSGR_DEFINE_UMID(LTE, RRC, DLM, RRC_CONNECTION_REESTABLISHMENT, 
                   RRC_DLM_RRC_CONNECTION_REESTABLISHMENT,
                   lte_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(LTE, RRC, DLM, RRC_CONNECTION_REESTABLISHMENT_REJECT, 
                   RRC_DLM_RRC_CONNECTION_REESTABLISHMENT_REJECT,
                   lte_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(LTE, RRC, DLM, RRC_CONNECTION_REJECT,
                   RRC_DLM_RRC_CONNECTION_REJECT,
                   lte_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(LTE, RRC, DLM, RRC_CONNECTION_SETUP, 
                   RRC_DLM_RRC_CONNECTION_SETUP,
                   lte_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(LTE, RRC, DLM, CSFB_PARAMETERS_RESPONSE_CDMA2000, 
                   RRC_DLM_CSFB_PARAMETERS_RESPONSE_CDMA2000,
                   lte_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(LTE, RRC, DLM, DL_INFORMATION_TRANSFER, 
                   RRC_DLM_DL_INFORMATION_TRANSFER,
                   lte_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(LTE, RRC, DLM, HANDOVER_FROM_EUTRA_PREPARATION_REQUEST, 
                   RRC_DLM_HANDOVER_FROM_EUTRA_PREPARATION_REQUEST,
                   lte_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(LTE, RRC, DLM, MOBILITY_FROM_EUTRA_COMMAND, 
                   RRC_DLM_MOBILITY_FROM_EUTRA_COMMAND,
                   lte_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(LTE, RRC, DLM, RRC_CONNECTION_RECONFIGURATION, 
                   RRC_DLM_RRC_CONNECTION_RECONFIGURATION,
                   lte_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(LTE, RRC, DLM, RRC_CONNECTION_RELEASE, 
                   RRC_DLM_RRC_CONNECTION_RELEASE,
                   lte_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(LTE, RRC, DLM, SECURITY_MODE_COMMAND, 
                   RRC_DLM_SECURITY_MODE_COMMAND,
                   lte_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(LTE, RRC, DLM, UE_CAPABILITY_ENQUIRY, 
                   RRC_DLM_UE_CAPABILITY_ENQUIRY,
                   lte_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(LTE, RRC, DLM, COUNTER_CHECK, 
                   RRC_DLM_COUNTER_CHECK,
                   lte_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(LTE, RRC, DLM, PAGING, 
                   RRC_DLM_PAGING,
                   lte_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(LTE, RRC, DLM, REL9_UE_INFORMATION_REQUEST, 
                   RRC_DLM_REL9_UE_INFORMATION_REQUEST,
                   lte_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(LTE, RRC, DLM, REL10_LOGGED_MEAS_CFG_REQUEST, 
                   LTE_DLM_REL10_LOGGED_MEAS_CFG_REQUEST,
                   lte_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(LTE, RRC, DLM, REL9_PROXIMITY_INDICATION, 
                   RRC_DLM_REL9_PROXIMITY_INDICATION,
                   lte_rrc_mh_dlm_s),
  #ifdef FEATURE_LTE_ELS_ENABLED
  MSGR_DEFINE_UMID(LTE, RRC, DLM, ELS_UE_CAPABILITY_ENQUIRY, 
                   RRC_DLM_ELS_UE_CAPABILITY_ENQUIRY,
                   lte_rrc_mh_dlm_s),

  MSGR_DEFINE_UMID(LTE, RRC, DLM, ELS_RRC_CONNECTION_RECONFIGURATION, 
                   RRC_DLM_ELS_RRC_CONNECTION_RECONFIGURATION,
                   lte_rrc_mh_dlm_s),
  #endif

  LTE_RRC_DLM_MAX,
  LTE_RRC_DLM_LAST = LTE_RRC_DLM_MAX - 1

};

/*! @brief Data structure for Timer expiry indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
} lte_rrc_tmri_s;

/*! @brief Enumeration of TMRI id nums
*/
/*********************************************************************
   DO NOT CHANGE THE ORDER OF THE ENUM BELOW - USED FOR EVENT LOGGING.
   ALWAYS ADD YOUR NEW TIMER AT THE BOTTOM
 *********************************************************************/
typedef enum
{
  RRC_TMRI_SIB_EVENT1_WT = 0x0, /* do not change */
  RRC_TMRI_SIB_EVENT2_WT = 0x01, /* do not change */
  RRC_TMRI_SIB_3HR_CLOCK_TICK = 0x02, /* do not change */
  RRC_TMRI_T300 = 0x03, /* do not change */
  RRC_TMRI_T301 = 0x04, /* do not change */
  RRC_TMRI_T302 = 0x05, /* do not change */
  RRC_TMRI_T303 = 0x06, /* do not change */
  RRC_TMRI_T305 = 0x07, /* do not change */
  RRC_TMRI_OOS = 0x09, /* do not change */
  RRC_TMRI_CELL_RESEL_STARTED = 0x0a, /* do not change */
  RRC_TMRI_T304 = 0x0b, /* do not change */
  RRC_TMRI_CONN_REL = 0x0c, /* do not change */
  RRC_TMRI_DEADLOCK = 0x0d, /* do not change */
  RRC_TMRI_CEP_RESEL = 0x0e, /* do not change */
  /*CRE*/
  RRC_TMRI_T311 = 0x0f, /* do not change */
  /*SEC*/
  RRC_TMRI_SMC_FAIL = 0x10, /*do not change*/
  RRC_TMRI_T320 = 0x11, /* do not change */
  RRC_TMRI_PLMN_SEARCH_GUARD = 0x12, /* do not change */
  RRC_TMRI_IRAT_REDIR_LIST = 0x13, /* do not change */
  RRC_TMRI_IRAT_REDIR_WAIT = 0x14, /* do not change */
  RRC_TMRI_FROM_G_REDIR    = 0x15, /* do not change */
  RRC_TMRI_IRAT_W_RESEL_WAIT    = 0x16, /* do not change */
  RRC_TMRI_PERIODIC_SIB8    = 0x17, /* do not change */
  RRC_TMRI_T321 = 0x18, /* do not change */
  RRC_TMRI_PLMN_SEARCH_ACQ_DB = 0x19, /* do not change */
  RRC_TMRI_OOS_SYSTEM_SCAN = 0x1a, /* do not change */
  RRC_TMRI_IRAT_CGI = 0x1b, /* do not change */
  RRC_TMRI_FROM_G_BLIND_REDIR = 0x1c, /* do not change */
  RRC_TMRI_FROM_W_BLIND_REDIR = 0x1d, /* do not change */
  RRC_TMRI_PLMN_SEARCH_MSEARCH_BSCAN_SEP = 0x1e, /* do not change */
	/*eMBMS*/
  RRC_TMRI_EMP_MCCH0_WT = 0x1f, /* do not change */
  RRC_TMRI_EMP_MCCH1_WT = 0x20, /* do not change */
  RRC_TMRI_EMP_MCCH2_WT = 0x21, /* do not change */
  RRC_TMRI_EMP_MCCH3_WT = 0x22, /* do not change */
  RRC_TMRI_EMP_MCCH4_WT = 0x23, /* do not change */
  RRC_TMRI_EMP_MCCH5_WT = 0x24, /* do not change */
  RRC_TMRI_EMP_MCCH6_WT = 0x25, /* do not change */
  RRC_TMRI_EMP_MCCH7_WT = 0x26, /* do not change */
  RRC_TMRI_EMP_OOS = 0x27, /* do not change */
  RRC_TMRI_FROM_TDS_BLIND_REDIR = 0x28, /* do not change */
  RRC_TMRI_IRAT_TDS_RESEL_WAIT    = 0x29, /* do not change */
  RRC_TMRI_CTLR_DEADLOCK = 0x2a, /* do not change */
  RRC_TMRI_RECONFIG_WAIT = 0x2b, /* do not change */
  /* CSG */
  RRC_TMRI_CSG_ASF = 0x2c, /* do not change */
  RRC_TMRI_T306 = 0x2d, /* do not change */
  /*UEINFO*/
  RRC_TMRI_UEINFO_RPT_VALID = 0x2e, /* do not change */
  RRC_TMRI_UEINFO_CONN_FAILURE = 0x2f, /* do not change */
  /*MDT*/
  RRC_TMRI_MDT_RPT_VALID = 0x30, /* do not change */
  RRC_TMRI_T330 = 0x31, /* do not change */
  RRC_TMRI_T325 = 0x32, /* do not change */
  /*RLF*/
  RRC_TMRI_RLF_SYSTEM_SCAN = 0x33, /* do not change */
  RRC_TMRI_SYSTEM_SCAN_PRIOR_T311_EXPIRY = 0x34, /* do not change */
  RRC_TMRI_PERIODIC_SIB16    = 0x35, /* do not change */
  RRC_TMRI_ACT_TMGI_GUARD = 0x36, /* do not change */
  RRC_TMRI_MBMS_INTEREST_IND = 0x37, /* do not change */
  RRC_TMRI_CEP_RF_UNAVAILABLE = 0x38, /* do not change */
  RRC_TMRI_ESMGR_CLEAR_CFL_WAIT = 0x39, /* do not change */
  RRC_TMRI_PLMN_SEARCH_FIRSTP = 0x3a, /* do not change */
  RRC_TMRI_PLMN_SEARCH_MOREP = 0x3b, /* do not change */
  /* Connection failure rpt */
  RRC_TMRI_UEINFO_CON_FAILURE_RPT_VALID = 0x3c, /* do not change */
  RRC_TMRI_ESMGR_SLTE_IN_COV_WAIT = 0x3d, /* do not change */
  RRC_TMRI_CEP_DEADLOCK = 0x3e,  /* do not change */
  RRC_TMRI_CEP_ACB_DELAY_RACH = 0x3F, /* do not change */ 
  RRC_TMRI_TRM_HIGH_PRIORITY = 0x40,  /* do not change */
  RRC_TMRI_UEINFO_LOC_INFO_HOF =  0x41,  /* do not change */
  RRC_TMRI_MAX     /*!< Max value */
}lte_rrc_tmri_type_e;

/*! @brief Timers maintained by the RRC procedures
*/
enum
{
  /* Internal Timers */

  LTE_RRC_TMRI_FIRST = MSGR_UMID_BASE(MSGR_LTE_RRC, MSGR_TYPE_TMRI),

  /*Message Handler*/

  /*RRC Controller*/
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, CTLR_DEADLOCK,
                   RRC_TMRI_CTLR_DEADLOCK, lte_rrc_tmri_s),

  /*SIB*/
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, SIB_EVENT1_WT, 
                   RRC_TMRI_SIB_EVENT1_WT, lte_rrc_tmri_s),
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, SIB_EVENT2_WT, 
                   RRC_TMRI_SIB_EVENT2_WT, lte_rrc_tmri_s),
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, SIB_3HR_CLOCK_TICK, 
                   RRC_TMRI_SIB_3HR_CLOCK_TICK, lte_rrc_tmri_s),
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, PERIODIC_SIB8, 
                   RRC_TMRI_PERIODIC_SIB8, lte_rrc_tmri_s),
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, PERIODIC_SIB16, 
                   RRC_TMRI_PERIODIC_SIB16, lte_rrc_tmri_s),

  /*CEP*/
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, T300, 
                   RRC_TMRI_T300, lte_rrc_tmri_s),
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, T301, 
                   RRC_TMRI_T301, lte_rrc_tmri_s),
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, T302, 
                   RRC_TMRI_T302, lte_rrc_tmri_s),
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, T303, 
                   RRC_TMRI_T303, lte_rrc_tmri_s),
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, T305, 
                   RRC_TMRI_T305, lte_rrc_tmri_s),
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, T306, 
                   RRC_TMRI_T306, lte_rrc_tmri_s),
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, CEP_RESEL, 
                   RRC_TMRI_CEP_RESEL, lte_rrc_tmri_s),
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, CEP_RF_UNAVAILABLE, 
                     RRC_TMRI_CEP_RF_UNAVAILABLE, lte_rrc_tmri_s),
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, CEP_DEADLOCK, 
                   RRC_TMRI_CEP_DEADLOCK, lte_rrc_tmri_s),
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, CEP_ACB_DELAY_RACH, 
                   RRC_TMRI_CEP_ACB_DELAY_RACH, lte_rrc_tmri_s),
  /*Paging*/

  /*CSP*/

  MSGR_DEFINE_UMID(LTE, RRC, TMRI, OOS, 
                   RRC_TMRI_OOS, lte_rrc_tmri_s),

  MSGR_DEFINE_UMID(LTE, RRC, TMRI, OOS_SYSTEM_SCAN, 
                   RRC_TMRI_OOS_SYSTEM_SCAN, lte_rrc_tmri_s),

  MSGR_DEFINE_UMID(LTE, RRC, TMRI, RLF_SYSTEM_SCAN, 
                   RRC_TMRI_RLF_SYSTEM_SCAN, lte_rrc_tmri_s),

  MSGR_DEFINE_UMID(LTE, RRC, TMRI, SYSTEM_SCAN_PRIOR_T311_EXPIRY, 
                   RRC_TMRI_SYSTEM_SCAN_PRIOR_T311_EXPIRY, lte_rrc_tmri_s),

  /*Config*/
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, T304, 
                   RRC_TMRI_T304, lte_rrc_tmri_s),
  /*CRP*/
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, CONN_REL, 
                   RRC_TMRI_CONN_REL, lte_rrc_tmri_s),
  /*LLC*/
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, DEADLOCK, 
                   RRC_TMRI_DEADLOCK, lte_rrc_tmri_s),

  /* CRE */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, T311, 
                   RRC_TMRI_T311, lte_rrc_tmri_s),

  /*SEC*/
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, SMC_FAIL, 
                   RRC_TMRI_SMC_FAIL, lte_rrc_tmri_s),

  /*Measurement*/
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, T320, 
                   RRC_TMRI_T320, lte_rrc_tmri_s),
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, IRAT_W_RESEL_WAIT, 
                   RRC_TMRI_IRAT_W_RESEL_WAIT, lte_rrc_tmri_s),
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, IRAT_TDS_RESEL_WAIT, 
                   RRC_TMRI_IRAT_TDS_RESEL_WAIT, lte_rrc_tmri_s),
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, T321, 
                   RRC_TMRI_T321, lte_rrc_tmri_s),
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, T325, 
                   RRC_TMRI_T325, lte_rrc_tmri_s),

  /*PLMN Search*/
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, PLMN_SEARCH_GUARD, 
                   RRC_TMRI_PLMN_SEARCH_GUARD, lte_rrc_tmri_s),

  MSGR_DEFINE_UMID(LTE, RRC, TMRI, PLMN_SEARCH_ACQ_DB, 
                   RRC_TMRI_PLMN_SEARCH_ACQ_DB, lte_rrc_tmri_s),

  MSGR_DEFINE_UMID(LTE, RRC, TMRI, PLMN_SEARCH_MSEARCH_BSCAN_SEP, 
                   RRC_TMRI_PLMN_SEARCH_MSEARCH_BSCAN_SEP, lte_rrc_tmri_s),

  MSGR_DEFINE_UMID(LTE, RRC, TMRI, PLMN_SEARCH_FIRSTP, 
                   RRC_TMRI_PLMN_SEARCH_FIRSTP, lte_rrc_tmri_s),

  MSGR_DEFINE_UMID(LTE, RRC, TMRI, PLMN_SEARCH_MOREP, 
                   RRC_TMRI_PLMN_SEARCH_MOREP, lte_rrc_tmri_s),

  /*IRAT to LTE Redir List (10sec) timer*/
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, IRAT_REDIR_LIST, 
                   RRC_TMRI_IRAT_REDIR_LIST, lte_rrc_tmri_s),
  
  /*IRAT to LTE Redir Wait timer*/
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, IRAT_REDIR_WAIT, 
                   RRC_TMRI_IRAT_REDIR_WAIT, lte_rrc_tmri_s),

  /* G To LTE Redir List Timer */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, FROM_G_REDIR,
                   RRC_TMRI_FROM_G_REDIR, lte_rrc_tmri_s),

  /* IRAT CGI Timer */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, IRAT_CGI, 
                   RRC_TMRI_IRAT_CGI, lte_rrc_tmri_s),

  /* G To LTE Redir High Priority Timer */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, TRM_HIGH_PRIORITY,
                   RRC_TMRI_TRM_HIGH_PRIORITY, lte_rrc_tmri_s),

  /* G To LTE Redir List Timer */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, FROM_G_BLIND_REDIR,
                   RRC_TMRI_FROM_G_BLIND_REDIR, lte_rrc_tmri_s),

  /* W To LTE Blind redir Timer */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, FROM_W_BLIND_REDIR,
                   RRC_TMRI_FROM_W_BLIND_REDIR, lte_rrc_tmri_s),

  /* TDS To LTE Blind redir Timer */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, FROM_TDS_BLIND_REDIR,
                   RRC_TMRI_FROM_TDS_BLIND_REDIR, lte_rrc_tmri_s),

	/* eMBMS MCCH Reading Timer for MCCH DB index 0 */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, EMP_MCCH0_WT, 
                   RRC_TMRI_EMP_MCCH0_WT, lte_rrc_tmri_s),

	/* eMBMS MCCH Reading Timer for MCCH DB index 1 */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, EMP_MCCH1_WT, 
                   RRC_TMRI_EMP_MCCH1_WT, lte_rrc_tmri_s),

	/* eMBMS MCCH Reading Timer for MCCH DB index 2 */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, EMP_MCCH2_WT, 
                   RRC_TMRI_EMP_MCCH2_WT, lte_rrc_tmri_s),

	/* eMBMS MCCH Reading Timer for MCCH DB index 3 */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, EMP_MCCH3_WT, 
                   RRC_TMRI_EMP_MCCH3_WT, lte_rrc_tmri_s),

	/* eMBMS MCCH Reading Timer for MCCH DB index 4 */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, EMP_MCCH4_WT, 
                   RRC_TMRI_EMP_MCCH4_WT, lte_rrc_tmri_s),

	/* eMBMS MCCH Reading Timer for MCCH DB index 5 */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, EMP_MCCH5_WT, 
                   RRC_TMRI_EMP_MCCH5_WT, lte_rrc_tmri_s),

	/* eMBMS MCCH Reading Timer for MCCH DB index 6 */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, EMP_MCCH6_WT, 
                   RRC_TMRI_EMP_MCCH6_WT, lte_rrc_tmri_s),

	/* eMBMS MCCH Reading Timer for MCCH DB index 7 */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, EMP_MCCH7_WT, 
                   RRC_TMRI_EMP_MCCH7_WT, lte_rrc_tmri_s),

	/* eMBMS OOS timer */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, EMP_OOS, 
                   RRC_TMRI_EMP_OOS, lte_rrc_tmri_s),
  
  /* RRC Reconfig Wait timer */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, RECONFIG_WAIT, 
                   RRC_TMRI_RECONFIG_WAIT, lte_rrc_tmri_s),

  /* RRC UEInfo Valid timer */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, UEINFO_RPT_VALID, 
                   RRC_TMRI_UEINFO_RPT_VALID, lte_rrc_tmri_s),

  /*RRC UEInfo Conn failure Timer */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, UEINFO_CONN_FAILURE, 
                   RRC_TMRI_UEINFO_CONN_FAILURE, lte_rrc_tmri_s),

  /*RRC UEInfo Loc info HOF Timer */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, UEINFO_LOC_INFO_HOF, 
                    RRC_TMRI_UEINFO_LOC_INFO_HOF, lte_rrc_tmri_s),

  /* RRC CSG ASF timer */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, CSG_ASF, 
                   RRC_TMRI_CSG_ASF, lte_rrc_tmri_s),

  /* RRC MDT report Valid timer */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, MDT_RPT_VALID, 
                   RRC_TMRI_MDT_RPT_VALID, lte_rrc_tmri_s),

  /* RRC MDT logging duration timer */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, T330, 
                   RRC_TMRI_T330, lte_rrc_tmri_s),

  /* RRC eMBMS Act TMGI timer */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, ACT_TMGI_GUARD, 
                   RRC_TMRI_ACT_TMGI_GUARD, lte_rrc_tmri_s),

  /* RRC eMBMS MBMS interest indication timer */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, MBMS_INTEREST_IND, 
                   RRC_TMRI_MBMS_INTEREST_IND, lte_rrc_tmri_s),

  /* RRC eMBMS clear CFL wait timer */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, ESMGR_CLEAR_CFL_WAIT, 
                   RRC_TMRI_ESMGR_CLEAR_CFL_WAIT, lte_rrc_tmri_s),

  /* RRC eMBMS clear CFL wait timer */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, ESMGR_SLTE_IN_COV_WAIT, 
                   RRC_TMRI_ESMGR_SLTE_IN_COV_WAIT, lte_rrc_tmri_s),
                   
  /* RRC eMBMS clear CFL wait timer */
  MSGR_DEFINE_UMID(LTE, RRC, TMRI, CONN_FAILURE_RPT_VALID, 
                   RRC_TMRI_UEINFO_CON_FAILURE_RPT_VALID, lte_rrc_tmri_s),
                    
  LTE_RRC_TMRI_MAX,
  LTE_RRC_TMRI_LAST = LTE_RRC_TMRI_MAX - 1
};


typedef struct
{
 uint8 num_bands_per_combo; /* This includes Pcell as well*/
 uint8  band[LTE_RRC_MAX_CA_BANDS_IN_BAND_COMBO]; /* Pcell + max of 4 scells . ENTRY 0 IS ALWAYS PCELL*/
 uint8  bw_class[LTE_RRC_MAX_CA_BANDS_IN_BAND_COMBO];
 uint8 ul_supported;
 uint32 bw_combo_set;
 uint64 band_mask;
} lte_rrc_cap_ca_rf_cc_per_combo_s;

/*Sttructure to hold CA bandwidth combos */
typedef struct
{  
  uint8 num_combos; /* Number of Supported CA band combos*/
  lte_rrc_cap_ca_rf_cc_per_combo_s  band_combo[128]; /* As per ASN1, 36331 spec at max we can only support 128 combos */
} lte_rrc_cap_ca_rf_band_combos_s;



/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


#endif /* LTE_RRC_INT_MSG_H */
