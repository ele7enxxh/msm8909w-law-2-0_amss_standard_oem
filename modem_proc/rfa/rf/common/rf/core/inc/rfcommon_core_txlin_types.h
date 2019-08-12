#ifndef RFCOMMON_CORE_TXLIN_TYPES_H
#define RFCOMMON_CORE_TXLIN_TYPES_H

/*!
  @file
  rfcommon_core_txlin_types.h

  @details
  This file exports the definitions and declartions to be used for Tx linearizers.
*/

/*==============================================================================

  Copyright (c) 2012 - 2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_core_txlin_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/16/16   cv      [port from TH]Converted common linearizer channel to 32 bits
03/03/16   as      WTR special writes support for 4FMOD performance improvement
09/17/15   kma     Added auto pin support for Pin cal
03/03/15   cd      Fix ILPC issue by enabling capping algorithm only if capped 
                   power is found for the PA state
02/10/15   cd      TxAGC updates for max power override support
11/24/14   kai     Added maximum calibrated PA state into log data
07/31/14   ndb     Added support for extended EARFCN for LTE
07/25/14   cd      Add Tx linearizer data logging support and provide interface
                   for tech to query
07/10/14   cd/cri  Merge Pin Cal
07/02/14   kr/kai  Non-0dB Pin SW implementation to support -ve pin offsets values
06/16/14   vs/kai  Non-0dB Pin SW implementation 
04/14/14   cd      Support for 16-bit 2 PA ICQ bias register configuration
03/05/14   cd      Support for loading freq comp index and full bias lin size to 
                   RFLM
02/13/14   cd      Fix index to const bias table mapping for init linearizer
11/22/13   cd      Load XPT table info to RFLM
11/20/13   cd      Added separate API to populate linearizer data per row - 
                   shared by online Tx AGC and Tx override
11/14/13   cd      Support for RFLM-based Tx AGC override
10/05/13   cd      Allow more PA/PAPM transactions because of unoptimized device
                   queries
09/30/13   cd      Add support to load common TxAGC event information to RFLM DM
09/26/13   cd      Common AGC. Renamed QPOET to PAPM
09/17/13   cd      Common AGC. Config AGC and linearizer interface check-in
05/16/13   cd      Temp Comp - Fixed scenario where both legacy and new Temp 
                   comp NVs are present in the phone
05/10/13   cd      Temp Comp - Normalize Pin and Pout values differently 
                   w.r.t frequency
05/08/13   rsr     Added Therm ADC  reading during calibration to Multilin V3 NV
04/29/13   cd      Apply Pin alignment during calibration
04/27/13   cd      Temp Comp - Added changes for characterized channel list 
                   and computing ref linearizer frequency
04/22/13   cd      Temp Comp - support for multi-chan linearizer compensation
                   - Multi-algorithm support based on NV versioning
04/04/13   cd      XPT Temp Comp, add initial support for multi-chan 
                   calibrated linearizer
03/21/13   Saul    MultiLin. Hooked up V3 NV PA current to mission mode struct.
03/20/13   Saul    MultiLin. Each RGI now has info for which freqcomp to use.  
03/18/13   cd      Updated pin/pout value calculation algorithm to use 
                   reference channel information
03/05/13   cd      Added common functionality for xPT temp and freq comp
02/22/13   Saul    Multilin. Added dpd_type to linearizer.
12/06/12   rsr     Change xpt_am_row_type and xpt_pm_row_type to 16 bit variables.
12/05/12   Saul    MultiLin. Process XPT DPD flat list so that each PA state 
                   ultimately contains a table of its corresponding DPDs.
11/13/12   Saul    MultiLin. Added num_dpd used by linearizer.
11/12/12   Saul    MultiLin. Removed dpd_data from lin table type.
11/08/12   Saul    MultiLin. Restructured EPT_V2 and ET DPDs.
11/05/12   dw      Revised APT structure
11/05/12   dw      Change MultiLin V1 data structure to support quiescent current control in APT
10/31/12   Saul    MultiLin. Moved iq_offset to linearizer data type
10/26/12   Saul    MultiLin. Changed typ ID size to two bytes.
10/24/12   Saul    MultiLin. Added variant_marker_type
08/27/12   Saul    MultiLin. AM/PM/Detrough now dynamically allocated per RGI.
08/23/12   Saul    MultiLin. Added PA current and iq offset NV load support.
08/21/12   Saul    MultiLin. Added support to load EPT V2 and ET NV Data.
08/17/12   Saul    Initial Release. Created to fix compile errors due to 
                   recursive includes in files using these defines.

==============================================================================*/

