#ifndef FTM_COMMON_CALIBRATION_V3_H
#define FTM_COMMON_CALIBRATION_V3_H
/*!
  @file
  ftm_common_calibration_v3.h

  @brief
  This module contains prototypes and definitions used by common RF calibration
  code.
*/

/*==============================================================================

  Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_common_calibration_v3.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/18/14    xsh   Fix compiler warning for enum type conversion
05/07/14    br     Included the header for Enhanced Internal Device Cal opcode function
30/10/13    pk     Added F3s enable/disable conditions to optimize the cal time
08/13/13    sk     Added support for compressed ESC Cal Command
07/15/13   cri     Added SYNC opcode support
04/19/13    dj     Support for Feedback Setup Opcode
04/16/13   aro     Variable/type/prototype cleanup
04/16/13   aro     Moved Opcode Data to HK data structure
04/15/13   aro     Mainline calv3 optimization code/delete obsolete code
04/09/13    id     Support for storing current band and channel in global status
04/02/13   aro     Migrated to inline Math functions
04/02/13   aro     Command for enable debug level for calv3
04/01/13   aro     Debug flag to switch between old and new calv3 implementation
04/01/13   aro     Added hookup for new calv3 infrastructure
04/01/13   aro     F3 update
03/29/13   aro     Radio Config/Retune optimized opcode proc functions
03/29/13   aro     Tx Config/Meas optimized opcode proc functions
03/29/13   aro     Rx Config/Meas optimized opcode proc functions
03/28/13   aro     Added results publishing and pushing interfaces
03/27/13   aro     Data structure restructing
03/27/13   aro     Added fetch opcode function
03/27/13   aro     OpCode unpack function
03/27/13   aro     Initial support for opcode sequencing
03/26/13   aro     Removed SW to HW timer transition to use ATS timer
01/02/13   aro     Support for results format revision
12/07/12   aki     Addded ftm_calibration_dev_flag_to_rfm_dev
12/03/12   aro     Renamed variable name
12/03/12   aro     Added support for Tx measure results buffer
11/30/12   aro     Initial support for Tx measure opcode
11/21/12   aro     Support for RX_MEASURE result 
11/19/12   aro     Added 0x4000 Rx Measure opcode
11/14/12   dw      Add Dime APT support, change pa_bias structure to support 
                   quiescent current 
                   control with multi-lin V1/Cal V3
11/05/12   aro     Updated OpCode fetch function to populate payload pointer
                   for ET opcodes
11/05/12   aro     Variable to store ISR offset value generated bny SYNC opcode
11/05/12   aro     Code to unpack sync opcode
11/05/12   aro     Added the sync event handling function pointer definitions
11/05/12   aro     Re-organized cal v3 data structure to be more modular
11/02/12   aro     Converted payload computation function to perform
                   opcode payload unpacking for Tx override
11/02/12   aro     Converted payload computation function to perform
                   opcode payload unpacking
11/02/12   aro     Added structure to hold current segment information
10/30/12   aro     Initial support for Tx override OpCode
10/29/12   aro     Added error code for Radio Setup
10/29/12   aro     Added data structure for Radio setup
10/22/12   aro     Added v2 function to fetch opcode using variable length
                   opcode infrastructure
10/18/12   aro     Interface to query the size of opcode from current segment
07/26/12   aro     Renamed XPT filename
07/19/12   aro     Enabled polling opcode in cal v3 infrastructure
07/19/12   aro     Flag to indicate that the sweep is paused for polling
07/19/12   aro     Added polling parameter initialization
07/19/12   aro     Added initial implementation of Polling OpOde
07/19/12   aro     F3 update + XPT variable name change
07/18/12   aro     Moved the XPT result pushing to common XPT framework
07/18/12   aro     Added Error Code and SubCopde fields in XPT results
06/13/12   aro     Implemented DPD coefficient interface
06/13/12   aro     Defined results buffer pattern for EPT
06/04/12   ks      Add FTM-PC interface for XPT
04/10/12   id      Make ftm_calibration_deinit_result_buffer function non-static
01/16/12   aro     Added ET Path disable action
01/13/12   aro     Converted Tx actions to Mask
01/13/12   aro     Documentation Update
01/12/12   aro     Converted Tx flag and Rx flag to device flag
01/12/12   aro     Converted Tx actions from MASK to command
11/15/11   aro     Added check to return error code for the case when a new
                   CCP is received in the middle of calibration sweep
10/28/11   aro     Deleted unused error codes
10/20/11   adk     Added support for SV diversity.
09/12/11   aro     Fixed the bug where the segment number warps around after
                   255
08/25/11   aro     Enabled Segment Error codes + expanded error code to 64 bit
08/25/11   aro     Partitioned Configuration and Results error Code
08/23/11   aro     Documentation Update
08/20/11   aro     Added Tx waveform type to support LTE-TDD Calibration
08/19/11   aro     Klocworks error fixes
08/19/11   aro     Added feature to enable debug mode using test tree parameter
08/18/11   aro     Added error code to flag timer uodate error
08/11/11   aro     Added OpCode in data structure for debugging purpose
08/10/11   aro     Documentation Update
08/09/11   aro     Added support to log error code for faulty segment
08/04/11   aro     Added support to perform Tx Sweep for different PA states in
                   different Cal sweep without radio tear down
07/28/11   aro     [1] Added Result Query counter to keep track of number of
                   result queries done by Tools side
                   [2] Documentation Update
07/28/11   aro     [1] Removed MC Rx Config Opcode
                   [2] Re-sequenced the OpCode Bit mask
                   [3] Added Frequency offset in RF Caonfig and retune opcode
                   to support dual carrier calibration 
07/27/11   aro     [1] Updated Radio Config and radio retune function to have
                   return type
                   [2] Removed expected RxAGC from ConfigRx Opcode
                   [3] Documentation Update
07/20/11   aro     Added Rx Measurement OpCode
07/20/11   aro     [1] Renamed Cal v3 Opcode/payload type names for consistency
                   [2] Documentation Update
07/20/11   aro     [1] Fixed maxuimum rx result buffer length
                   [2] Updated result type sttructure to pass length of rx and
                   tx length
                   [3] Removed unsed types
                   [4] Documentation Update
07/19/11   aro     [1] Updated Config Tx and Measure Tx functions to handle multiple
                   Tx Data
                   [2] Updated Rx/Tx Flag, Rx/Tx Action enums
07/19/11   aro     [1] Added ftm_calibration_segment_num_type to define
                   the segment number for result push
                   [2] Changed the segment result buffer pointer to byte ptr
07/19/11   aro     Updated Config Rx and Measure Rx functions to handle multiple
                   Device for a given segment
07/08/11   aro     Updated Rx measurement interface to pass pointer to result
                   container
07/08/11   aro     [1] Changed Rx cal actions to UINT16
                   [2] Added Tx Cal actions
07/08/11   aro     Added support to return multiple Rx measurement data
07/08/11   aro     Added entry for tech specific RxAGC settling time
06/30/11   aro     [1] Fixed wrong type of expected RxAGC
                   [2] Added documentation to fix specify that the Channel to be
                   sent from PC should be uplink channel.
06/29/11   aro     Documentation Update
06/28/11   aro     Documentation Update
06/27/11   aro     Added support for Tx waveform config OpCode
06/27/11   aro     [1] Documentation Update
                   [2] Moved Packet Header to separate type
06/27/11   aro     Renamed Type name
06/27/11   aro     Doxygen Documentation Update
06/27/11   aro     [1] Renamed Type name
                   [2] Regrouped type names
06/26/11   aro     Merged Read Node Data structure to Cal Config Data
06/26/11   aro     Moved Calibration Config Data to a seprate data str
06/26/11   aro     Converted instruction pointer to byte pointer in PUSH
                   sweep configuraiton function
06/25/11   aro     [1] Renamed function names
                   [2] Documentation Update
06/25/11   aro     [1] Added support to fetch CalV3 results
                   [2] Added support to teardown RF at the end of Sweep
06/24/11   aro     Moved results to separate data structure
06/24/11   aro     Enabled ISR CB function to push Cal Data to Result Buffer
06/24/11   aro     Added feature to push all Cal data at once
06/24/11   aro     Added feature to push all Cal data at once
06/23/11   aro     Added support to push Calibration data to result buffers
06/23/11   aro     Added PUSH/POP for off-target byte alignment
06/23/11   aro     Added support to push Calibration data to result buffer
06/23/11   aro     Added PUSH/POP for off-target byte alignment
06/21/11   aro     Deleted tech specific ISR func
06/20/11   aro     Updated Calibration Sweep Data structure to have opcode 
                   specific callback functions and state tracking variables 
06/20/11   aro     Off-target Test Fix : Removed PUSH/POP for packing
06/15/11   aro     Added function to allocation memory for results
06/13/11   aro     Changed CalSweep Config Response packet status to INT8
06/13/11   aro     [1] Changed cal_param type to UINT32 from void pointer
                   [2] Added Calibration Instruction Header for header sizing
06/13/11   aro     Added PACKED types
06/09/11   aro     Updated the RF Caonfiguration Opcode
06/08/11   aro     [1] Reused req_len field for Total instruction size
                   [2] Added functionality to send expected cal data from tools
06/08/11   aro     Removed Total instruction size field to replace with req_len
06/01/11   aro     Added Retune OpCode
06/01/11   aro     Added Retune OpCode
05/16/11   aro     Added HDET Measurement OpCode
05/16/11   aro     Updated the Tx Flag enum
05/16/11   aro     [1] Added RX Calibration Flags
                   [2] Updated Tx Flag enum types
05/16/11   aro     [1] Added Rx Calibration Action Enum
                   [2] Added HDET Measurement OpCode
                   [3] Updated Tx Measurement Payload
05/06/11   aro     Updated Calibration Sweep config interface to return
                   FTM response packet
05/05/11   aro     [1] Added error/success codes for return values
                   [2] Type name change
05/05/11   aro     Removed payload pointer to replace with individual opcode
                   payload pointers
05/05/11   aro     Added CRC check for Calibration packet
05/05/11   aro     Added pack(pop)
05/05/11   aro     Changed type for RxAGC
05/04/11   aro     [1] Documentation Update
                   [2] Renamed types/variable names
05/04/11   aro     Moved Calibration instruction type
05/04/11   aro     Changed the Payload pointer to constant pointer
05/04/11   aro     Removed interval from opcode specific Payload to common
05/04/11   aro     [1] Documentation Update
                   [2] Renamed types/variable names
05/04/11   aro     [1] Moved Calibration instruction type
                   [2] Added Opcode specific payload fields
05/04/11   aro     Updated Get Segment interface to return partitioned data
05/03/11   aro     Added C++ Protection
05/03/11   aro     [1] Added C++ Protection
                   [2] Fixed opcode Mask enum
                   [3] Added interval to all opcode payloads
04/25/11   aro     [1] Added interface to get Calibration config for a segment
                   [2] Renamed variables
                   [3] Defined Payload for opcodes
04/25/11   aro     Changed type names
04/25/11   aro     [1] Added interface to get Calibration config for a segment
                   [2] Renamed variables
04/25/11   aro     [1] Added support to configure CB function for various techs
                   [2] Added linked-list implementation to store RF Cal Config
                   [3] Renamed Sweep Config function
04/23/11   aro     Added Common Calibraiton OpCode Definitions
04/22/11   aro     Initial Revision

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#include "rfm_device_types.h"
#include "ftmdiag.h"
#include "ftm_calibration_v3_opcode.h"
#include "ftm_calibration_v3_types.h"
#include "ftm_calibration_v3_xpt.h"
#include "ftm_calibration_v3_polling.h"
#include "ftm_calibration_v3_radio.h"
#include "ftm_calibration_v3_tx.h"
#include "ftm_calibration_v3_sync.h"
#include "ftm_calibration_v3_rx.h"
#include "ftm_calibration_v3_data.h"
#include "ftm_calibration_data_module.h"
#include "ftm_calibration_v3_enh_internal_device_cal.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef T_WINNT
#error code not present
#endif

/*============================================================================*/
/*!
  @name Enums

  @brief
  This section contains enumerations used by Calibration Sweep
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration indicating the status response of Common Calibration
*/
typedef enum
{
  FTM_CALIBRATION_PACKET_ALLOCATED = 2,
  /*!< Success Code indicating that the Calibration request packet sent
  from PC is stored in Linked List in embedded side */

  FTM_CALIBRATION_ISR_SCHEDULED = 1,
  /*!< Success code indicating that ISE has been scheduled for calibration */

  FTM_CALIBRATION_ISR_UNDEFINED = 0,
  /*!< Initial error code value */

  FTM_CALIBRATION_CONFIG_RESULTS_ALLOC_FAILURE = -1,
  /*!< Error Code indicating that memory for results could not be allocated */

  FTM_CALIBRATION_CCP_WHILE_ACTIVE_SWEEP = -2,
  /*!< Error Code indicating that a new CCP is received while a calibration 
  sweep isn still active. This is a bad case indicating that tools 
  sent the CCP too early. */ 

  FTM_CALIBRATION_INVALID_CRC = -3,
  /*!< Error Code indicating bad CRC being detected at the end of the request
  packet */

  FTM_CALIBRATION_CONFIG_ALLOC_FAILURE = -4,
  /*!< Error Code indicating that the Configuration packet buffer allocation
  is empty */

  FTM_CALIBRATION_ISR_SCH_FAILURE = -5,
  /*!< Error Code indicating that the ISR could not be scheduled */

  FTM_CALIBRATION_UNCOMPRESSION_FAILURE = -6,
  /*!< Error code indicating that the uncompression process failed */

} ftm_calibration_config_error_code_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration indicating the status response of Common Calibration
*/
typedef enum
{
  FTM_CALIBRATION_RESULTS_READ_SUCCESS = 0,
  /*!< Success Code indicating that the Read to Result buffer is successful 
  and the result data sent back to PC is valid */

  FTM_CALIBRATION_RESULTS_ALLOC_FAILURE = -1,
  /*!< Error Code indicating that memory for results could not be allocated */

  FTM_CALIBRATION_RESULTS_READ_DISABLED = -2,
  /*!< Error Code indicating that the Read is disabled for the Result buffer 
  as write is happening currently */
  
  FTM_CALIBRATION_RESULTS_EMPTY_BUFFER = -3,
  /*!< Error Code indicating that the Result buffer is empty */
  
} ftm_calibration_result_error_code_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration of all valid OpCode used for RF Calibration.
*/
typedef enum
{

  FTM_CAL_OPCODE_MASK_CONFIG_RADIO   = 0x1,
  /*!< Opcode Bit indicating Cal action to configure Radio to RxTx State 
  for given band and channel on given devices. The payload for this 
  operation is documented in #ftm_cal_instr_payload_config_radio_type. */ 

  FTM_CAL_OPCODE_MASK_RETUNE_RADIO   = 0x2,
  /*!< Opcode Bit indicating Cal action to retune Radio to given band and 
  channel on given devices. The payload for this operation is documented in 
  #ftm_cal_instr_payload_retune_radio_type. */

  FTM_CAL_OPCODE_MASK_CONFIG_TX_WF  = 0x4,
  /*!< Opcode Bit indicating Cal action to configure Tx waveform. The payload 
  for this operation is documented in #ftm_cal_instr_payload_config_tx_wf_type.*/

  FTM_CAL_OPCODE_MASK_CONFIG_TX     = 0x8,
  /*!< Opcode Bit indicating Cal action to configure Tx for measurement. This 
  will basically override the TxAGC output parameter. The payload for this 
  operation is documented in #ftm_cal_instr_payload_config_tx_type. */

  FTM_CAL_OPCODE_MASK_MEAS_HDET     = 0x10,
  /*!< Opcode Bit indicating Cal action to measure HDET. This 
  will take HDET measurements. The payload for this operation is documented in 
  #ftm_cal_instr_payload_meas_hdet_type. */

  FTM_CAL_OPCODE_MASK_CONFIG_RX     = 0x20,
  /*!< Opcode Bit indicating Cal action to configure Rx for measurement. This 
  will override PowerMode, LNA state and LNA gain. The payload for this 
  operation is documented in #ftm_cal_instr_payload_config_rx_type. */

  FTM_CAL_OPCODE_MASK_MEAS_RX       = 0x40,
  /*!< Opcode Bit indicating Cal action to measure Rx. This 
  will take measurements to compute the LNA offsets. The payload for this 
  operation is documented in #ftm_cal_instr_payload_meas_rx_type. */

  FTM_CAL_OPCODE_XPT_MASK           = 0x80,
  /*!< Opcode Bit indicating Cal action to XPT operations. This 
  is the top level opcode for all XPT operations. The  
  operation is documented in #ftm_cal_xpt_instr_payload_type. */

  FTM_CAL_OPCODE_POLLING_MASK       = 0x400,
  /*!< Opcode Bit indicating Cal action to perform polling operations.The  
  operation is documented in #ftm_cal_instr_payload_polling_type. */

} ftm_calibration_opcode_mask_enum_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration of default CRC value to be checked against
*/
#define FTM_CALIBRATION_PACKET_CRC_VAL 0xDAAD

