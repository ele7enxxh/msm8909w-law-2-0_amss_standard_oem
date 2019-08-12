
#ifndef RFCOMMON_CORE_TXLIN_H
#define RFCOMMON_CORE_TXLIN_H
/*! @brief!
   @file
   rfcommon_core_txlin.h

   @brief
   Provides common TX Linearizer Functionality

   @details
    

*/

/*! @brief===========================================================================

Copyright (c) 2011 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_core_txlin.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/31/14   ndb     Added support for extended EARFCN for LTE
07/25/14   cd      Add Tx linearizer data logging support and provide interface
                   for tech to query
07/10/14   cd/cri  Merge Pin Cal
04/11/14   cd      Tx AGC timline optimization
02/04/14   cd      Add Tx lin data initialization for memory savings
01/22/14   cd      Added support for split static and dynamic linearizer 
                   loading
01/06/14   cd      Linearizer and Tx AGC updates for loading xPT information to 
                   RFLM
11/14/13   cd      Support for RFLM-based Tx AGC override
10/15/13   cd      Added new APIs to split config linearizer and update 
                   linearizer APIs 
10/14/13   rmb     Add support to get current tx linearizer freq comp table.
09/30/13   cd      Add support to load common TxAGC event information to RFLM DM 
09/17/13   cd      Common AGC. Config AGC and linearizer interface check-in 
06/07/13   jf      Fix LTE freq comp
05/10/13    cd     Temp Comp - Pin corner case handling should use different
                   transfer factors for APT/EPT/ET
04/29/13   cd      Apply Pin alignment during calibration
04/27/13   cd      Temp Comp - Added changes for characterized channel list 
                   and computing ref linearizer frequency
04/22/13   cd      Temp Comp - support for multi-chan linearizer compensation
                   - Multi-algorithm support based on NV versioning
03/20/13   cd      XPT Temp comp support
03/20/13   Saul    MultiLin. Each RGI now has info for which freqcomp to use.
03/05/13   cd      Added common functionality for xPT temp and freq comp
11/12/12   Saul    MultiLin. Removed dpd_data from lin table type.
11/08/12   Saul    MultiLin. Restructured EPT_V2 and ET DPDs.
10/31/12   Saul    MultiLin. Moved iq_offset to linearizer data type
10/24/12   Saul    MultiLin. Added variant_marker_type
08/23/12   Saul    MultiLin. Added PA current and iq offset NV load support.
08/21/12   Saul    MultiLin. Added support to load EPT V2 and ET NV Data.
                   Renamed API to reflect its freq-comp work.
08/17/12   Saul    MultiLin. Moved items to txlin_types.h
08/16/12   Saul    MultiLin. Removed support of Nikel's EPT from multilin V3.
                   Warnings clean-up.
08/01/12   jfc     Correct merge from last edit 
07/12/12   Saul    MultiLin. Added DPD V1/V2/V3 NV Types.
07/03/12   Saul    MultiLin. Added API to free tx band cal memory.
                   - Added check to ensure linearizers are contiguous with 
                     unique monotonic upper_bound_freq
                   - V3 OFT now tests for EPT DPD table.
                   - DPD status flag now populated when any DPD data is present.
07/02/12   Saul    MultiLin. Added framework for multilin V3 OFT testing.
06/29/12   Saul    MultiLin. EPT DPD data now populated into internal structure.
06/28/12   Saul    MultiLin. Added EPT,ET type IDs. Chan is now uint32.
06/27/12   Saul    MultiLin. Split band cal data into tx chans and freq offsets
06/25/12   Saul    MultiLin. Added "extract data" API per each type ID
06/22/12   Saul    MultiLin. Added API to load V3 NV with just proof-of-concept code.
06/21/12   Saul    MultiLin. Separated code into separate APIs per version.
06/06/12   cri     Fix Offtarget compile errors
06/04/12   cri     Add EPT support for WCDMA
05/01/12   cri     Updated EPT linearizer structure and processing
04/10/12   id      Definition of frequency to channel conversion handler 
04/04/12   cri     Support for processing EPT linearizers 
03/29/12   cri     Support for loading EPT linearizers from NV
12/07/11   whc     Common helper function for QSORT to sort linearizer table 
09/30/11    dw     Temp comp update
09/13/11    pl     Fix compiler warning
09/12/11    dw     Added temp comp support 
08/18/11    pl     Export freq comp interface
08/16/11    cd     Extern C support for off-target compilation 
08/11/11    vb     PACKed all the structures interfacing NV memory, as SUITE 
                   PACKs all the data
08/11/11    vb     Added RFCOM_TXLIN_INVALID_RGI
08/11/11    dw     Rename rfcommon_core_populate_final_linearizer()
08/03/11    cd     Added calibration list-size field to tx_band_cal_type
07/29/11    dw     Use 0xFFFF instead of 0 for invalid upper bound channel and RGI threshold  
07/19/11    dw     Initial version

============================================================================*/
#include "rfnv_items.h"
#include "rfcommon_nv.h"
#include "rfcommon_core_txlin_types.h"
#include "rfnv_item_types.h"
#include "rf_buffer_intf.h"
#include "rfm_gain_state_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/*------------------------ Multi-lin V1 NV Structure ----------------------- */

