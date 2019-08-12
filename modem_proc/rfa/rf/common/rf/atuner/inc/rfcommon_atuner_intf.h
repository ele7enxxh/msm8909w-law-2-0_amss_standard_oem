#ifndef RFCOMMON_ATUNER_INTF_H
#define RFCOMMON_ATUNER_INTF_H
/*!
  @file
  rfcommon_atuner_intf.h
  
  @brief
  Declaration of antenna tuner interface class.
  This header file contains all type common logic and implementation related to
  antenna tuner (atuner).
  This file the provides the interfaces for all the antenna tuner
  configuration supported in the current RF card and all the clients/callers
  (like mc/core/ftm etc) should use the APIs defined in this file
  to interface with any antenna tuner functionality.
  The APIs provided in this file are the interface APIs to program the
  tuner device available on the phone.
  
*/

/*===========================================================================

  Copyright (c) 2013-2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/atuner/inc/rfcommon_atuner_intf.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/23/16   hm      increasing the chan_num size to 32 bit to support LTE-B66
11/18/14   aca     AOL helper function
10/21/14   yb      Added priority configuratoin for CRAT scenarios
10/07/14   ndb     Added 3DL CA support(Phase 2)
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
07/28/14   yb      MCL feature: APIs added - stop_mcl
07/25/14   yb      MCL feature: APIs added - set_scenario_mcl
07/23/14   yb      MCL feature: APIs added - get_scenario_list
3/26/14    kg      Added support for LM handle 
11/29/13   yb      Removed low power API
10/28/13   vb      Support for force_detune()
09/27/13   yb      Added tune_code_override API
08/06/13   vb      Optimized update_tune_code() 
07/03/13   hm      Added tuner_low_power API
06/27/13   ndb     Added rfcommon_nv_ant_tuner_cl_ctrl_type in tuner tx mode 
06/27/13   ndb     Added rfcommon_nv_ant_tuner_cl_ctrl_type in tuner tx mode 
06/20/13   vb      Test case for CL call back registration function
06/12/13   vb      Added new method tx_disable()
05/25/13   vb      ATUNER_INVALID_TOKEN_VAL support
05/02/13   vb      init version.
============================================================================*/


#ifdef __cplusplus
extern "C"
{
#endif
#include "rfdevice_cmn_type_defs.h"
#include "rfcommon_nv.h"
#include "rfdevice_antenna_tuner.h"
#ifdef __cplusplus
}
#endif

//Safe C++ headers
#include "rf_buffer_intf.h"

//Token type for all the tuner scripts being provided
typedef uint32 script_token_type;

// Function protoype for CL event register call back
typedef void (*rf_cl_event_register_cb_fn)( void *class_obj_ptr );

// Structure to hold the CL NV data & LM handle 
typedef struct
{
  rfcommon_nv_ant_tuner_cl_ctrl_type *cl_nv_ctrl_info;
  uint32 lm_handle;
}rfcommon_atuner_cl_ctrl_info_type;

/*!
  @brief
  Enum for tuner CA ports allowed
  
  @detail
  Port numbers to be associated with a port object.
 
  In the current design it is assumed that at max there are
  three active radios which would communicate with the Tuner module.
  Hence maximum number of ports allowed are hardcoded to 3.
*/
typedef enum
{
  ATUNER_CA_PORT_0 = 0,
  ATUNER_CA_PORT_1 = 1,
  ATUNER_CA_PORT_2 = 2,
  ATUNER_CA_PORT_MAX =3
} rfcommon_atuner_ca_port_enum_type;

/*!
  @brief
   Enum to capture the return status of the  tuner override
  
  @detail
*/
typedef enum
{
  RFCMN_ATUNER_OVERRIDE_TUNE_CODE_SUCCESS,
  RFCMN_ATUNER_NOT_PRESENT,
  RFCMN_ATUNER_TUNE_CODE_FAILURE,
  RFCMN_ATUNER_ERROR = 0xFFFFFFFF
} rfcommon_atuner_override_status_type;

/*!
  @brief
  CA port number to be associated with a port object
*/
typedef uint8 rfcommon_atuner_ca_port_type;

/*!
  @brief
  Invalid token value. 

  If Tuner APIs return this token value, then the callers need not ack and assume
  that no tuner device script is provided part of the call.
*/
#define ATUNER_INVALID_TOKEN_VAL 0xFFFFFFFF