#include "comdef.h"
#include "rfm_gain_state_types.h"
#include "rfdevice_papm_intf.h"
#include "rfcommon_mdsp_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/*=============================================================================
                           CONSTANT DEFINITIONS
=============================================================================*/
#define XPT_DPD_NUM_COEFF 16
#define RFCOM_TXLIN_NUM_LINEARIZERS       8 
#define RFCOM_TXLIN_NUM_PA_STATES         8
#define RFCOM_TXLIN_NUM_SEGMENTS         64
#define RFCOM_TXLIN_NUM_CHANNELS         16
#define RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN  8
#define RFCOM_TXLIN_MAX_RGI_INDEX       128
#define RFCOM_TXLIN_DEFAULT_FREQ_INDEX    7
#define RFCOM_TXLIN_NV_SIZE_MAX       99880
#define RFCOM_TXLIN_NV_V2_SIZE_MAX   166440
#define RFCOM_TXLIN_INVALID_RGI      0xFFFF

/* Number of waveforms changed for Multi-lin V2. 
   Need to support V1 NV structure that only has 3 */
#define RFCOM_TXLIN_NUM_V1_WAVEFORMS      3
#define RFCOM_TXLIN_NUM_V2_WAVEFORMS      4

/* Number of freq comp sweeps updated for Multi-lin V2 */
#define RFCOM_TXLIN_NUM_V2_FCOMP_SWEEPS   2
#define TX_FREQCOMP_FB                    0
#define TX_FREQCOMP_APT                   1

/* Common Linearizer Compensation definitions */
#define RFCOM_TXLIN_TEMP_BIN_SIZE         8
#define RFCOM_TXLIN_FREQ_BIN_RESOLUTION   256

/* Pin/Pout compensation NV definitions */
#define RFCOM_TXLIN_PWR_COMP_REF_FREQ_INDEX   7
#define RFCOM_TXLIN_PWR_COMP_REF_TEMP_INDEX   4
/* IQ gain compensation limited to 6.3 dB to account 
for 6-bit WCDMA limitation */
#define RFCOM_TXLIN_PIN_COMP_IQ_GAIN_MAX_ADJUST  63

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Numer of transactions supported for PAPM data task
*/
#define RFCMN_TXLIN_AGC_PAPM_DATA_TASK_NUM_TRANS                              8
/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Numer of transactions supported for PAPM trigger task
*/
#define RFCMN_TXLIN_AGC_PAPM_TRIG_TASK_NUM_TRANS                              4
/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Numer of transactions supported for PA data and trigger task combined
*/
#define RFCMN_TXLIN_AGC_PA_TASK_NUM_TRANS                                     8
/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Numer of transactions supported for WXE BBF task
*/
#define RFCMN_TXLIN_AGC_WXE_BBF_TASK_NUM_TRANS                                1
/*----------------------------------------------------------------------------*
*! 
    @brief
    Numer of transactions supported for WXE 4FMOD SPECIAL REGISER task
*/
#define RFCMN_TXLIN_AGC_WXE_4FMOD_TASK_NUM_TRANS                           1

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Numer of transactions supported for WXE FBRx task
*/
#define RFCMN_TXLIN_AGC_WXE_FBRX_TASK_NUM_TRANS                               1
/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Numer of transactions supported for WTR data task
*/
#define RFCMN_TXLIN_AGC_WTR_DATA_TASK_NUM_TRANS                               4
/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Numer of transactions supported for WTR trigger task
*/
#define RFCMN_TXLIN_AGC_WTR_TRIG_TASK_NUM_TRANS                               1
/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Min Tx power across techs in dBm10: -60 dBm
*/
#define RFCMN_TXLIN_AGC_MIN_TX_PWR_DBM10                                   -700

/* Store the FB Rx gain state thresholds in dBm10 */
typedef struct
{
  int16 low_to_med_thresh;
  int16 med_to_high_thresh;
} rfcommon_txlin_fbrx_gain_state_thresh_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common Tx AGC Sequence definition
 
  @details
  This enum provides the exact sequence of tasks that will be provided to
  RFLM Tx AGC. The sequence is predetermined, and can be changed via this
  enum only
