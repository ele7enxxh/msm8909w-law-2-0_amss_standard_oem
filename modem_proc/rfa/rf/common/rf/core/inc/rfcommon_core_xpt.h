#ifndef RFCOMMON_CORE_XPT_H
#define RFCOMMON_CORE_XPT_H

/*!
  @file
  Definitions of types pertaining to EPT/ET/XPT.

  @details
  This file exports the definitions of types that are used in the RF Driver
  relating to EPT/ET/XPT.
*/

/*==============================================================================

Copyright (c) 2012 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

==============================================================================*/

/*==============================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_core_xpt.h#1 $

when       who     what, where, why
--------------------------------------------------------------------------------
02/10/15   cd      TxAGC updates for max power override support
07/21/14   aro/kai Added expansion ratio param in DPD expansion function
07/21/14   cd      Add predictive clipper support for 1x, DO rev0, DO revA
07/10/14   cd/cri  Merge Pin Cal
06/16/14   vs/kai  Non-0dB Pin SW implementation 
04/24/14   zhw     Use rfcommon_mdsp_xpt_mode_enum_type for all DPD loading
04/24/14   cri     wmore clipper enablement and nv support
01/30/14   jmf     Support for FBRx Type XPT IQ caps for FBRx Internal Cal
01/06/14   cd      Linearizer and Tx AGC updates for loading xPT information to 
                   RFLM
11/22/13   id      Load DPD changes for BOLT
11/14/13   cd      Support for RFLM-based Tx AGC override
10/25/13   id      Added initial support for Load DPD
08/30/13    pv     Optimization of Delay vs Temp vs Freq offset for XPT.
08/27/13   aka     1x and WCDMA updates
08/08/13   sl/cri  Common implementation for DPD scaling
07/23/13   aka     Self-test and FFT related types
07/17/13   jmf     [xPT] New Delay Table separate from DPD data module
07/10/13    pv     Support for Delay vs Temp vs Freq offset for XPT.
05/13/13   Saul    XPT. Support EPT_ET_CFG.
04/22/13   jmf     Implement xPT processing Enum Type
03/14/13   kai     Read DPD processing params from DPD_Config NV
02/27/13   Saul    CDMA. ET now uses separate envelope scale from EPT.
02/22/13   cri     Load DPD tables based on tx chain
01/22/13   aro     Added common XPT capture type
12/10/12   cri     Load DDP from current linearizer
12/05/12   Saul    Common XPT. Process DPD corresponding to relevant linearizer.
11/15/12   Saul    Common XPT. Update to populate dpd API.
11/13/12   Saul    Common XPT. DPD expansion framework.
11/13/12   Saul    Common XPT. Reverted changes made to commit_dpd_data.
11/08/12   cd      Commit DPD will send FW override to latch the tables
10/26/12   Saul    Common XPT. Added num elements check.
10/22/12   Saul    Common XPT. Use bank index/offset from saved DPD data.
10/19/12   Saul    COMMON XPT. Partitioned dpd data per tx path.
10/12/12   Saul    Common XPT. Load DPD framework.
08/31/12   cri     Added DPD NV support 
08/22/12   cri     EPT support for CDMA FW interfaces 
08/21/12   Saul    MultiLin. Added support to load EPT V2 and ET NV Data.
08/17/12  cri/Saul MultiLin. Added utility APIs to each tech to modify dpd_data.
08/03/12   Saul    CDMA. RX/TXLM sample capture support.
07/12/12   cri     CDMA interface changes for EPT
06/14/12   aro     Compiler error fix
06/13/12   aro     Implemented DPD coefficient interface
06/04/12   cri     Initial Release

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfcommon_core_txlin_types.h"
#include "rfcommon_core_txlin.h"
#include "rfcommon_nv.h"
#include "rfcommon_mdsp_types.h"
#include "rfm_gain_state_types.h"
#ifdef FEATURE_RF_HAS_DPD
#error code not present
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/* EPT Configuration parameters */
#define MAX_XPT_NUM_CONFIG_PARAMS       32
#define MAX_EPT_NUM_CONFIG_PARAMS       MAX_XPT_NUM_CONFIG_PARAMS
#define XPT_CFG                         0
#define EPT_CFG_CMN_EXP_SCALE           1
#define EPT_CFG_CMN_SEARCH_CENTER       2
#define EPT_CFG_CMN_SEARCH_WIDTH        3
#define EPT_CFG_CMN_EXP_SCALE_Q_FACT    4
#define EPT_CFG_CMN_LIN_TX_MAG          5
#define EPT_CFG_CMN_PM_ORDER            6
#define EPT_CFG_CMN_EVN_SCALE_UNITY     7
#define EPT_CFG_CMN_IQ_GAIN_UNITY       8
#define XPT_CFG_LTE_CLIP_THR_MPR0       9
#define XPT_CFG_CDMA_CLIP_THR_1X        9
#define XPT_CFG_LTE_CLIP_THR_MPR1       10
#define XPT_CFG_CDMA_CLIP_THR_DOR0      10
#define XPT_CFG_LTE_CLIP_THR_EN_LVL     11
#define XPT_CFG_CDMA_CLIP_THR_DORA      11
#define XPT_CFG_CMN_REF_ET_PIN_OFFSET   13
#define XPT_CFG_CMN_REF_EPT_PIN_OFFSET  14
/* Index 12 & 15 current unused */
#define DPD_CFG_EXTRAP_BIN              16
#define DPD_CFG_FIRST_BIN               17
#define DPD_CFG_MIN_BIN_COUNT           18
#define DPD_CFG_MIN_FIRST_BIN_COUNT     19
#define DPD_CFG_MIN_LAST_BIN_COUNT      20
#define DPD_CFG_RX_COMPACT              21
#define DPD_CFG_SPEC_INV                22
#define ET_CFG_CMN_ENV_SCALE_UNITY      23 
#define XPT_CFG_CMN_MAX_PWR_OVR_ALGO    26
#define XPT_CFG_CMN_MAX_PWR_OVR_VAL     27
#define XPT_PATH_DELAY_ARRAY_SIZ        16
#define XPT_TEMP_BIN_SIZ                 8

