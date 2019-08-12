/*!
  @file lte_rrc_log.h

  @brief
  This header file contains the variou defines, enums, and other data
  structure types which are necessary for logging binary packets via DIAG.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_log.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/11   np      Events for IRAT HO
06/08/11   mm      Added new events for RLF stats and RRC state change triggers
11/19/2010 dd      Added log packet version for rel9
08/16/10   sureshs Added cell blacklist event
02/16/10   sk      Updated cause values for 
                   lte_rrc_log_from_eutran_redir_end_cause_e
                   lte_rrc_log_from_eutran_resel_end_cause_e
02/16/10   sureshs Added one more reselection failure reason
01/14/10   sk      Added support for logging events for From LTE reselection
01/12/10   amit    Updated serv_cell_info_log_packet for version 2 of the log packet
11/23/09   sk      Added support for logging events for From LTE redirection
10/30/09   sureshs Added reselection failure reason for IRAT reselection failure
08/27/09   amit    Updated lte_rrc_log_ota_msg_s (removed array for ASN.1 
                   encoded OTA msg)
08/14/09   amit    Updated LTE_RRC_LOG_RRC_VER_4_0 to LTE_RRC_LOG_RRC_VER_5_0 
                   for March migration
08/10/09   amit    Updated serv_cell_info_log_packet - added new fields 
                   (ul_freq & allowed_access). Updated lte_rrc_log_serv_cell_info(); 
                   added new parameter sys_network_selection_mode_e_type)
04/15/09   sureshs Added one more reselection failure reason
02/09/09   sureshs Added enum for new cell indication cause
02/06/09   sureshs Added enum for reselection failure reason
02/06/09   da      Added event structures
12/04/08   amit    Initial Version
===========================================================================*/

#ifndef LTE_RRC_LOG_H
#define LTE_RRC_LOG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "lte_rrc_int_msg.h"
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_log_ext.h"
#include "lte_rrc_utils.h"
#include "lte_rrc_loc_servicesi.h"
#include <comdef.h>
#include <log.h>
#include <lte_as.h>
#include <lte_l1_types.h>
#include <lte_log_codes.h>

/*Please dont include files like csp.h/emp.h that include generated files from core*/

/*! @brief Maximum length of an ASN.1 encoded OTA msg (Max supported by diag is 
 *  8704(0x2220 bytes), 20 bytes for rest of the OTA log structure)
*/
#define LTE_RRC_LOG_MAX_ENCODED_MSG_LEN 8704

/*<! @brief DO CGI length in bytes */
#define LTE_RRC_LOG_DO_CGI_LENGTH_IN_BYTES  16   

/*<! @brief UTRA MAX Shared PLMN */
#define LTE_RRC_LOG_UTRA_MAX_SHARED_PLMN  WCDMA_RRC_MAX_SHARED_PLMN

/*<! @brief EUTRA MAX Shared PLMN */
#define LTE_RRC_LOG_EUTRA_MAX_SHARED_PLMN 5


/*! @brief Log packet version 1 */
#define LTE_RRC_LOG_PACKET_VER_1 1

/*! @brief Log packet version 2 */
#define LTE_RRC_LOG_PACKET_VER_2 2

/*! @brief Log packet version 3 */
#define LTE_RRC_LOG_PACKET_VER_3 3

/*! @brief Log packet version 3 */
#define LTE_RRC_LOG_PACKET_VER_4 4

/*! @brief Log packet version 5, with this version new field
 *   SIB_MASK_IN_SI gets introduced in 0xBOCO
*/
#define LTE_RRC_LOG_PACKET_VER_5 5

/*! @brief Log packet version 7, with this version 
  Rel 10 +  Rel 11 IEs can be logged in 0xBOCO*/
#define LTE_RRC_LOG_PACKET_VER_7 7

/*! @brief Log packet version 8 */
#define LTE_RRC_LOG_PACKET_VER_8 8

/*! @brief Log packet version 9 */
#define LTE_RRC_LOG_PACKET_VER_9 9

/*! @brief Log packet version 12 */
#define LTE_RRC_LOG_PACKET_VER_12 12

/*! @brief Log packet version 13 */
#define LTE_RRC_LOG_PACKET_VER_13 13

/*! @brief Log packet version 16 */
#define LTE_RRC_LOG_PACKET_VER_16 16

/*! @brief Log packet version 24 */
#define LTE_RRC_LOG_PACKET_VER_24 24

/*! @brief RRC release 8 */
#define LTE_RRC_LOG_RRC_REL_8 8 

/*! @brief RRC release 9 */
#define LTE_RRC_LOG_RRC_REL_9 9

/*! @brief RRC release 10 */
#define LTE_RRC_LOG_RRC_REL_10 0xa

/*! @brief RRC release 11 */
#define LTE_RRC_LOG_RRC_REL_11 0xb

/*! @brief RRC release 12 */
#define LTE_RRC_LOG_RRC_REL_12 0xc

/*! @brief RRC version 3.0 */
#define LTE_RRC_LOG_RRC_VER_3_0 0x30

/*! @brief RRC version 5.0 */
#define LTE_RRC_LOG_RRC_VER_5_0 0x50

/*! @brief RRC version a.0 */
#define LTE_RRC_LOG_RRC_VER_a_0 0xa0

/*! @brief RRC version 7.0 */
#define LTE_RRC_LOG_RRC_VER_7_0 0x70

/*! @brief RRC version 7.1 for Rel 10 + Rel 11 */
#define LTE_RRC_LOG_RRC_VER_7_1 0x71

/*! @brief RRC version 7.4 for Rel 11 + Rel 12 + ELS IEs*/
#define LTE_RRC_LOG_RRC_VER_7_4 0x74

/*! @brief Offsets used to identify Message type.  Offset + choice field in OTA
    message should identify the OTA msg type uniquely.  Please look at logging ICD.
 */
#define	LTE_RRC_LOG_DL_BCCH_MSG_OFFSET      0x00 
#define	LTE_RRC_LOG_DL_PCCH_MSG_OFFSET      0x40
#define LTE_RRC_LOG_DL_CCCH_MSG_OFFSET      0x48
#define LTE_RRC_LOG_DL_DCCH_MSG_OFFSET      0x80
#ifdef FEATURE_LTE_ELS_ENABLED
#define LTE_RRC_LOG_ELS_DL_DCCH_MSG_OFFSET      0x100
#define LTE_RRC_LOG_ELS_UL_DCCH_MSG_OFFSET      0x100
#endif
#define LTE_RRC_LOG_UL_CCCH_MSG_OFFSET      0x00
#define LTE_RRC_LOG_UL_DCCH_MSG_OFFSET      0x80

/*! @brief Logical channel types - used for logging */
#define LTE_RRC_LOG_LC_DL_BCCH_TYPE            0x01
#define LTE_RRC_LOG_LC_DL_PCCH_TYPE            0x02
#define LTE_RRC_LOG_LC_DL_CCCH_TYPE            0x03
#define LTE_RRC_LOG_LC_DL_DCCH_TYPE            0x04
#define LTE_RRC_LOG_LC_UL_CCCH_TYPE            0x05
#define LTE_RRC_LOG_LC_UL_DCCH_TYPE            0x06

/*! @brief Max cells for inter-rat neigh in MDT reports - 
    Original ASN1 max is 8, but due to limitaion on log packet
    size and also since ML1 can report at MAX 3 at a time */
#define LTE_CPHY_MDT_LOG_MAX_CELL_REPORT 3
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief RRC log enumeration type. Each enumeration will be mapped to a unique 
log packet/log code.
*/
typedef enum
{
  LTE_RRC_LOG_OTA_MSG_ID = 0x0000,
  LTE_RRC_LOG_SERV_CELL_INFO_ID = 0x0001,
  LTE_RRC_LOG_MIB_ID = 0x0002,
  LTE_RRC_LOG_PLMN_SEARCH_REQ_ID = 0x0003,
  LTE_RRC_LOG_PLMN_SEARCH_RSP_ID = 0x0004,
  LTE_RRC_LOG_EMBMS_ID = 0x0005,
  LTE_RRC_LOG_EMBMS_OOS_WARN_INDI_ID = 0x0006,
  LTE_RRC_LOG_IDLE_MEAS_CONFIG_ID = 0x0007,
  LTE_RRC_LOG_MDT_MEAS_CFG_ID = 0x0009,
  LTE_RRC_LOG_MDT_MEAS_INFO_ID = 0x000A,
  LTE_RRC_LOG_PAGING_UE_ID = 0x000B,
  LTE_RRC_LOG_CGI_INFO_ID = 0x000c,
  LTE_RRC_LOG_MAX_ID = 0x000D
} lte_rrc_log_id_e;

