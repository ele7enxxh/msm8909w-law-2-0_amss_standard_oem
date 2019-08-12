/*!
  @file
  intf_ul.h

  @brief
  This file contains LTE LL uplink command id's and parameters structures

  @detail
  The messages that describe the interface between the ML (UL Manager and 
  Grant Manager) and the LL (lower layer) are described.
 
*/

/*===========================================================================

  Copyright (c) 2008-13 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/intf_ul.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef INTF_UL_H
#define INTF_UL_H

#include "intf_common.h"
#include "intf_ul_msg.h"
#include "msgr.h"
#include "intf_ul_common_type.h"
#include "fw_rf_common_intf.h"

/*===========================================================================

      Constants

===========================================================================*/

#define NUM_DMRS_PER_PUSCH_SUBFRAME 2

/*! @brief  */
typedef enum
{
   SCHED_PUCCH_CSF_REPORT = 0,     ///<.
   SCHED_PUSCH_CSF_REPORT          ///<.
}lte_LL1_csf_sched_chan_type;

typedef enum
{
   LTE_LL1_UL_TX_WINDOW_LENGTH_CHANGE_SUCCESS = 0,  ///<.
   LTE_LL1_UL_TX_WINDOW_LENGTH_CHANGE_FAIL          ///<.
}lte_LL1_ul_tx_window_length_change_enum;


typedef enum
{
   LTE_LL1_UL_IFFT_SCALING_CHANGE_SUCCESS = 0,      ///<.
   LTE_LL1_UL_IFFT_SCALING_CHANGE_FAIL              ///<.
}lte_LL1_ul_ifft_scaling_change_enum;


typedef enum
{
   LTE_LL1_UL_IQ_SAMP_CAPTURE_SUCCESS=0,
   LTE_LL1_UL_IQ_SAMP_CAPTURE_FAIL  
}lte_LL1_ul_iq_samp_capture_enum;

typedef enum
{
   LTE_LL1_UL_ET_RESOURCE_ENABLE_SUCCESS=0,
   LTE_LL1_UL_ET_RESOURCE_ENABLE_FAIL 
}lte_LL1_ul_et_resource_enable_enum;



typedef enum
{
   LTE_LL1_UL_PHYCHAN_CONFIG_SUCCESS = 0,           ///<.
   LTE_LL1_UL_PHYCHAN_CONFIG_FAIL                   ///<.
}lte_LL1_ul_rsp_phychan_config_enum;


typedef enum
{
   LTE_LL1_UL_PHYCHAN_DECONFIG_SUCCESS = 0,         ///<.
   LTE_LL1_UL_PHYCHAN_DECONFIG_FAIL                 ///<.
}lte_LL1_ul_rsp_phychan_deconfig_enum;



/*! @brief PUSCH frequency hopping flag 36.211, sec. 5.3.4 */
typedef enum
{
   LTE_LL1_PUSCH_FREQ_HOPPING_DISABLED = 0,  ///< PRB = VRB from the UL grant
   LTE_LL1_PUSCH_FREQ_HOPPING_ENABLED        ///< Compute VRB to PRB mapping from hopping parameters
}lte_LL1_ul_pusch_freq_hopping_enum;

/*! @brief Enum for testing control information indicating channel in which control info is multiplexed */
typedef enum
{
    LTE_LL1_UL_TEST_CTL_INFO_PUSCH = 0,       ///<  Control info multiplexed on PUSCH
    LTE_LL1_UL_TEST_CTL_INFO_PUCCH            ///<  Control info multiplexed on PUCCH
}lte_LL1_ul_test_ctl_info_chan_type_enum;


/*! @brief Indicates whether scheduling request SR is sent on PUCCH */
typedef enum
{
   LTE_LL1_UL_PUCCH_SR_NOT_SENT = 0,          ///<  Scheduling request not sent on PUCCH
   LTE_LL1_UL_PUCCH_SR_SENT                   ///<  Scheduling request sent on PUCCH
}lte_LL1_ul_pucch_sr_present_enum;


/*! @brief Indicates whether SRS occasion is configured in current subframe ( PUCCH) */
typedef enum
{
   LTE_LL1_UL_PUCCH_SR_NOT_CONFIGURED = 0,   ///<.
   LTE_LL1_UL_PUCCH_SR_CONFIGURED            ///<.
}lte_LL1_ul_pucch_sr_configure_enum;

/*! @brief Indicates whether CSF is sent on PUSCH / PUCCH */
typedef enum
{
   LTE_LL1_UL_CSF_NOT_SENT = 0,       ///< CSF not sent on PUSCH / PUCCH
   LTE_LL1_UL_CSF_SENT,               ///< CSF sent on PUSCH / PUCCH
   LTE_LL1_UL_CSF_DEQUEUED_NOT_SENT   ///< CSF information is dequeued but it is not sent OTA
}lte_LL1_ul_csf_present_enum;


/*! @brief Indicates whether SRS occasion is present in current subframe (PUSCH / PUCCH) */
typedef enum
{
   LTE_LL1_UL_PUSCH_PUCCH_SRS_NOT_PRESENT = 0,   ///<.
   LTE_LL1_UL_PUSCH_PUCCH_SRS_PRESENT            ///<.
}lte_LL1_ul_pusch_pucch_srs_present_enum;

/*! @brief Indicates whether cell specific or ue specific */
typedef enum
{   
   LTE_LL1_SRS_CELL_SPECIFIC = 0,  ///<.
   LTE_LL1_SRS_UE_SPECIFIC    ///<.
}lte_LL1_srs_cell_or_ue_enum;

/*! @brief Indicates whether simultaneous ACK/NACK is on */
typedef enum
{
   LTE_LL1_UL_SRS_SIM_ACK_NAK_OFF=0,        // No simultaneously ACK/NACK SRS
   LTE_LL1_UL_SRS_SIM_ACK_NAK_ON=1,       // Simultaneously ACK/NACK SRS
}lte_LL1_srs_sim_ack_nak_enum;


/*! @brief Indicates the PUCCH ACK NAK reporting mode */
typedef enum
{
  LTE_LL1_UL_FDD = 0,             ///<  FDD mode (multiplexing & bundling are the same)
  LTE_LL1_UL_TDD_ACK_MUX,                ///<  Multiplexing mode for TDD with single carrier
  LTE_LL1_UL_TDD_ACK_BUNDLING,           ///<  Bundling mode for TDD with single carrier
  LTE_LL1_UL_TDD_ACK_PUCCH_1B_CH_SEL, ///< PUCCH FMT 1B with ch selection for TDD single cell
  LTE_LL1_UL_TDD_CA_ACK_PUCCH_1B_CH_SEL, ///< PUCCH FMT 1B with ch selection for TDD+CA
  LTE_LL1_UL_ACK_PUCCH_3          ///< PUCCH FMT 3 for FDD+CA, TDD with single carrier and TDD+CA        
}lte_LL1_ul_ack_reporting_mode_enum;

/*! @brief Indicates whether ACK/NAK is sent on PUSCH / PUCCH */
typedef enum
{
   LTE_LL1_UL_PUSCH_PUCCH_ACK_NOT_SENT = 0,   ///<  ACK/NAK not sent on PUSCH / PUCCH  
   LTE_LL1_UL_PUSCH_PUCCH_ACK_SENT            ///<  ACK/NAK sent on PUSCH / PUCCH
}lte_LL1_ul_pusch_pucch_ack_present_enum;

/*! @brief Indicates whether ACK/NAK is formed based on PDSCH CRC status or 
    forced to NAK by ML due to PDCCH failures within the 'M' SFs containing DL 
    grants
*/
typedef enum
{
  LTE_LL1_UL_PUSCH_PUCCH_USE_PDSCH_CRC = 0,   ///< Use the PDSCH CRC information to form the 
                                              ///< ACK/NAK bits to be sent on UL  
  LTE_LL1_UL_PUSCH_PUCCH_FORCE_NAK           ///< Force a NAK as a DL Grant was missed (TDD Only)
}lte_LL1_ul_pusch_pucch_tdd_ack_build_enum;

