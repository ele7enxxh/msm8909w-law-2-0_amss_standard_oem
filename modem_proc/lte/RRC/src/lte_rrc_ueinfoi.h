/*!
  @file
  lte_rrc_ueinfoi.h

  @brief
  Header file internal to RRC CRE Module

  @detail
  This header contains the constants definition and external function prototype 
  declarations used by the CRE module and its UTF test cases.

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_ueinfoi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/12   rg      Initial Version

===========================================================================*/

#ifndef LTE_RRC_UEINFOI_H
#define LTE_RRC_UEINFOI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <lte.h>
#include <lte_cphy_msg.h>
#include <lte_rrc_int_msg.h>
#include <lte_rrc_dispatcher.h>
#include <lte_rrc_utils.h>
#include <lte_rrc_irat_utils.h>
#include <lte_rrc_csp.h>
#include <lte_rrc_osys_asn1.h>
#include <lte_rrc_osys_asn1util.h>
#include <lte_rrc_controller.h>
#include "lte_rrc_crp.h"
#include "lte_rrc_config.h"
#include "lte_rrc_llc.h"
#include "lte_rrc_loc_servicesi.h"


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/ 
#define LTE_RRC_UEINFO_NCC_NUM_BITS                 3
#define LTE_RRC_UEINFO_BCC_NUM_BITS                 3
#define LTE_RRC_UEINFO_GLOBAL_CELLID_NUM_BITS       28
#define LTE_RRC_UEINFO_INVALID_GLOBAL_CELL_ID       0xFFFFFFFF
#define LTE_RRC_UEINFO_VALID_TMR_TICK_VALUE         24*60*60*1000
#define LTE_RRC_UEINFO_VALID_TMR_NUM_TICKS_MAX      2
#define LTE_RRC_UEINFO_CONN_FAILURE_TMR_TICK_VALUE  102300
#define LTE_RRC_UEINFO_LOC_INFO_HOF_TMR_TICK_VALUE  5  /*! 5 ms timer to get loc info*/
#define LTE_RRC_UEINFO_CONN_FAILURE_TMR_100MS       100
#define LTE_RRC_UEINFO_INVALID_VALUE                -1
/*Max valid value for TimeConnFailure is 1023*/
#define LTE_RRC_UEINFO_INVALID_CONN_FAILURE_TIME_VALUE 0xFFFF
#define LTE_RRC_UEINFO_LOGGING_INTERVAL_0           0
#define LTE_RRC_UEINFO_ASN_RSRP_MAX                 97
#define LTE_RRC_UEINFO_ASN_RSRQ_MAX                 34

// B66 MAX EARFCN Limit
#define LTE_RRC_UEINFO_ASN_EARFCN_MAX               67336
#define LTE_RRC_UEINFO_ASN_PHY_CELL_ID_MAX          503
#define LTE_RRC_UEINFO_ASN_UTRA_ARFCN_MAX           16383
#define LTE_RRC_UEINFO_ASN_UTRA_RSCP_MIN            -5
#define LTE_RRC_UEINFO_ASN_UTRA_RSCP_MAX            91
#define LTE_RRC_UEINFO_ASN_UTRA_ECNO_MAX            49
#define LTE_RRC_UEINFO_ASN_UTRA_PCI_FDD_MAX         511
#define LTE_RRC_UEINFO_ASN_UTRA_PCI_TDD_MAX         127
#define LTE_RRC_UEINFO_ASN_GERAN_ARFCN_MAX          1023
#define LTE_RRC_UEINFO_ASN_GERAN_RSSI_MAX           63
#define LTE_RRC_UEINFO_ASN_CDMA_ARFCN_MAX           2047
#define LTE_RRC_UEINFO_ASN_CDMA_PNOFFSET_MAX        511
#define LTE_RRC_UEINFO_ASN_CDMA_PILOT_PNPHASE_MAX   32767
#define LTE_RRC_UEINFO_ASN_CDMA_PILOT_STRN_MAX      63
#define LTE_RRC_UEINFO_LOC_CORD_MAX_T_VALUE         8 
#define LTE_RRC_UEINFO_VALIDITY_TMR_EFS_FILENAME    "/nv/item_files/modem/lte/rrc/ueinfo/valtmr"


/*! @brief Data structure for connection failure type
*/
typedef enum {
   LTE_RRC_UEINFO_FAILURE_RLF = 0,
   LTE_RRC_UEINFO_FAILURE_HOF = 1,
   LTE_RRC_UEINFO_FAILURE_CON_FAILURE = 2,
   LTE_RRC_UEINFO_FAILURE_MAX = 0xFF
} lte_rrc_ueinfo_conn_failure_type;

/*! @brief Data structure for RLF Cause type
*/
typedef enum {
   LTE_RRC_UEINFO_RL_CAUSE_T310_EXPIRY = 0,
   LTE_RRC_UEINFO_RL_CAUSE_RANDOM_ACCESS_PROBLEM = 1,
   LTE_RRC_UEINFO_RL_CAUSE_RLC_MAX_NUM_RETX = 2,
   LTE_RRC_UEINFO_RL_CAUSE_MAX = 0xFF
}lte_rrc_ueinfo_rlf_cause_type;

