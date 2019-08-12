/*! 
  @file
  rf_cdma_intelliceiver.c
 
  @brief
  This file includes all functions pertaining to CDMA Intelliceiver Operation
 
  @addtogroup RF_CDMA_COMMON_INTELLICEIVER

  @image html rf_cdma_intelliceiver_stm.jpg  
  @{
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_intelliceiver.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/06/14   spa     Change rx_nv_path to be per band per logical device
04/11/14   spa     Jammer detect algorithm updated
03/11/14   spa     Add NULL pointer check in rf_cdma_ic_init
04/05/13   bm      Corrected the lowest power mode during RF calibration for 
                   ADAPTIVE_PM_WB_AGC based devices
08/27/13   cd      Fixed compiler warning
08/27/13   cd      Add missing clean-up of Rx AGC LNA SM events after configure
08/23/13   spa     Skip JD operations if Iceiver is already stopepd :ic_stop API
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
08/07/13   spa     Add check for IC_DATA NULL pointer in IC_STOP API
07/25/13   spa     Check for HDR mode before any JD Algo related operations
07/17/13   spa     Check for return value before updating next IC state
07/15/13   zhw     Remove dev_status_w in mc layer call flow
07/15/13   JJ      Change F3 message from HIGH to ERROR
07/10/13   spa     Reduce F3 levels and remove duplicate calls (RF-FWRSP)
07/08/13   spa     Free semaphore if message sending fails
07/05/13   zhw/sty Clean up rf_cdma_data status mutable pointers
06/24/13   sty     Deleted redundant device call in 
                   rf_cdma_ic_get_starting_power_mode()
06/07/13   spa     Added band as arg to jammer detect thresh API 
05/30/13   spa     Mainlined "use_new_fwrsp", use new RF dispatch implementation 
05/28/13   JJ      Clean un-necessary cdma sys_band to rf_band_type conversion
04/30/13   APU     In rf_cdma_ic_power_mode_override() check for if the device 
                   is in the RX state.
04/13/13   spa     Added is_irat_mode flag to configure_rx_agc
04/01/13   spa     Migrated to RF Dispatch for RF-FW synchronization
03/27/13   ry      Corrected the rfm_mode passed to get_wbagc_jd_status()
03/27/13   ry      Added support for WB AGC Jammer detection
03/19/13   APU     Replaced memcpy with memscpy.
01/09/13   sty     Use RFCOMMON_MDSP_SNUM_TECH instead of RF_CDMA_MDSP_SNUM_TECH
01/04/13   sty     KW fix to use correct band type
12/27/12   shb     Updates for different physical rx_device per band
12/04/12   aro     Added HDR support for RF-FW synchronization
11/28/12   aki     Added rf_cdma_ic_deinit
11/05/12   sty     compile fix
11/05/12   sty     Enabled I-ceiver
07/23/12   Saul    CDMA. Now using semaphore APIs from RF COMMON.
09/28/12   aro     Migrated 1x MC to use new RF-FW Sync
09/10/12   aro     SNUM dispatch callback function for IC update response
09/05/12   aro     Added RF-FW Sync callback parameter in HDR MDSP Interface
09/05/12   aro     Added RF-FW Sync callback parameter in MDSP Interface
07/19/12   sbo     Put Back changes made by CL2585079
07/19/12   sbo     Back off CL2585079 for Emergency PLF11.
07/18/12   sbo     Initialize intelliceiver data members
07/09/12   aak     Move Quiet mode API from device to RFC 
06/28/12   cd      Fixed hysteresis counters to default to 0 and not -1 
06/26/12   cd      - Added functionality to query power mode configuration from 
                   device during get_starting_power_mode()
                   - Update IC data with starting power mode once decision is 
                   made
06/06/12   aro     Sequence number implementation for FW messages
06/06/12   aro     Handling of IRAT_RX_START_RSP
05/14/12   aro     Updated HDR FW messages to use semaphores from SemPool for
                   RF-FW synchronization
05/10/12   zw      Renamed rtr_handle to script_index 
05/08/12   aro     Initial support for semaphore POOL
05/08/12   aro     Renamed function to sem_create and senm_destroy
05/02/12   aro     Support for unique sequence number generation to be used for
                   RF-FW message synchronization
05/03/12   hdz     Added ic_cb_data.is_blocking_call  = TRUE for 1x and HDR mode
05/01/12   zhaow   Reverted rf_cdma_ic_stop() change
04/27/12   zhaow   Updated rf_cdma_ic_stop() so that device number is directly 
                   passed in 
04/19/12   cd      Fixed KW errors
04/03/12   aro     callback function for HDR MDSP Rx interfaces
04/02/12   aro     Added CB as argument in Rx / Tx MDSP interfaces
03/29/12   shb     Added rf_state arg to rf_1x/hdr_configure_rx_agc as it is 
                   needed by device lna phase offset query api 
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
03/22/12   aro     Migrated handling of HDR FW responses to RF_FWRSP task
03/08/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan  
03/02/12   aro     Mainlined Enhanced Rx Calibration
03/02/12   Saul    MCDO. Rx AGC freq comp and device droop compensation framework.
02/28/12   aro     Implemented 1x/HDR mode based quiet mode voting
02/27/12   aro     Added code to perform jammer reset after PLL is tuned. This
                   is required to prevent false trigger of jammer detector.
12/29/11   aro     Updated configure RxAGC interface to have RxLM buffer
12/13/11   aro     Semaphore wait to receive ACQ ACQ to TRACK indicator from FW
12/06/11   aro     Added Error Fatal for the unrecoverable situation when RxAGC 
                   is stopped and IC_UPDATE could not done because of failure 
                   in RX_AGC_CFG
12/05/11   aro     Added calibration mode specific AGC ACQ gain value to
                   reduce calibration time as AGC will be re-acquired later to
                   make sure that AGC  is settled before RxAGC sampling is done
12/05/11   aro     Added enums for enums for intelliceiver ACQ duration
12/05/11   aro     Added error fatal for the unrecoverable IC Update error
12/02/11   aro     Added NULL pointer check
12/02/11   aro     Added check to make sure that Cal data is not applied for
                   device which does not support iC
12/02/11   shb     Moved IC cal data applcation in 
                   rf_cdma_ic_get_starting_power_mode() to ensure cal data is 
                   applied during first tune
11/30/11   sty     Added new arg while invoking rf_hdr_configure_rx_agc()
11/22/11   aro     Added interface to disable iC for debug purpose
11/17/11   aro     Added CDMA quiet mode support
11/14/11   aro     Moved functions to appropriate doxygen block
11/14/11   aro     Renamed polling function
11/14/11   aro     Mainlined iC task debug flag
11/11/11   aro     F3 message Update
11/11/11   aro     Added F3 message to indicate the start of Sem Wait
11/06/11   aro     Fixed the bug where Sem Init was causing IC Init function
                   to fail
11/05/11   cd      Added semaphore blocking functionality to IC power mode 
                   override 
11/04/11   aro     Fixed the AGC acquisition for iC
10/28/11   aro     Added support to force power mode once after clear pm
                   override is done.
10/28/11   aro     Updated PM override function to be compatible with new
                   RF iC Task
10/28/11   aro     Deleted enable_ic_timer debug flag
10/28/11   aro     Klocwork error fix
10/27/11   aro     Added call flow diagram
10/26/11   aro     Re-architectured polling implementation to have only two
                   states in sub STM of RUNNING state
10/25/11   aro     Added implementation of funcitons to complete the wait for
                   iC update message from 1x and HDR FW
10/25/11   aro     Added RF ic Task handler function to complete the wait for
                   iC update message from 1x and HDR FW
10/25/11   aro     Added housekeeping variable to keep track of 1x and HDR
                   active device to be used during handling of iC Update rsp
10/25/11   aro     Added functions to schedule commands to enter SS, start
                   polling, and complete polling actions
10/25/11   aro     Added sub STM for RUNNING mode
10/25/11   aro     Documentation Update
10/25/11   aro     Added new interfaces splitting the polling action into
                   two parts to be handled through RF iC task dispatcher
10/25/11   aro     Added command handlers to handle start polling and complete
                   polling
10/25/11   aro     Implemented enhanced polling function to be compatible with
                   new RF iC Task dispatch
10/20/11   aro     Updated SS polling and enter schedule functions with error
                   checking and proper payload update
10/20/11   aro     Added RF_TASK handlers to perform SS polling and enter
                   operation
10/19/11   aro     Added new function to perform timer callback for SS polling
10/18/11   aro     Added new function to perform timer callback for Enter SS
10/13/11   aro     Klocwork fix
10/13/11   aro     Added interface to enable and disable various debug mode
10/06/11   aro     Added code to acquire critical section at the start of all
                   callback functions.
09/25/11   aro     Removed acquire and release of iC specific critical section
                   causing deadlock condition
09/22/11   aro     Added fix for the issue where RTR is forced to HIGH_LIN at
                   first wakeup_rx until HIGH_LIN down counter is zero. This was
                   caused because for all new band and chan configuration (which
                   has LOW_LIN as starting power), HIGH_LIN down counter is
                   set to MAX. Fix is to set to 0.
09/22/11   aro     Updated Documentation
09/22/11   aro     Updated F3 message for debugging purpose
09/20/11   aro     Fixed wrong F3 message level thrown in calibration mode 
                   in Stop IC function
09/20/11   aro     Added code to acquire CDMA common lock before doing polling.
                   This is required in order to prevent unwanted iC updates when
                   tune/retune is happening
09/20/11   aro     Added check of iC state before setting power mode. Since
                   polling and iC state change happen in two different task
                   contexts, we need to make sure that when we do Power mode
                   update, iC is in RUNNING state.
09/19/11   Saul    IRAT update. Use 1x Rx AGC Acq Duration of 2 ms for IRAT.
09/07/11   aro     Implemented new intelliceiver Power mode update sequence
08/24/11   sty     KW Fixes
08/12/11   aro     Moved set linearity function to be done after RX_STOP
08/11/11   aro     Removed intelliceiver enable flag
08/11/11   aro     Removed early return from rf_cdma_ic_stop() causing crash
                   during Cal v3 calibration
08/11/11   aro     Updated HDR Set power mode implementation
08/04/11   aro     Fixed the PM osicalltaion issue when switching from HL to LL
                   in LNA gain 0 by addting RX_STOP and RX_START around 
                   RX_AGC_CFG
08/04/11   aro     F3 message Update
07/30/11   aro     Removed old NV access from iC calibration function
07/25/11   cd      Null pointer handling for NV get API
07/18/11   aro     Added intelliceiver HDR support
07/18/11   aro     Updated RxAGC Config interface to pass AGC Mode Param
07/15/11   aro     F3 message update
07/14/11   aro     [1] Added debug flag not to throw F3 messages
                   [2] Added debug flag to disable intelliceiver
07/14/11   aro     [1] Added check not to update RTR with new power mode for
                   override mode
                   [2] Added redefinition of timer CB to steady state CB when
                   intelliceiver is restarted after stopping.
                   [3] Removed redundant iC state check in iC STOP func
                   [4] Added support to clear and re-define the timer when
                   power mode override is called in the middle of RUNNING or
                   STARTING mode.
                   [5] Documentation Update
                   [6] F3 message update
07/13/11   aro     [1] Changed F3 message levels
                   [2] Added debug code to disable timer
07/13/11   aro     [1] Added clear timer in intelliceive stop
                   [2] Updated F3 messages
07/13/11   aro     Documentation Update for polling function
07/13/11   aro     Updated intelliceiver interfaces not to have device param
07/13/11   aro     Updated "Get Starting PM" and "Get Lowest PM" interfaces to
                   remove hardcoding of Power Modes
07/13/11   aro     Added function to update the current jammer item based on
                   current band and channel. 
07/12/11   aro     [1] Implemented intelliceiver polling
                   [2] DOxygen grouping update
07/12/11   aro     Compiler Warning
07/12/11   aro     Compiler Warning
07/12/11   aro     [1] Added intelliceiver Steady State Enter implementation
                   [2] Created common "Set Power Mode" function
07/11/11   aro     Documentation Update
07/06/11   aro     [1] Updated Override PM interface to return status
                   [2] Added code to configure RxAGC during override PM
                   [3] Added code to have zero iC Cal Data for Cal Mode
07/06/11   aro     [1] Updated iC start interface to pass band and chan
                   [2] Added critical section protection for iC iterfaces
07/05/11   aro     Moved the check of iC support to MC
07/01/11   aro     Changed typename to rfdevice_cdma_ic_thres_param_type
07/01/11   aro     [1] Added Clear Power Mode override interface
                   [2] Added documentation for Power Mode override function
06/30/11   aro     Compiler Warning Fix
06/30/11   aro     Updated iC Cal Data structrue to hold the difference on G0
                   gain cal value between current channel and cal ref chan
06/29/11   aro     [1] Added initial support to override Power Mode
                   [2] Renamed intelliceiver function name
06/20/11   aro     Added check not to start IC in calibration Mode
06/20/11   aro     Changed function signature of intelliceiver Stop to pass
                   the intelliceiver data and logical device param
06/20/11   aro     Changed function signature of intelliceiver Start to pass
                   the intelliceiver data and logical device param
06/20/11   aro     Changed function signature of intelliceiver init to pass
                   the intelliceiver data and logical device param
06/20/11   aro     Added support for intelliceiver Start ans Stop
06/16/11   aro     Compiler warning fix
06/16/11   aro     Added support for intelliceiver init
06/06/11   sty     compiler warning fix
06/06/11   sty     Return normal mode for non-primary devices in 
                   rf_cdma_ic_get_starting_power_mode()
04/21/11   aro     Updated CDMA Data interface for RF Calibraiton/Init
02/17/11   aro     [1] Added Interface to get current and starting Power mode
                   [2] Renamed intelliceiver function
02/16/11   aro     Added initial implementation of intelliceiver init and
                   get/set power mode
02/01/11   aro     Added function to query Lowest Power Mode
02/01/11   aro     Promoted Power Mode types to RFM
01/31/11   aro     Added Set Power Mode function
01/12/11   aro     Changed the initial PowerMode to RF_CDMA_POWER_MODE_1 as
                   current RTR does not support PowerMode 2
01/08/11   aro     Changed @ingroup to @addtogroup
01/05/11   aro     Moved @defgroup to rf_cdma_documentation.h
01/03/11   aro     Changed the return value to TRUE
12/23/10   aro     Doxygen update
12/23/10   aro     Added Intelliceiver Start and Query function
12/20/10   aro     Removed including of rfcom.h
12/17/10   aro     Added Intelliceiver init function
12/14/10   aro     Initial Revision.

==============================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_cdma_intelliceiver.h"
#include "rf_cdma_data.h"
#include "rfcommon_msg.h"
#include "rfdevice_cdma_interface.h"
#include "rfdevice_cmn_intf.h"
#include "rfm_internal.h"
#include "rf_1x_rx_agc.h"
#include "rf_1x_mdsp.h"
#include "rf_hdr_rx_agc.h"
#include "rf_hdr_mdsp.h"
#include "rf_hdr_mc.h"
#include "rfcommon_locks.h"
#include "rf_cdma_utils_freq.h"
#include "rfc_card.h"
#include "rfcommon_math.h"
#include "rfm_internal.h"
#include "rfm.h"
#include "rfcommon_mdsp.h"
#include "rf_cdma_fwrsp.h"
#include "rf_cdma_mc.h"

/* memscpy */
#include "stringl.h"

/*----------------------------------------------------------------------------*/
/*! Time out value for High Linearity */
#define RF_CDMA_IC_TIME_OUT_HIGH_LIN 3

/*----------------------------------------------------------------------------*/
/*! Time out value for Mid Linearity */
#define RF_CDMA_IC_TIME_OUT_MID_LIN 3

/*----------------------------------------------------------------------------*/
/*! Polling interval to be used when the intelliceiver is in High Linearity 
Mode in mS */
#define RF_CDMA_IC_HL_STEADY_WARMUP_POLL_DELAY 1000

/*----------------------------------------------------------------------------*/
/*! Polling interval to be used when the intelliceiver is in Low Linearity 
Mode in mS */
#define RF_CDMA_IC_LL_STEADY_WARMUP_POLL_DELAY 10

/*----------------------------------------------------------------------------*/
/*! Interval to trigger intelliciver steady state mode in mS */
#define RF_CDMA_IC_START_STEADY_TRIGGER_DELAY 40

/*============================================================================*/
/*!
  @name Diagram

  @brief
  This section includes the illustration of intelliceiver state machine and
  intelliceiver call flows
*/
/*! @{ */

/*! 
  @image html rf_cdma_intelliceiver_stm.jpg  
  @image html rf_cdma_intelliceiver_call_flow_1.jpg
  @image html rf_cdma_intelliceiver_call_flow_2.jpg
  @image html rf_cdma_intelliceiver_call_flow_3.jpg
  @image html rf_cdma_intelliceiver_call_flow_4.jpg
*/

/*! @} */

/*----------------------------------------------------------------------------*/
static void rf_cdma_ic_set_polling_timer
(
  rf_cdma_ic_data_type *ic_data
);

/*----------------------------------------------------------------------------*/
static boolean
rf_cdma_ic_get_wbagc_jd_status
(
  rfm_device_enum_type device,
  rf_cdma_mdsp_chain_enum_type rx_mdsp_path,
  rfm_cdma_power_mode_type *curr_power_mode
);


/*============================================================================*/
/*!
  @name Jammer List

  @brief
  Helper functions to be manage Jammer Lists used for intelliceiver operation
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Insert Jammer Item to doubly linked list
  
  @details
  Inserts a rf_cdma_ic_jammer_item_type into a doubly-linked list of 
  rf_cdma_ic_jammer_item_type.  The item is inserted after the item passed
  in as item_before.
 
  This function will form an double linked list as shown in the diagram.
 
  @image html rf_cdma_intelliceiver_doubly_linkedlist.jpg
 
  @param item_before
  Jammer Item to which a new item is to be inserted
 
  @param item_to_insert
  New Jammer Item to be inserted
*/
static void
rf_cdma_ic_insert_jammer_list_item
(
  rf_cdma_ic_jammer_item_type *item_before, 
  rf_cdma_ic_jammer_item_type *item_to_insert
)
{

  item_to_insert->next = item_before->next;
  (item_before->next)->prev = item_to_insert;
  item_before->next = item_to_insert;
  item_to_insert->prev = item_before;

} /* rf_cdma_ic_insert_jammer_list_item */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Remove Jammer Item to doubly linked list
  
  @details
  Removes a rf_cdma_ic_jammer_item_type into a doubly-linked list of 
  rf_cdma_ic_jammer_item_type. 
 
  @param item_to_remove
  Jammer Item to be removed
*/
static void
rf_cdma_ic_remove_jammer_list_item
(
  const rf_cdma_ic_jammer_item_type *item_to_remove
)
{
  (item_to_remove->prev)->next = item_to_remove->next;
  (item_to_remove->next)->prev = item_to_remove->prev;

} /* rf_cdma_ic_remove_jammer_list_item */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Jammer List
  
  @details
  This will configure Intelliceiver driver state to a known value. This
  initialization process is hardware initialization.
 
  @param device
  Device on which Intelliceiver is to be initialized
 
  @return
  Status of Intelliceiver init
*/
static void
rf_cdma_ic_init_jammer_list
(
  rf_cdma_ic_data_type *ic_data
)
{
  uint32 idx;
  /* build the items into the doubly-linked list */

  memset(&ic_data->jammer_sentinel_spare,0,sizeof(rf_cdma_ic_jammer_item_type));
  memset(&ic_data->jammer_sentinel_used,0,sizeof(rf_cdma_ic_jammer_item_type));
  memset(ic_data->jammer_items,0,(RF_CDMA_IC_JAMMER_STATUS_NUM*sizeof(rf_cdma_ic_jammer_item_type)));

  ic_data->jammer_sentinel_spare.next = &(ic_data->jammer_sentinel_spare);
  ic_data->jammer_sentinel_spare.prev = &(ic_data->jammer_sentinel_spare);

  /*  */
  for ( idx = 0; idx < RF_CDMA_IC_JAMMER_STATUS_NUM; idx++ )
  {
    rf_cdma_ic_insert_jammer_list_item( &(ic_data->jammer_sentinel_spare),
                             &ic_data->jammer_items[idx] );
  }

  ic_data->jammer_sentinel_used.next = &(ic_data->jammer_sentinel_used);
  ic_data->jammer_sentinel_used.prev = &(ic_data->jammer_sentinel_used);

} /* rf_cdma_ic_init_jammer_list */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Find Jammer Item containg given band and channel in used jammer list
 
  @details
  This function locates a rf_cdma_ic_jammer_item_type in the "used" list. The 
  band/chan is used as the comparison key. If an item is found matching the 
  input comparison key, a pointer to that item is returned. If a match is not
  found a pointer to the sentinel is returned
 
  @param ic_data
  Pointer to the intelliceiver data on which Jammer item is to be searched
 
  @param band
  RF Band to be used as a comparision key to match in a jammer item
 
  @param channel
  RF channel to be used as a comparision key to match in a jammer item
 
  @return
  If an item is found matching the input comparison key, a pointer to that item 
  is returned. If a match is not found a pointer to the sentinel is returned.
