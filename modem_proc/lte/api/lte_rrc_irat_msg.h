/*!
  @file
  lte_irat_types.h

  @brief
  This file describes the interfaces to LTE Radio resource Controller (RRC_IRAT)

   <!-- Doxygen chokes on the @mainpage vs. @page inside a conditional, but the
       following seems to work.  Basically, make this the mainpage if it
       is the only component being built, otherwise, just make it a page and
       let the upper level component/s @ref it. -->
  @if rrc 
  @mainpage LTE Radio Resource Controller (RRC_IRAT)
  @endif
  @ifnot rrc
  @page LTE Radio Resource Controller (RRC_IRAT)
  @endif

  RRC_IRAT is a sub-module of RRC. This file contains data strucutres and UMIDs
  for messages that RRC will use to communicate with other RATs over 
  message router.

  <h2>High level architecture</h2>
  <img src='../pics/rrc_arch.jpg'>
  
  <h2>API</h2>
 <h3><i>Message Interfaces</i></h3> 
   -- External API to RRC_IRAT is defined in @ref lte_irat_types.h
  
 <h3><i>Function Call Interfaces</i></h3> 
  This module does not have any function call API.

  <b><i> QXDM Log Parser </i></b>
  - @ref QXDM_Parser_DLL

  @ref umids
  @ref all_umids

*/


/*===========================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_rrc_irat_msg.h#1 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------
02/02/12   sk        Interface support for L to G CCO/NACC
09/27/10   np        Add search window in 1x SIB8 ind
06/01/11   np        W2L PSHO interface
04/20/11   np        1x -> LTE Resel interface
04/06/10   sk        Added support for eHRPD pre-registration
09/07/10   sk        Added support for eHRPD to LTE Resel
08/05/10   np        Interface for Transfer of dedicated priorities during OOS
08/04/10   np        Add new failure causes for irat to lte failure cause (sib invalid)
08/03/10   np        Add time for resel fail for which LTE Cell should be barred.
07/14/10   np        Update LTE_RRC_1X_SIB8_INFO_IND with SIB-8 nbr info.
06/21/10   np        Update LTE_RRC_1X_SIB8_INFO_IND with PSIST/Access Bar Inf.
06/17/10   np        Define LTE_RRC_1X_SIB8_INFO_IND for 1xCSFB
04/20/10   np        Update blacklisted PCI to be of lte_phy_cell_id_t
03/12/10   sk        Added Resel failure cause value
03/10/10   np        Remove W Stubs for Capabilities
03/09/2010 sk        Updated lte_rrc_irat_nas_info_s
03/03/10   sk        Added dedicated priorities in G --> L reselection
                     and redirection requests
                     Defined fields in G --> L redirection request
02/20/10   sk        Removed dummy UMIDs and structs related to LTE --> G 
                     redir/resel
02/19/10   np        Remove W Stubs for Redir/Resel
02/18/10   np        Removed fail_cause from Redir Failed
02/10/10   np        W Stub Capabilities payload support
02/09/10   np        Include lte_as.h
02/04/10   np        Interface for G/W -> L Redir 
02/04/10   np        W -> L Redir frequencies contain black listed cells also.
02/02/10   vatsac    Initial version

===========================================================================*/

#ifndef LTE_RRC_IRAT_MSG_H
#define LTE_RRC_IRAT_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <msgr.h>
#include <msgr_lte.h>
#include <lte_as.h>
#include <lte.h>
#include <lte_irat_types.h>
#include <lte_cphy_msg.h>
#include <lte_rrc_ext_msg.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief External defines
*/
/*! WCDMA(FDD/TDD) RRC RedirectionInfo contains a list of max. of 8 LTE redirected frequency list*/
// TODO Check if same applies for GERAN
#define LTE_RRC_IRAT_MAX_REDIR_FREQ               (8)

/*! Max EUTRA Cells per frequency */
#define LTE_RRC_IRAT_MAX_EUTRA_CELLS_PER_FREQ     (16)

/*! Max LTE Neighbor frequency list size */
#define LTE_RRC_IRAT_MAX_NBR_FREQ                 (8) // Based on WCDMA spec

/*! Max LTE Freq from HDR from Spec*/
#define LTE_RRC_IRAT_MAX_FROM_HDR_REDIR_FREQ       7

/*! Max LTE PCI per Freq from HDR from Spec*/
#define LTE_RRC_IRAT_MAX_X2L_REDIR_PCI_PER_FREQ 32

/*! Max Size for E-UTRA Capabilities Container */
#define LTE_MAX_CAP_CONTAINER_LENGTH              1000
                                                 
#define GERAN_CS_MAX_CAP_CONTAINER_LENGTH 39
#define GERAN_PS_MAX_CAP_CONTAINER_LENGTH 52

/*! Max Number of HDR pre-registration zones */
#define LTE_RRC_MAX_HDR_PREREG_ZONES 2

/*! Max Number of HDR Other Rat neighbor list PLMNs */
#define LTE_RRC_IRAT_MAX_ORNL_PLMN   8

/*! Invalid priority for SIB - used currently for sib6/7  */
#define LTE_RRC_INVALID_SIB_PRIORITY 0xFF

/*! @brief External enumerations
*/
/*! @brief Enumeration for Inter-RAT To-LTE Reselection Failure Causes
*/
typedef enum
{
  LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_UNKNOWN             = 0,
  LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_CELL_BARRED         = 1,
  LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_PLMN_MISMATCH       = 2, // same as cell not suitable
  LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_FORBIDDEN_TA        = 3,
  LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_ACQ_FAILED          = 4,
  LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_DL_WEAK             = 5,
  LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_CFG_FAILURE         = 6,
  LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_CELL_SELECT_FAILURE = 7, // same as S_criteria fail 
  LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_SIB_READ_ERROR      = 8, // ** DEPRICATED, DO NOT USE **          
  LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_FREQ_BARRED         = 9,
  LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_FORBIDDEN_CSG_ID    = 10,
  LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_SIB_TIMEOUT_ERROR   = 11,
  LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_SIB_INVALID_ERROR   = 12,
  LTE_RRC_IRAT_TO_LTE_RESEL_FAILURE_CAUSE_DEACTIVATE_BY_NAS   = 13,
}lte_rrc_irat_to_lte_failure_cause_type_e;

/*! @brief External enumerations
*/
/*! @brief Enumeration for Inter-RAT To-LTE Redir Failure Causes
*/
typedef enum
{
  LTE_RRC_IRAT_TO_LTE_REDIR_FAILURE_CAUSE_UNKNOWN             = 0,
  LTE_RRC_IRAT_TO_LTE_REDIR_FAILURE_CAUSE_DEACTIVATE_BY_NAS   = 1,
}lte_rrc_irat_to_lte_redir_failure_cause_type_e;


/*! @brief Handover status */
typedef enum
{
  LTE_RRC_IRAT_TO_LTE_PSHO_SUCCESS,
  LTE_RRC_IRAT_TO_LTE_PSHO_FAILURE,
  LTE_RRC_IRAT_TO_LTE_PSHO_MAX_STATUS
} lte_rrc_irat_to_lte_psho_status_type_e;


/*! @brief Enumeration for Inter-RAT To-LTE PSHO Failure Causes
*/
typedef enum
{
  LTE_RRC_IRAT_TO_LTE_PSHO_FAILURE_CAUSE_NONE                 = 0, // Failure cause is not applicable
  LTE_RRC_IRAT_TO_LTE_PSHO_FAILURE_CAUSE_UNKNOWN              = 1, // Failure cause is unknown
  LTE_RRC_IRAT_TO_LTE_PSHO_FAILURE_CAUSE_INVALID_STATE        = 2, // PSHO received during unexpected state
  LTE_RRC_IRAT_TO_LTE_PSHO_FAILURE_CAUSE_PROTOCOL_ERROR       = 3, // PSHO Container ASN1 decode failure
  LTE_RRC_IRAT_TO_LTE_PSHO_FAILURE_CAUSE_INVALID_CONFIG       = 4, // invalid configuration
  LTE_RRC_IRAT_TO_LTE_PSHO_FAILURE_CAUSE_ACQ_FAILURE          = 5, // acq failures
  LTE_RRC_IRAT_TO_LTE_PSHO_FAILURE_CAUSE_FREQ_NOT_IMPLEMENTED = 6, // unsupported freq
  LTE_RRC_IRAT_TO_LTE_PSHO_FAILURE_CAUSE_RACH_FAILED          = 7, // RACH failed
} lte_rrc_irat_to_lte_psho_failure_cause_type_e;

