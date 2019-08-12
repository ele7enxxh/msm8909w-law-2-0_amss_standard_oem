#ifndef FTM_CALIBRATION_V3_OPCODE_H
#define FTM_CALIBRATION_V3_OPCODE_H
/*!
  @file
  ftm_calibration_v3_opcode.h

  @brief
  This module contains common code to handle OpCode processing
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_calibration_v3_opcode.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/22/14   shk     Added enumeration to define opcode versions
09/18/14   xsh    Fix compiler warning and match new_loc data type to new_location in opcode new loc assign
05/07/14   br      Added new opcode definition for Enhanced Internal device Calibration
10/16/13   aro     LLVM compiler compatibility
04/19/13    dj     Support for Feedback Setup Opcode
04/16/13   aro     Variable/type/prototype cleanup
04/16/13   aro     Moved Opcode Data to HK data structure
04/15/13   aro     Mainline calv3 optimization code/delete obsolete code
04/03/13   aro     Off-target compilation fix
04/02/13   aro     Migrated to inline Math functions
04/01/13   aro     Fixed random buffer size initialization
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
11/30/12   aro     Initial support for Tx measure opcode
11/19/12   aro     Added 0x4000 Rx Measure opcode
11/17/12   aro     Added Dime (on onwards) specific XPT OpCode
11/15/12   aro     Mainlined enabling of variable length OpCode
11/05/12   aro     Updated OpCode fetch function to populate payload pointer
                   for ET opcodes
10/15/12   aro     Interface to query Opcode length
10/22/12   aro     Update OpCode length calculation to be done in terms of
                   number of bytes
10/22/12   aro     Updated interface to detect opcode bit to pass the
                   opcode data from calling function
10/19/12   aro     Interface to check opcode bit
10/18/12   aro     Added opcode data deinit function
10/18/12   aro     Converted size query function to opcode init function
10/18/12   aro     Interface to query the size of opcode from current segment
10/18/12   aro     Initial 

==============================================================================*/

#include "comdef.h"

/*----------------------------------------------------------------------------*/
/*! Forward declaration of OpCode Dispatch Data  */
struct ftm_cal_opcode_dispatch_info_type_t;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration of all valid OpCode used for RF Calibration.
*/
typedef enum
{

  FTM_CAL_OPCODE_CONFIG_RADIO   = 0,
  /*!< Opcode Bit indicating Cal action to configure Radio to RxTx State 
  for given band and channel on given devices. The payload for this 
  operation is documented in #ftm_cal_instr_payload_config_radio_type. */ 

  FTM_CAL_OPCODE_RETUNE_RADIO   = 1,
  /*!< Opcode Bit indicating Cal action to retune Radio to given band and 
  channel on given devices. The payload for this operation is documented in 
  #ftm_cal_instr_payload_retune_radio_type. */

  FTM_CAL_OPCODE_CONFIG_TX_WF  = 2,
  /*!< Opcode Bit indicating Cal action to configure Tx waveform. The payload 
  for this operation is documented in 
  #ftm_cal_instr_payload_config_tx_wf_type.*/

  FTM_CAL_OPCODE_CONFIG_TX     = 3,
  /*!< Opcode Bit indicating Cal action to configure Tx for measurement. This 
  will basically override the TxAGC output parameter. The payload for this 
  operation is documented in #ftm_cal_instr_payload_config_tx_type. */

  FTM_CAL_OPCODE_MEAS_HDET     = 4,
  /*!< Opcode Bit indicating Cal action to measure HDET. This 
  will take HDET measurements. The payload for this operation is documented in 
  #ftm_cal_instr_payload_meas_hdet_type. */

  FTM_CAL_OPCODE_CONFIG_RX     = 5,
  /*!< Opcode Bit indicating Cal action to configure Rx for measurement. This 
  will override PowerMode, LNA state and LNA gain. The payload for this 
  operation is documented in #ftm_cal_instr_payload_config_rx_type. */

  FTM_CAL_OPCODE_MEAS_RX       = 6,
  /*!< Opcode Bit indicating Cal action to measure Rx. This 
  will take measurements to compute the LNA offsets. The payload for this 
  operation is documented in #ftm_cal_instr_payload_meas_rx_type. */

  FTM_CAL_OPCODE_XPT_NI         = 7,
  /*!< Opcode Bit indicating Cal action to XPT operations. This 
  is the top level opcode for all XPT operations. The  
  operation is documented in #ftm_cal_xpt_instr_payload_type. This is 
  supported only in NI. */ 

  FTM_CAL_OPCODE_RADIO_SETUP   = 8,
  /*!< Opcode Bit indicating Cal action to perform radio setup operations. 
  This will move the radio to provided radio state with lowest Tx output 
  power */

  FTM_CAL_OPCODE_TX_OVERRIDE   = 9,
  /*!< Opcode Bit indicating Cal action to perform tx override operation. 
  This will move the override several tx parameter to change the tx 
  output. */ 

  FTM_CAL_OPCODE_POLLING       = 10,
  /*!< Opcode Bit indicating Cal action to perform polling operations.The  
  operation is documented in #ftm_cal_instr_payload_polling_type. */

  FTM_CAL_OPCODE_DATA          = 11,
  /*!< Opcode Bit indicating Cal action to perform Data operation such 
  as data push and data processing. */

  FTM_CAL_OPCODE_SYNC          = 12,
  /*!< Opcode Bit indicating Cal action to perform ISR timeline synchronization 
  for a given event */

  FTM_CAL_OPCODE_XPT           = 13,
  /*!< Opcode Bit indicating Cal action to XPT operations. This 
  is the top level opcode for all XPT operations. The  
  operation is documented in #ftm_cal_xpt_instr_payload_type. */

  FTM_CAL_OPCODE_RX_MEASURE     = 14,
  /*!< Opcode Bit indicating Cal action to Rx measure. The  
  operation is documented in #ftm_cal_rx_meas_instr_payload_type. */

  FTM_CAL_OPCODE_TX_MEASURE     = 15,
  /*!< Opcode Bit indicating Cal action to Tx measure. The  
  operation is documented in #ftm_cal_tx_meas_instr_payload_type. */

  FTM_CAL_OPCODE_FEEDBACK_SETUP = 16,
  /*!< Opcode Bit indicating Cal action to Feedback Setup. The  
  operation is documented in #ftm_cal_tx_meas_instr_payload_type. */

  FTM_CAL_OPCODE_ENH_INTERNAL_DEVICE_CALIBRATION=17,
  /*!< Opcode Bit indicating Cal action to Enhanced Internal Device Calibration.*/

  FTM_CAL_OPCODE_MAX
  /*!< Indicates the maximum available OpCodes */

} ftm_cal_opcode_bit_type;

