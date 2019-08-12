#ifndef RFNV_ITEM_TYPES_H
#define RFNV_ITEM_TYPES_H
/*===========================================================================

                             R F   N V  Data  Types

DESCRIPTION
  This header file contains the Data types and IDs  used for variant NVs

*/

#if defined(RFA_INTERNAL_NOTES)
#error code not present
#endif

/*

Copyright (c) 2012-2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfnv/api/rfnv_item_types.h#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/25/16   ljz     Added rx static field for DBFS switchpoint\Jammper detection\Notch filter
05/16/16   aro     Typo correction
05/11/16   hm      Added Data Types for Tuner NVs
05/12/16   rb      New GSM RX/TX STATIC Datatypes to support Port AB NV structure/Cal 
05/12/16   aro     Added new WCDMA variant type for W static NV items
05/11/16   rb      New GSM RX/TX CAL Datatypes to support Port AB NV structure/Cal 
05/10/16   aro     Repurpose reserved field in rfnv_txagc_control_static_data_t
                   as carrier_config
05/09/16   rsr     MLine type
05/03/16    pv     Added #of Active PA states to 217.
05/02/16   aki     Use old name for 226 not to break old builds
05/02/16   ska     Remove duplicate fields from 226 as they overlap with 217
04/26/16   sg/aki  Change the MPR/AMPR NV parameter to agg_bw_mask and support 
                   for active cluster mask, add WTR ref path 
04/21/16   rsr     Added TX reference path type and rename fields in sub header
04/19/16   aki     Added CDMA types
04/15/16    rb     Added new GSM Datatypes for ASDIV NV changes
04/14/16   asn     Added new structure for NV with LNA switch points
04/12/16   aki     Added type for RX offset cal data and reference path
04/04/16    pv     Added new Variant Datatypes for Tx Static Unification.
03/15/16   ndb     Added conflict detune priority logic data payload type
03/25/16   aki     Add RX cal_type enum
03/23/16   cd      Add XPT types
03/08/16   aki     Rename asdiv_pos to ant_swt
03/07/16   aki     Add RSB types and PACK macro for RX type
03/02/16   aki     Remove 'reserved' field, as it's for same purpose as asdiv_pos
02/22/16   dw      Added RFNV_DATA_DELTA_GAIN_PIN_POUT_TYPE
02/08/16   rcui    Add new FBRX temp comp type
12/22/15   aki     RX to 32-bit channel, added cal-type
12/14/15   gvn     Static NV re-structuring
12/17/15   mg      Add support to retrieve gapless 2DLCA band combos from NV and
                   advertise the same to RRC.
12/17/15   stw     Added datatypes for BW class and Number Antennas
12/15/15   kga     Added datatype to support spur data in RX static nv
12/04/15   mg      Added dataype to support 2DLCA gapless band combos
12/01/15   rb      Variant NV Re-architecture datatypes added
11/20/15   rcui    Add ilpc switch point NV
11/17/15    kr     Added datatype to support ca_ns mpr backoff override
11/17/15   ars     Added datatype to support NLIC IMD and Harmonic NVs.
11/16/15   ndb     Added Detune NV datatype & detune priority NV datatype
11/13/15    pv     Added datatype to support ca_ns ampr backoff override
11/06/15   aki     Channel size to uint16 to match NvDefintion.xml
10/26/16   ck      Add GSM FBRx Gain NV
10/23/15   mcao    Add Tx Multilin uinfied NV containers, volterra support.
10/15/15   kr      Added Multi cluster BW class support
10/14/15   aki     RX update and add RFNV_DATA_RF_CAL_RFM_DEVICE_NV_CONTAINER
09/30/15   aki     And going to uint8 to save some space
09/28/15   aki     Rename RX cal NV defs
09/24/15   aki     Added updated RX cal NV defs
09/18/15   kr      Added datatype to support ampr backoff
08/25/15   sbm     FBRx NV writer for AT.
09/02/15   kr/bmg  Added dataype to support pa bais offset based on earfcn
09/01/15   kr/bmg  Added datatype to support max tx pwr based on earfcn
09/01/15   bmg     Added Rx and Tx cal offset data types for ASDIV
07/12/15   stw     Added Limits on MAX UNIQUE COMBOS in NV 28874
06/23/15   yb      Added ACL parameter payload type
07/02/15   bmg     Added support for Tx STATIC Data NVs for LTE bands for
                   Inta-band ULCA.
07/01/15   sw/zlg  AsDiv with max power back off for SAR consideration
06/04/15   ndb     Added Sparameter payload type
05/18/15   zhw     Added CDMA Rx static NV data types
04/20/15    sk     Re-structure Tx path specific NV's per Tx NV container
02/25/15   bg      Added Common Atuner binning scheme data type
02/20/15   stw     Added LTE DL path support macros
02/12/15   stw     Added Unified static type and Intraband static type
01/26/15   aa      Added data type for fbrx therm adc
01/21/15   kg      Added support for Rx RSB calibration
12/09/14   yzw     Added Rx static and Rx alt path MTPL data type
11/17/14   bmg     Added Rx RSB calibration data type
11/06/14   vb/ka   Added LTE FBRx droop cal
10/13/14   kab     Added support for 3DLCA BC Config
10/07/14   cri     Added support for LTE interband CA BW
09/26/16   kai     Added support for new WCDMA RX static and cal NVs
09/10/14   pl      Added support for C2K cal PM cal_type
05/09/14   pl      Added support for common RX CAL data
02/24/14   aa      Updated gain cal for predet
01/30/14   aa      Updated fbrx gain cal params
11/12/13   aa      Added data type for RFNV_DATA_TYPE_FBRX_GAIN_V2
08/15/13   dw      Update Rx static and Cal type
08/12/13   vs      Added data for Rx static and cal type
07/16/13   dbz     Added data type for FBRX_GAIN type
03/05/13   aka     Added data type ID for RFNV_DATA_TYPE_TX_PWR_ERR_LIST
10/25/12   aka     New file for Variant Nv strcutures and IDs
===========================================================================*/

/*===========================================================================

                                 INCLUDE FILES

===========================================================================*/
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) || defined(__MINGW32__)
/* 
  START of section to make visual studio happy in order for "PACK"
  to work properly for off-target testing.
  { 
*/
#pragma pack( push, 1 )
#endif

#define RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE   32

#define RFNV_LINEAR_MAX_SWPT_INDEX   5


#define GSM_AMAM_TBL_SIZE      128

#define GSM_AMPM_TBL_SIZE      128

#define RFNV_DATA_TYPE_ID_SIZE 2

#define RFNV_NUMBER_OF_PCLS    16

#define RFNV_MAX_LNA_GAIN_STATES 8

#define WCDMA_NUM_LNA_STATES 5

/* MAX DL CA Combos need to increase with "RFNV_LTE_CA_BC_CONFIG_I" NV Item Capacity. */
#define LTE_MAX_DL_CA_COMBOS   128

/* MAX # of DL CA carriers Supported is 5 DLCA. */
#define LTE_DL_CA_MAX_NUM_CARRIERS    5

/* MAX # of Containers for independent Tx path is 2 */
#define LTE_TX_MAX_NUM_CONTAINERS    2

/* MAX # of PATHS supported per band : 10 for now taking into account 5 DL CA */
#define LTE_DL_MAX_NUM_PATHS_BAND    10

/* MAX # of PATHS supported per TRANSCEIVER : 10 taking into account 5DL CA present on one transceiver*/
#define LTE_DL_MAX_NUM_PATHS_TRX    10

/* MAX # of PATHS supported per RF Card : 10 taking into account 5DL CA*/
#define LTE_DL_MAX_NUM_PATHS_RFC    10

/* Max Number of Unique Bands in a bandwidth class combo */
#define NV_MAX_UNIQUE_BAND_COMBOS 6

/* Max Number of Unique Bands in a Gapless CA band combo */
#define NV_MAX_GAPLESS_BAND_COMBOS 2

#define RFNV_RX_PATH_LNA_SWTPS   2*RFNV_MAX_LNA_GAIN_STATES

#define RFNV_TX_STATIC_APT_CHAR_ENTRIES   32

#define RFNV_TX_MAX_PWR_ENTRIES  16

#define RFNV_TX_STATIC_AMPR_TABLE_SIZE 128

#define RFNV_TX_STATIC_MPR_TABLE_SIZE 128

#define RFNV_MAX_NUM_SPLIT_LNA_CASES 3

#define RFNV_ATUNER_MAX_TUNE_CODE_SIZE 8

#define RFNV_RX_RSB_MAX_NUM_OF_BANDWITHS   8

#define RFNV_LTE_TX_TEMP_BIN_SIZE 8

#define RFNV_LTE_TX_CHAN_SIZE 16

#define RFNV_LTE_TX_PA_STATE 8

#define RFNV_MAX_PA_STATE 8

#define RFNV_MAX_SAR_STATE 8

#define RFNV_HDET_OFFSET_SPAN_IND 2

#define RFNV_TEMP_TABLE_SIZ 8

#define RFNV_WCDMA_MPR_BIN_NUM 8

#define RFNV_WCDMA_SAR_BIN_NUM 8

#define RFNV_ATUNER_UIACL_MAX_FREQ_NUMBER 50

#define RFNV_WCDMA_MAX_NUM_NOTCH 20

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RFNV Number of Tx PA states supported in variant types
*/
#define RFNV_TX_NUM_PA_STATE                                                  8

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RFNV channel list size used by Chan info type ID
*/
#define RFNV_CHAN_INFO_CHANNEL_LIST_SIZE                                     16

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RFNV temperature bin size
*/
#define RFNV_TEMP_BIN_SIZE                                                    8

#define RFNV_ATUNER_MAX_NUM_ANTENNAS 6

