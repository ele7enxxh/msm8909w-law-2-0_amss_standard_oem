#ifndef FTM_CALIBRATION_V3_SYNC_H
#define FTM_CALIBRATION_V3_SYNC_H
/*!
  @file
  ftm_calibration_v3_sync.h

  @brief
  This module contains prototypes and definitions used by common RF calibration
  Sync OpCode.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_calibration_v3_sync.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/15/13   cri     Added SYNC opcode support
04/15/13   aro     Mainline calv3 optimization code/delete obsolete code
04/01/13   aro     Fixed random buffer size initialization
04/01/13   aro     Debug flag to switch between old and new calv3 implementation
04/01/13   aro     Added hookup for new calv3 infrastructure
04/01/13   aro     F3 update
11/05/12   aro     Interface to query the offset to be applied during
                   ISR scheduling
11/05/12   aro     Removed instruction payload pointer from process
                   instruction function
11/05/12   aro     Added RFM Mode validity checking
11/05/12   aro     Added process instruction function
11/05/12   aro     Code to unpack sync opcode
11/05/12   aro     Added the sync event ID definitions
11/05/12   aro     Added the sync event handling function pointer definitions
11/05/12   aro     Initial version of Sync OpCode

==============================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Forward declaration of Cal v3 data  */
struct ftm_calibration_data_type_t;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  List of Sync event enumerations
*/
typedef enum
{
  SYNC_EVENT_TX_SYSTEM_BOUNDARY = 0,
  /*!< Event indicating that the synchronization needs to be dobe with Tx 
  System Boundary */


  
  /* ******* All Event enums should go before this ****** */

  SYNC_EVENT_MAX = 65535
  /*!< Enumeration to indicate maximum Sync event */

} ftm_cal_sync_event_id;

/*----------------------------------------------------------------------------*/
/*! Type defintion for Sync events */
typedef uint16 ftm_cal_sync_events;

/*----------------------------------------------------------------------------*/
/*! Payload format and Tx system boundary sync */
typedef PACK(struct)
{

  uint8 device;
  /*!< device on which the sync is to be done. */

} ftm_cal_sync_tx_sys_boundary_payload_type;

/*----------------------------------------------------------------------------*/
/*! Structure to extract the address of the payload associated with the 
sync event. */
typedef struct
{
  uint8 payload_ptr;
  /*!< Variable to extract the address of the payload associated with the 
  sync event. */
} ftm_cal_sync_event_payload_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for Sync Instruction.
*/
typedef PACK(struct)
{
  uint8 version;
  /*!< Version of the OpCode */

  uint8 debug_mode_en;
  /*!< Flag indicating debug mode for sync opcode */

  uint16 flag;
  /*!< 16-bit mask where each bit indicates a specific type of action. */

  ftm_cal_sync_events sync_event;
  /*!< Event for which the sync is to be done */

  ftm_cal_sync_event_payload_type event_payload;
  /*!< Variable to extract the address of the payload associated with the 
  sync event. */
  
} ftm_cal_sync_instr_payload_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure used to hold the unpacked Sync OpCOde parameter used to send
  to tech specific functions
*/
typedef struct
{
  boolean debug_en;
  /*!< Flag indicating debug Mode */

  uint16 flag;
  /*!< 16-bit flag indicating a specific type of action to be perform 
  the given payload. */

  ftm_cal_sync_events sync_event;
  /*!< Event for which the sync is to be done */

  union
  {
    struct
    {
      rfm_device_enum_type device;
      /*!< RFM device for which the Tx system boundary synchronization 
      needs to be done. */
    } tx_sys_boundary;
    /*!< Structure containing the payload for Tx System boundary 
    synchronization event */

  } event_payload;
  /*!< Structure containing the the payload for each even type */

} ftm_cal_sync_parameter_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Tech Function Pointer definition to query the synchronization time offset in
  the units of microseconds.
 
  @details
  This tech specific Function Pointer definition will be used to query the 
  synchronization time offset in the units of microseconds.
 
  @param sync_payload
  Pointer to sync opcode payload
 
  @return
  Time offset in microseconds to be adjusted on ISR
*/
typedef int32 (*ftm_cal_sync_get_offset_fp_type)
(
  ftm_cal_sync_parameter_type const *sync
);

/*----------------------------------------------------------------------------*/
/*! Structure to hold data pertaining to Sync OpCode */
typedef struct
{
  ftm_cal_sync_get_offset_fp_type oc_process_fp[FTM_CALIBRATION_NUM_OF_TECHS];
  /*!< 
  @brief
  Function Pointer definition for Callback function
  */

  ftm_cal_sync_parameter_type up_param;
  /*!< Structure holding all the unpacked payload for sync opcode */

} ftm_cal_sync_data_type;

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_sync_init
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_sync_deinit
(
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
uint16
ftm_calibration_sync_unpack
( 
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_sync_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

/*----------------------------------------------------------------------------*/
int32
ftm_calibration_sync_get_offset
(
  struct ftm_calibration_data_type_t *cal_data,
  rf_time_tick_type start_sweep_tick
);

#ifdef __cplusplus
}
#endif

#endif /* FTM_CALIBRATION_V3_SYNC_H */

