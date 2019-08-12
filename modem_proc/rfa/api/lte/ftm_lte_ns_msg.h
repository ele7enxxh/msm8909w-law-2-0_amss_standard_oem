/*!
  @file ftm_lte_ns_msg.h

  @brief
  This file the messages defiend by the FTM LTE non-signaling module

*/

/*============================================================================
                Copyright (c) 2008 - 2016 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/lte/ftm_lte_ns_msg.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 07:59:52 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/12/14   stw     Add Multiple Scell Support in NS mode
05/13/14   kab     Added Get DL BLER support for all carriers
05/13/14   kab     Added RF-ML1 interface and support for NS CA
05/13/14   kab     Added interface changes for NS CA
03/16/12   php     Added LTE NS support
10/30/10   mkv     Added support for LTE Get Rx Level       
10/29/10   mkv     Added support for LTE Reset and Get Sync BLER API       
10/26/10   mkv     Added LTE NS Tx AGC override        
10/24/10   mkv     Added LTE NS UL Grant reconfiguration  
05/06/10   pl      Adding FTM_LTE_NS_IS_CONN_REQ
03/24/10   pl      Adding redundant FEATURE_FTM_HAS_LTE to enable umid_gen to parse 
                   properly 
03/16/10   pl      Merge FTM_LTE_NS_START_DL and FTM_LTE_NS_START_UL to become FTM_LTE_NS_START_DP
03/02/10   pl      Change Transmission mode to additional specturm emission config
03/01/10   pl      Added Transmission mode and signaling MAX UE Tx power to       
                   START_DL interface
02/18/10   pl      Remove C-RNTI from START_UL interface
11/25/09   pl      Reduced command code to 2 bytes from 4 bytes
11/18/09   pl      Modify DIAG response header to match DIAG command header
10/23/09   pl      Added  FTM LTE NS UL data support
10/01/09   pl      Adding Diag Response packet definitions
08/06/09   pl      Adding payload to START_DL command
07/16/09   pl      Adding START_DL command
06/30/09   pl      Adding payload definition to ACQ_REQ/CNF
06/11/09   pl      Update FTM LTE NS messages with more information
04/23/09   pl      Initial Check-in
===========================================================================*/

#ifndef FTM_LTE_NS_MSG_H
#define FTM_LTE_NS_MSG_H


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <sys.h>
#include <ftm_msgr.h>
#include <lte_l1_types.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/* Create a 256 code for REQ space and CNF space 
 * For request coming from a REX task, UMID is nont used. Instead
 * a command ID will be used. Using the offset below, the incoming
 * messages can be identified as REQ or CNF, while keeping the 
 * a constant base command code constant.
 * The base command code is used in the UMID define for both REQ and CNF
 * UMID. 
 */

/* Command codes for the FTM LTE non-signaling feature */
/* These are the CMD codes base value */
#define FTM_LTE_NS_FIRST_CMD_CODE_C                      0
#define FTM_LTE_NS_START_LTE_MODE_CMD_CODE_C FTM_LTE_NS_FIRST_CMD_CODE_C
#define FTM_LTE_NS_STOP_LTE_MODE_CMD_CODE_C              1
#define FTM_LTE_NS_IDLE_CMD_CODE_C                       2
#define FTM_LTE_NS_ACQ_CMD_CODE_C                        3
#define FTM_LTE_NS_START_DP_CMD_CODE_C                   4
#define FTM_LTE_NS_DL_DATA_IND_CODE_C                    5
#define FTM_LTE_NS_IS_CONN_CMD_CODE_C                    6
#define FTM_LTE_NS_CONFIG_UL_WAVEFORM_CMD_CODE_C         7
#define FTM_LTE_NS_CONFIG_UL_POWER_CMD_CODE_C            8
#define FTM_LTE_NS_GET_DL_LEVEL_CMD_CODE_C               9
#define FTM_LTE_NS_RESET_DL_BLER_REPORT_CMD_CODE_C      10
#define FTM_LTE_NS_GET_DL_BLER_REPORT_CMD_CODE_C        11
#define FTM_LTE_NS_HANDOVER_CMD_CODE_C                  12 
#define FTM_LTE_NS_TDD_CONFIG_CMD_CODE_C                13
#define FTM_LTE_NS_ENABLE_SCELL_CMD_CODE_C              14
#define FTM_LTE_NS_DISABLE_SCELL_CMD_CODE_C             15
#define FTM_LTE_NS_GET_ALL_CARR_DL_BLER_CMD_CODE_C      16
#define FTM_LTE_NS_ENABLE_MULTI_SCELL_CMD_CODE_C        17
#define FTM_LTE_NS_DISABLE_MULTI_SCELL_CMD_CODE_C       18
#define FTM_LTE_NS_MAX_CMD_CODE_C                       19
#define FTM_LTE_NS_INVALID_CMD_CODE_C        (FTM_LTE_NS_MAX_CMD_CODE_C + 1)
#define FTM_LTE_NS_MAX_CARRIERS                          5

