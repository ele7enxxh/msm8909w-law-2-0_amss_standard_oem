/*!
  @file
  rf_cdma_tx_agc.c

  @brief
  This file defines the functionality and algorithms to be performed for Tx
  AGC operations

  @addtogroup RF_CDMA_COMMON_TXAGC
  @{
*/

/*==============================================================================

  Copyright (c) 2011 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_tx_agc.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/13/16   pkm     Make sure Pin/Pout not applied in temp comp when Autopin is running.
01/12/16   rs      Fix incorrect linarizer loading
12/22/15   rs      KW error fix
09/17/15   kma     Added auto pin support for Pin cal
02/13/15   cd      Tx Lin max power cap/boost updates
01/16/15   vr     CDMA predictive clipping NV based power thresh support
11/12/14   spa     Change tx_nv_path to be per band per logical device
11/10/14   zhw     Dynamic Predictive Cliping support
08/01/14   cd      Add support for FTM AGC logging with FED and common tx AGC
                   functionality
07/21/14   aro/kai Added expansion ratio param in DPD expansion function
07/02/14   kr/kai  Non-0dB Pin SW implementation to support -ve pin offsets values
06/16/14   vs/kai  Non-0dB Pin SW implementation
04/23/14   cd      Support for 8 PA state functionality
04/16/14   cd      Cleanup unused MDSP interfaces and Tx AGC functionality
04/09/14   sn      Updated PA interface
04/08/14   sty     KW fixes
11/08/13   cc      1x logging enhancement
02/13/14   cd      Restore functionality to load DPD only if temp comp init
                   trigger is TRUE
02/07/14   cd      Load DPD during Tx linearizer loading irrespective of
                   temp comp init trigger
02/06/14   cd      Remove hard-coding to retrieve common device instance
01/22/14   cd      Added support for static and dynamic linearizer loading
                   changes for temp comp
01/15/14   JJ      Removed hardcoded PA device name
01/08/14   cd      Provide xPT type to RFLM per PA state
01/06/14   cd      Linearizer and Tx AGC updates for loading xPT information to
                   RFLM
12/17/13   vr      Fix for lin table corruption when no QPOET
12/13/13   sty     deleted unused rf_cdma_tx_agc_get_template_id
12/06/13   sar     Restored num_rgi_with_dpd_info with xpt feature.
12/04/13   sar     Fixed compiler warning.
11/26/13   cd      Set device info pointers to NULL, if validity checks fail
11/25/13   cd      Remove flag-based functionality for legacy Tx AGC
11/20/13   sar     Fixed compiler warning.
11/13/13   zhw     Initialize max linearizer power to minumum txagc
11/14/13   cd      Added generic API to query CDMA Tx device pointers
10/16/13   cc      Add new ET logging for FTM AGC logs
10/09/13   cd      Configure Tx AGC differently when common Tx aGC is used
10/08/13   cd      Add support for common TxAGC debug flag
10/05/13   cd      Skip LUT validity check for device with common Tx interface
09/30/13   cd      Fixed query for WTR common device instance
09/27/13   cd      Hook up common load linearizer interface to CDMA
09/26/13   cd      Added functionality to load to RFLM through common TxAGC
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
08/13/13   Saul    XPT. KW fix.
08/12/13   Saul    XPT. DPD Scaling.
07/17/13   cd      Fixed compiler warning for dpd populated flag
07/10/13   JJ      Rename lagacy flag rumi_bup_cdma
07/09/13   zhw     Compiler warning fix
07/07/13   sbo     Added BW support for PA device
07/05/13   zhw/sty Clean up rf_cdma_data status mutable pointers
07/02/13   cd      Commit DPD to HW only if DPD tables are populated for any
                   one linearizer
06/24/13   zhw     Temporarily featurize template code for Triton
06/10/13   zhw     Convert pa icq write data from uint32 to byte array
06/07/13   APU     Check for _tx_agc_load_linearizer() before writing to FW.
06/07/13   jf      Fix LTE freq comp
05/30/13   JJ      Fixed rf_cdma_get_tx_min_max_pwr input band type
05/28/13   JJ      Clean un-necessary cdma sys_band to rf_band_type conversion
05/21/13   sty     update the QPOET addr in the linearizer table in
                   rf_cdma_tx_agc_process_linearizer_for_fw when the linearizer
                   is not valid
05/16/13   fh      Added template id 3 and 4
05/13/13   Saul    XPT. Support EPT_ET_CFG.
05/09/13   cd      Support for ref temp ADC value from multilin NV
05/07/13   cd      Temp Comp - Support legacy and xPT temp comp algorithms
04/22/13   cd      Temp Comp - support for multi-chan linearizer compensation
04/17/13   zhw     Support for variable transaction in QPOET/PA TxAGC templates
04/12/13   cd      Remove max PA state functionality
04/02/13   cd      DPD type must be correctly initialized for invalid linearizer
04/02/13   cd      - Allow skipping of PA states when computing max valid PA
                   state. Provide new API for valid/invalid PA state
                   - Remove dependence on max PA state where possible
03/28/13   zhw     Add support for getting apt value from linearizer
03/22/13   sr      changes to PAPM device interface.
03/19/13   APU     Replaced memcpy with memscpy.
03/20/13   cd      XPT Temp comp support
03/07/13   sr      changed the "pwr_tracker" name to "papm".
03/06/13   Saul    ET. Each pa state now has QPOET settings corresponding to
                   linearizer's xpt data type instead of ET for all states.
02/22/13   Saul    CDMA. Config xpt mode based on dpd info in linearizer.
02/21/13   Saul    CDMA. Added xpt_temp_comp_enabled debug flag.
02/21/13   zhw     Use SSBI bus from wtr driver instead of RFC
02/20/13   zhw     Use slave ID for PA from device driver instead of RFC
02/08/13   Saul    CDMA. Enable Max PA IQ Gain Update.
01/31/13   hdz     Use computed_lin_data when temp comp is disabled
01/29/13   spa     Not mark failure if QPOET object is NULL
01/22/13   zhw     APT PA Q current. Use icq value of 0 to get default setting
01/16/13   Saul    CDMA. Enabled EPT/ET tempcomp.
01/15/13   zhw     APT PA Q Current fix that returns default Q Current setting
                   when PA Q Current is disabled.
01/15/13   Saul    CDMA. Mainlined XPT_ONLINE code.
01/15/13   zhw     Warning fix
01/15/13   zhw     Enable PA Quiescent current
01/14/13   Saul    CDMA. 1x EPT/ET call support.
01/10/12   zhw     Move away from using common PA device state
12/20/12   zhw     Integrate GRFC PA device scripting to pa_state_script callflow
12/19/12   Saul    CDMA. TX AGC XPT Dev. Use num dpd from compensated lin.
12/14/12   Saul    CDMA. TX AGC XPT Dev. Using FEATURE_RF_HAS_XPT_ONLINE_SUPPORT
12/13/12   Saul    CDMA. TX AGC XPT Dev. HDR
12/12/12   Saul    CDMA. TX AGC XPT Dev. Call cmn DPD API only when lin has DPD.
12/11/12   zhw     Integrated GRFC PA device driver to CDMA
12/11/12   sty     KW fixes
12/11/12   sty     KW fixes
12/11/12   Saul    CDMA. TX AGC XPT Dev. Get max PA state based on lin NV.
12/09/12   zhw     Uses unified PA gain state script API for HCPA and QPA
12/10/12   cri     Load DDP from current linearizer
12/10/12   zhw     Fix for QPOET using multi-lin NV during APT Cal
12/07/12   Saul    CDMA. TX AGC XPT Development.
12/06/12   spa     Remove hard coding of QPOET addresses
12/06/12   spa     Initialize rffe index to zero
12/05/12   spa     Added API to get QPOET informaiton for templates
12/05/12   spa     Added support to load linearizer tables with QPOET data
12/05/12   Saul    CDMA. TX AGC XPT. Ini/Save DPD outside of PA state context.
12/03/12   APU     Added a one time check for therm cal monotonicity.
11/30/12   Saul    CDMA. TX AGC XPT Development.
11/29/12   zhw     PA API refactor and make pa_script API static
11/19/12   zhw     Fix PA Q current API being called for non-RFFE PA
11/12/12   zhw     Load FW mdsp interface with PA Q current RFFE writes
11/16/12   sty/av   Added partial GRFC PA support
11/15/12   Saul    CDMA. TX AGC XPT.
11/14/12   sty     Fixed call to rf_cdma_tx_agc_get_pa_scripts
11/12/12   Saul    MultiLin. Removed dpd_data from lin table type.
11/11/12   sty     Reference QPOET only if it is valid in
                   rf_cdma_tx_agc_get_device_config
11/09/12   spa     Set PA gain range for HCPA based on device ID and not HW ID
11/07/12   zhw     Rewrite get template ID API.
10/31/12   zhw     Fix OFT compiler error.
10/29/12   zhw     Support for query device_config from RFC and device driver
                   Remove hardcoding in Tx templates
10/23/12   zhw     Remove hardcoding in get_template_id()
10/23/12   spa     Support for template group 2
10/18/12   zhw     Template 1 support
10/16/12   zhw     Template ID F3 message & fixes
10/04/12   zhw     Compiler warning fix
10/02/12   zhw     Support for PA state setting for SSBI/RFFE PA
09/26/12   zhw     Get Template ID API based on RFM device.
                   Temporarily using HW ID. Should be rf device based.
09/20/12   sty     Deleted using fake_NVs - use NV read from qcn
08/24/12   cd      CDMA EPT Temp and HDET feature support
08/22/12   Saul    CDMA. Cleaned up DPD code in temp comp.
08/21/12   Saul    MultiLin. Added support to load EPT V2 and ET NV Data.
                   Renamed API to reflect its freq-comp work.
08/17/12  cri/Saul CDMA. Added dpd copy API calls due to removal of
                   dpd_data copying in compensation APIs
08/15/12   sty     Updated power table in rf_cdma_load_fake_linearizer()
08/14/12   Saul    COMMON. Bug fix on copying EPT cfg params.
08/14/12   sty     Added rf_cdma_load_fake_linearizer()
08/07/12   sty     Warning fix
07/17/12   cri     KW Error fix
07/11/12   cri     Added EPT support for CDMA
07/09/12   cd      - EPT Temp requires freq comp of first sweep linearizer using
                   multi-lin Rev2
                   - Refactored device LUT and freq comp functions to declutter
                   load linearizer functionality
                   - Populate IQ gain offset in the computed linearizer
                   - Temp comp functionality now uses both first sweep and APT
                   sweep linearizers
06/07/12   vs      Add WCDMA Temp comp support for EPT
06/04/12   cri     Add EPT support for WCDMA
05/01/12   cri     Updated EPT linearizer structure and processing
04/04/12   cri     Support for processing EPT linearizers
03/22/12   sty     Query RF device for LUT only for valid PA States
03/21/12   spa     Added max device check in rf_cdma_get_mutable_device_status()
03/16/12   hdz     Renamed temp_comp.is_combined_with_freq_comp to
                   temp_comp.init_trigger
02/29/12   cd      Cleaned up legacy band nv tbl references
02/09/12   Saul    MCDO. Tx AGC uses MCDO switch-points.
02/08/12   hdz     Cleaned up function.
02/07/12   hdz     Fixed compiler warnings
02/06/12   hdz     Added rf_cdma_update_tx_agc() for re-loading tx agc in calls
12/20/11   aro     Added inteface to query the TxLUT index for given PA state
                   and FW Table index
12/07/11   hdz     Change rf_cdma_tx_get_max_pa_state due to the new PA nv items
12/07/11   whc     Common helper function for QSORT to sort linearizer table
11/28/11   hdz     Added sanity check for all temp comp related NV items
11/21/11   hdz     Hook up NV items to rf_cdma_tx_temp_comp()
11/11/11   hdz     Hard-code offset in LIN_vs_TEMP to all zeros
11/09/11   cd      Added debug functionality to disable temp comp
11/08/11   hdz     Sort input tx_lin_LUT_temp before calling rf_1x_temp_comp()
11/07/11   hdz     Added support for temp comp
11/04/11   hdz     Added rf_cdma_tx_temp_comp()
11/03/11   cd      Provide interface to return max PA state for a given band
10/18/11   cd      Removed old linearizer load functionality and replaced
                   multi-lin to be the default
09/15/11   cd      Check for power values below min power that can be
                   represented in TxAGC. Instead of saturating at 32767, skip
                   loading of these, in order to ensure valid data in FW.
09/10/11   cd      Changed invalid avga data type from uint16 to uint32
09/01/11   cd      Add APT support
                   - Load the APT LUT values from NV into the FW interface table
                   using the multi-linearizer load functionality
08/17/11   cd      Fixed KW warnings
08/16/11   cd      Functionality to compute FW linearizer tables from new
                   multi-linearizer internal data structures
07/26/11   sty     added back changes from rev#8 and #9
07/26/11   sty     Reverted changes from rev#8 and #9
                   Fixed null-pointer access
07/26/11   sty     [1] use the correct index to reference a member in array
                   tx_lut_data[pa_state].lut_table_ptr
                   [2] load valid AVGA word in case of all entries being invalid
07/20/11   sty     Changed arg-list for rf_cdma_load_tx_linearizers
                   Deleted calls to get RFDEVICE_GET_TX_LIN_MAX_OFFSET and
                   RFDEVICE_GET_TX_LIN_MIN_OFFSET from device
07/13/11   sty     Doxygen update
07/12/11   sty     Minor debug message change
07/12/11   sty     Renamed rfdevice_lut_data_type_new to rfdevice_lut_data_type
07/07/11   sty     [1] Changed return type of rf_cdma_load_tx_linearizers
                   [2] rf_cdma_load_tx_linearizers now queries RFC to get the
                   name of LUT table to be used and passes the same to RF device
                   to get LUT table contents
06/14/11   bmg     Insure that LUT word is initialized in all cases
06/13/11   sty     Rename rf_cdma_txagc.h to rf_cdma_tx_agc.h
06/07/11   shb     Updated RF CDMA device interface to accept RX/TX device
                   pointers (queried from RFC) instead of the logical path
06/02/11   sty     Deleted bypassing NV reads
05/24/11   aro     Added Logical Device Mapping to MDSP Chain / NV for SV
05/17/11   sty     Added bringup workarounds - load single-point cal
05/10/11   cd      Remove old NV table pointer usage
05/10/11   cd      - Updated load linearizer functionality to load to firmware
                     in increasing monotonic power values
                   - Uses new NV data structures to query calibration data
                   - Added interpolation to support programming of freq comp
                     for Tx power
05/04/11   shb     Pass band to RF device in TX LUT address/data query functions
03/25/11   sty     Changed rf_cdma_txlin.h to rf_cdma_txagc.h
                   Doxygen fixes
03/24/11   sty     Initial revision

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_cdma_tx_agc.h"
#include "rfdevice_cdma_interface.h"
#include "rf_cdma_utils.h"
#include "rfcommon_msg.h"
#include "rf_cdma_utils_freq.h"
#include "rfcommon_math.h"
#include "rfm_internal.h"
#include "rfm_gain_state_types.h"
#include "rfc_cdma.h"
#include "rf_cdma_nv.h"
#include "rfcommon_core_txlin.h"
#include "rfcommon_core_xpt.h"
#include "rf_cdma_data.h"
#include "modem_mem.h"
#include "rf_cdma_temp_comp.h"
#include "rf_1x_tx_agc.h"
#include "rf_hdr_tx_agc.h"
#include "fw_dpd_intf.h"
#include "rfcommon_nv_mm.h"
#include "rfdevice_pa_intf.h"
#include "rf_cdma_data.h"
#include "rf_cdma_mc.h"
#include "rfc_card.h"
#include "rfcommon_autopin_api.h"

#include "rf_cdma_constants.h" /* For max GRFC writes */
#include "rfdevice_papm_intf.h" /* QPOET */
#include "rf_cdma_auto_pin.h"
#include "rflm_api_autopin_hw.h"

