#ifndef RF_CDMA_MDSP_H
#define RF_CDMA_MDSP_H

/*! 
  @file
  rf_cdma_mdsp.h
 
  @brief
  This module defines the function prototypes and defintions used by CDMA 
  RF-MDSP Internal(RF) interface.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_mdsp.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/15/14   vc      Added API rf_cdma_mdsp_abort_asd_events
05/23/14   sty     Added rf_cdma_mdsp_set_antenna()
04/17/14   JJ      Added api to read dac therm for temp comp
04/11/14   spa     Jammer detect algorithm support(for both 1x and HDR)
04/01/14   APU     PA params update hook-up for different RC/data rate configs.
03/18/14   APU     1> Have DM do the buffer deallocation.
                   2> Centralize allocation and association to 1 place.
03/05/14   sty     Added rflm_ccs_irat_intf.h to use RLFM defined irat 
                   interfaces instead of using FW interfaces
01/20/14   sty     remove deprecated api rf_cdma_mdsp_update_templates()
10/03/13   cd      Hook up PA switchpoint loading to RFLM
09/19/13   APU     MC changes to support DM.
08/16/13   ndb     Clean-up the SPI based antenna tuner support
06/03/13   sty     Deleted rf_cdma_mdsp_sanity_check API
05/09/13   sty     Added rf_cdma_mdsp_sanity_check()
04/01/13   spa     Migrated to RF Dispatch for RF-FW synchronization
03/21/13   zhw     Move to Antenna Tuner Open Loop common module
03/14/13   zhw     Ant tuner token FW interface support
01/12/13   cd      RxAGC LNA SM event configuration support
01/09/13   sty     deleted  RF_CDMA_MSDP_SNUM_* - unused
01/09/13   sty     Deleted rf_cdma_mdsp_get_sequence_number()
11/06/12   zhw     Beautify F3 messages. Make get_shared_mem_loc static
10/29/12   zhw     Support for query device_config from RFC and device driver
                   Remove hardcoding in Tx templates
10/23/12   spa     Added PA state machine param to update pa config
10/02/12   zhw     Support for PA state setting for SSBI/RFFE PA
09/28/12   zhw     Remove unused get_template_id API
09/26/12   zhw     Avoid using void pointer
09/21/12   zhw     Support PA scripting 
09/21/12   sty     Add rf_mode in rf_cdma_mdsp_get_device_buffer_index
09/17/12   sty     Changed order of args in rf_cdma_mdsp_configure_devices
09/14/12   spa     Removed refs to unsupported SPI antenna tuner scripts
09/12/12   spa     Removed featurization flag RF_HAS_CDMA_DIME_SUPPORT 
09/07/12   spa     Added rf mode argument to API rf_cdma_mdsp_configure_devices 
09/05/12   spa     Added API rf_cdma_mdsp_configure_devices 
08/17/12   aro     Added RF-FW Sync support on FWS Config comamnd
07/31/12   spa     Updated configure_temp-comp to use rf_hal_buffer interface 
07/24/12   spa     Added prototype for rf_cdma_mdsp_configure_temp_comp() 
07/17/12   sty     Added rf_cdma_mdsp_get_template_id() for tx templates
07/16/12   sty     Added rf_cdma_mdsp_update_templates()
                   Moved rf_cdma_mdsp_sum_data_type to rf_cdma_mdsp_types.h
07/13/12   zhw     Added new API for obtaining pointer to memory location for 
                   a particular buffer index. 
05/02/12   aro     Support for unique sequence number generation to be used for
                   RF-FW message synchronization
03/29/12   sty     Added rf_cdma_mdsp_get_device_buffer_index()
03/28/12   aro     Added callback function as a parameter of RxAGC resume
                   function
09/01/11   cd      Add APT support
                   - Adjust raw APT PDM value to match FW interface
07/21/11   cd      Functionality to convert dBm10 to RxAGC units
03/18/11   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "msgr_umid.h"
#include "rfcommon_locks.h"
#include "cdmafw_msg.h"
#include "rfm_mode_types.h"
#include "rf_cdma_mdsp_types.h"
#include "rfdevice_cmn_intf.h"
#include "rf_cdma_mdsp_ccs_events.h"
#include "rf_buffer_intf.h"
#include "rf_hal_buffer.h"
#include "rf_fwrsp_task.h"
#include "rf_dispatch_snum.h"
#include "rflm_ccs_irat_intf.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Event types that can be passed into CDMA-MDSP API callback handlers.

  @details
  Generic event types passed into call back handler when call back
  handler is called during MDSP configuration.
*/
typedef enum
{

  RF_CDMA_MDSP_EXECUTION_FAILURE = -1,
  /*!< Event indicating general execution failure */

  RF_CDMA_MDSP_EVENT_SUCCESS = 0,
  /*!< Event indicating that the RF-MDSP action is completed successfully */

} rf_cdma_mdsp_cb_event_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  CDMA MDSP interface callback type.

  @details
  Function signature of the callback expected by CDMA MDSP
  interface functions.  Callback functions are required to accept
  two parameters, to indiciate what event this callback invocation is
  responding to.
