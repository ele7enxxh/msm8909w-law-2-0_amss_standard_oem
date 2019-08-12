#ifndef RF_CDMA_NV_H
#define RF_CDMA_NV_H

/*!
  @file
  rf_cdma_nv.h

  @details
  This file exports the definitions and declartions necessary inorder to
  interface with the RF 1X NV Manager
*/

/*==============================================================================

  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_nv.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/16/15   vr     CDMA predictive clipping NV based power thresh support
11/10/14   zhw     Dynamic Predictive Cliping support
09/29/14   zhw     Compiler warning fix
08/20/14   cd      Legacy Tx switchpoints for 4-PA state are only 3
05/21/14   spa     Remove deprecated pin/pout part of NV (now in common)
05/02/14   cd      8-PA state functionality updates
04/29/14   cd      Add NV definitions for 8 PA state loading
04/23/14   cd      Support for 8 PA state functionality
01/16/14   zhw     CDMA Pin Cal support
01/09/14   zhw     CDMA NV loading memory leak fix
08/12/13   Saul    XPT. DPD Scaling.
07/22/13   APU     Removed et_path_delay_legacy from XPT NV structure.
07/19/13   Saul    XPT. ET delay vs freq vs temp support.
07/03/13   zhw     Antenna Tuner CL control data NV support
06/19/13   APU     Added new NV for ET delay. 
06/10/13   JJ      Remove legacy NVs  
05/31/13   JJ      Change rf_cdma_nv_get_calibration_table input band type
05/29/13   sty     Deleted rf_cdma_nv_load_antenna_tuner() and refs to it
04/22/13   cd      Temp Comp - support for multi-chan linearizer compensation
                   - Multi-algorithm support based on NV versioning
                   - Load static temp comp NVs into new common data structs
03/11/13   cd      Add support for loading new lin vs temp vs freq Pin and 
                   Pout NV items
02/22/13   Saul    Tx ET Path Delay NV support.
12/03/12   APU     Added a one time check for therm cal monotonicity. 
11/28/12   aki     Added rf_cdma_nv_cleanup
08/24/12   cd      CDMA EPT Temp and HDET feature support
07/26/12   APU     Added Enhanced PA backoff support.
07/11/12   cri     Added EPT support for CDMA
07/10/12   Saul    MultiLin. Added V2 and V3 NV-loading support to CDMA.
07/09/12   cri     Added remaining support for loading EPT config param from NV 
06/25/12   aro     Added code to load EPT config param from NV
06/25/12   cri     Added support for loading multi-lin 2 for CDMA
06/14/12   cd      Removed unused NV definitions. Clean up unused structures 
                   and functions
06/12/12   id      Add support to get HDET Vs AGC NV Item Id per tech and band
06/06/12   cd      - Remove legacy unused NV tables
                   - Remove functionality to load unused NV items
05/14/12   cd      Move lim vs temp SAR NV items to static table
04/10/12   id      Support for accessing Multi Lin global structure
03/22/12   id      Added support for getting Tx freq to chan conversion handler
03/22/12   sty     Deleted redundant rf_cdma_nv_set_pa_range_map_arr()
03/08/12   cd      NV data structure support for Tx GTR Thresh NV
03/08/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan  
02/29/12   cd      Added extra entry for R-SCH-6 for 1x NV
02/15/12   sar     Removing RF_HAS_HDR_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST.
02/13/12   sar     Feature reduction changes.
02/10/12   sty     Deleted extraneous hdr_tx_pwr_lim
02/08/12   sty     Added rfnv_cdma_spur_info_type
02/03/12   cd      Minor modification to LPM HDET NV description 
01/30/12   APU     Support for variable number of HDR PWR LIMITS NV items.
01/27/12   APU     Added new NV items for HDR PWR LIMITS.
01/25/12   sty     revert previous change since rfnv_items.h has not been changed
12/07/11   bmg     Added rf_cdma_nv_device_data_is_valid() to support the
                   rfm_cdma_is_band_chan_supported() function.
12/03/11   cd      Fix compiler error caused due to merge 
12/02/11   cd      Update default functionality for Tx switchpoints  
12/02/11   cd      Data structures to support loading of multi lin APT adjust 
                   NV 
12/01/11   cd      Data structures for new Tx switchpoint NV for 1x and HDR  
11/11/11   Saul    Thermal Backoff Support.
10/20/11   adk     Added support for SV diversity.
10/19/11   cd      Add data structures to support loading of lin_vs_temp and 
                   therm bins NV data
10/18/11   cd      Removed linearizer loading into old data structures and
                   renamed the new functions to be default
10/12/11   bmg     Added NV definitions for SAR Tx limit vs temperature.
09/19/11   cd      Load legacy Tx switchpoints
09/12/11   aro     Added support to load antenna tuner data for all chains
08/31/11   cd      Fixed KW warnings
08/30/11   cd      Add loading of multi-linearizer NV using common functionality
08/29/11   cd      Load Enc BTF delay NV for chains 0 and 2
08/22/11   cd      Updated HDET NV nomenclature
08/16/11   cd      Support load to old linearizer NV into multi-linearizer 
                   internal data representation
08/11/11   cd      Restored pack struct types to header to file to support 
                   packing override for off-target test cases
08/11/11   cd      - Moved pack struct types to source file
                   - Added functionality to load HDET calibration NV items for 
                   Chain0 and Chain2. LPM HDET NV functionality is unused.
08/09/11   cd      Cleaned up unused legacy LNA threshold parameters
08/04/11   cd      Changed new v2 freq comp function name to use sv string, as 
                   it is used for chain 2 freq comp loading
08/03/11   cd      Added support to load old Linearizer NV data into common 
                   data types created for APT and multilinearizer support
08/02/11   cd      Added tx NV item ID entries to cal and static NV item tables 
07/19/11   cd      Added intelliceiver cal data load functionality
06/13/11   cd      Changed Rx Switchpoint loading design for inactive NV items.
06/10/11   cd      Changed static NV items map to array of structs of size 
                   RF_CDMA_NV_PATH_NUM
06/09/11   cd      Removed unnecesary BC PA R Map init flag. Removed common NV 
                   loading support for PA R Map
06/08/11   cd      Deprecated old PA R Map NV loading support
06/08/11   cd      Added functionality and data structure to load band specific 
                   Tx PA R Map
06/06/11   cd      - Deprecated rf_cdma_nv_device_to_path()
                   - rf_cdma_nv_add_cal_chan() takes nv path as parameter
06/06/11   cd      Changed cal items map to array of structs
06/02/11   cd      - Added support for struct packing for offtarget tests 
                   - Added typedefs for Rx switchpoint data
                   - Removed old rx_cal_chan_lru variable
                   - Added static NV items map type
                   - Added functionality to load Rx switchpoints into new
                     data structures
                   - Added functionality to static NV items table
05/27/11   cd      - Extended cal NV items data structure to support SV path 
                   items
                   - Added LRU channel list variable
05/24/11   cd      Added support to load SV NV items
05/21/11   aro     Added NV PAth 2
05/10/11   cd      Removed rf_cdma_nv_get_freq_bin()
05/10/11   cd      Added rf_cdma_nv_get_tx_cal() and changed Tx freq offset data
                   to int16 to align with interpolation functionality
05/02/11   cd      Added
                   - rf_cdma_nv_load_tx_cal()
                   - rf_cdma_tx_lin_cal_type to store linearizer cal data
04/26/11   cd      Added Tx calibration data structure with support to load 
                   Tx calibration channel list
04/22/11   cd      Replaced old Rx cal channel list with new data structure
04/21/11   cd      Removed old NV structure LNA offsets
04/14/11   cd      Changed parameter type for gain state from int to 
                   rfm_lna_gain_state_type
04/13/11   cd      Extended rx cal load to all LNA gain states. Renamed 
                   rf_cdma_nv_load_rx_g0_cal() to rf_cdma_nv_load_rx_cal()
03/23/11   bmg     Added channel_list, freq_list, and freq_list_size items to
                   the Rx calibration data structure.
                   Added common rf_cdma_nv_load_rx_chan_list() function.
03/22/11   bmg     Removed unused function rf_cdma_nv_load_dvga_offset_data().
03/22/11   bmg     Added rf_cdma_nv_get_hdr_data(),
                   rf_cdma_nv_update_gain_cal(), and rf_cdma_nv_get_gain_cal().
                   removed DVGA from the old NV structure.
03/16/11   aro     Renamed enum to RF_CDMA_TX_LINEARIZER_NUM
03/14/11   aro     Doxygen documentation update
03/10/11   sty     Added rf_cdma_nv_set_pa_range_map_arr()
03/08/11   bmg     Added Rx gain cal data to the CDMA common structure.
03/03/11   sty     Added API to get pointer to common NV table
02/25/11   bmg     Removed CDMA NV items that are no longer used or will be
                   replaced with new functionality.
02/24/11   aro     [1] Added suuport for Common / BC0 NV load
                   [2] Renamed type name to rf_cdma_nv_band_items_type
02/24/11   bmg     Changed rf_cdma_config_type* in the 1x and HDR config
                   structures to a const pointer.
02/23/11   sty     Deleted unused RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ
                   Initilized LUT and linearizer tables
02/23/11   bmg     Added rf_cdma_nv_get_1x_data() config structure query
                   function
02/22/11   bmg     Introduced rf_cdma_config_type, rf_1x_config_type,
                   and rf_hdr_config_type.
02/01/11   aro     Added support for NV Commit
02/01/11   aro     Added function to add channel to Calibration
12/29/10   aro     Mainlined RF_HAS_POWER_MODE_LIB
12/29/10   aro     Added function to return Frequency Bin for given band/chan
12/29/10   aro     [1] Added NV Table Accessor function
                   [2] Added Device to Path mapping function
                   [3] Removed unused table
12/20/10   aro     Removed including of rfcom.h
12/17/10   aro     Added NV Load function and removed unused functions
12/13/10   aro     Added cagc.h include file for Linker Error Fix
12/09/10   aro     [1] Removed unncessary includes
                   [2] Doxygen Update
12/08/10   aro     Fixed include filename
12/08/10   aro     Fixing preprocessor Macros
12/06/10   aro     Initial Release.

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#include "rex.h"
#include "rfcommon_nv.h"
#include "rfcommon_core_xpt.h"
#include "rfcommon_core_txlin.h"
#include "rf_cdma_lna.h"
#include "rfm_device_types.h"
#include "rfm_cdma_band_types.h"
#include "rfm_cdma_power_mode_types.h"
#include "rfm_gain_state_types.h"
#include "sys.h"
#ifdef FEATURE_CMI
#include "sys_v.h"
#endif
#include "cagc.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef T_WINNT
#error code not present
#endif

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number represents the number of Tx linearizers supported in NV
*/
#define RF_CDMA_NV_TX_LINEARIZER_NUM                 RFCOM_TXLIN_NUM_LINEARIZERS

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number represents the frequency compensation list size in NV.
*/
#define RF_CDMA_NV_FREQ_LIST_SIZE                              NV_FREQ_TABLE_SIZ

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number represents the number of Tx linearizer entries for each
    linearizer.
