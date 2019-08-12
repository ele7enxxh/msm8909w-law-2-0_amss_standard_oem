/*!
  @file lte_rrc_measi.h

  @brief
  Internal definitions for Meas procedure
  
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_measi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/28/11   mm      Added definitions for IRAT CGI
04/01/11   mm      Change to prune the connected and idle mode measurements
05/10/10   mm      Added definitions related to SON / ANR 
04/05/10   da      Added dd_ptr in priv data for usage with dymically allocated mem
                   and sd_ptr for usage with statically allocated mem.
02/26/10   mm      Added a variable to store CPHY_IDLE_MEAS_CFG_REQ sent to ML1
                   in the private data structure. Added GSM related definitions
08/12/09   sk      March 09 spec migration for SIB4, SIB5 and connected meas 
06/30/09   ask     Meas changes for RLF and HO failure handling
03/25/09   ask     Added code for connected mode EUTRA measurements
03/19/09   sk      Added support for rxLevMin and pMax for inter and intra 
                   freq meas
02/23/09   sk      Removed unused header file
12/18/08   ask     Initial Revision

===========================================================================*/

#ifndef LTE_RRC_MEASI_H
#define LTE_RRC_MEASI_H

#include <lte_rrc_ext_api.h>
#include <lte_rrc_int_msg.h>
/*===========================================================================

                           DEFINES

===========================================================================*/

#define LTE_RRC_ASN_MAX_T_RESEL_EUTRAN      7   /*<! Maximun value of t_reselection_eutran */
#define LTE_RRC_ASN_MAX_RESEL_THRESHOLD     31  /*<! Maximun value of Reselection threshold */
#define LTE_RRC_ASN_MAX_CELL_RESEL_PRIORITY 7   /*<! Maximun value of Cell reselection priority */
#define LTE_RRC_ASN_MIN_CELL_CHANGE_MED     1   /*<! Minimum value of cell change medium */
#define LTE_RRC_ASN_MAX_CELL_CHANGE_MED     16  /*<! Maximum value of cell change medium  */
#define LTE_RRC_ASN_MIN_CELL_CHANGE_HIGH    1   /*<! Minimum value of cell change medium */
#define LTE_RRC_ASN_MAX_CELL_CHANGE_HIGH    16  /*<! Maximum value of cell change medium  */
#define LTE_RRC_ASN_MAX_SERV_THRESHOLD      31  /*<! Maximum value of reselection threshold  */

#define LTE_RRC_ASN_MIN_RX_LEV_MIN         -70  /*<! Minimum value of RxLevMin  */
#define LTE_RRC_ASN_MAX_RX_LEV_MIN         -22  /*<! Maximum value of RxLevMin  */
#define LTE_RRC_ASN_MIN_RX_LEV_MIN_UTRA    -60  /*<! Minimum value of RxLevMin in UTRA */
#define LTE_RRC_ASN_MAX_RX_LEV_MIN_UTRA    -13  /*<! Maximum value of RxLevMin in UTRA */
#define LTE_RRC_ASN_MAX_RX_LEV_MIN_GERAN   45   /*<! Maximum value of RxLevMin in GERAN */

#define LTE_RRC_ASN_MIN_Q_QUAL_MIN_UTRA    -24  /*<! Maximum value of qQualMin in UTRA */
#define LTE_RRC_ASN_MAX_Q_QUAL_MIN_UTRA     0   /*<! Maximum value of qQualMin in UTRA */

#define LTE_RRC_ASN_MIN_PMAX               -30  /*<! Minimum value of pMax  */
#define LTE_RRC_ASN_MAX_PMAX                33  /*<! Maximum value of pMax  */
#define LTE_RRC_ASN_MIN_PMAX_UTRA          -50  /*<! Minimum value of pMax in UTRA */
#define LTE_RRC_ASN_MAX_PMAX_UTRA           33  /*<! Maximum value of pMax in UTRA */
#define LTE_RRC_ASN_MAX_PMAX_GERAN          39  /*<! Maximum value of pMax in GERAN */

