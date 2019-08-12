/*!
  @file
  lte_rrc_llci.h

  @brief
  Internal API for RRC LLC.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_llci.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/16/12   kp      Added code for rel-10 feature CSI-RS.
06/01/12   ap      Handling added to prevent duplicate common cfg req to ML1
09/10/10   mm      Changes for supporting Rel 9 mandatory features (except full config)
06/15/10   mm      Added last_dedicated_cfg_succeeded to keep track of the status of 
                   the last dedicated config
05/20/10   mm      Added p_max to common mobility config parameters passed to ML1
04/05/10   mm      Changed the prototype of CQI, SRS, SR, PUCCH ded, SPS functions
04/05/10   da      Added dd_ptr in priv data for usage with dymically allocated mem.
05/28/09   mm      Added UE Timers and constants as a config parameter in 
                   cfg_db, required for RLF detection/recovery.
04/27/09   da      Added PHR processing.
12/08/08   mm      Added changes for Rel 1.6, to handle handover processing
10/21/08   da      Added support for connection setup and reconfig
10/09/08   da      Added support for processing LTE_RRC_CFG_REQI for Idle state.
===========================================================================*/

#ifndef LTE_RRC_LLCI_H
#define LTE_RRC_LLCI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <__lte_rrc_llc.h>
#include "lte_assert.h"
#include "lte_rrc_osys_asn1.h"
#include "lte_cphy_msg.h"
#include "lte_mac_rrc.h"
#include "lte_rlc_rrc.h"
#include "lte_pdcp_msg.h"
#include "lte_rrc_int_msg.h"
#include "lte_rrc_utils.h"
#include "lte_rrc_llc.h"
#include "lte_as.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*! @brief Number of lower layer configuration cmd kept in history for each layer
    as part of the command database
*/
#define LTE_RRC_MAX_NUM_OF_CMD_IN_HIST 2

/*! @brief Max number of lower layer commands that can be sent as part of one 
    LLC_RRC_CFG_REQI.
*/
#define LTE_RRC_MAX_NUM_LL_CMD 10

/*! @brief Size of array used for maintainance of Transaction id for LTE_RRC_CFG_REQI
    and LTE_RRC_CFG_CNFI.
*/
#define LTE_RRC_MAX_TRANS_ID (LTE_RRC_PENDQ_MAX_QUEUE_LEN + 1)

/*! @brief Indication that Pathloss Threshhold value was not signalled.
*/
#define LTE_RRC_PATHLOSS_TRESHHOLD_NOT_PRESENT 0

/*! @brief Min value for OTA field q_Rxlevmin
*/
#define LTE_RRC_LLC_MIN_Q_RXLEVMIN -70

/*! @brief Max value for OTA field q_Rxlevmin
*/
#define LTE_RRC_LLC_MAX_Q_RXLEVMIN -22

/*! @brief Indication that q_Rxlevminoffset was not signalled.
*/
#define LTE_RRC_LLC_DEFAULT_Q_RXLEVMIN_OFFSET 0

/*! @brief Max value for PRACH freq offset.
*/
#define LTE_RRC_LLC_MAX_PRACH_FREQ_OFFSET 94

/*! @brief Max value for PRACH ROOT_SEQUENCE_INDEX
*/
#define LTE_RRC_LLC_MAX_ROOT_SEQUENCE_INDEX 837

/*! @brief Min value for PDSCH reference Signal.
*/
#define LTE_RRC_LLC_MIN_PDSCH_REG_SIG_PWR -60

/*! @brief Max value for PDSCH reference Signal.
*/
#define LTE_RRC_LLC_MAX_PDSCH_REG_SIG_PWR 50

/*! @brief Max value for PUSCH Group Assignment.
*/
#define LTE_RRC_LLC_MAX_GROUP_ASSIGNMENT_PUSCH 29

/*! @brief Min Value for p0_NominalPUSCH.
*/
#define LTE_RRC_LLC_MIN_P0_NOMINAL_PUSCH -126

/*! @brief MAX Value for p0_NominalPUSCH.
*/
#define LTE_RRC_LLC_MAX_P0_NOMINAL_PUSCH 24

/*! @brief MAX Value for PUSCH hopping offset.
*/
#define LTE_RRC_LLC_MAX_PUSCH_HOPPING_OFFSET 98

/*! @brief Max value for n_RB_CQI
*/
#define LTE_RRC_LLC_MAX_N_RB_CQI 98

/*! @brief Maximum value drx cycle offset for SF 10.
*/
#define LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF10 9

/*! @brief Maximum value drx cycle offset for SF 20.
*/
#define LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF20 19

/*! @brief Maximum value drx cycle offset for SF 32.
*/
#define LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF32 31

/*! @brief Maximum value drx cycle offset for SF 40.
*/
#define LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF40 39

/*! @brief Maximum value drx cycle offset for SF 64.
*/
#define LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF64 63

/*! @brief Maximum value drx cycle offset for SF 80.
*/
#define LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF80 79

/*! @brief Maximum value drx cycle offset for SF 128.
*/
#define LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF128 127

/*! @brief Maximum value drx cycle offset for SF 160.
*/
#define LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF160 159

/*! @brief Maximum value drx cycle offset for SF 256.
*/
#define LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF256 255

/*! @brief Maximum value drx cycle offset for SF 320.
*/
#define LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF320 319

/*! @brief Maximum value drx cycle offset for SF 512.
*/
#define LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF512 511

/*! @brief Maximum value drx cycle offset for SF 640.
*/
#define LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF640 639

/*! @brief Maximum value drx cycle offset for SF 1024.
*/
#define LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF1024 1023

/*! @brief Maximum value drx cycle offset for SF 1280.
*/
#define LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF1280 1279

/*! @brief Maximum value drx cycle offset for SF 2048.
*/
#define LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF2048 2047

/*! @brief Maximum value drx cycle offset for SF 2560.
*/
#define LTE_RRC_LLC_MAX_LONG_DRX_START_OFFSET_SF2560 2559

/*! @brief Maximum value for CQI PUCCH resource index.
*/
#define LTE_RRC_LLC_MAX_CQI_PUCCH_RESOURCE_INDEX 1184

/*! @brief Maximum value for Subband CQI K.
*/
#define LTE_RRC_LLC_MAX_CQI_SUBBAND_K 4

/*! @brief Maximum value for frequency domain Position - SRS cfg dedicated.
*/
#define LTE_RRC_LLC_MAX_FREQ_DOMAIN_POS 23

#define LTE_RRC_LLC_MAX_TX_COMBO_SRS_AP_POS 1 

/*! @brief Maximum value for SR Configuration Index 
*/
#define LTE_RRC_LLC_MAX_SR_CONFIG_INDEX 155

/*! @brief Maximum value for SR Configuration Index 
*/
#define LTE_RRC_LLC_MAX_SR_CONFIG_INDEX_REL9 157

/*! @brief Minimum value for RNTI (CRNTI, SPS, and TPC)
*/
#define LTE_RRC_LLC_MIN_RNTI_VAL 0x1

/*! @brief Maximum value for RNTI (CRNTI, SPS, and TPC)
*/
#define LTE_RRC_LLC_MAX_RNTI_VAL 0xFFF3

/*! @brief Size of OSS bitsring MSFN one subframe 
*/
#define LTE_RRC_LLC_OSS_BITSTRING_SIZE_MBSFN_ONE_FRAME 6

/*! @brief Size of OSS bitsring MSFN four subframe 
*/
#define LTE_RRC_LLC_OSS_BITSTRING_SIZE_MBSFN_FOUR_FRAME 24

/*! @brief Default value for logical channel priority for SRB1 
*/
#define LTE_RRC_LLC_DEFAULT_SRB1_PRIORITY 1

/*! @brief Default value for logical channel priority for SRB2
*/
#define LTE_RRC_LLC_DEFAULT_SRB2_PRIORITY 3

/*! @brief Default value for logical channel group for SRB
*/
#define LTE_RRC_LLC_DEFAULT_SRB_LOGICAL_CHAN_GROUP 0

/*! @brief RB_CFG_IDX of SRB1
*/
#define LTE_RRC_LLC_SRB1_RB_CFG_IDX LTE_MAX_RB + LTE_RRC_DCCH_SRB1

/*! @brief RB_CFG_IDX of SRB2
*/
#define LTE_RRC_LLC_SRB2_RB_CFG_IDX LTE_MAX_RB + LTE_RRC_DCCH_SRB2

/*! @brief Default value for t_reordering 
*/
#define LTE_RRC_LLC_DEFAULT_T_REORDERING 35

/*! @brief Default value for t_status_prohibit 
*/
#define LTE_RRC_LLC_DEFAULT_T_STATUS_PROHIBIT 0

/*! @brief Default value for t_poll_retransmit 
*/
#define LTE_RRC_LLC_DEFAULT_T_POLL_RETRANSMIT 45

/*! @brief Default value for MaxRetxThreshold 
*/
#define LTE_RRC_LLC_DEFAULT_MAX_RETX_THRESHOLD 4

/*! @brief Indication that Discart timer value was not signalled.
*/
#define LTE_RRC_LLC_DISCARD_TIMER_NOT_PRESENT LTE_OTA_IE_INFINITY_UINT16

/*! @brief Default value retransmission BSR timer value
*/
#define LTE_RRC_LLC_DEFAULT_RETX_BSR_TIMER_VAL 2560

/*! @brief Default PUSCH delta offset ack index
*/
#define LTE_RRC_LLC_DEFAULT_PUSCH_DELTA_OFFSET_ACK_INDEX 10

/*! @brief Default PUSCH delta offset ri index
*/
#define LTE_RRC_LLC_DEFAULT_PUSCH_DELTA_OFFSET_RI_INDEX 12

/*! @brief Default PUSCH delta offset cqi index
*/
#define LTE_RRC_LLC_DEFAULT_PUSCH_DELTA_OFFSET_CQI_INDEX 15

/*! @brief Default p0_ue_pusch and p0_ue_pucch values
*/
#define LTE_RRC_LLC_DEFAULT_UL_PWR_CTRL_P0_UE 0

/*! @brief Default p_srs_offset value
*/
#define LTE_RRC_LLC_DEFAULT_UL_PWR_CTRL_P_SRS_OFFSET 7