*/
typedef enum
{
  RFCMN_TXLIN_AGC_PAPM_DATA_TASK = 0,

  RFCMN_TXLIN_AGC_PAPM_EARLY_TRIG_TASK = 1,

  RFCMN_TXLIN_AGC_PA_TASK = 2,

  RFCMN_TXLIN_AGC_PAPM_LATE_TRIG_TASK = 3,

  RFCMN_TXLIN_AGC_WXE_BBF_TASK = 4,

  RFCMN_TXLIN_AGC_WXE_FBRX_TASK = 5,

  RFCMN_TXLIN_AGC_WTR_GW_TASK = 6,

  RFCMN_TXLIN_AGC_WTR_EARLY_TRIG_TASK = 7,

  RFCMN_TXLIN_AGC_WTR_LATE_TRIG_TASK = 8,

  RFCMN_TXLIN_AGC_MAX_TASK = 9,/*!< Number of TxAGC tasks */

  RFCMN_TXLIN_AGC_NO_ACTION_TASK = 255

} rfcommon_core_txlin_agc_sequence_type;

/*! @brief 
  Tx waveform type. This for voice/data dependent linearizers
  */
typedef enum
{
  TX_LINEARIZER_VOICE,
  TX_LINEARIZER_DATA_TYPE_1,
  TX_LINEARIZER_DATA_TYPE_2,
  TX_LINEARIZER_FIRST_SWEEP,
  TX_LINEARIZER_NUM_WAVEFORM,
}tx_linearizer_waveform_type;

/*! @brief 
  Tx Multi-lin revision. This is to support APT/EPT/ET linearizer data
  */
typedef enum
{
  TX_MULTI_LIN_REV_1,
  //TX_MULTI_LIN_REV_2, OBSOLETE/NOT SUPPORTED
  TX_MULTI_LIN_REV_3 = 2,
  TX_MULTI_LIN_NUM_REVS,
  TX_MULTI_LIN_INVALID
}tx_multi_lin_rev_type;

/*! @brief 
  This lets us know if we have DPD data present for a given linearizer
  */
typedef enum
{
  TX_LINEARIZER_NO_AUX_DATA,
  TX_LINEARIZER_EPT_DPD_AUX_DATA,
  TX_LINEARIZER_EPT_DPD_V2_DATA,
  TX_LINEARIZER_ET_DPD_DATA,
  TX_LINEARIZER_DPD_INVALID
}tx_linearizer_aux_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Data structure type to store legacy Lin vs Temp NV data
*/
typedef struct rfcommon_core_txlin_vs_temp_legacy_type_s
{
  /*! 
    @brief
    Tx linearizer vs temperature slope, at the points
    specified by the Therm Bins NV item.
  */
  int16 slope[RFCOM_TXLIN_TEMP_BIN_SIZE];

  /*! 
    @brief
    Tx linearizer vs temperature offset, at the points
    specified by the Therm Bins NV item.
  */
  int16 offset[RFCOM_TXLIN_TEMP_BIN_SIZE];

  /*! 
    @brief
    Tx linearizer vs Temp vs Freq characterization, used by WCDMA. This
    stores the delta across frequency relative to the temp offsets stored
    above
  */
  int16 temp_vs_freq_delta[RFCOM_TXLIN_TEMP_BIN_SIZE]
                          [RFCOM_TXLIN_NUM_CHANNELS];

}rfcommon_core_txlin_vs_temp_legacy_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Data structure type to store Lin vs Temp vs Freq Pin and Pout NV data
*/
typedef struct rfcommon_core_txlin_vs_temp_vs_freq_type_s
{
  /*! 
    @brief
    Provides characterized NV data for linearizer vs temperature vs 
    frequency Pin compensation

    @details
    Holds the 2-dimensional characterized representation of linearizer 
    compensation, varying across temperature on the X-axis and
    frequency on the Y-axis. The characterization is provided for upto
    16 frequencies of interest across the band, for every temperature
    bin provided by NV_THERM_BINS (upto 8). Values are in dBx10 and
    are stored per PA state
  */
  int16 pin[RFCOM_TXLIN_NUM_LINEARIZERS]
           [RFCOM_TXLIN_TEMP_BIN_SIZE]
           [RFCOM_TXLIN_NUM_CHANNELS];

  /*! 
    @brief
    Provides characterized NV data for linearizer vs temperature vs 
    frequency Pout compensation

    @details
    Holds the 2-dimensional characterized representation of linearizer 
    compensation, varying across temperature on the X-axis and
    frequency on the Y-axis. The characterization is provided for upto
    16 frequencies of interest across the band, for every temperature
    bin provided by NV_THERM_BINS (upto 8). Values are in dBx10 and
    are stored per PA state. These compensation values are relative to 
    the calibrated Pout vs. frequency offset
  */
  int16 pout[RFCOM_TXLIN_NUM_LINEARIZERS]
            [RFCOM_TXLIN_TEMP_BIN_SIZE]
            [RFCOM_TXLIN_NUM_CHANNELS];

}rfcommon_core_txlin_vs_temp_vs_freq_type_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Data structure type to store Pin Cal vs Freq  NV data
*/
typedef struct rfcommon_core_txlin_pin_cal_vs_freq_type_s
{
  boolean has_pin_freq_comp_cal_nv[RFCOM_TXLIN_NUM_LINEARIZERS];
  /*!< Store whether new Pin Freq Comp Calibration NV is available or not for each PA state*/

  /*! 
    @brief
    Provides NV data for Calibrated Pin Compensation vs frequency

    @details
    Holds the 1-dimensional representation of calibrated pin
    compensation frequency on the X-axis. The characterization is provided for upto
    16 frequencies of interest across the band. Values are in dBx10 and are stored per PA state.
  */
  int16 pin[RFCOM_TXLIN_NUM_LINEARIZERS]
           [RFCOM_TXLIN_NUM_CHANNELS];
}rfcommon_core_txlin_pin_cal_vs_freq_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief 
  Enum to define the algorithms supported in software for temperature 
  compensation, based on multilin NV version and Temp Comp static NV version
