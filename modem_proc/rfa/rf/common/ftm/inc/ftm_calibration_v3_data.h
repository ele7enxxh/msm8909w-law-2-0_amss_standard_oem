
#ifndef FTM_CALIBRATION_V3_DATA_H
#define FTM_CALIBRATION_V3_DATA_H
/*!
  @file
  ftm_calibration_v3_data.h

  @brief
  This module contains prototypes and definitions used by common RF calibration
  code.
*/

/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_calibration_v3_data.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
02/25/15   myu     Add sub-opcode handling for extended PIN CAL process
11/10/14   id      Bug fix for enum of Rx PAR as part of XPT capture Results opcode
11/03/14   id      Add support for populating Rx PAR as part of XPT capture Results opcode
08/14/14   aj      Increased Maximum buffer size for FTM_CAL_DATA_RES response packet
07/10/14   cd/cri  Merge Pin Cal
2/19/14    rsr     Added Mline bias option to pick Mline entry based on proximity
                   to target power.
01/08/14   cac     Add new op code to support RGI delta table for EPT ET in different PA states
12/27/13   rsr     Add support for XPT_CAPTURE RESULT Opcode.
09/16/13   meh     changing MAX_NUM_RGI_PWR_PAIRS limit from 25 to 40
09/10/13   shk     Support to pack M Line results in the response packet
06/18/13   jmf     [xPT] ET delay implementation for Cal V4
   06/05/13   rsr     Mline opcode parameter packet change :- 
                     *  Separate EPT/ Et target powers 
                     *  Bias limiting option - Cap Bias at Vdmax or interpolate
05/04/13    id     Add support for VdMax for EPT and ET
05/02/13    id     Add Support to populate EPDT Results to result buffer
04/30/13    id     Add Load ISC and Load SC for Calc DPD Load and some clean up
04/23/13    id     Add Reference Channel in data payload
04/19/13    id     1. Support for Swp2 and 3
                   2. Support for Data Opcode Actions for DPD Proc and Load
                      Calc DPD
04/15/13   aro     Mainline calv3 optimization code/delete obsolete code
04/10/13   id     Do Mline Calculation on FTM Task
04/09/13   id     Support for RGIvsPwr Tbl and Mline Calc Data Actions
04/01/13   aro    Unpack signature fix
01/01/13   id     Initial version

==============================================================================*/

#include "comdef.h"
#include "ftm_common_xpt.h"
#include "rfcommon_mdsp_types.h"
#include "fw_dpd_intf.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Maximum allowed Data opcode results size in bytes for the current segment.
*/
#define FTM_CAL_DATA_RES_BUFF_SIZE_MAX 4000

#define FTM_CAL_V3_INVALID_LTE_BW 255

/*----------------------------------------------------------------------------*/
/*! Enumeration to indicate the maximum number of devices */
#define FTM_CAL_V3_DATA_MAX_DEV 3

/*----------------------------------------------------------------------------*/
/*! Enumeration to indicate Maximum number of RGI Pwr pairs in the RGI 
    Pwr Table */
#define MAX_NUM_RGI_PWR_PAIRS 40

/*----------------------------------------------------------------------------*/
/*! Enumeration to indicate Maximum number of AMAM/AMPM values in the DPD 
    Table */
#define MAX_NUM_DPD_VALS 64

/*----------------------------------------------------------------------------*/
/*! Enumeration to indicate Maximum number of Detrough values in the DPD 
    Table */
#define MAX_NUM_DET_VALS 128

/*----------------------------------------------------------------------------*/
/*! Enumeration to indicate Maximum number of DPD result size */
#define MAX_RES_DPD_SIZE 16

/*----------------------------------------------------------------------------*/
/*! Enumeration to indicate invalid values for RGI and Power */
#define RGID_INV_RGI 0xFFFF
#define RGID_INV_PWR 0xFFFFFFFF