#define LTE_RRC_ASN_MAX_HYSTERESIS          30  /*<! Maximum value of hysteresis in report cfg  */
#define LTE_RRC_ASN_REPORT_AMT_INFINITY   0xFF  /*<! Indicates infinite reporting amount */

#define LTE_RRC_ASN_MAX_RSRP                97  /*<! Maximum value of RSRP */
#define LTE_RRC_ASN_MAX_RSRQ                34  /*<! Maximum value of RSRQ */

#define LTE_RRC_ASN_FC_RSRP_DEFAULT         4   /*<! Default fc value for RSRP */
#define LTE_RRC_ASN_FC_RSRQ_DEFAULT         4   /*<! Default fc value for RSRQ */
#define LTE_RRC_ASN_FC_DEFAULT_GERAN        2   /*<! Default fc value for GERAN */
#define LTE_RRC_ASN_FC_DEFAULT_UTRA         4   /*<! Default fc value for UTRA */

#define LTE_RRC_ASN_NCC_PERMITTED_DEFAULT_GERAN  0xFF /*<! Default ncc_permitted value for GERAN */

#define LTE_RRC_ASN_A3_OFFSET_MIN          -30
#define LTE_RRC_ASN_A3_OFFSET_MAX           30

#define LTE_RRC_ASN_A6_OFFSET_MIN          -30
#define LTE_RRC_ASN_A6_OFFSET_MAX           30

#define LTE_RRC_ASN_MAX_SCELL_IDX           7  /*<! Maximum value of ServCellIndex-r10 */


#define LTE_RRC_ASN_GP0_MAX                 39  /*<! Maximum value for Gap0 */
#define LTE_RRC_ASN_GP1_MAX                 79  /*<! Maximum value for Gap1 */

#define LTE_RRC_ASN_MIN_Q_OFFSET_IRAT      -15  /*<! Minimum value of IRAT Offset Freq */
#define LTE_RRC_ASN_MAX_Q_OFFSET_IRAT       15  /*<! Maximum value of IRAT Offset Freq */

#define LTE_RRC_ASN_MIN_RSCP               -5   /*<! Minimum value of RSCP */
#define LTE_RRC_ASN_MAX_RSCP                91  /*<! Maxmum value of RSCP */

#define LTE_RRC_ASN_MAX_ECN0                49  /*<! Maxmum value of EcN0 */
#define LTE_RRC_ASN_MAX_RSSI                63  /*<! Maxmum value of RSSI */
#define LTE_RRC_ASN_GERAN_BCC_LENGTH         3  /*<! Length of GERAN BCC */
#define LTE_RRC_ASN_GERAN_NCC_LENGTH         3  /*<! Length of GERAN MCC */

#define LTE_RRC_ASN_MAX_PILOT_STRENGTH      63     /*<! Maxmum value of Pilot strength */

#define LTE_RRC_ASN_MAX_CELL_RESEL_PRIORITY 7  /*<! Maximum value for cell reselection priority*/

/*! Initialized value for Resel priority */
#define LTE_RRC_MEAS_RESEL_PRIORITY_INVALID   0xFE

#define LTE_RRC_ASN_CDMA_SYNC_SYSTEM_TIME_LENGTH 39 /*<! Length of synchronous CDMA system time */
#define LTE_RRC_ASN_CDMA_ASYNC_SYSTEM_TIME_LENGTH 49 /*<! Length of asynchronous CDMA system time */

#define LTE_RRC_CDMA_SINGLE_RX_CONFIG       1   /*<! Default CDMA Rx-Config */

#define LTE_RRC_CPHY_UNREPORTED_RSCP_VALUE -128  /*<! Unreported RSCP value */
#define LTE_RRC_CPHY_UNREPORTED_ECN0_VALUE -1    /*<! Unreported ECN0 value */