/* memscpy */
#include "stringl.h"
#include "math.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper to perform Tx frequency compensation

  @details
  This function acts as a wrapper to perform Tx frequency compensation using
  common functionality

  @param device
  RFM device information for freq comp functionality

  @param tx_multilin_search_params
  Search conditions for linearizer

  @param tx_multilin_nv_cal_ptr
  Tx Multilin NV cal pointer

  @param result_linearizer
  Linearizer table populated as a result of freq comp

  @return
  TRUE if freq comp was successful, else FALSE
*/
static
boolean
rf_cdma_tx_agc_freq_comp
(
  rfm_device_enum_type device,
  tx_linearizer_search_param_type tx_multilin_search_params,
  const tx_band_cal_type *tx_multilin_nv_cal_ptr,
  tx_linearizer_table_data_type *result_linearizer
)
{
  boolean valid_lin_data;
  rfcommon_core_txlin_cap_boost_type cap_boost_data;

  /* Cap-boost information from NV, used by freq comp */
  cap_boost_data.max_pwr_ovr_algo = rf_cdma_xpt_get_cfg_param(
                                                device,
                                                XPT_CFG_CMN_MAX_PWR_OVR_ALGO );
  cap_boost_data.lin_cap_boost_nv_val = rf_cdma_xpt_get_cfg_param(
                                                device,
                                                XPT_CFG_CMN_MAX_PWR_OVR_VAL );

  /* Obtain the frequency compensated linearizer using common functionality */
  valid_lin_data = rfcommon_core_freq_comp_linearizer(
                          &tx_multilin_search_params,
                          (tx_band_cal_type *)tx_multilin_nv_cal_ptr,
                          result_linearizer,
                          (void*)&cap_boost_data,
                          TRUE );

  /* Sort the frequency compensated linearizer */
  qsort( &(result_linearizer->lin_data->table),
         RFCOM_TXLIN_NUM_SEGMENTS,
         sizeof(tx_linearizer_row_type),
         rfcommon_core_qsort_compare_increasing_pwr );

  return valid_lin_data;
} /* rf_cdma_tx_agc_freq_comp */



