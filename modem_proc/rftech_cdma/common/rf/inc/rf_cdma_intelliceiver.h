#ifndef RF_CDMA_INTELLICEIVER_H
#define RF_CDMA_INTELLICEIVER_H

/*!
  @file
  rf_cdma_intelliceiver.h

  @details
  This file exports the definitions and declartions pertaining to Intelliceiver.
*/

/*==============================================================================

  Copyright (c) 2010 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_intelliceiver.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
03/03/14   JJ      Revert the RxAGC ACQ duration change
03/03/14   JJ      Reduce RxAGC ACQ duration to half
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
04/01/13   spa     Migrated to RF Dispatch for RF-FW synchronization
11/28/12   aki     Added rf_cdma_ic_deinit
09/10/12   aro     SNUM dispatch callback function for IC update response
07/23/12   Saul    CDMA. Now using semaphore APIs from RF COMMON.
06/26/12   cd      - Add PM configuration info to IC data
                   - Use Rx Bw info to get starting power mode
05/14/12   aro     Updated HDR FW messages to use semaphores from SemPool for
                   RF-FW synchronization
05/01/12   zhaow   Reverted rf_cdma_ic_stop() change
04/30/12   hdz     Moved rf_cdma_ic_resume_rx_agc_cb from static to globe
04/27/12   zhaow   Updated rf_cdma_ic_stop() so that device number is directly 
                   passed in 
03/28/12   aro     Merged the ic_update and rx_agc_resume interfaces
03/28/12   aro     Added Current RF State as the parameter for ic calibration
                   to be used for PM override function to check if the state
                   has changed between the time when PM Ocerride command is
                   posted and handled
03/28/12   aro     Documentation update
03/28/12   aro     Added callback function as a parameter of RxAGC resume
                   function
03/28/12   aro     Moved the handling FW response message to FWRSP Task
03/28/12   aro     Added commands to PAUSE, RESUME, and PM OVERRIDE
03/28/12   aro     Updated PM override sequence to have PAUSE and RESUME iC
                   before and after PM override
12/05/11   aro     Added enums for enums for intelliceiver ACQ duration
12/05/11   aro     Added error fatal for the unrecoverable IC Update error
11/22/11   aro     Added interface to disable iC for debug purpose
11/10/11   aro     Updates the type CDMA semaphore type
11/05/11   cd      Added semaphore blocking functionality to IC power mode 
                   override 
10/28/11   aro     Added support to force power mode once after clear pm
                   override is done.
10/26/11   aro     Re-architectured polling implementation to have only two
                   states in sub STM of RUNNING state
10/25/11   aro     Added RF ic Task handler function to complete the wait for
                   iC update message from 1x and HDR FW
10/25/11   aro     Documentation Update
10/25/11   aro     Added new interfaces splitting the polling action into
                   two parts to be handled through RF iC task dispatcher
10/25/11   aro     Added command handlers to handle start polling and complete
                   polling
10/25/11   aro     Implemented enhanced polling function to be compatible with
                   new RF iC Task dispatch
10/25/11   aro     Added sub STM for RUNNING mode
10/20/11   aro     Updated SS Enter and polling payload to be PACK structure
10/20/11   aro     Added RF_TASK handlers to perform SS polling and enter
                   operation
10/19/11   aro     Added new function to perform timer callback for SS polling
10/18/11   aro     Added Payload definition for Enter SS and Polling CB funcs
10/13/11   aro     Added interface to enable and disable various debug mode
07/18/11   aro     Added intelliceiver HDR support
07/14/11   aro     [1] Added debug flag not to throw F3 messages
                   [2] Added debug flag to disable intelliceiver
07/13/11   aro     Updated intelliceiver interfaces not to have device param
07/13/11   aro     Updated "Get Starting PM" and "Get Lowest PM" interface to
                   send iC data
07/13/11   aro     Updated the type for channel variable
07/12/11   aro     Added intelliceiver Steady State Enter implementation
07/06/11   aro     Updated Override PM interface to return status
07/06/11   aro     [1] Updated iC start interface to pass band and chan
                   [2] Added critical section protection for iC iterfaces
07/01/11   aro     Added Clear Power Mode override interface
06/29/11   aro     Added initial support to override Power Mode
06/20/11   aro     Changed function signature of intelliceiver Stop to pass
                   the intelliceiver data and logical device param
06/20/11   aro     Changed function signature of intelliceiver Start to pass
                   the intelliceiver data and logical device param
06/20/11   aro     Changed function signature of intelliceiver init to pass
                   the intelliceiver data and logical device param
06/16/11   aro     Renamed intelliceievr variable
06/15/11   aro     Initial Support : Added intelliceiver data structure
02/17/11   aro     Renamed intelliceiver function
02/01/11   aro     Added function to query Lowest Power Mode
02/01/11   aro     Promoted Power Mode types to RFM
01/31/11   aro     Added Set Power Mode function
01/04/11   sty     Added rf_cdma_intelliceiver_stop()
12/23/10   aro     Added Intelliceiver Start and Query function
12/20/10   aro     Removed including of rfcom.h
12/17/10   aro     Added Intelliceiver Init function
12/14/10   aro     Added Intelliceiver Interfaces
12/08/10   aro     Fixing preprocessor Macros 
12/06/10   aro     Initial Release.

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rfm_device_types.h"
#include "rfm_cdma_power_mode_types.h"
#include "rfm_cdma_band_types.h"
#include "rfc_cdma.h"
#include "rfcommon_locks.h"
#include "rf_cmd_dispatcher.h"
#include "rf_cmd_interface.h"
#include "timer.h"
#include "sys.h"
#include "rf_cdma_ic_task.h"
#include "rf_fwrsp_task.h"
#include "rf_cdma_mdsp.h"
#include "rf_1x_fw_response.h"
#include "rf_hdr_fw_response.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Number of Jammer Status Items  */
#define RF_CDMA_IC_JAMMER_STATUS_NUM 10