*/
static rf_cdma_ic_jammer_item_type*
rf_cdma_ic_find_jammer_item
(
  rf_cdma_ic_data_type *ic_data,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type channel
)
{
  /* Initialize the variable to point to next item pointed by sentinel */
  rf_cdma_ic_jammer_item_type *found = ic_data->jammer_sentinel_used.next;

  /* Loop through the linked list to find the jammer item containing the
  given band and channel */
  while ( found != &(ic_data->jammer_sentinel_used) )
  {
    if ( ( found->jammer_entry.band == band ) && 
         ( found->jammer_entry.channel == channel )
       )
    {
      /* Match is found. Therefore break */
      break;
    }
    else
    {
      /* No match, traverse through the linked list */
      found = found->next;
    }
  }

  /* return the item */
  return found;

} /* rf_cdma_ic_find_jammer_item */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update the current Jammer Item to point to the item pointer pertaining to
  given band and channel
 
  @details
  This function will update the current jammes pointer to pointer to correct
  item pointer.This function should be called suring intelliceiver start; so
  that rest if the intelliceiver operation can operate on the most recent jammer
  item pointer.
 
  This function will first search through the "used" item list to find the
  item which matches the band and channel. If the item is found in this list,
  then the item will be moved to the head node of the list and will be used as 
  current item for rest of the intelliceiver operation. If no item is found
  "Used" list, an item is popped from the "Spare" list, pushed into the
  head node of "used" list and then used as a current item for the given band 
  and channel. If, there is not item available in "Used" list, then the oldest
  item from "Spare" reused are a current item after moving that to head node.
 
  In case of no item found in "Used" list, the jammer item will be 
  re-initialized to default state of LOW_LIN, and HIGH_LIN/MID_LIN counters
  are reset to max value.
 
  @param ic_data
  Pointer to intelliceiver data for which the current jammer item is to be
  updated
 
  @param band
  RF Band to be used for match to search for the jammer item
 
  @param channel
  RF chanel to be used for match to search for the jammer item
*/ 
static void
rf_cdma_ic_update_current_jammer_item
(
  rf_cdma_ic_data_type *ic_data,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type channel
)
{
  /* Variable to store the jammer item which matches the band and channel */
  rf_cdma_ic_jammer_item_type *found = NULL;

  /* Find the jammer item which matches the band and channel */
  found = rf_cdma_ic_find_jammer_item( ic_data, band, channel );

  /* if the "found" does equals the address of "Used sentinal", this means that
  the match was found. */
  if ( found != &(ic_data->jammer_sentinel_used) )
  {
    /* The item is in "Used Sentinal" list */

    /* Check if the found item is in the head node or not. If it is not in
    head node, then move it to head*/
    if ( found != ic_data->jammer_sentinel_used.next )
    {
      /* It is not at the head, we need to move it to the head to keep
      the list items in time order */

      /* Remove the iteam from the list */
      rf_cdma_ic_remove_jammer_list_item( found );

      /* insert the item to head node */
      rf_cdma_ic_insert_jammer_list_item( &(ic_data->jammer_sentinel_used),
                                          found );

      if ( ic_data->dbg_data.f3_en )
      {
        RF_MSG_1( RF_MED, "rf_cdma_ic_update_current_jammer_item: Dev %d - Item "
                  "found in used list (not head)", ic_data->device );
      }

    } /* ( found != ic_data->jammer_sentinel_used.next ) */
    else
    {

      if ( ic_data->dbg_data.f3_en )
      {
        RF_MSG_1( RF_MED, "rf_cdma_ic_update_current_jammer_item: Dev %d - Item "
                  "found in used list (head)", ic_data->device );
      }
    }

  } /* if ( found != &(ic_data->jammer_sentinel_used) ) */
  else /* Not in "Used" list */
  {
    /* Jammer item is not in "Used" list, thus get an item from "Spare" list
    and add it to "Used" list */

    /* Check if the "Spare" list has atleast one item */
    if ( ic_data->jammer_sentinel_spare.next != 
         &(ic_data->jammer_sentinel_spare)
       )
    {
      /* There is free item in Spare list. Use that to push to "Used" list */
      found = ic_data->jammer_sentinel_spare.next;
    }
    else
    {
      /* Since there is not Spare List, Reuse the last item from Used list */
      found = ic_data->jammer_sentinel_used.prev;
    }

    /* Remove the item from the list */
    rf_cdma_ic_remove_jammer_list_item( found );

    /* Insert it at the head of the used list */
    rf_cdma_ic_insert_jammer_list_item( &(ic_data->jammer_sentinel_used),
                                        found );

    /* Initialize to item to a Linearity based on if intelliceiver is   
    supported or not */
    if ( ic_data->ic_supported == TRUE )
    {
      /* If intelliceiver is supported, initialize to LOW_LIN */
      (found->jammer_entry).jammer_state = RFM_CDMA_POWER_MODE_1;
      (found->jammer_entry).high_lin_counter = 0;
      (found->jammer_entry).mid_lin_counter = RF_CDMA_IC_TIME_OUT_MID_LIN;
      (found->jammer_entry).band = band;
      (found->jammer_entry).channel = channel;
    }
    else
    {
      /* If intelliceiver is supported, initialize to HIGH_LIN */
      (found->jammer_entry).jammer_state = RFM_CDMA_POWER_MODE_NORMAL;
      (found->jammer_entry).high_lin_counter = RF_CDMA_IC_TIME_OUT_HIGH_LIN;
      (found->jammer_entry).mid_lin_counter = RF_CDMA_IC_TIME_OUT_MID_LIN;
      (found->jammer_entry).band = band;
      (found->jammer_entry).channel = channel;
    }

  }

  /* Finally Update the current jammer item */
  ic_data->curr_jammer_item = found;

} /* rf_cdma_ic_update_current_jammer_item */

/*! @} */

/*============================================================================*/
/*!
  @name FW Callbacks

  @brief
  Functions used to program RTR to control intelliceiver operation
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Callback function to be executed after 1x iC update is done
 
  @details
  This function will be executed once the command to do intelliceiver update
  is sent to the Firmware. This function will do infinite wait for the response
  from the firmwware.
 
  @param mdsp_event
  Enumeration inidicating the MDSP event
 
  @param data
  Pointer to the data structure containing data passed to the callback function
*/
void 
rf_cdma_ic_resume_onex_rx_agc_cb
(
  rf_cdma_mdsp_cb_event_type mdsp_event,
  void* data
)
{
  rf_cdma_ic_resume_onex_rx_agc_cb_data_type *cb_data;
  cb_data = (rf_cdma_ic_resume_onex_rx_agc_cb_data_type*)data;

  /* Wait for semaphore only it is specified as a block call*/
  if ( cb_data->is_blocking_call == TRUE )
  {
    RF_MSG_1( RF_LOW, "rf_cdma_ic_resume_onex_rx_agc_cb : iC Update - "
              "Start Wait Sem @ 0x%x", cb_data->fw_rsp_hk->sema_token );
    /* Wait for the semaphore to be posted. This will be posted, when FWRSP
    task receives response from FW indicating that IC_UPDATE is complete */
    rfcommon_semaphore_wait( cb_data->fw_rsp_hk->sema_token );
    rfcommon_semaphore_push_item( cb_data->fw_rsp_hk->sema_token );
    cb_data->fw_rsp_hk->sema_token = NULL;
  }

  /* Debug message */
  if ( cb_data->ic_data->dbg_data.f3_en )
  {
    RF_MSG_1( RF_MED, "rf_cdma_ic_resume_onex_rx_agc_cb: Dev %d - "
              "iC Update complete", cb_data->ic_data->device );
  } /* if ( ic_data->dbg_data.f3_en ) */

  return;

} /* rf_cdma_ic_resume_onex_rx_agc_cb */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Callback function to be executed after 1x iC update is done

  @details
  This function will be executed once the command to do intelliceiver update
  is sent to the Firmware. This function will do infinite wait for the response
  from the firmwware.

  @param mdsp_event
  Enumeration inidicating the MDSP event

  @param data
  Pointer to the data structure containing data passed to the callback function
*/
void 
rf_cdma_ic_resume_hdr_rx_agc_cb
(
  rf_cdma_mdsp_cb_event_type mdsp_event,
  void* data
)
  {
  rf_cdma_ic_resume_hdr_rx_agc_cb_data_type *cb_data;
  cb_data = (rf_cdma_ic_resume_hdr_rx_agc_cb_data_type*)data;

  /* Wait for semaphore only it is specified as a block call*/
  if ( cb_data->is_blocking_call == TRUE )
  {
    RF_MSG_1( RF_LOW, "rf_cdma_ic_resume_hdr_rx_agc_cb : iC Update - "
              "Start Wait Sem @ 0x%x", cb_data->fw_rsp_hk->sema_token );
  /* Wait for the semaphore to be posted. This will be posted, when FWRSP
  task receives response from FW indicating that IC_UPDATE is complete */
    rfcommon_semaphore_wait( cb_data->fw_rsp_hk->sema_token );
    rfcommon_semaphore_push_item( cb_data->fw_rsp_hk->sema_token );
    cb_data->fw_rsp_hk->sema_token = NULL;
  }

  /* Debug message */
  if ( cb_data->ic_data->dbg_data.f3_en )
  {
    RF_MSG_1( RF_MED, "rf_cdma_ic_resume_hdr_rx_agc_cb: Dev %d - "
              "iC Update complete", cb_data->ic_data->device );
  } /* if ( ic_data->dbg_data.f3_en ) */

  return;

} /* rf_cdma_ic_resume_hdr_rx_agc_cb */

/*! @} */