/*-----------------------------------------------------------------------------*/
/*!
  @brief
  This function offsets the power provided with compensation.

  @details
  This function offsets the power provided with compensation in
  dB10. The power will be simply added with the sign intact:
  i.e. -ve sign will give you a lower power and +ve sign will
  result in higher power in the linearizer. As a final result
  the linearizer power will be shifted up[+ve] or down[-ve] by
  the compensation_in_db10 provided.

  @params tx_ascending_pwr_sorted_linearizer_data
  Linearizer data with power sorted in acsending order. This
  input and constraint is important since this fucntion does not
  check or sort, it just iterates until it sees 0xFFFF,
  signifying invalid RGI.

  @params compensation_in_db10
  This will be added to the linearizer power with sign impact.

  @params limit_in_dB10
  This is an unsigned parameter and if
  (abs(compensation_in_db10)) > limit_in_dB10, the compensation
  will NOT be applied, i.e. compensation_in_db10 = -56 and
  limit_in_dB10 = 30 and since 56 > 30, the compensation will
  not be applied.

  @return
  TRUE: If the compensation was applied.
  FASLE: Compensation not applied because of some error.

/*-----------------------------------------------------------------------------*/
boolean
rf_cdma_txagc_shift_linearizer
(
  tx_linearizer_table_type*     tx_ascending_pwr_sorted_linearizer_data ,
  int8                          compensation_in_db10 ,
  uint8                         limit_in_dB10
)
{
  int rgi_index ;
  boolean success = FALSE ;
  uint8 abs_compensation_in_db_10 = ( compensation_in_db10 > 0 )?
                        compensation_in_db10 : (-compensation_in_db10) ;

  if ( limit_in_dB10 > abs_compensation_in_db_10 )
  {
    if ( tx_ascending_pwr_sorted_linearizer_data != NULL )
    {
      success = TRUE ;
      for ( rgi_index = 0 ; rgi_index < RFCOM_TXLIN_NUM_SEGMENTS ; rgi_index++ )
      {
        /* copy Tx power in dB10 AGC units with freq comp*/
        if ( tx_ascending_pwr_sorted_linearizer_data->table[rgi_index].rgi != 0xFFFF )
        {
          tx_ascending_pwr_sorted_linearizer_data->table[rgi_index].power =
            tx_ascending_pwr_sorted_linearizer_data->table[rgi_index].power +
              compensation_in_db10 ;
        }
        else
        {
          /* Last valid RGI has been reached. Nothing more to compensate */
          break ;
        }
      }
    }
    else
    {
      RF_MSG ( RF_ERROR , "rf_cdma_txagc_shift_linearizer(): "
               "NULL tx_ascending_pwr_sorted_linearizer_data poitner " ) ;
    }
  }
  else
  {
    RF_MSG_2 ( RF_ERROR , "rf_cdma_txagc_shift_linearizer(): "
             "Compensation value [%d] greater than the limit value [%d] "
             "No compensation applied" , abs_compensation_in_db_10 ,
              limit_in_dB10 ) ;
  }
  return success ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Populate the linearizer tables with power and RGI data for all PA States

  @details
  This API populates the Tx linearizer tables for all valid PA gain states.

  Calibrated values are loaded from NV into multi-linearizer internal
  data structures.

  For each PA gain state, the RFC is first queried for obtaining a valid
  device lookup table for RGI data. The internal data with calibration
  information is then provided to common tx linearizer functionality to
  obtain a compensated linearizer.

  The compensated linearizer is sorted in order of increasing power values.
  These sorted values are then copied to the final linearizer table with
  corresponding RGI data. If the linearizer is partially invalid, the
  invalid entries are padded with last valid power and last valid RGI data.

  If all entries in NV for Pwr and/RGI are invalid, the linearizer for that
  PA state is loaded with RGI and power entries that result in the min. possible
  Tx power

  @param mode
  RFM mode. 1x or DO (Added because QPOET driver uses this as a parameter)

  @param device
  Device for which tables are to be loaded to FW

  @param rf_band
  band for which linearizers are to be loaded

  @param rf_channel
  Channel number to which device is tuned

  @param waveform_type
  Waveform type for which the common compensation algorithm
  should produce compensated linearizer values

  @param tx_lin_data
  pointer to the data structure that contains the Tx linearizers
  (power, RGI and APT) tables. On successful completion of this function,
  the tables will have been loaded with valid power and LUT values and
  will be extended for APT values. It will also be loaded with RFFE
  data for PA Quiescent Current control

  @param pa_stm_data
  PA STM Data to be populated with DPD type per PA state

  @internal
  tx_lin_data is populated with power values that are in increasing
  monotonic order. These values are converted into mDSP TxAGC units
  before loading to FW linearizer tables, and the converted TxAGC
  values are in decreasing monotonic order.

  pa_stm_data is populated with RFFE addresses that will be used
  in Tx templates.

  @return
  TRUE on success and FALSE on failure

*/
boolean
rf_cdma_tx_agc_load_linearizer
(
  rfm_mode_enum_type const mode,
  rfm_device_enum_type const device,
  rfm_cdma_band_class_type const rf_band,
  uint16 rf_channel,
  tx_linearizer_waveform_type waveform_type,
  rf_cdma_mdsp_pa_stm_config_type *pa_stm_data,
  lm_handle_type txlm_handle
)
{
  rfm_pa_gain_state_type pa_state;
  boolean init_status = TRUE;
  boolean is_dpd_populated = FALSE;

  /* Logical Device */
  const rfc_cdma_logical_device_params_type *logical_dev = NULL;
  rf_cdma_data_status_type *dev_status_w;
  tx_linearizer_table_data_type dev_lin_data;
  tx_linearizer_table_data_type computed_lin_data;
  tx_linearizer_table_data_type fb_lin_data;
  rfcommon_core_txlin_agc_device_info device_info;
  rfcommon_core_txlin_agc_pa_stm_info pa_stm_info;
  rfcommon_core_xpt_tx_agc_info_type xpt_info;

  /* Multilinearizer NV data retrieval */
  const tx_band_cal_type *tx_multilin_nv_cal_ptr = NULL;
  /* Cal data pointer, both static and multi lin */
  const rf_cdma_tx_static_type *tx_static_nv_cal_ptr = NULL;
  /* Common linearizer compensation search parameters */
  tx_linearizer_search_param_type tx_multilin_search_params;

  uint16 num_rgi_with_dpd_info = 0;
  xpt_dpd_scaling_value_type dpd_scale_factor;
  const rf_cdma_tx_cal_type* rf_cdma_tx_cal_data = NULL;
  /* ET logging for Pout_vs_freq table */
  int8 curr_tx_linearizer_freq_comp_table[RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN]={0};

  dpd_scale_factor.valid = FALSE;

  /* Device Status Write Pointer */
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  /* NULL pointer check */
  if ( dev_status_w == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_load_tx_linearizer: "
                        "Null device data for device %d ",device);
    init_status = FALSE;
  }

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  if ( logical_dev == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_tx_agc_load_linearizer: "
                      "Null pointer returned to logical_dev ");
    init_status = FALSE;
  }
  else
  {
    rf_cdma_tx_cal_data = rf_cdma_nv_get_tx_cal(
                                              logical_dev->tx_nv_path[rf_band],
                                              rf_band );
    if ( rf_cdma_tx_cal_data == NULL )
    {
      RF_MSG_1 ( RF_ERROR, "rf_cdma_tx_agc_load_linearizer: "
                 "Failed to get valid tx cal NV pointer "
                 "for device %d", device );
      init_status = FALSE;
    }
  }

  /* Get a pointer to the Tx calibration data */
  if( init_status == TRUE )
  {
    tx_multilin_nv_cal_ptr = rf_cdma_nv_get_tx_multilin_cal(
                                            logical_dev->tx_nv_path[rf_band],
                                            rf_band);
    /* null pointer check */
    if ( tx_multilin_nv_cal_ptr == NULL )
    {
      RF_MSG_1 ( RF_ERROR, "rf_cdma_tx_agc_load_linearizer: "
                           "Failed to get valid multilin NV pointer "
                           "for device %d", device);
      init_status = FALSE;
    }
  }

  if( init_status == TRUE )
  {
    tx_static_nv_cal_ptr = rf_cdma_nv_get_tx_static(
                                              logical_dev->tx_nv_path[rf_band],
                                              rf_band);
    if ( tx_static_nv_cal_ptr == NULL )
    {
      RF_MSG_1 ( RF_ERROR, "rf_cdma_tx_agc_load_linearizer: "
                       "Failed to get valid multilin NV pointer "
                       "for device %d", device);
      init_status = FALSE;
    }
  }

  if ( init_status == TRUE )
  {
    rf_cdma_tx_cal_data = rf_cdma_nv_get_tx_cal(
                                              logical_dev->tx_nv_path[rf_band],
                                              rf_band );
    if ( rf_cdma_tx_cal_data == NULL )
    {
      RF_MSG_1 ( RF_ERROR, "rf_cdma_tx_agc_load_linearizer: "
                       "Failed to get valid tx cal NV pointer "
                       "for device %d", device );
      init_status = FALSE;
    }
  }

  /* Retrieve device info */
  init_status &= rf_cdma_tx_agc_get_device_info( device, rf_band,
                                                 &device_info );

  /* Allocate memory for linearizer and DPD data. Stack is not large enough
     for these to be local variables */
  /*--- Linearizer data ---*/
  computed_lin_data.lin_data = (tx_linearizer_table_type *)modem_mem_alloc(
                                              sizeof(tx_linearizer_table_type),
                                              MODEM_MEM_CLIENT_RFA );
  if(computed_lin_data.lin_data == NULL)
  {
    RF_MSG( RF_ERROR,"rf_cdma_tx_agc_load_linearizer: "
                     "Fail to allocate memory for processing linearizer");
    init_status = FALSE;
  }

  /* Initialize the search params that must be used to identify the
  correct linearizers to be used */
  tx_multilin_search_params.rfm_device = device;
  tx_multilin_search_params.tx_freq =
  rf_cdma_get_tx_carrier_freq(rf_band,rf_channel);

  if( init_status == TRUE )
  {
    boolean skip_pout_comp = FALSE ;
    if ( dev_status_w->temp_comp.init_trigger == TRUE &&
         ( dev_status_w->ept_online_enabled ) )
    {
      init_status &= rf_common_xpt_init_dpd( device );
    }

    /* Skip pout compensation in Traffic when FBRx is enabled */
    if ( ( dev_status_w->traffic_state == TRUE ) &&
         ( rf_cdma_get_pout_comp_skip ( device ) == TRUE ) &&
           rflm_autopin_sanity_check(dev_status_w->txlm_handle) )
    {
      skip_pout_comp = TRUE ;
      RF_MSG ( RF_LOW ,
               "rf_cdma_tx_agc_load_linearizer(): Pout Comp will be bypassed " ) ;
    }

    /* Update the linearizer data for each PA gain state */
    for ( pa_state = RFM_PA_GAIN_STATE_0;
          pa_state < RFCOM_TXLIN_NUM_LINEARIZERS;
          pa_state++)
    {
      /* Is data valid to use a good linearizer */
      boolean valid_linearizer_lut_data;
      boolean comp_success = FALSE ;
      /* 5 dB arbitrary is the limiting. This will prevent in doing erroneous
          compensation in case the MCDO offset calculation went wrong  */
          uint8 compensation_limit = 50 ; /* In dB10 */

      /* Using the same memory for processing each PA state so we should
         initialize the linearizer and dpd tables each time. */
      memset(computed_lin_data.lin_data,0,sizeof(tx_linearizer_table_type));
      computed_lin_data.aux_lin_data = TX_LINEARIZER_NO_AUX_DATA;
      tx_multilin_search_params.pa_range = pa_state;

      /* Check if this PA state has valid calibrated linearizer data */
      valid_linearizer_lut_data = rf_cdma_tx_agc_pa_state_is_valid(
                                          rf_band,
                                          rf_channel,
                                          waveform_type,
                                          tx_multilin_nv_cal_ptr,
                                          pa_state );

      if ( valid_linearizer_lut_data == TRUE )
      {
        /* ----------- Freq Comp + Temp Comp ----------------------------------*/
        if ( rf_cdma_debug_flags.disable_temp_comp == 0 )
        {
          dev_lin_data.lin_data = (dev_status_w->apt_swp_tx_linearizer + pa_state);
          dev_lin_data.aux_lin_data = dev_status_w->tx_lin_aux_data[pa_state];

          if ( dev_status_w->temp_comp.init_trigger == TRUE )
          {
            /* Freq comp the full-bias first sweep linearizer */
              dev_lin_data.lin_data =
                (dev_status_w->fb_swp_tx_linearizer + pa_state);
              tx_multilin_search_params.tx_waveform = TX_LINEARIZER_FIRST_SWEEP;
              valid_linearizer_lut_data &= rf_cdma_tx_agc_freq_comp(
                                                    device,
                                                    tx_multilin_search_params,
                                                    tx_multilin_nv_cal_ptr,
                                                    &dev_lin_data );

            /* Always freq comp the required waveform type data */
            dev_lin_data.lin_data =
              (dev_status_w->apt_swp_tx_linearizer + pa_state);
            tx_multilin_search_params.tx_waveform = waveform_type;
            valid_linearizer_lut_data &= rf_cdma_tx_agc_freq_comp(
                                                  device,
                                                  tx_multilin_search_params,
                                                  tx_multilin_nv_cal_ptr,
                                                  &dev_lin_data );
          }
          else
          {
            /*!No need to do freq comp, just indicate valid freq comp is done */
            valid_linearizer_lut_data &= TRUE;
          }

          if( valid_linearizer_lut_data )
          {
            /* Temperature compensation. If succeeds, the output tx_computed_linearizer
            will contain compensated values, otherwise, it is identical to the input*/
            rf_cdma_tx_temp_comp( (dev_status_w->fb_swp_tx_linearizer + pa_state),
                                  &dev_lin_data,
                                  &computed_lin_data,
                                  device,
                                  rf_band,
                                  rf_channel,
                                  pa_state,
                                  waveform_type ,
                                  skip_pout_comp ) ;
          }
        }
        /* ------------------------- Freq Comp Only ---------------------------*/
        else
        {
          tx_multilin_search_params.tx_waveform = waveform_type;
          valid_linearizer_lut_data &= rf_cdma_tx_agc_freq_comp(
                                                device,
                                                tx_multilin_search_params,
                                                tx_multilin_nv_cal_ptr,
                                                &computed_lin_data );
        }
        /* Retain auxiliary data information for future use
           ( Aux lin data is not a pointer ) */
        dev_status_w->tx_lin_aux_data[pa_state] = computed_lin_data.aux_lin_data;

        /* IF MCDO adjust for the lowered gain. Since this call is in loop for
           all PA states; this will be done for all PA states. */
        if ( waveform_type == TX_LINEARIZER_DATA_TYPE_2 )
        {

          /* ADD gain to the linearizer because for MCDO DAC IQ gain is lesser
            than SCDO so the linearizer for the same row will give lower power
            and therefore adjust the linearizer-power accordingly */
          comp_success = rf_cdma_txagc_shift_linearizer (
                                  computed_lin_data.lin_data ,
                                  dev_status_w->mcdo_vs_scdo_iq_gain_dB10 ,
                                  compensation_limit ) ;
          if ( comp_success == FALSE )
          {
            RF_MSG ( RF_HIGH , "rf_cdma_tx_agc_load_linearizer(): MCDO gain "
                       "adjustment not applied as rf_cdma_txagc_shift_linearizer() "
                       "failed. Skipping the full bias adjustment as well" ) ;
          }
        }

      	/* ET logging for Pout_vs_freq */
      	rfcommon_core_get_tx_linearizer_freq_comp_table(
                                                curr_tx_linearizer_freq_comp_table );
      	memscpy( dev_status_w->pout_comp_val_freq[pa_state],
                       sizeof(int8)*RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN,
                       curr_tx_linearizer_freq_comp_table,
                       sizeof(int8)*RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN );

      } /* if ( valid_linearizer_lut_data == TRUE ) */

      /* Update DPD type information into the PA state machine */
      pa_stm_data->dpd_type[pa_state] = computed_lin_data.aux_lin_data;

      /* Init DPD to invalid vals */
      memset( xpt_info.amam_bank_info, 0xFF,
              64*sizeof(rfcommon_xpt_dpd_bank_info_type) );
      memset( xpt_info.ampm_bank_info, 0xFF,
              64*sizeof(rfcommon_xpt_dpd_bank_info_type) );
      memset( xpt_info.epdt_bank_info, 0xFF,
              64*sizeof(rfcommon_xpt_dpd_bank_info_type) );

      if ( (dev_status_w->temp_comp.init_trigger == TRUE) &&
           (computed_lin_data.aux_lin_data != TX_LINEARIZER_NO_AUX_DATA) &&
           (valid_linearizer_lut_data == TRUE) &&
           (dev_status_w->ept_online_enabled == TRUE) )
      {
        /* Get DPD scaling factor */
        init_status &=
          rf_common_xpt_dpd_scale_computation(
               (xpt_dpd_scaling_data_type*)
                (&rf_cdma_tx_cal_data->xpt_dpd_scaling_data),
               (uint32*)
                (&tx_static_nv_cal_ptr->lin_temp_comp_data.tx_char_freq),
               tx_static_nv_cal_ptr->lin_temp_comp_data.tx_char_chan_size,
               tx_multilin_search_params.tx_freq,
               &dpd_scale_factor );

        /* Populate DPD from NV */
        init_status &=
          rfcommon_core_populate_tx_dpd( device,
                                         &computed_lin_data,
                                         &(xpt_info.amam_bank_info[0]),
                                         &(xpt_info.ampm_bank_info[0]),
                                         &(xpt_info.epdt_bank_info[0]),
                                         &num_rgi_with_dpd_info,
                                         &dpd_scale_factor,
                                         NULL );

        if ( FALSE == init_status )
        {
          RF_MSG ( RF_HIGH , "rf_cdma_tx_agc_load_linearizer(): "
                              "Call to rfcommon_core_populate_tx_dpd() has failed.." ) ;
          break ;
        }
        else
        {
        /* If DPD is populated for any one linearizer, set this flag to TRUE */
        is_dpd_populated |= TRUE;
        }
      }

      if ( valid_linearizer_lut_data  == TRUE )
      {
        rfcom_band_type_u rfcom_band;

        /* Initialize first sweep linearizer data */
        fb_lin_data.lin_data = dev_status_w->fb_swp_tx_linearizer + pa_state;
        fb_lin_data.aux_lin_data = TX_LINEARIZER_NO_AUX_DATA;

        /* If MCDO and if the computed Linearizer update went through. */
        if ( ( comp_success == TRUE ) &&
             ( waveform_type == TX_LINEARIZER_DATA_TYPE_2 ) )
        {
          (void) /* If the 1st comp passed so will the 2nd. Skip Check */
          rf_cdma_txagc_shift_linearizer ( fb_lin_data.lin_data ,
                                           dev_status_w->mcdo_vs_scdo_iq_gain_dB10 ,
                                           compensation_limit ) ;
          RF_MSG_1 ( RF_LOW , "rf_cdma_tx_agc_load_linearizer(): Computed and FB Lin "
                     " shifted for MCDO by %d dB10 " ,
                     dev_status_w->mcdo_vs_scdo_iq_gain_dB10 ) ;
        }

        /* Initialize common band information */
        rfcom_band.cdma_band = rf_band;

        /* Initialize PA STM info */
        pa_stm_info.pa_state = pa_state;
        pa_stm_info.pa_range = rf_cdma_map_pa_state_to_gain_range( device,
                                                                   rf_band,
                                                                   pa_state ) ;

        /* If init trigger is TRUE, then load entire linearizer to RFLM */
        if ( dev_status_w->temp_comp.init_trigger == TRUE )
        {
        /* Configure the linearizer to RFLM through common Tx AGC */
          init_status &= rfcommon_core_txlin_config_init_linearizer(
                             mode, device,
                             rfcom_band, txlm_handle,
                             &computed_lin_data,
                             &fb_lin_data,
                             &device_info,
                             &pa_stm_info,
                             (void*)&xpt_info );
      }
        /* If init trigger is FALSE, then only update dynamic
        linearizer interface */
        else
        {
          /* Configure the linearizer to RFLM through common Tx AGC */
          init_status &= rfcommon_core_txlin_config_update_linearizer(
                               mode, device,
                               rfcom_band, txlm_handle,
                               &computed_lin_data,
                               &fb_lin_data,
                               &pa_stm_info ,
                               skip_pout_comp ) ;
        }
      }

      if ( FALSE == init_status )
      {
        RF_MSG ( RF_ERROR , "rf_cdma_tx_agc_load_linearizer(): "
                            "Call to rf_cdma_tx_agc_process_linearizer_for_fw() has failed..." ) ;
        break ;
      }
    } /* for ( pa_state = RFM_PA_GAIN_STATE_0;
         pa_state < RF_CDMA_TX_LINEARIZER_NUM;
         pa_state++ ) */

    if ( ( dev_status_w->temp_comp.init_trigger == TRUE ) &&
         ( dev_status_w->ept_online_enabled ) &&
         ( is_dpd_populated ) )
    {
      init_status &= rf_common_xpt_commit_dpd_data( device );
    }

  } /* if( init_status == TRUE ) */

  if( computed_lin_data.lin_data != NULL )
  {
    modem_mem_free(computed_lin_data.lin_data, MODEM_MEM_CLIENT_RFA);
    computed_lin_data.lin_data = NULL;
  }

  return init_status;

} /* rf_cdma_tx_agc_load_linearizer */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return whether the PA state is valid for the current band based on calibrated
  linearizer support

  @details
  The max PA state is defined to be the highest PA state for which a valid
  calibrated linearizer is available in multi-lin NV. This function determines
  if any PA state requested is a valid PA state for Tx operation

  @param band
  RF Band for which linearizer is being configured

  @param channel
  RF Channel for which linearizer is being configured

  @param tx_waveform
  Tx waveform type for which NV must be checked

  @param tx_band_cal_data
  Pointer to multi-linearizer NV for current configuration

  @param pa_state
  PA gain state which is to be queried

  @return
  TRUE if the PA state is valid, else FALSE