*/
typedef enum
{
  RFCOM_TXLIN_TEMP_COMP_ALGO_MULTILIN_V1 = 0,

  RFCOM_TXLIN_TEMP_COMP_ALGO_MULTILIN_V3_LEGACY_NV = 1,

  RFCOM_TXLIN_TEMP_COMP_ALGO_MULTILIN_V3 = 2,

  RFCOM_TXLIN_TEMP_COMP_ALGO_INVALID

}rfcom_txlin_temp_comp_algo_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief 
  Enum to define the normalization algorithm used when applying characterized
  Pin/Pout vs temp vs Freq offsets
*/
typedef enum
{
  RFCOM_TXLIN_NORMALIZE_TO_CURR_FREQ_REF_TEMP = 0,

  RFCOM_TXLIN_NORMALIZE_TO_REF_FREQ_REF_TEMP = 1,

  RFCOM_TXLIN_NORMALIZE_INVALID

}rfcom_txlin_temp_vs_freq_normalization_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief 
  Data structure to store temp comp NV data from legacy as well as updated
  Pin and Pout NV items (across freq)
 
  @details
  SW support is provided for legacy as well as new NV items, but the
  interpretation of these items varies 
 
*/
typedef struct 
{
  boolean is_legacy_temp_comp_nv;
  /*!< Store whether we're using legacy temp comp NV or not */

  boolean has_pin_pout_temp_comp_nv;
  /*!< Store whether new Pin/Pout temp comp NV is also available or not */

  rfcom_lte_earfcn_type tx_char_chan[RFCOM_TXLIN_NUM_CHANNELS];
  /*!< Temp vs freq characterization channel list */
  
  uint32 tx_char_freq[RFCOM_TXLIN_NUM_CHANNELS];
  /*!< Temp vs freq characterization channel frequency list */

  uint8  tx_char_chan_size;  
  /*!< Number of valid channels in the characterization channel list */

  uint16 tx_therm_bins[RFCOM_TXLIN_TEMP_BIN_SIZE];
  /*!< Therm calibration/characterization data */

  rfcommon_core_txlin_vs_temp_legacy_type 
             legacy_lin_vs_temp[RFCOM_TXLIN_NUM_LINEARIZERS];
  /*!< Store NV data from legacy temp comp NV into this data structure */

  rfcommon_core_txlin_vs_temp_vs_freq_type_type lin_vs_temp_vs_freq;
  /*!< Store NV data from new temp comp NV into this data structure */

  rfcommon_core_txlin_pin_cal_vs_freq_type pin_cal_vs_freq;
  /*!< For each PA state, store NV data from new pin_cal_vs_freq NV into this data structure */

}rfcom_txlin_temp_comp_nv_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Common Tx AGC - max power override algorithm
 
  @details
  Provides definitions of the different max power cap/override algorithms
  supported by common tx AGC to satisfy QPA and other customer requirements