/*----------------------------------------------------------------------------*/
/*! Forward declaration of Cal v3 data  */
struct ftm_calibration_data_type_t;

/*----------------------------------------------------------------------------*/
/*! Enumeration indicating the the length of opcode. These values are compared 
    against 2 MSBs of the OpCode field
*/ 
typedef enum
{
  OPCODE_INVALID_BYTES = 0,
  /*!< Indicates that the OpCodes has invalid length */

  OPCODE_2_BYTES = 2,
  /*!< Indicates that the OpCodes is 2-bytes long */

  OPCODE_8_BYTES = 8,
  /*!< Indicates that the OpCodes is 8-bytes long */

  OPCODE_16_BYTES = 16,
  /*!< Indicates that the OpCodes is 16-bytes long */

  OPCODE_32_BYTES = 32,
  /*!< Indicates that the OpCodes is 32-bytes long */

} ftm_cal_opcode_length_type;

/*----------------------------------------------------------------------------*/
/*! Enumeration indicating the version of opcode.
*/
typedef enum
{
  OPCODE_VERSION_V0 = 0,
  /*!< Opcode version V0, legacy */

  OPCODE_VERSION_V1 = 1,
  /*!< Opcode version V1 */

  OPCODE_VERSION_VMAX
  /*!< Max allowable versions of any opcode */

} ftm_cal_opcode_version_type;

/*----------------------------------------------------------------------------*/
/*! Struture to store the Opcode for the current segment. Current Maximum size 
    is 256 bits. As per tools packaging, one or more fields in this data
    structure will be populated.
*/
typedef struct
{
  uint64 oc_g3;
  /*!< OpCode group 3 containing OpCodes between bit 255 and bit 192 */

  uint64 oc_g2;
  /*!< OpCode group 2 containing OpCodes between bit 191 and bit 128 */

  uint64 oc_g1;
  /*!< OpCode group 1 containing OpCodes between bit 127 and bit 64 */

  uint64 oc_g0;
  /*!< OpCode group 0 containing OpCodes between bit 63 and bit 0 */

} ftm_cal_opcode_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data structure to keep track of information related to OpCode for the
  most recent segment.
*/
typedef struct
{
  boolean validity;
  /*!< Flag indicating the validity of the OpCode data*/

  ftm_cal_opcode_length_type opcode_size;
  /*!< Size of OpCode (in bytes) sent for current segment */

  ftm_cal_opcode_type opcode;
  /*!< Structure holding the opcode for current segment */

  uint8 opcode_group_mask;
  /*!< Mask to indicate if the given opcode group has atleast one valid 
  active opcode. 
  Bit 0 - OpCode Group 0 
  Bit 1 - OpCode Group 1 
  Bit 2 - OpCode Group 2 
  Bit 3 - OpCode Group 3 
  Bit 7:4 - Reserved */ 

} ftm_cal_opcode_info_type;