#if defined(_MSC_VER)
/* 
  START of section to make visual studio happy in order for "PACK"
  to work properly for off-target testing.
  { 
*/
#pragma pack( push, 1 )
#endif

/*----------------------------------------------------------------------------*/
/*! 
  @brief 
  Defines the variant marker type used in variable-size NVs
*/
typedef PACK(struct)
{
  /*! NV item version */
  uint16  version;

  /*! The number of elements contained in the NV item (not counting variant_marker element) */
  uint16  num_elements;
} rf_nv_data_variant_marker_type;

/*----------------------------------------------------------------------------*/
/*! @brief 
   NV data structure definitions: fixed portion. This is similar to mission mode structure except
   NV only has Tx channel number to save NV storage space, frequency will be calculated in mission mode
   */
typedef PACK(struct)
{
  /*! @brief upper bound channl of which the linearizer will be used
     if upper_bound_chan = 0xFFFF then this node has no Cal data */
  uint16 upper_bound_chan;
  /*! @brief memory offset of the dynamic linearizer table location from the end of fixed NV structure
     tx_band_cal_nv_type. Unit is per sizeof(tx_linearizer_table_nv_type)*/
  uint8 table_offset;
}tx_multi_linearizer_index_node_nv_type;

/*----------------------------------------------------------------------------*/
/*! @brief Tx linearizer index for 16 channels */
typedef tx_multi_linearizer_index_node_nv_type tx_linearizer_index_nv_type[RFCOM_TXLIN_NUM_CHANNELS];

/*----------------------------------------------------------------------------*/
typedef PACK(struct)
{
  /*! @brief Tx frequency compensation table */
  tx_freq_offset_table_type tx_freq_offsets;
  /*! @brief Linearizer index for all waveform types  */ 
  tx_linearizer_index_nv_type linearizer_index[RFCOM_TXLIN_NUM_V1_WAVEFORMS];
} tx_pa_state_cal_data_nv_type;

/*----------------------------------------------------------------------------*/
typedef PACK(struct) 
{
  /*! @brief Tx calibrated channel list */
  uint32 tx_cal_chan[RFCOM_TXLIN_NUM_CHANNELS];
} tx_cal_chan_nv_type_u32;

/*----------------------------------------------------------------------------*/
typedef PACK(struct)
{
  /*! @brief PA state this data belongs to */
  uint8 pa_state;
  /*! @brief The freq-comp type (i.e full bias or otherwise) this data belongs to */
  uint8 fcomp_sweep_type;
  /*! @brief Index indicating which freq-ccomp array the data belongs to */
  uint16 fcomp_idx;
  /*! @brief 16 channel frequency compensation value */
  int8 freq_offset[RFCOM_TXLIN_NUM_CHANNELS];
} tx_freq_offset_nv_type;