/*----------------------------------------------------------------------------*/
/*! Enumeration to indicate invalid values for EPT and ET compression Point */
#define COMP_POINT_INV 0xFFFF

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration of all Data Calibration Actions.
*/
typedef enum
{

  DATA_OPCODE_MASK_PUSH_RGI_PWR         = 1,
  /*!< Push RGI vs PWR (RGI Delta) Data */

  DATA_OPCODE_MASK_PUSH_DPD_TBL         = 2,
  /*!< Push DPD Table Data */

  DATA_OPCODE_MASK_PROCESS_MLINE        = 4,
  /*!< Process Data - M Line Calculation */

  DATA_OPCODE_MASK_PROCESS_DELAY_VAL    = 8,
  /*!< Process Data - Process Delay Value */

  DATA_OPCODE_MASK_PROCESS_DPD          = 16,
  /*! < Process Data - Perform DPD Calculation */
  
  DATA_OPCODE_MASK_LOAD_CALC_DPD        = 32,
  /*! < Process Data - Load Calculated DPDs */
  
  DATA_OPCODE_MASK_RETURN_MLINE_RESULTS = 64,
  /*!< Return Results - Return M Line Results */

  DATA_OPCODE_MASK_RETURN_XPT_CAPTURE_RESULTS = 128,
  /*!< Return Results - Return M Line Results */

  DATA_OPCODE_MASK_PROCESS_PIN_VAL    = 256,
  /*!< Process Data - Process Pin Value */

  DATA_OPCODE_MASK_PUSH_RGI_PWR_COM   = 512,
  /*!< Push RGI vs PWR (RGI Delta) based on the PA state and XPT mode. When EPT and ET are in different PA states*/

  DATA_OPCODE_MASK_PROCESS_PIN_VAL_EXT = 1024
  /*!< Process Data - Process Pin Value and some extra result such as AMPM scaling factor */

} ftm_calibration_data_opcode_enum_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enumeration of all Data Calibration Result Actions.
*/
typedef enum
{

  DATA_RES_ACTION_RGI_PWR_TBL           = 0,
  /*!< Result type for RGI PWR TBL Action */

  DATA_RES_ACTION_PUSH_DPD_TBL          = 1,
  /*!< Result type for Load DPD Action */

  DATA_RES_ACTION_PROCESS_MLINE         = 2,
  /*!< Result type for Mline Calc Action */

  DATA_RES_ACTION_PROCESS_DELAY         = 3,
  /*!< Result type for Process Delay Action */

  DATA_RES_ACTION_PROCESS_DPD           = 4,
  /*!< Result type for Process DPD Action */

  DATA_RES_ACTION_LOAD_CALC_DPD         = 5,
  /*!< Result type for Process DPD Action */

  DATA_RES_ACTION_RETURN_MLINE_RESULTS  = 6,
  /*!< Result type to return M Line results*/

  DATA_RES_ACTION_RETURN_XPT_CAPTURE_RESULTS  = 7,
  /*!< Result type to return XPT capture results*/

  DATA_RES_ACTION_PROCESS_PIN           = 8,
  /*!< Result type for Process Pin Action */

  DATA_RES_ACTION_PROCESS_PIN_EXT       = 10,
  /*!< Result type for Process Pin Action extended */

  DATA_RES_ACTION_MAX
  /*!< Result type Max */

} ftm_calibration_data_res_action_enum_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration of all Data Calibration Actions.
*/
typedef enum
{
  LOAD_CALC_DPD_ACTION_LOAD_UNITY         = 0,
  /*!< Load Calc DPD Action - Load Unity */

  LOAD_CALC_DPD_ACTION_LOAD_MEAS_ISC      = 1,
  /*!< Load Calc DPD Action - Load Meas DPD with Init Save and Commit */

  LOAD_CALC_DPD_ACTION_LOAD_MEAS_SC      = 2
  /*!< Load Calc DPD Action - Load Meas DPD with Save and Commit 
    (Without Init) */

} ftm_calibration_data_load_calc_action_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enumeration of XPT Calibration MLine Result Actions.
*/
typedef enum
{
   RESULT_NO_ACTION             = 0,
   /*!< XPT Result Action - No Action */

   RESULT_MLINE_TABLE           = 1,
   /*!< XPT Result Action - Return EXT M Line Table */

   RESULT_SELECTION_SWEEP       = 2,
   /*!< XPT Result Action - Return Selection Sweep table*/

   RESULT_COMPRESSION_POINTS    = 4,
   /*!< XPT Result Action - Return Compression Points*/

   RESULT_MLINE_SLOPE_POINTS    = 8,
   /*!< XPT Result Action - Return M Line Slope Points*/

   RESULT_AMAM_RMS    = 16
   /*!< XPT Result Action - Return AMAM RMS values*/

} ftm_calibration_mline_results_action_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enumeration of XPT IQ Capture Result Actions.
*/
typedef enum
{
   RESULT_NOP            = 0,
   /*!< XPT Result Action - No Action */

   RESULT_COMPRESSION_PT        = 1,
   /*!< XPT Result Action - Return Compression Points*/

   RESULT_DPD_TABLES            = 2,
   /*!< XPT Result Action - Return Selection Sweep table*/

   RESULT_RX_PAR                = 4,
   /*!< XPT Result Action - Return PAR*/

} ftm_calibration_xpt_capture_results_action_type;
/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Error mask enumeration indicating errors in each segment
*/
typedef enum
{

  DATA_ERR_OPERATION_FAILURE    = 1,
  /*!< Error code indicating the Data operation has failed. */

  DATA_ERR_NULL_CB              = 2,
  /*!< Error code to indicate that the tech specific Data callback 
  function is NULL. This error code will be seen when the Data 
  opcode bit is enabled while the CB function is NULL. */ 

  DATA_ERR_MAX_NUM
  /*!< Maximum number of segment error code. This enum should not be 
  used for any purpose other than counting the number of error codes. */

} ftm_calibration_data_error_code_mask;


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enumeration of all options for Mline Bias limiting.
*/
typedef enum
{
  DATA_MLINE_CALC_INTERP_BIAS = 0,
  /*!< When Bias to be picked out of Mline is greater than Vdmax, 
    interpolate bias between previous entry and the failing entry for required target power
    If this is less than Vdmax, then cap the Bias to Vdmax. If this is greater than Vdmax, then fail.*/
  DATA_MLINE_CALC_CAP_BIAS_TO_VdMAX = 1,
  /*!< When Bias to be picked out of Mline is greater than Vdmax, 
       cap it to Vdmax */
  DATA_MLINE_CALC_CLOSEST_POWER_INTERP_BIAS = 2,
  /*!< Pick power closest to target power from Mline, except for the max target power. 
      Bias action follows INTERP_BIAS behaviour. */
  DATA_MLINE_CLOSEST_PWR_RET_MAX_ENTRY = 3,
  /*!< Pick power closest to target power from Mline, except for the max target power. 
      Returns max possible entry from Mline for bias limit for all target powers that exceed bias limit and
      returns max entry for max tgt power, irrespective of max target power*/

}ftm_calibration_mline_bias_options_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enumeration of all options for extended PIN process.
*/
typedef enum
{
  PIN_PROCESS_OPTION_AMAM = 1,
  /*!< AMAM PIN Cal */
  PIN_PROCESS_OPTION_AMPM = 2
  /*!< AMPM Scaling CAL */

}ftm_calibration_pin_process_option;