#define LTE_RRC_T321_VALUE_EUTRA            1000   /*<! T321 value in ms for EUTRA */
#define LTE_RRC_T321_VALUE_EUTRA_WITH_SI_REQ_HO     150    /*<! T321 value in ms for EUTRA with SI REQ HO*/
#define LTE_RRC_T321_VALUE_UTRA_WITH_SI_REQ_HO_FDD  2000   /*<! T321 value in ms for UTRA with SI REQ HO in FDD mode */
#define LTE_RRC_T321_VALUE_UTRA_WITH_SI_REQ_HO_TDD  1000   /*<! T321 value in ms for UTRA with SI REQ HO in TDD mode */
#define LTE_RRC_T321_VALUE_IRAT             8000   /*<! T321 value in ms for IRAT */
#define LTE_RRC_STACK_SUSPEND_TIME          15   /*<! Value in ms to suspend LTE stack 
                                                    (10 ms + 5 ms for processing delay) */

#define LTE_RRC_ASN_MAX_PLMN_ID_LIST_SIZE   5  /*<! Maxmum PLMN-id List size */

#define LTE_RRC_ASN_RSCP_MIN_VALUE          -5  /*<! RSCP Min value */
#define LTE_RRC_ASN_RSCP_MAX_VALUE          91  /*<! RSCP Max value */
#define LTE_RRC_ASN_CSG_IDENTITY_LENGTH          27  /*<! CSG Identity length in bits */
#define LTE_RRC_ASN_TA_CODE_LENGTH          16  /*<! Tracking area code length in bits */
#define LTE_RRC_ASN_LA_CODE_LENGTH          16  /*<! Location area code length in bits */
#define LTE_RRC_ASN_RA_CODE_LENGTH          8   /*<! Routing area code length in bits */
#define LTE_RRC_ASN_CELL_ID_LENGTH          28  /*<! Cell identity length in bits */
#define LTE_RRC_ASN_GERAN_CELL_ID_LENGTH    16  /*<! Cell identity length in bits */
#define LTE_RRC_ASN_TA_CODE_BYTE_SIZE       ((LTE_RRC_ASN_TA_CODE_LENGTH/8))  /*<! Tracking area code size in bytes */
#define LTE_RRC_ASN_CELL_ID_BYTE_SIZE       ((LTE_RRC_ASN_CELL_ID_LENGTH/8)+1)  /*<! Cell identity size in bytes */
#define LTE_RRC_QTY_CFG_DISABLED_FC_VAL     0   /*!< Disabled FC value for Qty config */
#define LTE_RRC_ASN_DO_CGI_LENGTH           128  /*<! DO CGI length in bits */
#define LTE_RRC_ASN_DO_CGI_LENGTH_IN_BYTES  16   /*<! DO CGI length in bytes */
#define LTE_RRC_ASN_1X_CGI_LENGTH           47   /*<! 1X CGI length in bits */
#define LTE_RRC_ASN_1X_CGI_BASEID_LENGTH    16   /*<! 1X CGI (base_id) length in bits */
#define LTE_RRC_ASN_1X_CGI_SID_LENGTH       15   /*<! 1X CGI (sid) length in bits */
#define LTE_RRC_ASN_1X_CGI_NID_LENGTH       16   /*<! 1X CGI (nid) length in bits */
#define LTE_RRC_ASN_1X_CGI_BASEID_LENGTH_IN_BYTES    2   /*<! 1X CGI (base_id) length in bytes */
#define LTE_RRC_ASN_1X_CGI_SID_LENGTH_IN_BYTES       2   /*<! 1X CGI (sid) length in bytes */
#define LTE_RRC_ASN_1X_CGI_NID_LENGTH_IN_BYTES       2  /*<! 1X CGI (nid) length in bytes */
#define LTE_RRC_ASN_1X_CGI_LENGTH_IN_BYTES  6    /*<! 1X CGI length in bytes */
#define LTE_RRC_MAX_RXTX_TIME_DIFF_RESULT   4096 /*<! Max RxTx time diff result */
#define LTE_RRC_MAX_SFN                     1023  /*<! Max SFN */
#define LTE_RRC_ASN_SFN_LENGTH              10   /*<! Bitstring Length of SFN */
#define LTE_RRC_DEFAULT_PILOT_STRENGTH      63   /*<! Default Pilot Strength */
#define LTE_RRC_MEAS_EMBMS_RESEL_PRIO_OFFSET 9   /*!< eMBMS resel prio offset */
#define LTE_RRC_MEAS_HST_CELL_RESEL_PRIO 10   /*!< HST Cell reselection priority*/
#define LTE_RRC_MEAS_HST_T_RESEL 1   /*!< HST T resel*/
#define LTE_RRC_MEAS_HST_THRESHHIGH 8   /*!< HST ThreshHigh*/
#define LTE_RRC_S_MEASURE_MIN               -140 /*<! Minimum value possible for s-measure */
#define LTE_RRC_S_MEASURE_DISABLED          0    /*<! Value 0 indicates disabled */
#define LTE_RRC_MEAS_MAX_NUM_NEIGH          10   /*<! Max number of neighbor */
#define LTE_RRC_MEAS_MAX_NPG_LIST_EFS       12   /*<! Max Size of NPG List in EFS */
#define LTE_RRC_MEAS_SIBS_BITMASK (LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK | \
                                   LTE_RRC_SIB5_BITMASK | LTE_RRC_SIB6_BITMASK | \
                                   LTE_RRC_SIB7_BITMASK | LTE_RRC_SIB8_BITMASK)