/*----------------------------------------------------------------------------*/
typedef PACK(struct) 
{
  /*! @brief First 8 bytes of reserved space, just in case we need to change stuff */
  uint8  reserved[8];
  /*! @brief Tx calibrated channel list */
  uint16 tx_cal_chan[RFCOM_TXLIN_NUM_CHANNELS];
  /*! @brief Tx linearizer index for all PA gain states */
  tx_pa_state_cal_data_nv_type tx_pa_state_cal_data[RFCOM_TXLIN_NUM_LINEARIZERS];
} tx_band_cal_nv_type;

/*----------------------------------------------------------------------------*/
/*! @brief dynamic tx linearizer NV tables, each node location is based on table offset 
  this is different from the mission mode structure as it uses seperate arraies for rgi
  power and apt.It will save more EFS storage space after compression */
typedef PACK(struct)
{
  /*! @brief Tx channel of the linearizer*/
  uint16 tx_chan;
  /*! @brief RGI/LUT index */
  uint16 rgi[RFCOM_TXLIN_NUM_SEGMENTS];
  /*! @brief TxAGC in dB10 */
  int16  power[RFCOM_TXLIN_NUM_SEGMENTS];
  /*! @brief SMPS Bias */
  uint32 apt[RFCOM_TXLIN_NUM_SEGMENTS];
} tx_linearizer_table_nv_type;

/*----------------------------------------------------------------------------*/
/*! @brief dynamic tx linearizer V3 NV table */
typedef PACK(struct)
{
  /*! @brief PA state this linearizer belongs to */
  uint8 pa_state;
  /*! @brief Waveform type this linearizer belongs to */
  uint8 wave_type;
  /*! @brief Tx channel this linearizer belongs to in chan list */
  uint32 tx_chan;
  /*! @brief Upper bound tx frequency */
  uint32 upper_bound_tx_chan;
  /*! @brief RGI/LUT index */
  uint16 rgi[RFCOM_TXLIN_NUM_SEGMENTS];
  /*! @brief TxAGC in dB10 */
  int16  power[RFCOM_TXLIN_NUM_SEGMENTS];
  /*! @brief SMPS Bias */
  uint32 apt[RFCOM_TXLIN_NUM_SEGMENTS];
  /*! PA Current */
  uint16 pa_current[RFCOM_TXLIN_NUM_SEGMENTS];
  /*! IQ Offset */
  uint16 iq_offset[RFCOM_TXLIN_NUM_SEGMENTS];
  /*! DPD Type that this linearizer uses */
  uint8 xpt_dpd_type;
  /*! The total number of DPDs that this linearizer uses */
  uint16 num_dpd;
  /*! DPD Index */
  uint16 dpd_idx[RFCOM_TXLIN_NUM_SEGMENTS];
  /*! The type of freqcomp that this linearizer uses */
  uint8 fcomp_type;
  /*! The total number of freqcomp lists that this linearizer uses */
  uint16 num_fcomp;
  /*! Index indicating the freq-comp list corresponding to the RGI */
  uint16 fcomp_idx[RFCOM_TXLIN_NUM_SEGMENTS];
} tx_linearizer_table_v3_nv_type;