/*! @brief Indicates whether ACK/NAK is formed based on PDSCH CRC status or 
    forced to NAK if indicated  by ML1 for FDD
*/
typedef enum
{
  LTE_LL1_UL_PUSCH_PUCCH_USE_PDSCH_CRC_FDD = 0,   ///< Use the PDSCH CRC information to form the 
                                                  ///< ACK/NAK bits to be sent on UL  
  LTE_LL1_UL_PUSCH_PUCCH_FORCE_NAK_FDD           ///< Force a NAK 
}lte_LL1_ul_pusch_pucch_fdd_ack_build_enum;

/*! @brief  */
typedef enum
{
   LTE_LL1_UL_USE_ACK_FROM_DL_IF_PRESENT = 0, ///<.
   LTE_LL1_UL_SEND_ACK = 1,                   ///<.
   LTE_LL1_UL_SEND_NAK = 2                    ///<.
}lte_LL1_ul_trnti_ack_enum;

/*! @brief Indicates UL channels to be configured on receipt of phychan_config message  */
typedef enum
{
  LTE_LL1_UL_CONFIG_PUSCH_PUCCH = 0,            ///<  Configuration for PUSCH / PUCCH
  LTE_LL1_UL_CONFIG_SRS_FDD_TDD,                ///<  Configuration for SRS multiplexed with PUSCH / PUCCH in FDD, TDD mode
  LTE_LL1_UL_CONFIG_UPPTS,                      ///<  Configuration for UpPTS
  LTE_LL1_UL_CONFIG_SRS_TYPE1,                  ///<  Configuration for Aperiodic SRS (Type 1)
  LTE_LL1_UL_CONFIG_CHAN_TYPE_COUNT             ///<  Number of configuration messages 
}lte_LL1_ul_chan_config_enum;

/*! @brief  Indicates type of UL channel: PRACH / PUSCH / PUCCH / SRS only / UpPTS
*/
typedef enum
{
  LTE_LL1_UL_CMD_PUSCH=0,                     ///<.
  LTE_LL1_UL_CMD_PUCCH,                       ///<.
  LTE_LL1_UL_CMD_PRACH,                       ///<.
  LTE_LL1_UL_CMD_SRS,                         ///<.
  LTE_LL1_UL_CMD_SRS_UPPTS,                   ///<.
  LTE_LL1_UL_CMD_DTX,
  LTE_LL1_UL_CHAN_TYPE_COUNT ///< used to identify the number of channels supported
  // add more cmd id's here  
} lte_LL1_ul_chan_type_enum;


/*! @brief PRACH preamble formats specified in 36.211 Tab 5.7.1-1*/
typedef enum
{
   LTE_PRACH_PREAMBLE_FORMAT_0 = 0,           ///<.
   LTE_PRACH_PREAMBLE_FORMAT_1,               ///<.
   LTE_PRACH_PREAMBLE_FORMAT_2,               ///<.
   LTE_PRACH_PREAMBLE_FORMAT_3,               ///<.
   LTE_PRACH_PREAMBLE_FORMAT_4,               ///<.
   LTE_NUM_PRACH_PREAMBLE_FORMATS             ///<.
}lte_LL1_ul_prach_preamble_format_enum;

/*! @brief Flag to indicate inter vs intra sub-frame hopping. 36.211 sec. 5.3.4 */
typedef enum
{
   LTE_LL1_PUSCH_INTER_SF_HOPPING = 0,        ///<  inter subframe hopping only
   LTE_LL1_PUSCH_INTRA_PLUS_INTER_SF_HOPPING  ///<  inter and intra subframe (slot) hopping
}lte_LL1_pusch_sf_hopping_type_enum;


/*! @brief Flag to indicate if group hopping is enabled 
    36.211 sec. 5.5.1.3
*/
typedef enum
{
   LTE_LL1_GROUP_HOPPING_DISABLED=0,          ///<  group hopping disabled
   LTE_LL1_GROUP_HOPPING_ENABLED              ///<  group hopping enabled
}lte_LL1_group_hopping_enum;


/*! @brief Flag to indicate if group hopping is enabled 
    36.211 sec. 5.5.1.4. Note that sequence hopping is 
    only valid for 
*/
typedef enum
{
   LTE_LL1_SEQ_HOPPING_DISABLED=0,            ///<  sequence hopping disabled
   LTE_LL1_SEQ_HOPPING_ENABLED                ///<  sequence hopping enabled
}lte_LL1_seq_hopping_enum;

/*! @brief  Indicates whether SRS starting tone is an even tone */
typedef enum 
{
    LTE_LL1_SRS_EVEN = 0,                     ///<  SRS starting tone is an even tone
    LTE_LL1_SRS_ODD = 1                       ///<  SRS starting tone is an odd tone
} lte_LL1_srs_even_enum_type;

/*! @brief
    Parameter: SRS Bandwidth see TS 36.211 [21, 5.5.3.2: table 5.5.3.2-1].
*/
typedef enum {
  LTE_LL1_SRS_BW0 = 0,                        ///<.
  LTE_LL1_SRS_BW1,                            ///<.
  LTE_LL1_SRS_BW2,                            ///<.
  LTE_LL1_SRS_BW3                             ///<.
} lte_LL1_srs_bw_enum_type;


/*! @brief
    Periodicity of SRS transmissions enum. ms2 corresponds to periodicity of 2ms 
    etc. (ref: [36.213, 8.2],  [36.331, 6.3.2])
*/
typedef enum {
  LTE_LL1_SRS_PERIODICITY_MS2 = 2,              ///<.
  LTE_LL1_SRS_PERIODICITY_MS5 = 5,              ///<.
  LTE_LL1_SRS_PERIODICITY_MS10 = 10,            ///<.
  LTE_LL1_SRS_PERIODICITY_MS20 = 20,            ///<.
  LTE_LL1_SRS_PERIODICITY_MS40 = 40,            ///<.
  LTE_LL1_SRS_PERIODICITY_MS80 = 80,            ///<.
  LTE_LL1_SRS_PERIODICITY_MS160 = 160,          ///<.
  LTE_LL1_SRS_PERIODICITY_MS320 = 320           ///<.
} lte_LL1_srs_periodicity_enum;

/*! @brief
    SRS hopping bandwidth (b_hop)
*/
typedef enum {                                  ///<.
  LTE_LL1_SRS_HOPPING_BW_0 = 0,                 ///<.
  LTE_LL1_SRS_HOPPING_BW_1 = 1,                 ///<.
  LTE_LL1_SRS_HOPPING_BW_2 = 2,                 ///<.
  LTE_LL1_SRS_HOPPING_BW_3 = 3,                 ///<.
  LTE_LL1_SRS_HOPPING_BW_INVALID = 0xFF
} lte_LL1_srs_hopping_bandwidth_enum;

/*! @brief
    SRS bandwidth configuration (c_srs)
*/
typedef enum {
  LTE_LL1_SRS_HOPPING_BW_CONFIG_0 = 0,          ///<.
  LTE_LL1_SRS_HOPPING_BW_CONFIG_1 = 1,          ///<.
  LTE_LL1_SRS_HOPPING_BW_CONFIG_2 = 2,          ///<.
  LTE_LL1_SRS_HOPPING_BW_CONFIG_3 = 3,          ///<.
  LTE_LL1_SRS_HOPPING_BW_CONFIG_4 = 4,          ///<.
  LTE_LL1_SRS_HOPPING_BW_CONFIG_5 = 5,          ///<.
  LTE_LL1_SRS_HOPPING_BW_CONFIG_6 = 6,          ///<.
  LTE_LL1_SRS_HOPPING_BW_CONFIG_7 = 7,           ///<.
  LTE_LL1_SRS_INVALID_HOPPING_BW_CONFIG
} lte_LL1_srs_bandwidth_config_enum;

/*! @brief Indicates modulation type of data in PUSCH  */
typedef enum 
{                                               ///<.
    LTE_LL1_UL_MOD_TYPE_DATA_BPSK = 0,          ///<.
    LTE_LL1_UL_MOD_TYPE_DATA_16QAM = 2,         ///<.
    LTE_LL1_UL_MOD_TYPE_DATA_QPSK = 1,          ///<.
    LTE_LL1_UL_MOD_TYPE_DATA_64QAM = 3,         ///<.
} lte_LL1_tx_mod_type_data_enum_type;