#define LTE_RRC_MAX_PRS_OFFSET              39   /*<! Maximum value for PRS Offset in RSTD-InterFreqInfo-r10 element of 
                                                         InterFreqRSTDMeasurementIndication-r10 UL-DCCH message*/
#define LTE_RRC_MEAS_LOC_SER_LOG_INTERVAL   1000 /*<! Default logging interval for immediate MDT 
                                                      (connected mode measurements)*/
#define LTE_RRC_MEAS_LOC_COORD_MAX_T_VALUE   8 /*Max T value defined in ASN.1 B70 for Loc co-ordinates*/

/*! @brief Maximum number of Inter Freq L Meas Objs from SIBs and ACQ DB
  */
#define LTE_RRC_MEAS_MAX_LTE_MEAS_OBJ_FROM_SIBS_ACQ_DB 3

/*! @brief Maximum number of Inter Freq DL Only Meas Objs
  */
#define LTE_RRC_MEAS_MAX_LTE_DL_ONLY_MEAS_OBJ 1

/*! @brief Maximum number of W Meas Objs from SIBs and ACQ DB
  */
#define LTE_RRC_MEAS_MAX_WCDMA_MEAS_OBJ_FROM_SIBS_ACQ_DB 1

/*! @brief Maximum number of CDMA Meas Objs from SIBs
  */
#define LTE_RRC_MEAS_MAX_CDMA_MEAS_OBJ_FROM_SIBS_DB 1

/*! @brief Maximum number of IRAT MEAS Objs
  */
#define LTE_RRC_MEAS_MAX_IRAT_MEAS_OBJS 7

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Enum for cgi_reporting_type
*/
typedef enum
{
  LTE_RRC_MEAS_CGI_NONE,
  LTE_RRC_MEAS_CGI_FOR_EUTRA,
  LTE_RRC_MEAS_CGI_FOR_IRAT_UTRA,
  LTE_RRC_MEAS_CGI_FOR_IRAT_GERAN,
  LTE_RRC_MEAS_CGI_FOR_IRAT_CDMA

} lte_rrc_meas_cgi_reporting_type_e;