/*----------------------------------------------------------------------------*/
/*! Type defintion for Device mask */
typedef uint8 ftm_cal_data_device_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Definition of Data payload version
*/
typedef uint8 ftm_calibration_data_payload_version_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM Common Dispatch Request Packet to be used for Mline Calculation for
  calibration v3 sweep
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_RFM_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CDMA_RFM_1X_WAKEUP_RX */

  ftm_rf_technology_type ftm_rf_tech;
  /*!< FTM RF Tech for Mline Calculation */

  uint32 channel;
  /*!< Channel for MLine Calculation */

  uint32 data_param;
  /*!< To hold address of Data parameter. PACKED structure is not 
  used here as the this command is sent only internally */

} ftm_cal_data_proc_mline_req_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM Data Mline Proc sweep Response packet
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16 mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_COMMON_C */

  uint16 cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CALIBRATION_SWEEP */

  uint16 req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16 rsp_len;
  /*!< Length of response packet to Diag/PC */

  int8 mline_proc_status;
  /*!< Mline Proc Status */

} ftm_cal_sweep_data_mline_proc_rsp_pkt_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for Data Push RGI vs Pwr Payload

  @details
  Contains the Payload for Data Push RGI vs Pwr Table
*/
typedef PACK(struct)
{
  uint8 rgi;
  /*!< RGI of the RGI Pwr Table */

  int16 pwr_db100;
  /*!< dB100 Power of the RGI Pwr Table */

} ftm_cal_data_instr_payload_rgi_pwr_pair_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for First sweep Measurement data

  @details
  Contains the Payload for First Sweep Measurement data