/*! @brief Periodic PUCCH CQI reporting modes configured semi-statically 
    by higher layers 36.213 Tab. 7.2.2-1.
*/
typedef enum
{
   PUCCH_CSF_REPORT_MODE_1_0 = 0,               ///<.
   PUCCH_CSF_REPORT_MODE_1_1,                   ///<.
   PUCCH_CSF_REPORT_MODE_2_0,                   ///<.
   PUCCH_CSF_REPORT_MODE_2_1,                   ///<.
   PUCCH_CSF_INVALID_REPORT_MODE                ///<.
}lte_LL1_pucch_csf_report_mode_enum;

/*! @brief Periodic PUCCH CQI reporting mode 1-1 submode 
    configured semi-statically by higher layers 36.213 7.2.2
*/
typedef enum
{
   PUCCH_CSF_REPORT_MODE_1_1_SUBMODE_INVALID = 0,  ///< Invalid submode
   PUCCH_CSF_REPORT_MODE_1_1_SUBMODE_1       = 1,  ///< submode 1
   PUCCH_CSF_REPORT_MODE_1_1_SUBMODE_2       = 2,  ///< submode 2
}lte_LL1_pucch_csf_report_submode_enum;

/*! @brief Test mode for CSF so that the TX FIFO is not filled.
*/
typedef enum
{
   LTE_LL1_CSF_TEST_MODE_OFF = 0,
   LTE_LL1_CSF_TEST_MODE_ON
}lte_LL1_csf_test_mode_enum;


/*! @brief Periodic PUCCH CQI reporting types added for CSF schedule message. 
    Types are semi-statically configured through higher layers 36.213 Tab. 7.2.2-1.
*/
typedef enum
{
   PUCCH_CSF_REPORT_TYPE_1A_SBCQI_PMI2 = 0,          ///< type 1A: SB CQI and PMI_2
   PUCCH_CSF_REPORT_TYPE_1_SBCQI = 1,                ///< Type 1: SB CQI
   PUCCH_CSF_REPORT_TYPE_2_WBCQI_PMI = 2,            ///< Type 2: WB CQI, PMI
   PUCCH_CSF_REPORT_TYPE_3_RI = 3,                   ///< Type 3: RI
   PUCCH_CSF_REPORT_TYPE_4_WBCQI = 4,                ///< Type 4: WB CQI
   PUCCH_CSF_REPORT_TYPE_5_RI_WBPMI1 = 5,            ///< Type 5 report: RI and WB PMI_1
   PUCCH_CSF_REPORT_TYPE_6_RI_PTI = 6,               ///< Type 6 report: RI and PTI
   PUCCH_CSF_REPORT_TYPE_2A_WBPMI = 7,               ///< type 2A: WB PMI
   PUCCH_CSF_REPORT_TYPE_2B_WBCQI_WBPMI1_WBCQI2 = 8, ///< type 2B: WB CQI, WB PMI_1, differential WB CQI_2 if RI>1
   PUCCH_CSF_REPORT_TYPE_2C_WBCQI_WBPMI1_WBPMI2_WBCQI2 = 9,///< type 2C: WB CQI, WB PMI_1, WB PMI_2, differential WB CQI_2 if RI>1
   PUCCH_CSF_MAX_RPT_TYPE
}lte_LL1_pucch_reporting_type_enum;


/*! @brief Indicates the contents of the UpPTS sub-frame */
typedef enum
{
  LTE_LL1_UPPTS_PRACH = 0,        ///<  PRACH format 4 sent on UpPTS
  LTE_LL1_UPPTS_SRS_0,            ///<  One SRS sent on UpPts symbol 0 only
  LTE_LL1_UPPTS_SRS_1,            ///<  One SRS sent on UpPts symbol 1 only
  LTE_LL1_UPPTS_SRS_0_1           ///<  SRS sent on both UpPts symbols
}lte_LL1_ul_uppts_type_enum;


/*! @brief Flag to indicate is srs_max_uppts is enabled */
typedef enum
{
   LTE_LL1_SRS_MAX_UPPTS_DISABLED = 0,       ///<.
   LTE_LL1_SRS_MAX_UPPTS_ENABLED             ///<.
}lte_LL1_ul_srs_max_uppts_enum;


/*! @brief SRS Type and trigger */
typedef enum
{
   LTE_LL1_SRS_TRIGGER_TYPE0 = 0,           ///< Periodic(Type 0) SRS
   LTE_LL1_SRS_TRIGGER_TYPE1_DCI0 = 1,      ///< Aperiodic(Type 1) SRS triggered by DCI0
   LTE_LL1_SRS_TRIGGER_TYPE1_DCI1A2B2C = 2, ///< Aperiodic(Type 1) SRS triggered by DCI1A/2B/2C
}lte_LL1_ul_srs_trigger_type_enum;

/*===========================================================================

      Macros

===========================================================================*/


/*===========================================================================

      Typedefs

===========================================================================*/


/*! 
   @brief 
   Dynamic parameters for SRS dedicated occasion.
   This structure definitions contains SRS fields that change every sub-frame. 
*/
typedef struct 
{
  uint16                           t_offset;           ///<  SRS subframe offset 36.211 table 8.2.2
  lte_LL1_srs_cell_or_ue_enum      srs_occasion;       ///<  Indicate cell / UE specific SRS occasion
  int16                            beta_srs;   
 ///<  Amplitude scaling factor (used for SRS in PUSCH / PUCCH / SRS only / UpPts)
  int32                            total_tx_power_srs; ///< dBm, will be combined with total_tx_power when it's not SRS only  
  lte_LL1_ul_srs_trigger_type_enum srs_trigger_type;   ///<  Indicate SRS Trigger Type
}lte_LL1_srs_dyn_params_struct;

/*! 
   @brief Dynamic parameters for PRACH generation.
   This structure definition contain fields that are subject to change every preamble. 
*/
typedef struct 
{
   uint16 n_ra_prb;              ///< resource block number (36.211 sec. 5.7.3)
   uint16 chu_base_index_u;      ///< uth root sequence (36.211 sec. 5.7.2)
   uint16 chu_cyclic_shift_cv;   ///< cyclic shift (36.211 sec. 5.7.2)
   int32 total_tx_power;         ///< total PRACH transmit power.
   int16  beta_prach;            ///< PRACH digital gain
   uint8  preamble_seq_index;  ///< Preamble sequence index [0, 64)
   lte_LL1_ul_prach_preamble_format_enum preamble_format;
}lte_LL1_ul_prach_dyn_params_struct;

/*! 
   @brief Parameters for the first PUSCH phychan schedule request parameters
   when retx_index = 0.with the same hard_idx
*/
typedef struct 
{
  uint8  srs_type;          /// < 0: no SRS, 1: UE SRS, 2: CELL-SRS
  uint8  i_harq_ack_offset; ///< higher layer signalled index for HARQ-ACK 36.213 table 8.6.3-1. used to compute q_prime
  uint8  i_cqi_offset;      ///< higher layer signalled index for CQI 36.213 table 8.6.3-3. used to compute q_prime
  uint8  i_ri_offset;       ///< higher layer signalled index for RI 36.213 table 8.6.3-2. used to compute q_prime
  uint8  num_rb;            ///< number of RB's allocated for PUSCH transmission 
}lte_LL1_first_pusch_dyn_params_struct;