/*! @brief Enum for Qty config
*/
typedef enum
{
  LTE_RRC_MEAS_QTY_CFG_NONE  = 0x0,
  LTE_RRC_MEAS_QTY_CFG_EUTRA = 0x01,
  LTE_RRC_MEAS_QTY_CFG_UTRA  = 0x02,
  LTE_RRC_MEAS_QTY_CFG_GERAN = 0x04,
  LTE_RRC_MEAS_QTY_CFG_CDMA  = 0x08

} lte_rrc_meas_qty_cfg_e;

/*! @brief Var measurement config type
*/
typedef struct
{
  /*! IRAT CGI timer */
  lte_rrc_tmr_s irat_cgi_tmr; 

  /*! Flag to indicate if IRAT CGI tmr is running */
  boolean irat_cgi_tmr_running;

  /*! Flag to indicate if IRAT CGI info has been received and is valid */
  boolean irat_cgi_info_valid;

  /* Abort cause for IRAT CGI */
  lte_rrc_irat_abort_cause_type_e abort_cause;

  lte_irat_rat_type_e cgi_rat_type; /*!< RAT type */

  union {
    lte_rrc_irat_from_lte_to_w_get_cgi_cnfi_s  w_cgi_rsp; /*!< CGI info for W */
    lte_rrc_irat_from_lte_to_g_get_cgi_cnfi_s  g_cgi_rsp; /*!< CGI rsp from G */
    lte_rrc_irat_from_lte_to_do_get_cgi_cnfi_s do_cgi_rsp; /*!< CGI rsp from DO */
    lte_rrc_irat_from_lte_to_1x_get_cgi_cnfi_s onex_cgi_rsp; /*!< CGI rsp from DO */
  }irat_cgi_info_u;

} lte_rrc_meas_irat_cgi_info_s;

/*! @brief Enum for T320 status
*/
typedef enum
{
  LTE_RRC_MEAS_T320_STOPPED,
  LTE_RRC_MEAS_T320_RUNNING,
  LTE_RRC_MEAS_T320_INFINITY
} lte_rrc_meas_t320_status;

/*! @brief Neigh band info
*/
typedef struct
{
  /*!< Cell info */
  lte_rrc_cell_id_s cell_id;

  /*!< neigh band info obtained from SIB5 */
  sys_lte_band_mask_e_type band_mask;
}lte_rrc_meas_neigh_band_s;

/*! @brief Structure to hold LTE Deprioritization info and Deprioritization
           RPLMN and EPLMN info
*/
typedef struct
{
  /*! LTE Network Deprioritization info */
  lte_rrc_lte_ota_depri_info_s lte_ota_depri_info;

  /*! LTE limited mode IMS Deprioritization info */
  lte_rrc_lte_depri_info_s lte_ims_depri_info;

} lte_rrc_lte_depri_data_s;

  
/*! @brief Var measurement config type
*/
typedef struct
{

  /*!< Cell info */
  lte_rrc_cell_id_s cell_id;    

  /*! Measurement Config */
  lte_cphy_conn_meas_cfg_info_s cfg;  

  /*! Bitmask which indicates which measurement objects are valid */
  uint32 meas_obj_valid_bitmask;
  
  /*! Bitmask which indicates which measurement ids are valid */
  uint32 meas_id_valid_bitmask;

  /*! Bitmask which indicates which report configs are valid */
  uint32 report_cfg_valid_bitmask;

  /*! Bitmask which indicates which other config are valid */
  uint32 other_cfg_valid_bitmask;     

  /*! Number of frequency layers in connected mode - this could be the same as 
    what is provided in OTA or can be the value after pruning. */
  uint8 conn_meas_freq_layer_cnt;

} lte_rrc_meas_vmc_s;