*/
typedef PACK(struct)
{
  uint8 swp1_pa;
  /*!< PA State for the first sweep measurements */

  uint8 swp1_rgi;
  /*!< RGI for the first sweep measurements */

  uint32 swp1_bias;
  /*!< Bias for the first sweep measurements */

  int16 swp1_pwr_db100;
  /*!< dB100 Power for the first sweep measurements */

} ftm_cal_data_instr_payload_swp1_meas_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for Data Push RGI vs Pwr Payload

  @details
  Contains the Payload for Data Push RGI vs Pwr Table
*/
typedef PACK(struct)
{
  uint8 num_rgi_pwr_pairs;
  /*!< Number of RGI Pwr pairs in the payload */

  //ftm_cal_data_instr_payload_rgi_pwr_tbl_type rgi_pwr_tbl[MAX_NUM_RGI_PWR_PAIRS];
  /*!< RGI Pwr Table in the payload */

} ftm_cal_data_instr_payload_rgi_pwr_tbl_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for Data Push DPD Table Payload

  @details
  Contains the Payload for Data Push DPD Table
*/
typedef PACK(struct)
{
  uint8 xpt_type;
  /*!< XPT Type - EPT/ET,etc. */

  uint8 num_amam;
  /*!< Number of AMAM values in the payload */

  uint16 *amam;
  /*!< AMAM values of the DPD Table in the payload */

  uint8 num_ampm;
  /*!< Number of AMPM values in the payload */

  uint16 *ampm;
  /*!< AMPM values of the DPD Table in the payload */

  uint8 num_det;
  /*!< Number of Detrough values in the payload */

  uint16 *det;
  /*!< Detrough values of the DPD Table in the payload */

} ftm_cal_data_instr_payload_load_dpd_tbl_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for M Line Data Process Payload

  @details
  Contains the Payload for M Line Data Process/calculations
*/
typedef PACK(struct)
{
  uint16 rms_dbfs_x100;
  /*!< PAR for M Line calculation (x100) in the payload */

  uint16 env_scale_x100;
  /*!< Clip Percent for M Line calculation (x100) in the payload */

  uint16 lin_reg_percent_x100;
  /*!< Linear Region Percent for M Line calculation (x100) in the payload */

  uint16 comp_dB;
  /*!< Compression point for M Line calculation in the payload */

  ftm_common_mline_curve_fit_type curve_fit_type;
  /*!< Curve Fit Type for M Line calculation in the payload */

  uint8 num_meas_pwrs;
  /*!< Number of Pwrs to be measured after M Line Calc in the payload */

  uint8 num_calc_pwrs;
  /*!< Number of Pwrs to be calculated (not measured) after M Line Calc in the payload */

  int16 meas_pwrs[MAX_NUM_MEAS_PWRS];
  /*!< Powers to be measured after M Line Calc in the payload */

  int16 calc_pwrs[MAX_NUM_CALC_PWRS];
  /*!< Powers to be calculated (not measured) after M Line Calc in the payload */

} ftm_cal_data_instr_payload_mline_calc_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for Data Process Delay Payload

  @details
  Contains the Payload for Data Process for Delay Value
*/
typedef PACK(struct)
{
  uint8 delay;
  /*!< Delay Value */

} ftm_cal_data_instr_payload_proc_delay_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  definition of Data Instruction Segment Sub-OpCode
*/
typedef uint32 ftm_calibration_data_action_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure for defining header for Data Opcode.
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

  uint8 technology;
  /*!< WAN technology for which the data opcode is to be setup.This will be 
  defined by #ftm_rf_technology_type */

  ftm_cal_data_device_type device;
  /*!< 8-bit mask indicating which for which devices the Data message 
  is sent. The number of sets of Data payload will equal the number of 
  set bits in the mask */

  ftm_calibration_data_action_type data_action;
  /*!< 32 Bit Sub Opcode */

} ftm_cal_data_instr_payload_header_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Definition of Data Opcode Calibration Configuration Instruction
*/
typedef PACK(struct)
{
  ftm_cal_data_instr_payload_header_type data_header;
  /*!< Structure for data opcode payload header */

  ftm_cal_data_instr_payload_rgi_pwr_tbl_type const *rgi_pwr_tbl;
  /*!< Pointer to RGI Pwr Table payload */

  ftm_cal_data_instr_payload_load_dpd_tbl_type const *load_dpd_tbl;
  /*!< Pointer to Load DPD Table payload */

  ftm_cal_data_instr_payload_mline_calc_type const *mline_calc_type;
  /*!< Pointer to MLine Calc payload */

  ftm_cal_data_instr_payload_proc_delay_type const *proc_delay;
  /*!< Pointer to Process Delay payload */

} ftm_cal_data_instr_payload_type;