/*! @brief 
    Dynamic parameters for PUSCH generation.
    This structure definition contain fields that are subject to change every subframe.
*/
typedef struct
{
   uint32 transp_block_size_A;        ///< transport block size (36.212 sec. 5.1.1).
                                      ///< Total number of bits in transport block excluding CRC bits.

   int32 total_tx_power;    ///< total TX power used for PUSCH
   int16 beta_pusch;        ///< Gain for data and reference symbols on PUSCH (36.211 sec. 5.3.4 and 5..5.2.1.2)
                            ///< format is in dB S11.4
   uint16 n_rnti;           ///< UE ID required for initializing the scrambling state generator (36.211 sec. 5.3.1)
   uint16 n_2_dmrs;         ///< Used in determining the DM-RS cyclic shift, (36.211 sec. 5.5.2.1.1)

   uint8  starting_rb_k0;   ///< starting resource block from UL grant (36.211 sec. 5.3.4)
   uint8  rv;               ///< redundancy version, required for rate matching (36.212 sec. 5.1.4.1.2)
   uint8  retx_index;       ///< retransmission index (required for logging)
   uint8  num_rb;           ///< number of RB's allocated for PUSCH transmission 
   uint8  subframe_num;     ///< Subframe number when PUSCH needs to be sent OTA.
   uint8  antenna_num;      ///< The antenna number to schedule the PUSCH transmission.
   uint8  harq_idx;         ///< HARQ index [0-7] used to index into the encoder RAM
                            ///< For Genesis, HARQ index is sufficient. HW starts from position 0 in the HARQ bank

   // Index into Tables 8.6.3.1-8.6.3.3. The mapping is done from these values to beta_offset values
   // within LL1.
   uint8 i_harq_ack_offset; ///< higher layer signalled index for HARQ-ACK 36.213 table 8.6.3-1. used to compute q_prime
   uint8 i_cqi_offset;      ///< higher layer signalled index for CQI 36.213 table 8.6.3-3. used to compute q_prime
   uint8 i_ri_offset;       ///< higher layer signalled index for RI 36.213 table 8.6.3-2. used to compute q_prime

   uint8 ack_nak_len; ///<.

   int8 n_dmrs; ///< required for P-HICH 

   lte_LL1_ul_pusch_freq_hopping_enum pusch_freq_hopping_flag;  ///<  if disabled then PRB=VRB, else use pre-defined hopping seq 

   lte_LL1_tx_mod_type_data_enum_type  mod_type_data; ///< modulation type for data. Refer 36.211 sec. 5.3.4

   uint8 pusch_hopping_payload;     ///< Refer 36.213 Table 8.4.2 (info in hopping bits) used along with N_ul_rb
                                    ///< to determine the VRB-PRB mapping. 

   lte_LL1_ul_pusch_pucch_srs_present_enum  srs_present_flag;           ///< Indicate if SRS occasion is present in current subframe
                                                                        ///< and multiplexed with PUSCH

   lte_LL1_ul_csf_present_enum csf_present_flag;                        ///<.

   lte_LL1_ul_pusch_pucch_ack_present_enum ack_nak_present_flag;        ///<.

   lte_LL1_ul_ack_reporting_mode_enum ack_reporting_mode;               ///<.

   lte_LL1_ul_pusch_pucch_tdd_ack_build_enum tdd_ack_build;             ///<.

   lte_LL1_ul_pusch_pucch_fdd_ack_build_enum fdd_ack_build;

   uint8  n_bundled;                ///< N_bundled parameter(TDD Only) (36.213 sec. 7.3)

   lte_LL1_csf_sched_chan_type        csf_report_chan_type;
   boolean                            dci_adjusted_pusch;
   uint8                              w_ul_dai;
   uint16                             ack_nak_bits_to_send[LTE_LL1_CARRIER_COUNT]; 
   ///< Specifies which ack/nak bits to send in multiplexing
   ///< Bit 'n' will specify the value in the set K (From 36.213 Table 10.1-1)
   ///< Oack will equal the total number of bits set in this array.
   ///< Bit 0 will correspond to k_0 and bit1 will correspond to k_1 etc.
   ///<
   ///< For instance in ul/dl cfg 4, with 
   ///< ack_nak_bits_to_send = 1010 (in binary ) = 0xa (in hex) 
   ///< will mean only pick up 
   ///< ack feedback for UL subfn 2 from DL subframes n-8 (subfn 4) and 
   ///< (n-11) (subfn 1)

   lte_LL1_first_pusch_dyn_params_struct first_pusch_dyn_params; ///< first PUSCH phychan schedule request parameters when retx_index = 0.

   /// TTI Bundling: For a TTI bundled transmission, PHICH feedback for a bundle is 
   /// only received in one subframe for the entire bundle (even if some transmissions 
   /// were aborted due to the presence of a measurement gap).
   /// So in the simplest Case :
   /// -  Grant received at N
   /// -  PUSCH transmitted in N+4, N+5,N+6,N+7
   /// -  PHICH received in N+11 for the entire bundle.
   /// A case with gaps :
   /// -  Grant received at N
   /// -  Theres a 6ms measurement gap starting from N+5 to N+10 
   /// -  PUSCH transmitted in N+4. (N+5,N+6,N+7 cannot be txed because of the gap)
   /// -  PHICH still received in N+11 for the entire bundle.
   /// So pseudo-code:
   ///    if tti_bundle_index >= 0 for subframe N,
   ///        then flag PHICH decoding at subframe (N+4+(3-tti_bundle_index))

   int8     tti_bundle_index; // -1: TTI bundle disabled
                            // 0: bundle index 0 (first tx of the bundle)
                            // 1: bundle index 1
                // 2: bundle index 2
                // 3: bundle index 3 (last tx of this bundle)
   uint8    eib_idx;         // encoder input index, range=0,1 

   uint8    tx_ant_index;   // TX antenna index, range=0,1

   boolean 	is_sps_configured_tx; // True if in SPS mode 
   
   boolean  delta_mcs_enabled; //Additional pwr ctrl for PUSCH ctrl only ( used when transp_block_size_A = 0)
}lte_LL1_ul_pusch_dyn_params_struct;



/*! @brief 
  Dynamic parameters for PUCCH generation.
  This structure definition contain fields that are subject to change every subframe.
*/
typedef struct 
{
   int32 total_tx_power;   ///< total TX power used for PUCCH
   int16 beta_pucch;    ///< Gain for control tones/symbols on PUCCH (TBD if this is computed by TX CTL)
   uint16 n_rnti;       ///< UE ID required for initializing the scrambling state generator (36.211 sec. 5.4.2)
   uint8  subframe_num; ///<.
   uint8 ack_nak_len;   ///<.

   uint16 n_1_pucch;    ///< Resource index for formats 1/1a/1b (36.211 sec. 5.4.1)

   uint16 n_1_pucch_i[4]; ///< Resource indices for PUCCH 1/1a/1b formats in TDD and CA.
              ///< size = max{M} = 4, where M is defined in 36.213 sec. 10.1

   int32 n_2_pucch;       ///< Resource index for PUCCH 2/2a/2b formats in 

   int32 n_3_pucch;        ///< Resource index for PUCCH format 3

   lte_LL1_ul_trnti_ack_enum trnti_ack_flag;      ///< used for message 4 HARQ for PRACH

   lte_LL1_ul_pusch_pucch_srs_present_enum  srs_present_flag;           ///< Indicate if SRS occasion is present in current subframe
                                    ///< and multiplexed with PUCCH

   lte_LL1_ul_csf_present_enum csf_present_flag;  ///<.

   lte_LL1_ul_pusch_pucch_ack_present_enum  ack_nak_present_flag;       ///< ack_nak length (num CW on DL)

   lte_LL1_ul_pucch_sr_present_enum sr_present_flag;                    ///<.

   lte_LL1_ul_ack_reporting_mode_enum ack_reporting_mode;               ///<.

   lte_LL1_ul_pusch_pucch_tdd_ack_build_enum tdd_ack_build;             ///<.

   lte_LL1_ul_pusch_pucch_fdd_ack_build_enum fdd_ack_build;

   lte_LL1_csf_sched_chan_type        csf_report_chan_type;

   uint16                             ack_nak_bits_to_send[LTE_LL1_CARRIER_COUNT];  //used similar to DL DAI

   lte_LL1_ul_pucch_sr_configure_enum sr_configure_flag;    ///< Indicate if SR is configured or not. in pucch fmt 3, 
                                                            ///< report 0 if it's configured but negative. otherwise report 1.
   boolean                            format_fallback_flag; ///< True if FW needs to use format fallbck for PUCCH TDD 
}lte_LL1_ul_pucch_dyn_params_struct;

