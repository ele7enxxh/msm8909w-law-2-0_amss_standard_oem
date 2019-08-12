#ifndef RFCOMMON_ASDIV_MANAGER_H
#define RFCOMMON_ASDIV_MANAGER_H
/*!
  @file
  rfcommon_asdiv_manager.h

  @brief
  This file contains all declarations and definitions necessary to use the RF
  Antenna Switch Diversity 
*/

/*==============================================================================

  Copyright (c) 2013 -2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/mc/inc/rfcommon_asdiv_manager.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/01/14   as/dyc  Exposed ASDiv manager lock and release APIs
11/18/14   pk      Added "update_buffer_ptr" in rfcommon_asdiv_rsp_cb_data_type
07/29/14   zhh     create rfcommon_asdiv_get_previous_position API
03/14/14   kg      Porting changes from Dime 
02/21/14   hdz     Added AsDiv support
10/23/13   sg      Added API to check if ASDiv is supported in the card being used
09/04/13   pl      Added new interface to support init of default state and 
                   querying of switch in progress
08/13/13   sml     New API to ack Antenna switch complete and updating
                   asdiv_manager_state only
07/19/13   aro     Featurized ASDIV
07/17/13   aro     Moved the L1 callback definition to rfm.c
07/15/13   aro     Interface to query current antenna position
07/15/13   aro     Extern C
07/13/13   aro     Doxygen compatibility added
07/13/13   aro/pl  Consolidated all asdiv functionality to the module
06/06/13   pl      Initial version

============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfm_mode_types.h"
#include "rf_buffer_intf.h"
#include "rfcommon_mdsp_event_intf.h"
#include "rfm.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(FEATURE_RF_ASDIV) || defined(FEATURE_RF_HAS_TPTUNER_SWITCH)
/*----------------------------------------------------------------------------*/
/*! Type definition for antenna position */
typedef uint32 rfcommon_asdiv_position_type;

/*!
  @brief
  Enum for ASDiv switch position allowed
  
  @detail
  Assumed maximum of 2 position allowed, where 0 is considered as default position, 
  the pass through position. 
*/
typedef enum
{
  ASDIV_POSITION_0 = 0,
  ASDIV_POSITION_1 = 1,
  ASDIV_POSITION_MAX =2
} rfcommon_asdiv_position_enum_type;

/*----------------------------------------------------------------------------*/
/*! Function pointer defintion to get ASM script */
typedef boolean (*rfcommon_asdiv_asm_config_cb_type)
(
  rfcommon_asdiv_position_type to_pos, /*!< New Antenna Position */
  rf_buffer_intf *script_ptr, /*!< pointer to buffer to store Script */
  void *cb_data /*!< Callback data to be passed to the function */
);

/*----------------------------------------------------------------------------*/
/*! Function pointer defintion to send script */
typedef boolean (*rfcommon_asdiv_send_script_cb_type)
(
  rfm_device_enum_type device, /*!< RFM device for which script it to be sent */
  rfm_mode_enum_type rfm_mode, /*!< RFM mode for which script it to be sent */
  rfcommon_asdiv_position_type antenna_position, /*!< Antenna Position */
  rf_buffer_intf *script_ptr, /*!< Pointer to buffer containing script */
  uint32 asdiv_handle_id /*!< handle_id of the handle that stores AsDiv script  */
);

/*----------------------------------------------------------------------------*/
/*! Function pointer defintion to notify the switch has started */
typedef boolean (*rfcommon_asdiv_switch_start_notify_cb_type)
(
 void *cb_data /*!< Callback data to be passed to the function */
);

/*----------------------------------------------------------------------------*/
/*! Function pointer defintion to update tune code */
typedef void (*rfcommon_asdiv_update_tuner_cb_type)
(
 uint8 cb_data /*!< Callback data to be passed to the function */
);

/*----------------------------------------------------------------------------*/
/*! Structure to hold data to be used to get ASM script */
typedef struct
{
  rfcommon_asdiv_asm_config_cb_type cb_func;
  /*!< Function pointer to be called to create ASM script */

  void *cb_data;
  /*!< Callback Data to be used along with callback function */

} rfcommon_asdiv_asm_conifg_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold data to be used to send script */
typedef struct
{
  rfcommon_asdiv_send_script_cb_type cb_func;
  /*!< Function pointer to be called to send script */

  void *cb_data;
  /*!< Callback Data to be used along with callback function */

} rfcommon_asdiv_send_script_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold data to be used to send notification for starting of 
switching process */
typedef struct
{
  rfcommon_asdiv_switch_start_notify_cb_type cb_func;
  /*!< Function pointer to be called to notify switch start */

  void *cb_data;
  /*!< Callback Data to be used along with callback function */

} rfcommon_asdiv_switch_start_notify_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold data to be used to call update tune code */
typedef struct
{
  rfcommon_asdiv_update_tuner_cb_type cb_func;
  /*!< Function pointer to be called to send script */

  uint8 cb_data;
  /*!< Callback Data to be used along with callback function */

} rfcommon_asdiv_update_tuner_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold antenna switch diversity call back data to be used
  when switch completion response is received from FW.