#define DPD_SCALING_Q_FACTOR            12
/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enumeration indicating the XPT capture type
*/
typedef enum
{
  XPT_CAPTURE_EPT, /*!< Enhanced Power Tracking */

  XPT_CAPTURE_ET,  /*!< Envelop Tracking */

  XPT_CAPTURE_DELAY, /*!< Delay */

  XPT_CAPTURE_SELF_TEST_EVM = 3,   /*!< Self Test, keeping here to maintain order */

  // XPT_CAPTURE_IQ_RMS,           /*!< Self Test I/Q RMS capture only */

  XPT_CAPTURE_PIN, /*!< Pin */
  //XPT_CAPTURE_IQ_RMS (5) need to be ported from Dime

  XPT_CAPTURE_FBRX = 6,

  XPT_CAPTURE_SELF_TEST_VSWR = 7,    /*!< Self Test */

  XPT_CAPTURE_SELF_TEST_RX_RMS = 8, /*!< Self Test */

  XPT_CAPTURE_INVALID /*!< Invalid capture */

} rfcommon_xpt_capture_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enumeration indicating the XPT delay capture 
  signal bandwidth to differentiate LTE BWs
*/
typedef enum
{
  RF_COMMON_XPT_DELAY_CAP_INVALID = -1,
  RF_COMMON_XPT_DELAY_CAP_NONLTE,
  RF_COMMON_XPT_DELAY_CAP_LTE_BW_5,
  RF_COMMON_XPT_DELAY_CAP_LTE_BW_10,
  RF_COMMON_XPT_DELAY_CAP_LTE_BW_20
}rf_common_xpt_delay_capture_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enumeration indicating the XPT Pin capture 
*/
typedef enum
{
  RF_COMMON_XPT_PIN_CAP_INVALID = -1,
  RF_COMMON_XPT_PIN_CAP_ALL_TECHS
}rf_common_xpt_pin_capture_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enumeration indicating the XPT processing type
*/
typedef enum
{
  PROC_NONE, /*!< No processing */

  PROC_AM_PM_CURVE,  /*!< calculate AMAM-AMPM */

  PROC_DPD, /*!< calculate AMAM-AMPM and construct dpd */

  PROC_SELF_TEST /*!< Self test related calculations*/

} rfcommon_xpt_processing_type;