/*! @brief Minimum value for P_VAL
*/
#define LTE_RRC_LLC_MIN_P_VAL -30

/*! @brief Maximum value for P_VAL
*/
#define LTE_RRC_LLC_MAX_P_VAL 33

/*! @brief Maximum value for TPC index format 3 and format 3A
*/
#define LTE_RRC_LLC_MIN_TPC_INDEX 1

/*! @brief Maximum value for TPC index format 3
*/
#define LTE_RRC_LLC_MAX_TPC_INDEX_FORMAT3 15

/*! @brief Maximum value for TPC index format 3A
*/
#define LTE_RRC_LLC_MAX_TPC_INDEX_FORMAT3A 31

/*! @brief Max value for srs_cfg_index for FDD
  REVISIT: After every spec revision of RRC / ML1 spec
  Value range is 0-1023 as per 36.331 v8.5.0
  Values 637-1023 are reserved as per Table 8.2-1 in 36.213 v9.3.0/v10.7.0
*/
#define LTE_RRC_LLC_FDD_MAX_SRS_CFG_INDEX 636

/*! @brief Max value for srs_cfg_index for TDD
  REVISIT: After every spec revision of RRC / ML1 spec
  Value range is 0-1023 as per 36.331 v8.5.0
  Values 645-1023 are reserved as per Table 8.2-1 in 36.213 v9.3.0/v10.7.0
*/
#define LTE_RRC_LLC_TDD_MAX_SRS_CFG_INDEX 644

/*! @brief Max value for cqi_pmi_cfg_index
  REVISIT: After every spec revision of RRC / ML1 spec
  Value range is 0-1023 as per 36.331 v8.5.0
  Values 317, 542-1023 are reserved as per Table 7.2.2-1A in 36.213 v8.7.0
*/
#define LTE_RRC_LLC_MAX_CQI_PMI_CFG_INDEX 541

/*! @brief Max value for cqi_pmi_cfg_index_tdd*/
#define LTE_RRC_LLC_MAX_CQI_PMI_CFG_INDEX_TDD 315

/*! @brief Max value for cqi_pmi_cfg_index
  REVISIT: After every spec revision of RRC / ML1 spec
  Value range is 0-1023 as per 36.331 v8.5.0
  Values 317, 542-1023 are reserved as per Table 7.2.2-1A in 36.213 v8.7.0
*/
#define LTE_RRC_LLC_RESERVED_CQI_PMI_CFG_INDEX 317

/*! @brief Max value for ri_pmi_cfg_index
  REVISIT: After every spec revision of RRC / ML1 spec
  Value range is 0-1023 as per 36.331 v8.5.0
  Values 966-1023 are reserved as per Table 7.2.2-1B in 36.213 v8.7.0
*/
#define LTE_RRC_LLC_MAX_RI_CFG_INDEX 965

/*! @brief Minimum value for MAX CID.
*/
#define LTE_RRC_LLC_MIN_MAX_CID 1

/*! @brief Maximum value for MAX CID.
*/
#define LTE_RRC_LLC_MAX_MAX_CID 16383

/*! @brief Default value for MAX CID.
*/
#define LTE_RRC_LLC_DEFAULT_MAX_CID 15

/*! @brief Default value for MAX CID.
*/
#define LTE_RRC_LLC_KB_TO_BYTE_CONSTANT 1000

/*! @brief Max value for TDD srs_ConfigIndexAp_r10
*/
#define LTE_RRC_LLC_MAX_TDD_AP_SRS 24

/*! @brief TDD srs_ConfigIndexAp_r10 reserved value 0
*/
#define LTE_RRC_LLC_TDD_AP_SRS_0 0  

/*! @brief Max value for FDD srs_ConfigIndexAp_r10
*/
#define LTE_RRC_LLC_MAX_FDD_AP_SRS 16

/*! @brief Default value for SRB PDCP seq len in bits .
*/
#define LTE_RRC_LLC_DEFAULT_SRB_SEQ_LEN 5
#define LTE_RRC_LLC_SHORT_DRB_SEQ_LEN 7   
#define LTE_RRC_LLC_LONG_DRB_SEQ_LEN 12   

/*! @brief Default value for physical cell identity.
*/
#define LTE_RRC_LLC_MAX_VAL_PHY_CELL_ID 503

/*! @brief Maximum value for MAX MCCH offset.
*/
#define LTE_RRC_LLC_MAX_MCCH_OFFSET 10

/*! @brief Minimum value for subframe allocation for eMBMS.
*/
#define LTE_RRC_LLC_MIN_MBSFN_SUBF_ALLOCATION 1

/*! @brief Maximum value for subframe allocation for eMBMS.
*/
#define LTE_RRC_LLC_MAX_MBSFN_SUBF_ALLOCATION 8

/*! @brief Minimum number of PMCHs in an MBSFN area.
*/
#define LTE_RRC_LLC_MIN_PMCH_PER_MBSFN_AREA 0

/*! @brief Maximum number of PMCHs in an MBSFN area.
*/
#define LTE_RRC_LLC_MAX_PMCH_PER_MBSFN_AREA 15

/*! @brief Maximum value for Max subframe allocation for eMBMS.
*/
#define LTE_RRC_LLC_MAX_MBSFN_SUBF_ALLOCATION 8

/*! @brief Maximum value for Max data MCS.
*/
#define LTE_RRC_LLC_MAX_DATA_MCS 28

/*! @brief Maximum value for Max PMCH SF Alloc End.
*/
#define LTE_RRC_LLC_MAX_PMCH_SF_ALLOC_END 1535

/*! @brief Maximum value for MAX notification offset.
*/
#define LTE_RRC_LLC_MAX_MBMS_NOTIFI_OFFSET 10

/*! @brief Minimum value for notification SF index.
*/
#define LTE_RRC_LLC_MIN_MBMS_NOTIFI_SF_IDX 1

/*! @brief Maximum value for notification SF index.
*/
#define LTE_RRC_LLC_MAX_MBMS_NOTIFI_SF_IDX 6

/*! @brief Shortest MCCH Modification period value 
*/
#define LTE_RRC_LLC_SHORTEST_MCCH_MOD_PERIOD 512

/*! @brief Value MRB RLC SN length
*/
#define LTE_RRC_LLC_MRB_RLC_SN_LENGTH 5

/*! @brief Value MRB RLC T_reordering
*/
#define LTE_RRC_LLC_MRB_RLC_T_REORDERING 0

/*! @brief Maximum allowed value for PDCP seq num is only 4095 
  though ASN1 allows a range of 0 to 32767
*/
#define LTE_RRC_LLC_MAX_PDCP_SEQ_NUM 4095

/*! @brief Check sum source bytes size in bits
*/
#define LTE_RRC_LLC_PDCP_CHK_SUM_SRC_BYTES_SIZE 40

/*! @brief Value of Deadlock Timer in ms
*/

#define LTE_RRC_LLC_DEADLOCK_TMRI_VAL 1000

/*! @brief Value of Deadlock Timer in ms when in STLE mode
*/

#define LTE_RRC_LLC_DEADLOCK_TMRI_VAL_IN_SLTE_MODE 5000

/*! @brief MIN Value of Deadlock Timer in ms
*/

#define LTE_RRC_LLC_DEADLOCK_TMRI_MIN_VAL 100


/*! @brief List of Invalid FDD PRACH Config index.
*/
#define LTE_RRC_LLC_INVALID_FDD_PRACH_CONFIG_INDEX_30 30

#define LTE_RRC_LLC_INVALID_FDD_PRACH_CONFIG_INDEX_46 46

#define LTE_RRC_LLC_INVALID_FDD_PRACH_CONFIG_INDEX_60 60

#define LTE_RRC_LLC_INVALID_FDD_PRACH_CONFIG_INDEX_61 61

#define LTE_RRC_LLC_INVALID_FDD_PRACH_CONFIG_INDEX_62 62


/*! @brief Minimum PRACH config index for preamble format 4
*/
#define LTE_RRC_LLC_TDD_MIN_PRACH_CONFIG_INDEX_FMT_4 48

/*! @brief Maximum value of Invalid TDD PRACH Config index.
*/
#define LTE_RRC_LLC_MAX_TDD_PRACH_CONFIG_INDEX 57

/*! @brief bit values used to identify RB direction
  bit0 ...................1 RB has a DL pipe 
  bit1 ...................1 RB has a UL pipe
*/

#define  LTE_RRC_LLC_RB_NOT_ESTABLISHED  0x00   
#define  LTE_RRC_LLC_RB_DL_DIRECTION     0x01   
#define  LTE_RRC_LLC_RB_UL_DIRECTION     0x02
#define  LTE_RRC_LLC_RB_BI_DIRECTION     0x03
#define  LTE_RRC_LLC_UL_CP_LENGTH_NONE   0xFF

#ifdef FEATURE_LTE_ELS_ENABLED
/*! @brief UDC Cfg HDR length conversion factor
*/
#define LTE_RRC_LLC_UDC_CFG_HDR_LEN_CONV 20
#endif
/*! @brief Value to  indicate TM9 CodeBookRestriction Pattern number of bits
*/
#define LTE_RRC_LLC_TM9_CBR_NUM_BITS_6 6

#define LTE_RRC_LLC_TM9_CBR_NUM_BITS_64 64

#define LTE_RRC_LLC_TM9_CBR_NUM_BITS_109 109

/*! @brief Value to  indicate TM9 CodeBookRestriction Pattern number of bits = 45 for MSB LW
*/
#define LTE_RRC_LLC_TM9_CBR_MSB_LW_NUM_BITS 45

/*Unused mbsfn subframes in tdd cell
*/
#define LTE_RRC_LLC_TDD_MBSFN_ONE_FRAME_ALLOCATION_UNUSED_MASK 0x01

#define LTE_RRC_LLC_TDD_MBSFN_FOUR_FRAMES_ALLOCATION_UNUSED_MASK 0x00000F

/*! @brief Used to figure out if PDCPDL cfg is needed 
*/
#define LTE_RRC_LLC_IS_PDCPDL_CFG_NEEDED(cmd_list, cmd_idx) \
((cmd_list[cmd_idx].ll_cmd_u.pdcpdl_cfg_req.num_released_rb != 0) || \
 (cmd_list[cmd_idx].ll_cmd_u.pdcpdl_cfg_req.num_add_modified_rb != 0))