/*----------------------------------------------------------------------------*/
/*! 1x RxAGC ACQ duration to be used during intelliceievr update */
#define RF_CDMA_IC_1X_ACQ_DURATION_US 530

/*----------------------------------------------------------------------------*/
/*! 1x RxAGC ACQ duration to be used during intelliceievr update in
Calibration mode*/
#define RF_CDMA_IC_1X_CAL_MODE_ACQ_DURATION_US 100


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enumeration for Intelliceiver State
*/
typedef enum
{
  RF_CDMA_IC_STOPPED,
  /*!< Status indicating that intelliceiver is currently stopped */

  RF_CDMA_IC_STARTING,
  /*!< Status indicating that intelliceiver is currently starting. In this 
  state, continous Jammer detection will not happen */

  RF_CDMA_IC_RUNNING,
  /*!< Status indicating that intelliceiver is currently running. In this 
  state, jammer will be checked continuously */

  RF_CDMA_IC_PAUSE,
  /*!< Status indicating that intelliceiver is currently paused. In this 
  state, all timers for iC will be cleared */

  RF_CDMA_IC_INVALID
  /*!< Invalid intelliceiver state */

} rf_cdma_ic_state_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the intelliceiver steady state enter paylaod */
typedef PACK(struct)
{
  uint32 ic_data_addr;  
  /*!< Address of Intelliceiver data in uint32 */

} rf_cdma_ic_enter_steady_state_payload;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the intelliceiver steady state start polling paylaod */
typedef PACK(struct)
{
  uint32 ic_data_addr;  
  /*!< Address of Intelliceiver data in uint32 */

} rf_cdma_ic_start_polling_payload;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the pause intelliceiver command paylaod */
typedef PACK(struct)
{
  uint32 ic_data_addr;  
  /*!< Address of Intelliceiver data in uint32 */

} rf_cdma_ic_pause_payload;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the resume intelliceiver command paylaod */
typedef PACK(struct)
{
  uint32 ic_data_addr;  
  /*!< Address of Intelliceiver data in uint32 */

} rf_cdma_ic_resume_payload;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the power mode override command payload */
typedef PACK(struct)
{
  uint32 ic_data_addr;  
  /*!< Address of Intelliceiver data in uint32 */

  uint32 logical_addr;  
  /*!< Address of Logical device data in uint32 */

  rfm_cdma_band_class_type band;
  /*!< RF band for which PM override is to be done */

  uint32 num_carriers;
  /*!< Number of carrier */

  rfm_cdma_chan_type channel_array[RFM_CDMA_CARRIER_NUM];
  /*!< List of channels */

  rfm_cdma_power_mode_type power_mode;
  /*!< Power mode to which power mode is to be overriden */

  rf_cdma_state_type current_rf_state_1x;
  /*!< 1x RF state during PM override command post  */ 

  rf_cdma_state_type current_rf_state_hdr;
  /*!< HDR RF state during PM override command post  */ 

} rf_cdma_ic_override_pm_payload;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the power mode override clear command payload */
typedef PACK(struct)
{
  uint32 ic_data_addr;  
  /*!< Address of Intelliceiver data in uint32 */

} rf_cdma_ic_override_pm_clear_payload;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the intelliceiver update wait complete action paylaod 
for 1x response */
typedef PACK(struct)
{
  uint32 ic_data_addr;  
  /*!< Address of Intelliceiver data in uint32 */

} rf_cdma_ic_complete_1x_ic_update_wait_payload;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the intelliceiver update wait complete action paylaod 
for HDR response */
typedef PACK(struct)
{
  uint32 ic_data_addr;  
  /*!< Address of Intelliceiver data in uint32 */

} rf_cdma_ic_complete_hdr_ic_update_wait_payload;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Stucture to hold Jammer state for a band/channel
*/
typedef struct
{
  rfm_cdma_band_class_type band;
  /*!< RF Band for which Jammer State info is stored  */

  rfm_cdma_chan_type channel;
  /*!< RF Channel for which Jammer State info is stored  */

  rfm_cdma_power_mode_type jammer_state;
  /*!< Current Jamme State for given band and chan */

  uint8 high_lin_counter;
  /*!<    
  @brief 
  High Linearity Counter 
   
  @details
  High linearity Counter based on which High Linearity Mode will be 
  decided. If the counter assumes a value greater than zero, then the next 
  linearity will be High Linearity. Once this counter is set to some MAX number, 
  for each Mid or low linearity detected, the number will be decremented by 
  one. */ 

  uint8 mid_lin_counter;
  /*!<                                      
  @brief 
  Mid Linearity Counter 
   
  @details
  Mid linearity Counter based on which Mif Linearity Mode will be 
  decided. If the counter assumes a value greater than zero (after 
  high_lin_counter is zero), then the next linearity will be Mid Linearity. 
  Once this counter is set to some MAX number, for each low linearity detected, 
  the number will be decremented by one. 
   
  If both  high_lin_counter and mid_lin_counter is found to be zero, then the 
  next linearity mode decided will be  Low Lin
  */ 

} rf_cdma_ic_jammer_status_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  structure definition for Jammer Status doubly linked list
 
  @details
  Each Node will hold Jammer Status for a given band and channel information. 
  This has two pointer which are pointer to next and the previous node.