/*! \} */

/*============================================================================*/
/*!
  @name Error Logging

  @brief
  This section contains definitions used for error logging during calibration
  sweep.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Error mask enumeration indicating errors in each segment
*/
typedef enum
{

  FTM_CAL_SEG_ERR_SEG_DURATION_OVERRUN    = 0,
  /*!< Error code to indicate that there as segment duration overrun. Thus 
  the Rx or Tx measurement is not aligned with the external measurement tool.*/

  FTM_CAL_SEG_ERR_INVALID_RFM_MODE        = 1,
  /*!< Error code to indicate that the resolved RFM Mode for the calibration 
  sweep is invalid. */

  FTM_CAL_SEG_ERR_CFG_RADIO_FAILURE       = 2,
  /*!< Error code to indicate that tech specific Configure Radio action failed. 
  This indicates that radio could not be tuned to RxTx state on requested 
  device, band and channel.*/

  FTM_CAL_SEG_ERR_CFG_RADIO_NULL_CB       = 3,
  /*!< Error code to indicate that the tech specific configure radio callback 
  function is NULL. This error code will be seen when the Configure radio 
  opcode bit is enabled while the CB function is NULL. */ 

  FTM_CAL_SEG_ERR_CFG_TX_WF_FAILURE       = 4,
  /*!< Error code to indicate that tech specific Configure tx waveform action 
  failed. This indicates that Tx waveform could not be set to requested type.*/

  FTM_CAL_SEG_ERR_CFG_TX_WF_NULL_CB       = 5,
  /*!< Error code to indicate that the tech specific configure Tx waveform 
  callback function is NULL. This error code will be seen when the Configure 
  Tx wavefoem opcode bit is enabled while the CB function is NULL. */ 

  FTM_CAL_SEG_ERR_RETUNE_RADIO_FAILURE    = 6,
  /*!< Error code to indicate that tech specific retune Radio action failed. 
  This indicates that radio could not be retuned to RxTx state on requested 
  device, band and channel.*/

  FTM_CAL_SEG_ERR_RETUNE_RADIO_NULL_CB    = 7,
  /*!< Error code to indicate that the tech specific retune radio callback 
  function is NULL. This error code will be seen when the retune radio 
  opcode bit is enabled while the CB function is NULL. */ 

  FTM_CAL_SEG_ERR_CFG_TX_FAILURE          = 8,
  /*!< Error code to indicate that tech specific Configure Tx action failed. 
  This indicates that Tx radio could not be overridden to given PA State, RGI, 
  and PA Bias*/ 

  FTM_CAL_SEG_ERR_CFG_TX_NULL_CB          = 9,
  /*!< Error code to indicate that the tech specific configure Tx callback 
  function is NULL. This error code will be seen when the Configure Tx 
  opcode bit is enabled while the CB function is NULL. */ 

  FTM_CAL_SEG_ERR_MEAS_HDET_FAILURE       = 10,
  /*!< Error code to indicate that tech specific HDET measurement action 
  failed. */ 

  FTM_CAL_SEG_ERR_MEAS_HDET_NULL_CB       = 11,
  /*!< Error code to indicate that the tech specific measure HDET callback 
  function is NULL. This error code will be seen when the measure HDET 
  opcode bit is enabled while the CB function is NULL. */ 

  FTM_CAL_SEG_ERR_CFG_RX_FAILURE          = 12,
  /*!< Error code to indicate that tech specific configure Rx action 
  failed. This indicates that Rx radio could not be overriden to given 
  LNA state and LNA gain. */ 

  FTM_CAL_SEG_ERR_CFG_RX_NULL_CB          = 13,
  /*!< Error code to indicate that the tech specific configure Rx callback 
  function is NULL. This error code will be seen when the Configure Rx 
  opcode bit is enabled while the CB function is NULL. */ 

  FTM_CAL_SEG_ERR_MEAS_RX_FAILURE         = 14,
  /*!< Error code to indicate that tech specific Rx measurement action 
  failed. */ 

  FTM_CAL_SEG_ERR_MEAS_RX_NULL_CB         = 15,
  /*!< Error code to indicate that the tech specific measure Rx callback 
  function is NULL. This error code will be seen when the measure Rx 
  opcode bit is enabled while the CB function is NULL. */ 

  FTM_CAL_SEG_ERR_TEAR_RADIO_FAILURE      = 16,
  /*!< Error code to indicate Radio tear down failure occured. This indicates 
  the Radio will not be in sleep state at the end of calibration sweep. */

  FTM_CAL_SEG_ERR_TEAR_RADIO_NULL_CB      = 17,
  /*!< Error code to indicate that the tech specific radio callback 
  function is NULL. Radio should always be torn down at the end of the sweep. 
  This this error code will be invoked, if there is not callback funciton 
  register for a tech. */ 

  FTM_CAL_SEG_ERR_TX_MEAS_NUM_MISMATCH    = 18,
  /*!< Error code to indicate that there is a mismatch between the expected 
  number of Tx measurement by cal v3 common framework and the number of Tx 
  measurement sent by tech specific Tx measurement function. */ 

  FTM_CAL_SEG_ERR_RX_MEAS_NUM_MISMATCH    = 19,
  /*!< Error code to indicate that there is a mismatch between the expected 
  number of Rx measurement by cal v3 common framework and the number of Rx 
  measurement sent by tech specific Rx measurement function. */ 

  FTM_CAL_SEG_ERR_TX_MEAS_DATA_OVERFLOW    = 20,
  /*!< Error code to indicate that there is a mismatch Tx measurement data 
  returned by tech specific callback funciton is bigger than the max 
  value #FTM_CALIBRATION_SEG_TX_BUFF_SIZE_MAX. */ 

  FTM_CAL_SEG_ERR_RX_MEAS_DATA_OVERFLOW    = 21,
  /*!< Error code to indicate that there is a mismatch Rx measurement data 
  returned by tech specific callback funciton is bigger than the max 
  value #FTM_CALIBRATION_SEG_RX_BUFF_SIZE_MAX. */ 

  FTM_CAL_SEG_ERR_PUSH_RESULT_FAILURE      = 22,
  /*!< Error code to indicate that RxTx results could not be pushed into 
  the results possibly because of buffer overflow. */
  
  FTM_CAL_SEG_ERR_TIMER_UPDATE             = 23,
  /*!< Error code indicating the Timer installation or update has failed. */

  FTM_CAL_SEG_ERR_XPT_FAILURE              = 24,
  /*!< Error code indicating the XPT operation has failed. More details on 
  the type of error can be found out by looking at the Error Code in XPT
  results buffer */

  FTM_CAL_SEG_ERR_POLLING_FAILURE          = 25,
  /*!< Error code indicating the POLLING operation has failed. */

  FTM_CAL_SEG_ERR_RADIO_SETUP_FAILURE      = 26,
  /*!< Error code indicating the Radio Setup operation has failed. */

  FTM_CAL_SEG_ERR_TX_OVERRIDE_FAILURE      = 27,
  /*!< Error code indicating the Tx override operation has failed. */

  FTM_CAL_SEG_ERR_RX_MEASURE_FAILURE       = 28,
  /*!< Error code indicating the Rx measure operation has failed. */

  FTM_CAL_SEG_ERR_TX_MEASURE_FAILURE       = 29,
  /*!< Error code indicating the Tx measure operation has failed. */

  FTM_CAL_SEG_ERR_DATA_FAILURE             = 30,
  /*!< Error code indicating the Data operation has failed. */

  FTM_CAL_SEG_ERR_MAX_NUM
  /*!< Maximum number of segment error code. This enum should not be 
  used for any purpose other than counting the number of error codes. */

} ftm_calibration_segment_error_code_mask;