/*! @brief Used to figure out if PDCPUL cfg is needed 
*/
#define LTE_RRC_LLC_IS_PDCPUL_CFG_NEEDED(cmd_list, cmd_idx) \
((cmd_list[cmd_idx].ll_cmd_u.pdcpul_cfg_req.num_released_rb != 0) || \
 (cmd_list[cmd_idx].ll_cmd_u.pdcpul_cfg_req.num_add_modified_rb != 0))

/*! @brief Used to figure out if PDCPUL recfg prep is needed 
*/
#define LTE_RRC_LLC_IS_PDCPUL_RECFG_PREP_NEEDED(cmd_list, cmd_idx) \
(cmd_list[cmd_idx].ll_cmd_u.pdcpul_recfg_prep_req.num_released_rb != 0)

/*! @brief Used to figure out if RLCDL cfg is needed 
*/
#define LTE_RRC_LLC_IS_RLCDL_CFG_NEEDED(cmd_list, cmd_idx) \
((cmd_list[cmd_idx].ll_cmd_u.rlcdl_cfg_req.num_released_rb != 0) || \
 (cmd_list[cmd_idx].ll_cmd_u.rlcdl_cfg_req.num_modified_rb != 0) || \
 (cmd_list[cmd_idx].ll_cmd_u.rlcdl_cfg_req.embms_cfg.num_added_mrb != 0) || \
 (cmd_list[cmd_idx].ll_cmd_u.rlcdl_cfg_req.embms_cfg.num_released_mrb != 0))

/*! @brief Used to figure out if RLCUL cfg is needed 
*/
#define LTE_RRC_LLC_IS_RLCUL_CFG_NEEDED(cmd_list, cmd_idx) \
((cmd_list[cmd_idx].ll_cmd_u.rlcul_cfg_req.num_released_rb != 0) || \
 (cmd_list[cmd_idx].ll_cmd_u.rlcul_cfg_req.num_modified_rb != 0))

/*! @brief Used to figure out if MAC cfg is needed 
*/
#define LTE_RRC_LLC_IS_MAC_CFG_NEEDED(cmd_list, cmd_idx) \
(cmd_list[cmd_idx].ll_cmd_u.mac_cfg_req.cfg_data.cfg_bitmask != 0)

/*! @brief Used to figure out if phy common cfg is needed 
*/
#define LTE_RRC_LLC_IS_PHY_COMMON_CFG_NEEDED(cmd_list, cmd_idx) \
(cmd_list[cmd_idx].ll_cmd_u.cphy_common_cfg_req.common_cfg_param.cfg_mask != 0)

/*! @brief Macro used to figure out if phy dedicated cfg req is needed
*/
#define LTE_RRC_LLC_IS_PHY_DEDICATED_CFG_NEEDED(cmd_list, cmd_idx) \
(cmd_list[cmd_idx].ll_cmd_u.cphy_dedicated_cfg_req.dedicated_cfg_param.cfg_mask != 0)
/*! @brief Macro used to figure out if phy handover cfg req is needed
*/
#define LTE_RRC_LLC_IS_PHY_HANDOVER_CFG_NEEDED(cmd_list, cmd_idx) \
(cmd_list[cmd_idx].ll_cmd_u.cphy_handover_req.common_mobility_cfg_param.cfg_mask != 0)

/*! @brief Macro used to get the MRB config idx from MRB id
*/
#define LTE_RRC_LLC_GET_MRB_CFG_IDX(mrb_id) \
 (mrb_id - (LTE_EMBMS_MCCH_RB_ID + 1))

/*! @brief Macro used to define max allowed resourceConfig_r10 of csi_rs_r10 
*/
#define LTE_RRC_LLC_MAX_FDD_RESOURCE_CONFIG_R_10_CP 19

/*! @brief enumeration to identify the type of lower layer command 
*/
typedef enum
{
  CFG_PTR_IN_USE_NONE = 0,   /*!<  No cfg ptr in use */
  CFG_PTR_IN_USE_CC,       /*!< Indicates that CC is the cfg ptr in use */
  CFG_PTR_IN_USE_OC     /*!< Indicates that OC is the cfg ptr in use */
} lte_rrc_llc_cfg_ptr_in_use_e;

/*! @brief enumeration to indicate type of scell delet 
*/
typedef enum
{
  LTE_RRC_LLC_DELET_ONE_SCELL = 0,   /*!<  Delet only one scell from cfg db*/
  LTE_RRC_LLC_DELET_ALL_SCELL       /*!< Delet all scell's from cfg db  */
} lte_rrc_llc_scell_delet_type_e;


/*! @brief enumeration to identify the type of handover configuration 
*/
typedef enum
{
  LTE_RRC_LLC_HANDOVER_NONE = 0,   
  LTE_RRC_LLC_HANDOVER_FDD_INTRA_FREQ,
  LTE_RRC_LLC_HANDOVER_FDD_INTER_FREQ,
  LTE_RRC_LLC_HANDOVER_TDD_INTRA_FREQ,
  LTE_RRC_LLC_HANDOVER_TDD_INTER_FREQ,
  LTE_RRC_LLC_HANDOVER_FDD_TO_TDD,
  LTE_RRC_LLC_HANDOVER_TDD_TO_FDD,
  LTE_RRC_LLC_HANDOVER_TO_EUTRA
} lte_rrc_llc_handover_e;


