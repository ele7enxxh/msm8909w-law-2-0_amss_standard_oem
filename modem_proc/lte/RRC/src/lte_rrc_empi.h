
/*!
  @file lte_rrc_empi.h

  @brief
  EMP's internal header file.
*/

/*===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_empi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------   
10/20/11   zz      Moved encoded_pdu_info inside of MCCH db; Added enum for different   
                   modification period; Added boolean flag in MTCH db for re-config
                   and program ends; Added cnf_umid in dd to pair with commands rcvd
                   from ESMGR
10/06/11   zz      Added macros for initialization; Added union for last_cmd_rcvd
09/24/11   zz      Added SFN info in MCCH db, MRB pool
08/24/11   zz      Initial Version
===========================================================================*/

#ifndef LTE_RRC_EMPI_H
#define LTE_RRC_EMPI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>

#include "lte_rrc_int_msg.h"
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_utils.h"
#include "lte_rrc_emp.h"
#include "lte_rrc_esmgr.h"
#include "lte_rrc_osys_asn1util.h"
#include <lte_cphy_msg.h>

/*!< @brief Maximum MRB allowed in EMP  
*/
#define LTE_RRC_EMP_MAX_NUM_MRB 32

/*!< @brief Invalid MCCH db index  
*/
#define LTE_RRC_EMP_INVALID_MCCH_DB_IDX LTE_EMBMS_MAX_MBSFN_AREA

/*!< @brief Invalid MTCH db index  
*/
#define LTE_RRC_EMP_INVALID_MTCH_DB_IDX LTE_MAX_ACTIVE_MRB

/*!< @brief Invalid modification window length   
*/
#define LTE_RRC_EMP_INVALID_MOD_PRD_WIN_LEN 0

/*! @brief Maximum length of an encoded MCCH PDU
*/
#define LTE_RRC_EMP_MAX_MCCH_PDU_LENGTH 200

/*! @brief Minimum MCCH modification period
*/
#define LTE_RRC_EMP_MIN_MCCH_MOD_PRD 512

/*! @brief Maximum MCCH modification period
*/
#define LTE_RRC_EMP_MAX_MCCH_MOD_PRD 1024

/*! @brief Margin sfn for MCCH reading timer   
           (timer value = (one mod prd + LTE_RRC_EMP_TMR_MARGIN_SFN) * 10)
*/
#define LTE_RRC_EMP_MCCH_WT_TMR_MARGIN_SFN 2

/*! @brief Margin time for MCCH reading timer remaining time 
*/
#define LTE_RRC_EMP_MCCH_WT_TMR_SW_PROC_MARGIN 10

/*! @brief Unicast default OOS timer value (ms)
*/
#define LTE_RRC_EMP_DEFAULT_OOS_TMR 5120

/*! @brief Invalid MRB ID
*/
#define LTE_RRC_EMP_INVALID_MRB_ID (LTE_MIN_MTCH_RB_ID + LTE_RRC_EMP_MAX_NUM_MRB + 1)

/*! @brief Invalid notification indicator
*/
#define LTE_RRC_EMP_INVALID_NOTIFICATION_INDICATOR 0xFF

/*! @brief Macro to get first sfn in current modification period
*/
#define EMP_GET_MOD_PRD_FIRST_SFN(sfn, mod_prd_len) ((sfn) & (~(uint32)((mod_prd_len) - 1)))

/*! @brief EFS file path for canned MCCH PDU	 
*/
#define LTE_RRC_EMP_MCCH_EFS_FILENAME "/nv/item_files/modem/lte/rrc/emp/mcch"

/*! @brief EFS file path for canned MCCH timer value (ms) (for testing purpose only)
*/
#define LTE_RRC_EMP_MCCH_TIME_EFS_FILENAME "/nv/item_files/modem/lte/rrc/emp/mcch_time"

/*! @brief EFS file path for configurable OOS/RLF timer value (ms)
*/
#define LTE_RRC_EMP_OOS_TIME_EFS_FILENAME "/nv/item_files/modem/lte/embms_oos_time"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
typedef uint32 lte_rrc_emp_mcch_config_reason_bitmask_t;
typedef uint32 lte_rrc_emp_rcvd_cmd_bitmask_t;
typedef uint32 lte_rrc_emp_rsp_msg_bitmask_t;