/* These messages are defined for use with message router */
enum
{
  MSGR_DEFINE_UMID(FTM, LTE_NS, REQ, START_LTE_MODE, FTM_LTE_NS_START_LTE_MODE_CMD_CODE_C, ftm_lte_ns_start_mode_req_msg_type), /* FTM_LTE_NS_START_LTE_MODE_REQ */
  MSGR_DEFINE_UMID(FTM, LTE_NS, CNF, START_LTE_MODE, FTM_LTE_NS_START_LTE_MODE_CMD_CODE_C, ftm_lte_ns_start_mode_cnf_msg_type), /* FTM_LTE_NS_START_LTE_MODE_CNF */

  MSGR_DEFINE_UMID(FTM, LTE_NS, REQ, STOP_LTE_MODE,  FTM_LTE_NS_STOP_LTE_MODE_CMD_CODE_C,  ftm_lte_ns_stop_mode_req_msg_type), /* FTM_LTE_NS_STOP_LTE_MODE_REQ */
  MSGR_DEFINE_UMID(FTM, LTE_NS, CNF, STOP_LTE_MODE,  FTM_LTE_NS_STOP_LTE_MODE_CMD_CODE_C,  ftm_lte_ns_stop_mode_cnf_msg_type), /* FTM_LTE_NS_STOP_LTE_MODE_CNF */

  MSGR_DEFINE_UMID(FTM, LTE_NS, REQ, IDLE,           FTM_LTE_NS_IDLE_CMD_CODE_C,           ftm_lte_ns_idle_req_msg_type), /* FTM_LTE_NS_IDLE_REQ */
  MSGR_DEFINE_UMID(FTM, LTE_NS, CNF, IDLE,           FTM_LTE_NS_IDLE_CMD_CODE_C,           ftm_lte_ns_idle_cnf_msg_type), /* FTM_LTE_NS_IDLE_CNF */

  MSGR_DEFINE_UMID(FTM, LTE_NS, REQ, ACQ,            FTM_LTE_NS_ACQ_CMD_CODE_C,            ftm_lte_ns_acq_req_msg_type), /* FTM_LTE_NS_ACQ_REQ */
  MSGR_DEFINE_UMID(FTM, LTE_NS, CNF, ACQ,            FTM_LTE_NS_ACQ_CMD_CODE_C,            ftm_lte_ns_acq_cnf_msg_type), /* FTM_LTE_NS_ACQ_CNF */

  MSGR_DEFINE_UMID(FTM, LTE_NS, REQ, START_DP,       FTM_LTE_NS_START_DP_CMD_CODE_C,       ftm_lte_ns_start_dp_req_msg_type), /* FTM_LTE_NS_START_DL_REQ */
  MSGR_DEFINE_UMID(FTM, LTE_NS, CNF, START_DP,       FTM_LTE_NS_START_DP_CMD_CODE_C,       ftm_lte_ns_start_dp_cnf_msg_type), /* FTM_LTE_NS_START_DL_CNF */

  MSGR_DEFINE_UMID(FTM, LTE_NS, IND, DL_DATA,        FTM_LTE_NS_DL_DATA_IND_CODE_C,        ftm_lte_ns_dl_data_ind_msg_type), /* FTM_LTE_NS_DL_DATA_IND */

  MSGR_DEFINE_UMID(FTM, LTE_NS, REQ, IS_CONN,        FTM_LTE_NS_IS_CONN_CMD_CODE_C,        ftm_lte_ns_is_conn_req_msg_type), /* FTM_LTE_NS_IS_CONN_REQ */
  MSGR_DEFINE_UMID(FTM, LTE_NS, CNF, IS_CONN,        FTM_LTE_NS_IS_CONN_CMD_CODE_C,        ftm_lte_ns_is_conn_cnf_msg_type), /* FTM_LTE_NS_IS_CONN_CNF */