/*! @brief WCDMA to LTE IRAT Redirection Cause
*/
typedef enum
{
  LTE_RRC_IRAT_WCDMA_REDIR_CAUSE_CONN_RELEASE  = 0,
  LTE_RRC_IRAT_WCDMA_REDIR_CAUSE_CONN_REJECT   = 1,
  LTE_RRC_IRAT_WCDMA_REDIR_CAUSE_BLIND         = 2,
  LTE_RRC_IRAT_WCDMA_REDIR_CAUSE_MAX
} lte_rrc_irat_w_redir_cause_e;

/*! @brief TDSCDMA to LTE IRAT Redirection Cause
*/
typedef enum
{
  LTE_RRC_IRAT_TDSCDMA_REDIR_CAUSE_CONN_RELEASE  = 0,
  LTE_RRC_IRAT_TDSCDMA_REDIR_CAUSE_CONN_REJECT   = 1,
  LTE_RRC_IRAT_TDSCDMA_REDIR_CAUSE_BLIND         = 2,
  LTE_RRC_IRAT_TDSCDMA_REDIR_CAUSE_MAX
} lte_rrc_irat_tds_redir_cause_e;

/*! @brief Enumeration for rat type
*/
typedef enum
{
  LTE_RRC_IRAT_LTE,            /*!< 0 */
  LTE_RRC_IRAT_GERAN,          /*!< 1 */
  LTE_RRC_IRAT_UTRA_FDD,       /*!< 2 */
  LTE_RRC_IRAT_UTRA_TDD,       /*!< 3 */
  LTE_RRC_IRAT_CDMA2000_HRPD,  /*!< 4 */
  LTE_RRC_IRAT_CDMA2000_1xRTT, /*!< 5 */
  LTE_RRC_IRAT_RAT_MAX
} lte_rrc_irat_rat_type_e;

/*! @brief Enumeration for LTE to G CCO NACC status
*/
typedef enum
{
  LTE_RRC_IRAT_CCO_NACC_SUCCESS = 0, /*!< 0 */
  LTE_RRC_IRAT_CCO_NACC_ABORTED = 1,
  LTE_RRC_IRAT_CCO_NACC_MAX
} lte_rrc_cco_nacc_status_e;

/*! @brief Enumeration for Inter-RAT From-LTE Failure Phase - applicable for CCO only
*/
typedef enum
{
  /*!< Failure Phase -- Acquisition */
  LTE_RRC_IRAT_CCO_NACC_FAILURE_PHASE_ACQ                       = 0,

  /*!< Failure Phase -- RACH */
  LTE_RRC_IRAT_CCO_NACC_FAILURE_PHASE_RACH                      = 1,

  /*!< Failure Phase -- MAX value */
  LTE_RRC_IRAT_CCO_NACC_FAILURE_PHASE_MAX                       = 2,

}lte_rrc_irat_cco_nacc_failure_phase_type_e;

/*!
    @brief

    Common NAS related info needed for G/W(FDD/TDD) - LTE IRAT operations like redir/resel
    to LTE needed for camping.
*/
typedef struct
{
  /*! RAT priority list */
  sys_rat_pri_list_info_s_type        rat_pri_list;

  /*! Requested plmn - Network selection mode field below will determine
      if this field is valid and should be considered for processing 
      This field is taken into account only if network selection mode is
      AUTOMATIC or MANUAL
  */
  sys_plmn_id_s_type                  req_plmn;
  
  /*! Network select mode */
  sys_network_selection_mode_e_type   network_select_mode;

  /*! NAS Requested CSG Id */
  sys_csg_id_type                     csg_id;

} lte_rrc_irat_nas_info_s;

/*!
    @brief

    Common NAS related info needed for CDMA - LTE IRAT operations like redir/resel
    to LTE needed for camping.
*/
typedef struct
{
  /*! RAT priority list */
  sys_rat_pri_list_info_s_type        rat_pri_list;

  /*! OBSOLETE: Boolean Requested PLMN has been provided */
  boolean                             req_plmn_is_present;

  /*! Number of requested PLMNs provided. Range is 0-8 for HDR*/
  uint8                               num_req_plmn;

  /*! OBSOLETE: Requested plmn - Network selection mode field below will determine
      if this field is valid and should be considered for processing 
      This field is taken into account only if network selection mode is
      AUTOMATIC or MANUAL
  */
  sys_plmn_id_s_type                  req_plmn;

  /*! Requested plmn - Network selection mode field below will determine
      if this field is valid and should be considered for processing 
      This field is taken into account only if network selection mode is
      AUTOMATIC or MANUAL
  */
  sys_plmn_id_s_type                  requested_plmn[LTE_RRC_IRAT_MAX_ORNL_PLMN];
  
  /*! Network select mode */
  sys_network_selection_mode_e_type   network_select_mode;

} lte_rrc_cdma_irat_nas_info_s;

/*! @brief WCDMA to LTE Redirected EARFCN frequency 
 *         along with blacklisted cells per frequency.
*/
typedef struct
{
  lte_earfcn_t      earfcn;
  uint8             blacklisted_cells_count;
  lte_phy_cell_id_t blacklisted_cells[LTE_RRC_IRAT_MAX_EUTRA_CELLS_PER_FREQ];
} lte_rrc_wcdma_redir_earfcn_info_s;


/*! @brief WCDMA to LTE Redirected EARFCN frequency list along with
           black listed cells for each frequency.
*/
typedef struct
{
  uint8                                 earfcn_count; /*!< Num frequencies present */
  lte_rrc_wcdma_redir_earfcn_info_s     earfcn_info[LTE_RRC_IRAT_MAX_REDIR_FREQ]; /*!< LTE Frequency with black listed cells */
} lte_rrc_wcdma_redir_earfcn_list_s;

/*! @brief TDSCDMA to LTE Redirected EARFCN frequency 
 *         along with blacklisted cells per frequency.
*/
typedef struct
{
  lte_earfcn_t      earfcn;
  uint8             blacklisted_cells_count;
  lte_phy_cell_id_t blacklisted_cells[LTE_RRC_IRAT_MAX_EUTRA_CELLS_PER_FREQ];
} lte_rrc_tdscdma_redir_earfcn_info_s;


/*! @brief TDSCDMA to LTE Redirected EARFCN frequency list along with
           black listed cells for each frequency.
*/
typedef struct
{
  uint8                                 earfcn_count; /*!< Num frequencies present */
  lte_rrc_tdscdma_redir_earfcn_info_s   earfcn_info[LTE_RRC_IRAT_MAX_REDIR_FREQ]; /*!< LTE Frequency with black listed cells */
} lte_rrc_tdscdma_redir_earfcn_list_s;

/*! @brief Neighbor LTE frequency list if camping on redirected frequencies fails
*/
typedef struct
{
  uint8                          earfcn_count; /*!< Num frequencies present */
  lte_earfcn_t                   earfcn[LTE_RRC_IRAT_MAX_NBR_FREQ]; /*!< LTE Frequency */
} lte_rrc_irat_nbr_earfcn_list_s;


/*! @brief Struct for 1x Reg Params present in SIB-8
*/
typedef struct
{

  uint16    sid;                /*!< Used along with NID to control when UE should register/re-register with C2K */
  uint16    nid;                /*!< Used along with SID to control when UE should register/re-register with C2K */
  boolean   multipleSID;        /*!< C2K Multiple SID storage indicator */
  boolean   multipleNID;        /*!< C2K Multiple NID storage indicator */
  boolean   homeReg;            /*!< C2K Home Registration indicator */
  boolean   foreignSIDReg;      /*!< C2K SID Roamer Registration indicator */
  boolean   foreignNIDReg;      /*!< C2K NID Roamer Registration indicator */
  boolean   parameterReg;       /*!< C2K Parameter Change Registration indicator */
  boolean   powerUpReg;         /*!< C2K Power Up indicator */
  uint8     registrationPeriod; /*!< C2K Registration Period */
  uint16    registrationZone;   /*!< C2K Registration Zone */
  uint8     totalZone;          /*!< C2K Number of registration zones to be retained */
  uint8     zoneTimer;          /*!< C2K Zone timer length */
  
  // Rel-9 Additions
  boolean   powerDownReg_present; /*!< Is this optional param present? */
  boolean   powerDownReg;       /*!< If TRUE, UE will perform power down reg when switched off */
} lte_rrc_1x_sib8_reg_params_s;