/*! This structure defines all LTE related lower layer configuration
    information [PDCP, RLC, MAC, L1]
*/
typedef struct
{
  /* ================================ */
  /* Physical layer config parameters */
  /* ================================ */

  /* Common Physical layer Configuration */
  /* =================================== */

  /*! Parameters required to ul frequency info */
  lte_cphy_ul_freq_cfg_s ul_freq_cfg;
  /*! Parameters required to configure PDSCH physical channel */
  lte_cphy_pdsch_cfg_common_param_s pdsch_cfg_common; 
  /*! Parameters required to configure PUCCH physical channel */
  lte_cphy_pucch_cfg_common_param_s pucch_cfg_common;
  /*! Parameters required to configure PUSCH physical channel */ 
  lte_cphy_pusch_cfg_common_param_s pusch_cfg_common;
  /*! Parameters required to configure UL Sounding Reference Signal */
  lte_cphy_srs_cfg_common_param_s srs_cfg_common;
  /*! Parameters required to configure UL Power Control information */
  lte_cphy_ul_power_ctrl_common_param_s ul_power_ctrl_common;
  /*! Parameters required to configure UL prefix cyclic length */
  lte_cphy_ul_cyclic_prefix_length_e ul_cyclic_prefix_length;
  /*! Parameters required to configure MBSFN */
  lte_cphy_mbsfn_cfg_s mbsfn_cfg;
  /*! UE timers and constants required for Physical layer problems detection/recovery */
  lte_cphy_tmr_and_const_param_s tmr_and_const_param;
  /*! Cell selection params: required for S-criteria check */
  lte_cphy_cell_select_cfg_s cell_selection_cfg;

  /* ML1 needs to know all PMCH (MTCH/MCCH) related config to determine 
     PRS conflicting subframes with PMCH even we are not monitoring any */
  /*! Parameters required to acquire MBMS control information */
  lte_cphy_mbsfn_areas_mcch_schdl_cfg_s    mbsfn_areas_mcch_schdl_cfg;
  /*! Parameters required to find allocated PMCHs  */
  lte_cphy_mbsfn_areas_cfg_s               mbsfn_areas_cfg;
  /*! Parameters required to configure Rel-10 CSI-RS*/
  lte_cphy_pcch_cfg_common_param_s  pcch_cfg_common;
  /*! Parameters required to configure PRACH common cfg */
  lte_cphy_prach_cfg_common_param_s prach_cfg_common;

  /* Additional common Information used during Handover */
  /* ================================================== */

  /*! Parameters required to configure PHICH */
  lte_l1_phich_cfg_param_s phich_cfg;
  /*! Parameters required to configure common antenna info*/
  lte_cphy_antenna_cfg_common_param_s antenna_cfg_common;
  /*! Parameters required to configure TDD Config */
  lte_cphy_tdd_cfg_param_s tdd_cfg; 
  /*! Is a HO Configuration in progress? */
  lte_rrc_llc_handover_e ho_config_in_progress;
  /*! Source cell info during handover */
  lte_rrc_cell_info_s ho_source_cell_info;
  /*! Target cell info during handover */
  lte_cphy_ho_target_cell_info_s ho_target_cell_info;
  /*! p_max value used during handover */
  int8 target_cell_p_max;
  /*! neighCellConfig : Provides information related to MBSFN and TDD UL/DL configuration 
  of neighbour cells of this frequency  */
  uint8 neighbourCellConfig;


  /* Dedicated Physical layer Configuration */
  /* ====================================== */

  /*! Parameters required to configure PDSCH physical channel */
  lte_cphy_pdsch_cfg_dedicated_param_s pdsch_cfg_dedicated;
  /*! Parameters required to configure PUCCH physical channel */
  lte_cphy_pucch_cfg_dedicated_param_s pucch_cfg_dedicated;
  /*! Parameters required to configure PUSCH physical channel */
  lte_cphy_pusch_cfg_dedicated_param_s pusch_cfg_dedicated;
  /*! Parameters required to configure UL Sounding Reference Signal */
  lte_cphy_srs_cfg_dedicated_param_s srs_cfg_dedicated;
  /*! Parameters required to configure UL Power Control information */
  lte_cphy_ul_power_ctrl_dedicated_param_s ul_power_ctrl_dedicated;
  /*!Indicates if the cqi_cfg I.E received is rel-8 or Rel-10 */
  lte_3gpp_release_ver_e cqi_cfg_rel_version;
  /*! Parameters required to configure CQI reporting  */
  lte_cphy_cqi_reporting_param_s cqi_cfg;
  /*! Parameters required to configure the uplink shared channel */
  lte_cphy_ulsch_cfg_dedicated_param_s    ulsch_cfg; 
  /*!Indicates if the antenna_cfg I.E received is rel-8 or Rel-10 */
  lte_3gpp_release_ver_e antenna_cfg_rel_version;
  /*! Parameters required to configure dedicated Antenna information. */
  lte_cphy_antenna_cfg_dedicated_param_s antenna_cfg_dedicated;
  /*! Parameters required to configure connected mode DRX configuration. */
  lte_cphy_drx_cfg_dedicated_param_s connected_drx_cfg;
  /*! Parameters required to configure TPC for PUCCH  */
  lte_cphy_tpc_pdcch_cfg_s tpc_pdcch_cfg_pucch;
  /*! Parameters required to configure TPC for PUSCH  */
  lte_cphy_tpc_pdcch_cfg_s tpc_pdcch_cfg_pusch;
  /*! Parameters required to configure scheduling request parameters  */
  lte_cphy_scheduling_req_cfg_type scheduling_req_cfg;
  /*! Parameters required to configure UE related configuration */
  lte_cphy_ue_related_cfg_s ue_related_cfg;
  /*! Parameters required to configure Semi Persistent Scheduling info  */
  lte_cphy_sps_cfg_s sps_cfg;
  /*! Parameters required to configure Uplink Power headroom  */
  lte_cphy_phr_cfg_dedicated_param_s phr_cfg;
  /*! Dedicated RLF timers and constant params: used for RLF detection/recovery */
  lte_rrc_dedicated_rlf_timers_s tmr_and_const_dedicated;
  /*! DRX optimization for UM mode enabled in ML1 */
  boolean drx_opt_enabled;
  /*! Parameters required to configure Scell's for Carrier Aggregation feature */
  lte_cphy_scell_cfg_param_s scell_config;
  /*! Parameters required to configure dedicated pcell rel-10 information. */
  lte_cphy_dedicated_pcell_rel10_cfg_s pcell_rel10_cfg;
  /*! Parameters required to configure dedicated csi_rs information. */
  lte_cphy_dedicated_csi_rs_config_s csi_rs_cfg;
  /*! Time domain resource restriction pattern for the serving cell measurements 
     (RSRP, RSRQ and the radio link monitoring),*/
  lte_l1_subfm_pattern_cfg_param_s meas_subfm_pattern_pcell;
  /*! Time domain resource restriction pattern applicable to intra-freq neighbour cell 
      RSRP and RSRQ measurements */
  lte_cphy_meas_subfm_patter_neigh_s meas_subfm_pattern_intra_neigh;
  /*! Neighbor Cell CRS Assistance Info Configuration*/
  lte_cphy_neigh_cell_crs_cfg_param_s neigh_cell_crs_cfg;

  /* =========================== */
  /* MAC layer config parameters */
  /* =========================== */

  lte_mac_dl_cfg_s  mac_dl_cfg; /*!< Configuration Information for MAC DL */
  lte_mac_ul_cfg_s  mac_ul_cfg; /*!< Configuration Information for MAC UL */
  lte_mac_rach_cfg_s  mac_rach_cfg; /*!< Configuration Information for RACH */
  lte_mac_embms_cfg_s mac_embms_cfg; /*!< Configuration Information for eMBMS */

  uint8 mac_num_active_lc;
  lte_lc_id_t mac_active_lc[LTE_MAX_ACTIVE_LC];
  lte_mac_lcqos_cfg_s mac_lc_list[LTE_MAX_LC];
  boolean mac_tdd_subframe_assign_enabled;  /*!< If TDD subframe cfg is enabled in MAC */
  
  uint32  cdrx_length;   /*!< CDRX length configured for MAC */
  
  /* =========================== */
  /* RLC layer config parameters */
  /* =========================== */
  uint8 rlcdl_num_active_rb; /*!< Number of active RBs in RLC DL */
  lte_rb_cfg_idx_t rlcdl_active_rb_cfg_idx[LTE_MAX_ACTIVE_RB]; /*!< Array of indices of active RBs into base cfg info*/
  lte_rlcdl_rb_info_s rlcdl_rb_info[LTE_MAX_RB_CFG_IDX]; /*!< RLC DL base cfg info */

  uint8 rlcdl_num_active_mrb; /*!< Number of active MRBs in RLC DL */
  lte_rb_cfg_idx_t rlcdl_active_mrb_cfg_idx[LTE_MAX_ACTIVE_MRB]; /*!< Array of indices of active MRBs into base cfg info*/
  lte_rlcdl_mrb_info_s rlcdl_mrb_info[LTE_MAX_ACTIVE_MRB]; /*!< RLC DL base eMBMS cfg info */

  uint8 rlcul_num_active_rb; /*!< Number of active RBs in RLC UL */
  lte_rb_cfg_idx_t rlcul_active_rb_cfg_idx[LTE_MAX_ACTIVE_RB]; /*!< Array of indices of active RBs into base cfg info */
  lte_rlcul_rb_info_s rlcul_rb_info[LTE_MAX_RB_CFG_IDX]; /*!< RLC UL base cfg info */

  /* ============================= */
  /* PDCP layer config parameters  */
  /* ============================= */
  uint8 pdcpdl_num_active_rb; /*!< Number of active RBs in PDCP DL */
  lte_rb_cfg_idx_t pdcpdl_active_rb_cfg_idx[LTE_MAX_ACTIVE_RB]; /*!< Array of indices of active RBs into base cfg info*/
  lte_pdcpdl_info_s pdcpdl_info[LTE_MAX_RB_CFG_IDX]; /*!< PDCP DL base cfg info */
  uint8 pdcpul_num_active_rb; /*!< Number of active RBs in PDCP UL */
  lte_rb_cfg_idx_t pdcpul_active_rb_cfg_idx[LTE_MAX_ACTIVE_RB]; /*!< Array of indices of active RBs into base cfg info*/
  lte_pdcpul_info_s pdcpul_info[LTE_MAX_RB_CFG_IDX]; /*!< PDCP UL base cfg info */
  lte_pdcp_security_cfg_s pdcp_sec_cfg; /* PDCP security configuration */
  boolean rohc_reset; /* PDCP ROHC Reset */

  /* =========================== */
  /* Active EPS bearers */
  /* =========================== */ 
  lte_rrc_eps_rb_mapping_s active_eps_bearer_list[LTE_RRC_MAX_NUM_ACTIVE_EPS_BEARERS]; /*!< EPS bearers */
  uint8 num_active_bearers; /*!< Number of active EPS bearers */
  /*!< All EPS bearers that are currently active, this includes bearers that 
  have been newly added as well as those that are already existing */
  uint8 num_added_bearers; /*!< Number of added EPS bearers */
  /*!< EPS bearers that have been newly added */
  lte_rrc_eps_rb_mapping_s added_eps_bearers[LTE_RRC_MAX_NUM_ACTIVE_EPS_BEARERS]; 
  uint8 num_removed_bearers; /*!< Number of removed EPS bearers */
  lte_rrc_eps_rb_mapping_s removed_eps_bearers[LTE_RRC_MAX_NUM_ACTIVE_EPS_BEARERS]; /*!< Removed EPS bearers */

  /*! Flag to indicate that RBs have been suspendedn and ML1 has been aborted */
  boolean rbs_suspended_ml1_aborted;

  /*! Flag to indicate if ML1 has been aborted, Will be set when ML1 is aborted, 
    Will be reset when ML1 is configured after abort */
  boolean ml1_aborted;

} lte_rrc_cfg_db_type;


/*! Defines lower layer command type.  Next state
    will be used as identifier for telling what type of cmd it is. 
 */
typedef struct
{
  /*< next LLC state to transition to - also identifies the lower layer cmd */
  stm_state_t next_state;
  /*!< Request UMID to be sent - identifies cmd in union */
  msgr_umid_type req_umid; 
  /*< union of lower layer cmds */
  union 
  {
    /*!< cphy common cfg req data */
    lte_cphy_common_cfg_req_s  cphy_common_cfg_req;
    /*!< cphy dedicated cfg req data */
    lte_cphy_dedicated_cfg_req_s  cphy_dedicated_cfg_req;
    /*!< cphy ho cfg req data */
    lte_cphy_handover_req_s  cphy_handover_req;
    /*!< cphy con release data */
    lte_cphy_con_release_req_s  cphy_con_release_req;
    /*!< cphy abort req data */
    lte_cphy_abort_req_s cphy_abort_req;
    /*!< MAC_CONFIG_REQ data */
    lte_mac_cfg_req_msg_s  mac_cfg_req;
    /*!< MAC_ACCESS_ABORT_REQ data */
    lte_mac_access_abort_req_msg_s mac_access_abort_req;
    /*!< rlc dl cfg data */
    lte_rlcdl_cfg_req_s rlcdl_cfg_req;
    /*!< rlc ul cfg data */
    lte_rlcul_cfg_req_s rlcul_cfg_req;
    /*!< pdcp ul recfg prep  data */
    lte_pdcpul_recfg_prep_req_msg_s pdcpul_recfg_prep_req;
    /*!< pdcp dl cfg data */
    lte_pdcpdl_cfg_req_msg_s pdcpdl_cfg_req;
    /*!< pdcp ul cfg data */
    lte_pdcpul_cfg_req_msg_s pdcpul_cfg_req;
  }ll_cmd_u;

  /*!< Confirmation UMID to expect */
  msgr_umid_type cnf_umid;             
}lte_rrc_llc_lower_layer_cmd_s;