*/
typedef enum 
{
  RFCOMMON_CORE_TXLIN_MAX_PWR_OVR_DEFAULT = 0,
  /*!< Do nothing to default Tx AGC algorithm */

  RFCOMMON_CORE_TXLIN_MAX_PWR_OVR_BOOST = 1,
  /*!< Support digital gain boost to meet max power shortage */

  RFCOMMON_CORE_TXLIN_MAX_PWR_OVR_QPA_BACKOFF = 2,
  /*!< Support digital gain backoff to meet QPA output requirement */
  
}rfcommon_core_txlin_max_pwr_ovr_algo_type;


/*!
  @brief
  Structures to hold all Tx linearizer Cal data for one band in mission mode
  @details
  The Cal data structure holds:
  - Each Linearizer node includes RGI vs Tx power vs APT table of 64 element. The Cal data structure
  holds pointer to each node with Calibrated data for up to 16 channels, 4 PA gain states and 3 Tx waveform
  types
  - Tx freq comp table of 16 channels+8 bins per channel.
  - RGI thresholds for freq comp
  - upper bound Tx frequency for linearizer search
  - 16 Calibrated channel/frequency list for freq comp
  - Frequency of calibrated linearizers
 
  */
/*---------------------- Multi-lin V1 Linearizer (STD) --------------------- */
typedef struct
{
  /*! @brief RGI/LUT index */
  uint16 rgi;
  /*! @brief TxAGC in dB10 unit*/
  int16  power;
  /*! @brief APT PA control data structure */
  union
  {
    /*! @brief APT control for Dime*/
    struct
    {
      /*! @brief Lower 16 bits kept as bias voltage for backwards compatibility */
      uint16 pa_bias_voltage;
      /*! @brief PA quiescent current setting */
      uint16 pa_q_current;
    }apt_pa_struct;

    /*! @brief legacy APT control for multi lin V1: SMPS bias only*/
    uint32 apt;
  };
  /*! @brief DPD index */
  uint16 dpd_index;
  /*! @brief IQ offset for temp comp*/
  uint8 iq_offset;
  /*! Index indicates the freq-comp array to use */
  uint16 fcomp_idx;

  /*! @brief Tx RSB compensation components */
  struct 
  {
    /*! @brief Tx RSB IQ correction 
    coefficient alpha minus 1, called alpha for brevity, per RGI */
    uint16 alpha ;
    /*! @brief Tx RSB Beta components */
    uint16 beta  ;
  } rsb_correction_coeff ;

  /*! @brief Tx LOFT compensation */
  uint32 loft_correction ;

} tx_linearizer_row_type;

typedef struct 
{
  /*! @brief Tx channel frequency */
  uint32 chan_freq;
  /*! @brief 64 entry LUT */
  tx_linearizer_row_type table[RFCOM_TXLIN_NUM_SEGMENTS];
  /*! The DPD type used by this linearizer */
  tx_linearizer_aux_data_type dpd_type;
  /*! The number of DPDs used by this linearizer */
  uint16 num_dpd;
  /*! The number of freq-comps used by this linearizer */
  uint16 num_fcomp;
  /*! The type of freq-comps used by this linearizer. 
      NOTE: num_fcomp has higher precedence. Types are full-bias and final-sweep */
  uint8 fcomp_type;
} tx_linearizer_table_type;

/*---------------------- Multi-lin V2 Linearizer (EPT) --------------------- */
/* EPT DPD Number of Coefficient parameters */
#define DPD_NUM_OF_AMAM_COEFF 4
#define DPD_NUM_OF_AMPM_COEFF 3
#define DPD_NUM_OF_PHASE_SHIFT 3

typedef struct 
{
  /*! @brief AMAM Coeff */
  int32 am[DPD_NUM_OF_AMAM_COEFF];
  /*! @brief AMPM Coeff */
  int32 pm[DPD_NUM_OF_AMPM_COEFF];
  /*! @brief Phase Shift */
  uint8 pm_shift[DPD_NUM_OF_PHASE_SHIFT];
  /*! @brief DPD Status */
  int8 status;
} dpd_poly_coeff_type;