  MSGR_DEFINE_UMID(FTM, LTE_NS, REQ, CONFIG_UL_WAVEFORM,  FTM_LTE_NS_CONFIG_UL_WAVEFORM_CMD_CODE_C,    ftm_lte_ns_config_ul_waveform_req_msg_type), /* FTM_LTE_NS_CONFIG_UL_WAVEFORM_REQ */
  MSGR_DEFINE_UMID(FTM, LTE_NS, CNF, CONFIG_UL_WAVEFORM,  FTM_LTE_NS_CONFIG_UL_WAVEFORM_CMD_CODE_C,    ftm_lte_ns_config_ul_waveform_cnf_msg_type), /* FTM_LTE_NS_CONFIG_UL_WAVEFORM_CNF */

  MSGR_DEFINE_UMID(FTM, LTE_NS, REQ, CONFIG_UL_POWER,     FTM_LTE_NS_CONFIG_UL_POWER_CMD_CODE_C,       ftm_lte_ns_config_ul_power_req_msg_type), /* FTM_LTE_NS_CONFIG_UL_POWER_REQ */
  MSGR_DEFINE_UMID(FTM, LTE_NS, CNF, CONFIG_UL_POWER,     FTM_LTE_NS_CONFIG_UL_POWER_CMD_CODE_C,       ftm_lte_ns_config_ul_power_cnf_msg_type), /* FTM_LTE_NS_CONFIG_UL_POWER_CNF */

  MSGR_DEFINE_UMID(FTM, LTE_NS, REQ, GET_DL_LEVEL,        FTM_LTE_NS_GET_DL_LEVEL_CMD_CODE_C,          ftm_lte_ns_get_dl_level_req_msg_type), /* FTM_LTE_NS_GET_DL_LEVEL_REQ */
  MSGR_DEFINE_UMID(FTM, LTE_NS, CNF, GET_DL_LEVEL,        FTM_LTE_NS_GET_DL_LEVEL_CMD_CODE_C,          ftm_lte_ns_get_dl_level_cnf_msg_type), /* FTM_LTE_NS_GET_DL_LEVEL_CNF */

  MSGR_DEFINE_UMID(FTM, LTE_NS, REQ, RESET_DL_BLER_REPORT,       FTM_LTE_NS_RESET_DL_BLER_REPORT_CMD_CODE_C,  ftm_lte_ns_reset_dl_bler_req_msg_type), /* FTM_LTE_NS_RESET_DL_BLER_REQ */
  MSGR_DEFINE_UMID(FTM, LTE_NS, CNF, RESET_DL_BLER_REPORT,       FTM_LTE_NS_RESET_DL_BLER_REPORT_CMD_CODE_C,  ftm_lte_ns_reset_dl_bler_cnf_msg_type), /* FTM_LTE_NS_RESET_DL_BLER_CNF */

  MSGR_DEFINE_UMID(FTM, LTE_NS, REQ, GET_DL_BLER_REPORT,         FTM_LTE_NS_GET_DL_BLER_REPORT_CMD_CODE_C,    ftm_lte_ns_get_dl_bler_req_msg_type), /* FTM_LTE_NS_GET_DL_BLER_REQ */
  MSGR_DEFINE_UMID(FTM, LTE_NS, CNF, GET_DL_BLER_REPORT,         FTM_LTE_NS_GET_DL_BLER_REPORT_CMD_CODE_C,    ftm_lte_ns_get_dl_bler_cnf_msg_type), /* FTM_LTE_NS_GET_DL_BLER_CNF */

  MSGR_DEFINE_UMID(FTM, LTE_NS, REQ, HANDOVER,            FTM_LTE_NS_HANDOVER_CMD_CODE_C,              ftm_lte_ns_handover_req_msg_type), /* FTM_LTE_NS_HANDOVER_REQ */
  MSGR_DEFINE_UMID(FTM, LTE_NS, CNF, HANDOVER,            FTM_LTE_NS_HANDOVER_CMD_CODE_C,              ftm_lte_ns_handover_cnf_msg_type), /* FTM_LTE_NS_HANDOVER_CNF */

  MSGR_DEFINE_UMID(FTM, LTE_NS, REQ, TDD_CONFIG,            FTM_LTE_NS_TDD_CONFIG_CMD_CODE_C,              ftm_lte_ns_tdd_config_req_msg_type), /* FTM_LTE_NS_TDD_CONFIG_REQ */
  MSGR_DEFINE_UMID(FTM, LTE_NS, CNF, TDD_CONFIG,            FTM_LTE_NS_TDD_CONFIG_CMD_CODE_C,              ftm_lte_ns_tdd_config_cnf_msg_type), /* FTM_LTE_NS_TDD_CONFIG_CNF */