*/
#define RF_CDMA_NV_TX_LIN_TBL_SIZE                      RFCOM_TXLIN_NUM_SEGMENTS

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number represents the number of Tx switch points in the Tx AGC state
    machine for legacy Tx AGC support (4 PA states)

    @details
    The number of switch points is 1 less than the number of linearizers
    supported for legacy Tx AGC.
*/
#define RF_CDMA_NV_LEGACY_SWITCH_POINTS_NUM                                  3

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number represents the LUT delimiter value in legacy Tx linearizer NV
*/
#define RF_CDMA_NV_LEGACY_TX_LUT_DELIMITER                                32767

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number represents the Pwr delimiter value in legacy Tx linearizer NV
*/
#define RF_CDMA_NV_LEGACY_TX_PWR_DELIMITER                               -32768

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number represents the LUT delimiter value in Tx multi-linearizer NV
*/
#define RF_CDMA_NV_TX_LUT_DELIMITER                                       65535

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number represents the Pwr delimiter value in Tx multi-linearizer NV
*/
#define RF_CDMA_NV_TX_PWR_DELIMITER                                       32767

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number represents the max LUT size 
*/
#define RF_CDMA_NV_TX_LUT_MAX_VALUE                                         128

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number represents the number of Intelliceiver Jammer Detectors
    supported in NV.
*/
#define RF_CDMA_NV_INTELLICEIVER_DET_COUNT       NV_INTELLICEIVER_DETECTOR_COUNT

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number represents the number of power values for which expected HDET
    ADC readings are collected during calibration.
*/
#define RF_CDMA_NV_EXP_HDET_VS_AGC_SIZE             NV_CDMA_EXP_HDET_VS_AGC_SIZ

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number represents the start power level for HDET vs AGC calculations.
    It is represented in dBm*10
*/
#define RF_CDMA_NV_HPM_HDET_START_LVL_LOW_BANDS                             164

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number represents the start power level for HDET vs AGC calculations.
    It is represented in dBm*10
*/
#define RF_CDMA_NV_HPM_HDET_START_LVL_HIGH_BANDS                            134

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number represents the bin size over which exp_hdet_vs_agc readings
    are collected during calibration. It is provided in dB*10 i.e 1.6dB = 16
*/
#define RF_CDMA_NV_EXP_HDET_VS_AGC_BIN_SIZE                                  16

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This value represents the default Enc BTF value
*/
#define RF_CDMA_NV_ENC_BTF_DELAY_DEFAULT                                   3220

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  The number of SAR states that the NV supports.

  @details
  This is the total number of SAR states, including the disabled state, 0.
  Valid states are 0 (disabled: use legacy lim vs temp), and active states
  1 through 8, for a total of 9 states.

  This item is used to size the internal memory structures for lim vs temp
  data.
*/
#define RF_CDMA_NV_DSI_SAR_STATE_SIZE                                         9

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  The number of temperature points that the Tx max power limit can be
  specified at.
*/
#define RF_CDMA_NV_TEMP_LIMIT_SIZE                                            8

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  The default IQ gain back-off from the Link Manager default, in HW units
*/
#define RF_CDMA_NV_DEFAULT_IQ_GAIN_BACKOFF                              0x10000

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  The number of reverse link SCH rates for 1x, used for 1x Data Tx
  switchpoint NV
*/
#define RF_CDMA_NV_1X_R_SCH_RATES_NUM                                         6

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  The number of Reverse Rate Indicator rates for HDR in Rev. 0 state,
  used for HDR Rev0 Tx switchpoint NV
*/
#define RF_CDMA_NV_HDR_RRI_RATES_NUM                                          6

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number represents the ID of an invalid NV item.
*/
#define RF_CDMA_NV_INVALID_NV_ITEM                          RFNV_ITEMS_ENUM_MAX

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number represents the lowest Tx power in Dbm10 that can be used as
    PA switchpoint*/
#define RF_CDMA_PA_SWITCHPOINT_LOWEST_TX_POWER                              -700

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  This number represents the highest Tx power in Dbm10 that can be used as
  PA switchpoint
*/
#define RF_CDMA_PA_SWITCHPOINT_HIGHEST_TX_POWER                              400

/*----------------------------------------------------------------------------*/

/*! @cond Doxygen_Suppress */

/*
Number of elements in the array - each element represents the RL packet size is DO Rev A
(element 0 - reserved, 1-12 for PS1 to PS12, element 13 - 15: reserved
*/

#define RF_HDR_REVA_TX_PREDISTORT_NUM         16


/*===========================================================================

                       PUBLIC DATA DECLARATIONS

===========================================================================*/
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                            CONSTANT DEFINITIONS

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/* Build Version defaults
*/
#define RF_TEST_CODE_VER    {'N','O','N','E',' ',' ',' ',' '}
#define RF_SYS_SW_VER       {'N','O','N','E',' ',' ',' ',' '}
#define RF_RF_CAL_VER       {'N','O','N','E',' ',' ',' ',' '}
#define RF_CONFIG_VER       {'N','O','N','E',' ',' ',' ',' '}
#define RF_CAL_DATE         0x0
#define RF_NV_LOADED_DATE   0x0
#define RF_CAL_DAT_FILE     {'N','O','N','E',' ',' ',' ',' ',' '}

/* RF Chipset */

#define RF_CDMA_TX_LINEARIZER_NUM  RFCOM_TXLIN_NUM_LINEARIZERS

#define RF_THERMAL_MAX_LIMIT_DEFAULT_V {0,0,0,0,0,0,0,0}

/* Default Pin versus freq value for RFNV - 
This indicates Pin is not calibrated for that particular PA state*/
#define RFNV_CDMA_INVALID_PIN_CAL_VS_FREQ 32767
/*----------------------------------------------------------------------------*/
/*! 
  @brief  
  Pin Cal versus Freq NV structure
*/
typedef PACK (struct)
{
  /*! Pin Cal versus Freq NV information */
  int16 pin_cal_vs_freq[RFCOM_TXLIN_NUM_PA_STATES]
                       [RFCOM_TXLIN_NUM_CHANNELS];

}rfnv_cdma_pin_cal_vs_freq_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV structure to copy spur locations and notch settings for 1x and HDR
*/
typedef PACK(struct)
{
  PACK (struct)
  {                                     
    /*! Freq (in hertz) at which the spur is located */
    uint32 spur_at_freq_hz;
    
    /*! notch setting that is used to suppress the above spur */
    uint8 notch_depth;
  }spur_entry[RFCOMMON_MAX_SPUR_ENTRIES]; 

}rfnv_cdma_spur_info_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV structure to copy Tx power Freq Comp data
*/
typedef PACK(struct)
{
  /*! Freq comp data calibrated for upto 16 channels */
  int8 freq_comp[RF_CDMA_NV_FREQ_LIST_SIZE];
}rfnv_tx_pwr_freq_comp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV structure of the SAR state 0 limit vs temperature data.

  @details
  This is the structure of the "legacy" lim vs temp NV items, which contain
  8 points of tx limit adjustment verses temperature.

  Each point is in "top-quarter" unsigned Tx AGC units, so 0 corresponds to
  1023-Tx AGC of 768, and 255 corresponds to 1023-Tx AGC of 1023.  This
  limits the range of the max power limit for SAR state 0 to +16.5dBm in
  low bands (turnaround constant of 73), and +13.5dBm in high bands
  (turnaround constant of 76).
*/
typedef PACK(struct)
{
  /*! Temp comp data for 8 temperature points. */
  uint8 temp_comp[RF_CDMA_NV_TEMP_LIMIT_SIZE];
} rfnv_tx_pwr_sar_0_temp_comp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV structure of the SAR states 1 through 8 limit vs temperature data.

  @details
  This is the structure of the lim vs temp NV items for the 8 "active" SAR
  states.  Each SAR state has its own lim vs temp curve.  SAR state 0
  ("inactive") is stored in the legacy NV items, so this data structure
  has only 8 rows of lim vs temp data.

  Each limit vs temp point is in full 1023-Tx AGC units, allowing the limit
  to be set to any value in the Tx dynamic range.
*/
typedef PACK(struct)
{
  /*! Temp comp data for 8 active SAR states, with 8 temperature points. */
  uint16 temp_comp[RF_CDMA_NV_DSI_SAR_STATE_SIZE-1][RF_CDMA_NV_TEMP_LIMIT_SIZE];
} rfnv_tx_pwr_sar_1_8_temp_comp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV structure for loading Rx switchpoint data
*/
typedef PACK(struct)
{
  PACK(struct)
  {
    uint8 lin_mode_active;
    PACK(struct)
    {
      uint8 sp_mode;
      int16 fall_hysteresis;
      int16 rise_backoff;
      int16 fall_hold;
      int16 rise_hold;
    }sp_data[RFM_LNA_GAIN_STEP_NUM];
  }lin_data[RFM_CDMA_POWER_MODE_MAX];
  /*!< LNA Switchpoint Linearity Info */
}rfnv_rx_swpt_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV typedef to copy Tx PA Range Map data
*/
typedef byte rfnv_tx_pa_r_map_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure used to get Intelliceiver calibration Data from NV.
*/
typedef PACK(struct)
{
  uint16 channel;
  /*!< Reference channel for which Intelliceiver Cal is done */
  int16  calibration[RFM_CDMA_POWER_MODE_MAX]
                    [RF_CDMA_NV_INTELLICEIVER_DET_COUNT];
  /*!< Calibration data for intelliceiver */
} rfnv_ic_cal_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure used to get Intelliceiver Jammer Detector Threshold 
  Adjust values from NV.
*/
typedef PACK(struct)
{
  int8  adj_val[RF_CDMA_NV_INTELLICEIVER_DET_COUNT]
               [RFM_LNA_GAIN_STATE_NUM];
  /*!< Detector Threshold Adjust values */
} rfnv_ic_det_adjust_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV structure to copy Expected HDET vs AGC
*/
typedef PACK(struct)
{
  /*! HDET Vs AGC data across 16 steps */
  uint16 hdet_vs_agc[RF_CDMA_NV_EXP_HDET_VS_AGC_SIZE];
}rfnv_tx_hdet_vs_agc;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV structure to copy Low Power Mode (LPM) HDET Data
*/
typedef PACK(struct)
{
  /*! Enabled/Disabled */
  boolean enabled;
  /*! HDET offset val */
  uint16 off;
  /*! HDET span val */
  uint16 spn;
  /*! LPM Threshold val */
  uint16 thresh;
  /*! HDET Vs AGC data across 16 steps. Entry 0 will hold 
  the start power corresponding to the first HDET reading */
  uint16 hdet_vs_agc[RF_CDMA_NV_EXP_HDET_VS_AGC_SIZE + 1];
}rfnv_tx_lpm_hdet;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV structure to copy linearizer vs temperature data
*/
typedef PACK(struct)
{
  /*! Lin vs temp data for 4 PA states across 8 therm bins */
  int8 reserved;
  PACK(struct)
  {
    int8 offset[RF_CDMA_NV_TEMP_LIMIT_SIZE];
    int16 slope[RF_CDMA_NV_TEMP_LIMIT_SIZE];
  }data[RF_CDMA_NV_TX_LINEARIZER_NUM];
}rfnv_tx_lin_vs_temp;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV structure to copy therm bin calibration data
*/
typedef PACK(struct)
{
  /*! Thermistor calibration data */
  uint16 therm_cal[RF_CDMA_NV_TEMP_LIMIT_SIZE];
}rfnv_tx_therm_cal;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV structure for loading Tx switchpoint data.
  Used for Access/1x Voice/DO RevA/DO RevB