/*! @brief Struct for HDR Pre-reg Params present in SIB-8
*/
typedef struct
{

  boolean  prereg_allowed; /*!< Pre-registration allowed flag */
  uint8    preregZoneId;   /*!< HDR Pre-registration Zone Id - field valid only if prereg_allowed flag is set to TRUE */
  uint8    numOfSecPreRegZoneID; /* Number of secondary pre-registration zone id */
  uint8    secondary_preregZoneId_list[LTE_RRC_MAX_HDR_PREREG_ZONES]; /*!< HDR Secondary Pre-registration Zone Id List */
} lte_rrc_hdr_sib8_prereg_params_s;

/*! @brief Struct for 1x Access Barring (PSIST) Params present in SIB-8
*/
typedef struct
{
  uint8 ac_barring0to9; /*!< PSIST params for access overload classes 0 - 9 */
  uint8 ac_barring10;   /*!< PSIST params for access overload classs 10 */
  uint8 ac_barring11;   /*!< PSIST params for access overload classe 11 */
  uint8 ac_barring12;   /*!< PSIST params for access overload classe 12 */
  uint8 ac_barring13;   /*!< PSIST params for access overload classe 13 */
  uint8 ac_barring14;   /*!< PSIST params for access overload classe 14 */
  uint8 ac_barring15;   /*!< PSIST params for access overload classe 15 */
  uint8 ac_barringMsg;  /*!< PSIST params for message transmissions */
  uint8 ac_barringReg;  /*!< PSIST params for autonomous registrations */
  uint8 ac_barringEmg;  /*!< PSIST params for emergency calls */
} lte_rrc_1x_sib8_ac_barring_cfg_s;

/*! @brief Message for 1x CSFB related parameters present in SIB8
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  boolean                             sib8_present; /*!< Is SIB-8 present? */

  boolean                             reg_params_present; /*!< Are Reg Params present? */
  lte_rrc_1x_sib8_reg_params_s        reg_params; /*!< 1x CSFB Reg Params */

  boolean                                     system_time_present; /*!< Is System Time present? */
  lte_cphy_irat_cdma_sib8_system_time_info_s  system_time;  /*!< System Time */

  // Rel 9 Addition
  // PSIST/Access Barring Info
  boolean                             ac_barring_present; /*!< Are PSIST/Access Barring params present? */
  lte_rrc_1x_sib8_ac_barring_cfg_s    ac_barring_cfg;     /*!< Access Barring (PSIST) params */

  /*! num of CDMA neighbor cell info elements in the list below */
  uint8                                       neigh_cell_info_list_size;

  /*! List of 1x nbrs from SIB-8 */
  lte_cphy_meas_irat_cdma_neigh_cell_info_s   neigh_cell_info_list[LTE_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS_REL9];

  /*! Search window used to assist in searching for the neighbouring pilots*/
  boolean                             search_window_present;
  uint8                               search_window_size; 

  /*! 1xSvlte support parameter */
  boolean                             csfb_SupportForDualRxUEs_r9;
} lte_rrc_1x_sib8_info_ind_s;


/*! @brief Message containing SIB-7 params
*/
typedef struct
{
  msgr_hdr_s          msg_hdr;      /*!< Message router header */

  boolean             sib7_present; /*!< Is SIB-7 present? */

  int16               priority;     /*!< Highest priority of GERAN nbr */

} lte_rrc_sib7_info_ind_s;

/*! @brief Message containing SIB-6 params 
  W priority field will be invalidated when T neighbors are present
  and vice versa
*/
typedef struct
{
  msgr_hdr_s          msg_hdr;               /*!< Message router header */

  boolean             sib6_present;          /*!< Is SIB-6 present? */

  int16               priority_UTRAN_FDD;    /*!< Highest priority of WCDMA nbr */

  int16               priority_UTRAN_TDD;    /*!< Highest priority of TDSCDMA nbr */

} lte_rrc_sib6_info_ind_s;

typedef struct 
{
  msgr_hdr_s                          msg_hdr; /*!< Message router header */
  /*! LTE Layer Client that is returning Cnf */
  lte_layer_client_id_e               client_id;
}lte_common_msim_crat_feature_cnf_s;

/*! @brief Message for clearing dedicated priorities in LTE
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

} lte_rrc_clear_dedicated_pri_ind_s;

/*! @brief Message for Activation Indication sent from RRC to CM for 
    3GPP2 to 3GPP procedures
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_act_reason_e act_reason; /*!< Activation reason */
  lte_rrc_irat_rat_type_e src_rat; /* Source RAT information */

} lte_rrc_to_CM_act_ind_s;

/*! @brief Message for HDR pre-registration related parameters present in SIB8
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  boolean                             sib8_present; /*!< Is SIB-8 present? */

  lte_rrc_global_cell_id_t           global_cell_id;

  lte_rrc_plmn_s                     camped_plmn;

  lte_phy_cell_id_t phy_cell_id;  /*!< Physical cell id */
  lte_earfcn_t freq;              /*!< Frequency */

  boolean                             prereg_params_present; /*!< Are Pre-reg Params present? */
  lte_rrc_hdr_sib8_prereg_params_s    prereg_params; /*!< HDr Pre-reg Params */

  boolean                                     system_time_present; /*!< Is System Time present? */
  lte_cphy_irat_cdma_sib8_system_time_info_s  system_time;  /*!< System Time */

  /*! num of CDMA neighbor cell info elements in the list below */
  uint8                                       neigh_cell_info_list_size;

  /*! List of HDR nbrs from SIB-8 */
  lte_cphy_meas_irat_cdma_neigh_cell_info_s   neigh_cell_info_list[LTE_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS];
} lte_rrc_hdr_sib8_info_ind_s;

/*! @brief Message for HDR pre-registration related parameters present in Re-config
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_hdr_sib8_prereg_params_s    prereg_params; /*!< HDr Pre-reg Params */

  lte_rrc_global_cell_id_t           global_cell_id;

  lte_rrc_plmn_s                     camped_plmn;

  lte_phy_cell_id_t phy_cell_id;  /*!< Physical cell id */
  lte_earfcn_t freq;              /*!< Frequency */

} lte_rrc_hdr_pre_reg_info_ind_s;

typedef struct
{
  msgr_hdr_s                msg_hdr; /*!< Message router header */
  lte_rrc_cco_nacc_status_e status; /*!< CCO/NACC status */

} 
lte_rrc_cco_nacc_completed_ind_s;

/*! @brief PLMN Search Suspend Request 
*/
typedef struct
{
  /*!< message header type*/
  msgr_hdr_s msg_hdr;
} lte_irat_suspend_plmn_srch_ind_s;