  MSGR_DEFINE_UMID(FTM, LTE_NS, REQ, ENABLE_SCELL,            FTM_LTE_NS_ENABLE_SCELL_CMD_CODE_C,              ftm_lte_ns_enable_scell_req_msg_type), /* FTM_LTE_NS_ENABLE_SCELL_REQ */
  MSGR_DEFINE_UMID(FTM, LTE_NS, CNF, ENABLE_SCELL,            FTM_LTE_NS_ENABLE_SCELL_CMD_CODE_C,              ftm_lte_ns_enable_scell_cnf_msg_type), /* FTM_LTE_NS_ENABLE_SCELL_CNF */

  MSGR_DEFINE_UMID(FTM, LTE_NS, REQ, DISABLE_SCELL,            FTM_LTE_NS_DISABLE_SCELL_CMD_CODE_C,              ftm_lte_ns_disable_scell_req_msg_type), /* FTM_LTE_NS_DISABLE_SCELL_REQ */
  MSGR_DEFINE_UMID(FTM, LTE_NS, CNF, DISABLE_SCELL,            FTM_LTE_NS_DISABLE_SCELL_CMD_CODE_C,              ftm_lte_ns_disable_scell_cnf_msg_type), /* FTM_LTE_NS_DISABLE_SCELL_CNF */

  MSGR_DEFINE_UMID(FTM, LTE_NS, REQ, GET_ALL_CARR_DL_BLER,  FTM_LTE_NS_GET_ALL_CARR_DL_BLER_CMD_CODE_C,    ftm_lte_ns_get_all_carr_dl_bler_req_msg_type), /* FTM_LTE_NS_GET_ALL_CARR_DL_BLER_REQ */
  MSGR_DEFINE_UMID(FTM, LTE_NS, CNF, GET_ALL_CARR_DL_BLER,  FTM_LTE_NS_GET_ALL_CARR_DL_BLER_CMD_CODE_C,    ftm_lte_ns_get_all_carr_dl_bler_cnf_msg_type), /* FTM_LTE_NS_GET_ALL_CARR_DL_BLER_CNF */
 
  MSGR_DEFINE_UMID(FTM, LTE_NS, REQ, ENABLE_MULTI_SCELL,            FTM_LTE_NS_ENABLE_MULTI_SCELL_CMD_CODE_C,              ftm_lte_ns_enable_multi_scell_req_msg_type), /* FTM_LTE_NS_ENABLE_MULTI_SCELL_REQ */
  MSGR_DEFINE_UMID(FTM, LTE_NS, CNF, ENABLE_MULTI_SCELL,            FTM_LTE_NS_ENABLE_MULTI_SCELL_CMD_CODE_C,              ftm_lte_ns_enable_multi_scell_cnf_msg_type), /* FTM_LTE_NS_ENABLE_MULTI_SCELL_CNF */

  MSGR_DEFINE_UMID(FTM, LTE_NS, REQ, DISABLE_MULTI_SCELL,            FTM_LTE_NS_DISABLE_MULTI_SCELL_CMD_CODE_C,              ftm_lte_ns_disable_multi_scell_req_msg_type), /* FTM_LTE_NS_DISABLE_MULTI_SCELL_REQ */
  MSGR_DEFINE_UMID(FTM, LTE_NS, CNF, DISABLE_MULTI_SCELL,            FTM_LTE_NS_DISABLE_MULTI_SCELL_CMD_CODE_C,              ftm_lte_ns_disable_multi_scell_cnf_msg_type), /* FTM_LTE_NS_DISABLE_MULTI_SCELL_CNF */

};

typedef PACK(struct)
{
  uint8 sys_id;
  uint8 subsys_id;
  uint16 cmd_id;
  uint16 cmd_code;
} ftm_lte_ns_diag_rsp_pkt_hdr_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_START_LTE_MODE_REQ
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
} ftm_lte_ns_start_mode_req_msg_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_START_LTE_MODE_CNF
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
} ftm_lte_ns_start_mode_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         diag response packet for FTM_LTE_NS_START_LTE_MODE_CNF
 */
typedef PACK(struct)
{
  ftm_lte_ns_diag_rsp_pkt_hdr_type hdr;
  errno_enum_type                  status;
} ftm_lte_ns_start_mode_cnf_diag_rsp_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_STOP_LTE_MODE_REQ
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
} ftm_lte_ns_stop_mode_req_msg_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_STOP_LTE_MODE_CNF
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
} ftm_lte_ns_stop_mode_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         diag response packet for FTM_LTE_NS_STOP_LTE_MODE_CNF
 */
