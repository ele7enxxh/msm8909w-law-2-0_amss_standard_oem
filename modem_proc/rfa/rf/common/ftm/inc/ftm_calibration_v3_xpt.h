#ifndef FTM_COMMON_CALIBRATION_V3_XPT_H
#define FTM_COMMON_CALIBRATION_V3_XPT_H
/*!
  @file
  ftm_common_calibration_v3.h

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_calibration_v3_xpt.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/10/14   cd/cri  Merge Pin Cal
05/16/14   cri     IQ capture segment reduction changes
01/30/14   jmf     Support for FBRx Type XPT IQ caps for FBRx Internal Cal
08/27/13   aka     1x and WCDMA updates
07/30/13   aka     Added XPT_CONFIG_SELFTEST config
04/22/13   jmf     Implement xPT processing Enum Type
04/15/13   aro     Mainline calv3 optimization code/delete obsolete code
04/01/13   aro     Fixed random buffer size initialization
04/01/13   aro     Debug flag to switch between old and new calv3 implementation
04/01/13   aro     Added hookup for new calv3 infrastructure
04/01/13   aro     F3 update
03/18/13   aro     Changes to be compatible with XPT opcode defintion
                   update
01/22/13   aro     Added field in XPT IQ capture to indicate the type of
                   IQ capture
01/22/13   aro     Removed DPD index from IQ capture interface
01/18/13   aro     Deleted nikel-XPT specific code
12/03/12   aro     Renamed variable name
08/07/12   vs      Modifications to Cal V3 Tx config data packet
08/04/12   vs      Add IQ gain setpoint in Cal V3 Tx config data packet
07/25/12   aro     XPT Payload update
07/25/12   aro     Grouped XPT header into new structure
07/25/12   aro     Added time profiling for EPT
07/25/12   aro     Enabled XPT debug mode flag
07/19/12   aro     Function name update
07/18/12   aro     Converted results push function to STATIC
07/18/12   aro     DPD Index added to IQ Capture payload
07/18/12   aro     Renamed the XPT type definitions
07/18/12   aro     Moved the XPT result pushing to common XPT framework
07/18/12   aro     Added Error Code and SubCopde fields in XPT results
07/17/12   aro     Updated XPT Opcode payload
07/17/12   aro     Enhanced support of configure feedback path
07/17/12   aro     Added XPT opcode to configure feedback path
07/17/12   aro     Renamed XPT opcode enums
07/17/12   aro     F3 update
07/06/12   aro     Rearranged arguments for the XPT process function
06/14/12   anr     Updates and bug fixes to common framework for EPT 
06/14/12   aro     Added Doxygen block
06/14/12   aro     Added function to populate EPT results buffer
06/13/12   aro     Fixed Phase Shift size
06/13/12   aro     Doxygen documentation update
06/13/12   aro     Updated function to compute paylaod size to pass the pointer
                   to cal v3 data
06/13/12   aro     Implemented DPD coefficient interface
06/13/12   aro     Defined results buffer pattern for EPT
06/13/12   aro     Code beautification
06/13/12   aro     Update EPT configure, iq capture, and override functions
                   to have devices
06/13/12   aro     Removed unrelevant fields from EPT payloads
06/05/12   anr     Initial EPT Support
06/04/12   ks      Add FTM-PC interface for XPT

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfm_mode_types.h"
#include "rfcommon_core_xpt.h"
#include "ftmdiag.h"
#include "ftm_calibration_v3_opcode.h"
#include "ftm_calibration_v3_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef T_WINNT
#error code not present
#endif

/*----------------------------------------------------------------------------*/
/*! Forward declaration of Cal v3 data  */
struct ftm_calibration_data_type_t;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Maximum allowed XPT results size in bytes for the current segment.
 
  @details
  Currently set to dpd coefficient size