/*! Contains AMAM items for a single RGI  */
typedef struct
{
  uint16 xpt_am[XPT_DPD_NUM_COEFF];
} xpt_am_row_type;

/*! Contains AMPM items for a single RGI  */
typedef struct
{
  int16 xpt_pm[XPT_DPD_NUM_COEFF];
} xpt_pm_row_type;

/*! Contains Detrough items for a single RGI */
typedef struct
{
  uint16 xpt_dt[XPT_DPD_NUM_COEFF];
} xpt_dt_row_type;

/*! Table of XPT AMAM items */
typedef struct
{
  /*! AM items per each RGI in linearizer */
  xpt_am_row_type* xpt_am_row[RFCOM_TXLIN_NUM_SEGMENTS];
} xpt_am_table_type;

/*! Table of XPT AMPM items */
typedef struct
{
  /*! PM items per each RGI in linearizer */
  xpt_pm_row_type* xpt_pm_row[RFCOM_TXLIN_NUM_SEGMENTS];
} xpt_pm_table_type;

/*! Table of XPT Detrough items */
typedef struct
{
  /*! Detrough items per each RGI in linearizer */
  xpt_dt_row_type* xpt_dt_row[RFCOM_TXLIN_NUM_SEGMENTS];
} xpt_dt_table_type;

/* Pointers to tables with AM/PM/Detrough items */
typedef struct 
{
  /*! AM table for the entire linearizer */
  xpt_am_table_type* xpt_am_table;
  /*! PM table for the entire linearizer */
  xpt_pm_table_type* xpt_pm_table;
  /*! Detrough table for the entire linearizer */
  xpt_dt_table_type* xpt_dt_table;
  /*! IQ Offset table for the entire linearizer */
  //iq_offset_table_type* iq_offset_table;
} xpt_data_type;

/*! @brief This is the main data type that is used between the linearizer 
  processing APIs. The entire global multi-lin structure does not need to
  be passed around as processing only happens for one PA state/waveform/channel
  at a time. */
typedef struct
{
  /*! @brief Type of Auxiliary data outside of the linearizer table alone. 
    This is needed for decision making 
    throughout linearizer processing since dpd_data, etc. will point always
    to a local variable even if there is no DPD data for that PA state.
    This means a NULL pointer check is not sufficient. */
  tx_linearizer_aux_data_type aux_lin_data;
  /*! @brief A pointer to the linearizer table (RGI/TxAGC/SMPS) */
  tx_linearizer_table_type *lin_data;
  /*! @brief A pointer to dpd data (AM/PM/PMShift/Status)*/
  //dpd_table_data_type *dpd_data;

  /*! Pointer to XPT items for this linearizer */
  xpt_data_type * xpt_data;
} tx_linearizer_table_data_type;


/*--------------------- Multi-lin Common Static Defs ----------------------- */

typedef struct
{
  /*! @brief upper bound frequency of which the linearizer will be used
     if uppder bound frequency = 0 then then this node has no Cal data */
  uint32 upper_bound_freq;

  /*! @brief points to linearizer table data and DPD table data if it 
    exists. */
  tx_linearizer_table_data_type node_data;
} tx_multi_linearizer_index_node_type;

/*! @brief Tx linearizer index for 16 channels */
typedef tx_multi_linearizer_index_node_type tx_linearizer_index_type[RFCOM_TXLIN_NUM_CHANNELS];

typedef struct
{
    /*! @brief 16 channel frequency compensation value */
    int8 freq_comp[RFCOM_TXLIN_NUM_CHANNELS];
    /*! @brief threshold of the RGI under which the freq comp will be applied
       if rgi_threshold = 0xFFFF then this bin has no valid freqcomp data */
    uint16 rgi_threshold;
} tx_freq_offset_table_row_type;

/*! @brief up to 8 bins in one PA state for different freq comp */
typedef tx_freq_offset_table_row_type tx_freq_offset_table_type[RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN];

typedef struct
{
  /*! @brief Tx frequency compensation table */
  tx_freq_offset_table_type tx_freq_offsets[RFCOM_TXLIN_NUM_V2_FCOMP_SWEEPS];
  /*! @brief Linearizer index for all waveform types  */
  tx_linearizer_index_type linearizer_index[TX_LINEARIZER_NUM_WAVEFORM];
} tx_pa_state_cal_data_type;