*/
typedef void (*rf_cdma_mdsp_cb_handler_type)( rf_cdma_mdsp_cb_event_type,
                                              void* );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to store firmware response handler and the opaque data related
  to the handler
*/
typedef struct
{

  rf_fwrsp_snum_handler_func_type rsp_handler;
  /*!< Call back function to be used when RF receives the response back 
  from firmware */

  void *handler_data;
  /*!< Pointer to opaque data to be used by response handler. */

} rf_cdma_mdsp_cb_param_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to store firmware response handler and the opaque data related
  to the handler
*/
typedef struct
{

  rf_dispatch_snum_handler_fp_type rsp_handler;
  /*!< Call back function to be used when RF receives the response back 
  from firmware */

  void *handler_data;
  /*!< Pointer to opaque data to be used by response handler. */

} rf_cdma_mdsp_response_cb_param_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data type that is passed to the callback when FW acknowledges
  executing a queueued tuner script.
*/
typedef struct rf_cdma_mdsp_atuner_update_cb_data_s
{
  rf_cdma_mdsp_cb_handler_type callback_func;

  void* userdata;

} rf_cdma_mdsp_atuner_update_cb_data_type;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mdsp_init
(
  void
);

/*----------------------------------------------------------------------------*/
int16
rf_cdma_mdsp_dbm10_to_txagc
(
  int16 pwr,
  int16 K10
);

/*----------------------------------------------------------------------------*/
int16
rf_cdma_mdsp_dbm10_to_rxagc
(
  int16 pwr
);

/*----------------------------------------------------------------------------*/
int16
rf_cdma_mdsp_apt_val_adjust
(
  uint32 raw_apt_val
);

/*----------------------------------------------------------------------------*/
uint32
rf_cdma_mdsp_get_device_buffer_index
(
  rfm_mode_enum_type rf_mode,
  uint32 lm_handle
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mdsp_configure_devices
(
  rfm_device_enum_type device,
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rfm_mode_enum_type rf_mode,
  uint32 event_id,
  rfcommon_mdsp_event_handle** event_handle,
  const rfcommon_mdsp_event_settings_type* event_settings,
  rf_buffer_intf *buf_obj_ptr,
  uint32 buffer_index,
  lm_handle_type lm_handle ,
  void* const userdata,
  const rf_cdma_mdsp_cb_handler_type callback
);

/*----------------------------------------------------------------------------*/
cfw_rf_event_infor_type *
rf_cdma_mdsp_get_dynamic_smem_ptr
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rfm_mode_enum_type rf_mode,
  uint32 event_id
) ;

/*----------------------------------------------------------------------------*/
rfmeas_event_infor_type *
rf_cdma_mdsp_get_shared_mem_loc
(
  rfm_mode_enum_type rf_mode,
  uint32 buffer_index
) ;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mdsp_config_pa_params
(
  rfm_mode_enum_type rfm_mode,
  uint32 txlm_handle,
  xpt_cfg_type xpt_cfg,
  rf_cdma_mdsp_pa_stm_config_type *pa_stm_data
);

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_mdsp_rflm_set_iqgain_params 
( 
   rfm_mode_enum_type                     mode ,
   rf_cdma_mdsp_tx_iq_gain_data_type*     rf_cdma_tx_iq_params ,
   uint8                                  size_iq_gain_params  
) ;


/*----------------------------------------------------------------------------*/
uint32*
rf_cdma_mdsp_get_tx_aol_buffers
(
   lm_handle_type txlm_handle
) ;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mdsp_configure_jd
(
  const rfm_device_enum_type device,
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_cdma_mdsp_wbagc_config_type *jd_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mdsp_jd_algo_start_stop_control
(
  const rfm_device_enum_type device,
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  boolean jd_algo_start_stop
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mdsp_jd_algo_freeze_unfreeze_control
(
  const rfm_device_enum_type device,
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  boolean jd_algo_freeze_unfreeze
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mdsp_check_for_jammer
(
  const rfm_device_enum_type device,
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  boolean *jammer_status
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mdsp_get_jd_algo_running_status
(
  const rfm_device_enum_type device,
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  boolean *jd_algo_running_status
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_mdsp_iref_therm_read
( 
  lm_handle_type txlm_handle
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mdsp_set_antenna
(
  const lm_handle_type rx_handle,
  const lm_handle_type tx_handle
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_mdsp_abort_asd_events
(
  const lm_handle_type rxlm_handle
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RF_CDMA_MDSP_H */