/*! This contains the history of the last LTE_RRC_MAX_NUM_OF_LL_CONFIGS that went
    to lower layers[PDCP, RLC, MAC, L1].
*/
typedef struct
{
  /* ============================== */
  /* Physical layer config request  */
  /* ============================== */

  /*! next index in use for CPHY_COMMON_CFG_REQ */
  uint8 next_common_phy_idx; 
  lte_rrc_llc_lower_layer_cmd_s phy_common_cfg_cmd_list[LTE_RRC_MAX_NUM_OF_CMD_IN_HIST]; 

  /*! next index in use for CPHY_DEDICATED_CFG_REQ */
  uint8 next_dedicated_phy_idx; 
  lte_rrc_llc_lower_layer_cmd_s phy_dedicated_cfg_cmd_list[LTE_RRC_MAX_NUM_OF_CMD_IN_HIST]; 

  /*! next index in use for CPHY_HANDOVER_REQ */
  uint8 next_handover_phy_idx; 
  lte_rrc_llc_lower_layer_cmd_s phy_handover_cfg_cmd_list[LTE_RRC_MAX_NUM_OF_CMD_IN_HIST]; 

  /*! next index in use for CPHY_CON_RELEASE_REQ */
  uint8 next_con_release_phy_idx; 
  lte_rrc_llc_lower_layer_cmd_s phy_con_release_cmd_list[LTE_RRC_MAX_NUM_OF_CMD_IN_HIST]; 

  /*! next index in use for CPHY_ABORT_REQ */
  uint8 next_abort_req_idx; 
  lte_rrc_llc_lower_layer_cmd_s phy_abort_cmd_list[LTE_RRC_MAX_NUM_OF_CMD_IN_HIST];

  /* ======================== */
  /* MAC layer config request */
  /* ======================== */

  /*! next index in use for MAC_CONFIG_REQ */
  uint8 next_mac_idx;
  lte_rrc_llc_lower_layer_cmd_s mac_cfg_cmd_list[LTE_RRC_MAX_NUM_OF_CMD_IN_HIST];  

  /*! next index in use for MAC_ACCESS_ABORT_REQ */
  uint8 next_mac_access_abort_idx;
  lte_rrc_llc_lower_layer_cmd_s mac_access_abort_cmd_list[LTE_RRC_MAX_NUM_OF_CMD_IN_HIST];  

  /* ======================== */
  /* RLC layer config request */
  /* ======================== */

  /*! next index in use for RLCDL_CONFIG_REQ */
  uint8 next_rlcdl_idx;
  lte_rrc_llc_lower_layer_cmd_s rlcdl_cfg_cmd_list[LTE_RRC_MAX_NUM_OF_CMD_IN_HIST]; 

  /*! next index in use for RLCUL_CONFIG_REQ */
  uint8 next_rlcul_idx;
  lte_rrc_llc_lower_layer_cmd_s rlcul_cfg_cmd_list[LTE_RRC_MAX_NUM_OF_CMD_IN_HIST]; 


  /* ========================= */
  /* PDCP layer config request */
  /* ========================= */

  /*! next index in use for PDCPDL_CONFIG_REQ */
  uint8 next_pdcpdl_idx;
  lte_rrc_llc_lower_layer_cmd_s pdcpdl_cfg_cmd_list[LTE_RRC_MAX_NUM_OF_CMD_IN_HIST]; 

  /*! next index in use for PDCPUL_CONFIG_REQ */
  uint8 next_pdcpul_idx;
  lte_rrc_llc_lower_layer_cmd_s pdcpul_cfg_cmd_list[LTE_RRC_MAX_NUM_OF_CMD_IN_HIST]; 

  /*! next index in use for PDCPUL_RECFG_PREP_REQ */
  uint8 next_pdcpul_prep_idx;
  lte_rrc_llc_lower_layer_cmd_s pdcpul_recfg_prep_cmd_list[LTE_RRC_MAX_NUM_OF_CMD_IN_HIST];

  /* ================================== */
  /* Information regarding current reqi */
  /* ================================== */
  /*! Procedure ID of the initiating module */
  lte_rrc_proc_e proc; 
  /*! transaction ID to pair the CNF_CNFI with CNF_REQI. */ 
  uint32 cfg_trans_id;
  /*! Field to indicate the configuration type */
  lte_rrc_cfg_info_s cfg_info;
  /*! Field to indicate if CFG_CNFI is needed by Proc */
  boolean cfg_cnfi_required; 
  /*! Current cell info */
  lte_rrc_cell_info_s cell_info;
  /*! Status of last dedicated config request */
  lte_errno_e last_dedicated_cfg_status;
  /*! Flag to indicate if Rel 9 full configuration is in progress */
  boolean full_config_in_progress;
  
} lte_rrc_cmd_db_s;

/*! This structure contains pointers to the current commands that should go to lower
    layers as the result of LTE_RRC_CFG_REQI being processed.
*/
typedef struct
{
  /* Indicates the number of lower layer cmds */
  uint8 num_cmds;
  /* Pointer to lower layer cmds */
  lte_rrc_llc_lower_layer_cmd_s *ll_cmd_ptr[LTE_RRC_MAX_NUM_LL_CMD];
  /* Indicates the index of the next cmd to be sent */
  uint8 next_cmd_idx;
} lte_rrc_llc_cmd_buffer_s;


/*! @brief Structure for private data variables that can be allocated dynamically
     from the heap when LTE is activated, and deallocated when LTE is deactivated
*/
typedef struct
{
  /*! Two copies of cfg data - for Current and Ordered Configurations*/
  lte_rrc_cfg_db_type cfg_db[2]; 

  /*! Current Config ptr - points to the configuration the lower layers
       are currently configured with. */
  lte_rrc_cfg_db_type* cc_ptr; 

  /*! Ordered Config ptr - has the following uses 
       1. Acts as a scratch pad for applying the configuration given by the OTA
          message.  When lower layers are configured successfully with the 
          configuration given by the OTA message, OC becomes CC. 
       2. During HO failure due to RACH, OC points to the configuration that existed
          prior to receiving the HO mesg that failed. */
  lte_rrc_cfg_db_type* oc_ptr; 

  /*! Indicates the cfg ptr in use */
  lte_rrc_llc_cfg_ptr_in_use_e cfg_ptr_in_use; 

  /*! LLC lower layer command DB - Contains History of the last few cmds gone to LL */
  lte_rrc_cmd_db_s cmd_db;

  /*! Lower layer command buffer - cmds constructed due to current LTE_RRC_CFG_REQI */
  lte_rrc_llc_cmd_buffer_s curr_cmd_buffer;

  /*! Transaction identifier for CFG_REQI and CFG_CNFI*/
  boolean trans_id_in_use[LTE_RRC_MAX_TRANS_ID];

  /*! Deadlock Detect Timer */
  lte_rrc_tmr_s deadlock_tmr; 

  /*! Deadlock Timer remaining time */
  uint64 deadlock_tmr_remaining_time; 

}lte_rrc_llc_dd_s;


/*! @brief Structure for state-machine per-instance local variables
*/
typedef struct
{
  /*!< Dynamically allocated part of private data */
  lte_rrc_llc_dd_s *dd_ptr;

} lte_rrc_llc_s;

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern void lte_rrc_llc_init_data
(
  lte_rrc_llc_s *i_ptr   /*!< llc private data pointer */
);

extern lte_errno_e lte_rrc_llc_update_cfg_db_with_sibs
(
  lte_rrc_cfg_db_type* cfg_db_ptr,  /*!< config ptr to update with sib info */
  lte_rrc_cfg_info_s cfg_info,       /*!< config info to check if sib1, 2 and drx cfg needed */
  lte_rrc_cell_info_s cell_info,     /*!< Current cell info */
  lte_rrc_llc_s* i_ptr        /*!< ptr to global prv data */
);
extern lte_errno_e lte_rrc_llc_update_cfg_db_with_default_params
(
  lte_rrc_cfg_db_type* cfg_db_ptr,  /*!< config ptr to update with sib info */
  lte_rrc_cell_info_s cell_info     /*!< Current cell info */
);

extern void lte_rrc_llc_update_l2_with_old_config
(
  lte_rrc_llc_s* i_ptr              /*!< ptr to global prv data */
);
#ifdef FEATURE_LTE_ELS_ENABLED
extern void lte_rrc_llc_deconfig_udc
(
  lte_rrc_llc_s* i_ptr,       /*!< ptr to global prv data */
  lte_pdcp_udc_rel_reason_type_e udc_rel_reason /*!< UDC release reason */
);
extern boolean lte_rrc_llc_check_udc_enabled
(
);

#endif
extern lte_errno_e lte_rrc_llc_construct_lower_layer_cmds
(
  lte_rrc_llc_s* i_ptr              /*!< ptr to global prv data */
);

extern uint8 lte_rrc_llc_get_trans_id
(
  lte_rrc_llc_s *i_ptr   /*!< llc private data pointer */
);

extern void lte_rrc_llc_copy_cfg_db
(
  lte_rrc_cfg_db_type* rrc_llc_src_cfg_ptr,  /*!< Source config DB */
  lte_rrc_cfg_db_type* rrc_llc_dest_cfg_ptr  /*!< Destination config DB */
);

extern void lte_rrc_llc_swap_oc_and_cc
(
  lte_rrc_llc_s *i_ptr   /*!< llc private data pointer */
);

extern void lte_rrc_llc_copy_embms_cfg_between_dbs
(
  lte_rrc_cfg_db_type* src_cfg_db_ptr,  /*!< Ptr to Source config DB */
  lte_rrc_cfg_db_type* dest_cfg_db_ptr  /*!< Ptr to Destination config DB */
);

extern void lte_rrc_llc_init_cmd_buf
(
  lte_rrc_llc_cmd_buffer_s* cmd_buf_ptr /*<! ptr to command buffer */
);
extern void lte_rrc_llc_init_oc_ptr_fields
(
  lte_rrc_cfg_db_type* oc_ptr /*<! ptr to cfg db */
);
extern void lte_rrc_llc_init_action_fields
(
  lte_rrc_cmd_db_s *cmd_db_ptr /*!< Ptr to command db */
);

extern void lte_rrc_llc_clear_trans_id
(
  lte_rrc_llc_s *i_ptr,   /*!< llc private data pointer */
  uint32 trans_id  /*!< trans id to clear */
); 

extern lte_errno_e lte_rrc_llc_handle_rrc_connection_setup
(
  lte_rrc_llc_s* i_ptr,        /*!< ptr to global prv data */
  lte_rrc_cfg_info_s cfg_info  /*!< contains ASN.1 decoded OTA msg */
);

extern lte_errno_e lte_rrc_llc_handle_rrc_connection_recfg
(
  lte_rrc_llc_s* i_ptr,        /*!< ptr to global prv data */
  lte_rrc_cfg_info_s cfg_info  /*!< contains ASN.1 decoded OTA msg */
);
#ifdef FEATURE_LTE_ELS_ENABLED
extern lte_errno_e lte_rrc_llc_handle_rrc_connection_recfg_els
(
  lte_rrc_llc_s* i_ptr,        /*!< ptr to global prv data */
  lte_rrc_cfg_info_s cfg_info  /*!< contains ASN.1 decoded OTA msg */
);
#endif

extern lte_errno_e lte_rrc_llc_handle_rrc_connection_reest
(
  lte_rrc_llc_s* i_ptr,        /*!< ptr to global prv data */
  lte_rrc_cfg_info_s cfg_info  /*!< contains ASN.1 decoded OTA msg */
);