#ifdef __cplusplus

/* These header files are only valid for C++ compilation */
#include "rfa.h"
#include "rfdevice_class.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface for antenna tuner class that contains all the APIs for both of
  Advanced Open Loop (AOL) and Closed Loop (CL) algorithms of antenna tuner.
 
  @details
*/
class rfcommon_atuner_intf : public rfdevice_class
{
public:

  /*! Empty virtual destructor - base, interface class has no data so nothing 
  to delete */
  virtual ~rfcommon_atuner_intf()
  { }

  /* Initializes the Tuner */
  virtual boolean init(rfcom_mode_enum_type mode,
                       rf_device_execution_type dev_action,
                       rf_buffer_intf *script_buffer,
                       int16 script_timing,
                       script_token_type *script_token) = 0;

  /* Programs the associated antenna tuner in Rx only mode and 
     applies best possible tune code for Rx only mode */
  virtual boolean set_rx_mode(rfcom_antenna_tuner_priority_cfg priority_cfg,
                              rfcom_mode_enum_type mode,
                              rfcom_band_type_u band,
                              uint32 chan_num,
                              rf_device_execution_type dev_action,
                              rf_buffer_intf *script_buffer,
                              int16 script_timing,
                              script_token_type *script_token) = 0;

  /* Programs the associated tuner in Tx mode and
     applies best possible tune code for Tx mode */
  virtual boolean set_tx_mode(rfcom_mode_enum_type mode,
                              rfcom_band_type_u band,
                              uint32 chan_num,
                              rf_device_execution_type dev_action,
                              rf_buffer_intf *script_buffer,
                              int16 script_timing,
                              script_token_type *script_token,
                              rfcommon_atuner_cl_ctrl_info_type *cl_ctrl_info) = 0;

  /* Update the tune code every 200ms */
  virtual boolean update_tune_code(rfcom_mode_enum_type mode,
                                   rf_device_execution_type dev_action,
                                   rf_buffer_intf *script_buffer,
                                   int16 script_timing,
                                   script_token_type *script_token) = 0;

  /* Update the tune code every 200ms */
  virtual boolean force_detune(rfcom_mode_enum_type mode,
                               rf_device_execution_type dev_action,
                               rf_buffer_intf *script_buffer,
                               int16 script_timing,
                               script_token_type *script_token) = 0;

  /* Acknowlegement for the script previously provided */
  virtual boolean acknowledge_script(script_token_type script_token) = 0;


  /* Disables the tuner Tx mode */
  virtual boolean tx_disable( rfcom_mode_enum_type mode,
                              rfcom_band_type_u band,
                              uint32 chan_num,
                              rf_device_execution_type dev_action,
                              rf_buffer_intf *script_buffer,
                              int16 script_timing,
                              script_token_type *script_token ) = 0;

  /* Registers the call back func for updates on when CL event is scheduled */
  virtual boolean register_cl_cb_fn ( rfcom_mode_enum_type mode, 
                                      rf_cl_event_register_cb_fn cb_func, 
                                      void *obj_ptr ) = 0;

  /* Disables the associated tuner */
  virtual boolean disable(rf_device_execution_type dev_action,
                          rf_buffer_intf *script_buffer,
                          int16 script_timing,
                          script_token_type *script_token) = 0;

  virtual rfdevice_antenna_tuner_device_override_status_type tune_code_override (uint8  override_flag,
                                                                                 void*  data,
                                                                                 uint8* tuner_nv_ptr,
                                                                                 uint8* tuner_id_ptr) = 0;

  /* API for getting scenario list for given tech and band */
  virtual boolean get_scenario_list( rfcom_mode_enum_type mode,
                                     rfcom_band_type_u band,
                                     rfm_antenna_tuner_scenario_data *scenario_data,
                                     uint8 *free_space_index,
                                     rfm_tuner_handle_type *tuner_handle ) = 0;

  /* API to set scenario for mcl */
  virtual boolean set_scenario_mcl( uint8 scenario_val,
                            rfm_tuner_handle_type tuner_handle ) = 0;

  /* API to stop mcl */
  virtual boolean stop_mcl( rfm_tuner_handle_type tuner_handle ) = 0;

protected:

private:

};