/*----------------------------------------------------------------------------*/
/*!< 
  @brief
  Mask to store all error codes are the given segment.
 
  @details
  This is a mask where each bit corresponds to a particular type of error code
  experienced in a segment. The definition of error code for each bit is
  defined in #ftm_calibration_segment_error_code_mask. If this 32-bit mask
  is zero, then this indicates that all the operation in the segment executed
  without any error.
*/
typedef uint64 ftm_calibration_segment_error_type;

/*! \} */

/*============================================================================*/
/*!
  @name Calibration Results

  @brief
  This section contains definitions used for pushing and fetching Calibration
  results.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*! Emumeration indicating the results revision */ 
typedef enum
{

  FTM_CAL_RESULT_REV_0 = 0,
  /*!< Revision 0 */

  FTM_CAL_RESULT_REV_1 = 1
  /*!< Revision 1 */

}ftm_calibration_result_revision_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Type/Size of Calibration Result Descriptor
*/
typedef uint8 ftm_calibration_result_desp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Definition of Instruction Segment Number 
*/
typedef int16 ftm_calibration_segment_num_type;

/*! \} */

/*============================================================================*/
/*!
  @name Calibration Configuration Packet Fields

  @brief
  This section contains definitions of calibration configuration packet fields.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to extract the Header from the Calibration Configuration Packet
*/
typedef PACK(struct)
{
  uint8 start_cal_cmd;
  /*!< Start Calibration Sweep Command. This command will be used to trigger
  the calibration sweep after unpacking the calibraiton configuration data. 
  If the Calibration command is sent without this flag set, the embedded
  driver will just unpack and calibration configuration data and store it. */

  uint8 last_sweep;
  /*!< 
  @brief 
  Flag indicating, if the Calibration configruation packet(CCP) is for the 
  last sweep.
   
  @details 
  This is a flag indicating, if the Calibration configruation packet(CCP) is for 
  the last sweep. Based on this flag, RF tear down will be done. 
   
  If the flag is FALSE, that means more calibration actions will be coming in 
  future. Thus Radio will not be torn down. However, ceasing of ISR and 
  dealloction of the configuration buffer and results buffer will be done for 
  this flag. This puts a requirement on the tools side to send a result fetch 
  command to embedded side before sending another CCP. 
   
  If the flag is TRUE, this indicates that there will no more associated 
  calibration actions. Thus tearing down of RF, ceasring ISR, dellocating 
  buffers will be done. 
   
  The main purpose of this flag is to support those call boxes which does not 
  support multiple PA State power measurement in a single sweep. Thus, if 
  the Tx Power Sweep on different PA states are distributed on various 
  Cal sweep commands, then this flag will be useful not to tear down radio 
  in between different PA state transition. 
  */

  uint8 res_rev_dbg_mode;
  /*!< 8-bit field to store debug mode and results revision data. 
  Bit [0] : Indicates if the debug mode is enabled or not 
  Bit [7:1] : Indicates the results revision */ 

  uint16 num_of_segments;
  /*!< Number of calibration segments/instances passed with this command. Based
  on this number, approripate dynamic memory allocation is done. */

  uint16 tot_instr_size;
  /*!< Total size of all instruction(OpCode+Payload) from all segments. This 
  will be used to perform dynamic memory allocation for configuration parameter 
  storage. */

  uint16 cal_data_len;
  /*!< Expected Length of Calibration Data for this payload */

} ftm_calibration_config_packet_header_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to extract the Header from the Calibration Configuration Packet
*/
typedef PACK(struct)
{
  ftm_calibration_config_packet_header_type packet_header;
  /*!< Calibration Configuration Packet Header */
 
  uint8 instr_ptr;
  /*!< RF Calibration Instruction which include opcode and Payload */

} ftm_calibration_config_packet_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  definition of Instruction Segment OpCode 
*/
typedef uint16 ftm_calibration_opcode_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  definition of Instruction Segment Duration 
*/
typedef uint16 ftm_calibration_segment_duration_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Definition of RF Calibration Instruction Header
*/
typedef PACK(struct)
{

  ftm_calibration_opcode_type opcode;
  /*!< OpCode corresponding to the first segment. This will be the starting 
  point to iteratively extract N number of opcodes and thier respective
  payloads. */

  ftm_calibration_segment_duration_type interval;
  /*! The total duration in microseconds expected to be required to complete the 
  execution of the this instruction. */

} ftm_calibration_instruction_header_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Type/Size of CRC used for Calibration packet validation
*/
typedef uint16 ftm_calibration_packet_crc_type;