/*! @brief Bitmask of received commands from ESMGR
    LTE_RRC_ESMGR_ACT_TMGI_REQI and
    LTE_RRC_ESMGR_AVAIL_TMGI_LIST_REQI could come
    together.
*/
enum
{
  LTE_RRC_EMP_RCVD_CMD_NONE = 0x0,                  /*!< Initialized value */
  LTE_RRC_EMP_RCVD_CMD_ACT_TMGI_REQI = 0x1,         /*!< Received command is LTE_RRC_ESMGR_ACT_TMGI_REQI */
  LTE_RRC_EMP_RCVD_CMD_DEACT_TMGI_REQI = 0x2,       /*!< Received command is LTE_RRC_ESMGR_DEACT_TMGI_REQI */
  LTE_RRC_EMP_RCVD_CMD_AVAIL_TMGI_LIST_REQI = 0x4,  /*!< Received command is LTE_RRC_ESMGR_AVAIL_TMGI_LIST_REQI */
  LTE_RRC_EMP_RCVD_CMD_ACT_DEACT_TMGI_REQI = 0x8    /*!< Received command is LTE_RRC_ESMGR_ACT_DEACT_TMGI_REQI */
};

/*! @brief Bitmask of responsed message to ESMGR
*/
enum
{
  LTE_RRC_EMP_RSP_MSG_NONE = 0x0,                  /*!< Initialized value */
  LTE_RRC_EMP_RSP_MSG_ACT_TMGI_CNFI = 0x1,         /*!< Response message is LTE_RRC_ESMGR_ACT_TMGI_CNFI */
  LTE_RRC_EMP_RSP_MSG_DEACT_TMGI_CNFI = 0x2,       /*!< Response message is LTE_RRC_ESMGR_DEACT_TMGI_CNFI */
  LTE_RRC_EMP_RSP_MSG_AVAIL_TMGI_LIST_INDI = 0x4,  /*!< Response message is LTE_RRC_ESMGR_AVAIL_TMGI_LIST_INDI */
  LTE_RRC_EMP_RSP_MSG_DEACT_TMGI_INDI = 0x8,       /*!< Response message is LTE_RRC_ESMGR_DEACT_TMGI_INDI */
  LTE_RRC_EMP_RSP_MSG_ACT_DEACT_TMGI_CNFI = 0x10   /*!< Response message is LTE_RRC_ESMGR_ACT_DEACT_TMGI_CNFI */
};

/*! @brief Enum to indicate which mod prd window is being referred to
*/
typedef enum
{
  LTE_RRC_EMP_MOD_PRD_WIN_CURR, /*!< Current mod prd window */
  LTE_RRC_EMP_MOD_PRD_WIN_NEW   /*!< New mod prd window */
} lte_rrc_emp_mod_prd_win_e;

/*! @brief Bitmask of MCCH configuration reason
*/
enum
{
  LTE_RRC_EMP_CFG_REASON_NONE = 0x0,                  /*!< Initilization value */
  LTE_RRC_EMP_CFG_REASON_ACT_TMGI_REQI = 0x1,         /*!< MCCH config reason is activate TMGI request, search for the TMGI */
  LTE_RRC_EMP_CFG_REASON_AVAIL_TMGI_LIST_REQI = 0x2,  /*!< MCCH config reason is available TMGI list request */
  LTE_RRC_EMP_CFG_REASON_SIB13_UPDATE = 0x4,          /*!< MCCH config because SIB13 changes */
  LTE_RRC_EMP_CFG_REASON_MCCH_MOD_PRD_MONITOR = 0x8,  /*!< MCCH need to be monitored and configured every modification period */
  LTE_RRC_EMP_CFG_REASON_MCCH_UPDATE = 0x10,          /*!< Continue reading MCCH for the first time activation or 
                                                           need to update MCCH PDU in db when sib13 changes */
  LTE_RRC_EMP_CFG_REASON_CHANGE_NOTIFICATION = 0x20   /*!< MCCH config reason is MCCH change notification */
};