/*! @brief External requests that RRC receives
*/
enum
{
  /* WCDMA -> LTE IRAT Requests */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, WCDMA_RESEL, 0x80, lte_rrc_wcdma_resel_req_s), // LTE_RRC_WCDMA_RESEL_REQ
  MSGR_DEFINE_UMID(LTE, RRC, REQ, WCDMA_REDIR, 0x81, lte_rrc_wcdma_redir_req_s), // LTE_RRC_WCDMA_REDIR_REQ
  MSGR_DEFINE_UMID(LTE, RRC, REQ, WCDMA_ABORT_RESEL, 0x82, lte_rrc_wcdma_abort_resel_req_s), // LTE_RRC_WCDMA_ABORT_RESEL_REQ
  MSGR_DEFINE_UMID(LTE, RRC, REQ, WCDMA_ABORT_REDIR, 0x83, lte_rrc_wcdma_abort_redir_req_s), // LTE_RRC_WCDMA_ABORT_REDIR_REQ

  /* G To LTE Resel Request */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, G_RESEL, 0x84, lte_rrc_G_resel_req_s),

  /* G To LTE Resel Abort Request */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, G_ABORT_RESEL, 0x85, lte_rrc_G_abort_resel_req_s),

  /* G To LTE Redir Request */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, G_REDIR, 0x86, lte_rrc_G_redir_req_s),

  /* G To LTE Redir Abort Request */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, G_ABORT_REDIR, 0x87, lte_rrc_G_abort_redir_req_s),

  /*RAT_xyz-> EUTRA Capabilities Request*/
  MSGR_DEFINE_UMID(LTE, RRC, REQ, EUTRA_CAPABILITIES, 0x88, lte_rrc_eutra_capabilities_req_s),

  /* WCDMA -> LTE PLMN Search */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, WCDMA_PLMN_SRCH,       0x89, lte_irat_plmn_srch_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, WCDMA_ABORT_PLMN_SRCH, 0x8a, lte_irat_abort_plmn_srch_req_type),

  /* G -> LTE PLMN Search */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, G_PLMN_SRCH,       0x8b, lte_irat_plmn_srch_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, G_ABORT_PLMN_SRCH, 0x8c, lte_irat_abort_plmn_srch_req_type),

  /* RAT_xyz-> Dedicated Priorities Request */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, GET_DEDICATED_PRI, 0x8d, lte_rrc_get_dedicated_pri_req_s),

  /* eHRPD To LTE Resel Request */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, eHRPD_RESEL, 0x8e, lte_rrc_eHRPD_resel_req_s),

  /* eHRPD To LTE Resel Abort Request */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, eHRPD_ABORT_RESEL, 0x8f, lte_rrc_eHRPD_abort_resel_req_s),

  /* 1x To LTE Resel Request */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, 1X_RESEL, 0x90, lte_rrc_1x_resel_req_s), // LTE_RRC_1X_RESEL_REQ

  /* 1x To LTE Resel Abort Request */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, 1X_ABORT_RESEL, 0x91, lte_rrc_1x_abort_resel_req_s), // LTE_RRC_1X_ABORT_RESEL_REQ

  MSGR_DEFINE_UMID(LTE, RRC, REQ, WCDMA_PSHO, 0x92, lte_rrc_wcdma_psho_req_s), // LTE_RRC_WCDMA_PSHO_REQ
  MSGR_DEFINE_UMID(LTE, RRC, REQ, WCDMA_ABORT_PSHO, 0x93, lte_rrc_wcdma_abort_psho_req_s), // LTE_RRC_WCDMA_ABORT_PSHO_REQ

    /* TDSCDMA -> LTE IRAT Requests */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, TDSCDMA_RESEL, 0x94, lte_rrc_tdscdma_resel_req_s), // LTE_RRC_TDSCDMA_RESEL_REQ
  MSGR_DEFINE_UMID(LTE, RRC, REQ, TDSCDMA_REDIR, 0x95, lte_rrc_tdscdma_redir_req_s), // LTE_RRC_TDSCDMA_REDIR_REQ
  MSGR_DEFINE_UMID(LTE, RRC, REQ, TDSCDMA_ABORT_RESEL, 0x96, lte_rrc_tdscdma_abort_resel_req_s), // LTE_RRC_TDSCDMA_ABORT_RESEL_REQ
  MSGR_DEFINE_UMID(LTE, RRC, REQ, TDSCDMA_ABORT_REDIR, 0x97, lte_rrc_tdscdma_abort_redir_req_s), // LTE_RRC_TDSCDMA_ABORT_REDIR_REQ
  /* TDSCDMA -> LTE PLMN Search */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, TDSCDMA_PLMN_SRCH,       0x98, lte_irat_plmn_srch_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, TDSCDMA_ABORT_PLMN_SRCH, 0x99, lte_irat_abort_plmn_srch_req_type),
  /* TDSCDMA -> LTE IRAT Requests */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, TDSCDMA_PSHO, 0x9a, lte_rrc_tdscdma_psho_req_s), // LTE_RRC_TDSCDMA_PSHO_REQ
  MSGR_DEFINE_UMID(LTE, RRC, REQ, TDSCDMA_ABORT_PSHO, 0x9b, lte_rrc_tdscdma_abort_psho_req_s), // LTE_RRC_TDSCDMA_ABORT_PSHO_REQ
  /*IRAT->LTE GET PARTIAL PLMN RESULTS Requests*/
  MSGR_DEFINE_UMID(LTE, RRC, REQ, WCDMA_GET_PLMN_PRTL_RESULTS, 0x9c, lte_irat_get_plmn_prtl_results_req_s),// LTE_RRC_WCDMA_GET_PLMN_PRTL_RESULTS_REQ
  MSGR_DEFINE_UMID(LTE, RRC, REQ, TDSCDMA_GET_PLMN_PRTL_RESULTS, 0x9d, lte_irat_get_plmn_prtl_results_req_s),// LTE_RRC_TDSCDMA_GET_PLMN_PRTL_RESULTS_REQ
  MSGR_DEFINE_UMID(LTE, RRC, REQ, G_GET_PLMN_PRTL_RESULTS, 0x9e, lte_irat_get_plmn_prtl_results_req_s),// LTE_RRC_G_GET_PLMN_PRTL_RESULTS_REQ
  MSGR_DEFINE_UMID(LTE, RRC, REQ, UTRA_SERVING_CELL_INFO,  0x9f, lte_rrc_utra_serving_cell_info_req_s), //LTE_RRC_UTRA_SERVING_CELL_INFO_REQ

  /* eHRPD To LTE Redir Request */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, eHRPD_REDIR, 0xA0, lte_rrc_eHRPD_redir_req_s),

  /* eHRPD To LTE Redir Abort Request */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, eHRPD_ABORT_REDIR, 0xA1, lte_rrc_eHRPD_abort_redir_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, ACQ_DB_EARFCN,  0xA2, lte_rrc_acq_db_earfcn_req_s), //LTE_RRC_ACQ_DB_EARFCN_REQ


};

/*!
    @brief

    Payload for the actual redirection request that LTE RRC receives from
    WCDMA RRC.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                msg_hdr;

  /*! IRAT NAS related info needed for camping on LTE */
  lte_rrc_irat_nas_info_s             irat_nas_info;

  /*! Dedicated priorities */
  lte_irat_dedicated_priority_info_s  dedicated_priority_info;
  
  /*! Redirected List that WCDMA RRC RedirectionInfo contains of LTE frequencies */
  lte_rrc_wcdma_redir_earfcn_list_s   redir_earfcn_list;

  /*! LTE Neighbor List that WCDMA RRC gets from SIBs */
  lte_rrc_irat_nbr_earfcn_list_s      nbr_earfcn_list;

  /*! Redirected Cause */
  lte_rrc_irat_w_redir_cause_e        cause;

  /*! Redirected Wait Time if Redirection Cause is due to Connection Reject */
  uint64                              t_wait_time_ms;

  /*! Source cell info for CSG fingerprinting */
  source_cell_info_type               serving_cell_info;

} lte_rrc_wcdma_redir_req_s;

/*!
    @brief

    Payload for the actual handover request that LTE RRC receives from
    WCDMA RRC.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                msg_hdr;

  /*! DSM attachment containing the PSHO Container that W RRC gets
     in the Handover from UTRAN command.
   
     This container is basically the LTE 
     RRCConnectionReconfiguration OTA message sent as a DSM item
     because it is of variable size.
    
     Security key info is encapsulated in the container itself.
   */
  msgr_attach_s                       dsm_attach; 

  /*! IRAT NAS related info needed for camping on LTE */
  lte_rrc_irat_nas_info_s             irat_nas_info;

  /*! Optional IRAT timing transfer information for faster LTE acq. */
  boolean                             timing_tx_info_present;
  lte_irat_timing_tx_info_s           timing_tx_info;
  /*! W-RAT doesnt know the target freq's band as its present HO container so 
   	it passes the W2L HO capabilities explicitly*/
  boolean                             wtol_fdd_supported;
  boolean                             wtol_tdd_supported;
  /*! Source cell info for CSG fingerprinting */
  source_cell_info_type               serving_cell_info;
  
  /*! UTRA cell info for too early handover */
  utra_serving_cell_info_type                 utra_cell_info;

} lte_rrc_wcdma_psho_req_s;