/*! \} */

/*============================================================================*/
/*!
  @name Calibration Data

  @brief
  This section contains definitions used to maintain the configuration and 
  Status of Calibration Sweep.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure definition for the Node to store each of the RF Calibration
  request packet. This will be used as a node definiton of a linked list.
*/
struct ftm_calibration_config_node_type
{ 

  uint16 num_of_segment;
  /*!< Total number of calibration segments for this node. */

  void *cal_instruction;
  /*!< RF Calobration configuration parameter for the current request packet.
  The size of this configuration may vary between different nodes. */

  struct ftm_calibration_config_node_type *next_config;
  /*!< Pointer to the next node containing the RF calibration configuration
  for next request packet */

};

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Typedefinition for the Sweep Config Node
*/
typedef struct ftm_calibration_config_node_type ftm_calibration_packet_node_type;

/*----------------------------------------------------------------------------*/
/*!< 
  @brief
  Structure to hold the error log for each segment.
*/
typedef struct
{
  ftm_calibration_segment_num_type first_error_seg_num;
  /*!< Segment number for which the error code is stored */

  ftm_calibration_segment_error_type first_seg_error;
  /*!< 
    @brief
    Mask to store all error codes are the segment.
   
    @details
    This is a mask where each bit corresponds to a particular type of error code
    experienced in a segment. The definition of error code for each bit is
    defined in #ftm_calibration_segment_error_code_mask. If this 32-bit mask
    is zero, then this indicates that all the operation in the segment executed
    without any error.
   
    This error code will be cleared while initializing the calibration sweep
  */

  ftm_calibration_segment_error_type seg_error;
  /*!< Mask to store the segment error for the current sweep */

} ftm_calibration_segment_error_log_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure containing the Calibration configuration Data. This 
  includes data related to linked list where all instruction configuration 
  is stored.
