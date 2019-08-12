#ifndef RF_CDMA_ATUNER_H
#define RF_CDMA_ATUNER_H
/*!
  @file
  rf_cdma_atuner.h

  @brief
  <b>This is the atuner interface exposed to the MC layer.</b> This module 
  contains all CDMA-atuner-specific data and implementations. 

  @addtogroup RF_RFC_CDMA_MC_INTF
  @{
    @addtogroup RF_RFC_CDMA_TUNER_WRAPPER
    @{
*/

/*==============================================================================

  Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 
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
  written permission of Qualcomm Technologies Incorporated.d.

==============================================================================*/

/*==============================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_atuner.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
10/21/14   yb      Added priority configuration to config Tuners during CRAT scenarios
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
05/29/14   zhw     Clean up deprecated Tuner CL APIs
05/14/14   APU     Suppy CL tuner with valid TxLM handle.
10/09/13   yb      Added rf_cdma_atuner_tune_code_override API
10/02/13   zhw     Tuner Retune disable and configure API support
10/09/13   yb      Added rf_cdma_atuner_tune_code_override API
10/02/13   zhw     Tuner Retune disable and configure API support
03/10/14   as/spa  Added callback function as part of antenna switch
01/03/14   zhw     Update Tune Code API. Remove deprecated interface
11/26/13   yb      Added rf_cdma_atuner_tune_code_override API
09/04/13   zhw     Tuner AsDiv support
07/12/13   spa     Update #def being used for max possible instances per band
07/12/13   spa     Use master_token and sub_token for acknowledgement
07/05/13   zhw     Tuner CL NV support
06/28/13   JJ      Add API for antenna tuner to update tune code in ftm mode
06/25/13   zhw     Tuner CL support. Added Tx disable API which clears CL timer
06/21/13   zhw     Tuner CL support. Added RL meas call back registration
06/10/13   spa     Initial version

==============================================================================*/
#include "comdef.h"
#include "rfm_cdma_band_types.h"
#include "rfc_common.h"
#include "rfc_class.h"
#include "rfcommon_atuner_intf.h"
#include "rfcommon_mc.h"


#ifdef FEATURE_RF_ASDIV
#include "rfcommon_asdiv_manager.h"
#endif
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Antenna tuner script token acknowledgement tracking data structure.

  @details
  The antenna tuner generates scripts which are executed by FW asynchronously.
  When these scripts have been executed, the tuner needs an acknowledgement
  of the specific script that is complete.  Since script creation and FW
  acknowledgement occur in different function contexts in various parts
  of the driver, this data needs to be saved in the device status structure
  to communicate across function boundaries.
 
  If the antenna tuner successfully generated a script, token_valid will be
  set TRUE and the token should be stored in the token structure member.
 
  If the antenna tuner didn't run successfully, the token_valid member
  will not be TRUE, and the RF driver knows to not acknowledge this script
  later on.
*/
typedef struct ant_tuner_script_token_data_s
{
  /*! Tracks whether the current token data is valid. */
  boolean token_valid;

  /*! If token_valid is true, this holds the current token 
      for the outstanding antenna tuner script. */
  script_token_type token;
} ant_tuner_device_tokens_type;


/*----------------------------------------------------------------------------*/
#ifdef __cplusplus

/*!
  @brief
  RF CDMA ATUNER wrapper device class

  @details
  This class acts as an aggregator class for RF ATUNER device objects. It is 
  provided to configure more than one ATUNER device for a specific band/path
  configuration.
*/
class rf_cdma_atuner
{
  protected:
    int    rfm_device;
    uint8  default_atuner_num;
    rfcommon_atuner_intf*  default_tuner_list[RFC_MAX_DEVICE_INSTANCES_PER_BAND];
    /* Sub tokens, internally maintained by the tuner class */
    ant_tuner_device_tokens_type tuner_sub_tokens[RFC_MAX_DEVICE_INSTANCES_PER_BAND];

    uint8  default_antenna;    
    uint8 updated_tuner_list_length;
    rfcommon_atuner_intf** updated_tuner_list;

    #ifdef FEATURE_RF_ASDIV
    boolean tuner_asdiv_enabled;
    #endif

  public:
    /* Constructor - Initialize the aggregator object */
    rf_cdma_atuner( int rfm_device, uint8 default_ant_num );
  
    /* Add a new RF atuner device to the aggregator */
    boolean add_default_tuner_instance( rfcommon_atuner_intf* misc_device_descriptor );