/*! @brief Indicates EMP's per RB state
*/
typedef enum
{
  LTE_RRC_EMP_RB_INACTIVE,            /*!< 0 Inactive - Initial value for MCCH/MTCH, no active services for MCCH */
  LTE_RRC_EMP_RB_WAIT_FOR_SETUP_CNF,  /*!< 1 Wait for cfg_cnf - MCCH/MTCH setup cfg is sent to LLC, transition from INACTIVE to ACTIVE */
  LTE_RRC_EMP_RB_ACTIVE,              /*!< 2 Active - cfg_cnfi is rcvd for MCCH/MTCH setup. For MCCH means actively reading sib13 or MCCH PDU */
  LTE_RRC_EMP_RB_WAIT_FOR_RELEASE,    /*!< 3 Wait for release - MCCH/MTCH release cfg is sent to LLC, transition from ACTIVE to INACTIVE*/
  LTE_RRC_EMP_RB_WAIT_FOR_RECFG,      /*!< 4 Wait for recfg - MCCH/MTCH reconfig cfg is sent to LLC, transition from ACTIVE to ACTIVE */
  LTE_RRC_EMP_RB_WAIT_FOR_RECOVERY,   /*!< 5 Wait for recovery - Applicable only for MTCH, waiting for recovery from OOS */
} lte_rrc_emp_rb_state_e;

/*! @brief Indicates which timer is running
*/
typedef enum
{
  LTE_RRC_EMP_TMR_NONE,           /*!< 0 None - Initial value */
  LTE_RRC_EMP_TMR_MCCH,           /*!< 1 MCCH */
  LTE_RRC_EMP_TMR_OOS,            /*!< 2 OOS/RLF timer per MBSFN area */
} lte_rrc_emp_tmr_e;

/*! @brief Indicates which timer is running
*/
typedef enum
{
  LTE_RRC_EMP_OOS_RECOVERY_NONE,                /*!< 0 - Initail value */
  LTE_RRC_EMP_OOS_RECOVERY_UNICAST,             /*!< 1 Unicast OOS recovery */
  LTE_RRC_EMP_OOS_RECOVERY_MULTICAST,           /*!< 2 Multicast OOS recovery */
} lte_rrc_emp_oos_recovery_type_e;

/*! @brief This structure stores an encoded MCCH PDU
*/
typedef struct
{
  /*!< Size of encoded PDU in bytes */
  uint32 encoded_pdu_size;
  /*!< Encoded PDU */
  uint8 encoded_pdu[LTE_RRC_EMP_MAX_MCCH_PDU_LENGTH];
  /*!< Encoded PDU pointer - used when encoded msg is longer than 
       LTE_RRC_MAX_MCCH_PDU_LENGTH */
  uint8 *encoded_pdu_ptr;
} lte_rrc_emp_encoded_pdu_info_s;

/*! @brief This structure holds MRB ID
*/
typedef struct
{
  /*!< Boolean indicates if mrb is used or not */
  boolean is_used;
  /*!< MRB ID */
  lte_rb_id_t mrb_id;

} lte_rrc_emp_mrb_id_s;