*/
typedef struct
{
  rfm_device_enum_type device;
  /*!< RFM device on which the switch is done */

  rfm_mode_enum_type rfm_mode;
  /*! RFM mode for which the switch is done */

  rfcommon_asdiv_position_type ant_position;
  /*!< New antenna position */

  rfcommon_mdsp_event_handle* asdiv_ccs_event_handle;
  /*!< Pointer to CCS event handle for asdiv CCS event*/

  void* update_buffer_ptr;
  /*!< Pointer to AsDiv Handle */ 

} rfcommon_asdiv_rsp_cb_data_type;

/*----------------------------------------------------------------------------*/
boolean
rfcommon_asdiv_manager_state_ack
( 
  void 
);

/*----------------------------------------------------------------------------*/
boolean 
rfcommon_asdiv_manager_init
(
  void
);

/*----------------------------------------------------------------------------*/
boolean 
rfcommon_asdiv_manager_deinit
(
  void
);

/*----------------------------------------------------------------------------*/
boolean 
rfcommon_asdiv_manager_config_switch
(
  rfm_device_enum_type device,
  rfm_mode_enum_type rfm_mode,
  rfcommon_asdiv_position_type antenna_position,
  rf_buffer_intf *switch_script_ptr,
  rf_buffer_intf *tuner_script_ptr,
  rfcommon_asdiv_asm_conifg_type get_asm_script_cb,
  rfcommon_asdiv_send_script_type send_script_cb,
  rfcommon_asdiv_switch_start_notify_type switch_start_cb,
  rfcommon_asdiv_update_tuner_type update_tuner_cb,
  rfm_l1_cb_type l1_cb_func,
  void *l1_cb_data,
  boolean gen_script_only,
  boolean immediate,
  int16 tuner_start_delta,
  uint32 rxlm_handle
);

/*----------------------------------------------------------------------------*/
boolean 
rfcommon_asdiv_manager_abort_switch
(
  rfm_device_enum_type device,
  rfm_mode_enum_type rfm_mode,
  void *asdiv_atuner_ptr
);

/*----------------------------------------------------------------------------*/
boolean 
rfcommon_asdiv_manager_send_script
( 
   rfm_device_enum_type device,
   rfm_mode_enum_type rfm_mode,
   rfcommon_asdiv_position_type antenna_position,
   rf_buffer_intf *asdiv_script,
   uint32 rxlm_handle
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_asdiv_manager_cleanup
( 
  void *registered_cb_data
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_asdiv_is_switch_in_progress
(
  void
);

/*----------------------------------------------------------------------------*/
void
rfcommon_asdiv_init_switch_state
(
   rfcommon_asdiv_position_type antenna_position
);

/*----------------------------------------------------------------------------*/
boolean rfcommon_asdiv_is_supported(void);

/*----------------------------------------------------------------------------*/
rfcommon_asdiv_position_type
rfcommon_asdiv_get_current_position
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
rfcommon_asdiv_set_current_position
(
  rfcommon_asdiv_position_type pos
);

/*----------------------------------------------------------------------------*/
void rfcommon_asdiv_update_tune_code_timer_init
(
void
);

/*----------------------------------------------------------------------------*/
void rfcommon_asdiv_update_tune_code_timer_trigger
(
  uint32 t_unused,
  timer_cb_data_type data
);

/*----------------------------------------------------------------------------*/
rfcommon_asdiv_position_type
rfcommon_asdiv_get_previous_position
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void rfcommon_asdiv_manager_enter_lock(void);

/*----------------------------------------------------------------------------*/
void rfcommon_asdiv_manager_release_lock(void);

/*----------------------------------------------------------------------------*/
boolean rfcommon_asdiv_manager_try_enter_lock(void);

/*----------------------------------------------------------------------------*/

#endif /*defined(FEATURE_RF_ASDIV) || defined(FEATURE_RF_HAS_TPTUNER_SWITCH) */

#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_ASDIV_MANAGER_H */


