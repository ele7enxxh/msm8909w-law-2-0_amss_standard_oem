/*!
  @file
  tfw_const.h

  @brief
  TD-SCDMA FW constant header file

*/

/*===========================================================================

  Copyright (c) 2011-2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Confidential and Proprietary

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_tdscdma/api/tfw_const.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------  
9/12/2013  YL      add TFW_SUBFRAME_LEN_CX15
09/09/13   IS      Updated TFW_MAX_UL_PHY_PER_SLOT from 2 to 4.

===========================================================================*/

#ifndef TFW_CONSTANT_H
#define TFW_CONSTANT_H


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define TFW_ERR_RECOVERY
/*! \brief Number of antennas */ 
#define TFW_NUM_ANTENNAS                  2

/*! \brief Subframe length in Cx8 */
#define TFW_SUBFRAME_LEN_CX8              (6400*8)

/*! \brief Subframe length in Cx15 */
#define TFW_SUBFRAME_LEN_CX15              (6400*15)
#define TFW_GSM_FRAME_LEN_CX15             (0x15a27)


/*! \brief Number of slots per subframe */
#define TFW_SLOTS_PER_SUBFRAME            (7)
#define TFW_GSM_SLOT_LEN_CX15             (0x2B45)


/*! \brief Constants related to software commands */
#define TFW_CMD_ACTION_DISABLE        0
#define TFW_CMD_ACTION_ENABLE         1
#define TFW_CMD_ACTION_RECONFIG       2

#define TFW_SUBFRAME_CNT_WRAP        (0x2000)
#define TFW_SUBFRAME_CNT_MAX         (0x1FFF)
#define TFW_SUBFRAME_CNT_MASK        (0x1FFF)
#define TFW_CHIPX8_CNT_WRAP          (6400*8)
#define TFW_CHIPX8_CNT_MAX           (6400*8-1)
#define TFW_CHIPX8_CNT_MASK          (0xFFFF)

#define TFW_CMD_TSTD_DISABLE          0
#define TFW_CMD_TSTD_ENABLE           1

#define TFW_TDM_DISABLE               0
#define TFW_TDM_ENABLE                1

#define TFW_DL_TIME_SLOT_0            0
#define TFW_DL_TIME_SLOT_2            1
#define TFW_DL_TIME_SLOT_3            2
#define TFW_DL_TIME_SLOT_4            3
#define TFW_DL_TIME_SLOT_5            4
#define TFW_DL_TIME_SLOT_6            5

#define TFW_DL_TIME_SLOT_MAX          6

/*! \brief Constants related to demfront tasks */
#define TFW_MAX_NUM_JDS_CELL          4
#define TFW_MAX_NUM_SHIFT             8
#define TFW_MAX_NUM_BEAMS             8
#define TFW_MAX_NUM_XFER_SEGMENTS     70
#define TFW_MAX_NUM_CODE_CHANNELS     16
#define TFW_MAX_NUM_LMEM_COPYS        8
#define TFW_NUM_SYM_CODE_CHANNEL_SF16 44
#define TFW_MAX_NUM_OVSF              16 
#define TFW_OVSF_LEN                  16
#define TFW_CIR_SIZE_PER_SHIFT        16

#define CELL_SIZE                     128
#define CIR_SIZE                      128
#define UW_CIR_SIZE                   144

#define TFW_MAX_NUM_OF_TRCH           8
#define TFW_TRCH_INFO_POOL_SIZE       (2*TFW_MAX_NUM_OF_TRCH)
#define TFW_MAX_NUM_OF_TF             64
#define TFW_TF_INFO_POOL_SIZE         (2*TFW_MAX_NUM_OF_TF)
#define TFW_MAX_NUM_OF_TFC            128
#define TFW_TFC_INFO_POOL_SIZE        (2*TFW_MAX_NUM_OF_TFC)

/*! \brief Constants related to demback tasks */
/* Out of of 6 DL slots, these many Max slots would be active in a subframe
   for CCTrCH i.e NonHS process. */
#define TFW_DEMBACK_MAX_NUM_DL_SLOTS_HS 5
/* Out of of 6 DL slots, these many Max slots would be active in a subframe
   for HS process. */
#define TFW_DEMBACK_MAX_NUM_DL_SLOTS_NONHS 4 
/* Number of elements of Look Up Table which are actaully bins */
#define TFW_DEMBACK_LUT_SIZE 32

/*! \brief Constants related to DOB status FIFO */
#define TFW_NON_HS_DECOB_STATUS_FIFO_SIZE   128
#define TFW_HS_DECOB_STATUS_FIFO_SIZE       8


/*! \brief Midamble configuraiton related constant */
#define TFW_MIDAMBLE_ALLOC_DEFAULT      0
#define TFW_MIDAMBLE_ALLOC_COMMON       1
#define TFW_MIDAMBLE_ALLOC_UE_SPECIFIC  2