/*----------------------------------------------------------------------------*/
/* XPT conifguration - APT/EPT/ET */
typedef enum
{
  APT_CFG,
  EPT_CFG,
  ET_CFG,
  EPT_ET_CFG,
  XPT_CFG_TYPE_MAX
} xpt_cfg_type;

typedef enum
{
  LOAD_DPD_TYPE_EPT,
  LOAD_DPD_TYPE_ET,
  LOAD_DPD_TYPE_NONE
} load_dpd_type;

#define DPD_TYPE_AMAM_EXPANDED_TBL_SIZE    64
#define DPD_TYPE_AMPM_EXPANDED_TBL_SIZE    64
#define DPD_TYPE_EPDT_EXPANDED_TBL_SIZE    128

typedef enum
{
  DPD_TYPE_AMAM,
  DPD_TYPE_AMPM,
  DPD_TYPE_EPDT,
  DPD_TYPE_GENERIC,
  DPD_TYPE_NONE

}load_dpd_input_table_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  dpd_poly_coeff_type dpd_coeffs;
  uint8 amam_coeff_num;
  uint8 ampm_coeff_num;
  uint8 phase_shift_num;
}rfcommon_ept_coefficient_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold Common XPT DPD Bank Info
*/
typedef struct
{
  uint8 bank_id;
  /*!< Bank ID */
  uint16 bank_offset;
  /*!< The offset within bank representing the start of the dpd data */
} rfcommon_xpt_dpd_bank_info_type;

typedef enum
{
  XPT_DPD_SCALING_REV_INVALID,
  XPT_DPD_SCALING_REV_1
} XPT_DPD_SCALING_REV_TYPE;

typedef enum
{
  XPT_DPD_ACTION_ISC     = 0, //Init/Save/Commit
  XPT_DPD_ACTION_INIT    = 1, //INIT
  XPT_DPD_ACTION_SAVE    = 2, //SAVE
  XPT_DPD_ACTION_COMMIT  = 3, //COMMIT
  XPT_DPD_ACTION_NUM

} xpt_dpd_action_type;

/*----------------------------------------------------------------------------*/
/*! Enumeration for XPT Tx override AGC Sequence Type */
typedef enum
{

  RFCOMMON_XPT_TX_AGC_PA_L_2_H_APT_2_APT,
  /*!< PA change from Low PA(APT) to High PA(APT) */

  RFCOMMON_XPT_TX_AGC_PA_L_2_H_APT_2_EPT,
  /*!< PA change from Low PA(APT) to High PA(EPT) */

  RFCOMMON_XPT_TX_AGC_PA_L_2_H_APT_2_ET,
  /*!< PA change from Low PA(APT) to High PA(ET) */

  RFCOMMON_XPT_TX_AGC_PA_L_2_H_EPT_2_ET,
  /*!< PA change from Low PA(EPT) to High PA(ET) */

  RFCOMMON_XPT_TX_AGC_PA_H_2_L_APT_2_APT,
  /*!< PA change from High PA(APT) to Low PA(APT) */

  RFCOMMON_XPT_TX_AGC_PA_H_2_L_EPT_2_APT,
  /*!< PA change from High PA(EPT) to Low PA(APT) */

  RFCOMMON_XPT_TX_AGC_PA_H_2_L_ET_2_APT,
  /*!< PA change from High PA(ET) to Low PA(APT) */

  RFCOMMON_XPT_TX_AGC_PA_H_2_L_ET_2_EPT,
  /*!< PA change from High PA(ET) to Low PA(EPT) */

  RFCOMMON_XPT_TX_AGC_AVGA_L_2_H,
  /*!< No PA change, avga change for low pwr to high pwr */

  RFCOMMON_XPT_TX_AGC_AVGA_H_2_L,
  /*!< No PA change, avga change for High pwr to Low pwr */

  RFCOMMON_XPT_TX_AGC_OVR_SEQ_NUM,

  RFCOMMON_XPT_TX_AGC_OVR_SEQ_INVALID = -1,
  /*!< Invalid Tx AGC Override sequence */

} rfcommon_core_xpt_tx_agc_ovr_seq_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold Common XPT DPD Scaling Info
*/
typedef struct
{
  /*!< Indicates data revision type contained */
  XPT_DPD_SCALING_REV_TYPE rev;
  int16 scaling_data[RFCOM_TXLIN_NUM_CHANNELS];
} xpt_dpd_scaling_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold Common XPT DPD Scaling Value
*/
typedef struct
{
  /*!< Indicates a valid shift */
  boolean valid;
  int32 scale_value;
} xpt_dpd_scaling_value_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Common XPT Tx - Override Param information
 
  @details
  Provides an interface structure to hold all the override param information
  requred for Tx AGC override
 
