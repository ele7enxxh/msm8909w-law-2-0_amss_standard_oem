
/*!
  @file
  rfcommon_core_txlin.c

  @brief
  Provides common TX Linearizer Functionality

  @details

*/

/*===========================================================================

Copyright (c) 2011 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/src/rfcommon_core_txlin.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/16/16   cv      [port from TH]Converted common linearizer channel to 32 bits
12/20/15   vr      auto-pin check-in
12/09/15   ag      Fix issue of pin not getting updated after skip pot changes
09/17/15   kma     Added auto pin support for Pin cal
09/01/15   vs      Add support for RGI based phase compensation 
03/03/15   cd      KW error fixes
03/03/15   cd      Fix ILPC issue by enabling capping algorithm only if capped 
                   power is found for the PA state
                   - KW error fixes
02/27/15   whz     Change the timing of GRFC PA reset write
02/23/15   vc      Fix klocwork errors
02/18/15   whz     Postpone the reset write of GRFC PA
02/17/15   vc      Fix klocwork warnings
02/17/15   cd      TxAGC updates for max power override support
02/10/15   cd      Temp changes until tech support is available
02/10/15   cd      TxAGC updates for max power override support
02/04/15   vr      Check for NULL pointer in sorting functionality
11/24/14   kai     Only update the max calibrated tx power for the highest PA state in temp comp
10/21/14   px      Added RFM Device support for PAPM 
09/05/14   cd      Add checks to handle NULL device pointers
07/28/14   cd      Fix off-target compiler error due to variable decl
07/28/14   cd      Fix KW errors in WCDMA/TDS components
07/28/14   cd/zhh  - Fixed KW errors
                   - Fixed get logging data functionality
07/25/14   cd      Add Tx linearizer data logging support and provide interface
                   for tech to query
07/10/14   cd/cri  Merge Pin Cal
06/16/14   vs/kai  Non-0dB Pin SW implementation 
05/13/14   zhw     Common TxAGC GRFC PA support
05/13/14   sw      Initialize event handle to NULL before use
05/07/14   ka      Common tx agc / tx lin fix for fbrx gain state setting
05/05/14   cd      Check num trans for SSBI vs RFFE WTR task configuration
05/01/14   cd      Move append Tx AGC script after existing tasks
04/29/14   APU     Fix KW errors.
04/29/14   APU     Added support for 2 SV Tx devices.
04/23/14   cd      Fix KW errors
04/21/14   cd      Query PA device for variable ICQ address information
04/15/14   cd      Support for 16-bit 2 PA ICQ bias register configuration
04/15/14   cd      - FBRx gain state interface update
                   - SSBI vs RFFE WTR task creation update
04/11/14   cd      Tx AGC timline optimization
04/09/14   sn      Updated PA interface
04/08/14   cd      Support for ICQ address per PA range
03/27/14   zhw     Add SSBI support in WTR trig data/addr query
03/18/14   zhw     Fix missing SSBI support in TxAGC trig & dummy script
03/10/14   hm      Hook-up of FBRX enable API enhancement
03/05/14   cd      Support for loading freq comp index and full bias lin size to 
                   RFLM
02/13/14   cd      Fix index to const bias table mapping for init linearizer
02/04/14   cd      Add Tx lin data initialization for memory savings
01/22/14   cd      Added support for split static and dynamic linearizer 
                   loading
01/13/14   zhw     Memset after alloc during v1 linearizer loading to fix 
                   uninitialized fields such as iq_gain_offset.   
01/07/14   cd      Added null handler support for xPT info
01/06/14   cd      Linearizer and Tx AGC updates for loading xPT information to 
                   RFLM
12/18/13   cd      Optimize buffer creation in linearizer loading to reduce
                   timeline
12/12/13   cd      Provide FBRx gain state data to RFLM during linearizer 
                   loading
11/25/13   cd      Add support to load DPD info during Tx override
11/22/13   cd      Load XPT table info to RFLM
11/21/13   cd      Optimization for Tx override linearizer population
11/20/13   cd      - Added separate API to populate linearizer data per row - 
                   shared by online Tx AGC and Tx override
                   - Added handling for invalid linearizer data. Populate min
                   power to RFLM
                   - Added time profiling for common Tx lin APIs
11/14/13   cd      - Support for RFLM-based Tx AGC override
                   - Updated interface for passing in device info and PA STM 
                   info
11/05/13   cd      Common Tx AGC workarounds for:
                   - Issue Seq Monitor error due to strict timing values
10/25/13   cd      Fixed PA gain-range/ICQ query with correct PA on/off info
10/24/13   cd      Fixed compiler warnings
10/16/13   cd      IQ gain offset must be provided to RFLM
10/16/13   cd      Add support for querying IREF LUT index
10/15/13   cd      Added new APIs to split config linearizer and update 
                   linearizer APIs
10/14/13   rmb     Add support to get current tx linearizer freq comp table.
10/11/13   cd      Support to load front end address configuration
10/11/13   shb     Added argument to execute FBRX enable script inside device
10/07/13   cd      Force new task for WTR Gain word
10/05/13   cd      Abort Tx AGC event before clean-up
10/05/13   cd      Abort Tx AGC event to ensure second-time configuration
10/04/13   cd      - Added padding for invalid linearizer entries in pin and
                     pout comp
                   - Added linearizer loading hookup to NV tables
10/02/13   cd      - Fixed task creation for PA/QPOET multiple transactions
                   - Added API to finalize sequences in RFLM
10/02/13   cd      Load linearizer functionality
10/01/13   cd      Renamed TxAGC APIs and added execution type param
10/01/13   cd      Uncommented APIs for TxAGC device interface
09/30/13   cd      Check in skeleton for load linearizer
09/30/13   cd      Add support to load common TxAGC event information to RFLM DM
09/17/13   cd      Common AGC. Config AGC and linearizer interface check-in
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
06/17/13   rmb/cd  Allocate interim tx linearizer on the heap in rfcommon_core_txlin_temp_vs_freq_comp_update().
06/07/13   jf      Fix LTE freq comp
05/28/13   cd      Allocate interim tx linearizer on the heap
05/21/13   Saul    MultiLin. Changed debug message.
05/16/13   cd      Temp Comp - Fixed scenario where both legacy and new Temp 
                   comp NVs are present in the phone
05/10/13   cd      Temp Comp - Pin corner case handling should use different
                   transfer factors for APT/EPT/ET
05/10/13   cd      Temp Comp - Normalize Pin and Pout values differently 
                   w.r.t frequency
05/08/13   rsr     Added Therm ADC  reading during calibration to Multilin V3 NV
05/07/13   cd      Fixed therm bins usage for multilin v3 legacy NV algorithm
05/06/13   cd      Lower bound for channel list size check
04/29/13   cd      Apply Pin alignment during calibration
04/28/13   cd      Temp Comp - Fixed compiler warnings
04/27/13   cd      Temp Comp - Added changes for characterized channel list 
                   and computing ref linearizer frequency
04/26/13   cd      Temp Comp - Fixed compiler warnings
04/22/13   cd      Temp Comp - support for multi-chan linearizer compensation
                   - Multi-algorithm support based on NV versioning
04/09/13   cd      XPT Temp Comp, corner case handling for Pin RGI run-out
04/04/13   cd      XPT Temp Comp, add initial support for multi-chan 
                   calibrated linearizer
03/29/13   cd      XPT Temp Comp, Pin NV polarity needs to be handled 
                   correctly
03/26/13   cd      XPT Temp Comp, avoid subtracting offset for multi-chan 
                   linearizer
03/25/13   cd      MultiLin. Skip Freq bin index search if param validation 
                   fails
03/25/13   cd      MultiLin. Multi-channel NV loading fixes
03/25/13   Saul    MultiLin. Fixed DPD index for multi-channels.
03/21/13   Saul    MultiLin. Hooked up V3 NV PA current to mission mode struct.
03/20/13   cd      XPT Temp comp, correctly copy over DPD information
03/20/13   cd      XPT Temp comp support
03/20/13   Saul    MultiLin. Each RGI now has info for which freqcomp to use.  
03/18/13   cd      Fix compiler warnings
03/18/13   cd      Updated pin/pout value calculation algorithm to use 
                   reference channel information
03/12/13   cd      Fix DPD index population in APT Pout temp comp algorithm
03/11/13   sar     Updated file for APQ, GNSS only target.
03/08/13   cd      Free memory for decompressed for NV data from multilin V1
                   NV loading, in case of early return
03/07/13   cri     Revert dpd type post processing
03/07/13   cd      Fixed compiler error
03/07/13   cd      Fixed KW errors in common functionality for xPT temp and 
                   freq comp
03/05/13   cd      - Added common functions for xPT temp and freq comp
                   - Fixed issue where freq comp is not copying over iq
                   offset value to output linearizer
02/26/13   Saul    CDMA. Set EPT switch-point to 42 dBm when ET is enabled
02/22/13   Saul    Multilin. Added dpd_type to linearizer.
02/20/13   cri     Only create new DPD tables if current DPD index has not been used yet
02/08/13   kcj     Ensure memory allocation is cleaned up in all cases.
01/16/13   Saul    CDMA. Enabled EPT/ET tempcomp.
12/19/12   Saul    MultiLin. Temp comp populates num dpd in compensated lin.
12/10/12   cri     Load DDP from current linearizer   
12/07/12   Saul    MultiLin. Allow NULL lin to list before valid lin.
12/05/12   Saul    MultiLin. Bug fix in associating lin from NV to 
                   corresponding channel.
12/05/12   Saul    MultiLin. Consolidated process_xpt APIs.
12/05/12   Saul    MultiLin. Process XPT DPD flat list so that each PA state 
                   ultimately contains a table of its corresponding DPDs.
11/19/12   aki     Fix memory leaks in error case
11/13/12   Saul    MultiLin. Added num_dpd used by linearizer.
11/12/12   Saul    MultiLin. Removed dpd_data from lin table type.
11/08/12   Saul    MultiLin. Restructured EPT_V2 and ET DPDs.
10/31/12   Saul    MultiLin. Init pointers to NULL unconditionally.
10/31/12   Saul    MultiLin. Bug fix on size checking.
10/31/12   Saul    MultiLin. Moved iq_offset to linearizer data type
10/24/12   Saul    MultiLin. Added variant_marker_type
08/27/12   Saul    MultiLin. AM/PM/Detrough now dynamically allocated per RGI.
08/23/12   Saul    MultiLin. Added PA current and iq offset NV load support.
08/22/12   Saul    MultiLin. Temp-comp supports V3 lin.
08/21/12   Saul    MultiLin. Added support to load EPT V2 and ET NV Data.
                   Renamed API to reflect its freq-comp work.
08/17/12  cri/Saul MultiLin. Removed dpd_data copying from compensation APIs
                   since only dpd_index is needed during compensation.
08/16/12   Saul    MultiLin. Removed support of Nikel's EPT from multilin V3.
                   Warnings clean-up.
08/01/12   jfc     Correct merge from last edit 
07/26/12   cri     Sorting WCDMA Tx linearizer tables
07/12/12   Saul    MultiLin. Added DPD V1/V2/V3 NV Types.
07/03/12   Saul    MultiLin. Added API to free tx band cal memory.
                   - Added check to ensure linearizers are contiguous with 
                     unique monotonic upper_bound_freq
                   - V3 OFT now tests for EPT DPD table.
                   - DPD status flag now populated when any DPD data is present.
07/11/12   cri     Added EPT support for CDMA
07/09/12   cd      IQ gain offset initialization to zero for multilin Rev1
07/09/12   vs      Fixed QPA temp comp issues
07/02/12   Saul    MultiLin. Added framework for multilin V3 OFT testing.
06/29/12   Saul    MultiLin. EPT DPD data now populated into internal structure.
06/28/12   Saul    MultiLin. Added EPT,ET type IDs. Chan is now uint32.
06/27/12   Saul    MultiLin. Split band cal data into tx chans and freq offsets
06/26/12   Saul    MultiLin. Added init API to invalidate all values. 
06/25/12   Saul    MultiLin. Added "extract data" API per each type ID
06/22/12   Saul    MultiLin. Added API to load V3 NV with just proof-of-concept code.
06/21/12   Saul    MultiLin. Separated code into separate APIs per version.
06/12/12   cri     Fixed remaining issues for W EPT online mode support  
06/07/12   vs      Add WCDMA Temp comp support for EPT
06/04/12   cri     Add EPT support for WCDMA
05/01/12   cri     Updated EPT linearizer structure and processing
04/04/12   cri     Support for processing EPT linearizers 
03/29/12    cri    Support for loading EPT linearizers from NV 
12/16/11    dw     Add Null pointer check and mem free before early returns
12/15/11    dw     Allocating heap memory instead of using tmc stack for local variable nv_data_raw and add more comments in the function   
12/13/11    dw     change temp comp to track SMPS by power in cold and track by RGI in hot
12/07/11   whc     Common helper function for QSORT to sort linearizer table  
11/15/11    cd     Fix handling of invalid RGIs in common temp comp function
09/30/11    dw     [1] Update temp comp common function
                   [2] Lower some message severity
                   [3] In the TxLin table, update Tx Freq with target channel freq instead of Cal chan freq
09/26/11    dw     Revert the KW fix, fix the FREQCOMP RGI BIN threshold in the previous condition
09/15/11   cd      Fixed check for cal chan size.
09/14/11   sar     Fixed KW errors. 
09/12/11    dw     Added temp comp support
09/10/11    cd     Fixed functionality to obtain the correct freq comp bin 
                   based on RGI threshold. Invalid RGI (0xFFFF) should not be 
                   considered.
09/07/11   sar     Clockworks error fix for (System: 7890).
08/30/11   aki     Pass in rfnv buffer size to rfcommon_rfnv_get_item 
08/16/11    cd     Check for invalid RGI, in the case of which do not   
                   perform power frequency compensation 
08/11/11    vb     Added code to extract data from the packed structures as memcpy cannot be used
08/11/11    vb     Code beautification
08/11/11    dw     Added support for vairable freq comp channel size
08/11/11    dw     Rename rfcommon_core_populate_final_linearizer()
07/29/11    dw     Use 0xFFFF instead of 0 for invalid upper bound channel and RGI threshold 
07/22/11    dw     Fix warning
07/19/11    dw     Initial version
============================================================================*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "comdef.h"
#include "rfcom.h"
#include "rfm.h"
#include "rfcommon_core_txlin.h"
#include "rfcommon_core_xpt.h"
#include "rfcommon_nv.h"
#include "modem_mem.h"
#include "rfcommon_math.h"
#include "zlib.h"
#include "rfcommon_msg.h"
#include "rfcommon_mdsp.h"
#include "rfcommon_msm.h"
#include "rfcommon_fbrx_api.h"
#include "rfcommon_fbrx_nv.h"

/* Device and RFC related */
#include "rfc_common.h"
#include "rfc_card.h"
#include "rf_hal_buffer.h"
#include "rfdevice_cmn_intf.h"
#include "rfdevice_pa_intf.h"
#include "rfdevice_papm_intf.h"
#include "rf_rffe_common.h"

/* memscpy */
#include "stringl.h"

#define NV_STRUCT_SIZE sizeof(tx_band_cal_nv_type)
#define LIN_TABLE_SIZE sizeof(tx_linearizer_row_type)*RFCOM_TXLIN_NUM_SEGMENTS
#define RFCOM_TXLIN_TEMP_COMP_TRACK_POWER_THRESH 10
/* Pin-to-Pout delta conversion factor - alpha in fraction*100 percentage
For example, 0.8 = 0.8*100 = 80% */
#define RFCOM_TXLIN_PIN_POUT_DELTA_CONVERSION_ALPHA_APT  95
#define RFCOM_TXLIN_PIN_POUT_DELTA_CONVERSION_ALPHA_EPT  80
#define RFCOM_TXLIN_PIN_POUT_DELTA_CONVERSION_ALPHA_ET   25

#define RFCOM_MAX_TX_DEVICES_SUPPORTED                    2

/* Tx freq comp table of 8 sub states in one linearizer */
static int8 tx_linearizer_freq_comp_table[RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN]={0};


typedef struct 
{
  /*! Array of tx_agc_buffer pointers */
  rf_buffer_intf* tx_agc_buffer[RFCMN_TXLIN_AGC_MAX_TASK];

  /*! Address of dummy_trig buffer */
  rf_buffer_intf* dummy_trig_buffer;

  /*! Address of pa_trig buffer */
  rf_buffer_intf* pa_trig_buffer;

  /*! Array of fbrx_lut_buffer pointers */
  rf_buffer_intf* fbrx_lut_buffer[RFCOMMON_FBRX_NUM_GAIN_STATES];

  /*! Linearizer logging */
  rfcommon_core_txlin_log_data_type *log_data;

} rfcommon_core_txlin_data_type;

/* Structure per device to hold Tx Linearizer buffer information */
static 
rfcommon_core_txlin_data_type tx_lin_info[RFCOM_MAX_TX_DEVICES_SUPPORTED] ;

/*----------------------------------------------------------------------------*/
/*! EPT DPD V2 Data */
typedef struct
{
  /*! Number of total DPDs contained/available */
  uint16 num_dpd_avail;
  /*! AM DPDs */
  xpt_am_row_type* am_dpds;
  /*! PM DPDs */
  xpt_pm_row_type* pm_dpds;
} ept_dpd_v2_data_type;

/*----------------------------------------------------------------------------*/
/*! ET DPD Data */
typedef struct
{
  /*! Number of total DPDs contained/available */
  uint16 num_dpd_avail;
  /*! AM DPDs */
  xpt_am_row_type* am_dpds;
  /*! PM DPDs */
  xpt_pm_row_type* pm_dpds;
  /*! Detrough DPDs */
  xpt_dt_row_type* dt_dpds;
} et_dpd_data_type;


/*----------------------------------------------------------------------------*/
boolean 
rfcommon_core_load_linearizer_v1_from_nv
(
  rfnv_item_id_enum_type item_code, 
  tx_band_cal_type *tx_lut_band_data,
  rfcommon_core_convert_freq_handler_type convert_freq_handler
);

/*----------------------------------------------------------------------------*/
boolean 
rfcommon_core_load_linearizer_v3_from_nv
(
  rfnv_item_id_enum_type item_code, 
  tx_band_cal_type *tx_cal_data,
  rfcommon_core_convert_freq_handler_type convert_freq_handler
);

/*----------------------------------------------------------------------------*/
void 
rfcommon_core_load_dynamic_linearizer_v1_entry_from_nv
(
  void* nv_data_ptr,
  tx_band_cal_type *tx_lut_band_data,
  uint8 pa_range,
  uint8 tx_waveform,
  uint8 tx_chan_idx,
  uint8 fcomp_bin_num,
  uint16* rgi_thresh_info,
  rfcommon_core_convert_freq_handler_type convert_freq_handler
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_core_tx_multilin_extract_tx_cal_chans
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  rfcommon_core_convert_freq_handler_type const convert_freq_handler,
  uint32 * const extracted_element_size,
  tx_band_cal_type *tx_cal_data
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_core_tx_multilin_extract_freq_offsets
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  tx_band_cal_type *tx_cal_data
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_core_tx_multilin_extract_lin_v3
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  rfcommon_core_convert_freq_handler_type const convert_freq_handler,
  uint32 * const extracted_element_size,
  tx_band_cal_type *tx_cal_data,
  uint16* num_ept_dpd_used,
  uint16* num_et_dpd_used
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_core_tx_multilin_extract_lin_v4

(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  rfcommon_core_convert_freq_handler_type const convert_freq_handler,
  uint32 * const extracted_element_size,
  tx_band_cal_type *tx_cal_data,
  uint16* num_ept_dpd_used,
  uint16* num_et_dpd_used
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_core_tx_multilin_extract_ept_dpd_v2
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  tx_band_cal_type *tx_cal_data,
  ept_dpd_v2_data_type *ept_dpd_v2,
  uint16 *num_ept_dpd_v2_saved
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_core_tx_multilin_extract_et_dpd
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  tx_band_cal_type *tx_cal_data,
  et_dpd_data_type *et_dpd,
  uint16 *num_et_dpd_saved
);
/*----------------------------------------------------------------------------*/
boolean
rfcommon_core_tx_multilin_extract_ref_temp_adc(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  tx_band_cal_type *tx_cal_data);
/*----------------------------------------------------------------------------*/
boolean 
rfcommon_core_txlin_temp_comp_update_apt
(
  tx_linearizer_table_type *tx_lut,
  tx_linearizer_table_type *temp_comp_lut,
  int16 temp_comp_val,
  uint8 valid_lut_entries
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_core_linearizer_load_post_processing
(
  tx_band_cal_type *tx_cal_data
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_core_linearizer_load_post_process_xpt
(
  tx_band_cal_type *tx_cal_data,
  ept_dpd_v2_data_type *ept_dpd_v2,
  et_dpd_data_type *et_dpd
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_core_linearizer_load_post_process_dpd_type
(
  tx_band_cal_type *tx_cal_data
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Getter function for the given Tx AGC buffer for a given device.

  @details
  Getter function for the given Tx AGC buffer for a given device. Since there 
  are only 2 Tx devices supported RFM_DEVICE_0 is translated to tx_lin_info[0] 
  and RFM_DEVICE_2 is translated to tx_lin_info[1].

  @param device
  RFM_DEVICE_2 or RFM_DEVICE_0.

  @return rf_buffer_intf**
  Array to the buffer of the pointers of tx_agc_buffer. 
  In case of an error a NULL pointer is returned.

*/
static rf_buffer_intf**
rfcommon_core_txlin_get_tx_agc_buffer ( rfm_device_enum_type device )
{
  rf_buffer_intf** tx_agc_buff = NULL ;
  uint8 dev_index = 0xFF ;

  if ( device == RFM_DEVICE_0 )
  {
    dev_index = 0 ;
  }
  else if ( device == RFM_DEVICE_2 )
  {
    dev_index = 1 ;
  }
  else 
  {
    RF_MSG_1 ( RF_ERROR , "rfcommon_core_txlin_get_tx_agc_buffer(): "
                          "Devices supported are RFM_DEVICE_0 and RFM_DEVICE_2 " 
                          "Device passed: %d " , device ) ;
  }

  if ( 0xFF != dev_index )
  {
    tx_agc_buff = &(tx_lin_info[dev_index].tx_agc_buffer[0]); 
  }
  return tx_agc_buff ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Getter function for the given FBRx LUT buffer for a given device.

  @details
  Getter function for the given FBRx LUT buffer for a given device. Since there 
  are only 2 Tx devices supported RFM_DEVICE_0 is translated to 
  fbrx_lut_buffer[0] and RFM_DEVICE_2 is translated to fbrx_lut_buffer[1].

  @param device
  RFM_DEVICE_2 or RFM_DEVICE_0.

  @return rf_buffer_intf**
  Array to the buffer of the pointers of fbrx_lut_buffer. 
  In case of an error a NULL pointer is returned.

*/
static rf_buffer_intf**
rfcommon_core_txlin_get_fbrx_lut_buffer ( rfm_device_enum_type device )
{
  rf_buffer_intf** fbrx_lut_buffer = NULL ;

  uint8 dev_index = 0xFF ;

  if ( device == RFM_DEVICE_0 )
  {
    dev_index = 0 ;
  }
  else if ( device == RFM_DEVICE_2 )
  {
    dev_index = 1 ;
  }
  else 
  {
    RF_MSG_1 ( RF_ERROR , "rfcommon_core_txlin_get_fbrx_lut_buffer(): "
                          "Devices supported are RFM_DEVICE_0 and RFM_DEVICE_2 " 
                          "Device passed: %d " , device ) ;
  }

  if ( 0xFF != dev_index )
  {
    fbrx_lut_buffer = &(tx_lin_info[dev_index].fbrx_lut_buffer[0]) ;
  }
  return fbrx_lut_buffer ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Getter function for the given dummy_trig buffer for a given device.

  @details
  Getter function for the given dummy_trig buffer for a given device. Since there 
  are only 2 Tx devices supported RFM_DEVICE_0 is translated to 
  dummy_trig[0] and RFM_DEVICE_2 is translated to dummy_trig[1].

  @param device
  RFM_DEVICE_2 or RFM_DEVICE_0.

  @return rf_buffer_intf*
  Address of dummy_trig_buffer. 
  In case of an error a NULL pointer is returned.
*/
static rf_buffer_intf*
rfcommon_core_txlin_get_dummy_trig_buffer ( rfm_device_enum_type device )
{
  rf_buffer_intf* dummy_trig_buff = NULL ;
  uint8 dev_index = 0xFF ;

  if ( device == RFM_DEVICE_0 )
  {
    dev_index = 0 ;
  }
  else if ( device == RFM_DEVICE_2 )
  {
    dev_index = 1 ;
  }
  else 
  {
    RF_MSG_1 ( RF_ERROR , "rfcommon_core_txlin_get_dummy_trig_buffer(): "
                          "Devices supported are RFM_DEVICE_0 and RFM_DEVICE_2 " 
                          "Device passed: %d " , device ) ;
  }

  if ( 0xFF != dev_index )
  {
    dummy_trig_buff = tx_lin_info[dev_index].dummy_trig_buffer ; 
  }
  return dummy_trig_buff ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Getter function for the given pa_trig buffer for a given device.

  @details
  Getter function for the given dummy_trig buffer for a given device. Since there 
  are only 2 Tx devices supported RFM_DEVICE_0 is translated to 
  pa_trig[0] and RFM_DEVICE_2 is translated to pa_trig[1].

  @param device
  RFM_DEVICE_2 or RFM_DEVICE_0.

  @return rf_buffer_intf*
  Address of pa_trig_buffer. 
  In case of an error a NULL pointer is returned.
*/
static rf_buffer_intf*
rfcommon_core_txlin_get_pa_trig_buffer ( rfm_device_enum_type device )
{
  rf_buffer_intf* pa_trig_buff = NULL ;
  uint8 dev_index = 0xFF ;

  if ( device == RFM_DEVICE_0 )
  {
    dev_index = 0 ;
  }
  else if ( device == RFM_DEVICE_2 )
  {
    dev_index = 1 ;
  }
  else 
  {
    RF_MSG_1 ( RF_ERROR , "rfcommon_core_txlin_get_pa_trig_buffer(): "
                          "Devices supported are RFM_DEVICE_0 and RFM_DEVICE_2 " 
                          "Device passed: %d " , device ) ;
  }

  if ( 0xFF != dev_index )
  {
    pa_trig_buff = tx_lin_info[dev_index].pa_trig_buffer ; 
  }
  return pa_trig_buff ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to retrieve Tx linearizer logging info for a given device

  @details
  Obtain a pointer to linearizer logging data for a given device and 
  PA state. Since there are only 2 Tx devices supported RFM_DEVICE_0 
  is translated to log_data[0] and RFM_DEVICE_2 is translated to log_data[1].

  @param device
  RFM_DEVICE_0 or RFM_DEVICE_2

  @return Pointer to Tx linearizer logging data structure for requested device 
  In case of an error a NULL pointer is returned.
*/
static rfcommon_core_txlin_log_data_type*
rfcommon_core_txlin_get_lin_log_buffer
(
  rfm_device_enum_type device
)
{
  rfcommon_core_txlin_log_data_type* log_data = NULL;
  uint8 dev_index = 0xFF;

  if ( device == RFM_DEVICE_0 )
  {
    dev_index = 0;
  }
  else if ( device == RFM_DEVICE_2 )
  {
    dev_index = 1;
  }
  else 
  {
    RF_MSG_1 ( RF_ERROR , "rfcommon_core_txlin_get_lin_log_buffer(): "
                          "Devices supported are RFM_DEVICE_0 and RFM_DEVICE_2 " 
                          "Device passed: %d ", device );
  }

  if ( 0xFF != dev_index )
  {
    log_data = tx_lin_info[dev_index].log_data; 
  }
  return log_data;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function used to compute the IQ gain adjustment value in HW units from the
  dBx10 offset value provided in NV.

  @details
 
  @param dBx10_offset
  Power adjustment offset in dBx10
 
  @return
  Returns the IQ gain adjust value in Q15 format, after converting
  from NV dB offset value to HW units
*/
uint64
rfcommon_core_txlin_compute_iq_gain_scaling
(
  int8 dBx10_offset
)
{
  int32 exp10_input_q26, exp10_output_q15;
 
  if ( dBx10_offset != 0 )
  {
    //convert dBx10_offset to a format suitable for qdsp4_exp10
    //Input should be in Q26 => (dBx10_offset * 2^ 26 * 1/10 * 1/20) = 335544.
    exp10_input_q26 = (int32) dBx10_offset * (335544);
    
    //output is in Q15. 
    exp10_output_q15 =  rf_compute_exp10(exp10_input_q26);
  }
  else
  {
    /* Don't compute the scaling, just return 1 in Q15 format */
    exp10_output_q15 = (1 << 15);
  }
 
  return (exp10_output_q15);

} /* rfcommon_core_txlin_compute_iq_gain_scaling */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Obtain the Reference linearizer frequency with respect to the current Tx 
  frequency of operation, provided as input
 
  @details
  Searches through the MultiLin NV data structure to find the reference 
  linearizer with respect to the current frequency of operation

  @return 
  TRUE if ref linearizer is found, else FALSE 
*/
boolean
rfcommon_core_txlin_get_ref_linearizer_frequency
(
  tx_band_cal_type *tx_multilin_nv_cal_ptr,
  uint32 current_tx_freq,
  uint8 pa_state,
  tx_linearizer_waveform_type tx_waveform,
  uint32 *ref_lin_freq
)
{
  uint8 freq_iterator;
  /* Tx cal data linearizer */
  tx_multi_linearizer_index_node_type* tx_cal_data_index;

  tx_cal_data_index = 
     (tx_multi_linearizer_index_node_type*)&tx_multilin_nv_cal_ptr->
           tx_pa_state_cal_data[pa_state].linearizer_index[tx_waveform][0];

  /* Loop through 16 channels to find the closest linearizer for the 
  given Tx frequency*/
  for ( freq_iterator = 0; 
        freq_iterator < RFCOM_TXLIN_NUM_CHANNELS; 
        freq_iterator++ )
  {
    if ( (current_tx_freq <= 
                      tx_cal_data_index[freq_iterator].upper_bound_freq) &&
         (tx_cal_data_index[freq_iterator].node_data.lin_data != NULL) )
    {
      /* match linearizer table found */
      break;
    }
  }

  if ( freq_iterator == RFCOM_TXLIN_NUM_CHANNELS )
  {
    RF_MSG_1( RF_ERROR, "rfcommon_core_txlin_get_lin_vs_temp_vs_freq_compensation: "
                        "Failed to find linearizer table for Tx frequency: %d!", 
              current_tx_freq );
    *ref_lin_freq = 0;
    return FALSE;
  }
  else
  {
    /* Assign frequency to return value */
    *ref_lin_freq = 
             tx_cal_data_index[freq_iterator].node_data.lin_data->chan_freq;
  }

  return TRUE;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Search and Calculate the Tx AGC frequency compensation value.
 
  @details
  This API search the Tx Calibrated freqcomp tables find the closest freqcomp value 
  to the given Tx frequency, and do linear interpolation to calculate the frequency 
  compensation to Tx power in dB10 unit. 

  @param freq_comp
  Tx AGC calibrated frequency compensation table

  @param tx_cal_freq
  Tx Calibrated frequency list  

  @param nv_ptr
  pointer to the band-specific NV table
 
  @param tx_freq
  target Tx frequency
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
  The calculated frequency compensation is in reference to center frequency. Final 
  Tx comp in multi-tx linerizer scheme needs to be convered to Tx frequency compensation 
  in reference to new linearizer frequency: 
  freqcomp(f,f_newlin)=freqcomp(f,f_center)-freqcomp(f_newlin,f_center) 

*/
int8
rfcommon_core_freqcomp_linear_interpolate
(
  int8 freq_comp[],
  uint32 tx_cal_freq[],
  uint32 tx_freq
)
{
  int32 freq_bin_size =0;
  uint8 tx_start_bin =0;
  int32 delta_freq = 0;
  int32 comp_bin_size = 0;

  /* tx freq out of range case 1: return freq_comp[0] */
  if (tx_freq<tx_cal_freq[0])
  {
    return freq_comp[tx_start_bin];
  }

  /*Loop Tx target frequency through Tx Cal frequency list to find the closest freqcomp bin*/
  for (tx_start_bin=0; tx_start_bin<(RFCOM_TXLIN_NUM_CHANNELS-1); tx_start_bin++)
  {
    if ((tx_freq>=tx_cal_freq[tx_start_bin])&&(tx_freq<tx_cal_freq[tx_start_bin+1]))
    {
      /* match bin found */
      break;
    }
  }

  /* tx freq out of range case 2: return freq_comp[RFCOM_TXLIN_NUM_CHANNEL-1]*/
  if (tx_start_bin == (RFCOM_TXLIN_NUM_CHANNELS-1))
  {
    return freq_comp[tx_start_bin];
  }

  /* We are in Cal frequency range. Do interpolation*/
  /* interpolated freq comp = Fcomp[i]+(Fcomp[i+1]-Fcomp[i])*(F-Fcal[i])/(Fcal[i+1]-Fcal[i])*/
  /* Fcomp = freq comp table, Fcal = Cal frequency table, F = target Tx frequency, i = start bin */

  freq_bin_size = (int32)tx_cal_freq[tx_start_bin+1]-(int32)tx_cal_freq[tx_start_bin];

  comp_bin_size = (int32)freq_comp[tx_start_bin+1]-(int32)freq_comp[tx_start_bin];

  delta_freq = (int32)tx_freq-(int32)tx_cal_freq[tx_start_bin];

  return(freq_comp[tx_start_bin]+(int8)(delta_freq*comp_bin_size/freq_bin_size));
} /* rfcommon_core_freqcomp_linear_interpolate */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Performs frequency-compensation on linearizer data.
 
  @details
  This is a common function for all technologies. It uses the search  
  parameters to find the closest linearizer from Cal data, calculates 
  and applies frequency compensation for each RGI and populates the 
  final Tx linearizer table with compensated data.  

  @param tx_linearizer_search_param
  search parameters including target Tx frequency, PA range and Tx waveform type

  @param tx_band_cal_data
  pointer to the entire Tx linerizer band data structure which includes all calibrated
  linearizer tables, Tx Cal frequencies, Tx linearizer frequenfcies, Tx freqcomp tables
  etc of one band/technology

  @param tx_final_linearizer
  pointer to the final 64 element linearizer table: rgi vs power vs apt(SMPS Bias) that
  will be populated to firmware in online mode
 
  @param tx_linearizer_tech_param
  tech dependent paramters to indicate tech specfic differences in this function
  
  @internal
  Tx linearizer table is populated with power values that are in increasing monotonic order.
  Tx power is in dB10 units. RGI is LUT index. APT is 10 bit SMPS pdm value.
  Each element shall be converted into tech-specific FW AGC units before being populated to mdsp.
*/
boolean
rfcommon_core_freq_comp_linearizer
(
  tx_linearizer_search_param_type *tx_linearizer_search_param,
  tx_band_cal_type *tx_band_cal_data,
  tx_linearizer_table_data_type *tx_final_linearizer_data,
  void *tx_linearizer_tech_param,
  boolean do_freq_comp_shift
)
{
  uint8 freq_index,rgi_index,freqcomp_rgi_bin = 0;
  /* Target Tx frequency */
  uint32 tx_freq;
  /* PA range*/
  uint8 pa_range;
  /* Tx waveform type*/
  tx_linearizer_waveform_type tx_waveform;
  /* Tx cal data linearizer index given waveform type and PA range */
  tx_multi_linearizer_index_node_type* tx_cal_data_index;
  /* Tx frequency compensation table index given waveform type and PA range*/
  tx_freq_offset_table_row_type* tx_freq_offset_table_ptr;
  tx_linearizer_table_type *tx_final_linearizer = tx_final_linearizer_data->lin_data;
  uint16 fcomp_idx = -1, num_fcomp = -1;
  uint8 fcomp_type;
  rfcommon_core_txlin_log_data_type *log_data = NULL;
  rfcommon_core_txlin_cap_boost_type *cap_boost_data = NULL;

  if ((tx_linearizer_search_param == NULL) || (tx_band_cal_data == NULL) || (tx_final_linearizer == NULL))
  {
    RF_MSG( RF_ERROR, "rfcommon_core_freq_comp_linearizer: "
        "NULL Parameter value/values");
    return FALSE;
  }

  /* Initialize linearizer log data pointer for this device */
  log_data = rfcommon_core_txlin_get_lin_log_buffer( 
                                      tx_linearizer_search_param->rfm_device );

  if ( log_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_core_freq_comp_linearizer: "
                      "NULL pointer for logging linearizer data" );
    return FALSE;
  }

  if ( tx_linearizer_tech_param != NULL )
  {
    cap_boost_data = (rfcommon_core_txlin_cap_boost_type*)tx_linearizer_tech_param;
  }

  /* Target Tx frequency */
  tx_freq = tx_linearizer_search_param->tx_freq;
  /* PA range*/
  pa_range = tx_linearizer_search_param->pa_range;
  /* Tx waveform type*/
  tx_waveform = tx_linearizer_search_param->tx_waveform;

  if ( pa_range >= RFCOM_TXLIN_NUM_LINEARIZERS )
  {
    RF_MSG_1( RF_ERROR, "rfcommon_core_freq_comp_linearizer: "
                        "PA Range value greater than %d",
              RFCOM_TXLIN_NUM_LINEARIZERS );
    return FALSE;
  }

  /* Tx cal data linearizer index given waveform type and PA range */
  tx_cal_data_index = \
                      &tx_band_cal_data->tx_pa_state_cal_data[pa_range].linearizer_index[tx_waveform][0];

  if (tx_band_cal_data->tx_cal_chan_size > RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG( RF_ERROR, "rfcommon_core_populate_tx_linearzier: "
        "tx_cal_chan_size exceeds bounds");
    return FALSE;
  }

  /* Step 1: loop through 16 channel to find the closest linearizer for the given Tx frequency */

  for (freq_index=0; freq_index<RFCOM_TXLIN_NUM_CHANNELS; freq_index++)
  {
    if ((tx_freq <= tx_cal_data_index[freq_index].upper_bound_freq)&& \
        (tx_cal_data_index[freq_index].node_data.lin_data != NULL))
    {
      /* match linearizer table found */
      break;
    }
  }

  if (freq_index == RFCOM_TXLIN_NUM_CHANNELS)
  {
    RF_MSG_1( RF_MED, "Failed to find linearizer table for Tx frequency:%d!", tx_freq);
    /* What do we do if search fails? It may indicate lin data structures are not populated correctly */
    /* Use mid channel lut as default for now*/
    freq_index = RFCOM_TXLIN_DEFAULT_FREQ_INDEX;
    if (tx_cal_data_index[freq_index].node_data.lin_data == NULL)
    {
      /* reference channel table pointer is NULL, something is seriously wrong */  
      RF_MSG( RF_ERROR, "reference channel LUT not found!");
      return FALSE;
    }
  }

  /* Tx frequency compensation table index given waveform type and PA range */

  if(tx_band_cal_data->nv_rev_type != TX_MULTI_LIN_REV_1)
  {
    /* For non-V1 lins, info is in lin itself */
    fcomp_type = tx_cal_data_index[freq_index].node_data.lin_data->fcomp_type;
  }
  else if((tx_waveform == TX_LINEARIZER_FIRST_SWEEP) && (tx_band_cal_data->nv_rev_type != TX_MULTI_LIN_REV_1))
  {
    /* Full Bias */
    fcomp_type = TX_FREQCOMP_FB;
  }
  else
  {
    /* APT Bias */
    fcomp_type = TX_FREQCOMP_APT;
  }

  tx_freq_offset_table_ptr = &tx_band_cal_data->tx_pa_state_cal_data[pa_range].tx_freq_offsets[fcomp_type][0];

  /* Step 2: Calculate freqcomp table for up to 8 sub states at one Tx frequency and PA state */

  num_fcomp = tx_cal_data_index[freq_index].node_data.lin_data->num_fcomp;
  /* Validate total num of fcomps to use */
  if (num_fcomp > RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN)
  {
    RF_MSG_2( RF_ERROR, "rfcommon_core_freq_comp_linearizer - Num fcomp items %d must be <= %d",
              num_fcomp, RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN);
    return FALSE;
  }

  /* Init to zeroes in case lin uses no fcomps which is a valid case */
  memset( tx_linearizer_freq_comp_table, 0, sizeof(tx_linearizer_freq_comp_table) );

  /* Perform fcomp only on the lists needed. No adjustments made if lin uses no fcomps */
  for (freqcomp_rgi_bin = 0; freqcomp_rgi_bin < num_fcomp; freqcomp_rgi_bin++)
  {
    /* Calculate freq comp in reference to the new linearizer frequency. This is because freq comp data
       from calibratiion is in reference to center linearizer frequency: 
       freqcomp(f,f_newlin)=freqcomp(f,f_center)-freqcomp(f_newlin,f_center)
       Do not assume new linearizer frequency is at one of the freq comp bin, do search and interpolate
       for both target frequency and new linearizer frequency
     */
    tx_linearizer_freq_comp_table[freqcomp_rgi_bin] = \
      rf_lininterp_u32_s8(
                         tx_band_cal_data->tx_cal_freq,
                         tx_freq_offset_table_ptr[freqcomp_rgi_bin].freq_comp,
                         tx_band_cal_data->tx_cal_chan_size,
                         tx_freq) - \
      rf_lininterp_u32_s8(
                         tx_band_cal_data->tx_cal_freq,
                         tx_freq_offset_table_ptr[freqcomp_rgi_bin].freq_comp,
                         tx_band_cal_data->tx_cal_chan_size,
                         tx_cal_data_index[freq_index].node_data.lin_data->chan_freq);
  }

  /* Step 3: Loop thru the linearizer table, copy Cal data the final linearizer table, 
     and apply freq comp to Tx power for each RGI */

  /* Lin table population */
  for (rgi_index=0;rgi_index<RFCOM_TXLIN_NUM_SEGMENTS;rgi_index++)
  {
    uint16 current_rgi = tx_cal_data_index[freq_index].node_data.lin_data->table[rgi_index].rgi;

    /* copy RGI/LUT index to the final table */
    tx_final_linearizer->table[rgi_index].rgi = current_rgi;

    /* copy Tx power in dB10 AGC units with freq comp*/
    if ( tx_final_linearizer->table[rgi_index].rgi != 0xFFFF )
    {
      /* If the linearizer uses no fcomp, default to first fcomp_idx which by now 
         should all be zeroes for offsets */
      if (num_fcomp == 0)
      {
        fcomp_idx = 0;
      }
      else
      {
        fcomp_idx = tx_cal_data_index[freq_index].node_data.lin_data->table[rgi_index].fcomp_idx;
        if (fcomp_idx >= num_fcomp)
        {
          RF_MSG_2( RF_ERROR, "rfcommon_core_freq_comp_linearizer - FreqComp idx %d must be < %d",
                    fcomp_idx, RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN);
          return FALSE;
        }
      }
      if (do_freq_comp_shift)
      {
      tx_final_linearizer->table[rgi_index].power = tx_cal_data_index[freq_index].node_data.lin_data->table[rgi_index].power + \
                                                    tx_linearizer_freq_comp_table[fcomp_idx];
      }
      else
      {
        tx_final_linearizer->table[rgi_index].power = tx_cal_data_index[freq_index].node_data.lin_data->table[rgi_index].power;
      }
    }
    else
    {
      /* If the RGI is invalid, copy the power value without freq comp */
      tx_final_linearizer->table[rgi_index].power = tx_cal_data_index[freq_index].node_data.lin_data->table[rgi_index].power;
    }

    /* copy apt/SMPS Bias */
    tx_final_linearizer->table[rgi_index].apt = tx_cal_data_index[freq_index].node_data.lin_data->table[rgi_index].apt;

    /* Copy the IQ gain offset */
    tx_final_linearizer->table[rgi_index].iq_offset = 
         tx_cal_data_index[freq_index].node_data.lin_data->table[rgi_index].iq_offset;

    /* Populate indexes into DPD table. DPD and TxLin table start off one-to-one in terms of row entries,
      thus use current table index directly. */
    tx_final_linearizer->table[rgi_index].dpd_index = tx_cal_data_index[freq_index].node_data.lin_data->table[rgi_index].dpd_index;
    tx_final_linearizer->table[rgi_index].fcomp_idx = tx_cal_data_index[freq_index].node_data.lin_data->table[rgi_index].fcomp_idx;

    /* FreqComp: Copy the RSB correction Coefficients and LOFT correction */
    tx_final_linearizer->table[rgi_index].rsb_correction_coeff.alpha = 
      tx_cal_data_index[freq_index].node_data.lin_data->table[rgi_index].rsb_correction_coeff.alpha ;
    tx_final_linearizer->table[rgi_index].rsb_correction_coeff.beta = 
      tx_cal_data_index[freq_index].node_data.lin_data->table[rgi_index].rsb_correction_coeff.beta ;
    tx_final_linearizer->table[rgi_index].loft_correction = 
      tx_cal_data_index[freq_index].node_data.lin_data->table[rgi_index].loft_correction ;

  }
  
  /* Linearizer cap/boost algorithm support. Currently supported only for ET */
  if ( (tx_cal_data_index[freq_index].node_data.aux_lin_data == 
                                                  TX_LINEARIZER_ET_DPD_DATA ||
       tx_cal_data_index[freq_index].node_data.aux_lin_data ==
                                                  TX_LINEARIZER_EPT_DPD_V2_DATA) &&
       cap_boost_data != NULL )
  {
    if ( cap_boost_data->max_pwr_ovr_algo == 
                                    RFCOMMON_CORE_TXLIN_MAX_PWR_OVR_DEFAULT )
    {
      /* Don't do anything for the default algorithm */
      log_data->max_power_ovr_algo[pa_range] = 
                           RFCOMMON_CORE_TXLIN_MAX_PWR_OVR_DEFAULT;

    }
    else if ( cap_boost_data->max_pwr_ovr_algo == 
                                RFCOMMON_CORE_TXLIN_MAX_PWR_OVR_QPA_BACKOFF )
    {
      log_data->max_power_ovr_algo[pa_range] = 
                                     RFCOMMON_CORE_TXLIN_MAX_PWR_OVR_DEFAULT;

      /* Sort the frequency compensated linearizer */
      qsort( &(tx_cal_data_index[freq_index].node_data.lin_data->table),
             RFCOMMON_MDSP_TXLIN_EPT_TBL_SIZE,
             sizeof(tx_linearizer_row_type),
             rfcommon_core_qsort_compare_increasing_pwr );
      qsort( &(tx_final_linearizer->table),
             RFCOMMON_MDSP_TXLIN_EPT_TBL_SIZE,
             sizeof(tx_linearizer_row_type),
             rfcommon_core_qsort_compare_increasing_pwr );

      /* Lin table population */
      for ( rgi_index = 0; rgi_index < RFCOMMON_MDSP_TXLIN_EPT_TBL_SIZE; 
            rgi_index++ )
      {
        int16 lin_power = 
          tx_cal_data_index[freq_index].node_data.lin_data->table[rgi_index].power;
        uint16 rgi = 
          tx_cal_data_index[freq_index].node_data.lin_data->table[rgi_index].rgi;

        if ( lin_power > cap_boost_data->lin_cap_boost_nv_val &&
             rgi != RFCOM_TXLIN_INVALID_RGI )
        {
          log_data->max_add_env_scale_ept_et[pa_range][rgi_index] = 
            lin_power - cap_boost_data->lin_cap_boost_nv_val;
          /* found the smallest linearizer entry that is larger than the cap 
          value. Break here and copy over this entry to all 
          higher freq-comped entries */
          log_data->max_power_ovr_algo[pa_range] = 
                                   RFCOMMON_CORE_TXLIN_MAX_PWR_OVR_QPA_BACKOFF;
          rgi_index++;
          break;
        }
        else
        {
          log_data->max_add_env_scale_ept_et[pa_range][rgi_index] = 0;
        }
      }
      /* Pad the remaining entries */
      for ( ; rgi_index < RFCOMMON_MDSP_TXLIN_EPT_TBL_SIZE; rgi_index++ )
      {
        size_t data_length = sizeof(tx_linearizer_row_type);
        memscpy( &tx_final_linearizer->table[rgi_index] , data_length ,
                 &tx_final_linearizer->table[rgi_index-1] , data_length );
        log_data->max_add_env_scale_ept_et[pa_range][rgi_index] = 
                     log_data->max_add_env_scale_ept_et[pa_range][rgi_index-1];
      }
    }
    else if ( cap_boost_data->max_pwr_ovr_algo == 
                                RFCOMMON_CORE_TXLIN_MAX_PWR_OVR_BOOST )
    {
      log_data->max_power_ovr_algo[pa_range] = 
                                         RFCOMMON_CORE_TXLIN_MAX_PWR_OVR_BOOST;

      for ( rgi_index = 0; 
            rgi_index < RFCOMMON_MDSP_TXLIN_EPT_TBL_SIZE; rgi_index++ )
      {
        /* Negate the NV value before passing to RFLM, because the NV value 
        for the boost algorithm specifies a positive cap on the env scale boost, 
        but this is technically a cap on the -ve backoff on env scale */
        log_data->max_add_env_scale_ept_et[pa_range][rgi_index] = 
                                           (-1)*(cap_boost_data->lin_cap_boost_nv_val);
      }
    }

  }

  /* Retain the status of this PA state/waveform/channel auxiliary linearizer data flag for later processing */
  tx_final_linearizer_data->aux_lin_data = tx_cal_data_index[freq_index].node_data.aux_lin_data;

  /* copy new linearizer channel frequency to the final table */
  tx_final_linearizer->chan_freq = tx_freq;
  tx_final_linearizer->dpd_type  = tx_cal_data_index[freq_index].node_data.aux_lin_data;
  tx_final_linearizer->num_dpd   = tx_cal_data_index[freq_index].node_data.lin_data->num_dpd;
  tx_final_linearizer->num_fcomp = tx_cal_data_index[freq_index].node_data.lin_data->num_fcomp;
  tx_final_linearizer->fcomp_type = tx_cal_data_index[freq_index].node_data.lin_data->fcomp_type;

  /* Get XPT data for this linearizer */
  tx_final_linearizer_data->xpt_data = tx_cal_data_index[freq_index].node_data.xpt_data;

  return TRUE;
} /* rfcommon_core_freq_comp_linearizer */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Initializes linearizer to invalid settings 
 
  @details
  Upper layer clients must de-allocate all memory prior to re-initializing 
  any pointer item to NULL. This API fails when a pointer member is found 
  to be non-NULL.
 
  @param tx_band_data
  Pointer linerizer to initialize
 
  @return 
  TRUE if all ok.
*/
boolean
rfcommon_core_init_linearizer
(
  tx_band_cal_type * const tx_band_data
)
{
  /* Overall API status */
  boolean api_status = TRUE;
  uint8 pa_idx;
  uint8 wave_idx;
  uint8 chan_idx;
  uint8 fcomp_swp_idx;
  uint8 fcomp_bin_idx;

  /* Initialize all items to invalid settings */

  tx_band_data->nv_rev_type = TX_MULTI_LIN_INVALID;
  tx_band_data->tx_cal_chan_size = 0;
  memset( tx_band_data->tx_cal_chan, 0xFF, sizeof( tx_band_data->tx_cal_chan ) );
  memset( tx_band_data->tx_cal_freq, 0, sizeof( tx_band_data->tx_cal_freq ) );
  memset( &tx_band_data->ref_lin_temp_adc,0xFF, sizeof(tx_band_data->ref_lin_temp_adc));

  for( pa_idx = 0; 
       pa_idx < RFCOM_TXLIN_NUM_LINEARIZERS; 
       pa_idx++ )
  {
    for( fcomp_swp_idx = 0; 
         fcomp_swp_idx < RFCOM_TXLIN_NUM_V2_FCOMP_SWEEPS;
         fcomp_swp_idx++ )
    {
      for( fcomp_bin_idx = 0; 
           (fcomp_bin_idx < RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN) && (api_status == TRUE);
           fcomp_bin_idx ++ )
      {
         memset( tx_band_data->tx_pa_state_cal_data[ pa_idx ].
                  tx_freq_offsets[ fcomp_swp_idx ][ fcomp_bin_idx ].freq_comp,
                 0,
                 sizeof( tx_band_data->tx_pa_state_cal_data[ pa_idx ].
                         tx_freq_offsets[ fcomp_swp_idx ][ fcomp_bin_idx ].freq_comp
                       )
               );
      } /* for( fcomp_bin_idx ) */
    } /* for( fcomp_swp_idx )*/
    for( chan_idx = 0; 
         chan_idx < RFCOM_TXLIN_NUM_CHANNELS; 
         chan_idx++ )
    { 
      for( wave_idx = 0; 
           wave_idx < TX_LINEARIZER_NUM_WAVEFORM; 
           wave_idx++ )
      {
        memset( &tx_band_data->tx_pa_state_cal_data[ pa_idx ].
                  linearizer_index[ wave_idx ][ chan_idx ].upper_bound_freq, 
                0,
                sizeof( tx_band_data->tx_pa_state_cal_data[ pa_idx ].
                          linearizer_index[ wave_idx ][ chan_idx ].upper_bound_freq 
                      ) 
              );

        tx_band_data->tx_pa_state_cal_data[ pa_idx ].
          linearizer_index[ wave_idx ][ chan_idx ].node_data.aux_lin_data = TX_LINEARIZER_NO_AUX_DATA;
  
        tx_band_data->tx_pa_state_cal_data[ pa_idx ].
          linearizer_index[ wave_idx ][ chan_idx ].node_data.lin_data = NULL;

        tx_band_data->tx_pa_state_cal_data[ pa_idx ].
          linearizer_index[ wave_idx ][ chan_idx ].node_data.xpt_data = NULL;

      } /* for( wave_idx ) */
    } /* for( chan_idx ) */
  } /* for( pa_idx ) */

  return api_status;
} /* rfcommon_core_init_linearizer */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  De-allocates (frees) memory for all linearizers

  @param tx_band_data
  Pointer to all linerizer for which to de-allocate memory

  @return 
  TRUE if all de-allocation succeeded 
*/
boolean
rfcommon_core_dealloc_linearizer_tables
(
  tx_band_cal_type *tx_band_data
)
{
  /* Overall API status */
  boolean api_status = TRUE;
  uint8 pa_idx;
  uint8 wave_idx;
  uint8 chan_idx;

  /* Free memory for all linearizers */

  for( pa_idx = 0; pa_idx < RFCOM_TXLIN_NUM_LINEARIZERS; pa_idx++ )
  {
    for( chan_idx = 0; chan_idx < RFCOM_TXLIN_NUM_CHANNELS; chan_idx++ )
    { 
      for( wave_idx = 0; wave_idx < TX_LINEARIZER_NUM_WAVEFORM; wave_idx++ )
      { 
        if( tx_band_data->tx_pa_state_cal_data[ pa_idx ].
              linearizer_index[ wave_idx ][ chan_idx ].node_data.lin_data != NULL  )
        {
          modem_mem_free( tx_band_data->tx_pa_state_cal_data[ pa_idx ].
                            linearizer_index[ wave_idx ][ chan_idx ].node_data.lin_data,
                          MODEM_MEM_CLIENT_RFA);
          tx_band_data->tx_pa_state_cal_data[ pa_idx ].
                          linearizer_index[ wave_idx ][ chan_idx ].node_data.lin_data = NULL;
        }
      } /* for( wave_idx ) */
    } /* for( chan_idx ) */
  } /* for( pa_idx ) */

  return api_status;
} /* rfcommon_core_dealloc_linearizer_tables */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  De-allocates (frees) memory for all dpd tables

  @param tx_band_data
  Pointer to all dpd data for which to de-allocate memory

  @return 
  TRUE if all de-allocation succeeded 
*/
boolean
rfcommon_core_dealloc_dpd_tables
(
  tx_band_cal_type *tx_band_data
)
{
  /* Overall API status */
  boolean api_status = TRUE;
  return api_status;
} /* rfcommon_core_dealloc_dpd_tables */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  De-allocates (frees) memory for all XPT data

  @param tx_band_data
  Structure containing XPT data for which to de-allocate memory

  @return 
  TRUE if all de-allocation succeeded 
*/
boolean
rfcommon_core_dealloc_xpt_data
(
  tx_band_cal_type *tx_band_data
)
{
  /* Overall API status */
  boolean api_status = TRUE;
  uint8 pa_idx;
  uint8 wave_idx;
  uint8 chan_idx;
  uint8 rgi_idx;

  /* Free memory for all dpd tables */

  for( pa_idx = 0; pa_idx < RFCOM_TXLIN_NUM_LINEARIZERS; pa_idx++ )
  {
    for( chan_idx = 0; chan_idx < RFCOM_TXLIN_NUM_CHANNELS; chan_idx++ )
    {
      for( wave_idx = 0; wave_idx < TX_LINEARIZER_NUM_WAVEFORM; wave_idx++ )
      {
        /* XPT Data */
        if( tx_band_data->tx_pa_state_cal_data[ pa_idx ].
              linearizer_index[ wave_idx ][ chan_idx ].node_data.xpt_data != NULL  )
        {
          /* AMAM */
          if( tx_band_data->tx_pa_state_cal_data[ pa_idx ].
              linearizer_index[ wave_idx ][ chan_idx ].node_data.xpt_data->xpt_am_table != NULL  )
          {
            /* for( rgi ) */
            for( rgi_idx = 0; rgi_idx < RFCOM_TXLIN_NUM_SEGMENTS; ++rgi_idx )
            {
              if(tx_band_data->tx_pa_state_cal_data[ pa_idx ].linearizer_index[ wave_idx ][ chan_idx ].node_data.
                   xpt_data->xpt_am_table->xpt_am_row[rgi_idx] != NULL)
              {
                modem_mem_free( tx_band_data->tx_pa_state_cal_data[ pa_idx ].linearizer_index[ wave_idx ][ chan_idx ].node_data.
                                  xpt_data->xpt_am_table->xpt_am_row[rgi_idx],
                                MODEM_MEM_CLIENT_RFA);
                tx_band_data->tx_pa_state_cal_data[ pa_idx ].linearizer_index[ wave_idx ][ chan_idx ].node_data.
                   xpt_data->xpt_am_table->xpt_am_row[rgi_idx] = NULL;
              }
            }/* for( rgi ) */
            modem_mem_free( tx_band_data->tx_pa_state_cal_data[ pa_idx ].
                              linearizer_index[ wave_idx ][ chan_idx ].node_data.xpt_data->xpt_am_table,
                            MODEM_MEM_CLIENT_RFA);
            tx_band_data->tx_pa_state_cal_data[ pa_idx ].
                            linearizer_index[ wave_idx ][ chan_idx ].node_data.xpt_data->xpt_am_table = NULL;
          }/* AMAM */

          /* AMPM */
          if( tx_band_data->tx_pa_state_cal_data[ pa_idx ].
              linearizer_index[ wave_idx ][ chan_idx ].node_data.xpt_data->xpt_pm_table != NULL  )
          {
            /* for( rgi ) */
            for( rgi_idx = 0; rgi_idx < RFCOM_TXLIN_NUM_SEGMENTS; ++rgi_idx )
            {
              if(tx_band_data->tx_pa_state_cal_data[ pa_idx ].linearizer_index[ wave_idx ][ chan_idx ].node_data.
                   xpt_data->xpt_pm_table->xpt_pm_row[rgi_idx] != NULL)
              {
                modem_mem_free( tx_band_data->tx_pa_state_cal_data[ pa_idx ].linearizer_index[ wave_idx ][ chan_idx ].node_data.
                                  xpt_data->xpt_pm_table->xpt_pm_row[rgi_idx],
                                MODEM_MEM_CLIENT_RFA);
                tx_band_data->tx_pa_state_cal_data[ pa_idx ].linearizer_index[ wave_idx ][ chan_idx ].node_data.
                   xpt_data->xpt_pm_table->xpt_pm_row[rgi_idx] = NULL;
              }
            }/* for( rgi ) */
            modem_mem_free( tx_band_data->tx_pa_state_cal_data[ pa_idx ].
                              linearizer_index[ wave_idx ][ chan_idx ].node_data.xpt_data->xpt_pm_table,
                            MODEM_MEM_CLIENT_RFA);
            tx_band_data->tx_pa_state_cal_data[ pa_idx ].
                            linearizer_index[ wave_idx ][ chan_idx ].node_data.xpt_data->xpt_pm_table = NULL;
          }/* AMPM */

          /* Detrough */
          if( tx_band_data->tx_pa_state_cal_data[ pa_idx ].
              linearizer_index[ wave_idx ][ chan_idx ].node_data.xpt_data->xpt_dt_table != NULL  )
          {
            /* for( rgi ) */
            for( rgi_idx = 0; rgi_idx < RFCOM_TXLIN_NUM_SEGMENTS; ++rgi_idx )
            {
              if(tx_band_data->tx_pa_state_cal_data[ pa_idx ].linearizer_index[ wave_idx ][ chan_idx ].node_data.
                   xpt_data->xpt_dt_table->xpt_dt_row[rgi_idx] != NULL)
              {
                modem_mem_free( tx_band_data->tx_pa_state_cal_data[ pa_idx ].linearizer_index[ wave_idx ][ chan_idx ].node_data.
                                  xpt_data->xpt_dt_table->xpt_dt_row[rgi_idx],
                                MODEM_MEM_CLIENT_RFA);
                tx_band_data->tx_pa_state_cal_data[ pa_idx ].linearizer_index[ wave_idx ][ chan_idx ].node_data.
                   xpt_data->xpt_dt_table->xpt_dt_row[rgi_idx] = NULL;
              }
            }/* for( rgi ) */
            modem_mem_free( tx_band_data->tx_pa_state_cal_data[ pa_idx ].
                              linearizer_index[ wave_idx ][ chan_idx ].node_data.xpt_data->xpt_dt_table,
                            MODEM_MEM_CLIENT_RFA);
            tx_band_data->tx_pa_state_cal_data[ pa_idx ].
                            linearizer_index[ wave_idx ][ chan_idx ].node_data.xpt_data->xpt_dt_table = NULL;
          }/* Detrough */

          /* Main XPT pointers table */
          modem_mem_free( tx_band_data->tx_pa_state_cal_data[ pa_idx ].
                            linearizer_index[ wave_idx ][ chan_idx ].node_data.xpt_data,
                          MODEM_MEM_CLIENT_RFA);
          tx_band_data->tx_pa_state_cal_data[ pa_idx ].
                          linearizer_index[ wave_idx ][ chan_idx ].node_data.xpt_data = NULL;
        } 
      } /* for( wave_idx ) */
    } /* for( chan_idx ) */
  } /* for( pa_idx ) */

  return api_status;
} /* rfcommon_core_dealloc_xpt_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  De-allocates (frees) memory for all tx band cal
   
  @details 
  Upper layers should use this API for de-allocating memory for
  all pointer items in tx_band_cal_type.
   
  @param tx_band_data
  Pointer to all tx band cal data for which to de-allocate memory
   
  @return 
  TRUE if all de-allocation succeeded 
*/
boolean
rfcommon_core_dealloc_tx_band_cal
(
  tx_band_cal_type *tx_band_data
)
{
  /* Overall API status */
  boolean api_status = TRUE;

  api_status &= rfcommon_core_dealloc_linearizer_tables( tx_band_data );
  api_status &= rfcommon_core_dealloc_dpd_tables( tx_band_data );
  api_status &= rfcommon_core_dealloc_xpt_data( tx_band_data );

  return api_status;
} 

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Populate the mission mode internal data structure from NV per band/tech
 
  @details
  This is a common function for all technologies that populates the mission mode
  internal data structure from NV to hold multi-linearizer Cal data per band/tech.
  It first retrieves the NV data and decompresses it, for fixed portion, directly
  copy the data from NV with converting channel number to frequency. It then dynamically
  allocate memory and copy data from NV for channels with calibrated linearizer tables
   
  @param linearizer_rev 
  The linearizer revision 
   
  @param item_code
  RFNV number for the Tx multi-linearizer Cal data.

  @param tx_lut_band_data
  pointer to the entire internal Tx linerizer band data structure which includes all 
  calibrated linearizer tables, Tx Cal frequencies, Tx linearizer frequenfcies, Tx freqcomp 
  tables etc of one band/technology

  @param convert_freq_handler
  CB handler to convert channel to frequency
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal

*/
boolean
rfcommon_core_load_linearizer_from_nv
(
  tx_multi_lin_rev_type linearizer_rev,
  rfnv_item_id_enum_type item_code, 
  tx_band_cal_type *tx_lut_band_data,
  rfcommon_core_convert_freq_handler_type convert_freq_handler
)
{
  /* Overall API status */
  boolean api_status = TRUE;
  
  /* Initialize linearizer to invalid settings first */
  api_status &= rfcommon_core_init_linearizer( tx_lut_band_data );

  /* There is no point to continue if init already failed */
  if( api_status == FALSE )
  {
    return api_status;
  }

  /* Load linearizer from NV */
  switch( linearizer_rev )
  {
    case TX_MULTI_LIN_REV_1:
      api_status &= rfcommon_core_load_linearizer_v1_from_nv( item_code,
                                                              tx_lut_band_data,
                                                              convert_freq_handler
                                                            );
      break;

    case TX_MULTI_LIN_REV_3:
      api_status &= rfcommon_core_load_linearizer_v3_from_nv( item_code,
                                                              tx_lut_band_data,
                                                              convert_freq_handler
                                                            );
      break;

    default:
        RF_MSG_1( RF_ERROR, "Multi-lin version %d not supported.", (linearizer_rev+1) );
        api_status = FALSE;
      break;
  } /* switch( linearizer_rev ) */
  
  return api_status;
}/* rfcommon_core_load_linearizer_from_nv */
  
/*----------------------------------------------------------------------------*/
void 
rfcommon_core_load_dynamic_linearizer_v1_entry_from_nv
(
  void* nv_data,
  tx_band_cal_type *tx_lut_band_data,
  uint8 pa_range,
  uint8 tx_waveform,
  uint8 tx_chan_idx,
  uint8 fcomp_bin_num,
  uint16* rgi_thresh_info,
  rfcommon_core_convert_freq_handler_type convert_freq_handler
)
{
  /* Internal lin node - Common */
  tx_multi_linearizer_index_node_type *lin_node_index = NULL;
  /* Internal TX lin table*/
  tx_linearizer_table_type *lin_table_ptr = NULL;

  /* NV lin data - Multi-lin rev1 */
  tx_band_cal_nv_type *nv_data_ptr = NULL;
  /* NV lin node - Multi-lin rev1 */
  tx_multi_linearizer_index_node_nv_type *lin_node_nv_index = NULL; 
  /* NV lin table - Multi-lin rev1 */
  tx_linearizer_table_nv_type *lin_table_nv_loc = NULL; 

  uint8 tx_lin_idx;
  uint8 fcomp_list_idx = 0;
  uint16 rgi;
  uint16 rgi_thresh;

  /************************************************************************
  Note: Both linearizer tables are functionally the same, if making any
  updates or bug fixes, please apply to all cases
  **********************************************************************/
  /* FIRST SWEEP waveform not valid for Multi-lin V1 NVs */
  if(tx_waveform >= RFCOM_TXLIN_NUM_V1_WAVEFORMS)
  {
    return;
  }

  if(fcomp_bin_num > RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN)
  {
    RF_MSG_2( RF_ERROR, "MultiLin V1. Num fcomp bins % must be <= %d",
              fcomp_bin_num, RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN );
    return;
  }

  if(rgi_thresh_info == NULL)
  {
    RF_MSG( RF_ERROR, "MultiLin V1. Null freqcomp info." );
    return;
  }

  nv_data_ptr = (tx_band_cal_nv_type *) nv_data;
  lin_node_index = &tx_lut_band_data->tx_pa_state_cal_data[pa_range].linearizer_index[tx_waveform][tx_chan_idx];
  lin_node_nv_index = &nv_data_ptr->tx_pa_state_cal_data[pa_range].linearizer_index[tx_waveform][tx_chan_idx];

  /*allocate dynamic linearizer node memory if upper_bound_chan != 0xFFFF*/
  if (lin_node_nv_index->upper_bound_chan!=0xFFFF)
  {
    lin_node_index->upper_bound_freq = convert_freq_handler(lin_node_nv_index->upper_bound_chan);

    lin_table_ptr = (tx_linearizer_table_type*)modem_mem_alloc( sizeof(tx_linearizer_table_type),
                                                                MODEM_MEM_CLIENT_RFA ); 

    if (lin_table_ptr != NULL)
    {
      memset(lin_table_ptr, 0, sizeof(tx_linearizer_table_type) );

      lin_table_nv_loc = (tx_linearizer_table_nv_type*)((uint32)nv_data_ptr +\
                                                        NV_STRUCT_SIZE+lin_node_nv_index->table_offset*sizeof(tx_linearizer_table_nv_type));

      /* Save the number of freq-comps used by this lin */
      lin_table_ptr->num_fcomp = fcomp_bin_num;

      /* Copy dynamic linearizer table from NV to missimon mode structure */
      /* Can not do memcpy here since the two structures are different*/
      for (tx_lin_idx=0; tx_lin_idx<RFCOM_TXLIN_NUM_SEGMENTS; tx_lin_idx++)
      {
        rgi = lin_table_nv_loc->rgi[tx_lin_idx];
        lin_table_ptr->table[tx_lin_idx].rgi = rgi;
        lin_table_ptr->table[tx_lin_idx].power = lin_table_nv_loc->power[tx_lin_idx];
        lin_table_ptr->table[tx_lin_idx].apt = lin_table_nv_loc->apt[tx_lin_idx];

        /*  V1 multi lin doesnt have RSB or LOFT corrections */
        lin_table_ptr->table[tx_lin_idx].rsb_correction_coeff.alpha = RFCOM_TXLIN_INVALID_RSB_VAL ;
        lin_table_ptr->table[tx_lin_idx].rsb_correction_coeff.beta = RFCOM_TXLIN_INVALID_RSB_VAL ;
        lin_table_ptr->table[tx_lin_idx].loft_correction = RFCOM_TXLIN_INVALID_LOFT_VAL ;

        /* At this point thresholds are supposed to be ordered in increasing magnitudes
           For EVERY RGI, search for the qualifying thresh in rgi_thresh_info
           and populate corresponding fcomp_idx */
        fcomp_list_idx = 0;
        /* If current RGI exceeds thresh, check the next lower index rgi_thresh_info for the next thresh */
        do
        {
          rgi_thresh = rgi_thresh_info[fcomp_list_idx];
          fcomp_list_idx++;
        }
        while( (fcomp_list_idx < fcomp_bin_num) && (rgi > rgi_thresh) );
        /* Backoff just one */
        fcomp_list_idx -= 1;

        /* "point" to the corresponding freq-comp array*/
        lin_table_ptr->table[tx_lin_idx].fcomp_idx = fcomp_list_idx;
      }

      /* convert tx channel to frequency using handler */ 
      lin_table_ptr->chan_freq = convert_freq_handler(lin_table_nv_loc->tx_chan);

      /* Assign the Tx linearizer node index pointer */
      lin_node_index->node_data.lin_data = lin_table_ptr;
    }
    else
    {
      RF_MSG_3( RF_ERROR, 
            "Fail to allocate memory for linearizer node for waveform:%d, pa_range:%d, tx chan index:%d!", 
            tx_waveform, pa_range, tx_chan_idx); 
    }
  }
  else
  {
    /* If no Cal data available for this channel */
    lin_node_index->upper_bound_freq = 0;
    lin_node_index->node_data.lin_data = NULL;
    lin_node_index->node_data.aux_lin_data = TX_LINEARIZER_NO_AUX_DATA;
  }
}/* rfcommon_core_load_dynamic_linearizer_v1_entry_from_nv */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain the algorithm to be used for linearizer temperature compensation

  @details
  Based on MultiLin NV version information and Temp Comp NV version 
  information, this function will decide which algorithm must be used by 
  SW for linearizer temperature compensation

  =============================================================================
  MultiLin | xPT Mode | Temp Comp | Compensation Value |    Lin Comp Algo
    Ver    |          |   NV Ver  |       Compute      |*RFCOM_TXLIN_TEMP_COMP*
  -----------------------------------------------------------------------------
     V1    | APT only |  Legacy   |   Legacy method    |   Pout compensation
           |          |           |  w/ no ref ADC     | with APT bias align
           |          |           |                    |   ALGO_MULTILIN_V1
           |          |           |                    |
     V3    | APT only |  Legacy   |   Legacy method    |   Pin compensation 
           |          |           |  w/ no ref ADC     |        only
           |          |           |                    | ALGO_MULTILIN_V3
           |          |           |                    |            _LEGACY_NV
           |          |           |                    |
     V3    |   xPT    | New Pin/  |   New Computation  |   Pin and Pout comp
           |          | Pout NV   |  w/ normalization  |      algorithms
           |          |           | using ref Lin ADC  |   ALGO_MULTILIN_V3
           |          |           |                    |
  =============================================================================

  @param lin_rev_used
  MultiLin Version that is currently being used

  @param nv_data
  Temp Comp NV data

  @return 
  Temp Comp algorithm type based on NV inputs

*/
rfcom_txlin_temp_comp_algo_type 
rfcommon_core_txlin_get_temp_comp_algo
(
  tx_multi_lin_rev_type lin_rev_used,
  rfcom_txlin_temp_comp_nv_data_type *nv_data
)
{
  rfcom_txlin_temp_comp_algo_type algo_decision;

  /* MultiLin Rev V1 */
  if ( lin_rev_used == TX_MULTI_LIN_REV_1 )
  {
    if ( nv_data->is_legacy_temp_comp_nv == TRUE )
    {
      algo_decision = RFCOM_TXLIN_TEMP_COMP_ALGO_MULTILIN_V1;
    }
    else
    {
      RF_MSG( RF_ERROR, "rfcommon_core_txlin_get_temp_comp_algo: Invalid "
                        "Temp Comp NV and MultiLin configuration" );
      algo_decision = RFCOM_TXLIN_TEMP_COMP_ALGO_INVALID;
    }
  }
  /* MultiLin Rev V3 */
  else if ( lin_rev_used == TX_MULTI_LIN_REV_3 )
  {
    /* Check for new temp comp NV first */
    if ( nv_data->has_pin_pout_temp_comp_nv == TRUE )
    {
      algo_decision = RFCOM_TXLIN_TEMP_COMP_ALGO_MULTILIN_V3;
    }
    /* If not present, check for legacy temp comp NV */
    else if ( nv_data->is_legacy_temp_comp_nv == TRUE )
    {
      algo_decision = RFCOM_TXLIN_TEMP_COMP_ALGO_MULTILIN_V3_LEGACY_NV;
    }
    else
    {
      RF_MSG( RF_ERROR, "rfcommon_core_txlin_get_temp_comp_algo: Invalid "
                        "Temp Comp NV and MultiLin configuration" );
      algo_decision = RFCOM_TXLIN_TEMP_COMP_ALGO_INVALID;
    }
  }
  else
  {
    RF_MSG( RF_ERROR, "rfcommon_core_txlin_get_temp_comp_algo: Invalid "
                      "Temp Comp NV and MultiLin configuration" );
    algo_decision = RFCOM_TXLIN_TEMP_COMP_ALGO_INVALID;
  }

  return algo_decision;

} /* rfcommon_core_txlin_get_temp_comp_algo */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns whether Pin Cal is enabled for a given PA state in Pin
  Cal NV
 
  @param lin_temp_comp_data
  temp comp data which contains Tx Char channel list, which contains
  Pin_cal_vs_freq NV structure
 
  @param pa_state
  PA state
 
  @return pin_cal_enabled
  TRUE if PIN Cal is enbaled. FALSE if not enabled or API is in error condition
*/
boolean
rfcommon_core_txlin_has_pin_cal_offset
(
  rfcom_txlin_temp_comp_nv_data_type* lin_temp_comp_data,
  uint8 pa_state
)
{
  boolean pin_cal_enabled = FALSE;

  if ( lin_temp_comp_data == NULL || pa_state >= RFCOM_TXLIN_NUM_LINEARIZERS )
  {
    RF_MSG_1( RF_ERROR, "rfcommon_core_txlin_has_pin_cal_offset:"
                        "NULL pointer or invalide PA state %d.", pa_state );
    pin_cal_enabled = FALSE;
  }
  else
  {
    pin_cal_enabled = lin_temp_comp_data->pin_cal_vs_freq.has_pin_freq_comp_cal_nv[ pa_state ];
  }

  return pin_cal_enabled;
}/* rfcommon_core_txlin_has_pin_cal_offset */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Obtain the temperature compensation based on legacy temp comp NV items
 
  @details
  This API uses the input data for current temperature and frequency and 
  legacy temp comp NV data to compute the current temperature compensation 
  value. The value is obtained through linearizer interpolation of the 
  legacy temp comp offsets. There is also support to add a frequency 
  dependent delta value, which is used in legacy NV functionality by WCDMA.

  @param temp_vs_freq_params
  Provide current temperature and frequency information

  @param temp_comp_nv_data
  Pointer to all temp comp NV information. This API only uses the legacy 
  temp comp data

  @return 
  Power compensation value in dB10 units 
*/
static
int16
rfcommon_core_txlin_get_lin_vs_temp_compensation
(
  tx_lin_vs_temp_vs_freq_comp_param_type *temp_vs_freq_params,
  rfcom_txlin_temp_comp_nv_data_type *temp_comp_nv_data
)
{
  /* Input params */
  /* Current Tx frequency */
  uint32 tx_freq;
  /* Cal chan frequency list */
  uint32 *tx_cal_freq;
  /* Cal chan list size */
  uint8  tx_cal_chan_size;
  /* Current therm read */
  uint16 temp_read;
  /* Therm cal data */
  uint16 *therm_cal;
  /* Pointer to legacy temp comp NV data */
  rfcommon_core_txlin_vs_temp_legacy_type *lin_vs_temp;
  /* Requested PA state */
  uint8 pa_state;

  int16  pcomp_val;
  uint8  i;
  int16  freq_comp_indicator, freq_interp;
  uint8  freq_index;
  int16  temp_vs_freq_comp_deltas[RFCOM_TXLIN_TEMP_BIN_SIZE];
  int16  temp_vs_freq_comp_n, temp_vs_freq_comp_n_plus_1;
  int32  result;


  /* This array is used for finding the freq comp bin and interp factor*/
  const int16 freq_comp_indicator_array[RFCOM_TXLIN_NUM_CHANNELS] =
  {
    0*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,  1*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    2*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,  3*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    4*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,  5*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    6*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,  7*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    8*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,  9*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    10*RFCOM_TXLIN_FREQ_BIN_RESOLUTION, 11*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    12*RFCOM_TXLIN_FREQ_BIN_RESOLUTION, 13*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    14*RFCOM_TXLIN_FREQ_BIN_RESOLUTION, 15*RFCOM_TXLIN_FREQ_BIN_RESOLUTION
  };

  /* Extract the compensation parameters */
  pa_state = temp_vs_freq_params->pa_state;

  tx_freq = temp_vs_freq_params->freq_params.tx_freq;
  tx_cal_freq = (uint32*)
                  temp_vs_freq_params->tx_multilin_nv_cal_ptr->tx_cal_freq;
  tx_cal_chan_size = 
             temp_vs_freq_params->tx_multilin_nv_cal_ptr->tx_cal_chan_size;

  temp_read = temp_vs_freq_params->temp_params.temp_read;
  therm_cal = temp_comp_nv_data->tx_therm_bins;

  lin_vs_temp = &temp_comp_nv_data->legacy_lin_vs_temp[pa_state];

  /* Do error checking */
  if ( tx_cal_freq == NULL || therm_cal == NULL || lin_vs_temp == NULL )
  {
    RF_MSG( RF_ERROR, 
            "rfcommon_core_txlin_get_lin_vs_temp_compensation: "
            "Null pointer input to compensation function" );
    return 0;
  }

  if ( tx_cal_chan_size > RFCOM_TXLIN_NUM_CHANNELS )
  {
    RF_MSG_2( RF_ERROR, 
              "rfcommon_core_txlin_get_lin_vs_temp_compensation: "
              "Cal Chan Size %d exceeds max supported num channels %d",
              tx_cal_chan_size, RFCOM_TXLIN_NUM_CHANNELS );
    return 0;
  }

  /* ============================ Proceed with calculation ================= */
  /* Get freq_comp bin and multiplier */
  freq_comp_indicator = rf_lininterp_u32_s16( tx_cal_freq,
                                              freq_comp_indicator_array,
                                              tx_cal_chan_size,
                                              tx_freq );

  /* Calculate freq_comp bin index and interp factor */
  freq_interp = freq_comp_indicator % RFCOM_TXLIN_FREQ_BIN_RESOLUTION;
  freq_index  = (uint8)( (freq_comp_indicator - freq_interp)/
                                    RFCOM_TXLIN_FREQ_BIN_RESOLUTION );

  /* Compute the vs freq deltas supported by legacy WCDMA temp comp */
  /* ------------ Interpolate at current frequency across all temps -------- */
  for( i = 0; i < RFCOM_TXLIN_TEMP_BIN_SIZE; i++ )
  {
    /* Get the temp_comp arrays for the freq bins */
    temp_vs_freq_comp_n = 
                        (int16)lin_vs_temp->temp_vs_freq_delta[i][freq_index];

    /* Interpolate correctly if we're in the last but one frequency bin */
    if ( freq_index < RFCOM_TXLIN_NUM_CHANNELS - 1 )
    {
      temp_vs_freq_comp_n_plus_1 = 
              (int16)lin_vs_temp->temp_vs_freq_delta[i][freq_index+1];
    }
    /* We're out of bounds. Handle correctly */
    else
    {
      temp_vs_freq_comp_n_plus_1 = 
              (int16)lin_vs_temp->temp_vs_freq_delta[i][freq_index];
    }

    /* Linear interpolation of two temp_comp arrays between two freq_bins */
    result = rf_divide_with_rounding( ( (temp_vs_freq_comp_n_plus_1 - 
                                                  temp_vs_freq_comp_n)*
                                       freq_interp ),
                                      RFCOM_TXLIN_FREQ_BIN_RESOLUTION );
    result += temp_vs_freq_comp_n;
    temp_vs_freq_comp_deltas[i] = (int16)rf_limit_s32( result, -32768, 32767 );

  }

  /* Compute the final temp comp value by using the legacy offset value and 
  adding the vs freq delta on top of it */
  pcomp_val = 
    (rf_lininterp_u16_s16( therm_cal,
                           lin_vs_temp->offset,
                           RFCOM_TXLIN_TEMP_BIN_SIZE,
                           temp_read )) +
    (rf_lininterp_u16_s16( therm_cal,
                           temp_vs_freq_comp_deltas,
                           RFCOM_TXLIN_TEMP_BIN_SIZE,
                           temp_read ));

  return pcomp_val;
} /* rfcommon_core_txlin_get_lin_vs_temp_compensation */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Obtain the temperature and frequency compensation value based on the 2-D
  NV input provided
 
  @details
  This API uses the input Frequency and Temperature Compensation tables from 
  NV, as well as current frequency and temperature data to obtain a 2 
  dimensional compensation value in dB10 units. The compensation value is 
  then provided for Pout or Pin linearizer compensation by each tech.

  Pin/Pout characterization is performed across temperature and frequency, 
  and the offsets are stored in NV relative to a specific temperature and 
  frequency. When computing the temp vs freq offsets during calibration and 
  online operation, the static NV data must be normalized to the frequency 
  and temperature of calibration. The ref_lin_adc value provides the Therm 
  ADC value at which the linearizer is calibrated.

  @param temp_vs_freq_params
  Provide current temperature and frequency information

  @param lin_vs_temp_vs_freq
  Pin or Pout static NV data across temp and freq, for all PA states

  @param normalization
  Normalize the compensation offsets with respect to multi-lin

  @return 
  Power compensation value in dB10 units 
*/
static
int16
rfcommon_core_txlin_get_lin_vs_temp_vs_freq_compensation_w_adc
(
  tx_lin_vs_temp_vs_freq_comp_param_type *temp_vs_freq_params,
  rfcom_txlin_temp_comp_nv_data_type *temp_comp_nv_data,
  int16  (*lin_vs_temp_vs_freq)[RFCOM_TXLIN_TEMP_BIN_SIZE]
                               [RFCOM_TXLIN_NUM_CHANNELS],
  rfcom_txlin_temp_vs_freq_normalization_type normalization
)
{
  /* Input params */
  /* Current Tx frequency */
  uint32 tx_freq;
  /* Reference Linearizer Tx frequency */
  uint32 ref_lin_tx_freq;
  /* Char chan frequency list */
  uint32 *tx_char_freq;
  /* Char chan list size */
  uint8  tx_char_chan_size;
  /* Current therm read */
  uint16 temp_read;
  /* Reference Linearizer therm read */
  uint16 ref_lin_temp_adc;
  /* Therm cal data */
  uint16 *therm_cal;
  /* Requested PA state */
  uint8 pa_state;

  int16  pcomp_val;
  uint8  i;
  int16  freq_comp_indicator, freq_interp;
  uint8  freq_index;
  int16  temp_vs_freq_comp[RFCOM_TXLIN_TEMP_BIN_SIZE];
  int16  temp_vs_freq_comp_n, temp_vs_freq_comp_n_plus_1;
  int32  result;
  int16  center_point_pcomp_val;
  int16  normalize_freq_interp;
  uint8  normalize_freq_index;
  int16  normalize_temp_vs_freq_comp[RFCOM_TXLIN_TEMP_BIN_SIZE];

  /* This array is used for finding the freq comp bin and interp factor*/
  const int16 freq_comp_indicator_array[RFCOM_TXLIN_NUM_CHANNELS] =
  {
    0*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,  1*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    2*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,  3*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    4*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,  5*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    6*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,  7*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    8*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,  9*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    10*RFCOM_TXLIN_FREQ_BIN_RESOLUTION, 11*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    12*RFCOM_TXLIN_FREQ_BIN_RESOLUTION, 13*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    14*RFCOM_TXLIN_FREQ_BIN_RESOLUTION, 15*RFCOM_TXLIN_FREQ_BIN_RESOLUTION
  };

  /* Extract the compensation parameters */
  pa_state = temp_vs_freq_params->pa_state;

  tx_freq = temp_vs_freq_params->freq_params.tx_freq;
  tx_char_freq = (uint32*)temp_comp_nv_data->tx_char_freq;
  tx_char_chan_size = temp_comp_nv_data->tx_char_chan_size;
  ref_lin_tx_freq = temp_vs_freq_params->freq_params.ref_lin_tx_freq;

  temp_read = temp_vs_freq_params->temp_params.temp_read;
  ref_lin_temp_adc = temp_vs_freq_params->temp_params.ref_lin_temp_adc;
  therm_cal = temp_comp_nv_data->tx_therm_bins;
  
  /* Do error checking */
  if ( tx_char_freq == NULL || therm_cal == NULL || 
       lin_vs_temp_vs_freq == NULL )
  {
    RF_MSG( RF_ERROR, 
            "rfcommon_core_txlin_get_lin_vs_temp_vs_freq_compensation: "
            "Null pointer input to compensation function" );
    return 0;
  }

  if ( tx_char_chan_size == 0 || tx_char_chan_size > RFCOM_TXLIN_NUM_CHANNELS )
  {
    RF_MSG_2( RF_ERROR, 
              "rfcommon_core_txlin_get_lin_vs_temp_vs_freq_compensation: "
              "Char Chan Size %d is invalid, Max supported num channels %d",
              tx_char_chan_size, RFCOM_TXLIN_NUM_CHANNELS );
    return 0;
  }

  /* ============================ Proceed with calculation ================= */
  /* Get freq_comp bin and multiplier */
  freq_comp_indicator = rf_lininterp_u32_s16( tx_char_freq,
                                              freq_comp_indicator_array,
                                              tx_char_chan_size,
                                              tx_freq );

  /* Calculate freq_comp bin index and interp factor */
  freq_interp = freq_comp_indicator % RFCOM_TXLIN_FREQ_BIN_RESOLUTION;
  freq_index  = (uint8)( (freq_comp_indicator - freq_interp)/
                                    RFCOM_TXLIN_FREQ_BIN_RESOLUTION );

  /* Get freq_comp bin for the reference linearizer channel and its 
  multiplier */
  freq_comp_indicator = rf_lininterp_u32_s16( tx_char_freq,
                                              freq_comp_indicator_array,
                                              tx_char_chan_size,
                                              ref_lin_tx_freq );

  if ( normalization == 
              RFCOM_TXLIN_NORMALIZE_TO_REF_FREQ_REF_TEMP )
  {
    /* Calculate freq_comp bin index and interp factor for ref channel 
    linearizer */
    normalize_freq_interp = 
                 freq_comp_indicator % RFCOM_TXLIN_FREQ_BIN_RESOLUTION;
    normalize_freq_index  = (uint8)( 
                              (freq_comp_indicator - normalize_freq_interp)/
                                      RFCOM_TXLIN_FREQ_BIN_RESOLUTION );
  }
  else if ( normalization ==
              RFCOM_TXLIN_NORMALIZE_TO_CURR_FREQ_REF_TEMP )
  {
    /* Use freq_comp bin index and interp factor from the current 
    Tx frequency */
    normalize_freq_interp = freq_interp;
    normalize_freq_index = freq_index;
  }
  else
  {
    RF_MSG_1( RF_ERROR, 
              "rfcommon_core_txlin_get_lin_vs_temp_vs_freq_compensation: "
              "Invalid offset normalization type %d",
              normalization );
    return 0;
  }

  /* Compute reference point corresponding to reference linearizer at 
  calibrated temperature */
  for( i = 0; i < RFCOM_TXLIN_TEMP_BIN_SIZE; i++ )
  {
    /* Get the temp_comp arrays for the freq bins */
    temp_vs_freq_comp_n = (int16)lin_vs_temp_vs_freq
                                        [pa_state][i][normalize_freq_index];

    /* Interpolate correctly if we're in the last but one frequency bin */
    if ( freq_index < RFCOM_TXLIN_NUM_CHANNELS - 1 )
    {
      temp_vs_freq_comp_n_plus_1 = 
             (int16)lin_vs_temp_vs_freq[pa_state][i][normalize_freq_index+1];
    }
    /* We're out of bounds. Handle correctly */
    else
    {
      temp_vs_freq_comp_n_plus_1 = 
               (int16)lin_vs_temp_vs_freq[pa_state][i][normalize_freq_index];
    }

    /* Linear interpolation of two temp_comp arrays between two freq_bins */
    result = rf_divide_with_rounding( ( (temp_vs_freq_comp_n_plus_1 - 
                                                  temp_vs_freq_comp_n)*
                                       normalize_freq_interp ),
                                      RFCOM_TXLIN_FREQ_BIN_RESOLUTION );
    result += temp_vs_freq_comp_n; 
    normalize_temp_vs_freq_comp[i] = 
                              (int16)rf_limit_s32( result, -32768, 32767 );
  }
  /* Interpolate at the linearizer ADC value to obtain the reference point */
  center_point_pcomp_val = 
    rf_lininterp_u16_s16( therm_cal,
                          normalize_temp_vs_freq_comp,
                          RFCOM_TXLIN_TEMP_BIN_SIZE,
                          ref_lin_temp_adc );

  /* ------------ Interpolate at current frequency across all temps -------- */
  for( i = 0; i < RFCOM_TXLIN_TEMP_BIN_SIZE; i++ )
  {
    /* Get the temp_comp arrays for the freq bins */
    temp_vs_freq_comp_n = (int16)lin_vs_temp_vs_freq[pa_state][i][freq_index];

    /* Interpolate correctly if we're in the last but one frequency bin */
    if ( freq_index < RFCOM_TXLIN_NUM_CHANNELS - 1 )
    {
      temp_vs_freq_comp_n_plus_1 = 
                        (int16)lin_vs_temp_vs_freq[pa_state][i][freq_index+1];
    }
    /* We're out of bounds. Handle correctly */
    else
    {
      temp_vs_freq_comp_n_plus_1 = 
                        (int16)lin_vs_temp_vs_freq[pa_state][i][freq_index];
    }

    /* Linear interpolation of two temp_comp arrays between two freq_bins */
    result = rf_divide_with_rounding( ( (temp_vs_freq_comp_n_plus_1 - 
                                                  temp_vs_freq_comp_n)*
                                       freq_interp ),
                                      RFCOM_TXLIN_FREQ_BIN_RESOLUTION );
    result += temp_vs_freq_comp_n;
    temp_vs_freq_comp[i] = (int16)rf_limit_s32( result, -32768, 32767 );

    /* Normalize to the centerpoint pcomp value based on linearizer 
    calibrated temperature */
    temp_vs_freq_comp[i] -= center_point_pcomp_val;
  }

  pcomp_val = 
    rf_lininterp_u16_s16( therm_cal,
                          temp_vs_freq_comp,
                          RFCOM_TXLIN_TEMP_BIN_SIZE,
                          temp_read );

  return pcomp_val;

} /* rfcommon_core_txlin_get_lin_vs_temp_vs_freq_compensation_w_adc */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Obtain the pin cal vs temp vs freq compensation value based on the 2-D
  NV input provided
 
  @details
  This API uses the input Frequency and Temperature Compensation tables from 
  NV, as well as current frequency and temperature data to obtain a 2 
  dimensional compensation value in dB10 units. The compensation value is 
  then provided for Pout or Pin linearizer compensation by each tech.

  Pin/Pout characterization is performed across temperature and frequency, 
  and the offsets are stored in NV relative to a specific temperature and 
  frequency. When computing the temp vs freq offsets during calibration and 
  online operation, the static NV data must be normalized to the frequency 
  and temperature of calibration. The ref_lin_adc value provides the Therm 
  ADC value at which the linearizer is calibrated.

  @param temp_vs_freq_params
  Provide current temperature and frequency information

  @param lin_vs_temp_vs_freq
  Pin or Pout static NV data across temp and freq, for all PA states

  @return 
  Power compensation value in dB10 units 
*/
static
int16
rfcommon_core_txlin_get_pin_cal_vs_temp_vs_freq_compensation_w_adc
(
  tx_lin_vs_temp_vs_freq_comp_param_type *temp_vs_freq_params,
  rfcom_txlin_temp_comp_nv_data_type *temp_comp_nv_data,
  int16  (*lin_vs_temp_vs_freq)[RFCOM_TXLIN_TEMP_BIN_SIZE]
                               [RFCOM_TXLIN_NUM_CHANNELS]
)
{
  /* Input params */
  /* Current Tx frequency */
  uint32 tx_freq;
  /* Reference Linearizer Tx frequency */
  //uint32 ref_lin_tx_freq;
  /* Char chan frequency list */
  uint32 *tx_char_freq;
  /* Char chan list size */
  uint8  tx_char_chan_size;
  /* Current therm read */
  uint16 temp_read;
  /* Reference Linearizer therm read */
  uint16 ref_lin_temp_adc;
  /* Therm cal data */
  uint16 *therm_cal;
  /* Requested PA state */
  uint8 pa_state;

  int16  pcomp_val;
  uint8  i;
  int16  freq_comp_indicator, freq_interp;
  uint8  freq_index;
  int16  static_pin_temp_i_freq_n, static_pin_temp_i_freq_n_plus_1;
  int16  pin_cal_vs_freq_comp_n, pin_cal_vs_freq_comp_n_plus_1;
  int32  temp_vs_freq_interp_val;
  int16  center_point_pcomp_val, curr_temp_pcomp_val, pin_cal_interp_result;
  int16  static_pin_vs_temp[RFCOM_TXLIN_TEMP_BIN_SIZE];

  /* This array is used for finding the freq comp bin and interp factor*/
  const int16 freq_comp_indicator_array[RFCOM_TXLIN_NUM_CHANNELS] =
  {
    0*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,  1*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    2*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,  3*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    4*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,  5*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    6*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,  7*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    8*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,  9*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    10*RFCOM_TXLIN_FREQ_BIN_RESOLUTION, 11*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    12*RFCOM_TXLIN_FREQ_BIN_RESOLUTION, 13*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    14*RFCOM_TXLIN_FREQ_BIN_RESOLUTION, 15*RFCOM_TXLIN_FREQ_BIN_RESOLUTION
  };

  /* Extract the compensation parameters */
  pa_state = temp_vs_freq_params->pa_state;

  tx_freq = temp_vs_freq_params->freq_params.tx_freq;
  tx_char_freq = (uint32*)temp_comp_nv_data->tx_char_freq;
  tx_char_chan_size = temp_comp_nv_data->tx_char_chan_size;
  //ref_lin_tx_freq = temp_vs_freq_params->freq_params.ref_lin_tx_freq;

  temp_read = temp_vs_freq_params->temp_params.temp_read;
  ref_lin_temp_adc = temp_vs_freq_params->temp_params.ref_lin_temp_adc;
  therm_cal = temp_comp_nv_data->tx_therm_bins;
  
  /* Do error checking */
  if ( tx_char_freq == NULL || therm_cal == NULL || 
       lin_vs_temp_vs_freq == NULL )
  {
    RF_MSG( RF_ERROR, 
            "rfcommon_core_txlin_get_pin_cal_vs_temp_vs_freq_compensation_w_adc: "
            "Null pointer input to compensation function" );
    return 0;
  }

  if ( tx_char_chan_size == 0 || tx_char_chan_size > RFCOM_TXLIN_NUM_CHANNELS )
  {
    RF_MSG_2( RF_ERROR, 
              "rfcommon_core_txlin_get_pin_cal_vs_temp_vs_freq_compensation_w_adc: "
              "Char Chan Size %d is invalid, Max supported num channels %d",
              tx_char_chan_size, RFCOM_TXLIN_NUM_CHANNELS );
    return 0;
  }

  /* ============================ Proceed with calculation ================= */
  /* Get freq_comp bin and multiplier */
  freq_comp_indicator = rf_lininterp_u32_s16( tx_char_freq,
                                              freq_comp_indicator_array,
                                              tx_char_chan_size,
                                              tx_freq );

  /* Calculate freq_comp bin index and interp factor */
  freq_interp = freq_comp_indicator % RFCOM_TXLIN_FREQ_BIN_RESOLUTION;
  freq_index  = (uint8)( (freq_comp_indicator - freq_interp)/
                                    RFCOM_TXLIN_FREQ_BIN_RESOLUTION );

  /* Compute reference point corresponding to reference linearizer at 
  calibrated temperature */
  for( i = 0; i < RFCOM_TXLIN_TEMP_BIN_SIZE; i++ )
  {
    /* Get the temp_comp arrays for the freq bins */
    static_pin_temp_i_freq_n = (int16)lin_vs_temp_vs_freq[pa_state][i][freq_index];

    /* Interpolate correctly if we're in the last but one frequency bin */
    if ( freq_index < RFCOM_TXLIN_NUM_CHANNELS - 1 )
    {
      static_pin_temp_i_freq_n_plus_1 = 
             (int16)lin_vs_temp_vs_freq[pa_state][i][freq_index+1];
    }
    /* We're out of bounds. Handle correctly */
    else
    {
      static_pin_temp_i_freq_n_plus_1 = 
               (int16)lin_vs_temp_vs_freq[pa_state][i][freq_index];
    }

    /* Linear interpolation of two temp_comp arrays between two freq_bins */
    temp_vs_freq_interp_val = rf_divide_with_rounding( 
       ( (static_pin_temp_i_freq_n_plus_1 - static_pin_temp_i_freq_n) * 
         freq_interp ), RFCOM_TXLIN_FREQ_BIN_RESOLUTION );
    temp_vs_freq_interp_val += static_pin_temp_i_freq_n;

    static_pin_vs_temp[i] = (int16)rf_limit_s32( temp_vs_freq_interp_val, -32768, 32767 );
  }

  /* Interpolate to obtain the static pin value at cal temp */
  center_point_pcomp_val = 
    rf_lininterp_u16_s16( therm_cal,
                          static_pin_vs_temp,
                          RFCOM_TXLIN_TEMP_BIN_SIZE,
                          ref_lin_temp_adc );

  /* Interpolate to obtain the static pin value at online temp */
  curr_temp_pcomp_val = 
    rf_lininterp_u16_s16( therm_cal,
                          static_pin_vs_temp,
                          RFCOM_TXLIN_TEMP_BIN_SIZE,
                          temp_read );

  /*--------------------------------------------------------------------------*/
  /* Interpolate vs freq for pin_cal value */
  pin_cal_vs_freq_comp_n = temp_comp_nv_data->pin_cal_vs_freq.pin[pa_state][freq_index];

  if ( freq_index < RFCOM_TXLIN_NUM_CHANNELS - 1 )
  {
    pin_cal_vs_freq_comp_n_plus_1 = temp_comp_nv_data->pin_cal_vs_freq.pin[pa_state][freq_index+1];
  }
  else /* We're out of bounds. Handle correctly */
  {
    pin_cal_vs_freq_comp_n_plus_1 = temp_comp_nv_data->pin_cal_vs_freq.pin[pa_state][freq_index];
  }

  pin_cal_interp_result = rf_divide_with_rounding( 
     ( (pin_cal_vs_freq_comp_n_plus_1 - pin_cal_vs_freq_comp_n)* freq_interp ), 
                             RFCOM_TXLIN_FREQ_BIN_RESOLUTION );

  pin_cal_interp_result += temp_comp_nv_data->pin_cal_vs_freq.pin[pa_state][freq_index];
  /*--------------------------------------------------------------------------*/

  if (temp_vs_freq_params->auto_pin_enable)
  {
    /*If auto pin is enabled, initial seeding value will only account for freq offset*/
    pcomp_val = pin_cal_interp_result;

    RF_MSG_2( RF_MED, "Pin_cal_offset %d, chan_idx %d", pcomp_val, freq_index);

  }
  else
  {

  /* Get the difference between online temp static pin and cal temp static pin,
  then apply the difference to cal'ed pin for the frequency */
  pcomp_val = curr_temp_pcomp_val - center_point_pcomp_val + pin_cal_interp_result;

  RF_MSG_7( RF_MED, "Pin comp %d for pin_cal_offset %d curr_adc %d, chan_idx %d"
                    ", curr_temp_comp %d, ref_temp_comp %d, ref_adc %d",
            pcomp_val, pin_cal_interp_result, temp_read, freq_index, 
            curr_temp_pcomp_val, center_point_pcomp_val, ref_lin_temp_adc );

  }
  return pcomp_val;

} /* rfcommon_core_txlin_get_lin_vs_temp_vs_freq_compensation_w_adc */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Obtain the temperature vs frequency compensation value based on the NV input 
  provided
 
  @details
  This API provides the interface to the RF driver modules for obtaining the 
  temperature compensation value from static NV data. The decision for which 
  algorithm must be used to compute the value is based on multilin NV version 
  and temp comp NV version. Based on the algorithm, this API populates both 
  Pin and Pout temp comp offsets as required. Current temperature and 
  frequency information is provided by the driver 

  @param temp_comp_algo
  Temperature compensation algorithm to be used

  @param pin_offset
  Populate computed pin compensation value here

  @param pout_offset
  Populate computed pout compensation value here

  @param temp_vs_freq_params
  Provide current temperature and frequency information

  @param temp_comp_nv_data
  All temp comp static NV data
*/
void
rfcommon_core_txlin_get_temp_compensation
(
  rfcom_txlin_temp_comp_algo_type temp_comp_algo,
  int16 *pin_offset,
  int16 *pout_offset,
  tx_lin_vs_temp_vs_freq_comp_param_type *temp_vs_freq_params,
  rfcom_txlin_temp_comp_nv_data_type *temp_comp_nv_data
)
{
  rfcom_txlin_temp_comp_algo_type check_temp_comp_algo;
  boolean allow_pin_cal_allowed;

  /* For Pin alignment during RF calibration, the algorithm is fixed 
  by the callee */
  if ( rfm_get_calibration_state() == FALSE )
  {
    /* Check whether the data provided to compute compensation is consistent 
    with the algorithm type provided */
    check_temp_comp_algo = rfcommon_core_txlin_get_temp_comp_algo(
               temp_vs_freq_params->tx_multilin_nv_cal_ptr->nv_rev_type,
               temp_comp_nv_data );
    if ( temp_comp_algo != check_temp_comp_algo )
    {
      /* Consistency check for temp comp algorithm has failed */
      RF_MSG_2( RF_ERROR, "rfcommon_core_txlin_get_temp_compensation: "
                          "Mismatch in temp comp algorithm. "
                          "Provided %d, but data indicates %d",
                temp_comp_algo, check_temp_comp_algo );
      *pin_offset = 0;
      *pout_offset = 0;
      return;
    }
    allow_pin_cal_allowed = TRUE;
  }
  else
  {
    allow_pin_cal_allowed = FALSE;
  }

  switch ( temp_comp_algo )
  {
     case RFCOM_TXLIN_TEMP_COMP_ALGO_MULTILIN_V1:
       /* Please refer to algorithm details. Pin is always zero. Pout 
       compensation is applied */
       *pin_offset = 0;
       *pout_offset = rfcommon_core_txlin_get_lin_vs_temp_compensation( 
                                      temp_vs_freq_params, temp_comp_nv_data );
       break;

     case RFCOM_TXLIN_TEMP_COMP_ALGO_MULTILIN_V3_LEGACY_NV:
       /* Please refer to algorithm details. Pout is always zero. Computed 
       compensation is applied to Pin */
       *pin_offset = rfcommon_core_txlin_get_lin_vs_temp_compensation( 
                                      temp_vs_freq_params, temp_comp_nv_data );
       *pout_offset = 0;
       break;

     case RFCOM_TXLIN_TEMP_COMP_ALGO_MULTILIN_V3:
       /* Please refer to algorithm details. Pin and Pout are computed based 
       on lin vs temp vs freq static NV data */
       if ( ( rfcommon_core_txlin_has_pin_cal_offset(
               temp_comp_nv_data, temp_vs_freq_params->pa_state ) == TRUE )
            && ( allow_pin_cal_allowed == TRUE ) )
       {
         *pin_offset = 
           rfcommon_core_txlin_get_pin_cal_vs_temp_vs_freq_compensation_w_adc(
                                   temp_vs_freq_params,
                                   temp_comp_nv_data,
                                   temp_comp_nv_data->lin_vs_temp_vs_freq.pin );
       }
       else
       {
         *pin_offset = 
           rfcommon_core_txlin_get_lin_vs_temp_vs_freq_compensation_w_adc(
                                   temp_vs_freq_params,
                                   temp_comp_nv_data,
                                   temp_comp_nv_data->lin_vs_temp_vs_freq.pin,
                                   RFCOM_TXLIN_NORMALIZE_TO_REF_FREQ_REF_TEMP );
       }

       /* Add ref channel room temp pin offset */
       *pin_offset += temp_vs_freq_params->ref_xpt_pin_offset;
       RF_MSG_1( RF_HIGH, "rfcommon_core_txlin_get_temp_compensation: "
                          "ref_xpt_pin_offset %d ",
                 temp_vs_freq_params->ref_xpt_pin_offset );

       *pout_offset = 
         rfcommon_core_txlin_get_lin_vs_temp_vs_freq_compensation_w_adc(
                                 temp_vs_freq_params,
                                 temp_comp_nv_data,
                                 temp_comp_nv_data->lin_vs_temp_vs_freq.pout,
                                 RFCOM_TXLIN_NORMALIZE_TO_CURR_FREQ_REF_TEMP );
       break;

     default:
       *pin_offset = 0;
       *pout_offset = 0;
       RF_MSG( RF_ERROR, "rfcommon_core_txlin_get_temp_compensation: "
                         "Invalid Temp Comp Algorithm Requested" );
       break;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Obtain the temperature and frequency compensation value based on the 2-D
  NV input provided
 
  @details
  This API uses the input Frequency and Temperature Compensation tables from 
  NV, as well as current frequency and temperature data to obtain a 2 
  dimensional compensation value in dB10 units. The compensation value is 
  then provided for Pout or Pin linearizer compensation by each tech.

  @param temp_vs_freq_params
  Provide current temperature and frequency information

  @return 
  Power compensation value in dB10 units 
*/
int16
rfcommon_core_txlin_get_lin_vs_temp_vs_freq_compensation
(
  tx_lin_vs_temp_vs_freq_comp_param_type *temp_vs_freq_params
)
{
  /* Input params */
  /* Current Tx frequency */
  uint32 tx_freq;
  /* Cal chan frequency list */
  uint32 *tx_cal_freq;
  /* Cal chan list size */
  uint8  tx_cal_chan_size;
  /* Current therm read */
  uint16 temp_read;
  /* Therm cal data */
  uint16 *therm_cal;
  /* Pin/Pout lin vs temp vs freq static NV data */
  int16  (*lin_vs_temp_vs_freq)[RFCOM_TXLIN_NUM_CHANNELS];
  /* Tx cal data linearizer */
  tx_multi_linearizer_index_node_type* tx_cal_data_index;
  /* Requested PA state */
  uint8 pa_state;
  /* Requested waveform type */
  tx_linearizer_waveform_type tx_waveform;

  int16  pcomp_val;
  uint8  i;
  int16  freq_comp_indicator, freq_interp, ref_freq_interp;
  uint8  freq_index, ref_freq_index;
  uint8  freq_iterator;
  int16  temp_vs_freq_comp[RFCOM_TXLIN_TEMP_BIN_SIZE];
  int16  temp_vs_freq_comp_n, temp_vs_freq_comp_n_plus_1;
  int32  result;
  int16  center_point_pcomp_val;
  int16  delta_from_ref;

  /* This array is used for finding the freq comp bin and interp factor*/
  const int16 freq_comp_indicator_array[RFCOM_TXLIN_NUM_CHANNELS] =
  {
    0*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,  1*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    2*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,  3*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    4*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,  5*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    6*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,  7*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    8*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,  9*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    10*RFCOM_TXLIN_FREQ_BIN_RESOLUTION, 11*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    12*RFCOM_TXLIN_FREQ_BIN_RESOLUTION, 13*RFCOM_TXLIN_FREQ_BIN_RESOLUTION,
    14*RFCOM_TXLIN_FREQ_BIN_RESOLUTION, 15*RFCOM_TXLIN_FREQ_BIN_RESOLUTION
  };

  /* Extract the compensation parameters */
  pa_state = temp_vs_freq_params->pa_state;
  tx_waveform = temp_vs_freq_params->freq_params.waveform_type;

  tx_freq = temp_vs_freq_params->freq_params.tx_freq;
  tx_cal_freq = (uint32*)
                  temp_vs_freq_params->tx_multilin_nv_cal_ptr->tx_cal_freq;
  tx_cal_chan_size = 
             temp_vs_freq_params->tx_multilin_nv_cal_ptr->tx_cal_chan_size;

  temp_read = temp_vs_freq_params->temp_params.temp_read;
  therm_cal = temp_vs_freq_params->temp_params.therm_cal;
  
  lin_vs_temp_vs_freq = temp_vs_freq_params->lin_vs_temp_vs_freq[pa_state];
  tx_cal_data_index = 
     (tx_multi_linearizer_index_node_type*)
       &temp_vs_freq_params->tx_multilin_nv_cal_ptr->
       tx_pa_state_cal_data[pa_state].linearizer_index[tx_waveform][0];

  /* Do error checking */
  if ( tx_cal_freq == NULL || therm_cal == NULL || 
       lin_vs_temp_vs_freq == NULL || tx_cal_data_index == NULL )
  {
    RF_MSG( RF_ERROR, 
            "rfcommon_core_txlin_get_lin_vs_temp_vs_freq_compensation: "
            "Null pointer input to compensation function" );
    return 0;
  }

  if ( tx_cal_chan_size > RFCOM_TXLIN_NUM_CHANNELS )
  {
    RF_MSG_2( RF_ERROR, 
              "rfcommon_core_txlin_get_lin_vs_temp_vs_freq_compensation: "
              "Cal Chan Size %d exceeds max supported num channels %d",
              tx_cal_chan_size, RFCOM_TXLIN_NUM_CHANNELS );
    return 0;
  }

  /* Loop through 16 channels to find the closest linearizer for the 
  given Tx frequency*/
  for ( freq_iterator = 0; 
        freq_iterator < RFCOM_TXLIN_NUM_CHANNELS; 
        freq_iterator++ )
  {
    if ( (tx_freq <= tx_cal_data_index[freq_iterator].upper_bound_freq) && 
         (tx_cal_data_index[freq_iterator].node_data.lin_data != NULL) )
    {
      /* match linearizer table found */
      break;
    }
  }

  if ( freq_iterator == RFCOM_TXLIN_NUM_CHANNELS )
  {
    RF_MSG_1( RF_MED, "rfcommon_core_txlin_get_lin_vs_temp_vs_freq_compensation: "
                      "Failed to find linearizer table for Tx frequency:%d!", 
              tx_freq );
    /* Use mid channel lut as default for now*/
    freq_iterator = RFCOM_TXLIN_DEFAULT_FREQ_INDEX;
    if (tx_cal_data_index[freq_iterator].node_data.lin_data == NULL)
    {
      /* Reference channel table pointer is NULL, something is seriously wrong */  
      RF_MSG( RF_ERROR, "rfcommon_core_txlin_get_lin_vs_temp_vs_freq_compensation: "
                        "Reference channel LUT not found!" );
      return 0;
    }
  }

  /* Obtain the reference pcomp value in NV, relative to which all other 
  compensation values are provided. This is at center frequency F7 and at 
  room temperature T4 */
  center_point_pcomp_val = lin_vs_temp_vs_freq[RFCOM_TXLIN_PWR_COMP_REF_TEMP_INDEX]
                                              [RFCOM_TXLIN_PWR_COMP_REF_FREQ_INDEX];

  /* ============================ Proceed with calculation ================= */
  /* Get freq_comp bin and multiplier */
  freq_comp_indicator = rf_lininterp_u32_s16( tx_cal_freq,
                                              freq_comp_indicator_array,
                                              tx_cal_chan_size,
                                              tx_freq );

  /* Calculate freq_comp bin index and interp factor */
  freq_interp = freq_comp_indicator % RFCOM_TXLIN_FREQ_BIN_RESOLUTION;
  freq_index  = (uint8)( (freq_comp_indicator - freq_interp)/
                                    RFCOM_TXLIN_FREQ_BIN_RESOLUTION );

  /* Get freq_comp bin for the reference linearizer channel and its 
  multiplier */
  freq_comp_indicator = 
       rf_lininterp_u32_s16( 
              tx_cal_freq,
              freq_comp_indicator_array,
              tx_cal_chan_size,
              tx_cal_data_index[freq_iterator].node_data.lin_data->chan_freq );

  /* Calculate freq_comp bin index and interp factor for ref channel 
  linearizer */
  ref_freq_interp = freq_comp_indicator % RFCOM_TXLIN_FREQ_BIN_RESOLUTION;
  ref_freq_index  = (uint8)( (freq_comp_indicator - ref_freq_interp)/
                                    RFCOM_TXLIN_FREQ_BIN_RESOLUTION );

  /* -------------- Compute delta from center freq room temp value --------- */

  /* For the reference frequency index, obtain the value at room temperature */
  /* Get the temp_comp arrays for the freq bins */
  temp_vs_freq_comp_n = 
       (int16)lin_vs_temp_vs_freq[RFCOM_TXLIN_PWR_COMP_REF_TEMP_INDEX]
                                 [ref_freq_index];

  /* Interpolate correctly if we're in the last but one frequency bin */
  if ( ref_freq_index < RFCOM_TXLIN_NUM_CHANNELS - 1 )
  {
    temp_vs_freq_comp_n_plus_1 = 
       (int16)lin_vs_temp_vs_freq[RFCOM_TXLIN_PWR_COMP_REF_TEMP_INDEX]
                                 [ref_freq_index+1];
  }
  /* We're out of bounds. Handle correctly */
  else
  {
    temp_vs_freq_comp_n_plus_1 = 
       (int16)lin_vs_temp_vs_freq[RFCOM_TXLIN_PWR_COMP_REF_TEMP_INDEX]
                                 [ref_freq_index];
  }

  /* Linear interpolation of two temp_comp arrays between two freq_bins */
  result = rf_divide_with_rounding( ( (temp_vs_freq_comp_n_plus_1 - 
                                                temp_vs_freq_comp_n)*
                                     ref_freq_interp ),
                                    RFCOM_TXLIN_FREQ_BIN_RESOLUTION );
  result += temp_vs_freq_comp_n;

  /* Having computed the Pin compensation value at the reference channel at 
  room temperature (REF_TEMP_INDEX), calculate the delta from the NV 
  reference Pcomp value */
  delta_from_ref = center_point_pcomp_val - result;

  /* ------------ Interpolate at current frequency across all temps -------- */
  /* calculate array of deltas of comp_vs_freq_vs_temp with interp */
  for( i = 0; i < RFCOM_TXLIN_TEMP_BIN_SIZE; i++ )
  {
    /* Get the temp_comp arrays for the freq bins */
    temp_vs_freq_comp_n = (int16)lin_vs_temp_vs_freq[i][freq_index];

    /* Interpolate correctly if we're in the last but one frequency bin */
    if ( freq_index < RFCOM_TXLIN_NUM_CHANNELS - 1 )
    {
      temp_vs_freq_comp_n_plus_1 = 
                          (int16)lin_vs_temp_vs_freq[i][freq_index+1];
    }
    /* We're out of bounds. Handle correctly */
    else
    {
      temp_vs_freq_comp_n_plus_1 = (int16)lin_vs_temp_vs_freq[i][freq_index];
    }

    /* Linear interpolation of two temp_comp arrays between two freq_bins */
    result = rf_divide_with_rounding( ( (temp_vs_freq_comp_n_plus_1 - 
                                                  temp_vs_freq_comp_n)*
                                       freq_interp ),
                                      RFCOM_TXLIN_FREQ_BIN_RESOLUTION );
    result += temp_vs_freq_comp_n;
    temp_vs_freq_comp[i] = (int16)rf_limit_s32( result, -32768, 32767 );

    /* Add the delta from center frequency room temperature */
    temp_vs_freq_comp[i] += delta_from_ref;
  }

  pcomp_val = 
    rf_lininterp_u16_s16( therm_cal,
                          temp_vs_freq_comp,
                          RFCOM_TXLIN_TEMP_BIN_SIZE,
                          temp_read );

  RF_MSG_1( RF_LOW, "rfcommon_core_txlin_get_lin_vs_temp_vs_freq_compensation: "
                    "Calculated linearizer compensation is %d dB10",
            pcomp_val );

  return pcomp_val;

} /* rfcommon_core_txlin_get_lin_vs_temp_vs_freq_compensation */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Performs linearizer Pin compensation

  @details
  The Pin compensation algorithm adjusts the Tx linearizer based on a dB10 
  compensation value. The linearizer is compensated to adjust the input power 
  to the PA, by recalculating the IQ gain and RGI combination based on 
  full-bias RGI delta sweep information. One of the advantages of this method 
  of compensation is that it allows for the output power and DPD relationship, 
  if any, to be maintained.

  @param input_tx_lut
  Input Tx linearizer which must be compensated

  @param output_tx_lut
  Output Tx linearizer, with RGI and IQ gain compensated

  @param delta_rgi_lut
  Linearizer with RGI delta information, usually performed at full PA bias

  @param pcomp_val
  Linearizer Pin compensation value in dB10 units

  @param valid_lut_entries
  Number of entries valid in the input Tx linearizer

  @param aux_lin_data
  DPD information for the current linearizer being compensated

  @return 
  TRUE if the algorithm proceeded successfully, else FALSE

*/
boolean
rfcommon_core_txlin_pin_comp
(
  tx_linearizer_table_type *input_tx_lut,
  tx_linearizer_table_type *output_tx_lut,
  tx_linearizer_table_type *delta_rgi_lut,
  int16 pcomp_val,
  uint8 valid_lut_entries,
  tx_linearizer_aux_data_type aux_lin_data
)
{
  uint8  lut_index, iq_local, valid_first_lut_entries;
  uint8  fs_lut_index, cnt, lut_beg;
  int16  new_power = 0;
  int16  pin_shortage = 0;
  uint16 transfer_factor = 0;

  /* Check for NULL pointers */
  if ( delta_rgi_lut == NULL || input_tx_lut == NULL || output_tx_lut == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_core_txlin_pin_comp: "
                      "Input LUTs are NULL. Cannot proceed");   
    return FALSE;
  }
  
  /* In case of RGI run-out, we need to transfer Pin shortage. Determine the 
  conversion factor based on DPD type for the linearizer */
  switch ( aux_lin_data )
  {
     case TX_LINEARIZER_NO_AUX_DATA:
       transfer_factor = RFCOM_TXLIN_PIN_POUT_DELTA_CONVERSION_ALPHA_APT;
       break;

     case TX_LINEARIZER_EPT_DPD_V2_DATA:
       transfer_factor = RFCOM_TXLIN_PIN_POUT_DELTA_CONVERSION_ALPHA_EPT;
       break;

     case TX_LINEARIZER_ET_DPD_DATA:
       transfer_factor = RFCOM_TXLIN_PIN_POUT_DELTA_CONVERSION_ALPHA_ET;
       break;

     default:
       transfer_factor = RFCOM_TXLIN_PIN_POUT_DELTA_CONVERSION_ALPHA_APT;
       break;
  }

  /* Find the number of valid entries in first sweep */
  for ( cnt = 0; cnt< RFCOM_TXLIN_NUM_SEGMENTS; cnt++ )
  {  
    if(delta_rgi_lut->table[cnt].rgi == 0xFFFF)
    {
      break;
    }
  }
  valid_first_lut_entries = cnt;

  if (valid_first_lut_entries == 0)
  {
    RF_MSG( RF_ERROR, "rfcommon_core_txlin_pin_comp: "
                      "No valid RGI delta sweep linearizer entries");
    return FALSE;
  }

  for ( lut_index = 0; lut_index< valid_lut_entries; lut_index++ )
  {
    /* check Invalid entry in the LUT */
    if ( input_tx_lut->table[lut_index].rgi != 0xFFFF )
    {
      fs_lut_index = lut_index;

      /* TODO: now that we stopped messing with first sweep data in process linearizer,
         clean up this code to search from 0 and stop at 0xFF */    
      for( cnt = 0; cnt < valid_first_lut_entries; cnt++ )
      {
        /* Find FS index */
        if( delta_rgi_lut->table[cnt].rgi == input_tx_lut->table[lut_index].rgi )
        {
          fs_lut_index = cnt;
          break;
        }
      } /* for( cnt ) */

      if( cnt == valid_first_lut_entries )
      {
        RF_MSG_1( RF_ERROR, "rfcommon_core_txlin_pin_comp: "
                            "Could not find RGI %d in first sweep. "
                            "Pin compensation failed",
                  input_tx_lut->table[lut_index].rgi );
        return FALSE;
      }

      /* --------------------------------- Invariants ---------------------- */
      /* Copy over DPD index */
      output_tx_lut->table[lut_index].dpd_index = 
                                      input_tx_lut->table[lut_index].dpd_index;
      /* Copy over fcomp index */
      output_tx_lut->table[lut_index].fcomp_idx = 
                                      input_tx_lut->table[lut_index].fcomp_idx;
      /* Copy the power with no changes. Only RGI will change */
      output_tx_lut->table[lut_index].power = 
                                      input_tx_lut->table[lut_index].power;
      /* Copy the APT bias */
      output_tx_lut->table[lut_index].apt = input_tx_lut->table[lut_index].apt;

      /* PinComp: RSB and LOFT corrections are not tied to the temperature */
      output_tx_lut->table[lut_index].rsb_correction_coeff.alpha = 
        input_tx_lut->table[lut_index].rsb_correction_coeff.alpha ;
      output_tx_lut->table[lut_index].rsb_correction_coeff.beta = 
        input_tx_lut->table[lut_index].rsb_correction_coeff.beta ;
      output_tx_lut->table[lut_index].loft_correction = 
        input_tx_lut->table[lut_index].loft_correction ;
      /* --------------------------------- Variants ------------------------ */
      /* calculating the new power with temp comp val */
      new_power = delta_rgi_lut->table[fs_lut_index].power + (-1)*pcomp_val;

      /* Only RGI will be replaced in the new second sweep LUT */
      if( new_power >= delta_rgi_lut->table[valid_first_lut_entries-1].power )
      {
        /* If we run out of RGIs to compensate for Pin, we must ensure that the 
        RGI is capped at max, but the Pout in the linearizer is correctly updated 
        to reflect the shortage of Pin for this linearizer row */
        pin_shortage = delta_rgi_lut->table[valid_first_lut_entries-1].power -
                                                            new_power;
        RF_MSG_2( RF_MED, "rfcommon_core_txlin_pin_comp: Run out of RGIs for "
                          "Pin, %d(dB10) shortage, transfer factor %d",
                  pin_shortage, transfer_factor );

        /* Convert pin shortage to pout delta using alpha */
        pin_shortage = rf_divide_with_rounding( 
                          (pin_shortage * transfer_factor), 100 );
        /* Update the pout in the table to reflect correct estimated Pout */
        output_tx_lut->table[lut_index].power += pin_shortage;

        /* RGI should be coming from first sweep */
        output_tx_lut->table[lut_index].rgi = 
                          delta_rgi_lut->table[valid_first_lut_entries-1].rgi;
        /* Set IQ offset to zero, no backoff required */
        output_tx_lut->table[lut_index].iq_offset = 0;

        RF_MSG_3( RF_MED, "rfcommon_core_txlin_pin_comp: "
                          "Corrected shortage %d(dB10), "
                          "New Pout %d(dBm10) for RGI %d",
                  pin_shortage, output_tx_lut->table[lut_index].power,
                  output_tx_lut->table[lut_index].rgi );

      } /* if(new_power >=  power) */
      else if ( new_power <= delta_rgi_lut->table[0].power )
      {
        /* Bias chosen from second sweep */
        output_tx_lut->table[lut_index].apt = input_tx_lut->table[0].apt;
        /* RGI from first sweep */
        output_tx_lut->table[lut_index].rgi = delta_rgi_lut->table[0].rgi;
        /* Calc IQ based on diff */
        iq_local = delta_rgi_lut->table[0].power - new_power;
        iq_local = (iq_local > RFCOM_TXLIN_PIN_COMP_IQ_GAIN_MAX_ADJUST) ? 
                          RFCOM_TXLIN_PIN_COMP_IQ_GAIN_MAX_ADJUST : iq_local;
        output_tx_lut->table[lut_index].iq_offset = iq_local;

      }/* if(new_power <=  power) */
      else
      {
        /* Linear search implementation
           Start from fs_lut_index instead of lut_index as we need to 
           start from matching RGI. From here, we can go up or down based on 
           compensation val */
        lut_beg = fs_lut_index;
        while( (lut_beg <= (valid_first_lut_entries-1)) && 
               (lut_beg < (RFCOM_TXLIN_NUM_SEGMENTS-1)) &&
               (pcomp_val != 0) )
        {
          if( ( lut_beg != 0 ) && 
              ( delta_rgi_lut->table[lut_beg].power >= new_power ) &&
              ( delta_rgi_lut->table[lut_beg-1].power < new_power ) )
          {
            break;
          }
          else
          {
            /* if temp comp is negative, search for lower indices,
               if temp comp is positive , search for higher indices */
            (pcomp_val < 0) ? (lut_beg++) : (lut_beg--);
          }
        } /* while() */

        /* Here we find the exact RGI for the new power */
        output_tx_lut->table[lut_index].rgi = 
                                          delta_rgi_lut->table[lut_beg].rgi;
        /* storing the difference as IQ offset */
        iq_local = ( delta_rgi_lut->table[lut_beg].power - new_power );
        iq_local = ( iq_local > RFCOM_TXLIN_PIN_COMP_IQ_GAIN_MAX_ADJUST ) ? 
                           RFCOM_TXLIN_PIN_COMP_IQ_GAIN_MAX_ADJUST : iq_local;
        output_tx_lut->table[lut_index].iq_offset = iq_local;
      } /* else */

    } /* rgi != 0xFFFF */
    else /* rgi == 0xFFFF */
    {
      /* For invalid RGIs, copy the second sweep LUT entries to 
      the compensated LUT */
      output_tx_lut->table[lut_index].power= 
                                          input_tx_lut->table[lut_index].power;
      output_tx_lut->table[lut_index].rgi = input_tx_lut->table[lut_index].rgi;
      output_tx_lut->table[lut_index].iq_offset = 
                                      input_tx_lut->table[lut_index].iq_offset;
      output_tx_lut->table[lut_index].dpd_index = 
                                      input_tx_lut->table[lut_index].dpd_index;
      output_tx_lut->table[lut_index].fcomp_idx = 
                                      input_tx_lut->table[lut_index].fcomp_idx;
      output_tx_lut->table[lut_index].apt = input_tx_lut->table[lut_index].apt;
      output_tx_lut->table[lut_index].rsb_correction_coeff.alpha = 
        input_tx_lut->table[lut_index].rsb_correction_coeff.alpha ;
      output_tx_lut->table[lut_index].rsb_correction_coeff.beta =
        input_tx_lut->table[lut_index].rsb_correction_coeff.beta ;
      output_tx_lut->table[lut_index].loft_correction = 
        input_tx_lut->table[lut_index].loft_correction ;

    } /* rgi == 0xFFFF */

  } /* for(lut_index ) */

  /* Pad the remaining table */
  for ( ; lut_index < RFCOM_TXLIN_NUM_SEGMENTS; lut_index++ )
  {
    output_tx_lut->table[lut_index].rgi = 
                               output_tx_lut->table[valid_lut_entries-1].rgi;
    output_tx_lut->table[lut_index].power = 
                               output_tx_lut->table[valid_lut_entries-1].power;
    output_tx_lut->table[lut_index].apt = 
                               output_tx_lut->table[valid_lut_entries-1].apt;
    output_tx_lut->table[lut_index].iq_offset = 
                           output_tx_lut->table[valid_lut_entries-1].iq_offset;
    output_tx_lut->table[lut_index].dpd_index = 
                           output_tx_lut->table[valid_lut_entries-1].dpd_index;
    output_tx_lut->table[lut_index].fcomp_idx = 
                           output_tx_lut->table[valid_lut_entries-1].fcomp_idx;
    output_tx_lut->table[lut_index].rsb_correction_coeff.alpha = 
        output_tx_lut->table[valid_lut_entries-1].rsb_correction_coeff.alpha ;
    output_tx_lut->table[lut_index].rsb_correction_coeff.beta = 
        output_tx_lut->table[valid_lut_entries-1].rsb_correction_coeff.beta ;
    output_tx_lut->table[lut_index].loft_correction = 
                    output_tx_lut->table[valid_lut_entries-1].loft_correction ;
  }

  /* Copy items from the original table */
  output_tx_lut->chan_freq = input_tx_lut->chan_freq;
  output_tx_lut->num_fcomp = input_tx_lut->num_fcomp;
  output_tx_lut->fcomp_type = input_tx_lut->fcomp_type;
  output_tx_lut->dpd_type  = input_tx_lut->dpd_type;
  output_tx_lut->num_dpd   = input_tx_lut->num_dpd;
 
  return TRUE; 
} /* rfcommon_core_txlin_pin_comp */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Performs linearizer Pout compensation

  @details
  The Pout compensation algorithm adjusts the Tx linearizer based on a dB10 
  compensation value. The linearizer is compensated to adjust the ouput 
  power of the PA, by recalculating the Tx power entries in the linearizer.

  @param input_tx_lut
  Input Tx linearizer which must be compensated

  @param output_tx_lut
  Output Tx linearizer, with Tx power compensated

  @param pcomp_val
  Linearizer Pout compensation value in dB10 units

  @param valid_lut_entries
  Number of entries valid in the input Tx linearizer

  @return 
  TRUE if the algorithm proceeded successfully, else FALSE

*/
boolean 
rfcommon_core_txlin_pout_comp
(
  tx_linearizer_table_type *input_tx_lut,
  tx_linearizer_table_type *output_tx_lut,
  int16 pcomp_val,
  uint8 valid_lut_entries
)
{
  uint8 lut_index = 0;
  int16 new_power = 0;

  if ((input_tx_lut == NULL ) || (output_tx_lut == NULL ))
  {
    RF_MSG( RF_ERROR, " rfcommon_core_txlin_pout_comp: "
                      "NULL pointer for LUT!"); 
    return FALSE;
  }

  /* loop through the LUT to apply pout comp for each entry*/
  for ( lut_index = 0; lut_index < valid_lut_entries; lut_index++ )
  {
    /* check Invalid entry in the LUT */
    if ( input_tx_lut->table[lut_index].rgi != 0xFFFF )
    {
      /* Apply pout comp val to offset the power */
      new_power = input_tx_lut->table[lut_index].power + pcomp_val;
      output_tx_lut->table[lut_index].power = new_power;

      output_tx_lut->table[lut_index].rgi = input_tx_lut->table[lut_index].rgi;
      output_tx_lut->table[lut_index].iq_offset = 
                                 input_tx_lut->table[lut_index].iq_offset; 
      output_tx_lut->table[lut_index].dpd_index = 
                                  input_tx_lut->table[lut_index].dpd_index;
      output_tx_lut->table[lut_index].fcomp_idx = 
                                  input_tx_lut->table[lut_index].fcomp_idx;
      output_tx_lut->table[lut_index].apt = input_tx_lut->table[lut_index].apt;

      /* PoutComp: Copy the RSB and LOFT corrections */
      output_tx_lut->table[lut_index].rsb_correction_coeff.alpha = 
        input_tx_lut->table[lut_index].rsb_correction_coeff.alpha ;
      output_tx_lut->table[lut_index].rsb_correction_coeff.beta = 
        input_tx_lut->table[lut_index].rsb_correction_coeff.beta ;
      output_tx_lut->table[lut_index].loft_correction = 
        input_tx_lut->table[lut_index].loft_correction ;
    }
    else
    {
      /* For invalid RGI values, copy over the original LUT values to the
      output LUT */
      output_tx_lut->table[lut_index].rgi = input_tx_lut->table[lut_index].rgi;
      output_tx_lut->table[lut_index].power = 
                                          input_tx_lut->table[lut_index].power;
      output_tx_lut->table[lut_index].apt = input_tx_lut->table[lut_index].apt;
      output_tx_lut->table[lut_index].iq_offset = 
                                 input_tx_lut->table[lut_index].iq_offset; 
      output_tx_lut->table[lut_index].dpd_index = 
                                  input_tx_lut->table[lut_index].dpd_index;
      output_tx_lut->table[lut_index].fcomp_idx = 
                                  input_tx_lut->table[lut_index].fcomp_idx;
      output_tx_lut->table[lut_index].rsb_correction_coeff.alpha = 
        input_tx_lut->table[lut_index].rsb_correction_coeff.alpha ;
      output_tx_lut->table[lut_index].rsb_correction_coeff.beta = 
        input_tx_lut->table[lut_index].rsb_correction_coeff.beta ;
      output_tx_lut->table[lut_index].loft_correction = 
        input_tx_lut->table[lut_index].loft_correction ;
    }
  }
  /* Pad the remaining table */
  for ( ; lut_index < RFCOM_TXLIN_NUM_SEGMENTS; lut_index++ )
  {
    output_tx_lut->table[lut_index].rgi = 
                               output_tx_lut->table[valid_lut_entries-1].rgi;
    output_tx_lut->table[lut_index].power = 
                               output_tx_lut->table[valid_lut_entries-1].power;
    output_tx_lut->table[lut_index].apt = 
                               output_tx_lut->table[valid_lut_entries-1].apt;
    output_tx_lut->table[lut_index].iq_offset = 
                           output_tx_lut->table[valid_lut_entries-1].iq_offset;
    output_tx_lut->table[lut_index].dpd_index = 
                           output_tx_lut->table[valid_lut_entries-1].dpd_index;
    output_tx_lut->table[lut_index].fcomp_idx = 
                           output_tx_lut->table[valid_lut_entries-1].fcomp_idx;
    output_tx_lut->table[lut_index].rsb_correction_coeff.alpha = 
      output_tx_lut->table[valid_lut_entries-1].rsb_correction_coeff.alpha ;
    output_tx_lut->table[lut_index].rsb_correction_coeff.alpha = 
      output_tx_lut->table[valid_lut_entries-1].rsb_correction_coeff.beta ;
    output_tx_lut->table[lut_index].loft_correction = 
      output_tx_lut->table[valid_lut_entries-1].loft_correction ;
  }

  /* End updating the output LUT table 
     Copy the channel frequency from the original table */
  output_tx_lut->chan_freq = input_tx_lut->chan_freq;
  output_tx_lut->dpd_type  = input_tx_lut->dpd_type;
  output_tx_lut->num_dpd   = input_tx_lut->num_dpd;
  output_tx_lut->num_fcomp = input_tx_lut->num_fcomp;
  output_tx_lut->fcomp_type = input_tx_lut->fcomp_type;
    
  return TRUE;
} /* rfcommon_core_txlin_pout_comp */

/*----------------------------------------------------------------------------*/

/*!
  @brief  
  Populate Tx linearizer table with temp comp updates
 
  @details
  This is a common function for all technologies that populate the Tx LUT with temp comp
  updates. For each LUT entry, it will apply the temp comp power updates first, then search
  the originial LUT to find the corresponding smps Bias
 
  @param tx_lut
  Original Tx linearizer table

  @param temp_comp_lut
  Updated LUT table with temp comp

  @param temp_comp_val
  Temp comp offset in dB10 unit.
 
  @internal
  LUT table needs to be sorted by power in increasing order
*/
boolean
rfcommon_core_txlin_temp_vs_freq_comp_update
(
  tx_multi_lin_rev_type lin_rev_used,
  tx_linearizer_table_data_type *input_tx_lut_data,
  tx_linearizer_table_data_type *output_tx_lut_data,
  tx_linearizer_table_data_type *txlin_first_sweep_tbl,
  int16 pin_comp_val,
  int16 pout_comp_val,
  uint8 valid_lut_entries,
  boolean skip_pout_comp
)
{
  uint8 lut_index = 0;
  tx_linearizer_table_type *input_tx_lut = 
                                  input_tx_lut_data->lin_data;
  tx_linearizer_table_type *output_tx_lut = 
                                  output_tx_lut_data->lin_data;
  tx_linearizer_table_type *first_sweep_lut = 
                                  txlin_first_sweep_tbl->lin_data;
  boolean status = TRUE;
  
  /* Maintain DPD data */
  output_tx_lut_data->aux_lin_data = input_tx_lut_data->aux_lin_data;
  output_tx_lut_data->lin_data->num_dpd = input_tx_lut_data->lin_data->num_dpd;
  output_tx_lut_data->lin_data->num_fcomp = input_tx_lut_data->lin_data->num_fcomp;
  output_tx_lut_data->lin_data->fcomp_type = input_tx_lut_data->lin_data->fcomp_type;
  output_tx_lut_data->xpt_data = input_tx_lut_data->xpt_data;

  /* Check for NULL pointers */
  if ((input_tx_lut == NULL )||(output_tx_lut == NULL))
  {
    RF_MSG( RF_ERROR, "rfcommon_core_txlin_temp_vs_freq_comp_update: "
                      "NULL pointer in input LUT!");    
    return FALSE;
  }

  /* Check if the input LUT is sorted by power in increasing order */
  for ( lut_index = 0; lut_index < valid_lut_entries - 1; lut_index++ )
  {
    if ( input_tx_lut->table[lut_index].power > 
         input_tx_lut->table[lut_index+1].power )
    {
      RF_MSG( RF_ERROR, "rfcommon_core_txlin_temp_vs_freq_comp_update: "
                        "Input LUT is not sorted by power!");
      return FALSE;
    }
  }

  if ( lin_rev_used == TX_MULTI_LIN_REV_1 )
  {
    /* Legacy temp comp NVs are now populated into pin compensation 
    data structures. Hence provide pin comp val to the legacy APT 
    algorithm */
    status &= rfcommon_core_txlin_temp_comp_update_apt( input_tx_lut,
                                                       output_tx_lut,
                                                       pin_comp_val,
                                                       valid_lut_entries );
  }
  else if ( lin_rev_used == TX_MULTI_LIN_REV_3 )
  {
         tx_linearizer_table_type* interim_tx_lut = NULL ;
         tx_linearizer_table_type* temp_tx_lut = NULL ;
         /* If XPT is enabled, we apply linearizer vs temp vs freq Pin and 
         Pout compensation to the input linearizer. The input linearizer MUST be 
         frequency compensated already, based on calibrated lin vs freq data */

         /* For both cases above, apply both Pin and Pout as provided by 
         rfcommon_core_txlin_get_temp_compensation() */

         if ( skip_pout_comp == FALSE )
         {
         /* Allocate memory for interim linearizer */
         interim_tx_lut = (tx_linearizer_table_type *)modem_mem_alloc( 
                                              sizeof(tx_linearizer_table_type),
                                              MODEM_MEM_CLIENT_RFA ) ;

           if ( NULL == interim_tx_lut )
           {
             RF_MSG( RF_ERROR , "rfcommon_core_txlin_temp_vs_freq_comp_update: "
                             "Failed to allocate interim_tx_lut memory" );
             /* Get out of the function with error */
             status = FALSE ;
			 
			  size_t data_length = sizeof(tx_linearizer_table_type);
    memscpy( output_tx_lut , data_length ,
             input_tx_lut , data_length );

    RF_MSG_3( RF_ERROR, "rfcommon_core_txlin_temp_vs_freq_comp_update: "
                         "Temp Comp failed for Algorithm %d,"
                         "Pin %d Pout %d", 
              lin_rev_used, pin_comp_val, pout_comp_val );
	          return status;
			 
           }

           else
	   {
             memset( interim_tx_lut, 0, sizeof(tx_linearizer_table_type) );
             /* First apply Pout compensation */
             status &= rfcommon_core_txlin_pout_comp( input_tx_lut, 
                                                   interim_tx_lut,
                                                   pout_comp_val, 
                                                   valid_lut_entries ) ;
             temp_tx_lut = interim_tx_lut ;
	    }
         }
         else
         {
           temp_tx_lut = input_tx_lut ;
         }
       
        
    /* The output from Lin vs Temp vs Freq Pout compensation is provided as input
    to the Pin compensation algorithm */
      status &= rfcommon_core_txlin_pin_comp(temp_tx_lut,
                                            output_tx_lut,
                                            first_sweep_lut,
                                            pin_comp_val,
                                            valid_lut_entries,
                                            input_tx_lut_data->aux_lin_data );

	 
      RF_MSG_3( RF_LOW, "rfcommon_core_txlin_temp_vs_freq_comp_update: "
                        "Used Temp Comp Algorithm %d, Pin %d Pout %d",
                        lin_rev_used, pin_comp_val, pout_comp_val );
      if ( skip_pout_comp == FALSE )
      {
        modem_mem_free( interim_tx_lut, MODEM_MEM_CLIENT_RFA );
      }
       
  }

  /* If the algorithms failed, copy over the input linearizer to the output 
  linearizer and return TRUE with an error message */
  if ( status == FALSE )
  {
    size_t data_length = sizeof(tx_linearizer_table_type);
    memscpy( output_tx_lut , data_length ,
             input_tx_lut , data_length );

    RF_MSG_3( RF_ERROR, "rfcommon_core_txlin_temp_vs_freq_comp_update: "
                         "Temp Comp failed for Algorithm %d,"
                         "Pin %d Pout %d", 
              lin_rev_used, pin_comp_val, pout_comp_val );
  }

  return status;
} /* rfcommon_core_txlin_temp_vs_freq_comp_update */

/*----------------------------------------------------------------------------*/

/*!
  @brief  
  Populate Tx linearizer table with temp comp updates
 
  @details
  This is a common function for all technologies that populates the Tx LUT 
  with temp comp updates. Please refer to 
  rfcommon_core_txlin_get_temp_comp_algo() for details regarding the temp 
  comp algorithm to be used. Based on the NV versioning, we apply legacy 
  temperature compensation, or new Pin/Pout compensation vs temp and freq.
 
  @param temp_comp_algo
  Temperature compensation algorithm to be used

  @param input_tx_lut_data
  Input linearizer information

  @param output_tx_lut_data
  Output linearizer to be populated with compensated information

  @param txlin_first_sweep_tbl
  First linearizer information

  @param pin_comp_val
  Pin compensation value in dB10, used only if MultiLin V3 or above is 
  supported

  @param pout_comp_val
  Pout compensation value in dB10

  @param valid_lut_entries
  Number of valid LUT entries
 
  @internal
  LUT table needs to be sorted by power in increasing order
*/
boolean
rfcommon_core_txlin_temp_vs_freq_comp_update_v2
(
  rfcom_txlin_temp_comp_algo_type temp_comp_algo,
  tx_linearizer_table_data_type *input_tx_lut_data,
  tx_linearizer_table_data_type *output_tx_lut_data,
  tx_linearizer_table_data_type *txlin_first_sweep_tbl,
  int16 pin_comp_val,
  int16 pout_comp_val,
  uint8 valid_lut_entries ,
  boolean skip_pout_comp 
)
{
  uint8 lut_index = 0;
  tx_linearizer_table_type *input_tx_lut = 
                                  input_tx_lut_data->lin_data;
  tx_linearizer_table_type *output_tx_lut = 
                                  output_tx_lut_data->lin_data;
  tx_linearizer_table_type *first_sweep_lut = 
                                  txlin_first_sweep_tbl->lin_data;
  tx_linearizer_table_type *interim_tx_lut = NULL;
  boolean status = TRUE;
  
  /* Maintain linearizer meta data */
  output_tx_lut_data->aux_lin_data = input_tx_lut_data->aux_lin_data;
  output_tx_lut_data->xpt_data = input_tx_lut_data->xpt_data;

  output_tx_lut_data->lin_data->num_dpd = input_tx_lut_data->lin_data->num_dpd;
  output_tx_lut_data->lin_data->num_fcomp = 
                                   input_tx_lut_data->lin_data->num_fcomp;
  output_tx_lut_data->lin_data->fcomp_type = 
                                   input_tx_lut_data->lin_data->fcomp_type;
  output_tx_lut_data->lin_data->chan_freq = 
                                   input_tx_lut_data->lin_data->chan_freq;
  

  /* Check for NULL pointers */
  if ((input_tx_lut == NULL )||(output_tx_lut == NULL))
  {
    RF_MSG( RF_ERROR, "rfcommon_core_txlin_temp_vs_freq_comp_update_v2: "
                      "NULL pointer in input LUT!");    
    return FALSE;
  }

  /* Check if the input LUT is sorted by power in increasing order */
  for ( lut_index = 0; lut_index < valid_lut_entries - 1; lut_index++ )
  {
    if ( input_tx_lut->table[lut_index].power > 
         input_tx_lut->table[lut_index+1].power )
    {
      RF_MSG( RF_ERROR, "rfcommon_core_txlin_temp_vs_freq_comp_update_v2: "
                        "Input LUT is not sorted by power!");
      status = FALSE;
    }
  }

  if ( status == TRUE )
  {
    switch ( temp_comp_algo )
    {
       case RFCOM_TXLIN_TEMP_COMP_ALGO_MULTILIN_V1:
         /* Legacy temp comp NVs are now populated into pin compensation 
         data structures. Hence provide pin comp val to the legacy APT 
         algorithm */
         status = rfcommon_core_txlin_temp_comp_update_apt( input_tx_lut,
                                                            output_tx_lut,
                                                            pout_comp_val,
                                                            valid_lut_entries );
         RF_MSG_2( RF_MED, "rfcommon_core_txlin_temp_vs_freq_comp_update_v2: "
                           "Used Temp Comp Algorithm %d, Pout %d",
                 temp_comp_algo, pout_comp_val );
         break;

       case RFCOM_TXLIN_TEMP_COMP_ALGO_MULTILIN_V3_LEGACY_NV:
         /* For Multilin V3 with legacy NV support, we apply the legacy 
         temp comp NV values to Pin. This is handled by the get temp comp 
         functionality. See rfcommon_core_txlin_get_temp_compensation() */
       case RFCOM_TXLIN_TEMP_COMP_ALGO_MULTILIN_V3:
       {  
         tx_linearizer_table_type* temp_tx_lut = NULL ;
         /* If XPT is enabled, we apply linearizer vs temp vs freq Pin and 
         Pout compensation to the input linearizer. The input linearizer MUST be 
         frequency compensated already, based on calibrated lin vs freq data */

         /* For both cases above, apply both Pin and Pout as provided by 
         rfcommon_core_txlin_get_temp_compensation() */

         if ( skip_pout_comp == FALSE )
         {
         /* Allocate memory for interim linearizer */
         interim_tx_lut = (tx_linearizer_table_type *)modem_mem_alloc( 
                                              sizeof(tx_linearizer_table_type),
                                              MODEM_MEM_CLIENT_RFA );
         if ( interim_tx_lut == NULL )
         {
           RF_MSG( RF_ERROR, "rfcommon_core_txlin_temp_vs_freq_comp_update_v2: "
                             "Failed to allocate interim lin memory" );
           status = FALSE;
           break;
         }
       
           memset( interim_tx_lut, 0, sizeof(tx_linearizer_table_type) );

           /* First apply Pout compensation */
           status = rfcommon_core_txlin_pout_comp( input_tx_lut, 
                                                   interim_tx_lut,
                                                   pout_comp_val, 
                                                   valid_lut_entries );
            temp_tx_lut = interim_tx_lut ;
         }
         else
         {
           temp_tx_lut = input_tx_lut ;
         }
         if ( temp_tx_lut == NULL )
         {
           RF_MSG( RF_ERROR, "rfcommon_core_txlin_temp_vs_freq_comp_update: "
                             "Failed to allocate interim lin memory" );
           status = FALSE;
         }
         else
         {
           /* The output from Lin vs Temp vs Freq Pout compensation is provided as input
           to the Pin compensation algorithm */
           status &= rfcommon_core_txlin_pin_comp( temp_tx_lut,
                                                   output_tx_lut,
                                                   first_sweep_lut,
                                                   pin_comp_val,
                                                   valid_lut_entries,
                                                   input_tx_lut_data->aux_lin_data );

           RF_MSG_3( RF_MED, "rfcommon_core_txlin_temp_vs_freq_comp_update_v2: "
                             "Used Temp Comp Algorithm %d, Pin %d Pout %d",
                   temp_comp_algo, pin_comp_val, pout_comp_val );

           if ( NULL != interim_tx_lut )
           {
           modem_mem_free( interim_tx_lut, MODEM_MEM_CLIENT_RFA );
           }
         }

         break;
      }

       default:
         RF_MSG_1( RF_ERROR, "rfcommon_core_txlin_temp_vs_freq_comp_update_v2: "
                             "Unsupported Temp Comp Algorithm %d",
                 temp_comp_algo );
         status = FALSE;
         break;
    }
  }

  /* If the algorithms failed, copy over the input linearizer to the output 
  linearizer and return TRUE with an error message */
  if ( status == FALSE )
  {
    size_t data_length = sizeof(tx_linearizer_table_type);
    memscpy( output_tx_lut , data_length ,
             input_tx_lut , data_length );

    RF_MSG_3( RF_ERROR, "rfcommon_core_txlin_temp_vs_freq_comp_update_v2: "
                         "Temp Comp failed for Algorithm %d,"
                         "Pin %d Pout %d", 
              temp_comp_algo, pin_comp_val, pout_comp_val );
  }

  return TRUE;
} /* rfcommon_core_txlin_temp_vs_freq_comp_update_v2 */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Performs temp-comp on APT data
 
  @param tx_lut
  Original Tx linearizer table
 
  @param temp_comp_lut
  Compensated LUT

  @param first_sweep_lut
  Full-bias LUT
 
  @param temp_comp_val
  Compensation value
 
  @param valid_lut_entries
  The number of valid LUT entries

  @return
  TRUE if freq comp was successful, else FALSE
*/
boolean 
rfcommon_core_txlin_temp_comp_update_apt
(
  tx_linearizer_table_type *tx_lut,
  tx_linearizer_table_type *temp_comp_lut,
  int16 temp_comp_val,
  uint8 valid_lut_entries
)
{
  uint8 lut_index,lut_beg,lut_mid,lut_end = 0;
  int16 new_power = 0;

  if ((tx_lut == NULL ) || (temp_comp_lut == NULL ))
  {
    RF_MSG( RF_ERROR, " Common Txlin temp comp update: NULL pointer for LUT!"); 
    return FALSE;
  }

  /* loop through the LUT to apply temp comp for each entry*/
  for (lut_index = 0; lut_index<valid_lut_entries; lut_index++)
  {
    /* check Invalid entry in the LUT */
    if (tx_lut->table[lut_index].rgi != 0xFFFF)
    {
      /* Apply temp comp val to offset the power */
      new_power = tx_lut->table[lut_index].power + temp_comp_val;
      temp_comp_lut->table[lut_index].rgi = tx_lut->table[lut_index].rgi;
      temp_comp_lut->table[lut_index].power = new_power;
      /* explicitly program IQ offset to zero in case of APT */
      temp_comp_lut->table[lut_index].iq_offset = 0; 
      temp_comp_lut->table[lut_index].dpd_index = 
                                  tx_lut->table[lut_index].dpd_index;
      temp_comp_lut->table[lut_index].fcomp_idx = 
                                  tx_lut->table[lut_index].fcomp_idx;
      temp_comp_lut->table[lut_index].rsb_correction_coeff.alpha =
        tx_lut->table[lut_index].rsb_correction_coeff.alpha ;
      temp_comp_lut->table[lut_index].rsb_correction_coeff.beta =
        tx_lut->table[lut_index].rsb_correction_coeff.beta ;
      temp_comp_lut->table[lut_index].loft_correction = 
        tx_lut->table[lut_index].loft_correction ;

      /* Only track by power in cold */
      if (temp_comp_val > RFCOM_TXLIN_TEMP_COMP_TRACK_POWER_THRESH)
      {

        /* for each new power, search the original LUT to find the power vs. smps */

        /* First do boundary Check, saturating smps bias at both boundary*/
        if (new_power<=tx_lut->table[0].power)
        {
          temp_comp_lut->table[lut_index].apt = tx_lut->table[0].apt;
        }
        else if (new_power>=tx_lut->table[valid_lut_entries-1].power)
        {
          temp_comp_lut->table[lut_index].apt = tx_lut->table[valid_lut_entries-1].apt;
        }
        else
        /* Target new power is within the original LUT boundries */
        {
          /* Binary Search the original LUT to find the entry where power=new_power */
          lut_beg = 0;
          lut_end = valid_lut_entries-1;
          lut_mid = (lut_beg+lut_end)/2;
          while ((lut_beg<=lut_end)&&(tx_lut->table[lut_mid].power!=new_power))
          {
            if (new_power<tx_lut->table[lut_mid].power)
            {
              lut_end=lut_mid-1;
            }
            else
            {
              lut_beg=lut_mid+1;
            }
            lut_mid = (lut_beg+lut_end)/2;
          }
          /* Binning the smps bias at the higher power entry from the orginal LUT to be
          more conservative*/
          if (tx_lut->table[lut_mid].power<new_power)
          {
            temp_comp_lut->table[lut_index].apt = tx_lut->table[lut_mid+1].apt;
          }
          else
          {
            temp_comp_lut->table[lut_index].apt = tx_lut->table[lut_mid].apt;
          }
        /* End binary search for the SMPS Bias on the given Power */
        }
      }
      else
      /* Track by RGI in hot i.e. only shift the power in the LUT, no change to apt smps so rgi vs apt relationship remains the same */
      {
        temp_comp_lut->table[lut_index].apt = tx_lut->table[lut_index].apt;
      }
    }
    else
    {
      /* For invalid RGI values, copy over the original LUT values to the
      temp comp LUT */
      temp_comp_lut->table[lut_index].rgi = tx_lut->table[lut_index].rgi;
      temp_comp_lut->table[lut_index].power = tx_lut->table[lut_index].power;
      temp_comp_lut->table[lut_index].apt = tx_lut->table[lut_index].apt;
      /* explicitly program IQ offset to zero in case of APT */
      temp_comp_lut->table[lut_index].iq_offset = 0;
      temp_comp_lut->table[lut_index].dpd_index = 
                                  tx_lut->table[lut_index].dpd_index;
      temp_comp_lut->table[lut_index].fcomp_idx = 
                                  tx_lut->table[lut_index].fcomp_idx;

      /* TempComp: RSB and LOFT corrections */
      temp_comp_lut->table[lut_index].rsb_correction_coeff.alpha =
                          tx_lut->table[lut_index].rsb_correction_coeff.alpha ;
      temp_comp_lut->table[lut_index].rsb_correction_coeff.beta =
                          tx_lut->table[lut_index].rsb_correction_coeff.beta ;
      temp_comp_lut->table[lut_index].loft_correction =
                          tx_lut->table[lut_index].loft_correction ;
    }
  }
  /* End updating the temp comp LUT table 
     Copy the channel frequency from the original table */
  temp_comp_lut->chan_freq = tx_lut->chan_freq;
  temp_comp_lut->dpd_type  = tx_lut->dpd_type;
  temp_comp_lut->num_dpd   = tx_lut->num_dpd;
  temp_comp_lut->num_fcomp = tx_lut->num_fcomp;
  temp_comp_lut->fcomp_type = tx_lut->fcomp_type;
    
  return TRUE;
} /* rfcommon_core_txlin_temp_comp_update_apt */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to compare values during qsort for increasing order sort
 
  @param p1
  Pointer to the first element to be compared
 
  @param p2
  Pointer to the second element to be compared
 
  @return
  Return -1 if p1 less than p2,
          0 if p1 equals p2,
          1 if p1 greater than p2
*/
int
rfcommon_core_qsort_compare_increasing_pwr
(
  const void* p1,
  const void* p2
)
{
  int ret_val;

  /* Typecast each element to the struct type */
  tx_linearizer_row_type *lin_data_1 = (tx_linearizer_row_type *)p1;
  tx_linearizer_row_type *lin_data_2 = (tx_linearizer_row_type *)p2;

  if ( lin_data_1 == NULL || lin_data_2 == NULL )
  {
    ret_val = 0;
  }
  else
  {
  /* Perform a subtraction for the Power values to
  compute greater than/less than/equals result */
  ret_val = (lin_data_1->power) - (lin_data_2->power);
  }

  return ret_val;
} /* rfcommon_core_qsort_compare_increasing_pwr */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common functionality to load Tx AGC across techs - Create Tx AGC buffers

  @details

  @return
  TRUE if the API was a success, else FALSE
*/
boolean
rfcommon_core_txlin_init_data
(
  void
)
{
  boolean ret_val = TRUE;
  uint8 fbrx_gain_state = 0;
  uint8 dev_count = 0 ;

  for ( dev_count = 0 ; dev_count < RFCOM_MAX_TX_DEVICES_SUPPORTED ; dev_count++ )
  {
    rf_buffer_intf** txagc_buf = &(tx_lin_info[dev_count].tx_agc_buffer[0]) ;
    rf_buffer_intf** fbrx_lut_buf = &(tx_lin_info[dev_count].fbrx_lut_buffer[0]) ;
    /* Create a buffer to hold RFFE data for LUT index */
    txagc_buf[RFCMN_TXLIN_AGC_WTR_GW_TASK] = 
                      rf_buffer_create( RFCMN_TXLIN_AGC_WTR_DATA_TASK_NUM_TRANS, 
                                        RFCMN_TXLIN_AGC_WTR_DATA_TASK_NUM_TRANS, 
                                        0 );
    txagc_buf[RFCMN_TXLIN_AGC_WXE_BBF_TASK] = 
                      rf_buffer_create( 0, 
                                        RFCMN_TXLIN_AGC_WXE_BBF_TASK_NUM_TRANS, 
                                        0 );
    txagc_buf[RFCMN_TXLIN_AGC_WTR_EARLY_TRIG_TASK] = 
                      rf_buffer_create( RFCMN_TXLIN_AGC_WTR_TRIG_TASK_NUM_TRANS, 
                                        RFCMN_TXLIN_AGC_WTR_TRIG_TASK_NUM_TRANS, 
                                        0 );
    txagc_buf[RFCMN_TXLIN_AGC_WXE_FBRX_TASK] = 
                      rf_buffer_create( 0, 
                                        RFCMN_TXLIN_AGC_WXE_FBRX_TASK_NUM_TRANS, 
                                        0 );
    /* Make the early and late trig tasks point to the same rf buffer */
    txagc_buf[RFCMN_TXLIN_AGC_WTR_LATE_TRIG_TASK] = 
          txagc_buf[RFCMN_TXLIN_AGC_WTR_EARLY_TRIG_TASK];

    for ( fbrx_gain_state = 0; fbrx_gain_state < RFCOMMON_FBRX_NUM_GAIN_STATES; 
          fbrx_gain_state++ )
    {
      fbrx_lut_buf[fbrx_gain_state] = 
                        rf_buffer_create( 0, 
                                          RFCMN_TXLIN_AGC_WXE_FBRX_TASK_NUM_TRANS, 
                                          0 );
    }

    /* Create a buffer to hold RFFE data for PA */
    txagc_buf[RFCMN_TXLIN_AGC_PA_TASK] = 
                        rf_buffer_create( 0, 
                                          RFCMN_TXLIN_AGC_PA_TASK_NUM_TRANS, 
                                          RFCMN_TXLIN_AGC_PA_TASK_NUM_TRANS );

    /* Create a buffer to hold RFFE data for QPOET */
    txagc_buf[RFCMN_TXLIN_AGC_PAPM_DATA_TASK] = 
                      rf_buffer_create( 0, 
                                        RFCMN_TXLIN_AGC_PAPM_DATA_TASK_NUM_TRANS,
                                        0 );

    /* Create a buffer to hold RFFE data for QPOET */
    txagc_buf[RFCMN_TXLIN_AGC_PAPM_EARLY_TRIG_TASK] = 
                      rf_buffer_create( 0, 
                                        RFCMN_TXLIN_AGC_PAPM_TRIG_TASK_NUM_TRANS,
                                        0 );
    txagc_buf[RFCMN_TXLIN_AGC_PAPM_LATE_TRIG_TASK] = 
      txagc_buf[RFCMN_TXLIN_AGC_PAPM_EARLY_TRIG_TASK];

    /* Supplemental buffers */
    tx_lin_info[dev_count].dummy_trig_buffer = rf_buffer_create( 
                                          RFCMN_TXLIN_AGC_WTR_TRIG_TASK_NUM_TRANS,
                                          RFCMN_TXLIN_AGC_WTR_TRIG_TASK_NUM_TRANS, 
                                          0 );
    tx_lin_info[dev_count].pa_trig_buffer = rf_buffer_create( 
                                       RFCMN_TXLIN_AGC_PA_TASK_NUM_TRANS, 
                                       RFCMN_TXLIN_AGC_PA_TASK_NUM_TRANS, 
                                       0 );

    /* Initialize linearizer logging data for upto two Tx devices */
    tx_lin_info[dev_count].log_data = 
      (rfcommon_core_txlin_log_data_type*)modem_mem_alloc( 
                                   sizeof(rfcommon_core_txlin_log_data_type),
                                   MODEM_MEM_CLIENT_RFA );
    memset( tx_lin_info[dev_count].log_data, 0, 
            sizeof(rfcommon_core_txlin_log_data_type) );

    RF_MSG( RF_LOW, "rfcommon_core_txlin_init_data: Tx Lin Init data done" );

  }
  return ret_val;
}/* rfcommon_core_txlin_init_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common functionality to load Tx AGC across techs - Destroy Tx AGC buffers

  @details

  @return
  TRUE if the API was a success, else FALSE
*/
boolean
rfcommon_core_txlin_deinit_data
(
  void
)
{
  boolean ret_val = TRUE;
  rfcommon_core_txlin_agc_sequence_type task_type;
  uint8 fbrx_gain_state = 0;
  uint8 dev_count = 0 ;

  for ( dev_count = 0 ; dev_count < RFCOM_MAX_TX_DEVICES_SUPPORTED ; dev_count++ )
  {
    rf_buffer_intf** txagc_buf = &(tx_lin_info[dev_count].tx_agc_buffer[0]) ;
    rf_buffer_intf** fbrx_lut_buf = &(tx_lin_info[dev_count].fbrx_lut_buffer[0]) ;

    /* Destory buffers after all tasks have been pushed, because some tasks 
    such as PAPM early and trig, share the same rf buffer */
    for ( task_type = RFCMN_TXLIN_AGC_PAPM_DATA_TASK;
          task_type < RFCMN_TXLIN_AGC_MAX_TASK;
          task_type++ )
    {
      if ( txagc_buf[task_type] != NULL && 
           task_type != RFCMN_TXLIN_AGC_PAPM_LATE_TRIG_TASK &&
           task_type != RFCMN_TXLIN_AGC_WTR_LATE_TRIG_TASK )
      {
        rf_buffer_destroy( txagc_buf[task_type] );
      }
      txagc_buf[task_type] = NULL;
    }

    for ( fbrx_gain_state = 0; fbrx_gain_state < RFCOMMON_FBRX_NUM_GAIN_STATES; 
          fbrx_gain_state++ )
    {
      if ( fbrx_lut_buf[fbrx_gain_state] != NULL )
      {
        rf_buffer_destroy( fbrx_lut_buf[fbrx_gain_state] );
      }
    }

    /* Destroy the supplemental buffers */
    if ( tx_lin_info[dev_count].dummy_trig_buffer != NULL )
    {
      rf_buffer_destroy( tx_lin_info[dev_count].dummy_trig_buffer );
    }
    if ( tx_lin_info[dev_count].pa_trig_buffer != NULL )
    {
      rf_buffer_destroy( tx_lin_info[dev_count].pa_trig_buffer );
    }

    /* Deinitialize linearizer logging data for upto two Tx devices */
    if( tx_lin_info[dev_count].log_data != NULL )
    {
      modem_mem_free( tx_lin_info[dev_count].log_data,
                      MODEM_MEM_CLIENT_RFA );
    }

    RF_MSG( RF_LOW, "rfcommon_core_txlin_deinit_data: Tx Lin DeInit data done" );

  }
  return ret_val;
}/* rfcommon_core_txlin_deinit_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common functionality to load Tx AGC across techs - Query Device LUT

  @details
  Provide common interface for Tx AGC and linearizer loading to query LUT 
  information from device. Device provides an interface to common in order to 
  meet this requirement.

  @param rfm_mode
  Current tech that is requesting the LUT information query

  @param rfm_band
  Data structure holding band information for the current tech

  @param wtr_device
  Pointer to WTR device obtained from RFC

  @param num_queries
  Number of linearizer row queries which must be made

  @param row_info
  Row info for number of queries

  @param log_data
  Linearizer logging data structure 

  @param tx_lin_data
  Tx linearizer interface data structure

  @param tx_front_end_addr
  Front end address interface data structure

  @return
  TRUE if the API was a success, else FALSE
*/
static boolean
rfcommon_core_txlin_get_tx_device_lut_data
(
  rfm_mode_enum_type rfm_mode,
  rfm_device_enum_type device,
  rfcom_band_type_u rfm_band,
  int32 tx_handle_id,
  void* wtr_device,
  uint8 num_queries,
  rfcommon_core_txlin_row_info_type row_info[],
  rfcommon_core_txlin_log_data_type *log_data,
  rfcommon_mdsp_tx_linearizer_config_type *tx_lin_data,
  rfcommon_mdsp_tx_front_addr_config_type *tx_front_end_addr
)
{
  boolean ret_val = FALSE ;
  uint8 lin_index = 0;
  uint8 data_count;
  uint8 trans_count;
  uint8 num_trans;
  uint8 wtr_data_count;
  const rf_hal_bus_resource_script_type *rffe_script_ptr = NULL;
  const rf_hal_bus_resource_script_type *ssbi_script_ptr = NULL;

  rfdevice_tx_lut_data_type lut_data;
  uint8 last_valid_index = 0;
  uint8 fbrx_gain_state;
  rfm_pa_gain_state_type pa_state = RFM_PA_GAIN_STATE_0;

  rf_buffer_intf** txagc_buf = rfcommon_core_txlin_get_tx_agc_buffer ( device ) ;
  rf_buffer_intf** fbrx_lut_buf = rfcommon_core_txlin_get_fbrx_lut_buffer ( device ) ;

  /* Extract PA state information */
  pa_state = row_info[0].pa_state;

  if ( txagc_buf == NULL )
  {
    RF_MSG ( RF_ERROR , "rfcommon_core_txlin_get_tx_device_lut_data(): "  
             "rfcommon_core_txlin_get_tx_agc_buffer() returned a NULL pointer " ) ;
    ret_val = FALSE;
  }
  else if ( fbrx_lut_buf == NULL )
  {
    RF_MSG ( RF_ERROR , "rfcommon_core_txlin_get_tx_device_lut_data(): "  
             "rfcommon_core_txlin_get_fbrx_lut_buffer() returned a NULL pointer " ) ;
    ret_val = FALSE;
  }
  /* Check if we have a valid device */
  else if ( wtr_device == NULL )
  {
    RF_MSG ( RF_ERROR , "rfcommon_core_txlin_get_tx_device_lut_data(): "
                        "Null WTR Tx device pointer" );
    ret_val = TRUE;
  }
  else
  {
    /* Buffer check passed */
    ret_val = TRUE;

    /* Check PA state */
    if ( pa_state >= RFM_PA_GAIN_STATE_NUM )
    {
      RF_MSG_1( RF_ERROR , "rfcommon_core_txlin_get_tx_device_lut_data(): "
                           "Invalid PA state %d", pa_state ) ;
      pa_state = RFM_PA_GAIN_STATE_0;
    }

    /* Wrap data for query */
    lut_data.bbf_script = txagc_buf[RFCMN_TXLIN_AGC_WXE_BBF_TASK];
    lut_data.tx_gain_script = txagc_buf[RFCMN_TXLIN_AGC_WTR_GW_TASK];
    lut_data.trigger_script = txagc_buf[RFCMN_TXLIN_AGC_WTR_EARLY_TRIG_TASK];

    /* Linearizer configuration */
    tx_lin_data->full_bias_lin_size = 0;
    tx_lin_data->calibrated_lin_size = 0;

    /* Query FB Rx enable data */
    for ( fbrx_gain_state = 0; fbrx_gain_state < RFCOMMON_FBRX_NUM_GAIN_STATES; 
          fbrx_gain_state++ )
    {
      /* Clear the buffer before use */
      if ( fbrx_lut_buf[fbrx_gain_state] != NULL )
      {
        rf_buffer_clear( fbrx_lut_buf[fbrx_gain_state] );

        ret_val &= rfdevice_cmn_get_fbrx_enable_script( 
                              wtr_device,
                              fbrx_gain_state,
                              RFDEVICE_CREATE_SCRIPT,
                              fbrx_lut_buf[fbrx_gain_state],
                              FALSE,
                              0,
                              rfm_mode );
        rffe_script_ptr = rf_buffer_get_script_ptr( 
                                    fbrx_lut_buf[fbrx_gain_state],
                                    RF_HAL_BUS_RESOURCE_RFFE );
        if ( rffe_script_ptr != NULL && 
             rffe_script_ptr->script_ptr.rffe != NULL )
        {
          tx_lin_data->wtr_config.fbrx_non_trig[fbrx_gain_state] =
            rffe_script_ptr->script_ptr.rffe[0].data[0];
        } /* if ( rffe_script_ptr->script_ptr.rffe != NULL ) */
      }
    }

    for ( lin_index = 0; lin_index < num_queries; lin_index++ )
    {
      uint16 iref_uamp_x10;
      uint16 full_bias_rgi = row_info[lin_index].fullbias_rgi_index;
      uint16 apt_bias_rgi = row_info[lin_index].smps_rgi_index;

      if ( full_bias_rgi != RFCOM_TXLIN_INVALID_RGI )
      {
        /* Log the RGI used */
        log_data->pa_state_data[pa_state].rgi_data[lin_index].rgi = 
                                                        full_bias_rgi;
        tx_lin_data->full_bias_lin_size++;

        /* Also provide RGI to FED */
        tx_lin_data->wtr_config.data[lin_index].rgi_val = full_bias_rgi;

        if ( lut_data.bbf_script != NULL &&
             lut_data.tx_gain_script != NULL && 
             lut_data.trigger_script != NULL &&
             txagc_buf[RFCMN_TXLIN_AGC_WXE_FBRX_TASK] != NULL )
        {
          rf_buffer_clear( lut_data.bbf_script );
          rf_buffer_clear( lut_data.tx_gain_script );
          rf_buffer_clear( lut_data.trigger_script );

          /* Query LUT data */
          ret_val &= rfdevice_cmn_get_tx_lut_scripts( 
                                wtr_device, rfm_mode, 
                                rfm_band,
                                pa_state,
                                full_bias_rgi,
                                RFDEVICE_CREATE_SCRIPT,
                                &lut_data );
          iref_uamp_x10 = (uint16)lut_data.iref_uamp_x10;

          /* Obtain FBRx gain state information */
          fbrx_gain_state = rfcommon_fbrx_mc_txagc_get_fbrx_gain_state( 
                                              tx_handle_id, 
                                              row_info[lin_index].smps_pwr );

          /* Provide previously queried FB Rx enable data */
          txagc_buf[RFCMN_TXLIN_AGC_WXE_FBRX_TASK] = fbrx_lut_buf[fbrx_gain_state];

          /* ----------------------------- WTR Config -------------------------- */
          wtr_data_count = 0;
          ssbi_script_ptr = rf_buffer_get_script_ptr( lut_data.tx_gain_script,
                                                      RF_HAL_BUS_RESOURCE_SSBI );

          if ( ssbi_script_ptr != NULL &&
               ssbi_script_ptr->script_ptr.sbi != NULL )
          {
            num_trans = ssbi_script_ptr->num_trans;
            /* For SSBI, each transaction can only have 1 byte of data */
            for ( data_count = 0; data_count < num_trans; 
                  data_count++, wtr_data_count++ )
            {
              tx_lin_data->wtr_config.data[lin_index].non_trig[wtr_data_count] =
                  ssbi_script_ptr->script_ptr.sbi[data_count].data;

              tx_front_end_addr->wtr_config_addr.non_trig[wtr_data_count] =
                  ssbi_script_ptr->script_ptr.sbi[data_count].addr;
            }
          }

          wtr_data_count = 0;
          ssbi_script_ptr = rf_buffer_get_script_ptr( lut_data.trigger_script,
                                                      RF_HAL_BUS_RESOURCE_SSBI );

          if ( ssbi_script_ptr != NULL &&
               ssbi_script_ptr->script_ptr.sbi != NULL )
          {
            num_trans = ssbi_script_ptr->num_trans;
            /* For SSBI, each transaction can only have 1 byte of data */
            for ( trans_count = 0; trans_count < num_trans; 
                  trans_count++, wtr_data_count++ )
            {
              tx_lin_data->wtr_config.data[lin_index].trig[wtr_data_count] =
                  ssbi_script_ptr->script_ptr.sbi[trans_count].data;

              tx_front_end_addr->wtr_config_addr.trig[trans_count] = 
                ssbi_script_ptr->script_ptr.sbi[trans_count].addr;
            }
          }

          rffe_script_ptr = rf_buffer_get_script_ptr( lut_data.bbf_script,
                                                      RF_HAL_BUS_RESOURCE_RFFE );
          if ( rffe_script_ptr != NULL && 
               rffe_script_ptr->script_ptr.rffe != NULL )
          {
            num_trans = rffe_script_ptr->num_trans;
            for ( trans_count = 0; trans_count < num_trans; trans_count++ )
            {
              for ( data_count = 0; 
                    data_count < rffe_script_ptr->
                                         script_ptr.rffe[trans_count].num_bytes;
                    data_count++,wtr_data_count++ )
              {
                tx_lin_data->wtr_config.data[lin_index].non_trig[wtr_data_count] =
                  rffe_script_ptr->script_ptr.rffe[trans_count].data[data_count];
              }

              tx_front_end_addr->wxe_config_addr.non_trig[trans_count] = 
                rffe_script_ptr->script_ptr.rffe[trans_count].addr;

              tx_front_end_addr->wxe_config_addr.non_trig_numb_bytes[trans_count] =
                rffe_script_ptr->script_ptr.rffe[trans_count].num_bytes;
            }
          } /* if ( rffe_script_ptr->script_ptr.rffe != NULL ) */

          rffe_script_ptr = rf_buffer_get_script_ptr( 
                              txagc_buf[RFCMN_TXLIN_AGC_WXE_FBRX_TASK],
                                      RF_HAL_BUS_RESOURCE_RFFE );
          if ( rffe_script_ptr != NULL && 
               rffe_script_ptr->script_ptr.rffe != NULL )
          {
            num_trans = rffe_script_ptr->num_trans;
            for ( trans_count = 0; trans_count < num_trans; trans_count++ )
            {
              for ( data_count = 0; 
                    data_count < rffe_script_ptr->
                                         script_ptr.rffe[trans_count].num_bytes;
                    data_count++,wtr_data_count++ )
              {
                tx_lin_data->wtr_config.data[lin_index].non_trig[wtr_data_count] =
                  rffe_script_ptr->script_ptr.rffe[trans_count].data[data_count];
              }

              tx_front_end_addr->fbrx_config_addr.non_trig[trans_count] = 
                rffe_script_ptr->script_ptr.rffe[trans_count].addr;

              tx_front_end_addr->fbrx_config_addr.non_trig_numb_bytes[trans_count] =
                rffe_script_ptr->script_ptr.rffe[trans_count].num_bytes;
            }
          } /* if ( rffe_script_ptr->script_ptr.rffe != NULL ) */

          rffe_script_ptr = rf_buffer_get_script_ptr( lut_data.tx_gain_script,
                                                      RF_HAL_BUS_RESOURCE_RFFE );
          if ( rffe_script_ptr != NULL && 
               rffe_script_ptr->script_ptr.rffe != NULL )
          {
            num_trans = rffe_script_ptr->num_trans;
            for ( trans_count = 0; trans_count < num_trans; trans_count++ )
            {
              for ( data_count = 0; 
                    data_count < rffe_script_ptr->
                                         script_ptr.rffe[trans_count].num_bytes;
                    data_count++,wtr_data_count++ )
              {
                tx_lin_data->wtr_config.data[lin_index].non_trig[wtr_data_count] =
                  rffe_script_ptr->script_ptr.rffe[trans_count].data[data_count];
              }

              tx_front_end_addr->wtr_config_addr.non_trig[trans_count] = 
                rffe_script_ptr->script_ptr.rffe[trans_count].addr;

              tx_front_end_addr->wtr_config_addr.non_trig_numb_bytes[trans_count] =
                rffe_script_ptr->script_ptr.rffe[trans_count].num_bytes;
            }
          } /* if ( rffe_script_ptr->script_ptr.rffe != NULL ) */

          /* Trigger data */
          wtr_data_count = 0;
          rffe_script_ptr = rf_buffer_get_script_ptr( lut_data.trigger_script,
                                                      RF_HAL_BUS_RESOURCE_RFFE );
          if ( rffe_script_ptr != NULL && 
               rffe_script_ptr->script_ptr.rffe != NULL )
          {
            num_trans = rffe_script_ptr->num_trans;
            for ( trans_count = 0; trans_count < num_trans; trans_count++ )
            {
              for ( data_count = 0; 
                    data_count < rffe_script_ptr->
                                         script_ptr.rffe[trans_count].num_bytes;
                    data_count++,wtr_data_count++ )
              {
                tx_lin_data->wtr_config.data[lin_index].trig[wtr_data_count] =
                  rffe_script_ptr->script_ptr.rffe[trans_count].data[data_count];
              }

              tx_front_end_addr->wtr_config_addr.trig[trans_count] = 
                rffe_script_ptr->script_ptr.rffe[trans_count].addr;

              tx_front_end_addr->wtr_config_addr.trigger_num_bytes[trans_count] =
                rffe_script_ptr->script_ptr.rffe[trans_count].num_bytes;
            }
          } /* if ( rffe_script_ptr->script_ptr.rffe != NULL ) */

          /* Iref index in LUT */
          tx_lin_data->wtr_config.data[lin_index].iref_index = 
                                  rfcommon_msm_get_iref_lut_index( iref_uamp_x10 );

          /* Provide FBRx gain state in LUT */
          tx_lin_data->wtr_config.data[lin_index].fbrx_gain_state = 
                                                                fbrx_gain_state;

           /* Provide Phase comp value in LUT */
          tx_lin_data->wtr_config.data[lin_index].phase = lut_data.phase;

          /* 1> Provide RSB coefficients from WTR per linearizer if 
             calibrated values are not found. 
             2> Calibrated RSB correction takes precedence over the static 
             values that come from the device. 
             3> Both calibrated alpha and beta values have to be valid to be 
             applied otherwise we revert to static values coing from the device.
          */
          if ( ( row_info[lin_index].rsb_corr_coeffs.alpha != 
                 RFCOM_TXLIN_INVALID_RSB_VAL ) && 
               ( row_info[lin_index].rsb_corr_coeffs.beta != 
                 RFCOM_TXLIN_INVALID_RSB_VAL ) )
          {
            tx_lin_data->wtr_config.data[lin_index].
              wtr_iq_adj_params.rsb_coeff_alpha = 
              row_info[lin_index].rsb_corr_coeffs.alpha ;
            tx_lin_data->wtr_config.data[lin_index].
              wtr_iq_adj_params.rsb_coeff_beta = 
              row_info[lin_index].rsb_corr_coeffs.beta ;
          }
          else
          { /* Apply static RSB values coming from WTR driver */
            tx_lin_data->wtr_config.data[lin_index].
              wtr_iq_adj_params.rsb_coeff_alpha = lut_data.rsb_coeff_alpha ;
            tx_lin_data->wtr_config.data[lin_index].
              wtr_iq_adj_params.rsb_coeff_beta = lut_data.rsb_coeff_beta ;
          }

        }
      }

      if ( apt_bias_rgi != RFCOM_TXLIN_INVALID_RGI )
      {
        tx_lin_data->calibrated_lin_size++;
      }
    }/* for ( lin_index = 0; lin_index < ... */

    if ( tx_lin_data->full_bias_lin_size > 1 )
    {
      /* Pad the linearizer for invalid entries */
      last_valid_index = tx_lin_data->full_bias_lin_size - 1;
      for ( lin_index = tx_lin_data->full_bias_lin_size; 
            lin_index < RFCOM_TXLIN_NUM_SEGMENTS; 
            lin_index++ )
      {
        /* For invalid RGIs, pad the linearizer with last valid data */

        /* Log the RGI used */
        log_data->pa_state_data[pa_state].rgi_data[lin_index].rgi = 
              log_data->pa_state_data[pa_state].rgi_data[last_valid_index].rgi;

        /* Also provide RGI to FED */
        tx_lin_data->wtr_config.data[lin_index].rgi_val = 
              tx_lin_data->wtr_config.data[last_valid_index].rgi_val;

        /* Non-trigger */
        for ( data_count = 0; 
              data_count < RFCOMMON_MDSP_TXLIN_NON_TRIG_DATA_SIZE;
              data_count++ )
        {
          /* WTR Config */
          tx_lin_data->wtr_config.data[lin_index].non_trig[data_count] =
            tx_lin_data->wtr_config.data[last_valid_index].non_trig[data_count];
        }

        /* Trigger */
        for ( data_count = 0; 
              data_count < RFCOMMON_MDSP_TXLIN_TRIG_DATA_SIZE;
              data_count++ )
        {
          /* WTR Config */
          tx_lin_data->wtr_config.data[lin_index].trig[data_count] =
            tx_lin_data->wtr_config.data[last_valid_index].trig[data_count];
        }

        /* Iref index in LUT */
        tx_lin_data->wtr_config.data[lin_index].iref_index = 
                      tx_lin_data->wtr_config.data[last_valid_index].iref_index;

	/*Phase value in LUT */
	tx_lin_data->wtr_config.data[lin_index].phase = 
		  tx_lin_data->wtr_config.data[last_valid_index].phase;

        /* Pad the remainder RSB coeffients with last values 
        corresponding to valid RGI entry */
        tx_lin_data->wtr_config.data[lin_index].wtr_iq_adj_params.rsb_coeff_alpha = 
          tx_lin_data->wtr_config.data[last_valid_index].wtr_iq_adj_params.rsb_coeff_alpha ;
        tx_lin_data->wtr_config.data[lin_index].wtr_iq_adj_params.rsb_coeff_beta =  
          tx_lin_data->wtr_config.data[last_valid_index].wtr_iq_adj_params.rsb_coeff_beta ;

      }
    }
  } /* Buffer check passed */

  if ( ret_val == FALSE )
  {
    RF_MSG_1( RF_ERROR, "rfcommon_core_txlin_get_tx_device_lut_data: "
                        "Failed to get Tx LUT info for RFM Mode %d",
              rfm_mode );
  }
  return ret_val;
}/* rfcommon_core_txlin_get_tx_device_lut_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common functionality to load Tx AGC across techs - Query PA data

  @details
  Provide common interface for Tx AGC and linearizer loading to query PA 
  information from device.

  @param rfm_mode
  Current tech that is requesting the LUT information query

  @param rfm_band
  Data structure holding band information for the current tech

  @param pa_device
  Pointer to PA device obtained from RFC

  @param num_queries
  Number of linearizer row queries which must be made

  @param row_info
  Row info for number of queries

  @param log_data
  Linearizer logging data structure 

  @param tx_lin_data
  Tx linearizer interface data structure

  @param tx_front_end_addr
  Front end address interface data structure

  @return
  TRUE if the API was a success, else FALSE
*/
static boolean
rfcommon_core_txlin_get_pa_device_lut_data
(
  rfm_mode_enum_type rfm_mode,
  rfm_device_enum_type device,
  rfcom_band_type_u rfm_band,
  void* pa_device,
  uint8 num_queries,
  rfcommon_core_txlin_row_info_type row_info[],
  rfcommon_core_txlin_log_data_type *log_data,
  rfcommon_mdsp_tx_linearizer_config_type *tx_lin_data,
  rfcommon_mdsp_tx_front_addr_config_type *tx_front_end_addr
)
{
  boolean ret_val = TRUE;
  uint8 lin_index = 0;
  uint8 data_count;
  uint8 trans_count;
  uint8 num_trans;
  uint8 pa_data_count;
  const rf_hal_bus_resource_script_type *rffe_script_ptr = NULL;
  const rf_hal_bus_resource_script_type *grfc_script_ptr = NULL;
  uint8 last_valid_index = 0;
  rfm_pa_gain_state_type pa_state = RFM_PA_GAIN_STATE_0;

  /* Query PA device status to obtain ICQ configuration information */
  rf_buffer_intf** txagc_buf = rfcommon_core_txlin_get_tx_agc_buffer ( device ) ;

  if ( txagc_buf == NULL )
  {
    RF_MSG ( RF_ERROR , "rfcommon_core_txlin_get_pa_device_lut_data(): "  
             "rfcommon_core_txlin_get_tx_agc_buffer() returned a NULL pointer " ) ;
    ret_val = FALSE ;
  }
  /* Check for PA device */
  else if ( pa_device == NULL )
  {
    RF_MSG ( RF_ERROR , "rfcommon_core_txlin_get_pa_device_lut_data(): "
                        "Null PA device pointer" );
    ret_val = TRUE;
  }
  else
  {
    /* Extract PA state information */
    pa_state = row_info[0].pa_state;

    /* Check PA state */
    if ( pa_state >= RFM_PA_GAIN_STATE_NUM )
    {
      RF_MSG_1( RF_ERROR , "rfcommon_core_txlin_get_pa_device_lut_data(): "
                           "Invalid PA state %d", pa_state ) ;
      pa_state = RFM_PA_GAIN_STATE_0;
    }

    tx_front_end_addr->pa_icq_fixed_addr = rfdevice_get_pa_info( pa_device );

    /* Linearizer configuration */
    for ( lin_index = 0; lin_index < num_queries; lin_index++ )
    {
      uint16 pa_icq = (uint16)row_info[lin_index].pa_curr;
      rfm_pa_range_type pa_range = row_info[lin_index].pa_range;
      rfdevice_pa_config_type pa_params_s;
      rfdevice_pa_txagc_type pa_txagc_s;

      /* Log the PA info used */
      log_data->pa_state_data[pa_state].rgi_data[lin_index].pa_icq = pa_icq;

      if ( txagc_buf[RFCMN_TXLIN_AGC_PA_TASK] != NULL )
      {
        rf_buffer_clear( txagc_buf[RFCMN_TXLIN_AGC_PA_TASK] );
   
        pa_params_s.mode = rfm_mode;
        pa_params_s.band = rfm_band;
        pa_params_s.bw = RFDEVICE_PA_BW_NOT_APPLICABLE;

        pa_params_s.trigger_type = RFDEVICE_PA_TRIGGER_TXAGC;
        pa_params_s.rfm_device = device;


        pa_txagc_s.pa_gain_range = pa_range;
        pa_txagc_s.is_mod_type_gmsk = FALSE;
        pa_txagc_s.pa_icq_array_count = 1;
        pa_txagc_s.pa_icq_array_p = &pa_icq;
        pa_txagc_s.use_default_icq_pdm = FALSE;

        ret_val &= rfdevice_set_pa_txagc( &pa_params_s, &pa_txagc_s , pa_device, 
                                          txagc_buf[RFCMN_TXLIN_AGC_PA_TASK],
                                          RFDEVICE_CREATE_SCRIPT, 
                                          RFCOM_INVALID_TIMING_OFFSET );

        /* ----------------------------- PA Config --------------------------- */
        pa_data_count = 0;
        rffe_script_ptr = rf_buffer_get_script_ptr( 
                                txagc_buf[RFCMN_TXLIN_AGC_PA_TASK],
                                RF_HAL_BUS_RESOURCE_RFFE );
        if ( rffe_script_ptr != NULL && 
             rffe_script_ptr->script_ptr.rffe != NULL )
        {
          num_trans = rffe_script_ptr->num_trans;
          for ( trans_count = 0; trans_count < num_trans; trans_count++ )
          {
            for ( data_count = 0; 
                  data_count < rffe_script_ptr->
                                       script_ptr.rffe[trans_count].num_bytes;
                  data_count++,pa_data_count++ )
            {
              tx_lin_data->pa_config.data[lin_index].ssbi_rffe[pa_data_count] =
                rffe_script_ptr->script_ptr.rffe[trans_count].data[data_count];
            }

            tx_front_end_addr->pa_config_addr[pa_state].non_trig[trans_count] = 
              rffe_script_ptr->script_ptr.rffe[trans_count].addr;

            tx_front_end_addr->pa_config_addr[pa_state].
                                             non_trig_numb_bytes[trans_count] =
              rffe_script_ptr->script_ptr.rffe[trans_count].num_bytes;
          }
        } /* if ( rffe_script_ptr->script_ptr.rffe != NULL ) */

        /* Check for GRFC PA script */
        grfc_script_ptr = rf_buffer_get_script_ptr( 
                                txagc_buf[RFCMN_TXLIN_AGC_PA_TASK],
                                RF_HAL_BUS_RESOURCE_GRFC );
        if ( grfc_script_ptr != NULL &&
             grfc_script_ptr->script_ptr.grfc != NULL &&
             grfc_script_ptr->num_trans > 0 )
        {

          for ( trans_count = 0; trans_count < grfc_script_ptr->num_trans; 
                trans_count++ )
          {
            /* Lin table GRFC[1] is used as bit mask for reset task*/
            tx_lin_data->pa_config.data[lin_index].grfc[1] |= 
                (1 << grfc_script_ptr->script_ptr.grfc[trans_count].grfc_id);              

            /* Lin table GRFC[0] is used as bit mask for set task */
            if ( grfc_script_ptr->script_ptr.grfc[trans_count].start_logic == 0)
            {
              tx_lin_data->pa_config.data[lin_index].grfc[0] |= 
                (1 << grfc_script_ptr->script_ptr.grfc[trans_count].grfc_id);
            }
          }
        }

      }
    }

    /* Pad the linearizer for invalid entries */
    if ( num_queries > 1 )
    {
      last_valid_index = num_queries - 1;
      for ( lin_index = num_queries; lin_index < RFCOM_TXLIN_NUM_SEGMENTS; 
            lin_index++ )
      {
        /* For invalid RGIs, pad the linearizer with last valid data */

        /* Log the PA info used */
        log_data->pa_state_data[pa_state].rgi_data[lin_index].pa_icq = 
           log_data->pa_state_data[pa_state].rgi_data[last_valid_index].pa_icq;

        /* Non-trigger */
        for ( data_count = 0; 
              data_count < RFCOMMON_MDSP_TXLIN_NON_TRIG_DATA_SIZE;
              data_count++ )
        {
          /* PA Config */
          tx_lin_data->pa_config.data[lin_index].ssbi_rffe[data_count] =
            tx_lin_data->pa_config.data[last_valid_index].ssbi_rffe[data_count];
        }
      }
    }
  }

  if ( ret_val == FALSE )
  {
    RF_MSG_1( RF_ERROR, "rfcommon_core_txlin_get_pa_device_lut_data: "
                        "Failed to get PA info for RFM Mode %d",
              rfm_mode );
  }

  return ret_val;
}/* rfcommon_core_txlin_get_pa_device_lut_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common functionality to load Tx AGC across techs - Query PAPM data

  @details
  Provide common interface for Tx AGC and linearizer loading to query PAPM 
  information from device.

  @param rfm_mode
  Current tech that is requesting the LUT information query

  @param rfm_band
  Data structure holding band information for the current tech

  @param papm_device
  Pointer to PAPM device obtained from RFC

  @param num_queries
  Number of linearizer row queries which must be made

  @param row_info
  Row info for number of queries

  @param log_data
  Linearizer logging data structure 

  @param tx_lin_data
  Tx linearizer interface data structure

  @param tx_front_end_addr
  Front end address interface data structure

  @return
  TRUE if the API was a success, else FALSE
*/
static boolean
rfcommon_core_txlin_get_papm_device_lut_data
(
  rfm_mode_enum_type rfm_mode,
  rfm_device_enum_type device,
  rfcom_band_type_u rfm_band,
  void* papm_device,
  uint8 num_queries,
  rfcommon_core_txlin_row_info_type row_info[],
  rfcommon_core_txlin_log_data_type *log_data,
  rfcommon_mdsp_tx_linearizer_config_type *tx_lin_data,
  rfcommon_mdsp_tx_front_addr_config_type *tx_front_end_addr
)
{
  boolean ret_val = TRUE;
  uint8 lin_index = 0;
  uint8 data_count;
  uint8 trans_count;
  uint8 num_trans;
  uint8 papm_data_count;
  const rf_hal_bus_resource_script_type *rffe_script_ptr = NULL;
  uint8 last_valid_index = 0;
  rfm_pa_gain_state_type pa_state = RFM_PA_GAIN_STATE_0;
  rf_buffer_intf** txagc_buf = rfcommon_core_txlin_get_tx_agc_buffer ( device ) ;

  /* Extract PA state information */
  pa_state = row_info[0].pa_state;

  if ( txagc_buf == NULL )
  {
    RF_MSG ( RF_ERROR , "rfcommon_core_txlin_get_papm_device_lut_data(): "  
             "rfcommon_core_txlin_get_tx_agc_buffer() returned a NULL pointer " ) ;
    ret_val = FALSE ;
  }
  /* Check for PAPM device. If NULL, exit the function but don't return failure as 
  we can have front-end configurations with no PAPM device */
  else if ( papm_device == NULL )
  {
    RF_MSG ( RF_ERROR , "rfcommon_core_txlin_get_papm_device_lut_data(): "
                        "Null PAPM device pointer" );
    ret_val = TRUE;
  }
  else
  {
    rf_buffer_clear( txagc_buf[RFCMN_TXLIN_AGC_PAPM_DATA_TASK] );
    rf_buffer_clear( txagc_buf[RFCMN_TXLIN_AGC_PAPM_EARLY_TRIG_TASK] );

    /* Check PA state */
    if ( pa_state >= RFM_PA_GAIN_STATE_NUM )
    {
      RF_MSG_1( RF_ERROR , "rfcommon_core_txlin_get_papm_device_lut_data(): "
                           "Invalid PA state %d", pa_state ) ;
      pa_state = RFM_PA_GAIN_STATE_0;
    }

    /* Make one dummy query so that trigger data can be extracted */
    /* Query PAPM mode plus bias control */
    ret_val &= rfdevice_papm_set_mode_bias_no_trigger( 
                                 papm_device, device, rfm_mode, rfm_band,
                                 RFDEVICE_APT_MODE,
                                 3700,
                                 txagc_buf[RFCMN_TXLIN_AGC_PAPM_DATA_TASK],
                                 RFDEVICE_CREATE_SCRIPT );
    /* Query PAPM trigger information together for early and late task */
    ret_val &= rfdevice_papm_trigger( 
                         papm_device, 
                         txagc_buf[RFCMN_TXLIN_AGC_PAPM_EARLY_TRIG_TASK],
                         device,
                         RFDEVICE_CREATE_SCRIPT );

    /* Linearizer configuration */
    for ( lin_index = 0; lin_index < num_queries; lin_index++ )
    {
      rfdevice_papm_mode_type xpt_mode = row_info[lin_index].xpt_mode;
      uint16 papm_bias = row_info[lin_index].papm_bias;

      /* Log the PAPM info used */
      log_data->pa_state_data[pa_state].rgi_data[lin_index].papm_bias = 
                                                                  papm_bias;

      rf_buffer_clear( txagc_buf[RFCMN_TXLIN_AGC_PAPM_DATA_TASK] );

      /* Query PAPM mode plus bias control */
      ret_val &= rfdevice_papm_set_mode_bias_no_trigger(papm_device, 
                                                        device, 
                                                        rfm_mode, 
                                                        rfm_band,
                                                        xpt_mode,
                                                        papm_bias,
                                                        txagc_buf[RFCMN_TXLIN_AGC_PAPM_DATA_TASK],
                                                        RFDEVICE_CREATE_SCRIPT);

      /* ---------------------------- PAPM Config ------------------------- */
      /* Non-trigger */
      papm_data_count = 0;
      rffe_script_ptr = rf_buffer_get_script_ptr( 
                         txagc_buf[RFCMN_TXLIN_AGC_PAPM_DATA_TASK],
                              RF_HAL_BUS_RESOURCE_RFFE );
      if ( rffe_script_ptr != NULL && 
           rffe_script_ptr->script_ptr.rffe != NULL )
      {
        num_trans = rffe_script_ptr->num_trans;
        for ( trans_count = 0; trans_count < num_trans; trans_count++ )
        {
          for ( data_count = 0; 
                data_count < rffe_script_ptr->
                                     script_ptr.rffe[trans_count].num_bytes;
                data_count++,papm_data_count++ )
          {
            tx_lin_data->
                 papm_config.data[lin_index].non_trig[papm_data_count] =
              rffe_script_ptr->script_ptr.rffe[trans_count].data[data_count];
          }

          tx_front_end_addr->papm_config_addr.non_trig[trans_count] = 
            rffe_script_ptr->script_ptr.rffe[trans_count].addr;

          tx_front_end_addr->papm_config_addr.non_trig_numb_bytes[trans_count] =
            rffe_script_ptr->script_ptr.rffe[trans_count].num_bytes;
        }
      } /* if ( rffe_script_ptr->script_ptr.rffe != NULL ) */

      /* Trigger */
      papm_data_count = 0;
      rffe_script_ptr = rf_buffer_get_script_ptr( 
            txagc_buf[RFCMN_TXLIN_AGC_PAPM_EARLY_TRIG_TASK],
                           RF_HAL_BUS_RESOURCE_RFFE );
      if ( rffe_script_ptr != NULL && 
           rffe_script_ptr->script_ptr.rffe != NULL )
      {
        num_trans = rffe_script_ptr->num_trans;
        for ( trans_count = 0; trans_count < num_trans; trans_count++ )
        {
          for ( data_count = 0; 
                data_count < rffe_script_ptr->
                                     script_ptr.rffe[trans_count].num_bytes;
                data_count++,papm_data_count++ )
          {
            tx_lin_data->papm_config.data[lin_index].trig[papm_data_count] =
              rffe_script_ptr->script_ptr.rffe[trans_count].data[data_count];
          }

          tx_front_end_addr->papm_config_addr.trig[trans_count] = 
            rffe_script_ptr->script_ptr.rffe[trans_count].addr;

          tx_front_end_addr->papm_config_addr.trigger_num_bytes[trans_count] =
            rffe_script_ptr->script_ptr.rffe[trans_count].num_bytes;
        }
      } /* if ( rffe_script_ptr->script_ptr.rffe != NULL ) */
    }

    if ( num_queries > 1 )
    {
      /* Pad the linearizer for invalid entries */
      last_valid_index = num_queries - 1;
      for ( lin_index = num_queries; lin_index < RFCOM_TXLIN_NUM_SEGMENTS; 
            lin_index++ )
      {
        /* For invalid RGIs, pad the linearizer with last valid data */

        /* Log the PAPM info used */
        log_data->pa_state_data[pa_state].rgi_data[lin_index].papm_bias = 
         log_data->pa_state_data[pa_state].rgi_data[last_valid_index].papm_bias;

        /* Non-trigger */
        for ( data_count = 0; 
              data_count < RFCOMMON_MDSP_TXLIN_NON_TRIG_DATA_SIZE;
              data_count++ )
        {
          /* PAPM Config */
          tx_lin_data->papm_config.data[lin_index].non_trig[data_count] =
            tx_lin_data->papm_config.data[last_valid_index].non_trig[data_count];
        }

        /* Trigger */
        for ( data_count = 0; 
              data_count < RFCOMMON_MDSP_TXLIN_TRIG_DATA_SIZE;
              data_count++ )
        {
          /* PAPM Config */
          tx_lin_data->papm_config.data[lin_index].trig[data_count] =
            tx_lin_data->papm_config.data[last_valid_index].trig[data_count];
        }
      }
    }
  }

  if ( ret_val == FALSE )
  {
    RF_MSG_1( RF_ERROR, "rfcommon_core_txlin_get_papm_device_lut_data: "
                        "Failed to get PAPM info for RFM Mode %d",
              rfm_mode );
  }

  return ret_val;
}/* rfcommon_core_txlin_get_papm_device_lut_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common functionality to load Tx AGC across techs - Update task start time 
  information

  @details
  Based on the trigger offsets for the trigger tasks provided by each tech,
  this API will back-compute the start times for the TxAGC tasks

  @param trig_times
  Array to hold the start/trigger times for all TxAGC tasks, including the 
  trigger tasks

  @param timing_params
  Structure holding the timing information provided by tech

  @param tx_agc_buffer
  Array holding pointers to rf_buffer_intf scripts for Tx AGC. This API 
  uses all the populated rf buffers

  @return
  TRUE if the API was a success, else FALSE
*/
static boolean
rfcommon_core_txlin_compute_txagc_trig_time
(
  int32 *trig_times,
  rfcommon_core_txlin_agc_timing_params *timing_params,
  rf_buffer_intf **tx_agc_buffer
)
{
  boolean ret_val = TRUE;
  const int8 rffe_trans_time_x10 = 20;

  /* PAPM early trig task */
  trig_times[RFCMN_TXLIN_AGC_PAPM_EARLY_TRIG_TASK] = 
    timing_params->papm_early_trig_offset - 
    (rf_buffer_get_num_trans( tx_agc_buffer[RFCMN_TXLIN_AGC_PAPM_EARLY_TRIG_TASK],
                              RF_BUFFER_RFFE )*
                              rffe_trans_time_x10);

  /* PAPM data task */
  trig_times[RFCMN_TXLIN_AGC_PAPM_DATA_TASK] = 
    trig_times[RFCMN_TXLIN_AGC_PAPM_EARLY_TRIG_TASK] - 10 -
    (rf_buffer_get_num_trans( tx_agc_buffer[RFCMN_TXLIN_AGC_PAPM_DATA_TASK],
                              RF_BUFFER_RFFE )*
                              rffe_trans_time_x10);

  /* PA task */
  trig_times[RFCMN_TXLIN_AGC_PA_TASK] = 
    timing_params->pa_trig_offset - 
    (rf_buffer_get_num_trans( tx_agc_buffer[RFCMN_TXLIN_AGC_PA_TASK],
                              RF_BUFFER_RFFE )*
                              rffe_trans_time_x10);

  /* PAPM late trig task */
  trig_times[RFCMN_TXLIN_AGC_PAPM_LATE_TRIG_TASK] = 
    timing_params->papm_late_trig_offset - 
    (rf_buffer_get_num_trans( tx_agc_buffer[RFCMN_TXLIN_AGC_PAPM_LATE_TRIG_TASK],
                              RF_BUFFER_RFFE )*
                              rffe_trans_time_x10);

  /* WTR late trig task */
  trig_times[RFCMN_TXLIN_AGC_WTR_LATE_TRIG_TASK] =
    timing_params->wtr_late_trig_offset -
    (rf_buffer_get_num_trans( tx_agc_buffer[RFCMN_TXLIN_AGC_WTR_LATE_TRIG_TASK],
                              RF_BUFFER_RFFE )*
                              rffe_trans_time_x10);

  /* WTR early trig task */
  trig_times[RFCMN_TXLIN_AGC_WTR_EARLY_TRIG_TASK] =
    timing_params->wtr_early_trig_offset -
    (rf_buffer_get_num_trans( tx_agc_buffer[RFCMN_TXLIN_AGC_WTR_EARLY_TRIG_TASK],
                              RF_BUFFER_RFFE )*
                              rffe_trans_time_x10);

  /* WTR GW task */
  trig_times[RFCMN_TXLIN_AGC_WTR_GW_TASK] =
    trig_times[RFCMN_TXLIN_AGC_WTR_EARLY_TRIG_TASK] - 10 -
    ((rf_buffer_get_num_trans( tx_agc_buffer[RFCMN_TXLIN_AGC_WTR_GW_TASK],
                              RF_BUFFER_RFFE ) + 2)*
                              rffe_trans_time_x10);

  /* WXE FBRx task */
  trig_times[RFCMN_TXLIN_AGC_WXE_FBRX_TASK] =
    trig_times[RFCMN_TXLIN_AGC_WTR_GW_TASK] - 10 -
    (rf_buffer_get_num_trans( tx_agc_buffer[RFCMN_TXLIN_AGC_WXE_FBRX_TASK],
                              RF_BUFFER_RFFE )*
                              rffe_trans_time_x10);

  /* WXE BBF task */
  trig_times[RFCMN_TXLIN_AGC_WXE_BBF_TASK] = 
    trig_times[RFCMN_TXLIN_AGC_WXE_FBRX_TASK] - 10 -
    ((rf_buffer_get_num_trans( tx_agc_buffer[RFCMN_TXLIN_AGC_WXE_BBF_TASK],
                              RF_BUFFER_RFFE )+1)*
                              rffe_trans_time_x10);

  RF_MSG_9( RF_MED, "rfcommon_core_txlin_compute_txagc_trig_time:"
                    "PAPM Data %d, PAPM Early Trig %d, PAPM Late Trig %d,"
                    "PA %d, WXE BBF %d, WXE FBRx %d, WTR Data %d,"
                    "WTR Early Trig %d, WTR Late Trig %d",
            trig_times[RFCMN_TXLIN_AGC_PAPM_DATA_TASK],
            trig_times[RFCMN_TXLIN_AGC_PAPM_EARLY_TRIG_TASK],
            trig_times[RFCMN_TXLIN_AGC_PAPM_LATE_TRIG_TASK],
            trig_times[RFCMN_TXLIN_AGC_PA_TASK],
            trig_times[RFCMN_TXLIN_AGC_WXE_BBF_TASK],
            trig_times[RFCMN_TXLIN_AGC_WXE_FBRX_TASK],
            trig_times[RFCMN_TXLIN_AGC_WTR_GW_TASK],
            trig_times[RFCMN_TXLIN_AGC_WTR_EARLY_TRIG_TASK],
            trig_times[RFCMN_TXLIN_AGC_WTR_LATE_TRIG_TASK] );

  return ret_val;
} /* rfcommon_core_txlin_compute_txagc_trig_time */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common functionality to load Tx AGC across techs - Computes the RGI 
  mapping index from smps bias linearizer to constant bias linearizer.
  This is used by RFLM for indexing RGI values

  @param smps_lut_rgi
  The RGI which needs to be searched for in the full-bias LUT

  @param delta_rgi_lut
  Structure holding the timing information provided by tech

  @param tx_agc_buffer
  Array holding pointers to rf_buffer_intf scripts for Tx AGC. This API 
  uses all the populated rf buffers

  @return
  TRUE if the API was a success, else FALSE
*/
static uint8
rfcommon_core_txlin_search_rgi_in_full_bias_lut
(
  uint16 smps_lut_rgi,
  tx_linearizer_table_data_type *delta_rgi_lut
)
{
  uint8 lin_index = 0;
  uint8 full_bias_lut_index = 0;
  tx_linearizer_table_type *tx_fullbias_linearizer =
                                   delta_rgi_lut->lin_data;

  /* Search for RGI in the full-bias tx_lin_data */
  for ( lin_index = 0; lin_index < RFCOM_TXLIN_NUM_SEGMENTS; lin_index++ )
  {
    uint16 full_bias_rgi = tx_fullbias_linearizer->table[lin_index].rgi;

    /* If the requested RGI matches the current RGI in the full bias LUT, 
    then we have found the matching index. Hence, break */
    if ( smps_lut_rgi == full_bias_rgi )
    {
      full_bias_lut_index = lin_index;
      break;
    }
  }

  return full_bias_lut_index;
} /* rfcommon_core_txlin_search_rgi_in_full_bias_lut */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common functionality to load Tx AGC across techs - Update front end address
  information

  @details
  This API configures the address information for all Tx devices to RFLM.
  This is handled as part of TxAGC configuration

  @param rfm_mode
  RFM Mode/Technology which is calling the common Tx AGC configuration

  @param device
  Device for which Tx AGC must be configured

  @param rfm_band
  Common variable to hold the tech specific band information

  @param tx_handle_id
  Tx RFLM Handle ID

  @param script_buffer_id
  Tx script buffer which must be configured for AGC

  @param wtr_device
  Pointer to WTR device for band/tech/device

  @param pa_device
  Pointer to the PA device for band/tech/device

  @param papm_device
  Pointer to the PAPM device for band/tech/device

  @param pa_state
  PA state for which linearizer must be configured

  @param tx_front_end_addr
  Structure to hold the front end address data

  @return
  TRUE if the API was a success, else FALSE
*/
static boolean
rfcommon_core_txlin_config_dummy_addr_info
(
  rfm_mode_enum_type rfm_mode,
  rfm_device_enum_type device,
  rfcom_band_type_u rfm_band,
  void* wtr_device,
  void* pa_device,
  void* papm_device,
  rfm_pa_gain_state_type pa_state,
  rfcommon_mdsp_tx_front_addr_config_type *tx_front_end_addr
)
{
  boolean ret_val = TRUE;
  uint8 papm_dummy_addr;
  uint8 pa_dummy_addr;
  uint8 trans_count;
  uint8 num_trans;
  const rf_hal_bus_resource_script_type *rffe_script_ptr = NULL;
  rfdevice_pa_config_type pa_params_s;

  rf_buffer_intf** txagc_buf = rfcommon_core_txlin_get_tx_agc_buffer ( device ) ;
  rf_buffer_intf* dummy_trig_buf = rfcommon_core_txlin_get_dummy_trig_buffer ( device ) ;
  rf_buffer_intf* pa_trig_buf = rfcommon_core_txlin_get_pa_trig_buffer ( device ) ;

  /* ----------------------------- WTR Config -------------------------- */
  /* Get dummy trigger address information */
  if ( dummy_trig_buf != NULL )
  {
    rf_buffer_clear( dummy_trig_buf );
    ret_val &= 
      rfdevice_cmn_get_dummy_trigger_script( wtr_device, dummy_trig_buf ); 

    rffe_script_ptr = 
      rf_buffer_get_script_ptr( dummy_trig_buf, RF_HAL_BUS_RESOURCE_RFFE );

    if ( rffe_script_ptr != NULL && 
         rffe_script_ptr->script_ptr.rffe != NULL )
    {
      num_trans = rffe_script_ptr->num_trans;
      for ( trans_count = 0; trans_count < num_trans; trans_count++ )
      {
        if ( trans_count > 1 )
        {
            RF_MSG_2( RF_ERROR, "rfcommon_core_txlin_config_dummy_addr_info: "
                              "Num WTR Dummy Addr exceeded %d, "
                              "Max allowed %d", trans_count, 1 );
          ret_val &= FALSE;
        }
        else
        {
          tx_front_end_addr->wtr_config_addr.dummy =
            rffe_script_ptr->script_ptr.rffe[trans_count].addr;
        }
      }
    }
  }
 
  if ( txagc_buf != NULL )
  {
    if ( txagc_buf[RFCMN_TXLIN_AGC_WXE_BBF_TASK] != NULL )
    {
      rffe_script_ptr = rf_buffer_get_script_ptr( 
                      txagc_buf[RFCMN_TXLIN_AGC_WXE_BBF_TASK],
                                  RF_HAL_BUS_RESOURCE_RFFE );
      if ( rffe_script_ptr != NULL && 
           rffe_script_ptr->script_ptr.rffe != NULL )
      {
        num_trans = rffe_script_ptr->num_trans;
        for ( trans_count = 0; trans_count < num_trans; trans_count++ )
        {
          tx_front_end_addr->wxe_config_addr.non_trig[trans_count] = 
            rffe_script_ptr->script_ptr.rffe[trans_count].addr;

          tx_front_end_addr->wxe_config_addr.non_trig_numb_bytes[trans_count] =
            rffe_script_ptr->script_ptr.rffe[trans_count].num_bytes;

          /* For FBRx, the dummy address is the same as the WXE BBF addr */
          tx_front_end_addr->fbrx_config_addr.dummy = 
                tx_front_end_addr->wxe_config_addr.non_trig[0];
        }
      } /* if ( rffe_script_ptr->script_ptr.rffe != NULL ) */
    }
    else
    {
       RF_MSG ( RF_ERROR, "rfcommon_core_txlin_config_dummy_addr_info: "
                 "txagc_buf[RFCMN_TXLIN_AGC_WXE_BBF_TASK] is NULL" );
      ret_val = FALSE ;
    }
  }
  else
  {
    RF_MSG ( RF_ERROR, "rfcommon_core_txlin_config_dummy_addr_info: "
              "rfcommon_core_txlin_get_tx_agc_buffer() returned NULL" );
   ret_val = FALSE ;
  }

  /* ----------------------------- PA Config --------------------------- */
  ret_val &= rffe_get_dummy_address( &pa_dummy_addr );
  tx_front_end_addr->pa_config_addr[pa_state].dummy = pa_dummy_addr;

  /* Query for PA trigger address */
  if ( pa_trig_buf != NULL )
  {
    rf_buffer_clear ( pa_trig_buf ) ;
    pa_params_s.mode = rfm_mode;
    pa_params_s.band = rfm_band;
    pa_params_s.bw = RFDEVICE_PA_BW_NOT_APPLICABLE;
    pa_params_s.trigger_type = RFDEVICE_PA_TRIGGER_TXAGC;
    pa_params_s.rfm_device = device;

    ret_val &= rfdevice_set_pa_trigger( &pa_params_s , pa_device, 
                                        pa_trig_buf, 
                                        RFDEVICE_CREATE_SCRIPT,
                                        RFCOM_INVALID_TIMING_OFFSET );

    rffe_script_ptr = 
      rf_buffer_get_script_ptr( pa_trig_buf, RF_HAL_BUS_RESOURCE_RFFE );

    if ( rffe_script_ptr != NULL && 
         rffe_script_ptr->script_ptr.rffe != NULL )
    {
      num_trans = rffe_script_ptr->num_trans;
      for ( trans_count = 0; trans_count < num_trans; trans_count++ )
      {
          tx_front_end_addr->pa_config_addr[pa_state].trig[trans_count] = 
          rffe_script_ptr->script_ptr.rffe[trans_count].addr;

          tx_front_end_addr->pa_config_addr[pa_state].
                                         trigger_num_bytes[trans_count] =
          rffe_script_ptr->script_ptr.rffe[trans_count].num_bytes;
      }
    }
  }

  /* ---------------------------- PAPM Config ------------------------- */
  ret_val &= rffe_get_dummy_address( &papm_dummy_addr );
  tx_front_end_addr->papm_config_addr.dummy = papm_dummy_addr;

  return ret_val;
} /* rfcommon_core_txlin_config_dummy_addr_info */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common functionality to load Tx AGC across techs

  @details
  Provides functionality for loading the Tx AGC task/sequence information to 
  FED. This invovles a two step process. 
  1. Query the Tx devices for the required script information to build the tasks.
  2. Populate the tasks in a predetermined generic sequence to FED through CCS

  @param rfm_mode
  RFM Mode/Technology which is calling the common Tx AGC configuration

  @param device
  Device for which Tx AGC must be configured

  @param rfm_band
  Common variable to hold the tech specific band information

  @param tx_handle_id
  Tx RFLM Handle ID

  @param script_buffer_id
  Tx script buffer which must be configured for AGC

  @param wtr_device
  Pointer to WTR device for band/tech/device

  @param pa_device
  Pointer to the PA device for band/tech/device

  @param papm_device
  Pointer to the PAPM device for band/tech/device

  @param tech_append_tx_agc
  Pointer to the RF buffer pointer which holds tech-specific transactions
  that must be appended to Tx AGC

  @param timing_params
  Pointer to data structure that holds timing offset parameters for Tx AGC
  tasks

  @return
  TRUE if the API was a success, else FALSE
*/
boolean
rfcommon_core_txlin_config_agc
(
  rfm_mode_enum_type rfm_mode,
  rfm_device_enum_type device,
  rfcom_band_type_u rfm_band,
  int32 tx_handle_id,
  uint16 script_buffer_id,
  void* wtr_device,
  void* pa_device,
  void* papm_device,
  rf_buffer_intf* tech_append_tx_agc,
  rfcommon_core_txlin_agc_timing_params* timing_params
)
{
  boolean ret_val = TRUE;
  rf_time_tick_type prof_start_t;
  rf_time_type prof_elapsed_t;
  uint8 i;

  rfcommon_mdsp_event_handle* event_handle = NULL;
  rfcommon_core_txlin_agc_sequence_type task_type;
  rfcommon_mdsp_tx_front_addr_config_type *tx_front_end_addr = NULL;
  const rf_hal_bus_resource_script_type *rffe_script_ptr = NULL;
  const rf_hal_bus_resource_script_type *ssbi_script_ptr = NULL;
  const rf_hal_bus_resource_script_type* grfc_script_ptr = NULL;

  int32 trig_times[RFCMN_TXLIN_AGC_MAX_TASK];
  boolean force_new_task[RFCMN_TXLIN_AGC_MAX_TASK] =
  {
    FALSE, /* RFCMN_TXLIN_AGC_PAPM_DATA_TASK */         
    TRUE,  /* RFCMN_TXLIN_AGC_PAPM_EARLY_TRIG_TASK */
    FALSE, /* RFCMN_TXLIN_AGC_PA_TASK */
    TRUE,  /* RFCMN_TXLIN_AGC_PAPM_LATE_TRIG_TASK */
    TRUE,  /* RFCMN_TXLIN_AGC_WXE_BBF_TASK */
    FALSE, /* RFCMN_TXLIN_AGC_WXE_FBRX_TASK */
    TRUE,  /* RFCMN_TXLIN_AGC_WTR_GW_TASK */
    TRUE,  /* RFCMN_TXLIN_AGC_WTR_EARLY_TRIG_TASK */
    TRUE,  /* RFCMN_TXLIN_AGC_WTR_LATE_TRIG_TASK */
  };

  uint16 num_tx_agc_tasks_pushed = 0;
  boolean grfc_pa_detected = FALSE;  
  uint16 grfc_list[2] = {0};
  uint8 grfc_num_trans = 0;
  rf_buffer_intf** txagc_buf = NULL;

  /* Get start time for profiling */
  prof_start_t = rf_time_get_tick( );

  txagc_buf = rfcommon_core_txlin_get_tx_agc_buffer ( device ) ;

  if ( txagc_buf == NULL )
  {
    RF_MSG ( RF_ERROR , "rfcommon_core_txlin_config_agc(): "  
             "rfcommon_core_txlin_get_tx_agc_buffer() returned a NULL pointer " ) ;
    ret_val = FALSE ;
  }
  else
  {
    /* Clear script buffer before its used for the next linearizer entry */
    for ( task_type = RFCMN_TXLIN_AGC_PAPM_DATA_TASK;
          task_type < RFCMN_TXLIN_AGC_MAX_TASK;
          task_type++ )
    {
      if ( txagc_buf[task_type] != NULL )
      {
        rf_buffer_clear( txagc_buf[task_type] );
      }
    }

    tx_front_end_addr =
            modem_mem_alloc( sizeof(rfcommon_mdsp_tx_front_addr_config_type),
                             MODEM_MEM_CLIENT_RFA );

    if ( tx_front_end_addr == NULL )
    {
      RF_MSG( RF_ERROR, "rfcommon_core_txlin_config_agc: "
                        "Failed to allocate tx_front_end_addr on heap" );
      return FALSE;
    }

    memset( tx_front_end_addr, 0, sizeof(rfcommon_mdsp_tx_front_addr_config_type) );

    /* ---------------------------------- PAPM ------------------------------- */
    if ( papm_device != NULL )
    {
      /* Query PAPM mode plus bias control */
      ret_val &= rfdevice_papm_set_mode_bias_no_trigger( papm_device, device, rfm_mode, 
                                                         rfm_band, RFDEVICE_APT_MODE, 0 /* bias */,
                                                         txagc_buf[RFCMN_TXLIN_AGC_PAPM_DATA_TASK],
                                                         RFDEVICE_CREATE_SCRIPT );

      /* Query PAPM trigger information together for early and late task */
      ret_val &= rfdevice_papm_trigger( 
                           papm_device, 
                           txagc_buf[RFCMN_TXLIN_AGC_PAPM_EARLY_TRIG_TASK],
                           device,
                           RFDEVICE_CREATE_SCRIPT );
    }
    else
    {
      RF_MSG( RF_HIGH, "rfcommon_core_txlin_config_agc: NULL PAPM device" );
    }

    /* ----------------------------------- PA -------------------------------- */
    if ( pa_device != NULL )
    {
      rfdevice_pa_config_type pa_params_s;
      rfdevice_pa_txagc_type pa_txagc_s;
      uint16 pa_icq = 0;

      pa_params_s.mode = rfm_mode;
      pa_params_s.band = rfm_band;
      pa_params_s.bw = RFDEVICE_PA_BW_NOT_APPLICABLE;
      pa_params_s.trigger_type = RFDEVICE_PA_TRIGGER_TXAGC;
      pa_params_s.rfm_device = device;

      pa_txagc_s.pa_gain_range = RFM_PA_R1_R0_0_0;
      pa_txagc_s.is_mod_type_gmsk = FALSE;
      pa_txagc_s.pa_icq_array_count = 1;
      pa_txagc_s.pa_icq_array_p = &pa_icq;
      pa_txagc_s.use_default_icq_pdm = FALSE;


      ret_val &= rfdevice_set_pa_txagc( &pa_params_s, &pa_txagc_s , pa_device, 
                                        txagc_buf[RFCMN_TXLIN_AGC_PA_TASK],
                                        RFDEVICE_CREATE_SCRIPT, 
                                        RFCOM_INVALID_TIMING_OFFSET );
    }
    else
    {
      RF_MSG( RF_ERROR, "rfcommon_core_txlin_config_agc: NULL PA device" );
    }

    /* Detect GRFC PA script. If found, replace the PA TASK script with set 
    script -- a script that uses dummy GRFC task with RFC_LOW. The set task
    in RFLM will send out RFC_LOW for the set bitmask specified in LUT data*/
    /* There will be two tasks created from two scripts:
    1. Reset script -- 1 task that sets the reset GRFC mask to high
    2. Set script -- 1 task that sets the set GRFC mask to low*/
    grfc_script_ptr = rf_buffer_get_script_ptr( 
                              txagc_buf[RFCMN_TXLIN_AGC_PA_TASK],
                              RF_HAL_BUS_RESOURCE_GRFC );

    if ( grfc_script_ptr != NULL &&
         grfc_script_ptr->script_ptr.grfc != NULL &&
         grfc_script_ptr->num_trans > 0 )
    {
      /*Store GRFC numbers locally for reset script*/
      grfc_num_trans =  grfc_script_ptr->num_trans;

      /* make sure only storing no more than 2 GRFCs.*/
      for(i = 0; i < grfc_script_ptr->num_trans && i < 2; i++)
      {
        grfc_list[i] = grfc_script_ptr->script_ptr.grfc[i].grfc_id;
      }        

      rf_buffer_clear( txagc_buf[RFCMN_TXLIN_AGC_PA_TASK]);

      rf_buffer_append_grfc(  txagc_buf[RFCMN_TXLIN_AGC_PA_TASK],
                              1,// grfc mask
                              0, // RFC_LOW during set task
                              0,
                              FALSE );
      grfc_pa_detected = TRUE;      
    }

    /* ---------------------------------- WTR -------------------------------- */
    if ( wtr_device != NULL )
    {
      rfdevice_tx_lut_data_type lut_data;

      /* Wrap data for query */
      lut_data.bbf_script = txagc_buf[RFCMN_TXLIN_AGC_WXE_BBF_TASK];
      lut_data.tx_gain_script = txagc_buf[RFCMN_TXLIN_AGC_WTR_GW_TASK];
      lut_data.trigger_script = txagc_buf[RFCMN_TXLIN_AGC_WTR_EARLY_TRIG_TASK];

      /* Query LUT data */
      ret_val &= rfdevice_cmn_get_tx_lut_scripts( wtr_device, rfm_mode, rfm_band,
                                             RFM_PA_GAIN_STATE_0,
                                                 0, /* default RGI */
                                                  RFDEVICE_CREATE_SCRIPT,
                                                  &lut_data );

      /* Query FB Rx enable data */
      ret_val &= rfdevice_cmn_get_fbrx_enable_script( 
                            wtr_device,
                            0,
                            RFDEVICE_CREATE_SCRIPT,
                            txagc_buf[RFCMN_TXLIN_AGC_WXE_FBRX_TASK],
                            FALSE,
                            0,
                            rfm_mode );

      /* Distinguish between RFFE and SSBI WTR for task creation */
      ssbi_script_ptr = rf_buffer_get_script_ptr( lut_data.tx_gain_script,
                                                  RF_HAL_BUS_RESOURCE_SSBI );

      /* If we have an SSBI WTR without WXE, don't force new task */
      if ( ssbi_script_ptr != NULL &&
           ssbi_script_ptr->script_ptr.sbi != NULL &&
           ssbi_script_ptr->num_trans > 0 )
      {
        force_new_task[RFCMN_TXLIN_AGC_WTR_GW_TASK] = FALSE;
      }
      /* If we have an RFFE WTR with WXE, force new gainword task */
      rffe_script_ptr = rf_buffer_get_script_ptr( lut_data.tx_gain_script,
                                                  RF_HAL_BUS_RESOURCE_RFFE );
      if ( rffe_script_ptr != NULL && 
           rffe_script_ptr->script_ptr.rffe != NULL &&
           rffe_script_ptr->num_trans > 0 )
      {
        force_new_task[RFCMN_TXLIN_AGC_WTR_GW_TASK] = TRUE;
      }
    }
    else
    {
      RF_MSG( RF_ERROR, "rfcommon_core_txlin_config_agc: NULL WTR device" );
      return FALSE;
    }

    /* Proceed with TxAGC configuration if device query APIs succeeded */
    if ( ret_val == TRUE)
    {
      /* ----------------------------------------------------------------------- */
      /* Start the CCS event for common Tx AGC config */
      ret_val &= rfcommon_mdsp_txagc_config_event_start( 
                                              rfm_mode,
                                              RFCOMMON_MDSP_CCS_EVENT_TXAGC_ID, 
                                              tx_handle_id, script_buffer_id, 
                                              &event_handle );

      /* Push the Tx AGC scripts to RFLM DM through RF CCS interface, 
      in the required sequence */
      for ( task_type = RFCMN_TXLIN_AGC_PAPM_DATA_TASK;
            task_type < RFCMN_TXLIN_AGC_MAX_TASK;
            task_type++ )
      {
        ret_val &= rfcommon_mdsp_txagc_config_event_push( 
                                               RFCOMMON_MDSP_CCS_EVENT_TXAGC_ID,
                                               tx_handle_id, script_buffer_id,
                                               &num_tx_agc_tasks_pushed,
                                               task_type,
                                               &event_handle, 
                                               txagc_buf[task_type],
                                               force_new_task[task_type] );
      }

      if (grfc_pa_detected == TRUE)
      {
        rf_buffer_intf* reset_script = rf_buffer_create(0,0,32);

        if ( reset_script != NULL)
        {
          for (i = 0; i < grfc_num_trans; i++) 
          {
            rf_buffer_append_grfc(reset_script, 
                                  grfc_list[i], // grfc mask
                                  1, //RFC_HIGH for reset task
                                  -7,
                                  FALSE );

            ret_val &= rfcommon_mdsp_txagc_config_event_push( 
                                                   RFCOMMON_MDSP_CCS_EVENT_TXAGC_ID,
                                                   tx_handle_id, script_buffer_id,
                                                   &num_tx_agc_tasks_pushed,
                                                   RFCMN_TXLIN_AGC_NO_ACTION_TASK,
                                                   &event_handle, 
                                                   reset_script,
                                                   FALSE);
          }
        }

        rf_buffer_destroy(reset_script);
      }

      /* Push the tech-specific buffer to Tx AGC */
      if ( tech_append_tx_agc != NULL )
      {
        ret_val &= rfcommon_mdsp_txagc_config_event_push( 
                                             RFCOMMON_MDSP_CCS_EVENT_TXAGC_ID,
                                             tx_handle_id, script_buffer_id,
                                             &num_tx_agc_tasks_pushed,
                                             RFCMN_TXLIN_AGC_NO_ACTION_TASK,
                                             &event_handle,
                                             tech_append_tx_agc,
                                             FALSE );
      }

      /* Commit event to RFLM */
      ret_val &= rfcommon_mdsp_txagc_config_event_commit( 
                                               RFCOMMON_MDSP_CCS_EVENT_TXAGC_ID,
                                               &event_handle );

      if ( ret_val == TRUE )
      {
        /* Update timing information in the CCS tasks, based on timing offsets 
        provided by tech */
        ret_val &= rfcommon_core_txlin_compute_txagc_trig_time( 
                                                     trig_times,
                                                     timing_params,
                                                     txagc_buf );

        ret_val &= rfcommon_mdsp_txagc_config_event_update_timing( 
                                                 RFCOMMON_MDSP_CCS_EVENT_TXAGC_ID,
                                                 tx_handle_id, script_buffer_id,
                                                 trig_times,
                                                 num_tx_agc_tasks_pushed,
                                                 &event_handle );

        /* Query dummy device address information data not captured by linearizer 
        configuration. This is also required by RFLM */
        ret_val &= rfcommon_core_txlin_config_dummy_addr_info( 
                                                 rfm_mode,
                                                 device, rfm_band,
                                                 wtr_device, 
                                                 pa_device,
                                                 papm_device,
                                                 RFM_PA_GAIN_STATE_0,
                                                 tx_front_end_addr );

        ret_val &= rfcommon_mdsp_txagc_config_dummy_front_end_addr( 
                                                              tx_handle_id,
                                                              RFM_PA_GAIN_STATE_0,
                                                              tx_front_end_addr );
      }
      else
      {
        RF_MSG( RF_ERROR, "rfcommon_core_txlin_config_agc: Tx AGC task "
                          "configuration into DM failed" );
      }

      /* Finalize sequences in RFLM after all information has been updated */
      ret_val &= rfcommon_mdsp_txagc_config_finalize_seq( tx_handle_id );

      /* Clean-up Tx AGC event handle */
      ret_val &= rfcommon_mdsp_ccs_abort_event( &event_handle );
      ret_val &= rfcommon_mdsp_ccs_cleanup_event( &event_handle );
    }/* if ( ret_val == TRUE) */

    if ( tx_front_end_addr != NULL )
    {
      modem_mem_free( tx_front_end_addr, MODEM_MEM_CLIENT_RFA );
    }

    prof_elapsed_t = rf_time_get_elapsed( prof_start_t, RF_USEC );

    RF_MSG_2( RF_HIGH, "rfcommon_core_txlin_config_agc: API status %d,"
                       "Elapsed Time %d",
              ret_val, prof_elapsed_t );
  }
    
  return ret_val;
}/* rfcommon_core_txlin_config_agc */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common functionality to load Tx linearizer across techs

  @details
  Provides functionality to query and store data corresponding to a single 
  linearizer row

  @param rfm_mode
  RFM Mode/Technology which is calling the common Tx AGC configuration

  @param device
  Device for which Tx AGC must be configured

  @param rfm_band
  Common variable to hold the tech specific band information

  @param full_bias_lin_size
  Number of linearizer row queries which must be made for valid full bias data

  @param calibrated_lin_size
  Number of linearizer row queries which must be made for valid cal data

  @param xpt_mode_type
  XPT mode (APT = 0, EPT = 1, ET = 2)

  @param row_info
  Row info for number of queries

  @param log_data
  Linearizer logging data structure 

  @param tx_lin_data
  Tx linearizer interface data structure

  @return
  TRUE if the API was a success, else FALSE
*/
static
boolean
rfcommon_core_txlin_get_non_device_linearizer_data
(
  rfm_mode_enum_type rfm_mode,
  rfm_device_enum_type device,
  rfcom_band_type_u rfm_band,
  uint8 full_bias_lin_size,
  uint8 calibrated_lin_size,
  rfcommon_mdsp_xpt_mode_enum_type xpt_mode_type,
  rfcommon_core_txlin_row_info_type row_info[],
  rfcommon_core_txlin_log_data_type *log_data,
  rfcommon_mdsp_tx_linearizer_config_type *tx_lin_data
)
{
  boolean ret_val = TRUE;
  uint8 lin_index = 0;
  uint8 last_valid_index = 0;
  rfm_pa_gain_state_type pa_state = RFM_PA_GAIN_STATE_0;

  /* Obtain PA state value once */
  pa_state = row_info[0].pa_state;

  if ( full_bias_lin_size < 1 || full_bias_lin_size > RFCOM_TXLIN_NUM_SEGMENTS ||
       calibrated_lin_size < 1 || calibrated_lin_size > RFCOM_TXLIN_NUM_SEGMENTS ||
       pa_state >= RFM_PA_GAIN_STATE_NUM )
  {
    RF_MSG_2( RF_ERROR, "rfcommon_core_txlin_get_non_device_linearizer_data: "
                        "full_bias_lin_size or calibrated_lin_size "
                        "value greater than %d or PA state greater than %d",
              RFCOM_TXLIN_NUM_SEGMENTS, RFM_PA_GAIN_STATE_NUM );
    return FALSE;
  }

  /* Dynamic linearizer configuration */
  for ( lin_index = 0; lin_index < calibrated_lin_size; lin_index++ )
  {
    /* SMPS bias power dBm10 */
    tx_lin_data->txpwr_dbm10_smpsbias[lin_index] = 
                                                 row_info[lin_index].smps_pwr;

    /* Power capping/boost algorithm support */
    if ( lin_index < RFCOMMON_MDSP_TXLIN_EPT_TBL_SIZE &&
         xpt_mode_type != RFCOMMON_MDSP_XPT_MODE_APT )
    {
      tx_lin_data->add_env_scale_cap[lin_index] = 
       log_data->max_add_env_scale_ept_et[pa_state][lin_index];
    }

    /* Mapping information from compensated linearizer RGI list to full-bias/
    RGI-delta linearizer RGI list */
    tx_lin_data->index_to_const_bias_tbl[lin_index] = 
                            row_info[lin_index].smps_rgi_to_const_rgi_mapping;

    /* IQ Gain offset */
    tx_lin_data->iq_gain_offset[lin_index] = row_info[lin_index].iq_offset;

    /* Frequency compensation index */
    tx_lin_data->fcomp_index[lin_index] = row_info[lin_index].fcomp_index;

    /* Load XPT information */
    /* AMAM */
    tx_lin_data->amam_addr[lin_index].mem_bank_id = 
                                   row_info[lin_index].amam_addr.mem_bank_id;
    tx_lin_data->amam_addr[lin_index].start_offset = 
                                  row_info[lin_index].amam_addr.start_offset;
    tx_lin_data->amam_addr[lin_index].tbl_size = 
                                      row_info[lin_index].amam_addr.tbl_size;
    /* AMPM */
    tx_lin_data->ampm_addr[lin_index].mem_bank_id = 
                                   row_info[lin_index].ampm_addr.mem_bank_id;
    tx_lin_data->ampm_addr[lin_index].start_offset = 
                                  row_info[lin_index].ampm_addr.start_offset;
    tx_lin_data->ampm_addr[lin_index].tbl_size = 
                                      row_info[lin_index].ampm_addr.tbl_size;
    /* EPDT */
    tx_lin_data->detrough_addr[lin_index].mem_bank_id = 
                               row_info[lin_index].detrough_addr.mem_bank_id;
    tx_lin_data->detrough_addr[lin_index].start_offset = 
                              row_info[lin_index].detrough_addr.start_offset;
    tx_lin_data->detrough_addr[lin_index].tbl_size = 
                                  row_info[lin_index].detrough_addr.tbl_size;
  }

  /* Pad remaining linearizer */
  last_valid_index = calibrated_lin_size - 1;
  for ( lin_index = calibrated_lin_size; 
        lin_index < RFCOM_TXLIN_NUM_SEGMENTS; 
        lin_index++ )
  {
    /* Power capping/boost algorithm support */
    if ( lin_index < RFCOMMON_MDSP_TXLIN_EPT_TBL_SIZE &&
         xpt_mode_type != RFCOMMON_MDSP_XPT_MODE_APT )
    {
      tx_lin_data->add_env_scale_cap[lin_index] = 
       log_data->max_add_env_scale_ept_et[pa_state][last_valid_index];
    }

    /* Power */
    tx_lin_data->txpwr_dbm10_smpsbias[lin_index] = 
                        tx_lin_data->txpwr_dbm10_smpsbias[last_valid_index];

    /* Index mapping */
    tx_lin_data->index_to_const_bias_tbl[lin_index] = 
                      tx_lin_data->index_to_const_bias_tbl[last_valid_index];

    /* IQ gain */
    tx_lin_data->iq_gain_offset[lin_index] = 
                      tx_lin_data->iq_gain_offset[last_valid_index];

    /* Frequency compensation index */
    tx_lin_data->fcomp_index[lin_index] = 
                      tx_lin_data->fcomp_index[last_valid_index];

    /* Load XPT information */
    /* AMAM */
    tx_lin_data->amam_addr[lin_index].mem_bank_id = 
                      tx_lin_data->amam_addr[last_valid_index].mem_bank_id;
    tx_lin_data->amam_addr[lin_index].start_offset = 
                      tx_lin_data->amam_addr[last_valid_index].start_offset;
    tx_lin_data->amam_addr[lin_index].tbl_size = 
                      tx_lin_data->amam_addr[last_valid_index].tbl_size;
    /* AMPM */
    tx_lin_data->ampm_addr[lin_index].mem_bank_id = 
                      tx_lin_data->ampm_addr[last_valid_index].mem_bank_id;
    tx_lin_data->ampm_addr[lin_index].start_offset = 
                      tx_lin_data->ampm_addr[last_valid_index].start_offset;
    tx_lin_data->ampm_addr[lin_index].tbl_size = 
                      tx_lin_data->ampm_addr[last_valid_index].tbl_size;
    /* EPDT */
    tx_lin_data->detrough_addr[lin_index].mem_bank_id = 
                    tx_lin_data->detrough_addr[last_valid_index].mem_bank_id;
    tx_lin_data->detrough_addr[lin_index].start_offset = 
                    tx_lin_data->detrough_addr[last_valid_index].start_offset;
    tx_lin_data->detrough_addr[lin_index].tbl_size = 
                    tx_lin_data->detrough_addr[last_valid_index].tbl_size;
  }

  /* Static linearizer configuration */
  for ( lin_index = 0; lin_index < full_bias_lin_size; lin_index++ )
  {
    /* Full bias power dBm10 */
    tx_lin_data->txpwr_dbm10_fullbias[lin_index] = 
                                           row_info[lin_index].fullbias_pwr;
  }
  /* Pad remaining linearizer */
  last_valid_index = full_bias_lin_size - 1;
  for ( lin_index = full_bias_lin_size; 
        lin_index < RFCOM_TXLIN_NUM_SEGMENTS; 
        lin_index++ )
  {
    /* Full bias power dBm10 */
    tx_lin_data->txpwr_dbm10_fullbias[lin_index] = 
                         tx_lin_data->txpwr_dbm10_fullbias[last_valid_index];
  }

  /* Update the max calibrated linearizer value for this PA state. The 
  last valid PA state will update this value with its maximum calibrated 
  power */
  if ( calibrated_lin_size-1 >= 0 && 
       calibrated_lin_size-1 < RFCOM_TXLIN_NUM_SEGMENTS)
  {
    log_data->max_calibrated_pa_state = pa_state;
  log_data->max_calibrated_power = 
                  tx_lin_data->txpwr_dbm10_smpsbias[calibrated_lin_size - 1];
  }

  if ( xpt_mode_type != RFCOMMON_MDSP_XPT_MODE_APT )
  {
    /* For max power boost algorithm, the max calibrated linearizer entry 
    does not represent the absolute limit, because digital gain is allowed to 
    be boosted over unity gain to transmit higher power. Hence, we add to 
    max_calibrated_power the cap which specifies how much boost on top of 
    lin power is allowed */
    if ( log_data->max_power_ovr_algo[pa_state] == 
                                        RFCOMMON_CORE_TXLIN_MAX_PWR_OVR_BOOST )
    {
      log_data->max_calibrated_power += 
        (-1)*(log_data->max_add_env_scale_ept_et[pa_state]
                                                [RFCOMMON_MDSP_TXLIN_EPT_TBL_SIZE-1]);
    }

    /* Also update max power algorithm to interface data structure */
    tx_lin_data->max_lin_cap_boost_algo = 
                              (uint8)log_data->max_power_ovr_algo[pa_state];
  }
  else
  {
    /* Also update max power algorithm to interface data structure */
    tx_lin_data->max_lin_cap_boost_algo = RFCOMMON_CORE_TXLIN_MAX_PWR_OVR_DEFAULT;
  }

  return ret_val;

}/* rfcommon_core_txlin_get_non_device_linearizer_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common functionality to load Tx linearizer across techs for Tx AGC 
  override

  @details
  Provides functionality for loading the Tx AGC linearizer corresponding to 
  the Tx AGC task sequence to FED

  @param rfm_mode
  RFM Mode/Technology which is calling the common Tx AGC configuration

  @param device
  Device for which Tx AGC must be configured

  @param rfm_band
  Common variable to hold the tech specific band information

  @param device_info
  Holder for pointers to Tx devices

  @param tx_ovr_params
  Pointer to structure holding Tx override parameters

  @return
  TRUE if the API was a success, else FALSE
*/
boolean
rfcommon_core_txlin_config_tx_override
(
  rfm_mode_enum_type rfm_mode,
  rfm_device_enum_type device,
  rfcom_band_type_u rfm_band,
  rfcommon_core_txlin_agc_device_info* device_info,
  void *tx_ovr_params
)
{
  boolean ret_val = TRUE;
  rfcommon_mdsp_tx_linearizer_config_type *tx_lin_data;
  rfcommon_mdsp_tx_front_addr_config_type *tx_front_end_addr = NULL;
  rfcommon_core_txlin_log_data_type *log_data = NULL;
  uint8 lin_index = 0;
  rfdevice_papm_mode_type xpt_mode;
  uint32 tx_handle_id;
  uint8 num_tx_overrides = 1;
  rfcommon_core_xpt_tx_override_type *curr_tx_ovr;
  rfcommon_core_txlin_row_info_type *row_info;

  /* Initialize linearizer log data pointer for this device */
  log_data = rfcommon_core_txlin_get_lin_log_buffer( device );

  if ( log_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_core_txlin_config_tx_override: "
                      "NULL pointer for logging linearizer data" );
    return FALSE;
  }

  /* Extract override params */
  curr_tx_ovr = (rfcommon_core_xpt_tx_override_type*)tx_ovr_params;
  tx_handle_id = curr_tx_ovr->rflm_handle;
  switch ( curr_tx_ovr->xpt_mode )
  {
  case RFCOMMON_MDSP_XPT_MODE_APT:
  case RFCOMMON_MDSP_XPT_MODE_EPT:
    xpt_mode = RFDEVICE_APT_MODE;
    break;
  case RFCOMMON_MDSP_XPT_MODE_ET:
    xpt_mode = RFDEVICE_ET_MODE;
    break;
  default:
    xpt_mode = RFDEVICE_NO_PT_MODE;
    break;
  }

  /* Allocate Memory for Linearizer Table*/
  tx_lin_data = 
          modem_mem_alloc( sizeof(rfcommon_mdsp_tx_linearizer_config_type),
                           MODEM_MEM_CLIENT_RFA );
  tx_front_end_addr =
          modem_mem_alloc( sizeof(rfcommon_mdsp_tx_front_addr_config_type),
                           MODEM_MEM_CLIENT_RFA );
  row_info =
          modem_mem_alloc( sizeof(rfcommon_core_txlin_row_info_type)*
                                                     num_tx_overrides,
                           MODEM_MEM_CLIENT_RFA );

  if ( tx_lin_data == NULL || tx_front_end_addr == NULL || row_info == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_core_txlin_config_tx_override: "
                      "Failed to allocate tx_lin_data or tx_front_end_addr "
                      "on heap" );
    ret_val &= FALSE;
    return ret_val;
  }

  memset( tx_lin_data, 0, sizeof(rfcommon_mdsp_tx_linearizer_config_type) );
  memset( tx_front_end_addr, 0, sizeof(rfcommon_mdsp_tx_front_addr_config_type) );
  memset( row_info, 0, sizeof(rfcommon_core_txlin_row_info_type)*
                                                       num_tx_overrides );

  /* Static linearizer configuration */
  for ( lin_index = 0; lin_index < num_tx_overrides; lin_index++ )
  {
    row_info[lin_index].smps_pwr = RFCMN_TXLIN_AGC_MIN_TX_PWR_DBM10;
    row_info[lin_index].fullbias_pwr = RFCMN_TXLIN_AGC_MIN_TX_PWR_DBM10;
    row_info[lin_index].smps_rgi_index = curr_tx_ovr->rgi;
    row_info[lin_index].fullbias_rgi_index = curr_tx_ovr->rgi;
    row_info[lin_index].smps_rgi_to_const_rgi_mapping = 0; /* Unused - default to zero */
    row_info[lin_index].pa_state = curr_tx_ovr->pa_state;
    row_info[lin_index].pa_range = curr_tx_ovr->pa_range;
    row_info[lin_index].pa_curr = curr_tx_ovr->pa_curr;
    row_info[lin_index].xpt_mode = xpt_mode;
    row_info[lin_index].papm_bias = curr_tx_ovr->papm_bias;
    row_info[lin_index].iq_offset = 0;
    row_info[lin_index].amam_addr.mem_bank_id = curr_tx_ovr->amam_addr.mem_bank_id;
    row_info[lin_index].amam_addr.start_offset = curr_tx_ovr->amam_addr.start_offset;
    row_info[lin_index].amam_addr.tbl_size = curr_tx_ovr->amam_addr.tbl_size;
    row_info[lin_index].ampm_addr.mem_bank_id = curr_tx_ovr->ampm_addr.mem_bank_id;
    row_info[lin_index].ampm_addr.start_offset = curr_tx_ovr->ampm_addr.start_offset;
    row_info[lin_index].ampm_addr.tbl_size = curr_tx_ovr->ampm_addr.tbl_size;
    row_info[lin_index].detrough_addr.mem_bank_id = 
                                     curr_tx_ovr->detrough_addr.mem_bank_id;
    row_info[lin_index].detrough_addr.start_offset = 
                                    curr_tx_ovr->detrough_addr.start_offset;
    row_info[lin_index].detrough_addr.tbl_size = curr_tx_ovr->detrough_addr.tbl_size;

  }/* for ( lin_index = 0; lin_index < num_tx_overrides; ... */

  /* Query WTR data */
  ret_val &= rfcommon_core_txlin_get_tx_device_lut_data( 
                                              rfm_mode, device ,
                                              rfm_band , tx_handle_id,
                                              device_info->wtr_device,
                                              num_tx_overrides,
                                              row_info,
                                              log_data,
                                              tx_lin_data,
                                              tx_front_end_addr );

  /* Query PA data */
  ret_val &= rfcommon_core_txlin_get_pa_device_lut_data( 
                                              rfm_mode, device ,
                                              rfm_band, 
                                              device_info->pa_device,
                                              num_tx_overrides,
                                              row_info,
                                              log_data,
                                              tx_lin_data,
                                              tx_front_end_addr );

  /* Query PAPM data */
  ret_val &= rfcommon_core_txlin_get_papm_device_lut_data( 
                                              rfm_mode, device ,
                                              rfm_band, 
                                              device_info->papm_device,
                                              num_tx_overrides,
                                              row_info,
                                              log_data,
                                              tx_lin_data,
                                              tx_front_end_addr );

  /* Query non-device related linearizer data  */
  ret_val &= rfcommon_core_txlin_get_non_device_linearizer_data( 
                                              rfm_mode, device, 
                                              rfm_band,
                                              num_tx_overrides,
                                              num_tx_overrides,
                                              curr_tx_ovr->xpt_mode,
                                              row_info,
                                              log_data,
                                              tx_lin_data );

  /* Configure linearizer for Tx override to RFLM */
  ret_val &= rfcommon_mdsp_txagc_config_static_linearizer( 
                                            tx_handle_id,
                                            curr_tx_ovr->pa_state,
                                            num_tx_overrides,
                                            tx_lin_data );

  ret_val &= rfcommon_mdsp_txagc_config_dynamic_linearizer( 
                                            tx_handle_id,
                                            curr_tx_ovr->pa_state,
                                            num_tx_overrides,
                                            tx_lin_data,
                                            FALSE  /*Don't Skip Pout Comp*/);

  /* Configure front-end address information */
  ret_val &= rfcommon_mdsp_txagc_config_front_end_addr( tx_handle_id,
                                                        curr_tx_ovr->pa_state,
                                                        tx_front_end_addr );

  if ( tx_lin_data != NULL )
  {
    modem_mem_free( tx_lin_data, MODEM_MEM_CLIENT_RFA );
  }
  if ( tx_front_end_addr != NULL )
  {
    modem_mem_free( tx_front_end_addr, MODEM_MEM_CLIENT_RFA );
  }
  if ( row_info != NULL )
  {
    modem_mem_free( row_info, MODEM_MEM_CLIENT_RFA );
  }

  return ret_val;

}/* rfcommon_core_txlin_config_tx_override */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common functionality to load Tx linearizer across techs on wakeup

  @details
  Provides functionality for loading the Tx AGC linearizer corresponding to 
  the Tx AGC task sequence to FED. This API is called one-time per Tx wakeup
  in order to provide all the static/unchanging Tx linearizer configuration 
  to FED DM

  @param rfm_mode
  RFM Mode/Technology which is calling the common Tx AGC configuration

  @param device
  Device for which Tx AGC must be configured

  @param rfm_band
  Common variable to hold the tech specific band information

  @param tx_handle_id
  Tx RFLM Handle ID

  @param final_tx_lut
  Temperature and frequency compensated LUT data

  @param delta_rgi_lut
  Full bias linearizer data

  @param device_info
  Structure holding Tx device pointers

  @param pa_stm_info
  Pointer to structure holding PA state machine info

  @param rfcommon_core_xpt_info
  Pointer to information related to xPT linearizer loading

  @return
  TRUE if the API was a success, else FALSE
*/
boolean
rfcommon_core_txlin_config_init_linearizer
(
  rfm_mode_enum_type rfm_mode,
  rfm_device_enum_type device,
  rfcom_band_type_u rfm_band,
  int32 tx_handle_id,
  tx_linearizer_table_data_type *final_tx_lut,
  tx_linearizer_table_data_type *delta_rgi_lut,
  rfcommon_core_txlin_agc_device_info* device_info,
  rfcommon_core_txlin_agc_pa_stm_info* pa_stm_info,
  void *rfcommon_core_xpt_info
)
{
  boolean ret_val = TRUE;
  rf_time_tick_type prof_start_t;
  rf_time_type prof_elapsed_t;

  rfcommon_mdsp_tx_linearizer_config_type *tx_lin_data;
  rfcommon_mdsp_tx_front_addr_config_type *tx_front_end_addr = NULL;
  rfcommon_core_txlin_log_data_type *log_data = NULL;
  uint8 lin_index = 0;
  tx_linearizer_table_type *tx_final_linearizer;
  tx_linearizer_table_type *tx_fullbias_linearizer;
  boolean lin_data_is_valid = TRUE;
  rfm_pa_gain_state_type pa_state;
  rfm_pa_range_type pa_range;
  rfcommon_mdsp_xpt_mode_enum_type xpt_mode_type = RFCOMMON_MDSP_XPT_MODE_NOPT;

  rfcommon_core_txlin_row_info_type *row_info;
  rfcommon_core_xpt_tx_agc_info_type *xpt_info;

  /* Initialize linearizer log data pointer for this device */
  log_data = rfcommon_core_txlin_get_lin_log_buffer( device );

  if ( log_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_core_txlin_config_init_linearizer: "
                      "NULL pointer for logging linearizer data" );
    return FALSE;
  }

  if ( pa_stm_info != NULL )
  {
    /* Extract PA STM info */
    pa_state = pa_stm_info->pa_state;
    pa_range = pa_stm_info->pa_range;

    if ( pa_state >= RFM_PA_GAIN_STATE_NUM )
    {
      pa_state = RFM_PA_GAIN_STATE_0;
    }

    log_data->pa_state_data[pa_state].pa_state_valid = TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR, "rfcommon_core_txlin_config_init_linearizer: "
                      "NULL pointer for PA STM info. Using defaults" );
    pa_state = RFM_PA_GAIN_STATE_0;
    pa_range = RFM_PA_R1_R0_0_0;
  }

  if ( final_tx_lut == NULL || delta_rgi_lut == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_core_txlin_config_init_linearizer: "
                      "NULL pointer for input linearizer data" );
    log_data->pa_state_data[pa_state].pa_state_valid = FALSE;

    return FALSE;
  }
  else
  {
    tx_final_linearizer = final_tx_lut->lin_data;
    tx_fullbias_linearizer = delta_rgi_lut->lin_data;
  }

  /* Get start time for profiling */
  prof_start_t = rf_time_get_tick( );

  /* Allocate Memory for Linearizer Table*/
  tx_lin_data = 
          modem_mem_alloc( sizeof(rfcommon_mdsp_tx_linearizer_config_type),
                           MODEM_MEM_CLIENT_RFA );
  tx_front_end_addr =
          modem_mem_alloc( sizeof(rfcommon_mdsp_tx_front_addr_config_type),
                           MODEM_MEM_CLIENT_RFA );
  row_info =
          modem_mem_alloc( sizeof(rfcommon_core_txlin_row_info_type)*
                                                  RFCOM_TXLIN_NUM_SEGMENTS,
                           MODEM_MEM_CLIENT_RFA );

  if ( tx_lin_data == NULL || tx_front_end_addr == NULL || row_info == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_core_txlin_config_init_linearizer: "
                      "Failed to allocate tx_lin_data or tx_front_end_addr "
                      "or row_info on heap" );
    log_data->pa_state_data[pa_state].pa_state_valid = FALSE;

    ret_val &= FALSE;
    return ret_val;
  }

  memset( tx_lin_data, 0, sizeof(rfcommon_mdsp_tx_linearizer_config_type) );
  memset( tx_front_end_addr, 0, sizeof(rfcommon_mdsp_tx_front_addr_config_type) );
  memset( row_info, 0, sizeof(rfcommon_core_txlin_row_info_type)*
                                                       RFCOM_TXLIN_NUM_SEGMENTS );

  if ( tx_final_linearizer == NULL || tx_fullbias_linearizer == NULL )
  {
    RF_MSG_1( RF_HIGH, "rfcommon_core_txlin_config_init_linearizer: "
                       "NULL pointer for input linearizer for PA state %d",
              pa_state );
    log_data->pa_state_data[pa_state].pa_state_valid = FALSE;

    lin_data_is_valid = FALSE;
  }
  else
  {
    /* For safety, re-sort the frequency and temperature compensated
    linearizer by increasing power values */
    qsort( &(tx_final_linearizer->table), RFCOM_TXLIN_NUM_SEGMENTS,
           sizeof(tx_linearizer_row_type), 
           rfcommon_core_qsort_compare_increasing_pwr );
    qsort( &(tx_fullbias_linearizer->table), RFCOM_TXLIN_NUM_SEGMENTS,
           sizeof(tx_linearizer_row_type), 
           rfcommon_core_qsort_compare_increasing_pwr );
  }

  /* Update RFM Mode to linearizer logging data */
  log_data->rfm_mode = rfm_mode;

  /* Initialize XPT info pointer */
  xpt_info = (rfcommon_core_xpt_tx_agc_info_type*)rfcommon_core_xpt_info;

  /* Linearizer configuration */
  for ( lin_index = 0; lin_index < RFCOM_TXLIN_NUM_SEGMENTS; lin_index++ )
  {
    if ( lin_data_is_valid )
    {
      row_info[lin_index].smps_pwr = 
                                   tx_final_linearizer->table[lin_index].power;
      row_info[lin_index].fullbias_pwr = 
                                tx_fullbias_linearizer->table[lin_index].power;
      row_info[lin_index].smps_rgi_index = 
                                     tx_final_linearizer->table[lin_index].rgi;
      row_info[lin_index].fullbias_rgi_index = 
                                  tx_fullbias_linearizer->table[lin_index].rgi;
      row_info[lin_index].smps_rgi_to_const_rgi_mapping = 
        rfcommon_core_txlin_search_rgi_in_full_bias_lut( 
                                  row_info[lin_index].smps_rgi_index,
                                                         delta_rgi_lut );
      row_info[lin_index].fcomp_index = 
                               tx_final_linearizer->table[lin_index].fcomp_idx;
      row_info[lin_index].pa_state = pa_state;
      row_info[lin_index].pa_range = pa_range;
      row_info[lin_index].pa_curr = tx_final_linearizer->table[lin_index].
                             apt_pa_struct.pa_q_current;

      /* Populate the row with calibrated rsb correction coefficients */
      row_info[lin_index].rsb_corr_coeffs.alpha = 
        tx_final_linearizer->table[lin_index].rsb_correction_coeff.alpha ;
      row_info[lin_index].rsb_corr_coeffs.beta = 
        tx_final_linearizer->table[lin_index].rsb_correction_coeff.beta ;
      row_info[lin_index].loft_correction = 
        tx_final_linearizer->table[lin_index].loft_correction ;

      /* Set PAPM device xPT mode based on linearizer AUX data information */
      switch ( tx_final_linearizer->dpd_type )
      {
      case TX_LINEARIZER_ET_DPD_DATA:
        row_info[lin_index].xpt_mode = RFDEVICE_ET_MODE;
        xpt_mode_type = RFCOMMON_MDSP_XPT_MODE_ET;
        break;
      case TX_LINEARIZER_EPT_DPD_V2_DATA:
        row_info[lin_index].xpt_mode = RFDEVICE_APT_MODE;
        xpt_mode_type = RFCOMMON_MDSP_XPT_MODE_EPT;
        break;
      case TX_LINEARIZER_NO_AUX_DATA:
        row_info[lin_index].xpt_mode = RFDEVICE_APT_MODE;
        xpt_mode_type = RFCOMMON_MDSP_XPT_MODE_APT;
        break;

      default:
        row_info[lin_index].xpt_mode = RFDEVICE_APT_MODE;
        xpt_mode_type = RFCOMMON_MDSP_XPT_MODE_APT;
        break;
      }
      row_info[lin_index].papm_bias = tx_final_linearizer->table[lin_index].
                                apt_pa_struct.pa_bias_voltage;
      row_info[lin_index].iq_offset = 
                               tx_final_linearizer->table[lin_index].iq_offset;
      if ( xpt_info != NULL )
      {
        /* Populate xpt data */
        row_info[lin_index].amam_addr.mem_bank_id = 
                              xpt_info->amam_bank_info[lin_index].bank_id;
        row_info[lin_index].amam_addr.start_offset = 
                              xpt_info->amam_bank_info[lin_index].bank_offset;
        row_info[lin_index].amam_addr.tbl_size = 0;
        row_info[lin_index].ampm_addr.mem_bank_id = 
                              xpt_info->ampm_bank_info[lin_index].bank_id;
        row_info[lin_index].ampm_addr.start_offset = 
                              xpt_info->ampm_bank_info[lin_index].bank_offset;
        row_info[lin_index].ampm_addr.tbl_size = 0;
        row_info[lin_index].detrough_addr.mem_bank_id = 
                              xpt_info->epdt_bank_info[lin_index].bank_id;
        row_info[lin_index].detrough_addr.start_offset = 
                              xpt_info->epdt_bank_info[lin_index].bank_offset;
        row_info[lin_index].detrough_addr.tbl_size = 0;
      }
      else
      {
        row_info[lin_index].amam_addr.mem_bank_id = -1;
        row_info[lin_index].amam_addr.start_offset = -1;
        row_info[lin_index].amam_addr.tbl_size = 0;
        row_info[lin_index].ampm_addr.mem_bank_id = -1;
        row_info[lin_index].ampm_addr.start_offset = -1;
        row_info[lin_index].ampm_addr.tbl_size = 0;
        row_info[lin_index].detrough_addr.mem_bank_id = -1;
        row_info[lin_index].detrough_addr.start_offset = -1;
        row_info[lin_index].detrough_addr.tbl_size = 0;
      }
    }
    else
    {
      /* If linearizer data is invalid, then populate min power values into 
      the linearizer RFLM interface */
      row_info[lin_index].smps_pwr = RFCMN_TXLIN_AGC_MIN_TX_PWR_DBM10;
      row_info[lin_index].fullbias_pwr = RFCMN_TXLIN_AGC_MIN_TX_PWR_DBM10;
      row_info[lin_index].smps_rgi_index = 0; /* Unused - default to zero */
      row_info[lin_index].fullbias_rgi_index = 0; /* Unused - default to zero */
      row_info[lin_index].smps_rgi_to_const_rgi_mapping = 0; /* Unused - default to zero */
      row_info[lin_index].fcomp_index = 0; /* Unused - default to zero */
      row_info[lin_index].pa_state = RFM_PA_GAIN_STATE_0;
      row_info[lin_index].pa_range = RFM_PA_R1_R0_0_0;
      row_info[lin_index].pa_curr = 0;
      row_info[lin_index].xpt_mode = RFDEVICE_APT_MODE;
      row_info[lin_index].papm_bias = RFDEVICE_PAPM_MIN_TX_BIAS_VOLTAGE;
      row_info[lin_index].iq_offset = 0;
      row_info[lin_index].amam_addr.mem_bank_id = -1;
      row_info[lin_index].amam_addr.start_offset = -1;
      row_info[lin_index].amam_addr.tbl_size = 0;
      row_info[lin_index].ampm_addr.mem_bank_id = -1;
      row_info[lin_index].ampm_addr.start_offset = -1;
      row_info[lin_index].ampm_addr.tbl_size = 0;
      row_info[lin_index].detrough_addr.mem_bank_id = -1;
      row_info[lin_index].detrough_addr.start_offset = -1;
      row_info[lin_index].detrough_addr.tbl_size = 0;
    }
  }/* for ( lin_index = 0; lin_index < RFCOM_TXLIN_NUM_SEGMENTS; ... */

  /* Query WTR data */
  ret_val &= rfcommon_core_txlin_get_tx_device_lut_data( 
                                              rfm_mode, device ,
                                              rfm_band, 
                                              tx_handle_id,
                                              device_info->wtr_device,
                                              RFCOM_TXLIN_NUM_SEGMENTS,
                                              row_info,
                                              log_data,
                                              tx_lin_data,
                                              tx_front_end_addr );

  /* Query PA data */
  ret_val &= rfcommon_core_txlin_get_pa_device_lut_data( 
                                              rfm_mode, device, rfm_band, 
                                              device_info->pa_device,
                                              tx_lin_data->calibrated_lin_size,
                                              row_info,
                                              log_data,
                                              tx_lin_data,
                                              tx_front_end_addr );

  /* Query PAPM data */
  ret_val &= rfcommon_core_txlin_get_papm_device_lut_data( 
                                              rfm_mode, device , rfm_band, 
                                              device_info->papm_device,
                                              tx_lin_data->calibrated_lin_size,
                                              row_info,
                                              log_data,
                                              tx_lin_data,
                                              tx_front_end_addr );

  /* Query non-device related linearizer data  */
  ret_val &= rfcommon_core_txlin_get_non_device_linearizer_data( 
                                              rfm_mode, device, 
                                              rfm_band,
                                              tx_lin_data->full_bias_lin_size,
                                              tx_lin_data->calibrated_lin_size,
                                              xpt_mode_type,
                                              row_info,
                                              log_data,
                                              tx_lin_data );

  /* Configure linearizer to RFLM */
  ret_val &= rfcommon_mdsp_txagc_config_static_linearizer( 
                                            tx_handle_id,
                                            pa_state,
                                            RFCOM_TXLIN_NUM_SEGMENTS,
                                            tx_lin_data );

  ret_val &= rfcommon_mdsp_txagc_config_dynamic_linearizer( 
                                            tx_handle_id,
                                            pa_state,
                                            RFCOM_TXLIN_NUM_SEGMENTS,
                                            tx_lin_data,
                                            FALSE /*Don't Skip Pout Comp*/);

  /* Configure front-end address information */
  ret_val &= rfcommon_mdsp_txagc_config_front_end_addr( tx_handle_id,
                                                        pa_state,
                                                        tx_front_end_addr );

  if ( tx_lin_data != NULL )
  {
    modem_mem_free( tx_lin_data, MODEM_MEM_CLIENT_RFA );
  }
  if ( tx_front_end_addr != NULL )
  {
    modem_mem_free( tx_front_end_addr, MODEM_MEM_CLIENT_RFA );
  }
  if ( row_info != NULL )
  {
    modem_mem_free( row_info, MODEM_MEM_CLIENT_RFA );
  }

  prof_elapsed_t = rf_time_get_elapsed( prof_start_t, RF_USEC );

  if ( ret_val == TRUE )
  {
    RF_MSG_2( RF_MED, "rfcommon_core_txlin_config_init_linearizer: "
                      "Configured for PA State %d, Elapsed Time %d", 
              pa_state, prof_elapsed_t );
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rfcommon_core_txlin_config_init_linearizer: "
                        "Failed for PA State %d", pa_state );
  }

  return ret_val;

}/* rfcommon_core_txlin_config_init_linearizer */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common functionality to load Tx linearizer across techs on wakeup

  @details
  Provides functionality for loading the Tx AGC linearizer corresponding to 
  the Tx AGC task sequence to FED. This API is called one-time per Tx wakeup
  in order to provide all the static/unchanging Tx linearizer configuration 
  to FED DM

  @param rfm_mode
  RFM Mode/Technology which is calling the common Tx AGC configuration

  @param device
  Device for which Tx AGC must be configured

  @param rfm_band
  Common variable to hold the tech specific band information

  @param tx_handle_id
  Tx RFLM Handle ID

  @param final_tx_lut
  Temperature and frequency compensated LUT data

  @param delta_rgi_lut
  Full bias linearizer data

  @param pa_stm_info
  Pointer to structure holding PA state machine info

  @return
  TRUE if the API was a success, else FALSE
*/
boolean
rfcommon_core_txlin_config_update_linearizer
(
  rfm_mode_enum_type rfm_mode,
  rfm_device_enum_type device,
  rfcom_band_type_u rfm_band,
  int32 tx_handle_id,
  tx_linearizer_table_data_type *final_tx_lut,
  tx_linearizer_table_data_type *delta_rgi_lut,
  rfcommon_core_txlin_agc_pa_stm_info* pa_stm_info,
  boolean skip_pout_comp
)
{
  boolean ret_val = TRUE;
  rf_time_tick_type prof_start_t;
  rf_time_type prof_elapsed_t;

  rfcommon_mdsp_tx_linearizer_config_type *tx_lin_data;
  rfcommon_core_txlin_log_data_type *log_data = NULL;
  uint8 lin_index = 0;
  uint8 last_valid_index;
  rfm_pa_gain_state_type pa_state = RFM_PA_GAIN_STATE_0;
  tx_linearizer_table_type *tx_final_linearizer;

  /* Initialize linearizer log data pointer for this device */
  log_data = rfcommon_core_txlin_get_lin_log_buffer( device );

  if ( log_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_core_txlin_config_update_linearizer: "
                      "NULL pointer for logging linearizer data" );
    return FALSE;
  }

  if ( pa_stm_info != NULL )
  {
    /* Extract PA STM info */
    pa_state = pa_stm_info->pa_state;

    if ( pa_state >= RFM_PA_GAIN_STATE_NUM )
    {
      pa_state = RFM_PA_GAIN_STATE_0;
    }

    log_data->pa_state_data[pa_state].pa_state_valid = TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR, "rfcommon_core_txlin_config_update_linearizer: "
                      "NULL pointer for PA STM info. Using defaults" );
    pa_state = RFM_PA_GAIN_STATE_0;
  }

  if ( final_tx_lut == NULL || delta_rgi_lut == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_core_txlin_config_update_linearizer: "
                      "NULL pointer for input linearizer data" );
    log_data->pa_state_data[pa_state].pa_state_valid = FALSE;

    return FALSE;
  }
  else
  {
    tx_final_linearizer = final_tx_lut->lin_data;
  }

  /* Get start time for profiling */
  prof_start_t = rf_time_get_tick( );

  /* Allocate Memory for Linearizer Table*/
  tx_lin_data = 
          modem_mem_alloc( sizeof(rfcommon_mdsp_tx_linearizer_config_type),
                           MODEM_MEM_CLIENT_RFA );

  if ( tx_lin_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_core_txlin_config_update_linearizer: "
                      "Failed to allocate tx_lin_data on heap" );
    log_data->pa_state_data[pa_state].pa_state_valid = FALSE;

    return FALSE;
  }

  memset( tx_lin_data, 0, sizeof(rfcommon_mdsp_tx_linearizer_config_type) );

  /* Dynamic linearizer configuration */
  /* Initialize last valid index = 0 */
  last_valid_index = 0;
  for ( lin_index = 0; lin_index < RFCOM_TXLIN_NUM_SEGMENTS; lin_index++ )
  {
    uint16 rgi_lut_index;
    rgi_lut_index = tx_final_linearizer->table[lin_index].rgi;

    /* If the RGI is valid */
    if ( rgi_lut_index != RFCOM_TXLIN_INVALID_RGI )
    {
      /* SMPS bias power dBm10 */
      tx_lin_data->txpwr_dbm10_smpsbias[lin_index] = 
                         tx_final_linearizer->table[lin_index].power;

      /* Mapping information from compensated linearizer RGI list to full-bias/
      RGI-delta linearizer RGI list */
      tx_lin_data->index_to_const_bias_tbl[lin_index] = 
        rfcommon_core_txlin_search_rgi_in_full_bias_lut( rgi_lut_index,
                                                         delta_rgi_lut );

      /* IQ Gain offset */
      tx_lin_data->iq_gain_offset[lin_index] = 
                               tx_final_linearizer->table[lin_index].iq_offset;

      /* Update last valid row */
      last_valid_index = lin_index;
    }/* if ( rgi_lut_index != 0xFFFF ) */
    else
    {
      /* For invalid RGIs, pad the linearizer with last valid data */

      /* Power */
      tx_lin_data->txpwr_dbm10_smpsbias[lin_index] = 
                          tx_lin_data->txpwr_dbm10_smpsbias[last_valid_index];

      /* Index mapping */
      tx_lin_data->index_to_const_bias_tbl[lin_index] = 
                        tx_lin_data->index_to_const_bias_tbl[last_valid_index];

      /* IQ gain */
      tx_lin_data->iq_gain_offset[lin_index] = 
                        tx_lin_data->iq_gain_offset[last_valid_index];
    }
  }/* for ( lin_index = 0; lin_index < RFCOM_TXLIN_NUM_SEGMENTS; ... */

  /* Update max calibrated linearizer power */
  if ( pa_state == log_data->max_calibrated_pa_state)
  {
    log_data->max_calibrated_power = 
                             tx_lin_data->txpwr_dbm10_smpsbias[last_valid_index];
    /* For max power boost algorithm, the max calibrated linearizer entry 
    does not represent the absolute limit, because digital gain is allowed to 
    be boosted over unity gain to transmit higher power. Hence, we add to 
    max_calibrated_power the cap which specifies how much boost on top of 
    lin power is allowed */
    if ( log_data->max_power_ovr_algo[pa_state] == 
                                         RFCOMMON_CORE_TXLIN_MAX_PWR_OVR_BOOST )
    {
      log_data->max_calibrated_power += 
        (-1)*(log_data->max_add_env_scale_ept_et[pa_state]
                                                [RFCOMMON_MDSP_TXLIN_EPT_TBL_SIZE-1]);
    }
  }

  ret_val &= rfcommon_mdsp_txagc_config_dynamic_linearizer( 
                                            tx_handle_id,
                                            pa_state,
                                            RFCOM_TXLIN_NUM_SEGMENTS,
                                            tx_lin_data,
                                            skip_pout_comp);

  if ( tx_lin_data != NULL )
  {
    modem_mem_free( tx_lin_data, MODEM_MEM_CLIENT_RFA );
  }

  prof_elapsed_t = rf_time_get_elapsed( prof_start_t, RF_USEC );

  if ( ret_val == TRUE )
  {
    RF_MSG_2( RF_LOW, "rfcommon_core_txlin_update_linearizer: Configured for "
                      "PA State %d, Elapsed Time %d", 
              pa_state, prof_elapsed_t );
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rfcommon_core_txlin_update_linearizer: "
                        "Failed for PA State %d", pa_state );
  }

  return ret_val;

}/* rfcommon_core_txlin_update_linearizer */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain Linearizer logging data from Common tx AGC

  @details
  Provides logging information related to common Tx AGC for a specific RFM 
  device, to the querying technology

  @param rfm_mode
  RFM Mode/Technology which is calling the common Tx AGC logging request

  @param device
  Device for which Tx AGC logging information is provided

  @return
  Pointer to logging data if successful, else NULL
*/
const rfcommon_core_txlin_log_data_type*
rfcommon_core_txlin_get_log_data
(
  rfm_mode_enum_type rfm_mode,
  rfm_device_enum_type device
)
{
  rfcommon_core_txlin_log_data_type *log_data = NULL;

  if( (rfm_mode >= RFM_NUM_MODES) || 
      ((device != RFM_DEVICE_0) && (device != RFM_DEVICE_2)) )
  {
    RF_MSG_2( RF_ERROR, "rfcommon_core_txlin_get_log_data: Unsupported "
                        "tech %d or device %d",
              rfm_mode, device );
    return NULL;
  }

  /* Initialize linearizer log data pointer for this device */
  log_data = rfcommon_core_txlin_get_lin_log_buffer( device );

  if ( log_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_core_txlin_get_log_data: "
                      "NULL pointer for logging linearizer data" );
    return NULL;
  }

  return (const rfcommon_core_txlin_log_data_type*)log_data;
}/* rfcommon_core_txlin_get_log_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for loading V1 MultiLin NV
 
  @details
  This is a helper function that populates the mission mode
  internal data structure from NV to hold multi-linearizer Cal data.
  It first retrieves the NV data and decompresses it, for fixed portion, directly
  copy the data from NV with converting channel number to frequency. It then dynamically
  allocate memory and copy data from NV for channels with calibrated linearizer tables
 
  @param item_code
  RFNV number for the Tx multi-linearizer Cal data.

  @param tx_lut_band_data
  pointer to the entire internal Tx linerizer band data structure which includes all 
  calibrated linearizer tables, Tx Cal frequencies, Tx linearizer frequenfcies, Tx freqcomp 
  tables etc of one band/technology

  @param convert_freq_handler
  CB handler to convert channel to frequency
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal

*/
boolean 
rfcommon_core_load_linearizer_v1_from_nv
(
  rfnv_item_id_enum_type item_code, 
  tx_band_cal_type *tx_lut_band_data,
  rfcommon_core_convert_freq_handler_type convert_freq_handler
)
{
  /* NV data pointer after decompression */
  tx_band_cal_nv_type *nv_data_ptr = NULL;
  /* Raw NV data pointer before decompression */
  rfnv_item_type *nv_data_raw_ptr;
  /* NV item size (maximum) */
  uint16 nv_item_size = sizeof(rfnv_item_type);
  /* actual NV data size after decompression*/
  uint32 uncompress_size=RFCOM_TXLIN_NV_SIZE_MAX;
  /* NV read status */
  nv_stat_enum_type nv_read_status = NV_FAIL_S;
  /* NV decompressino status */
  int8 uncompress_status = Z_ERRNO;
  /* Freqcomp RGI bin, upto 8 to support diffrent freq comp with in one pa range */
  uint8 freqcomp_rgi_bin = 0;
  /* Misc index variables*/
  uint8 tx_chan_idx, tx_waveform, pa_range, tx_cal_chan_size = 0;
  uint16 rgi_thresh_info[RFCOM_TXLIN_NUM_LINEARIZERS][RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN];
  uint8 fcomp_bin_num[RFCOM_TXLIN_NUM_LINEARIZERS];
  uint16 rgi_thresh;
  uint16 rgi_thresh_nxt;

  /* Allocate heap memory for raw NV data*/
  nv_data_raw_ptr = (rfnv_item_type *)modem_mem_alloc(nv_item_size, MODEM_MEM_CLIENT_RFA);

  /* Null pointer check */
  if (nv_data_raw_ptr == NULL)
  {
    RF_MSG( RF_ERROR, "Fail to allocate memory for raw NV data pointer ");
    return FALSE; 
  }
  /* Retrieve raw NV data from EFS */
  nv_read_status = rfcommon_rfnv_get_item(item_code, 
                                          nv_data_raw_ptr,
                                          &nv_item_size,
                                          NULL,
                                          (rex_sigs_type) 0,
                                          NULL);

  if (nv_read_status == NV_NOTACTIVE_S)
  {
    RF_MSG_1( RF_MED, "multi-linearizer NV item:%d not active ", item_code);
    modem_mem_free((void*)nv_data_raw_ptr, MODEM_MEM_CLIENT_RFA);
    return FALSE;
  }
  else if (nv_read_status == NV_FAIL_S)
  {
    RF_MSG_1( RF_ERROR, "multi-linearizer NV item:%d read failed!", item_code); 
    modem_mem_free((void*)nv_data_raw_ptr, MODEM_MEM_CLIENT_RFA);
    return FALSE; 
  }

  /* Allocate memory for NV data decompression. Allocate worst case memeory for Tx linearizer data sturcture */
  /* Allocate based on TX Multi-lin Version */
  nv_data_ptr = (tx_band_cal_nv_type *)modem_mem_alloc(RFCOM_TXLIN_NV_SIZE_MAX, MODEM_MEM_CLIENT_RFA);
  
  if (nv_data_ptr == NULL)
  {
    RF_MSG( RF_ERROR, "Fail to allocate memory for decompress multi-linearizer NV!"); 
    /* Free the memory for raw NV data*/
    modem_mem_free((void*)nv_data_raw_ptr, MODEM_MEM_CLIENT_RFA);
    return FALSE;
  }
  
  /* Decompress NV data */
  uncompress_status = uncompress((uint8 *)nv_data_ptr,   
                                 &uncompress_size,
                                 (const uint8 *)nv_data_raw_ptr, 
                                 (uint32)nv_item_size);
  
  if (uncompress_status != Z_OK)
  {
    RF_MSG_1( RF_ERROR, "decompression of multi-linearizer NV failed!status:%d", uncompress_status); 
    /* Free the memory for decompressed NV data */
    modem_mem_free((void*)nv_data_ptr, MODEM_MEM_CLIENT_RFA);
    /* Free the memory for raw NV data*/
    modem_mem_free((void*)nv_data_raw_ptr, MODEM_MEM_CLIENT_RFA);
    return FALSE;  
  }

  /*************************** 
  NV data has been successfully retrieved, proceed to populating the mission mode band_cal data structure
  ****************************/

  tx_lut_band_data->nv_rev_type = TX_MULTI_LIN_REV_1;

  /* Mainly fill-in info with invalid threshold */
  memset( rgi_thresh_info, 0xFF, sizeof(rgi_thresh_info) );

  /* Copy Freqcomp table from NV */
  /*!@NOTE: Since NV structures are PACKed vs the mission mode structure hence 
            need to copy each member individually and we should not use memcpy*/
  for (pa_range=0; pa_range<RFCOM_TXLIN_NUM_LINEARIZERS; pa_range++)
  {
    for (freqcomp_rgi_bin=0; freqcomp_rgi_bin<RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN; freqcomp_rgi_bin++)
    {
      /* Save RGI thresh to fill in fcomp_idx info in lin later */
      rgi_thresh_info[pa_range][freqcomp_rgi_bin] = nv_data_ptr->tx_pa_state_cal_data[pa_range].tx_freq_offsets[freqcomp_rgi_bin].rgi_threshold;

      /*Copy freq_comp for each of the channels*/
      for (tx_chan_idx=0; tx_chan_idx<RFCOM_TXLIN_NUM_CHANNELS; tx_chan_idx++)
      {
        /* Multiple freqcomp sweeps not supported in multi-lin v1 */
        tx_lut_band_data->tx_pa_state_cal_data[pa_range].tx_freq_offsets[TX_FREQCOMP_APT][freqcomp_rgi_bin].freq_comp[tx_chan_idx] =
        nv_data_ptr->tx_pa_state_cal_data[pa_range].tx_freq_offsets[freqcomp_rgi_bin].freq_comp[tx_chan_idx];
      }
    }

    /* FIRST: Find last monotonically increasing magnitude */
    for ( freqcomp_rgi_bin = 0; freqcomp_rgi_bin < (RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN - 1); )
    {
      rgi_thresh     = rgi_thresh_info[pa_range][freqcomp_rgi_bin + 0];
      rgi_thresh_nxt = rgi_thresh_info[pa_range][freqcomp_rgi_bin + 1];
      freqcomp_rgi_bin++;
      /* STOP if thresh is not in increasing magnitude or max thresh is reached */
      if((rgi_thresh_nxt <= rgi_thresh) || (rgi_thresh >= RFCOM_TXLIN_MAX_RGI_INDEX))
      {
        break;
      }
    }
    /* The total num of fcomp lists that are usable for this pa state based on above check */
    fcomp_bin_num[pa_range] = freqcomp_rgi_bin;
 
    /* SECOND: Find first fcomp list with out-of-bounds thresh */
    for (freqcomp_rgi_bin = 0; freqcomp_rgi_bin < fcomp_bin_num[pa_range]; )
    {
      rgi_thresh = rgi_thresh_info[pa_range][freqcomp_rgi_bin];
      freqcomp_rgi_bin++;
      /* STOP if max thresh is reached */
      if(rgi_thresh >= RFCOM_TXLIN_MAX_RGI_INDEX)
      {
        break;
      }
    }
    /* The total num of fcomp lists that are usable for this pa state based on above check */
    fcomp_bin_num[pa_range] = freqcomp_rgi_bin;
  }

  /* Loop through NV data and copy rest of data */
  for (tx_chan_idx=0; tx_chan_idx<RFCOM_TXLIN_NUM_CHANNELS; tx_chan_idx++)
  {
    if (nv_data_ptr->tx_cal_chan[tx_chan_idx]!=0xFFFF)
    {
      tx_lut_band_data->tx_cal_chan[tx_chan_idx] = nv_data_ptr->tx_cal_chan[tx_chan_idx];
      /* Convert channel to frequency using the CB handler */
      tx_lut_band_data->tx_cal_freq[tx_chan_idx] = convert_freq_handler(tx_lut_band_data->tx_cal_chan[tx_chan_idx]);
      /* Increment Valid Tx Cal channel count for Freq Comp */
      tx_cal_chan_size++;
    }

    /* Loop through Tx waveform types*/
    for (tx_waveform=0; tx_waveform<TX_LINEARIZER_NUM_WAVEFORM; tx_waveform++)
    {
      /*Loop through PA ranges */
      for (pa_range=0; pa_range<RFCOM_TXLIN_NUM_LINEARIZERS; pa_range++)
      {
        rfcommon_core_load_dynamic_linearizer_v1_entry_from_nv( nv_data_ptr,
                                                                tx_lut_band_data,
                                                                pa_range,
                                                                tx_waveform,
                                                                tx_chan_idx,
                                                                fcomp_bin_num[pa_range],
                                                                rgi_thresh_info[pa_range],
                                                                convert_freq_handler);
      }
    }
  }

  /* Set Tx Cal channel size */
  tx_lut_band_data->tx_cal_chan_size = tx_cal_chan_size;
  /* Free the memory for decompressed NV data */
  
  modem_mem_free((void*)nv_data_ptr, MODEM_MEM_CLIENT_RFA);

  /* Free the memory for raw NV data*/
  modem_mem_free((void*)nv_data_raw_ptr, MODEM_MEM_CLIENT_RFA);

  return TRUE;
} /* rfcommon_core_load_linearizer_v1_from_nv */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for loading V3 MultiLin NV
 
  @details
  This is a helper function that populates the mission mode
  internal data structure from NV to hold multi-linearizer Cal data.
  It first retrieves the NV data and decompresses it, for fixed portion, directly
  copy the data from NV with converting channel number to frequency. It then dynamically
  allocate memory and copy data from NV for channels with calibrated linearizer tables
 
  @param item_code
  RFNV number for the Tx multi-linearizer Cal data.

  @param tx_cal_data
  Pointer to the entire internal Tx data structure which includes all 
  calibrated linearizer tables, Tx Cal frequencies, Tx linearizer frequenfcies, Tx freqcomp 
  tables etc of one band/technology

  @param convert_freq_handler
  CB handler to convert channel to frequency
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal

*/
boolean 
rfcommon_core_load_linearizer_v3_from_nv
(
  rfnv_item_id_enum_type item_code, 
  tx_band_cal_type *tx_cal_data,
  rfcommon_core_convert_freq_handler_type convert_freq_handler
)
{
  /* Overall status for this API */
  boolean api_status = TRUE;
  /* NV Multi-lin V3 data pointer after decompression */
  uint8 *nv_data_rev3_ptr = NULL;
  /* Helper */
  uint8 *temp_data_ptr = NULL;
  /* Raw NV data pointer before decompression */
  rfnv_item_type *nv_data_raw_ptr;
  /* NV item size (maximum) */
  uint16 nv_item_size = sizeof(rfnv_item_type);
  /* actual NV data size after decompression for Multi-lin V3 */
  uint32 rev3_uncompress_size=RFCOM_TXLIN_NV_V3_SIZE_MAX;
  /* NV read status */
  nv_stat_enum_type nv_read_status = NV_FAIL_S;
  /* NV decompression status */
  int8 uncompress_status = Z_ERRNO;
  /* Used for looping through TYPE IDs */
  uint16 type_id_idx;
  /* The TYPE ID of the current element */
  uint16 type_id;
  /* Contains the number of bytes for an element in NV */
  uint32 nv_element_size;
  /* Keeps track of the running total of number of bytes already parsed */
  uint32 num_bytes_parsed;
  /* Helper */
  int32 temp_num_bytes_left;
  /* Contains info about data contained in the NV item */
  rf_nv_data_variant_marker_type variant_marker;
  /* Keep track of num DPDs saved */
  uint16 num_ept_dpd_v2_saved;
  uint16 num_et_dpd_saved;

  ept_dpd_v2_data_type ept_dpd_v2;
  et_dpd_data_type et_dpd;

  memset(&ept_dpd_v2, 0x0, sizeof(ept_dpd_v2));
  memset(&et_dpd, 0x0, sizeof(et_dpd));
  
  /* Allocate memory for raw NV data */
  nv_data_raw_ptr = modem_mem_alloc(nv_item_size, MODEM_MEM_CLIENT_RFA);

  /* Null pointer check */
  if (nv_data_raw_ptr == NULL)
  {
    RF_MSG_1( RF_ERROR, "MultiLin V3 - failed to allocate memory for %d bytes.", nv_item_size );
    api_status = FALSE;
  }

  if( api_status == TRUE )
  {
    /* Retrieve raw NV data from EFS */
    nv_read_status = rfcommon_rfnv_get_item(item_code, 
                                            nv_data_raw_ptr,
                                            &nv_item_size,
                                            NULL,
                                            (rex_sigs_type) 0,
                                            NULL);
  
    if (nv_read_status == NV_NOTACTIVE_S)
    {
      RF_MSG_1( RF_ERROR, "MultiLin V3 - NV ID %d not active.", item_code );
      api_status = FALSE;
    }
    else if (nv_read_status == NV_FAIL_S)
    {
      RF_MSG_1( RF_ERROR, "MultiLin V3 - NV ID %d read failed.", item_code );
      api_status = FALSE;
    }
  }

  if( api_status == TRUE )
  {
    /* Allocate worst-case memory size for NV data decompression. */
    nv_data_rev3_ptr = modem_mem_alloc(rev3_uncompress_size, MODEM_MEM_CLIENT_RFA);
  
    if (nv_data_rev3_ptr == NULL)
    {
      RF_MSG_1( RF_ERROR, "MultiLin V3 - failed to allocate memory for %d bytes.", rev3_uncompress_size );
      api_status = FALSE;
    }
  }
  
  if( api_status == TRUE )
  {
    /* Decompress NV data */
    uncompress_status = uncompress( (uint8 *)nv_data_rev3_ptr,   
                                    &rev3_uncompress_size,
                                    (const uint8 *)nv_data_raw_ptr, 
                                    (uint32)nv_item_size
                                  );
  
    if (uncompress_status != Z_OK)
    {
      RF_MSG_1( RF_ERROR, "MultiLin V3 - decompression of NV ID %d failed.", item_code );
      api_status = FALSE;
    }
  }

  /* IF NV data has been successfully retrieved, 
     proceed to populating the mission mode band_cal data structure */

  if( api_status == TRUE )
  {
    /* 
      FORMULA: To ensure there is enough memory left 
        if( (TOTAL - PARSED) < NEEDED )
        {
          ERROR;
        }
    */ 
     
    /* Initialize running total of bytes already parsed */
    num_bytes_parsed = 0;
  
    /* First element in NV item must be the number of type IDs it contains */
    nv_element_size = sizeof( variant_marker );
  
    /* Ensure there is enough memory left before actually extracting the element */
    if( (rev3_uncompress_size - num_bytes_parsed) < nv_element_size )
    {
      RF_MSG( RF_ERROR, "MultiLin V3 - Not enough elements present to extract "
                "first item NUM_TYPE_ID." );
      api_status = FALSE;
    }
  }

  if( api_status == TRUE )
  {
    /* EXTRACT ELEMENT: Number of type IDs present */
    temp_data_ptr = nv_data_rev3_ptr + num_bytes_parsed;
    memscpy( (void*) &variant_marker, nv_element_size,(void*)temp_data_ptr, nv_element_size );
    /* Update running total of bytes parsed with num bytes from last element extracted */
    num_bytes_parsed += nv_element_size;
    /* Ensure there is enough memory left before actually extracting the element */
    if( variant_marker.num_elements == 0 )
    {
      RF_MSG( RF_ERROR, "MultiLin V3 - Number of elements value of 0 "
              "is invalid.");
      api_status = FALSE;
    }
  }

  if( api_status == TRUE )
  {  
    num_ept_dpd_v2_saved = 0;
    num_et_dpd_saved = 0;
    for( type_id_idx = 0; (type_id_idx < variant_marker.num_elements) && (api_status == TRUE); type_id_idx++ )
    {
      /* Next element in NV item must be the type ID of the subsequent element */
      nv_element_size = sizeof( type_id );
  
      /* Ensure there is enough memory left before actually extracting the element */
      if( (rev3_uncompress_size - num_bytes_parsed) < nv_element_size )
      {
        RF_MSG_1( RF_ERROR, "MultiLin V3 - Not enough elements left to extract "
                  "item named TYPE_ID for type index %d", type_id_idx );
        api_status = FALSE;
        break;
      }
  
      /* EXTRACT ELEMENT: Type ID */
      temp_data_ptr = nv_data_rev3_ptr + num_bytes_parsed;
      memscpy( (void*) &type_id, nv_element_size,(void*)temp_data_ptr, nv_element_size );
      /* Update running total of bytes parsed with num bytes from last element extracted */
      num_bytes_parsed += nv_element_size;
  
      /* EXTRACT ELEMENT: Type corresponding to the type ID just extracted. */
  
      temp_data_ptr = nv_data_rev3_ptr + num_bytes_parsed;
      temp_num_bytes_left = rev3_uncompress_size - num_bytes_parsed;
      if( temp_num_bytes_left <= 0 )
      {
        RF_MSG_2( RF_ERROR, "MultiLin V3 - Not elements left to extract "
                  "item of TYPE_ID for type index %d", type_id, type_id_idx );
        api_status = FALSE;
        break;
      }

      switch( type_id )
      {
        case RFNV_DATA_TYPE_ID_CAL_CHANS:
          api_status &= rfcommon_core_tx_multilin_extract_tx_cal_chans( 
                                            temp_data_ptr,
                                            temp_num_bytes_left,
                                            convert_freq_handler,
                                            &nv_element_size,
                                            tx_cal_data );  
        break;

        case RFNV_DATA_TYPE_ID_FREQ_OFFSETS:
          api_status &= rfcommon_core_tx_multilin_extract_freq_offsets( 
                                            temp_data_ptr,
                                            temp_num_bytes_left,
                                            &nv_element_size,
                                            tx_cal_data );  
        break;
  
        case RFNV_DATA_TYPE_ID_LIN_TABLE_V3:
           api_status &= rfcommon_core_tx_multilin_extract_lin_v3( 
                                            temp_data_ptr,
                                            temp_num_bytes_left,
                                            convert_freq_handler,
                                            &nv_element_size,
                                            tx_cal_data,
                                            &ept_dpd_v2.num_dpd_avail,
                                            &et_dpd.num_dpd_avail );
        break;

        case RFNV_DATA_TYPE_ID_EPT_DPD_V2:
           api_status &= rfcommon_core_tx_multilin_extract_ept_dpd_v2( 
                                            temp_data_ptr,
                                            temp_num_bytes_left,
                                            &nv_element_size,
                                            tx_cal_data,
                                            &ept_dpd_v2,
                                            &num_ept_dpd_v2_saved );
        break;
  
        case RFNV_DATA_TYPE_ID_ET_DPD:
           api_status &= rfcommon_core_tx_multilin_extract_et_dpd( 
                                            temp_data_ptr,
                                            temp_num_bytes_left,
                                            &nv_element_size,
                                            tx_cal_data,
                                            &et_dpd,
                                            &num_et_dpd_saved );
        break;

        case RFNV_DATA_TYPE_REF_LIN_TEMP_ADC:
            api_status &=  rfcommon_core_tx_multilin_extract_ref_temp_adc(
                                            temp_data_ptr,
                                            temp_num_bytes_left,
                                            &nv_element_size,
                                            tx_cal_data);
        break;

        case RFNV_DATA_TYPE_ID_LIN_TABLE_V4:
        api_status &= rfcommon_core_tx_multilin_extract_lin_v4( 
                                            temp_data_ptr,
                                            temp_num_bytes_left,
                                            convert_freq_handler,
                                            &nv_element_size,
                                            tx_cal_data,
                                            &ept_dpd_v2.num_dpd_avail,
                                            &et_dpd.num_dpd_avail );
        break ;

        default:
          {
            RF_MSG_2( RF_ERROR, "MultiLin V3 - element of type ID %d not supported "
                      "for type index %d", type_id, type_id_idx);
            api_status = FALSE;
            break;
          }
        break;
      } /* switch( type_id ) */
  
      if( api_status == TRUE )
      {
        /* Update running total of bytes parsed with num bytes from last element extracted */
        num_bytes_parsed += nv_element_size;
      }
      else
      {
        RF_MSG_2( RF_ERROR, "MultiLin V3 - Error extracting "
                  "item of TYPE_ID %d for type index %d", type_id, type_id_idx );
        api_status = FALSE;
        break;/* DO NOT CONTINUE as memory may not be aligned any more for proper extraction */
      }

      /* It is a requirement that the first TYPE_ID populate the channel information
         since subsequent APIs depend on this information for populating channel-depending info */
      if( ( tx_cal_data->tx_cal_chan_size == 0 ) ||
          ( tx_cal_data->tx_cal_chan_size > RFCOM_TXLIN_NUM_CHANNELS ) )
      {
        api_status = FALSE;
        break;/* DO NOT CONTINUE as channel information is not present as required */
      }
    } /* for( type_id_idx ) */
  } /* if( api_status == TRUE ) */

  /* Post-checking/processing */
  if( api_status == TRUE )
  {
    api_status &= rfcommon_core_linearizer_load_post_processing( tx_cal_data );

    /* Ensure number EPT DPDs saved is what was announced in linearizers */
    if(num_ept_dpd_v2_saved != ept_dpd_v2.num_dpd_avail )
    {
      RF_MSG_2( RF_ERROR, "MultiLin V3 - EPT Num saved DPDs (%d) != num DPDs available (%d)", 
                num_ept_dpd_v2_saved, ept_dpd_v2.num_dpd_avail );
      api_status = FALSE;
    }
 
    /* Ensure number ET DPDs saved is what was announced in linearizers */
    if(num_et_dpd_saved != et_dpd.num_dpd_avail )
    {
      RF_MSG_2( RF_ERROR, "MultiLin V3 - ET Num saved DPDs (%d) != num DPDs available (%d)", 
                num_et_dpd_saved, et_dpd.num_dpd_avail );
      api_status = FALSE;
    }

    if (api_status == TRUE)
    {
      api_status &= rfcommon_core_linearizer_load_post_process_xpt( tx_cal_data, &ept_dpd_v2, &et_dpd );
      /* TODO: Come up with a common solution */
      /* api_status &= rfcommon_core_linearizer_load_post_process_dpd_type( tx_cal_data ); */
    }
  } /* if( api_status == TRUE ) */

  if( api_status == TRUE )
  {
    /* Set lin rev to valid rev only if all went well */
    tx_cal_data->nv_rev_type = TX_MULTI_LIN_REV_3;
  }

  /* Cleanup */

  if( nv_data_rev3_ptr != NULL )
  {
    modem_mem_free( nv_data_rev3_ptr, MODEM_MEM_CLIENT_RFA );
  }

  if( nv_data_raw_ptr != NULL )
  {
    modem_mem_free( nv_data_raw_ptr, MODEM_MEM_CLIENT_RFA );
  }

  if(ept_dpd_v2.am_dpds != NULL)
  {
    modem_mem_free( ept_dpd_v2.am_dpds, MODEM_MEM_CLIENT_RFA );
    ept_dpd_v2.am_dpds = NULL;
  }
  if(ept_dpd_v2.pm_dpds != NULL)
  {
    modem_mem_free( ept_dpd_v2.pm_dpds, MODEM_MEM_CLIENT_RFA );
    ept_dpd_v2.pm_dpds = NULL;
  }

  if(et_dpd.am_dpds != NULL)
  {
    modem_mem_free( et_dpd.am_dpds, MODEM_MEM_CLIENT_RFA );
    et_dpd.am_dpds = NULL;
  }
  if(et_dpd.pm_dpds != NULL)
  {
    modem_mem_free( et_dpd.pm_dpds, MODEM_MEM_CLIENT_RFA );
    et_dpd.pm_dpds = NULL;
  }
  if(et_dpd.dt_dpds != NULL)
  {
    modem_mem_free( et_dpd.dt_dpds, MODEM_MEM_CLIENT_RFA );
    et_dpd.dt_dpds = NULL;
  }

  return api_status;
} /* rfcommon_core_load_linearizer_v3_from_nv */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for extracting tx cal channels data
 
  @param nv_data_ptr
  Pointer to data to be extracted

  @param num_bytes_available
  The number of nv data bytes available 

  @param convert_freq_handler
  CB handler to convert channel to frequency
 
  @param extracted_element_size
  Output. The number of bypes extracted for the element

  @param tx_cal_data
  Output. Destination for the extracted data.
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/
boolean
rfcommon_core_tx_multilin_extract_tx_cal_chans
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  rfcommon_core_convert_freq_handler_type const convert_freq_handler,
  uint32 * const extracted_element_size,
  tx_band_cal_type *tx_cal_data
)
{
  /* Overall API status */
  boolean api_status = TRUE;
   /* The item to extract from NV */
  tx_cal_chan_nv_type_u32 tx_cal_chans;
  /* Contains the number of bytes for the element in NV */
  uint32 num_bytes_needed;
  /* Contains previous frequency for validation purposes */
  uint32 previous_freq = 0;
  /* For indexing */
  uint8 chan_idx;
  uint32 chan;

  /* Initialize to zero here in case checks below fail */
  *extracted_element_size = 0;

  /* The number of bytes needed to extract element */
  num_bytes_needed = sizeof( tx_cal_chans );

  /* Ensure there is enough memory left before actually extracting the element */
  if( num_bytes_available < num_bytes_needed )
  {
    RF_MSG( RF_ERROR, "MultiLin V3 - Not enough elements left to extract item: tx cal chans" );
    api_status = FALSE;
  }

  if( api_status == TRUE )
  {
    /* Extract the element */
    memscpy( (void*) &tx_cal_chans, num_bytes_needed,nv_data_ptr, num_bytes_needed );
    *extracted_element_size = num_bytes_needed;
    
    /* Process channel information */
    for( chan_idx = 0; chan_idx < RFCOM_TXLIN_NUM_CHANNELS; chan_idx++ )
    {
      chan = tx_cal_chans.tx_cal_chan[ chan_idx ];
      /* STOP if channel is invalid */
      if( chan == -1 )
      {
        /* If this is the first channel and it is invalid */
        if( chan_idx == 0)
        {
          RF_MSG( RF_ERROR, "MultiLin V3 - Channel list detects the first channel "
                  "as invalid, which is not allowed." );
          api_status = FALSE;
        }
        break;
      }

      tx_cal_data->tx_cal_chan[ chan_idx ] = chan;
      tx_cal_data->tx_cal_freq[ chan_idx ] = 
                       convert_freq_handler( chan );
      /* If new freq is <= to previous freq : error */
      /* Must be listed in increasing order only */
      if( tx_cal_data->tx_cal_freq[ chan_idx ] <= previous_freq )
      {
        RF_MSG( RF_ERROR, "MultiLin V3 - Channel frequencies not listed in ascending order." );
        api_status = FALSE;
        break;
      }
      previous_freq = tx_cal_data->tx_cal_freq[ chan_idx ];
    } /* for( chan_idx ) */
    /* Update num chans available */
    tx_cal_data->tx_cal_chan_size = chan_idx;
  } /* if( api_status == TRUE ) */

  return api_status;
} /* rfcommon_core_tx_multilin_extract_tx_cal_chans */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for extracting frequency offsets data
 
  @param nv_data_ptr
  Pointer to data to be extracted

  @param num_bytes_available
  The number of nv data bytes available 
 
  @param extracted_element_size
  Output. The number of bypes extracted for the element

  @param tx_cal_data
  Output. Destination for the extracted data.
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/
boolean
rfcommon_core_tx_multilin_extract_freq_offsets
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  tx_band_cal_type *tx_cal_data
)
{
  /* Overall API status */
  boolean api_status = TRUE;
   /* The item to extract from NV */
  tx_freq_offset_nv_type tx_freq_offsets;
  /* Contains the number of bytes for the element in NV */
  uint32 num_bytes_needed;
  /* For indexing */
  uint16 chan_idx;

  num_bytes_needed = sizeof( tx_freq_offsets );

  /* Initialize to zero here in case checks below fail */
  *extracted_element_size = 0;

  /* Ensure there is enough memory left before actually extracting the element */
  if( num_bytes_available < num_bytes_needed )
  {
    RF_MSG( RF_ERROR, "MultiLin V3 - Not enough elements left to extract item: freq offsets" );
    api_status = FALSE;
  }

  if( api_status == TRUE )
  {
    /* Extract the element */
    memscpy( (void*) &tx_freq_offsets, num_bytes_needed, nv_data_ptr, num_bytes_needed );
    *extracted_element_size = num_bytes_needed;

    /* Validate parameters */

    if( tx_freq_offsets.pa_state >= RFCOM_TXLIN_NUM_LINEARIZERS )
    {
      RF_MSG_2( RF_ERROR, "MultiLin V3 - Param validation failed. PA state %d >= %d.",
                tx_freq_offsets.pa_state, RFCOM_TXLIN_NUM_LINEARIZERS );
      api_status = FALSE;
    }

    if( tx_freq_offsets.fcomp_sweep_type >= RFCOM_TXLIN_NUM_V2_FCOMP_SWEEPS )
    {
      RF_MSG_2( RF_ERROR, "MultiLin V3 - Param validation failed. Fcomp sweep type %d >= %d.",
                tx_freq_offsets.fcomp_sweep_type, RFCOM_TXLIN_NUM_V2_FCOMP_SWEEPS );
      api_status = FALSE;
    }

    if( tx_freq_offsets.fcomp_idx >= RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN )
    {
      RF_MSG_2( RF_ERROR, "MultiLin V3 - Param validation failed. Fcomp index %d must be < %d.",
                tx_freq_offsets.fcomp_idx, RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN );
      api_status = FALSE;
    }
  }/* if( api_status == TRUE ) */
    
  if( api_status == TRUE )
  {
    for( chan_idx = 0; chan_idx < tx_cal_data->tx_cal_chan_size; chan_idx++ )
    {
      tx_cal_data->tx_pa_state_cal_data[ tx_freq_offsets.pa_state ].
        tx_freq_offsets[ tx_freq_offsets.fcomp_sweep_type ][ tx_freq_offsets.fcomp_idx ].freq_comp[ chan_idx ] = 
          tx_freq_offsets.freq_offset[ chan_idx ];
    } /* for( chan_idx ) */
  } /* if( api_status == TRUE ) */

  return api_status;
} /* rfcommon_core_tx_multilin_extract_freq_offsets */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for extracting linearizer data
 
  @param nv_data_ptr
  Pointer to data to be extracted

  @param num_bytes_available
  The number of nv data bytes available 

  @param convert_freq_handler
  CB handler to convert channel to frequency
 
  @param extracted_element_size
  Output. The number of bypes extracted for the element

  @param tx_cal_data
  Output destination for the extracted data.
 
  @param num_ept_dpd_used
  OUTPUT. The number of EPT DPDs to be used by this linearizer
 
  @param num_et_dpd_used
  OUTPUT. The number of ET DPDs to be used by this linearizer
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/
boolean
rfcommon_core_tx_multilin_extract_lin_v3
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  rfcommon_core_convert_freq_handler_type const convert_freq_handler,
  uint32 * const extracted_element_size,
  tx_band_cal_type *tx_cal_data,
  uint16* num_ept_dpd_used,
  uint16* num_et_dpd_used
)
{
  /* Overall API status */
  boolean api_status = TRUE;
  /* The item to extract from NV */
  tx_linearizer_table_v3_nv_type tx_linearizer_table_v3;
  /* Contains the number of bytes for the element in NV */
  uint32 num_bytes_needed;
  /* Internal TX lin table*/
  tx_linearizer_table_type *lin_table_ptr = NULL;
  /* For indexing */
  uint16 row_idx;
  uint32 freq, upper_bound_freq;
  uint8 freq_bin_index = -1;
  boolean has_fcomps = TRUE;

  num_bytes_needed = sizeof( tx_linearizer_table_v3 );

  /* Initialize to zero here in case checks below fail */
  *extracted_element_size = 0;

  /* Ensure there is enough memory left before actually extracting the element */
  if( num_bytes_available < num_bytes_needed )
  {
    RF_MSG( RF_ERROR, "MultiLin V3 - Not enough elements left to extract item: linearizer" );
    api_status = FALSE;
  }

  if( api_status == TRUE )
  {
    /* Extract the element */
    memscpy( (void*) &tx_linearizer_table_v3, num_bytes_needed, nv_data_ptr, num_bytes_needed );
    *extracted_element_size = num_bytes_needed;

    /* Validate parameters */

    if( tx_linearizer_table_v3.pa_state >= RFCOM_TXLIN_NUM_LINEARIZERS )
    {
      RF_MSG_2( RF_ERROR, "MultiLin V3 - Param validation failed. Lin PA state %d >= %d.",
                tx_linearizer_table_v3.pa_state, RFCOM_TXLIN_NUM_LINEARIZERS );
      api_status = FALSE;
    }

    if( tx_linearizer_table_v3.wave_type >= TX_LINEARIZER_NUM_WAVEFORM )
    {
      RF_MSG_2( RF_ERROR, "MultiLin V3 - Param validation failed. Lin Wave type %d >= %d.",
                tx_linearizer_table_v3.wave_type, TX_LINEARIZER_NUM_WAVEFORM );
      api_status = FALSE;
    }

    if ( api_status == TRUE )
    {
      /* Find an empty linearizer reference freq bin to put this calibrated linearizer */
      for( freq_bin_index = 0; freq_bin_index < RFCOM_TXLIN_NUM_CHANNELS; freq_bin_index++ )
      {
        if( tx_cal_data->tx_pa_state_cal_data[ tx_linearizer_table_v3.pa_state ].
        linearizer_index[ tx_linearizer_table_v3.wave_type ][ freq_bin_index ].
          node_data.lin_data == NULL )
        {
          /* Found an empty spot to put this linearizer */
          break;
        }
      }

      if ( freq_bin_index == RFCOM_TXLIN_NUM_CHANNELS )
      {
        RF_MSG_1( RF_ERROR, "MultiLin V3 - Ref linearizers exceeded max allowed %d",
                  RFCOM_TXLIN_NUM_CHANNELS );
        api_status = FALSE;
      }
    }
    else
    {
      freq_bin_index = 0;
      RF_MSG( RF_ERROR, "MultiLin V3 - Param validation failed." );
      api_status = FALSE;
    }

    freq = convert_freq_handler( tx_linearizer_table_v3.tx_chan );
    upper_bound_freq = convert_freq_handler( tx_linearizer_table_v3.upper_bound_tx_chan );
    if( freq > upper_bound_freq )
    {
      RF_MSG_4( RF_ERROR, "MultiLin V3 - Param validation failed. "
                "Lin freq %d > %d upper-bound freq. Respective channels: ", 
                freq, upper_bound_freq, 
                tx_linearizer_table_v3.tx_chan, tx_linearizer_table_v3.upper_bound_tx_chan );
      api_status = FALSE;
    }

    if( ( tx_linearizer_table_v3.xpt_dpd_type == TX_LINEARIZER_EPT_DPD_AUX_DATA ) ||
        ( tx_linearizer_table_v3.xpt_dpd_type >= TX_LINEARIZER_DPD_INVALID ) )
    {
      RF_MSG_3( RF_ERROR, "MultiLin V3 - Param validation failed. Lin DPD type %d cannot "
                "be %d or >= %d.", tx_linearizer_table_v3.xpt_dpd_type, 
                TX_LINEARIZER_EPT_DPD_AUX_DATA, TX_LINEARIZER_DPD_INVALID );
      api_status = FALSE;
    }

    if( ( tx_linearizer_table_v3.xpt_dpd_type != TX_LINEARIZER_NO_AUX_DATA ) &&
        ( ( tx_linearizer_table_v3.num_dpd == 0) || 
          ( tx_linearizer_table_v3.num_dpd > RFCOM_TXLIN_NUM_SEGMENTS) ) )
    {
      RF_MSG_2( RF_ERROR, "MultiLin V3 - Param validation failed. "
                "Lin NUM DPD %d. Cannot be 0 or > %d when using DPD data.",
                tx_linearizer_table_v3.num_dpd, RFCOM_TXLIN_NUM_SEGMENTS );
      api_status = FALSE;
    }

    if( ( tx_linearizer_table_v3.xpt_dpd_type == TX_LINEARIZER_NO_AUX_DATA ) &&
        ( tx_linearizer_table_v3.num_dpd != 0 ) )
    {
      RF_MSG_1( RF_ERROR, "MultiLin V3 - Param validation failed. "
                "Lin NUM DPD %d. It must be 0 when not using DPD data.",
                tx_linearizer_table_v3.num_dpd );
      api_status = FALSE;
    }

    if( tx_linearizer_table_v3.num_fcomp > RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN )
    {
      RF_MSG_2( RF_ERROR, "MultiLin V3 - Param validation error. "
                "Lin NUM Fcomp %d must be <= %d.",
                tx_linearizer_table_v3.num_fcomp, RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN );
      api_status = FALSE;
    }

    if( tx_linearizer_table_v3.num_fcomp == 0 )
    {
      RF_MSG_4( RF_ERROR, "MultiLin V3 - Param validation potential failure. "
                "Lin NUM Fcomp %d is not > 0 and <= %d. This may be acceptable only "
                "if PA %d has a lin of wave type other than %d containing fcomp info. "
                "Only if such condition is met this can be considered a warning.",
                tx_linearizer_table_v3.num_fcomp, RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN,
                tx_linearizer_table_v3.pa_state, tx_linearizer_table_v3.wave_type );
      has_fcomps = FALSE;
    }

    if( tx_linearizer_table_v3.fcomp_type >= RFCOM_TXLIN_NUM_V2_FCOMP_SWEEPS )
    {
      RF_MSG_2( RF_ERROR, "MultiLin V3 - Param validation error. "
                "Lin Fcomp Type %d must be < %d.",
                tx_linearizer_table_v3.fcomp_type, RFCOM_TXLIN_NUM_V2_FCOMP_SWEEPS );
      api_status = FALSE;
    }
  } /* if( api_status == TRUE ) */

  /* Populate destination with nv data */
  if( api_status == TRUE )
  {
    lin_table_ptr = (tx_linearizer_table_type*)modem_mem_alloc( sizeof(tx_linearizer_table_type),
                                                                          MODEM_MEM_CLIENT_RFA );
    if( lin_table_ptr == NULL )
    {
      RF_MSG( RF_ERROR, "MultiLin V3 - mem alloc failed for linearizer element." );
      api_status = FALSE;
    }
  }
  
  if( api_status == TRUE )
  {
    tx_cal_data->tx_pa_state_cal_data[ tx_linearizer_table_v3.pa_state ].
      linearizer_index[ tx_linearizer_table_v3.wave_type ][ freq_bin_index ].
        upper_bound_freq = convert_freq_handler( tx_linearizer_table_v3.upper_bound_tx_chan );

    for( row_idx = 0; row_idx < RFCOM_TXLIN_NUM_SEGMENTS; row_idx++ )
    {
      lin_table_ptr->table[ row_idx ].rgi         = tx_linearizer_table_v3.rgi[ row_idx ];
      lin_table_ptr->table[ row_idx ].power       = tx_linearizer_table_v3.power[ row_idx ];
      lin_table_ptr->table[ row_idx ].apt         = tx_linearizer_table_v3.apt[ row_idx ];
      lin_table_ptr->table[ row_idx ].
                       apt_pa_struct.pa_q_current = tx_linearizer_table_v3.pa_current[ row_idx ];

      if( (tx_linearizer_table_v3.iq_offset[ row_idx ]) >= 
          (1 << (8 * sizeof(lin_table_ptr->table[ row_idx ].iq_offset) ) ) )
      {
        RF_MSG_2( RF_ERROR, "MultiLin V3 - Lin IQ offset value of %d does not fit in destination of size % bytes.",
                  tx_linearizer_table_v3.iq_offset[ row_idx ], sizeof(lin_table_ptr->table[ row_idx ].iq_offset) );
        api_status = FALSE;
      }
      lin_table_ptr->table[ row_idx ].iq_offset   = (uint8)tx_linearizer_table_v3.iq_offset[ row_idx ];
      lin_table_ptr->table[ row_idx ].dpd_index   = tx_linearizer_table_v3.dpd_idx[ row_idx ];
      lin_table_ptr->table[ row_idx ].fcomp_idx   = tx_linearizer_table_v3.fcomp_idx[ row_idx ];

      /* Initialize RSB and LOFT corrections with invalid values */
      lin_table_ptr->table[ row_idx ].rsb_correction_coeff.alpha = 
        lin_table_ptr->table[ row_idx ].rsb_correction_coeff.beta =
          RFCOM_TXLIN_INVALID_RSB_VAL ;
      lin_table_ptr->table[ row_idx ].loft_correction = RFCOM_TXLIN_INVALID_LOFT_VAL ;


      if ( ( has_fcomps == FALSE ) &&
           ( lin_table_ptr->table[ row_idx ].fcomp_idx < RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN ) )
      {
        RF_MSG_3( RF_ERROR, "MultiLin V3 - Param validation error. Lin has no fcomps. Row idx %d "
                  "fcomp idx of %d cannot be < %d",
                  row_idx, lin_table_ptr->table[ row_idx ].fcomp_idx, 
                  RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN );
        api_status = FALSE;
      }
    }

    lin_table_ptr->chan_freq  = convert_freq_handler( tx_linearizer_table_v3.tx_chan );
    lin_table_ptr->dpd_type   = tx_linearizer_table_v3.xpt_dpd_type;
    lin_table_ptr->num_dpd    = tx_linearizer_table_v3.num_dpd;
    lin_table_ptr->num_fcomp  = tx_linearizer_table_v3.num_fcomp;
    lin_table_ptr->fcomp_type = tx_linearizer_table_v3.fcomp_type;

    tx_cal_data->tx_pa_state_cal_data[ tx_linearizer_table_v3.pa_state ].
      linearizer_index[ tx_linearizer_table_v3.wave_type ][ freq_bin_index ].
        node_data.lin_data = lin_table_ptr;

    tx_cal_data->tx_pa_state_cal_data[ tx_linearizer_table_v3.pa_state ].
      linearizer_index[ tx_linearizer_table_v3.wave_type ][ freq_bin_index ].
        node_data.aux_lin_data = tx_linearizer_table_v3.xpt_dpd_type;
    switch (tx_linearizer_table_v3.xpt_dpd_type)
    {
     case TX_LINEARIZER_NO_AUX_DATA:
       // DON'T ADD ANYTHING AS NO DPDs are to be used.
     break;
     case TX_LINEARIZER_EPT_DPD_V2_DATA:
       *num_ept_dpd_used += tx_linearizer_table_v3.num_dpd;
     break;
     case TX_LINEARIZER_ET_DPD_DATA:
       *num_et_dpd_used += tx_linearizer_table_v3.num_dpd;
     break;
     default:
       RF_MSG_1( RF_ERROR, "MultiLin V3 - Lin DPD Type % not supported.",
                 tx_linearizer_table_v3.xpt_dpd_type );
      api_status = FALSE;
     break;
    }
  } /* if( api_status == TRUE ) */

  return api_status;
} /* rfcommon_core_tx_multilin_extract_lin_v3 */


/* These a conversion functions for obtaining RSB corrections stored as a 
   32 bit number but appllied as two 15 bit entities.   
*/
/* DTR_TXRFI_DPRDB_IQMC_CHAIN0/1 = [29:15 Beta | 14:0 AlphaMinus1]. */
uint16 ALPHA_CONVERSION (uint32 x) { return ( (uint16)(0x7FFF & (x) )) ; } 
uint16 BETA_CONVERSION (uint32 x)  { return ( (uint16)((0x3FFF8000 & x)>>15) ) ; } 
uint32 REVERSE_CONVERSION (uint16 alpha , uint16 beta ) 
{ return  ( (uint32) ( (((uint32)(beta)) << 15) | alpha ) ) ; }
/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for extracting linearizer data with RSB and LOFT
  corrections
 
  @param nv_data_ptr
  Pointer to data to be extracted

  @param num_bytes_available
  The number of nv data bytes available 

  @param convert_freq_handler
  CB handler to convert channel to frequency
 
  @param extracted_element_size
  Output. The number of bypes extracted for the element

  @param tx_cal_data
  Output destination for the extracted data.
 
  @param num_ept_dpd_used
  OUTPUT. The number of EPT DPDs to be used by this linearizer
 
  @param num_et_dpd_used
  OUTPUT. The number of ET DPDs to be used by this linearizer
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/
boolean
rfcommon_core_tx_multilin_extract_lin_v4
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  rfcommon_core_convert_freq_handler_type const convert_freq_handler,
  uint32 * const extracted_element_size,
  tx_band_cal_type *tx_cal_data,
  uint16* num_ept_dpd_used,
  uint16* num_et_dpd_used
)
{
  /* Overall API status */
  boolean api_status = TRUE;
  /* The item to extract from NV */
  tx_linearizer_table_v4_nv_type tx_linearizer_table_v4;
  /* Contains the number of bytes for the element in NV */
  uint32 num_bytes_needed;
  /* Internal TX lin table*/
  tx_linearizer_table_type *lin_table_ptr = NULL;
  /* For indexing */
  uint16 row_idx;
  uint32 freq, upper_bound_freq;
  uint8 freq_bin_index = -1;
  boolean has_fcomps = TRUE;

  num_bytes_needed = sizeof( tx_linearizer_table_v4 );

  /* Initialize to zero here in case checks below fail */
  *extracted_element_size = 0;

  /* Ensure there is enough memory left before actually extracting the element */
  if( num_bytes_available < num_bytes_needed )
  {
    RF_MSG( RF_ERROR, "MultiLin V3 RSB- Not enough elements left to extract item: linearizer" );
    api_status = FALSE;
  }

  if( api_status == TRUE )
  {
    /* Extract the element */
    memscpy( (void*) &tx_linearizer_table_v4, num_bytes_needed, nv_data_ptr, num_bytes_needed );
    *extracted_element_size = num_bytes_needed;

    /* Validate parameters */

    if( tx_linearizer_table_v4.pa_state >= RFCOM_TXLIN_NUM_LINEARIZERS )
    {
      RF_MSG_2( RF_ERROR, "MultiLin V3 RSB- Param validation failed. Lin PA state %d >= %d.",
                tx_linearizer_table_v4.pa_state, RFCOM_TXLIN_NUM_LINEARIZERS );
      api_status = FALSE;
    }

    if( tx_linearizer_table_v4.wave_type >= TX_LINEARIZER_NUM_WAVEFORM )
    {
      RF_MSG_2( RF_ERROR, "MultiLin V3 RSB- Param validation failed. Lin Wave type %d >= %d.",
                tx_linearizer_table_v4.wave_type, TX_LINEARIZER_NUM_WAVEFORM );
      api_status = FALSE;
    }

    if ( api_status == TRUE )
    {
      /* Find an empty linearizer reference freq bin to put this calibrated linearizer */
      for( freq_bin_index = 0; freq_bin_index < RFCOM_TXLIN_NUM_CHANNELS; freq_bin_index++ )
      {
        if( tx_cal_data->tx_pa_state_cal_data[ tx_linearizer_table_v4.pa_state ].
        linearizer_index[ tx_linearizer_table_v4.wave_type ][ freq_bin_index ].
          node_data.lin_data == NULL )
        {
          /* Found an empty spot to put this linearizer */
          break;
        }
      }

      if ( freq_bin_index == RFCOM_TXLIN_NUM_CHANNELS )
      {
        RF_MSG_1( RF_ERROR, "MultiLin V3 RSB- Ref linearizers exceeded max allowed %d",
                  RFCOM_TXLIN_NUM_CHANNELS );
        api_status = FALSE;
      }
    }
    else
    {
      freq_bin_index = 0;
      RF_MSG( RF_ERROR, "MultiLin V3 RSB- Param validation failed." );
      api_status = FALSE;
    }

    freq = convert_freq_handler( tx_linearizer_table_v4.tx_chan );
    upper_bound_freq = convert_freq_handler( tx_linearizer_table_v4.upper_bound_tx_chan );
    if( freq > upper_bound_freq )
    {
      RF_MSG_4( RF_ERROR, "MultiLin V3 RSB- Param validation failed. "
                "Lin freq %d > %d upper-bound freq. Respective channels: ", 
                freq, upper_bound_freq, 
                tx_linearizer_table_v4.tx_chan, tx_linearizer_table_v4.upper_bound_tx_chan );
      api_status = FALSE;
    }

    if( ( tx_linearizer_table_v4.xpt_dpd_type == TX_LINEARIZER_EPT_DPD_AUX_DATA ) ||
        ( tx_linearizer_table_v4.xpt_dpd_type >= TX_LINEARIZER_DPD_INVALID ) )
    {
      RF_MSG_3( RF_ERROR, "MultiLin V3 RSB- Param validation failed. Lin DPD type %d cannot "
                "be %d or >= %d.", tx_linearizer_table_v4.xpt_dpd_type, 
                TX_LINEARIZER_EPT_DPD_AUX_DATA, TX_LINEARIZER_DPD_INVALID );
      api_status = FALSE;
    }

    if( ( tx_linearizer_table_v4.xpt_dpd_type != TX_LINEARIZER_NO_AUX_DATA ) &&
        ( ( tx_linearizer_table_v4.num_dpd == 0) || 
          ( tx_linearizer_table_v4.num_dpd > RFCOM_TXLIN_NUM_SEGMENTS) ) )
    {
      RF_MSG_2( RF_ERROR, "MultiLin V3 RSB- Param validation failed. "
                "Lin NUM DPD %d. Cannot be 0 or > %d when using DPD data.",
                tx_linearizer_table_v4.num_dpd, RFCOM_TXLIN_NUM_SEGMENTS );
      api_status = FALSE;
    }

    if( ( tx_linearizer_table_v4.xpt_dpd_type == TX_LINEARIZER_NO_AUX_DATA ) &&
        ( tx_linearizer_table_v4.num_dpd != 0 ) )
    {
      RF_MSG_1( RF_ERROR, "MultiLin V3 RSB- Param validation failed. "
                "Lin NUM DPD %d. It must be 0 when not using DPD data.",
                tx_linearizer_table_v4.num_dpd );
      api_status = FALSE;
    }

    if( tx_linearizer_table_v4.num_fcomp > RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN )
    {
      RF_MSG_2( RF_ERROR, "MultiLin V3 RSB- Param validation error. "
                "Lin NUM Fcomp %d must be <= %d.",
                tx_linearizer_table_v4.num_fcomp, RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN );
      api_status = FALSE;
    }

    if( tx_linearizer_table_v4.num_fcomp == 0 )
    {
      RF_MSG_4( RF_ERROR, "MultiLin V3 RSB- Param validation potential failure. "
                "Lin NUM Fcomp %d is not > 0 and <= %d. This may be acceptable only "
                "if PA %d has a lin of wave type other than %d containing fcomp info. "
                "Only if such condition is met this can be considered a warning.",
                tx_linearizer_table_v4.num_fcomp, RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN,
                tx_linearizer_table_v4.pa_state, tx_linearizer_table_v4.wave_type );
      has_fcomps = FALSE;
    }

    if( tx_linearizer_table_v4.fcomp_type >= RFCOM_TXLIN_NUM_V2_FCOMP_SWEEPS )
    {
      RF_MSG_2( RF_ERROR, "MultiLin V3 RSB- Param validation error. "
                "Lin Fcomp Type %d must be < %d.",
                tx_linearizer_table_v4.fcomp_type, RFCOM_TXLIN_NUM_V2_FCOMP_SWEEPS );
      api_status = FALSE;
    }
  } /* if( api_status == TRUE ) */

  /* Populate destination with nv data */
  if( api_status == TRUE )
  {
    lin_table_ptr = (tx_linearizer_table_type*)modem_mem_alloc( sizeof(tx_linearizer_table_type),
                                                                          MODEM_MEM_CLIENT_RFA );
    if( lin_table_ptr == NULL )
    {
      RF_MSG( RF_ERROR, "MultiLin V3 RSB- mem alloc failed for linearizer element." );
      api_status = FALSE;
    }
  }
  
  if( api_status == TRUE )
  {
    tx_cal_data->tx_pa_state_cal_data[ tx_linearizer_table_v4.pa_state ].
      linearizer_index[ tx_linearizer_table_v4.wave_type ][ freq_bin_index ].
        upper_bound_freq = convert_freq_handler( tx_linearizer_table_v4.upper_bound_tx_chan );

    for( row_idx = 0; row_idx < RFCOM_TXLIN_NUM_SEGMENTS; row_idx++ )
    {
      uint32 rsb_correction                       = tx_linearizer_table_v4.tx_rsb_correction[ row_idx ] ;
      lin_table_ptr->table[ row_idx ].rgi         = tx_linearizer_table_v4.rgi[ row_idx ];
      lin_table_ptr->table[ row_idx ].power       = tx_linearizer_table_v4.power[ row_idx ];
      lin_table_ptr->table[ row_idx ].apt         = tx_linearizer_table_v4.apt[ row_idx ];
      lin_table_ptr->table[ row_idx ].
                       apt_pa_struct.pa_q_current = tx_linearizer_table_v4.pa_current[ row_idx ];
      lin_table_ptr->table[ row_idx ].rsb_correction_coeff.alpha = 
                                    ALPHA_CONVERSION(rsb_correction) ;
      lin_table_ptr->table[ row_idx ].rsb_correction_coeff.beta = 
                                    BETA_CONVERSION(rsb_correction) ;
      lin_table_ptr->table[ row_idx ].loft_correction = 
        tx_linearizer_table_v4.loft_correction[ row_idx ]  ;

      if( (tx_linearizer_table_v4.iq_offset[ row_idx ]) >= 
          (1 << (8 * sizeof(lin_table_ptr->table[ row_idx ].iq_offset) ) ) )
      {
        RF_MSG_2( RF_ERROR, "MultiLin V3 RSB- Lin IQ offset value of %d does not fit in destination of size % bytes.",
                  tx_linearizer_table_v4.iq_offset[ row_idx ], sizeof(lin_table_ptr->table[ row_idx ].iq_offset) );
        api_status = FALSE;
      }
      lin_table_ptr->table[ row_idx ].iq_offset   = (uint8)tx_linearizer_table_v4.iq_offset[ row_idx ];
      lin_table_ptr->table[ row_idx ].dpd_index   = tx_linearizer_table_v4.dpd_idx[ row_idx ];
      lin_table_ptr->table[ row_idx ].fcomp_idx   = tx_linearizer_table_v4.fcomp_idx[ row_idx ];


      if ( ( has_fcomps == FALSE ) &&
           ( lin_table_ptr->table[ row_idx ].fcomp_idx < RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN ) )
      {
        RF_MSG_3( RF_ERROR, "MultiLin V3 RSB- Param validation error. Lin has no fcomps. Row idx %d "
                  "fcomp idx of %d cannot be < %d",
                  row_idx, lin_table_ptr->table[ row_idx ].fcomp_idx, 
                  RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN );
        api_status = FALSE;
      }


    }

    lin_table_ptr->chan_freq  = convert_freq_handler( tx_linearizer_table_v4.tx_chan );
    lin_table_ptr->dpd_type   = tx_linearizer_table_v4.xpt_dpd_type;
    lin_table_ptr->num_dpd    = tx_linearizer_table_v4.num_dpd;
    lin_table_ptr->num_fcomp  = tx_linearizer_table_v4.num_fcomp;
    lin_table_ptr->fcomp_type = tx_linearizer_table_v4.fcomp_type;

    tx_cal_data->tx_pa_state_cal_data[ tx_linearizer_table_v4.pa_state ].
      linearizer_index[ tx_linearizer_table_v4.wave_type ][ freq_bin_index ].
        node_data.lin_data = lin_table_ptr;

    tx_cal_data->tx_pa_state_cal_data[ tx_linearizer_table_v4.pa_state ].
      linearizer_index[ tx_linearizer_table_v4.wave_type ][ freq_bin_index ].
        node_data.aux_lin_data = tx_linearizer_table_v4.xpt_dpd_type;
    switch (tx_linearizer_table_v4.xpt_dpd_type)
    {
     case TX_LINEARIZER_NO_AUX_DATA:
       // DON'T ADD ANYTHING AS NO DPDs are to be used.
     break;
     case TX_LINEARIZER_EPT_DPD_V2_DATA:
       *num_ept_dpd_used += tx_linearizer_table_v4.num_dpd;
     break;
     case TX_LINEARIZER_ET_DPD_DATA:
       *num_et_dpd_used += tx_linearizer_table_v4.num_dpd;
     break;
     default:
       RF_MSG_1( RF_ERROR, "MultiLin V3 RSB- Lin DPD Type % not supported.",
                 tx_linearizer_table_v4.xpt_dpd_type );
      api_status = FALSE;
     break;
    }
  } /* if( api_status == TRUE ) */

  return api_status;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for extracting EPT DPD V2 data
 
  @param nv_data_ptr
  Pointer to data to be extracted

  @param num_bytes_available
  The number of nv data bytes available 
 
  @param extracted_element_size
  Output. The number of bypes extracted for the element

  @param tx_cal_data
  Output destination for the extracted data.
 
  @param ept_dpd_v2
  Input and Output. Flat list of DPD items.
 
  @param num_ept_dpd_v2_saved
  Input and Output. Used to know how many DPDs have been
  saved as well as increment with the number DPDs saved in this API.
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/
boolean
rfcommon_core_tx_multilin_extract_ept_dpd_v2
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  tx_band_cal_type *tx_cal_data,
  ept_dpd_v2_data_type *ept_dpd_v2,
  uint16 *num_ept_dpd_v2_saved
)
{
  /* Overall API status */
  boolean api_status = TRUE;
  /* The item to extract from NV */
  tx_ept_dpd_v2_nv_type tx_ept_dpd_v2_record;
  /* Contains the number of bytes for the element in NV */
  uint32 num_bytes_needed;

  /* Internal table */
  xpt_am_row_type *xpt_am_data_ptr = NULL;
  xpt_pm_row_type *xpt_pm_data_ptr = NULL;
  /* For indexing */
  uint8 index;

  num_bytes_needed = sizeof( tx_ept_dpd_v2_record );

  /* Initialize to zero here in case checks below fail */
  *extracted_element_size = 0;

  /* Ensure there is enough memory left before actually extracting the element */
  if( num_bytes_available < num_bytes_needed )
  {
    RF_MSG( RF_ERROR, "MultiLin V3 - Not enough elements left to extract item: DPD EPT V2" );
    api_status = FALSE;
  }

  if( api_status == TRUE )
  {
    /* Extract the element */
    memscpy( (void*) &tx_ept_dpd_v2_record, num_bytes_needed, nv_data_ptr, num_bytes_needed );
    *extracted_element_size = num_bytes_needed;

    /* Validate parameters */

    if( tx_ept_dpd_v2_record.index >= ept_dpd_v2->num_dpd_avail )
    {
      RF_MSG_2( RF_ERROR, "MultiLin V3 - Param validation failed. DPD EPT index %d must be < %d.",
                tx_ept_dpd_v2_record.index, ept_dpd_v2->num_dpd_avail );
      api_status = FALSE;
    }

    if (*num_ept_dpd_v2_saved == 0)
    {
      if (ept_dpd_v2->am_dpds != NULL)
      {
        RF_MSG( RF_ERROR, "MultiLin V3 - Param validation failed. DPD EPT AM prt "
                "must be NULL before the first DPD is saved." );
        api_status = FALSE;
      }

      if (ept_dpd_v2->pm_dpds != NULL)
      {
        RF_MSG( RF_ERROR, "MultiLin V3 - Param validation failed. DPD EPT PM prt "
                "must be NULL before the first DPD is saved." );
        api_status = FALSE;
      }
    }
  } /* if( api_status == TRUE ) */

  /* Allocate mem if not already allocated */
  if( api_status == TRUE )
  {
    xpt_am_data_ptr = ept_dpd_v2->am_dpds;
    /* If XPT AM data not yet created... */
    if( xpt_am_data_ptr == NULL )
    {
      /* Create XPT AM Pointers table */
      xpt_am_data_ptr = 
        (xpt_am_row_type*)modem_mem_alloc( ept_dpd_v2->num_dpd_avail * sizeof(xpt_am_row_type),
                                           MODEM_MEM_CLIENT_RFA );
      if( xpt_am_data_ptr == NULL )
      {
        RF_MSG( RF_ERROR, "MultiLin V3 - mem alloc failed for XPT AM Data table." );
        api_status = FALSE;
      }
      else
      { 
        /* memory allocation went ok: assign memory */
        memset( xpt_am_data_ptr, 0x0, ept_dpd_v2->num_dpd_avail * sizeof(xpt_am_row_type) ); 
        ept_dpd_v2->am_dpds = xpt_am_data_ptr;
      } /* XPT Data != NULL */
    }

    xpt_pm_data_ptr = ept_dpd_v2->pm_dpds;
    /* If XPT PM data not yet created... */
    if( xpt_pm_data_ptr == NULL )
    {
      /* Create XPT PM Pointers table */
      xpt_pm_data_ptr = 
        (xpt_pm_row_type*)modem_mem_alloc( ept_dpd_v2->num_dpd_avail * sizeof(xpt_pm_row_type),
                                           MODEM_MEM_CLIENT_RFA );
      if( xpt_pm_data_ptr == NULL )
      {
        RF_MSG( RF_ERROR, "MultiLin V3 - mem alloc failed for XPT PM Data table." );
        api_status = FALSE;
      }
      else
      { 
        /* memory allocation went ok: assign memory */
        memset( xpt_pm_data_ptr, 0x0, ept_dpd_v2->num_dpd_avail * sizeof(xpt_pm_row_type) ); 
        ept_dpd_v2->pm_dpds = xpt_pm_data_ptr;
      } /* XPT Data != NULL */
    }
  } /* Allocate mem if not already allocated */
  
  /* Save XPT Data */
  if( api_status == TRUE )
  {
    /* Save xpt AM data */
    for( index = 0; index < XPT_DPD_NUM_COEFF; index++ )
    {
      xpt_am_data_ptr[tx_ept_dpd_v2_record.index].xpt_am[ index ] = tx_ept_dpd_v2_record.am[ index ];
      xpt_pm_data_ptr[tx_ept_dpd_v2_record.index].xpt_pm[ index ] = tx_ept_dpd_v2_record.pm[ index ];
    }
    *num_ept_dpd_v2_saved = *num_ept_dpd_v2_saved + 1;
  } /* if( api_status == TRUE ) */

  return api_status;
} /* rfcommon_core_tx_multilin_extract_ept_dpd_v2 */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for extracting ET DPD data
 
  @param nv_data_ptr
  Pointer to data to be extracted

  @param num_bytes_available
  The number of nv data bytes available 
 
  @param extracted_element_size
  Output. The number of bypes extracted for the element

  @param tx_cal_data
  Output destination for the extracted data.
 
  @param et_dpd
  Input and Output. Flat list of DPD items.
 
  @param num_et_dpd_saved
  Input and Output. Used to know how many DPDs have been
  saved as well as increment with the number DPDs saved in this API.
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/
boolean
rfcommon_core_tx_multilin_extract_et_dpd
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  tx_band_cal_type *tx_cal_data,
  et_dpd_data_type *et_dpd,
  uint16 *num_et_dpd_saved
)
{
  /* Overall API status */
  boolean api_status = TRUE;
  /* The item to extract from NV */
  tx_et_dpd_nv_type tx_et_dpd_record;
  /* Contains the number of bytes for the element in NV */
  uint32 num_bytes_needed;

  /* Internal table */
  xpt_am_row_type *xpt_am_data_ptr = NULL;
  xpt_pm_row_type *xpt_pm_data_ptr = NULL;
  xpt_dt_row_type *xpt_dt_data_ptr = NULL;
  /* For indexing */
  uint8 index;

  num_bytes_needed = sizeof( tx_et_dpd_record );

  /* Initialize to zero here in case checks below fail */
  *extracted_element_size = 0;

  /* Ensure there is enough memory left before actually extracting the element */
  if( num_bytes_available < num_bytes_needed )
  {
    RF_MSG( RF_ERROR, "MultiLin V3 - Not enough elements left to extract item: DPD ET" );
    api_status = FALSE;
  }

  if( api_status == TRUE )
  {
    /* Extract the element */
    memscpy( (void*) &tx_et_dpd_record, num_bytes_needed, nv_data_ptr, num_bytes_needed );
    *extracted_element_size = num_bytes_needed;

    /* Validate parameters */

    if( tx_et_dpd_record.index >= et_dpd->num_dpd_avail )
    {
      RF_MSG_2( RF_ERROR, "MultiLin V3 - Param validation failed. DPD ET index %d must be < %d.",
                tx_et_dpd_record.index, et_dpd->num_dpd_avail );
      api_status = FALSE;
    }

    if (*num_et_dpd_saved == 0)
    {
      if (et_dpd->am_dpds != NULL)
      {
        RF_MSG( RF_ERROR, "MultiLin V3 - Param validation failed. DPD ET AM prt "
                "must be NULL before the first DPD is saved." );
        api_status = FALSE;
      }

      if (et_dpd->pm_dpds != NULL)
      {
        RF_MSG( RF_ERROR, "MultiLin V3 - Param validation failed. DPD ET PM prt "
                "must be NULL before the first DPD is saved." );
        api_status = FALSE;
      }

      if (et_dpd->dt_dpds != NULL)
      {
        RF_MSG( RF_ERROR, "MultiLin V3 - Param validation failed. DPD ET DT prt "
                "must be NULL before the first DPD is saved." );
        api_status = FALSE;
      }
    }
  } /* if( api_status == TRUE ) */

  /* Allocate mem if not already allocated */
  if( api_status == TRUE )
  {
    xpt_am_data_ptr = et_dpd->am_dpds;
    /* If XPT AM data not yet created... */
    if( xpt_am_data_ptr == NULL )
    {
      /* Create XPT AM Pointers table */
      xpt_am_data_ptr = 
        (xpt_am_row_type*)modem_mem_alloc( et_dpd->num_dpd_avail * sizeof(xpt_am_row_type),
                                           MODEM_MEM_CLIENT_RFA );
      if( xpt_am_data_ptr == NULL )
      {
        RF_MSG( RF_ERROR, "MultiLin V3 - mem alloc failed for XPT AM Data table." );
        api_status = FALSE;
      }
      else
      { 
        /* memory allocation went ok: assign memory */
        memset( xpt_am_data_ptr, 0x0, et_dpd->num_dpd_avail * sizeof(xpt_am_row_type) ); 
        et_dpd->am_dpds = xpt_am_data_ptr;
      } /* XPT Data != NULL */
    }

    xpt_pm_data_ptr = et_dpd->pm_dpds;
    /* If XPT PM data not yet created... */
    if( xpt_pm_data_ptr == NULL )
    {
      /* Create XPT PM Pointers table */
      xpt_pm_data_ptr = 
        (xpt_pm_row_type*)modem_mem_alloc( et_dpd->num_dpd_avail * sizeof(xpt_pm_row_type),
                                           MODEM_MEM_CLIENT_RFA );
      if( xpt_pm_data_ptr == NULL )
      {
        RF_MSG( RF_ERROR, "MultiLin V3 - mem alloc failed for XPT PM Data table." );
        api_status = FALSE;
      }
      else
      { 
        /* memory allocation went ok: assign memory */
        memset( xpt_pm_data_ptr, 0x0, et_dpd->num_dpd_avail * sizeof(xpt_pm_row_type) ); 
        et_dpd->pm_dpds = xpt_pm_data_ptr;
      } /* XPT Data != NULL */
    }

    xpt_dt_data_ptr = et_dpd->dt_dpds;
    /* If XPT DT data not yet created... */
    if( xpt_dt_data_ptr == NULL )
    {
      /* Create XPT DT Pointers table */
      xpt_dt_data_ptr = 
        (xpt_dt_row_type*)modem_mem_alloc( et_dpd->num_dpd_avail * sizeof(xpt_dt_row_type),
                                           MODEM_MEM_CLIENT_RFA );
      if( xpt_dt_data_ptr == NULL )
      {
        RF_MSG( RF_ERROR, "MultiLin V3 - mem alloc failed for XPT DT Data table." );
        api_status = FALSE;
      }
      else
      { 
        /* memory allocation went ok: assign memory */
        memset( xpt_dt_data_ptr, 0x0, et_dpd->num_dpd_avail * sizeof(xpt_dt_row_type) ); 
        et_dpd->dt_dpds = xpt_dt_data_ptr;
      } /* XPT Data != NULL */
    }

  } /* Allocate mem if not already allocated */

  /* Save XPT Data */
  if( api_status == TRUE )
  {
    /* Save xpt AM data */
    for( index = 0; index < XPT_DPD_NUM_COEFF; index++ )
    {
      xpt_am_data_ptr[tx_et_dpd_record.index].xpt_am[ index ] = tx_et_dpd_record.am[ index ];
      xpt_pm_data_ptr[tx_et_dpd_record.index].xpt_pm[ index ] = tx_et_dpd_record.pm[ index ];
      xpt_dt_data_ptr[tx_et_dpd_record.index].xpt_dt[ index ] = tx_et_dpd_record.dt[ index ];
    }
    *num_et_dpd_saved = *num_et_dpd_saved + 1;
  } /* if( api_status == TRUE ) */

  return api_status;
} /* rfcommon_core_tx_multilin_extract_et_dpd */
/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for extracting temp ADC reading at which linearizer cal was performed
 
  @param nv_data_ptr
  Pointer to data to be extracted

  @param num_bytes_available
  The number of nv data bytes available 

  @param convert_freq_handler
  CB handler to convert channel to frequency
 
  @param extracted_element_size
  Output. The number of bypes extracted for the element

  @param tx_cal_data
  Output destination for the extracted data.
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/
boolean
rfcommon_core_tx_multilin_extract_ref_temp_adc(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  tx_band_cal_type *tx_cal_data)
{
  /* Overall API status */
  boolean api_status = TRUE;
  /* Contains the number of bytes for the element in NV */
  uint32 num_bytes_needed;

  
  num_bytes_needed = sizeof( uint16 );

  /* Initialize to zero here in case checks below fail */
  *extracted_element_size = 0;

    /* Ensure there is enough memory left before actually extracting the element */
  if( num_bytes_available < num_bytes_needed )
  {
    RF_MSG( RF_ERROR, "MultiLin V3 - Not enough elements left to extract item: REF LIN ADC value " );
    api_status = FALSE;
  }

  /* Ensure band cal pointer is not NULL*/
  if (tx_cal_data == NULL) {
     RF_MSG( RF_ERROR, "MultiLin V3 - Band cal type to extract REF LIN ADC value to is NULL. " );
      api_status = FALSE;
    }

  if( api_status == TRUE )
         {
    /* Extract the element */
    memscpy( (void*) &tx_cal_data->ref_lin_temp_adc, num_bytes_needed, 
             nv_data_ptr, num_bytes_needed );
    *extracted_element_size = num_bytes_needed;

  }
  return api_status;
} /* rfcommon_core_tx_multilin_extract_ref_temp_adc*/


/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for validating/populating linerizer post-load of NV.
 
  @details
  Performs post NV processing of linearizer data. For example:
  It ensures linearizers order and locations.
  Populates invalid DPD indecies with valid vals.

  @param tx_cal_data
  Output destination for the extracted data.
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/
boolean
rfcommon_core_linearizer_load_post_processing
(
  tx_band_cal_type *tx_cal_data
)
{
  boolean api_status = TRUE;
  uint16 chan_idx;
  boolean current_lin_exists;
  boolean found_null_lin_data;
  uint32 previous_freq;
  uint32 current_freq;
  uint8 pa_idx;
  uint8 wave_idx;
  tx_multi_linearizer_index_node_type *lin_node;

  /* Ensure that linearizers are contiguously located (no NULLs in between)
     Also that they are populated with unique upper_bound_freq from lowest to highest */
  for( pa_idx = 0; pa_idx < RFCOM_TXLIN_NUM_LINEARIZERS; pa_idx++ )
  {
    for( wave_idx = 0; wave_idx < TX_LINEARIZER_NUM_WAVEFORM; wave_idx++ )
    {
      memset( &previous_freq, 0, sizeof(previous_freq) ); 
      found_null_lin_data = FALSE;
      for( chan_idx = 0; chan_idx < RFCOM_TXLIN_NUM_CHANNELS; chan_idx++ )
      {
        lin_node = &tx_cal_data->tx_pa_state_cal_data[pa_idx].linearizer_index[wave_idx][chan_idx];

        /* Keep track of lin availability */
        if( lin_node->node_data.lin_data == NULL )
        {
          current_lin_exists = FALSE;
          found_null_lin_data = TRUE;
        }
        else
        {
          if ( found_null_lin_data == TRUE )
          {
            /* Linearizers must be in contiguous indices. If we're now 
            finding a new linearizer, while a previous spot determined 
            that we already found NULL lin data, this is an error condition */
            api_status = FALSE;
          }
          current_lin_exists = TRUE;
        }
    
        /* Ensure unique frequencies in ascending magnitudes */
        if( current_lin_exists == TRUE )
        {
          current_freq = lin_node->upper_bound_freq;
          if( current_freq <= previous_freq )
          {
            RF_MSG_2( RF_ERROR, "MultiLin V3 - Error validating linearizer information. "
                      "Current linearizer's upper-bound frequency %d <= %d "
                      "previous linearizer upper-bound frequency not allowed. "
                      "Linearizers must be indexed with unique upper-bound "
                      "frequencies in increasing magnitude.", 
                      current_freq, previous_freq );
            api_status = FALSE;
            //break; DO NOT BREAK. Want to validate all items
          }
          previous_freq = current_freq;
        } /* if( current_lin_exists == FALSE ) */

      } /* for( chan_idx ) */
    } /* for( wave_idx ) */
  } /* for( pa_idx ) */

  return api_status;
} /* rfcommon_core_linearizer_load_post_processing */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for validating/populating linerizer post-load of XPT NV.
 
  @details
  Performs post NV processing of linearizer XPT data.
 
  @param tx_cal_data
  Output destination for the extracted data.
 
  @param ept_dpd_v2
  Flat list of EPT DPD items.
 
  @param et_dpd
  Flat list of ET DPD items.
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/
boolean
rfcommon_core_linearizer_load_post_process_xpt
(
  tx_band_cal_type *tx_cal_data,
  ept_dpd_v2_data_type *ept_dpd_v2,
  et_dpd_data_type *et_dpd
)
{
  boolean api_status = TRUE;
  uint16 chan_idx;
  boolean current_lin_exists;
  uint8 pa_idx;
  uint8 wave_idx;
  uint16 dpd_idx_flat;/* To index flat table */
  uint16 row_idx;
  uint16 row_idx_w_dpd;
  uint16 dpd_usage_idx;
  uint8 coeff_idx;
  tx_multi_linearizer_index_node_type *lin_node;
  xpt_data_type   *xpt_data_ptr = NULL;
  xpt_am_row_type *am_dpds = NULL;
  xpt_pm_row_type *pm_dpds = NULL;
  xpt_dt_row_type *dt_dpds = NULL;
  uint16 num_dpds_flat = 0; /* Number of DPDs the flat table contains */
  typedef struct
  {
    /* Index for flat list */
    uint16 dpd_idx_flat;
    /* Index local to linerizer */
    uint16 dpd_idx_lcl;
  }dpd_idx_used_info;
  dpd_idx_used_info dpd_idx_used_list[RFCOM_TXLIN_NUM_SEGMENTS];
  dpd_idx_used_info dpd_idx_used;
  boolean dpd_idx_allocated;

  for( pa_idx = 0; pa_idx < RFCOM_TXLIN_NUM_LINEARIZERS; pa_idx++ )
  {
    for( wave_idx = 0; wave_idx < TX_LINEARIZER_NUM_WAVEFORM; wave_idx++ )
    {
      for( chan_idx = 0; chan_idx < RFCOM_TXLIN_NUM_CHANNELS; chan_idx++ )
      {
        lin_node = &tx_cal_data->tx_pa_state_cal_data[pa_idx].linearizer_index[wave_idx][chan_idx];

        /* Keep track of lin availability */
        if( lin_node->node_data.lin_data == NULL )
        {
          current_lin_exists = FALSE;
        }
        else
        {
          current_lin_exists = TRUE;
        }
    
        /* If existing lin has DPD... */
        if( ( api_status == TRUE ) && ( current_lin_exists == TRUE ) )
        {
          /* Skipt current lin if not using DPDs */
          if( lin_node->node_data.lin_data->num_dpd == 0 )
          {
            continue;
          }

          /* Get DPD pointers based on DPD type */
          switch(lin_node->node_data.aux_lin_data)
          {
             case TX_LINEARIZER_NO_AUX_DATA:
               continue;
               break;

            case TX_LINEARIZER_EPT_DPD_V2_DATA:
               am_dpds = ept_dpd_v2->am_dpds;
               pm_dpds = ept_dpd_v2->pm_dpds;
               dt_dpds = NULL;
               num_dpds_flat = ept_dpd_v2->num_dpd_avail;
               break;

             case TX_LINEARIZER_ET_DPD_DATA:
               am_dpds = et_dpd->am_dpds;
               pm_dpds = et_dpd->pm_dpds;
               dt_dpds = et_dpd->dt_dpds;
               num_dpds_flat = et_dpd->num_dpd_avail;
               break;

             default:
               RF_MSG_1( RF_ERROR, "MultiLin V3 - DPD Type %d not supported.",
                         lin_node->node_data.aux_lin_data );
               api_status = FALSE;
               break;
          }

          /* Create main XPT table */
          xpt_data_ptr = (xpt_data_type*)modem_mem_alloc( sizeof(xpt_data_type),
                                                          MODEM_MEM_CLIENT_RFA );
          if( xpt_data_ptr == NULL )
          {
            RF_MSG( RF_ERROR, "MultiLin V3 - mem alloc failed for XPT Data table." );
            api_status = FALSE;
          }
          else
          { 
            /* memory allocation went ok: assign memory */
            memset( xpt_data_ptr, 0x0, sizeof(xpt_data_type) ); 
            lin_node->node_data.xpt_data = xpt_data_ptr;

            /* Create XPT AM table */
            xpt_data_ptr->xpt_am_table = (xpt_am_table_type*)modem_mem_alloc( sizeof(xpt_am_table_type),
                                                                              MODEM_MEM_CLIENT_RFA );
            if( xpt_data_ptr->xpt_am_table == NULL )
            {
              RF_MSG( RF_ERROR, "MultiLin V3 - mem alloc failed for XPT AM table." );
              api_status = FALSE;
            }
            else
            {
              memset(xpt_data_ptr->xpt_am_table, 0x0, sizeof(xpt_am_table_type));
            }

            /* Create XPT PM table */
            xpt_data_ptr->xpt_pm_table = (xpt_pm_table_type *)modem_mem_alloc(sizeof(xpt_pm_table_type),
                                                                              MODEM_MEM_CLIENT_RFA);
            if (xpt_data_ptr->xpt_pm_table == NULL)
            {
              RF_MSG( RF_ERROR, "MultiLin V3 - mem alloc failed for XPT PM table." );
              api_status = FALSE;
            }
            else
            {
              memset(xpt_data_ptr->xpt_pm_table, 0x0, sizeof(xpt_pm_table_type));
            }

            /* DT Section */
            if (lin_node->node_data.aux_lin_data == TX_LINEARIZER_ET_DPD_DATA)
            {
              /* Create XPT DT table */
              xpt_data_ptr->xpt_dt_table = (xpt_dt_table_type *)modem_mem_alloc(sizeof(xpt_dt_table_type),
                                                                                MODEM_MEM_CLIENT_RFA);
              if (xpt_data_ptr->xpt_dt_table == NULL)
              {
                RF_MSG( RF_ERROR, "MultiLin V3 - mem alloc failed for XPT DT table." );
                api_status = FALSE;
              }
              else
              {
                memset(xpt_data_ptr->xpt_dt_table, 0x0, sizeof(xpt_dt_table_type));
              }
            } /* DT Section */

          } /* Create main XPT table */

          /* Set to invalid vals before filling up/using */
          memset( dpd_idx_used_list, 0xFF, sizeof(dpd_idx_used_list) );

          /* Create DPD Rows */
          if (api_status == TRUE)
          {
            /* For each lin row */
            row_idx_w_dpd = -1;
            for ( row_idx = 0;
                  (row_idx < RFCOM_TXLIN_NUM_SEGMENTS) && 
                  (lin_node->node_data.lin_data->table[row_idx].rgi != RFCOM_TXLIN_INVALID_RGI ) && 
                  (api_status == TRUE);
                  row_idx++ )
            {
              /* Set items to invalid vals before filling up/using */
              memset( &dpd_idx_used, 0xFF, sizeof(dpd_idx_used) );

              /* If current != previous index */
              dpd_idx_flat = lin_node->node_data.lin_data->table[row_idx].dpd_index;
              
              /* If this and invalid DPD index: STOP */
              if (dpd_idx_flat >= num_dpds_flat)
              {
                if (row_idx == 0)
                {
                  RF_MSG( RF_ERROR, "MultiLin V3 - RGI idx 0 has invalid DPD idx." );
                  api_status = FALSE;
                }
                break;
              }

              /* Search if we have already allocated space for this particular index */

              dpd_idx_allocated = FALSE;
              for (dpd_usage_idx = 0;dpd_usage_idx < RFCOM_TXLIN_NUM_SEGMENTS; dpd_usage_idx++)
              {
                /* Init dpd used info. 
                   If already allocated, it will have valid info which is later used to save
                   as "local" information for the current linearizer row index.
                   If not yet allocated, it will be set to valid vals below */
                dpd_idx_used.dpd_idx_flat = dpd_idx_used_list[dpd_usage_idx].dpd_idx_flat;
                dpd_idx_used.dpd_idx_lcl = dpd_idx_used_list[dpd_usage_idx].dpd_idx_lcl;
                /* If we have reached the end of the list */
                if (dpd_idx_used.dpd_idx_flat == 0xFFFF)
                {
                  break;
                }
                /* If found, the current index has already been accounted/allocated */
                if (dpd_idx_used.dpd_idx_flat == dpd_idx_flat)
                {
                  dpd_idx_allocated = TRUE;
                  break;
                }
              }

              /* Allocate new DPD row only if not already allocated */

              if (dpd_idx_allocated == FALSE)
              {
                row_idx_w_dpd++;

                /* Save info for the current dpd index we are allocating */
                dpd_idx_used.dpd_idx_flat = dpd_idx_flat;
                dpd_idx_used.dpd_idx_lcl = row_idx_w_dpd;

                /* AM */
                xpt_data_ptr->xpt_am_table->xpt_am_row[row_idx_w_dpd] =
                   (xpt_am_row_type *)modem_mem_alloc(sizeof(xpt_am_row_type),
                                                      MODEM_MEM_CLIENT_RFA);
                if (xpt_data_ptr->xpt_am_table->xpt_am_row[row_idx_w_dpd] == NULL)
                {
                  RF_MSG( RF_ERROR, "MultiLin V3 - mem alloc failed for XPT AM row." );
                  api_status = FALSE;
                }
                else
                {
                  memset(xpt_data_ptr->xpt_am_table->xpt_am_row[row_idx_w_dpd], 0x0, sizeof(xpt_am_row_type));
                }
                /* PM */
                xpt_data_ptr->xpt_pm_table->xpt_pm_row[row_idx_w_dpd] =
                   (xpt_pm_row_type *)modem_mem_alloc(sizeof(xpt_pm_row_type),
                                                      MODEM_MEM_CLIENT_RFA);
                if (xpt_data_ptr->xpt_pm_table->xpt_pm_row[row_idx_w_dpd] == NULL)
                {
                  RF_MSG( RF_ERROR, "MultiLin V3 - mem alloc failed for XPT PM row." );
                  api_status = FALSE;
                }
                else
                {
                  memset(xpt_data_ptr->xpt_pm_table->xpt_pm_row[row_idx_w_dpd], 0x0, sizeof(xpt_pm_row_type)); 
                }
                /* DT */
                if (lin_node->node_data.aux_lin_data == TX_LINEARIZER_ET_DPD_DATA)
                {
                  xpt_data_ptr->xpt_dt_table->xpt_dt_row[row_idx_w_dpd] = 
                    (xpt_dt_row_type*)modem_mem_alloc( sizeof(xpt_dt_row_type),
                                                       MODEM_MEM_CLIENT_RFA );
                  if( xpt_data_ptr->xpt_dt_table->xpt_dt_row[row_idx_w_dpd] == NULL )
                  {
                    RF_MSG( RF_ERROR, "MultiLin V3 - mem alloc failed for XPT DT row." );
                    api_status = FALSE;
                  }
                  else
                  {
                    memset( xpt_data_ptr->xpt_dt_table->xpt_dt_row[row_idx_w_dpd], 0x0, sizeof(xpt_dt_row_type) );
                  }
                }
                
                if( api_status == TRUE )
                {
                  /* Copy DPDs from flat table to this lin */
                  for (coeff_idx = 0; coeff_idx < XPT_DPD_NUM_COEFF; coeff_idx++)
                  {
                    /* AM */
                    xpt_data_ptr->xpt_am_table->xpt_am_row[row_idx_w_dpd]->xpt_am[coeff_idx] = 
                      am_dpds[dpd_idx_flat].xpt_am[coeff_idx];
                    /* PM */
                    xpt_data_ptr->xpt_pm_table->xpt_pm_row[row_idx_w_dpd]->xpt_pm[coeff_idx] = 
                      pm_dpds[dpd_idx_flat].xpt_pm[coeff_idx];
                    /* DT */
                    if (lin_node->node_data.aux_lin_data == TX_LINEARIZER_ET_DPD_DATA)
                    {
                      xpt_data_ptr->xpt_dt_table->xpt_dt_row[row_idx_w_dpd]->xpt_dt[coeff_idx] = 
                        dt_dpds[dpd_idx_flat].xpt_dt[coeff_idx];
                    }
                  } /* Copy DPDs */
                }

                /* Save info for the newly allocated dpd to later keep track if already allocated */
                dpd_idx_used_list[row_idx_w_dpd].dpd_idx_flat = dpd_idx_used.dpd_idx_flat;
                dpd_idx_used_list[row_idx_w_dpd].dpd_idx_lcl = dpd_idx_used.dpd_idx_lcl;

              }/* If current index has not been allocated */

              /* Save the corresponding "local" dpd index from above by either 
                 searching already allocated or newly allocated dpd */
              lin_node->node_data.lin_data->table[row_idx].dpd_index = dpd_idx_used.dpd_idx_lcl;

            } /* For each lin row */
          } /* Create DPD Rows */
        } /* If existing lin has DPD... */
      } /* for( chan_idx ) */
    } /* for( wave_idx ) */
  } /* for( pa_idx ) */

  return api_status;
} /* rfcommon_core_linearizer_load_post_process_xpt */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for validating linerizer post-load of dpd type.
 
  @details
  Linearizer-to-PA State mapping must be such that XPT types are listed in
  increasing order just like PA state.
 
  @param tx_cal_data
  Output destination for the extracted data.
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/
boolean
rfcommon_core_linearizer_load_post_process_dpd_type
(
  tx_band_cal_type *tx_cal_data
)
{
  boolean api_status = TRUE;
  uint16 chan_idx;
  boolean current_lin_exists;
  uint8 pa_idx;
  uint8 wave_idx;
  tx_multi_linearizer_index_node_type *lin_node;
  tx_linearizer_aux_data_type prev_dpd_data_type = TX_LINEARIZER_DPD_INVALID;

  for( chan_idx = 0; chan_idx < RFCOM_TXLIN_NUM_CHANNELS; chan_idx++ )
  {
    for( wave_idx = 0; wave_idx < TX_LINEARIZER_NUM_WAVEFORM; wave_idx++ )
    {
      for( pa_idx = 0; pa_idx < RFCOM_TXLIN_NUM_LINEARIZERS; pa_idx++ )
      {
        lin_node = &tx_cal_data->tx_pa_state_cal_data[pa_idx].linearizer_index[wave_idx][chan_idx];

        /* Keep track of lin availability */
        if( lin_node->node_data.lin_data == NULL )
        {
          current_lin_exists = FALSE;
        }
        else
        {
          current_lin_exists = TRUE;
        }
    
        /* Ensure current lin's dpd type is same or higher than prevous lin's dpd type */
        if( current_lin_exists == TRUE )
        {
          
          if (pa_idx == 0) /* Save dpd when in first PA state */
          {
            prev_dpd_data_type = lin_node->node_data.aux_lin_data;
          }
          else /* Validate current dpd type against previous */
          {
            if(lin_node->node_data.aux_lin_data < prev_dpd_data_type)
            {
              RF_MSG_2( RF_ERROR, "MultiLin V3 - Current DPD type %d must be >= than previous DPD type %d.",
                        lin_node->node_data.aux_lin_data, prev_dpd_data_type );
              api_status = FALSE;
            }
          }
        } /* If current lin exists */
      } /* for( pa_idx ) */
    } /* for( wave_idx ) */
  } /* for( chan_idx ) */

  return api_status;
} /* rfcommon_core_linearizer_load_post_process_dpd_type */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns a copy of the current Tx linearizer Freq Comp Table

  @details
  A copy of current Tx linearizer Freq Comp Table can be fetched

  @return None

*/

void rfcommon_core_get_tx_linearizer_freq_comp_table(int8* curr_tx_linearizer_freq_comp_table)
{
  memscpy(curr_tx_linearizer_freq_comp_table, sizeof(int8)*RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN, tx_linearizer_freq_comp_table, sizeof(int8)*RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN);
}
#endif /* FEATURE_GNSS_ONLY_NO_WWAN */