typedef PACK(struct)
{
  ftm_lte_ns_diag_rsp_pkt_hdr_type hdr;
  errno_enum_type                  status;
} ftm_lte_ns_stop_mode_cnf_diag_rsp_type;

/*! @brief This structure holds the parameters of ACQ_REQ that was last sent to LTE L1
           These parameters are used to compared with the ACQ_CNF recevied from LTE L1
*/
typedef PACK(struct)
{
  /*! E-UTRA Band (optional; if 0 L1 calculates band from EARFCN) Range:
      0, 1..14, 33..40 */
  sys_sband_lte_e_type band;
  /* E-UTRA Absolute Radio Frequency Channel Number. This goes with the 
   * band to calcualte the exact frequency to tune to 
   */
  lte_earfcn_t         earfcn;
} ftm_lte_ns_acq_req_data_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_ACQ_REQ
 */
typedef struct
{
  msgr_hdr_struct_type         hdr;
  ftm_lte_ns_acq_req_data_type acq_data;
} ftm_lte_ns_acq_req_msg_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_ACQ_CNF
 */
typedef PACK(struct)
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
  /*! E-UTRA Band (optional; if 0 L1 calculates band from EARFCN) Range:
      0, 1..14, 33..40 */
  sys_sband_lte_e_type band;
  /* E-UTRA Absolute Radio Frequency Channel Number. This goes with the 
   * band to calcualte the exact frequency to tune to 
   */
  lte_earfcn_t         earfcn;
  /* Cell ID of the acquired system. 0..503 */
  lte_phy_cell_id_t    cell_id;
} ftm_lte_ns_acq_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         diag response packet for FTM_LTE_NS_ACQ_CNF
 */
typedef PACK(struct)
{
  ftm_lte_ns_diag_rsp_pkt_hdr_type hdr;
  errno_enum_type                  status;
  /*! E-UTRA Band (optional; if 0 L1 calculates band from EARFCN) Range:
      0, 1..14, 33..40 */
  sys_sband_lte_e_type band;
  /* E-UTRA Absolute Radio Frequency Channel Number. This goes with the 
   * band to calcualte the exact frequency to tune to 
   */
  lte_earfcn_t         earfcn;
  /* Cell ID of the acquired system. 0..503 */
  lte_phy_cell_id_t    cell_id;
} ftm_lte_ns_acq_cnf_diag_rsp_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_IDLE_REQ
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
} ftm_lte_ns_idle_req_msg_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_IDLE_CNF
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
} ftm_lte_ns_idle_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         diag response packet for FTM_LTE_NS_IDLE_CNF
 */
typedef PACK(struct)
{
  ftm_lte_ns_diag_rsp_pkt_hdr_type hdr;
  errno_enum_type                  status;
} ftm_lte_ns_idle_cnf_diag_rsp_type;

/*! @brief This structure holds the parameters of START_DP_REQ that was last sent to LTE L1
           These parameters will be used to configure DP data path
    @param c_rnti the C-RNTI used by the UE
    @param add_spec_emission_cfg The NS_XX value for additional spectrum emission configuration
    @param bs_ue_max_tx_pwr The maximum UE TX power limited by the BS
    @param dl_lc_id  The Logical Channel ID used for the DL channel, 
                     This ID will be used also as Radio Bearer ID, which is
                     needed for RLC configuration but not relevant in 
                     Non-singaling
    @param ul_lc_id  The Logical Channel ID used for UL channel,
                     This ID will be used also as Radio Bearer ID, which is
                     needed for RLC configuration but not relevant in
                     Non-Signaling
    @param ta_adj    Timing Advancement adjustment
*/
typedef PACK(struct)
{
  uint32                ta_adj;
  lte_rnti_val_t        c_rnti;
  uint8                 add_spec_emission_cfg;
  int8                  bs_ue_max_tx_pwr;
  lte_lc_id_t           dl_lc_id;
  lte_lc_id_t           ul_lc_id;
} ftm_lte_ns_dp_req_data_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_START_DP_REQ
 *
 */
typedef struct
{
  msgr_hdr_struct_type        hdr;
  ftm_lte_ns_dp_req_data_type dp_cfg_data;
} ftm_lte_ns_start_dp_req_msg_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_START_DP_CNF
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
} ftm_lte_ns_start_dp_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         diag response packet for FTM_LTE_NS_START_DP_CNF
 */
typedef PACK(struct)
{
  ftm_lte_ns_diag_rsp_pkt_hdr_type hdr;
  errno_enum_type                  status;
} ftm_lte_ns_start_dp_cnf_diag_rsp_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_DL_DATA_IND
 */