*/
boolean
rf_cdma_tx_agc_pa_state_is_valid
(
  rfm_cdma_band_class_type const band,
  rfm_cdma_chan_type const channel,
  tx_linearizer_waveform_type tx_waveform,
  const tx_band_cal_type *tx_band_cal_data,
  rfm_pa_gain_state_type pa_state
)
{
  uint8 freq_index;
  /* Target Tx frequency */
  uint32 tx_freq;
  /* Tx cal data linearizer index given waveform type and PA range */
  const tx_multi_linearizer_index_node_type* tx_cal_data_index;

  /* Target Tx frequency */
  tx_freq = rf_cdma_get_tx_carrier_freq( band, channel );

  if ( tx_band_cal_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_tx_agc_pa_state_is_valid: NULL Pointer to "
                      "band cal data." );
    return FALSE;
  }

  /* Tx cal data linearizer index given waveform type and PA State */
  tx_cal_data_index =
    &tx_band_cal_data->
     tx_pa_state_cal_data[pa_state].linearizer_index[tx_waveform][0];

  if ( tx_cal_data_index == NULL )
  {
    RF_MSG_1( RF_MED, "rf_cdma_tx_agc_pa_state_is_valid: "
                      "PA state %d is invalid",
              pa_state );
    return FALSE;
  }

  /* Loop through 16 channels to find the closest linearizer for the given
  Tx frequency*/
  for ( freq_index = 0;
        freq_index < RFCOM_TXLIN_NUM_CHANNELS;
        freq_index++ )
  {
    if ( (tx_freq <= tx_cal_data_index[freq_index].upper_bound_freq) &&
         (tx_cal_data_index[freq_index].node_data.lin_data != NULL) )
    {
      /* match linearizer table found */
      break;
    }
  }

  if ( (freq_index == RFCOM_TXLIN_NUM_CHANNELS) ||
       (tx_cal_data_index[freq_index].node_data.lin_data == NULL) )
  {
    /* This PA state does not have a valid linearizer for this
    frequency */
    RF_MSG_1( RF_MED, "rf_cdma_tx_agc_pa_state_is_valid: "
                      "PA state %d is invalid",
              pa_state );
    return FALSE;
  }
  else
  {
    /* Found a valid linearizer for the current frequency for this PA
    state. So this PA state can now be considered the current max.
    Move to next PA state for evaluation */
    RF_MSG_1( RF_LOW, "rf_cdma_tx_agc_pa_state_is_valid: PA state %d is valid",
              pa_state );
  }

  return TRUE;

} /* rf_cdma_tx_agc_pa_state_is_valid */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Calculate the therm offset according to therm reading and update tx_lin_LUT

  @detail
  A new therm offset (int8) is obtained through interpolation for a given therm
  reading(uint16) for the given PA state. Then the new tx_lin_LUT is updated
  based on the new offset for the PA state.

  @param tx_lin_data_fb_swp
  Pt to the input linearizer for the given PA state, based on first sweep
  cal data

  @param tx_lin_data_apt_swp
  Pt to the input linearizer for the given PA state, based on second sweep
  cal data

  @param tx_lut_after
  Pt to the updated tx_lin_LUT for the given PA state;

  @param device
  The device the current operation is on;

  @param band
  The band of the current operation;

  @param channel
  The current channel of operation, required for lin vs temp vs freq
  compensation

  @param pa_state
  The current PA state

  @return TURE if tx_temp_comp succeeded
              FALSE otherwise;