/*! @brief Structure for private data variables that can be allocated dynamically
     from the heap when LTE is activated, and deallocated when LTE is deactivated
*/
typedef struct
{
  /*! Array of 2 VMCs (one for storing current config and other for storing
      target config */
  lte_rrc_meas_vmc_s vmc[2];

  /*! VMC storing the current config */
  lte_rrc_meas_vmc_s *src_vmc_ptr;

  /*! VMC storing the target config */
  lte_rrc_meas_vmc_s *tgt_vmc_ptr;

  /*! Is meas config pending? 
      i.e.. validate_connected_config has been called, but
      subsequent meas_cfg_reqi msg has not been received yet
  */
  boolean meas_cfg_pending;

  /*! Bitmask which indicates which meas objects have been touched (add/modify/delete) */
  uint32 meas_obj_mod_bitmask;

  /*! Bitmask which indicates which meas ids have been touched (add/modify/delete) */
  uint32 meas_id_mod_bitmask;

  /*! Bitmask which indicates which report configs have been touched (add/modify/delete) */
  uint32 report_cfg_mod_bitmask;

  /*! Bitmask which indicates which other cfgs have been modified */
  uint32 other_mod_bitmask;

  /*! Bitmask which indicates Qty config signaled across all RATs */
  lte_rrc_meas_qty_cfg_e qty_configured_bitmask;

  /*! SIBs scheduled bitmask - Indicates the sibs aquired */
  lte_rrc_sib_bitmask_t sibs_scheduled_bitmask;

  /*! Cell id for the sib schedule bitmask is for */
  lte_rrc_cell_id_s sibs_scheduled_cell_id;

  /*! Current Idle meas config request sent to ML1 */
  lte_cphy_idle_meas_cfg_req_s  curr_cphy_idle_meas_cfg_req;

  /*! Number of frequency layers in idle mode - this could be the same as what is provided
      in SIBs or can be after pruning. */
  uint8  idle_meas_freq_layer_cnt;

  /*! Num of unique resel priorities in the list */
  uint8 num_resel_priorities;

  /*! Unique set of cell reselection priorities given for all RATs */
  lte_l1_resel_priority_t cell_resel_priority_list[LTE_RRC_ASN_MAX_CELL_RESEL_PRIORITY + 1];

  /*! Last Conn meas config request sent to ML1 */
  lte_cphy_conn_meas_cfg_req_s  last_cphy_conn_meas_cfg_req;

  /*! T321 timer */
  lte_rrc_tmr_s t321_tmr; 

  /*! Flag to indicate if T321 is running */
  boolean t321_running;

  /*! Flag to indicate if CPHY_SERV_CELL_MEAS_CNF is pending*/
  boolean cphy_serv_cell_meas_cnf_pending;

  /*! Enum to indicate what type of CGI reporting is configured */
  lte_rrc_meas_cgi_reporting_type_e cgi_reporting_type;

  /*! MeadId tied to CGI reporting */
  lte_cphy_meas_id_t cgi_meas_id;

  /*! SIB1 for CGI cell */
  lte_rrc_osys_BCCH_DL_SCH_Message *cgi_cell_sib1_ptr;

  /*! IRAT CGI info */
  lte_rrc_meas_irat_cgi_info_s irat_cgi_info;

  /*! NMR report type requested */
  lte_rrc_nmr_report_type_e rrc_nmr_report_type;

  /*! NMR n freq report ptr */
  lte_cphy_nmr_n_freq_report_s *rrc_nmr_n_freq_report_ptr;

  /*! Flag to indicate if LTE_CPHY_NMR_INFO_CNF is pending*/
  boolean cphy_nmr_cnf_pending;

  /*! Flag to indicate whether Report Config UTRA FDD R10 is present*/
  boolean rpt_cfg_utra_fdd_r10_included;

  /*! Flag to indicate whether Quantity Config UTRA R10 is present*/
  boolean qty_cfg_utra_r10_included;

  /*! Flag to indicate whether ObtainLocCfg has been configured by the network*/
  boolean obtain_loc_cfg_present;

  /*! Flag to indicate whether Loc ser engine has been started by Meas module*/
  boolean sent_start_loc_ser;

}lte_rrc_meas_dd_s;


