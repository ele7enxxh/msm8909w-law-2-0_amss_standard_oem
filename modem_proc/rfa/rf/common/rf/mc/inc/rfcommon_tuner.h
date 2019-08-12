#ifndef RFCOMMON_TUNER_H
#define RFCOMMON_TUNER_H

/*!
   @file
   rfcommon_tuner.h

   @brief
   Tuner Update Events (AOL/CL) MC layer APIs

   @details
   - callback data create API
   - Call back API for AOL event complete
   - AOL/CL event clean up API
   
*/

/*===========================================================================
Copyright (c) 2008 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/mc/inc/rfcommon_tuner.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/28/14   ndb/yb  SNUM debug code 
09/30/14   ndb     Add "rflm_handle" in rfcommon_tuner_update_cb_data_type
08/28/14   kg      Add tech_id in rfcommon_tuner_update_cb_data_type()
3/21/14    kg      Change rfcommon_tuner_device_data_type to be able to accomodate upto 4 tuners 
01/04/13   zhw     Common helper API for callback data creation
12/03/13   zhw     Initial Revision.

============================================================================*/
#include "rfcommon_atuner_intf.h"
#include "rfcommon_mdsp_tuner.h"
#include "rfc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RFCOMMON_TUNER_CLOSED_LOOP_MEAS_MAX                                   10

/*!
  @brief
  Max size of each tune code value. 

  This is the max size (in bytes)of each tune code value that is currently supported.
*/
#define RFCOMMON_TUNER_CLOSED_LOOP_MAX_TUNE_CODE_SIZE                          8

typedef enum
{
  RF_TUNER_UPDATE_CHAIN_0,
  RF_TUNER_UPDATE_CHAIN_1,
  RF_TUNER_UPDATE_CHAIN_NUM
} rfcommon_tuner_chain_num_type;

typedef struct
{
  /*!< Pointer to antenna tuner manager device object */
  rfcommon_atuner_intf *ant_tuner;

  void* return_loss_tuner;

  /*!< Pointer to tuner script token object*/
  script_token_type tuner_token;

  /*!< scenario in use*/
  uint8 scenario_in_use;

  /*!< nv tuner code  in use*/
  uint8 nv_tune_code_in_use[RFCOMMON_TUNER_CLOSED_LOOP_MAX_TUNE_CODE_SIZE]; 

  /*!< size of each tune code*/
  uint8 tune_code_size;

}rfcommon_tuner_device_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold antenna tuner code update call back data
*/
typedef struct
{
  void* tuner_update_buffer_ptr;

  /*!< Pointer to CCS event handle for tuner CCS event*/
  rfcommon_mdsp_event_handle* tuner_ccs_event_handle;

  /*!< An array that holds tuner_dev pointer and token for all the tuners associated with the event*/
  rfcommon_tuner_device_data_type tuner_data[RFC_MAX_DEVICE_INSTANCES_PER_BAND-1];

  rfm_mode_enum_type tech_id;

  uint32 handle_id;

} rfcommon_tuner_update_cb_data_type;

/*----------------------------------------------------------------------------*/
rfcommon_tuner_update_cb_data_type*
rfcommon_tuner_event_create_cb_data
( 
  void* tuner_event_buf,
  rfm_mode_enum_type tech_id,
  uint32 handle_id
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_tuner_open_loop_response_handler
( 
  void *cmd_ptr,
  const rf_dispatch_snum_info_type *snum_info
);

/*----------------------------------------------------------------------------*/

void rfcommon_aol_tuner_snum_debug_callback(
  /*Client info passed in to be passed back to callback function*/
  void *client_debug_info, 
  /* Snum debug information */
  rf_dispatch_snum_debug_info snum_debug_info
);

/*----------------------------------------------------------------------------*/

void rfcommon_cl_tuner_snum_debug_callback(
  /*Client info passed in to be passed back to callback function*/
void *client_debug_info, 
  /* Snum debug information */
  rf_dispatch_snum_debug_info snum_debug_info
);

/*----------------------------------------------------------------------------*/
void
rfcommon_tuner_event_cleanup
( 
  void *registered_cb_data
);

#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_TUNER_H */