/*! @brief RRC log control structure
*/
typedef struct
{
  uint16 log_code;
  uint16 max_size;
  uint16 write_index;
  uint16 buf_size;
  uint8 *write_buf;
  uint8 *read_buf;
} lte_rrc_log_ctl_s;

/*! @brief RRC Timer Status enum type. 
*/
typedef enum
{
  LTE_RRC_LOG_TIMER_STARTED = 0,
  LTE_RRC_LOG_TIMER_STOPPED = 1,
  LTE_RRC_LOG_TIMER_EXPIRED = 2,
  LTE_RRC_LOG_TIMER_MAX_STATUS
} lte_rrc_log_timer_status_e;


/*! RRC State enumeration */
typedef enum
{
  LTE_RRC_LOG_INACTIVE = 0,
  LTE_RRC_LOG_IDLE_NOT_CAMPED = 1,
  LTE_RRC_LOG_IDLE_CAMPED = 2,
  LTE_RRC_LOG_CONNECTING = 3,
  LTE_RRC_LOG_CONNECTED = 4,
  LTE_RRC_LOG_SUSPENDED = 5,
  LTE_RRC_LOG_IRAT_TO_LTE_STARTED = 6,
  LTE_RRC_LOG_CLOSING = 7,
  LTE_RRC_LOG_MAX_STATE
}lte_rrc_log_ctrl_state_e;


/*! RRC State change trigger enumeration */
typedef enum
{
  LTE_RRC_LOG_STATE_CHANGE_INACTIVE_TRIGGER_OTHER = 0,
  LTE_RRC_LOG_STATE_CHANGE_CAMPED_TRIGGER_CONN_EST_FAILURE_CONN_ABORTED = 1,
  LTE_RRC_LOG_STATE_CHANGE_CAMPED_TRIGGER_CONN_EST_FAILURE_T300_EXPIRY = 2,
  LTE_RRC_LOG_STATE_CHANGE_CAMPED_TRIGGER_CONN_EST_FAILURE_CONN_REJECT = 3,
  LTE_RRC_LOG_STATE_CHANGE_CAMPED_TRIGGER_CONN_EST_FAILURE_CELL_RESEL = 4,
  LTE_RRC_LOG_STATE_CHANGE_CAMPED_TRIGGER_CONN_EST_FAILURE_ACCESS_BARRED = 5,
  LTE_RRC_LOG_STATE_CHANGE_CAMPED_TRIGGER_CONN_EST_FAILURE_OTHER = 6,
  LTE_RRC_LOG_STATE_CHANGE_CAMPED_TRIGGER_OTHER = 7,
  LTE_RRC_LOG_STATE_CHANGE_CONNECTING_TRIGGER_EMERGENCY = 8,
  LTE_RRC_LOG_STATE_CHANGE_CONNECTING_TRIGGER_HIGH_PRI_ACCESS = 9,
  LTE_RRC_LOG_STATE_CHANGE_CONNECTING_TRIGGER_MT_ACCESS = 10,
  LTE_RRC_LOG_STATE_CHANGE_CONNECTING_TRIGGER_MO_SIGNALING = 11,
  LTE_RRC_LOG_STATE_CHANGE_CONNECTING_TRIGGER_MO_DATA = 12,
  LTE_RRC_LOG_STATE_CHANGE_CONNECTED_TRIGGER_REEST_RECFG_FAILURE = 13,
  LTE_RRC_LOG_STATE_CHANGE_CONNECTED_TRIGGER_REEST_HO_FAILURE = 14,
  LTE_RRC_LOG_STATE_CHANGE_CONNECTED_TRIGGER_REEST_OTHER_FAILURE = 15,
  LTE_RRC_LOG_STATE_CHANGE_CONNECTED_TRIGGER_OTHER = 16,
  LTE_RRC_LOG_STATE_CHANGE_CLOSING_TRIGGER_T311_EXPIRY = 17,
  LTE_RRC_LOG_STATE_CHANGE_CLOSING_TRIGGER_T301_EXPIRY = 18,
  LTE_RRC_LOG_STATE_CHANGE_CLOSING_TRIGGER_REEST_REJECT = 19,
  LTE_RRC_LOG_STATE_CHANGE_CLOSING_TRIGGER_LOAD_BALANCE = 20,
  LTE_RRC_LOG_STATE_CHANGE_CLOSING_TRIGGER_OTHER = 21,
  LTE_RRC_LOG_STATE_CHANGE_CLOSING_TRIGGER_CONN_ABORT = 22,
  LTE_RRC_LOG_STATE_CHANGE_CLOSING_TRIGGER_CONN_ABORT_IRAT_SUCCESS = 23,
  LTE_RRC_LOG_STATE_CHANGE_CONNECTING_TRIGGER_DELAY_TOLERANT_ACCESS = 24,
  LTE_RRC_LOG_STATE_CHANGE_TRIGGER_MAX
}lte_rrc_log_ctrl_state_change_trigger_e;

/*! Reselection failure cause enumeration */
typedef enum
{
  LTE_RRC_LOG_RESEL_FAIL_S_FAILURE = 0,
  LTE_RRC_LOG_RESEL_FAIL_SIB_READ_FAILURE = 1,
  LTE_RRC_LOG_RESEL_FAIL_DL_WEAK_IND = 2,
  LTE_RRC_LOG_RESEL_FAIL_CELL_BARRED = 3,
  LTE_RRC_LOG_RESEL_FAIL_NON_EQ_PLMN = 4,
  LTE_RRC_LOG_RESEL_FAIL_FORBIDDEN_TA = 5,
  LTE_RRC_LOG_RESEL_FAIL_ABORTED = 6,
  LTE_RRC_LOG_RESEL_FAIL_FORBIDDEN_CSG_ID = 7,
  LTE_RRC_LOG_RESEL_FAIL_IRAT_RESEL_FAILURE = 8,
  LTE_RRC_LOG_RESEL_FAIL_FREQ_BARRED = 9,
  LTE_RRC_LOG_RESEL_FAIL_SIB_READ_TIMEOUT = 10,
  LTE_RRC_LOG_RESEL_FAIL_NOT_CSG = 11,
  LTE_RRC_LOG_RESEL_FAIL_BSIC_MISMATCH = 12,
  LTE_RRC_LOG_RESEL_FAIL_MAX
}lte_rrc_log_resel_fail_cause_e;

/*! New cell indication cause enumeration */
typedef enum
{
  LTE_RRC_LOG_NEW_CELL_CAUSE_SELECTION = 0,
  LTE_RRC_LOG_NEW_CELL_CAUSE_RESELECTION = 1,
  LTE_RRC_LOG_NEW_CELL_CAUSE_HANDOVER = 2,
  LTE_RRC_LOG_NEW_CELL_CAUSE_REDIRECTION = 3,
  LTE_RRC_LOG_NEW_CELL_CAUSE_MAX
}lte_rrc_log_new_cell_cause_e;

/*! Inter-RAT technology enumeration */
typedef enum
{
  LTE_RRC_LOG_RAT_CDMA_HRPD = 0,
  LTE_RRC_LOG_RAT_CDMA_1X = 1,
  LTE_RRC_LOG_RAT_GSM = 2,
  LTE_RRC_LOG_RAT_WCDMA = 3,
  LTE_RRC_LOG_RAT_LTE = 4,
  LTE_RRC_LOG_RAT_TDS = 5,
  LTE_RRC_LOG_RAT_MAX
}lte_rrc_log_rat_type_e;

/*! Inter-RAT Mobility From EUTRAN procedure enumeration */
typedef enum
{
  LTE_RRC_LOG_IRAT_HO_PROC_UTRA_HO    = 0,
  LTE_RRC_LOG_IRAT_HO_PROC_GERAN_HO   = 1,
  LTE_RRC_LOG_IRAT_HO_PROC_GERAN_CCO  = 2,
  LTE_RRC_LOG_IRAT_HO_PROC_E1XCSFB    = 3,
  LTE_RRC_LOG_IRAT_HO_PROC_HRPD_HO    = 4,
  LTE_RRC_LOG_IRAT_HO_PROC_1XSRVCC    = 5,
  LTE_RRC_LOG_IRAT_HO_PROC_MAX
}lte_rrc_log_irat_ho_proc_type_e;