    /* Interface for atuner device init */
    boolean init(  rf_cdma_atuner *atuner_ptr, 
          rfm_mode_enum_type mode, 
          rf_device_execution_type dev_action, 
          rf_buffer_intf *script_buffer, 
          int16 script_timing, 
          ant_tuner_device_tokens_type *script_token );

    /* Interface for atuner device disable */
    boolean disable( rf_cdma_atuner *atuner_ptr, 
          rf_device_execution_type dev_action, 
          rf_buffer_intf *script_buffer, 
          int16 script_timing, 
          ant_tuner_device_tokens_type *script_token ) ;

    /* Interface for atuner device tx disable */
    boolean tx_disable( rf_cdma_atuner *atuner_ptr, 
                        rfcom_mode_enum_type mode,
                        rfcom_band_type_u band,
                        uint16 chan_num,
                        rf_device_execution_type dev_action,
                        rf_buffer_intf *script_buffer,
                        int16 script_timing,
                        ant_tuner_device_tokens_type *script_token ); 

    /* Interface for atuner device retune disable */
    boolean retune_disable( rf_cdma_atuner *atuner_ptr, 
                            rfcom_mode_enum_type mode,
                            rfcom_band_type_u band,
                            uint16 chan_num,
                            rf_device_execution_type dev_action,
                            rf_buffer_intf *script_buffer,
                            int16 script_timing,
                            ant_tuner_device_tokens_type *master_token,
                            boolean is_tx_retune );

    /* Interface for atuner device Rx mode configuration */
    boolean configure_for_rx(  rfcom_antenna_tuner_priority_cfg priority_cfg,
          rf_cdma_atuner *atuner_ptr, 
          rfm_mode_enum_type mode, 
          rfcom_band_type_u band,
          uint16 chan_num,
          rf_device_execution_type dev_action, 
          rf_buffer_intf *script_buffer, 
          int16 script_timing, 
          ant_tuner_device_tokens_type *script_token );

    /* Interface for atuner device Tx mode configuration */
    boolean configure_for_tx( lm_handle_type txlm_handle ,
                              rf_cdma_atuner *atuner_ptr, 
                              rfm_mode_enum_type mode, 
                              rfcom_band_type_u band,
                              uint16 chan_num,
                              rf_device_execution_type dev_action, 
                              rf_buffer_intf *script_buffer, 
                              int16 script_timing, 
                              ant_tuner_device_tokens_type *script_token,
                              rfcommon_nv_ant_tuner_cl_ctrl_type *tuner_cl_nv_ptr );

    /* Interface for atuner device Retune mode configuration */
    boolean configure_for_retune ( 
                    rfcom_antenna_tuner_priority_cfg priority_cfg,
                    rf_cdma_atuner *atuner_ptr, 
                    rfm_mode_enum_type mode, 
                    rfcom_band_type_u band,
                    uint16 chan_num,
                    rf_device_execution_type dev_action, 
                    rf_buffer_intf *script_buffer, 
                    int16 script_timing, 
                    ant_tuner_device_tokens_type *master_token,
                    rfcommon_atuner_cl_ctrl_info_type *tuner_cl_nv_ptr,
                    boolean is_tx_retune );

    /* Interface for atuner update tuner code */
    boolean update_tune_code( rf_cdma_atuner *atuner_ptr,
           rfm_mode_enum_type mode,
           rfcom_band_type_u band,
           uint16 chan_num,
           boolean is_this_rx_operation,
           void* tuner_cb_data,
           uint16 tuner_mdsp_chain,
           rf_device_execution_type dev_action,
           rf_buffer_intf *script_buffer,
           int16 script_timing );

    /* Interface for atuner update tuner code in ftm */
    boolean update_ftm_tune_code( rf_cdma_atuner *atuner_ptr,
           rfm_mode_enum_type mode,
           rfcom_band_type_u band,
           uint16 chan_num,
           boolean is_this_rx_operation);

    /* Interface for atuner acknowledge script */
    boolean ack_script( rf_cdma_atuner *atuner_ptr, 
          ant_tuner_device_tokens_type *script_token );

    /* Interface for atuner CL callback registration */
    boolean register_close_loop_callback(
                                     rf_cdma_atuner *atuner_ptr, 
                                     rfm_mode_enum_type mode, 
                                     rf_cl_event_register_cb_fn cb_func,
                                     void *obj_ptr );