*/
typedef struct
{
  boolean debug_mode_en;
  /*!< Flag indicating the debug mode is enabled for Cal Sweep V3. Debug mode 
  will include profile of segments and send F3 messages. */

  uint64 dbg_level;
  /*!< Mask indicating the F3 debug level for calv3. Definition of each 
  bit is defined in #ftm_cal_debug_level_type */

  boolean tear_down_radio;
  /*!< Flag indicating, if Radio should be torn down */

  ftm_calibration_packet_node_type *head_node;
  /*!< Pointer to the first Node of the linked list containing the RF
  calibration sweep configruaiton. This will be dynamically allocated everytime
  when Calibration Sweep is initiated and will be de-allocated at the end of
  Sweep ISR. This pointer does not change thorugh out the cal process. */

  ftm_calibration_packet_node_type *read_node;
  /*!< Pointer to the current Node Read Pointer. This will be used when
  the segment Calibration Config is read for Calibration ISR. The pointer
  should be set to start node, when the Start Node is craeted. */

  uint16 read_node_seg_counter;
  /*!< Counter to keep track of the number of Segments in the current Read 
  Node. This counter is used while traversing through all the instruction 
  during calibration Sweep. */ 

  uint8 *read_node_payload_ptr;
  /*!< Pointer to traverse through various opcode payload within the Current 
   Segment Number of the current node */

  uint32 instraverse_prev_ptr;
  /*!< Debug variavble to store the previous paylaod poinbter when instruction 
  is traversed */

  ftm_cal_opcode_data_type opcode_data;
  /*!< Structure to hold common OpCode Data */
  
} ftm_calibration_config_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure containing RF Calibration Sweep Results Data.
*/
typedef struct
{
  uint8 result_rev;
  /*!< variable to indicate the result revision */

  void *buff_ptr;
  /*!< Pointer to the memory allocated to store the calibration results data. 
  The size of this memory will correspond to cal_data_rsp_size. This chunk 
  of memeory should be de-callocated once the data the sent back to PC. */

  uint16 buff_size;
  /*!< Variable to store the total size of Calibration Data response packet. 
  Each Packet Node will content the total expected size of the Calibration 
  Data. This information from eac packet node will be added to this variable 
  to compute the total calibbration data size. */ 
  
  uint8 *write_ptr;
  /*!< Pointer to traverse through Result Buffer. This will point to the 
  current location on the Result buffer, where results data is being written.*/

  uint16 buff_written;
  /*!< Variable keeping track of usage of result buffer written. This cannot 
  exceed result_buffer_size */

  uint8 *read_ptr;
  /*!< Pointer to traverse through Result Buffer. This will point to the 
  current location on the Result buffer, where results data is being read.*/
  
  uint16 buff_read;
  /*!< Variable keeping track of usage of result buffer read. */

  boolean buff_read_en;
  /*!<                                    
  @brief 
  Flag indicating if the buffer is currently enabled for Read or not. 
   
  @details
  Flag indicating if the buffer is currently enabled for Read or not. 
  Read will be disabled on the buffer until write is completed. Thus, if PC 
  sends a command to read the buffer before Write is done, then error code 
  will be sent back based on this flag */

  uint8 query_num;
  /*!< Variable to store the number of times result has been queried from 
  Tools side */

} ftm_calibration_result_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure containing RF Calibration Sweep status.
*/
typedef struct
{
  boolean cal_config_init_done;
  /*!< Flag indicating, if the static init is done */

  rfm_mode_enum_type current_rf_mode;
  /*!< Variable keeping track of current RF Mode on which Calibraiton Sweep 
  is being performed */

  uint16 curr_band;
  /*!< Variable keeping track of current band on which Calibraiton Sweep 
  is being performed */

  uint32 curr_chan;
  /*!< Variable keeping track of current channel on which Calibraiton Sweep 
  is being performed */

  boolean is_sweep_active;
  /*!< Flag indicating, if the calibration sweep is active or not. This will 
  be set to 1 through the duration when calibration sweep. This will be be 
  set to 0, whne calibration sweep has ceased. This Flag will be checked 
  at the start of CCP configuration request in order to make sure that new 
  CCP is not processed while calibration sweep is going on. */ 

  boolean pause_to_poll;
  /*!< Flag to indicate that the fetching of new instructions from the 
  linked list is paused in order to perform polling operation for an event 
  compeletion. This variable will be set to TRUE when POLLING OpCode is 
  executing. */ 

  int32 isr_duration_offset;
  /*!< Variable used to store the ISR duration offset to be applied during 
  ISR scheduling. This variable is updated by SYNC OpCode */

  boolean apply_isr_offset;
  /*!< Variable used to signal that a SYNC opcode has been performed and that 
    the segment to follow should apply the offset returned from the handler.
    This variable is updated by SYNC OpCode */

  ftm_calibration_segment_error_log_type seg_error_log;
  /*!< Structure to hold the error log for the first faulty segment. This 
  log will be used to send the status when result is fetched from the Tools 
  side */

} ftm_calibration_status_type;