/*----------------------------------------------------------------------------*/

/*!< M Line Results Selection Sweep parameters */
typedef struct
{
  uint8 num_selec_swp_pwrs;
  /*!< Number of Selection Sweep target powers */

  int16 selec_swp_pwrs[MAX_NUM_MEAS_PWRS];
  /*!< Selection Sweep target powers */

} mline_results_selec_swp_params;
       
/*----------------------------------------------------------------------------*/ 
/*!< M Line Results Compression Point paramters */  
typedef struct
{
  uint8 num_compression_points;
  /*!< Number of Compression Point RGIs */

  uint8 compression_point_rgi[MAX_NUM_MEAS_PWRS];
  /*!< Compression point RGIs */

  uint32 compression_point_bias[MAX_NUM_MEAS_PWRS];
  /*!< Compression point Bias corresponding to the RGIs */

} mline_results_compression_point;

/*----------------------------------------------------------------------------*/
/*!< M Line Results AMAM RMS paramters */  
typedef struct
{
  uint8 num_amam_rms_vals;
  /*!< Number of AMAM RMS values */

} mline_results_amam_rms;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure used to hold the unpacked data opcode parameter
*/
typedef struct
{
  uint8 debug_mode_en;
  /*!< Flag indicating, if the debug mode is enabled */

  uint16 flag;
  /*!< 16-bit flag indicating a specific type of action to be perform the 
  given payload. */

  uint8 technology;
  /*!< WAN technology for which the data opcode is to be setup.This will be 
  defined by #ftm_rf_technology_type */

  uint32 refChan;
  /*!< Ref Channel for data opcode */

  ftm_cal_data_device_type device;
  /*!< 8-bit mask indicating which for which devices the Data message 
  is sent. The number of sets of Data payload will equal the number of 
  set bits in the mask */

  struct
  {
    ftm_calibration_data_action_type data_action;
    /*!< 32 Bit Sub Opcode */

    struct
    {
      uint8 num_rgi_pwr_pairs;
      /*!< Number of RGI Pwr pairs in the payload */

      uint8 pa_state;
      /*!< PA state for RGI Pwr pairs table in the payload */

      uint8 xptMode;
      /*!< XPT mode for RGI Pwr pairs table in the payload */

      ftm_cal_data_instr_payload_rgi_pwr_pair_type rgi_pwr_tbl[MAX_NUM_RGI_PWR_PAIRS];
      /*!< RGI Pwr Table in the payload */

    } rgi_pwr;
    /*!< RGI Pwr Table */

    struct
    {
      uint8 xpt_type;
      /*!< XPT Type - EPT/ET,etc. */

      uint8 num_amam;
      /*!< Number of AMAM values in the payload */

      uint16 amam[MAX_NUM_DPD_VALS];
      /*!< AMAM values of the DPD Table in the payload */

      uint8 num_ampm;
      /*!< Number of AMPM values in the payload */

      uint16 ampm[MAX_NUM_DPD_VALS];
      /*!< AMPM values of the DPD Table in the payload */

      uint8 num_det;
      /*!< Number of Detrough values in the payload */

      uint16 det[MAX_NUM_DET_VALS];
      /*!< Detrough values of the DPD Table in the payload */

    } load_dpd;
    /*!< Load DPD */

    struct
    {
      uint16 rms_dbfs_x100;
      /*!< RMS dBFs for M Line calculation (x100) in the payload */

      uint16 env_scale_x100;
      /*!< Env Scale for M Line calculation (x100) in the payload */

      uint16 lin_reg_percent_x100;
      /*!< Linear Region Percent for M Line calculation (x100) in the payload */

      uint16 comp_dB_ept;
      /*!< EPT Compression point for M Line calculation in the payload */

      uint16 comp_dB_et;
      /*!< ET Compression point for M Line calculation in the payload */

      int8 pa_ept;
      /*!< EPT PA State for M Line calculation in the payload */

      int8 pa_et;
      /*!< ET PA State for M Line calculation in the payload */

      ftm_common_mline_curve_fit_type curve_fit_type;
      /*!< Curve Fit Type for M Line calculation in the payload */

      uint32 vd_max_ept;
      /*!< Vd Max for EPT for M Line calculation in the payload */

      uint32 vd_max_et;
      /*!< Vd Max for EPT for M Line calculation in the payload */

      uint8 mline_bias_option;
      /*!< Mline Bias option :- Algorithm used to limit the bias in Mline outputs */

      uint8 num_swp1_meas_pwrs;
      /*!< Number of Pwrs measured in Swp1 in the payload */

      uint8 num_swp2_ept_meas_pwrs;
      /*!< Number of Pwrs to be measured after M Line Calc in the payload */

      uint8 num_swp2_ept_calc_pwrs;
      /*!< Number of Pwrs to be calculated (not measured) after M Line Calc in the payload */

      uint8 num_swp2_et_meas_pwrs;
      /*!< Number of Pwrs to be measured after M Line Calc in the payload */

      uint8 num_swp2_et_calc_pwrs;
      /*!< Number of Pwrs to be calculated (not measured) after M Line Calc in the payload */

      ftm_cal_data_instr_payload_swp1_meas_type swp1_meas_data[MAX_NUM_MEAS_PWRS];
      /*!< Measured data for Swp1 in the payload */

      int16 swp2_ept_meas_pwrs[MAX_NUM_MEAS_PWRS];
      /*!< Powers to be measured after M Line Calc in the payload */

      int16 swp2_ept_calc_pwrs[MAX_NUM_CALC_PWRS];
      /*!< Powers to be calculated (not measured) after M Line Calc in the payload */

      int16 swp2_et_meas_pwrs[MAX_NUM_MEAS_PWRS];
      /*!< Powers to be measured after M Line Calc in the payload */

      int16 swp2_et_calc_pwrs[MAX_NUM_CALC_PWRS];
      /*!< Powers to be calculated (not measured) after M Line Calc in the payload */
    } mline_params;
    /*!< MLine Calc Params */

    struct
    {
      uint8 xpt_mode;
      /*!< XPT Mode */

      uint8 num_swp2_meas_pwrs;
      /*!< Number of Meas Pwrs for DPD Constrcution in the payload */

      uint8 num_swp2_calc_pwrs;
      /*!< Number of Calc Pwrs for DPD Constrcution in the payload */

      int16 swp2_meas_pwrs[MAX_NUM_MEAS_PWRS];
      /*!< Powers to be measured for DPD Constrcution in the payload */

      int16 swp2_calc_pwrs[MAX_NUM_CALC_PWRS];
      /*!< Powers to be calculated (not measured) for DPD Constrcution in the payload */

    } dpd_constr_params;
    /* DPD Processing Params */

    struct
    {
      uint8 load_action;
      /*!< Load Calc DPD Action*/

      uint8 xpt_mode;
      /*!< XPT Mode */

      uint16 qpoet_max_volt_x100;
      /*!< Max QPOET Voltage */

      uint16 qpoet_min_volt_x100;
      /*!< Min QPOET Voltage */

      uint16 et_min_volt_x100;
      /*!< Min ET Voltage */

      uint8 num_swp2_meas_pwrs;
      /*!< Number of Pwrs to be measured after M Line Calc in the payload */

      uint8 num_swp2_calc_pwrs;
      /*!< Number of Pwrs to be calculated (not measured) after M Line Calc in the payload */

      int16 swp2_meas_pwrs[MAX_NUM_MEAS_PWRS];
      /*!< Powers to be measured after M Line Calc in the payload */

      int16 swp2_calc_pwrs[MAX_NUM_CALC_PWRS];
      /*!< Powers to be calculated (not measured) after M Line Calc in the payload */

    } load_calc_dpd_params;
    /*!< Load Calc DPD Params */

    struct
    {
      int32 upper_bound_tx_earfcn;
      /*!< Upper bound Tx Channel*/

    } et_delay_params;
    /*!< ET Delay Params */

    struct
    {
      uint8 et_compression_point;
      /*!< Target compression point for calculating pin backoff*/

      uint32 current_chan;
      /*!< Reference channel used for normalizing pin compensation*/

      uint8 pa_state;
      /*PA state to process cal data*/

      uint8 pin_process_mask;
      /*!< Specify what PIN process is required: */
      /*!< if bit0 is 1, requiring AMAM PIN CAL. */
      /*!< if bit1 is 1, requiring AMPM scaling. */
      /*!< This mask is not used in traditional PIN CAL( sub-opcode is 0x100) */
	  
    } pin_params;
    /*!< pin Params */

    struct
    {
       uint16 ept_mline_results_action;
       /*!< EPT M Line Action sub-opcode*/

       uint16 et_mline_results_action;
       /*!< ET M Line Action sub-opcode*/

       mline_results_selec_swp_params ept_mline_results_selec_swp_meas_params;
       /*!< EPT M Line Results Selection Sweep Measured parameters */

       mline_results_selec_swp_params ept_mline_results_selec_swp_calc_params;
       /*!< EPT M Line Results Selection Sweep Calculated parameters */

       mline_results_selec_swp_params et_mline_results_selec_swp_params;
       /*!< ET M Line Results Selection Sweep parameters */

       mline_results_compression_point ept_mline_results_compression_point;
       /*!< EPT M Line Results Compression point parameters */

       mline_results_compression_point et_mline_results_compression_point;
       /*!< ET M Line Results Compression point parameters */

       mline_results_amam_rms ept_mline_results_amam_rms;
       /*!< EPT M Line Results AMAM RMS parameters */

       mline_results_amam_rms et_mline_results_amam_rms;
       /*!< ET M Line Results AMAM RMS parameters */

    } mline_results;
    /*!< M Line results */

    struct
    {
      uint32 result_mask;
      uint32 num_results;
    }xpt_capture_results;

    /*!< XPT capture results */

  } data_param;
  /*!< List of Data parameter in the payload */

} ftm_cal_data_parameter_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold detrough row information of Load DPD result
*/
typedef struct
{
  uint8 det_valid;
  /*!< Flag to indicate if Det data is valid or not */

  uint8 measured;
  /*!< Flag to indicate if measured or calculated */

  uint16 epdt_val[MAX_NUM_DET_VALS];
  /*!< Detrough Result Table */

} ftm_calibration_data_epdt_row_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold row information of Data DPD Construction result
*/
typedef struct
{
  uint8 dpd_valid;
  /*!< Falg to indicate if DPD data is valid or not */

  int8 xpt_mode;
  /*!< XPT Mode for current row */

  int16 rgi;
  /*!< RGI for current row */

  int16 smps_bias;
  /*!< Vcc for current row */

  int8 pa_state;
  /*!< PA State for current row */

  int8 measured;
  /*!< To indicate Measured or Calculatd entry for current row */

  int16 power;
  /*!< Power requested for current row */

  uint16 amam_res[MAX_RES_DPD_SIZE];
  /*!< AMAM Table to be written to NV */

  int16 ampm_res[MAX_RES_DPD_SIZE];
  /*!< AMPM Table to be written to NV */

} ftm_calibration_data_dpd_result_row_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to be used to push Data Operation results to calv3
  common framework