/*! @brief Final structure to hold all Tx Band Cal data in mission mode */
typedef struct 
{
  /*! @brief Tx calibrated channel list */
  uint32 tx_cal_chan[RFCOM_TXLIN_NUM_CHANNELS];
  /*! @brief Tx calibrated channel frequency list */
  uint32 tx_cal_freq[RFCOM_TXLIN_NUM_CHANNELS];
  /*! @brief Number of valid entries in Tx calibration channel list that has freq comp data*/
  uint8  tx_cal_chan_size;
  /*! @brief Revision of Multi-lin used. This is needed for decision making 
    with internal cal. The code needs to know which EFS ID to write to based
    on the internal global structure. */
  tx_multi_lin_rev_type nv_rev_type;
  /*! Reference ADC at which this linearizer was calibrated*/
  uint16 ref_lin_temp_adc; 
  /*! @brief Tx linearizer index for all PA gain states */
  tx_pa_state_cal_data_type tx_pa_state_cal_data[RFCOM_TXLIN_NUM_LINEARIZERS];
} tx_band_cal_type;

/*! @brief Tx linearizer search parameters */
typedef struct
{
  /*! @brief rfm device for query */
  rfm_device_enum_type rfm_device;
  /*! @brief target Tx frequency */
  uint32 tx_freq;
  /*! @brief PA range */
  uint8 pa_range;
  /*! @brief Tx waveform type */
  tx_linearizer_waveform_type tx_waveform; 
} tx_linearizer_search_param_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief 
  Tx linearizer vs temp vs freq compensation params
*/
typedef struct
{
  /*! 
    @brief
    Waveform type for the current linearizer
  */
  tx_linearizer_waveform_type waveform_type;

  /*! 
    @brief
    Current tuned Tx frequency
  */
  uint32 tx_freq;

  /*! 
    @brief
    Reference Linearizer Frequency
  */
  uint32 ref_lin_tx_freq;

} tx_lin_freq_comp_param_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Tx linearizer vs temp vs freq compensation params
*/
typedef struct
{
  /*! 
    @brief
    Current Therm Read
  */
  uint16 temp_read;

  /*! 
    @brief
    Therm bins static/cal data
  */
  uint16 *therm_cal;

  /*! 
    @brief
    Reference Linearizer Therm Read
  */
  uint16 ref_lin_temp_adc;

} tx_lin_temp_comp_param_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Tx linearizer vs temp vs freq compensation params
*/
typedef struct
{
  tx_lin_freq_comp_param_type freq_params;
  tx_lin_temp_comp_param_type temp_params;
  tx_band_cal_type *tx_multilin_nv_cal_ptr;
  uint8 pa_state;
  int16 (*lin_vs_temp_vs_freq)[RFCOM_TXLIN_TEMP_BIN_SIZE]
                              [RFCOM_TXLIN_NUM_CHANNELS];
  int32 ref_xpt_pin_offset;
  boolean auto_pin_enable;
} tx_lin_vs_temp_vs_freq_comp_param_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Common FED AGC configuration - timing offset params
 
  @details
  Provides interface for trigger times for the Tx AGC tasks. Timing information
  must be provided in usec x 10
*/
typedef struct
{
  /*! 
    @brief
    The time in usec x 10 at which the early WTR trigger must be done
  */
  int16 wtr_early_trig_offset;
  /*! 
    @brief
    The time in usec x 10 at which the late WTR trigger must be done
  */
  int16 wtr_late_trig_offset;
  /*! 
    @brief
    The time in usec x 10 at which the PA trigger must be done
  */
  int16 pa_trig_offset;
  /*! 
    @brief
    The time in usec x 10 at which the early PAPM trigger must be done
  */
  int16 papm_early_trig_offset;
  /*! 
    @brief
    The time in usec x 10 at which the late PAPM trigger must be done
  */
  int16 papm_late_trig_offset;
} rfcommon_core_txlin_agc_timing_params;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Common Tx AGC configuration - device configuration
 
  @details
  Provides tech-specific device information which is used by Tx AGC config