extern lte_errno_e lte_rrc_llc_handle_security_config
(
  lte_rrc_llc_s *i_ptr  /*!< Pointer to LLC internal structure */
);

extern lte_errno_e lte_rrc_llc_handle_security_config_for_ho
(
  lte_rrc_llc_s *i_ptr  /*!< Pointer to LLC internal structure */
);

extern void lte_rrc_llc_update_eps_info
(
  void
);

extern void lte_rrc_llc_release_all_rbs
(
  lte_rrc_cfg_db_type* cfg_db_ptr /*!< config ptr to update with drb info */ 
);

extern void lte_rrc_llc_init_phy_common_param
(
 lte_rrc_cfg_db_type *cfg_db_ptr
);

extern void lte_rrc_llc_init_phy_dedicated_param
(
 lte_rrc_cfg_db_type *cfg_db_ptr
);

extern void lte_rrc_llc_init_dedicated_tmr_and_const_param
(
  lte_rrc_dedicated_rlf_timers_s *ded_tmr_and_const_ptr
);

extern void lte_rrc_llc_deadlock_tmri_ef
(
  void
);

extern lte_errno_e lte_rrc_llc_revert_cqi_srs_sr_config_to_default
(
  lte_rrc_cfg_db_type* cfg_db_ptr  /*!< config ptr to update with sib info */
);
extern lte_errno_e lte_rrc_llc_update_pdsch_cfg_common
(
  lte_rrc_osys_PDSCH_ConfigCommon pdsch_cfg_sib, /*!< PDSCH config common from sib2 */
  lte_rrc_cfg_db_type* cfg_db_ptr  /*!< config ptr to update with sib info */
);
extern lte_errno_e lte_rrc_llc_update_prach_cfg
(
  lte_rrc_osys_PRACH_Config prach_cfg, /*!< PRACH config common from mobility ctrl IE */
  lte_rrc_cfg_db_type* cfg_db_ptr  /*!< config ptr to update with sib info */
);

extern lte_errno_e lte_rrc_llc_update_prach_cfg_sib
(
  lte_rrc_osys_PRACH_ConfigSIB prach_cfg_sib, /*!< PRACH config common from sib2 */
  lte_rrc_cfg_db_type* cfg_db_ptr  /*!< config ptr to update with sib info */
);

extern lte_errno_e lte_rrc_llc_update_pucch_cfg_common
(
  lte_rrc_osys_PUCCH_ConfigCommon pucch_cfg_common, /*!< PUCCH config common */
  lte_rrc_cfg_db_type* cfg_db_ptr  /*!< config ptr to update with sib info */
);

extern lte_errno_e lte_rrc_llc_update_pusch_cfg_common
(
  lte_rrc_osys_PUSCH_ConfigCommon pusch_cfg_sib, /*!< PUSCH config common from sib2 */
  lte_rrc_cfg_db_type* cfg_db_ptr  /*!< config ptr to update with sib info */
);

extern lte_errno_e lte_rrc_llc_update_srs_cfg_common
(
  lte_rrc_osys_SoundingRS_UL_ConfigCommon srs_cfg_sib, /*!< srs config common */
  lte_rrc_cfg_db_type* cfg_db_ptr,  /*!< config ptr to update with sib info */
  uint32 srs_cfg_common_present  /*!< if srs_cfg_common was signaled */
);

extern lte_errno_e lte_rrc_llc_update_ul_power_ctrl_common
(
  lte_rrc_osys_UplinkPowerControlCommon ul_pwr_ctrl_sib, /*!< ul power ctrl from sib2 */
  lte_rrc_cfg_db_type* cfg_db_ptr  /*!< config ptr to update with sib info */
);

extern lte_errno_e lte_rrc_llc_update_antenna_info_common
(
  lte_rrc_osys_AntennaInfoCommon antenna_info, /*!< Antenna Info from mobility ctrl IE */
  lte_rrc_cfg_db_type* cfg_db_ptr  /*!< config ptr to update with sib info */
);

extern lte_errno_e lte_rrc_llc_update_mbsfn_cfg
(
  lte_rrc_osys_MBSFN_SubframeConfigList *mbsfg_cfg_ptr, /*!< MBSFN configuration */
  lte_rrc_cfg_db_type* cfg_db_ptr,  /*!< config ptr to update with sib info */
  uint32 mbsfn_cfg_bit      /*!< field reflecting mbsfn bit mask in OTA msg*/
);

extern lte_errno_e lte_rrc_llc_update_tmr_and_const_param
(
  lte_rrc_osys_UE_TimersAndConstants tmr_and_const_param, /*!< MBSFN configuration */
  lte_rrc_cfg_db_type* cfg_db_ptr  /*!< config ptr to update with sib info */
);

extern lte_errno_e lte_rrc_llc_update_phich_cfg
(
  lte_rrc_osys_PHICH_Config phich_cfg, /*!< PRACH config common from mobility ctrl IE */
  lte_l1_phich_cfg_param_s* phich_cfg_ptr,  /*!< config ptr to update with sib info or scell config */
  boolean update_pcell_bitmask  /*update bit-mask to indicate that PHICH info needs to be updated */
);

extern lte_errno_e lte_rrc_llc_update_target_cell_info
(
  /*!< Mobility Ctrl IE used for target cell info */
  lte_rrc_osys_MobilityControlInfo* mobility_ctrl_info_ptr, 
  /*!< LLC internal data ptr */
  lte_rrc_llc_s* i_ptr  
);

extern lte_errno_e lte_rrc_llc_update_tdd_cfg
(
  lte_rrc_osys_TDD_Config tdd_cfg, /*!< TDD Config from mobility ctrl IE */
  lte_rrc_cfg_db_type* cfg_db_ptr  /*!< config ptr to update with sib info */
);
extern lte_errno_e lte_rrc_llc_update_r11_tdd_cfg
(
   lte_rrc_osys_TDD_Config tdd_cfg, /*!< TDD Config from mobility ctrl IE */
   lte_rrc_cfg_db_type* cfg_db_ptr,  /*!< config ptr to update with sib info */
   lte_rrc_osys_TDD_Config_v1130 tdd_Config_v1130 /*!< R11 TDD Config from mobility ctrl IE */
);                                                    
extern lte_errno_e lte_rrc_llc_update_ul_freq
(
  lte_rrc_osys_SystemInformationBlockType2* sib2_ptr, /*!< SIB2 */
  lte_rrc_cfg_db_type* cfg_db_ptr,  /*!< config ptr to update with sib info */
  lte_rrc_cell_info_s cell_info,     /*!< Current cell info */
  lte_rrc_llc_s* i_ptr        /*!< ptr to global prv data */
); 

extern lte_errno_e lte_rrc_llc_process_pdsch_cfg_dedicated
(
  lte_rrc_cfg_db_type* cfg_db_ptr,  /*!< config ptr to update with sib info */
  lte_rrc_osys_PDSCH_ConfigDedicated* pdsch_ded_cfg_ptr /*<! Dedicated PDSCH info */
);

extern lte_errno_e lte_rrc_llc_process_pucch_cfg_dedicated
(
  lte_rrc_cfg_db_type* cfg_db_ptr,  /*!< config ptr to update with sib info */
  lte_rrc_osys_PhysicalConfigDedicated* phy_ded_cfg_ptr, /*!< Dedicated Phy Configuration info */ 
  uint32 pucch_cfg_present /*!< if pucch_cfg was signaled */
);

extern lte_errno_e lte_rrc_llc_process_srs_cfg_dedicated
(
  lte_rrc_cfg_db_type* cfg_db_ptr,  /*!< config ptr to update with sib info */
  lte_rrc_osys_SoundingRS_UL_ConfigDedicated* srs_cfg_ded_ptr, /*!< dedicated srs config info */
  uint32 srs_cfg_present /*!< Indicates if SRS Config is signaled */
);

extern lte_errno_e lte_rrc_llc_process_aperiodic_srs_cfg_dedicated
(
  lte_rrc_cfg_db_type* cfg_db_ptr,  /*!< config ptr to update with sib info */
  lte_rrc_osys_SoundingRS_UL_ConfigDedicatedAperiodic_r10* srs_cfg_ded_ptr, /*!< dedicated srs config info */
  uint32 ap_srs_cfg_present /*!< If Aperiodic SRS Config is signaled */
);

extern lte_errno_e lte_rrc_llc_process_ul_power_ctrl_dedicated
(
  lte_rrc_cfg_db_type* cfg_db_ptr,  /*!< config ptr to update with sib info */
  lte_rrc_osys_UplinkPowerControlDedicated* ul_pwr_ctrl_ded_ptr /*!< dedicated ul power ctrl info */
);

extern lte_errno_e lte_rrc_llc_process_tpc_pdcch_cfg_pucch
(
  lte_rrc_cfg_db_type* cfg_db_ptr,  /*!< config ptr to update with sib info */
  lte_rrc_osys_TPC_PDCCH_Config* tpc_pdcch_cfg_pucch_ptr /*!< Dedicated PUCCH config */
);

extern lte_errno_e lte_rrc_llc_process_tpc_pdcch_cfg_pusch
(
  lte_rrc_cfg_db_type* cfg_db_ptr,  /*!< config ptr to update with sib info */
  lte_rrc_osys_TPC_PDCCH_Config* tpc_pdcch_cfg_pusch_ptr /*!< Dedicated PUSCH config */
);

extern lte_errno_e lte_rrc_llc_process_antenna_info
(
  lte_rrc_cfg_db_type* cfg_db_ptr,  /*!< config ptr to update with sib info */
  lte_rrc_osys_PhysicalConfigDedicated* phy_ded_cfg_ptr /*!< Dedicated Phy Configuration info */ 
);

extern lte_errno_e lte_rrc_llc_validate_cqi_mode_against_tm9
(
/*! Parameters required to configure CQI reporting  */
lte_cphy_cqi_reporting_param_s *cqi_cfg,
/*! CSI-RS cfg*/
lte_cphy_dedicated_csi_rs_s *csi_rs
);

