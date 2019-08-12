#ifndef FTM_CALIBRATION_V3_POLLING_H
#define FTM_CALIBRATION_V3_POLLING_H
/*!
  @file
  ftm_calibration_v3_polling.h

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_calibration_v3_polling.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/15/13   aro     Mainline calv3 optimization code/delete obsolete code
04/01/13   aro     Fixed random buffer size initialization
04/01/13   aro     Debug flag to switch between old and new calv3 implementation
04/01/13   aro     Added hookup for new calv3 infrastructure
04/01/13   aro     F3 update
12/06/12   aki     Added RX threshold aka RX wait functionality
11/21/12   aro     Fixed polling timeout
11/05/12   aro     Renamed polling event enum
08/31/12   Saul    CDMA. Added support for EPT_PROCESSING_DONE FW message
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
07/25/12   aro     Added payload for each event
07/25/12   aro     Added debug_mode_en in polling OpCode payload
07/20/12   aro     Updated polling payload
07/19/12   aro     Added polling counter + F3 message update
07/19/12   aro     Added generic call to event callback function
07/19/12   aro     Implementation to process polling opcode
07/19/12   aro     Function to deinitialize polling data structure
07/19/12   aro     Implemented polling event registration
07/19/12   aro     Enumeration for Polling Events
07/19/12   aro     Added polling parameter initialization
07/19/12   aro     Type name updates
07/18/12   aro     Added structures to hold polling segment data 
07/18/12   aro     Initial version of Polling segment code

==============================================================================*/

#include "comdef.h"
#include "rfcommon_time_profile.h"
#include "rfm_mode_types.h"
#include "ftm_calibration_v3_opcode.h"

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
  List of Polling event enumerations
*/
typedef enum
{
  POLLING_EVENT_EPT_EXPANSION_DONE = 0,
  /*!< Event indicating that the EPT Expansion is complete */

  POLLING_EVENT_RXAGC_THRES_REACHED = 1,
  /*!< Event indicating that the RxAGC threshold has reached */

  POLLING_EVENT_EPT_PROCESSING_DONE = 2,
  /*!< Event indicating that the EPT Processing is complete */



  /* ******* All Event enums should go before this ****** */

  POLLING_EVENT_MAX = 64
  /*!< Enumeration to indicate maximum Polling event */

} ftm_cal_polling_event_id;

/*----------------------------------------------------------------------------*/
/*! Enumeration to indicate the infinite polling */
#define FTM_CAL_POLLING_INFINITE 0xFFFFFFFF