/*! @brief enumeration to identify the type of Mobility
    When UEINFO STM receives HANDOVER_STARTED_INDI, INTRA_RAT
    When UEINFO STM receives PSHO_STARTED_INDI, INTER-RAT 
*/
typedef enum
{
   LTE_RRC_UEINFO_MOBILITY_TYPE_INTRA_RAT = 0,
   LTE_RRC_UEINFO_MOBILITY_TYPE_INTER_RAT = 1,
   LTE_RRC_UEINFO_MOBILITY_TYPE_MAX = 0xFF
} lte_rrc_ueinfo_mobility_type_e;

/*! @brief Data structure for cell global info
*/
typedef struct
{
  /*!< Selected PLMN - req PLMN or its equivalent or first entry in SIB1 list for limited service */
  lte_rrc_plmn_s selected_plmn;

  lte_rrc_plmn_s primary_plmn;

  /*!< DL Frequency */
  lte_earfcn_t earfcn; 

  /*!< Physical cell id */
  lte_phy_cell_id_t phy_cell_id; 

  /*!< Global Cell Id */
  lte_rrc_global_cell_id_t cell_identity;

  /*!< Tracking area code */
  uint16 tac;
} lte_rrc_ueinfo_cell_global_info_s;

/*! @brief Data structure for Connection failure report
*/
typedef struct
{
  boolean rach_info_present;
  uint8 num_preambles_sent; /*!< Preambles sent in last successfull RACH */  
  uint8 num_contention_detected; /*!< Contentions detected in last successfull RACH */
  
  boolean conn_failure_info_present;

  /*! Meas related params */
  lte_cphy_ueinfo_rlf_rpt_s rlf_rpt;

  /* To indicate whether or not the maximum power level was used 
      for the last transmitted preamble 
   */
  boolean maxTxPowerReached;

  /*! the PCell in which t-300 expiry is detected*/ 
  lte_rrc_ueinfo_cell_global_info_s failed_pcell_info;
  
  /*!< Decoded location info */
  lte_rrc_loc_services_loc_info_s decoded_loc_info;

}lte_rrc_ueinfo_conn_failure_report_s;

typedef struct
{

  /*! Mask for reports requested */
  lte_cphy_ueinfo_rpt_mask_e req_mask;  

  /*! Mask for reports sent for this transaction */
  lte_cphy_ueinfo_rpt_mask_e sent_mask;    

  /*! Transaction ID for the UEInfo Request-Response transfer */  
  lte_rrc_osys_RRC_TransactionIdentifier dlm_transaction_id;

  /*! connection failure type */
  lte_rrc_ueinfo_conn_failure_type failure;
  
  /*! RLF cause type */
  lte_rrc_ueinfo_rlf_cause_type rlf_cause;

  /*! current pcell info */
  lte_rrc_ueinfo_cell_global_info_s curr_pcell_info;  

  /*! Pointer sent to CPHY to collect the report */
  lte_cphy_ueinfo_rlf_rpt_s* cphy_rlf_rpt_ptr;

  /*! the PCell in which RLF is detected or the target PCell of the failed handover */ 
  lte_rrc_ueinfo_cell_global_info_s failed_pcell_info;

  /*! source PCell of the last handover request */ 
  lte_rrc_ueinfo_cell_global_info_s prev_pcell_info;

  /*! RPLMN where failure was observed*/ 
  lte_rrc_plmn_s rplmn;

  /*! MAC RACH report confirmation from MAC to RRC   */
  lte_mac_rach_rpt_cnf_msg_s rach_rpt;

   /*!< Message unit identifier - for book keeping */
  uint8 mu_id;

   /*!< Set by MH - 0 means SUCCESS */   
  uint8 encode_status;

  /*!< Total duration of T304 time for which RF is unavailable */
  uint32 t304_time_rf_unavail;
 
  /*!< Boolean, which if set to TRUE indicates RF is unavailable with LTE*/
  boolean is_rf_unavail;

  /*!< T304 timer value*/
  uint32 t304_tmr_value;

  /*! UTRA cell info for too early handover */
  utra_serving_cell_info_type                 previous_utra_cell_info;

    /*! UTRA cell info for too late handover */
  utra_serving_cell_info_type                 selected_utra_cell_info;

    /*! Source cell C-RNTI */
  lte_rnti_val_t         source_cell_crnti; 

  /*! To determine if the last MobilityControlInfo was for intra-lte HO or inter-RAT HO*/
  lte_rrc_ueinfo_mobility_type_e mobility_type;

  /* Temp Conn est failure rept */
  lte_rrc_ueinfo_conn_failure_report_s *con_failure_rpt_ptr;   

}lte_rrc_ueinfo_dd_s;