/*! @brief This structure holds MCCH db
*/
typedef struct
{
  /*!< MCCH RB State - used as validation flag
       - MCCH RB state is MAX: Invalid
       - Other states: Valid */
  lte_rrc_emp_rb_state_e mcch_state;
  /*!< MCCH config reason bitmask 
       - MCCHs config are handled according to this bitmask
       - act_tmgi_reqi and avail_tmgi_list_reqi could be handled together */
  lte_rrc_emp_mcch_config_reason_bitmask_t mcch_config_reason_bitmask;
  /*!< MBSFN Area ID */
  lte_mbsfn_area_id_t mbsfn_area_id;
  /*!< Notification Indicator */
  uint8 notification_indicator;
  /*!< Number of active MTCHs per MCCH */
  uint8 num_mtch_active;
  /*!< Encoded MCCH PDU info */
  lte_rrc_emp_encoded_pdu_info_s encoded_pdu_info;
  /*!< Boolean flag indicating if decoded MCCH PDU is present or not */
  boolean mcch_pdu_is_present;
  /*!< Current stored MCCH PDU */
  lte_rrc_osys_MCCH_Message *curr_mcch_pdu_ptr;

  /**************************************************************************/

  /*!< Modification period window length (in SFN) */
  uint16 mod_period_window_length;
  /*!< SFN where current modification window starts */
  lte_l1_sfn_t curr_window_start;
  /*!< SFN where current modification window ends */
  lte_l1_sfn_t curr_window_end;
  /*!< SFN where last MCCH PDU is rcvd */
  lte_l1_sfn_t last_rcvd_mcch_sfn;
  /*!< MCCH is rcvd in current modification or not */
  boolean curr_pdu_rcvd;

  /**************************************************************************/

  /*!< MCCH reading timer value */
  lte_rrc_tmr_s wt_tmr;
  /*!< Gaurd timer for reading MCCH */
  lte_rrc_emp_tmr_e mcch_reading_tmr;
  /*!< Remaining time of MCCH reading timer */
  uint32 rem_time;

} lte_rrc_emp_mcch_db_s;

/*! @brief This structure holds MTCH db
*/
typedef struct
{
  /*!< MTCH RB State - used as validation flag
       - MTCH RB state is MAX: Invalid
       - Other states: Valid */
  lte_rrc_emp_rb_state_e mtch_state;
  /*!< MBSFN Area ID */
  lte_mbsfn_area_id_t mbsfn_area_id; 
  /*!< MRB ID */
  lte_rb_id_t mrb_id;
  /*!< PMCH ID */
  lte_pmch_id_t pmch_id;
  /*!< LC ID */
  lte_lc_id_t lc_id;
  /*!< Includes TMGI and session ID */
  lte_rrc_embms_session_info_s session_info;
  /*!< Boolean indicates if program is end - set only when 
       TMGI is removed from MCCH PDU */
  boolean tmgi_deleted_from_mcch;

} lte_rrc_emp_mtch_db_s;

/*! @brief This structure holds MTCH db - contains MTCHs that 
  need to be searched in other MBSFN areas due to inter-MBSFN
  mobility
*/
typedef struct
{
  /*!< MBSFN Area ID */
  lte_mbsfn_area_id_t mbsfn_area_id;
  /*!< Includes TMGI and session ID */
  lte_rrc_embms_session_info_s session_info;
} lte_rrc_emp_prev_mbsfn_area_session_info_s;

/*! @brief This structure holds EMP db
*/
typedef struct
{
  /*!< Number of active MCCHs in db */
  uint8 num_active_mcch;

  /*!< MCCH DB - MAX 8 MCCHS can be available in db */
  lte_rrc_emp_mcch_db_s mcch_db[LTE_EMBMS_MAX_MBSFN_AREA];

  /*!< Number of active MTCHs in db */
  uint8 num_active_mtch;

  /*!< MTCH DB */
  lte_rrc_emp_mtch_db_s mtch_db[LTE_MAX_ACTIVE_MRB];

  /*!< Number of active MRBs - used for MTCH only */
  uint8 num_active_mrb;

  /*!< MRB DB */
  lte_rrc_emp_mrb_id_s mrb[LTE_RRC_EMP_MAX_NUM_MRB];

  /*!< Number of MTCHs need to be searched */
  uint8 num_prev_mbsfn_area_session_info;

  /*!< Need searched MTCH DB */
  lte_rrc_emp_prev_mbsfn_area_session_info_s prev_mbsfn_area_session_info[LTE_MAX_ACTIVE_MRB];

} lte_rrc_emp_db_s;

/*! @brief This structure holds the Static part of private data
*/
typedef struct
{
  /*!< Used configurable MCCH reading time */
  boolean use_canned_mcch_time;
  /*!< Canned MCCH reading timer value */
  uint32 canned_mcch_time;
  /*!< Use configurable OOS time */
  boolean use_efs_oos_time;
  /*!< Configurable OOS timer value */
  uint32 efs_oos_time;
	/*!< Used canned MCCH */
	boolean use_canned_mcch;
	/*!< Canned MCCH size */
	int32 canned_mcch_size;
	/*!< Canned MCCH message */
	unsigned char canned_mcch[LTE_RRC_MAX_SI_LENGTH];

} lte_rrc_emp_static_data_s;