/*! @brief 
  Dynamic parameters for UpPTS subframe
  This structure definition contains fields that are subject to change on every transmission of UpPTS
*/
typedef struct 
{
   lte_LL1_ul_uppts_type_enum   uppts_type; ///<  Indicates UpPts type: PRACH or SRS (one SRS, two SRS's)
   uint8                        n_ra;       ///<  Number of format 4 PRACH instances in UpPTS. Refer 36.211 sec. 5.5.3.2, Tab. 5.7.1-4
 //  boolean                      t_ra_1;     ///<  Indicates half frame location of RACH resource (0 or 1). Refer 36.211 Tab. 5.7.1-4
}lte_LL1_ul_uppts_dyn_params_struct;

/*! 
  @brief Used to configure semi-static parameters from RRC that do not change 
  on a subframe basis. This SRS configuration is for both PUSCH and PUCCH (which
  handles also SRS_alone).  This structure shall be included in pusch_pussh_stat
  structure.
*/
typedef struct 
{
   lte_LL1_srs_bw_enum_type               b_srs;          ///<  SRS Bandwidth
   lte_LL1_srs_hopping_bandwidth_enum     b_hop;          ///<  SRS hopping bandwidth (Set to invalid for Type1 SRS)
   lte_LL1_srs_periodicity_enum           t_srs;          ///<  UE specific periodicity
   uint16                                 n_rrc;          ///<  SRS frequency domain position
   lte_LL1_srs_even_enum_type             k_tc;           ///<  transmission combining
   lte_l1_srs_cyclic_shift_e              cyclic_shift;   ///<  Used to calculate n-SRS  - n_cs_srs as defined in spec. 36.211

   lte_LL1_srs_sim_ack_nak_enum           sim_ack_nak_srs;///<  Flag indicating whether SRS and ACK can be transmitted simultaneously
   uint16                                 t_offset_max;   ///<  SRS max subframe offset 36.211 table 8.2.2
   boolean                                srs_ant_switch_en; 
}lte_LL1_ul_srs_stat_params_struct;


/*! 
    @brief Used to configure semi-static parameters from RRC that do not change 
  on a subframe basis. This configuration is for both PUSCH and PUCCH.
*/
typedef struct 
{
   uint16 n_1_dmrs;           ///< Used in determining the DM-RS cyclic shift, (36.211 sec. 5.5.2.1.1)
   uint8  n_rb_pucch;         ///< number of RBs used for PUCCH transmission 36.211 sec. 5.3.4
   uint8  n_sb;               ///< number of subbands used for PUSCH transmission 36.211 sec. 5.3.4    
   uint8  delta_ss;           ///< required for sequence shift pattern
   lte_l1_pucch_delta_shift_e pucch_delta_shift;  ///< higher layer parameter used for computing circular shift, orthogonal cover index
   uint8  n_cs_1;             ///< Number of cyclic shifts used for PUCCH for a resource block with a mix of 1/1a/1b & 2/2a/2b 36.211 5.4
   uint8  n_rb_2;             ///< Number of RB's used for transmission of formats 2/2a/2b (36.211 sec. 5.4.2)

   lte_LL1_pusch_sf_hopping_type_enum pusch_sf_hopping; ///< hopping on a slot boundary [1=>configured, 0=> no hopping]
   lte_LL1_group_hopping_enum pusch_group_hopping_flag; ///< indicates group hopping
   lte_LL1_seq_hopping_enum pusch_seq_hopping_flag;     ///< indicates sequence hopping
   lte_LL1_srs_bandwidth_config_enum      c_srs;          ///<  SRS bandwidth configuration                                                        
   lte_LL1_ul_srs_max_uppts_enum      srs_max_uppts_flag; ///<  Cell specific parameter 36.211 sec. 5.5.3.2

}lte_LL1_ul_pusch_pucch_stat_params_struct;

#if 0
/*! 
    @brief Used to configure semi-static parameters from RRC that do not change 
    on a subframe basis. This configuration is for UpPTS
*/
/*
typedef struct 
{
   lte_LL1_ul_srs_max_uppts_enum      srs_max_uppts_flag; ///<  Cell specific parameter 36.211 sec. 5.5.3.2
   lte_LL1_ul_srs_stat_params_struct  srs_stat_tdd0;      ///<  SRS0 structure for UpPTS
   lte_LL1_ul_srs_stat_params_struct  srs_stat_tdd1;      ///<  SRS1 structure for UpPTS
}lte_LL1_ul_uppts_stat_params_struct;
*/

/*! 
    @brief Type 3 report comprising of RI only. Ref. 36.213 Tab. 7.2.2-3
*/
typedef struct
{
   uint8 ri_val;  ///< actual RI bits
   uint8 ri_len;  ///< depending on number of eNB Tx Ants (can be 1 or 2)
                  ///< 0=> RI = 1 ..  3=> RI = 4
}lte_LL1_pucch_ri_csf_report_struct;


/*! 
    @brief Type 2 report comprising of WB_CQI + PMI. Ref. 36.213 Tab. 7.2.2-3
*/
typedef struct
{
   uint8 wb_cqi_val;           ///< WB CQI payload
   uint8 wb_spat_diff_cqi_val; ///< Differential CQI for CW2 depending if RI > 1
   uint8 pmi_val;              ///< PMI selected for WB report
   uint8 num_cw;               ///< 0=>1 CW and 1=> 2 CW
}lte_LL1_pucch_wb_cqi_pmi_csf_report_struct;


/*! 
    @brief Type 1 report comprising of SB_CQI + SB label. Ref. 36.213 Tab. 7.2.2-3
*/
typedef struct
{
   uint8 sb_cqi_val;            ///< SB CQI payload
   uint8 sb_spat_diff_cqi_val;  ///< SB diff CQI of CW2 w.r.t CW 1 
   uint8 sb_label;              ///< label indicating best subband within BWP
   uint8 sb_label_len;          ///< 0=>1 bit and 1=> 2 bit SB label
   uint8 num_cw;                ///< conditioned on WB RI and PMI
}lte_LL1_pucch_sb_cqi_label_csf_report_struct;

/*! 
    @brief Type 4 report comprising of WB_CQI only Ref. 36.213 Tab. 7.2.2-3
*/
typedef struct
{
   uint8 wb_cqi;    ///< WB CQI 
}lte_LL1_pucch_wb_cqi_csf_report_struct;


/*! 
    @brief Union for all the payload types reported on PUCCH
*/
typedef union
{
   lte_LL1_pucch_sb_cqi_label_csf_report_struct pucch_sb_csf_report;        ///<.
   lte_LL1_pucch_wb_cqi_pmi_csf_report_struct pucch_wb_cqi_pmi_csf_report;  ///<.
   lte_LL1_pucch_ri_csf_report_struct pucch_ri_csf_report;                  ///<.
   lte_LL1_pucch_wb_cqi_csf_report_struct pucch_wb_cqi_csf_report;          ///<.
}lte_LL1_pucch_csf_report_payload_union;
#endif

/*! 
    @brief Place holder for CSF payload on PUSCH
*/
typedef struct
{
   uint32 temp;     ///< place holder until PUSCH reporting is finalized
}lte_LL1_pusch_csf_report_payload;


typedef struct
{
   uint32 test_param;
}unused_srs_struct;

/*! @brief Static parameters for SRS Type 1 */
typedef struct 
{
   boolean                           dci0_enable;               ///< Enable Type1 SRS on DCI 0
   lte_LL1_ul_srs_stat_params_struct srs_stat_params_dci0;      ///< Type1 SRS params for DCI0 trigger
   boolean                           dci1a2b2c_enable;          ///< Enable Type1 SRS on DCI 1a/2b/2c
   lte_LL1_ul_srs_stat_params_struct srs_stat_params_dci1a2b2c; ///< Type1 SRS params for DCI 1a/2b/2c trigger
} lte_LL1_ul_srs_type1_stat_params_struct;

/*! @brief Union of static parameters for all channels */
typedef union
{
   lte_LL1_ul_pusch_pucch_stat_params_struct  pusch_pucch_stat_params;  ///<  static parameters for PUSCH & PUCCH channels
   lte_LL1_ul_srs_stat_params_struct          srs_stat_fdd_tdd_params;  ///< SRS static params for FDD and TDD 
                                                                        ///< (multiplexed with PUSCH/ PUCCH, SRS only)

   lte_LL1_ul_srs_type1_stat_params_struct    srs_type1_stat_params;    ///< SRS Type1 static params for FDD and TDD 
//   lte_LL1_ul_uppts_stat_params_struct        uppts_stat_params;        ///<  static parameters for UpPTS

   // add more channels here 
}lte_LL1_ul_stat_params_union;