*/
typedef struct
{
  /*! 
    @brief
    RFLM handle for baseband Tx overrides
  */
  uint32 rflm_handle;
  /*! 
    @brief
    Tx Modem chain
  */
  uint8 modem_chain;
  /*! 
    @brief
    Tx AGC sequence override type
  */
  rfcommon_core_xpt_tx_agc_ovr_seq_type txagc_seq;
  /*! 
    @brief
    XPT mode for Tx override
  */
  rfcommon_mdsp_xpt_mode_enum_type xpt_mode;
  /*! 
    @brief
    IQ gain value for Tx override
  */
  uint32 iq_gain;
  /*! 
    @brief
    IQ gain action type for Tx override
  */
  uint8 iq_gain_action_type;
  /*! 
    @brief
    Envelope scale value for Tx override
  */
  uint32 env_scale;
  /*! 
    @brief
    Envelope scale action type for Tx override
  */
  uint8 env_scale_action_type;
  /*! 
    @brief
    ET Delay value in nsx10 for Tx override
  */
  int32 et_delay;
  /*! 
    @brief
    Delay action type for Tx override
  */
  uint8 delay_action_type;
  /*! 
    @brief
    AMAM table information for Tx override
  */
  rfcommon_mdsp_dpd_table_info_t amam_addr;
  /*! 
    @brief
    AMPM table information for Tx override
  */
  rfcommon_mdsp_dpd_table_info_t ampm_addr;
  /*! 
    @brief
    EPDT table information for Tx override
  */
  rfcommon_mdsp_dpd_table_info_t detrough_addr;
  /*! 
    @brief
    Latch immediate timing information used for baseband latch
  */
  boolean latch_immediate;
  /*! 
    @brief
    Tech specific sample count information to control baseband latch
  */
  uint32 max_sample_count;
  /*! 
    @brief
    WTR RGI value for override
  */
  uint16 rgi;
  /*! 
    @brief
    PAPM bias value for APT override
  */
  uint16 papm_bias;
  /*! 
    @brief
    PA state for Tx override
  */
  rfm_pa_gain_state_type pa_state;
  /*! 
    @brief
    Tech-specific PA range for Tx override
  */
  rfm_pa_range_type  pa_range;
  /*! 
    @brief
    PA ICQ value for Tx override
  */
  uint16 pa_curr;

} rfcommon_core_xpt_tx_override_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Common XPT Tx - Tx AGC configuration information
 
  @details
  Provides an interface structure to hold all xPT specific information
  required for common TxAGC configuration
*/
typedef struct
{
  /*!
    @brief
    AMAM bank information for Tx AGC
  */
  rfcommon_xpt_dpd_bank_info_type amam_bank_info[RFCOM_TXLIN_NUM_SEGMENTS];
  /*!
    @brief
    AMPM bank information for Tx AGC
  */
  rfcommon_xpt_dpd_bank_info_type ampm_bank_info[RFCOM_TXLIN_NUM_SEGMENTS];
  /*!
    @brief
    EPDT bank information for Tx AGC
  */
  rfcommon_xpt_dpd_bank_info_type epdt_bank_info[RFCOM_TXLIN_NUM_SEGMENTS];

} rfcommon_core_xpt_tx_agc_info_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the expansion ratios to be used for AMAM, AMPM, and 
EPDT */
typedef struct
{
  uint16 amam; /*!< Expansopn ratio for AMAM */
  uint16 ampm; /*!< Expansopn ratio for AMPM */
  uint16 epdt; /*!< Expansopn ratio for EPDT */
} xpt_expansion_ratio_type;