*/
typedef PACK(struct)
{
  PACK(struct)
  {
    int16 fall;
    int16 rise;
  }sp_data[RF_CDMA_NV_LEGACY_SWITCH_POINTS_NUM];
  int16 fall_time_hysteresis;
}rfnv_tx_swpt_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV structure for loading Tx switchpoint data.
  Used for 1x Data
*/
typedef PACK(struct)
{
  PACK(struct)
  {
    rfnv_tx_swpt_type sp;
  }rate[RF_CDMA_NV_1X_R_SCH_RATES_NUM];
}rfnv_tx_1x_data_swpt_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV structure for loading Tx switchpoint data.
  Used for HDR Rev0
*/
typedef PACK(struct)
{
  PACK(struct)
  {
    rfnv_tx_swpt_type sp;
  }rate[RF_CDMA_NV_HDR_RRI_RATES_NUM];
}rfnv_tx_dor0_swpt_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV structure to copy Tx APT Adjust static NV data for 1x and HDR
*/
typedef PACK(struct)
{
  /*! Reserved byte in NV */
  int8 reserved;

  /*! 1x APT adjust data */
  PACK(struct)
  {
    int8 pilot;
    int8 rc1rc2;
    int8 rc3rc4_voice;
    int8 rc3rc4_data;
    int8 rc8_voice;
    int8 rc8_data;
  }data_1x;

  /*! DOr0 APT adjust data */
  PACK(struct)
  {
    int8 dor0_pilot;
    int8 dor0_1;
    int8 dor0_2;
    int8 dor0_3;
    int8 dor0_4;
    int8 dor0_5;
  }data_dor0;

  /*! DOrA APT adjust data */
  PACK(struct)
  {
    int8 dorA_ps0;
    int8 dorA_ps1;
    int8 dorA_ps2;
    int8 dorA_ps3;
    int8 dorA_ps4;
    int8 dorA_ps5;
    int8 dorA_ps6;
    int8 dorA_ps7;
    int8 dorA_ps8;
    int8 dorA_ps9;
    int8 dorA_ps10;
    int8 dorA_ps11;
    int8 dorA_ps12;
  }data_dorA;

}rfnv_tx_apt_adj_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  RF NV structure to copy the CDMA Tx GTR Threshold NV data
*/
typedef PACK(struct)
{
  /* Indicator of whether it is active or not */
  boolean active;
  /* Rise threshold in dBm10 */
  int16 rise_value;
  /* Fall threshold in dBm10 */
  int16 fall_value;
  /* Reserved */
  int16 reserved;
}rfnv_tx_gtr_thresh_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV structure to copy DPD configuration parameters
*/
typedef PACK(struct)
{
  /*! DPD configuration parameters */
  uint32 dpd_cfg[MAX_EPT_NUM_CONFIG_PARAMS];
}rfnv_dpd_cfg_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV structure to copy HDET vs Temp static NV data
*/
typedef PACK(struct)
{
  /*! HDET vs Temperature compensation data */
  int8 hdet_vs_temp[RF_CDMA_NV_TEMP_LIMIT_SIZE];
}rfnv_hdet_vs_temp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV structure to copy Lim vs Temp vs Freq static NV data
*/
typedef PACK(struct)
{
  /*! Lim vs Temp vs Freq NV information */
  int16 lim_vs_temp_vs_freq[RF_CDMA_NV_TEMP_LIMIT_SIZE]
                           [RF_CDMA_NV_FREQ_LIST_SIZE];
}rfnv_lim_vs_temp_vs_freq_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV structure to copy HDET mod range static NV data
*/
typedef PACK(struct)
{
  /*! HDET Mod Range NV information */
  int16 hdet_mod_range[2]
                      [RF_CDMA_NV_TEMP_LIMIT_SIZE];
}rfnv_hdet_mod_range_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV structure to copy Lin vs Temp vs Freq static NV data
*/
typedef PACK(struct)
{
  /*! Lin vs Temp vs Freq NV information */
  uint8 reserved;
  int16 lin_vs_temp_vs_freq[RF_CDMA_NV_TX_LINEARIZER_NUM]
                           [RF_CDMA_NV_TEMP_LIMIT_SIZE]
                           [RF_CDMA_NV_FREQ_LIST_SIZE];
}rfnv_lin_vs_temp_vs_freq_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure used to get Predictive Clipping Threshold Data from NV.
*/
typedef PACK(struct)
{
  uint8 reserved;
  
  int16 enable_threshold;

  int16 disable_threshold;
  
} rfnv_pred_clip_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ENUM containing MAX x and y imbalances.
*/
typedef enum
{
  MAX_NV_HDR_PWR_LIMIT_COORDINATES = 7  
   /*!< Maximum HDR Tx power limit coordinates stored in NV */
} hdr_nv_tx_pwr_coordinates_type ;

/*!
  @brief
  ENUM containing MAX x and y imbalances.
*/
typedef enum
{
  MAX_HDR_PWR_LIMIT_COORDINATES = 8  
 /*!< Max HDR Tx power limit coordinates sent to FW */
} hdr_tx_pwr_coordinates_type ;

typedef enum
{
  MAX_HDR_PWR_PATTERNS_PER_BAND = 25 
 /*!< Max carrier config patterns that can be stores per band */
} hdr_tx_pwr_patttern_pwer_band_type ;

typedef enum
{
 MAX_HDR_PWR_LIMIT_THERMAL_BINS = 8 
  /*!< Number of temperature bins over which the phone operates supported in NV */   
} hdr_tx_pwr_max_thermal_bins_type ;



/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV structure to copy static NV x and y imbalances 
  used by rfnv_tx_pwr_limit_data_type.
*/
typedef PACK(struct)
{
  int16 x_axis_val ;
  int16 y_axis_val ;
} rfnv_hdr_tx_pwr_dbm10_coordinate_t ;


/*----------------------------------------------------------------------------*/


/*! @brief Maximum number of Bins. Limitation comes from the 
  definition the NV
  */
enum { MAX_FREQ_COMP_ET_PATH_DELAY_BINS   = 16 } ;

/*! @brief Minimum acceptable path delay as defined by the 
  systems.
  */
enum { MIN_VALID_ET_PATH_DELAY_IN_NS      = -2147483647 } ;   


/*! @brief Maximum acceptable path delay as defined by the 
  systems.
  */
enum { MAX_VALID_ET_PATH_DELAY_IN_NS      =  2147483647 } ;   


/*! @details 
  RF NV structure to hold et path delay wrt freq. We can have
  upto 16 frequency bins and corresponding path delays. The
  number of bins usable are defined by number_of_valid_entries
  field. If this NV is used, it must follow all the rules.
  1> Number of valid entries should corresponsing number of
  valid items.
  2> Number of valid entries should be between 1 and 16. So if
  this NV is used atleast the first item must be populated.
  3> There cannpt be gaps in the NV table. i.e. the user cannot
  populate entries 1 and 3. If the number of valid items is say
  5. The first five entries must be filled. Same delays are
  allowed acoss different bins, for example bin 3 and bin 4 may
  have the same delay.
  4> The delays must be filled in order of ascending frequency.
  Since the NV contains channels and not the frequency the user
  may have to convert channel isnto frquency, assign delay and
  sort the table so channels are in ascending frequency order.
  If this is not followed this NV will not be used.
  5> The value of path delay should be within valid limits.
  6> The value of each channel contained within number of valid
  entries must be a valid channel for a given band.
*/
typedef PACK(struct)
{
  /*!<  Number of bins user wants to use. This can be between 1 
    and 16.*/
  uint8     number_of_valid_entries ;

  /*!<  This field contains the channel. The number of fields 
    filled should be between 1 number_of_valid_entries. The
    channels must be populated in the order of ascending
    frequency. The populated channels must be a valid channel
    for a given band */
  uint32    upper_bound_channel[MAX_FREQ_COMP_ET_PATH_DELAY_BINS] ;

  /*!<  This field contains the ET path delay for the 
    corresponding frequency in upper_bound_channel[] The number
    of entries should be between 1 and number_of_valid_entries.
    The value of delay should be valid otherwise bad values will
    be written to the HW */
  int32     et_path_delay_vs_freq[MAX_FREQ_COMP_ET_PATH_DELAY_BINS]  ;
} rfnv_tx_et_path_delay_data_type ;
/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV structure to copy static NV data HDR PWR limits.
*/
typedef PACK(struct)
{
  /*!<  In units of 1.25MHz, the bandwidth over which the MCDO carriers are spread ;
     a "2" will correspond to 1.25 MHz X 2 = 2.5 MHz  */
  uint8 bw ;           

  /*!<  For the next to elements: valid Channels range from 0 to 0xFFFE, 
     0xFFFF is used for representing RFC_HDR_LF_CHAN_ANY. */  
  uint16 carrier_config_pattern ;

  /*!< In case of multiple carriers, the channel with the lowest frequency */
  uint16 lowest_freq_chan ;
  
  /*!< In case of multiple carriers, the channel with the highest frequency */
  uint16 highest_freq_chan ;
 
  /*!< Max power spread among multiple carriers */
  int16  tx_pwr_limit_dBm10[MAX_HDR_PWR_LIMIT_THERMAL_BINS] ;

  /*!< L[x][y] and R[x][y], where x and y are coordinates in 
     dbm10. Two LUTs are needed for the 3-carrier case, and
     because satisfying requirements for 450 MHz, and BC6 may
     require different mappings for LUT_indicator. For instance,
     in the 450 MHz case, high power on the rightmost RL carriers
     can be most detrimental to FL receiver desense, and so an
     LUT_indicator of ‘0 0 1’ might be employed. On the contrary
     for BC6, the leftmost RL carriers can be most detrimental
     to the PHS band, and so an LUT_indicator of ‘1 0 0’ might
     be employed.*/
  /*!< Higher value LUT where x,y signify:
       x: Power on the strongest non-data carrier  
       Y: Max power of the carriers combined */
  rfnv_hdr_tx_pwr_dbm10_coordinate_t 
   hi_tx_imbalanced_carrier_pwr_lut [MAX_NV_HDR_PWR_LIMIT_COORDINATES] ;           

  /*!< Lower value LUT  */
  rfnv_hdr_tx_pwr_dbm10_coordinate_t 
   lo_tx_imbalanced_carrier_pwr_lut [MAX_NV_HDR_PWR_LIMIT_COORDINATES] ;            

  /*!< N-bit vector LUT_indicator indicatinf whether to use 
       Hi_Backoff_LUT  and a Lo_Backoff_LUT, where N is the 
       number of active carriers in the channel configuration. */
  uint8 LUT_indicator ;

  /*!< Power differential between the data carrying carrier and 
       the strongest non-data bearing carrier that L1 can apply
       to the data carrying carrier. */
  uint8 funnel_bias ;

  /*!< A reference temperature at which tx_pwr_limit_dBm10[] measurements are made 
       The hi and lo LUTs are then moved up or down wrt this temp */
  uint16 mdsp_reading_at_reference_temperature ;

  uint8 table_version ;

  uint8 reserved_bytes[3] ;
} rfnv_tx_pwr_limit_data_type ;