/*! \brief Constants related to IRAT */
/* Number of cells used for iterative interference cancellation in X2T IRAT state */
#define TFW_IRAT_MAX_JDS_CELLS          8
/* Number of CX8 of latency for VPE processing when entering LTE gap from TDS */
#define TFW_IRAT_T2L_VPE_PROC_LATENCY_CX8          (864<<3)

/*! \brief Spreading factor for TX Physical Channels */
#define TFW_UL_SF_1                     0
#define TFW_UL_SF_2                     1  
#define TFW_UL_SF_4                     2  
#define TFW_UL_SF_8                     3  
#define TFW_UL_SF_16                    4  

/*! \brief TFW UL DPCH related constant */
#define TFW_UL_DPCH_TIME_ADV_CTRL_ENABLE    1
#define TFW_UL_DPCH_TIME_ADV_CTRL_DISABLE   0
#define TFW_MAX_UL_CCTRCH                   2
#define TFW_MAX_UL_PHY_PER_SLOT             2
#define TFW_MAX_UL_SLOT                     5
#define TFW_MAX_UL_CCTRCH_NUM               2
/** Used in UL DPCH configuration. CCTrCH info presence indicator */
#define TFW_UL_CCTRCH_VALID                 1
#define TFW_UL_CCTRCH_INVALID               0

/*! \brief Number of SS/TPC bits in an OVSF Code Channel */
#define TFW_UL_SS_TPC_BITS_SIZE_0           0
#define TFW_UL_SS_TPC_BITS_SIZE_2           1
#define TFW_UL_SS_TPC_BITS_SIZE_4           2
#define TFW_UL_SS_TPC_BITS_SIZE_8           3
#define TFW_UL_SS_TPC_BITS_SIZE_16          4
#define TFW_UL_SS_TPC_BITS_SIZE_32          7

/*! \brief SS/TPC reports to SW */
#define TFW_SS_DOWN                         0
#define TFW_SS_UP                           1
#define TFW_SS_HOLD                         2
#define TFW_TPC_DOWN                        0
#define TFW_TPC_UP                          1
#define TFW_TPC_HOLD                        2

/*! \brief Number of TFCI bits in an OVSF Code Channel */
#define TFW_UL_TFCI_BITS_SIZE_0             0
#define TFW_UL_TFCI_BITS_SIZE_2             1
#define TFW_UL_TFCI_BITS_SIZE_4             2
#define TFW_UL_TFCI_BITS_SIZE_8             3
#define TFW_UL_TFCI_BITS_SIZE_16            4
#define TFW_UL_TFCI_BITS_SIZE_32            5

/*! \brief FPACH related constants */
#define TFW_UL_SYNC_NO_ACK                  0
#define TFW_UL_SYNC_ACK                     1
#define TFW_UL_SYNC_ABORT                   2

/*! \brief EUL DL channel related constants */
#define TFW_SCHLD_EHICH_CNT_MAX             4
#define TFW_UNSCHLD_EHICH_CNT_MAX           1
#define TFW_EHICH_CNT_MAX        (TFW_SCHLD_EHICH_CNT_MAX + TFW_UNSCHLD_EHICH_CNT_MAX)
#define TFW_EAGCH_CNT_MAX                   4
#define TFW_EHICH_SCRM_MSK_LEN_BIT          88
#define TFW_EHICH_SCRM_MSK_LEN_BYTE         ((TFW_EHICH_SCRM_MSK_LEN_BIT+8-1)/8)

/* EHICH_SYMBOLS format is changed from 16Q12 to 16Q10 */
/** Ack/Nack correlation threshold in 32S10 from 32S12 in
 *  Jolokia for performance improvement and avoid saturation */
#define TFW_EHICH_ACK_NACK_CORR_THRESH      (0xffffce00) // from (0xffffa083) 
#define TFW_EHICH_ORTHO_SEQ_LEN_BIT         80
#define TFW_EHICH_ORTHO_SEQ_LEN_BYTE        ((TFW_EHICH_ORTHO_SEQ_LEN_BIT+8-1)/8)
#define TFW_EHICH_SIG_SEQ_GRP_MAX           20
#define TFW_EHICH_NUM_SIG_PER_SEQ_GRP       4

/** Specific Gap Count that is invalid but used to indicate always closed
    loop operation */
/* This count is used by SW to indicate always closed loop operation*/
#define TFW_DEMBACK_HS_SICH_ALWAYS_CLOSED_LOOP_GAP_CNT    256

/*! \brief DL Power Control Related Constants. */
/** 3 for DPCH + 1 for HS-SCCH and 1 for E-AGCH*/
#define TFW_DL_TARGET_SIR_UPDATE_MAX        5
#define TFW_DL_TARGET_SIR_CCTRCH_0          0
#define TFW_DL_TARGET_SIR_CCTRCH_1          1
#define TFW_DL_TARGET_SIR_CCTRCH_2          2
#define TFW_DL_TARGET_SIR_HSSCCH            3
#define TFW_DL_TARGET_SIR_EAGCH             4