/*! Inter-RAT Mobility From EUTRAN Failure cause enumeration */
typedef enum
{
  LTE_RRC_LOG_IRAT_HO_ABORTED                     = 0,
  LTE_RRC_LOG_IRAT_HO_FAIL_UNKNOWN                = 1,
  LTE_RRC_LOG_IRAT_HO_FAIL_INVALID_ST             = 2,
  LTE_RRC_LOG_IRAT_HO_FAIL_PROT_ER                = 3,
  LTE_RRC_LOG_IRAT_HO_FAIL_INVALID_CFG            = 4,
  LTE_RRC_LOG_IRAT_HO_FAIL_PHY_CHN_FAIL           = 5,
  LTE_RRC_LOG_IRAT_HO_FAIL_INVALID_CIPH           = 6,
  LTE_RRC_LOG_IRAT_HO_FAIL_UNSUPP_FREQ            = 7,
  LTE_RRC_LOG_IRAT_HO_FAIL_MSG_INVALID            = 8,
  LTE_RRC_LOG_IRAT_HO_FAIL_UNEXP_MSG              = 9, /* e1xCSFB */
  LTE_RRC_LOG_IRAT_HO_FAIL_ACQ_FAIL               = 10,/* e1xCSFB */
  LTE_RRC_LOG_IRAT_HO_FAIL_CALL_REL_REL_ORD       = 11,/* e1xCSFB */
  LTE_RRC_LOG_IRAT_HO_FAIL_CALL_REL_REORDER       = 12,/* e1xCSFB */
  LTE_RRC_LOG_IRAT_HO_FAIL_CALL_REL_INTER_ORDER   = 13,/* e1xCSFB */
  LTE_RRC_LOG_IRAT_HO_FAIL_CALL_REL_NORMAL        = 14,/* e1xCSFB */
  LTE_RRC_LOG_IRAT_HO_FAIL_CALL_REL_SO_REJ        = 15,/* e1xCSFB */
  LTE_RRC_LOG_IRAT_HO_FAIL_CALL_REL_OTASP_SPC_ER  = 16,/* e1xCSFB */
  LTE_RRC_LOG_IRAT_HO_FAIL_MAX
}lte_rrc_log_irat_ho_fail_cause_e;

/*! From E-UTRAN 'Redir Ended' cause enumeration */
typedef enum
{
  LTE_RRC_LOG_FROM_EUTRAN_REDIR_END_SUCCESS = 0,
  LTE_RRC_LOG_FROM_EUTRAN_REDIR_END_FAIL = 1,
  LTE_RRC_LOG_FROM_EUTRAN_REDIR_END_ABORT_DUE_TO_SVC_REQ = 2,
  LTE_RRC_LOG_FROM_EUTRAN_REDIR_END_ABORT_DUE_TO_MANUAL_PLMN_SRCH_REQ = 3,
  LTE_RRC_LOG_FROM_EUTRAN_REDIR_END_ABORT_DUE_TO_LTE_STACK_DEACTIVATION = 4,
  LTE_RRC_LOG_FROM_EUTRAN_REDIR_END_MAX
}lte_rrc_log_from_eutran_redir_end_cause_e;

/*! From E-UTRAN 'Resel Ended' cause enumeration */
typedef enum
{
  LTE_RRC_LOG_FROM_EUTRAN_RESEL_END_SUCCESS = 0,
  LTE_RRC_LOG_FROM_EUTRAN_RESEL_END_FAIL_CAUSE_UNKNOWN = 1,
  LTE_RRC_LOG_FROM_EUTRAN_RESEL_END_FAIL_CAUSE_CELL_BARRED = 2,
  LTE_RRC_LOG_FROM_EUTRAN_RESEL_END_FAIL_CAUSE_FREQ_BARRED = 3,
  LTE_RRC_LOG_FROM_EUTRAN_RESEL_END_ABORT_DUE_TO_SVC_REQ = 4,
  LTE_RRC_LOG_FROM_EUTRAN_RESEL_END_ABORT_DUE_TO_MANUAL_PLMN_SRCH_REQ = 5,
  LTE_RRC_LOG_FROM_EUTRAN_RESEL_END_ABORT_DUE_TO_LTE_STACK_DEACTIVATION = 6,
  LTE_RRC_LOG_FROM_EUTRAN_RESEL_END_MAX
}lte_rrc_log_from_eutran_resel_end_cause_e;

/*! From E-UTRAN 'Handover Ended' cause enumeration */
typedef enum
{
  LTE_RRC_LOG_FROM_EUTRAN_HO_END_SUCCESS      = 0,
  LTE_RRC_LOG_FROM_EUTRAN_HO_END_FAIL         = 1,
  LTE_RRC_LOG_FROM_EUTRAN_HO_END_ABORTED      = 2,
  LTE_RRC_LOG_FROM_EUTRAN_HO_END_MAX
}lte_rrc_log_from_eutran_ho_end_cause_e;

/*! From E-UTRAN 'CCO Ended' cause enumeration */
typedef enum
{
  LTE_RRC_LOG_FROM_EUTRAN_CCO_END_SUCCESS = 0,
  LTE_RRC_LOG_FROM_EUTRAN_CCO_END_FAIL_CAUSE_ACQ = 1,
  LTE_RRC_LOG_FROM_EUTRAN_CCO_END_FAIL_CAUSE_RACH = 2,
  LTE_RRC_LOG_FROM_EUTRAN_CCO_END_FAIL_CAUSE_ABORTED_BY_LTE_DUE_TO_HIGH_PRI_REQ = 3,
  LTE_RRC_LOG_FROM_EUTRAN_CCO_END_FAIL_CAUSE_ABORTED_BY_GSM_DUE_TO_HIGH_PRI_REQ = 4,
  LTE_RRC_LOG_FROM_EUTRAN_CCO_END_MAX
}lte_rrc_log_from_eutran_cco_end_cause_e;

/*! From E-UTRAN 'RTT Ended' cause enumeration */
typedef enum
{
  LTE_RRC_LOG_FROM_EUTRAN_RTT_END_SUCCESS = 0,
  LTE_RRC_LOG_FROM_EUTRAN_RTT_END_FAILURE = 1,
  LTE_RRC_LOG_FROM_EUTRAN_RTT_END_ABORTED = 2,
  LTE_RRC_LOG_FROM_EUTRAN_RTT_END_MAX
}lte_rrc_log_from_eutran_rtt_end_cause_e;

/*! RRC Meas Obj type enumeration */
typedef enum
{
  LTE_RRC_LOG_MEAS_OBJ_EUTRA = 0,
  LTE_RRC_LOG_MEAS_OBJ_UTRA = 1,
  LTE_RRC_LOG_MEAS_OBJ_GERAN = 2,
  LTE_RRC_LOG_MEAS_OBJ_CDMA = 3,
  LTE_RRC_LOG_MEAS_OBJ_MAX
}lte_rrc_log_meas_obj_type_e;

/*! RRC Meas Obj mode type enumeration */
typedef enum
{
  LTE_RRC_LOG_MEAS_OBJ_MODE_FDD = 0,
  LTE_RRC_LOG_MEAS_OBJ_MODE_TDD = 1,
  LTE_RRC_LOG_MEAS_OBJ_MODE_NOT_APPLICABLE = 2,
  LTE_RRC_LOG_MEAS_OBJ_MODE_MAX
}lte_rrc_log_meas_obj_mode_type_e;

/*! RRC MDT Logging Duration */
typedef enum
{
  LTE_RRC_MDT_LOG_DURATION_10 = 1,
  LTE_RRC_MDT_LOG_DURATION_20,
  LTE_RRC_MDT_LOG_DURATION_40,
  LTE_RRC_MDT_LOG_DURATION_60,
  LTE_RRC_MDT_LOG_DURATION_90,
  LTE_RRC_MDT_LOG_DURATION_120,
  LTE_RRC_MDT_LOG_DURATION_MAX
}lte_rrc_log_mdt_duration_e;


#ifdef _WIN32
#pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