*/
boolean
rf_cdma_tx_temp_comp
(
  tx_linearizer_table_type *tx_lin_data_fb_swp,
  tx_linearizer_table_data_type *tx_lin_data_apt_swp,
  tx_linearizer_table_data_type *tx_lut_after,
  rfm_device_enum_type const device,
  rfm_cdma_band_class_type band,
  uint16 channel,
  rfm_pa_gain_state_type pa_state,
  tx_linearizer_waveform_type waveform_type ,
  boolean skip_pout_comp
)
{
  /* Logical Device */
  const rfc_cdma_logical_device_params_type *logical_dev;
  /* pointer to cdma NV item*/
  const rf_1x_config_type* rf_1x_config_ptr;
  /* Multilinearizer NV data retrieval */
  const tx_band_cal_type *tx_multilin_nv_cal_ptr = NULL;
  /* Device Status Write Pointer */
  rf_cdma_data_status_type *dev_status_w;
  boolean temp_comp_status = TRUE;
  tx_linearizer_table_data_type rf_cdma_tx_lin_first_sweep;

  int16 pout_comp_offset;
  int16 pin_comp_offset;
  tx_lin_vs_temp_vs_freq_comp_param_type temp_vs_freq_params;
  uint32 ref_lin_freq;

  /* Initialize first sweep data before providing to pin compensation */
  rf_cdma_tx_lin_first_sweep.lin_data = tx_lin_data_fb_swp;
  rf_cdma_tx_lin_first_sweep.aux_lin_data = TX_LINEARIZER_NO_AUX_DATA;

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    size_t data_length = sizeof(tx_linearizer_table_type) ;
    RF_MSG( RF_ERROR, "rf_cdma_tx_temp_comp: "
                      "Null pointer returned for logical_dev");

    /* Copy over data for early return */
    memscpy ( tx_lut_after->lin_data, data_length ,
             tx_lin_data_apt_swp->lin_data , data_length ) ;
    /* Retain Aux data status */
    tx_lut_after->aux_lin_data = tx_lin_data_apt_swp->aux_lin_data;

    return TRUE;
  }

  /* Gets a modifiable pointer, but stores it as a pointer to const
  to prevent changing anything. */
  rf_1x_config_ptr = rf_cdma_nv_get_1x_data( logical_dev->tx_nv_path[band],
                                             band );

  if ( rf_1x_config_ptr == NULL )
  {
    size_t data_length = sizeof(tx_linearizer_table_type) ;
    RF_MSG( RF_ERROR, "rf_cdma_tx_temp_comp:  "
                      "Null pointer returned for rf_1x_config_ptr");

    /* Copy over data for early return */
    memscpy ( tx_lut_after->lin_data , data_length ,
              tx_lin_data_apt_swp->lin_data , data_length ) ;
    /* Retain Aux data status */
    tx_lut_after->aux_lin_data = tx_lin_data_apt_swp->aux_lin_data;

    return TRUE;
  }

  /* Get a pointer to the Tx calibration data */
  tx_multilin_nv_cal_ptr = rf_cdma_nv_get_tx_multilin_cal(
                                          logical_dev->tx_nv_path[band],
                                          band );

  /* Null pointer check */
  if ( tx_multilin_nv_cal_ptr == NULL )
  {
    size_t data_length = sizeof(tx_linearizer_table_type) ;
    RF_MSG_1 ( RF_ERROR, "rf_cdma_tx_temp_comp: "
                         "Failed to get valid multilin NV pointer "
                         "for device %d", device);

    /* Copy over data for early return */
    memscpy ( tx_lut_after->lin_data , data_length ,
             tx_lin_data_apt_swp->lin_data , data_length ) ;
    /* Retain Aux data status */
    tx_lut_after->aux_lin_data = tx_lin_data_apt_swp->aux_lin_data;

    return TRUE;
  }

  /* Sanity check for tx_therm_cal*/
  if ( rf_1x_config_ptr->cdma_common_ptr->tx_cal.
       therm_cal_data.is_therm_cal_nv_data_valid == FALSE )
  {
    size_t data_length = sizeof(tx_linearizer_table_type) ;
    RF_MSG( RF_ERROR, "rf_cdma_tx_temp_comp:  "
                      "Therm Cal NV data is invalid");

    /* Copy over data for early return */
    memscpy ( tx_lut_after->lin_data, data_length ,
              tx_lin_data_apt_swp->lin_data , data_length ) ;
    /* Retain Aux data status */
    tx_lut_after->aux_lin_data = tx_lin_data_apt_swp->aux_lin_data;

    return TRUE;
  }

  dev_status_w = rf_cdma_get_mutable_device_status( device );

  /* NULL pointer check */
  if ( dev_status_w == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_tx_temp_comp: "
                        "Null device data for device %d ",device);
    return FALSE;
  }/*if ( dev_status_w == NULL )*/
  else
  {
    /* Obtain the linearizer compensation offset from temp vs freq NV */

    /* If last temp comp read failed, skip temp comp by setting offsets
    to zero; otherwise, do interpolation */
    if ( dev_status_w->temp_comp.is_last_reading_valid == TRUE )
    {
      temp_vs_freq_params.freq_params.tx_freq =
        rf_cdma_get_tx_carrier_freq( band,
                                     channel );
      temp_vs_freq_params.freq_params.waveform_type = waveform_type;
      rfcommon_core_txlin_get_ref_linearizer_frequency(
                               (tx_band_cal_type*)tx_multilin_nv_cal_ptr,
                               temp_vs_freq_params.freq_params.tx_freq,
                               pa_state,
                               waveform_type,
                               &ref_lin_freq );
      temp_vs_freq_params.freq_params.ref_lin_tx_freq = ref_lin_freq;

      temp_vs_freq_params.temp_params.temp_read =
        dev_status_w->temp_comp.last_temp_sensor_read;
      temp_vs_freq_params.temp_params.ref_lin_temp_adc =
                               tx_multilin_nv_cal_ptr->ref_lin_temp_adc;

      temp_vs_freq_params.tx_multilin_nv_cal_ptr =
                             (tx_band_cal_type*)tx_multilin_nv_cal_ptr;
      temp_vs_freq_params.pa_state = pa_state;
      temp_vs_freq_params.ref_xpt_pin_offset = 0;

      if( tx_lin_data_apt_swp->aux_lin_data == TX_LINEARIZER_ET_DPD_DATA )
      {
       temp_vs_freq_params.ref_xpt_pin_offset =
         (int32)rf_cdma_xpt_get_cfg_param( device, XPT_CFG_CMN_REF_ET_PIN_OFFSET );
      }
      else if( tx_lin_data_apt_swp->aux_lin_data == TX_LINEARIZER_EPT_DPD_V2_DATA )
      {
       temp_vs_freq_params.ref_xpt_pin_offset =
         (int32)rf_cdma_xpt_get_cfg_param( device, XPT_CFG_CMN_REF_EPT_PIN_OFFSET );
      }

      temp_vs_freq_params.auto_pin_enable = (rfcommon_autopin_is_enabled(dev_status_w->temp_comp.mode) && rflm_autopin_sanity_check(dev_status_w->txlm_handle));

      /* Get compensation values */
      rfcommon_core_txlin_get_temp_compensation(
         dev_status_w->temp_comp.temp_comp_algo,
         &pin_comp_offset,
         &pout_comp_offset,
         &temp_vs_freq_params,
         &rf_1x_config_ptr->cdma_common_ptr->tx_static.lin_temp_comp_data );
    }
    else
    {
      pout_comp_offset = 0;
      pin_comp_offset = 0;
    }



    /* Skip Pout Compensation when in traffic and FBRx is enabled */
    if ( ( dev_status_w->traffic_state == TRUE ) &&
		( rf_cdma_get_pout_comp_skip ( device ) == TRUE ) &&
	   rflm_autopin_sanity_check(dev_status_w->txlm_handle) ) 

    {
      skip_pout_comp = TRUE ;
      return TRUE;
    }
    else
    {
      skip_pout_comp = FALSE ;
    }

    RF_MSG_3( RF_MED, "rf_cdma_tx_temp_comp: Pin offset = %d, "
                      "Pout offset = %d skip_or_not:%d",
              pin_comp_offset, pout_comp_offset, skip_pout_comp );

    temp_comp_status &=
      rfcommon_core_txlin_temp_vs_freq_comp_update_v2(
                                 dev_status_w->temp_comp.temp_comp_algo,
                                 tx_lin_data_apt_swp,
                                 tx_lut_after,
                                 &rf_cdma_tx_lin_first_sweep,
                                 pin_comp_offset,
                                 pout_comp_offset,
                                 RF_CDMA_MDSP_TX_LIN_TBL_SIZ ,
                                 skip_pout_comp ) ;

    /* If temp comp update failed, we need to copy the input tbl to output tbl*/
    if ( temp_comp_status == FALSE )
    {
      size_t data_length = sizeof(tx_linearizer_table_type) ;
      /* Copy over data */
      memscpy ( tx_lut_after->lin_data , data_length  ,
               tx_lin_data_apt_swp->lin_data , data_length ) ;
      /* Retain Aux data status */
      tx_lut_after->aux_lin_data = tx_lin_data_apt_swp->aux_lin_data;
    }

    dev_status_w->pin_comp_val_temp_vs_freq[pa_state] = pin_comp_offset;
    if ( skip_pout_comp == FALSE )
    { /* Log the Temp part of Pout only if it is not being skipped */
    dev_status_w->pout_comp_val_temp_vs_freq[pa_state] = pout_comp_offset;
    }

  }/*if ! ( dev_status_w == NULL )*/

  return TRUE;

} /* rf_cdma_tx_temp_comp */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update tx agc for a given device

  @details
  This API will update all necessary tx agc parameter for a given device which
  is in TXRX mode.

  @param device
  RFM Device in operation

  @return
  TRUE if the device is eligible for re-loading and re-loading is successful;
  FAIL otherwise;