/*! 
    @brief UL static parameters that are subject to change only due to L3 signalling. 
    Depending upon the channel type (PRACH, PUSCH, etc), the corresponding element in 
    the union is valid.
*/
#define TX_EFS_PA_TOGGING_DISABLE_BIT 0x1

typedef struct 
{
   lte_LL1_req_header_struct   req_hdr;          ///< request header
   lte_LL1_ul_chan_config_enum   chan_type;       ///< indicates type of channel to be configured. 
                                                  ///< PUSCH and PUCCH are configured together
   lte_LL1_ul_stat_params_union stat_params_un;   ///< union of static parameters for different UL channels.
                                                  ///< same structure used for PUSCH & PUCCH.
   uint32                       tx_efs_config;    /// bit0: pa_toggling_disable
} lte_LL1_ul_phychan_config_req_struct;

LTE_LL1_INTF_MSG( ul_phychan_config_req );


/*! 
    @brief UL Command for de-configuring a channel (i.e. disabling the static dB parameters)
*/
typedef struct 
{
      lte_LL1_req_header_struct   req_hdr;          ///< request header
   lte_LL1_ul_chan_config_enum   chan_type;      ///< indicates type of channel to be deconfigured. 
                                                 ///< PUSCH and PUCCH are deconfigured together
} lte_LL1_ul_phychan_deconfig_req_struct;

LTE_LL1_INTF_MSG( ul_phychan_deconfig_req );


/*! @brief Union of dynamic parameters for all channels */
typedef union 
{
   lte_LL1_ul_pusch_dyn_params_struct   pusch_dyn_params; ///<.
   lte_LL1_ul_pucch_dyn_params_struct   pucch_dyn_params; ///<.
   lte_LL1_ul_prach_dyn_params_struct   prach_dyn_params; ///<.
   unused_srs_struct                    unused_srs_params; ///< required since PLT client determines chan
                                                           // type to populate the structure
   lte_LL1_ul_uppts_dyn_params_struct   uppts_dyn_params; ///<.
}lte_LL1_ul_dyn_params_union;


/* @brief Response to a configuration request */
typedef struct
{
   lte_LL1_cnf_header_struct cnf_hdr;         ///<  confirmation header
   lte_LL1_ul_chan_config_enum   chan_type;   ///<  indicates type of channel to be configured. 
                                              ///<  PUSCH and PUCCH are configured together
} lte_LL1_ul_phychan_config_cnf_struct;

LTE_LL1_INTF_MSG( ul_phychan_config_cnf );


/* @brief Response to a de-configuration request */
typedef struct
{
   lte_LL1_cnf_header_struct      cnf_hdr;      ///<  confirmation header
   lte_LL1_ul_chan_config_enum    chan_type;    ///<  indicates type of channel to be configured.
                                                ///<  PUSCH and PUCCH are configured together
} lte_LL1_ul_phychan_deconfig_cnf_struct;

LTE_LL1_INTF_MSG( ul_phychan_deconfig_cnf );


/* @brief Response to an IFFT scaling change request */
typedef struct
{
   lte_LL1_cnf_header_struct cnf_hdr;                              ///<  confirmation header
   lte_LL1_ul_ifft_scaling_change_enum ifft_scaling_change_flag;   ///<.
} lte_LL1_ul_ifft_scaling_change_cnf_struct;

LTE_LL1_INTF_MSG( ul_ifft_scaling_change_cnf );


/* @brief Response to a TX window length change request */
typedef struct
{
   lte_LL1_cnf_header_struct cnf_hdr;                               ///<  confirmation header
   lte_LL1_ul_tx_window_length_change_enum tx_window_change_flag;   ///<.
} lte_LL1_ul_tx_window_length_change_cnf_struct;

LTE_LL1_INTF_MSG( ul_tx_window_length_change_cnf );

/*! @brief structure for hold cxm related parameters */
typedef struct
{
  /* Flag to indicate whether RRC procedure is on going 
   * TRUE - indicates RRC procedure is ongoing for this subframe
   * FALSE - indicates RRC procedure is not ongoing
   **/
  boolean rrc_procedure_ongoing;
}lte_LL1_ul_cxm_params_s;

/*! @brief UL dynamic parameters that are subject to change every sub-frame. Depending
    upon the channel type (PRACH, PUSCH, etc), the corresponding element in the union
    is valid.
*/
typedef struct 
{
   lte_LL1_req_header_struct          req_hdr;          ///< request header
   lte_LL1_ul_chan_type_enum          chan_type;         ///< PRACH / PUSCH / PUCCH / SRS only / UpPTS
   lte_LL1_srs_dyn_params_struct      srs_dyn_params_fdd_tdd0;///< SRS for either fdd or tdd0 
   lte_LL1_srs_dyn_params_struct      srs_dyn_params_tdd1;  ///< SRS for tdd1 
   lte_LL1_ul_dyn_params_union        ul_dyn_params_un;  ///<.
   lte_LL1_ul_txpll_script_array_t    ul_txpll_array;
   /* These apply only for the duration of the subframe */ 
   lte_LL1_ul_cxm_params_s            cxm_dyn_params;
   boolean                            capture_dtr_out;
} lte_LL1_ul_phychan_schedule_req_struct;

LTE_LL1_INTF_MSG( ul_phychan_schedule_req );


/*! @brief Scaling coefficients for the IFFT */
typedef struct
{
   lte_LL1_req_header_struct   req_hdr;          ///< request header
   uint8 scaling_fac[5];  ///<.
} lte_LL1_ul_ifft_scaling_change_req_struct;

LTE_LL1_INTF_MSG( ul_ifft_scaling_change_req );

/* @brief Req msg for Tx IQ sample capture during Calibration mode for APT/EPT/ET*/

typedef struct
{
lte_LL1_req_header_struct req_hdr;      ///< request header
fw_rf_capture_param_t capture_param; ///< per-capture parameters
}lte_LL1_ul_iq_samp_capture_req_struct;


LTE_LL1_INTF_MSG( ul_iq_samp_capture_req );

/* @brief Response to UL IQ sample capture Req */
typedef struct
{
   lte_LL1_cnf_header_struct cnf_hdr;                              ///<  confirmation header
   lte_LL1_ul_iq_samp_capture_enum samp_capture_done;           // Indicates if sample capture was done or not
} lte_LL1_ul_iq_samp_capture_cnf_struct;

LTE_LL1_INTF_MSG( ul_iq_samp_capture_cnf );


/*! @brief  */
typedef struct
{
   lte_LL1_req_header_struct   req_hdr;          ///< request header
  uint32 window_length; ///<.
} lte_LL1_ul_tx_window_length_change_req_struct;

LTE_LL1_INTF_MSG( ul_tx_window_length_change_req );

/*! @brief  */
typedef struct
{
   lte_LL1_req_header_struct   req_hdr;          ///< request header
   uint32 simultaneous_ul_dl_testmode; ///<.
}  lte_LL1_ul_sim_ul_dl_test_req_struct;

LTE_LL1_INTF_MSG( ul_sim_ul_dl_test_req );


/*! @brief  */
typedef struct
{
   uint8 length:2;  ///<.
   uint8 payload:2; ///<.
}ack_ri_payload_struct;

/*! @brief  */
typedef struct
{
   uint8 length;     ///< in bits of CQI + PMI payload
   uint64 payload;  ///< For P1 MVT, the max CQI payload size < 64 bits
}cqi_pmi_payload_struct;

/*! @brief  */
typedef struct 
{
   uint8 length;    ///<.
   uint16 payload;  ///<.
}pucch_cqi_pmi_ri_payload_struct;


/*! 
    @brief Structure containing PUSCH control info parameters for PLT/MVT
*/
typedef struct
{
   ack_ri_payload_struct ack_payload;       ///< ACK/NAK (1-2 bits)
   ack_ri_payload_struct rank_ind_payload;  ///< rank indicator (1-2 bits)
   cqi_pmi_payload_struct cqi_pmi_payload;  ///< worst case CQI+PMI payload pn PUSCH is ~ 107 bytes
}pusch_cntl_info_test_struct;