/*! @brief eMBMS OOS warning indication log structure
*/
typedef PACK(struct)
{
  uint8 log_packet_ver;  /*!< Log packet version */

  uint8 reason;  /*!< OOS reason - use lte_rrc_emp_warn_reason_e */

  uint8 num_affected_sessions;  /*!< Number of affected sessions */
  struct{
    uint8 plmn_id[3];  /*!< PLMN ID */
    uint8 service_id[3];  /*!< Service ID */
    uint8 session_id_present; /*!< Session ID present or not */
    uint8 session_id; /*!< Session ID */
    uint8 mrb_id;  /*!< MRB ID */
  } affected_session_info_list[LTE_MAX_ACTIVE_MRB];

  uint8 num_affected_areas;  /*!< Number of affected MBSFN areas */
  struct{
    uint8 mbsfn_area_id;  /*!< MBSFN area ID */
  } affected_mbsfn_areas[LTE_EMBMS_MAX_MBSFN_AREA];
} lte_rrc_log_embms_oos_warning_ind_s;


/*! @brief MIB log structure
*/
typedef PACK(struct)
{
  uint8 log_packet_ver;  /*!< Log packet version */
  uint16 phy_cell_id;  /*!< Physical cell ID */
  uint32 freq;  /*!< DL Frequency of the cell */
  uint16 sfn;  /*!< SFN on which MIB was received */
  uint8 num_tx_antennas;  /*!< Number of Tx Antennas */
  uint8 dl_bw;  /*!< Downlink bandwidth */
} lte_rrc_log_mib_s;

/*! @brief eMBMS bearer list info log structure
*/
typedef PACK(struct)
{
  uint8 log_packet_ver;  /*!< Log packet version */

  uint8 num_modified_services;  /*!< Number of modified services */
  struct{
    uint8 plmn_id[3];  /*!< PLMN ID */
    uint8 service_id[3];  /*!< Service ID */
    uint8 session_id_present; /*!< Session ID present or not */
    uint8 session_id; /*!< Session ID */
    uint8 bearer_action;  /*!< Bearer action - lte_rrc_log_embms_bearer_action_e */
  } modified_bearer_list[LTE_MAX_ACTIVE_MRB];

  uint8 num_active_bearers;  /*!< Number of active bearers */
  struct{
    uint8 plmn_id[3];  /*!< PLMN ID */
    uint8 service_id[3];  /*!< Service ID */
    uint8 session_id_present; /*!< Session ID present or not */
    uint8 session_id; /*!< Session ID */
    uint8 mbsfn_area_id;  /*!< MBSFN area ID */
    uint8 mrb_id; /*!< MRB ID */
    uint8 pmch_id;  /*!< PMCH ID */
    uint8 lc_id;  /*!< LC ID */
  } active_bearer_list[LTE_MAX_ACTIVE_MRB];
} lte_rrc_log_embms_bearer_list_info_s;

/*! @brief Idle Meas Config info log structure
*/
typedef PACK(struct)
{
  uint8 log_packet_ver;  /*!< Log packet version */

  uint32 serving_freq; /*!< Serving Frequency */

  uint8 num_freq_lyr;  /*!< Number of neighbor frequencies in the array below */
  PACK (struct){
    uint8 rat;  /*!< RAT type - sys_sys_mode_e_type*/
    int32 band; /*!< Band type - sys_band_class_e_type */
    uint32 frequency;  /*!< Frequency */
    int8 priority; /*!< Priority */
  } freq_lyr_list[LTE_CPHY_MEAS_MAX_ALLOWED_FREQ_LAYERS];

  uint16 t325_remaining_in_sec;  /*!< Timer 325 remaining (in Seconds) */

  /*!< Indication of whether Deprioritization of entire EUTRAN is present or not */
  boolean depri_all_earfcn;

  uint8 num_depri_list;  /*!< Number of frequencies in deprioritization list */
  PACK (struct){
    uint32 frequency;  /*!< frequency */
  } depri_list[LTE_MAX_NUM_DEPRI_FREQ];

} lte_rrc_log_idle_meas_config_info_s;

/*! @brief RRC eMBMS OOS warning indication event structure
*/
typedef PACK(struct)
{
  uint8 reason;  /*!< OOS reason - use lte_rrc_emp_warn_reason_e */
  uint8 num_affected_sessions;  /*!< Number of affected sessions due to OOS */
} lte_rrc_log_embms_oos_warn_ind_event_s;

/*! @brief RRC eMBMS active TMGI list indication event structure
*/
typedef PACK(struct)
{
  uint8 num_active_sessions;  /*!< Number of active sessions */
} lte_rrc_log_embms_act_tmgi_list_ind_event_s;

/*! @brief RRC eMBMS available TMGI list indication event structure
*/
typedef PACK(struct)
{
  uint8 num_avail_sessions;  /*!< Number of available sessions */
} lte_rrc_log_embms_avail_tmgi_list_ind_event_s;

/*! @brief RRC eMBMS MCCH change notification event structure
*/
typedef PACK(struct)
{
  uint16 freq;  /*!< DL-ARFCN */
  uint16 phy_cell_id; /*!< Physical Cell Identity */
  uint8 mask; /*!< PDCCH bit mask indicates which MCCH will change */
  uint16 sfn; /*!< SFN when RRC receives MCCH change notification */
} lte_rrc_log_embms_mcch_change_notification_event_s;

/*! @brief RRC eMBMS MCCH change notification event structure (Ext EARFCN)
*/
typedef PACK(struct)
{
  uint32 freq;  /*!< DL-ARFCN */
  uint16 phy_cell_id; /*!< Physical Cell Identity */
  uint8 mask; /*!< PDCCH bit mask indicates which MCCH will change */
  uint16 sfn; /*!< SFN when RRC receives MCCH change notification */
} lte_rrc_log_embms_mcch_change_notification_event_ext_earfcn_s;


/*! @brief RRC eMBMS available SAI list indication event structure
*/
typedef PACK(struct)
{
  uint8 num_sai;  /*!< Number of available SAIs across all frequencies */
} lte_rrc_log_embms_avail_sai_list_ind_event_s;

/*! @brief RRC eMBMS Interest Indication list indication event structure
*/
typedef PACK(struct)
{
  boolean  mbms_preferred; /*!< If multicast is preferred over unicast */
} lte_rrc_log_embms_interest_ind_event_s;

/*! @brief RRC eMBMS signal strength reporting event structure
*/
typedef PACK(struct)
{
  uint8 num_active_mbsfn_areas;
} lte_rrc_log_embms_sig_strngth_rep_event_s;

/*! @brief RRC Timer Status event structure
*/
typedef PACK(struct)
{
  uint8 timer;  /*!< Timer Name - use lte_rrc_tmri_type_e */
  uint64 remain_time;  /*!< Remaining time in ms */
  uint8 status;  /*!< timer status:  Started, Stopped or Expired - lte_rrc_log_timer_status_e*/
} lte_rrc_log_timer_status_event_s;

/*! @brief RRC State Change event structure
*/
typedef PACK(struct)
{
  uint8 rrc_state;  /*!< RRC Controller state - use lte_rrc_log_ctrl_state_e */
} lte_rrc_log_state_change_event_s;

/*! @brief RRC State Change event structure
*/
typedef PACK(struct)
{
  uint8 rrc_state_change_trigger; /*!< Trigger for RRC state change - lte_rrc_log_ctrl_state_change_trigger_e */
 
} lte_rrc_log_state_change_trigger_event_s;


/*! @brief RRC Out Of Service Area event structure
*/
typedef PACK(struct)
{
  uint16 counter;  /*!< Running counter which indicates how many OOSs UE has seen */
} lte_rrc_log_out_of_service_event_s;

/*! @brief RRC Radio Link Failure event structure
*/
typedef PACK(struct)
{
  uint16 counter;  /*!< Running counter which indicates how many RLFs UE has seen 
                        in the current RRC connection */
} lte_rrc_log_radio_link_failure_event_s;

/*! @brief RRC Radio Link Failure statistics event structure
*/
typedef PACK(struct)
{
  uint16 rlf_count_since_rrc_connected;  /*!< Running counter which indicates how many RLFs 
                                            UE has seen since UE went into connected state */

  uint16 rlf_count_since_lte_active;  /*!< Running counter which indicates how many RLFs 
                                           UE has seen since LTE stack got activated */
  uint16 cause; /*!< Cause for RLF - lte_rrc_cre_cause_e*/

} lte_rrc_log_radio_link_failure_stat_event_s;

/*! @brief SFN event structure
*/
typedef PACK(struct)
{
  uint16 sub_fn;  /*!< Sub Frame Number */
  uint16 sys_fn;  /*!< System Frame Number */
} lte_rrc_log_sfn_event_s;