/*============================================================================*/
/*!
  @name RTR Device Programming

  @brief
  Functions used to program RTR to control intelliceiver operation
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Compute Next power Mode based on current power mode measurement
  
  @details
  Update the internal IntelliCeiver jammer state with the most recent
  RF environment reading.  This function operates on the shared band/
  channel jammer list, updating the state and counters for the currently
  tuned channel.  Callers that need to use the new state must access
  the decision through the band/channel jammer list.  This function
  will not update the hardware state.
 
  @param ic_data
  Pointer to the intelliciever Data
 
  @param power_mode
  Current intelliceiver Power Mode measurement for the jammer environment
 
  @return
  Status of Intelliceiver init
*/
static void
rf_cdma_ic_compute_next_state
(
  rf_cdma_ic_data_type *ic_data,
  rfm_cdma_power_mode_type power_mode
)
{
  rf_cdma_ic_jammer_status_type *current_jammer_state_ptr;

  /* Get Current Jammer Entry */
  current_jammer_state_ptr = &(ic_data->curr_jammer_item->jammer_entry);

  /* Update the counter state */
  switch ( power_mode )
  {
  case RFM_CDMA_POWER_MODE_NORMAL:
    /* Reset both high and mid counters to their maximum */
    current_jammer_state_ptr->high_lin_counter =
      RF_CDMA_IC_TIME_OUT_HIGH_LIN;
    current_jammer_state_ptr->mid_lin_counter =
      RF_CDMA_IC_TIME_OUT_MID_LIN;
    break;

  case RFM_CDMA_POWER_MODE_1:
    /* Decrement high linearity counter by one if necessary
    and reset mid counter to the maximum */
    if ( current_jammer_state_ptr->high_lin_counter > 0 )
    {
      current_jammer_state_ptr->high_lin_counter--;
    }
    current_jammer_state_ptr->mid_lin_counter =
      RF_CDMA_IC_TIME_OUT_MID_LIN;
    break;

  case RFM_CDMA_POWER_MODE_2:
    /* Decrement high and mid linearity counters by one if necessary */
    if ( current_jammer_state_ptr->high_lin_counter > 0 )
    {
      current_jammer_state_ptr->high_lin_counter--;
    }
    if ( current_jammer_state_ptr->mid_lin_counter > 0 )
    {
      current_jammer_state_ptr->mid_lin_counter--;
    }
    break;

  default:
    RF_MSG_2( RF_ERROR, "rf_cdma_ic_compute_next_state: Dev %d - Bad PM %d",
              ic_data->device, power_mode );
    break;

  } /* switch ( power_mode ) */

  /* Update the jammer state based on the counters */
  if ( current_jammer_state_ptr->high_lin_counter > 0 )
  {
    current_jammer_state_ptr->jammer_state = RFM_CDMA_POWER_MODE_NORMAL;
  }
  else if ( current_jammer_state_ptr->mid_lin_counter > 0 )
  {
    current_jammer_state_ptr->jammer_state = RFM_CDMA_POWER_MODE_1;
  }
  else
  {
    current_jammer_state_ptr->jammer_state = RFM_CDMA_POWER_MODE_2;
  }

  if ( ic_data->dbg_data.f3_en )
  {
    RF_MSG_5( RF_MED, "rf_cdma_ic_compute_next_state: Dev %d - HL Count %d, "
              "ML Count %d, PM Read from RTR %d, Next PM %d", ic_data->device,
              current_jammer_state_ptr->high_lin_counter, 
              current_jammer_state_ptr->mid_lin_counter, power_mode,
              current_jammer_state_ptr->jammer_state );
  }

  /* Do not update the hardware state in this function */
  return;

} /* rf_cdma_ic_compute_next_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Program intelliceiver calibration data to device driver

  @details
  This function pushes the intelliceiver calibration data to the device
  driver so that the drive can calculate the Jammer detect threshold values.
  The difference of LNA G0 gain value between the calibration reference channel
  and the currrent channel is also sent to the device driver.
 
  @param ic_data
  Pointer to intelliceiver data for which calibration data is to be 
  programmed
 
  @param logical_dev
  Pointer to the logical device parameter from RFC. This will be used to
  get the current device driver instance
 
  @param band
  RF Band for which the Calibration Data is to be programmed. Band information
  is required because all intelliceiver calibration data is in per band basis
 
  @param band
  RF Chan for which the Calibration Data is to be programmed. Chan information
  is required because all intelliceiver calibration data is in per band basis
 
  @param power_mode
  Power Mode fow which Calibration Data is to be programmed
*/
static void
rf_cdma_ic_program_cal_data
(
  rf_cdma_ic_data_type *ic_data,
  const rfc_cdma_logical_device_params_type *logical_dev,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type channel,
  rfm_cdma_power_mode_type power_mode
)
{
  rfdevice_cdma_ic_thres_param_type ic_cal_data; /* Intelliceiver Cal Data */
  uint8 det_idx = 0; /* Variable to loop through all detectors */
  uint8 lna_idx = 0; /* Variable to loop through all LNA Gain States */
  int16 g0_gain_cal_ref_chan = 0; /* Calibrated GO Gain value for intelliceiver
  calibration reference channel */
  int16 g0_gain_cal_curr_chan = 0; /* Calibrated GO Gain value for intelliceiver
  current channel */
  const rf_1x_config_type* rf_1x_config_ptr; /* Pointer to 1x NV config */
  const rf_cdma_config_type* cdma_cfg_ptr; /* Pointer to CDMA Common NV */
  uint16 ref_chan; /* Reference Channel */
  
  /* Get 1x NV configuration data */
  rf_1x_config_ptr = 
    rf_cdma_nv_get_1x_data( logical_dev->rx_nv_path[band], band );

  /* NULL Pointer check */
  if ( rf_1x_config_ptr == NULL )
  {
    RF_MSG_2( RF_ERROR, "rf_cdma_ic_program_cal_data: 1x NV configuration not "
              "allocated for Path %d Band %d", logical_dev->rx_nv_path[band], 
                band ); 
    return ;
  }

  /* Get CDMA config pointer */
  cdma_cfg_ptr = rf_1x_config_ptr->cdma_common_ptr;

  /* NULL Pointer check */
  if ( cdma_cfg_ptr == NULL )
  {
    RF_MSG_2( RF_ERROR, "rf_cdma_ic_program_cal_data: Common NV configuration "
              "not allocated for Path %d Band %d", logical_dev->rx_nv_path[band]
              , band );
    return;
  }

  /* Get the reference Channel */
  ref_chan = cdma_cfg_ptr->ic_data.ref_channel;

  /* Set ic_cal_data to zero. */
  memset( &ic_cal_data, 0, sizeof(rfdevice_cdma_ic_thres_param_type) );

  /* Copy Intelliceiver cal data to device interface specific data structure
  only if it is "not" Calibration Data. For calibration Mode, the cal values
  to be used need to be zeros.*/
  if ( rfm_get_calibration_state() == FALSE )
  {
    for ( det_idx = 0; det_idx < RFDEVICE_CDMA_IC_NUM_OF_DET; det_idx++ )
    {
      /* Copy intelliceiver cal data  */
      ic_cal_data.cal_value[det_idx] = 
                   cdma_cfg_ptr->ic_data.calibration[power_mode][det_idx];
  
  
      /* Copy intelliceiver detector adjust */
      for ( lna_idx = 0; lna_idx < RFM_LNA_GAIN_STATE_NUM; lna_idx++ )
      {
        ic_cal_data.detector_adjust[det_idx][lna_idx] = 
                    cdma_cfg_ptr->ic_data.det_adjust[det_idx][lna_idx];
      }
    }
  
    /* Get LNA Gain0 Gain Cal data  for current channel*/
    g0_gain_cal_curr_chan = rf_cdma_nv_get_gain_cal( 
                               logical_dev->rx_nv_path[band], band, channel, 
                               power_mode, RFM_LNA_GAIN_STATE_0);
  
    /* Get LNA Gain0 Gain Cal data  for Intelliceiver Reference channel*/
    g0_gain_cal_ref_chan = rf_cdma_nv_get_gain_cal( 
                              logical_dev->rx_nv_path[band], 
                              band, ref_chan, power_mode,RFM_LNA_GAIN_STATE_0 );
  }

  /* Adjust the switchpoints based on the difference between the DVGA gain
  at the current channel verses the calibration reference channel */
  ic_cal_data.g0_gain_cal_delta = g0_gain_cal_curr_chan - g0_gain_cal_ref_chan;

  /* New Power Mode to be overriden to */
  ic_cal_data.new_power_mode = power_mode;

  /* Populate the current RFM Mode */
  ic_cal_data.curr_rf_mode = rfm_get_current_mode(ic_data->device);

  /* Push the intelleiver calibration data to device driver so that the device
  driver can compute the jammer detect switch point*/
  rfdevice_cdma_set_ic_cal_data( logical_dev->rx_device[band], 
                                 &ic_cal_data,
                                 band,
                                 power_mode );

  RF_MSG_6( RF_MED,"rf_cdma_ic_program_cal_data: Dev %d - Mode %d, new PM %d, "
            "G0 Gain Delta %d, Cal0 %d, Cal1 %d", ic_data->device,
            ic_cal_data.curr_rf_mode, ic_cal_data.new_power_mode, 
            ic_cal_data.g0_gain_cal_delta, ic_cal_data.cal_value[0],
            ic_cal_data.cal_value[1]);

} /* rf_cdma_ic_program_cal_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set intellicever Power Mode

  @details
  This function pushes the intelliceiver calibration data to the device
  driver so that the the drive can calculate the Jammer detect threshold values.
  The difference of LNA G0 gain value between the calibration reference channel
  and the currrent channel is also sent to the device driver.
 
  Once the JD threshold is set, device is programmed with the requested
  power mode.
  
  @param logical_dev
  Pointer to the logical device parameter from RFC. This will be used to
  get the current device driver instance
 
  @param ic_data
  Pointer to the intelliceiver data
 
  @param num_carriers
  Number of carrier for which power mode is to be set
 
  @param band
  RF Band for which the power mode is to be overriden. Band information is
  required because all intelliceiver calibration data is in per band basis
 
  @param channel_array
  RF Chan array for which the power mode is to be overriden. Chan information is
  required because all intelliceiver calibration data is in per band basis
 
  @param power_mode
  Power Mode to which the device is to be set
 
  @return
  Flag indicating if power mode override was successful
*/
static boolean
rf_cdma_ic_set_power_mode
(
  rf_cdma_ic_data_type *ic_data,
  const rfc_cdma_logical_device_params_type *logical_dev,
  uint32 num_carriers,
  rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type * const channel_array,
  rfm_cdma_power_mode_type power_mode
)
{
  rfm_mode_enum_type curr_mode; /* Current RFM Mode */
  boolean ret_val = TRUE; /* Return Value */
  rfm_cdma_chan_type center_chan;
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  rf_cdma_mdsp_ic_update_type ic_cfg;
  const rfc_common_logical_device_params_type* cmn_logical_dev;
  rf_cdma_mdsp_response_cb_param_type rsp_cb_param;
  rfdevice_cdma_ic_power_mode_config_type power_mode_config;     
  rf_cdma_fwrsp_data_type *fwrsp_hk = NULL;

  /* Get FW Response Data */
  fwrsp_hk = rf_cdma_get_fwrsp_data( ic_data->device );

  /* NULL Pointer Check */
  if ( fwrsp_hk == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_set_power_mode: NULL FWRSP Data" );
    return FALSE;
  } /* if ( fwrsp_data == NULL ) */

  /* Validate number of carriers */
  if ( num_carriers > RFM_CDMA_CARRIER_NUM )
  {
    RF_MSG_2( RF_ERROR, "rf_cdma_ic_set_power_mode: Dev %d, Invalid Num of "
              "Carriers %d", ic_data->device, num_carriers );
    return FALSE;
  }

  /* Get Read-only and Writable Device Status */
  dev_status_r = rf_cdma_get_device_status( ic_data->device );

  if ( dev_status_r == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_ic_set_power_mode: Dev %d, NULL device Data",
              ic_data->device );
    return FALSE;
  }

  /* Get pointer to common device pointer */
  cmn_logical_dev = rfc_common_get_logical_device_params(ic_data->device);

  /* NULL Pointer check */
  if ( cmn_logical_dev == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_ic_set_power_mode: Dev %d, NULL Common "
              "device Data", ic_data->device );
    return FALSE;
  }

  /* Get Current RFM Mode */
  curr_mode = rfm_get_current_mode(ic_data->device);
  
  /* Vote for Quiet mode, if RTR device is going to be programmed in High 
  Liearity Mode. However if the RTR is going to Low Linearity mode, then
  the voting should be removed. */
  if ( power_mode == RFM_CDMA_POWER_MODE_NORMAL )
  {
    /* Vote for quiet mode */
    rfc_set_quiet_mode( curr_mode, TRUE );
  } /* if ( power_mode == RFM_CDMA_POWER_MODE_NORMAL ) */
  else
  {
    /* Remove vote for quiet mode */
    rfc_set_quiet_mode(curr_mode, FALSE );
  } /* if ! ( power_mode == RFM_CDMA_POWER_MODE_NORMAL ) */

  /* Compute the Center Channel for the given set of channel array */
  center_chan = rf_cdma_compute_center_tune_chan( num_carriers,band, 
                                                  channel_array);
  
  /* Configure RxAGC for new Power mode. When RxAGC configure message is
  sent to FW, then FW may reset the LNA state to some default state. Thus
  if PM override function is called when LNA state is in overridde state,
  LNA will then be in new default LNA state override mode. However, if
  this function is called when LNA is in free-runnig mode, FW will reset
  the LNA STM and start to converge to actual LNA state from the default
  LNA State */
  if ( curr_mode == RFM_1X_MODE )
  {
    rf_1x_agc_param_type agc_mode_info; /* 1x RxAGC Mode info */
    rf_1x_fw_response_data_type *fwrsp_data = NULL;

    /* Get a pointer to the FW Response Data */
    fwrsp_data = rf_cdma_get_1x_fw_response_data();

    /* If pointer to the task data cannot be achieved, then it is a
    fatal error */
    if ( fwrsp_data == NULL )
    {
      RF_MSG( RF_ERROR, "rf_cdma_ic_set_power_mode: NULL FWRSP Data" );
      ret_val = FALSE;
    } /* if ( fwrsp_data == NULL ) */
    else
    {

      /* Stop RxAGC */
      rf_1x_mdsp_stop_rx_agc( logical_dev->rx_mdsp_path, NULL, NULL, NULL );
      
      /* As FW stops RxAGC after 52uS of sending STOP Rx response, wait
      for that duration */
      DALSYS_BusyWait(52);
      
      /* Push the calibration Data pertaining to the override power mode
      to Device Driver */
      rf_cdma_ic_program_cal_data( ic_data, logical_dev, band, center_chan,
                                   power_mode);
      
        /* Program the linearity mode after setting intelliceiver cal data as
        it is a requirement Device Driver to ensure cal data for the correct 
        linearity is written to the device. rfdevice_cdma_set_ic_cal_data() just 
        stores the data in a Device Data structure. 
        rfdevice_cdma_ic_set_linearity() writes it to device with other linearity 
        settings  */
      rfdevice_cdma_ic_set_linearity( logical_dev->rx_device[band], power_mode );
      
      /* Indicate the AGC Mode as Resume Normal to specify that Accum init
      should not be latched and used from the value saved by FW side. */
      agc_mode_info.mode = RF_1X_AGC_MODE_RESUME_WITH_CURR;
      
      /* To reduce calibration time, AGC ACQ duration set smaller because
      actual AGC ACQ to TRACK will be done after Set PM is done. */
      if ( rfm_get_calibration_state() == TRUE )
      {
          /* Thus after PM update is done AGC would not have settled. However
          later after the sequence (Set PM, Set LNA, Set Gain), when Set AGC ACQ 
          to TRK is done, AGC will be re-acquired and AGC would settle by the
          time RxAGC sampling is done. */
        agc_mode_info.acq_duration = RF_CDMA_IC_1X_CAL_MODE_ACQ_DURATION_US;
      }
      else
      {
        agc_mode_info.acq_duration = RF_CDMA_IC_1X_ACQ_DURATION_US;
      } /* if ( rfm_get_calibration_state() == TRUE ) */
      
      /* Configure 1x RxAGC */
      ret_val &= rf_1x_configure_rx_agc( ic_data->device,
                                         dev_status_r->rxlm_handle,
                                         band,
                                         channel_array[RFM_CDMA_CARRIER_0] ,
                                         power_mode,
                                         dev_status_r->data_1x.rf_state,
                                         &agc_mode_info );
      
      /* Send IC_UPDATE message only if RX_AGC_CFG is successfull */
      if ( ret_val == TRUE )
      {
        ic_cfg.script_index = -1;
        ic_cfg.rxlm_handle = dev_status_r->rxlm_handle;
        ic_cfg.settling_time = 1024;
      
        /* Start Rx */
        fwrsp_hk->ic_update_rsp.device = ic_data->device;
        fwrsp_hk->ic_update_rsp.sema_token = rfcommon_semaphore_pop_item();
        fwrsp_hk->ic_update_rsp.blocking_call = TRUE;
        rsp_cb_param.rsp_handler = rf_cdma_ic_update_rsp_cb;
        rsp_cb_param.handler_data = &fwrsp_hk->ic_update_rsp;
    
        ret_val = rf_1x_mdsp_resume_rx_agc( logical_dev->rx_mdsp_path, &ic_cfg, 
                                  &rsp_cb_param, NULL, NULL );
    
        /* If sending of the IC update message fails after we have stopped 
        RxAGC that is a non recoverable situation. This is because we have
        stopped RxAGC and have not updated it, we will come back and try to 
        stop RxAGC again, which is going to cause issues*/ 
        /* Check if message was sent succesfully */ 
        if ( ret_val == FALSE )
        {
          RF_MSG ( RF_ERROR, "rf_cdma_ic_set_power_mode(): "
                             "rf_1x_mdsp_resume_rx_agc failed "); 
          /* Message sending failed, free the semaphore */
          RF_MSG_2 ( RF_LOW, "rf_cdma_ic_set_power_mode():"
                     " Freeing the semaphore and not waiting "   
                     " [D%d] IC_UPDATE pushed back : Sem @ 0x%x" , 
                     ic_data->device , fwrsp_hk->ic_update_rsp.sema_token ) ;
        }
        else
        {
        RF_MSG_2( RF_LOW, "[1X][RF-FW Sync] [D%d] IC_UPDATE : "
                  "Start Wait Sem @ 0x%x", ic_data->device,
                  fwrsp_hk->ic_update_rsp.sema_token );
        rfcommon_semaphore_wait( fwrsp_hk->ic_update_rsp.sema_token );
        }

        /* Push back semaphore after wait if mssg was sent successfully,
        or immediately if mssg sending failed*/
        rfcommon_semaphore_push_item( fwrsp_hk->ic_update_rsp.sema_token );

      } /* if ( ret_val == TRUE ) */
      else
      {
        /* This has landed in unrecoverable situation as RxAGC is stopped and
        IC_UPDATE could not done because of failure in RX_AGC_CFG */
        ERR_FATAL( "rf_cdma_ic_set_power_mode: Dev %d - 1X_RX_AGC_CFG "
                   "failed", ic_data->device, 0 ,0 );
      } /* if ! ( ret_val == TRUE ) */
    } /* if ! ( fwrsp_data == NULL ) */
  } /* if ( curr_mode == RFM_1X_MODE ) */
  else if ( curr_mode == RFM_1XEVDO_MODE )
  {
    rfm_hdr_agc_acq_type acq_mode_data;  /* HDR RxAGC Mode info */
    rf_hdr_fw_response_data_type *fwrsp_data = NULL;
    
    /* Get FW Response Data */
    fwrsp_data = rf_cdma_get_hdr_fw_response_data();
    
    /* NULL pointer check */
    if ( fwrsp_data == NULL )
    {
      RF_MSG( RF_ERROR, "rf_cdma_ic_set_power_mode: NULL FWRSP Data" );
      ret_val = FALSE;
    } /* if ( fwrsp_data == NULL ) */
    else
    {
      power_mode_config = 
        rfdevice_cdma_get_ic_pm_config( logical_dev->rx_device[band],
                                        band,
                                        dev_status_r->rx_bw_khz );
      /* Stop RxAGC */
      rf_hdr_mdsp_stop_rx_agc( logical_dev->rx_mdsp_path, NULL, NULL, NULL );
  
      /* As FW stops RxAGC after 52uS of sending STOP Rx response, wait
      for that duration */
      DALSYS_BusyWait(866);
  
      /* Push the intelliceiver cal data and program linearity only if 
      jammer detection is not through WBAGC algorithm. For WBAGC, we only need 
      to disable/enable OLTA with a change in jammer status */
      if(power_mode_config != RFDEVICE_CDMA_IC_ADAPTIVE_PM_WB_AGC)
      {
        /* Push the calibration Data pertaining to the override power mode
        to Device Driver */
        rf_cdma_ic_program_cal_data( ic_data, logical_dev, band, center_chan,
                                     power_mode);
      
        /* Program the linearity mode after setting intelliceiver cal data as it
        is a requirement Device Driver to ensure cal data for the correct 
        linearity is written to the device. rfdevice_cdma_set_ic_cal_data() just 
        stores the data in a Device Data structure. 
        rfdevice_cdma_ic_set_linearity() writes it to device with other linearity 
        settings  */
        rfdevice_cdma_ic_set_linearity( logical_dev->rx_device[band], 
                                        power_mode );
      }
      
      /* Indicate the AGC Mode as Resume Normal to specify that Accum init
      should not be latched and used from the value saved by FW side. */
      acq_mode_data.mode = RFM_HDR_AGC_MODE_RESUME_WITH_CURR;
  
      /* Configure HDR RxAGC */
      ret_val &= rf_hdr_configure_rx_agc( ic_data->device, 
                                          dev_status_r->rxlm_handle,
                                          num_carriers, band,
                                          channel_array, 
                                          center_chan,
                                          &acq_mode_data, power_mode,
                                          dev_status_r->data_hdr.rf_state,
                                          FALSE,/* is_irat_mode */
                                          dev_status_r->carrier_index );
  
      /* Send IC_UPDATE message only if RX_AGC_CFG is successfull */
      if ( ret_val == TRUE )
      {
        ic_cfg.script_index = -1;
        ic_cfg.rxlm_handle = dev_status_r->rxlm_handle;
        ic_cfg.settling_time = 1024;

        /* Perform IC Update */
        fwrsp_hk->ic_update_rsp.device = ic_data->device;
        fwrsp_hk->ic_update_rsp.sema_token = rfcommon_semaphore_pop_item();
        fwrsp_hk->ic_update_rsp.blocking_call = TRUE;
        rsp_cb_param.rsp_handler = rf_cdma_ic_update_rsp_cb;
        rsp_cb_param.handler_data = &fwrsp_hk->ic_update_rsp;

        ret_val = rf_hdr_mdsp_resume_rx_agc( logical_dev->rx_mdsp_path, &ic_cfg, 
                                   &rsp_cb_param, NULL, NULL  );

        /* If sending of the IC update message fails after we have stopped 
        RxAGC that is a non recoverable situation. This is because we have
        stopped RxAGC and have not updated it, we will come back and try to 
        stop RxAGC again, which is going to cause issues*/ 
        /* Check if message was sent succesfully */
        if ( ret_val == FALSE )
        {
          RF_MSG ( RF_ERROR, "rf_cdma_ic_set_power_mode(): "
                             "rf_hdr_mdsp_resume_rx_agc failed "); 
          /* Message sending failed, free the semaphore */
          RF_MSG_2 ( RF_LOW, "rf_cdma_ic_set_power_mode():"
                     " Freeing the semaphore and not waiting "   
                     " [D%d] IC_UPDATE pushed back : Sem @ 0x%x" , 
                     ic_data->device , fwrsp_hk->ic_update_rsp.sema_token ) ;
        }
        else
        {
        RF_MSG_2( RF_LOW, "[HDR][RF-FW Sync] [D%d] IC_UPDATE : "
                  "Start Wait Sem @ 0x%x", ic_data->device,
                  fwrsp_hk->ic_update_rsp.sema_token );
        rfcommon_semaphore_wait( fwrsp_hk->ic_update_rsp.sema_token );
        }

        /* Push back semaphore after wait if mssg was sent successfully,
        or immediately if mssg sending failed*/
        rfcommon_semaphore_push_item( fwrsp_hk->ic_update_rsp.sema_token );

        rf_hdr_mc_rx_agc_convergence(ic_data->device);  
      } /* if ( ret_val == TRUE ) */
      else
      {
        /* This has landed in unrecoverable situation as RxAGC is stopped and
        IC_UPDATE could not done because of failure in RX_AGC_CFG */
        ERR_FATAL( "rf_cdma_ic_set_power_mode: Dev %d - HDR_RX_AGC_CFG "
                   "failed", ic_data->device, 0 ,0 );
      } /* if ! ( ret_val == TRUE ) */
    } /* if ! ( fwrsp_data == NULL ) */
  } /* else if ( curr_mode == RFM_1XEVDO_MODE ) */ 
  else
  {
    ret_val = FALSE;

    /* Invalid RFM Mode. Throw error */
    RF_MSG_2( RF_ERROR, "rf_cdma_ic_set_power_mode: Dev %d - Invalid RFM "
                        "Mode %d ", ic_data->device, curr_mode );
  } /* if ( curr_mode == RFM_1X_MODE ) */

  /* Unconditionally clean-up the Rx AGC LNA SM events here, after 
  Rx AGC configuration is completed */
  ret_val &= rf_cdma_mc_cleanup_rx_agc_lna_ccs_events( ic_data->device );

  if ( ret_val == TRUE )
  {
    /* Update Inetlliceiver Power Mode */
    ic_data->power_mode = power_mode;

    /* F3 message */
    RF_MSG_2( RF_MED, "rf_cdma_ic_set_power_mode: Dev %d - Set to PM %d ",
              ic_data->device, power_mode );
  } /* if ( ret_val == TRUE ) */
  else
  {
    /* F3 message */
    RF_MSG_2( RF_ERROR, "rf_cdma_ic_set_power_mode: Dev %d - Set to PM %d "
              "failed", ic_data->device, power_mode );
  } /* if ! ( ret_val == TRUE ) */

  return ret_val;

} /* rf_cdma_ic_set_power_mode */

/*! @} */

/*============================================================================*/
/*!
  @name Schedule Command

  @brief
  This section includes functions used by intelliciever to schedule commands
  for various intelliceiver operation.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Schedule intelliceiver Enter Steady State Polling command
 
  @details
  This function is used to schedule the command to enter steady state
  action. This will put a command in RF_IC_TASK queue.
 
  @param data
  Intelliceiver Data for which command is put in the queue.
*/
void
rf_cdma_ic_schedule_enter_steady_state
(
  timer_cb_data_type data
)
{
  rf_cdma_ic_cmd_type *ic_ss_enter_cmd;
  rf_cdma_ic_enter_steady_state_payload ss_enter_payload;
  rf_cdma_ic_data_type *ic_data = (rf_cdma_ic_data_type*)data;
    
  /* validate the ic_data */
  if ( ic_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_schedule_steady_state_enter: Null data!" );
    return;
  }
  
  /* Check the size of payload */
  if ( sizeof(rf_cdma_ic_enter_steady_state_payload) > 
       RF_CDMA_IC_MAX_CMD_PAYLOAD_SIZE )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_schedule_steady_state_enter: Payload is"
                      "too big!" );
    return;
  }
  
  /* Populate the payload */
  ss_enter_payload.ic_data_addr = (uint32)ic_data;
  
  /* Get the pointer to the free buffer of RF command queue. */
  if( ( ic_ss_enter_cmd = rf_cdma_get_command_buffer() ) == NULL)
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_schedule_enter_steady_state: RF command"
                      "queue is full!" );
    return;
  }
  
  /* fill the cmd_id and module_id */
  ic_ss_enter_cmd->rex_hdr.cmd_hdr.cmd_id = RF_CDMA_IC_ENTER_STEADY_STATE_C;
  ic_ss_enter_cmd->rex_hdr.cmd_hdr.module_id = MSGR_MODULE_RF_1X;
  
  /* Copy the payload */
  {
    size_t data_length = 
    sizeof( rf_cdma_ic_enter_steady_state_payload ) ; 
    memscpy ( ic_ss_enter_cmd->payload , data_length , 
              &ss_enter_payload , data_length ) ;
  }
  
  /* Debug Message */
  if ( ic_data->dbg_data.f3_en )
  {
    RF_MSG( RF_LOW, "rf_cdma_ic_schedule_enter_steady_state : Done" );
  }

  /* place command to RF ic Task command queue */
  rf_cdma_ic_put_command( ic_ss_enter_cmd );
   
} /* rf_cdma_ic_schedule_enter_steady_state */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Schedule intelliceiver Start Polling command
 
  @details
  This function is used to schedule the command to perform start polling
  action. This will put a command in RF_IC_TASK queue.
 
  @param data
  Intelliceiver Data for which command is put in
  the queue.
