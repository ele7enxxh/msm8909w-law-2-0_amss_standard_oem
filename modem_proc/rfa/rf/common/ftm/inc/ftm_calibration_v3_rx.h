#ifndef FTM_CALIBRATION_V3_RX_H
#define FTM_CALIBRATION_V3_RX_H
/*!
  @file
  ftm_calibration_v3_rx.h

  @brief
  This module contains prototypes and definitions used by common RF calibration
  code.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_calibration_v3_rx.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
06/03/14   bsh     Support for itnra band CA cal
09/13/13   jmf     [Calv4] Support for Raw Rx AGC read in RX_MEASURE OpCode for FBRxCal
08/14/13   aro     Support for DC calibration in RX_MEASURE OpCode
04/15/13   aro     Mainline calv3 optimization code/delete obsolete code
04/01/13   aro     Fixed random buffer size initialization
04/01/13   aro     Debug flag to switch between old and new calv3 implementation
04/01/13   aro     Added hookup for new calv3 infrastructure
04/01/13   aro     F3 update
03/29/13   aro     Radio Config/Retune optimized opcode proc functions
03/29/13   aro     Tx Config/Meas optimized opcode proc functions
03/29/13   aro     Rx Config/Meas optimized opcode proc functions
11/30/12   aro     Renamed variable
11/21/12   aro     Support for RX_MEASURE result 
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
  Enumeration of all Rx Calibration Actions.
*/
typedef enum
{

  RX_ACTION_LNA0_CAL    = 0x1,
  /*!< LNA0 Calibration */

  RX_ACTION_LNA1_CAL    = 0x2,
  /*!< LNA1 Calibration */

  RX_ACTION_LNA2_CAL    = 0x4,
  /*!< LNA2 Calibration */

  RX_ACTION_LNA3_CAL    = 0x8,
  /*!< LNA3 Calibration */

  RX_ACTION_LNA4_CAL    = 0x10,
  /*!< LNA4 Calibration */

  RX_ACTION_LNA5_CAL    = 0x20,
  /*!< LNA5 Calibration */

  RX_ACTION_WBDC_CAL    = 0x100,
  /*!< Wide band DC Calibration */

} ftm_calibration_rx_action_enum_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Maximum allowed Rx results size in bytes for the current segment.
 
  @details
  This enum defines the Maximum allowed results size for the current segment.
  The mathematics behind this maximum is based on following.
 
  For Rx Measurement, one Rx measuremtnis 2 bytes. In a segment, the assumption
  is that the LNA0 to LNA5 calibrated for three simultaneous Rx path.
  Thus maxiumum Rx results size can be  (2 x 6 x 3) = 36 bytes.
*/
#define FTM_CAL_SEG_RX_BUFF_SIZE_MAX 36

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Type/Size of Rx Calibration Result
*/
typedef int16 ftm_calibration_rx_meas_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Type of Rx Action Mask for FTM Calibration Command
*/
typedef uint16 ftm_calibration_rx_action_mask_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for Rx Measurement configuration Instruction.

  @details
  Contains the Rx Merasurement configuration instruction (Payload)
