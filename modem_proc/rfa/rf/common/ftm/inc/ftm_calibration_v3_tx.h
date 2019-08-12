#ifndef FTM_CALIBRATION_V3_TX_H
#define FTM_CALIBRATION_V3_TX_H
/*!
  @file
  ftm_calibration_v3_tx.h

  @brief
  This module contains prototypes and definitions used by common RF calibration
  code.
*/

/*==============================================================================

  Copyright (c) 2012 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_calibration_v3_tx.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/07/14    br     TxRSB-Add support to compute TxIQMC offsets and process final Tx IQMC 
07/17/14    dj     Tx Measure opcode update to read FB power
02/14/14    id     Fix for compile error for Dime PM PL with rflm file include
02/13/14    id     Updates needed for FBRx Cal API to match FBRx capture API
02/13/14    id     Fix for FB Gain Meas addition in Tx Meas Opcode
04/29/13   cd      Apply Pin alignment during calibration
04/24/13   cd      Add support for Therm read as part of Tx Meas Opcode
04/23/13   cri     Add reference channel to tx override
04/15/13   aro     Mainline calv3 optimization code/delete obsolete code
04/01/13   aro     Fixed random buffer size initialization
04/01/13   aro     Debug flag to switch between old and new calv3 implementation
04/01/13   aro     Added hookup for new calv3 infrastructure
04/01/13   aro     F3 update
03/29/13   aro     Updated Tx Override OpCode to support modular payload
03/29/13   aro     Radio Config/Retune optimized opcode proc functions
03/29/13   aro     Tx Config/Meas optimized opcode proc functions
03/29/13   aro     Rx Config/Meas optimized opcode proc functions
12/03/12   aro     Added support for Tx measure results buffer
11/30/12   aro     Initial support for Tx measure opcode
11/30/12   aro     Added type definitions for Tx measurement
11/15/12   aro     Tx Override 1x handler function
11/05/12   aro     Removed instruction payload pointer from process
                   instruction function
11/05/12   aro     Added RFM Mode validity checking
11/05/12   aro     Removed OpCode init from opcode modules to common cal v3
                   init function
11/02/12   aro     Converted payload computation function to perform
                   opcode payload unpacking
10/31/12   aro     Updated oayload size query function to decrypt teh payload
                   to send it to callback function during opcode processing
10/31/12   aro     Function to process Tx override Opcode
10/31/12   aro     Added function to query the paylaod size of the current
                   Tx override opcode
10/30/12   aro     Interface to initialize tx override opcode
10/30/12   aro     Interface to deinitialize tx override opcode
10/30/12   aro     Added type definition for tx Override OpCode
10/29/12   aro     Initial version

==============================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Static Global Variable to store the delta phi and alpha factor constants in Tx RSB cal
*/

static int32 delta_phi[4] = {47453133,47453133,-47453133,-47453133};
static int32 alpha_factor[4] = {1123362656,1028317345,1028317345,1123362656};

/*!
  @brief
  Static Global Variable to store the cfinal constant used to calculate eps and dphi in Tx RSB cal
*/
static int32 cfinal = 1518500250;
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration of all Tx Calibration Actions.
*/
typedef enum
{
  TX_ACTION_RGI_OVERRIDE         = 0x0,
  /*!< Perform Tx RGI override */

  TX_ACTION_FB_RX_ENABLE            = 0x1,
  /*!< Enable Tx Feedback receiver */
  
  TX_ACTION_FB_RX_DISABLE           = 0x2,
  /*!< Disable Tx Feedback receiver */
    
  TX_ACTION_MOBILE_TRANSMIT_DIV  = 0x4,
  /*!< Enable Mobile Transmit Diversity */

} ftm_calibration_tx_action_enum_type;

/*----------------------------------------------------------------------------*/
/*! Enumeration of Tx waveform type to be used for calibration */
typedef enum
{
  FTM_CAL_TX_WF_OFF = 0,
  /*!< Enumeration indicating Tx should be turned off */

  FTM_CAL_TX_WF_CW = 1,
  /*!< Enumerataion indicating that Tx should be enabled with continuous 
  waveform (tone)*/

  FTM_CAL_TX_WF_CDMA_RC1 = 2,
  /*!< Enumerataion indicating that Tx should be enabled with CDMA RC1 
  waveform*/

  FTM_CAL_TX_WF_CDMA_RC3 = 3,
  /*!< Enumerataion indicating that Tx should be enabled with CDMA RC3
  waveform*/

  FTM_CAL_TX_WF_WCDMA = 4,
  /*!< Enumerataion indicating that Tx should be enabled with WCDMA waveform */

  FTM_CAL_TX_WF_LTE_PUSCH = 5
  /*!< Enumerataion indicating that Tx should be enabled with LTE PUSCH
  waveform */

} ftm_cal_tx_waveform_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Maximum allowed Tx results size in bytes for the current segment.
 
  @details
  This enum defines the Maximum allowed results size for the current segment.
  The mathematics behind this maximum is based on following.
  
  For Tx measurement, one Tx measurement is 2 bytes. In a segment,
  the assumption is that HDET and LPM HDET can be measured on one Tx path. Thus
  the maximum Tx results size can be (2 x 2 x 1) = 4 bytes.