/*! @brief Structure for private data variables that shall use static memory
*/
typedef struct
{
  /*! T320 timer */
  lte_rrc_tmr_s t320_tmr; 

  /*! Flag to indicate if T320 is running */
  lte_rrc_meas_t320_status t320_running;

  /*! Dedicated cell reselection priority list :  This info will be passed to IRAT module 
       as a pointer.  Therfore, DO NOT RESET OR INITIALIZE THE DEDICATED PRIORITY LIST 
       EXCEPT FOR DURING SM_ENTRY. */
  lte_irat_dedicated_priority_list_s freq_list; 

  /*! W reselection wait timer */
  lte_rrc_tmr_s w_resel_wait_tmr; 

  /*! Flag to indicate if W reselection wait timer is running */
  boolean w_resel_wait_tmr_running;

  /*! TDS reselection wait timer */
  lte_rrc_tmr_s tds_resel_wait_tmr; 

  /*! Flag to indicate if TDD reselection wait timer is running */
  boolean tds_resel_wait_tmr_running;

  /*! The owner of dedicated priorities */
  lte_rrc_dedicated_pri_owner_e dedicated_pri_owner;

  /*! Camped PLMN ID of the cell where dedicated priorities are received */
  sys_plmn_id_s_type sys_ded_pri_camped_plmn;

  /*! Flag to indicate if qty config validation is enabled, read from EFS */
  boolean qty_chk_enabled;

  /*! Flag to indicate if the check to validate duplicate meas objects is enabled */
  boolean dup_meas_obj_chk_enabled;

  /*! Flag to indicate if the check to skip_conn_meas_rat_validation is enabled */
  boolean skip_conn_meas_rat_validation;

  /*! Flag to indicate if RSCP and EcNO for WCDMA is to be 
           populated in CGI Meas report or not*/
  boolean rscp_ecno_in_cgi_rpt;

  /*! Bitmask read from EFS that specifies LTE bands that have unsupported EARFCNs 
      LSB corresponds to Band 1 and MSB to Band 64.*/
  uint64 skip_unsupported_band_meas;

  /*! T325 timer */
  lte_rrc_tmr_s t325_tmr; 

  /*! Customized T325 value configured via EFS*/
  uint8 t325_val_custom;

  /*! LTE Deprioritization Data */
  lte_rrc_lte_depri_data_s lte_depri_data;
  
  /*!< Last 10 different cells' neigh band info obtained 
  from SIB5 to be used in CAP rsp to W */
  lte_rrc_meas_neigh_band_s lte_neigh_band[LTE_RRC_MEAS_MAX_NUM_NEIGH];

  /*!< idx where next cell's neigh band info will be added */
  uint8 next_lte_neigh_band_idx;

  /*!< Enable measCycleScell check for Scells */
  uint8 enable_measCycle_scell_check;  

  /*!< Boolean to indicate if idle search meas has to be disabled */
  boolean skip_idle_srch_meas;  

  /*!< Boolean to indicate if HST Idle Mode optimization should be enabled or not*/
  boolean enable_hst_idle_mode_opt;

  /*!< Boolean to indicate if invalid Meas Configuration skip should be enabled or not*/
  boolean skip_invalid_meas_cfg;

  /*! Size of NPG list for LTE connected mode
      configured via EFS*/
  uint8 lte_npg_list_size_conn;

  /*! Size of NPG list for LTE idle mode 
      configured via EFS */
  uint8 lte_npg_list_size_idle;

}lte_rrc_meas_sd_s;

/*! @brief Private data of the measurement module
*/
typedef struct
{
  /*!< Dynamically allocated part of private data */
  lte_rrc_meas_dd_s *dd_ptr;

  /*!< Statically allocated part of private data */
  lte_rrc_meas_sd_s *sd_ptr;

} lte_rrc_meas_s;

/*! @brief GSM unique frequency structure
*/
typedef struct
{
  lte_l1_gsm_band_e gsm_band;

  lte_l1_gsm_arfcn_t arfcn;

} lte_rrc_meas_gsm_unique_freq_s;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* LTE_RRC_MEASI_H */