/*! 
    @brief Structure containing PUCCH control info parameters for PLT/MVT
*/
typedef struct
{
   ack_ri_payload_struct ack_payload;                   ///<.
   pucch_cqi_pmi_ri_payload_struct cqi_pmi_ri_payload;  ///<.
}pucch_cntl_info_test_struct;

/*! @brief  */
typedef union
{
   pusch_cntl_info_test_struct pusch_cntl_info_test_params; ///<.
   pucch_cntl_info_test_struct pucch_cntl_info_test_params; ///<.
}lte_LL1_ul_cntl_info_test_union;

/*! @brief  */
typedef struct
{
   lte_LL1_req_header_struct   req_hdr;          ///< request header
   lte_LL1_ul_test_ctl_info_chan_type_enum  chan_type; ///< PUCCH / PUSCH
   lte_LL1_ul_cntl_info_test_union ctl_info_test_un; ///< control parameters
} lte_LL1_ul_cntl_info_test_req_struct;

LTE_LL1_INTF_MSG( ul_cntl_info_test_req );


typedef struct
{
   uint8 tone_shift;
   uint8 tone_density;
   lte_LL1_csf_test_mode_enum test_mode;
}csf_ctrl_params_struct;

typedef struct 
{
   lte_LL1_req_header_struct    req_hdr;              ///<  request header
   csf_ctrl_params_struct       csf_ctrl_params;
}lte_LL1_ul_csf_ctrl_info_test_req_struct;

LTE_LL1_INTF_MSG(ul_csf_ctrl_info_test_req);


/*! @brief Enum to indicate type of UL control info in ul_ctrl_info_buf_test_req test LL-ML API */
typedef enum
{
  LTE_LL1_UL_ACK_INFO = 0,
  LTE_LL1_UL_CSF_INFO
} lte_LL1_ul_ctrl_info_type_enum;

/*! @brief Enum to indicate type of UL chan type for sending CSF info */
typedef enum
{
  LTE_LL1_UL_CSF_ON_PUSCH = 0,
  LTE_LL1_UL_CSF_ON_PUCCH
} lte_LL1_ul_csf_chan_type_enum;

/*! 
  @brief
  ACK / NAK info struct containing info corresponding to a specific DL subframe.
  Used for testing only
*/
typedef struct
{
  lte_LL1_sys_time_struct   dl_time_info;   ///<  DL frame num, subframe num
  uint8                     ack_nak_bits;   ///<  ACK / NAK bits for the corresponding DL subframe
  uint8                     ack_bit_mask;   ///<  ACK bit mask indicating valid PDSCH codewords
                                            ///<  {01 =>TB0. 10=>TB1, 11=>TB0 & TB1, 00=>Indicates PDCCH decode failure}
} ack_buf_info_struct;

/*! 
  @brief
  CSF info struct containing CQI/PMI/RI info corresponding to a specific DL subframe.
  Used for testing only
*/
typedef struct
{
  lte_LL1_ul_csf_chan_type_enum   csf_chan_type;  ///< UL chan type on which CSF info is sent.
                                                  ///< This can be used to debug ML scheduling
  uint8   dl_subframe_num;   ///< 0~9
  uint8   cqi_len;      ///< Number of channel quality information bits (CQI + PMI)
  uint32  cqi_bits_w0;  ///< First 32 bits of CQI+PMI
  uint32  cqi_bits_w1;  ///< Second 32 bits of CQI+PMI
  uint32  cqi_bits_w2;  ///< Thrid 32 bits of CQI+PMI
  uint32  cqi_bits_w3;  ///< Fourth 32 bits of CQI+PMI
  uint8   ri_len;       ///< Indicates whether RI is present or not. RI length is 1 / 2 bits.
  uint8   ri_val;       ///< RI bits
} csf_buf_info_struct;

/*! @brief Union of ACK and CSF info to send ACK / CSF information asynchronously */
typedef union
{
  ack_buf_info_struct   ack_buf_info; ///< ACK info in ul_ctrl_info_buf_test_req API
  csf_buf_info_struct   csf_buf_info; ///< CSF info in ul_ctrl_info_buf_test_req API
} lte_LL1_ul_ctrl_params_union;

/*! 
  @brief
  Test message used to populate PDSCH control info buffers (ACK/NAK, CQI/PMI/RI in txhwctl module.

  @detail  
   - ACK/NAK bits are available from DL in regular mode of operation
   - CQI/PMI/RI bits are available from CSF module
   Union so that ACK and CSF info can be sent asynchronously
*/
typedef struct
{
  lte_LL1_req_header_struct       req_hdr;              ///<  request header
  lte_LL1_ul_ctrl_info_type_enum  ctrl_info_type;       ///<  Enum indicating ACK (or) CSF information   
  lte_LL1_ul_ctrl_params_union    ul_ctrl_params_un;    ///<  Union of ACK / CSF information
  lte_LL1_carrier_type_enum       carrier_type;         ///< carrier_index, 0=PCC, 1=SCC
} lte_LL1_ul_ctrl_info_buf_test_req_struct;

LTE_LL1_INTF_MSG( ul_ctrl_info_buf_test_req );

/*! 
  @brief
  Calibration request and abort message to specify number of subframes of continuous transmission with 
  one PHYCHAN SCHEDULE request message.

  @note
  This message is used only for calibration, not used in connected mode.

  @detail
  This message should be used only during calibration. During calibration, RF SW sends one PHYCHAN 
  SCHEDULE message request preceded by this message specifying the number of subframes to transmit.
  On receipt on the PHYCHAN SCHEDULE message, FW shall issue tasks to continuously transmit the same 
  waveform continuously on successive SFs.

  This message can also be used to abort transmissions. To abort, issue message with cal_num_sf_to_tx=0
*/
typedef struct
{
  lte_LL1_req_header_struct       req_hdr;              ///<  request header
  uint32                          cal_sf_to_transmit;   ///<  Number of SFs to continuously transmit with one request message
} lte_LL1_ul_cal_cont_tx_req_struct;

LTE_LL1_INTF_MSG( ul_cal_cont_tx_req );

/*! @brief  
  HDET HKADC conversion ready IND message to send to RFSW

  @note
  This message is used during online mode for HDET loop in TXAGC

  @detail
  FW maintains a timer loop (period depends on the ACK/TRACK mode i.e the frequency is higher initially but reduced by a
  magnitude once we have some collect a number of readings) & when the timer expires the HDET start is initiated by a
  SSBI write. Another SSBI write after some settling time delay starts the HKADC conversion. The message is sent to RFSW
  to signal that the conversion is complete & a new PDET reading is available. RFSW will update the MTPL value which
  affects the TXAGC output. Ideally, the MSG should be sent right around time when conversion is expected to complete
  but to avoid additional interrupt code & latency overhead, the MSG is sent 1 SF (i.e 1ms hence)

*/
typedef struct {
  int32 txagc_cnt;  ///< requested TXAGC transmitted power  in TXAGC count units
} lte_LL1_ul_hdet_ind_struct;

LTE_LL1_INTF_MSG( ul_hdet_ind );


/*! @brief  
  HDET HKADC conversion ready & read by FW IND message to send to RFSW

  @note
  This message is used during online mode for HDET loop in TXAGC

  @detail
  FW maintains a timer loop (period depends on the ACK/TRACK mode i.e the frequency is higher initially but reduced by a 
  magnitude once we have some collect a number of readings) & when the timer expires the HDET start is initiated by a 
  SSBI write. Another SSBI write after some settling time delay starts the HKADC conversion. The message is sent to RFSW
  to signal that the conversion is complete & a new PDET reading is available. RFSW will update the MTPL value which 
  affects the TXAGC output. Ideally, the MSG should be sent right around time when conversion is expected to complete
  but to avoid additional interrupt code & latency overhead, the MSG is sent 1 SF (i.e 1ms hence)
  
*/
#define NUM_HDET_READ_TRANS 12
typedef struct {
  uint16 addr;
  uint8  value;
}  lte_LL1_ul_hdet_hw_rd_data_type_struct;