extern lte_errno_e lte_rrc_llc_process_cqi_reporting
(
  lte_rrc_cfg_db_type* cfg_db_ptr,  /*!< config ptr to update with sib info */
  lte_rrc_osys_CQI_ReportConfig* cqi_rep_ptr, /*!< CQI reporting info */
  uint32 cqi_rep_config_present, /*!< Indicates if cqi_reporting Config is present */
  boolean scc_cqi_rel /*!< Boolean to indicate if Scc CQI is to be released or not */
);

extern lte_errno_e lte_rrc_llc_process_sr_cfg
(
  lte_rrc_cfg_db_type* cfg_db_ptr,  /*!< config ptr to update with sib info */
  lte_rrc_osys_SchedulingRequestConfig* sr_cfg_ptr, /*!< Scheduling request info */ 
  uint32 sr_cfg_present  /*!< Indicates if SR Config is signaled */
);

extern lte_errno_e lte_rrc_llc_validate_freq_info
(
  lte_earfcn_t dl_earfcn, /*!< DL EARFCN */
  boolean ul_earfcn_present, /*<! Boolean flag indicating if UL EARFCN is passed in */
  lte_earfcn_t ul_earfcn /*!< UL EARFCN */
);

extern lte_errno_e lte_rrc_llc_process_sps_cfg
(
  lte_rrc_cfg_db_type* cfg_db_ptr, /*!< config ptr to update with drb info */ 
  lte_rrc_osys_SPS_Config* sps_cfg_ptr, /*!< SPS configuration info */ 
  uint32 sps_cfg_present /*!< if SPS config is signaled */
);

extern lte_errno_e lte_rrc_llc_update_ul_cyclic_prefix_length
(
  lte_rrc_osys_UL_CyclicPrefixLength ul_prefix_cyclic_len, /*!< ul cyclic length */
  lte_rrc_cfg_db_type* cfg_db_ptr  /*!< config ptr to update with sib info */
);

extern lte_errno_e lte_rrc_llc_get_preambles_grp_a_config
(
  lte_mac_rach_cfg_s  *mac_rach_cfg_ptr,    /*!< MAC RACH Config pointer */
  lte_rrc_osys_RACH_ConfigCommon rach_cfg_common /*!< RACH configuration common that 
                                                        comes over sib2 */
);

extern lte_errno_e lte_rrc_llc_get_num_ra_preambles
(
  uint8* ra_index2,                          /*!< ra_index2 */
  lte_rrc_osys_RACH_ConfigCommon rach_cfg_common /*!< RACH configuration common that 
                                                        comes over sib2 */
);

extern lte_errno_e lte_rrc_llc_get_preamble_initial_power
(
  int16* preamble_initial_power,            /*!< preamble_initial_power */
  lte_rrc_osys_RACH_ConfigCommon rach_cfg_common /*!< RACH configuration common that 
                                                        comes over sib2 */
);

extern lte_errno_e lte_rrc_llc_get_power_ramping_step
(
  uint8* power_ramping_step, /*!< preamble_initial_power */
  lte_rrc_osys_RACH_ConfigCommon rach_cfg_common /*!< RACH configuration common that 
                                                        comes over sib2 */
);

extern lte_errno_e lte_rrc_llc_validate_prach_offset_with_ul_bw
(
  lte_rrc_cfg_db_type* cfg_db_ptr /*!< CFG DB ptr */
);


extern lte_errno_e lte_rrc_llc_get_preamble_trans_max
(
  uint16* preamble_trans_max, /*!< preamble_trans_max */
  lte_rrc_osys_RACH_ConfigCommon rach_cfg_common /*!< RACH configuration common that 
                                                        comes over sib2 */
);

extern lte_errno_e lte_rrc_llc_get_ra_response_win_size
(
  uint8* ra_rsp_win_size,  /*!< ra_rsp_win_size */
  lte_rrc_osys_RACH_ConfigCommon rach_cfg_common /*!< RACH configuration common that 
                                                        comes over sib2 */
);

extern lte_errno_e lte_rrc_llc_get_contention_resolution_tmr
(
  uint16* contention_resolution_timer,  /*!< contention_resolution_timer */
  lte_rrc_osys_RACH_ConfigCommon rach_cfg_common /*!< RACH configuration common that 
                                                 comes over sib2 */
);

extern lte_errno_e lte_rrc_llc_get_max_harq_tx
(
  lte_l1_max_harq_tx_e* max_harq_tx_ptr, /*!< Max harq tx info ptr */
  lte_rrc_osys_MAC_MainConfig* trch_cfg_ptr /*!< Transport channel cfg info */
);

extern lte_errno_e lte_rrc_llc_get_bsr_timer
(
  lte_rrc_osys_MAC_MainConfig* trch_cfg_ptr, /*!< Transport channel cfg info */
  lte_mac_ul_cfg_s*  mac_ul_cfg_ptr, /*!< MAC UL cfg ptr */
  uint32 bsr_present /*!< Indicates whether BSR has been signalled */
);

extern lte_errno_e lte_rrc_llc_get_retx_bsr_timer
(
  lte_rrc_osys_MAC_MainConfig* trch_cfg_ptr, /*!< Transport channel cfg info */
  lte_mac_ul_cfg_s*  mac_ul_cfg_ptr /*!< MAC UL cfg ptr */
);

extern lte_errno_e lte_rrc_llc_get_ta_timer
(
  lte_rrc_osys_TimeAlignmentTimer ta_tmr, /*!< TA Timer */
  lte_mac_dl_cfg_s*  mac_dl_cfg      /*!< ptr to mac dl ptr */
);

extern lte_errno_e lte_rrc_llc_process_phr_config
(
  lte_rrc_cfg_db_type* cfg_db_ptr, /*!< config ptr to update with drb info */ 
  lte_rrc_osys_MAC_MainConfig* trch_cfg_ptr /*!< Transport channel cfg info */
);

extern lte_errno_e lte_rrc_llc_get_pbr
(
  lte_rrc_osys_LogicalChannelConfig* log_chan_cfg_ptr, /*!< Log chan cfg info */
  uint16* pbr,   /*!< Priority byte rate in Kbyte/s*/
  lte_lc_id_t lc_id /*!< logical channel identity */
);

extern lte_errno_e lte_rrc_llc_get_bucket_size_duration
(
  lte_rrc_osys_LogicalChannelConfig* log_chan_cfg_ptr, /*!< Log chan cfg info */
  lte_mac_lcqos_cfg_s *lc_list_ptr, /*!< Pointer to MAC lc_list */
  uint16 *bucket_size_duration_ptr  /*!< Pointer to bucket size duration */
);

extern lte_errno_e lte_rrc_llc_update_drx_on_duration_timer
(
  lte_cphy_drx_cfg_dedicated_param_s *drx_cfg_ptr, /*!< Drx cfg ptr */ 
  lte_rrc_osys_MAC_MainConfig* trch_cfg_ptr, /*!< Transport channel cfg info */
  uint32 *on_duration_tmr_ptr /*!< Ptr to On duration timer value*/
);

extern lte_errno_e lte_rrc_llc_update_drx_inactivity_timer
(
  lte_cphy_drx_cfg_dedicated_param_s *drx_cfg_ptr, /*!< Drx cfg ptr */ 
  lte_rrc_osys_MAC_MainConfig* trch_cfg_ptr /*!< Transport channel cfg info */
);

extern lte_errno_e lte_rrc_llc_update_drx_retx_timer
(
  lte_cphy_drx_cfg_dedicated_param_s *drx_cfg_ptr, /*!< Drx cfg ptr */ 
  lte_rrc_osys_MAC_MainConfig* trch_cfg_ptr /*!< Transport channel cfg info */
);

extern lte_errno_e lte_rrc_llc_update_long_drx_cycle_start_offset
(
  lte_cphy_drx_cfg_dedicated_param_s *drx_cfg_ptr, /*!< Drx cfg ptr */ 
  lte_rrc_osys_MAC_MainConfig* trch_cfg_ptr, /*!< Transport channel cfg info */
  uint32 *long_drx_cycle_ptr /*!< Ptr to LongDRX cycle value */
);

extern lte_errno_e lte_rrc_llc_update_short_drx_cycle
(
  lte_cphy_drx_cfg_dedicated_param_s *drx_cfg_ptr, /*!< Drx cfg ptr */ 
  lte_rrc_osys_MAC_MainConfig* trch_cfg_ptr, /*!< Transport channel cfg info */
  uint32 *short_drx_cycle_ptr /*!< Ptr to ShortDRX cycle value */
);

extern lte_errno_e lte_rrc_llc_validate_connected_drx_config
(
  lte_cphy_drx_cfg_dedicated_param_s *drx_cfg_ptr, /*!< Drx cfg ptr */ 
  uint32 short_drx_cycle, /*!< ShortDRX cycle value */
  uint32 long_drx_cycle, /*!< LongDRX cycle value */
  uint32 on_duration_tmr /*!< On Duration timer value */
);

extern lte_errno_e lte_rrc_llc_get_t_reordering
(
  lte_rrc_osys_T_Reordering t_reordering_ota,   /*!< t_reordering value as signalled in the msg */
  uint16* t_reordering                /*!< t_reordering value converted into ms */
);

extern lte_errno_e lte_rrc_llc_get_t_status_prohibit
(
  lte_rrc_osys_T_StatusProhibit t_statusprohibit_ota,/*!< t_statusprohibit value as signalled in the msg */
  uint16* t_statusprohibit      /*!< t_statusprohibit value converted into ms */
);

extern lte_errno_e lte_rrc_llc_get_t_poll_retransmit
(
  lte_rrc_osys_T_PollRetransmit t_pollretransmit_ota,/*!< t_pollretransmit value as signalled in the msg */
  uint16* t_pollretransmit      /*!< t_statusprohibit value converted into ms */
);

extern lte_errno_e lte_rrc_llc_get_t_poll_pdu
(
  lte_rrc_osys_PollPDU pollpdu_ota,/*!< pollPDU value as signalled in the msg */
  uint16 *poll_pdu      /*!< poll_pdu value converted into ms */
);

extern lte_errno_e lte_rrc_llc_get_t_poll_byte
(
  lte_rrc_osys_PollByte pollbyte_ota,/*!< pollByte value as signalled in the msg */
  uint32 *poll_byte      /*!< poll_byte value converted into bytes */
);