*/
#define FTM_CAL_SEG_TX_BUFF_SIZE_MAX 4

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration to indicate invalid RGI
*/
#define FTM_CALIBRATION_INVALID_RGI 0xFF

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration to Tx override Flag value to apply Pin alignment
*/
#define FTM_CALIBRATION_TX_OVR_APPLY_PIN_ALIGNMENT  1

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration to indicate size of Cross Correlation Results durign FBRX Gain
  Cal Meas.
*/
#define FTM_CALIBRATION_FBRX_GAIN_MEAS_XCORR_LOG_SIZE 32

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Type/Size of Tx Calibration Result
*/
typedef uint16 ftm_calibration_tx_meas_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Type of Tx Action Mask for FTM Calibration Command. Tx Actions are defined in
  #ftm_calibration_tx_action_enum_type
*/
typedef uint8 ftm_calibration_tx_action_mask_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for Tx Measurement configuration Instruction.

  @details
  Contains the Tx Merasurement configuration instruction (Payload).
  To perform Tx RGI override, RFI index must be invalid. Thus, if RGI
  override is to be skipped, then invalid RFI of #FTM_CALIBRATION_INVALID_RGI
  should be passed from tools.
*/
typedef PACK(struct)
{
  ftm_calibration_tx_action_mask_type cal_action;
  /*!<        
  @brief
  The transmit calibration actions. 
   
  @details 
  This indicates the Tx action to be performed with this opcode 
  on a given segment.Tx Action definitions can be found in 
  #ftm_calibration_tx_action_enum_type.
  */

  uint8 pa_state;
  /*!< PA State to which the TxAGC is to be overridden during Tx Measurement */

  uint8 rgi;
  /*!< RTR Gain Index to be overridden to. If the RGI is invalid 
  (#FTM_CALIBRATION_INVALID_RGI), then Tx RGI override will be skipped. */

  union
  {
    struct
    {
      uint16 pa_bias;
      /*!< Lower 16 bits kept as bias voltage for backwards compatibility */
      uint16 quiscent_current : 15;
      /*!< PA quiescent current setting */
      uint16 quiscent_current_en : 1;
      /*!<  1 bit quiescent current control enable indicator */
    } pa_ctl_struct;
    /*! @brief APT PA stucture with q_current control for Cal V3*/

    uint32 pa_bias;
    /*!< PA Bias value to be applied. Legacy field*/
  };

  ftm_calibration_device_flag_mask_type device_flag;
  /*!< 
  @brief
  Tx Flag Mask indicating which device to calibrate. 

  @details 
  This is a mask indicating which devices are to be calibrated simultaneously. 
  Following are the definitions of the bits defining Device Calibration 
  -# B[0] : RFM Device 0 calibration 
  -# B[1] : RFM Device 1 calibration 
  -# B[2] : RFM Device 2 calibration 
  -# B[3] : RFM Device 3 calibration 
  -# B[4] : RFM Device 4 calibration 
  -# B[5] : RFM Device 5 calibration 
  -# B[6] : RFM Device 6 calibration 
  -# B[7] : RFM Device 7 calibration 

  The bits are defined in by #ftm_calibration_device_flag_mask_enum_type 
  */
  
} ftm_cal_instr_payload_config_tx_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for HDETMeasurement configuration Instruction.

  @details
  Contains the HDET Merasurement configuration instruction (Payload)
