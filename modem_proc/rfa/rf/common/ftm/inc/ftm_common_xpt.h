#ifndef FTM_COMMON_XPT_H
#define FTM_COMMON_XPT_H

/*!
  @file
  ftm_common_xpt.h

  @brief
  This module contains defintions and prototypes for FTM COMMON XPT Control
  APIs which are used to control COMMON XPT functionality.
*/

/*==============================================================================

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_common_xpt.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/27/15   aa      Compiler warning fix
02/25/15   aa      Discard points for AMPM scaling algorithm
10/15/14    cd     Fix compiler warnings
12/27/13   rsr     Add compression pt calculation to proc done handler.
11/14/13   cd      Support for RFLM-based Tx AGC override
09/03/13   dbz     Merge self-test related features
07/24/13    pv     Support for Delay vs Temp comp Enum.
06/24/13   cri     Add support for rc process variation during tx override
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
05/01/13   mpt     Added xpt_type field to mline packet to determine env scale
03/05/13   aro     Enhanced XPT override command and migrated to FTM common
                   dispatch
03/04/13   aro     Command to perform XPT cal config
02/26/13   cd      Added Tx override action type for override from NV
02/22/13    id     Support for new MLine computation using RMS dBFs
01/23/13   mpt     Updated to support the new changes to dpd file struct 
01/22/13   aro     Perform mapping of XPT capture type defined by tools 
                   to embedded enum
01/22/13   aro     Added common XPT capture type
01/10/13   mpt     Updated M-Line Calculation and added ftm_common_xpt_get_dpd_data()
12/27/12   cd      Added invalid value handling for XPT Tx override
11/26/12   mpt     Added Compute Mline API
11/13/12    id     Initial Check in for M Line Calculation
10/25/12   cd/Saul Initial Release. Generalized load DPD table API
                   - Changed request packet type
                   - Added response packet type
                   - Made XPT IQ capture common.

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "ftm_calibration_v3_xpt.h"
#include "rfcommon_core_xpt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FTM_COMMON_XPT_TX_OVR_RGI_INVALID       0xFFFF
#define FTM_COMMON_XPT_TX_OVR_PA_STATE_INVALID  0xFF
#define FTM_COMMON_XPT_TX_OVR_PA_CURR_INVALID   0xFFFF
#define FTM_COMMON_XPT_TX_OVR_SMPS_BIAS_INVALID 0xFFFF
#define FTM_COMMON_XPT_TX_OVR_IQ_GAIN_INVALID   0xFFFFFFFF
#define FTM_COMMON_XPT_TX_OVR_ENV_SCALE_INVALID 0xFFFFFFFF
#define FTM_COMMON_XPT_TX_OVR_DELAY_INVALID     20000
#define FTM_COMMON_XPT_TX_OVR_DELAY_THRESH      10000 /* 10000 * 0.1 = 1000ns,
                                                         1 usec max delay */
#define FTM_COMMON_XPT_TX_OVR_DPD_INFO_INVALID  255

#define XPT_CAL_MAX_AMAM_SIZE 256
#define XPT_CAL_EQ_FIR_COEF_SIZE 4
#define XPT_CAL_PREDIST_SIZE 16
#define XPT_CAL_EXPANDED_PREDIST_SIZE 64
#define XPT_CAL_AMPM_DISCARD_SAMPLES 60

/*----------------------------------------------------------------------------*/
/*! Enumeration to indicate Maximum number of max number of Measured and 
    Calculated powers for MLine Calc */
#define MAX_NUM_MEAS_PWRS 10
#define MAX_NUM_CALC_PWRS 10

/*----------------------------------------------------------------------------*/
/*! Enumeration for XPT Tx override Action Type */
typedef enum
{

  FTM_COMMON_XPT_TX_OVERRIDE_SKIP,
  /*!< Skip programming the Tx override field */

  FTM_COMMON_XPT_TX_OVERRIDE_VAL,
  /*!< Use the provided value for Tx override */

  FTM_COMMON_XPT_TX_OVERRIDE_DEFAULT,
  /*!< Use a SW provided default value for Tx override */

  FTM_COMMON_XPT_TX_OVERRIDE_NV,
  /*!< Use value from NV for Tx override */

  FTM_COMMON_XPT_TX_OVERRIDE_W_RC,
  /*!< Use the provided value for Tx override with RC tuner component */

  FTM_COMMON_XPT_TX_OVERRIDE_NV_WITH_TEMP_COMP,
  /*!< Use value from NV for Tx override with Delay vs temp comp */

  FTM_COMMON_XPT_TX_OVERRIDE_NUM

} ftm_common_xpt_tx_ovr_action_type;