*/
typedef struct rf_cdma_ic_jammer_item
{
  rf_cdma_ic_jammer_status_type jammer_entry;
  /*!< Jammer Status for for a given band and channel combination for this 
  node */

  struct rf_cdma_ic_jammer_item *prev;
  /*!< Pointer to Previous Node in the linked list */

  struct rf_cdma_ic_jammer_item *next;
  /*!< Pointer to Next Node in the linked list */

} rf_cdma_ic_jammer_item_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold all the infomation while moving in or out of the PAUSE
  state. The data in this structure will be invalid, if the iC state is not
  in PAUSE state.
*/
typedef struct
{
  rf_cdma_ic_state_type ic_state_before_pause;
  /*!< Intelliceiver State just before the iC was put to PAUSE state */

} rf_cdma_ic_pause_state_info_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold the list of semaphores used by intelliceiver module to
  perform blocking/signalling operation
*/
typedef struct
{

  rfcommon_semaphore_token_type pause_complete;
  /*!< Semaphore to block on iC PAUSE completion when the PAUSE state is 
  requested */

  rfcommon_semaphore_token_type resume_complete;
  /*!< Semaphore to block on iC RESUME completion when the PAUSE state is 
  requested to be cleared */

  rfcommon_semaphore_token_type pm_ovr_complete;
  /*!< Semaphore to block on iC power mode override completion */

  rfcommon_semaphore_token_type clear_pm_ovr_complete;
  /*!< Semaphore to block on iC clear power mode override completion */

} rf_cdma_ic_semaphores_types;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure containing the intelliceiver debug data
*/
typedef struct
{
  boolean ic_disable;
  /*!< Flag to disable intelliceiver operation completely for debug purpose. 
  When this flag is set to TRUE while intelliceiver is still running, then this 
  flag will take effect only after iC is stopped. Therefore, make sure to 
  stop iC after setting this flag. */

  boolean f3_en;
  /*!< Flag to be used to send F3 debug messages. For normal Operation, this 
  should be disabled to reduce USB overhead. Error messages are not protected 
  by this flag. For debugging purpose, this flag should be set to true using a 
  debugger*/

} rf_cdma_ic_debug_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure containing the intelliceiver data
*/
typedef struct
{
  rfm_device_enum_type device;
  /*!< RFM device to which the calibration data is associated with */

  boolean init_done;
  /*!< Flag indicating if intelliceiver initialization is already done */

  boolean ic_supported;
  /*!< Flag indicating if intelliceiver is supported for this device */

  rf_cdma_ic_state_type ic_state;
  /*!< Current Intelliceiver State */

  rfm_cdma_power_mode_type power_mode;
  /*!< Current Power Mode */
  
  rfdevice_cdma_ic_power_mode_config_type pm_config;
  /*!< Current Power Mode Configuration */

  boolean power_mode_override;
  /*!< Flag indicating if the power mode is in Override mode */

  boolean force_pm_update;
  /*!<                             
  @brief
  Flag indicating iC software to force power mode. 
   
  @details 
  When pm override is cleared, then iC software will continue to put RTR in last 
  override pm value until Jammer status change is detected. Thus to make sure 
  that RTR is in right PM, this flag is set when PM override is cleared. 
   
  Only one PM update is done when this flag is set. Thus as soon as one PM 
  update is done, this flag will be set to FALSE. 
  */ 

  rf_cdma_ic_pause_state_info_type pause_info;
  /*!< Structure to hold all the infomation while moving in or out of the PAUSE
  state. The data in this structure will be invalid, if the iC state is not
  in PAUSE state. */

  rf_cdma_ic_jammer_item_type jammer_items[RF_CDMA_IC_JAMMER_STATUS_NUM];
  /*!< Jammer status items where each entry will correspond to Jammer status 
  for a given band and channel combination */

  rf_cdma_ic_jammer_item_type jammer_sentinel_spare;
  /*!< 
  @brief 
  Sentinel for Spare Jammer status items 
   
  @details 
  This list contains unallocated Jammer status items. Based on Jammer detection, 
  jammer status from this list will be removed and put in jammer_sentinel_used 
  list. 
  */

  rf_cdma_ic_jammer_item_type jammer_sentinel_used;
  /*!< 
  @brief 
  Sentinel for Used Jammer status items 
   
  @details 
  This list contains Jammer items which are used atleast once. Based on Jammer 
  detection, jammer status from jammer_sentinel_spare list will be removed and 
  put in this list. The Jammer status items in jammer_sentinel_used are 
  maintained in time order.  The first item in the list is the most recent 
  (current) Jammer status item being used
  */

  rf_cdma_ic_jammer_item_type *curr_jammer_item;
  /*!< Pointer to the current Jammer Item */

  timer_type ic_timer;
  /*!< Timer handle used to perform intelliceiver steady state and polling 
  operation. */

  timer_group_type ic_timer_group;
  /*!< Timer handle group used to perform intelliceiver steady state and polling 
  operation. */

  rf_lock_data_type crit_section;
  /*!< Global Mutex for CDMA intelliceiver Driver */

  rf_cdma_ic_semaphores_types ic_sems;
  /*!< Structure to hold the list of semaphores used by intelliceiver module to
  perform blocking/signalling operation */

  rf_cdma_ic_debug_data_type dbg_data;
  /*!< Structure containing the intelliceiver debug data */

} rf_cdma_ic_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold the Intelliceiver data passed with the callback function
*/
typedef struct
{
  rf_cdma_ic_data_type *ic_data;
  /*!< Pointer to the Intelliceiver data for the device */  

  boolean is_blocking_call;
 /*!< Indicate if the current call is a block call or not. */

} rf_cdma_ic_resume_rx_agc_cb_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold the Intelliceiver data passed with the callback function
  for 1x