typedef struct
{
  msgr_hdr_struct_type hdr;
} ftm_lte_ns_dl_data_ind_msg_type;

/*! @brief This structure defines the message format of the
 *         diag response packet for FTM_LTE_NS_DL_DATA_IND
 */
typedef PACK(struct)
{
  ftm_lte_ns_diag_rsp_pkt_hdr_type hdr;
} ftm_lte_ns_dl_data_ind_diag_rsp_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_IS_CONN_REQ
 */
typedef struct
{
  msgr_hdr_struct_type hdr;
} ftm_lte_ns_is_conn_req_msg_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_IS_CONN_CNF
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
} ftm_lte_ns_is_conn_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         diag response packet for FTM_LTE_NS_IS_CONN_CNF
 */
typedef PACK(struct)
{
  ftm_lte_ns_diag_rsp_pkt_hdr_type hdr;
  errno_enum_type                  status;
} ftm_lte_ns_is_conn_cnf_diag_rsp_type;


/*! @brief This structure holds the parameters of CONFIG_UL_WAVEFORM sent to LTE L1
           
*/
typedef PACK(struct)
{
 uint8 enable_ul_grant_overide;
 uint8 tx_chain;
 uint8 start_rb_index;
 uint8 num_rbs;
 uint8 ul_mcs;
} ftm_lte_ns_config_ul_waveform_req_data_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_CONFIG_UL_WAVEFORM_REQ
 */
typedef struct
{
  msgr_hdr_struct_type         hdr;
  ftm_lte_ns_config_ul_waveform_req_data_type ul_waveform_data;
} ftm_lte_ns_config_ul_waveform_req_msg_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_CONFIG_UL_WAVEFORM_RSP
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
} ftm_lte_ns_config_ul_waveform_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         diag response packet for FTM_LTE_NS_CONFIG_UL_WAVEFORM_RSP
 */
typedef PACK(struct)
{
  ftm_lte_ns_diag_rsp_pkt_hdr_type hdr;
  errno_enum_type                  status;
} ftm_lte_ns_config_ul_waveform_cnf_diag_rsp_type;



/*! @brief This structure holds the parameters of CONFIG_UL_POWER sent to LTE FW
           
*/
typedef PACK(struct)
{
 uint8 tx_chain;
 uint8 tx_power_control_mode;
 int16 tx_level_dBm;
} ftm_lte_ns_config_ul_power_req_data_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_CONFIG_UL_WAVEFORM_REQ
 */
typedef struct
{
  msgr_hdr_struct_type         hdr;
  ftm_lte_ns_config_ul_power_req_data_type ul_power_data;
} ftm_lte_ns_config_ul_power_req_msg_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_CONFIG_UL_WAVEFORM_RSP
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
} ftm_lte_ns_config_ul_power_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         diag response packet for FTM_LTE_NS_CONFIG_UL_WAVEFORM_RSP
 */
typedef PACK(struct)
{
  ftm_lte_ns_diag_rsp_pkt_hdr_type hdr;
  errno_enum_type                  status;
} ftm_lte_ns_config_ul_power_cnf_diag_rsp_type;

/*! @brief This structure holds the parameters of GET_DL_LEVEL sent to LTE L1
*/
typedef PACK(struct)
{
 uint8 rx_chain;
} ftm_lte_ns_get_dl_level_req_data_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_GET_DL_LEVEL_REQ
 */
typedef struct
{
  msgr_hdr_struct_type         hdr;
  ftm_lte_ns_get_dl_level_req_data_type get_dl_level_data;
} ftm_lte_ns_get_dl_level_req_msg_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_GET_DL_LEVEL_CNF
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
  int16                 rx_agc; /*! Rx AGC units */
  int16                 rx_power_dBm; /*! Rx Power in dBm units*/
} ftm_lte_ns_get_dl_level_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         diag response packet for FTM_LTE_NS_GET_DL_LEVEL_CNF
 */
typedef PACK(struct)
{
  ftm_lte_ns_diag_rsp_pkt_hdr_type hdr;
  errno_enum_type                  status;
  int16                            rx_agc; /*! Rx AGC units */
  int16                            rx_power_dBm; /*! Rx Power in dBm units*/
} ftm_lte_ns_get_dl_level_diag_rsp_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_RESET_DL_BLER_REPORT_REQ
 */
typedef struct
{
  msgr_hdr_struct_type hdr;
} ftm_lte_ns_reset_dl_bler_req_msg_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_RESET_DL_BLER_REPORT_CNF
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
} ftm_lte_ns_reset_dl_bler_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         diag response packet for FTM_LTE_NS_RESET_DL_BLER_REPORT_CNF
 */