extern lte_errno_e lte_rrc_llc_get_max_re_tx_threshold
(
  lte_rrc_osys_UL_AM_RLC* ul_am_rlc_ptr,/*!< UL AM RLC configuration info ptr */
  uint8*  max_retx_threshold      /*!< max_retx_threshold value converted into uint8 */
);

extern lte_errno_e lte_rrc_llc_get_discard_timer
(
  lte_rrc_osys_PDCP_Config *pdcp_cfg_ptr, /* pdcp configuration */
  uint16* discard_timer /*!< PDCP discard timer in msec*/
);

extern lte_errno_e lte_rrc_llc_handle_mcch_cfg
(
  lte_rrc_llc_s* i_ptr,        /*!< ptr to global prv data */
  lte_rrc_llc_mcch_cfg_s* mcch_cfg_ptr  /*!< Ptr to MCCH config */
);

extern lte_errno_e lte_rrc_llc_handle_mtch_cfg
(
  lte_rrc_llc_s* i_ptr, /*!< ptr to global prv data */
  lte_rrc_llc_mrb_cfg_s* mrb_cfg_ptr  /*!< Ptr to MRB config */
);

extern void lte_rrc_llc_remove_mbsfn_area_with_no_pmch
(
  lte_rrc_cfg_db_type *cfg_db_ptr   /*!< ptr to Cfg DB */
);

extern lte_errno_e lte_rrc_llc_config_all_pmch_in_mbsfn_area
(
  lte_rrc_cfg_db_type *cfg_db_ptr,  /*!< Ptr to config DB */
  lte_cphy_mbsfn_area_cfg_s *mbsfn_area_cfg_ptr /*! Ptr to MBSFN area config */
);

extern void lte_rrc_llc_update_mac_embms_lc
(
  lte_rrc_cfg_db_type *cfg_db_ptr /*!< Ptr to Config DB */
);

extern void lte_rrc_llc_release_all_mrbs
(
  lte_rrc_llc_s* i_ptr        /*!< ptr to global prv data */
);

extern void lte_rrc_llc_set_drx_opt_in_ml1
(
  lte_rrc_llc_s* i_ptr
);

extern lte_errno_e lte_rrc_llc_validate_reconfig_msg_for_full_config
(
  lte_rrc_osys_RRCConnectionReconfiguration_r8_IEs* conn_recfg_ptr  /*!< ASN.1 decoded OTA msg */
);

extern lte_rrc_cfg_db_type* lte_rrc_llc_get_prv_cfg_db_ptr
(
  void
);

extern lte_errno_e lte_rrc_llc_process_rel10_mac_main_config
(
  lte_rrc_cfg_db_type* cfg_db_ptr, /*!< config ptr to update with drb info */ 
  lte_rrc_osys_MAC_MainConfig* trch_cfg_ptr /*!< Transport channel cfg info */
);

extern lte_errno_e lte_rrc_llc_update_mbsfn_sf_cfg
(
  lte_rrc_osys_MBSFN_SubframeConfigList *ota_mbsfn_area_cfg_ptr,   /*!< ptr to MBSFN area config */
  lte_cphy_mbsfn_cfg_s *mbsfn_area_cfg_ptr  /*!< Ptr to MBSFN area config in DB */
);

extern lte_bandwidth_e lte_rrc_llc_update_scell_common_cfg_dl_bwth
(
  lte_rrc_osys_RadioResourceConfigCommonSCell_r10_nonUL_Configuration_r10_dl_Bandwidth_r10_Root dl_bwth   /*!< DL bandwith from ASN.1 */
);

extern lte_l1_antenna_info_ports_count_e lte_rrc_llc_update_scell_common_cfg_antenna_ports
(
  lte_rrc_osys_AntennaInfoCommon_antennaPortsCount_Root antenna_ports   /*!< antenna_ports from ASN.1 */
);

extern lte_errno_e lte_rrc_llc_process_radio_resource_scell_cfg_common
(
  lte_rrc_llc_s* i_ptr,        /*!< ptr to global prv data */
  uint8 scell_idx,            /*< scell_idx*/
  lte_rrc_osys_RadioResourceConfigCommonSCell_r10* scell_common_cfg_info_ptr /*!< ASN.1 decoded OTA msg */
);

extern lte_errno_e lte_rrc_llc_process_scell_add_modify_list
(
  lte_rrc_llc_s* i_ptr,        /*!< ptr to global prv data */
  lte_rrc_osys_SCellToAddModList_r10* scell_add_mod_list/*!< ASN.1 decoded OTA msg */
);

extern lte_errno_e lte_rrc_llc_update_rel10_antenna_info_for_scell
(
  lte_cphy_antenna_cfg_dedicated_param_s* antenna_cfg_db_ptr,  /*!< config ptr to update with antenna info */
  lte_rrc_osys_AntennaInfoDedicated_r10* antennaInfo_r10, /*!< Dedicated Phy antennaInfo_r10 info */ 
  lte_cphy_common_scell_cfg_param_s *scell_common_cfg_db_ptr, /*!< config ptr which has scell common cfg*/
  lte_earfcn_t freq /*!< Scell Freq*/
);

extern lte_errno_e lte_rrc_llc_process_codebookrestriction_r10
(
  lte_rrc_cfg_db_type* cfg_db_ptr,  /*!< config ptr to update with sib info */
  lte_rrc_osys_PhysicalConfigDedicated* phy_ded_cfg_ptr /*!< Dedicated Phy Configuration info */ 
);

extern lte_errno_e lte_rrc_llc_process_codebookrestriction_r10_for_scell
(
  lte_cphy_antenna_cfg_dedicated_param_s* antenna_cfg_db_ptr,  /*!< config ptr to update with antenna info */
  lte_rrc_osys_AntennaInfoDedicated_r10* antennaInfo_r10, /*!< Dedicated Phy antennaInfo_r10 info */ 
  boolean pmi_ri_report_configured, /*!< Boolean to indicate if PMI/RI is configured */
  lte_l1_antenna_info_ports_count_e antenna_port, /*!< CSI RS Antenna port count */
  boolean csi_rs_enabled /*!< Boolean to indicate if CSI RS is enabled*/
);


extern lte_errno_e lte_rrc_llc_update_rel10_csi_rs_cfg_info 
(
  lte_cphy_dedicated_csi_rs_config_s* csi_rs_cfg_db_ptr, /*!< config ptr to update with csi_rs cfg for scell */
  lte_rrc_osys_CSI_RS_Config_r10* csi_rs_cfg_r10 /*!< Dedicated csi_RS_Config_r10 info from ASN.1*/ 
);

extern lte_errno_e lte_rrc_llc_process_radio_resource_scell_cfg_ded
(
  lte_rrc_llc_s* i_ptr,        /*!< ptr to global prv data */
  uint8 scell_idx,            /*< scell_idx*/
  lte_rrc_osys_RadioResourceConfigDedicatedSCell_r10* scell_ded_cfg_info_ptr /*!< ASN.1 decoded OTA msg */
);

extern lte_errno_e lte_rrc_llc_update_antenna_info_ul_tran_mode_r10 
(
  lte_l1_dl_tx_mode_e* transmission_mode,                /*! transmission_mode param*/
  lte_rrc_osys_AntennaInfoUL_r10_transmissionModeUL_r10 transmission_mode_ul_r10  /*! transmission mode ul r10 ASN.1 ptr*/
);

extern lte_errno_e lte_rrc_llc_release_scell_from_cfg_db
(
  uint8 scell_id,        /*!< Scell ID to be deleted, need to pass  valid value  only if 
          scell_delet_type = LTE_RRC_LLC_DELET_ONE_SCELL, dont care otherwise */
  lte_rrc_llc_scell_delet_type_e scell_delet_type /*!< indicates if we need to delete only one scell or all*/
);

extern lte_errno_e lte_rrc_llc_process_radio_resource_cfg_ded
(
  lte_rrc_llc_s* i_ptr,        /*!< ptr to global prv data */
  lte_rrc_osys_RadioResourceConfigDedicated* ded_cfg_info_ptr /*!< ASN.1 decoded OTA msg */
);

extern lte_errno_e lte_rrc_llc_process_cqi_reporting_rel10
(
  lte_rrc_llc_s* i_ptr,        /*!< ptr to global prv data */
  lte_cphy_cqi_reporting_param_s* cqi_reporting_cfg,  /*! CQI reporting param*/
  lte_rrc_osys_CQI_ReportConfig_r10* cqi_rep_r10  /*! CQI reporting ASN.1 ptr*/
);


extern lte_errno_e lte_rrc_llc_update_cqi_report_periodic_r10 
(
  lte_rrc_llc_s* i_ptr,        /*!< ptr to global prv data */
  lte_cphy_cqi_reporting_param_s* cqi_rep_cfg,                           /*! CQI reporting param*/
  lte_rrc_osys_CQI_ReportPeriodic_r10_setup* cqi_rep_mode_periodic_r10  /*! CQI reporting ASN.1 ptr*/
);

extern lte_errno_e lte_rrc_llc_process_pcell_rel10_param
(
  lte_rrc_cfg_db_type* cfg_db_ptr,  /*!< config ptr to update with sib info */
  lte_rrc_osys_PhysicalConfigDedicated* phy_ded_cfg_ptr /*!< Ptr to PhyConfigDedicated */
);

extern void lte_rrc_llc_chk_mbsfn_sf_collision_and_update_monitor_pattern
(
  lte_rrc_llc_s* i_ptr
);

extern void lte_rrc_llc_send_cfg_cnfi
(
  lte_rrc_llc_s *i_ptr,  /*!< private data ptr */
  lte_errno_e status,                 /*!< status field in CNFI to indicate success or failure */ 
  lte_rrc_cfg_failure_type_e failure_type, /*!< Failure field in CNFI to indicate success or failure */ 
  lte_rrc_pended_cfg_reqi_s *pended_cfg_reqi_ptr, /*!< Ptr to Pended CFG REQI */
  boolean pended_cfg_reqi, /*!< Boolean to Indicate if CFG CNFI is for  Pended CFG REQI */
  lte_rrc_cfg_reqi_s *reqi_ptr,  /*!< pointer to the LTE_RRC_CFG_REQI cmd */
  boolean get_new_trans_id /*!< Boolean to Indicate if new trans_id is required */
);


#endif /* LTE_RRC_LLCI_H */