/*----------------------------------------------------------------------------*/
/*! Enumeration for XPT type */
typedef enum
{

  XPT_TYPE_EPT,
  /*!< EPT */

  XPT_TYPE_ET,
  /*!< ET */

  XPT_TYPE_MAX
  /*!< XPT TYPE MAX */

} rf_common_xpt_type;

/*----------------------------------------------------------------------------*/
/*! Enumeration for XPT DPD Override type */
typedef enum
{

  RF_COMMON_EPT_DPD_OVERRIDE_TYPE,
  /*!< EPT IQ dpd override type */

  RF_COMMON_ET_DPD_OVERRIDE_TYPE,
  /*!< ET IQ dpd override type */

  RF_COMMON_DPD_OVERRIDE_NUM
  /*!< DPD override type num */

} rf_common_xpt_dpd_override_type;


/*----------------------------------------------------------------------------*/
/*! Enumeration for M Line Curve Fit */
typedef enum
{
  MLINE_CURVE_FIT_LINEAR = 0,
  /*!< Linear Curve Fit */

  MLINE_CURVE_FIT_QUADRATIC = 1,
  /*!< Quadratic Curve Fit */

  MLINE_CURVE_FIT_THIRD_ORDER = 2,
  /*!< Third Order Curve Fit */

  MLINE_CURVE_FIT_MAX_NUM
  /*!< MLine Curve Fit max num */
} ftm_common_mline_curve_fit_type;

typedef enum
{
  FTM_COMMON_SELFTEST_FBRX_GAIN_0,
  FTM_COMMON_SELFTEST_FBRX_GAIN_1,

} ftm_common_xpt_selftest_fbrx_gain_type;

/*! Enumeration for bit masks to be used as M Line failure indicators */
typedef enum
{

  MLINE_DEBUG_MASK_NDB_COMPR_PT_FAILURE = 0,
  /*! Did not achieve expected compression for M Line */

  MLINE_DEBUG_MASK_EXPPWR_GT_MAXPWR_MEAS = 1,
  /*! Did not find a power in the M Line table 
    greater than requested power for measured powers */

  MLINE_DEBUG_MASK_EXPPWR_LT_MAXPWR_MEAS = 2,
  /*! Did not find a power in the M Line table less 
    than requested power  for measured powers*/

  MLINE_DEBUG_MASK_INTBIAS_LT_VMAX_MEAS = 3,
  /*! Selected Bias > Vdmax but interpolated Bias < Vmax 
    for measured powers*/

  MLINE_DEBUG_MASK_INTBIAS_GT_VMAX_MEAS = 4,
  /*! Selected Bias > Vdmax and interpolated Bias > Vmax 
    for measured powers */

  MLINE_DEBUG_MASK_EXPPWR_GT_MAXPWR_CALC = 5,
  /*! Did not find a power in the M Line table 
    greater than requested power for calculated powers*/

  MLINE_DEBUG_MASK_EXPPWR_LT_MAXPWR_CALC = 6,
  /*! Did not find a power in the M Line table less 
    than requested power for calculated powers */

  MLINE_DEBUG_MASK_INTBIAS_LT_VMAX_CALC = 7,
  /*! Selected Bias > Vdmax but interpolated Bias < Vmax 
    for calculated powers */

  MLINE_DEBUG_MASK_INTBIAS_GT_VMAX_CALC = 8,
  /*! Selected Bias > Vdmax and interpolated Bias > Vmax 
    for calculated powers */

  MLINE_DEBUG_MASK_NEG_BIAS_SLOPE = 9
  /*! Progressively decreasing Bias condition in the M Line table*/

}ftm_common_xpt_mline_debug_mask;

/*----------------------------------------------------------------------------*/
/*! Structure for XPT Cal Input Data */
typedef PACK(struct)
{
  uint8 rgi;
  /*!< XPT RGI used during IQ Capture */

  uint32 bias;
  /*!< XPT Bias used during IQ Capture */

} ftm_common_xpt_cal_input_data;