/*----------------------------------------------------------------------------*/
/*! Structure to store per segment data for results */
typedef struct
{
  uint8 total_result_sz;
  /*!< Total size of results for the segment */

  uint8 total_result_cnt;
  /*!< Total count of opcodes in the segment which generated results */

  ftm_cal_opcode_info_type result_avl;
  /*!< Data structure to list the opcodes which have results available */

} ftm_cal_result_per_segment_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold per segment House Keeping data */
typedef struct
{
  ftm_cal_tear_down_data_type tear_down;
  /*!< Structure to hold radio tear down data */

  ftm_cal_radio_config_data_type radio_config;
  /*!< Structure to hold radio config Data */

  ftm_cal_radio_retune_data_type radio_retune;
  /*!< Structure to hold radio retune Data */

  ftm_cal_tx_waveform_data_type tx_wf;
  /*!< Structure to hold tx waveform config Data */

  ftm_cal_config_tx_data_type config_tx;
  /*!< Structure to hold config Tx Data */

  ftm_cal_meas_hdet_data_type meas_hdet;
  /*!< Structure to hold meas HDET data */
  
  ftm_cal_config_rx_data_type config_rx;
  /*!< Structure to hold config Rx Data */

  ftm_cal_meas_rx_data_type meas_rx;
  /*!< Structure to hold meas Rx data */

  ftm_cal_radio_setup_data_type radio_setup;
  /*!< Structure to hold the radio setup opcode data */

  ftm_cal_tx_override_data_type tx_override;
  /*!< Structure to hold the Tx override opcode data */

  ftm_cal_polling_data_type polling;
  /*!< Structure to hold polling opcode data */

  ftm_cal_data_opcode_data_type data;
  /*!< Structure to hold Data OpCode data */

  ftm_cal_sync_data_type sync;
  /*!< Structure to hold Sync OpCode Data */

  ftm_cal_xpt_data_type xpt;
  /*!< Structure to hold XPT data */

  ftm_cal_rx_measure_data_type rx_measure;
  /*!< Structure to hold Rx Measure opcode data */

  ftm_cal_tx_measure_data_type tx_measure;
  /*!< Structure to hold Tx Measure opcode data */

  ftm_cal_feedback_setup_data_type feedback_setup;
  /*!< Structure to hold Feedback Setup opcode data */

  ftm_cal_enh_int_cal_opcode_data_type enh_int_dev_cal;
  /*!< Structure to hold Enhanced Internal Device Cal opcode data */

} ftm_cal_opcode_payload_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold all information pertaining to current segment */
typedef struct
{
  ftm_calibration_segment_num_type cal_instance;
  /*!< Current Calibration Sweep Instance. Since ISR is fired repeatedly at a 
  given interval, this variable will be used to indentify the ISR number 
  which is being fired now. In other words, this variable defines that 
  calibration step that we are executing now. cal_instance = -1 refers to the 
  "very first" operation after the request packet is recieved from Tool for Cal 
  Sweep. */

  ftm_calibration_segment_duration_type interval;
  /*! The total duration in microseconds expected to be required to complete the 
  execution of the this instruction. */
    
  ftm_cal_opcode_per_segment_data_type opcode_data;
  /*!< Data structure containing opcode data pertaining to the current 
  segment */

  ftm_cal_opcode_payload_type oc_payload;
  /*!< Structure to hold per segment House Keeping data */
  
  ftm_cal_result_per_segment_data_type result_data;
  /*!< Structure to store per segment data for results */
  
} ftm_calibration_curr_segment_info_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure containing RF Calibration Sweep Configuration.
*/
typedef struct ftm_calibration_data_type_t
{
  ftm_calibration_status_type status;
  /*!< Structure containing current RF Calibration Sweep status. */
  
  ftm_calibration_curr_segment_info_type seg_data;
  /*!< Structure to hold all information pertaining to current segment */

  ftm_calibration_config_data_type cal_config;
  /*!< Structure containing the Calibration configuration Data. This 
  includes data related to linked list where all instruction configuration 
  is stored. */ 

  ftm_calibration_result_data_type result;
  /*!< Structure containing RF Calibration Sweep Results Data */

} ftm_calibration_data_type;