/*!
    @brief

    Payload for the actual rsp that LTE RRC receives from
    WCDMA/TDSCDMA RRC.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                msg_hdr;
  /*! UTRA cell info for too late handover */
  utra_serving_cell_info_type                 utra_cell_info;
} lte_rrc_utra_serving_cell_info_rsp_s;

/*!
    @brief

    Payload for the actual reselection request that LTE RRC receives from
    WCDMA RRC
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                msg_hdr;

  /*! IRAT NAS related info needed for camping on LTE */
  lte_rrc_irat_nas_info_s             irat_nas_info;

  /*! Dedicated priorities */
  lte_irat_dedicated_priority_info_s  dedicated_priority_info;
  
  /*! LTE Frequency information */
  lte_earfcn_t                        earfcn;
  
  /*! LTE Cell Id information */
  lte_phy_cell_id_t                   lte_cell_id;

  /*!< Serving Cell Info */
  source_cell_info_type               serving_cell_info;

  /* CSG reselection */
  boolean                             csg_resel;

} lte_rrc_wcdma_resel_req_s;

/*!
    @brief
    Payload of redirection abort request received by LTE RRC if 
    WCDMA stack decides to abort redirection.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                msg_hdr;

} lte_rrc_wcdma_abort_redir_req_s;


/*!
    @brief
    Payload of reselection abort request received by LTE RRC if 
    WCDMA stack decides to abort reselection.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                msg_hdr;

} lte_rrc_wcdma_abort_resel_req_s;

/*!
    @brief
    Payload of PSHO abort request received by LTE RRC if 
    WCDMA stack decides to abort PSHO.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                msg_hdr;

} lte_rrc_wcdma_abort_psho_req_s;

/*!
    @brief

    Payload for the actual redirection request that LTE RRC receives from
    TDSCDMA RRC.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                msg_hdr;

  /*! IRAT NAS related info needed for camping on LTE */
  lte_rrc_irat_nas_info_s             irat_nas_info;

  /*! Dedicated priorities */
  lte_irat_dedicated_priority_info_s  dedicated_priority_info;
  
  /*! Redirected List that TDSCDMA RRC RedirectionInfo contains of LTE frequencies */
  lte_rrc_tdscdma_redir_earfcn_list_s   redir_earfcn_list;

  /*! LTE Neighbor List that TDSCDMA RRC gets from SIBs */
  lte_rrc_irat_nbr_earfcn_list_s      nbr_earfcn_list;

  /*! Redirected Cause */
  lte_rrc_irat_tds_redir_cause_e        cause;

  /*! Redirected Wait Time if Redirection Cause is due to Connection Reject */
  uint64                              t_wait_time_ms;

} lte_rrc_tdscdma_redir_req_s;

/*!
    @brief

    Payload for the actual handover request that LTE RRC receives from
    TDSCDMA RRC.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                msg_hdr;

  /*! DSM attachment containing the PSHO Container that TDS RRC gets
     in the Handover from UTRAN command.
   
     This container is basically the LTE 
     RRCConnectionReconfiguration OTA message sent as a DSM item
     because it is of variable size.
    
     Security key info is encapsulated in the container itself.
   */
  msgr_attach_s                       dsm_attach; 

  /*! IRAT NAS related info needed for camping on LTE */
  lte_rrc_irat_nas_info_s             irat_nas_info;

  /*! Optional IRAT timing transfer information for faster LTE acq. */
  boolean                             timing_tx_info_present;
  lte_irat_timing_tx_info_s           timing_tx_info;

  /*! TDS-RAT doesnt know the target freq's band as its present HO container so 
    it passes the TDS2L HO capabilities explicitly*/
  boolean                             tdstol_fdd_supported;
  boolean                             tdstol_tdd_supported;

   /*! UTRA cell info for too early handover */
  utra_serving_cell_info_type                 utra_cell_info;

} lte_rrc_tdscdma_psho_req_s;

/*!
    @brief

    Payload for the actual reselection request that LTE RRC receives from
    TDSCDMA RRC
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                msg_hdr;

  /*! IRAT NAS related info needed for camping on LTE */
  lte_rrc_irat_nas_info_s             irat_nas_info;

  /*! Dedicated priorities */
  lte_irat_dedicated_priority_info_s  dedicated_priority_info;
  
  /*! LTE Frequency information */
  lte_earfcn_t                        earfcn;
  
  /*! LTE Cell Id information */
  lte_phy_cell_id_t                   lte_cell_id;

} lte_rrc_tdscdma_resel_req_s;

/*!
    @brief
    Payload of redirection abort request received by LTE RRC if 
    TDSCDMA stack decides to abort redirection.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                msg_hdr;

} lte_rrc_tdscdma_abort_redir_req_s;


/*!
    @brief
    Payload of reselection abort request received by LTE RRC if 
    TDSCDMA stack decides to abort reselection.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                msg_hdr;

} lte_rrc_tdscdma_abort_resel_req_s;

/*!
    @brief
    Payload of PSHO abort request received by LTE RRC if 
    TDSCDMA stack decides to abort PSHO.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                msg_hdr;

} lte_rrc_tdscdma_abort_psho_req_s;

/*! @brief External confirmations that RRC sends
*/

/*! @brief Message for G to LTE Resel Request
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  /*! Frequency of the LTE cell to be selected */
  lte_earfcn_t              earfcn;
  
  /*! Cell id of the LTE cell to be selected */
  lte_phy_cell_id_t        lte_cell_id;

  /*! NAS related paramaters required for camping on LTE */
  lte_rrc_irat_nas_info_s  nas_params;

  /*! Dedicated priorities */
  lte_irat_dedicated_priority_info_s dedicated_priority_info;

}lte_rrc_G_resel_req_s;


/*! @brief Message for G to LTE Resel Abort Request
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

}lte_rrc_G_abort_resel_req_s;

/*! @brief GSM to LTE redirection list structure
*/
typedef struct
{
  /* Measurement bandwidth field is present 
     TRUE indicates the bandwidth field is present, FALSE otherwise
  */
  boolean meas_bandwidth_is_present;

  /* Measurement bandwidth field */
  uint32 meas_bandwidth;

  /*! Frequency of the LTE cell to be selected */
  lte_earfcn_t      earfcn;

  /*! Flag to indicate if physical cell id provided is valid 
      TRUE indicates the phy_cell_id field is present, FALSE otherwise 
  */
  boolean phy_cell_id_is_present;

  /*! LTE Physical Cell ID*/
  lte_phy_cell_id_t phy_cell_id;

  /*! Black listed cell count, 0 indicates none present */
  uint8             blacklisted_cell_cnt;

  /*! Black listed cell count list */
  lte_phy_cell_id_t blacklisted_cell_list[LTE_RRC_IRAT_MAX_EUTRA_CELLS_PER_FREQ];

} lte_rrc_geran_redir_info_s;

typedef struct
{
  /*!< Num of redirection members in the list */
  uint8                       redir_info_list_count; 

  /*!< LTE Frequency with target PCID and blacklisted cell list */
  lte_rrc_geran_redir_info_s  redir_info[LTE_RRC_IRAT_MAX_REDIR_FREQ];

}lte_rrc_geran_redir_list_s;

/*! @brief Message for G to LTE Redir Request
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  /*! Redirection band/freq/cell id list */
  lte_rrc_geran_redir_list_s         redir_list_info;

  /*! LTE Neighbor List obtained through SI messages */
  lte_rrc_irat_nbr_earfcn_list_s     nbr_list_info;

  /*! NAS related paramaters required for camping on LTE */
  lte_rrc_irat_nas_info_s            nas_params;

  /*! Dedicated priorities */
  lte_irat_dedicated_priority_info_s dedicated_priority_info;

  /*! Boolean to indicate Redirection type - UE Initiated or NW Initiated */
  boolean                            redir_is_nw_initiated;

  /*! Boolean to indicate whether redir timer is provided by GERAN */
  boolean                            use_redir_tmr;

  /*! Total Lte scan time for G2L redirection in ms*/
  uint64                             redir_tmr_val;


}lte_rrc_G_redir_req_s;