/*----------------------------------------------------------------------------*/
boolean 
rfcommon_core_populate_tx_dpd
(
  rfm_device_enum_type device,
  tx_linearizer_table_data_type * tx_final_linearizer_data,
  rfcommon_xpt_dpd_bank_info_type* amam_bank_info,
  rfcommon_xpt_dpd_bank_info_type* ampm_bank_info,
  rfcommon_xpt_dpd_bank_info_type* epdt_bank_info,
  uint16* num_rgi_with_dpd_info,
  xpt_dpd_scaling_value_type *dpd_scale_factor,
  xpt_expansion_ratio_type *expansion_ratio_p
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_core_txlin_temp_comp_update_dpd
(
  tx_linearizer_table_type *tx_lut,
  tx_linearizer_table_type *temp_comp_lut,
  tx_linearizer_table_type *first_sweep_lut,
  tx_linearizer_aux_data_type aux_data,
  int16 temp_comp_val,
  uint8 valid_lut_entries
);

/*----------------------------------------------------------------------------*/
boolean
rf_common_xpt_init_dpd
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_common_xpt_save_dpd_data
(
  rfm_device_enum_type device,
  uint16* dpd_tbl_ptr,
  uint16  dpd_tbl_sz,
  load_dpd_input_table_type tbl_type,
  uint8* bank_index,
  uint16* bank_offset
);

/*----------------------------------------------------------------------------*/
boolean
rf_common_xpt_commit_dpd_data
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/

boolean
rf_common_xpt_expand_dpd_data(
   load_dpd_input_table_type dpd_tbl_type,
   uint16 num_dpd_coeffs,
   uint16* dpd_coeffs,
   uint16 expansn_ratio,
   uint16* dpd_tbl,
   xpt_dpd_scaling_value_type *dpd_scale_factor
);

/*----------------------------------------------------------------------------*/

boolean 
rf_common_xpt_delay_offset_computation
( 
 rfnv_tx_delay_offsets_type *offset_nv_ptr,
 const uint16 *therm_bin_array,
 uint16 current_temp_adc, 
 uint16 calibration_temp_adc,
 uint32 current_tx_freq,
 boolean calibration_temp_based_realignment,
 int16 *final_delay_offset,
 rfcommon_core_convert_freq_handler_type convert_freq_handler
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_xpt_load_dpd_scaling_nv
(
  rfnv_item_id_enum_type item_code,
  xpt_dpd_scaling_data_type *xpt_dpd_scaling_data,
  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
boolean 
rf_common_xpt_dpd_scale_computation
(
   xpt_dpd_scaling_data_type *scaling_nv_ptr,
   uint32 *tx_char_freq,
   uint8 tx_char_chan_size,
   uint32 current_tx_freq,
   xpt_dpd_scaling_value_type *final_dpd_scale_factor
);
boolean 
rf_common_xpt_get_delay_offset_array_for_current_temp
( 
  rfnv_tx_delay_offsets_type *offset_nv_ptr,
  const uint16 *therm_bin_array,
  uint16 current_temp_adc,
  int16 *temp_delay_vs_temp_vs_freq
);
/*----------------------------------------------------------------------------*/
boolean 
rf_common_xpt_calibration_based_delay_offset_realignment
( 
  rfnv_tx_delay_offsets_type *offset_nv_ptr,
  const uint16 *therm_bin_array,
  uint16 calibration_temp_adc
);
/*----------------------------------------------------------------------------*/
boolean 
rf_common_xpt_delay_offset_computation_v2
( 
  rfnv_tx_delay_offsets_type *offset_nv_ptr,
  int16 *temp_delay_vs_temp_vs_freq,
  uint32 current_tx_freq,
  int16 *final_delay_offset,
  rfcommon_core_convert_freq_handler_type convert_freq_handler
);

/*----------------------------------------------------------------------------*/
boolean
rf_common_xpt_load_dpd_tbls
(
  uint8 device,
  uint16* dpd_tbl_ptr,
  uint16  dpd_tbl_sz,
  load_dpd_input_table_type tbl_type,
  uint8* bank_index,
  uint16* bank_offset,
  xpt_dpd_action_type action_type,
  rfcommon_mdsp_xpt_mode_enum_type xpt_mode
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_xpt_load_linearizer_for_override
(
  rfm_mode_enum_type rfm_mode,
  rfm_device_enum_type device,
  rfcom_band_type_u rfm_band,
  rfcommon_core_xpt_tx_override_type* tx_ovr_params
);

#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_CORE_XPT_H */