enum{
  /* 0-99 is reserved for legacy data types, will be added later */
  RFNV_DATA_TYPE_ID_CAL_CHANS                        = 100,
  RFNV_DATA_TYPE_ID_FREQ_OFFSETS                     = 101,
  RFNV_DATA_TYPE_ID_LIN_TABLE_V3                     = 102,
  //RFNV_DATA_TYPE_ID_EPT_DPD_V1                     = 103, /* <-- Not used. Here only to acknowledge Nikel's EPT */
  RFNV_DATA_TYPE_ID_EPT_DPD_V2                       = 104,
  RFNV_DATA_TYPE_ID_ET_DPD                           = 105,
  RFNV_DATA_TYPE_ID_RGI_LIST                         = 106,   /* rfnv_data_rgi_list_type */
  RFNV_DATA_TYPE_ID_PMEAS_LIST                       = 107,   /* rfnv_data_pmeas_list_type */
  RFNV_DATA_TYPE_ID_AMAM_LIST                        = 108,   /* rfnv_data_amam_list_type */
  RFNV_DATA_TYPE_ID_AMPM_LIST                        = 109,   /* rfnv_data_ampm_list_type */
  RFNV_DATA_TYPE_TX_PWR_ERR_LIST                     = 110,   /* rfnv_data_pwr_err_list_type */
  RFNV_DATA_TYPE_REF_LIN_TEMP_ADC                    = 111,   /* rfnv_data_ref_lin_temp_adc_type */
  RFNV_DATA_TYPE_FBRX_GAIN                           = 112,   /* rfnv_data_fbrx_gain_type */
  RFNV_DATA_TYPE_ID_XPT_DPD_SCALING_V1               = 113,   /* rfnv_data_xpt_dpd_scaling_v1 */
  RFNV_DATA_TYPE_LNA_SWPTS_V1                        = 114,   /* rfnv_data_lna_swpts_type */
  RFNV_DATA_TYPE_RX_CAL_OFFSET_V1                    = 115,   /* rfnv_rx_cal_offset_type */
  RFNV_DATA_TYPE_RX_OFFSET_VS_FREQ_V1                = 116,   /* rfnv_rx_offset_vs_freq_type */
  RFNV_DATA_TYPE_FBRX_GAIN_V2                        = 117,   /* rfnv_data_fbrx_gain_type_v2 */
  RFNV_DATA_TYPE_RX_CAL_OFFSET_V2                    = 118,   /* rfnv_rx_cal_data_typeid_118_type in rflte_nv.h */
  RFNV_DATA_TYPE_ID_LIN_TABLE_V4                     = 119,   /* Multi Lin NV with Tx RSB and LOFT correction  */
  RFNV_DATA_TYPE_LNA_SWPTS_V2                        = 120,   /* rfnv_data_lna_swpts_type_v2 */
  RFNV_DATA_TYPE_LNA_SWPTS_V3                        = 122,   /* rfnv_data_lna_swpts_type_v3 */
  RFNV_DATA_TYPE_ID_LTE_FBRX_DROOP_FIR_FILTER_COEFF  = 123,   /* FBRx internal data structure: rfcommon_fbrx_nv_lte_bw_params_droop_type */
  RFNV_DATA_TYPE_RXRSB_V1                            = 124,   /* rfnv_data_type_rxrsb_v1 */
  RFNV_DATA_TYPE_LTE_RX_STATIC                       = 125,   /* rfnv_rx_static_data_type*/
  RFNV_DATA_TYPE_LTE_RX_ALT_PATH_MTPL                = 126,   /* rfnv_rx_alt_path_mtpl_data_type*/
  RFNV_DATA_TYPE_FBRX_THERM_ADC_V1                   = 127,   /* rfcommon_fbrx_therm_adc_val_type */
  RFNV_DATA_TYPE_LTE_RX_STATIC_INTRABAND             = 128,   /* rfnv_rx_static_data_type*/
  RFNV_DATA_TYPE_ATUNER_BINNING_SCHEME_TYPE          = 129,   /* Common Atuner binning scheme data type */
  RFNV_DATA_TYPE_1X_LNA_SWITCHPOINTS                 = 130,   /* rfnv_rx_swpt_type */
  RFNV_DATA_TYPE_DO_REVA_LNA_SWITCHPOINTS            = 131,   /* rfnv_rx_swpt_type */
  RFNV_DATA_TYPE_DO_REVB_SC_LNA_SWITCHPOINTS         = 132,   /* rfnv_rx_swpt_type */
  RFNV_DATA_TYPE_DO_REVB_MC_LNA_SWITCHPOINTS         = 133,   /* rfnv_rx_swpt_type */
  RFNV_DATA_TYPE_1X_SPUR_TABLE                       = 134,   /* rfcommon_spur_table_type */
  RFNV_DATA_TYPE_DO_SPUR_TABLE                       = 135,   /* rfcommon_spur_table_type */
  RFNV_DATA_TYPE_ACL_PARAMS_BIN_TYPE                 = 136,   /* ACL Parameters Bin type */
  RFNV_DATA_TYPE_DL_CA_BW_COMBO                      = 137,
  RFNV_DATA_TYPE_UL_CA_BW_COMBO                      = 138,
  RFNV_DATA_TYPE_ID_SAR_BACKOFF                      = 139,   /* rfnv_data_sar_backoff_type */
  RFNV_DATA_TYPE_LTE_TX_STATIC                       = 140,   /* rfnv_tx_static_data_type */
  RFNV_DATA_TYPE_ASDIV_RX_GAIN_OFFSET                = 141,   /* rfnv_asdiv_rx_gain_offset_type */
  RFNV_DATA_TYPE_ASDIV_RX_SWITCH_POINTS              = 142,   /* rfnv_asdiv_rx_switch_points_type */
  RFNV_DATA_TYPE_ASDIV_TX_GAIN_OFFSET                = 143,   /* rfnv_asdiv_tx_gain_offset_type */
  RFNV_DATA_TYPE_LTE_TX_MAX_POWER_BASED_ON_EARFCN    = 144,   /* rfnv_max_tx_pwr_vs_earfcn_static_data_type */
  RFNV_DATA_TYPE_PA_BIAS_OFFSET_CHANNELS             = 145,   /*rfnv_data_pa_bias_combo_offset_type*/
  RFNV_DATA_TYPE_FBRX_GAIN_V3                        = 146,   /* rfcommon_fbrx_ref_chan_gain_nv_type */
  RFNV_DATA_FBRX_CHAN_LIST                           = 147,   /* rfcommon_fbrx_channel_nv_type */
  RFNV_DATA_TYPE_FBRX_THERM_ADC_V2                   = 148,   /* rfcommon_fbrx_therm_adc_nv_type */
  RFNV_DATA_FBRX_FREQ_COMP_GAIN_TYPE                 = 149,   /* rfcommon_fbrx_freq_comp_gain_nv_type */
  RFNV_DATA_FBRX_DC_CAL_TYPE                         = 150,   /* rfcommon_fbrx_dc_cal_nv_type */
  RFNV_DATA_TYPE_LTE_AMPR                            = 151,   /* rfnv_ampr_static_data_type*/
  RFNV_DATA_TYPE_STAND_ALONE_RX_CAL_OFFSET_V3        = 152,   /* rfnv_rx_stand_alone_lna_data_v3 */
  RFNV_DATA_TYPE_SPLIT_RX_CAL_OFFSET_V3              = 153,   /* rfnv_rx_split_lna_data_v3 */
  RFNV_DATA_RF_CAL_RFM_DEVICE_NV_CONTAINER           = 154,   /* rfnv_variant_element_header */
  RFNV_DATA_TYPE_MULTI_CLUSTER_BW_CLASS_I            = 155,   /* rfnv_multi_cluster_bw_class_data_type*/
  RFNV_DATA_TYPE_ID_VOLTERRA_EPT                     = 156,   /* tx_ept_volterra_nv_type */
  RFNV_DATA_TYPE_ID_VOLTERRA_ET                      = 157,   /* tx_et_volterra_nv_type */
  RFNV_DATA_GSM_FB_GAIN_TYPE                         = 158,   /* rfnv_data_gsm_fb_gain_type */
  RFNV_DATA_TYPE_LTE_CA_NS_AMPR                      = 159,   /* rfnv_ca_ns_ampr_static_data_type*/
  RFNV_DATA_TYPE_ATUNER_DETUNE_DATA_TYPE             = 160,   /* detune datatype*/
  RFNV_DATA_TYPE_ATUNER_DETUNE_PRIORITY_TABLE_TYPE   = 161,   /* detune priority table type*/
  RFNV_DATA_TYPE_NLIC_HARMONIC_PARAMS                = 162,
  RFNV_DATA_TYPE_NLIC_IMD_PARAMS                     = 163,
  RFNV_DATA_TYPE_LTE_CA_NS_MPR                       = 164,   /* rfnv_ca_ns_mpr_static_data_type*/
  RFNV_DATA_TYPE_FBRX_ILPC_SWITCHPOINTS              = 165,   /* rfcommon_fbrx_ilpc_switch_pt_nv_type */
  RFNV_DATA_TYPE_FBRX_XPT_SWITCHPOINTS               = 166,
  RFNV_DATA_TYPE_FBRX_SELFTEST_SWITCHPOINTS          = 167,
  RFNV_DATA_TYPE_GSM_TX_TIMING_I_LIST                = 168,   /*Variant NV dataypes*/
  RFNV_DATA_TYPE_GSM_EXTENDED_PA_SWPT_I_LIST         = 169,
  RFNV_DATA_TYPE_GSM_TX_GTR_THRESH_I_LIST            = 170,
  RFNV_DATA_TYPE_VBATT_I_LIST                        = 171,
  RFNV_DATA_TYPE_ENV_GAIN_I_LIST                     = 172,
  RFNV_DATA_TYPE_AMAM_TEMP_COMP_I_LIST               = 173,
  RFNV_DATA_TYPE_TEMP_COMP_I_LIST                    = 174,
  RFNV_DATA_TYPE_ENH_TEMP_COMP_I_LIST                = 175,
  RFNV_DATA_TYPE_LINEAR_TX_GAIN_PARAM_I_LIST         = 176,
  RFNV_DATA_TYPE_MULTISLOT_MAX_TX_PWR_I_LIST         = 177,
  RFNV_DATA_TYPE_POWER_LEVELS_I_LIST                 = 178,
  RFNV_DATA_TYPE_EXTENDED_SMPS_PDM_TBL_I_LIST        = 179,
  RFNV_DATA_TYPE_EXTENDED_SAR_BACKOFF_I_LIST         = 180,
  RFNV_DATA_TYPE_EXTENDED_POLAR_RAMP_PROFILE_I_LIST  = 181,
  RFNV_DATA_TYPE_EXTENDED_PA_RANGE_MAP_I_LIST        = 182,
  RFNV_DATA_TYPE_ENH_APT_I_LIST                      = 183,
  RFNV_DATA_TYPE_PA_ICQ_BIAS_LIST                    = 184,
  RFNV_DATA_TYPE_CAL_TEMP_I_LIST                     = 185,
  RFNV_DATA_TYPE_ANT_TUNER_CL_I_LIST                 = 186,
  RFNV_DATA_TYPE_COEX_TX_OFFSET_INFO_TBL_LIST        = 187,
  RFNV_DATA_TYPE_RX_CAL_DATA_I_LIST                  = 188,
  RFNV_DATA_TYPE_TX_CAL_CHAN_I_LIST                  = 189,
  RFNV_DATA_TYPE_TX_CAL_AMAM_RGI_I_LIST              = 190,
  RFNV_DATA_TX_CAL_RGI_LIST_TYPE                     = 191,
  RFNV_DATA_TX_CAL_PMEAS_LIST_TYPE                   = 192,
  RFNV_DATA_TX_CAL_AMAM_LIST_TYPE                    = 193,
  RFNV_DATA_TX_CAL_PWR_ERR_LIST_TYPE                 = 194,
  RFNV_DATA_TX_CAL_AMPM_LIST_TYPE                    = 195,
  RFNV_DATA_TYPE_GSM_LNA_SWPT_LIST                   = 196,  /*RX Items*/
  RFNV_DATA_TYPE_GSM_TX_RX_ANT_TUNER_LIST            = 197,
  RFNV_DATA_TYPE_GSM_COEX_RX_OFFSET_INFO_TBL_LIST    = 198,
  RFNV_DATA_TYPE_2DLCA_GAPLESS_BAND_COMBO            = 199,
  RFNV_DATA_TYPE_STATIC_SPUR_DATA_LIST               = 200,
  RFNV_DATA_TYPE_DL_CA_BW_ANTENNA_COMBO              = 201,
  RFNV_DATA_TYPE_UL_CA_BW_ANTENNA_COMBO              = 202,
  RFNV_DATA_TYPE_LTE_RX_SWITCH_POINT_CONTAINER       = 203,
  RFNV_DATA_DELTA_GAIN_PIN_POUT_TYPE                 = 204,
  RFNV_DATA_FB_GAIN_VS_TEMP_VS_FREQ_TYPE             = 205,
  RFNV_DATA_VARIANT_SUB_HEADER_V2                    = 206, /* rfnv_variant_element_header_v2 */
  RFNV_DATA_RSB_CAL_BB_VEC_TYPE                      = 207, /* rfnv_data_rsb_cal_bb_vec_type */
  RFNV_DATA_TYPE_TX_LIN_VS_TEMP_FREQ                 = 208, /* rfnv_tx_lin_vs_freq_vs_temp_data_type */
  RFNV_DATA_TYPE_CHAN_INFO                           = 209, /* rfnv_data_chan_info_type */
  RFNV_DATA_TYPE_ID_XPT_MODE_CONTROL                 = 210, /* rfnv_xpt_static_mode_ctrl_type */
  RFNV_DATA_TYPE_ID_XPT_PIN_POUT                     = 211, /* rfnv_xpt_static_pin_pout_data_type */
  RFNV_DATA_TYPE_ID_XPT_ANALOG_DELAY                 = 212, /* rfnv_xpt_static_analog_delay_data_type */
  RFNV_DATA_TYPE_ID_XPT_ANALOG_DELAY_OFFSETS         = 213, /* rfnv_xpt_static_analog_delay_offsets_data_type*/
  RFNV_DATA_TYPE_ID_XPT_TXAGC                        = 214, /* rfnv_xpt_static_txagc_data_type */
  RFNV_DATA_TYPE_LTE_MPR                             = 215, /* rfnv_mpr_static_data_t */
  RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL          = 216, /* rfnv_tx_sar_power_limit_control_static_data_t */
  RFNV_DATA_TYPE_TXAGC_CONTROL                       = 217, /* rfnv_txagc_control_static_data_t */
  RFNV_DATA_TYPE_TX_MIN_POWER_LIMIT_CONTROL          = 218, /* rfnv_tx_min_power_limit_control_static_data_t */
  RFNV_DATA_ATUNER_CONFLICT_DETUNE_PRIORITY_LOGIC_TYPE   = 219, /*conflict detune priority logic payload type*/
  RFNV_DATA_TYPE_STAND_ALONE_RX_CAL_CHAR_OFFSET_V3   = 220,  /* rfnv_rx_stand_alone_offset_lna_data_v3 */
  RFNV_DATA_VARIANT_REFERENCE_SUB_HEADER_V2          = 221,  /* rfnv_variant_element_ref_header */
  RFNV_GSM_EXTENDED_PA_SWPT_DATA_TYPE                = 222,  /* NEW GSM TYPES */
  RFNV_GSM_MULTISLOT_MAX_TX_PWR_DATA_TYPE            = 223,
  RFNV_GSM_LNA_SWPT_DATA_TYPE                        = 224,  /* NEW GSM TYPES */
  RFNV_DATA_TYPE_TDS_TX_MAX_POWER_LIMIT_CONTROL      = 225,  /* rfnv_tds_tx_max_power_limit_control_static_data_t*/
  RFNV_DATA_TYPE_CDMA_PA_SWITCHPOINTS_TYPE           = 226,  /* rfnv_data_type_cdma_pa_switchpoints */
  RFNV_DATA_TYPE_CDMA_PA_STATIC                      = 226,  /* rfnv_data_type_cdma_pa_static */ // INTENTIONAL DUPLICATE
  RFNV_DATA_TYPE_CDMA_RX_SWITCHPOINTS_TYPE           = 227,  /* rfnv_data_type_cdma_rx_switchpoints*/
  RFNV_DATA_TYPE_CDMA_ENC_BTF_TYPE                   = 228,  /* rfnv_data_type_cdma_enc_btf*/
  RFNV_DATA_TYPE_LTE_CA_MPR                          = 229,  /* rfnv_ca_mpr_static_data_type */
  RFNV_DATA_OFFSET_TX_FREQ_WRT_REFPATH_TYPE          = 230,
  RFNV_DATA_TYPE_PA_COMP_CONTROL                     = 231,  /* rfnv_data_type_pa_comp_control*/
  RFNV_DATA_TYPE_XPT_MLINE_RES_TYPE                  = 232,  /* rfnv_data_type_mline_results */   
  RFNV_GSM_RX_CAL_DATA_TYPE                          = 233, /*New GSM CAL Rx Type*/
  RFNV_GSM_TX_CAL_CHAN_DATA_TYPE                     = 234, /*New GSM CAL Tx Type begin*/
  RFNV_GSM_TX_CAL_AMAM_RGI_DATA_TYPE                 = 235,
  RFNV_GSM_TX_CAL_RGI_DATA_TYPE                      = 236,
  RFNV_GSM_TX_CAL_PMEAS_DATA_TYPE                    = 237,
  RFNV_GSM_TX_CAL_AMAM_DATA_TYPE                     = 238,
  RFNV_GSM_TX_CAL_PWR_ERR_DATA_TYPE                  = 239,
  RFNV_GSM_TX_CAL_AMPM_DATA_TYPE                     = 240,
  RFNV_WCDMA_VARIANT_TX_POWER                        = 241, /* rfnv_wcdma_variant_tx_power_t */
  RFNV_WCDMA_VARIANT_TX_TIMING                       = 242, /* rfnv_wcdma_variant_tx_timing_t */
  RFNV_WCDMA_VARIANT_TX_MPR                          = 243, /* rfnv_wcdma_variant_tx_mpr_t */
  RFNV_WCDMA_VARIANT_SAR                             = 244, /* rfnv_wcdma_variant_tx_sar_t */
  RFNV_WCDMA_VARIANT_RX_NOISE_FLOOR                  = 245, /* rfnv_wcdma_variant_rx_noise_floor_t */
  RFNV_WCDMA_VARIANT_BTF                             = 246, /* rfnv_wcdma_variant_btf_t */
  RFNV_GSM_TX_TIMING_DATA_TYPE                       = 247,
  RFNV_GSM_TX_GTR_THRESH_DATA_TYPE                   = 248,
  RFNV_GSM_VBATT_DATA_TYPE                           = 249,
  RFNV_GSM_ENV_GAIN_DATA_TYPE                        = 250,
  RFNV_GSM_AMAM_TEMP_COMP_DATA_TYPE                  = 251,
  RFNV_GSM_TEMP_COMP_DATA_TYPE                       = 252,
  RFNV_GSM_ENH_TEMP_COMP_DATA_TYPE                   = 253,
  RFNV_GSM_LINEAR_TX_GAIN_PARAM_DATA_TYPE            = 254,
  RFNV_GSM_POWER_LEVELS_DATA_TYPE                    = 255,
  RFNV_GSM_EXTENDED_SMPS_PDM_TBL_DATA_TYPE           = 256,
  RFNV_GSM_EXTENDED_SAR_BACKOFF_DATA_TYPE            = 257,
  RFNV_GSM_EXTENDED_POLAR_RAMP_PROFILE_DATA_TYPE     = 258,
  RFNV_GSM_EXTENDED_PA_RANGE_MAP_DATA_TYPE           = 259,
  RFNV_GSM_ENH_APT_DATA_TYPE                         = 260,
  RFNV_GSM_PA_ICQ_BIAS_DATA_TYPE                     = 261,
  RFNV_GSM_CAL_TEMP_DATA_TYPE                        = 262,
  RFNV_GSM_ANT_TUNER_CL_DATA_TYPE                    = 263,
  RFNV_GSM_COEX_TX_OFFSET_INFO_TBL_DATA_TYPE         = 264,
  RFNV_DATA_TYPE_ATUNER_UIACL_PARAMS_ANTENNA_GROUP_DELIMITER    = 265,  /* rfnv_atuner_uiacl_params_antenna_group_delimiter */
  RFNV_DATA_TYPE_ATUNER_UIACL_SCENARIO_TYPE                     = 266,  /* rfnv_atuner_uiacl_scenario_type */
  RFNV_DATA_TYPE_TDS_TX_CHAN_LIST                    = 267,
  RFNV_DATA_TYPE_TDS_MPR_VAL_V2                      = 268,
  RFNV_DATA_TYPE_TDS_LNA_RANGE_RISE_FALL             = 269,
  RFNV_WCDMA_VARIANT_RX_LNA_SP_TYPE                  = 270,  /* rfnv_wcdma_variant_rx_lna_sp_t */
  RFNV_WCDMA_VARIANT_RX_JAMMER_THRES_TYPE            = 271,  /* rfnv_wcdma_variant_rx_jammer_thres_t */
  RFNV_WCDMA_VARIANT_RX_NOTCH_TYPE                   = 272,  /* rfnv_wcdma_variant_rx_notch_t */
};