/*! @brief RRC Downlink Message event structure
*/
typedef PACK(struct)
{
  uint8 lc_type;         /*!< Logical channel type */
  uint8 message_type;       /*!< Downlink msg type */
} lte_rrc_log_dl_msg_event_s;

/*! @brief RRC Uplink Message event structure
*/
typedef PACK(struct)
{
  uint8 lc_type;         /*!< Logical channel type */
  uint8 message_type;  /*!< uplink msg type */
} lte_rrc_log_ul_msg_event_s;

/*! @brief RRC Uplink Message for Meas Report event structure
*/
typedef PACK(struct)
{
  uint8 mea_obj_type; /*!< Meas Obj type: lte_rrc_log_meas_obj_type_e*/

} lte_rrc_log_ul_msg_meas_report_event_s;

/*! @brief RRC Uplink Message for Meas Report with Measobj mode event structure
*/
typedef PACK(struct)
{
  uint8 mea_obj_type; /*!< Meas Obj type: lte_rrc_log_meas_obj_type_e*/
  uint8 mea_obj_mode_type; /*!< Meas Obj mode type: lte_rrc_log_meas_obj_mode_type_e*/

} lte_rrc_log_ul_msg_meas_report_measobj_mode_event_s;

/*! @brief RRC Uplink Message for Meas Report EXT event structure
*/
typedef PACK(struct)
{
  uint8 meas_id; /*!< Meas Id*/
  uint8 meas_obj_id; /*!< Meas Obj Id*/
  uint8 rpt_cfg_id;  /*!< Report Cfg Id*/
  uint8 mea_obj_type; /*!< Meas Obj type: lte_rrc_log_meas_obj_type_e*/
  uint8 mea_obj_mode_type; /*!< Meas Obj mode type: lte_rrc_log_meas_obj_mode_type_e*/

} lte_rrc_log_ul_msg_meas_report_ext_event_s;


/*! @brief RRC New Cell Ind event structure for EXT EARFCN
*/
typedef PACK(struct)
{
  uint8 cause;  /*!< Mobility cause: lte_rrc_log_new_cell_cause_e */
  uint32 freq;  /*!< DL-ARFCN */
  uint16 phy_cell_id; /*!< Physical Cell Identity */
} lte_rrc_log_new_cell_ind_ext_earfcn_s;

/*! @brief RRC New Cell Ind event structure
*/
typedef PACK(struct)
{
  uint8 cause;  /*!< Mobility cause: lte_rrc_log_new_cell_cause_e */
  uint16 freq;  /*!< DL-ARFCN */
  uint16 phy_cell_id; /*!< Physical Cell Identity */
} lte_rrc_log_new_cell_ind_s;


/*! @brief RRC Configure Security event structure
*/
typedef PACK(struct)
{
  uint8 status;/*!< AS Security Configuration Status*/
               /*!< 0 indicates success. 1 indicates failure*/
}lte_rrc_log_security_config_s;

/*! @brief RRC Cell re-selction started event structure
*/
typedef PACK(struct)
{
  uint16 freq;  /*!< DL-ARFCN of target cell */
  uint16 phy_cell_id; /*!< Physical Cell Identity of target cell */
} lte_rrc_log_cell_resel_started_event_s;

/*! @brief RRC Cell re-selction started event structure (Ext EARFCN)
*/
typedef PACK(struct)
{
  uint32 freq;  /*!< DL-ARFCN of target cell */
  uint16 phy_cell_id; /*!< Physical Cell Identity of target cell */
} lte_rrc_log_cell_resel_started_event_ext_earfcn_s;


/*! @brief RRC Cell re-selction failure event structure
*/
typedef PACK(struct)
{
  uint16 freq;  /*!< DL-ARFCN of tried new cell */
  uint16 phy_cell_id; /*!< Physical Cell Identity of tried new cell */
  uint8 cause;  /*!< Failure reason - lte_rrc_log_resel_fail_cause_e */
} lte_rrc_log_cell_resel_failure_event_s;

/*! @brief RRC Cell re-selction failure event structure (Ext EARFCN)
*/
typedef PACK(struct)
{
  uint32 freq;  /*!< DL-ARFCN of tried new cell */
  uint16 phy_cell_id; /*!< Physical Cell Identity of tried new cell */
  uint8 cause;  /*!< Failure reason - lte_rrc_log_resel_fail_cause_e */
} lte_rrc_log_cell_resel_failure_event_ext_earfcn_s;


/*! @brief RRC Handover failure event structure
*/
typedef PACK(struct)
{
  uint16 freq;  /*!< DL-ARFCN */
  uint16 phy_cell_id; /*!< Physical Cell Identity */
  lte_rrc_ho_failure_cause_e cause;  /*!< Failure reason */
} lte_rrc_log_ho_failure_event_s;

/*! @brief RRC Handover failure event structure (Ext EARFCN)
*/
typedef PACK(struct)
{
  uint32 freq;  /*!< DL-ARFCN */
  uint16 phy_cell_id; /*!< Physical Cell Identity */
  lte_rrc_ho_failure_cause_e cause;  /*!< Failure reason */
} lte_rrc_log_ho_failure_event_ext_earfcn_s;


/*! @brief RRC Paging DRX cycle event structure
*/
typedef PACK(struct)
{
  uint16 drx_cycle;  /*!< UE paging DRX cycle - units in ms */
} lte_rrc_log_paging_drx_cycle_event_s;

/*! @brief RRC SIB read failure event structure
*/
typedef PACK(struct)
{
  uint16 freq;  /*!< DL-ARFCN */
  uint16 phy_cell_id; /*!< Physical Cell Identity */
  uint16 rcvd_sib_mask;  /*!< Bit mask of SIBS rxed so far - LSB is MIB, LSB1 is SIB1 etc..  */
} lte_rrc_log_sib_read_failure_event_s;

/*! @brief RRC SIB read failure event structure (Ext EARFCN)
*/
typedef PACK(struct)
{
  uint32 freq;  /*!< DL-ARFCN */
  uint16 phy_cell_id; /*!< Physical Cell Identity */
  uint16 rcvd_sib_mask;  /*!< Bit mask of SIBS rxed so far - LSB is MIB, LSB1 is SIB1 etc..  */
} lte_rrc_log_sib_read_failure_event_ext_earfcn_s;


/*! @brief RRC IRAT handover from EUTRAN event structure
*/
typedef PACK(struct)
{
  uint8 rat_proc;  /*!< See lte_rrc_log_irat_ho_proc_type_e */
} lte_rrc_log_irat_ho_from_eutran_event_s;

/*! @brief RRC IRAT handover from EUTRAN failure event structure
*/
typedef PACK(struct)
{
  uint8 cause;  /*!< See lte_rrc_log_irat_ho_fail_cause_e */
} lte_rrc_log_irat_ho_from_eutran_failure_event_s;

/*! @brief RRC IRAT handover from EUTRAN end event structure
*/
typedef PACK(struct)
{
  uint8 cause;  /*!< See lte_rrc_log_from_eutran_ho_end_cause_e */
} lte_rrc_log_irat_ho_from_eutran_ended_event_s;

/*! @brief RRC IRAT re-selection from EUTRAN event structure
*/
typedef PACK(struct)
{
  uint8 rat;          /*!< RAT type - lte_rrc_log_rat_type_e */
  int32 band;        /*!< Frequency band indicator */
  uint16 freq;        /*!< Frequency */
  /*!< Physical cell id a.k.a Scrambling code(W), Pn(1X/DO) BSIC_Id (G) */
  uint16 phy_cell_id; 
} lte_rrc_log_irat_resel_from_eutran_started_event_s;

/*! @brief RRC IRAT RTT from EUTRAN started event structure
*/
typedef PACK(struct)
{
  uint8 rat;  /*!< RAT type - lte_rrc_log_rat_type_e */
} lte_rrc_log_irat_rtt_from_eutran_started_event_s;

/*! @brief RRC IRAT reselection from EUTRAN failure event structure
*/
typedef PACK(struct)
{
  uint8 rat;    /*!< RAT type - lte_rrc_log_rat_type_e */
  uint8 cause;  /*!< End reason - lte_rrc_log_from_eutran_resel_end_cause_e */
} lte_rrc_log_irat_resel_from_eutran_ended_event_s;