typedef struct
{
  /*! Boolean to indicate if report is present */
  boolean is_rpt_present; 

  /*! Mask for reports present */
  lte_cphy_ueinfo_rpt_mask_e rpt_mask;

  /*! RLF report populated by CPHY */  
  lte_cphy_ueinfo_rlf_rpt_s cphy_rlf_rpt;

  /*! the PCell in which RLF is detected or the target PCell of the failed handover */ 
  lte_rrc_ueinfo_cell_global_info_s failed_pcell_info;

  /*! source PCell of the last handover request */ 
  lte_rrc_ueinfo_cell_global_info_s prev_pcell_info;

  /*! cell in which the re-establishment attempt was made after connection failure */
  lte_rrc_ueinfo_cell_global_info_s reest_pcell_info;

  /*! RPLMN where failure was observed*/ 
  lte_rrc_plmn_s rplmn;

  /*! connection failure type */
  lte_rrc_ueinfo_conn_failure_type failure;

    /*! RLF cause type */
  lte_rrc_ueinfo_rlf_cause_type rlf_cause;

  /*!< 24*2 hrs  report validity timer  */
  lte_rrc_tmr_s ueinfo_valid_tmr;

  /*!< Validity period derived from EFS or 48hrs  */
  uint64 ueinfo_validity_period;  

  /*!< Number of ticks */
  uint8 ueinfo_valid_tmr_tick;

  /*!< 103.2s Conn Failure timer */
  lte_rrc_tmr_s conn_failure_tmr;

  /*! 2 sec timer to get Location info during HOF*/
  lte_rrc_tmr_s loc_info_hof_tmr;

  /*!< Time elapsed between reception of reconfig with mobility and connection failure 
    Max value 1023*/
  uint16 conn_failure_time;
  /*! UTRA cell info for too early handover */
  utra_serving_cell_info_type                 previous_utra_cell_info;
 
  /*! indicates presence of R11 field SelectedUTRA*/
  boolean selected_utra_cell_id_present;

    /*! UTRA cell info for too late handover */
  utra_serving_cell_info_type                 selected_utra_cell_info;

    /*! Source cell C-RNTI */
  lte_rnti_val_t         source_cell_crnti; 

  /*! To determine if the last MobilityControlInfo was for intra-lte HO or inter-RAT HO*/
  lte_rrc_ueinfo_mobility_type_e mobility_type;

  /*!< Decoded location info */
  lte_rrc_loc_services_loc_info_s decoded_loc_info;


  /*!< 0-172800 s Conn Failure timer */
  lte_rrc_tmr_s conn_failure_rpt_valid_tmr;

  uint8 con_fail_rpt_timer_tick;

  /* Conn est failure rept */
  lte_rrc_ueinfo_conn_failure_report_s *con_failure_rpt_ptr;  

}lte_rrc_ueinfo_sd_s;

typedef struct
{
    /*!< Dynamically allocated part of private data */
  lte_rrc_ueinfo_sd_s *sd_ptr;
    
  /*!< Dynamically allocated part of private data */
  lte_rrc_ueinfo_dd_s *dd_ptr;

} lte_rrc_ueinfo_s;


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern void lte_rrc_ueinfo_populate_cell_global_info
(
  lte_rrc_osys_CellGlobalIdEUTRA *asn_cell_info,
  lte_rrc_plmn_s *selected_plmn,
  lte_rrc_global_cell_id_t *cell_identity
);
extern boolean lte_rrc_ueinfo_populate_eutra_neigh_info
(
  void *extd_earfcn_container_ptr,
  boolean is_log_meas_info_ptr,
  lte_rrc_osys_MeasResult2EUTRA_r9 *asn_neigh,
  lte_cphy_ueinfo_meas_neigh_results_eutran_s *cphy_neigh,
  uint32 neigh_size
);

extern boolean lte_rrc_ueinfo_populate_utra_neigh_info
(
  lte_rrc_osys_MeasResult2UTRA_r9 *asn_neigh,
  lte_cphy_ueinfo_meas_neigh_results_wcdma_s *cphy_neigh,
  uint32 neigh_size
);
extern boolean lte_rrc_ueinfo_populate_geran_neigh_info
(
  lte_rrc_osys_MeasResultGERAN *asn_neigh,
  lte_cphy_conn_meas_report_list_gsm_s *cphy_neigh,
  uint32 neigh_size
);
extern boolean lte_rrc_ueinfo_populate_cdma2k_neigh_info
(
  lte_rrc_osys_MeasResult2CDMA2000_r9 *asn_neigh,
  lte_cphy_ueinfo_meas_neigh_results_cdma2k_s *cphy_neigh,
  uint32 neigh_size
);

extern boolean lte_rrc_ueinfo_populate_rlf_rpt
(
  lte_rrc_ueinfo_s *i_ptr,
  lte_rrc_osys_UEInformationResponse_r9_IEs *ueinfo_response_ie_ptr
);
#endif