*/
boolean
rf_cdma_update_tx_agc
(
  rfm_device_enum_type device
)
{
  rf_cdma_data_status_type *dev_status = NULL;
  const rfc_cdma_logical_device_params_type *logical_dev;
  boolean status = TRUE;
  rfm_cdma_band_class_type rf_band;

  /* NV related variables */
  const rf_1x_config_type *rf_1x_config_ptr;   /* Pointer to 1x RF NV data */
  const rf_hdr_config_type *rf_hdr_config_ptr; /* Pointer to hdr RF NV data*/

  /* Get device Status */
  dev_status = rf_cdma_get_mutable_device_status(device);

  /* Get the logical device*/
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL || dev_status == NULL )
  {
    /* If this physical device can't be translated to a logical device,
        then no band is supported. */
    RF_MSG_1( RF_ERROR, "rf_cdma_update_tx_agc: Invalid Dev = %d", device );

    return FALSE;
  }

  if ( dev_status->temp_comp.init_done == FALSE )
  {
    RF_MSG( RF_ERROR, "rf_cdma_update_tx_agc: temp comp critical section is not"
            "init, stop update tx agc" );

    return FALSE;
  }

  /*enter temp comp's critical section protection in order to prevent collison
  with temp comp tx agc update*/
  rf_common_enter_critical_section(
    &(dev_status->temp_comp.crit_section) );

  /* rf state and tx_stop status check*/
  if ( !( (dev_status->data_1x.rf_state == RF_CDMA_STATE_RXTX)
          ||
          (dev_status->data_hdr.rf_state == RF_CDMA_STATE_RXTX) )
       ||
       (dev_status->temp_comp.temp_comp_status == FALSE) )
  {
    RF_MSG_2( RF_ERROR, "rf_cdma_update_tx_agc: Invalid 1x state = %d/hdr state %d",
              dev_status->data_1x.rf_state, dev_status->data_hdr.rf_state );

    rf_common_leave_critical_section( &(dev_status->temp_comp.crit_section) );

    return FALSE;
  }

  /*Get current band */
  rf_band = dev_status->curr_band;

  /*Get hdr nv pointer*/
  rf_hdr_config_ptr = rf_cdma_nv_get_hdr_data( logical_dev->tx_nv_path[rf_band],
                                               rf_band );

  /* Get 1x nv pointer */
  rf_1x_config_ptr = rf_cdma_nv_get_1x_data( logical_dev->tx_nv_path[rf_band],
                                             rf_band );

  /*hdr and 1x nv pointer check*/
  if ( NULL == rf_hdr_config_ptr || NULL == rf_1x_config_ptr )
  {
    RF_MSG_1 (RF_ERROR, "rf_cdma_update_tx_agc: Inviald hdr nv pointer"
              " from NV for band %d", rf_band);

    rf_common_leave_critical_section( &(dev_status->temp_comp.crit_section) );

    return FALSE;
  }

  if ( dev_status->rf_mode == RFM_1X_MODE )
  {
    RF_MSG(RF_MED, "rf_cdma_update_tx_agc: start reloading 1x tx agc");

    status &= rf_1x_configure_tx_agc(device,
                                     rf_band,
                                     dev_status->lo_tuned_to_chan,
                                     dev_status->txlm_handle,
                                     rf_1x_config_ptr);

  }
  else if ( dev_status->rf_mode == RFM_1XEVDO_MODE )
  {
    RF_MSG(RF_MED, "rf_cdma_update_tx_agc: start reloading hdr tx agc");
    status &=rf_hdr_configure_tx_agc(device,
                                     dev_status->num_carriers,
                                     rf_band,
                                     dev_status->lo_tuned_to_chan,
                                     dev_status->txlm_handle,
                                     rf_hdr_config_ptr);
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_update_tx_agc: Invalid Tech = %d",
              dev_status->rf_mode );
    status = FALSE;
  }

  rf_common_leave_critical_section( &(dev_status->temp_comp.crit_section) );

  return status;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Populate Tx AGC configuration to RFLM through common Tx AGC configuration

  @details
  Provides functionality to query RFC specific information for devices and
  passes on the information to common Tx AGC configuration. Common TxAGC
  will then build Tx AGC sequence information to be used by RFLM TxAGC

  @param device
  RFM device

  @param band
  RFM CDMA band

  @param txlm_handle
  Handle that points to the RFLM buffers

  @return
  TRUE if successful, else FALSE