/*! @brief This structure holds the Dynamic part of private data
*/
typedef struct
{
  /*!< Transaction id for cfg LLC */
  uint32 cfg_transaction_id;

  /*!< Config info will be added one MBSFN area by one and 
		   will be cleared only when cfg_reqi is sent */
  lte_rrc_cfg_reqi_s cfg_req;

  /*!< EMP db - db is in dd structure because EMP would not store 
       previous MCCH/MTCH data to make use of them */
  lte_rrc_emp_db_s emp_db;

  /*!< Current cmd being processed */
  lte_rrc_emp_rcvd_cmd_bitmask_t curr_cmd_rcvd; 

  /*!< Union contains current cmd sent from ESMGR 
       - No need for available TMGI list since no payload in that command */
  union {
    lte_rrc_esmgr_act_tmgi_reqi_s act_tmgi_reqi;
    lte_rrc_esmgr_deact_tmgi_reqi_s deact_tmgi_reqi;
  } curr_cmd_rcvd_u;

  /*!< Current umid needs to send to ESMGR */
  lte_rrc_emp_rsp_msg_bitmask_t cnf_umid; 

	/*!< This variable is set when deact_tmgi_reqi has deact_all_tmgi set to 
	 	   TRUE and is used when EMP receives cfg_cnf. */
	boolean deact_all_tmgi;

  /*!< Deactivation reason for LTE_RRC_EMP_DEACT_TMGI_INDI */
  lte_rrc_embms_deact_reason_e deact_reason;

  /*!< Boolean flag indicates if OOS timer is running or not. 
       This will catch the case where the tmr has expired but
       not handled (it is still sitting in the queue). */
  boolean oos_tmr_is_running;

  /*!< Unicast OOS timer for eMBMS - This timer has higher priority than per 
       MBSFN area OOS timer. */
  lte_rrc_tmr_s oos_wt_tmr;

  /*!< OOS recovery type - Used for deciding to send oos_warn_indi or not 
       after receiving MCCH PDU */
  lte_rrc_emp_oos_recovery_type_e oos_recovery_type;

  /*!< DSM pointer for avail_tmgi_list_indi */
  dsm_item_type *avail_tmgi_list_dsm_ptr;

  /*!< Boolean flag indicates if back to back change notification is rcvd and
       pending for processing. Set when new change notification comes before previous one
       finishing processing */
  boolean back_to_back_chg_ntf_rcvd;

  /*!< This mask stores the MCCH change notification mask which is currently 
       processing */
  uint8 curr_chg_ntf_mask;

  /*!< This mask stores the MCCH change notification mask which needs to be 
       processed after the current one is finished. Mask is populated only
       when back_to_back_chg_ntf_rcvd is set to TRUE */
  uint8 next_chg_ntf_mask;

  /*!< Last SFN when EMP receives MCCH change notification */
  lte_l1_sfn_t last_rcvd_chg_ntf_sfn;

  /*!< This flag will set to TRUE if total number of areas in MCCH db and 
       in SIB13 exceed LTE_EMBMS_MAX_MBSFN_AREA in sib_updated_indi handler.
       It will be used and reset after process all PDUs */
  boolean areas_more_than_max_mbsfn_area;

  /*!< This variable stores super command confirmation */
  lte_rrc_esmgr_act_deact_tmgi_cnfi_s act_deact_cnfi;

} lte_rrc_emp_dynamic_data_s;

/*! @brief Structure for state-machine per-instance local variables
*/
typedef struct
{
  /*!< Static part of private data */
  lte_rrc_emp_static_data_s *sd_ptr;

  /*!< Dynamic part of private data */
  lte_rrc_emp_dynamic_data_s *dd_ptr;

} lte_rrc_emp_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


#endif /* LTE_RRC_EMPI_H */