/*! @brief RRC IRAT reselection from EUTRAN failure event structure
*/
typedef PACK(struct)
{
  uint8 rat;    /*!< RAT type - lte_rrc_log_rat_type_e */
  uint8 cause;  /*!< End reason - lte_rrc_log_from_eutran_rtt_end_cause_e */
} lte_rrc_log_irat_rtt_from_eutran_ended_event_s;

/*! @brief RRC IRAT redirection from EUTRAN started event structure
*/
typedef PACK(struct)
{
  uint8 rat;  /*!< RAT type - lte_rrc_log_rat_type_e */
} lte_rrc_log_irat_redir_from_eutran_started_event_s;

/*! @brief RRC IRAT redirection from EUTRAN complete event structure
*/
typedef PACK(struct)
{
  uint8 rat;  /*!< RAT type - lte_rrc_log_rat_type_e */
  uint8 cause;  /*!< End reason - lte_rrc_log_from_eutran_redir_end_cause_e */
} lte_rrc_log_irat_redir_from_eutran_ended_event_s;

/*! @brief RRC IRAT redirection from EUTRAN started event structure
*/
typedef PACK(struct)
{
  uint8 rat;  /*!< RAT type - lte_rrc_log_rat_type_e */
  int32 band;        /*!< Frequency band indicator */
  uint16 freq;        /*!< Frequency */
  /*!< Physical cell id a.k.a Scrambling code(W), Pn(1X/DO) BSIC_Id (G) */
  uint16 phy_cell_id; 
  boolean                  nw_ctrl_order_is_present;
  uint8                    nw_ctrl_order;/*!<Network Control Order */
  boolean                  proc_for_csfb; /*!<Boolean to indicate if procedure is for CSFB */
} lte_rrc_log_irat_cco_from_eutran_started_event_s;

/*! @brief RRC IRAT cco from EUTRAN failure event structure
*/
typedef PACK(struct)
{
  uint8 rat;    /*!< RAT type - lte_rrc_log_rat_type_e */
  uint8 cause;  /*!< End reason - lte_rrc_log_from_eutran_cco_end_cause_e */
} lte_rrc_log_irat_cco_from_eutran_ended_event_s;

/*! @brief RRC IRAT Handover to EUTRAN start event structure
*/
typedef PACK(struct)
{
  uint8 rat;          /*!< RAT type - lte_rrc_log_rat_type_e */
  int32 band;        /*!< Frequency band indicator */
  uint32 freq;        /*!< Frequency */
  /*!< Physical cell id of LTE*/
  uint16 phy_cell_id; 
} lte_rrc_log_irat_ho_to_eutran_started_event_s;

/*! @brief RRC cell blacklist event structure
*/
typedef PACK(struct)
{
  uint16 freq;  /*!< DL-ARFCN */
  uint16 phy_cell_id; /*!< Physical Cell Identity */
} lte_rrc_log_cell_blacklist_event_s;

/*! @brief RRC cell blacklist event structure (Ext EARFCN)
*/
typedef PACK(struct)
{
  uint32 freq;  /*!< DL-ARFCN */
  uint16 phy_cell_id; /*!< Physical Cell Identity */
} lte_rrc_log_cell_blacklist_event_ext_earfcn_s;


/*! @brief RRC PLMN Search RAT change event structure
*/
typedef PACK(struct)
{
  uint8 rat;  /*!< RAT type - lte_rrc_log_rat_type_e */
} lte_rrc_log_search_rat_search_start_event_s;

/*! @brief RRC PLMN Stop RAT change event structure
*/
typedef PACK(struct)
{
  uint8 rat;  /*!< RAT type - lte_rrc_log_rat_type_e */
} lte_rrc_log_search_rat_search_stop_event_s;

/*! @brief RRC PLMN Search BG to FG conversion event structure
*/
typedef PACK(struct)
{
  uint8 rat;  /*!< RAT type - lte_rrc_log_rat_type_e */
} lte_rrc_log_search_bg_to_fg_event_s;

/*! @brief RRC PLMN Search late IRAT response event structure
*/
typedef PACK(struct)
{
  uint8 rat;  /*!< RAT type - lte_rrc_log_rat_type_e */
  uint32 time;  /*!< Delayed time */
} lte_rrc_log_search_rat_late_rsp_event_s;

#define LTE_RRC_SYS_PRIORITY_PLMN_LIST_MAX_LENGTH 256
#define LTE_RRC_SYS_MAX_ACQ_PREF_RAT_LIST_NUM 15
#define LTE_RRC_SYS_PLMN_LIST_MAX_LENGTH 40

/*! @brief This structure is for logging PLMN Search Request from NAS or from 3GPP RATs
*/
typedef PACK(struct)
{
    uint32 log_packet_ver:8; ///< Log packet version. Range: 0...255. Version 24
    /** 
      Allowed access on the cell based on network select mode indicated by NAS
      1 – Automatic
      2 - Manual
    */
    uint32 network_select_mode:8; 
    /** Service Search type indicated by NAS.
      1 – None
      2 – Background Automatic
      3 - Background Manual
      4 – IRAT PLMN
      5 – CSG
      6 – Priority PLMN
    */
    uint32 service_search_type:8;
    uint32 num_rats:8; ///< Range 0-15
    /// RAT_LIST – contains 15 entries of the following elements of which only NUM_RATS entries are valid
    struct {
     /** 1 – GSM
       2 – WCDMA
       3 – LTE
       4 - TDS 
       */
        uint32 rat;
        uint32 reserved; ///< Reserved
        uint64 band_cap; ///< Bands 1-64 supported by the RAT
        uint64 band_cap_65_128; ///< Bands 65-128 applicable for LTE only
        uint64 band_cap_129_192; ///< Bands 129-192 applicable for LTE only
        uint64 band_cap_193_256; ///< Bands 193-256 applicable for LTE only
    } rat_list[LTE_RRC_SYS_MAX_ACQ_PREF_RAT_LIST_NUM];

    uint32 num_plmns; ///< Range 0-256
    /// PLMN_LIST – contains 256 entries of the following elements of which only NUM_PLMNS entries are valid
    struct {
     /** 1 – GSM
       2 – WCDMA
       3 – LTE
       4 - TDS 
       */
        uint8 rat;
     /** MCC0 – Lower 4 bits of PLMN[0]
        MCC1 – Upper 4 bits of PLMN[0]
        MCC2 – Lower 4 bits of PLMN[1]
        MNC0 – Lower 4 bits of PLMN[2]
        MNC1 – Upper 4 bits of PLMN[2]
        MNC2 – Upper 4 bits of PLMN[1]
        */
        uint8 plmn[3];
    } plmn_list[LTE_RRC_SYS_PRIORITY_PLMN_LIST_MAX_LENGTH];
} lte_rrc_log_plmn_search_req_s;

/*! @brief Paging UE ID log structure
*/
typedef PACK(struct)
{
  uint8 version;
  uint8 num_paging_records; /* Max Page Record is 16 */
  PACK(struct)
  {
    uint8 cn_domain_type;
    uint8 ue_id_type; 
    boolean is_page_record_for_dut;
    union
    {
      PACK(struct)
      {
        uint32 m_tmsi;
        uint8  mmec;
      }s_tmsi;
      PACK(struct)
      {
        uint8 num_digits;     
        uint8 imsi_digits[21]; 
      }imsi;
    }ue_id_u;
  }paging_record[16];
}lte_rrc_log_paging_ue_id_s;

/*! @brief PLMN search rsp log structure
*/
typedef PACK(struct)
{
    uint32 log_packet_ver:8;  /*!< Log packet version */
    uint32 rat:8; 
    uint32 search_status:8;
    uint32 reserved:8;

    uint32 num_plmns;
    struct {
        uint8 rat;
        uint8 plmn[3];
        uint32 earfcn;
    } plmn_list[LTE_RRC_SYS_PLMN_LIST_MAX_LENGTH];
} lte_rrc_log_plmn_search_rsp_s;

/*! @brief Internal RRC Event
*/
typedef PACK(struct)
{
  uint8 cause;  /*!< cause for event */
} lte_rrc_log_internal_event_0_event_s;