*/
typedef PACK(struct)
{
  ftm_calibration_rx_action_mask_type cal_action;
  /*!<        
  @brief
  The receive calibration actions mask. 
   
  @details 
  This is a mask indicating the Rx action to be performed with this opcode 
  on a given segment. Following are the definitions of the bits defining 
  Rx action 
  -# B[0] : LNA 0 Calibration 
  -# B[1] : LNA 1 Calibration 
  -# B[2] : LNA 2 Calibration 
  -# B[3] : LNA 3 Calibration 
  -# B[4] : LNA 4 Calibration 
  -# B[5] : LNA 5 Calibration 
  -# B[7:6] : Reserved 
  -# B[8] : Wideband DC calibration
  -# B[15:9] : Reserved 
   
  These definitions are also documented in #ftm_calibration_rx_action_enum_type. 
  */

  uint8 power_mode;
  /*!< Intelliceiver Power Mode for calibration. 0 indicates Low Power Mode
  and 1 indicates High Power Mode */

  ftm_calibration_device_flag_mask_type device_flag;
  /*!< 
  @brief
  Rx Flag Mask indicating which device to calibrate. 
   
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
  
} ftm_cal_instr_payload_config_rx_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for Rx Measurement Instruction.

  @details
  Contains the Rx Merasurement instruction (Payload)
*/
typedef PACK(struct)
{
  ftm_calibration_rx_action_mask_type cal_action;
  /*!<        
  @brief
  The receive calibration actions mask. 
   
  @details 
  This is a mask indicating the Rx action to be performed with this opcode 
  on a given segment. Following are the definitions of the bits defining 
  Rx action 
  -# B[0] : LNA 0 Calibration 
  -# B[1] : LNA 1 Calibration 
  -# B[2] : LNA 2 Calibration 
  -# B[3] : LNA 3 Calibration 
  -# B[4] : LNA 4 Calibration 
  -# B[5] : LNA 5 Calibration 
  -# B[8:6] : Reserved 
  -# B[9] : Wideband DC calibration
  -# B[15:10] : Reserved 
   
  These definitions are also documented in #ftm_calibration_rx_action_enum_type. 
  */

  uint8 power_mode;
  /*!< Intelliceiver Power Mode for calibration. 0 indicates Low Power Mode
  and 1 indicates High Power Mode */

  int16 exp_rxagc;
  /*!< Expected RxAGC in 1/10 dB units */

  ftm_calibration_device_flag_mask_type device_flag;
  /*!< 
  @brief
  Rx Flag Mask indicating which device to calibrate. 
   
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
  
} ftm_cal_instr_payload_meas_rx_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold Rx calibration Data
*/
typedef struct
{
  uint8 *rx_buffer;
  /*!< 
  @brief
  Pointer to the Rx Result buffer for current segment to be filled by tech 
  specific Rx measurement function.
   
  @details
  Container to store the calibration results. Each calibration result will
  of size #ftm_calibration_rx_meas_data_type. All these results will be packed
  together. The order of calibration results will be as following.
  - INT16 : RFM Device 0 PM 0 Rx Cal Action 0 Measurement ( LNA0 )
  - INT16 : RFM Device 0 PM 0 Rx Cal Action 1 Measurement ( LNA1 )
  - INT16 : RFM Device 0 PM 0 Rx Cal Action 2 Measurement ( LNA1 )
  - INT16 : RFM Device 0 PM 0 Rx Cal Action 3 Measurement ( LNA2 )
  - INT16 : RFM Device 0 PM 0 Rx Cal Action 4 Measurement ( LNA3 )
  - INT16 : RFM Device 0 PM 0 Rx Cal Action 5 Measurement ( LNA4 )
  - INT16 : RFM Device 0 PM 0 Rx Cal Action 6 Measurement ( LNA5 )
  - INT16 : RFM Device 0 PM 0 Rx Cal Action 7 Measurement 
  - <i>Other Rx Cal Action measurement for RFM Device 0 </i>  
  - INT16 : RFM Device 0 Rx PM 0 Cal Action 15 Measurement
  - INT16 : RFM Device 0 PM 1 Rx Cal Action 0 Measurement ( LNA0 )
  - INT16 : RFM Device 0 PM 1 Rx Cal Action 1 Measurement ( LNA1 )
  - INT16 : RFM Device 0 PM 1 Rx Cal Action 2 Measurement ( LNA1 )
  - INT16 : RFM Device 0 PM 1 Rx Cal Action 3 Measurement ( LNA2 )
  - INT16 : RFM Device 0 PM 1 Rx Cal Action 4 Measurement ( LNA3 )
  - INT16 : RFM Device 0 PM 1 Rx Cal Action 5 Measurement ( LNA4 )
  - INT16 : RFM Device 0 PM 1 Rx Cal Action 6 Measurement ( LNA5 )
  - INT16 : RFM Device 0 PM 1 Rx Cal Action 7 Measurement 
  - <i>Other Rx Cal Action measurement for RFM Device 0 </i>  
  - INT16 : RFM Device 0 Rx PM 1 Cal Action 15 Measurement
  - INT16 : RFM Device 1 PM 0 Rx Cal Action 0 Measurement ( LNA0 )
  - INT16 : RFM Device 1 PM 0 Rx Cal Action 1 Measurement ( LNA1 )
  - INT16 : RFM Device 1 PM 0 Rx Cal Action 2 Measurement ( LNA1 )
  - INT16 : RFM Device 1 PM 0 Rx Cal Action 3 Measurement ( LNA2 )
  - INT16 : RFM Device 1 PM 0 Rx Cal Action 4 Measurement ( LNA3 )
  - INT16 : RFM Device 1 PM 0 Rx Cal Action 5 Measurement ( LNA4 )
  - INT16 : RFM Device 1 PM 0 Rx Cal Action 6 Measurement ( LNA5 )
  - INT16 : RFM Device 1 PM 0 Rx Cal Action 6 Measurement ( LNA1 )
  - <i>Other Rx Cal Action measurement for RFM Device 1 </i>
  - INT16 : RFM Device 1 PM 0 Rx Cal Action 15 Measurement 
  - <i>Other Rx Cal Action measurement for Remaining devices </i>
  - <i>Other Rx Cal Action measurement for Remaining devices </i>
  - INT16 : RFM Device7 PM 1 Rx Cal Action 15 Measurement 
   
  Based on Rx Cal Action Mask and Rx Flag mask, all above listed rx measurement 
  may not be present. In that case, <b>all data should still be packed 
  together. </b>
  */ 

  uint8 num_of_rx_reads;
  /*!< Pointer to the container where tech specific rx measurement query 
  function fills the number of Rx reads taken during this rx measurement. */

  uint16 max_rx_buffer_len;
  /*!< Maximum result buffer in bytes that can be used to store Rx results for 
  this segment. Tech specific Rx measurement query function should check 
  against this number to avoid buffer overflow. */ 

} ftm_calibration_rx_result_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Function Pointer definition for Callback function to configure Rx
 
  @details
  This function will configure Rx Path for Rx measurement. This function will
  set the Linearity Mode (for CDMA only), override the LNA state, and
  override the LNA Gain. No Rx measurement will be done in this function as
  measurement will be done later after RxAGC has settled. The RxAGC settling
  time will be used for Tx Measurement so reduce calibration time
*/
typedef boolean (*ftm_calibration_cb_config_rx_fp_type)
(
  ftm_cal_instr_payload_config_rx_type const *config_rx
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Function Pointer definition for Callback function to mesaure Rx
 
  @details
  This function will perform Rx measurement. It will sample RxAGC and
  return an average value as Rx calibration result.
 
  @param rx_meas
  Pointer to the Rx measurement Opcode to be used during Rx measurement.
 
  @param rx_result
  Container to store the calibration results. Each calibration result will
  of size #ftm_calibration_rx_meas_data_type. All these results will be packed
  together. The order of calibration results will be as following.
  - RFM Device 0 PM 0 Rx Cal Action 0 Measurement ( LNA0 )
  - RFM Device 0 PM 0 Rx Cal Action 1 Measurement ( LNA1 )
  - RFM Device 0 PM 0 Rx Cal Action 2 Measurement ( LNA1 )
  - RFM Device 0 PM 0 Rx Cal Action 3 Measurement ( LNA2 )
  - RFM Device 0 PM 0 Rx Cal Action 4 Measurement ( LNA3 )
  - RFM Device 0 PM 0 Rx Cal Action 5 Measurement ( LNA4 )
  - RFM Device 0 PM 0 Rx Cal Action 6 Measurement ( LNA5 )
  - RFM Device 0 PM 0 Rx Cal Action 7 Measurement 
  - <i>Other Rx Cal Action measurement for RFM Device 0 </i>  
  - RFM Device 0 Rx PM 0 Cal Action 15 Measurement
  - RFM Device 0 PM 1 Rx Cal Action 0 Measurement ( LNA0 )
  - RFM Device 0 PM 1 Rx Cal Action 1 Measurement ( LNA1 )
  - RFM Device 0 PM 1 Rx Cal Action 2 Measurement ( LNA1 )
  - RFM Device 0 PM 1 Rx Cal Action 3 Measurement ( LNA2 )
  - RFM Device 0 PM 1 Rx Cal Action 4 Measurement ( LNA3 )
  - RFM Device 0 PM 1 Rx Cal Action 5 Measurement ( LNA4 )
  - RFM Device 0 PM 1 Rx Cal Action 6 Measurement ( LNA5 )
  - RFM Device 0 PM 1 Rx Cal Action 7 Measurement 
  - <i>Other Rx Cal Action measurement for RFM Device 0 </i>  
  - RFM Device 0 Rx PM 1 Cal Action 15 Measurement
  - RFM Device 1 PM 0 Rx Cal Action 0 Measurement ( LNA0 )
  - RFM Device 1 PM 0 Rx Cal Action 1 Measurement ( LNA1 )
  - RFM Device 1 PM 0 Rx Cal Action 2 Measurement ( LNA1 )
  - RFM Device 1 PM 0 Rx Cal Action 3 Measurement ( LNA2 )
  - RFM Device 1 PM 0 Rx Cal Action 4 Measurement ( LNA3 )
  - RFM Device 1 PM 0 Rx Cal Action 5 Measurement ( LNA4 )
  - RFM Device 1 PM 0 Rx Cal Action 6 Measurement ( LNA5 )
  - RFM Device 1 PM 0 Rx Cal Action 6 Measurement ( LNA1 )
  - <i>Other Rx Cal Action measurement for RFM Device 1 </i>
  - RFM Device 1 PM 0 Rx Cal Action 15 Measurement 
  - <i>Other Rx Cal Action measurement for Remaining devices </i>
  - <i>Other Rx Cal Action measurement for Remaining devices </i>
  - RFM Device7 PM 1 Rx Cal Action 15 Measurement 
 
  Based on Rx Cal Action Mask and Rx Flag mask, all above listed rx measurement 
  may not be present. In that case, <b>all data should still be packed 
  together. </b>
 
  Tech specific measurement function will also need to populate the total number
  of measurements taken.
 
  As a part of this parameter, maximum size of result buffer for this segment
  is also sent. Therefore the tech specific measure function should do a
  check in order to prevent buffer overflow.
*/
typedef boolean (*ftm_calibration_cb_measure_rx_fp_type)
(
  ftm_cal_instr_payload_meas_rx_type const *meas_rx,
  ftm_calibration_rx_result_type *rx_result 
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Maximum allowed Rx meas results size in bytes for the current segment.
*/
#define FTM_CAL_RX_MEAS_BUFF_SIZE_MAX 27

/*----------------------------------------------------------------------------*/
/*! Enumeration to indicate the maximum number of devices */
#define FTM_CAL_V3_RX_MAX_DEV 6

/*----------------------------------------------------------------------------*/
/*! Type defintion for Device mask */
typedef uint8 ftm_cal_rx_meas_device_mask_type;

/*----------------------------------------------------------------------------*/
/*! Type defintion for Rx Measurement action mask */
typedef uint16 ftm_cal_rx_meas_action_params_mask_type;

/*----------------------------------------------------------------------------*/
/*! Type defintion for LNA State */
typedef uint8 ftm_cal_rx_meas_lna_state_type;

/*----------------------------------------------------------------------------*/
/*! Type defintion for Power Mode */
typedef uint8 ftm_cal_rx_meas_power_mode_type;

/*----------------------------------------------------------------------------*/
/*! Type defintion for LNA Offset */
typedef int16 ftm_cal_rx_meas_lna_offset_type;

/*----------------------------------------------------------------------------*/
/*! Type defintion for Expected Rx AGC */
typedef int16 ftm_cal_rx_meas_exp_agc_type;

/*----------------------------------------------------------------------------*/
/*! Enumeration listing the Rx Measurement types */
typedef enum
{
  RX_MEAS_LNA_OFFSET = 0,
  /*!< LNA offset measurement */

  RX_MEAS_DC_LNA_OFFSET = 1,
  /*!< LNA offset measurement for DC */

  RX_MEAS_RAW_RX_AGC = 2,
  /*!< Read Raw Rx AGC for FB Rx Cal */

} ftm_cal_rx_meas_action_enum_type;

/*----------------------------------------------------------------------------*/
/*! Data Structure to hold the Rx measurement configuration data for each 
device for which the bit is set */
typedef PACK(struct)
{
  ftm_cal_rx_meas_action_params_mask_type rx_meas_action;
  /*!< 64-bit mask indicating the Rx measurement action */

  uint8 payload_addr;
  /*!< Variable to get the address of the payload associated with the set bits 
  of rx_meas_action. */

} ftm_cal_rx_meas_param_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for Rx Measurement Instruction.
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

  ftm_cal_rx_meas_device_mask_type device_mask;
  /*!< 8-bit mask indicating which for which devices the Rx Meas message 
  is sent. The number of sets of Rx Meas payload will equal the number of 
  set bits in the mask */

  ftm_cal_rx_meas_param_data_type rx_meas_data;
  /*!< Data Structure to hold the rx meas configuration data for each 
  device for which the bit is set */

} ftm_cal_rx_meas_instr_payload_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure used to hold the unpacked Rx meas parameter to be sent
  to tech specific functions
*/
typedef struct
{
  boolean debug_mode_en;
  /*!< Flag indicating, if the debug mode is enabled */

  uint16 flag;
  /*!< 16-bit flag indicating a specific type of action to be perform the 
  given payload. */

  ftm_cal_rx_meas_device_mask_type device_mask;
  /*!< 8-bit mask indicating which for which devices the Rx Meas message 
  is sent. The number of sets of Rx Meas payload will equal the number of 
  set bits in the mask */

  struct
  {
    ftm_cal_rx_meas_action_params_mask_type rx_meas_action;
    /*!< 64-bit mask indicating the Rx measurement action */

    struct
    {
      ftm_cal_rx_meas_lna_state_type lna_state;
      /*!< LNA Gain state for which the Rx measurement is to be done */

      ftm_cal_rx_meas_power_mode_type power_mode;
      /*!< Analog transceiver power mode for which the Rx measurment is to be 
      performed. */

      ftm_cal_rx_meas_exp_agc_type expected_exagc;
      /*!< Expected RxAGC in 1/10th dB units. Difference between this value and 
      actual measured RxAGC will be LNA Offset for the given LNA state and 
      Power Mode */

    } lna_offset;
    /*!< LNA offset measurement */

    struct
    {
      ftm_cal_rx_meas_lna_state_type lna_state;
      /*!< LNA Gain state for which the Rx measurement is to be done */

      ftm_cal_rx_meas_power_mode_type power_mode;
      /*!< Analog transceiver power mode for which the Rx measurment is to be 
      performed. */

      ftm_cal_rx_meas_exp_agc_type expected_exagc;
      /*!< Expected RxAGC in 1/10th dB units. Difference between this value and 
      actual measured RxAGC will be LNA Offset for the given LNA state and 
      Power Mode */

    } dc_lna_offset;
    /*!< LNA offset measurement */

    struct
    {
      ftm_cal_rx_meas_lna_state_type lna_state;
      /*!< LNA Gain state for which the Rx measurement is to be done */

      ftm_cal_rx_meas_power_mode_type power_mode;
      /*!< Analog transceiver power mode for which the Rx measurment is to be 
      performed. */

    } raw_rx_agc;
    /*!< LNA offset measurement */

  } rx_meas_param[FTM_CAL_V3_RX_MAX_DEV];
  /*!< Rx Measurement parameter per enabled device */

} ftm_cal_rx_meas_parameter_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to be used by tech functions to push Rx results to calv3
  common framework
*/
typedef struct
{
  ftm_cal_rx_meas_device_mask_type device_mask;
  /*!< 8-bit mask indicating which for which devices the Rx Meas message 
  is sent. The number of sets of Rx Meas payload will equal the number of 
  set bits in the mask */

  struct
  {
    ftm_cal_rx_meas_action_params_mask_type rx_meas_action;
    /*!< 64-bit mask indicating the Rx measurement action */

    struct
    {
      ftm_cal_rx_meas_lna_state_type lna_state;
      /*!< LNA Gain state for which the Rx measurement is to be done */

      ftm_cal_rx_meas_power_mode_type power_mode;
      /*!< Analog transceiver power mode for which the Rx measurment is to be 
      performed. */

      ftm_cal_rx_meas_lna_offset_type lna_offset;
      /*!< LNA Offset for computed by taking the difference between expected 
      RxAGC and actual measured RxAG ta the given LNA state and Power Mode */

    } lna_offset;
    /*!< LNA offset meas result */

    struct
    {
      ftm_cal_rx_meas_lna_state_type lna_state;
      /*!< LNA Gain state for which the Rx measurement is to be done */

      ftm_cal_rx_meas_power_mode_type power_mode;
      /*!< Analog transceiver power mode for which the Rx measurment is to be 
      performed. */

      ftm_cal_rx_meas_lna_offset_type lna_offset;
      /*!< LNA Offset for computed by taking the difference between expected 
      RxAGC and actual measured RxAG ta the given LNA state and Power Mode */

    } dc_lna_offset;
    /*!< LNA offset meas result for DC mode */

    struct
    {
      ftm_cal_rx_meas_lna_state_type lna_state;
      /*!< LNA Gain state for which the Rx measurement is to be done */

      ftm_cal_rx_meas_power_mode_type power_mode;
      /*!< Analog transceiver power mode for which the Rx measurment is to be 
      performed. */

      ftm_cal_rx_meas_lna_offset_type raw_rx_agc;
      /*!< Actual measured RxAGC for the given LNA state and Power Mode */

    } raw_rx_agc;
    /*!< Raw Rx AGC meas result */
  } rx_meas_param[FTM_CAL_V3_RX_MAX_DEV];
  /*!< Rx Measurement parameter per enabled device */

} ftm_cal_rx_meas_result_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Function Pointer definition to perform Rx Measurement
 
  @details
  This function will be called by v3 common framework to perform given Rx
  measurement action.
 
  @param rx_measure
  Pointer to the data structure containing the unpacked paylaod for Rx
  measurement.
 
  @param cal_result
  Pointer to the structure where the calibration result is stored
 
  @return
  Status indicating the status of execution of Rx measurement function
*/
typedef boolean (*ftm_cal_rx_measure_fp_type)
(
  ftm_cal_rx_meas_parameter_type const *rx_measure,
  ftm_cal_rx_meas_result_data_type *cal_result
);

/*----------------------------------------------------------------------------*/
/*! Structure to hold data pertaining to Radio Retune OpCode */
typedef struct
{
  ftm_calibration_cb_config_rx_fp_type 
                           oc_process_fp[FTM_CALIBRATION_NUM_OF_TECHS];
  /*!< 
  @brief
  Function Pointer definition for Callback function to configure waveform
  */

  uint16 rxagc_settling_time_us[FTM_CALIBRATION_NUM_OF_TECHS];
  /*!< RxAGC Settling time */

  rf_time_tick_type rx_cfg_time;
  /*!< Varaible to store the time when Rx Config Opcode process is 
  completed. This reference time is used compute the lapse between Rx 
  config and rx Meas OpCode. This lapse is substracted from the RxAGC 
  settling time to remove extra settling time. */ 

  boolean rx_prof_in_progress;
  /*!< Flag indicating that the Rx profiling is in progress */

} ftm_cal_config_rx_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold data pertaining to Radio Retune OpCode */
typedef struct
{
  ftm_calibration_cb_measure_rx_fp_type 
                           oc_process_fp[FTM_CALIBRATION_NUM_OF_TECHS];
  /*!< 
  @brief
  Function Pointer definition for Callback function to configure waveform
  */

  ftm_calibration_rx_result_type rx_result;
  /*!< Structure to hold Tx Calibration results */

} ftm_cal_meas_rx_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold data pertaining to Rx Measure OpCode */
typedef struct
{
  ftm_cal_rx_measure_fp_type oc_process_fp[FTM_CALIBRATION_NUM_OF_TECHS];
  /*!< 
  @brief
  Function Pointer definition for Callback function
  */

  uint8 *res_buff;
  /*!< 
  @brief 
  Temporary Rx measurement result buffer used to store calibration results for 
  the current segment. 

  @details 
  This buffer will be reused in every segment. The purpose 
  of this buffer to pre-allocate the enough memory to store the maximum 
  possible Rx meas results for a given segment. When measure 
  Rx is called, pointer to this buffer is sent; so that each tech 
  specific measure rx function can fill the calibration results. At the 
  end of the segment, the data in this buffer will be pushed to results 
  buffer. 

  This buffer is allocated during Calibration configuration, and deallocated 
  at the end of the sweep. 
  */

  ftm_cal_rx_meas_parameter_type up_param;
  /*!< Structure holding all the unpacked payload for Rx Measure opcode */

} ftm_cal_rx_measure_data_type;

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_config_rx_init
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_config_rx_deinit
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
uint16
ftm_calibration_config_rx_unpack
(
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_config_rx_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_meas_rx_init
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_meas_rx_deinit
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
uint16
ftm_calibration_meas_rx_unpack
(
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_meas_rx_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_rx_measure_init
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_rx_measure_deinit
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
uint16
ftm_calibration_rx_measure_unpack
(
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_rx_measure_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

#ifdef __cplusplus
}
#endif

#endif /* FTM_CALIBRATION_V3_RX_H */