typedef enum 
{
  LUT_MONOTONIC = 0 ,
  HI_LUT_X_VALUES_NOT_ASCENDING   = ( 1 << 0 ) ,
  HI_LUT_Y_VALUES_NOT_DESCENDING  = ( 1 << 1 ) ,

  LO_LUT_X_VALUES_NOT_ASCENDING   = ( 1 << 2 )  ,
  LO_LUT_Y_VALUES_NOT_DESCENDING  = ( 1 << 3 ) 

} lut_monotonicity_t ;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF HDR structure containing  x and y imbalances 
  used by rf_cdma_tx_pwr_limit_data_type.
*/
typedef struct
{
  int16 x_axis_val ;
  int16 y_axis_val ;
} rf_cdma_hdr_tx_pwr_dbm10_coordinate_t ; 


/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF HDR structure containing HDR PWR limits.
*/
typedef struct
{
  uint8 number_of_elements ;
  
  uint8 bw ;           /* rounded to in units of 1.25MHz */

  uint16 carrier_config_pattern ;

  /* Channels range from 0 to 0xFFFE, 0xFFFF is used for representing RFC_HDR_LF_CHAN_ANY.  */  
  uint16 lowest_freq_chan ;
  uint16 highest_freq_chan ;

  /* tx_pwr_limit_dBm10[pwr_lim_dbm10] */
  int16  tx_pwr_limit_dBm10[MAX_HDR_PWR_LIMIT_THERMAL_BINS] ;

  /* L[x][y] and R[x][y], where x and y are coordinates in dbm10 */
  rf_cdma_hdr_tx_pwr_dbm10_coordinate_t hi_tx_imbalanced_carrier_pwr_lut[MAX_HDR_PWR_LIMIT_COORDINATES] ;           
  rf_cdma_hdr_tx_pwr_dbm10_coordinate_t lo_tx_imbalanced_carrier_pwr_lut[MAX_HDR_PWR_LIMIT_COORDINATES] ;            
  
  uint8 LUT_indicator ;
  uint8 funnel_bias ; 
  uint16 mdsp_reading_at_reference_temperature ;

  lut_monotonicity_t table_monotonicity ;
  boolean is_max_eq_pwr_greater_than_max_neq_value ;
  uint8 table_version ;

} rf_cdma_tx_pwr_limit_data_type ;


/*----------------------------------------------------------------------------*/
/*! Enumeration for available Paths for NV usage only */
typedef enum
{

  RF_CDMA_NV_PATH_0,
  /*!< Physical Path 0 for NV */

  RF_CDMA_NV_PATH_1,
  /*!< Physical Path 1 for NV */

  RF_CDMA_NV_PATH_2,
  /*!< Physical Path 2 for NV */

  RF_CDMA_NV_PATH_3,
  /*!< Physical Path 3 for NV */

  RF_CDMA_NV_PATH_NUM,
  /*!< Total number of NV Path Available */

  RF_CDMA_NV_PATH_MAX = RF_CDMA_NV_PATH_NUM
  /*!< Max Physical Path for NV */

} rf_cdma_nv_path_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Define Tx Reverse Link Configuration Types for IQ Gain support
*/
typedef enum
{
  RF_CDMA_NV_1X_TX_IQGAIN_PILOT = 0,

  RF_CDMA_NV_1X_TX_IQGAIN_RC1 = 1,
  RF_CDMA_NV_1X_TX_IQGAIN_RC2 = 1,

  RF_CDMA_NV_1X_TX_IQGAIN_RC8_VOICE = 2,
  RF_CDMA_NV_1X_TX_IQGAIN_RC8_DATA = 3,

  RF_CDMA_NV_1X_TX_IQGAIN_RC3_VOICE = 4,
  RF_CDMA_NV_1X_TX_IQGAIN_RC4_VOICE = 4,

  RF_CDMA_NV_1X_TX_IQGAIN_RC3_DATA = 5,
  RF_CDMA_NV_1X_TX_IQGAIN_RC4_DATA = 5,

  RF_CDMA_NV_1X_TX_IQGAIN_RC_NUM

} rf_cdma_nv_1x_tx_iqgain_rc_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Define Tx Reverse Link Configuration Types for IQ Gain support
*/
typedef enum
{
  RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_PILOT = 0,
  RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_RRI0 = 1,
  RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_RRI1 = 2,
  RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_RRI2 = 3,
  RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_RRI3 = 4,
  RF_CDMA_NV_HDR_TX_IQGAIN_DOR0_RRI4 = 5,

  RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS0 = 6,
  RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS1 = 7,
  RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS2 = 8,
  RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS3 = 9,
  RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS4 = 10,
  RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS5 = 11,
  RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS6 = 12,
  RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS7 = 13,
  RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS8 = 14,
  RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS9 = 15,
  RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS10 = 16,
  RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS11 = 17,
  RF_CDMA_NV_HDR_TX_IQGAIN_DORA_PS12 = 18,

  RF_CDMA_NV_HDR_TX_IQGAIN_RC_NUM

} rf_cdma_nv_hdr_tx_iqgain_rc_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Define HDET Mod Range Hi Low Index enum
*/
typedef enum
{
  RF_CDMA_NV_HDET_MOD_RANGE_LO,
  RF_CDMA_NV_HDET_MOD_RANGE_HI,

  RF_CDMA_NV_HDET_MOD_RANGE_NUM

} rf_cdma_nv_hdet_mod_range_hi_lo_type;

/* -----------------------------------------------------------------------
** Common NV calibration tables
** ----------------------------------------------------------------------- */
typedef struct {

  /* General RF Items */

  byte    rfnv_test_code_ver[NV_SIZE_OF_VERSION];
  byte    rfnv_sys_sw_ver[NV_SIZE_OF_VERSION];
  byte    rfnv_rf_cal_ver[NV_SIZE_OF_VERSION];
  byte    rfnv_rf_config_ver[NV_SIZE_OF_VERSION];
  dword   rfnv_rf_cal_date;
  dword   rfnv_rf_nv_loaded_date;
  byte    rfnv_rf_cal_dat_file[NV_QC_SERIAL_NUM_LEN];

  /*  HDR Tx Power limit due to thermal condition in dBm10
  Only the first entry is used. */
  int16 rfnv_cdma_thermal_max_limit[NV_TEMP_TABLE_SIZ];

} rf_cdma_nv_common_items_type;

/*! @endcond */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure holding the mapping information for CDMA RF Calibration NV items
*/
typedef struct
{

  rfnv_item_id_enum_type rx_cal_chan_list;
  /*!< Cal Channel List */

  rfnv_item_id_enum_type rx_cal_chan_lru_list;
  /*!< Least recenrly used chans */

  rfnv_item_id_enum_type dvga;
  /*!< DVGA Calibration Data */

  rfnv_item_id_enum_type dvga_vs_freq;
  /*!< DVGA Calibration Data */

  rfnv_item_id_enum_type lna_1;
  /*!< LNA1 Calibration Data */

  rfnv_item_id_enum_type lna_1_vs_freq;
  /*!< LNA2 Calibration Data */

  rfnv_item_id_enum_type lna_2;
  /*!< LNA2 Calibration Data */

  rfnv_item_id_enum_type lna_2_vs_freq;
  /*!< LNA2 Calibration Data */

  rfnv_item_id_enum_type lna_3;
  /*!< LNA3 Calibration Data */

  rfnv_item_id_enum_type lna_3_vs_freq;
  /*!< LNA3 Calibration Data */

  rfnv_item_id_enum_type lna_4;
  /*!< LNA4 Calibration Data */

  rfnv_item_id_enum_type lna_4_vs_freq;
  /*!< LNA4 Calibration Data */

  rfnv_item_id_enum_type im2_i;
  /*!< IM2 I Calibration Data */

  rfnv_item_id_enum_type im2_q;
  /*!< IM2 Q Calibration Data */

  rfnv_item_id_enum_type im2_gm;
  /*!< IM2 Transconductance Calibration Data */

  rfnv_item_id_enum_type im2_lpm1;
  /*!< IM2 LPM1 Calibration Data */

  rfnv_item_id_enum_type im2_lpm2;
  /*!< IM2 LPM2 Calibration Data */

  rfnv_item_id_enum_type intelliceiver_cal;
  /*!< IM2 Intelliciever Cal Data */

  rfnv_item_id_enum_type tx_cal_chan_list;
  /*!< Tx Calibration Channel List */

  rfnv_item_id_enum_type multi_lin;
  /*!< Multi-linearizer Data for all PA states */

  rfnv_item_id_enum_type multi_lin_v2;
  /*!< Multi-linearizer V2 Data for all PA states */

  rfnv_item_id_enum_type multi_lin_v3;
  /*!< Multi-linearizer V3 Data for all PA states */

  rfnv_item_id_enum_type lim_vs_freq;
  /*!< Max Power Freq Comp */

  rfnv_item_id_enum_type hpm_exp_hdet_vs_agc;
  /*!< Expected HDET vs AGC */

  rfnv_item_id_enum_type hpm_hdet_off;
  /*!< HDET offset */

  rfnv_item_id_enum_type hpm_hdet_spn;
  /*!< HDET span */

  rfnv_item_id_enum_type lpm_hdet;
  /*!< Low Power mode HDET */

  rfnv_item_id_enum_type hdet_vs_freq;
  /*!< Calibrated HDET compensation across freq */

  rfnv_item_id_enum_type et_path_delay;
  /*!< Tx ET path delay */

  rfnv_item_id_enum_type et_path_delay_vs_freq ;
  /*!< Tx ET path delay vs freq */

  rfnv_item_id_enum_type et_path_delay_offsets_vs_freq_vs_temp;
  /*!< Tx ET path delay offsets vs freq vs temp */

  rfnv_item_id_enum_type xpt_dpd_scaling;
  /*!< Tx XPT DPD scaling NV ID */

  rfnv_item_id_enum_type pin_cal_vs_freq;
  /*!< PIN Cal vs Freq NV ID */

  rfnv_item_id_enum_type rx_cal_data ;
  /*!< Rx LNA offset vs Freq NV and cal channels NV */

} rf_cdma_cal_nv_items_map_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure holding the mapping information for CDMA RF Static NV items
*/
typedef struct
{

  rfnv_item_id_enum_type rx_1x_swpts;
  /*!< 1x Rx Switchpoint Items */

  rfnv_item_id_enum_type rx_dor0_a_swpts;
  /*!< HDR Rev0 RevA Rx Switchpoint Items */

  rfnv_item_id_enum_type rx_dorb_sc_swpts;
  /*!< HDR RevB Single Carrier Rx Switchpoint Items */

  rfnv_item_id_enum_type rx_dorb_mc_swpts;
  /*!< HDR RevB MultiCarrier Rx Switchpoint Items */

  rfnv_item_id_enum_type intelliceiver_det_thresh;
  /*!< Intelliceiver Jammer Detector Threshold Adjust Items */

  rfnv_item_id_enum_type ant_tuner;
  /*!< CDMA Antenna Tuner codes */

  rfnv_item_id_enum_type bc_pa_r_map;
  /*!< Bandclass PA R Map Item */

  rfnv_item_id_enum_type enc_btf_delay;
  /*!< Enc BTF Delay Item */

  rfnv_item_id_enum_type pa_r1_rise;
  /*!< PA State 0 to PA State 1 Rise Swpt Item */

  rfnv_item_id_enum_type pa_r1_fall;
  /*!< PA State 1 to PA State 0 Fall Swpt Item */

  rfnv_item_id_enum_type pa_r2_rise;
  /*!< PA State 1 to PA State 2 Rise Swpt Item */

  rfnv_item_id_enum_type pa_r2_fall;
  /*!< PA State 2 to PA State 1 Fall Swpt Item */

  rfnv_item_id_enum_type pa_r3_rise;
  /*!< PA State 2 to PA State 3 Rise Swpt Item */

  rfnv_item_id_enum_type pa_r3_fall;
  /*!< PA State 3 to PA State 2 Fall Swpt Item */

  rfnv_item_id_enum_type access_1x_swpts;
  /*!< 1x Access Tx Switchpoint Item */

  rfnv_item_id_enum_type voice_1x_swpts;
  /*!< 1x Voice Tx Switchpoint Item */

  rfnv_item_id_enum_type data_1x_swpts;
  /*!< 1x Data Tx Switchpoint Item */

  rfnv_item_id_enum_type access_hdr_swpts;
  /*!< HDR Access Tx Switchpoint Item */

  rfnv_item_id_enum_type dor0_hdr_swpts;
  /*!< HDR Rev0 Tx Switchpoint Item */

  rfnv_item_id_enum_type dora_hdr_swpts;
  /*!< HDR RevA Tx Switchpoint Item */

  rfnv_item_id_enum_type dorb_hdr_swpts;
  /*!< HDR RevB Tx Switchpoint Item */

  rfnv_item_id_enum_type lin_vs_temp;
  /*!< Linearizer vs temperature for all PA states */

  rfnv_item_id_enum_type multi_lin_apt_adj;
  /*!< APT Adjust NV item */

  rfnv_item_id_enum_type hdr_tx_pwr_lim;
  /*!< HDR tx limits NV item  */

  rfnv_item_id_enum_type onex_spur_table;
  /*!< Spur table info for 1x */

  rfnv_item_id_enum_type hdr_spur_table;
  /*!< Spur table info for HDR */

  rfnv_item_id_enum_type tx_gtr_thresh;
  /*!< Tx GTR Threshold Indicator NV item */

  rfnv_item_id_enum_type lim_vs_temp_sar_0;
  /*!< Max power limit vs temperature for SAR state 0 */

  rfnv_item_id_enum_type lim_vs_temp_sar_1_8;
  /*!< Max power limit vs temperature for SAR states 1-8 */

  rfnv_item_id_enum_type dpd_config_params;
  /*!< Configuration items that characterize DPD */

  rfnv_item_id_enum_type hdet_vs_temp;
  /*!< Characterized HDET compensation across temp */

  rfnv_item_id_enum_type lim_vs_temp_vs_freq;
  /*! Max power limit across temperature and frequency */

  rfnv_item_id_enum_type hdet_mod_range;
  /*! Allowed range of power offset for HDET correction */

  rfnv_item_id_enum_type lin_vs_temp_vs_freq_pin;
  /*! Linearizer vs Temperature vs Frequency Pin compensation */

  rfnv_item_id_enum_type lin_vs_temp_vs_freq_pout;
  /*! Linearizer vs Temperature vs Frequency Pout compensation */

  rfnv_item_id_enum_type ant_tuner_cl;
  /*! Tuner CL Tx Blanking Power threshold */

  rfnv_item_id_enum_type pa_static_nv;
  /*! Tx PA Static NV data */

  rfnv_item_id_enum_type pa_static_nv_z;
  /*! Tx PA Static NV compressed data */

  rfnv_item_id_enum_type pred_clip_pwr_thresh ;
  /*!< Predictive Clipping Enable/Disable Power Threshold NV */

} rf_cdma_static_nv_items_map_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA RF driver Rx calibration data structure

  @details
  This data structure stores Rx calibration data that is common between
  1x and HDR modes of operation.

  The data type is independent of device and band.  All devices and bands
  utilize a consistent configuration data model.