*/
typedef struct
{
  ftm_cal_data_device_type device;
  /*!< 8-bit mask indicating which for which devices the Data Operation 
    message is sent. The number of sets of RT Meas payload will equal the
    number of set bits in the mask */

  struct
  {
    uint8 num_swp2_meas_pwrs;
    /*!< Number of Meas Pwrs for DPD Constrcution in the payload */

    uint8 num_swp2_calc_pwrs;
    /*!< Number of Calc Pwrs for DPD Constrcution in the payload */

    ftm_calibration_data_epdt_row_type meas_epdt_row[MAX_NUM_MEAS_PWRS];
    /*!< Meas Pwr EPDT Results */

    ftm_calibration_data_epdt_row_type calc_epdt_row[MAX_NUM_MEAS_PWRS];
    /*!< Calc Pwr EPDT Results */

  } load_unity_dpd_results;

  struct
  {
    uint8 num_swp2_meas_pwrs;
    /*!< Number of Meas Pwrs for DPD Constrcution in the payload */

    uint8 num_swp2_calc_pwrs;
    /*!< Number of Calc Pwrs for DPD Constrcution in the payload */

    ftm_calibration_data_dpd_result_row_type dpd_meas_res_row[MAX_NUM_MEAS_PWRS];
    /*!< Meas Pwr DPD Results */

    ftm_calibration_data_dpd_result_row_type dpd_calc_res_row[MAX_NUM_CALC_PWRS];
    /*!< Calc Pwr DPD Results */

  } dpd_constr_results;

} ftm_calibration_data_result_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to be used to push Delay Values from Data Operation results
  to calv3 common framework