/*----------------------------------------------------------------------------*/
/*! Structure for XPT Cal Output Data */
typedef PACK(struct)
{
  float pout_meas;
  /*!< Power measured during IQ Capture */

  float rms_amam;
  /*!< RMS value of measured AMAM */ 

} ftm_common_xpt_cal_output_data;

/*----------------------------------------------------------------------------*/
/*! Structure for XPT Cal Output Data (no floats) */
typedef PACK(struct)
{
  uint32 pout_meas_db100;
  /*!< Power measured during IQ Capture */

} ftm_common_xpt_cal_output_int_data;

/*----------------------------------------------------------------------------*/
/*! Structure for XPT cal AMAM results */
typedef struct
{
  uint16 amam_dac[XPT_CAL_MAX_AMAM_SIZE];
  /*!< Vins to represent the AMAM Data */

  uint16 amam_val[XPT_CAL_MAX_AMAM_SIZE];
  /*!< Vouts to represent the AMAM Data */

} ftm_common_xpt_cal_amam;

/*----------------------------------------------------------------------------*/
/*! Structure for XPT cal AMAM results (notfloats)*/
typedef struct
{
  uint32 vin_x100[XPT_CAL_MAX_AMAM_SIZE];
  /*!< Vins to represent the AMAM Data */

  int32 vout_x100[XPT_CAL_MAX_AMAM_SIZE];
  /*!< Vouts to represent the AMAM Data */

} ftm_common_xpt_cal_amam_int_data;

/*----------------------------------------------------------------------------*/
/*! Structure for XPT cal Data */
typedef struct
{
  ftm_common_xpt_cal_input_data xpt_cal_inputs;
  /*!< XPT Cal Inputs */

  ftm_common_xpt_cal_output_data xpt_cal_outputs;
  /*!< XPT Cal Outputs */

  ftm_common_xpt_cal_amam xpt_cal_amams;
  /*!< XPT Cal AMAMs */

} ftm_common_xpt_cal_data;

/*----------------------------------------------------------------------------*/
/*! Structure for XPT cal PC Data */
typedef PACK(struct)
{
  ftm_common_xpt_cal_input_data xpt_cal_inputs;
  /*!< XPT Cal Inputs */

  ftm_common_xpt_cal_output_int_data xpt_cal_outputs;
  /*!< XPT Cal Outputs */

} ftm_common_xpt_cal_int_data;

/*----------------------------------------------------------------------------*/
/*! Structure for RGI Deltas Sweep Data */
typedef struct
{
  uint8 rgi;
  /*!< RGI used for RGI Deltas Sweep */

  float pmeas;
  /*!< Power measured for the above RGI during the RGI Deltas Sweep */

} ftm_common_rgi_delta_data;


/*----------------------------------------------------------------------------*/
/*! Structure for RGI Deltas Sweep Data */
typedef PACK(struct)
{
  uint8 rgi;
  /*!< RGI used for RGI Deltas Sweep */

  uint32 pmeas_db100;
  /*!< Power measured for the above RGI during the RGI Deltas Sweep */

} ftm_common_rgi_delta_int_data;

/*----------------------------------------------------------------------------*/
/*! Structure for M Line Params */
typedef struct
{
  float lin_reg_percent;
  /*!< Linear Region Percentage to be used in M Line Calculation */

  float compression_db;
  /*!< Compression Point to be used in M Line Calculation */

  ftm_common_mline_curve_fit_type curve_fit_type;
  /*!< M Line Curve Fit Type */

  float rms_dbfs;
  /*!< RMS DBFS value of the modulated waveform to be used in 
    M Line Calculation */

  int32 env_scale;
  /*!< Envelope Scale value for M Line calculation */

} ftm_common_xpt_mline_params;