*/
typedef struct rf_cdma_rx_cal_s
{
  /*!
    @brief
    Channels at which the calibration data was taken.

    @details
    Rx calibration supports up to 16 points at which frequency dependent
    variables can be measured.  The channel at which this calibration
    data was taken is stored in this array.  Frequency calibration
    is connected to the channel (and thereby the frequency) through
    the index location in the frequency arrays.

    This array must be arranged in increasing frequency order, and
    will be guaranteed by the NV load process.
  */
  int16 channel_list[NV_FREQ_TABLE_SIZ];

  /*!
    @brief
    Least recently used channel list
  */
  uint8 lru_channel_list[NV_FREQ_TABLE_SIZ];

  /*!
    @brief
    Frequency in KHz where the calibration data is valid.

    @details
    Interpolation of frequency dependent calibration is done based on the
    absolute frequency, instead of the channel.  Channel mapping within
    a band may be disjoint and inconsistent.  This data member stores
    the frequency in KHz of the channel in the corresponding index
    of channel_list.

    This array must be sorted in strictly increasing order.  This will
    be guaranteed by the NV load process.

    @internal
    The array is arranged with the frequency bin dimension as the last
    (in this case only) dimension to interface with the linear interpolation
    algorithms.  This eliminates the need to copy the data to a temporary
    structure when doing frequency interpolation.
  */
  uint32 freq_list[NV_FREQ_TABLE_SIZ];

  /*!
    @brief
    The number of valid entries in the frequency list.

    @details
    This variable is used to bound linear interpolation searchs to
    only examine the number of valid entries in the frequency list.
  */
  uint8 freq_list_size;

  /*!
    @brief
    Rx calibration data for each gain state in the receiver.

    @details
    Each IntelliCeiver state has NV_FREQ_TABLE_SIZ (16) entries for the
    RFM_LNA_GAIN_STATE_NUM (5) gain states that the receiver can be in.
    The gain cal data in the frequency bin dimension contains the
    calibration data taken on the channel in the corresponding index of
    the Rx calibration channel list.

    @internal
    The array is arranged with the frequency bin dimension as the last
    dimension to interface with the linear interpolation algorithms.  This
    eliminates the need to copy the data to a temporary structure when
    doing frequency interpolation.
  */
  int16 rx_gain_calibration[RFM_CDMA_POWER_MODE_MAX]
                           [RFM_LNA_GAIN_STATE_NUM]
                           [NV_FREQ_TABLE_SIZ];

} rf_cdma_rx_cal_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA RF driver Intelliceiver NV data information

  @details
  This data structure stores Rx Intelliceiver data that is common between
  1x and HDR modes of operation.

  The data type is independent of device and band.  All devices and bands
  utilize a consistent configuration data model.
*/
typedef struct rf_cdma_intelliceiver_s
{
  /*! 
    @brief
    Reference channel for which Intelliceiver Cal is done
  */
  uint16 ref_channel;

  /*! 
    @brief
    Calibration data for intelliceiver
   
    @details
    Intelliceiver calibration data is stored for each
    supported power modes for upto max jammer detector count
  */
  int16 calibration[RFM_CDMA_POWER_MODE_MAX]
                   [RF_CDMA_NV_INTELLICEIVER_DET_COUNT];

  /*! 
    @brief
    Jammer Detector Threshold Adjust values

    @details
    Allows the specification of adjustments relative to the default
    jammer detection thresholds hardcoded in software. Each threshold
    is a signed byte with 1/10th dB resolution.
  */
  int8 det_adjust[RF_CDMA_NV_INTELLICEIVER_DET_COUNT]
                 [RFM_LNA_GAIN_STATE_NUM];

} rf_cdma_intelliceiver_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to hold the CDMA Antenna Tuner NV Data
*/
typedef rfcommon_nv_ant_tuner_type rf_cdma_ant_tuner_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA RF driver 1x Rx static NV data structure

  @details
  This data structure stores static NV data for Rx functionality along the
  respective NV path. It is specific to the 1x mode of operation but is common
  across all devices and bands.
*/
typedef struct rf_1x_rx_static_s
{
  /*! 
    @brief
    LNA switch point information for all linearity modes for 1x
   
    @details
    Rx switchpoint data is stored in static NV and is loaded into the RF driver
    here for 1x mode of operation. This data contains information whether the
    switchpoints are active/inactive for a particular linearity state. Each
    switchpoint structure contains the swpt type, the rise/fall data and the
    time hysteresis data
  */
  rf_cdma_lna_info_type lna_info;

  /*! 
    @brief
    Table that contains freq at which a spur could occur and the notch settings
    to supress that spur

    @details
    The spur data is stored in static NV and is loaded into the RF driver
    here for 1X mode of operation. 
  */
  rfcommon_spur_table_type onex_spur_table;

} rf_1x_rx_static_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA RF driver CDMA Tx APT Adjust Data structure

  @details
  This data structure is used by the 1x and HDR APT adjust data structures
  to store the actual power adjustment values based on NV data. Each value
  provided by NV is a signed 1/10th dB power adjustment. This adjustment
  is represented by two values that must be provided to FW:
  - Updated IQ gain value (with reference to default)
  - LUT offset, used by FW to compensate the linearizer entries
 
  This data structure stores the precomputed information required to provide
  the above two values to FW:
  - IQ gain adjustment with reference to default, in HW units
  - LUT offset value in dB10
 
*/
typedef struct rf_cdma_tx_apt_adj_val_s
{
  /*! 
    @brief
    LUT Offset value in dBx10
   
    @details
    This value is equal to the NV entry, and provides the offset
    that must be applied to the linearizer entries.
  */
  int8 lut_offset;

  /*! 
    @brief
    IQ Gain adjust value in HW units

    @details
    This value is the adjustment that must be applied to the default
    IQ gain, based on the adjustment provided in NV (in dBx10).
   
    We store this value in 16Q16 format, which provides a resolution of
    1/65536 (2^16), when using this value to compute the new IQ gain.
    The fractional representation of this value is derived from the NV
    val using the below formula:
   
    iq_gain_adj = 10 ^ (NV_val/200)
  */
  uint64 iq_gain_adj;
  

} rf_cdma_tx_apt_adj_val_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA RF driver CDMA Tx GTR Threshold Indicator data structure

  @details
  This structure holds the feature enable, rise threshold and 
  fall threshold values for the Tx GTR Thresh feature. The 
  rise and values are in dBm10 
*/
typedef struct
{
  /*! 
    @brief 
    Indicator of whether the feature is active or not
  */
  boolean active;

  /*! 
    @brief 
    Rise threshold value in dBm10
  */
  int16 rise_value;

  /*! 
    @brief 
    Fall threshold value in dBm10
  */
  int16 fall_value;

}rf_cdma_tx_gtr_thresh_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA RF driver Rx switchpoint data structure

  @details