*/
#define FTM_CAL_SEG_XPT_BUFF_SIZE_MAX 55

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration of all XPT Calibration Actions.
*/
typedef enum
{

  XPT_OPCODE_MASK_EPT_CONFIG            = 0x01,
  /*!< EPT Config Item */

  XPT_OPCODE_MASK_EPT_CAPTURE_IQ        = 0x02,
  /*!< Enable EPT IQ Capture - Also indicates EPT processing */

} ftm_calibration_xpt_action_enum_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Error mask enumeration indicating errors in each segment
*/
typedef enum
{

  XPT_ERR_OPERATION_FAILURE    = 1,
  /*!< Error code indicating the XPT operation has failed. */

  XPT_ERR_NULL_CB              = 2,
  /*!< Error code to indicate that the tech specific XPT callback 
  function is NULL. This error code will be seen when the XPT 
  opcode bit is enabled while the CB function is NULL. */ 

  XPT_ERR_CAL_CONFIG_FAILURE    = 3,
  /*!< Error code indicating the XPT Cal config has failed. */

  XPT_ERR_IQ_CAPTURE_FAILURE    = 4,
  /*!< Error code indicating the XPT IQ capture has failed. */

  XPT_ERR_MAX_NUM
  /*!< Maximum number of segment error code. This enum should not be 
  used for any purpose other than counting the number of error codes. */

} ftm_calibration_xpt_error_code_mask;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enumeration indicating the type of Mode for XPT confi
*/
typedef enum
{
  XPT_CONFIG_APT = 0, /*!< APT mode */

  XPT_CONFIG_EPT = 1,  /*!< EPT mode */

  XPT_CONFIG_ET = 2, /*!< ET Mode */
  
  XPT_CONFIG_SELFTEST = 3 /*!< Self Test Mode */
} ftm_calibration_xpt_config_mode_enum;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enumeration indicating the type of XPT IQ capture
*/
typedef enum
{
  XPT_IQCAP_FLAG_EPT = 0, /*!< Enhanced Power Tracking */

  XPT_IQCAP_FLAG_ET = 1,  /*!< Envelop Tracking */

  XPT_IQCAP_FLAG_DELAY = 2, /*!< Delay */

  XPT_IQCAP_FLAG_SELF_TEST_EVM = 3, /*!< Self Test */

  XPT_IQCAP_FLAG_PIN = 4, /*!< Self Test */
  //XPT_IQCAP_FLAG_IQ_RMS (5) need to be ported from Dime

  XPT_IQCAP_FLAG_FBRX = 6,

  XPT_IQCAP_FLAG_SELF_TEST_VSWR = 7,

  XPT_IQCAP_FLAG_SELF_TEST_RX_RMS = 8

} ftm_calibration_xpt_iq_capture_enum;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enumeration indicating the type of XPT IQ capture processing
*/
typedef enum
{
  XPT_IQCAP_PROC_0 = 0, /*!< No Processing */

  XPT_IQCAP_PROC_1 = 1,  /*!< Processing Type 1 */

  XPT_IQCAP_PROC_2 = 2, /*!< Processing Type 2 */

  XPT_IQCAP_PROC_3 = 3 /*!< Processing Type 3 */

} ftm_calibration_xpt_iq_capture_proc_enum;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Definition of XPT result mask
*/
typedef uint32 ftm_calibration_xpt_result_mask_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  definition of XPT Instruction Debug Mode OpCode
*/
typedef uint8 ftm_calibration_xpt_debug_mode_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  definition of XPT Instruction Segment OpCode. This is defined by
  #ftm_calibration_xpt_action_enum_type
*/
typedef uint32 ftm_calibration_xpt_action_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Definition of XPT Instruction Segment Error Code
*/
typedef uint64 ftm_calibration_xpt_error_code_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Definition of XPT results version
*/
typedef uint8 ftm_calibration_xpt_result_version_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Definition of XPT version
*/
typedef uint8 ftm_calibration_xpt_version_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Definition of XPT IQ capture type
*/
typedef uint8 ftm_calibration_xpt_iq_capture_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for EPT Configure Payload

  @details
  Contains the XPT Payload for EPT Configure
*/
typedef PACK(struct)
{
  uint8 primary_device;
  /*!< Primary Device associated with the feedback path */

  uint8 feedback_device;
  /*!< Feedback device which needs to be configured */

  uint8 xpt_mode;
  /*!< Vairable to indicate the XPT mode of operation. This is defined 
  by #ftm_calibration_xpt_config_mode_enum*/

} ftm_cal_xpt_instr_payload_ept_config_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for EPT Capture IQ XPT Payload

  @details
  Contains the XPT Payload for EPT Capture IQ