/*! @brief Message for G to LTE Redir Abort Request
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

}lte_rrc_G_abort_redir_req_s;


/*! @brief Message for RAT_xyz to LTE EUTRA Capabilities Request
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  sys_lte_band_mask_e_type       eutra_band_preference; /*!< EUTRA band preferenceDeprecated-- obselete*/
  lte_rrc_irat_rat_type_e        src_rat; /*!<The RAT originating the EUTRA Capabilities Request */
  lte_irat_capabilities_s        rat_capabilities; /*! < RAT capabilities. */

}lte_rrc_eutra_capabilities_req_s;


/*! @brief PLMN Search Request  
*/
typedef struct
{
  /*!< message header type*/
  msgr_hdr_s msg_hdr;

  /*!< Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type network_select_mode;

  /*!< determines if the plmn search request is for "full service search" or "plmn list search" */
  sys_service_search_e_type search_type;

  /*!< number of PLMNs present in the prioritized list */
  byte num_plmns;

  /*!< PLMN Ids to be searched on the RAT with 0th element being the HPLMN */
  sys_plmn_id_s_type plmn[SYS_PRIORITY_PLMN_LIST_MAX_LENGTH];

  sys_band_cap_u_type band_pref;

  /*!< bst band preferences, only be populated for L2G BPLMN search*/
  sys_band_cap_u_type bst_band_pref;

  /*!< Time allowed for the PLMN search on the RAT */
  uint32 timer_val;

  /*!< Detected Frequencies*/
  detected_frequency_list_type detected_frequency_list;

  /*!< Serving Cell Info */
  source_cell_info_type serving_cell_info;

  sys_scan_scope_e_type   scan_scope;

  boolean foreground_search;

} lte_irat_plmn_srch_req_s;

/*! @brief Get PLMN Partial Results Request  
*/
typedef struct
{
  /*!< message header type*/
  msgr_hdr_s msg_hdr;

} lte_irat_get_plmn_prtl_results_req_s;

/*! @brief PLMN Search Abort Request 
*/
typedef struct
{
  /*!< message header type*/
  msgr_hdr_s msg_hdr;
} lte_irat_abort_plmn_srch_req_s;

/*! @brief Acq DB Info Request 
*/
typedef struct
{
  /*!< message header type*/
  msgr_hdr_s msg_hdr;
} lte_rrc_acq_db_earfcn_req_s;

/*! @brief WCDMA to LTE Redirected EARFCN frequency 
 *         along with blacklisted cells per frequency.
*/
typedef struct
{
  /*! Frequency of the LTE cell to be selected */
  lte_earfcn_t              earfcn;
  
    /*! PLMN List from SIB1 */
  lte_rrc_plmn_list_s plmn_list;  

} lte_rrc_acq_db_list_s;


/*! @brief Message for Acq Db Response
 */
typedef struct
{
  /*! Message header */
 msgr_hdr_struct_type        msg_hdr;
  
 uint8                     num_earfcn_count;

 lte_rrc_acq_db_list_s earfcn_list[10];
  
} lte_rrc_acq_db_earfcn_rsp_s;

/*! @brief UTRA Serving Cell Info Request 
*/
typedef struct
{
  /*!< message header type*/
  msgr_hdr_s msg_hdr;
} lte_rrc_utra_serving_cell_info_req_s;

/*! @brief Dedicated Priorities Request
*/
typedef struct
{
  /*!< message header type*/
  msgr_hdr_s msg_hdr;
} lte_rrc_get_dedicated_pri_req_s;

/*! @brief Message for WCDMA to LTE Resel Response
 */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type        msg_hdr;
   
  /*Carries W cell information which is passed in resel request*/
  source_cell_info_type       w_cell_info;

  /*Carries L macro information*/
  source_cell_info_type       lte_cell_info;
  
} lte_rrc_wcdma_resel_rsp_s;

/*! @brief Message for eHRPD to LTE Resel Request
 */
typedef struct
{
  msgr_hdr_s                msg_hdr; /*!< Message router header */

  /*! Frequency of the LTE cell to be selected */
  lte_earfcn_t              earfcn;
  
  /*! Cell id of the LTE cell to be selected */
  lte_phy_cell_id_t        lte_cell_id;

  /*! NEED TO BE OBSOLETED :: NAS related paramaters required for camping on LTE */
  lte_rrc_irat_nas_info_s  nas_params_info;

  /*! NAS related paramaters required for camping on LTE */
  lte_rrc_cdma_irat_nas_info_s  nas_params;

}lte_rrc_eHRPD_resel_req_s;

/*! @brief Message for eHRPD to LTE Resel Abort Request
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

}lte_rrc_eHRPD_abort_resel_req_s;

/*! @brief Message for 1x to LTE Resel Request
 */
typedef struct
{
  msgr_hdr_s                msg_hdr; /*!< Message router header */

  /*! Frequency of the LTE cell to be selected */
  lte_earfcn_t              earfcn;
  
  /*! Cell id of the LTE cell to be selected */
  lte_phy_cell_id_t         lte_cell_id;

  /*! NAS related paramaters required for camping on LTE */
  lte_rrc_cdma_irat_nas_info_s  nas_params;

}lte_rrc_1x_resel_req_s;

/*! @brief Message for 1x to LTE Resel Abort Request
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

}lte_rrc_1x_abort_resel_req_s;

/*! @brief Message for G to LTE Resel Failed Response
 */
typedef struct
{
  msgr_hdr_s  msg_hdr; /*!< Message router header */

  /*!< Reselection Failure cause */
  lte_rrc_irat_to_lte_failure_cause_type_e fail_cause;

  /*!< Add LTE cell barring time applicable for some failure scenarios */
  uint32                                   cell_bar_time;

}lte_rrc_G_resel_failed_rsp_s;


/*! @brief Message for G to LTE Redir Failed Response
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
}lte_rrc_G_redir_failed_rsp_s;


/*! @brief Message for G to LTE Resel Abort Response
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
}lte_rrc_G_abort_resel_rsp_s;


/*! @brief Message for G to LTE Redir Abort Response
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
}lte_rrc_G_abort_redir_rsp_s;

/*! @brief Message for eHRPD to LTE Resel Failed Response
 */
typedef struct
{
  msgr_hdr_s  msg_hdr; /*!< Message router header */

  /*!< Reselection Failure cause */
  lte_rrc_irat_to_lte_failure_cause_type_e fail_cause;

  /*!< Add LTE cell barring time applicable for some failure scenarios */
  uint32                                   cell_bar_time;

}lte_rrc_eHRPD_resel_failed_rsp_s;

/*! @brief Message for 1x to LTE Resel Failed Response
 */
typedef struct
{
  msgr_hdr_s  msg_hdr; /*!< Message router header */

  /*! Reselection Failure cause */
  lte_rrc_irat_to_lte_failure_cause_type_e fail_cause;

  /*! Add LTE cell barring time applicable for some failure scenarios */
  uint32                                   cell_bar_time;

}lte_rrc_1x_resel_failed_rsp_s;

/*! @brief Message for eHRPD to LTE Resel Abort Response
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
}lte_rrc_eHRPD_abort_resel_rsp_s;

/*! @brief Message for 1x to LTE Resel Abort Response
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
}lte_rrc_1x_abort_resel_rsp_s;

/*! @brief Message for Activation Response from CM to RRC 
    for 3GPP2 to 3GPP procedures
 */
typedef struct
{
    msgr_hdr_s msg_hdr; /*!< Message router header */

    boolean status; /*!< Has CM accepted activation indication? */

} lte_rrc_from_CM_act_rsp_s;

/*! @brief GSM to LTE redirection list structure
*/
typedef struct
{

  /*! Frequency of the LTE cell to be selected */
  lte_earfcn_t      earfcn;

  /*! Count to indicate the number of PCI's included with this earfcn 
      0 indicates the pci_list is invalid
  */
  uint8 pci_count; //optional in the redirection record

  /*! LTE Physical Cell ID*/
  lte_phy_cell_id_t pci_list[LTE_RRC_IRAT_MAX_X2L_REDIR_PCI_PER_FREQ];

} lte_rrc_hdr_redir_info_s;

typedef struct
{
  /*!< Num of redirection members in the list */
  uint8                       redir_info_list_count; 

  /*!< LTE Frequency with target PCID and blacklisted cell list */
  lte_rrc_hdr_redir_info_s  redir_info[LTE_RRC_IRAT_MAX_FROM_HDR_REDIR_FREQ];

}lte_rrc_hdr_redir_earfcn_list_s;