*/
void
rf_cdma_ic_schedule_start_polling
(
  timer_cb_data_type data
)
{
  rf_cdma_ic_cmd_type *ic_ss_poll_cmd;
  rf_cdma_ic_start_polling_payload ss_poll_payload;
  rf_cdma_ic_data_type *ic_data = (rf_cdma_ic_data_type*)data;
    
  /* Validate ic_data */
  if ( ic_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_schedule_start_polling: Null data!" );
    return;
  }
  
  /* Check the size of payload */
  if ( sizeof(rf_cdma_ic_start_polling_payload) > 
       RF_CDMA_IC_MAX_CMD_PAYLOAD_SIZE )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_schedule_start_polling: Payload is"
                      "too big!" );
    return;
  }
  
  ss_poll_payload.ic_data_addr = (uint32)ic_data;
  
  /* Get the pointer to the free buffer of RF command queue. */
  if( ( ic_ss_poll_cmd = rf_cdma_get_command_buffer() ) == NULL)
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_schedule_start_polling: RF command"
                      "queue is full!" );
    return;
  }
  
  /* fill the cmd_id and module_id */
  ic_ss_poll_cmd->rex_hdr.cmd_hdr.cmd_id = RF_CDMA_IC_START_POLLING_C;
  ic_ss_poll_cmd->rex_hdr.cmd_hdr.module_id = MSGR_MODULE_RF_1X;
  
  /* Copy the payload */
  {
    size_t data_length = sizeof(rf_cdma_ic_start_polling_payload) ; 
    memscpy ( ic_ss_poll_cmd->payload , data_length , 
              &ss_poll_payload , data_length ) ;
  }
  
  /* Debug Message */
  if ( ic_data->dbg_data.f3_en )
  {
    RF_MSG( RF_LOW, "rf_cdma_ic_schedule_start_polling : Done" );
  }

  /* place command to RF iC Task command queue */
  rf_cdma_ic_put_command( ic_ss_poll_cmd );
   
} /* rf_cdma_ic_schedule_start_polling */

/*! @} */

/*============================================================================*/
/*!
  @name Polling

  @brief
  This section includes functions used for Jammer Detect Polling implementation
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Schedule steady state Polling callback
 
  @details
  Based on the current linearity mode, this funciton will decide on how to 
  schedule the steady state polling callback.
  
  @param ic_data
  Pointer to Intelliceiver Data
*/
static void
rf_cdma_ic_set_polling_timer
(
  rf_cdma_ic_data_type *ic_data
)
{

  /* This timer was also used to enter intelliceiver steady state mode. Thus
  clear the timer before resuing this for Steady State Polling */
  (void)timer_clr( &(ic_data->ic_timer), T_NONE );

  /* Re-Initialize Timer with callback funciton for steady state pooling */
  timer_def( &ic_data->ic_timer,&ic_data->ic_timer_group, NULL, 0,
      rf_cdma_ic_schedule_start_polling, (timer_cb_data_type)ic_data );

  /* Based on the current Power mode, set proper interval for the callback
  funciton */
  switch ( ic_data->power_mode )
  {

  case RFM_CDMA_POWER_MODE_NORMAL:
    timer_set( &(ic_data->ic_timer), RF_CDMA_IC_HL_STEADY_WARMUP_POLL_DELAY,
               0, T_MSEC );
    break;

  case RFM_CDMA_POWER_MODE_1:
    timer_set( &(ic_data->ic_timer), RF_CDMA_IC_LL_STEADY_WARMUP_POLL_DELAY,
               0, T_MSEC );
    break;

  default:
    RF_MSG_2( RF_ERROR, "rf_cdma_ic_set_polling_timer: Dev %d - Bad "
              "Lin Mode %d", ic_data->device, ic_data->power_mode );
    break;

  }

  if ( ic_data->dbg_data.f3_en )
  {
    RF_MSG_2( RF_MED, "rf_cdma_ic_set_polling_timer: Dev %d - "
              "Scheduled polling with current PM %d", ic_data->device,
              ic_data->power_mode );
  }

} /* rf_cdma_ic_set_polling_timer */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Function to retrieve the WBAGC jammer status from mDSP
 
  @details
  WBAGC algorithm if implemented in firmware would update the jammer status based on
  the threshold values passed during IC start.
  In 1x mode the algorithm updates the JD status every slot
  In HDR mode the algorithm updates the JD status every half slot
  
  @param curr_rfm_mode
  Gives the current rfm mode

  @param rx_mdsp_path
  gives the current mdsp path for which WBAGC based JD status is desired 

  @param *curr_power_mode
  Output of the function indicating power mode for current jammer status 
 
   @return
   Flag indicating the success/failure of the function
 
*/
static boolean
rf_cdma_ic_get_wbagc_jd_status
(
  rfm_device_enum_type device,
  rf_cdma_mdsp_chain_enum_type rx_mdsp_path,
  rfm_cdma_power_mode_type *curr_power_mode
)
{
  boolean api_status = FALSE;
  boolean mdsp_jammer_flag = FALSE;

  api_status = rf_cdma_mdsp_check_for_jammer( device,
                                              rx_mdsp_path,
                                              &mdsp_jammer_flag);
  if(api_status)
  {
    if(mdsp_jammer_flag)
    {
      /* WB AGC algo indicates Jammer, switch to HLIN */
      *curr_power_mode = RFM_CDMA_POWER_MODE_NORMAL;
    }
    else
    {
      /* WB AGC algo indicates no Jammer, switch to LLIN */
      *curr_power_mode = RFM_CDMA_POWER_MODE_1;
    }
  }
  else
  {
    /*mdsp_get_jd_status API failed. report Error */
    RF_MSG( RF_HIGH, "rf_cdma_ic_get_wbagc_jd_status: MDSP API to get jammer  "
                      "status returned FALSE, either WB AGC algo is disabled  "
                      " or is a genuine MDSP failure");
    *curr_power_mode = RFM_CDMA_POWER_MODE_INVALID;
    return FALSE;
  }
  return TRUE;
} /* rf_cdma_ic_get_wbagc_jd_status */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Callback to perform intelliceiver Steady State polling
 
  @details
  After the jammer detectors have warmed up and can be read, perform polling
  operation. This polling operation will be done periodically and the
  periodicity of this operation depends on the current linearity state
  of the RTR chipset.
 
  During each polling operation, the jammer status from device driver is
  queried and proper state transition is done. Power mode state transition is
  done only if PowerMode is not in override mode. This is followed by
  re-scheduling the next polling operation based on the new jammer state.
 
  @param ic_data
  Pointer to intelliceiver data on which jammer detect polling is to be done
 
  @param logical_dev
  Pointer to logical device parameter to be used while calling device driver
 
  @param num_carriers
  Number of carrier for which intelliceiver polling is to be done
 
  @param band
  RF Band on which polling is done. This information will be used while
  reconfiguring the RxAGC
 
  @param channel_array
  RF channel array on which polling is done. This information will be used while
  reconfiguring the RxAGC
 
  @return
  Flag indicating the status of power mode update
 
  @retval TRUE
  This indicates that power mode update has been initiated by sending a update
  message to FW
 
  @retval FALSE
  This indicates that power mode update has "not" been initiated
*/
static boolean
rf_cdma_ic_polling
(
  rf_cdma_ic_data_type *ic_data,
  const rfc_cdma_logical_device_params_type *logical_dev,
  uint32 num_carriers,
  rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type * const channel_array
)
{
  rfm_cdma_power_mode_type old_power_state;
  rfm_cdma_power_mode_type current_jammer_reading;
  boolean pm_update = FALSE; /* Flag indicating status if PM update */
  boolean api_status = TRUE;

  /* Remember what the jammer state is before possibly doing an update */
  old_power_state = ic_data->curr_jammer_item->jammer_entry.jammer_state;

  /* During the startup phase, the jammer detector will be enabled, so
  a reading can be made.  This is not guarenteed in the running case,
  so assume that it is off and make no update. */
  if ( ic_data->pm_config == RFDEVICE_CDMA_IC_ADAPTIVE_PM_CHIP )
  {
    current_jammer_reading = 
      rfdevice_cdma_get_ic_state( logical_dev->rx_device[band] );
  }
  else if ( ic_data->pm_config == RFDEVICE_CDMA_IC_ADAPTIVE_PM_WB_AGC)
  {  
    api_status = 
       rf_cdma_ic_get_wbagc_jd_status( ic_data->device,
                                      logical_dev->rx_mdsp_path,
                                      &current_jammer_reading);
  }
  else
  {
    /* Set to invalid power mode if neither is true*/
    current_jammer_reading = RFM_CDMA_POWER_MODE_INVALID;
  }

  /* Check for return value being TRUE or FALSE, If return value is FALSE
  at this point that implies that the jammer detect algo is disabled
  and we should not be computing the next state, as the 
  jammer_state_at_shutdown could have some garbage value */ 
  if ( api_status == TRUE ) 
  {
    /* Update the IntelliCeiver linearity mode based on this reading */
    rf_cdma_ic_compute_next_state( ic_data, current_jammer_reading );
  }

  /* If the computed linearity mode changed, execute the change */
  if ((ic_data->force_pm_update == TRUE) ||
      (ic_data->curr_jammer_item->jammer_entry.jammer_state != old_power_state)
     )
  {
    /* Force PM only once when force_pm_update is TRUE. Thus, this variable
    will be set to FALSE immediately */
    if ( ic_data->force_pm_update == TRUE )
    {
      ic_data->force_pm_update = FALSE;
    }

    /* Set the power mode during polling, only if PowerMode override is not 
    enabled. */
    if ( ic_data->power_mode_override == FALSE )
    {
      /* Set Power Mode only for the case when iC is in RUNNING state. */
      if ( ic_data->ic_state == RF_CDMA_IC_RUNNING )
      {
        /* freeze the WBAGC algorithm before switching the linearity */
        if ( ic_data->pm_config == RFDEVICE_CDMA_IC_ADAPTIVE_PM_WB_AGC )
        {
          rf_cdma_mdsp_jd_algo_freeze_unfreeze_control( ic_data->device, 
                                       logical_dev->rx_mdsp_path,
                                       TRUE /* Freeze algo*/);
        }/*if ( ic_data->pm_config == RFDEVICE_CDMA_IC_ADAPTIVE_PM_WB_AGC )*/ 

        /* Set Power Mode */
        pm_update = rf_cdma_ic_set_power_mode( ic_data, logical_dev, 
                       num_carriers, band,channel_array, 
                       ic_data->curr_jammer_item->jammer_entry.jammer_state );

        /* Linearity was switched, PLL was retuned to new linearity. During the
        PLL relock time, a false jammer might have been triggered if TX power is 
        high Avoid false intelliceiver state change by resetting Jammer detector 
        after lienarity change */
        if ( ic_data->pm_config == RFDEVICE_CDMA_IC_ADAPTIVE_PM_CHIP )
        {
          rfdevice_cdma_ic_reset_jammer_detector( logical_dev->rx_device[band] );
        }
        else if ( ic_data->pm_config == RFDEVICE_CDMA_IC_ADAPTIVE_PM_WB_AGC )
        {
          /* Un-Freeze JD algo */
          rf_cdma_mdsp_jd_algo_freeze_unfreeze_control( ic_data->device, 
                                       logical_dev->rx_mdsp_path,
                                       FALSE /* Unfreeze algo*/);
        }/*else if ( ic_data->pm_config == RFDEVICE_CDMA_IC_ADAPTIVE..*/
      } /* if ( ic_data->ic_state == RF_CDMA_IC_RUNNING ) */

      RF_MSG_6( RF_MED, "rf_cdma_ic_polling: Dev %d - PM %d updated on Band %d "
                "Chan %d {RTR Reading %d, old PM %d}", ic_data->device, 
                ic_data->power_mode, band, channel_array[0], 
                current_jammer_reading, old_power_state );

    } /* if ( ic_data->power_mode_override == FALSE ) */
    else
    {
      RF_MSG_2( RF_HIGH, "rf_cdma_ic_polling: Dev %d - PM in override Mode "
                "(%d) - No Device Update", ic_data->device,ic_data->power_mode);
    } /* else ( ic_data->power_mode_override == FALSE ) */
  } /* if ( ic_data->curr_jammer_item->jammer_entry.jammer_state != old_power_state ) */
  
  if ( ic_data->dbg_data.f3_en )
  {
    RF_MSG_5( RF_HIGH, "rf_cdma_ic_polling: Dev %d - Polling done, band %d, "
              "newPM %d, {RTR Reading %d, old PM %d}", 
              ic_data->device, band,
              ic_data->curr_jammer_item->jammer_entry.jammer_state,
              current_jammer_reading, old_power_state );
  }

  return pm_update;

} /* rf_cdma_ic_polling */

/*! @} */

/*============================================================================*/
/*!
  @name RF iC Task Handlers 

  @brief
  This section includes handler functions for RF_IC_TASK to be executed when 
  intelliceiver Steady State enter and polling is to be done.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  RF ic Task Dispatcher function to enter intelliceiver Steady State Mode
 
  @details
  This callback function will be executed after 40 ms of starting the
  intelliceiver to move the intelliceiver to steady state. Till this function is
  executed, no power mode change is allowed. Following this, the steady state 
  timer will be schedules and intelliceiver mode will be mode to RUNNING state.
 
  Before executing this function, it is made sure that common critical 
  section lock for CDMA driver is acquired. This is necessary to preventing the
  clobbering of CDMA data used by iC due to new RFM API activity
 
  This function checks for the current iC State before proceeding. This function
  will schedule the the SteadyState timer only if the the ic State is
  in RF_CDMA_IC_STARTING. However, there might be a case when this function
  gets called when ic State is not in RF_CDMA_IC_STARTING. An instance of
  such occurance would be
  - iC is started when Rx is woken up. During this process, ic_state will be
    set to RF_CDMA_IC_STARTING in L1 task context. Then a command is sent to
    RF_IC_TASK to move to steady state.
  - In the mean time, before RF_IC_TASK executes the command, Rx Sleep command
    is called by L1. This causes the ic_state to be moved back to
    RF_CDMA_IC_STOPPED.
  - Then the RF_IC_TASK pops the command out of the queue to execute. In this 
    case, this funciton should not be executed.
 
  Thus a state checking is required.

  @param ic_cmd
  Intelliciver command for which this handler function is called
 
  @param cmd_payload_size
  Size of the command payload
 
  @param cmd_ptr
  Pointer to the command packet corresponding to the current command
*/
void
rf_cdma_ic_handle_enter_steady_state
(
  rf_cdma_ic_cmd_id_type ic_cmd, 
  rf_cdma_ic_cmd_payload_size_type cmd_payload_size, 
  rf_cdma_ic_cmd_type* cmd_ptr
)
{
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  rf_cdma_ic_data_type *ic_data;
  rfm_device_enum_type device;
  rf_cdma_ic_enter_steady_state_payload ss_enter_payload;

  /* Before executing this function, make sure that common critical 
  section lock for CDMA driver is acquired. This is necessary to preventing the
  clobbering of CDMA data used by iC due to new RFM API activity. Moreover, make 
  sure that the iC state is running, when Power Mode is set. This is to cover 
  the case, when polling CB is called, and RFM CDMA API immediately does RX_STOP
  (due to Sleep Rx or Retune). */

  /* Enter Critical Section */
  rf_common_enter_critical_section( rf_cdma_get_lock_data() );

  /* Copy the Payload to local data */
  {
    size_t data_length = 
	  sizeof(rf_cdma_ic_enter_steady_state_payload) ; 
    memscpy ( &ss_enter_payload , data_length , 
              cmd_ptr->payload , data_length ) ;
  }
  /* Get Intelliceiver, Band, and Channel info */
  ic_data = (rf_cdma_ic_data_type*)(ss_enter_payload.ic_data_addr);

  /* Get RF Device */
  device = ic_data->device;

  /* Get Device Status */
  dev_status_r = rf_cdma_get_device_status( device );

  /* Perform NULL pointer check */
  if ( dev_status_r != NULL )
  {
    /* Check for the current iC State before proceeding. This function
    will schedule the the SteadyState timer only if the the ic State is
    in RF_CDMA_IC_STARTING. However, there might be a case when this function
    gets called when ic State is not in RF_CDMA_IC_STARTING. An instance of
    such occurance would be
    - iC is started when Rx is woken up. During this process, ic_state will be
      set to RF_CDMA_IC_STARTING in L1 task context. Then a command is sent to
      RF_IC_TASK to move to steady state.
    - In the mean time, before RF_IC_TASK executes the command, Rx Sleep command
      is called by L1. This causes the ic_state to be moved back to
      RF_CDMA_IC_STOPPED.
    - Then the RF_IC_TASK pops the command out of the queue to execute. In this 
      case, this funciton should not be executed.
      Thus a state checking is required
    */
    if ( ic_data->ic_state == RF_CDMA_IC_STARTING )
    {
      /* Schedule Steady State Polling timer based on current Power Mode */
      rf_cdma_ic_set_polling_timer( ic_data );
  
      /* Update the intelliceiver state to RUNNING state */
      ic_data->ic_state = RF_CDMA_IC_RUNNING;
  
      RF_MSG_1( RF_MED,"rf_cdma_ic_handle_enter_steady_state: Dev %d - "
                "Entered Steady State", ic_data->device );
    } /* if ( ic_data->ic_state == RF_CDMA_IC_STARTING ) */
    else
    {
      RF_MSG_1( RF_HIGH,"rf_cdma_ic_handle_enter_steady_state: Dev %d - "
                "Not in STARTING State - thus aborting", ic_data->device );
    } /* if !( ic_data->ic_state == RF_CDMA_IC_STARTING ) */
  } /* if ( dev_status_w != NULL ) */
  else
  {
    RF_MSG_1( RF_ERROR,"rf_cdma_ic_handle_enter_steady_state: Dev %d - "
              "Bad Device Data", ic_data->device );
  } /* if !( dev_status_w != NULL ) */

  /* Leave Critical Section */
  rf_common_leave_critical_section( rf_cdma_get_lock_data() );

} /* rf_cdma_ic_handle_enter_steady_state */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  RF ic Task Dispatcher function to start intelliceiver polling
 
  @details
  This callback function will be executed after intelliceiver has entered a
  steady state mode. This function will perform polling; based on which
  the power mode will be changed. However, if the RTR does not detect any 
  change in jammer status, then no power mode will be altered.
 
  Before executing this function, it is extremely important to check the current 
  RUNNING state of intellicever before doing anything (even acquiring the CDMA 
  lock). If the RUNNING state of intelliceiver is not RUNNING-POLLING, then 
  that means that polling had already been started and has not finished that 
  instance of polling. When polling is not finished, this function will not 
  execute the the new polling command. The new polling command will be dropped.
 
  Following this, attempt to acquire a common ciritcal lock for CDMA driver 
  will be done. This lock will be held, until the polling operation is complete. 
  This is necessary to allow existing Tune procedure to complete. This lock
  will be held by RF_IC_TASK until the complete Polling is complete.
  If polling is initiated successfully by this function, then the lock will be
  released after complete_polling command is execited
 
  This function also checks for the current iC State before proceeding. This 
  function will update linearity (if required) and also schedule the the 
  SteadyState polling timer only if the the ic State is in RF_CDMA_IC_RUNNING. 
  However, there might be a case when this function gets called when ic State is 
  not in RF_CDMA_IC_RUNNING. An instance of such occurance would be
  - iC is already started and is in RUNNING state. Then steady state polling 
    timer is schedule to exeucted this function through RF_IC_TASK.
  - In the mean time, before RF_IC_TASK executes the command, Rx Sleep command
    or retune command is called by L1. This causes the ic_state to be moved 
    back to RF_CDMA_IC_STOPPED.
  - Then the RF_IC_TASK pops the command out of the queue to execute. In this 
    case, this funciton should not be executed.
 
  Thus a state checking is required.
 
  @param ic_cmd
  Intelliciver command for which this handler function is called
 
  @param cmd_payload_size
  Size of the command payload
 
  @param cmd_ptr
  Pointer to the command packet corresponding to the current command