enum
{
  /* WCDMA RX NV_TYPE use 1-5000*/
  RFNV_RX_CAL_DATA_NV_TYPE_WCDMA_SC = 1,
  RFNV_RX_CAL_DATA_NV_TYPE_WCDMA_DC = 2,
  RFNV_RX_CAL_DATA_NV_TYPE_WCDMA_3C = 3,

  /* LTE RX NV_TYPE use 5001-10000*/
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_BW_1p4 = 5001,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_BW_3p5 = 5002,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_BW_5p0 = 5003,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_BW_10p0 = 5004,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_BW_15p0 = 5005,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_BW_20p0 = 5006,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_BW_40p0 = 5007,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_INTRA_BW_1p4 = 5008,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_INTRA_BW_3p5 = 5009,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_INTRA_BW_5p0 = 5010,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_INTRA_BW_10p0 = 5011,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_INTRA_BW_15p0 = 5012,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_INTRA_BW_20p0 = 5013,
  RFNV_RX_CAL_DATA_NV_TYPE_LTE_INTRA_BW_40p0 = 5014,

  /* C2K RX NV_TYPE use 10001-15000*/
  RFNV_RX_CAL_NV_TYPE_CDMA_PM0         = 10001 ,
  RFNV_RX_CAL_NV_TYPE_CDMA_PM1         = 10002 ,
  RFNV_RX_CAL_NV_TYPE_CDMA_PM2         = 10003 ,