/*----------------------------------------------------------------------------*/
/*! Structure for holding Internal M Line Data */
typedef struct
{
  uint8 sp1_rgi;
  /*!< RGI of first sweep to be used in M Line Calculation */

  float sp1_vin;
  /*!< Vin of first sweep to be used in M Line Calculation */

  float sp1_pout;
  /*!< Pout of first sweep to be used in M Line Calculation */

  float sp1_vout;
  /*!< Vout of first sweep to be used in M Line Calculation */

  uint32 sp1_smps;
  /*!< SMPS of first sweep to be used in M Line Calculation */

  float sp1_vcc;
  /*!< Vcc of first sweep to be used in M Line Calculation */

  float sp1_rms_amam;
  /*!< RMS of first sweep to be used in M Line Calculation */

  float vin_scaled[XPT_CAL_MAX_AMAM_SIZE];
  /*!< Scaled Vin from first sweep to be used in M Line Calculation */

  float vout_scaled[XPT_CAL_MAX_AMAM_SIZE];
  /*!< Scaled Vin from first sweep to be used in M Line Calculation */

} ftm_common_xpt_mline_sp1_data;

/*----------------------------------------------------------------------------*/
/*! Structure for M Line Results */
typedef struct
{
  float res_vcc;
  /*!< Vcc calculated from M Line */

  float res_vout_peak;
  /*!< Peak Vout calculated from M Line */

  float res_pout_peak;
  /*!< Peak Pout calculated from M Line */

  float res_pout;
  /*!< Pout calculated from M Line */

  uint32 res_smps;
  /*!< SMPS calculated from M Line */

} ftm_common_xpt_mline_res;

/*----------------------------------------------------------------------------*/
/*! Structure for M Line Outputs */
typedef struct
{
  uint32 bias_out;
  /*!< Bias calculated from M Line */

  float pow_out;
  /*!< Power calculated from M Line */

} ftm_common_xpt_mline_outputs;

/*----------------------------------------------------------------------------*/
/*! Structure for M Line Outputs (no floats) */
typedef PACK(struct)
{
  uint8 rgi;
  /*!< XPT RGI used during IQ Capture */

  uint32 bias_out;
  /*!< Bias calculated from M Line */

  uint32 pow_out_db10;
  /*!< Power calculated from M Line */

} ftm_common_xpt_mline_outputs_int_data;

/*----------------------------------------------------------------------------*/
/*! Structure for M Line Points */
typedef struct
{
  float vin;
  /*!< Vin calculated from M Line for the ndb compression point */

  float vout;
  /*!< Vout calculated from M Line for the ndb compression point */

  float vcc;
  /*!< Vcc used in M Line compute */

} ftm_common_xpt_mline_points;

/*----------------------------------------------------------------------------*/
/*! Structure for DPD File */
typedef PACK(struct)
{
  uint8 capt_status;
  /*!< Capture Status from FW (0 = success) */

  uint8 proc_status;
  /*!< Processing Status from FW (0 = success) */

  uint16 num_amam_samples;
  /*!< Number of AMAM samples (=256) */

  uint16 num_ampm_samples; 
  /*!< Number of AMPM samples (=256) */

  uint16 amam[XPT_CAL_MAX_AMAM_SIZE];
  /*!< AMAM results */

  int16 ampm[XPT_CAL_MAX_AMAM_SIZE];
  /*!< AMPM results*/

  int32 rx_dc_i; 
  /*!< Rx DC I */

  int32 rx_dc_q;
  /*!< Rx DC Q*/

  uint32 rx_pre_eq_rms_mag;
  /*!< Rx pre-EQ RMS Mag */

  int32 eq_fir_coefficients[XPT_CAL_EQ_FIR_COEF_SIZE];
  /*!< EQ FIR coefficients */

  int8 eq_fir_shift;
  /*!< EQ FIR shift */

  uint16 tx_rms_mag; 
  /*!< Tx RMS Mag */

  uint16 rx_post_eq_rms_mag;
  /*!< Rx Post_EQ RMS Mag */

  int16 rx_peak_offset;
  /*!< Rx Peak Offset */

  int16 post_eq_snr;
  /*!< Post-EQ SNR*/

  int16 post_eq_par;
  /*!< Post-EQ PAR*/

  int16 post_eq_par_max;
  /*!< Post-EQ PAR for max peak*/

  uint16 num_am_predist;
  /*!< Num. entries in AM Predistortion table=16 */

  uint16 num_pm_predist;
  /*!< Num. entries in PM Predistortion table=16 */

  uint16 am_predist_table[XPT_CAL_PREDIST_SIZE];
  /*!< AM predistortion table */

  int16 pm_predist_table[XPT_CAL_PREDIST_SIZE];
  /*!< PM predistortion table */

  int8 am_dist_shift;
  /*!< AM distortion shift */

  uint16 am_expanded_predist_table[XPT_CAL_EXPANDED_PREDIST_SIZE]; 
  /*!< Expanded AM predistortion table */

  int16 pm_expanded_predist_table[XPT_CAL_EXPANDED_PREDIST_SIZE]; 
  /*!< Expanded PM predistortion table */

} ftm_common_xpt_dpd_file;