/*! @brief RRC MDT cfg
*/
typedef PACK(struct)
{
  uint8 log_packet_ver;  /*!< Log packet version */

  PACK(struct){    
    PACK(struct){
     uint16 plmn_mcc;  /*!< MCC of PLMN */
     uint8 num_mnc_digits;  /*!< Number of digits in MNC (2 or 3) */
     uint16 plmn_mnc;  /*!< MNC of PLMN */
    }plmn_id;
    uint8 trace_id[3];
  }trace_reference;

  uint8  trace_session_ref[2];
  uint8  tce_id_r10; 
  uint8 absolute_time[6];
  uint8 logging_duration;
  uint8 logging_interval;

  boolean area_cfg_present;
  PACK(struct){
    uint8 num_cgi;
    PACK(struct){
      PACK(struct){
       uint16 plmn_mcc;  /*!< MCC of PLMN */
       uint8 num_mnc_digits;  /*!< Number of digits in MNC (2 or 3) */
       uint16 plmn_mnc;  /*!< MNC of PLMN */
      }plmn_id;
      uint32 cell_identity;    
    }cgi_list[LTE_RRC_MDT_AREA_CFG_MAX_NUM_CGI];
    uint8 num_tac;
    uint16 tac[LTE_RRC_MDT_AREA_CFG_MAX_NUM_TAC];
  }area_cfg;
}lte_rrc_log_mdt_meas_cfg_s;

/*! @brief RRC MDT report
*/
typedef PACK(struct)
{
  uint8 log_packet_ver;  /*!< Log packet version */
  uint64 reports_received;
  uint64 heap_mem_used;
  boolean location_info_present;
  PACK(struct){
    uint8 latitude_sign;
    uint32 deg_latitude;
    int32 deg_longitude;
    boolean eppi_pt_w_alt_present;
    PACK(struct){
      uint8 alt_dir;
      int16 alt;
    } eppi_pt_w_alt_info;
  }location_info;

  uint16 relative_timestamp;
  
  boolean sev_cell_info_present;
  PACK(struct){
   uint16 plmn_mcc;  /*!< MCC of PLMN */
   uint8 num_mnc_digits;  /*!< Number of digits in MNC (2 or 3) */
   uint16 plmn_mnc;  /*!< MNC of PLMN */
   uint32 cell_id;
  }sev_cell_info;
  
  int8 serv_cell_rsrp;
  int8 serv_cell_rsrq;

  uint8 num_meas_neigh_eutra;
  PACK(struct){
    uint32 freq;
    uint8 size;
    PACK(struct){
      uint16 pci;
      boolean cgi_info_present;
      PACK(struct){
        PACK(struct){
          uint16 plmn_mcc;
          uint8 num_mnc_digits;
          uint16 plmn_mnc;
          uint32 cell_id;
        }cell_global_id_eutra;
        uint16 tac;
        boolean plmn_id_list_present;
        PACK(struct){
          uint8 num_plmn_id;
          PACK(struct){
            uint16 plmn_mcc;
            uint8 num_mnc_digits;
            uint16 plmn_mnc;
          }plmn_id[5];
        }plmn_id_list_eutra;
      }cgi_info_eutra;
      boolean rsrp_present;
      int8 rsrp;
      boolean rsrq_present;
      int8 rsrq;
      boolean add_si_info_present;
      PACK(struct){
        boolean csg_member_status_present;
        uint8 csg_member_status;
        boolean csg_id_present;
        uint32 csg_id;
      }add_si_info_eutra;
    }eutra_list[LTE_CPHY_MAX_CELL_REPORT];
  }eutra_neigh[LTE_CPHY_MAX_FREQUENCIES];
  
  uint8 num_meas_neigh_utra;
  uint8 num_meas_neigh_cdma2k;
  union{
  PACK(struct){
    uint16 freq;
    uint8 size;
    PACK(struct){
      uint8 wcdma_mode;
      uint16 psc;
      boolean cgi_info_present;
      PACK(struct){
        PACK(struct){
          uint16 plmn_mcc;
          uint8 num_mnc_digits;
          uint16 plmn_mnc;
          uint32 cell_id;
        }cell_global_id_utra;
        boolean lac_present;
        uint16 lac;
        boolean rac_present;
        uint16 rac;
        boolean plmn_id_list_present;
        PACK(struct){
          uint8 num_plmn_id;
          PACK(struct){
            uint16 plmn_mcc;
            uint8 num_mnc_digits;
            uint16 plmn_mnc;
          }plmn_id[5];
        }plmn_id_list_utra;
      }cgi_info_utra;
      boolean rscp_present;
      int8 cpich_rscp;
      boolean ecno_present;
      int8 cpich_ecno;
      boolean add_si_info_present;
      PACK(struct){
        boolean csg_member_status_present;
        uint8 csg_member_status;
        boolean csg_id_present;
        uint32 csg_id;
      }add_si_info_utra;
      }utra_list[LTE_CPHY_MDT_LOG_MAX_CELL_REPORT];
  }utra_neigh[LTE_CPHY_MAX_FREQUENCIES];
  
    PACK(struct){
      uint8 band_class;
      uint16 freq;
      boolean pre_reg_status_hrpd;
      uint8 size;
      PACK(struct){
        uint16 phy_cell_id;
        boolean cgi_info_present;
        PACK(struct){
          uint32 cgi_1xrtt[2];
          uint32 cgi_hrpd[4];
        }cgi_info_cdma2k;
        boolean pilot_pn_phase_present;
        uint16 pilot_pn_phase;
        uint8 pilot_strength;
      }cdma2k_list[LTE_CPHY_MDT_LOG_MAX_CELL_REPORT];
    }cdma2k_neigh[LTE_CPHY_MAX_FREQUENCIES];
  }selected_rat_u;
  uint8 num_meas_neigh_geran;
  PACK(struct){
    uint16 freq;
    uint8 gsm_band;
    uint8 ncc;
    uint8 bcc;
    boolean cgi_info_present;
    PACK(struct){
      PACK(struct){
          uint16 plmn_mcc;
          uint8 num_mnc_digits;
          uint16 plmn_mnc;
          uint16 lac;
          uint16 cell_id;
      }cell_global_id_geran;
      boolean rac_present;
      uint8 rac;
    }cgi_info_geran;
    uint8 rssi;
  }geran_neigh[LTE_CPHY_MDT_LOG_MAX_CELL_REPORT];
  
}lte_rrc_log_mdt_meas_info_s;

/*! @brief RRC CGI Start INDI event structure
*/
typedef PACK(struct)
{
  uint8 rat;  /*!< RAT type - sys_sys_mode_e_type*/
  uint8 cgi_meas_obj_id;  /*!< CGI Meas obj id*/
  uint16 cphy_irat_cgi_time; /*!< IRAT CGI Time from ML1*/ 
  uint64 t321_remaining_time; /*!< T321 remaining time*/
  uint16 actual_time_avail_for_irat_cgi; /*!< Actual time available for IRAT CGI*/
} lte_rrc_log_cgi_start_indi_event_s;