  /* TDS RX NV_TYPE use 15001-20000*/
  RFNV_RX_CAL_DATA_NV_TYPE_TDSCDMA_LL = 15001,
  RFNV_RX_CAL_DATA_NV_TYPE_TDSCDMA_HL = 15002,
};

/*
  Expected values for BW mask:

  RFNV_INVALID_BW               = 0,
  RFNV_BW_1P4                   = 1,
  RFNV_BW_3                     = 2,
  RFNV_BW_5                     = 4,
  RFNV_BW_10                    = 8,
  RFNV_BW_15                    = 16,
  RFNV_BW_20                    = 32,
  RFNV_BW_40                    = 64,
  RFNV_BW_INTRA_ULCA_CONTIGUOUS = 256,

If its applicable to all BW type set all bits to 1.

*/

/*
  Expected values for Modulation mask:

  RFNV_INVALID_MOD       = 0,
  RFNV_MOD_BPSK          = 1,
  RFNV_MOD_QPSK          = 2,
  RFNV_MOD_16QAM         = 4,
  RFNV_MOD_64QAM         = 16,
  RFNV_MOD_256QAM        = 32,

If MPR is applicable to all modulation type set all bits to 1.
*/

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
} rfnv_data_variant_marker_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Defines the variant type and data array.

  @details
  When reading/writing variant NVs this is what the buffer will have
*/
typedef PACK(struct)
{
  /*! Data type */
  uint16 data_type;

  /*! data of variant is here */
  uint8 data[];
} rfnv_data_variant_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Defines common channel table data type
*/
typedef PACK(struct)
{
  /*! Number of valid channels in the channel list */
  uint8 number_of_valid_channels;

  /*! Channel list*/
  uint32  channel_list[RFNV_CHAN_INFO_CHANNEL_LIST_SIZE];
} rfnv_data_chan_info_type;