*/
void
rf_cdma_ic_handle_start_polling
(
  rf_cdma_ic_cmd_id_type ic_cmd, 
  rf_cdma_ic_cmd_payload_size_type cmd_payload_size, 
  rf_cdma_ic_cmd_type* cmd_ptr
)
{
  const rfc_cdma_logical_device_params_type *logical_dev;
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  rf_cdma_ic_data_type *ic_data;
  rfm_device_enum_type device;
  rf_cdma_ic_start_polling_payload ss_poll_payload;
  boolean is_pm_updated = FALSE;

  /* Copy the Payload into local data structure */  
  {
    size_t data_length = sizeof(rf_cdma_ic_start_polling_payload) ;
    memscpy ( &ss_poll_payload , data_length , 
              cmd_ptr->payload , data_length ) ;
  }

  /* Get Intelliceiver data pointer */
  ic_data = (rf_cdma_ic_data_type*)(ss_poll_payload.ic_data_addr);

  /* NULL Pointer check. If NULL pointer is detected, then the iC polling
  will be ceased until next Rx Wakeup is done. In other words, NULL data
  means no re-scheduling for timer for next polling until next Rx wakeup */
  if ( ic_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_handle_start_polling: NULL iC data" );
    return;
  }

  /* Now try to acquire a common ciritcal lock for CDMA driver. This
  lock will be held, until the polling operation is complete. This is necessary
  to allow existing Tune procedure to complete.
 
  This lock will be held by RF_IC_TASK until the complete Polling is complete.
  If polling is initiated successfully by this function, then the lock will be
  released after complete_polling command is execited. */
  
  /* Enter Critical Section */
  rf_common_enter_critical_section( rf_cdma_get_lock_data());
  
  /* Determine the current device */
  device = ic_data->device;
  
  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  
  /* Get Device Status */
  dev_status_r = rf_cdma_get_device_status( device );
  
  /* Null pointer check */
  if ( ( logical_dev == NULL ) || ( dev_status_r == NULL ) )
  {
    /* Since Logical Device or Device Data is invalid, next polling
    will no be scheduled. This iC will be disabled until next Rx Wakeup. */
    RF_MSG_1( RF_ERROR, "rf_cdma_ic_handle_start_polling: Invalid dev %d "
                        "data. iC ceased until next Rx wakeup", device );
  } /* if ( ( logical_dev == NULL ) || ( dev_status_w == NULL ) ) */
  else
  {
    /* Check for the current iC State before proceeding. This function
    will update linearity (if required) and also schedule the the SteadyState
    polling timer only if the the ic State is in RF_CDMA_IC_RUNNING. However, 
    there might be a case when this function gets called when ic State is not in 
    RF_CDMA_IC_RUNNING. An instance of such occurance would be
    - iC is already started and is in RUNNING state. Then steady state polling 
      timer is schedule to exeucted this function through RF_IC_TASK.
    - In the mean time, before RF_IC_TASK executes the command, Rx Sleep command
      or retune command is called by L1. This causes the ic_state to be moved 
      back to RF_CDMA_IC_STOPPED.
    - Then the RF_IC_TASK pops the command out of the queue to execute. In this 
      case, this funciton should not be executed.
    Thus a state checking is required.
    */
    if ( ( ic_data->ic_state == RF_CDMA_IC_RUNNING ) && 
         ( dev_status_r->num_carriers <= RFM_CDMA_CARRIER_NUM ) )
    {
      /* Perform Intelliciever polling */
      is_pm_updated = rf_cdma_ic_polling( ic_data, logical_dev, 
                 dev_status_r->num_carriers,
                 dev_status_r->curr_band,
                 dev_status_r->curr_chans );

      /* Re-schedule the steady-state callback if it is necessary */
      rf_cdma_ic_set_polling_timer( ic_data );
  
      /* Debug message */
      if ( ic_data->dbg_data.f3_en )
      {
        RF_MSG_2( RF_MED, "rf_cdma_ic_handle_start_polling: Dev %d - "
                  "Polling Started, is_pm_updated %d", ic_data->device,
                  is_pm_updated );
      } /* if ( ic_data->dbg_data.f3_en ) */
 
    } /* if ( ic_data->ic_state == RF_CDMA_IC_RUNNING ) */
    else
    {
      /* This condition is not always a bad one. If there is the last
      pending polling command just after pushing PAUSE command to
      IC_TASK Q and just before handling the command. Thus for this
      scenario, dont print error message */
      if ( ic_data->ic_state == RF_CDMA_IC_PAUSE )
      {
        RF_MSG_1( RF_HIGH, "rf_cdma_ic_handle_start_polling: Dev %d - "
                  "iC moved to PAUSE just b4 the last pending Polling",
                  ic_data->device );
      } /* if ( ic_data->ic_state == RF_CDMA_IC_PAUSE ) */
      else
      {
        RF_MSG_1( RF_ERROR, "rf_cdma_ic_handle_start_polling: Dev %d - "
                  "Not in RUNNING State", ic_data->device );
      } /* if ! ( ic_data->ic_state == RF_CDMA_IC_PAUSE ) */
    } /* else ( ic_data->ic_state == RF_CDMA_IC_RUNNING ) */
  } /* else ( ( logical_dev == NULL ) || ( dev_status_w == NULL ) ) */
  
  /* Leave Critical Section */
  rf_common_leave_critical_section( rf_cdma_get_lock_data() );
  
} /* rf_cdma_ic_handle_start_polling */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  RF ic Task Dispatcher function to pause intelliceiver
 
  @details
  This callback function will be executed when intelliceiver block is requested
  to be moved to PAUSE state.
 
  In this state, no intelliceiver operations will be active as all timers will
  be cleared. Just before entering the pause state, current intelliceiver
  state information is saved so that intelliceiver will be resumed from the
  same point when PAUSE is cleared.
 
  @param ic_cmd
  Intelliciver command for which this handler function is called
 
  @param cmd_payload_size
  Size of the command payload
 
  @param cmd_ptr
  Pointer to the command packet corresponding to the current command
*/
void
rf_cdma_ic_handle_pause
(
  rf_cdma_ic_cmd_id_type ic_cmd, 
  rf_cdma_ic_cmd_payload_size_type cmd_payload_size, 
  rf_cdma_ic_cmd_type* cmd_ptr
)
{
  const rfc_cdma_logical_device_params_type *logical_dev;
  rf_cdma_ic_data_type *ic_data;
  rfm_device_enum_type device;
  rf_cdma_ic_pause_payload pause_payload;

  /* Copy the Payload into local data structure */  
  {
    size_t data_length = sizeof(rf_cdma_ic_pause_payload) ;
    memscpy ( &pause_payload , data_length ,
              cmd_ptr->payload , data_length ) ;
  }

  /* Get Intelliceiver data pointer */
  ic_data = (rf_cdma_ic_data_type*)(pause_payload.ic_data_addr);

  /* NULL Pointer check. If NULL pointer is detected, then the iC polling
  will be ceased until next Rx Wakeup is done. In other words, NULL data
  means no re-scheduling for timer for next polling until next Rx wakeup */
  if ( ic_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_handle_pause: NULL iC data" );
    return;
  }

  ic_data->pause_info.ic_state_before_pause = ic_data->ic_state;

  /* Now try to acquire a common ciritcal lock for CDMA driver. This
  lock will be held, until the operation is complete. This lock will be held by 
  RF_IC_TASK until the PAUSE action is complete. */
  
  /* Enter Critical Section */
  rf_common_enter_critical_section( rf_cdma_get_lock_data());
  
  /* Determine the current device */
  device = ic_data->device;
  
  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
    
  /* Null pointer check */
  if ( logical_dev == NULL )
  {
    /* Since Logical Device or Device Data is invalid, next polling
    will no be scheduled. This iC will be disabled until next Rx Wakeup. */
    RF_MSG_1( RF_ERROR, "rf_cdma_ic_handle_pause: Invalid dev %d "
                        "data. PAUSE failed", device );
  } /* if ( logical_dev == NULL ) */
  else
  {
    /* Clear the timer so that no iC operation is trigerred */
    (void)timer_clr( &(ic_data->ic_timer), T_NONE );

    ic_data->ic_state = RF_CDMA_IC_PAUSE;
    
  } /* else ( logical_dev == NULL ) */
  
  /* Post the Semaphore indicating that PAUSE action is complete */
  rfcommon_semaphore_post( &(ic_data->ic_sems.pause_complete) );

  RF_MSG_1( RF_HIGH, "rf_cdma_ic_handle_pause: Dev %d - Handled PAUSE cmd",
            ic_data->device );

  /* Leave Critical Section */
  rf_common_leave_critical_section( rf_cdma_get_lock_data() );

  return;
  
} /* rf_cdma_ic_handle_pause */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  RF ic Task Dispatcher function to resume intelliceiver
 
  @details
  This callback function will be executed when intelliceiver block is requested
  to be moved out of PAUSE state.
  
  @param ic_cmd
  Intelliciver command for which this handler function is called
 
  @param cmd_payload_size
  Size of the command payload
 
  @param cmd_ptr
  Pointer to the command packet corresponding to the current command
*/
void
rf_cdma_ic_handle_resume
(
  rf_cdma_ic_cmd_id_type ic_cmd, 
  rf_cdma_ic_cmd_payload_size_type cmd_payload_size, 
  rf_cdma_ic_cmd_type* cmd_ptr
)
{
  const rfc_cdma_logical_device_params_type *logical_dev;
  rf_cdma_ic_data_type *ic_data;
  rfm_device_enum_type device;
  rf_cdma_ic_resume_payload resume_payload;
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  size_t data_length = 0 ;

  /* Copy the Payload into local data structure */  
  data_length = sizeof(rf_cdma_ic_resume_payload) ;
  memscpy ( &resume_payload , data_length ,
            cmd_ptr->payload , data_length ) ;

  /* Get Intelliceiver data pointer */
  ic_data = (rf_cdma_ic_data_type*)(resume_payload.ic_data_addr);

  /* NULL Pointer check. If NULL pointer is detected, then the iC polling
  will be ceased until next Rx Wakeup is done. In other words, NULL data
  means no re-scheduling for timer for next polling until next Rx wakeup */
  if ( ic_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_handle_resume: NULL iC data" );
    return;
  }

  /* Now try to acquire a common ciritcal lock for CDMA driver. This
  lock will be held, until the operation is complete. This lock will be held by 
  RF_IC_TASK until the PAUSE action is complete. */
  
  /* Enter Critical Section */
  rf_common_enter_critical_section( rf_cdma_get_lock_data());
  
  /* Determine the current device */
  device = ic_data->device;
  
  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
    
  /* Get Device Status */
  dev_status_r = rf_cdma_get_device_status( device );

  /* Null pointer check */
  if ( ( logical_dev == NULL ) || ( dev_status_r == NULL ) )
  {
    /* Since Logical Device or Device Data is invalid, next polling
    will no be scheduled. This iC will be disabled until next Rx Wakeup. */
    RF_MSG_1( RF_ERROR, "rf_cdma_ic_handle_resume: Invalid dev %d "
                        "data. PAUSE failed", device );
  } /* if ( ( logical_dev == NULL ) || ( dev_status_w == NULL ) ) */
  else
  {
    /* Based on the iC State just before the PAUSE, perform RESUME 
    operation*/ 
    rf_cdma_ic_state_type old_ic_state;
    old_ic_state = ic_data->pause_info.ic_state_before_pause;
    ic_data->pause_info.ic_state_before_pause = RF_CDMA_IC_INVALID;

    /* Move the iC state to STARTING */
    ic_data->ic_state = RF_CDMA_IC_STARTING;

    /* Intelliceiver should be resumed only if CDMA RF state is in RX or RxTx
    state. */
    if ( ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RX ) 
         ||
         ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RXTX ) 
         ||
         ( dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_RX ) 
         ||
         ( dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_RXTX ) )
    {

      if ( ( old_ic_state == RF_CDMA_IC_STARTING ) ||
           ( old_ic_state == RF_CDMA_IC_RUNNING ) )
      {
        /* PLL may be retuned. During the PLL relock time, a false jammer might 
        have been  triggered if TX power is high Avoid false intelliceiver state 
        change by resetting Jammer detector after lienarity change */
	if ( ic_data->pm_config == RFDEVICE_CDMA_IC_ADAPTIVE_PM_CHIP )
	{
          rfdevice_cdma_ic_reset_jammer_detector( 
                          logical_dev->rx_device[dev_status_r->curr_band] );
	}

        /* Move to STARTING state */
        rf_cdma_ic_schedule_enter_steady_state((timer_cb_data_type)ic_data);
      }
      else
      {
        ERR_FATAL( "rf_cdma_ic_handle_resume: Dev %d - Something"
                   "wrong, iC was paused with last state %d", device,
                   old_ic_state, 0 );
      }
    }
    else
    {
      RF_MSG_3( RF_ERROR, "rf_cdma_ic_handle_resume: Dev %d - With Current "
                "1x RF State %d/hdr RF State %d, no additional action for RESUME", device,
                dev_status_r->data_1x.rf_state, dev_status_r->data_hdr.rf_state );
    }  
  } /* else ( ( logical_dev == NULL ) || ( dev_status_w == NULL ) ) */
  
  /* Post the Semaphore indicating that PAUSE action is complete */
  rfcommon_semaphore_post( &(ic_data->ic_sems.resume_complete) );
    
  RF_MSG_1( RF_HIGH, "rf_cdma_ic_handle_resume: Dev %d - Handled RESUME cmd",
            ic_data->device );
   
  /* Leave Critical Section */
  rf_common_leave_critical_section( rf_cdma_get_lock_data() );

  return;
  
} /* rf_cdma_ic_handle_resume */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  RF ic Task Dispatcher function to override power mode
 
  @details
  This callback function will be executed when power mode override command
  is posted to RF iC Task
  
  @param ic_cmd
  Intelliciver command for which this handler function is called
 
  @param cmd_payload_size
  Size of the command payload
 
  @param cmd_ptr
  Pointer to the command packet corresponding to the current command
*/
void
rf_cdma_ic_handle_pm_override
(
  rf_cdma_ic_cmd_id_type ic_cmd, 
  rf_cdma_ic_cmd_payload_size_type cmd_payload_size, 
  rf_cdma_ic_cmd_type* cmd_ptr
)
{
  rf_cdma_ic_data_type *ic_data;
  rfc_cdma_logical_device_params_type *logical_dev;
  uint32 num_carriers = 0;
  rfm_cdma_band_class_type band = RFM_CDMA_MAX_BAND;
  rfm_cdma_chan_type channel_array[RFM_CDMA_CARRIER_NUM];
  rfm_cdma_power_mode_type power_mode = RFM_CDMA_POWER_MODE_INVALID;
  uint8 chan_idx = 0;
  rf_cdma_ic_override_pm_payload pm_payload;
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Write Pointer*/
  rf_cdma_state_type rf_state_b4_cmd_1x;
  rf_cdma_state_type rf_state_b4_cmd_hdr;
  boolean skip_pm_ovr = FALSE; /* Flag indicating if the pm override is to
  be skipped. This will be the case when radio goes to undesired state
  between the time when pm ovr command is posted and handled */
  boolean chan_state_same = TRUE; /* Flag indicating that the Radio channel
  is same between the time when pm ovr command is posted and handled. */
  size_t data_length = 0 ;

  data_length = sizeof(rf_cdma_ic_override_pm_payload) ;
  /* Copy the Payload into local data structure */  
  memscpy ( &pm_payload , data_length ,
            cmd_ptr->payload, data_length ) ;

  /* Get Intelliceiver data pointer */
  ic_data = (rf_cdma_ic_data_type*)(pm_payload.ic_data_addr);
  logical_dev = (rfc_cdma_logical_device_params_type*)pm_payload.logical_addr;
  num_carriers = pm_payload.num_carriers;
  band = pm_payload.band;
  power_mode = pm_payload.power_mode;
  rf_state_b4_cmd_1x = pm_payload.current_rf_state_1x;
  rf_state_b4_cmd_hdr = pm_payload.current_rf_state_hdr;

  if ( num_carriers > RFM_CDMA_CARRIER_NUM )
  {
    RF_MSG_2 ( RF_ERROR, "rf_cdma_ic_handle_pm_override: "
                         "num_carriers[%d] > RFM_CDMA_CARRIER_NUM[%d]" ,
                         num_carriers , RFM_CDMA_CARRIER_NUM ) ;
    return ;
  }

  for ( chan_idx = 0 ; chan_idx < num_carriers ; chan_idx++ )
  {
    channel_array[chan_idx] = pm_payload.channel_array[chan_idx];
  }

  /* NULL Pointer check  */
  if ( ( ic_data == NULL )  || ( logical_dev == NULL ) )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_handle_pm_override: NULL data" );
    return;
  }

  /* Get Current Device Status for the given Device and check whether the data 
  returned is valid. */
  dev_status_r = rf_cdma_get_device_status( ic_data->device );

  /* NULL Pointer check  */
  if ( dev_status_r == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_handle_pm_override: NULL data" );
    return;
  }

  /* Now try to acquire a common ciritcal lock for CDMA driver. This
  lock will be held, until the operation is complete. This lock will be held by 
  RF_IC_TASK until the PAUSE action is complete. */
  
  /* Enter Critical Section */
  rf_common_enter_critical_section( rf_cdma_get_lock_data());

  /* Check if the radio state has changed between Posting and handling
  of PM override. If there is a retune, then perform Power mode override
  based on new state. */

  /* Find if Channel are still same or not */
   for ( chan_idx = 0; 
         chan_idx < rf_limit_s32( num_carriers, 0, RFM_CDMA_CARRIER_NUM );
         chan_idx++ )
  {
    if ( pm_payload.channel_array[chan_idx] != 
         dev_status_r->curr_chans[chan_idx] )
    {
      chan_state_same &= FALSE;
    } /* f ( pm_payload.channel_array[chan_idx] != 
         dev_status_r->curr_chans[chan_idx] ) */
  } /*  for ( chan_idx = 0; 
        chan_idx < rf_limit_s32(num_carriers, 0, RFM_CDMA_CARRIER_NUM );
        chan_idx++ ) */

   /* Find if RF State and band are still same or not */
  if ( ( dev_status_r->data_1x.rf_state != rf_state_b4_cmd_1x ) ||
       ( dev_status_r->data_hdr.rf_state != rf_state_b4_cmd_hdr ) ||
       ( dev_status_r->curr_band != band ) ||
       ( chan_state_same != TRUE ) )
  {
     /* If radio state changed, new RF state should be Rx or RxTx  */
    if ( ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RX ) 
         ||
         ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RXTX )
         ||
         ( dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_RX ) 
         ||
         ( dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_RXTX ) )
    {
      band = dev_status_r->curr_band;
      for ( chan_idx = 0; 
            chan_idx < rf_limit_s32(num_carriers, 0, RFM_CDMA_CARRIER_NUM );
            chan_idx++ )
      {
        channel_array[chan_idx] = dev_status_r->curr_chans[chan_idx];
      } /* for ( chan_idx = 0; 
            chan_idx < rf_limit_s32(num_carriers, 0, RFM_CDMA_CARRIER_NUM );
            chan_idx++ ) */

      RF_MSG_7( RF_HIGH, "rf_cdma_ic_handle_pm_override: Dev %d - RF "
                "(1x State %d-> %d/hdr State %d -> %d, Band %d -> %d, Chan) changed. PM OVR for "
                "new STATE", ic_data->device, 
                dev_status_r->data_1x.rf_state, pm_payload.current_rf_state_1x,
                dev_status_r->data_hdr.rf_state, pm_payload.current_rf_state_hdr,
                band, pm_payload.band );

    }/* if ( ( dev_status_r->rf_state != RF_CDMA_STATE_RX ) ||
         ( dev_status_r->rf_state != RF_CDMA_STATE_RXTX ) ) */
    else
    {
      /* Since the RF State is not in Rx or RxTx, dont perform PM override */
      skip_pm_ovr = TRUE;
    }
  } /* if ( ( dev_status_r->rf_state != rf_state_b4_cmd ) ||
       ( dev_status_r->curr_band != band ) ||
       ( chan_state_same != TRUE ) ) */

  /* Perform PM override only if we dont need to skip */
  if ( skip_pm_ovr == FALSE )
  {
    /* Perform Power Mode override */
     if ( rf_cdma_ic_set_power_mode( ic_data, logical_dev, 
                                     num_carriers, band, channel_array,
                                     power_mode )
        )
     {
       ic_data->power_mode_override = TRUE;
     }
     else
     {
       RF_MSG_1( RF_ERROR, "rf_cdma_ic_handle_pm_override: Dev %d - "
                           "PM Override failed", ic_data->device );
     }
  } /* if ( skip_pm_ovr == FALSE ) */
  else
  {
    RF_MSG_3( RF_HIGH, "rf_cdma_ic_handle_pm_override: Dev %d, SKIPPED PM "
              "override since 1x RF State is %d/hdr RF State is %d", ic_data->device,
              dev_status_r->data_1x.rf_state, dev_status_r->data_hdr.rf_state );
  } /* if ! ( skip_pm_ovr == FALSE ) */

  /* Post the Semaphore indicating that PM Override action is complete */
  rfcommon_semaphore_post( &(ic_data->ic_sems.pm_ovr_complete) );

  /* Debug Message */  
  RF_MSG_2( RF_HIGH, "rf_cdma_ic_handle_pm_override: Dev %d - "
                     "Overriden PM %d", ic_data->device, power_mode );

  /* Leave Critical Section */
  rf_common_leave_critical_section( rf_cdma_get_lock_data() );
  
  return;
  
} /* rf_cdma_ic_handle_pm_override */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  RF ic Task Dispatcher function to clear power mode override
 
  @details
  This callback function will be executed when clear power mode override command
  is posted to RF iC Task
  
  @param ic_cmd
  Intelliciver command for which this handler function is called
 
  @param cmd_payload_size
  Size of the command payload
 
  @param cmd_ptr
  Pointer to the command packet corresponding to the current command