*/
typedef struct
{
  rf_cdma_ic_data_type *ic_data;
  /*!< Pointer to the Intelliceiver data for the device */  

  boolean is_blocking_call;
 /*!< Indicate if the current call is a block call or not. */

  rf_1x_ic_update_rsp_data_type *fw_rsp_hk;
  /*!< Pointer to FW response housekeeping data for IC Update Response */

} rf_cdma_ic_resume_onex_rx_agc_cb_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold the Intelliceiver data passed with the callback function
  for HDR
*/
typedef struct
{
  rf_cdma_ic_data_type *ic_data;
  /*!< Pointer to the Intelliceiver data for the device */  

  boolean is_blocking_call;
 /*!< Indicate if the current call is a block call or not. */

  rf_hdr_ic_update_rsp_data_type *fw_rsp_hk;
  /*!< Pointer to FW response housekeeping data for IC Update Response */

} rf_cdma_ic_resume_hdr_rx_agc_cb_data_type;

/*----------------------------------------------------------------------------*/
void
rf_cdma_ic_handle_enter_steady_state
(
  rf_cdma_ic_cmd_id_type ic_cmd, 
  rf_cdma_ic_cmd_payload_size_type cmd_payload_size, 
  rf_cdma_ic_cmd_type* cmd_ptr
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_ic_handle_start_polling
(
  rf_cdma_ic_cmd_id_type ic_cmd, 
  rf_cdma_ic_cmd_payload_size_type cmd_payload_size, 
  rf_cdma_ic_cmd_type* cmd_ptr
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_ic_handle_pause
(
  rf_cdma_ic_cmd_id_type ic_cmd, 
  rf_cdma_ic_cmd_payload_size_type cmd_payload_size, 
  rf_cdma_ic_cmd_type* cmd_ptr
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_ic_handle_resume
(
  rf_cdma_ic_cmd_id_type ic_cmd, 
  rf_cdma_ic_cmd_payload_size_type cmd_payload_size, 
  rf_cdma_ic_cmd_type* cmd_ptr
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_ic_handle_pm_override
(
  rf_cdma_ic_cmd_id_type ic_cmd, 
  rf_cdma_ic_cmd_payload_size_type cmd_payload_size, 
  rf_cdma_ic_cmd_type* cmd_ptr
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_ic_handle_pm_override_clear
(
  rf_cdma_ic_cmd_id_type ic_cmd, 
  rf_cdma_ic_cmd_payload_size_type cmd_payload_size, 
  rf_cdma_ic_cmd_type* cmd_ptr
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_ic_update_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_ic_handle_onex_ic_update
(
  rf_fwrsp_cmd_type* req_ptr
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_ic_handle_hdr_ic_update
(
  rf_fwrsp_cmd_type* req_ptr
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_ic_init
(
  rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  rf_cdma_ic_data_type *ic_data
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_ic_deinit
(
  rf_cdma_ic_data_type *ic_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_ic_start
(
  rf_cdma_ic_data_type *ic_data,
  const rfc_cdma_logical_device_params_type *logical_dev,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type channel
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_ic_stop
(
  rf_cdma_ic_data_type *ic_data,
  const rfc_cdma_logical_device_params_type *logical_dev
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_ic_pause
(
  rf_cdma_ic_data_type *ic_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_ic_resume
(
  rf_cdma_ic_data_type *ic_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_ic_power_mode_override
(
  rf_cdma_ic_data_type *ic_data,
  const rfc_cdma_logical_device_params_type *logical_dev,
  uint32 num_carriers,
  rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type * const channel_array,
  rf_cdma_state_type curr_rf_state_1x,
  rf_cdma_state_type curr_rf_state_hdr,
  rfm_cdma_power_mode_type power_mode
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_ic_clear_power_mode_override
(
  rf_cdma_ic_data_type *ic_data
);

/*----------------------------------------------------------------------------*/
rfm_cdma_power_mode_type
rf_cdma_ic_get_current_power_mode
(
  rf_cdma_ic_data_type *ic_data
);

/*----------------------------------------------------------------------------*/
rfm_cdma_power_mode_type
rf_cdma_ic_get_starting_power_mode
(
  rf_cdma_ic_data_type *ic_data,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type channel,
  uint32 rx_bw_khz
);

/*----------------------------------------------------------------------------*/
rfm_cdma_power_mode_type
rf_cdma_ic_get_lowest_power_mode
(
  rf_cdma_ic_data_type *ic_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_ic_debug_configure_f3
(
  rf_cdma_ic_data_type *ic_data,
  boolean debug_en
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_ic_debug_configure_ic
(
  rf_cdma_ic_data_type *ic_data,
  boolean disable_ic
);

/*----------------------------------------------------------------------------*/
void 
rf_cdma_ic_resume_onex_rx_agc_cb
(
  rf_cdma_mdsp_cb_event_type mdsp_event,
  void* data
);

/*----------------------------------------------------------------------------*/
void 
rf_cdma_ic_resume_hdr_rx_agc_cb
(
  rf_cdma_mdsp_cb_event_type mdsp_event,
  void* data
);
#endif /* FEATURE_CDMA1X */

#ifdef __cplusplus
}
#endif

#endif  /* RF_CDMA_INTELLICEIVER_H */