*/
boolean
rf_cdma_tx_agc_config
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  lm_handle_type txlm_handle
)
{
  boolean api_status = TRUE;
  rfcom_band_type_u rf_band;
  rfcommon_core_txlin_agc_device_info device_info;

  /* RF mode. 1x or HDR */
  rfm_mode_enum_type mode = rf_cdma_data_get_rf_mode( device ) ;

  /* Retrieve device info */
  api_status &= rf_cdma_tx_agc_get_device_info( device, band,
                                                &device_info );

  /* Update common band information */
  rf_band.cdma_band = band;

  { /* using this to limit scope of temp_timing_param_ptr */
    rfcommon_core_txlin_agc_timing_params* temp_timing_param_ptr =
                                  rf_cdma_data_get_txagc_timing_params(device) ;

    if ( temp_timing_param_ptr != NULL )
    {
      /* Call common Tx AGC configuration */
      api_status &=  rfcommon_core_txlin_config_agc(
                                   mode, device, rf_band,
                                   txlm_handle,
                                   0 /* CDMA uses script buffer 0 */,
                                   device_info.wtr_device,
                                   device_info.pa_device,
                                   device_info.papm_device,
                                   NULL, temp_timing_param_ptr );
    }
    else
    {
      api_status = FALSE;
      RF_MSG( RF_ERROR,
              "rf_cdma_tx_agc_config: call to "
              " rf_cdma_data_get_txagc_timing_params() returned NULL" );
    }
  } /* using this to limit scope of temp_timing_param_ptr */

  return api_status;

} /* rf_cdma_tx_agc_config */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Retrieve CDMA device pointers as required by Tx AGC configuration

  @details
  Provides functionality to query RFC specific information for devices and
  passes on the information to common Tx AGC configuration

  @param device
  RFM device

  @param band
  RFM CDMA band

  @return
  TRUE if successful, else FALSE
*/
boolean
rf_cdma_tx_agc_get_device_info
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfcommon_core_txlin_agc_device_info *device_info
)
{
  boolean api_status = TRUE;
  rfdevice_rxtx_common_class* device_ptr = NULL;
  /* Logical Device */
  const rfc_cdma_logical_device_params_type *logical_dev = NULL;

  if ( device_info == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_tx_agc_get_device_info: Bad device_info ptr ");
    return FALSE;
  }
  /* Memset the device info pointer to zero, in order to ensure that
  NULL pointers are returned in the event of failure */
  memset( device_info, 0, sizeof(rfcommon_core_txlin_agc_device_info) );

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_tx_agc_get_device_info: "
                      "Null logical dev pointer ");
    return FALSE;
  }

  if ( device_info == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_tx_agc_get_device_info: "
                      "Null device info pointer ");
    return FALSE;
  }

  if ( logical_dev->pa[band] == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_tx_agc_get_device_info: "
                      "Null PA device not supported" );
    api_status &= FALSE;
  }
  else
  {
    /* Get PA device object from RFC */
    device_info->pa_device = get_cdma_pa_device ( logical_dev->pa[band] );
  }

  /* Get the common device pointer for the Tx device */
  device_ptr = rfdevice_cdma_tx_get_common_device_pointer(
                                                 logical_dev->tx_device[band] );

  if ( device_ptr != NULL )
  {
    device_info->wtr_device = (void*)device_ptr;
  }
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_tx_agc_get_device_info: "
                      "Null WTR device not supported" );
    api_status &= FALSE;
  }

  if ( logical_dev->qpoet_obj[band] != NULL )
  {
    /* Get pointer to qpoet device */
    device_info->papm_device = logical_dev->qpoet_obj[band];
  }

  return api_status;

} /* rf_cdma_tx_agc_get_device_info */

/*-----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the current Pin Compensation value for logging

  @details
  Get the current PA State and using Lineariser table, find the current Pin
  Compensation value.

  @param device
  RFM Device for which the Tx Lin Index is queried

  @param pa_state
  The PA State for which to obtain the corresponding LUT index.

  @return current Pin Compensation value
*/
int16 rf_cdma_tx_agc_get_pin_comp_val
(
  rfm_device_enum_type device,
  uint8 pa_state
)
{
  const rf_cdma_data_status_type *dev_status_r = NULL;
  int16 pin_comp_val = 0;

  if (pa_state >= RFCOM_TXLIN_NUM_LINEARIZERS)
  {
    RF_MSG_1( RF_ERROR, "Invalid PA=%d", pa_state );
    return pin_comp_val;
  }
  /* Get device Status */
  dev_status_r = rf_cdma_get_device_status(device);

  /* NULL pointer check */
  if ( dev_status_r == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_tx_agc_get_pin_comp_val: Invalid Dev %d Data",
              device );
    return pin_comp_val;
  }

  pin_comp_val = dev_status_r->pin_comp_val_temp_vs_freq[pa_state];

  return pin_comp_val;
}/* rf_cdma_tx_agc_get_pin_comp_val */

/*-----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the current Pout Compensation value for logging

  @details
  Get the current PA State and using Lineariser table, find the current Pout
  Compensation value.

  @param device
  RFM Device for which the Tx Lin Index is queried

  @param pa_state
  The PA State for which to obtain the corresponding LUT index.

  @param fw_rgi
  The mdsp array index corresponding to the LUT index.

  @return current Pout Compensation value
*/
int16 rf_cdma_tx_agc_get_pout_comp_val
(
  rfm_device_enum_type device,
  uint8 pa_state,
  uint16 fw_rgi
)
{
  const rf_cdma_data_status_type *dev_status_r = NULL;
  int16 pout_comp_val = 0;
  uint16 fcomp_idx = 0xFFFF;

  /* Get device Status */
  dev_status_r = rf_cdma_get_device_status(device);

  /* NULL pointer check */
  if ( dev_status_r == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_tx_agc_get_pout_comp_val: Invalid Dev %d Data",
              device );
    return pout_comp_val;
  }

  if (( pa_state >= RFCOM_TXLIN_NUM_LINEARIZERS )
  	        || ( fw_rgi >= RFCOM_TXLIN_NUM_SEGMENTS ))
  {
    RF_MSG_2( RF_ERROR, "Invalid PA=%d range or RGI index = %d",
		                                 pa_state, fw_rgi);
    return pout_comp_val;
  }

  fcomp_idx =
        dev_status_r->apt_swp_tx_linearizer[pa_state].table[fw_rgi].fcomp_idx;

  if ( fcomp_idx >= RFCOM_TXLIN_NUM_FREQCOMP_RGI_BIN )
  {
    RF_MSG_1( RF_ERROR, "Invalid Freq Comp Bin=%d",fcomp_idx );
    return pout_comp_val;
  }
  pout_comp_val = dev_status_r->pout_comp_val_temp_vs_freq[pa_state] +
  	              dev_status_r->pout_comp_val_freq[pa_state][fcomp_idx];

  return pout_comp_val;
}/* rf_cdma_tx_agc_get_pout_comp_val */