*/
typedef PACK(struct)
{
  uint8 primary_device;
  /*!< Primary Device associated with the feedback path */

  uint8 feedback_device;
  /*!< Feedback device which needs to be configured */

  ftm_calibration_xpt_iq_capture_type capture_type;
  /*!< Field indicting which type of IQ capture is to be done. Defined by 
  #ftm_calibration_xpt_iq_capture_enum */

  uint16 num_samples;
  /*!< Number of IQ Samples to be captured  */

  uint8 first_trigger;
  /*!< Indicator for first trigger  */

  uint8 last_trigger;
  /*!< Indicator for first trigger  */

  uint8 proc_flag;
  /*!< Flag indicating the processing to be done. This is defined by 
  ftm_calibration_xpt_iq_capture_proc_enum */

} ftm_cal_xpt_instr_payload_iq_capture_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM CDMA Dispatch Request Packet to be used to setup radio for
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

  ftm_cal_xpt_instr_payload_iq_capture_type xpt_param;
  /*!< Structure containing the Radio Setup parameter. PACKED structure is not 
  used here as the this command is sent only internally */

} ftm_cal_xpt_instr_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Definition of XPT Calibration Configuration Instruction header
*/
typedef PACK(struct)
{
  ftm_calibration_xpt_debug_mode_type debug_mode_en;
  /*!< Flag indicating EPT debuig mode */

  ftm_calibration_xpt_version_type version;
  /*!< Version of the OpCode */

  uint16 flag;
  /*!< 16-bit mask where each bit indicates a specific type of action. */

  ftm_calibration_xpt_action_type xpt_action;
  /*!< OpCode corresponding to the xpt segment. This will be the starting
  point to iteratively extract N number of opcodes and thier respective
  payloads. */

} ftm_cal_xpt_instr_payload_header_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Definition of XPT Calibration Configuration Instruction
*/
typedef PACK(struct)
{
  ftm_cal_xpt_instr_payload_header_type xpt_header;
  /*!< Structure for XPT */

  ftm_cal_xpt_instr_payload_ept_config_type const *ept_config;
  /*!< Pointer to EPT config payload */

  ftm_cal_xpt_instr_payload_iq_capture_type const *iq_capture;
  /*!< Pointer to EPT capture IQ payload */

} ftm_cal_xpt_instr_payload_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function Pointer definition for Callback function to XPT Configure EPT Capture

  @details
  This funtion will configure Tx Waveform for the given mode. Radio should
  already be in Tx State when this function is called.
*/
typedef boolean (*ftm_cal_xpt_ept_configure_cb_type)
(
   ftm_cal_xpt_instr_payload_ept_config_type const *payload
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function Pointer definition for Callback function to XPT Configure EPT Capture

  @details
  This funtion will configure Tx Waveform for the given mode. Radio should
  already be in Tx State when this function is called.
*/
typedef boolean (*ftm_cal_xpt_iq_capture_cb_type)
(
  ftm_cal_xpt_instr_payload_iq_capture_type const *iq_capture
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure containing RF XPT Calibration Sweep Tech specific parameter
*/
typedef struct
{
  ftm_cal_xpt_ept_configure_cb_type configure_ept_fp;
  /*!<
  @brief
  Function Pointer definition for Callback function to configure EPT capture
  
  @details
  This function will configure the EPT IQ Capture.
  */
  
  ftm_cal_xpt_iq_capture_cb_type capture_ept_iq_fp;
  /*!<
  @brief
  Function Pointer definition for Callback function to EPT capture IQ

  @details
  This function will capture IQ and compute AMAM/AMPM tables and
  associate those tables with the given DPD index.
  */
  
} ftm_calibration_xpt_fp_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold data pertaining to XPT OpCode */
typedef struct
{
  ftm_calibration_xpt_fp_type oc_process_fp[FTM_CALIBRATION_NUM_OF_TECHS];
  /*!< Function Pointer definition for Callback function */

  uint8 *res_buff;
  /*!< 
  @brief 
  Temporary XPT Results buffer used to store calibration results for the 
  current segment. 

  @details 
  This buffer will be reused in every segment. The purpose 
  of this buffer to pre-allocate the enough memory to store the maximum 
  possible XPT results for a given segment. When measure 
  XPT is called, pointer to this buffer is sent; so that each tech 
  specific measure XPT function can fill the calibration results. At the 
  end of the segment, the data in this buffer will be pushed to results 
  buffer. 

  This buffer is allocated during Calibration configuration, and deallocated 
  at the end of the sweep. 
  */ 

} ftm_cal_xpt_data_type;

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_xpt_init
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_xpt_deinit
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
uint16
ftm_calibration_xpt_unpack
(
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_xpt_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
rfcommon_xpt_processing_type
ftm_calibration_xpt_map_proc_type(ftm_calibration_xpt_iq_capture_proc_enum xpt_proc_flag);

#ifdef T_WINNT
#error code not present
#endif

#ifdef __cplusplus
}
#endif

#endif /* FTM_COMMON_CALIBRATION_V3_XPT_H */