*/
typedef struct rf_hdr_rx_static_s
{
  /*! 
    @brief
    LNA switch point information for all linearity modes for HDR Rev0/RevA

    @details
    Rx switchpoint data is stored in static NV and is loaded into the RF driver
    here for HDR Rev0/RevA mode of operation. This data contains information
    whether the switchpoints are active/inactive for a particular linearity
    state. Each switchpoint structure contains the swpt type, the rise/fall
    data and the time hysteresis data
  */
  rf_cdma_lna_info_type lna_info_dor0_a;

  /*! 
    @brief
    LNA switch point information for all linearity modes for HDR RevB Single
    Carrier

    @details
    Rx switchpoint data is stored in static NV and is loaded into the RF driver
    here for HDR RevB SC mode of operation. This data contains information
    whether the switchpoints are active/inactive for a particular linearity
    state. Each switchpoint structure contains the swpt type, the rise/fall
    data and the time hysteresis data
  */
  rf_cdma_lna_info_type lna_info_dorb_sc;

  /*! 
    @brief
    LNA switch point information for all linearity modes for HDR RevB Multi
    Carrier

    @details
    Rx switchpoint data is stored in static NV and is loaded into the RF driver
    here for HDR RevB MC mode of operation. This data contains information
    whether the switchpoints are active/inactive for a particular linearity
    state. Each switchpoint structure contains the swpt type, the rise/fall
    data and the time hysteresis data
  */
  rf_cdma_lna_info_type lna_info_dorb_mc;

  /*! 
    @brief
    Table that contains freq at which a spur could occur and the notch settings
    to supress that spur

    @details
    The spur data is stored in static NV and is loaded into the RF driver
    here for HDR mode of operation. 
  */
  rfcommon_spur_table_type hdr_spur_table;

} rf_hdr_rx_static_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold Tx state switch point configuration
*/
typedef struct rf_cdma_tx_switch_points_s
{
  /*!
    @brief
    Array of Tx AGC fall switch points in dBm10.

    @details
    The Tx AGC will be configured to switch to state n when the total Tx AGC
    power falls below fall[n+1].  A Tx AGC state may disabled by setting the
    fall value to at least 42.0dBm (420) in low bands, or 39.0dBm (390) in
    high bands. The fall value for the lowest Tx AGC state must be set to
    minimum power in dBm10 e.g. -70 dBm (-700)
  */
  int16 fall[RFM_PA_GAIN_STATE_NUM];

  /*!
    @brief
    Array of Tx AGC rise switch points in dBm10.

    @details
    The Tx AGC will be configured to switch to state n when the total Tx AGC
    power rises above rise[n].  A Tx AGC state may disabled by setting the
    rise value to at least 42.0dBm (420) in low bands, or 39.0dBm (390) in
    high bands. The rise value for the lowest Tx AGC state must be set to
    minimum power in dBm10 e.g. -70 dBm (-700)
  */
  int16 rise[RFM_PA_GAIN_STATE_NUM];

  /*!
    @brief
    Time in microseconds to wait before falling to state n after the total
    Tx AGC power drops below rise[n+1], if not below fall[n].

    @details
    Tx AGC will fall to the next lower state whenever the power has been
    below the rise value for more than fall_hold_time microseconds.  The
    hold timer is disregarded as soon as the power drops below fall[n],
    so this timer will not guarantee that Tx AGC state switching does not
    occur very rapidly.
  */
  int16 fall_hold_time;
} rf_cdma_tx_switch_points_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA RF driver 1x Tx static NV data structure

  @details
  This data structure stores static NV data for Tx functionality along the
  respective NV path. It is specific to the 1x mode of operation but is common
  across all devices and bands.
*/
typedef struct rf_1x_tx_static_s
{
  /*! 
    @brief
    Tx AGC switch point configuration when the radio is in access state.
  */
  rf_cdma_tx_switch_points_type access_swpts;

  /*! 
    @brief
    Tx AGC switch point configuration when the radio is in voice traffic state,
    (supplemental channel is not active).
  */
  rf_cdma_tx_switch_points_type voice_swpts;

  /*! 
    @brief
    Tx AGC switch point configuration when the radio is in data traffic state,
    for the following reverse supplemental channel rates:
    [0] - 9600 bps
    [1] - 19200 bps
    [2] - 38400 bps
    [3] - 76800 bps
    [4] - 153600 bps
    [5] - 307200 bps
  */
  rf_cdma_tx_switch_points_type 
    data_swpts_rsch[RF_CDMA_NV_1X_R_SCH_RATES_NUM];

  /*! 
    @brief
    Data structure to hold APT Adjust values

    @details
    The data describes how Tx linearizers stored in the multi_lin data
    structures must be adjusted based on the reverse link data
    configuration, in order to optimize the PA bias based on waveform.

    This data structure stores static NV data for Tx functionality along the
    respective NV path. The data describes how Tx linearizers stored in the
    multi_lin data structures must be adjusted based on the reverse link data
    configuration, in order to optimize the PA bias based on waveform. The
    data specifically contains power adjustments that must be made to the
    linearizer tables and Tx DAC set point. 
  */
  rf_cdma_tx_apt_adj_val_type apt_adj_value[RF_CDMA_NV_1X_TX_IQGAIN_RC_NUM];

} rf_1x_tx_static_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA RF driver HDR Tx static NV data structure

  @details
  This data structure stores static NV data for Tx functionality along the
  respective NV path. It is specific to the HDR mode of operation but is common
  across all devices and bands.
*/
typedef struct rf_hdr_tx_static_s
{
  /*! 
    @brief
    Tx AGC switch point configuration when the data radio is in access state.
  */
  rf_cdma_tx_switch_points_type access_swpts;

  /*! 
    @brief
    Tx AGC switch point configuration when the radio is in data traffic state,
    for the following RRI rates:
    [0] - 0 bps
    [1] - 9600 bps
    [2] - 19200 bps
    [3] - 38400 bps
    [4] - 76800 bps
    [5] - 153600 bps
  */
  rf_cdma_tx_switch_points_type 
    dor0_swpts_rri[RF_CDMA_NV_HDR_RRI_RATES_NUM];

  /*! 
    @brief
    Tx AGC switch point configuration when the data radio is in EV-DO Rev.A
    state
  */
  rf_cdma_tx_switch_points_type dorA_swpts;

  /*! 
    @brief
    Tx AGC switch point configuration when the data radio is in EV-DO Rev.B
    multi-carrier state
  */
  rf_cdma_tx_switch_points_type dorB_mc_swpts;

  /*! 
    @brief
    Data structure to hold APT Adjust values

    @details
    The data describes how Tx linearizers stored in the multi_lin data
    structures must be adjusted based on the reverse link data
    configuration, in order to optimize the PA bias based on waveform.

    The data specifically contains power adjustments that must be made to the
    linearizer tables and Tx DAC set point, for different RRIs for Rev0 and
    Payload sizes for RevA.   
  */
  rf_cdma_tx_apt_adj_val_type apt_adj_value[RF_CDMA_NV_HDR_TX_IQGAIN_RC_NUM];

  /*! 
    @brief
    The data holds the static data for HDR PWR LIMITS.   

    @details
    The data holds the static data for HDR PWR LIMITS.   
  */
  rf_cdma_tx_pwr_limit_data_type* tx_pwr_limit_data ;
} rf_hdr_tx_static_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  A data structure to contain all the Thermal Characterization data.

  @details
  This data structure contains all the Thermal Characterization data along the
  respective NV path. 
*/

typedef struct
{
   /*! 
    @brief
    This flag holds the monotonicty information about the therm cal data.
  */
  boolean is_therm_cal_nv_data_valid ;

 /*! 
    @brief
    Tx Thermistor calibration data, stored as therm bins for use by
    temperature compensation
  */
  uint16 tx_therm_cal[RF_CDMA_NV_TEMP_LIMIT_SIZE] ;
} rf_cdma_therm_properties_type ;
 
/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA RF driver Tx HDET cal Data structure

  @details
  This data structure stores the HDET calibration data that is common between
  1x and HDR modes of operation. A copy of this data exists for normal high
  power mode and low power mode HDEt operation.

  The data type is independent of device and band.  All devices and bands
  utilize a consistent configuration data model.
*/
typedef struct rf_cdma_tx_hdet_cal_s
{
  /*! 
    @brief
    Specifies whether HDET operation is enabled in normal high power mode
  */
  boolean enabled;

  /*! 
    @brief
    Specifies the power level at which HDET measurements start for
    calculating exp_hdet_vs_agc

    @details
    In normal HDET operation, for low bands, the start value for the
    steps is +16.4dBm and for high bands, it is +13.4dBm.
   
    For LPM HDET operation, the start power level is required to be loaded
    from NV. The start power level is stored in 16-bit signed
    2's complement form in units of 1/10thdB. For example,
    +15dBm = 150 (0x0096)
    -2.6dBm = -26 (0xFFE6)
  */
  int16 start_pwr_lvl;

  /*! 
    @brief
    Provides the HDET ADC reading corresponding to AGC value obtained from
    calibration at reference channel
   
    @details
    The expected HDET value vs AGC reading obtained from calibration
    allows software to reverse map the HDET value read during power
    limiting to convert to AGC value. Values range from 0-4095 and
    are stored in steps of 1.6 dB for 16 power levels.
  */
  uint16 exp_hdet_vs_agc[RF_CDMA_NV_EXP_HDET_VS_AGC_SIZE];

  /*! 
    @brief
    Provides the HDET ADC reading that corresponds approximately to
    the desired Tx limit power minus 6 dB
   
    @details
    If the desired maximum power limit is +24dBm, this value
    corresponds to the HDET reading when Tx power is 24-6 = +18dBm.
  */
  uint16 offset;

  /*! 
    @brief
    Provides the HDET ADC reading that corresponds approximately to the
    desired Tx limit power plus 2dB, minus the hdet_off
   
    @details
    If the desired maximum power limit is +24dBm,
    calibration identifies the HDET reading when Tx power is +26dBm.
    Subtracting the hdet_off from this HDET reading gives hdet_spn
  */
  uint16 span;

} rf_cdma_tx_hdet_cal_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA RF driver Tx Power Limiting Data structure

  @details
  This data structure stores Tx Power Limiting data that is common between
  1x and HDR modes of operation.

  The data type is independent of device and band.  All devices and bands
  utilize a consistent configuration data model.
*/
typedef struct rf_cdma_tx_lim_cal_s
{
  /*! 
    @brief
    Provides compensation for max Tx power variation over frequency

    @details
    For each frequency in the Tx calibration list, max power
    variation based on HDET reading is stored in NV.
    Frequency power-offsets are stored in dBm*10
  */
  int8 hdet_vs_freq[RF_CDMA_NV_FREQ_LIST_SIZE];

  /*! 
    @brief
    Provides HDET calibration data for normal high power mode HDET operation
  */  
  rf_cdma_tx_hdet_cal_type hpm_hdet;

  /*! 
    @brief
    Provides the power level, in dBm10, at which LPM HDET operation
    becomes active, if enabled
  */  
  uint16 lpm_threshold;

  /*! 
    @brief
    Provides HDET calibration data for low power mode HDET operation
  */
  rf_cdma_tx_hdet_cal_type lpm_hdet;

} rf_cdma_tx_lim_cal_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA RF driver ET related structure for holding NV items.

  @details
  This data structure stores ET related NV items. 

  The data type is independent of device and band.  All devices and bands
  utilize a consistent configuration data model.