*/
typedef struct
{
  /*! 
    @brief
    Pointer to the WTR device obtained from RFC
  */
  void* wtr_device;
  /*! 
    @brief
    Pointer to the PA device obtained from RFC
  */
  void* pa_device;
  /*! 
    @brief
    Pointer to the PAPM device obtained from RFC
  */
  void* papm_device;
} rfcommon_core_txlin_agc_device_info;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Common Tx AGC configuration - PA STM information
 
  @details
  Provides tech-specific PA state information
*/
typedef struct
{
  /*! 
    @brief
    Current PA state for which configuration must be done
  */
  rfm_pa_gain_state_type pa_state;
  /*! 
    @brief
    Tech specific PA range information corresponding to PA state
  */
  rfm_pa_range_type pa_range;
  /*! 
    @brief
    XPT mode for the current PA state
  */
  uint8 xpt_mode;

} rfcommon_core_txlin_agc_pa_stm_info;


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Common Tx AGC configuration - Linearizer Row info holder
 
  @details
  Holds data specific to a given linearizer row
*/
typedef struct 
{
  /*! @brief Tx RSB IQ correction coefficient alpha minus 1, 
    called alpha for brevity, per RGI */
  uint16 alpha ;

  /*! @brief Tx RSB Beta components */
   uint16 beta  ;
} rfcommon_core_txlin_row_rsb_correction_coeff_t ;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Common Tx AGC configuration - Linearizer Capping Algorithm info
 
  @details
  Holds data for linearizer capping/boosting algorithms passed in by tech
*/
typedef struct 
{
  /*! @brief Linearizer cap/boost algorithm */
  rfcommon_core_txlin_max_pwr_ovr_algo_type max_pwr_ovr_algo;

  /*! @brief NV value corresponding to algorithm */
  int16 lin_cap_boost_nv_val;

} rfcommon_core_txlin_cap_boost_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Common Tx AGC configuration - Linearizer Row info holder
 
  @details
  Holds data specific to a given linearizer row
*/
typedef struct
{
  int16 smps_pwr;
  int16 fullbias_pwr;
  uint16 smps_rgi_index;
  uint16 fullbias_rgi_index;
  uint8 smps_rgi_to_const_rgi_mapping;
  rfm_pa_gain_state_type pa_state;
  rfm_pa_range_type pa_range;
  uint16 pa_curr;
  rfdevice_papm_mode_type xpt_mode;
  uint16 papm_bias;
  uint8 iq_offset;
  uint16 fcomp_index;
  rfcommon_mdsp_dpd_table_info_t amam_addr;
  rfcommon_mdsp_dpd_table_info_t ampm_addr;
  rfcommon_mdsp_dpd_table_info_t detrough_addr;
  rfcommon_core_txlin_row_rsb_correction_coeff_t rsb_corr_coeffs ;
  /*! @brief Tx LOFT compensation */
  uint32 loft_correction ;
} rfcommon_core_txlin_row_info_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Common Tx AGC configuration - Linearizer Logging data type 
 
  @details
  Holds data for linearizer data logging for a specific device
*/
typedef struct 
{
  struct
  {
    struct
    {
      /*! RGI linearizer log information */
      uint16 rgi;

      /*! PAPM bias linearizer log information */
      uint16 papm_bias;

      /*! PA ICQ linearizer log information */
      uint16 pa_icq;
    }rgi_data[RFCOM_TXLIN_NUM_SEGMENTS];

    /*! Flag to indicate PA state valid/invalid status */
    boolean pa_state_valid;

  }pa_state_data[RFCOM_TXLIN_NUM_LINEARIZERS];

  /*! Maximum calibrated linearizer power */
  int16 max_calibrated_power;

  /*! Maximum calibrated PA state */
  rfm_pa_gain_state_type max_calibrated_pa_state;

  /*! RFM mode */
  rfm_mode_enum_type rfm_mode;
  
  /*! Max Power override algorithm */
  rfcommon_core_txlin_max_pwr_ovr_algo_type 
              max_power_ovr_algo[RFCOM_TXLIN_NUM_LINEARIZERS];

  /*! Max env-scale back-off for EPT/ET mode */
  int16 max_add_env_scale_ept_et[RFCOM_TXLIN_NUM_LINEARIZERS][8];
  
}rfcommon_core_txlin_log_data_type;

#ifdef __cplusplus
}
#endif
#endif /*! @brief RFCOMMON_CORE_TXLIN_TYPES_H */