*/
typedef PACK(struct)
{
  uint8 hdet_sample;
  /*!< Number of HDET Samples to be taken for averaging */

  uint8 lpm_hdet_sample;
  /*!< Number of LPM HDET Samples to be taken for averaging */

  ftm_calibration_device_flag_mask_type device_flag;
  /*!< 
  @brief
  Tx Flag Mask indicating which device to calibrate. 

  @details 
  This is a mask indicating which devices are to be calibrated simultaneously. 
  Following are the definitions of the bits defining Device Calibration 
  -# B[0] : RFM Device 0 calibration 
  -# B[1] : RFM Device 1 calibration 
  -# B[2] : RFM Device 2 calibration 
  -# B[3] : RFM Device 3 calibration 
  -# B[4] : RFM Device 4 calibration 
  -# B[5] : RFM Device 5 calibration 
  -# B[6] : RFM Device 6 calibration 
  -# B[7] : RFM Device 7 calibration 

  The bits are defined in by #ftm_calibration_device_flag_mask_enum_type 
  */
  
} ftm_cal_instr_payload_meas_hdet_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold Tx calibration Data
*/
typedef struct
{
  uint8 *tx_buffer;
  /*!< 
  @brief
  Pointer to the Tx Result buffer for current segment to be filled by tech 
  specific Tx measurement function.
   
  @details
  Container to store the calibration results. Each calibration result will
  of size #ftm_calibration_tx_meas_data_type. All these results will be packed
  together. The order of calibration results will be as following.
  - INT16 : RFM Device 0 HDET Measurement
  - INT16 : RFM Device 0 LPM HDET
  - INT16 : RFM Device 1 HDET Measurement
  - INT16 : RFM Device 1 LPM HDET
  - <i>Other Tx measurement for Remaining devices </i> 
  - INT16 : RFM Device 7 HDET Measurement
  - INT16 : RFM Device 7 LPM HDET 
   
  Based on Tx Flag mask, all above listed Tx measurement 
  may not be present. In that case, <b>all data should still be packed 
  together. </b>
  */ 

  uint8 num_of_tx_reads;
  /*!< Pointer to the container where tech specific Tx measurement query 
  function fills the number of Tx reads taken during this Tx measurement. */

  uint16 max_tx_buffer_len;
  /*!< Maximum result buffer that can be used to store Tx results for 
  this segment. Tech specific Tx measurement query function should check 
  against this number to avoid buffer overflow. */ 

} ftm_calibration_tx_result_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Function Pointer definition for Callback function to configure Tx
 
  @details
  This function will configure Tx for Tx measurement. This function will 
  basically override Tx with a given RGI, Betap Gain, PA Bias, and PA state.