*/

typedef struct 
{
  /*! 
    @brief
    Keeps the version. 
  */	
 uint8      version ;
 
 /*! 
    @brief
    Out ofall the bins available not all might be used. This
    variable contains the number of bins used. Valid values 
    are from 0 to maximum [MAX_FREQ_COMP_ET_PATH_DELAY_BINS] bins. 
  */
 uint8      number_of_valid_entries ;
 
 /*! 
    @brief
    Indicates whether the delay values are calibrated or characterized. 
  */
 uint8      is_delay_cald;
 
 /*! 
    @brief
    The path delay is for a bunch of channels with contigous frequency.
    This variable holds the channel with highest frequency that defines
    the bin.  
  */
 uint32     upper_bound_channel[MAX_FREQ_COMP_ET_PATH_DELAY_BINS] ;
 
  /*! 
    @brief
    The frequency is needed to ascertain the bin. Instead of computing the 
    frequency MAX_FREQ_COMP_ET_PATH_DELAY_BINS times, we store these numbers
    once the validity of the NVs have been positively ascertained.
  */
 uint32     upper_bound_freq_KHz[MAX_FREQ_COMP_ET_PATH_DELAY_BINS] ;
  
 /*! 
    @brief
    Provides path delay for a given frequency range. The frquency range is 
    defined by bins. 
  */
 int32      et_path_delay_vs_freq[MAX_FREQ_COMP_ET_PATH_DELAY_BINS] ;
 
  /*! 
    @brief
    Provides the information if the et path delay vs freq NV is usable or not.
    If this variable is unset, legacy ET delay NV is used.
  */
 boolean    is_et_path_delay_vs_freq_valid ;

 /*! 
    @brief
    Contains et path delay offsets vs freq vs temp NV data.
  */
 rfnv_tx_delay_offsets_type et_path_delay_offsets_vs_freq_vs_temp;

} rf_cmda_et_nv_items_type ;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA RF driver Tx calibration data structure

  @details
  This data structure stores Tx calibration data that is common between
  1x and HDR modes of operation.

  The data type is independent of device and band.  All devices and bands
  utilize a consistent configuration data model.
*/
typedef struct rf_cdma_tx_cal_s
{
  /*! 
    @brief
    Tx HDET calibration data   
  */
  rf_cdma_tx_lim_cal_type tx_lim_cal;

  /*! 
    @brief
    Tx calibration data for multi-chan linearizer for
    this device and band   
  */
  tx_band_cal_type tx_multi_lin_cal;

  /*! 
    @brief
    All Therm cal related data like, Tx Thermistor calibration data, 
    it's validity, stored as therm bins for use by temperature compensation.
  */
  rf_cdma_therm_properties_type therm_cal_data ;

  /*! ET path delay related information */
  rf_cmda_et_nv_items_type et_path_delay_info ;

  /*! XPT DPD scaling data */
  xpt_dpd_scaling_data_type xpt_dpd_scaling_data;

} rf_cdma_tx_cal_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA RF driver Tx linearizer vs temperature data structure

  @details
  This data structure stores the Tx linearizer vs temperature NV data for 1x
  and HDR modes of operation. CDMA temperature compensation uses both
  slope as well as offset to compute final Tx linearizer power.

  The data type is independent of device and band.  All devices and bands
  utilize a consistent configuration data model.
*/
typedef struct rf_cdma_tx_lin_vs_temp_s
{
  /*! 
    @brief
    Tx linearizer vs temperature slope, at the points
    specified by the THERM_BINS NV item.

    @details
    This item contains the temperature compensation slope adjustment. This
    value is stored as an unsigned fractional integer in Q15 format. For
    no slope adjustment, 32768 should be programmed, which is equal to 1.0
    in Q15 format.

    The slope adjustment is provided at 8 discrete points identified by
    the THERMS_BIN NV item.

    The actual linearizer compensation is computed using:
    TxPowerComp = (CompSlope/32768)*TxPowerNV + CompOffset
    where,
    CompSlope -> slope adjustment
    TxPowerNV -> Power value to be compensated
    CompOffset -> fixed compensation value   
  */
  int16 slope[RF_CDMA_NV_TEMP_LIMIT_SIZE];

  /*! 
    @brief
    Tx linearizer vs temperature offset, at the points
    specified by the THERM_BINS NV item.

    @details
    This item contains the temperature compensation fixed offset. This
    value is stored in dBx10 and is provided at 8 discrete points
    identified by the THERMS_BIN NV item.

    The actual linearizer compensation is computed using:
    TxPowerComp = (CompSlope/32768)*TxPowerNV + CompOffset
    where,
    CompSlope -> slope adjustment
    TxPowerNV -> Power value to be compensated
    CompOffset -> fixed compensation value   
  */
  int8 offset[RF_CDMA_NV_TEMP_LIMIT_SIZE];

} rf_cdma_tx_lin_vs_temp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA RF driver CDMA PC Power Threshold data structure
*/
typedef struct
{
  /*! 
    @brief 
    Enable threshold value in dBm10
  */
  int16 enable_threshold;

  /*! 
    @brief 
    Disable threshold value in dBm10
  */
  int16 disable_threshold;

}rf_cdma_tx_pred_clip_nv_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA RF driver Tx static data structure

  @details
  This data structure stores Tx static NV data that is common between
  1x and HDR modes of operation.

  The data type is independent of device and band.  All devices and bands
  utilize a consistent configuration data model.
*/
typedef struct rf_cdma_tx_static_s
{
  /*! 
    @brief
    PA static NV header information
   
    @details
    Stores the NV version information for PA static NV data
  */
  int16 pa_static_nv_version;

  /*! 
    @brief
    PA static NV number of active PA states

    @details
    Stores the number of active Tx AGC states out of a maximum of 8,
    as provided by NV data
  */
  int16 num_active_pa_states;

  /*!
    @brief
    PA Range Map information

    @details
    This item contains the PA_R0 and PA_R1 logic level for the four
    Tx AGC PA states.
   
    The four 2-bit R1/R0 values are packed into the single 8-bit item,
    starting with PA state 0 in the two LSBs, ending with PA state 3 in
    the two MSBs.
    Bit 7: PA_R1 when in PA state 3
    Bit 6: PA_R0 when in PA state 3
   
    Bit 5: PA_R1 when in PA state 2
    Bit 4: PA_R0 when in PA state 2
   
    Bit 3: PA_R1 when in PA state 1
    Bit 2: PA_R0 when in PA state 1
   
    Bit 1: PA_R1 when in PA state 0
    Bit 0: PA_R0 when in PA state 0
  */
  byte tx_pa_r_map;

  /*!
    @brief
    PA Range Map information

    @details
    This item contains the PA_R0 and PA_R1 logic level for the eight
    logical Tx AGC PA states.
  */
  int16 pa_r_map[RFM_PA_GAIN_STATE_NUM];

  /*!
    @brief
    Tx limit vs temperature for all DSI SAR states, at the points specified
    by the THERM_BINS NV item.

    @details
    This item contains the limit in dBm10 units that is applied at each
    ADC reading point in the THERM_BINS NV item.  Between the ADC points,
    the temperature limit is linearily interpolated based on the two nearest
    values.

    Each DSI SAR state has its own limit vs temperature curve, allowing the
    apps processor to control the transmit power limit based on conditions
    detected external to the modem.
  */
  int16 lim_vs_temp[RF_CDMA_NV_DSI_SAR_STATE_SIZE][RF_CDMA_NV_TEMP_LIMIT_SIZE];

  /*! 
    @brief
    Tx linearizer vs temperature for all PA states, at the points
    specified by the THERM_BINS NV item.

    @details
    This item contains the temperature compensation slope adjustment and
    fixed offset, both of which are provided at 8 discrete points
    identified by the THERMS_BIN NV item.

    The actual linearizer compensation is computed using:
    TxPowerComp = (CompSlope/32768)*TxPowerNV + CompOffset
    where,
    CompSlope -> slope adjustment
    TxPowerNV -> Power value to be compensated
    CompOffset -> fixed compensation value   
  */
  rf_cdma_tx_lin_vs_temp_type lin_vs_temp[RF_CDMA_NV_TX_LINEARIZER_NUM];

  /*! 
    @brief
    Data structure to hold CDMA Tx threshold indicator values

    @details
    This structure holds the feature enable, rise threshold and 
    fall threshold values for the Tx GTR Thresh feature. The rise 
    and values are in dBm10
  */
  rf_cdma_tx_gtr_thresh_type tx_gtr_thresh_data;

  /*! 
    @brief
    Data structure to hold CDMA Tx EPT values
   
    EPT DPD static params for expansion and processing
   
    @details
    This structure holds parameters used for EPT DPD. Each 32-bit fields in
    the array corresponds to one EPT parametere.
   
    - XPT_CFG                         0
    - EPT_CFG_CMN_EXP_SCALE           1
    - EPT_CFG_CMN_SEARCH_CENTER       2
    - EPT_CFG_CMN_SEARCH_WIDTH        3
    - EPT_CFG_CMN_EXP_SCALE_Q_FACT    4
    - EPT_CFG_CMN_LIN_TX_MAG          5
    - EPT_CFG_CMN_PM_ORDER            6
    - EPT_CFG_CMN_EVN_SCALE_UNITY     7
    - EPT_CFG_CMN_IQ_GAIN_UNITY       8
    - EPT_CFG_W_RGI_SEL_MAX_PWR       9
    - EPT_CFG_W_RGI_SEL_OFFSET_LOW_0  10
    - EPT_CFG_W_RGI_SEL_OFFSET_LOW_1  11
    - EPT_CFG_W_RGI_SEL_OFFSET_LOW_2  12
    - EPT_CFG_W_RGI_SEL_OFFSET_HIGH_0 13
    - EPT_CFG_W_RGI_SEL_OFFSET_HIGH_1 14
    - EPT_CFG_W_RGI_SEL_OFFSET_HIGH_2 15
    - MAX_EPT_NUM_CONFIG_PARAMS       32
  */
  uint32 ept_cfg_params[MAX_EPT_NUM_CONFIG_PARAMS];
  
  /*! 
    @brief
    Data structure to hold HDET vs Temp static NV data
   
    @details
    Holds the compensation offsets that are characterized for HDET
    response over temperature. The offsets are provided in dB10 and
    correspond to the temperature bins provided by NV_THERM_BINS i.e.
    a total of 8 compensation offsets.
  */
  int8 hdet_vs_temp[RF_CDMA_NV_TEMP_LIMIT_SIZE];

  /*! 
    @brief
    Provides characterized NV data for absolute max power limiting values
    across temperature and frequency
   
    @details
    Holds the 2-dimensional characterized representation of absolute
    max power limits, varying across temperature on the X-axis and
    frequency on the Y-axis. The characterization is provided for upto
    16 frequencies of interest across the band, for every temperature
    bin provided by NV_THERM_BINS (upto 8). Values are in dBmx10
  */
  int16 lim_vs_temp_vs_freq[RF_CDMA_NV_TEMP_LIMIT_SIZE]
                           [RF_CDMA_NV_FREQ_LIST_SIZE];

  /*! 
    @brief
    Holds the Tx power offset range values for which HDET modification
    to the linearizer is acceptable
  */
  int16 hdet_mod_range[RF_CDMA_NV_HDET_MOD_RANGE_NUM]
                      [RF_CDMA_NV_TEMP_LIMIT_SIZE];

  /*! 
    @brief
    Holds Linearizer Temperature compensation static NV data from
    legacy as well as Pin/Pout Temp vs Freq NV items. This data stores
    all lin vs temp information and is used to run temperature compensation
    across different NV versions
  */
  rfcom_txlin_temp_comp_nv_data_type lin_temp_comp_data;

  /*! 
    @brief
    Pointer to memory allocated by rfcommon_nv_ant_tuner module to hold 
    Antenna Tuner Close Loop Control data.  
  */
  rfcommon_nv_ant_tuner_cl_ctrl_type ant_tuner_cl;

  /*! 
    @brief
    Holds CDMA Predictive Cliper Power threshold data
  */
  rf_cdma_tx_pred_clip_nv_type pred_clip_nv;

} rf_cdma_tx_static_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA RF driver configuration structure to store all common items for a
  single device on a single band.

  @details
  This data structure stores the items necessary to configure a device
  for a band that are common to both 1x and HDR, which would waste memory
  space if they were duplicated in the 1x and HDR structures.  The 1x
  and HDR data structures will contain a pointer to the common structure
  that contains the shared data.

  The data type is independent of device and band.  All devices and bands
  utilize a consistent configuration data model.