/*----------------------------------------------------------------------------*/
/*! Structure for M Line Compute response packet */

typedef PACK(struct)
{ 
  uint8   cmd;
  uint8   sub_sys;
  uint16  mode;
  uint16  subsys_cmd;
  uint16  req_len;
  uint16  rsp_len;
  uint16  payload_len;    /* length of payload field in bytes */
  uint8   payload;        /* start of the rsp payload. the size can be deterined by payload_len  */
}ftm_xpt_mline_compute_rsp_pkt_type;

/*----------------------------------------------------------------------------*/
/*! Structure defining the request packet for XPT cal configure command */
typedef PACK(struct)
{
  uint8   cmd;
  uint8   sub_sys;
  uint16  mode;
  uint16  subsys_cmd;
  uint16  req_len;
  uint16  rsp_len;
  uint8   pri_device;
  uint8   fb_device;
  uint8   xpt_mode;
} ftm_xpt_cal_configure_req_type;

/*----------------------------------------------------------------------------*/
/*! Structure defining the response packet for XPT cal configure command */
typedef PACK(struct)
{
  uint8   cmd;
  uint8   sub_sys;
  uint16  mode;
  uint16  subsys_cmd;
  uint16  req_len;
  uint16  rsp_len;
  uint8   status;
} ftm_xpt_cal_configure_rsp_type;

/*----------------------------------------------------------------------------*/
/*! Structure for M Line Compute request packet data*/

typedef PACK(struct)
{
  uint8   xpt_type;
  uint32  lin_region_pct_x100;
  uint32  comp_pt_x10;
  uint8   curve_fit_type;
  uint8   num_xpt_cal;
  uint8   num_rgi_delta;
  uint8   dpd_file_name_len;
  uint8   payload[];               /* start of the req pkt payload. */
}ftm_xpt_mline_compute_type;


/*----------------------------------------------------------------------------*/
boolean
ftm_common_xpt_dpd_override
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb,
  uint8 xpt_type,
  uint16 num_amam,
  uint16 *amam_tbl_ptr,
  uint16 num_ampm,
  int16 *ampm_tbl_ptr,
  uint16 num_epdt,
  uint16 *epdt_tbl_ptr
);

/*----------------------------------------------------------------------------*/
/*! Struct to hold M Line results table*/

typedef struct
{
  uint8 num_mline_table_rows;
  ftm_common_xpt_mline_outputs_int_data mline_results[RFCOM_TXLIN_NUM_SEGMENTS];
}ftm_calibration_mline_result_table_type;

/*----------------------------------------------------------------------------*/
/*! Struct to hold XPT Selection Sweep elements*/

typedef struct
{
  uint8 selec_swp_valid;
  uint8 selec_swp_rgi;
  int16 selec_swp_pwr;
  uint32 selec_swp_bias;
}ftm_calibration_mline_selec_swp_op;

/*----------------------------------------------------------------------------*/
/*! Struct to hold XPT Selection sweep table*/

typedef struct
{
  uint8 num_selec_swp_pwrs;
  ftm_calibration_mline_selec_swp_op mline_selec_swp[MAX_NUM_MEAS_PWRS];
}ftm_calibration_mline_result_selec_sweep_type;

/*----------------------------------------------------------------------------*/
/*! Struct to hold XPT Compression point list*/

typedef struct
{
  uint8 num_compression_points;
  uint8 compression_point_list[MAX_NUM_MEAS_PWRS];
}ftm_calibration_mline_result_compression_point_type;

/*----------------------------------------------------------------------------*/
/*! Struct to hold XPT AMAM RMS values*/