*/ 
typedef boolean (*ftm_calibration_cb_config_tx_fp_type)
(
  ftm_cal_instr_payload_config_tx_type const *config_tx
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Function Pointer definition for Callback function to mesaure Tx
 
  @details
  This funtion will perform HDET read and report that as Tx Calibration
  result.
 
  @param rx_meas
  Pointer to the Rx measurement Opcode to be used during Rx measurement.
 
  @param tx_result
  Container to store the calibration results. Each calibration result will
  of size #ftm_calibration_tx_meas_data_type. All these results will be packed
  together. The order of calibration results will be as following.
  - RFM Device 0 HDET Measurement
  - RFM Device 0 LPM HDET
  - RFM Device 1 HDET Measurement
  - RFM Device 1 LPM HDET
  - <i>Other Tx measurement for Remaining devices </i> 
  - RFM Device 7 HDET Measurement
  - RFM Device 7 LPM HDET 
   
  Based on Tx Flag mask, all above listed Tx measurement 
  may not be present. In that case, <b>all data should still be packed 
  together. </b>
 
  Tech specific measurement function will also need to populate the total number
  of measurements taken.
 
  As a part of this parameter, maximum size of result buffer for this segment
  is also sent. Therefore the tech specific measure function should do a
  check in order to prevent buffer overflow. 
 
*/
typedef boolean (*ftm_calibration_cb_measure_tx_fp_type)
(
  ftm_cal_instr_payload_meas_hdet_type const *meas_hdet,
  ftm_calibration_tx_result_type *tx_result
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Maximum allowed Tx meas results size in bytes for the current segment.
*/
#define FTM_CAL_TX_MEAS_BUFF_SIZE_MAX 120

/*----------------------------------------------------------------------------*/
/*! Enumeration to indicate the maximum number of devices */
#define FTM_CAL_V3_TX_MAX_DEV 6

/*----------------------------------------------------------------------------*/
/*! Enumeration listing the Tx override types */
typedef enum
{
  TX_OVR_PA_STATE = 0,
  /*!< PA State override */

  TX_OVR_RGI = 1,
  /*!< RGI override */

  TX_OVR_PA_BIAS = 2,
  /*!< PA Bias override */

  TX_OVR_IQ_GAIN = 3,
  /*!< IQ Gain override */

  TX_OVR_ENV_SCALE = 4,
  /*!< Envelope Scale override */

  TX_OVR_QUISCENT_CURRENT = 5,
  /*!< PA quiscent current override */

  TX_OVR_DELAY = 6,
  /*!< TX Delay override */

} ftm_cal_tx_ovr_power_param_enum;

/*----------------------------------------------------------------------------*/
/*! Enumeration listing the Tx override XPT mode */
typedef enum
{
  TX_OVR_APT = 0,
  /*!< APT Mode */

  TX_OVR_EPT = 1,
  /*!< EPT */

  TX_OVR_ET = 2,
  /*!< ET */

} ftm_cal_tx_override_xpt_mode_enum;

/*----------------------------------------------------------------------------*/
/*! Enumeration listing the Tx override type */
typedef enum
{
  TX_OVR_POWER = 0,
  /*!< APT Mode */

  TX_OVR_APPLY_DPD = 1,
  /*!< EPT */

} ftm_cal_tx_override_type_mask_enum;

/*----------------------------------------------------------------------------*/
/*! Enumeration listing the Power override type */
typedef enum
{
  TX_OVR_POWER_IND_PARAM = 1,
  /*!< APT Mode */

  TX_OVR_POWER_ABS = 2,
  /*!< EPT */

} ftm_cal_tx_ovr_power_ovr_enum;

/*----------------------------------------------------------------------------*/
/*! Type defintion for Tx override action time */
typedef uint8 ftm_cal_tx_override_action_type;

/*----------------------------------------------------------------------------*/
/*! Type defintion for Tx override IQ Gain value */
typedef uint32 ftm_cal_tx_override_iq_gain_type;

/*----------------------------------------------------------------------------*/
/*! Type defintion for Tx override Envelope Scale value */
typedef uint32 ftm_cal_tx_override_env_scale_type;

/*----------------------------------------------------------------------------*/
/*! Type defintion for Tx override Envelope Scale value */
typedef int32 ftm_cal_tx_override_delay_type;

/*----------------------------------------------------------------------------*/
/*! Type defintion for Device mask */
typedef uint8 ftm_cal_tx_override_device_mask_type;

/*----------------------------------------------------------------------------*/
/*! Type defintion for XPT mode. This is defined by 
#ftm_cal_tx_override_xpt_mode_enum */
typedef uint8 ftm_cal_tx_override_xpt_mode_type;

/*----------------------------------------------------------------------------*/
/*! PA state override payload format */
typedef uint8 ftm_cal_tx_ovr_pa_state_payload_type;

/*----------------------------------------------------------------------------*/
/*! RGI override payload format */
typedef uint8 ftm_cal_tx_ovr_rgi_payload_type;

/*----------------------------------------------------------------------------*/
/*! PA Bias override payload format */
typedef uint32 ftm_cal_tx_ovr_bias_payload_type;

/*----------------------------------------------------------------------------*/
/*! Tx IQ Gain override payload format */
typedef PACK(struct)
{
  ftm_cal_tx_override_action_type ovr_action;
  /*!<  Action to be taken for given IQ Gain value. This is defined by 
  #ftm_common_xpt_tx_ovr_action_type */

  ftm_cal_tx_override_iq_gain_type iq_gain;
  /*!< Tx IQ Gain to which the digital Tx is to be overriden to */

} ftm_cal_tx_ovr_iq_gain_payload_type;

/*----------------------------------------------------------------------------*/
/*! Envelope Scale override payload format */
typedef PACK(struct)
{

  ftm_cal_tx_override_action_type ovr_action;
  /*!<  Action to be taken for given Envelope Scale value. This is defined by 
  #ftm_common_xpt_tx_ovr_action_type */

  ftm_cal_tx_override_env_scale_type env_scale;
  /*!< Envelope Scale which the digital Tx is to be overriden to */

} ftm_cal_tx_ovr_env_scale_payload_type;

/*----------------------------------------------------------------------------*/
/*! PA Quiscent current override payload format */
typedef uint32 ftm_cal_tx_ovr_quiscent_payload_type;

/*----------------------------------------------------------------------------*/
/*! DPD index override payload format */
typedef uint16 ftm_cal_tx_ovr_dpd_payload_type;

/*----------------------------------------------------------------------------*/
/*! Tx Power in dBm10 override payload format */
typedef int16 ftm_cal_tx_ovr_power_payload_type;

/*----------------------------------------------------------------------------*/
/*! Reference channel override payload format */
typedef uint32 ftm_cal_tx_ovr_ref_channel_payload_type;

/*----------------------------------------------------------------------------*/
/*! Type defintion for Power override. This is defined by 
#ftm_cal_tx_ovr_power_ovr_enum */
typedef uint8 ftm_cal_tx_ovr_power_ovr_type;

/*----------------------------------------------------------------------------*/
/*! Type defintion for Tx override param mask */
typedef uint32 ftm_cal_tx_ovr_power_param_type;

/*----------------------------------------------------------------------------*/
/*! Type defintion for Tx override type mask. This is defined 
by #ftm_cal_tx_override_type_mask_enum*/
typedef uint32 ftm_cal_tx_override_type_mask_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for Radio Setup Instruction.
*/
typedef PACK(struct)
{
  uint8 version;
  /*!< Version of the OpCode */

  uint8 debug_mode_en;
  /*!< Flag indicating, if the debug mode is enabled */

  uint16 flag;
  /*!< 16-bit flag indicating a specific type of action to be perform the 
  given payload. */

  ftm_cal_tx_override_device_mask_type device_mask;
  /*!< 8-bit mask indicating which for which devices the Tx override message 
  is sent. The number of sets of Tx override payload will equal the number of 
  set bits in the mask */

} ftm_cal_tx_override_instr_payload_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure used to hold the unpacked radio config parameter used to send
  to tech specific functions
*/
typedef struct
{
  boolean debug_mode_en;
  /*!< Flag indicating, if the debug mode is enabled */

  uint16 flag;
  /*!< 16-bit flag indicating a specific type of action to be perform the 
  given payload. */

  ftm_cal_tx_override_device_mask_type device_mask;
  /*!< 8-bit mask indicating which for which devices the Tx override message 
  is sent. The number of sets of Tx override payload will equal the number of 
  set bits in the mask */

  struct
  {
    ftm_cal_tx_override_xpt_mode_type xpt_mode;
    /*!< Variable to indicate the mode to which the Tx override is to be done */

    ftm_cal_tx_override_type_mask_type override_type;
    /*!< Mask indicating active override types. Each bit indicates a Tx 
    property to override. If the bit is enabled, there would be payload for 
    that bit which will have the value for the Tx param to be overriden */

    struct /* power_ovr */
    {
      ftm_cal_tx_ovr_power_ovr_type power_ovr_type;
      /*!< Variable to indicate the tx power override type.  */

      struct  /* individual_param */
      {

        ftm_cal_tx_ovr_power_param_type param_mask;
        /*!< Mask indicating the params for which the the Tx Override is to be 
            done. */

        struct
        {
              ftm_cal_tx_ovr_pa_state_payload_type gain_state;
          /*!< PA Gain state to which the PA is to be overriden to */
        } pa_state;
        /*!< PA State Override */

        struct
        {
              ftm_cal_tx_ovr_rgi_payload_type gain_index;
          /*!< RGI to which the WTR/RTR is to be overriden to */
        } rgi;
        /*!< RGI Override */

        struct
        {
              ftm_cal_tx_ovr_bias_payload_type pa_bias;
          /*!< PA Bias to which the PA is to be overriden to */
        } bias;
        /*!< PA Bias Override */

        struct
        {
              ftm_cal_tx_override_action_type ovr_action;
              /*!<  Action to be taken for given IQ Gain value. This is defined 
              by #ftm_common_xpt_tx_ovr_action_type */
            
              ftm_cal_tx_override_iq_gain_type iq_gain;
          /*!< Tx IQ Gain to which the digital Tx is to be overriden to */
        } iq_gain;
        /*!< Tx IQ Gain Override */

        struct
        {
              ftm_cal_tx_override_action_type ovr_action;
              /*!<  Action to be taken for given Envelope Scale value. This is 
              defined by #ftm_common_xpt_tx_ovr_action_type */
            
              ftm_cal_tx_override_env_scale_type env_scale;
          /*!< Envelope Scale which the digital Tx is to be overriden to */
        } env_scale;
        /*!< Envelope Scale Override */

        struct
        {
              ftm_cal_tx_ovr_quiscent_payload_type quiscent_current;
          /*!< PA Quiscent current which the PA is to be overriden to */
        } current;
        /*!< Quiscent current Override */

        struct
        {
          ftm_cal_tx_override_action_type ovr_action;
          /*!<  Action to be taken for given Envelope Scale value. This is 
          defined by #ftm_common_xpt_tx_ovr_action_type */
        
          ftm_cal_tx_override_delay_type delay_val;
          /*!< Chain delay which the digital Tx is to be overriden to */
        } delay;
        /*!< Envelope Scale Override */
      
      } individual_param;
      /*!< Structure to hold parameter for out_power override */

      struct /* abs_power */
      {
        ftm_cal_tx_ovr_power_payload_type power_dbm10;
        /*!< Tx power Override */

        ftm_cal_tx_ovr_ref_channel_payload_type ref_channel;
        /*!< Reference Channel */
      } abs_power;
    } power_ovr;

    struct /* apply_dpd */
    {
      struct /* dpd */
      {
        ftm_cal_tx_ovr_dpd_payload_type dpd_index;
        /*!< DPD which the Tx is to be overriden to */
      } dpd;
      /*!< DPD index Override */
    } apply_dpd;

  } tx_param[FTM_CAL_V3_TX_MAX_DEV];
  /*!< List if Tx parameter to be overriden */

} ftm_cal_tx_override_parameter_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Function Pointer definition to perform Tx override
 
  @details
  This function will tech specific function to peform Tx override
 
  @param device_mask
  Mask indicating which devices have tx param payload
 
  @param x_param
  Pointer to the structure containing the tx param for devices.
 
  @retval TRUE
  Indicates that the tx override is successful
 
  @retval FALSE
  Indicates that the tx override encountered failure.
*/
typedef boolean (*ftm_cal_tx_override_fp_type)
(
  ftm_cal_tx_override_parameter_type const *tx_override
);

/*----------------------------------------------------------------------------*/
/*! Type defintion for Device mask */
typedef uint8 ftm_cal_tx_meas_device_mask_type;

/*----------------------------------------------------------------------------*/
/*! Type defintion for Tx Measurement action mask */
typedef uint16 ftm_cal_tx_meas_action_params_mask_type;

/*----------------------------------------------------------------------------*/
/*! Type defintion for Numebr of samples for HDET samples */
typedef uint8 ftm_cal_tx_meas_hdet_sample_type;

/*----------------------------------------------------------------------------*/
/*! Type defintion for Numebr of samples for LPM HDET samples */
typedef uint8 ftm_cal_tx_meas_lpm_hdet_sample_type;

/*----------------------------------------------------------------------------*/
/*! Type defintion for HDET read */
typedef uint16 ftm_cal_tx_meas_hdet_read_type;

/*----------------------------------------------------------------------------*/
/*! Type defintion for LPM HDET read */
typedef uint16 ftm_cal_tx_meas_lpm_hdet_read_type;

/*----------------------------------------------------------------------------*/
/*! Type defintion for Linearizer Therm read */
typedef uint16 ftm_cal_tx_meas_lin_therm_read_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Type defintion for FB Gain Measurement Results 
*/
typedef PACK(struct) 
{
  uint8 fb_gain_state;
  /*!< FB Gain Meas Gain State */

  uint8 fbrx_mode;
  /*!< FBRx Mode */

  uint8 fb_meas_stat;
  /*!< FB Gain Measurement Status */

  int16 fb_gain_val;
  /*!< FB Gain Measurement Gain Result Value */

  uint32 fb_meas_loft;
  /*!< FB Gain Measurement LO FeedThrough Result */

  uint32 fb_meas_rsb;
  /*!< FB Gain Measurement RSB Result */

  int16 fb_meas_gain_imb;
  /*!< FB Gain Measurement Gain Imbalance Result */

  int16 fb_meas_phase_imb;
  /*!< FB Gain Measurement Phase Imbalance Result */

  uint32 fb_meas_ls_err;
  /*!< FB Gain Measurement LS Error in the Result */

  uint16 fb_meas_xcorr_log[FTM_CALIBRATION_FBRX_GAIN_MEAS_XCORR_LOG_SIZE];
  /*!< FB Gain Measurement Correlation Log of the Result */

} ftm_cal_tx_meas_fb_gain_res_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Type defintion for FB Gain Measurement Results 
*/
typedef int16 ftm_cal_tx_meas_fb_power_res_type;
  /*!< FB Gain Measurement Gain Result Value */

/*----------------------------------------------------------------------------*/
/*! Enumeration listing the Tx Measurement types */
typedef enum
{
  TX_MEAS_HDET = 0,
  /*!< HDET measurement */

  TX_MEAS_LPM_HDET = 1,
  /*!< LPM HDET measurement */

  TX_MEAS_LINEARIZER_THERM = 2,
  /*!< Linearizer Therm value measurement */

  TX_MEAS_FB_GAIN = 3,
  /*!< FB Gain measurement */

  TX_MEAS_FB_PWR = 4,
  /*!< FB Power measurement */

} ftm_cal_tx_meas_action_enum_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure used to hold the unpacked FB Gain meas parameter 
*/
typedef struct
{
  uint8 xpt_mode;
  /*!< XPT Mode for the measurement */

  int16 gain_comp;
  /*!< Gain Comp value used doe equalizing TA Interpolator Gain Droop */

  uint8 fb_gain_state;
  /*!< Gain state for the FBRx */

  uint8 ubias;
  /*!< UBias value for the FBRx Cal */

  uint16 rx_scale;
  /*!< Rx Scale value for the FBRx Cal */

  boolean debug_mode_en;
  /*!< Debug Mode Enable for the FBRx Cal */

} ftm_cal_tx_meas_fb_gain_payload_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure used to hold the unpacked FB Power meas parameter 
*/
typedef struct
{
  uint8 fb_gain_state;
  /*!< Gain state for the FBRx */

  uint8 fb_power_samples;
  /*!< Number of samples for the FBRx power measurment */

} ftm_cal_tx_meas_fb_power_payload_type;

/*----------------------------------------------------------------------------*/
/*! Data Structure to hold the Tx measurement configuration data for each 
device for which the bit is set */
typedef PACK(struct)
{
  ftm_cal_tx_meas_action_params_mask_type tx_meas_action;
  /*!< 16-bit mask indicating the Tx measurement action */

  uint8 payload_addr;
  /*!< Variable to get the address of the payload associated with the set bits 
  of tx_meas_action. */

} ftm_cal_tx_meas_param_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for Tx Measurement Instruction.
*/
typedef PACK(struct)
{
  uint8 version;
  /*!< Version of the OpCode */

  uint8 debug_mode_en;
  /*!< Flag indicating, if the debug mode is enabled */

  uint16 flag;
  /*!< 16-bit flag indicating a specific type of action to be perform the 
  given payload. */

  ftm_cal_tx_meas_device_mask_type device_mask;
  /*!< 8-bit mask indicating which for which devices the Tx Meas message 
  is sent. The number of sets of Tx Meas payload will equal the number of 
  set bits in the mask */

  ftm_cal_tx_meas_param_data_type tx_meas_data;
  /*!< Data Structure to hold the tx meas configuration data for each 
  device for which the bit is set */

} ftm_cal_tx_meas_instr_payload_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure used to hold the unpacked Tx meas parameter to be sent
  to tech specific functions
*/
typedef struct
{
  boolean debug_mode_en;
  /*!< Flag indicating, if the debug mode is enabled */

  uint16 flag;
  /*!< 16-bit flag indicating a specific type of action to be perform the 
  given payload. */

  ftm_cal_tx_meas_device_mask_type device_mask;
  /*!< 8-bit mask indicating which for which devices the Tx Meas message 
  is sent. The number of sets of Tx Meas payload will equal the number of 
  set bits in the mask */

  struct
  {
    ftm_cal_tx_meas_action_params_mask_type tx_meas_action;
    /*!< 64-bit mask indicating the Tx measurement action */

    struct
    {
      ftm_cal_tx_meas_hdet_sample_type hdet_sample;
      /*!< Number of HDET samples to be taken */

    } hdet;
    /*!< HDET measurement*/

    struct
    {
      ftm_cal_tx_meas_hdet_sample_type hdet_sample;
      /*!< Number of LPM HDET samples to be taken */

    } lpm_hdet;
    /*!< HDET measurement*/

    struct
    {
      ftm_cal_tx_meas_fb_gain_payload_type fb_gain_params;
      /*!< FB Gain Measurement Params */

    } fb_gain;
    /*!< FB Gain measurement*/

    struct
    {
      ftm_cal_tx_meas_fb_power_payload_type fp_power_params;
      /*!< FB Power Measurement Params */

    } fb_power;
    /*!< FB Power measurement*/

  } tx_meas_param[FTM_CAL_V3_TX_MAX_DEV];
  /*!< Tx Measurement parameter per enabled device */

} ftm_cal_tx_meas_parameter_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to be used by tech functions to push Tx results to calv3
  common framework
*/
typedef struct
{
  ftm_cal_tx_meas_device_mask_type device_mask;
  /*!< 8-bit mask indicating which for which devices the Tx Meas message 
  is sent. The number of sets of RT Meas payload will equal the number of 
  set bits in the mask */

  struct
  {
    ftm_cal_tx_meas_action_params_mask_type tx_meas_action;
    /*!< mask indicating the Tx measurement action */

    struct
    {
      ftm_cal_tx_meas_hdet_read_type hdet_read;
      /*!< Average HDET read */

    } hdet;
    /*!< Average LPM HDET read */

    struct
    {
      ftm_cal_tx_meas_lpm_hdet_read_type hdet_read;
      /*!< Number of LPM HDET samples to be taken */

    } lpm_hdet;
    /*!< HDET measurement*/

    struct
    {
      ftm_cal_tx_meas_lin_therm_read_type therm_read;
      /*!< Current Therm Read */

    } lin_therm;
    /*!< Linearizer Therm Measurement */

    struct
    {
      ftm_cal_tx_meas_fb_gain_res_type fb_meas_res;
      /*!< FB Gain Measurement Results */

    } fb_gain_meas;
    /*!< FB Gain Measurement Results */

    struct
    {
      ftm_cal_tx_meas_fb_power_res_type fb_pwr_meas_res;
      /*!< FB Power Measurement Results */

    } fb_power_meas;
    /*!< FB Gain Measurement Results */

  } tx_meas_param[FTM_CAL_V3_TX_MAX_DEV];
  /*!< Tx Measurement parameter per enabled device */

} ftm_cal_tx_meas_result_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Function Pointer definition to perform Tx Measurement
 
  @details
  This function will be called by v3 common framework to perform given Tx
  measurement action.
 
  @param tx_measure
  Pointer to the data structure containing the unpacked paylaod for Tx
  measurement.
 
  @param cal_result
  Pointer to the structure where the calibration result is stored
 
  @return
  Status indicating the status of execution of Tx measurement function
*/
typedef boolean (*ftm_cal_tx_measure_fp_type)
(
  ftm_cal_tx_meas_parameter_type const *tx_measure,
  ftm_cal_tx_meas_result_data_type *cal_result
);


/*----------------------------------------------------------------------------*/
/*! Structure to hold data pertaining to Config Tx OpCode */
typedef struct
{
  ftm_calibration_cb_config_tx_fp_type 
                           oc_process_fp[FTM_CALIBRATION_NUM_OF_TECHS];
  /*!< 
  @brief
  Function Pointer definition for Callback function
  */
} ftm_cal_config_tx_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold data pertaining to Meas HDET OpCode */
typedef struct
{
  ftm_calibration_cb_measure_tx_fp_type 
                           oc_process_fp[FTM_CALIBRATION_NUM_OF_TECHS];
  /*!< 
  @brief
  Function Pointer definition for Callback function
  */

  ftm_calibration_tx_result_type tx_result;
  /*!< Structure to hold Tx Calibration results */

} ftm_cal_meas_hdet_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold data pertaining to Tx Override OpCode */
typedef struct
{
  ftm_cal_tx_override_fp_type oc_process_fp[FTM_CALIBRATION_NUM_OF_TECHS];
  /*!< 
  @brief
  Function Pointer definition for Callback function
  */

  ftm_cal_tx_override_parameter_type up_param;
  /*!< Structure holding all the unpacked payload for Tx Override opcode */

} ftm_cal_tx_override_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold data pertaining to Tx Measure OpCode */
typedef struct
{
  ftm_cal_tx_measure_fp_type oc_process_fp[FTM_CALIBRATION_NUM_OF_TECHS];
  /*!< 
  @brief
  Function Pointer definition for Callback function
  */

  uint8 *res_buff;
  /*!< 
  @brief 
  Temporary Tx measurement result buffer used to store calibration results for 
  the current segment. 

  @details 
  This buffer will be reused in every segment. The purpose 
  of this buffer to pre-allocate the enough memory to store the maximum 
  possible Tx meas results for a given segment. When measure 
  Tx is called, pointer to this buffer is sent; so that each tech 
  specific measure Tx function can fill the calibration results. At the 
  end of the segment, the data in this buffer will be pushed to results 
  buffer. 

  This buffer is allocated during Calibration configuration, and deallocated 
  at the end of the sweep. 
  */

  ftm_cal_tx_meas_parameter_type up_param;
  /*!< Structure holding all the unpacked payload for Rx Measure opcode */

} ftm_cal_tx_measure_data_type;


/*----------------------------------------------------------------------------*/
typedef struct
{
  uint16 alpha;
  uint16 beta;
}ftm_rsb_coeff;
/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_config_tx_init
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_config_tx_deinit
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
uint16
ftm_calibration_config_tx_unpack
(
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_config_tx_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_meas_hdet_init
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_meas_hdet_deinit
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
uint16
ftm_calibration_meas_hdet_unpack
(
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_meas_hdet_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_tx_override_init
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_tx_override_deinit
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
uint16
ftm_calibration_tx_override_unpack
( 
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_tx_override_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_tx_measure_init
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_tx_measure_deinit
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
uint16
ftm_calibration_tx_measure_unpack
(
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_tx_measure_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
boolean 
ftm_calibration_txiqmc_override
(
 ftm_rsb_coeff *rsb_coeff,
 uint8 counter,
 uint16 *alpha, 
 uint16 *beta
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_process_txrsb
(
 ftm_rsb_coeff *rsb_coeff,
 int32 *rrsb_res,
 uint32 *txrsb_final
);
#ifdef __cplusplus
}
#endif

#endif /* FTM_CALIBRATION_V3_TX_H */