*/
typedef struct
{
  ftm_cal_data_device_type device;
  /*!< 8-bit mask indicating which for which devices the Data Operation 
    message is sent. The number of sets of RT Meas payload will equal the
    number of set bits in the mask */

  struct 
  {
    uint32 channel;
    /*!< channel corresponding to ET path delay calc */

    uint32 bandwidth;
    /*!< bandwidth corresponding to ET path delay calc */

    int32 delay_p1ns;
    /*!< ET path delay result in units of 0.1 ns */
  } et_path_delay_results;

}ftm_calibration_delay_results_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to be used to push Pin Values from Data Operation results
  to calv3 common framework
*/
typedef struct
{
  ftm_cal_data_device_type device;
  /*!< 8-bit mask indicating which for which devices the Data Operation 
    message is sent. The number of sets of RT Meas payload will equal the
    number of set bits in the mask */

  struct 
  {
    uint32 channel;
    /*!< channel corresponding to Pin calc */

    int32 pin_compensation;
    /*!< calculate pin compensation value for each chan */

    uint8 pin_peak_compression;
    /*!< highest compression achieved by pin IQ capture */

    boolean pin_cal_success;
    /*!< flag to indicate if calculation is successful */
  } et_pin_results;

}ftm_calibration_pin_results_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to be used to push extended Pin Values from Data Operation results
  to calv3 common framework