/*! @brief Message for eHRPD to LTE Resel Request
*/
typedef struct
{
  msgr_hdr_s                msg_hdr; /*!< Message router header */

    /*! Redirected List that HDR RRC RedirectionInfo contains of LTE frequencies */
  lte_rrc_hdr_redir_earfcn_list_s   redir_earfcn_list;

  /*! NAS related paramaters required for camping on LTE */
  lte_rrc_cdma_irat_nas_info_s  nas_params;

}lte_rrc_eHRPD_redir_req_s;

/*! @brief Message for HDR to LTE Redir Abort Request
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
}lte_rrc_eHRPD_abort_redir_req_s;

/*! @brief Message for HDR to LTE Redir Abort Response
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
}lte_rrc_eHRPD_abort_redir_rsp_s;

/*! @brief Message for HDR to LTE Redir Failed Response
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
}lte_rrc_eHRPD_redir_failed_rsp_s;

/*! @brief External responses that RRC sends/receives
*/
enum
{
  /* Responses for WCDMA -> LTE IRAT Requests */
  MSGR_DEFINE_UMID(LTE, RRC, RSP, WCDMA_RESEL_FAILED, 0x81, lte_rrc_wcdma_resel_failed_rsp_s), // LTE_RRC_WCDMA_RESEL_FAILED_RSP
  MSGR_DEFINE_UMID(LTE, RRC, RSP, WCDMA_REDIR_FAILED, 0x82, lte_rrc_wcdma_redir_failed_rsp_s), // LTE_RRC_WCDMA_REDIR_FAILED_RSP
  MSGR_DEFINE_UMID(LTE, RRC, RSP, WCDMA_ABORT_RESEL, 0x83, lte_rrc_wcdma_abort_resel_rsp_s), // LTE_RRC_WCDMA_ABORT_RESEL_RSP
  MSGR_DEFINE_UMID(LTE, RRC, RSP, WCDMA_ABORT_REDIR, 0x84, lte_rrc_wcdma_abort_redir_rsp_s), // LTE_RRC_WCDMA_ABORT_REDIR_RSP

  /* G To LTE Resel Failed Response */
  MSGR_DEFINE_UMID(LTE, RRC, RSP, G_RESEL_FAILED, 0x85, lte_rrc_G_resel_failed_rsp_s),
  /* G To LTE Redir Failed Response */
  MSGR_DEFINE_UMID(LTE, RRC, RSP, G_REDIR_FAILED, 0x86, lte_rrc_G_redir_failed_rsp_s),
  /* G To LTE Resel Abort Response */
  MSGR_DEFINE_UMID(LTE, RRC, RSP, G_ABORT_RESEL, 0x87, lte_rrc_G_abort_resel_rsp_s),
  /* G To LTE Redir Abort Response */
  MSGR_DEFINE_UMID(LTE, RRC, RSP, G_ABORT_REDIR, 0x88, lte_rrc_G_abort_redir_rsp_s),

  /*EUTRA->RAT_xyz Capabilities Response */
  MSGR_DEFINE_UMID(LTE, RRC, RSP, EUTRA_CAPABILITIES, 0x89, lte_rrc_eutra_capabilities_rsp_s),

  /* Responses for WCDMA -> LTE PLMN Search */
  MSGR_DEFINE_UMID(LTE, RRC, RSP, WCDMA_PLMN_SRCH,       0x8a, lte_irat_plmn_srch_rsp_s),
  MSGR_DEFINE_UMID(LTE, RRC, RSP, WCDMA_ABORT_PLMN_SRCH, 0x8b, lte_irat_abort_plmn_srch_rsp_s),

  /* Responses for G -> LTE PLMN Search */
  MSGR_DEFINE_UMID(LTE, RRC, RSP, G_PLMN_SRCH,       0x8c, lte_irat_plmn_srch_rsp_s),
  MSGR_DEFINE_UMID(LTE, RRC, RSP, G_ABORT_PLMN_SRCH, 0x8d, lte_irat_abort_plmn_srch_rsp_s),

  /* EUTRA->RAT_xyz Dedicated Priorities Response */
  MSGR_DEFINE_UMID(LTE, RRC, RSP, GET_DEDICATED_PRI, 0x8e, lte_rrc_get_dedicated_pri_rsp_s),

  /* eHRPD To LTE Resel Failed Response */
  MSGR_DEFINE_UMID(LTE, RRC, RSP, eHRPD_RESEL_FAILED, 0x8f, lte_rrc_eHRPD_resel_failed_rsp_s),

  /* eHRPD To LTE Resel Abort Response */
  MSGR_DEFINE_UMID(LTE, RRC, RSP, eHRPD_ABORT_RESEL, 0x90, lte_rrc_eHRPD_abort_resel_rsp_s),

  /* activation Rsp from CM */
  MSGR_DEFINE_UMID(LTE, RRC, RSP, FROM_CM_ACTIVATION, 0x91, lte_rrc_from_CM_act_rsp_s),

  /* 1x To LTE Resel Failed Response */
  MSGR_DEFINE_UMID(LTE, RRC, RSP, 1X_RESEL_FAILED, 0x92, lte_rrc_1x_resel_failed_rsp_s),

  /* 1x To LTE Resel Abort Response */
  MSGR_DEFINE_UMID(LTE, RRC, RSP, 1X_ABORT_RESEL, 0x93, lte_rrc_1x_abort_resel_rsp_s),

  /* W2L PSHO Responses */
  MSGR_DEFINE_UMID(LTE, RRC, RSP, WCDMA_PSHO, 0x94, lte_rrc_wcdma_psho_rsp_s), // LTE_RRC_TDSCDMA_PSHO_RSP
  MSGR_DEFINE_UMID(LTE, RRC, RSP, WCDMA_ABORT_PSHO, 0x95, lte_rrc_wcdma_abort_psho_rsp_s), // LTE_RRC_TDSCDMA_ABORT_PSHO_RSP

  /* Responses for TDSCDMA -> LTE IRAT Requests */
  MSGR_DEFINE_UMID(LTE, RRC, RSP, TDSCDMA_RESEL_FAILED, 0x96, lte_rrc_tdscdma_resel_failed_rsp_s), // LTE_RRC_TDSCDMA_RESEL_FAILED_RSP
  MSGR_DEFINE_UMID(LTE, RRC, RSP, TDSCDMA_REDIR_FAILED, 0x97, lte_rrc_tdscdma_redir_failed_rsp_s), // LTE_RRC_TDSCDMA_REDIR_FAILED_RSP
  MSGR_DEFINE_UMID(LTE, RRC, RSP, TDSCDMA_ABORT_RESEL, 0x98, lte_rrc_tdscdma_abort_resel_rsp_s), // LTE_RRC_TDSCDMA_ABORT_RESEL_RSP
  MSGR_DEFINE_UMID(LTE, RRC, RSP, TDSCDMA_ABORT_REDIR, 0x99, lte_rrc_tdscdma_abort_redir_rsp_s), // LTE_RRC_TDSCDMA_ABORT_REDIR_RSP
   /* Responses for TDSCDMA -> LTE PLMN Search */
  MSGR_DEFINE_UMID(LTE, RRC, RSP, TDSCDMA_PLMN_SRCH,       0x9a, lte_irat_plmn_srch_rsp_s),
  MSGR_DEFINE_UMID(LTE, RRC, RSP, TDSCDMA_ABORT_PLMN_SRCH, 0x9b, lte_irat_abort_plmn_srch_rsp_s),
  /* TDS2L PSHO Responses */
  MSGR_DEFINE_UMID(LTE, RRC, RSP, TDSCDMA_PSHO, 0x9c, lte_rrc_tdscdma_psho_rsp_s), // LTE_RRC_TDSCDMA_PSHO_RSP
  MSGR_DEFINE_UMID(LTE, RRC, RSP, TDSCDMA_ABORT_PSHO, 0x9d, lte_rrc_tdscdma_abort_psho_rsp_s), // LTE_RRC_TDSCDMA_ABORT_PSHO_RSP

  MSGR_DEFINE_UMID(LTE, RRC, RSP, WCDMA_RESEL, 0x9e, lte_rrc_wcdma_resel_rsp_s), // LTE_RRC_TDSCDMA_ABORT_PSHO_RSP