/*----------------------------------------------------------------------------*/
/*! Type defintion for polling events */
typedef uint64 ftm_cal_polling_events;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold the Event 0 (EPT Expansion #EVENT_EPT_EXPANSION_DONE)
  Payload. 
*/
typedef PACK(struct)
{
  uint8 primary_device;
  /*!< Primary Device for which EPT expansion event is to be checked */

  uint8 feedback_device;
  /*!< Feedback Device for which EPT expansion event is to be checked */
  
} ftm_cal_polling_event_0_payload_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Direction of signal for RX Threshold polling
  */
typedef enum 
{
  SIGNAL_RISING  = 0,
    /*!< Indicates that a rising signal is expected */

  SIGNAL_FALLING = 1,
    /*!< Indicates that a falling signal is expected */

} ftm_cal_polling_event_1_direction;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold the Event 1 (Event indicating that RxAGC has reached certain
  threshold)
  Payload. 
*/
typedef PACK(struct)
{
  uint8 primary_device;
  /*!< Primary Device from which RX AGC is read*/

  int8 direction;
  /*!< Trigger for rising signal level = 0, trigger falling = 1 */

  int16 threshold;
  /*!< RX Threshold as dBm16 for GSM and dBm10 for other technologies */
  
} ftm_cal_polling_event_1_payload_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for Polling Instruction.
*/
typedef PACK(struct)
{
  uint8 version;
  /*!< Version of the OpCode */

  uint8 debug_mode_en;
  /*!< Flag indicating debug mode for polling opcode */

  uint16 flag;
  /*!< 16-bit mask where each bit indicates a specific type of action. */

  uint32 timeout_us;
  /*!< Polling Timeout time in microseconds. If the event does not occur after 
  this time, then polling will be stopped with error code.  
  FTM_CAL_POLLING_INFINITE = Infinite microseconds */

  ftm_cal_polling_events polling_event;
  /*!< Event for which the infinite polling is to be done. Each bit will 
  represent a polling event. */
  
} ftm_cal_polling_instr_payload_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Function Pointer definition for Callback function to initialize the event
  polling callbacks
 
  @details
  This function will call event callback function to initialize the event
  polling callbacks
 
  @retval TRUE
  Indicates that initialization is successful
 
  @retval FALSE
  Indicates that initialization is not successful
*/
typedef boolean (*ftm_cal_polling_event_init_cb_type)
(
  void
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Function Pointer definition for Callback function to check the status of event 
  completion
 
  @details
  This function will call event specific callback function to check, if the
  event has occured or not.
 
  @param polling
  Pointer to the data containing payload for Polling OpCode
 
  @retval TRUE
  Indicates that the event has occurred allowing the ending of polling segment
 
  @retval FALSE
  Indicates that the event has not yet occurred; so next polling is still needed
  to check the completion of event.
*/
typedef boolean (*ftm_cal_polling_event_cb_type)
(
  ftm_cal_polling_instr_payload_type const *polling,
  void const *event_payload
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to hold the list of event IDs and their corresponding callback
  functions
*/
typedef struct
{
  ftm_cal_polling_event_id event_id;
  /*!< Enumeration for Event ID */

  ftm_cal_polling_event_cb_type event_cb;
  /*!< Callback function to be called for event_id to check, if the event 
  has occured or not */

} ftm_cal_polling_event_list;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to hold data to to handle Polling OpCode
*/
typedef struct
{
  ftm_cal_polling_event_init_cb_type polling_init_cb;
  /*!< Function Pointer for Callback function to initialize the event
  polling callbacks */

  void *event_check_cb;
  /*!< Pointer to the dynamically allocated memory which holds the event_check 
  callback functions for each event. The memory for this will be dynamically 
  allocated during initialization */ 

  ftm_cal_polling_event_id max_event_id; 
  /*!< Variable to store the maximum Event ID which is dynamically 
  registered for the polling opcode. */

  ftm_cal_polling_events event_to_be_done;
  /*!< Mask indicating the events remained to be completed before ceasing 
  the polling segment. For instance - 1 in bit[5] and 1 in bit[13], indicates 
  that the event corresponding to bit[5] and bit[13] still need to be 
  completed */

  rf_time_tick_type start_time;
  /*!< Variable to keep track of start time for polling */ 

  rf_time_type elapsed_duration_us;
  /*!< Variable to keep track of elapsed duration while doing polling. 
  Polling exit criteria will be 
  - elapsed_duration_us > timeout_duration_us, 
    for timeout_duration_us != FTM_CAL_POLLING_INFINITE
  */ 

  uint16 polling_count;
  /*!< Counter to count the number of polling segments till now in order to 
  wait for event completion */

} ftm_cal_polling_opcode_data;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to hold data to to handle Polling OpCode
*/
typedef struct
{
  ftm_cal_polling_opcode_data oc_data[FTM_CALIBRATION_NUM_OF_TECHS];
  /*!< Tech specific data for polling opcode */

} ftm_cal_polling_data_type;

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_polling_register_event
(
  rfm_mode_enum_type rf_mode,
  ftm_cal_polling_event_id event_id,
  ftm_cal_polling_event_cb_type event_check_cb
);

/*----------------------------------------------------------------------------*/
boolean 
ftm_calibration_polling_init
( 
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
boolean 
ftm_calibration_polling_deinit
( 
  struct ftm_calibration_data_type_t *cal_data
);

/*----------------------------------------------------------------------------*/
uint16
ftm_calibration_polling_unpack
( 
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
);

/*----------------------------------------------------------------------------*/
boolean
ftm_calibration_polling_process
( 
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
);

#ifdef T_WINNT
#error code not present
#endif

#ifdef __cplusplus
}
#endif

#endif /* FTM_CALIBRATION_V3_POLLING_H */