*/
typedef struct rf_cdma_config_s
{
  /*!
    @brief
    Indicates what NV chain this structure was initialized with.
  */
  const rf_cdma_nv_path_type path;

  /*!
    @brief
    Indicates what band this structure was initialized with.
  */
  const rfm_cdma_band_class_type band;

  /*!
    @brief
    Rx calibration data for this device and band.
  */
  rf_cdma_rx_cal_type rx_cal;

  /*!
    @brief
    Intelliceiver data for this device and band.
  */
  rf_cdma_intelliceiver_type ic_data;

  /*!
    @brief
    Antenna Tune data for this device and band.
  */
  rf_cdma_ant_tuner_type ant_tuner;

  /*!
    @brief
    Tx calibration data for this device and band.
  */
  rf_cdma_tx_cal_type tx_cal;

  /*!
    @brief
    Tx static NV data for this device and band.
  */
  rf_cdma_tx_static_type tx_static;

} rf_cdma_config_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold 1x NV Configuration
*/
typedef struct rf_1x_config_s
{
  /*!
    @brief
    The reference to the device and band data that is common between 1x and
    HDR.
  */
  rf_cdma_config_type* const cdma_common_ptr;

  /*! 
    @brief
    Device and band Rx NV data when the device is operating in 1x mode
  */
  rf_1x_rx_static_type rx_static;

  /*! 
    @brief
    Device and band Tx static NV data for 1x mode
  */
  rf_1x_tx_static_type tx_static;

  /*! 
    @brief
    Device and band specific BTF (back-to-the-future) value when the device
    is in 1x mode
  */
  uint32 enc_btf_delay;

} rf_1x_config_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold HDR NV Configuration
*/
typedef struct rf_hdr_config_s
{
  /*!
    @brief
    The reference to the device and band data that is common between 1x and
    HDR.
  */
  rf_cdma_config_type* const cdma_common_ptr;

  /*! 
    @brief
    Device and band Rx NV data when the device is operating in HDR mode
  */
  rf_hdr_rx_static_type rx_static;

  /*! 
    @brief
    Device and band Tx static NV data for HDR mode
  */
  rf_hdr_tx_static_type tx_static;

} rf_hdr_config_type;


/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_device_data_is_valid
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band
);

/*----------------------------------------------------------------------------*/
const rf_1x_config_type*
rf_cdma_nv_get_1x_data
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band
);

/*----------------------------------------------------------------------------*/
const rf_hdr_config_type*
rf_cdma_nv_get_hdr_data
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_load
(
  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_nv_cleanup
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_load_rx_chan_list
(
  rf_cdma_rx_cal_type* const cal_struct,
  const rfnv_item_id_enum_type rx_channel_list_item,
  const rfnv_item_id_enum_type rx_channel_lru_list_item,
  const rfm_cdma_band_class_type band,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_load_rx_cal
(
  rf_cdma_rx_cal_type* const cal_struct,
  const rfm_lna_gain_state_type rx_gain_state,
  const rfnv_item_id_enum_type gain_offset_base_item,
  const rfnv_item_id_enum_type gain_offset_freq_item,
  const int num_power_modes,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_load_1x_rx_swpts
(
  rf_1x_rx_static_type* rx_1x_ptr,
  const rfnv_item_id_enum_type rx_swpt_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_load_dor0_a_rx_swpts
(
  rf_1x_rx_static_type *rx_1x_ptr,
  rf_hdr_rx_static_type *rx_hdr_ptr,
  const rfnv_item_id_enum_type rx_swpt_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_load_dorb_sc_rx_swpts
(
  rf_hdr_rx_static_type *rx_hdr_ptr,
  const rfnv_item_id_enum_type rx_swpt_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_load_dorb_mc_rx_swpts
(
  rf_hdr_rx_static_type *rx_hdr_ptr,
  const rfnv_item_id_enum_type rx_swpt_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_load_tx_chan_list
(
  tx_band_cal_type* const cal_struct,
  const rfnv_item_id_enum_type tx_channel_list_item,
  const rfm_cdma_band_class_type band,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_load_legacy_tx_cal
(
  rf_cdma_nv_path_type nv_path,
  tx_band_cal_type* const cal_struct,
  const rfm_pa_gain_state_type pa_gain_state,
  const rfnv_item_id_enum_type tx_rgi_nv_item,
  const rfnv_item_id_enum_type tx_lin_nv_item,
  const rfnv_item_id_enum_type tx_lin_freq_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_load_tx_pa_r_map
(
  rf_cdma_tx_static_type* const static_nv_struct,
  const rfnv_item_id_enum_type bc_pa_r_map_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_load_tx_lim_cal
(
  rf_cdma_tx_lim_cal_type* const cal_struct,
  const rfm_cdma_band_class_type band,
  const rfnv_item_id_enum_type lim_vs_freq_item,
  const rfnv_item_id_enum_type hdet_vs_freq_item,
  const rfnv_item_id_enum_type exp_hdet_vs_agc_item,
  const rfnv_item_id_enum_type hdet_off_item,
  const rfnv_item_id_enum_type hdet_spn_item,
  const rfnv_item_id_enum_type lpm_hdet_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_load_intelliceiver_cal
(
  rf_cdma_intelliceiver_type* const ic_struct,
  const rfnv_item_id_enum_type ic_cal_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_load_intelliceiver_det_thresh
(
  rf_cdma_intelliceiver_type* const ic_struct,
  const rfnv_item_id_enum_type ic_det_thresh_nv_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_load_enc_btf_dly
(
  rf_1x_config_type* const rf_1x_nv_ptr,
  const rfnv_item_id_enum_type enc_btf_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_load_old_pa_swpts
(
  rfm_cdma_band_class_type band,
  rf_1x_tx_static_type* tx_1x_ptr, 
  const rfnv_item_id_enum_type pa_r1_rise_item,
  const rfnv_item_id_enum_type pa_r1_fall_item,
  const rfnv_item_id_enum_type pa_r2_rise_item,
  const rfnv_item_id_enum_type pa_r2_fall_item,
  const rfnv_item_id_enum_type pa_r3_rise_item,
  const rfnv_item_id_enum_type pa_r3_fall_item,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
rf_cdma_nv_common_items_type*
rf_cdma_nv_get_common_table
(
  void
);

/*----------------------------------------------------------------------------*/
rf_cdma_cal_nv_items_map_type*
rf_cdma_nv_get_calibration_table
(
  rfm_cdma_band_class_type band,
  rf_cdma_nv_path_type nv_path
);

/*----------------------------------------------------------------------------*/
rf_cdma_static_nv_items_map_type*
rf_cdma_nv_get_static_nv_table
(
  rfm_cdma_band_class_type band,
  rf_cdma_nv_path_type nv_path
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_write
(
  rfnv_item_id_enum_type nv_item,
  void *data_ptr
);

/*----------------------------------------------------------------------------*/
int32
rf_cdma_nv_add_cal_chan
(
  rf_cdma_nv_path_type nv_path,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan
);
/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_update_gain_cal
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band,
  int channel,
  rfm_cdma_power_mode_type power_mode,
  rfm_lna_gain_state_type gain_state,
  int gain
);

/*----------------------------------------------------------------------------*/
int16
rf_cdma_nv_get_gain_cal
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band,
  int channel,
  rfm_cdma_power_mode_type power_mode,
  rfm_lna_gain_state_type gain_state
);

/*----------------------------------------------------------------------------*/
const
rf_cdma_tx_cal_type*
rf_cdma_nv_get_tx_cal
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band
);

/*----------------------------------------------------------------------------*/
const
rf_cdma_tx_static_type*
rf_cdma_nv_get_tx_static
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band
);

/*----------------------------------------------------------------------------*/
const
tx_band_cal_type*
rf_cdma_nv_get_tx_multilin_cal
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band
);

/*----------------------------------------------------------------------------*/
rfcommon_core_convert_freq_handler_type
rf_cdma_nv_get_chan_to_tx_freq_handler
(
  rfm_cdma_band_class_type band
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_load_hdr_spur_info
(
  rf_hdr_rx_static_type *rx_hdr_ptr,
  const rfnv_item_id_enum_type spur_table_nv_item,
  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
);


/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_load_1x_spur_info
(
  rf_1x_rx_static_type *rx_1x_ptr,
  const rfnv_item_id_enum_type spur_table_nv_item,
  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
rfcommon_core_convert_chan_handler_type
rf_cdma_nv_get_tx_freq_to_chan_handler
(
  uint8 band
);

/*----------------------------------------------------------------------------*/
tx_band_cal_type*
rf_cdma_nv_get_mutable_tx_multilin_cal
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band
);

/*----------------------------------------------------------------------------*/
rfcommon_core_txlin_pin_cal_vs_freq_type*
rf_cdma_nv_get_mutable_tx_pin_cal
(
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_get_txlin_cal_data
(
  uint8 band,
  uint8 device,
  rfnv_item_id_enum_type* item_code,
  tx_band_cal_type **tx_band_cal_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_get_hdet_vs_agc_nvid
(
  uint8 band,
  uint8 device,
  rfnv_item_id_enum_type* item_code
);


/*----------------------------------------------------------------------------*/
boolean
rf_cdma_nv_load_unified_rx_cal
(
  rf_cdma_rx_cal_type* const cal_struct,
  const rfnv_item_id_enum_type rx_channel_list_item,
  const rfm_cdma_band_class_type band,
  const rf_cdma_nv_path_type nv_path,

  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
) ;

#ifdef T_WINNT
#error code not present
#endif

#ifdef __cplusplus
}
#endif

#endif /* RF_CDMA_NV_H */