*/
typedef struct
{
  ftm_cal_data_device_type device;
  /*!< 8-bit mask indicating which for which devices the Data Operation 
    message is sent. The number of sets of RT Meas payload will equal the
    number of set bits in the mask */

  struct 
  {
    uint32 channel;
    /*!< channel corresponding to Pin calc */

    uint8 pin_process_mask;
    /*!< Specify what extra process data is contained */

    uint8 pin_status_mask;
    /*!< Specify what extra process is successful */

    int32 pin_compensation;
    /*!< calculate pin compensation value for each chan */

    uint8 pin_peak_compression;
    /*!< highest compression achieved by pin IQ capture */

    int32 ampm_scaling_factor;
    /*!< AMPM Scaling factor */
  } et_pin_results;

}ftm_calibration_ext_pin_results_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold data pertaining to Radio Retune OpCode */
typedef struct
{
  uint8 *res_buff;
  /*!< 
  @brief 
  Temporary Data result buffer used to store calibration results for 
  the current segment. 

  @details 
  This buffer will be reused in every segment. The purpose 
  of this buffer to pre-allocate the enough memory to store the maximum 
  possible data results for a given segment. When data operation 
  is called, pointer to this buffer is sent; so that the corresponding 
  data function can fill the calibration results. At the 
  end of the segment, the data in this buffer will be pushed to results 
  buffer. 

  This buffer is allocated during Calibration configuration, and deallocated 
  at the end of the sweep. 
  */

  ftm_cal_data_parameter_type up_param;
  /*!< Structure holding all the unpacked payload for Data opcode */

} ftm_cal_data_opcode_data_type;

/*----------------------------------------------------------------------------*/
boolean 
ftm_calibration_data_process_rgi_pwr_tbl_instr
( 
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_data_parameter_type const *data_param,
  uint32 chan,
  boolean mul_table,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
boolean 
ftm_calibration_data_process_load_dpd_tbl_instr
( 
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_data_parameter_type const *data_param,
  ftm_rf_technology_type ftm_rf_tech,
  uint8 device_ind,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type 
ftm_calibration_data_process_mline_calc_instr
( 
  void *ftm_req_data
);

/*----------------------------------------------------------------------------*/
boolean 
ftm_calibration_data_process_mline
( 
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_data_parameter_type const *data_param,
  ftm_rf_technology_type ftm_rf_tech,
  uint32 chan,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_data_process_dpd_construction
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_data_parameter_type const *data_param,
  uint32 chan,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_data_load_calc_dpd
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_data_parameter_type const *data_param,
  ftm_rf_technology_type ftm_rf_tech,
  uint32 chan,
  uint8 device_ind,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_data_process_mline_results
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_data_parameter_type const *data_param,
  uint32 chan,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_data_init
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_data_deinit
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
uint16
ftm_calibration_data_unpack
( 
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_data_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_data_process_pin
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_data_parameter_type const *data_param,
  ftm_rf_technology_type ftm_rf_tech,
  uint32 chan,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

#ifdef __cplusplus
}
#endif

#endif /* FTM_CALIBRATION_V3_DATA_H */