typedef PACK(struct)
{
  ftm_lte_ns_diag_rsp_pkt_hdr_type hdr;
  errno_enum_type                  status;
} ftm_lte_ns_reset_dl_bler_cnf_diag_rsp_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_GET_DL_BLER_REPORT_REQ
 */
typedef struct
{
  msgr_hdr_struct_type hdr;
} ftm_lte_ns_get_dl_bler_req_msg_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_GET_DL_BLER_REPORT_CNF
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
  uint32                total_blocks_recieved;
  uint32                total_block_errors; /*NACKS*/
  uint32                throughput;  /*Raw bits per sec*/
} ftm_lte_ns_get_dl_bler_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         diag response packet for FTM_LTE_NS_GET_DL_BLER_REPORT_CNF
 */
typedef PACK(struct)
{
  ftm_lte_ns_diag_rsp_pkt_hdr_type hdr;
  errno_enum_type       status;
  uint32                total_blocks_recieved;
  uint32                total_block_errors; /*NACKS*/
  uint32                throughput;  /*Raw bits per sec*/
} ftm_lte_ns_get_dl_bler_cnf_diag_rsp_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_GET_ALL_CARR_DL_BLER_REPORT_REQ
 */
typedef struct
{
  msgr_hdr_struct_type hdr;
} ftm_lte_ns_get_all_carr_dl_bler_req_msg_type;

/*! @brief This structure defines the data format of the
 *         FTM_LTE_NS_GET_DL_BLER_REPORT_CNF
 */
typedef PACK(struct)
{
  /* Indicated which carrier id the data belongs to */
  uint8                 carrier_id;
  uint32                total_blocks_recieved;
  uint32                total_block_errors; /*NACKS*/
  uint32                throughput;  /*Raw bits per sec*/
} ftm_lte_ns_get_all_carr_dl_bler_cnf_data_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_GET_ALL_CARR_DL_BLER_REPORT_CNF
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
  /*! Number of valid elements in dl_bler_per_carrier[] */
  uint8 num_carriers;

  ftm_lte_ns_get_all_carr_dl_bler_cnf_data_type dl_bler_per_carrier[FTM_LTE_NS_MAX_CARRIERS];
} ftm_lte_ns_get_all_carr_dl_bler_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         diag response packet for FTM_LTE_NS_GET_ALL_CARR_DL_BLER_REPORT_CNF
 */
typedef PACK(struct)
{
  ftm_lte_ns_diag_rsp_pkt_hdr_type hdr;
  errno_enum_type       status;
  /*! Number of valid elements in dl_bler_per_carrier[] */
  uint8 num_carriers;

  ftm_lte_ns_get_all_carr_dl_bler_cnf_data_type dl_bler_per_carrier[FTM_LTE_NS_MAX_CARRIERS];
} ftm_lte_ns_get_all_carr_dl_bler_cnf_diag_rsp_type;


/*! @brief This structure holds the parameters of LTE Handover sent to LTE L1
*/
typedef PACK(struct)
{
 uint8   lte_band;
 lte_earfcn_t lte_channel;
} ftm_lte_ns_handover_req_data_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_HANDOVER_REQ
 */
typedef struct
{
  msgr_hdr_struct_type         hdr;
  ftm_lte_ns_handover_req_data_type handover_data;
} ftm_lte_ns_handover_req_msg_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_HANDOVER_CNF
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
} ftm_lte_ns_handover_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         diag response packet for FTM_LTE_NS_HANDOVER_CNF
 */
typedef PACK(struct)
{
  ftm_lte_ns_diag_rsp_pkt_hdr_type hdr;
  errno_enum_type                  status;
} ftm_lte_ns_handover_diag_rsp_type;


/*! @brief This structure holds the parameters of 
           TDD_CONFIG message
*/
typedef PACK(struct)
{
 uint8 subframe_assign;
 uint8 special_sub_fn_patterns;
} ftm_lte_ns_tdd_config_req_data_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_TDD_CONFIG_REQ
 */
typedef struct
{
  msgr_hdr_struct_type         hdr;
  ftm_lte_ns_tdd_config_req_data_type tdd_config_data;
} ftm_lte_ns_tdd_config_req_msg_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_TDD_CONFIG_RSP
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
} ftm_lte_ns_tdd_config_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         diag response packet for FTM_LTE_NS_TDD_CONFIG_RSP
 */