    /* Interface to override current tuner code */
    rfdevice_antenna_tuner_device_override_status_type tune_code_override
    (
      rf_cdma_atuner *atuner_ptr,
      uint8  override_flag,
      void*  data,
      uint8* tuner_nv_ptr,
      uint8* tuner_id_ptr
    );

    #ifdef FEATURE_RF_ASDIV
    /* Interface for atuner  */
    boolean asdiv_config_switch(
                        rfm_device_enum_type device,
                        rfm_mode_enum_type rfm_mode,
                        rfcommon_asdiv_position_type antenna_position,
                        rf_buffer_intf *script_ptr,
                        rfcommon_asdiv_asm_conifg_type asm_script_cfg,
                        rfcommon_asdiv_send_script_type send_script_cfg,
                        rfcommon_asdiv_switch_start_notify_type switch_start_cfg,
                        rfcommon_asdiv_update_tuner_type update_tuner_cb,
                        rfm_l1_cb_type l1_cb_func,
                        void *l1_cb_data,
                        boolean gen_script_only );
    #endif

  private:
      void update_tuner_object_list();
};

#endif /* #ifdef __cplusplus */

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct rf_cdma_atuner rf_cdma_atuner ;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_atuner_init
(
  rf_cdma_atuner *atuner_ptr,
  rfm_mode_enum_type mode,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  ant_tuner_device_tokens_type *script_token
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_atuner_disable
(
  rf_cdma_atuner *atuner_ptr,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  ant_tuner_device_tokens_type *script_token
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_atuner_tx_disable
(
  rf_cdma_atuner *atuner_ptr,
  rfcom_mode_enum_type mode,
  rfcom_band_type_u band,
  uint16 chan_num,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  ant_tuner_device_tokens_type *script_token
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_atuner_configure_for_rx
(
  rfcom_antenna_tuner_priority_cfg priority_cfg,
  rf_cdma_atuner *atuner_ptr,
  rfm_mode_enum_type mode,
  rfcom_band_type_u band,
  uint16 chan_num,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  ant_tuner_device_tokens_type *script_token
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_atuner_configure_for_tx
(
  lm_handle_type txlm_handle,
  rf_cdma_atuner *atuner_ptr,
  rfm_mode_enum_type mode,
  rfcom_band_type_u band,
  uint16 chan_num,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  ant_tuner_device_tokens_type *script_token,
  rfcommon_nv_ant_tuner_cl_ctrl_type *tuner_cl_nv_ptr
) ;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_atuner_update_tune_code
(
  rf_cdma_atuner *atuner_ptr,
  rfm_mode_enum_type mode,
  rfcom_band_type_u band,
  uint16 chan_num,
  boolean is_this_rx_operation,
  uint16 tuner_mdsp_chain,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  void* tuner_cb_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_atuner_update_ftm_tune_code
(
  rf_cdma_atuner *atuner_ptr,
  rfm_mode_enum_type mode,
  rfcom_band_type_u band,
  uint16 chan_num,
  boolean is_this_rx_operation
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_atuner_ack_script
(
  rf_cdma_atuner *atuner_ptr,
  ant_tuner_device_tokens_type *script_token
);

/*----------------------------------------------------------------------------*/
rfdevice_antenna_tuner_device_override_status_type rf_cdma_atuner_tune_code_override
(
  rf_cdma_atuner *atuner_ptr,
  uint8  override_flag,
  void*  data,
  uint8* tuner_nv_ptr,
  uint8* tuner_id_ptr
);

#ifdef FEATURE_RF_ASDIV
/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_atuner_asdiv_config_switch
(
  rf_cdma_atuner *atuner_ptr, 
  rfm_device_enum_type device,
  rfm_mode_enum_type rfm_mode,
  rfcommon_asdiv_position_type antenna_position,
  rf_buffer_intf *script_ptr,
  rfcommon_asdiv_asm_conifg_type asm_script_cfg,
  rfcommon_asdiv_send_script_type send_script_cfg,
  rfcommon_asdiv_switch_start_notify_type switch_start_cfg,
  rfcommon_asdiv_update_tuner_type update_tuner_cb,
  rfm_l1_cb_type l1_cb_func,
  void *l1_cb_data,
  boolean gen_script_only
);
#endif

#ifdef __cplusplus
}
#endif

/*! @} */
/*! @} */
#endif /* #ifndef RF_CDMA_ATUNER_H */