/*----------------------------------------------------------------------------*/
/*! @brief dynamic tx linearizer V3 NV table + Tx RSB items */
typedef PACK(struct)
{
  /*! @brief PA state this linearizer belongs to */
  uint8 pa_state;
  /*! @brief Waveform type this linearizer belongs to */
  uint8 wave_type;
  /*! @brief Tx channel this linearizer belongs to in chan list */
  uint32 tx_chan;
  /*! @brief Upper bound tx frequency */
  uint32 upper_bound_tx_chan;
  /*! @brief RGI/LUT index */
  uint16 rgi[RFCOM_TXLIN_NUM_SEGMENTS];
  /*! @brief TxAGC in dB10 */
  int16  power[RFCOM_TXLIN_NUM_SEGMENTS];
  /*! @brief SMPS Bias */
  uint32 apt[RFCOM_TXLIN_NUM_SEGMENTS];
  /*! PA Current */
  uint16 pa_current[RFCOM_TXLIN_NUM_SEGMENTS];
  /*! IQ Offset */
  uint16 iq_offset[RFCOM_TXLIN_NUM_SEGMENTS];
  /*! DPD Type that this linearizer uses */
  uint8 xpt_dpd_type;
  /*! The total number of DPDs that this linearizer uses */
  uint16 num_dpd;
  /*! DPD Index */
  uint16 dpd_idx[RFCOM_TXLIN_NUM_SEGMENTS];
  /*! The type of freqcomp that this linearizer uses */
  uint8 fcomp_type;
  /*! The total number of freqcomp lists that this linearizer uses */
  uint16 num_fcomp;
  /*! Index indicating the freq-comp list corresponding to the RGI */
  uint16 fcomp_idx[RFCOM_TXLIN_NUM_SEGMENTS];

  /*! Tx RSB IQ correction coefficient alpha minus 1, called 
    alpha for brevity, per RGI*/
  uint32 tx_rsb_correction[RFCOM_TXLIN_NUM_SEGMENTS] ;
  /*! WTR LOFT correction coefficent */
  uint32 loft_correction[RFCOM_TXLIN_NUM_SEGMENTS] ; 
} tx_linearizer_table_v4_nv_type;


/*----------------------------------------------------------------------------*/
/*! @brief Tx EPT DPD V2 NV Type */
typedef PACK(struct)
{
  /*! @brief Index identifying this EPT DPD record */
  uint16 index;
  /*! @brief AMAM Coeff */
  uint32 am[XPT_DPD_NUM_COEFF];
  /*! @brief AMPM Coeff */
  int32 pm[XPT_DPD_NUM_COEFF];
} tx_ept_dpd_v2_nv_type;

/*----------------------------------------------------------------------------*/
/*! @brief Tx ET DPD NV Type */
typedef PACK(struct)
{
  /*! @brief Index identifying this ET DPD record */
  uint16 index;
  /*! @brief AMAM Coeff */
  uint32 am[XPT_DPD_NUM_COEFF];
  /*! @brief AMPM Coeff */
  int32 pm[XPT_DPD_NUM_COEFF];
  /*! @brief Detrough */
  uint32 dt[XPT_DPD_NUM_COEFF];
} tx_et_dpd_nv_type;

/*------------------------ Multi-lin V2 NV Structure ----------------------- */
#if 0
/*! @brief dynamic tx linearizer NV tables, each node location is based on table offset 
  this is different from the mission mode structure as it uses seperate arraies for rgi
  power and apt.It will save more EFS storage space after compression */
typedef PACK(struct)
{
  /*! @brief DPD coeff */
  dpd_poly_coeff_type dpd[RFCOM_TXLIN_NUM_SEGMENTS];
} dpd_table_data_nv_type;
#endif
/*----------------------------------------------------------------------------*/
typedef PACK(struct)
{
  /*! @brief upper bound channl of which the linearizer will be used
     if upper_bound_chan = 0xFFFF then this node has no Cal data */
  uint16 upper_bound_chan;
  /*! @brief memory offset of the dynamic linearizer table location from the end of fixed NV structure
     tx_band_cal_nv_type. Unit is per sizeof(tx_linearizer_table_nv_type)*/
  uint8 table_offset;
  /*! @brief table offset for the DPD table */
  uint8 dpd_table_offset;
}tx_multi_linearizer_index_node_nv_rev2_type;