typedef struct
{
  uint8 num_amam_rms_values;
  uint32 amam_rms[MAX_NUM_MEAS_PWRS];
}ftm_calibration_mline_result_amam_rms_type;

/*----------------------------------------------------------------------------*/
/*! Struct to hold XPT Slope-offset elements*/

typedef struct
{
  int32 vout_vin_a; //slope
  int32 vout_vin_b; //offset
  int32 vout_vin_c;
  int32 vcc_vin_d;
  int32 vcc_vin_e;
  int32 vcc_vin_f;
}ftm_calibration_mline_result_slope_type;

/*----------------------------------------------------------------------------*/
/*! Struct to hold M Line results*/

typedef struct
{
   ftm_calibration_mline_result_table_type mline_table_result;
   ftm_calibration_mline_result_selec_sweep_type selec_swp_calc_result;
   ftm_calibration_mline_result_selec_sweep_type selec_swp_meas_result;
   ftm_calibration_mline_result_compression_point_type compression_point_result;
   ftm_calibration_mline_result_slope_type mline_slope_result;
   ftm_calibration_mline_result_amam_rms_type amam_rms_result;
   uint32 mline_debug_mask;
}ftm_calibration_mline_result_xpt_type;

/*----------------------------------------------------------------------------*/
/*! Struct to hold M Line results for each XPT Type*/

typedef struct
{
  ftm_calibration_mline_result_xpt_type ept_mline_results;
  ftm_calibration_mline_result_xpt_type et_mline_results;
}ftm_calibration_mline_result_module_type;

/*----------------------------------------------------------------------------*/
void
ftm_common_xpt_find_ndb_point
(
   float* vin,
   float* vout,
   uint16 size,
   float compdb,
   float lin_region,
   uint8* ndb_point,
   uint8 debug_msg
);

/*----------------------------------------------------------------------------*/
void
ftm_common_xpt_find_max_gain_ndb_point
(
   float* vin,
   float* vout,
   uint16 size,
   float compdb,
   uint8* compv_out,
   uint8* ndb_point,
   uint8* foundCompPt,
   uint8 debug_msg
);

/*----------------------------------------------------------------------------*/
boolean
ftm_common_xpt_mline_calc
(
  ftm_common_xpt_cal_data* xpt_cal_data,
  uint8 num_xpt_cal_data,
  ftm_common_rgi_delta_data* rgi_delta_data,
  uint8 num_rgi_delta_data,
  ftm_common_xpt_mline_params* xpt_mline_params,
  ftm_common_xpt_mline_outputs* mline_outputs,
  ftm_calibration_mline_result_xpt_type* mline_results,
  uint8 debug_msg
);

/*----------------------------------------------------------------------------*/
boolean
ftm_common_xpt_get_dpd_data
(
   char* file_name,
   ftm_common_xpt_dpd_file* data_ptr
);

/*----------------------------------------------------------------------------*/
rfcommon_xpt_capture_type
ftm_common_xpt_map_capture_type
(
  ftm_calibration_xpt_iq_capture_type capture_type
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_common_xpt_cal_configure
(
  void* ftm_req_data,
  ftm_rf_technology_type ftm_rf_tech
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_common_xpt_override_config
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
void
ftm_common_xpt_convert_delay
(
  int32 delay_unit_1000th_of_sample,
  uint32 samp_rate,
  int32 * delay_unit_p1ns
);

/*----------------------------------------------------------------------------*/
int16
ftm_common_xpt_find_comppt
(
   uint16* amam,
   uint16 size
);

/*----------------------------------------------------------------------------*/
boolean
ftm_common_xpt_set_curr_tx_override
(
  rfm_device_enum_type device,
  rfcommon_core_xpt_tx_override_type *tx_ovr_params
);

/*----------------------------------------------------------------------------*/
boolean
ftm_common_xpt_get_curr_tx_override
(
  rfm_device_enum_type device,
  rfcommon_core_xpt_tx_override_type *curr_tx_ovr
);

/*----------------------------------------------------------------------------*/
void calcLinearFactors( float* xin, float* yin, float* a, float* b, uint8 size);

/*----------------------------------------------------------------------------*/
void interp1(float *x, int num_x, float *y, float *xx, int num_xx, float *yy);
#ifdef __cplusplus
}
#endif

#endif /* FTM_COMMON_XPT_H */