/*! \} */

/*============================================================================*/
/*!
  @name Request and Reponse Packets

  @brief
  This section contains definitions of request and response packets used for
  Calibration sweep configuraiton and Result request commands.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM Common Calibration Config sweep Request packet
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_COMMON_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CALIBRATION_SWEEP */

  uint16 req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16 rsp_len;
  /*!< Length of response packet to Diag/PC */

  uint32 cal_param;
  /*!< Caibraiton Configuration Param */

} ftm_cal_sweep_config_req_pkt_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM Common Calibration Config sweep Response packet
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

  int8 cfg_sweep_status;
  /*!< Calibration Sweep Configuration Status */

} ftm_cal_sweep_config_rsp_pkt_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM Common Calibration sweep Get results Request packet
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_COMMON_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CALIBRATION_SWEEP_GET_RESULTS */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

} ftm_cal_sweep_get_results_req_pkt_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM Common Calibration sweep Get results Response packet
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command Identification */

  uint8 sub_sys;
  /*!< Sub-system Identification. For instance - 11 for FTM */

  uint16  mode;
  /*!< RF Mode Identification. For Instance - 0 for FTM_COMMON_C */

  uint16  cdma_cmd;
  /*!< FTM Sub Command. Example-  FTM_CALIBRATION_SWEEP_GET_RESULTS */

  uint16  req_len;
  /*!< Length of Diag Request Packet to the Target */

  uint16  rsp_len;
  /*!< Length of response packet to Diag/PC */

  int8 status;
  /*!< Error/Success Code indicating the status of Get results. This will be 
  used to validate the returned Result packet*/

  uint64 seg_error_code;
  /*!< Error Code for the first faulty segment. If this is zero, then all 
  segemts were executed without any issues. If non-zero error is seen by 
  tools side, then Error message should be thrown during calibration.*/ 

  uint16 faulty_segment_num;
  /*!< If the seg_error_code in non-zero, then this variable indicates 
  the first segment on which the error code was observed. */
  
  uint8 empty_result_buffer;
  /*!< This will be a flag indicating that the resulter buffer is empty. PC 
  side should no longer request for call results */

  uint16  cal_result_len;
  /*!< Length of Calibration results in Bytes for this response packet. */

  uint8 cal_result;
  /*!< Start of Calibration results. The size of this calibration results 
  will be given by cal_result_len */

} ftm_cal_sweep_get_results_rsp_pkt_type;

/*! \} */

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_calibration_configure_sweep
(
  void *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_calibration_configure_sweep_compressed
(
  void *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_calibration_fetch_sweep_results
(
  void *ftm_req_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_deinit_result_buffer
(
  void
);

/*----------------------------------------------------------------------------*/
void
ftm_calibration_set_debug_level
(
  ftm_cal_debug_level_type dbg_lvl,
  boolean active_flag
);

/*----------------------------------------------------------------------------*/
rfm_device_enum_type 
ftm_calibration_dev_flag_to_rfm_dev 
(
   uint32 device_flag
);

#ifdef T_WINNT
#error code not present
#endif

#ifdef __cplusplus
}
#endif

#endif /* FTM_COMMON_CALIBRATION_V3_H */