/*----------------------------------------------------------------------------*/
/*! @brief Tx linearizer index for 16 channels */
typedef tx_multi_linearizer_index_node_nv_rev2_type tx_linearizer_index_nv_rev2_type[RFCOM_TXLIN_NUM_CHANNELS];

/*----------------------------------------------------------------------------*/
typedef PACK(struct)
{
  /*! @brief Tx frequency compensation table */
  tx_freq_offset_table_type tx_freq_offsets[RFCOM_TXLIN_NUM_V2_FCOMP_SWEEPS];
  /*! @brief Linearizer index for all waveform types  */ 
  tx_linearizer_index_nv_rev2_type linearizer_index[RFCOM_TXLIN_NUM_V2_WAVEFORMS];
} tx_pa_state_cal_data_nv_rev2_type;

/*----------------------------------------------------------------------------*/
typedef PACK(struct) 
{
  /*! @brief First 8 bytes of reserved space, just in case we need to change stuff */
  uint8  reserved[8];
  /*! @brief Tx calibrated channel list */
  uint16 tx_cal_chan[RFCOM_TXLIN_NUM_CHANNELS];
  /*! @brief Tx linearizer index for all PA gain states */
  tx_pa_state_cal_data_nv_rev2_type tx_pa_state_cal_data[RFCOM_TXLIN_NUM_LINEARIZERS];
} tx_band_cal_nv_rev2_type;


#if defined(_MSC_VER)
/* 
  } 
  END of section to make visual studio happy in order for "PACK"
  to work properly for off-target testing. 
*/
#pragma pack( pop )
#endif



/* WORST CASE SIZE = 1268360 bytes (from below calculation). 
   DO NOT decrease, only increase */
#define RFCOM_TXLIN_NV_V3_SIZE_MAX  ( 0                              \
           +        sizeof( rf_nv_data_variant_marker_type )         \
           + (1   *  ( RFNV_DATA_TYPE_ID_SIZE                        \
                      + sizeof( tx_cal_chan_nv_type_u32 ) ) )        \
           + (64  *  ( RFNV_DATA_TYPE_ID_SIZE                        \
                      + sizeof( tx_freq_offset_nv_type ) ) )         \
           + (96 *  ( RFNV_DATA_TYPE_ID_SIZE                        \
                      + sizeof( tx_linearizer_table_v4_nv_type ) ) ) \
           + (96 *  ( RFNV_DATA_TYPE_ID_SIZE                        \
                      + ( sizeof( tx_et_dpd_nv_type ) * 8 ) ) ) )

#define RFCOM_TXLIN_INVALID_RSB_VAL     0xFFFF 
#define RFCOM_TXLIN_INVALID_LOFT_VAL    0xFFFFFFFF 

/*-------------------------------------------------------------------------- */

/*! @brief CB handler for converting Tx channel to frequency */
typedef uint32 (*rfcommon_core_convert_freq_handler_type)(rfcom_lte_earfcn_type);

/*! @brief CB handler for converting Tx channel to frequency */
typedef rfcom_lte_earfcn_type (*rfcommon_core_convert_chan_handler_type)(uint32);

/*===========================================================================

                     FUNCTION DECLARATIONS
===========================================================================*/
/*----------------------------------------------------------------------------*/
boolean 
rfcommon_core_load_linearizer_from_nv
(
  tx_multi_lin_rev_type linearizer_type,
  rfnv_item_id_enum_type item_code, 
  tx_band_cal_type *tx_lut_band_data,
  rfcommon_core_convert_freq_handler_type convert_freq_handler
);

/*----------------------------------------------------------------------------*/
boolean 
rfcommon_core_freq_comp_linearizer
(
  tx_linearizer_search_param_type *tx_linearizer_search_param,
  tx_band_cal_type *tx_band_cal_data,
  tx_linearizer_table_data_type *tx_final_linearizer_data,
  void *tx_linearizer_tech_param,
  boolean do_freq_comp_shift
);