/*! \brief Number of RX AGC gain states */
#define TFW_RX_TX_MAX_NUM_RX_GAIN_STATES        4
/*! \brief Number of RX AGC gain state switch points. 
  NOTE: MUST BE <=4. Affects alignment of elements in tfw_rf_lna_swpts_t*/
#define TFW_RX_TX_MAX_NUM_RX_GAIN_SWPTS         (TFW_RX_TX_MAX_NUM_RX_GAIN_STATES - 1)
/*! \brief There are total of 12 T2T, inter-frequency buffers. */
#define TFW_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS    12
/*! \brief Permutation in switching point calculation. */
// XXX ioei - Remove MAX_NUM_RX_AGC_SWITCH_POINTS after successful integration
// XXX ioei - changes in tfw_rf_sm_intf.h are remove
#define TFW_RX_TX_MAX_NUM_RX_AGC_SWITCH_POINTS  12
/*! \brief Number of Rx SSBI entries. */
#define TFW_RX_TX_MAX_NUM_RX_SSBI_ENTRIES       35
/*! \brief Number of HDET SSBI read entries. */
#define TFW_RX_TX_MAX_NUM_HDET_SSBI_READ_ENTRIES  20
/*! \brief Number of Therm SSBI read entries. */
#define TFW_RX_TX_MAX_NUM_THERM_SSBI_READ_ENTRIES  20
/*! \brief */
#define TFW_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS   2
/*! \brief Number of UL time slots */
#define TFW_RX_TX_MAX_NUM_UL_SLOTS              6
/*! \brief */
#define TFW_RX_TX_MAX_NUM_TX_BUFFERS            2
/*! \brief Number of Tx SSBI entries */
#define TFW_RX_TX_MAX_NUM_TX_SSBI_ENTRIES       150
/*! \brief Number of TX AGC gain states */
#define TFW_RX_TX_MAX_NUM_TX_GAIN_STATES        4
/*! \brief Tx switching point combinations.*/
#define TFW_RX_TX_MAX_NUM_TX_AGC_SWITCH_POINTS  6
/*! \brief Number of SSBI addresses */
#define TFW_RX_TX_MAX_NUM_TX_SSBI_ADDRESSES     4
/*! \brief Number of MPR bins */
#define TFW_RX_TX_MAX_NUM_MPR_BINS              17
/*! \brief Number of steps per tx gain state */
#define TFW_RX_TX_MAX_NUM_STEPS_PER_GAIN_STATE  64
/*! \brief XXX - TBD */
#define TFW_RX_TX_NUM_PDET_SSBI                 10
/*! \brief */
#define TFW_RX_TX_MAX_NUM_PDET_READINGS         4
/*! \brief */
#define TFW_RX_TX_MAX_NUM_SLOW_CALI_RESULTS     1 
/*! \brief */
#define TFW_RX_TX_MAX_NUM_TEMP_READINGS         1
/*! \brief SSBI data size in bytes */
#define TFW_RX_TX_SSBI_DATA_SIZE_IN_BYTES       4
/*! \brief The following thermo definitions are TBD */
#define TFW_RX_TX_MAX_NUM_THERMO_SSBI_VALUES    1
#define TFW_RX_TX_MAX_NUM_THERMO_SSBI_BUSES     1
#define TFW_RX_TX_MAX_NUM_THERMO_SSBI_ADDRESSES 1
/*! \brief Definition for number of tx bamnds */
#define  TFW_RX_TX_MAX_NUM_TX_AGC_LUT_BUFFERS   2
/*! \brief Definition for number of SSBI bytes per UL txAgc slot */
#define  TFW_RX_TX_NUM_OF_SSBI_BYTES_PER_SLOT   4   
/*! \brief Definition for number of RFFE bytes for APT update */
#define  TFW_RX_TX_NUM_OF_RFFE_BYTES_APT        5   
/*! \brief Definition for GRFC attribute */
#define TFW_GRFC_ENGINE_BASE                           0x100
/*! \brief Definition for RF GRFC attribute */
#define TFW_GRFC_ENGINE_RF_BASE                        0x200
/*! \brief Definition for RF GRFC ID mask */
#define TFW_GRFC_ID_MASK                               0xff   
/*! \brief Definition for Invalid GRFC ID */
#define TFW_GRFC_ID_INVALID                            0x7fff    
       
/*! \brief Number of GRFC entries for antSel arrays */ 
#define TFW_RX_TX_MAX_NUM_GRFC_ANT_SEL_ENTRIES 20 

/*! \brief DL flow control RBTS CAP table size*/
#define TFW_RTBS_CAP_TABLE_SIZE                 6
       


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


#endif /* TFW_CONSTANT_H */