*/
void
rf_cdma_ic_handle_pm_override_clear
(
  rf_cdma_ic_cmd_id_type ic_cmd, 
  rf_cdma_ic_cmd_payload_size_type cmd_payload_size, 
  rf_cdma_ic_cmd_type* cmd_ptr
)
{
  rf_cdma_ic_data_type *ic_data;
  rf_cdma_ic_override_pm_clear_payload pm_payload;
  size_t data_length = 0 ;

  data_length= sizeof(rf_cdma_ic_override_pm_clear_payload) ;
  /* Copy the Payload into local data structure */  
  memscpy ( &pm_payload , data_length , 
            cmd_ptr->payload , data_length );

  /* Get Intelliceiver data pointer */
  ic_data = (rf_cdma_ic_data_type*)(pm_payload.ic_data_addr);

  /* NULL Pointer check  */
  if ( ic_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_handle_pm_override_clear: NULL data" );
    return;
  }

  /* Now try to acquire a common ciritcal lock for CDMA driver. This
  lock will be held, until the operation is complete. This lock will be held by 
  RF_IC_TASK until the PAUSE action is complete. */
  
  /* Enter Critical Section */
  rf_common_enter_critical_section( rf_cdma_get_lock_data());
  
  ic_data->power_mode_override = FALSE;
  ic_data->force_pm_update = TRUE;

  /* Post the Semaphore indicating that Clear PM Override action is complete */
  rfcommon_semaphore_post( &(ic_data->ic_sems.clear_pm_ovr_complete) );
  
  /* Debug Message */  
  RF_MSG_1( RF_HIGH, "rf_cdma_ic_handle_pm_override_clear: Dev %d - "
                     "Override cleared", ic_data->device );

  /* Leave Critical Section */
  rf_common_leave_critical_section( rf_cdma_get_lock_data() );

  return;
  
} /* rf_cdma_ic_handle_pm_override_clear */

/*! @} */

/*============================================================================*/
/*!
  @name RF FWRSP Task Handlers 

  @brief
  This section includes handler functions for RF_FWRSP_TASK to be executed when 
  intelliceiver Steady State enter and polling is to be done.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Intelliceiver Update Response handler
 
  @details
  This callback function is executed in second level dispatching when
  Intelliceiver Update Response is received from firmware. This function will basically post 
  the semaphore so that blocking call can be unblocked.
 
  @param snum_data
  Pointer to Sequence number data structure
 
  @return
  Flag indcating the status of handling of response
*/
boolean
rf_cdma_ic_update_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
)
{
  boolean ret_val = TRUE;
  rf_cdma_ic_update_rsp_data_type *cb_data = NULL;
  rf_fwrsp_cmd_type *req_ptr;
  /* Type cast to FWRSP type*/
  req_ptr = (rf_fwrsp_cmd_type*)cmd_ptr;

  /* NULL Pointer check */
  if ( ( req_ptr == NULL ) || ( snum_data == NULL ) )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_update_rsp_cb: NULL Argument(s)" );
    return FALSE;
  } /* if ( ( req_ptr == NULL ) || ( snum_data == NULL ) ) */

  /* Extract the callback data */
  cb_data = (rf_cdma_ic_update_rsp_data_type*)snum_data->snum_cb_data;

  RF_MSG_3( RF_MED, "[CDMA][FW->RF] IC_UPDATE_RSP 0x%x [ %d | %d ]", 
            snum_data->snum, 
            RFCOMMON_MDSP_SNUM_TECH(snum_data->snum),
            RFCOMMON_MDSP_SNUM_COUNT(snum_data->snum) );

  /* Post the Semaphore only if an active semaphore is available */
  if ( cb_data->blocking_call == TRUE )
  {
    if ( cb_data->sema_token != NULL )
    {
      ret_val &= rfcommon_semaphore_post( cb_data->sema_token );
    } /* if ( cb_data->sema_token != NULL ) */
    else
    {
      ret_val = FALSE;
      ERR_FATAL( "rf_cdma_ic_update_rsp_cb: NULL Sem Token", 0, 0, 0 );
    } /* if ! ( cb_data->sema_token != NULL ) */

    RF_MSG_4( RF_LOW, "rf_cdma_ic_update_rsp_cb: Dev %d - [%d] SNUM.0x%x "
              "Sem @ 0x%x [Done]", cb_data->device, snum_data->item_id, 
              snum_data->snum, cb_data->sema_token );

  } /* if ( cb_data->blocking_call == TRUE ) */
  else
  {
    RF_MSG_3( RF_LOW, "rf_cdma_ic_update_rsp_cb: Dev %d - [%d] SNUM.0x%x "
              "[Done]", cb_data->device, snum_data->item_id, snum_data->snum );
  } /* if ! ( cb_data->blocking_call == TRUE ) */

  return ret_val;

} /* rf_cdma_ic_update_rsp_cb */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Handler for iC Update Reponse received through RF FWRSP task
 
  @details
  This function is called when response to iC Update message is sent to RF
  FWRSP task. This function will basically post the semaphore so that blocking 
  call can be unblocked.
 
  @param req_ptr
  Pointer to RF FWRSP Task command
*/
void
rf_cdma_ic_handle_onex_ic_update
(
  rf_fwrsp_cmd_type* req_ptr
)
{
  rf_1x_fw_response_data_type *fwrsp_data = NULL;
  cfw_rx_config_rsp_info_t *rsp_payload;

  /* Get FW Response Data */
  fwrsp_data = rf_cdma_get_1x_fw_response_data();

  /* NULL Pointer Check */
  if ( fwrsp_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_handle_onex_ic_update: NULL FWRSP "
                      "Data" );
    return;
  } /* if ( fwrsp_data == NULL ) */

  /* Typecast the payload to cfw_rx_stop_rsp_msg_t */
  rsp_payload = (cfw_rx_config_rsp_info_t*)req_ptr->payload;

  RF_MSG_3( RF_MED, "[FW->SW] 1X_IC_UPDATE_RSP 0x%x [ %d | %d ]", 
            rsp_payload->sequence_number, 
            RFCOMMON_MDSP_SNUM_TECH(rsp_payload->sequence_number),
            RFCOMMON_MDSP_SNUM_COUNT(rsp_payload->sequence_number) );

  /* Post the Semaphore only if an active semaphore is available */
  if ( fwrsp_data->ic_update_rsp.sema_token != NULL )
  {
    rfcommon_semaphore_post( fwrsp_data->ic_update_rsp.sema_token );
  } /* if ( fwrsp_data->ic_update_rsp.sema_token != NULL ) */
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_handle_onex_ic_update: Inactive "
                      "Sem - Possible POST-BEFORE-WAIT" );
  } /* if ! ( fwrsp_data->tx_start_rsp.sema_token != NULL ) */

  RF_MSG( RF_LOW, "rf_cdma_ic_handle_onex_ic_update: Handled" );

  return;

} /* rf_cdma_ic_handle_onex_ic_update */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Handler for iC Update Reponse received through RF FWRSP task
 
  @details
  This function is called when response to iC Update message is sent to RF
  FWRSP task. This function will basically post the semaphore so that blocking 
  call can be unblocked.
 
  @param req_ptr
  Pointer to RF FWRSP Task command
*/
void
rf_cdma_ic_handle_hdr_ic_update
(
  rf_fwrsp_cmd_type* req_ptr
)
{
  rf_hdr_fw_response_data_type *fwrsp_data = NULL;
  cfw_rx_config_rsp_info_t *rsp_payload;

  /* Get FW Response Data */
  fwrsp_data = rf_cdma_get_hdr_fw_response_data();

  /* NULL Pointer Check */
  if ( fwrsp_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_handle_hdr_ic_update: NULL FWRSP "
                      "Data" );
    return;
  } /* if ( fwrsp_data == NULL ) */

  /* Typecast the payload to cfw_rx_stop_rsp_msg_t */
  rsp_payload = (cfw_rx_config_rsp_info_t*)req_ptr->payload;

  RF_MSG_3( RF_MED, "[FW->SW] HDR_IC_UPDATE_RSP 0x%x [ %d | %d ]", 
            rsp_payload->sequence_number, 
            RFCOMMON_MDSP_SNUM_TECH(rsp_payload->sequence_number),
            RFCOMMON_MDSP_SNUM_COUNT(rsp_payload->sequence_number) );

  /* Post the Semaphore only if an active semaphore is available */
  if ( fwrsp_data->ic_update_rsp.sema_token != NULL )
  {
    rfcommon_semaphore_post( fwrsp_data->ic_update_rsp.sema_token );
  } /* if ( fwrsp_data->ic_update_rsp.sema_token != NULL ) */
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_handle_hdr_ic_update: Inactive "
                      "Sem - Possible POST-BEFORE-WAIT" );
  } /* if ! ( fwrsp_data->tx_start_rsp.sema_token != NULL ) */


  RF_MSG( RF_LOW, "rf_cdma_ic_handle_hdr_ic_update: Handled" );

  return;

} /* rf_cdma_ic_handle_hdr_ic_update */

/*! @} */

/*============================================================================*/
/*!
  @name MC Interface

  @brief
  Functions used to control intelliceiver operation from Main Control
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Intelliceiver
  
  @details
  This will configure Intelliceiver driver state to a known value. This
  initialization process is hardware initialization.
 
  This will initialize all the intelliceiver state variables to reset state.
  It will create a doubly linked list to hold all Jammer items. This funciton
  will always force the start intelliceiver state to be low linearity.
 
  This function will not program hardware.
 
  @param device
  Device on which Intelliceiver is to be initialized
 
  @param logical_dev
  Pointer to Logical Device Parameter to be set during intelliceiver init
 
  @param ic_data
  Pointer to intelliceiver Data in RF CDMA data structure
 
  @return
  Status of Intelliceiver init
*/
boolean
rf_cdma_ic_init
(
  rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  rf_cdma_ic_data_type *ic_data
)
{
  rf_cdma_ic_jammer_item_type *jammer_list_item_ptr;
  boolean ic_supported = FALSE; /* Flag indicating if intelliceiver is supported */
  boolean ret_val = TRUE;
  boolean sem_init_res = TRUE; /* result of initializing semaphore */
  rfm_cdma_band_class_type band;

  /* Check if intelliceiver is supported for this device */
  for ( band = RFM_CDMA_BC0;
        band < RFM_CDMA_MAX_BAND;
        band++ )
  {
    /* Check if this band has a valid rx device pointer */
    if ( logical_dev->rx_device[band] != NULL )
    {
      /* If IC is supported for any band, it is enabled */
      if (rfdevice_cdma_is_ic_supported(logical_dev->rx_device[band]) == TRUE)
      {
        ic_supported = TRUE;
        break;
      }
    }/* if ( logical_dev->rx_device[band] != NULL ) */
    else
    {
      RF_MSG_2( RF_HIGH, "rf_cdma_ic_init: NULL Rx device for band: %d"
                         " on device :%d", band,device ); 
    }
  }/* for ( band = RFM_CDMA_BC0....*/
 
  if ( ( rf_cdma_debug_flags.use_icvr == FALSE ) ||
       ( rfc_cdma_is_tx_device ( device ) == FALSE ) )
  {
    ic_supported = FALSE;
  }
  else
  {
    RF_MSG_3 ( RF_HIGH, 
               "rf_cdma_ic_init: Dev %d - is not a Tx device or use_icvr unset "
               "||Dev: %d|use_cvr: %d|IS_DEVICE_TX?: %d|" ,
               device , rf_cdma_debug_flags.use_icvr ,
               rfc_cdma_is_tx_device ( device ) ) ;
  }

  /* Continue with initialization, only if the initialization is done for
  the first time */
  if ( ic_data->init_done != TRUE )
  {
    /* Update the Device */
    ic_data->device = device;

    /* Indicate that intelliceiver is initialized */
    ic_data->init_done = TRUE;

    /* Iinitialize to Stopped state */
    ic_data->ic_state = RF_CDMA_IC_STOPPED;
      
    /* Init and create a Doubly Linked linked list to hold Jammer Items*/
    rf_cdma_ic_init_jammer_list( ic_data );

    /* Initialize ic_timer_group */
    memset(&ic_data->ic_timer_group,0,sizeof(timer_group_type));

    /*Initialize pause_info */
    ic_data->pause_info.ic_state_before_pause = RF_CDMA_IC_INVALID ;
  
 
    /* Move one item from the free list into the used list as a placeholder
    for the "untuned" state the radio initially comes up in. */
    jammer_list_item_ptr = ic_data->jammer_sentinel_spare.next;
    rf_cdma_ic_remove_jammer_list_item( jammer_list_item_ptr );
    rf_cdma_ic_insert_jammer_list_item( &ic_data->jammer_sentinel_used,
                                        jammer_list_item_ptr );

    /* Based on intelliciever supported init following fields */
    if ( ic_supported == TRUE )
    {
      /* Flag that Intelliceiver is supported for this device */
      ic_data->ic_supported = TRUE;

      /* Update Inetlliceiver Power Mode */
      ic_data->power_mode = RFM_CDMA_POWER_MODE_1;
   
      /* Set this item to stay in Low linearity mode */
      jammer_list_item_ptr->jammer_entry.jammer_state = RFM_CDMA_POWER_MODE_1;
    }
    else
    {
      /* Flag that Intelliceiver is supported for this device */
      ic_data->ic_supported = FALSE;

      /* Set Inetlliceiver Power Mode to Normal for the devices for which
      intelliceiver is not supported */
      ic_data->power_mode = RFM_CDMA_POWER_MODE_NORMAL;
   
      /* Set this item to stay in High linearity mode */
      jammer_list_item_ptr->jammer_entry.jammer_state = 
                                              RFM_CDMA_POWER_MODE_NORMAL;

      RF_MSG_1( RF_MED, "rf_cdma_ic_init: Dev %d - Not supported", device );
    }

    /* Clear the Power Mode Override */
    ic_data->power_mode_override = FALSE;

    jammer_list_item_ptr->jammer_entry.high_lin_counter = 0;
    jammer_list_item_ptr->jammer_entry.mid_lin_counter = 0;
    jammer_list_item_ptr->jammer_entry.band = RFM_CDMA_MAX_BAND;
    jammer_list_item_ptr->jammer_entry.channel = 0;

    /* Update the Current Jammer Item */
    ic_data->curr_jammer_item = jammer_list_item_ptr;

    /* Initialize Timer with callback funciton to trigger steady state mode */
    timer_def( &ic_data->ic_timer,&ic_data->ic_timer_group, NULL, 0,
               rf_cdma_ic_schedule_enter_steady_state, 
               (timer_cb_data_type)ic_data );

    /* Disable debug F3 messages by default. For debugging purpose,
    this flag should be set to true using a debugger. */
    ic_data->dbg_data.f3_en = FALSE;

    /* Dont disable intelliceiver. For debugging purpose,
    this flag should be set to FALSE using a debugger. */
    ic_data->dbg_data.ic_disable = FALSE;

    /* Initialize the semaphore that is set when Intelliceiver update to FW
    is sent, and posted when the response is received from FW.
    Used only in the case of Power mode override */
    /* return value of 0 means success, -1 is a failure */
    //sem_init_res &= rfcommon_semaphore_create(&(ic_data->ic_sems.ic_update_rsp));
    sem_init_res &= rfcommon_semaphore_create(&(ic_data->ic_sems.pause_complete));
    sem_init_res &= rfcommon_semaphore_create(&(ic_data->ic_sems.resume_complete));
    sem_init_res &= rfcommon_semaphore_create(&(ic_data->ic_sems.pm_ovr_complete));
    sem_init_res &= 
          rfcommon_semaphore_create(&(ic_data->ic_sems.clear_pm_ovr_complete));
    
    /* ensure that sem_init call passed */
    if (sem_init_res == FALSE)
    {
      RF_MSG( RF_ERROR, "rf_cdma_ic_init: Semaphore Init failed!!!");
      ret_val = FALSE;
    }

    /* Init Critical Section */
    rf_common_init_critical_section( &(ic_data->crit_section) );
    
    RF_MSG_1( RF_HIGH, "rf_cdma_ic_init: Dev %d - Initialized", device );
  }
  else
  {
    RF_MSG_1( RF_HIGH, "rf_cdma_ic_init: Dev %d - Already initialized",device );
  }

  return ret_val;
} /* rf_cdma_ic_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize Intelliceiver
  
  @details
  This will release semaphores allocated in @ref rf_cdma_ic_init
 
  @param ic_data
  Pointer to intelliceiver Data in RF CDMA data structure
 
*/
void
rf_cdma_ic_deinit
(
  rf_cdma_ic_data_type *ic_data
)
{
  rfcommon_semaphore_destroy(&(ic_data->ic_sems.pause_complete));
  rfcommon_semaphore_destroy(&(ic_data->ic_sems.resume_complete));
  rfcommon_semaphore_destroy(&(ic_data->ic_sems.pm_ovr_complete));
  rfcommon_semaphore_destroy(&(ic_data->ic_sems.clear_pm_ovr_complete));
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Begin IntelliCeiver operation

  @details
  This function will start Intelliceiver Operation. Based on the starting
  linearity, which was calculated during "Init" or Stop Intelliceiver", RTR
  device will be programmed with the linearity mode.
 
  All the timers used for intelliceiver polling will also be initialized nd
  started in this function.
 
  This function is protected with critical section to make it thread safe.
  This function should also have a single return point to avoid deadlock 
  scenario.
 
  This function should be called only after current jammer pointer is updated.
  Current Jammer pointer will be updated when intelliceiver module is
  queried to get starting power mode. This will be done by calling
  <b>#rf_cdma_ic_get_starting_power_mode ()</b> function.
 
  @param logical_dev
  Pointer to Logical Device Parameter to be used during intelliceiver Start
 
  @param ic_data
  Pointer to intelliceiver Data in RF CDMA data structure
 
  @param band
  RF Band on which intelliceiver is to be started
 
  @param channel
  RF Channel on which intelliceiver is to be started
 
  @return
  Flag indicating if intelliceiver is started
*/
boolean
rf_cdma_ic_start
(
  rf_cdma_ic_data_type *ic_data,
  const rfc_cdma_logical_device_params_type *logical_dev,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type channel
)
{
  rf_cdma_ic_jammer_item_type *jammer_state_ptr;
  boolean ret_val = TRUE; /* return Value */
  const rfc_common_logical_device_params_type* cmn_logical_dev;
  rfm_mode_enum_type curr_mode; /* Current RFM Mode */

  /* Check if the intelliceiver is disabled for debug mode. If disabled,
  then dont allow iC to start. */
  if ( ic_data->dbg_data.ic_disable == TRUE )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_ic_start: Dev %d - iC Disabled for debug mode",
              ic_data->device );
    ret_val &= FALSE;
  }
  
  /* Intelliceiver should not be started in Calibration Mode. This check for
  calibration Mode and based on that continue the execution. */
  if ( rfm_get_calibration_state() == TRUE )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_ic_start: Dev %d - cannot start in Cal Mode",
              ic_data->device );
    ret_val &= FALSE;
  }

  /* Check intelliceiver state. This function can be called only when it is
  in Stopped state. */
  if ( ic_data->ic_state != RF_CDMA_IC_STOPPED )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_ic_start: Dev %d - Not in STOPPED state",
              ic_data->device );
    ret_val &= FALSE;
  }

  /* Perform NULL Pointer Check for Current Jammer Item */
  if ( ic_data->curr_jammer_item == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_ic_start: Dev %d - Curr jammer item in NULL",
              ic_data->device );
    ret_val &= FALSE;
  }

  /* Perform Power Mode Configuration Check. If device requests Fixed Power
  Mode operation, do not even start Intelliceiver */
  if (( ic_data->pm_config != RFDEVICE_CDMA_IC_ADAPTIVE_PM_CHIP ) &&
  	  ( ic_data->pm_config != RFDEVICE_CDMA_IC_ADAPTIVE_PM_WB_AGC))
  {
    RF_MSG_3( RF_ERROR, "rf_cdma_ic_start: Dev %d Band %d IC Not Started "
                       "because PM Config = %d",
              ic_data->device, band, ic_data->pm_config );
    ret_val &= FALSE;
  }

  /* Get pointer to common device pointer */
  cmn_logical_dev = rfc_common_get_logical_device_params(ic_data->device);
  
  /* NULL Pointer check */
  if ( cmn_logical_dev == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_ic_start: Dev %d, NULL Common device "
              "Data", ic_data->device );

    ret_val &= FALSE; /* Failure */
  } /* if ( cmn_logical_dev == NULL ) */
  
  if ( ret_val == TRUE )
  {
  
    /* Get the current Jammer State pointer */
    jammer_state_ptr = ic_data->curr_jammer_item;

    /* Get Current RFM Mode */
    curr_mode = rfm_get_current_mode(ic_data->device);
    
    /* If the starting Jammer State is High Lin then vote for Quiet mode */
    if ( jammer_state_ptr->jammer_entry.jammer_state == 
         RFM_CDMA_POWER_MODE_NORMAL )
    {
      rfc_set_quiet_mode(curr_mode, TRUE );
    }
    
    /* Update the intelliceiver state variable to reflect Starting state */
    ic_data->ic_state = RF_CDMA_IC_STARTING;
  
    /* PLL was tuned. During the PLL relock time, a false jammer might have been 
    triggered if TX power is high Avoid false intelliceiver state change by 
    resetting Jammer detector after lienarity change */
    if ( ic_data->pm_config == RFDEVICE_CDMA_IC_ADAPTIVE_PM_CHIP )
    {
      rfdevice_cdma_ic_reset_jammer_detector( logical_dev->rx_device[band] );
    }
    else if ( ic_data->pm_config == RFDEVICE_CDMA_IC_ADAPTIVE_PM_WB_AGC )
    {
      boolean wb_agc_algo_running_status = FALSE;	

        /* start WB AGC algo in mDSP if not already running */
      ret_val = rf_cdma_mdsp_get_jd_algo_running_status( ic_data->device,
                                               logical_dev->rx_mdsp_path,
                                                 &wb_agc_algo_running_status);
        if(ret_val == TRUE)
        {
          if(!wb_agc_algo_running_status)
          {
          /* Populate JD threshold values */
          ret_val &= rf_cdma_configure_jd_thresh( ic_data->device,
                                                 logical_dev, 
                                                 band );
          
          /* Start jammer detect algorithm */
          ret_val = rf_cdma_mdsp_jd_algo_start_stop_control( ic_data->device,
                                               logical_dev->rx_mdsp_path,
                                               TRUE /* Start algo */ );
          RF_MSG_1( RF_LOW, "rf_cdma_ic_start: WB AGC algo started ",
                    ret_val); 
          }
        }/*if(ret_val == TRUE)*/
        else
        {
          RF_MSG( RF_ERROR, "rf_cdma_ic_start" 
                            "WB AGC algo get running status has failed!!!");
        }
    }/* RFDEVICE_CDMA_IC_ADAPTIVE_PM_WB_AGC */
  
    /* During the short IntelliCeiver start period, force the jammer detectors 
    on; so a reading can be made if this is a quick paging timeline */

    /* Initialize Timer with callback funciton to trigger steady state mode */
    if ( ic_data->ic_state != RF_CDMA_IC_PAUSE )
    {
      timer_def( &ic_data->ic_timer,&ic_data->ic_timer_group, NULL, 0,
        rf_cdma_ic_schedule_enter_steady_state, (timer_cb_data_type)ic_data );
  
      /* Set the timer */
      timer_set( &(ic_data->ic_timer), RF_CDMA_IC_START_STEADY_TRIGGER_DELAY, 0,
                 T_MSEC );
    } /* if ( ic_data->ic_state != RF_CDMA_IC_PAUSE ) */
    else
    {
      RF_MSG_1( RF_MED, "rf_cdma_ic_start: Dev %d - iC is in PAUSE"
                        "state", ic_data->device );
    } /* if ! ( ic_data->ic_state != RF_CDMA_IC_PAUSE ) */

    RF_MSG_2( RF_HIGH, "rf_cdma_ic_start: Dev %d - Started with PM %d", 
              ic_data->device,
              ic_data->curr_jammer_item->jammer_entry.jammer_state );
  }
  
  return ret_val;

} /* rf_cdma_ic_start */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Terminate IntelliCeiver operation.

  @details
  This function will put intelliceiver state machine to stop state. It will
  clear all the timers used for intelliceiver polling. This function will also
  compute next intelliceiver linearity mode by querying the RTR driver and
  checking the high linearity and low linearity mode counter. The computed
  linearity mode value will be used to start intellicevier in future when
  "Start Intelliceiver" function is called.
 
  This function is protected with critical section to make it thread safe.
  This function should also have a single return point to avoid deadlock 
  scenario.
  
  @param logical_dev
  Pointer to Logical Device Parameter to be used during intelliceiver Stop
 
  @param ic_data
  Pointer to intelliceiver Data in RF CDMA data structure
 
  @return
  Flag indicating if intelliceiver is stopped