/*-----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns EPT/ET DPD index for logging

  @details
  Get the current PA State and using Lineariser table, find the current DPD index

  @param device
   RFM Device for which the Tx Lin Index is queried

   @param pa_state
   The PA State for which to obtain the corresponding LUT index.

   @param fw_rgi
   The mdsp array index corresponding to the LUT index.

   @return
   DPD index
*/
uint16 rf_cdma_tx_agc_get_dpd_index
(
  rfm_device_enum_type device,
  rfm_pa_gain_state_type pa_state,
  uint16 fw_rgi
)
{
  const rf_cdma_data_status_type *dev_status_r = NULL;
  uint16 dpd_index = 0xFFFF;

  /* Validate the argument */
  if( (pa_state >= RFM_PA_GAIN_STATE_NUM) ||
    (fw_rgi >= RFCOM_TXLIN_NUM_SEGMENTS ) )
  {
    RF_MSG_3( RF_ERROR, "rf_cdma_tx_agc_get_dpd_index: Dev %d argument - "
  					"PA Index %d, FW RGI %d", device, pa_state, fw_rgi );
    return dpd_index;
  }

  /* Get device Status */
  dev_status_r = rf_cdma_get_device_status(device);

  /* NULL pointer check */
  if ( dev_status_r == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_tx_agc_get_dpd_index: Invalid Dev %d Data",
  			device );
    return dpd_index;
  }

  dpd_index = dev_status_r->apt_swp_tx_linearizer[pa_state].table[fw_rgi].dpd_index;

  return dpd_index;
}/* rf_cdma_tx_agc_get_dpd_index */

/*-----------------------------------------------------------------------------*/
/*!
  @brief
  This function populates the threshold for turning on/off predictive clipper
  into xpt_config_params based on pa switchpoints

  @param rf-band
  Band info for TxAGC conversion

  @param pa_stm_data
  PA state machine data with PA switchpoints

  @param xpt_config_params
  Output param for thresholds

  @return
  TRUE if succeed. FALSE if fail
*/
uint16
rf_cdma_txagc_get_pred_clip_thresh
(
  rfm_cdma_band_class_type const rf_band,
  rf_cdma_mdsp_pa_stm_config_type* pa_stm_data,
  rf_cdma_mdsp_xpt_config_type* xpt_config_params
)
{
  if ( pa_stm_data == NULL || xpt_config_params == NULL)
  {
    RF_MSG(RF_ERROR,"rf_cdma_txagc_get_pred_clip_thresh: NULL Pointer!");
    return FALSE;
  }

  xpt_config_params->pred_clip_on =
    rf_cdma_mdsp_dbm10_to_txagc( pa_stm_data->pred_clip_en_thresh,
                                 rf_cdma_get_turnaround_constant(rf_band)*10 );
  xpt_config_params->pred_clip_off =
    rf_cdma_mdsp_dbm10_to_txagc( pa_stm_data->pred_clip_dis_thresh,
                                 rf_cdma_get_turnaround_constant(rf_band)*10 );

  RF_MSG_4(RF_MED,"rf_cdma_txagc_get_pred_clip_thresh: "
                  "Pred Clip Threshold: on %d (%d dBm10), "
                  "off %d (%d dBm10)",
                  xpt_config_params->pred_clip_on,
                  pa_stm_data->pred_clip_en_thresh,
                  xpt_config_params->pred_clip_off,
                  pa_stm_data->pred_clip_dis_thresh );
  return TRUE;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function update the Tx linearizer using AutoPin

  @details
  Initial linearizer on the current band/channel needs to be populated first

  @param device
  Tx device that enabled Auto Pin

  @param pin_comp_val
  Pin compensation value

  @param pa_index
  PA state

*/
boolean
rf_cdma_tx_agc_autopin_update
(
  rfm_device_enum_type device,
  int16 pin_comp_val,
  uint8 pa_index
)
{
  boolean status_flag = FALSE;
  tx_linearizer_table_data_type txlin_first_sweep_tbl;
  tx_linearizer_table_data_type txlin_final_table;
  rf_cdma_data_status_type *dev_status = NULL;
  const rfc_cdma_logical_device_params_type *logical_dev = NULL;
  sys_channel_type band_chan;
  tx_linearizer_table_data_type txlin_curr_table;
  rfcommon_core_txlin_agc_pa_stm_info pa_stm_info;
  rfcom_band_type_u band_info;

  RF_MSG_2( RF_MED, "rf_cdma_tx_agc_autopin_update: Pin: %d, PA state: %d",
            pin_comp_val, pa_index );

  /*!
   *   Get device data structs
   * */
  dev_status = rf_cdma_get_mutable_device_status( device );
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if(( NULL != dev_status )&&( NULL != logical_dev ))
  {
    band_info.cdma_band = dev_status->curr_band;

    /*! TX Lin table memory allocations and initialization */
    txlin_first_sweep_tbl.lin_data = dev_status->fb_swp_tx_linearizer + pa_index;
    if(txlin_first_sweep_tbl.lin_data != NULL)
    {
      /* Initialize first sweep data before providing to pin compensation */
      /* Clear current first sweep table for population */
      txlin_first_sweep_tbl.aux_lin_data = TX_LINEARIZER_NO_AUX_DATA ;
      txlin_first_sweep_tbl.xpt_data = NULL;

      txlin_final_table.lin_data = (tx_linearizer_table_type *)
            modem_mem_alloc(sizeof(tx_linearizer_table_type),MODEM_MEM_CLIENT_RFA );

      if(txlin_final_table.lin_data != NULL)
      {
        /* Clear final table for population */
        memset(txlin_final_table.lin_data, 0, sizeof(tx_linearizer_table_type));
        txlin_final_table.aux_lin_data = TX_LINEARIZER_NO_AUX_DATA;
        txlin_final_table.xpt_data = NULL;

        /* All the preparation is done, start to process Pin */
        txlin_curr_table.lin_data = (dev_status->apt_swp_tx_linearizer + pa_index);
        txlin_curr_table.aux_lin_data = dev_status->tx_lin_aux_data[pa_index];

        status_flag = rfcommon_core_txlin_temp_vs_freq_comp_update(
                                      (tx_multi_lin_rev_type)dev_status->temp_comp.temp_comp_algo,
                                      &txlin_curr_table,
                                      &txlin_final_table,
                                      &txlin_first_sweep_tbl,
                                      pin_comp_val,
                                      0, RF_CDMA_MDSP_TX_LIN_TBL_SIZ,
                                      TRUE /* skip_Pout_comp ? */ );

        if( TRUE == status_flag )
        {
          /* Initialize PA STM info */
          pa_stm_info.pa_state = pa_index;
          pa_stm_info.pa_range = rf_cdma_map_pa_state_to_gain_range( device,
                                                        band_info.cdma_band,
                                                        pa_index ) ;

          /* Load the TX linearizer table (pertaining to the current Tx chan)
             for each PA range into MDSP */
          status_flag = rfcommon_core_txlin_config_update_linearizer(
                                      dev_status->rf_mode, device,
                                      band_info,
                                      dev_status->txlm_handle,
                                      &txlin_final_table,
                                      &txlin_first_sweep_tbl,
                                      &pa_stm_info, TRUE );

          if( FALSE == status_flag )
          {
            RF_MSG_1( RF_ERROR,
                      "rf_cdma_tx_agc_autopin_update: update MDSP failure %d",
                      pa_index );
          }
        }
        else
        {
          RF_MSG_2( RF_ERROR,
                    "rf_cdma_tx_agc_autopin_update: temp comp populate "
                    "failure for pa_index = %d pin = %d",
                    pa_index, pin_comp_val);
        }

        /*Deallocate the final lin data */
        modem_mem_free(txlin_final_table.lin_data, MODEM_MEM_CLIENT_RFA);
        txlin_final_table.lin_data = NULL;
      }
      else
      {
        RF_MSG( RF_ERROR, "rf_cdma_tx_agc_autopin_update: "
                "Fail to allocate memory for processing linearizer final");
      }
    }
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_tx_agc_autopin_update: "
              "Fail to allocate memory for "
              "processing linearizer sweep" );
    }
  }
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_tx_agc_autopin_update: "
            "Fail to get device status");
  }

  RF_MSG_1( RF_MED, "rf_cdma_tx_agc_autopin_update: status: %d", status_flag);

  return status_flag;
}




#endif /* FEATURE_CDMA1X */
/*! @} */