  MSGR_DEFINE_UMID(LTE, RRC, RSP, UTRA_SERVING_CELL_INFO,  0x9f, lte_rrc_utra_serving_cell_info_rsp_s), // LTE_RRC_UTRA_SERVING_CELL_INFO_RSP
    /* eHRPD To LTE Resel Failed Response */
  MSGR_DEFINE_UMID(LTE, RRC, RSP, eHRPD_REDIR_FAILED, 0xA0, lte_rrc_eHRPD_redir_failed_rsp_s),
    /* eHRPD To LTE Resel Failed Response */
  MSGR_DEFINE_UMID(LTE, RRC, RSP, eHRPD_ABORT_REDIR, 0xA1, lte_rrc_eHRPD_abort_redir_rsp_s),
  MSGR_DEFINE_UMID(LTE, RRC, RSP, ACQ_DB_EARFCN,  0xA2, lte_rrc_acq_db_earfcn_rsp_s), //LTE_RRC_ACQ_DB_EARFCN_RSP
};

/*!
    @brief
    Payload if IRAT redirection to LTE fails

    Only reason for failure is that LTE was not able to find
    any suitable cell so returns failure back to WCDMA.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                        msg_hdr;

} lte_rrc_wcdma_redir_failed_rsp_s;


/*!
    @brief
    Payload if IRAT reselection to LTE fails
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                        msg_hdr;

  /*! failure cause */
  lte_rrc_irat_to_lte_failure_cause_type_e    cause;

  /*! Time ( in Sec ) when the same cell should be considered */
  uint32                                      time_to_reselect;
  
} lte_rrc_wcdma_resel_failed_rsp_s;

/*!
    @brief
    Payload if IRAT reselection to LTE fails
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                            msg_hdr;

  /*! Did HO succeed or fail */
  lte_rrc_irat_to_lte_psho_status_type_e          ho_status;

  /*! failure cause only if HO failed */
  lte_rrc_irat_to_lte_psho_failure_cause_type_e   failure_cause;
 
} lte_rrc_wcdma_psho_rsp_s;

/*!
    @brief
    Payload of redirection aborts
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                        msg_hdr;

} lte_rrc_wcdma_abort_redir_rsp_s;


/*!
    @brief
    Payload of reselection aborts
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                        msg_hdr;

} lte_rrc_wcdma_abort_resel_rsp_s;

/*!
    @brief
    Payload of PSHO aborts
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                        msg_hdr;

} lte_rrc_wcdma_abort_psho_rsp_s;

/*!
    @brief
    Payload if IRAT redirection to LTE fails

    Only reason for failure is that LTE was not able to find
    any suitable cell so returns failure back to TDSCDMA.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                        msg_hdr;

} lte_rrc_tdscdma_redir_failed_rsp_s;


/*!
    @brief
    Payload if IRAT reselection to LTE fails
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                        msg_hdr;

  /*! failure cause */
  lte_rrc_irat_to_lte_failure_cause_type_e    cause;

  /*! Time ( in Sec ) when the same cell should be considered */
  uint32                                      time_to_reselect;
  
} lte_rrc_tdscdma_resel_failed_rsp_s;

/*!
    @brief
    Payload if IRAT reselection to LTE fails
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                            msg_hdr;

  /*! Did HO succeed or fail */
  lte_rrc_irat_to_lte_psho_status_type_e          ho_status;

  /*! failure cause only if HO failed */
  lte_rrc_irat_to_lte_psho_failure_cause_type_e   failure_cause;
 
} lte_rrc_tdscdma_psho_rsp_s;

/*!
    @brief
    Payload of redirection aborts
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                        msg_hdr;

} lte_rrc_tdscdma_abort_redir_rsp_s;


/*!
    @brief
    Payload of reselection aborts
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                        msg_hdr;

} lte_rrc_tdscdma_abort_resel_rsp_s;

/*!
    @brief
    Payload of PSHO aborts
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type                        msg_hdr;

} lte_rrc_tdscdma_abort_psho_rsp_s;


/*Message struct for the EUTRA->RAT_xyz Capabilities Response*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msg_hdr;

  msgr_attach_s dsm_attach; /*!< dsm attachment
                                 containing the LTE capabilities Container */

  lte_rrc_irat_rat_type_e  dest_rat; /*!<The dest RAT for the capabilities Response*/
  uint32                capabilities_container_length;
  uint8                 capabilities_container[LTE_MAX_CAP_CONTAINER_LENGTH];
  /*! LTE recent camped bands */
  sys_lte_band_mask_e_type  lte_camped_band_mask;
  /*! LTE neigh bands (source SIB5) */
  sys_lte_band_mask_e_type  lte_neigh_band_mask;
} lte_rrc_eutra_capabilities_rsp_s;


/*! @brief PLMN Search Response 
*/
typedef struct
{
  /*!< message header type*/
  msgr_hdr_s msg_hdr;

  /*!< List of higher priority PLMNs that were found on 3G */
  sys_detailed_plmn_list_s_type available_plmn_list;

  /*!< Indicates if the BPLMN search is done or not */
  boolean srch_done;

  /*!Set to TRUE for Partial Plmn results*/
  boolean partial_srch;

  /*!< Indicates if the highest priority PLMN was found or not */
  boolean hplmn_found;

  /*!< Detected Frequencies */
  detected_frequency_list_type detected_frequency_list;

  /*!< Detected CSG cells */
  detected_csg_cell_list_type detected_csg_cell_list;

  /*!< Detected Macro cells for fingerprinting */
  detected_macro_cell_list_type detected_macro_cell_list;

  /*!< TRUE if the ASF search failed due to lack of FP info with the serving cell,
       FALSE otherwise. If TRUE, srch_done should also be TRUE */
  boolean no_fp_info;
} lte_irat_plmn_srch_rsp_s;


/*! @brief PLMN Search Abort Response 
*/
typedef struct
{
  /*!< message header type*/
  msgr_hdr_s msg_hdr;
} lte_irat_abort_plmn_srch_rsp_s;

/*! @brief Dedicated Priorities Response 
*/
typedef struct
{
  /*! message header type*/
  msgr_hdr_s msg_hdr;

  /*! Registered plmn info of the cell where dedicated priorities were received */
  sys_plmn_id_s_type plmn;

  /*! Boolean field indicating if dedicated priorities are valid
      to account for corner cases when they have expired by the time
      target RAT requests for this information
   */
  boolean dedicated_priorities_valid;

  /*! Dedicated priorities, if valid */
  lte_irat_dedicated_priority_info_s  dedicated_priority_info;
  
} lte_rrc_get_dedicated_pri_rsp_s;

/*! @brief External IRAT indications that RRC throws
*/

enum
{
  MSGR_DEFINE_UMID(LTE, RRC, IND, 1X_SIB8_INFO, 0x81, lte_rrc_1x_sib8_info_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, CLEAR_DEDICATED_PRI, 0x82, lte_rrc_clear_dedicated_pri_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, TO_CM_ACTIVATION,    0x83, lte_rrc_to_CM_act_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, HDR_SIB8_INFO, 0x84, lte_rrc_hdr_sib8_info_ind_s ),
  MSGR_DEFINE_UMID(LTE, RRC, IND, HDR_PRE_REG_INFO, 0x85, lte_rrc_hdr_pre_reg_info_ind_s ),
  MSGR_DEFINE_UMID(LTE, RRC, IND, CCO_NACC_COMPLETED,  0x86, lte_rrc_cco_nacc_completed_ind_s),

  /* W/G -> LTE PLMN Search suspend */
  MSGR_DEFINE_UMID(LTE, RRC, IND, WCDMA_SUSPEND_PLMN_SRCH, 0x87, lte_irat_suspend_plmn_srch_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, G_SUSPEND_PLMN_SRCH, 0x88, lte_irat_suspend_plmn_srch_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, TDSCDMA_SUSPEND_PLMN_SRCH, 0x89, lte_irat_suspend_plmn_srch_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, SIB7_INFO, 0x8A, lte_rrc_sib7_info_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, SIB6_INFO, 0x8B, lte_rrc_sib6_info_ind_s),
};

#endif /* LTE_RRC_IRAT_MSG_H */