*/
boolean
rf_cdma_ic_stop
(
  rf_cdma_ic_data_type *ic_data,
  const rfc_cdma_logical_device_params_type *logical_dev
)
{
  rfm_cdma_power_mode_type jammer_state_at_shutdown;
  boolean ret_val = TRUE; /* return Value */
  boolean cont_ic_stop = TRUE;
  const rfc_common_logical_device_params_type* cmn_logical_dev;
  rfm_mode_enum_type curr_mode; /* Current RFM Mode */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Write Pointer*/
  
  /* Make sure ic_data pointer is not NULL */
  if ( ic_data == NULL  )
  {
    /* If IC data is NULL, throw error mssg and bail out */
    RF_MSG( RF_ERROR, "rf_cdma_ic_stop: IC Data is NULL !" );
    return FALSE;
  }

  /* Get Current RFM Mode */
  curr_mode = rfm_get_current_mode(ic_data->device);

  /* Check intelliceiver state. This function can be called only when it is
  not in Stopped state. */
  if ( ic_data->ic_state == RF_CDMA_IC_STOPPED )
  {
    /* Intelliceiver should not be started in Calibration Mode. This check for
    calibration Mode and based on that continue the execution. */
    if ( rfm_get_calibration_state() == TRUE )
    {
      RF_MSG_1( RF_HIGH, "rf_cdma_ic_stop: Dev %d - Cal Mode ic already "
                "stopped", ic_data->device );
      ret_val &= TRUE;
      cont_ic_stop &= FALSE;
    }
    else
    {
      RF_MSG_1( RF_ERROR, "rf_cdma_ic_stop: Dev %d - Already in STOPPED state",
                ic_data->device );
      ret_val &= FALSE; /* Return Failure */
      cont_ic_stop &= FALSE;
    }
  }/* if ( ic_data->ic_state == RF_CDMA_IC_STOPPED ) */

  /* Perform NULL Pointer Check for Current Jammer Item */
  if ( ic_data->curr_jammer_item == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_ic_stop: Dev %d - Curr jammer item is NULL",
              ic_data->device );
    ret_val &= FALSE; /* Failure */
    cont_ic_stop &= FALSE;
  }

  /* Get pointer to common device pointer */
  cmn_logical_dev = rfc_common_get_logical_device_params(ic_data->device);

  /* NULL Pointer check */
  if ( cmn_logical_dev == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_ic_stop: Dev %d, NULL Common device "
              "Data", ic_data->device );

    ret_val &= FALSE; /* Failure */
    cont_ic_stop &= FALSE;
  } /* if ( cmn_logical_dev == NULL ) */
  
  /* Get Current Device Status for the given Device and check whether the data 
  returned is valid. */
  dev_status_r = rf_cdma_get_device_status( ic_data->device );

  /* NULL Pointer check  */
  if ( dev_status_r == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_stop: NULL data" );
    cont_ic_stop &= FALSE;
  }

  if ( cont_ic_stop == TRUE )
  {
    /* During the startup phase, the jammer detector will be enabled, so
    a reading can be made.  This is not guarenteed in the running case,
    so assume that it is off and make no update. */
    if ( ic_data->pm_config == RFDEVICE_CDMA_IC_ADAPTIVE_PM_CHIP )
    {
      jammer_state_at_shutdown = 
          rfdevice_cdma_get_ic_state( 
                             logical_dev->rx_device[dev_status_r->curr_band] );
    }
    else if ( ic_data->pm_config == RFDEVICE_CDMA_IC_ADAPTIVE_PM_WB_AGC)
    {
      ret_val = 
       rf_cdma_ic_get_wbagc_jd_status( ic_data->device,
                                      logical_dev->rx_mdsp_path,
                                      &jammer_state_at_shutdown);
    } 
    else
    {
      /* Set to invalid power mode if neither is true*/
      jammer_state_at_shutdown = RFM_CDMA_POWER_MODE_INVALID;
    }

    /* Check for return value being TRUE or FALSE, If return value is FALSE
    at this point that implies that the jammer detect algo is disabled
    and we should not be computing the next state, as the 
    jammer_state_at_shutdown could have some garbage value */ 
    if ( ret_val == TRUE ) 
    {
      /* Update the IntelliCeiver linearity mode based on this reading */
      rf_cdma_ic_compute_next_state( ic_data, jammer_state_at_shutdown );
    }

    /* Remove vote for quiet mode */
    rfc_set_quiet_mode(curr_mode, FALSE );

    /* Clear the timers */
    (void)timer_clr( &(ic_data->ic_timer), T_NONE );
  
    /* Update the State variable to reflect Stopped intelliceiver state */
    ic_data->ic_state = RF_CDMA_IC_STOPPED;
  
    RF_MSG_2( RF_HIGH, "rf_cdma_ic_stop: Dev %d - Stopped with next PM %d", 
              ic_data->device,
              ic_data->curr_jammer_item->jammer_entry.jammer_state );

    /* Stop WB AGC algo in mDSP if supported and running */
    if(ic_data->pm_config == RFDEVICE_CDMA_IC_ADAPTIVE_PM_WB_AGC)
    {
      boolean wb_agc_algo_running_status = FALSE; 

      /* Disable jammer detect algo in HDR only */
      /* Check if Algo is running or not */
      ret_val = rf_cdma_mdsp_get_jd_algo_running_status( ic_data->device,
                                               logical_dev->rx_mdsp_path,
                                                 &wb_agc_algo_running_status);

        if(ret_val == FALSE)
        {
          RF_MSG( RF_ERROR, "rf_cdma_ic_stop: rf_hdr_mdsp_get_jd_algo_status API "
                            "failed!!");
        }

        if(wb_agc_algo_running_status == TRUE)
        {
        ret_val = rf_cdma_mdsp_jd_algo_start_stop_control( ic_data->device,
                                            logical_dev->rx_mdsp_path,
                                            FALSE /* Stop algo */);
          if(ret_val == FALSE)
          {
            RF_MSG( RF_ERROR, "rf_cdma_ic_stop: rf_hdr_mdsp_stop_jd_algo API "
                    "failed!!"); 
          }
        }/*if(wb_agc_algo_running_status == TRUE)*/
    } /* RFDEVICE_CDMA_IC_ADAPTIVE_PM_WB_AGC */
  }/*if ( cont_ic_stop == TRUE )*/
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_stop: Skipping all JD/Iceiver operations"
                      " since intelliceiver is already stopped "); 
  }/*if ( cont_ic_stop != TRUE )*/


  return ret_val; /* Return Success/Failure */
  
} /* rf_cdma_ic_stop */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Pause IntelliCeiver operation.

  @details
  This function will put intelliceiver state machine to pause state. It will
  clear all the timers used for intelliceiver polling.
 
  This function is protected with critical section to make it thread safe.
  This function should also have a single return point to avoid deadlock 
  scenario.
  
  @param ic_data
  Pointer to intelliceiver Data in RF CDMA data structure
  
  @return
  Flag indicating if intelliceiver is paused or not
*/
boolean
rf_cdma_ic_pause
(
  rf_cdma_ic_data_type *ic_data
)
{
  boolean ret_val = TRUE; /* return Value */
  boolean cont_ic_pause = TRUE;
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */

  RF_MSG( RF_MED, "rf_cdma_ic_pause: [START]" );
  
  /* Get Current Device Status for the given Device and check whether the data 
  returned is valid. */
  dev_status_r = rf_cdma_get_device_status( ic_data->device );

  /* NULL pointer check */
  if (dev_status_r == NULL )
  {   
    RF_MSG_1( RF_ERROR, "rf_cdma_ic_pause: NULL Device %d data ", 
              ic_data->device );
    return FALSE;
  } /* if (dev_status_r == NULL ) */
  
  /* Check intelliceiver state. This function can be called only when it is
  not in Stopped state. */
  if ( ic_data->ic_state == RF_CDMA_IC_STOPPED )
  {
    /* Intelliceiver should not be started in Calibration Mode. This check for
    calibration Mode and based on that continue the execution. */
    if ( rfm_get_calibration_state() == TRUE )
    {
      RF_MSG_1( RF_HIGH, "rf_cdma_ic_pause: Dev %d - Cal Mode iC stopped", 
                ic_data->device );
      ret_val &= TRUE;
      cont_ic_pause &= FALSE;
    } /* if ( rfm_get_calibration_state() == TRUE ) */
    else
    {
      RF_MSG_1( RF_ERROR, "rf_cdma_ic_pause: Dev %d - No PAUSE in STOP state",
                ic_data->device );
      ret_val &= FALSE; /* Return Failure */
      cont_ic_pause &= FALSE;
    } /* if ! ( rfm_get_calibration_state() == TRUE ) */
  } /* if ( ic_data->ic_state == RF_CDMA_IC_STOPPED ) */

  /* Check the CDMA radio state */
  if ( !( ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RX )
          ||
          ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RXTX )
          ||
          ( dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_RX )
          ||
          ( dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_RXTX ) ) )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_ic_pause: Dev %d - Not in Rx or RxTx",
              ic_data->device );
    ret_val &= FALSE; /* Return Failure */
    cont_ic_pause &= FALSE;
  } /* if ( ( dev_status_r->rf_state != RF_CDMA_STATE_RX ) &&
       ( dev_status_r->rf_state != RF_CDMA_STATE_RXTX ) ) */

  /* Proceed based on cont_ic_pause */
  if ( cont_ic_pause == TRUE )
  {
    rf_cdma_ic_cmd_type *pause_cmd;
    rf_cdma_ic_pause_payload pause_payload;

    /* Check the size of payload */
    if ( sizeof(rf_cdma_ic_pause_payload) > 
         RF_CDMA_IC_MAX_CMD_PAYLOAD_SIZE )
    {
      RF_MSG( RF_ERROR, "rf_cdma_ic_pause: Payload is too big!" );
      return FALSE;
    }

    pause_payload.ic_data_addr = (uint32)ic_data;

    /* Get the pointer to the free buffer of RF command queue. */
    if( ( pause_cmd = rf_cdma_get_command_buffer() ) == NULL)
    {
      RF_MSG( RF_ERROR, "rf_cdma_ic_pause: RF command queue is full!" );
      ret_val &= FALSE;
    } /* if( ( pause_cmd = rf_cdma_get_command_buffer() ) == NULL) */
    else
    {      
      size_t data_length = sizeof(rf_cdma_ic_pause_payload) ; 
      /* fill the cmd_id and module_id */
      pause_cmd->rex_hdr.cmd_hdr.cmd_id = RF_CDMA_IC_PAUSE_IC_C;
      pause_cmd->rex_hdr.cmd_hdr.module_id = MSGR_MODULE_RF_1X;
  
      /* Copy the payload */
      memscpy ( pause_cmd->payload , data_length ,
                &pause_payload , data_length ) ;
      
      /* place command to RF iC Task command queue */
      rf_cdma_ic_put_command( pause_cmd );

      ret_val &= TRUE;

    } /* if ! ( ( pause_cmd = rf_cdma_get_command_buffer() ) == NULL) */

    /* Wait for Semaphore only if the PAUSE command was posted */
    if ( ret_val == TRUE )
    {
      RF_MSG_1( RF_LOW, "rf_cdma_ic_pause : iC Pause - Start Wait Sem @ 0x%x", 
                &(ic_data->ic_sems.pause_complete) );
  
      /* Wait for PAUSE action to be completed before proceeding. */
      rfcommon_semaphore_wait(&(ic_data->ic_sems.pause_complete));

      RF_MSG_1( RF_HIGH, "rf_cdma_ic_pause : iC Pause - Sem @ 0x%x may be"
                "posted before the wait. ***** SO IGNORE THE ERROR MSG *****", 
                &(ic_data->ic_sems.pause_complete) );
    }
    
    /* Debug Message */  
    RF_MSG_1( RF_HIGH, "rf_cdma_ic_pause: Dev %d - [PAUSED]", ic_data->device );
  } /* if ( cont_ic_pause == TRUE ) */

  return ret_val; /* Return Success/Failure */
  
} /* rf_cdma_ic_pause */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Resume IntelliCeiver operation.

  @details
  This function will put intelliceiver state machine out to pause state. It will
  reset all the timers used for intelliceiver polling.
 
  @param ic_data
  Pointer to intelliceiver Data in RF CDMA data structure
  
  @return
  Flag indicating if intelliceiver is move out of paused state or not