/*----------------------------------------------------------------------------*/
typedef PACK (struct)
{
  uint8 mod;     /*0 = GMSK, 1 = 8PSK*/
  uint8 pa_state;
  uint8 valid_rgi_num;
  uint8 rgi_list[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
}rfnv_data_rgi_list_type;


typedef PACK (struct)
{
  uint8 channel_index;
  uint8 gain_state;
  uint8 modulation;
  int16 fbrx_rms;
  int16 box_rms;
}rfnv_data_fbrx_gain_cal_node_type;


typedef PACK (struct)
{
  uint8 mod;     /*0 = GMSK, 1 = 8PSK*/
  uint8 channel_index;
  uint8 pa_state;
  int16 pmeas_list[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
}rfnv_data_pmeas_list_type;

typedef PACK (struct)
{
  uint8 amam_ampm_identifier;  /* 0 = AMAM, 1= AMPM */
  uint8 channel_index;
  uint16 data_list[GSM_AMAM_TBL_SIZE];
}rfnv_data_amam_list_type;

typedef PACK (struct)
{
  uint8 amam_ampm_identifier;  /* 0 = AMAM, 1= AMPM */
  uint8 channel_index;
  int16 data_list[GSM_AMPM_TBL_SIZE];
}rfnv_data_ampm_list_type;

typedef PACK (struct)
{
  uint8 mod;     /*0 = GMSK, 1 = 8PSK*/
  uint8 channel_index;
  int16 pwr_err_list[RFNV_NUMBER_OF_PCLS];
}rfnv_data_pwr_err_list_type;

typedef PACK (struct)
{
  uint8 channel_index;
  uint8 gain_state_index;
  int16 fbrx_gain;
}rfnv_data_fbrx_gain_type;

typedef PACK (struct)
{
  uint8 channel_index;
  uint8 fbrx_mode_index;
  uint8 gain_state_index;
  int32 fbrx_gain;
  int16 tx_power;
  uint8 ubias;
  uint16 rx_scale;
  uint16 predet_index;
}rfnv_data_fbrx_gain_type_v2;

typedef PACK(struct)
{
  uint8 chain_id;
  uint8 carr_id;
  int16 lna_rise[WCDMA_NUM_LNA_STATES];
  int16 lna_fall[WCDMA_NUM_LNA_STATES];
  int16 lna_hs_rise[WCDMA_NUM_LNA_STATES];
  int16 lna_hs_fall[WCDMA_NUM_LNA_STATES];
}rfnv_data_lna_swpts_type;

typedef PACK(struct)
{
  uint8 chain_id;
  uint8 carr_id;
  int16 lna_offsets[WCDMA_NUM_LNA_STATES];
  int16 vga_offset;
}rfnv_rx_cal_offset_type;

typedef PACK(struct)
{
  uint8 chain_id;
  uint16 channel;
  uint8 chan_index;
  int8 vga_gain_offset_vs_freq;
  int8 lna_offset_vs_freq;
  int8 lna_offset_vs_freq_2;
  int8 lna_offset_vs_freq_3;
  int8 lna_offset_vs_freq_4;
}rfnv_rx_offset_vs_freq_type;


typedef PACK ( struct )
{
  uint16   path_idx;
  uint16   cal_type;
  uint16  channel_list[RFNV_NUMBER_OF_PCLS];
  int16   freq_comp_offset[RFNV_MAX_LNA_GAIN_STATES][RFNV_NUMBER_OF_PCLS];
} rfnv_rx_cal_data_typeid_118_type;

typedef PACK ( struct )
{
  uint16   path_idx;
  uint16   cal_type;
  rfcom_lte_earfcn_type  channel_list[RFNV_NUMBER_OF_PCLS];
  int16   freq_comp_offset[RFNV_MAX_LNA_GAIN_STATES][RFNV_NUMBER_OF_PCLS];
} rfnv_rx_cal_data_typeid_118_v2_type;

typedef PACK( struct )
{
  uint8 carr_id;
  uint8 hs_config;
  int16 lna_rise[WCDMA_NUM_LNA_STATES];
  int16 lna_fall[WCDMA_NUM_LNA_STATES];
}rfnv_data_lna_swpts_type_v3;

typedef PACK(struct)
{
  uint8  nv_container;
  uint8  rfm_dev;
} rfnv_variant_element_header;

typedef PACK(struct)
{
  uint8  nv_container;
  uint8  rfm_dev;
  uint8  ant_swt;
} rfnv_variant_element_header_v2;

typedef PACK(struct)
{
   PACK(struct){
     uint8  nv_container;
     uint8  rfm_dev;
     uint8  ant_swt;
   } offset;
   PACK(struct) {
     uint8  nv_container;
     uint8  rfm_dev;
     uint8  ant_swt;
   } abs;
} rfnv_variant_element_ref_header;

typedef enum {
  FULL_CAL = 0,
  OFFSET_CAL_ONE_GAIN_STATE = 1,
  OFFSET_CAL_ALL_GAIN_STATES = 2
} rfnv_rx_cal_type;

typedef PACK ( struct )
{
  uint16 cal_type;
  uint8 number_of_valid_channels;
  uint32 channel_list[RFNV_NUMBER_OF_PCLS];
  int16  freq_comp_offset[RFNV_MAX_LNA_GAIN_STATES][RFNV_CHAN_INFO_CHANNEL_LIST_SIZE];
} rfnv_rx_stand_alone_lna_data_v3;

typedef enum {
  RX_STANDLONE_OFFSET_DATA_ONE_GAIN_STATE = 0,
  RX_STANDLONE_OFFSET_DATA_ALL_GAIN_STATES = 1
} rfnv_rx_offset_data_type;

typedef PACK ( struct )
{
  uint16 type_of_offset_data; //rfnv_rx_offset_data_type
  rfnv_data_chan_info_type channel_table;
  uint16 gain_state_mask;
  int16  freq_comp_offset[RFNV_MAX_LNA_GAIN_STATES][RFNV_CHAN_INFO_CHANNEL_LIST_SIZE];
} rfnv_rx_stand_alone_offset_lna_data_v3;

typedef PACK ( struct )
{
  uint8 number_of_valid_channels;
  uint32 channel_list[RFNV_NUMBER_OF_PCLS];
  uint8 number_of_valid_splits;
  PACK (struct) {
    uint16 concurrent_dev_mask;
    int16  freq_comp_offset[RFNV_MAX_LNA_GAIN_STATES][RFNV_NUMBER_OF_PCLS];
  } split_freq_comp_offset[RFNV_MAX_NUM_SPLIT_LNA_CASES];
} rfnv_rx_split_lna_data_v3;

typedef PACK(struct)
{
   uint16 bw;
   uint8 num_rsb_cal_taps;
   PACK(struct) {
      int32 r;
      int32 i;
   } rsb_bb_vec[RFNV_RX_RSB_MAX_NUM_OF_BANDWITHS];
} rfnv_data_rsb_cal_bb_vec_type;

typedef PACK(struct)
{
  uint8    nv_active;
  int16    sin_theta;
  int16    cos_theta;
  int16    gain_inv;
} rsb_data_list_per_gain;

typedef PACK(struct)
{
  uint16                     path_idx;
  uint8                      rsb_type;
  uint16                     dl_channel;
  int32                      freq_offset_khz;
  uint8                      bw;
  rsb_data_list_per_gain     rsb_data[RFNV_MAX_LNA_GAIN_STATES];
} rfnv_rx_cal_data_typeid_124_type;

typedef PACK(struct)
{
  rfnv_data_variant_marker_type     variant_marker;
  rfnv_rx_cal_data_typeid_118_type  *cal_118_data;
  uint32                            num_of_cal_118_elements;
  uint32                            num_of_cal_124_elements;
  rfnv_rx_cal_data_typeid_124_type  *cal_124_data;
} rfnv_rx_cal_data_type;

typedef PACK(struct)
{
  rfnv_data_variant_marker_type       variant_marker;
  rfnv_rx_cal_data_typeid_118_v2_type  *cal_118_data;
  uint32                              num_of_cal_118_elements;
  uint32                              num_of_cal_124_elements;
  rfnv_rx_cal_data_typeid_124_type    *cal_124_data;
} rfnv_rx_cal_data_v2_type;

typedef PACK(struct)
{
  uint8   rx_num_gain_states;
  int16   rx_gain_offsets[RFNV_MAX_LNA_GAIN_STATES];
  int16   rx_path_lna_swtps[RFNV_RX_PATH_LNA_SWTPS];
}rfnv_rx_static_data_type;

typedef PACK(struct)
{
  uint8 enable;
  int16 mtpl_value;
}rfnv_rx_alt_path_mtpl_data_type;

/*----------------------------------------------------------------------------*/
/* QMSL_LTE_RX_SWITCH_POINT_CONTAINER_DATA_TYPE (type ID 203) */
typedef PACK(struct)
{
  uint8 nv_container;
  uint8 reserved;
  /* switch_point_type:
     0: single-carrier switch points
     1: multi-carrier shared-LNA switch points
     2: LTE-D mode switch points */
  uint8 switch_point_type;
  PACK(struct) switch_point_s
  {
    /* 0: static (dBm) switch point
       1: dynamic (dBFS) switch point */
    uint8 type;
    /* if type 1, backoff must be a negative number in dB10 that is the
       distance from saturation that the LNA will rise to the next state */
    int16 rise_or_backoff;
    /* if type 1, hyst must be a negative number in dB10 that is the amount
       below the rise that the LNA will fall back into this state */
    int16 fall_or_hyst;
  } switch_points[RFNV_MAX_LNA_GAIN_STATES - 1];
} rfnv_lte_rx_switch_point_container_data_type;

typedef PACK(struct)
{
   /*static data for LTE Rx paths*/
   rfnv_rx_static_data_type         *rx_path_static_data;
   /* Rx alternate path based MTPL NV */
   rfnv_rx_alt_path_mtpl_data_type  *rx_alt_path_mtpl_data;
   /*Intraband static data for LTE Rx paths*/
   rfnv_rx_static_data_type         *rx_path_intra_static_data;
   uint32                           num_of_rx_path_elements;
   uint32                           num_of_rx_path_intra_elements;
   uint32                           num_of_rx_alt_path_mtpl_elements;
}rfnv_rx_static_unified_data_type;

typedef PACK(struct)
{
  uint8    nv_container_idx; /* NV Container Index for each Tx path for a given LTE band */
  uint32   tx_earfcn; /* Channel based APT Char data for given Tx Path */
  uint8    pa_state_idx; /* For a given PA state */
  uint8    tx_bandwidth; /* For a given Tx Bandwidth */
  uint32   reserved; /* Reserved for future use. */
  int16    pwr_db10 [RFNV_TX_STATIC_APT_CHAR_ENTRIES]; /* Characterization data: Power measurement */
  uint16   vcc_mv[RFNV_TX_STATIC_APT_CHAR_ENTRIES]; /* Characterization data: Applied VCC measurement */
  uint32   icq [RFNV_TX_STATIC_APT_CHAR_ENTRIES]; /* Characterization data: Applied ICQ measurement */
}rfnv_tx_static_data_type;

/* RFNV_ASDIV_RX_GAIN_OFFSET
   RFNV variant typeid 141 */
typedef PACK(struct)
{
  /* NV "path." */
  uint8 nv_container_index;
  /* Reserved for now, not used. */
  uint8 reserved;
  /* The ASDIV switch position, must match the definition from the RF card. */
  uint8 switch_position;
  /* Offsets to be applied to the Rx calibration data per gain state for
     this switch position.  Units are tech specific. */
  int16 gain_offset[8];
} rfnv_asdiv_rx_gain_offset_type;

/* RFNV_SWITCH_POINT_FALL_RISE_OFFSET
   Helper type to define a switch point rise/fall offset pair. */
typedef PACK(struct)
{
  /* Switch point fall offset, to be used in ASDIV Rx switch points NV. */
  int16 fall_offset;
  /* Switch point rise offset, to be used in ASDIV Rx switch points NV. */
  int16 rise_offset;
} rfnv_switch_point_fall_rise_offset_type;

/* RFNV_ASDIV_RX_SWITCH_POINTS
   RFNV variant typeid 142 */
typedef PACK(struct)
{
  /* NV "path." */
  uint8 nv_container_index;
  /* Reserved for now, not used. */
  uint8 reserved;
  /* The ASDIV switch position, must match the definition from the RF card. */
  uint8 switch_position;
  /* Offsets to be applied to the Rx switch points for this switch
     position.  Units are tech specific. */
  rfnv_switch_point_fall_rise_offset_type switch_point_offset[7];
} rfnv_asdiv_rx_switch_points_type;

/* RFNV_ASDIV_TX_GAIN_OFFSET
   RFNV variant typeid 143 */
typedef PACK(struct)
{
  /* NV "path." */
  uint8 nv_container_index;
  /* Reserved for now, not used. */
  uint8 reserved;
  /* The ASDIV switch position, must match the definition from the RF card. */
  uint8 switch_position;
  /* Offsets to be applied to the Tx calibration data (for all gain states)
     for this switch position.  Units are tech specific. */
  int16 switch_point_offset;
} rfnv_asdiv_tx_gain_offset_type;

/* RFNV_DATA_TYPE_LTE_TX_MAX_POWER_BASED_ON_EARFCN
   RFNV variant typeid 144 */
typedef PACK(struct)
{
  /* NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;
  /* Reserved for now, not used. */
  uint8 reserved;
  /* Channel boundaries for power limit regions */
  uint32 tx_upper_bound_earfcn[RFNV_TX_MAX_PWR_ENTRIES];
  /* Maximum power limit for channels up to the EARFCN in tx_upper_bound_earfcn */
  int16 pwr_dbm10[RFNV_TX_MAX_PWR_ENTRIES];
} rfnv_max_tx_pwr_vs_earfcn_static_data_type;

typedef PACK(struct)
{
  /* Channel based PA Bias offset for given Tx Path */
  uint32 tx_earfcn;
  /* PA Bias offset to be applied to PA Bias for given channel*/
  int16 pa_bias_offset;
} rfnv_data_pa_bias_offset_type;

/* RFNV_DATA_TYPE_PA_BIAS_OFFSET_CHANNELS
   RFNV variant typeid 145 */
typedef PACK(struct)
{
  /* NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;
  /* Reserved for now, not used. */
  uint8 reserved;
  /* structure to hold tx_earfcn and pa bias offset*/
  rfnv_data_pa_bias_offset_type offset[LTE_TX_CAL_CHAN_SIZ];
} rfnv_data_pa_bias_combo_offset_type;

typedef PACK(struct)
{
  /* channel boundaries for ampr backoff region */
  int32 tx_earfcn_min;
  /* channel boundaries for ampr backoff region */
  int32 tx_earfcn_max;
  /* modulation type mask for ampr backoff region */
  uint32 modulation_type_mask;
  /* channel bandwidth mask for ampr backoff region */
  uint32 bandwidth_mask;
  /* MPR boundaries for ampr backoff region */
  int16 mpr_min;
  /* MPR boundaries for ampr backoff region */
  int16 mpr_max;
  /* RB start boundaries for ampr backoff region */
  int16 rb_start_min;
  /* RB start boundaries for ampr backoff region */
  int16 rb_start_max;
  /* RB block boundaries for ampr backoff region */
  int16 rb_block_min;
  /* RB block boundaries for ampr backoff region */
  int16 rb_block_max;
  /* RB end boundaries for ampr backoff region */
  int16 rb_end_min;
  /* RB end boundaries for ampr backoff region */
  int16 rb_end_max;
  /* ampr backoff in db10 */
  int16 ampr_db10;
} rfnv_lte_ampr_table_type;


/* RFNV_DATA_TYPE_LTE_AMPR RFNV variant typeid 151 to be used with Tx STatic NV*/
typedef PACK(struct)
{
  /* NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;
  /* Reserved for now, not used. */
  uint8 reserved;
  /* Network Signalling Value */
  uint32 ns_number;
  /* AMPR table structure */
  rfnv_lte_ampr_table_type ampr_table[RFNV_TX_STATIC_AMPR_TABLE_SIZE];
} rfnv_ampr_static_data_type;

/* RFNV_DATA_TYPE_MULTI_CLUSTER_BW_CLASS_I RFNV variant typeid 155 to be used with Tx Static NV */
typedef PACK(struct)
{
  /* NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;
  /* Reserved for now, not used. */
  uint8 reserved;
  /* bit 0 - class A,bit 1 - class B, bit 2 - class C, bit 3 - class D, and so on */
  uint32 multi_cluster_bw_class;
} rfnv_multi_cluster_bw_class_data_type;

typedef PACK(struct)
{
  /* channel boundaries for ampr backoff region */
  int32 tx_earfcn_min;
  /* channel boundaries for ampr backoff region */
  int32 tx_earfcn_max;

  /* Aggregated modulation type mask for ampr backoff region */
  uint32 agg_modulation_type_mask;

  /*Aggregated channel bandwidth mask for ampr backoff region */
  /* bit 0 - class A (NA), bit 1 - class B, bit 2 - class C, and so on */
  uint32 bandwidth_class;

  /* Aggregate RB start boundaries for ampr backoff region */
  int16 agg_rb_start_min;
  /* Aggregate RB start boundaries for ampr backoff region */
  int16 agg_rb_start_max;
  /* Aggregate RB block boundaries for ampr backoff region */
  int16 agg_rb_block_min;
  /* Aggregate RB block boundaries for ampr backoff region */
  int16 agg_rb_block_max;

  /* Aggregate RB end boundaries for ampr backoff region */
  int16 agg_rb_end_min;
  /* Aggregate RB end boundaries for ampr backoff region */
  int16 agg_rb_end_max;

  /* CA NS ampr backoff in db10 */
  int16 ca_ns_ampr_db10;

} rfnv_lte_ca_ns_ampr_table_type;

/* RFNV_DATA_TYPE_LTE_CA_NS_AMPR RFNV variant typeid 159 to be used with Tx Static NV */
typedef PACK(struct)
{
  /* NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;

  /* Reserved for now, not used. */
  uint8 reserved;

  /* CA-Network Signalling Value */
  uint32 ca_ns_number;

  /* AMPR table structure */
  rfnv_lte_ca_ns_ampr_table_type ca_ns_ampr_table[RFNV_TX_STATIC_AMPR_TABLE_SIZE];

} rfnv_ca_ns_ampr_static_data_type;

typedef PACK(struct)
{
  uint16 tech;
  uint64 band_map;
  uint8 detune_code_size_in_bytes;
  uint8 detune_code[ RFNV_ATUNER_MAX_TUNE_CODE_SIZE];
}rfnv_atuner_detune_data_type;


typedef PACK(struct)
{
  uint8 enable_bit;
  uint8 detune_priority_for_ant[RFNV_ATUNER_MAX_NUM_ANTENNAS-1];
}rfnv_atuner_detune_priority_logic_conflict_info_for_antenna_data_type;


typedef PACK(struct)
{
  uint8 num_antennas;
  rfnv_atuner_detune_priority_logic_conflict_info_for_antenna_data_type conflict_data_for_ant[RFNV_ATUNER_MAX_NUM_ANTENNAS];
}rfnv_atuner_detune_priority_logic_conflict_data_type;

typedef PACK(struct)
{
  /* channel boundaries for mpr backoff region */
  int32 tx_earfcn_min;
  /* channel boundaries for mpr backoff region */
  int32 tx_earfcn_max;

  /* Aggregated modulation type mask for mpr backoff region */
  uint32 agg_modulation_type_mask;

  /*Aggregated channel bandwidth mask for mpr backoff region */
  /* bit 0 - 25RB_100RB
     bit 1 - 50RB_100RB
     bit 2 - 75RB_75RB
     bit 3 - 75RB_100RB
     bit 4 - 100RB_100RB */
  uint32 agg_bw_mask;

  /* Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_min;
  /* Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_max;
  /* Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_min;
  /* Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_max;

  /* CA mpr backoff in db10 */
  int16 ca_mpr_db10;
} rfnv_lte_ca_mpr_table_type;


/* RFNV_DATA_TYPE_LTE_CA_MPR RFNV variant typeid 229 to be used with Tx Static NV */
typedef PACK(struct)
{
  /* NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;
  /* Reserved for now, not used. */
  uint8 reserved;
  /* Cluster Mask:
     00000001 - single cluster 
     00000010 - Multicluster */
  uint8 active_cluster_mask;
  /* BW class mask*/
  uint32 bw_class_mask;
 /* MPR table structure */
  rfnv_lte_ca_mpr_table_type ca_mpr_table[RFNV_TX_STATIC_MPR_TABLE_SIZE];
} rfnv_ca_mpr_static_data_type;

typedef PACK(struct)
{
  /* channel boundaries for mpr backoff region */
  int32 tx_earfcn_min;
  /* channel boundaries for mpr backoff region */
  int32 tx_earfcn_max;

  /* Aggregated modulation type mask for mpr backoff region */
  uint32 agg_modulation_type_mask;

  /*Aggregated channel bandwidth mask for mpr backoff region */
  /* bit 0 - class A (NA), bit 1 - class B, bit 2 - class C, and so on */
  uint32 bandwidth_class;

  /* Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_min;
  /* Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_max;
  /* Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_min;
  /* Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_max;

  /* CA NS mpr backoff in db10 */
  int16 ca_ns_mpr_db10;
} rfnv_lte_ca_ns_mpr_table_type;


/* RFNV_DATA_TYPE_LTE_CA_NS_MPR RFNV variant typeid 159 to be used with Tx Static NV */
typedef PACK(struct)
{
  /* NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;
  /* Reserved for now, not used. */
  uint8 reserved;
  /* CA-Network Signalling Value */
  uint32 ca_ns_number;
 /* MPR table structure */
  rfnv_lte_ca_ns_mpr_table_type ca_mpr_table[RFNV_TX_STATIC_MPR_TABLE_SIZE];

} rfnv_ca_ns_mpr_static_data_type;

/*----------------------------------------------------------------------------*/
/* QMSL_RX_SPUR_DATA_TYPE (type ID 200) */
typedef PACK(struct)
{
  /*! Path index or NV container, used to connect this item to the RFC
      data structures. */
  uint8 nv_container;
  /*! Absolute frequency(Hz) at which spur is present. */
  uint64 spur_freq_hz;
  /*! Notch depth (hardware dependent) to be used supress the spur. */
  uint8 notch_depth;
  /*! Rx power threshold in dBm10 below which a notch will be applied to the
      spur. */
  int32 spur_thresh;
  /*! Hysteresis in dB10, the level above the Rx threshold above which the
      notch will be removed. */
  uint16 hysteresis;
} rfnv_rx_spur_data_type;


/*----------------------------------------------------------------------------*/
/* RFNV_DATA_TYPE_TX_LIN_VS_TEMP_FREQ RFNV variant typeid 208 to be used with
   Tx Static NV */
typedef PACK(struct)
{
  /*! NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;

  /*! Reserved for now, not used. */
  uint8 reserved;

  /*! Radio bandwidth bitmask that defines which modes this configuration
      data applies to.
      Bit 0: 1.4MHz
      Bit 1:   3MHz
      Bit 2:   5MHz
      Bit 3:  10MHz
      Bit 4:  15MHz
      Bit 5:  20MHz
      Bit 6:  40MHz
      Bit 7:  20MHz + 20MHz intra-contiguous ULCA
  */
  uint32 bandwidth_mask;

  /*! channel list*/
  int32 channel_list[RFNV_LTE_TX_CHAN_SIZE];

  /*! PA input power compensation vs temperature and frequency. */
  int16 tx_lin_vs_temp_vs_freq_p_in[RFNV_LTE_TX_TEMP_BIN_SIZE][RFNV_LTE_TX_CHAN_SIZE][RFNV_LTE_TX_PA_STATE];

  /*! Tx output power compensation vs temperature and frequency. */
  int16 tx_lin_vs_temp_vs_freq_p_out[RFNV_LTE_TX_TEMP_BIN_SIZE][RFNV_LTE_TX_CHAN_SIZE][RFNV_LTE_TX_PA_STATE];

} rfnv_tx_lin_vs_freq_vs_temp_data_type;

/*----------------------------------------------------------------------------*/
/* Variant type 210 */
typedef PACK (struct)
{
  /* Volterra Mode, 0 = Disable, 1 = Enable, 2 = Enable, No Update */
  uint8 volterra_mode;
  /* Auto Pin Mode, 0 = Disable, 1 = Enable, 2 = Enable, No Update */
  uint8 autopin_mode;
  /* Auto Delay Mode, 0 = Disable, 1 = Enable, 2 = Enable, No Update */
  uint8 autodelay_mode;
  /* DPD Mode,  0 = Memory Mask DPD, 1 = MemoryLess DPD */
  uint8 dpd_mode;
} rfnv_xpt_static_mode_ctrl_type;

/*----------------------------------------------------------------------------*/
/* Bandwidth header info used for XPT variant types */
typedef PACK (struct)
{
  int32 min_bw_kHz;
  int32 max_bw_kHz;
} rfnv_xpt_bw_header;

/*----------------------------------------------------------------------------*/

/* Variant type 211 */
typedef PACK(struct)
{
  /* TX BW info */
  rfnv_xpt_bw_header bw_info;
  /* Tx channel info */
  rfnv_data_chan_info_type chan_info;
  /* Pout offsets for PA state Vs Temp Vs Chan */
  int16 pout_vs_temp_vs_freq[RFNV_TEMP_BIN_SIZE]
                            [RFNV_CHAN_INFO_CHANNEL_LIST_SIZE]
                            [RFNV_TX_NUM_PA_STATE];
  /* Pin offsets for PA state Vs Temp Vs Chan */
  int16 pin_vs_temp_vs_freq[RFNV_TEMP_BIN_SIZE]
                           [RFNV_CHAN_INFO_CHANNEL_LIST_SIZE]
                           [RFNV_TX_NUM_PA_STATE];
  /* reference Pin for each PA state */
  int16 ref_pin_offset[RFNV_TX_NUM_PA_STATE];
} rfnv_xpt_static_pin_pout_data_type;

/*----------------------------------------------------------------------------*/

/* Variant type 212 */
typedef PACK (struct)
{
  /* TX BW info */
  rfnv_xpt_bw_header bw_info;
  /* Tx channel info */
  rfnv_data_chan_info_type chan_info;
  /*ADC temperature reading */
  uint32 therm_adc;
  /* ET delay offsets based on temp vs freq */
  int32 delay_vs_freq[RFNV_CHAN_INFO_CHANNEL_LIST_SIZE];
} rfnv_xpt_static_analog_delay_data_type;

/*----------------------------------------------------------------------------*/

/* Variant type 213 */
typedef PACK (struct)
{
  /* TX BW info */
  rfnv_xpt_bw_header bw_info;
  /* Tx channel info */
  rfnv_data_chan_info_type chan_info;
  /* ET analog delay based on freq */
  int32 delay_vs_temp_vs_freq_offset[RFNV_TEMP_BIN_SIZE]
                                    [RFNV_CHAN_INFO_CHANNEL_LIST_SIZE];
} rfnv_xpt_static_analog_delay_offsets_data_type;

/*----------------------------------------------------------------------------*/
/* Variant type 214 */
typedef PACK (struct)
{
  /* EPT mode env scale offset */
  int16 ept_env_scale_offset;
  /* ET mode env scale offset */
  int16 et_env_scale_offset;
  /* EPT mode env scale offset for LTE ULCA*/
  int16 ept_env_scale_offset_ulca;
  /* ET mode env scale offset for LTE ULCA*/
  int16 et_env_scale_offset_ulca;
  /* Max power linearizer, 0 = Disable, 1 = Env Scale Boost, 2 = Env Scale Cap */
  uint8 max_power_lin_algo;
  /* env scale boost value */
  int16 lin_algo_env_scale_boost_val;
  /* env scale cap value */
  int16 lin_algo_env_scale_cap_val;
} rfnv_xpt_static_txagc_data_type;

/*----------------------------------------------------------------------------*/

typedef PACK(struct)
{
  /*! channel boundaries for mpr backoff region */
  int32 tx_earfcn_min;
  /*! channel boundaries for mpr backoff region */
  int32 tx_earfcn_max;
  /*! Modulation type mask for mpr backoff region
      rfnv_lte_mod_type_s */
  uint32 modulation_type_mask;
  /*! Identifies all the Bandwidths supported for this MPR selection
      rfnv_lte_bw_type_s */
  uint32 bandwidth_type_mask;
  /*! Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_min;
  /*! Aggregate RB start boundaries for mpr backoff region */
  int16 agg_rb_start_max;
  /*! Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_min;
  /*! Aggregate RB block boundaries for mpr backoff region */
  int16 agg_rb_block_max;
  /*! mpr backoff in db10 */
  int16 mpr_db10;
} rfnv_lte_mpr_table_t;

/* RFNV_DATA_TYPE_LTE_MPR RFNV variant typeid 215 to be used with Tx Static NV */
typedef PACK(struct)
{
  /*! NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;
  /*! Reserved for now, not used. */
  uint8 reserved;
  /* Cluster Mask:
     00000001 - single cluster 
     00000010 - Multicluster */
  uint8 active_cluster_mask;
  /*! MPR table structure */
  rfnv_lte_mpr_table_t mpr_table[RFNV_TX_STATIC_MPR_TABLE_SIZE];
} rfnv_mpr_static_data_t;

typedef PACK(struct)
{
  /* SAR BACKOFF LIMIT: RFNV_LTE_Bxx_DB10_SAR_BACK_OFF_LIMIT_I
     0th index = Default Antenna POS
     1st index = Swap Antenna POS for 8 SAR STATES */
  int16 sar_tx_power_limit_dB10[2][RFNV_MAX_SAR_STATE];

}rfnv_tx_sar_power_limit_control_t;

/* RFNV_DATA_TYPE_TX_SAR_POWER_LIMIT_CONTROL RFNV variant typeid 216 to be used with Tx Static NV */
typedef PACK(struct)
{
  /*! NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;

  /*! Reserved for now, not used. */
  uint8 reserved;

  /*! TxPL Control structure */
  rfnv_tx_sar_power_limit_control_t tx_max_pl_ctrl;

} rfnv_tx_sar_power_limit_control_static_data_t;


typedef PACK(struct)
{
  /* HDET HPM LPM SWITCHPOINT : RFNV_LTE_Bxx_HDET_HPM_LPM_SWITCH_POINT_I */
  int16 hpm_lpm_swpt_dB10;

  /* HDET THRESHOLD: RFNV_LTE_Bxx_HDET_HPM_THRESHOLD_I: in dB10 */
  int16 threshold_dB10;

  /* HDET OFFSET and SPAN: RFNV_LTE_Bxx_HDET_OFF_SPAN_I */
  uint16 offset_span[RFNV_HDET_OFFSET_SPAN_IND];

  /* HDET 2D compensation: RFNV_LTE_Bxx_TX_LIM_VS_TEMP_VS_FREQ_I */
  int8 lim_vs_temp_vs_freq[LTE_TX_CAL_CHAN_SIZ][NV_TEMP_TABLE_SIZ];

}rfnv_tx_hdet_control_t;

/* RFNV_DATA_TYPE_TX_HDET_CONTROL RFNV variant typeid 209 to be used with Tx Static NV */
typedef PACK(struct)
{
  /*! NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;

  /*! Reserved for now, not used. */
  uint8 reserved;

  /*! Tx HDET Control structure */
  rfnv_tx_hdet_control_t tx_hdet_ctrl;

} rfnv_tx_hdet_control_static_data_t;

/* RFNV_LTE_Cx_Bxx_PA_STATIC_Z_I is getting broken into more sub-sections for more flexibility */
typedef PACK(struct)
{
  /* Number of active PA states */
  uint8 number_of_active_pa_states;
  
  /* PA Range Map NV */
  int16 pa_range_map[RFNV_MAX_PA_STATE];

  /* PA Hysterisis in us  */
  int16 pa_hysterisis_us;

  /* PA Fall switchpoint: 0-> Reserved | 1-> R1 fall | 2 -> R2 fall etc */
  int16 pa_fall_sw[RFNV_MAX_PA_STATE];

  /* PA Rise switchpoint: 0-> reserved | 1-> R1 rise | 2 -> R2 rise etc */
  int16 pa_rise_sw[RFNV_MAX_PA_STATE];

   /* MPR based PA Fall switchpoint Backoff to the actual Switchpoints:
   0->    Reserved | 1-> R1-O1 fall | 2 -> R2-O2 fall etc */
  int16 mpr_based_pa_fall_sw_backoff[RFNV_MAX_PA_STATE];

  /* MPR based PA Rise switchpoint Backoff to the actual Swithpoints:
  0-> reserved | 1-> R1-O1 rise | 2 -> R2-O2 rise etc */
  int16 mpr_based_pa_rise_sw_backoff[RFNV_MAX_PA_STATE];

  /* Tx Rotator Angle - per PA State */
  int16 tx_rot_angle[RFNV_MAX_PA_STATE];

}rfnv_txagc_control_t;

/* RFNV_DATA_TYPE_TXAGC_CONTROL RFNV variant typeid 217 to be used with Tx Static NV */
typedef PACK(struct)
{
  /*! NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;

  /*!Carrier config : 0 - Single Carrier, 1 - Dual Carrier */
  uint8 carrier_config;

  /*Aggregated channel bandwidth mask for mpr backoff region */
  /* bit 0 - class A (NA), bit 1 - class B, bit 2 - class C, and so on */
  uint32 bandwidth_class;

  /*! Identifies all the Bandwidths supported for this MPR selection */
  uint32 bandwidth_type_mask;

  /* CA-Network Signalling Value */
  uint32 ca_ns_number;

  /*! TxAGC Control structure */
  rfnv_txagc_control_t txagc_ctrl;

} rfnv_txagc_control_static_data_t;

typedef PACK(struct)
{
  /* MIN POWER: RFNV_LTE_Bxx_MIN_TX_POWER_DB10_I */
  int16 min_tx_power_dB10;

}rfnv_tx_min_power_limit_control_t;

/* RFNV_DATA_TYPE_TX_MIN_POWER_LIMIT_CONTROL RFNV variant typeid 218 to be used with Tx Static NV */
typedef PACK(struct)
{
  /*! NV Container Index for each Tx path for a given LTE band */
  uint8 nv_container_index;

  /*! Reserved for now, not used. */
  uint8 reserved;

  /*! TxPL Control structure */
  rfnv_tx_min_power_limit_control_t tx_min_pl_ctrl;

} rfnv_tx_min_power_limit_control_static_data_t;


typedef PACK(struct)
{
  /* MAX POWER: RFNV_TDS_TX_POWER_DBM10_I */
  int16 tds_tx_power_dBm10;

}rfnv_tds_tx_power_dbm10_t;

/*RFNV_DATA_TYPE_TDS_TX_MAX_POWER_LIMIT_CONTROL RFNV variant typeid 225*/
typedef PACK(struct)
{
  /*! NV Container Index for each Tx path */
  uint8 reserved;

  /*! Reserved */
  uint8 reserved2;

  /*! MTPL Control*/
  rfnv_tds_tx_power_dbm10_t tx_max_power_dbm10;

}rfnv_tds_tx_max_power_limit_control_static_data_t;

typedef int16 rfnv_data_type_cdma_pa[RFNV_MAX_PA_STATE];
typedef rfnv_data_type_cdma_pa rfnv_data_type_cdma_freq[RFNV_CHAN_INFO_CHANNEL_LIST_SIZE];
typedef rfnv_data_type_cdma_freq rfnv_data_type_cdma_pin_pout[RFNV_MAX_PA_STATE];

/* Variant type 226 */
typedef PACK(struct){
   rfnv_data_type_cdma_pa Access_PA_FALL_SW;
   rfnv_data_type_cdma_pa Access_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate1_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate1_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate2_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate2_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate3_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate3_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate4_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate4_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate5_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate5_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate6_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate6_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_Access_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_Access_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI0_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI0_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI1_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI1_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI2_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI2_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI3_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI3_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI4_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI4_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI5_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI5_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DORA_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DORA_PA_RISE_SW;
   rfnv_data_type_cdma_pa cdma_reserved[RFNV_DATA_TYPE_ID_SIZE];
} rfnv_data_type_cdma_pa_switchpoints_type;

/* Variant type 226 */
typedef PACK(struct){
   rfnv_data_type_cdma_pa Access_PA_FALL_SW;
   rfnv_data_type_cdma_pa Access_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate1_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate1_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate2_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate2_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate3_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate3_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate4_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate4_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate5_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate5_PA_RISE_SW;
   rfnv_data_type_cdma_pa DATA_rate6_PA_FALL_SW;
   rfnv_data_type_cdma_pa DATA_rate6_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_Access_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_Access_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI0_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI0_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI1_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI1_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI2_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI2_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI3_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI3_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI4_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI4_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI5_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DOR0_RRI5_PA_RISE_SW;
   rfnv_data_type_cdma_pa HDR_DORA_PA_FALL_SW;
   rfnv_data_type_cdma_pa HDR_DORA_PA_RISE_SW;
   rfnv_data_type_cdma_pa cdma_reserved[RFNV_DATA_TYPE_ID_SIZE];
} rfnv_data_type_cdma_pa_static;

typedef PACK(struct){
  uint8 swpt_type;
  int16 fall_or_hyst;
  int16 rise_or_backoff;
  int16 fall_hold;
  int16 rise_hold;
} ic_swt_type;

typedef PACK(struct){
  uint8 active;
  ic_swt_type SWPT[6];
} icx_type;

typedef PACK(struct){
  icx_type pm[3];
} icx_pmx_type;

/* Variant type 227 */
typedef PACK(struct){
  icx_pmx_type onex_c0;
  icx_pmx_type onex_c1;
  icx_pmx_type onex_c2;
  icx_pmx_type onex_c3;
  icx_pmx_type hdr_c0;
  icx_pmx_type hdr_c1;
  icx_pmx_type hdr_c2;
  icx_pmx_type hdr_c3;
} rfnv_data_type_cdma_rx_switchpoints;

 /* Variant type 228 */
typedef PACK(struct){
  uint32 onex_enc_btf;
  uint32 hdr_enc_btf;
} rfnv_data_type_cdma_enc_btf;


/* RFNV_DATA_TYPE_PA_COMP_CONTROL RFNV variant typeid 231*/
typedef PACK(struct)
{
  /*! reserved*/
  uint8 reserved;
  /*! Reserved */
  uint8 reserved2;
  /*! MTPL Control*/
  int16 pa_comp_up[8];
  int16 pa_comp_down[8];
  int16 pa_comp_up_vs_freq[16][8];
  int16 pa_comp_down_vs_freq[16][8];
}rfnv_pa_comp_control_static_data_t;

 /* Variant type 232 */
typedef PACK(struct){
  uint8 pa_state;
  uint8 wvfm_type;
  uint8 lin_chan_idx;
  int32 alpha_vin;
  int32 beta_vin;
  int32 alpha_vcc;
  int32 beta_vcc;
}rfnv_data_type_mline_results;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_TX_POWER */
typedef PACK(struct)
{
  uint8 reserved;
  /*!< Reserved for future use */

  int16 max_power_dbm10;
  /*!< Maximum Tx power in dBm10 units */

  int16 lim_vs_temp[RFNV_TEMP_TABLE_SIZ];
  /*!< Tx lim vs Freq */

} rfnv_wcdma_variant_tx_power_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_TX_TIMING */
typedef PACK(struct)
{
  uint8 reserved;
  /*!< Reserved for future use */

  uint16 pa_on_rise;
  /*!< PA On rise */

  uint16 pa_on_fall;
  /*!< PA On fall */

  uint16 tx_on_rise;
  /*!< Tx On rise */

  uint16 tx_on_fall;
  /*!< Tx On fall */

} rfnv_wcdma_variant_tx_timing_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_TX_MPR */
typedef PACK(struct)
{
  uint8 reserved;
  /*!< Reserved for future use */

  uint8 carrier_mode;
  /*!< Carrier Mode: 0 - SC, 1 - DC */

  uint16 beta_scaling[RFNV_WCDMA_MPR_BIN_NUM];
  /*!< Beta scaling */

  uint16 agc_offset[RFNV_WCDMA_MPR_BIN_NUM];
  /*!< AGC Offset */

  uint16 tx_mpr_backoff[RFNV_WCDMA_MPR_BIN_NUM];
  /*!< Tx MPR backoff */

  uint16 pa_mpr_backoff[RFNV_WCDMA_MPR_BIN_NUM-1];
  /*!< PA MPR backoff */

} rfnv_wcdma_variant_tx_mpr_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_SAR */
typedef PACK(struct)
{
  uint8 reserved;
  /*!< Reserved for future use */

  uint8 antenna_config;
  /*!< Antenna config */

  int16 sar_backoff[RFNV_WCDMA_SAR_BIN_NUM];
  /*!< Beta scaling */

} rfnv_wcdma_variant_tx_sar_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_RX_NOISE_FLOOR */
typedef PACK(struct)
{
  uint8 reserved;
  /*!< Reserved for future use */

  uint8 carrier_mode;
  /*!< Carrier Mode: 0 - SC, 1 - DC */

  int16 noise_floor;
  /*!< Beta scaling */

} rfnv_wcdma_variant_rx_noise_floor_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_BTF */
typedef PACK(struct)
{
  uint8 reserved;
  /*!< Reserved for future use */

  uint32 btf;
  /*!< BTF */

} rfnv_wcdma_variant_btf_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_RX_LNA_SP_T */
typedef PACK( struct )
{
  uint8 reserved;
  uint8 carr_id;
  uint8 hs_config;
  uint8 jammer;
  int16 lna_rise[WCDMA_NUM_LNA_STATES];
  int16 lna_fall[WCDMA_NUM_LNA_STATES];
} rfnv_wcdma_variant_rx_lna_sp_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_RX_JAMMER_THRES_TYPE */
typedef PACK( struct )
{
  uint8 reserved;
  int32 wbee_thresh;
  int32 ee_aci_thresh;
} rfnv_wcdma_variant_rx_jammer_thres_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_RX_NOTCH_PARAM_TYPE */
typedef PACK( struct )
{
  uint8 reserved;
  int32 freq_hz;
  uint8 notch_depth;
  int16 activation_threshold_db;
  int16 deactivation_threshold_db;
} rfnv_wcdma_rx_notch_param_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Type definition for Variant RFNV_WCDMA_VARIANT_RX_NOTCH_TYPE */
typedef PACK( struct )
{
  uint8 reserved;
  uint8 no_of_notch;
  rfnv_wcdma_rx_notch_param_t notch_param[RFNV_WCDMA_MAX_NUM_NOTCH];
} rfnv_wcdma_variant_rx_notch_t;

/*
  RFNV_DATA_TYPE_TDS_TX_CHAN_LIST                    = 267,
  RFNV_DATA_TYPE_TDS_MPR_VAL_V2                      = 268,
  RFNV_DATA_TYPE_TDS_LNA_RANGE_RISE_FALL             = 269,
*/

#ifndef RFNV_TDS_LNA_GS_PAIR_SIZE
#define RFNV_TDS_LNA_GS_PAIR_SIZE 12
#endif

typedef PACK(struct)
{
  uint16 reserved;
  /*!< Tx channel list */
  uint16 tx_chan_list[16];
}rfnv_tds_tx_chan_list_t;


typedef PACK(struct)
{
  uint16 reserved;
  /*!< MPR power backoff V2 */
  uint16 mpr_val_v2[17];
}rfnv_tds_mpr_val_v2_t;


typedef PACK(struct)
{
  uint16 reserved;
  /*!<  lna threshold in narrow band in acquisation mode*/
  uint8   lna_range_rise_fall_nb_mode_acq[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in wide band in acquisation mode*/
  uint8   lna_range_rise_fall_wb_mode_acq[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in narrow band in acquisation mode*/
  uint8   lna_range_rise_fall_nb_mode_acq_2[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in wide band in acquisation mode*/
  uint8   lna_range_rise_fall_wb_mode_acq_2[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in narrow band in idle mode*/
  uint8   lna_range_rise_fall_nb_mode_idle[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in wide band in idle mode*/
  uint8   lna_range_rise_fall_wb_mode_idle[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in narrow band in inter freq mode*/
  uint8   lna_range_rise_fall_nb_mode_ifreq[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in wide band in inter freq mode*/
  uint8   lna_range_rise_fall_wb_mode_ifreq[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in narrow band in power scan mode*/
  uint8   lna_range_rise_fall_nb_mode_power_scan[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in wide band in power scan mode*/
  uint8   lna_range_rise_fall_wb_mode_power_scan[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in narrow band in tracking mode*/
  uint8   lna_range_rise_fall_nb_mode_tracking[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in wide band in tracking mode*/
  uint8   lna_range_rise_fall_wb_mode_tracking[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in narrow band in tracking mode*/
  uint8   lna_hs_range_rise_fall_nb_mode_tracking[RFNV_TDS_LNA_GS_PAIR_SIZE];
  /*!<  lna threshold in wide band in tracking mode*/
  uint8   lna_hs_range_rise_fall_wb_mode_tracking[RFNV_TDS_LNA_GS_PAIR_SIZE];
}rfnv_tds_lna_range_rise_fall_t;


#if defined(_MSC_VER) || defined(__MINGW32__)
/* 
  } 
  END of section to make visual studio happy in order for "PACK"
  to work properly for off-target testing.
*/
#pragma pack( pop )
#endif


#ifdef __cplusplus
}
#endif

#endif