typedef PACK(struct)
{
  ftm_lte_ns_diag_rsp_pkt_hdr_type hdr;
  errno_enum_type                  status;
} ftm_lte_ns_tdd_config_cnf_diag_rsp_type;

/*! @brief This structure holds the parameters of ENABLE_SCELL_REQ to be sent to LTE L1
*/
typedef PACK(struct)
{
  /*! E-UTRA Band (optional; if 0 L1 calculates band from EARFCN) Range:
      0, 1..14, 33..40 */
  sys_sband_lte_e_type band;
  /* E-UTRA Absolute Radio Frequency Channel Number. This goes with the 
   * band to calcualte the exact frequency to tune to 
   */
  lte_earfcn_t         earfcn;

  /*! The cell id this configuration applies to or the source cell in case of
      handover */
  /* Cell ID of the acquired system. 0..503 */
  lte_phy_cell_id_t    cell_id;
} ftm_lte_ns_enable_scell_req_data_type;

typedef struct
{
  uint8                                 carr_cfg_idx;
  ftm_lte_ns_enable_scell_req_data_type scell_data[FTM_LTE_NS_MAX_CARRIERS-1];
} ftm_lte_ns_enable_multi_scell_req_data_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_ENABLE_SCELL_REQ
 */
typedef struct
{
  msgr_hdr_struct_type         hdr;
  ftm_lte_ns_enable_scell_req_data_type enable_scell_data;
} ftm_lte_ns_enable_scell_req_msg_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_ENABLE_MULTI_SCELL_REQ
 */
typedef struct
{
  msgr_hdr_struct_type         hdr;
  ftm_lte_ns_enable_multi_scell_req_data_type enable_multi_scell_data;
} ftm_lte_ns_enable_multi_scell_req_msg_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_ENABLE_SCELL_CNF
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
} ftm_lte_ns_enable_scell_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_ENABLE_MULTI_SCELL_CNF
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
} ftm_lte_ns_enable_multi_scell_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         diag response packet for FTM_LTE_NS_ENABLE_SCELL_CNF
 */
typedef PACK(struct)
{
  ftm_lte_ns_diag_rsp_pkt_hdr_type hdr;
  errno_enum_type       status;
} ftm_lte_ns_enable_scell_cnf_diag_rsp_type;

/*! @brief This structure defines the message format of the
 *         diag response packet for FTM_LTE_NS_ENABLE_MULTI_SCELL_CNF
 */
typedef PACK(struct)
{
  ftm_lte_ns_diag_rsp_pkt_hdr_type hdr;
  errno_enum_type       status;
} ftm_lte_ns_enable_multi_scell_cnf_diag_rsp_type;

/*! @brief This structure holds the parameters for the
 *         FTM_LTE_NS_DISABLE_MULTI_SCELL_REQ
 */
typedef struct
{
  uint8                carr_cfg_idx;
} ftm_lte_ns_disable_multi_scell_req_data_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_DISABLE_SCELL_REQ
 */
typedef struct
{
  msgr_hdr_struct_type hdr;
} ftm_lte_ns_disable_scell_req_msg_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_DISABLE_SCELL_CNF
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
} ftm_lte_ns_disable_scell_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_DISABLE_MULTI_SCELL_REQ
 */
typedef struct
{
  msgr_hdr_struct_type hdr;
  ftm_lte_ns_disable_multi_scell_req_data_type  disable_multi_scell_data;
} ftm_lte_ns_disable_multi_scell_req_msg_type;

/*! @brief This structure defines the message format of the
 *         FTM_LTE_NS_DISABLE_MULTI_SCELL_CNF
 */
typedef struct
{
  msgr_hdr_struct_type  hdr;
  errno_enum_type       status;
} ftm_lte_ns_disable_multi_scell_cnf_msg_type;

/*! @brief This structure defines the message format of the
 *         diag response packet for FTM_LTE_NS_DISABLE_SCELL_CNF
 */
typedef PACK(struct)
{
  ftm_lte_ns_diag_rsp_pkt_hdr_type hdr;
  errno_enum_type       status;
} ftm_lte_ns_disable_scell_cnf_diag_rsp_type;

/*! @brief This structure defines the message format of the
 *         diag response packet for FTM_LTE_NS_DISABLE_MULTI_SCELL_CNF
 */
typedef PACK(struct)
{
  ftm_lte_ns_diag_rsp_pkt_hdr_type hdr;
  errno_enum_type       status;
} ftm_lte_ns_disable_multi_scell_cnf_diag_rsp_type;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
#endif /*  FTM_LTE_NS_MSG_H */