*/
boolean
rf_cdma_ic_resume
(
  rf_cdma_ic_data_type *ic_data
)
{
  boolean ret_val = TRUE; /* return Value */
  boolean cont_ic_resume = TRUE;

  RF_MSG( RF_MED, "rf_cdma_ic_resume: [START]" );
  
  /* Check intelliceiver state. This function can be called only when it is
  not in Stopped state. */
  if ( ic_data->ic_state != RF_CDMA_IC_PAUSE )
  {
    /* Intelliceiver should not be started in Calibration Mode. This check for
    calibration Mode and based on that continue the execution. */
    if ( rfm_get_calibration_state() == TRUE )
    {
      RF_MSG_1( RF_HIGH, "rf_cdma_ic_resume: Dev %d - Cal Mode iC stopped", 
                ic_data->device );
      ret_val &= TRUE;
      cont_ic_resume &= FALSE;
    }
    else
    {
      RF_MSG_1( RF_ERROR, "rf_cdma_ic_resume: Dev %d - Need to be in PAUSE",
                ic_data->device );
      ret_val &= FALSE; /* Return Failure */
      cont_ic_resume &= FALSE;
    }
  } /* if ( ic_data->ic_state != RF_CDMA_IC_PAUSE ) */

  /* Proceed based on cont_ic_pause */
  if ( cont_ic_resume == TRUE )
  {
    rf_cdma_ic_cmd_type *resume_cmd;
    rf_cdma_ic_resume_payload resume_payload;

    /* Check the size of payload */
    if ( sizeof(rf_cdma_ic_resume_payload) > 
         RF_CDMA_IC_MAX_CMD_PAYLOAD_SIZE )
    {
      RF_MSG( RF_ERROR, "rf_cdma_ic_resume: Payload is too big!" );
      return FALSE;
    }

    resume_payload.ic_data_addr = (uint32)ic_data;

    /* Get the pointer to the free buffer of RF command queue. */
    if( ( resume_cmd = rf_cdma_get_command_buffer() ) == NULL)
    {
      RF_MSG( RF_ERROR, "rf_cdma_ic_resume: RF command queue is full!" );
      ret_val &= FALSE;
    } /* if( ( resume_cmd = rf_cdma_get_command_buffer() ) == NULL) */
    else
    {     
      size_t data_length = sizeof(rf_cdma_ic_resume_payload) ; 
      /* fill the cmd_id and module_id */
      resume_cmd->rex_hdr.cmd_hdr.cmd_id = RF_CDMA_IC_RESUME_IC_C;
      resume_cmd->rex_hdr.cmd_hdr.module_id = MSGR_MODULE_RF_1X;
  
      /* Copy the payload */
      memscpy( resume_cmd->payload, data_length ,
              &resume_payload ,data_length ) ;
      
      /* place command to RF iC Task command queue */
      rf_cdma_ic_put_command( resume_cmd );

      ret_val &= TRUE;

    } /* if ! ( ( pause_cmd = rf_cdma_get_command_buffer() ) == NULL) */

    /* Wait for Semaphore only if the PAUSE command was posted */
    if ( ret_val == TRUE )
    {
      RF_MSG_1( RF_LOW, "rf_cdma_ic_resume : iC Resume - Start Wait Sem @ 0x%x", 
                &(ic_data->ic_sems.resume_complete) );
  
      /* Wait for PAUSE action to be completed before proceeding. */
      rfcommon_semaphore_wait(&(ic_data->ic_sems.resume_complete));

      RF_MSG_1( RF_HIGH, "rf_cdma_ic_resume : iC Resume - Sem @ 0x%x may be"
                "posted before the wait. ***** SO IGNORE THE ERROR MSG *****", 
                &(ic_data->ic_sems.resume_complete) );

    }
    
    /* Debug Message */  
    RF_MSG_1( RF_HIGH, "rf_cdma_ic_resume: Dev %d - [RESUMED]", 
              ic_data->device );
  } /* if ( cont_ic_pause == TRUE ) */

  return ret_val; /* Return Success/Failure */
  
} /* rf_cdma_ic_resume */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Override intellicever Power Mode (<b>FTM Only</b>)

  @details
  This function pushes the intelliceiver calibration data to the device
  driver so that the the drive can calculate the Jammer detect threshold values.
  The difference of LNA G0 gain value between the calibration reference channel
  and the currrent channel is also sent to the device driver.
 
  Once the JD threshold is set, device is programmed with the requested
  override power mode.
 
  Once the intelliceiver is put in override mode, Power Mode will not be changed
  until override clear is called. In override mode, the intelliceiver will
  still be running but, it will not change the power mode.
 
  As a part of override PM, RxAGC will also be reconfigured for new Power mode. 
  When RxAGC configure message is sent to FW, then FW may reset the LNA state to 
  some default state. Thus if PM override function is called when LNA state is 
  in overridde state, LNA will then be in new default LNA state override mode. 
  However, if this function is called when LNA is in free-runnig mode, FW will 
  reset the LNA STM and start to converge to actual LNA state from the default
  LNA State.
 
  If the current intelliceiver state is not in stopped state, then make
  sure that the timer is cleared before overriding the power mode. Later,
  when the power mode is overriden, schedule the steady state timer. This
  shoould be scheduled because in override mode, the intellicever will still
  be operational but there will be no power mode update during polling.
 
  This function is protected with critical section to make it thread safe.
  This function should also have a single return point to avoid deadlock 
  scenario.
   
  @param logical_dev
  Pointer to the logical device parameter from RFC. This will be used to
  get the current device driver instance
 
  @param ic_data
  Pointer to the intelliceiver data
 
  @param num_carriers
  Number of carrier for which power mode is to be set
 
  @param band
  RF Band for which the power mode is to be overriden. Band information is
  required because all intelliceiver calibration data is in per band basis
 
  @param channel_array
  RF Chan array for which the power mode is to be overriden. Chan information is
  required because all intelliceiver calibration data is in per band basis
 
  @param curr_rf_state_1x
  The current rf state for 1x
 
  @param curr_rf_state_hdr
  The current rf state for hdr
 
  @param power_mode
  Power Mode to which the device is to be set
 
  @return
  Flag indicating if power mode override was successful
*/
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
)
{
  boolean ret_val = TRUE; /* Return Value */
  rf_cdma_ic_cmd_type *pm_update_cmd;
  rf_cdma_ic_override_pm_payload pm_update_payload;
  uint8 chan_idx;
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */

  RF_MSG( RF_MED, "rf_cdma_ic_power_mode_override: [START]" );

  /* Check the CDMA radio state */

  /* Get Current Device Status for the given Device and check whether the data 
  returned is valid. */
  dev_status_r = rf_cdma_get_device_status( ic_data->device );

  /* NULL pointer check */
  if (dev_status_r == NULL )
  {   
    RF_MSG_1( RF_ERROR, "rf_cdma_ic_power_mode_override: NULL Device %d data ", 
              ic_data->device );
    return FALSE;
  } /* if (dev_status_r == NULL ) */
  
  if ( ! ( ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RX ) 
           ||
           ( dev_status_r->data_1x.rf_state == RF_CDMA_STATE_RXTX )
           ||
           ( dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_RX ) 
           ||
           ( dev_status_r->data_hdr.rf_state == RF_CDMA_STATE_RXTX ) ) )
  {
    ret_val = FALSE ;
    RF_MSG_3 ( RF_ERROR , 
               "rf_cdma_ic_power_mode_override: Dev %d , [1x State: %d] [hdr State: %d] " 
               "Device not in RF_CDMA_STATE_RX or RXTX state" ,
               ic_data->device, dev_status_r->data_1x.rf_state, dev_status_r->data_hdr.rf_state ) ;
  }
  else
  {
    /* Check the size of payload */
    if ( sizeof(rf_cdma_ic_override_pm_payload) > 
         RF_CDMA_IC_MAX_CMD_PAYLOAD_SIZE )
    {
      RF_MSG( RF_ERROR, "rf_cdma_ic_power_mode_override: Payload is too big!" );
      return FALSE;
    }

    /* Memset the structure to zero before copying over required data */
    memset( &(pm_update_payload), 0,
            sizeof(rf_cdma_ic_override_pm_payload) );

    pm_update_payload.ic_data_addr = (uint32)ic_data;
    pm_update_payload.logical_addr = (uint32)logical_dev;
    pm_update_payload.num_carriers = num_carriers;
    pm_update_payload.band = band;
    pm_update_payload.power_mode = power_mode;
    for ( chan_idx = 0; 
          chan_idx < rf_limit_s32(num_carriers, 0, RFM_CDMA_CARRIER_NUM );
          chan_idx++ )
    {
      pm_update_payload.channel_array[chan_idx] = channel_array[chan_idx];
    }
    pm_update_payload.current_rf_state_1x = curr_rf_state_1x;
    pm_update_payload.current_rf_state_hdr = curr_rf_state_hdr;

    /* Get the pointer to the free buffer of RF command queue. */
    if( ( pm_update_cmd = rf_cdma_get_command_buffer() ) == NULL)
    {
      RF_MSG( RF_ERROR, "rf_cdma_ic_power_mode_override: RF command queue "
                        "is full!" );
      ret_val &= FALSE;
    } /* if( ( resume_cmd = rf_cdma_get_command_buffer() ) == NULL) */
    else
    {  
      size_t data_length = sizeof(rf_cdma_ic_override_pm_payload) ; 
      /* fill the cmd_id and module_id */
      pm_update_cmd->rex_hdr.cmd_hdr.cmd_id = RF_CDMA_IC_OVERRIDE_PM_C;
      pm_update_cmd->rex_hdr.cmd_hdr.module_id = MSGR_MODULE_RF_1X;

      /* Copy the payload */
      memscpy( pm_update_cmd->payload,  data_length ,
              &pm_update_payload , data_length ) ;

      /* place command to RF iC Task command queue */
      rf_cdma_ic_put_command( pm_update_cmd );

      RF_MSG_1( RF_LOW, "rf_cdma_ic_power_mode_override : PM ovr - "
                "Start Wait Sem @ 0x%x", &(ic_data->ic_sems.pm_ovr_complete) );

      /* Wait for PM override action to be completed before proceeding. */
      rfcommon_semaphore_wait(&(ic_data->ic_sems.pm_ovr_complete));

      ret_val &= TRUE;

    } /* if ! ( ( pause_cmd = rf_cdma_get_command_buffer() ) == NULL) */

    RF_MSG_2( RF_LOW, "rf_cdma_ic_power_mode_override: Dev %d - Overriden to "
                      "PM %d", ic_data->device, power_mode );
  }
  return ret_val;
} /* rf_cdma_ic_power_mode_override */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Clear intellicever Power Mode override (<b>FTM Only</b>)

  @details
  This function clears the override mode of intelliceiver. After this the
  Power Mode will be updated based on the detection of the Jammer.
 
  This function is protected with critical section to make it thread safe.
  This function should also have a single return point to avoid deadlock 
  scenario.
    
  @param ic_data
  Pointer to the intelliceiver data
*/
boolean
rf_cdma_ic_clear_power_mode_override
(
  rf_cdma_ic_data_type *ic_data
)
{
  rf_cdma_ic_cmd_type *pm_update_cmd;
  rf_cdma_ic_override_pm_clear_payload pm_update_payload;
  boolean ret_val = TRUE;
  
  /* Update Inetlliceiver Power Mode to No override state */

  /* Check the size of payload */
  if ( sizeof(rf_cdma_ic_override_pm_clear_payload) > 
       RF_CDMA_IC_MAX_CMD_PAYLOAD_SIZE )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_clear_power_mode_override: Payload is "
                      "too big!" );
    return FALSE;
  }

  pm_update_payload.ic_data_addr = (uint32)ic_data;

  /* Get the pointer to the free buffer of RF command queue. */
  if( ( pm_update_cmd = rf_cdma_get_command_buffer() ) == NULL)
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_clear_power_mode_override: RF command queue "
                      "is full!" );
    ret_val &= FALSE;
  } /* if( ( resume_cmd = rf_cdma_get_command_buffer() ) == NULL) */
  else
  {     
    size_t data_length = sizeof(rf_cdma_ic_override_pm_clear_payload) ; 
    /* fill the cmd_id and module_id */
    pm_update_cmd->rex_hdr.cmd_hdr.cmd_id = RF_CDMA_IC_CLEAR_OVERRIDE_PM_C;
    pm_update_cmd->rex_hdr.cmd_hdr.module_id = MSGR_MODULE_RF_1X;

    /* Copy the payload */
    memscpy ( pm_update_cmd->payload , data_length , 
              &pm_update_payload ,data_length ) ;

    /* place command to RF iC Task command queue */
    rf_cdma_ic_put_command( pm_update_cmd );

    ret_val &= TRUE;

    RF_MSG_1( RF_LOW, "rf_cdma_ic_power_mode_override : PM ovr - "
              "Start Wait Sem @ 0x%x", 
              &(ic_data->ic_sems.clear_pm_ovr_complete) );

    /* Wait for PM override action to be completed before proceeding. */
    rfcommon_semaphore_wait(&(ic_data->ic_sems.clear_pm_ovr_complete) );

  } /* if ! ( ( pause_cmd = rf_cdma_get_command_buffer() ) == NULL) */

  RF_MSG_1( RF_LOW, "rf_cdma_ic_clear_power_mode_override: Dev %d - cleared",
            ic_data->device );

  return ret_val;
} /* rf_cdma_ic_clear_power_mode_override */

/*! @} */

/*============================================================================*/
/*!
  @name Queries

  @brief
  Functions used to query intelliceiver
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get current Intelliceiver Power Mode for given Device
  
  @details
  This function will return the current power mode for given Device
 
  @param ic_data
  Pointer to intelliceiver data whose current power mode is queried

  @return
  Power Mode for given Device
*/
rfm_cdma_power_mode_type
rf_cdma_ic_get_current_power_mode
(
  rf_cdma_ic_data_type *ic_data
)
{
  rfm_cdma_power_mode_type power_mode; /* Power Mode */

  /* Return Inetlliceiver Power Mode */
  power_mode = ic_data->power_mode;

  RF_MSG_2( RF_LOW, "rf_cdma_ic_get_current_power_mode: Dev %d - PM %d",
            ic_data->device, power_mode );

  return power_mode;

} /* rf_cdma_ic_get_current_power_mode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Starting Intelliceiver Power Mode for given Device
  
  @details
  This function will return the starting power mode for given Device based on 
  the intelliceiver history for the device. This power mode will be used during 
  wakeup and tune sequence to start intelliceiver.
 
  This function will be update the current jammer item pointer to point to the
  item corresponding to given band and channel. If no such jammer item is
  found, then new jammer item will be initialized for the new band and channel.
  For new jammer item, the power mode will be initialized to LOW_LIN, if
  intelliceiver is supported on that device; if not supported, then it will be
  initialized to HIGH_LIN.
 
  After updating the current jammer pointer, the starting power mode is
  determined by looking at the jammer state corresponding to the current jammer
  item pointer.
 
  @param ic_data
  Pointer to intelliceiver data for which starting power mode is to be
  calculated

  @param band
  RF Band for which Intelliceiver Power Mode is queried
 
  @param channel
  RF channel for which Intelliceiver Power Mode is queried
 
  @param rx_bw_khz
  Rx bandwidth for which driver is requesting power mode configuration
 
  @return
  Starting Power Mode for given Device
*/
rfm_cdma_power_mode_type
rf_cdma_ic_get_starting_power_mode
(
  rf_cdma_ic_data_type *ic_data,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type channel,
  uint32 rx_bw_khz
)
{
  /* holds the power mode */
  rfm_cdma_power_mode_type power_mode;

  /* Get Logical Device Param */
  const rfc_cdma_logical_device_params_type *logical_dev = 
    rfc_cdma_get_logical_device_params( ic_data->device );

  if ( logical_dev == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_ic_get_starting_power_mode: Invalid logical "
                        "Data for dev %d", ic_data->device );
    return RFM_CDMA_POWER_MODE_MAX;
  }

  /* Obtain the power mode configuration from device */
  ic_data->pm_config = 
    rfdevice_cdma_get_ic_pm_config( logical_dev->rx_device[band],
                                    band, rx_bw_khz );

  /* Based on the Power Mode configuration returned by device,
  start intelliceiver if required in adaptive mode */
  if (( ic_data->pm_config == RFDEVICE_CDMA_IC_ADAPTIVE_PM_CHIP ) ||
   	  ( ic_data->pm_config == RFDEVICE_CDMA_IC_ADAPTIVE_PM_WB_AGC))
  {
    /* Update the Current Jammer Pointer for given band and channel */
    rf_cdma_ic_update_current_jammer_item( ic_data, band, channel );
  
    power_mode = ic_data->curr_jammer_item->jammer_entry.jammer_state;
  }
  /* If invalid or not adaptive, set Intelliceiver to a fixed power
  mode and stop its operation. IC start functionality will handle this */
  else if ( ic_data->pm_config == RFDEVICE_CDMA_IC_PM_CONFIG_INVALID ||
            ic_data->pm_config == RFDEVICE_CDMA_IC_FIXED_PM_0 )
  {
    power_mode = RFM_CDMA_POWER_MODE_NORMAL;
  }
  else if ( ic_data->pm_config == RFDEVICE_CDMA_IC_FIXED_PM_1 )
  {
      power_mode = RFM_CDMA_POWER_MODE_1;
  }
  else if ( ic_data->pm_config == RFDEVICE_CDMA_IC_FIXED_PM_2 )
  {
    power_mode = RFM_CDMA_POWER_MODE_2;
  }
  else
  {
    power_mode = RFM_CDMA_POWER_MODE_NORMAL;
  }

  /* Push the calibration Data pertaining to the override power mode
  to Device Driver only if Jammer detection is not through WBAGC algorithm */
  if ( ( ic_data->ic_supported == TRUE ) && 
	     ( ic_data->pm_config != RFDEVICE_CDMA_IC_ADAPTIVE_PM_WB_AGC) )
  {
    rf_cdma_ic_program_cal_data( ic_data, 
                                 logical_dev, 
                                 band, 
                                 channel,
                                 power_mode );
  }

  /* Update iC Power Mode State */
  ic_data->power_mode = power_mode;

  RF_MSG_3( RF_LOW, "rf_cdma_ic_get_starting_power_mode: Dev %d - "
                    "PM %d, PM Config %d", ic_data->device, power_mode,
            ic_data->pm_config );

  return power_mode;

} /* rf_cdma_ic_get_starting_power_mode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Intelliceiver Lowest Power Mode
  
  @details
  This function will return the lowest power mode available to be applied
  for a given Device. For the devices with intelliceiver support, lowest power
  mode will be LOW_LIN; whereas for the devices without intelliceiver
  support, lowest pwer will be NORMAL_LIN.
 
  @param ic_data
  Pointer to intelliceiver data for which lowest power mode is to be determined

  @return
  Lowest Power Mode for given Device
*/
rfm_cdma_power_mode_type
rf_cdma_ic_get_lowest_power_mode
(
  rf_cdma_ic_data_type *ic_data
)
{
  rfm_cdma_power_mode_type power_mode; /* Lowest Pwer Mode */

  /* check the power mode configuration of device */
  if((ic_data->pm_config == RFDEVICE_CDMA_IC_ADAPTIVE_PM_WB_AGC) &&
  	 (rfm_get_calibration_state() == TRUE ))
  {
    /*  WBAGC PM config indicates that device supports only one power mode
    and still Intelliceiver module is enabled. 
    Return lowest power mode as NOMAL_LIN for such cases to avoid 
    LNA calibration for both power modes though Intelliceiver is supported */
    power_mode = RFM_CDMA_POWER_MODE_NORMAL;
  }
  /* Based on if intelliceiver is supported, determine the lowest power
  mode for the device */
  else if ( ic_data->ic_supported == TRUE )
  {
    /* Lowest Power Mode is LOW_LIN for devices with iC supported */
      power_mode = RFM_CDMA_POWER_MODE_1;
  }
  else
  {
    /* Lowest Power Mode is NORMAL_LIN for devices with iutC supported */
    power_mode = RFM_CDMA_POWER_MODE_NORMAL;
  }

  RF_MSG_2( RF_LOW, "rf_cdma_ic_get_lowest_power_mode: Dev %d - PM %d",
            ic_data->device, power_mode );

  return power_mode;

} /* rf_cdma_ic_get_lowest_power_mode */

/*! @} */

/*============================================================================*/
/*!
  @name Debug

  @brief
  Functions used for intelliceiver debugging
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Configure Intelliceiver debug F3 messages (<b>FTM Only</b>)
 
  @details
  This function will set the intelliceiver variables to debug mode,
 
  @param ic_data
  Pointer to intelliceiver data for which debug mode is to be se
 
  @param debug_en
  Flag indicating, if the debug mode is to be enabled or not.
 
  @return
  Status indicating, if the debug mode is enabled or not.
*/
boolean
rf_cdma_ic_debug_configure_f3
(
  rf_cdma_ic_data_type *ic_data,
  boolean debug_en
)
{
  boolean ret_val = FALSE;

  if ( ic_data != NULL )
  {
    ic_data->dbg_data.f3_en = debug_en;
    RF_MSG_2( RF_LOW, "rf_cdma_ic_debug_configure_f3: Dev %d - Debug Mode %d", 
              ic_data->device, debug_en );
    ret_val = TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_debug_configure_f3: Failed to "
              "Configure debug mode" );
    ret_val = FALSE;
  }

  return ret_val;
} /* rf_cdma_ic_debug_configure_f3 */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Configure debug Mode by enabling or disabling the Intelliceiver
  operation (<b>FTM Only</b>)
 
  @details
  This function will enable or disable the intelliceiver operation for debug 
  mode on a given device
 
  @param ic_data
  Pointer to intelliceiver data for which debug mode is to be set
 
  @param disable_ic
  Flag indicating, if the intelliceiver is to be disabled or not.
 
  @return
  Status indicating, if the debug mode is enabled or not.
*/
boolean
rf_cdma_ic_debug_configure_ic
(
  rf_cdma_ic_data_type *ic_data,
  boolean disable_ic
)
{
  boolean ret_val = FALSE;

  if ( ic_data != NULL )
  {
    ic_data->dbg_data.ic_disable = disable_ic;
    RF_MSG_2( RF_LOW, "rf_cdma_ic_debug_configure_ic: Dev %d - Debug Mode %d", 
              ic_data->device, disable_ic );
    ret_val = TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_debug_configure_ic: Failed to "
              "Configure debug mode" );
    ret_val = FALSE;
  }

  return ret_val;
} /* rf_cdma_ic_debug_configure_ic */
#endif /* FEATURE_CDMA1X */
/*! @} */

/*! @} */