#else

struct rfcommon_atuner_s;
typedef struct rfcommon_atuner_s rfcommon_atuner_intf;

#endif /* #ifdef __cplusplus */

#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------------------------------------------------------*/
extern boolean rfcommon_atuner_manager_init
(
  rfcommon_atuner_intf *atuner_ptr,
  rfcom_mode_enum_type mode,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  script_token_type *script_token
);

/*----------------------------------------------------------------------------*/
extern boolean rfcommon_atuner_manager_set_rx_mode
(
  rfcom_antenna_tuner_priority_cfg priority_cfg,
  rfcommon_atuner_intf *atuner_ptr,
  rfcom_mode_enum_type mode,
  rfcom_band_type_u band,
  uint32 chan_num,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  script_token_type *script_token
);

/*----------------------------------------------------------------------------*/
extern boolean rfcommon_atuner_manager_set_tx_mode
(
  rfcommon_atuner_intf *atuner_ptr,
  rfcom_mode_enum_type mode,
  rfcom_band_type_u band,
  uint32 chan_num,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  script_token_type *script_token,
  rfcommon_atuner_cl_ctrl_info_type *cl_ctrl_info
);

/*----------------------------------------------------------------------------*/
extern boolean rfcommon_atuner_manager_update_tune_code
(
  rfcommon_atuner_intf *atuner_ptr,
  rfcom_mode_enum_type mode,
  rfcom_band_type_u band,
  uint32 chan_num,
  boolean is_this_rx_operation,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  script_token_type *script_token
);

/*----------------------------------------------------------------------------*/
extern boolean rfcommon_atuner_manager_force_detune
(
  rfcommon_atuner_intf *atuner_ptr,
  rfcom_mode_enum_type mode,
  rfcom_band_type_u band,
  uint32 chan_num,
  boolean is_this_rx_operation,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  script_token_type *script_token
);

/*----------------------------------------------------------------------------*/
extern boolean rfcommon_atuner_manager_acknowledge_script
(
  rfcommon_atuner_intf *atuner_ptr,
  script_token_type script_token
);

/*----------------------------------------------------------------------------*/
extern boolean rfcommon_atuner_manager_tx_disable
(
  rfcommon_atuner_intf *atuner_ptr,
  rfcom_mode_enum_type mode,
  rfcom_band_type_u band,
  uint32 chan_num,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  script_token_type *script_token
);

/*----------------------------------------------------------------------------*/
extern boolean rfcommon_atuner_manager_cl_event_register_cb_fn
(
  rfcommon_atuner_intf *atuner_ptr,
  rfcom_mode_enum_type mode,
  rf_cl_event_register_cb_fn cb_func,
  void *obj_ptr
);

/*----------------------------------------------------------------------------*/
extern boolean rfcommon_atuner_manager_disable
(
  rfcommon_atuner_intf *atuner_ptr,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  script_token_type *script_token
);

/*----------------------------------------------------------------------------*/
extern rfdevice_antenna_tuner_device_override_status_type rfcommon_atuner_tune_code_override
(
  rfcommon_atuner_intf *atuner_ptr,
  uint8  override_flag,
  void*  data,
  uint8* tuner_nv_ptr,
  uint8* tuner_id_ptr
);

/*----------------------------------------------------------------------------*/
extern boolean rfcommon_atuner_manager_get_scenario_list
(
  rfcommon_atuner_intf *atuner_ptr,
  rfcom_mode_enum_type mode,
  rfcom_band_type_u band,
  rfm_antenna_tuner_scenario_data *scenario_data,
  uint8 *free_space_index,
  rfm_tuner_handle_type *tuner_handle
);

extern boolean rfcommon_atuner_manager_set_scenario_mcl 
(
  rfcommon_atuner_intf *atuner_ptr, 
  uint8 scenario_val,
  rfm_tuner_handle_type tuner_handle
);

extern boolean rfcommon_atuner_manager_stop_mcl 
(
  rfcommon_atuner_intf *atuner_ptr,
  rfm_tuner_handle_type tuner_handle
);

boolean rfcommon_atuner_manager_is_update_required();

#ifdef __cplusplus
}
#endif


#endif /* RFCOMMON_ATUNER_INTF_H */