/*! @brief RRC CGI Info structure
*/
typedef PACK(struct)
{
  uint8 log_packet_ver;  /*!< Log packet version */
  uint8 rat;  /*!< RAT type - sys_sys_mode_e_type*/
  uint8 cgi_meas_obj_id;  /*!< CGI Meas obj id*/
  boolean cgi_info_included; /*!< CGI Info included or not*/
  //EUTRA CGI Info
  PACK(struct)
  {
    uint32 earfcn; /*!< EARFCN of the cell for which CGI info was requested */
    uint16 pci;  /*!< PCI of the cell for which CGI info was requested */
    uint8 cgi_plmn_id[3]; /*!< CGI PLMN ID*/ 
    uint32 cgi_cell_id; /*!< CGI Cell ID*/
    uint8 num_of_shared_plmns;  /*!<Number of shared PLMNs of the CGI Cell */
    PACK (struct)
    {
      uint8 plmn_id[3];  /*!< PLMN ID */
    } shared_plmn_list[LTE_RRC_LOG_EUTRA_MAX_SHARED_PLMN];  /*!<  Shared PLMN list of CGI cell*/
    uint32 csg_id; /*!< CSG ID*/
    uint16 tac; /*!< TAC*/
  } eutra_cgi_info;
  //UTRA CGI Info (W and TDS)
  PACK(struct)
  {
    uint32 uarfcn; /*!< UARFCN of the cell for which CGI info was requested */
    uint32 psc;  /*!< PSC of the cell for which CGI info was requested */
    uint8  cgi_plmn_id[3];  /* !< PLMN ID of the CGI Cell */
    uint32 cgi_cell_id; /* !< Cell ID of the CGI Cell */  
    uint16 lac;  /*!< LAC of the CGI cell */  
    uint8 rac;  /*!< RAC of the CGI Cell */  
    uint8 num_of_shared_plmns;  /*!<Number of shared PLMNs of the CGI Cell */
    PACK (struct)
    {
      uint8 plmn_id[3];  /*!< PLMN ID */
    } shared_plmn_list[LTE_RRC_LOG_UTRA_MAX_SHARED_PLMN];  /*!<  Shared PLMN list of CGI cell*/
    uint32 csg_id;  /*!<  CSG ID of CGI cell*/
    int16 rscp;  /*!<  RSCP of CGI cell*/
    uint8 ecno; /*!<  EcNO of CGI cell*/
  } utra_cgi_info;
  //GSM CGI Info
  PACK(struct)
  {
    uint16 arfcn;  /*!< ARFCN of the cell in which the CGI cell was found */
    uint8 band_ind;  /*!< Band type -geran_grr_band_ind_e*/
    uint8 bcc; /* !< PhyCellIDGERAN BaseStation Colour Code */
    uint8 ncc;  /*!<  PhyCellIDGERAN Network Colour Code */
    uint8 cgi_plmn_id[3];  /* !< PLMN ID of the CGI Cell */
    uint32 cgi_cell_id; /* !< Cell ID of the CGI Cell */
    uint16 lac;  /*!< LAC of the CGI cell */  
    uint8 rac;  /*!< RAC of the CGI Cell */
    uint8 rssi; /*! RSSI (RXLEV), range is 0 - 63 */
  } gsm_cgi_info;
  //CDMA CGI Info (1X and DO)
  PACK(struct)
  {
    uint8 cdma_type; /*!< CDMA type - lte_irat_rat_type_e*/ 
    uint8 band; /*!< Band of the CGI cell*/
    uint16 channel_num;  /*!< Channel number of the CGI cell*/
    PACK (struct)
    {
      /*!< CGI Info which includes ATI, Sector ID and Subnet ID*/
      uint8 cgi_info[LTE_RRC_LOG_DO_CGI_LENGTH_IN_BYTES]; 
    } do_cgi_info;  /*!<  DO CGI Info*/
    PACK (struct)
    {
      uint16 base_id; /*!< Base ID of the CGI cell*/ 
      uint16 sid; /*!< S ID of the CGI cell*/
      uint16 nid; /*!< N ID of the CGI cell*/
    } onex_cgi_info;  /*!<  1X CGI Info*/
     int16 pilot_strength; /*!< Pilot strength of the CGI Cell */
     uint16 pn_offset; /*!< PN Offset of the CGI cell*/
  } cdma_cgi_info;
}lte_rrc_log_cgi_info_s;

/*! @brief RRC Camped Cell Status event structure
*/
typedef PACK(struct)
{
  uint8 status;  /*!< Status: lte_rrc_camped_status_e */
} lte_rrc_log_camped_cell_status_s;


#ifdef _WIN32
#pragma pack(pop) // Revert alignment to what it was previously
#endif

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern uint8 *lte_rrc_log_alloc
(
  lte_rrc_log_id_e log_id,
  uint16 size
);

extern void lte_rrc_log_commit
(
  lte_rrc_log_id_e log_id
);

extern void lte_rrc_log_init
(
  lte_rrc_log_id_e log_id
);

extern boolean lte_rrc_log_check_enable
(
  lte_rrc_log_id_e log_id
);

extern void lte_rrc_log_serv_cell_info
(
  lte_rrc_cell_id_s cell_id,
  uint8 sib_db_key,
  lte_rrc_plmn_s sel_plmn,
  sys_network_selection_mode_e_type sys_network_selection_mode
);

extern void lte_rrc_log_ota_msg
(
  lte_rrc_cell_id_s cell_id,
  lte_rb_id_t rb_id,
  int pdu_num,
  uint16 encoded_msg_len,
  unsigned char *encoded_msg
);

extern void lte_rrc_log_ota_msg_with_sfn
(
  lte_rrc_cell_id_s cell_id,
  lte_rb_id_t rb_id,
  lte_sfn_s sfn,
  int pdu_num,
  uint16 encoded_msg_len,
  unsigned char *encoded_msg
);

extern void lte_rrc_log_ota_msg_with_sfn_sib_bitmask
(
  lte_rrc_cell_id_s cell_id,
  lte_rb_id_t rb_id,
  lte_sfn_s sfn,
  int pdu_num,
  uint16 encoded_msg_len,
  unsigned char *encoded_msg,  
  uint32 sib_mask_in_si
);

void lte_rrc_log_ota_msg_external
( 
  uint16 pdu_num,
  uint16 encoded_msg_len,
  unsigned char *encoded_msg,
  uint8 msg_type,
  uint8 lc_type
);

extern void lte_rrc_log_register_mmal_cb_for_idle_meas
(
  void (*mmal_fn_ptr)(lte_cphy_idle_meas_cfg_req_s  *idle_meas_cfg_req_ptr)
);

extern void lte_rrc_log_register_mmal_cb_for_conn_meas
(
  void (*mmal_fn_ptr)(lte_cphy_conn_meas_cfg_req_s  *conn_meas_cfg_req_ptr)
);

extern void lte_rrc_log_idle_meas_neigh_external
( 
  lte_cphy_idle_meas_cfg_req_s  *idle_meas_cfg_req_ptr  /* Ptr to Idle Meas Config*/
);

extern void lte_rrc_log_conn_meas_neigh_external
( 
  lte_cphy_conn_meas_cfg_req_s  *conn_meas_cfg_req_ptr  /* Ptr to Conn Meas Config*/
);

extern void lte_rrc_log_mib
(
  lte_rrc_cell_id_s cell_id,
  lte_l1_mib_info_param_s *mib,
  lte_l1_sfn_t sfn
);

extern void lte_rrc_log_paging_ue_id 
(
  lte_rrc_ue_id_s *ue_id_ptr,
  boolean *ue_id_valid_ptr,
  lte_rrc_osys_Paging *pg_msg_ptr
);

extern void lte_rrc_log_plmn_search_req
(
 sys_network_selection_mode_e_type sys_network_selection_mode,
 sys_service_search_e_type search_type,
 const sys_rat_pri_list_info_s_type *rat_pri_list,
 const sys_plmn_rat_list_s_type *plmn_rat_list
);

extern void lte_rrc_log_plmn_search_rsp
(
 sys_sys_mode_e_type active_rat,
 lte_rrc_plmn_search_status_e status,
 const sys_detailed_plmn_list_s_type *plmn_rat_list
);

extern void lte_rrc_log_embms_bearer_list_info
(
  lte_rrc_emp_embms_modified_service_s *modified_svc_ptr,
  lte_rrc_emp_embms_active_service_s *active_svc_ptr
);

extern void lte_rrc_log_cgi_info
(
  lte_rrc_log_cgi_info_s *log_data_ptr
);

extern void lte_rrc_log_idle_meas_config_info
(
  lte_cphy_idle_meas_cfg_req_s *idle_meas_cfg_req_ptr,
  lte_rrc_tmr_s *tmr_ptr,
  lte_rrc_lte_depri_info_s *depri_info_ptr
);

extern void lte_rrc_log_idle_meas_config_freq_list_populate
(
  lte_rrc_log_idle_meas_config_info_s *idle_meas_cfg_info_ptr,
  lte_cphy_idle_meas_cfg_req_s *idle_meas_cfg_req_ptr
);

extern void lte_rrc_log_idle_meas_config_depri_list_populate
(
  lte_rrc_log_idle_meas_config_info_s *idle_meas_cfg_info_ptr,
  lte_rrc_tmr_s *tmr_ptr,
  lte_rrc_lte_depri_info_s *depri_info_ptr
);

extern void lte_rrc_log_embms_oos_warning_ind
(
  lte_rrc_emp_embms_oos_warning_ind_info_s *oos_warning_indi_info_ptr
);

extern void lte_rrc_log_mdt_meas_info
(
  lte_cphy_ueinfo_mdt_report_ind_s *rpt_ind_ptr,
  uint16 rel_timestamp,
  lte_rrc_plmn_s serv_plmn,
  uint32 global_cell_id,
  uint64 reports_received,
  uint64 heap_mem_used,
  boolean location_info_present,
  lte_rrc_loc_services_loc_info_s decoded_loc_info
);

extern void lte_rrc_log_mdt_meas_cfg
(
  lte_rrc_mdt_cfg_r10_s *logged_mdt_cfg
);
#endif /* LTE_RRC_LOG_H */