typedef struct {
  lte_LL1_ul_hdet_hw_rd_data_type_struct hdet_rd_data[NUM_HDET_READ_TRANS];
  uint8 num_trans;
}  lte_LL1_ul_hdet_result_struct;


typedef struct {                      
  int32 txagc_cnt;  ///< requested TXAGC transmitted power  in TXAGC count units
  uint16 hdet_adc;   ///< PDET reading read from the HKADC for the UL SF for 
                     /// which HDET measurement was scheduled
  uint8 hdet_rd_clobbered; ///< Indicates that above read is invalid 
                           /// (clobbered by another READ)
  uint8 hdet_rd_ready;     /// < Indicates HDET ADC value ready to be read in HW
  uint8 start_rb;  ///< Start RB of slot 0 for UL SF HDET measurement is done on
  uint8 num_rbs;  ///< Number of RBs of slot 0 for UL SF HDET measurement is done on

  lte_LL1_ul_hdet_result_struct hdet_result_data;
} lte_LL1_ul_hdet_fw_rd_ind_struct;

LTE_LL1_INTF_MSG( ul_hdet_fw_rd_ind );

/*! @brief
  THERM HKADC conversion ready & read by FW IND message to send to RFSW

  @note
  This message is used during online mode for therm loop in TXAGC

  @detail
  FW maintains a timer loop (period is defined statically in FW) & when the timer expires the therm start is initiated by a
  SSBI write. Another SSBI write after some settling time delay starts the HKADC conversion. The message is sent to RFSW
  to signal that the conversion is complete & a new reading is available.
  
*/
typedef struct {
  uint16 addr[7];
  uint8  value[7];
}  lte_LL1_ul_therm_hw_rd_data_type_struct;

typedef struct {                      
  uint8 subframe_num;  ///< UL subframe number corresponding to THERM reading
  uint16 therm_adc;   ///< PDET reading read from the HKADC for the UL SF for
                     /// which therm measurement was scheduled
  uint8 therm_rd_clobbered; ///< Indicates that above read is invalid
                           /// (clobbered by another READ)
  uint8 therm_rd_ready;     /// < Indicates therm ADC value ready to be read in HW
  uint8 num_trans;
  lte_LL1_ul_therm_hw_rd_data_type_struct therm_result_data;
} lte_LL1_ul_therm_fw_rd_ind_struct;

LTE_LL1_INTF_MSG( ul_therm_fw_rd_ind );

/*! @brief
  K-sensor values for QFE1510 ready & read by FW IND message to send to RFSW

  @note
  This message is used during online mode for TDET Comp loop in TXAGC

  @detail
  FW maintains a timer loop (period is defined statically in FW) & when the timer expires the TDET start is initiated by a
  RFFE read/write. The message is sent to RFSW to signal that the conversion is complete & a new reading is available.
  
*/
typedef struct {
  uint16 addr[7];
  uint8  value[7];
}  lte_LL1_ul_tdet_temp_comp_hw_rd_data_type_struct;

typedef struct {                      
  uint8 subframe_num;  ///< UL subframe number corresponding to TDET reading
  uint16 tdet_adc[2];   ///< TDET reading read from the HKADC for the UL SF for
                     /// which therm measurement was scheduled
  uint8 tdet_rd_clobbered[2]; ///< Indicates that above read is invalid
                           /// (clobbered by another READ)
  uint8 tdet_rd_ready[2];     /// < Indicates therm ADC value ready to be read in HW
  uint8 num_trans;
  lte_LL1_ul_tdet_temp_comp_hw_rd_data_type_struct tdet_result_data;
} lte_LL1_ul_tdet_temp_comp_fw_rd_ind_struct;

LTE_LL1_INTF_MSG( ul_tdet_temp_comp_fw_rd_ind );


/*! @brief
  Send indication back to ML1 on the RI value sent from UL 

  @note

  @detail
  Send indication back to ML1 on the RI value sent from UL 
  
*/
typedef struct {                      
  uint32 frame_subframe;                               // frame*10+subframe
  boolean ri_sent[LTE_LL1_CARRIER_COUNT];
  uint8 ri_val[LTE_LL1_CARRIER_COUNT];
  boolean wb_cqi_present[LTE_LL1_DL_NUM_CARRIERS][2];  //[carrier][CODEWORD]  
  uint8   wb_cqi_val[LTE_LL1_DL_NUM_CARRIERS][2];      //[carrier][CODEWORD]
} lte_LL1_ul_phychan_ind_struct;

LTE_LL1_INTF_MSG( ul_phychan_ind );




// --------------------------------------------------------------------------
//
// run TX LM config
//
// --------------------------------------------------------------------------

/*! @brief  Message structure for TX LM config request message
            
            When received, LM config (at specified index) is executed to 
            take effect immediately.
            
            Confirmation to the request is sent after completion of the 
            script.
            
            The LM config request should be issued before ul transmission.
            
*/
typedef struct {                      

  lte_LL1_req_header_struct req_hdr;
  
  uint32 tx_lm_buf_index;  ///< The index in the TX LM buffer 
                          ///< for the LTE FW to execute
                           
} lte_LL1_ul_tx_lm_config_req_struct;

LTE_LL1_INTF_MSG( ul_tx_lm_config_req );

/*! @brief TX LM config confirmation structure

   This confirmation will be sent when TX link manager specified buffer
   has been completed hw register update.
*/
typedef struct {                      
  
  lte_LL1_cnf_header_struct cnf_hdr;
  
} lte_LL1_ul_tx_lm_config_cnf_struct;

LTE_LL1_INTF_MSG( ul_tx_lm_config_cnf );

// --------------------------------------------------------------------------
//
// run TX LM config for ET/EPT
//
// --------------------------------------------------------------------------

typedef struct {                      

  lte_LL1_req_header_struct req_hdr;

  uint8 enable;   ///< field will be passed to the RFFW API 
            ///< to update the active PA state mask
  uint32 tx_lm_buf_index;  ///< The index in the TX LM buffer 
                          ///< for the LTE FW to execute
                           
} lte_LL1_ul_et_resource_enable_req_struct;

LTE_LL1_INTF_MSG( ul_et_resource_enable_req );

/*! @brief TX LM config confirmation structure for ET */

typedef struct {                      
  
  lte_LL1_cnf_header_struct cnf_hdr;
  lte_LL1_ul_et_resource_enable_enum et_resource_enable_done;
  
} lte_LL1_ul_et_resource_enable_cnf_struct;

LTE_LL1_INTF_MSG( ul_et_resource_enable_cnf );


// CR 314715: new message
/*! @brief
  Message for sending UL/CSF statistics for MMAL logging

  @note
  This message is sent every reporting period (480 ms)

  @detail
  Collect UL and CSF statistics and send it for MMAL logging
  
*/

typedef struct
{
  uint16   ack;
  uint16   nack;
  uint16   dtx;
} lte_LL1_pusch_cqi_lte_dist_t;

typedef struct
{
  lte_LL1_pusch_cqi_lte_dist_t    cqi_dist[16];
} lte_LL1_pusch_cqi_band_dist_t;


typedef struct
{
  int16                            total_pusch_tx_power;
  int16                            num_RB;
  boolean                          wb_report_present;
  int8                             num_sb_report;
  lte_LL1_pusch_cqi_band_dist_t    cqi_wb_dist;
  lte_LL1_pusch_cqi_band_dist_t    cqi_sb_dist[4];
  uint16                           rank_ind_dist[5];
  uint16                           pmi_dist[16];
} lte_LL1_ul_mmal_rep_t;

typedef struct {                      
  lte_LL1_ul_mmal_rep_t mmal_report;
} lte_LL1_ul_pusch_tx_status_ind_struct;

LTE_LL1_INTF_MSG( ul_pusch_tx_status_ind );


/* @brief Response to async phychan schedule request */
typedef struct
{
   uint16 frame_num;
   uint16 subframe_num;
   boolean is_scheduled;
} lte_LL1_ul_async_phychan_request_cnf_struct;

LTE_LL1_INTF_MSG( ul_async_phychan_request_cnf );


/*===========================================================================

            Request Messages with header definition

===========================================================================*/


/*===========================================================================

            Confirmation Messages with header definition

===========================================================================*/


#endif /* INTF_UL_H */