/*----------------------------------------------------------------------------*/
/*! Structure containing result buffer parameter */
typedef struct
{
  uint8 *buff_ptr;
  /*!< Pointer to the memory where each opcode should populate the results, 
  if present */

  uint16 buff_sz_max;
  /*!< Maximum size of buffer memory avaliable */

  uint16 buff_sz;
  /*!< Total buffer size populated with results for each OpCode */

} ftm_cal_result_buffer_type;

/*----------------------------------------------------------------------------*/
/*! Function pointer to be called when the opcode payload is to be unpacked 
during each segement. */
typedef uint16 (*ftm_cal_opcode_unpack_fp)(
    struct ftm_calibration_data_type_t *cal_data,
    struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
    void const *instr_payload );

/*----------------------------------------------------------------------------*/
/*! Function pointer to be called when the opcode payload is to be processed 
during each segement. */
typedef boolean (*ftm_cal_opcode_process_fp)(
    struct ftm_calibration_data_type_t *cal_data,
    struct ftm_cal_opcode_dispatch_info_type_t *oc_disp );

/*----------------------------------------------------------------------------*/
/*! Structure to pass opcode registration data */
typedef struct
{

  ftm_cal_opcode_unpack_fp unpack_fp;
  /*!< Function pointer to be called when the opcode payload is to be unpacked 
  during each segement. This will be populated when opcode registration is
  done by each opcode module */

  ftm_cal_opcode_process_fp process_fp;
  /*!< Function pointer to be called when the opcode payload is to be processed 
  during each segement. This will be populated when opcode registration is
  done by each opcode module */

  ftm_cal_result_buffer_type res_buff_info;
  /*!< Structure containing information related to results buffer */

} ftm_cal_opcode_reg_data_type;

/*----------------------------------------------------------------------------*/
/*! Table containing the dispatch data for each opcode */
typedef struct ftm_cal_opcode_dispatch_info_type_t
{
  boolean validity;
  /*!< Flag indicating, if the entry is valid */

  uint16 new_loc;
  /*!< New opcode location in the dispatch table pertaining to the opcode 
  sequence requirement as per #oc_sequence*/

  ftm_cal_opcode_bit_type oc_bit;
  /*!< OpCode bit. This will be populated when opcode registration is
  done by each opcode module */

  ftm_cal_opcode_unpack_fp unpack_fp;
  /*!< Function pointer to be called when the opcode payload is to be unpacked 
  during each segement. This will be populated when opcode registration is
  done by each opcode module */

  ftm_cal_opcode_process_fp process_fp;
  /*!< Function pointer to be called when the opcode payload is to be processed 
  during each segement. This will be populated when opcode registration is
  done by each opcode module */

  uint8 *payload_ptr;
  /*!< Pointer to the memory which contains Opcode payload for the given 
   opcode. This will be populated in each segment */

  ftm_cal_result_buffer_type res_buff_info;
  /*!< Structure containing information related to results buffer */
  
} ftm_cal_opcode_dispatch_info_type;

/*----------------------------------------------------------------------------*/
/*! Structure holding all the segment specific opcode data  */
typedef struct
{
  ftm_cal_opcode_info_type oc_info;
  /*!< Data structure to keep track of information related to opcode pertaining 
  to most recent segment. */
  
  ftm_cal_opcode_info_type oc_info_reseq;
  /*!< Data structure to keep track of information related to opcode pertaining 
  to most recent segment. */

  ftm_cal_opcode_info_type oc_info_remained;
  /*!< Variable to store the remaning OpCode for execution. This would 
  be the same as oc_info_reseq structure at the beginning of the 
  the segment and zero at the end of the segment. */
  
} ftm_cal_opcode_per_segment_data_type;

/*----------------------------------------------------------------------------*/
/*! OpCode Location mapping */
typedef struct
{
  ftm_cal_opcode_bit_type opcode;
  /*!< OpCode whose location is shifted */

  uint8 new_loc;
  /*!< Table containing the mapping of new opcode location in the dispatch 
  table pertaining to the opcode sequence requirement as per #oc_sequence*/
} ftm_cal_opcode_location_map_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold common OpCode Data */
typedef struct
{
  ftm_cal_opcode_location_map_type location_map[FTM_CAL_OPCODE_MAX];
  /*!< Table containing the mapping of new opcode location in the dispatch 
  table pertaining to the opcode sequence requirement as per #oc_sequence*/

  ftm_cal_opcode_dispatch_info_type dispatch_table[FTM_CAL_OPCODE_MAX];
  /*!< Table containing the dispatch data for each opcode */

} ftm_cal_opcode_data_type;

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_init_opcode
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_deinit_opcode
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_register_opcode
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_bit_type opcode_bit,
  ftm_cal_opcode_reg_data_type reg_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_get_opcode
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type **oc_disp
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_unpack_instruction
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_invalidate_opcode
(
  struct ftm_calibration_data_type_t *cal_data
);

#endif /* FTM_CALIBRATION_V3_OPCODE_H */