/*----------------------------------------------------------------------------*/
rfcom_txlin_temp_comp_algo_type 
rfcommon_core_txlin_get_temp_comp_algo
(
  tx_multi_lin_rev_type lin_rev_used,
  rfcom_txlin_temp_comp_nv_data_type *nv_data
);

/*----------------------------------------------------------------------------*/
void
rfcommon_core_txlin_get_temp_compensation
(
  rfcom_txlin_temp_comp_algo_type temp_comp_algo,
  int16 *pin_offset,
  int16 *pout_offset,
  tx_lin_vs_temp_vs_freq_comp_param_type *temp_vs_freq_params,
  rfcom_txlin_temp_comp_nv_data_type *temp_comp_nv_data
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_core_txlin_has_pin_cal_offset
(
  rfcom_txlin_temp_comp_nv_data_type* lin_temp_comp_data,
  uint8 pa_state
);

/*----------------------------------------------------------------------------*/
int16
rfcommon_core_txlin_get_lin_vs_temp_vs_freq_compensation
(
  tx_lin_vs_temp_vs_freq_comp_param_type *temp_vs_freq_params
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_core_txlin_pin_comp
(
  tx_linearizer_table_type *input_tx_lut,
  tx_linearizer_table_type *output_tx_lut,
  tx_linearizer_table_type *delta_rgi_lut,
  int16 pcomp_val,
  uint8 valid_lut_entries,
  tx_linearizer_aux_data_type aux_lin_data
);

/*----------------------------------------------------------------------------*/
boolean 
rfcommon_core_txlin_pout_comp
(
  tx_linearizer_table_type *input_tx_lut,
  tx_linearizer_table_type *output_tx_lut,
  int16 pcomp_val,
  uint8 valid_lut_entries
);

/*----------------------------------------------------------------------------*/
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
);

/*----------------------------------------------------------------------------*/
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
);

/*----------------------------------------------------------------------------*/
int8
rfcommon_core_freqcomp_linear_interpolate
(
  int8 freq_comp[],
  uint32 tx_cal_freq[],
  uint32 tx_freq
);

/*----------------------------------------------------------------------------*/
int 
rfcommon_core_qsort_compare_increasing_pwr
(
  const void* p1,
  const void* p2
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_core_dealloc_tx_band_cal
(
  tx_band_cal_type *tx_band_data
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_core_txlin_get_ref_linearizer_frequency
(
  tx_band_cal_type *tx_multilin_nv_cal_ptr,
  uint32 current_tx_freq,
  uint8 pa_state,
  tx_linearizer_waveform_type tx_waveform,
  uint32 *ref_lin_freq
);

/*----------------------------------------------------------------------------*/
uint64
rfcommon_core_txlin_compute_iq_gain_scaling
(
  int8 dBx10_offset
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_core_txlin_init_data
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_core_txlin_deinit_data
(
  void
);

/*----------------------------------------------------------------------------*/
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
);

/*----------------------------------------------------------------------------*/
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
);

/*----------------------------------------------------------------------------*/
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
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_core_txlin_config_tx_override
(
  rfm_mode_enum_type rfm_mode,
  rfm_device_enum_type device,
  rfcom_band_type_u rfm_band,
  rfcommon_core_txlin_agc_device_info* device_info,
  void *tx_ovr_params
);

/*----------------------------------------------------------------------------*/
const rfcommon_core_txlin_log_data_type*
rfcommon_core_txlin_get_log_data
(
  rfm_mode_enum_type rfm_mode,
  rfm_device_enum_type device
);

/*--------------------------------------------------------------------------*/
void rfcommon_core_get_tx_linearizer_freq_comp_table(int8* curr_tx_linearizer_freq_comp_table);

#ifdef __cplusplus
}
#endif
#endif /*! @brief RFCOMMON_CORE_TXLIN_H */
