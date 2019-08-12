#ifndef RFCOMMON_ATUNER_MANAGER_INTF_H
#define RFCOMMON_ATUNER_MANAGER_INTF_H 
/*!
  @file
  rfcommon_atuner_manager_intf.h
  
  @brief
  Declaration of antenna tuner carrier aggregation (CA) manager interface class.
  This header file contains all type common logic and implementation related to
  antenna tuner (atuner).
  This implementation supports all the tuner configurations with and without the CA
  configuration. 
  
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/atuner/inc/rfcommon_atuner_manager_intf.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/23/16   hm      increasing the chan_num size to 32 bit to support LTE-B66
10/21/14   yb      Added priority configuratoin for CRAT scenarios
10/07/14   ndb     Added 3DL CA support(Phase 2)
09/22/14   ndb     Added "rfcommon_atuner_num_active_carriers_type" for Multi-Carrier System
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
07/28/14   yb      MCL feature: APIs added - stop_mcl
07/25/14   yb      MCL feature: APIs added - set_scenario_mcl
07/23/14   yb      MCL feature: APIs added - get_scenario_list
05/20/14   yb      Change the Flag usage in the API program_nvg_detune_tuner
05/20/14   yb      Added API's get_port_status(), program_nvg_detune_tuner() & 
                   control_cl_timers()
3/26/14    kg      Added support for LM handle 
03/14/14   kg      Port from Dime
03/10/14   as      Added interface to set update_tune_code_flag
02/18/14   ndb     Added API's set_tuner_config_info() & get_tuner_config_info
02/11/14   vb      Re-design asdiv_tuner to support multiple RF configurations
01/28/14   ndb     Added Atuner operation modes to support for SGLTE/SVLTE/CA
11/29/13   yb      Removed low power API
10/29/13  yb/ndb  Added the support for dynamic tune code size
10/28/13   vb      Support for force_detune()
09/27/13   yb      Added tune_code_override API
09/03/13   ndb     Corrected "RFCOMMON_ATUNER_STATE_LOWPOWER" enum
08/14/13   pl      Added an interface to allow access for port state
08/06/13   vb      Optimized update_tune_code()  
07/18/13   hm      Added tuner_low_power API
07/12/13   aca     Tuner manager cmd update
07/01/13   aca     Over ride support
06/27/13   ndb     Added rfcommon_nv_ant_tuner_cl_ctrl_type in tuner tx mode 
06/20/13   vb      Added API for registering a call back to the Tuner manager
06/15/13   ndb     Tuner Commands that RF APPS task will receive
06/12/13   vb      Added new method tx_disable()
05/25/13   vb      ATUNER_INVALID_TOKEN_VAL support
05/02/13   vb      init version.
============================================================================*/


#ifdef __cplusplus
extern "C"
{
#endif
#include "rfdevice_cmn_type_defs.h"
#ifdef __cplusplus
}
#endif

//Safe C++ headers
#include "rf_buffer_intf.h"
#include "rfa.h"
#include "rfdevice_class.h"
#include "rfcommon_atuner_intf.h"
#include "rfdevice_antenna_tuner.h"

/*----------------------------------------------------------------------------*/
/*
  @brief
  This enum defines all cmds that need rf_apps_task to dispatch. To add a new
  cmd to rf_apps_task, one need to add a new cmd in this enum, and then add
  the corresponding cmd handler in rf_apps_cmd_dispatch().
*/
typedef enum
{

  /*Return loss timer expiry  cmd*/
  RFCOMMOM_ATUNER_APPS_TASK_RL_TIMER_EXPIRY_CMD,

  /* End of adding new cmd*/
  RFCOMMOM_ATUNER_APPS_TASK_MAX_CMD,

}rfcommon_atuner_apps_task_cmd_enum_type;
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Default value to be used when ever data is not available
*/
#define ATUNER_MGR_AOL_DEFAULT_VAL 0

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Invalid scenario used for initializing
*/
#define ATUNER_MGR_AOL_INVALID_SCENARIO_VAL 128

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Invalid tune code used for initializing (per byte entry)
*/
#define ATUNER_MGR_AOL_INVALID_TUNE_CODE 0xFF

/*!
  @brief
  Max size of each tune code value. 

  This is the max size (in bytes)of each tune code value that is currently supported.
*/
#define ATUNER_MGR_MAX_TUNE_CODE_SIZE 8

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Default size of each tune code in bytes
*/
#define ATUNER_MGR_DEFAULT_TUNE_CODE_SIZE 2


//Enum for the different tuner states
typedef enum
{
  RFCOMMON_ATUNER_STATE_INIT,
  RFCOMMON_ATUNER_STATE_RX_MODE,
  RFCOMMON_ATUNER_STATE_TX_MODE,
  RFCOMMON_ATUNER_STATE_DISABLE,
  RFCOMMON_ATUNER_STATE_UNKNOWN,
  RFCOMMON_ATUNER_STATE_MAX
} rfcommon_atuner_state_type;

/* Supported commands by Tuner manager dispatch */
typedef enum
{
  RFCOMMON_ATUNER_OVER_RIDE_RL,
  RFCOMMON_ATUNER_OVER_RIDE_CL,  
  RFCOMMON_ATUNER_OVER_RIDE_INVALID
} rfcommon_atuner_cmd_type;

//Structure to track the complete port status
typedef struct
{
  rfcom_mode_enum_type mode;
  rfcom_band_type_u band;
  uint32 chan_number;
  rfcommon_atuner_state_type atuner_state;
  boolean is_rx_state;
  rfcom_antenna_tuner_priority_cfg priority_cfg;
}rfcommon_atuner_port_status_type;


/* Structure to store the config parameters of the Tuner manager
   The info is needed to exchange the configuration parameters between the tuners 
   during ASD switching
*/
typedef struct
{
  /*Port stats info*/
  rfcommon_atuner_port_status_type port_status;
  
  /*CL control params*/
  rfcommon_atuner_cl_ctrl_info_type cl_ctrl_info;
  
  /*Priority table for tuner sharing*/
}rfcommon_atuner_config_info_params_type;

/*Operating mode of the tuner manager

Tuner manager can be operated in Three modes

a. RFCOMMON_ATUNER_STANDALONE_MODE  - Single Tech is active in the Tuner Manager.	
																																						Either  PORT_0 or PORT_1 is in Rx or RX/TX state.
																																								
b. RFCOMMON_ATUNER_CA_MODE   - Two carriers or more carriers of same tech are active(either in Rx State or Tx State) 
*****PORT_0 has the First Carrier.
*****PORT_1 has the Second Carrier.
*****PORT_2 has the Third Carrier.
																																
c. RFCOMMON_ATUNER_CRAT_MODE - Two  different Tech's are active.
*****PORT_0 has the priority tech which is in Rx or RX/TX state.
*****PORT_1 has the non-priority tech which is inRx or  RX/TX state.
*/

typedef enum
{
  RFCOMMON_ATUNER_STANDALONE_MODE = 1,
  RFCOMMON_ATUNER_CA_MODE,
  RFCOMMON_ATUNER_CRAT_MODE,  
  RFCOMMON_ATUNER_MAX_MODES,
} rfcommon_atuner_concurrency_mode_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Internal enum to track the number of Carriers active in Tuner
*/

typedef enum
{
  RFCOMMON_ATUNER_NUM_ACTIVE_CARRIER_0 = 0,
  RFCOMMON_ATUNER_NUM_ACTIVE_CARRIER_1 = 1,
  RFCOMMON_ATUNER_NUM_ACTIVE_CARRIER_2,
  RFCOMMON_ATUNER_NUM_ACTIVE_CARRIER_3,
  RFCOMMON_ATUNER_MAX_ACTIVE_CARRIER = RFCOMMON_ATUNER_NUM_ACTIVE_CARRIER_3
} rfcommon_atuner_num_active_carriers_type;

/*!
  @brief
  Data structure to get the operation mode of the tuner
a. rfcommon_atuner_num_active_carriers_type -- Number of Active Carriers.
b. rfcommon_atuner_ca_port_enum_type        -- List of Active CA ports for the active carriers.
   Active CA ports: PORTS which are in either Rx or TX state
c. rfcommon_atuner_concurrency_mode_type    -- Standalone or CA mode or CRAT mode
d. tx_active                                -- Is Tx active in any of the CA port ?
*/
typedef struct
{
  rfcommon_atuner_num_active_carriers_type num_active_carriers;
  rfcommon_atuner_ca_port_enum_type active_port_list[RFCOMMON_ATUNER_MAX_ACTIVE_CARRIER];
  rfcommon_atuner_concurrency_mode_type concurrent_info;
  boolean tx_active;
}rfcommon_atuner_operating_mode_type;


/*!
  @brief
  Data structure to track the status associated with each port
*/
typedef struct
{
  rfcommon_atuner_state_type tuner_state;
  rfcom_mode_enum_type mode;
  rfcom_band_type_u band;
  uint16 chan_num;
}port_state_type;
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface antenna manager class that contains all the APIs for both of
  Advanced Open Loop (AOL) and Closed Loop (CL) algorithms of antenna tuner.
 
  @details
*/
class rfcommon_atuner_manager_intf : public rfdevice_class
{
public:

  /*! Empty virtual destructor - base, interface class has no data so nothing 
  to delete */
  virtual ~rfcommon_atuner_manager_intf()
  { }
  
  /* Grabs the port instance for the port# provided*/
  virtual rfdevice_class *get_port_instance(
                                   rfcommon_atuner_ca_port_type port_number) = 0;

  /* Initializes the Tuner */
  virtual boolean init(rfcommon_atuner_ca_port_type port_num,
                       rfcom_mode_enum_type mode,
                       rf_device_execution_type dev_action,
                       rf_buffer_intf *script_buffer,
                       int16 script_timing,
                       script_token_type *script_token) = 0;

  /* Programs the associated antenna tuner in Rx only mode and 
     applies best possible tune code for Rx only mode */
  virtual boolean set_rx_mode(rfcom_antenna_tuner_priority_cfg priority_cfg,
                              rfcommon_atuner_ca_port_type port_num,
                              rfcom_mode_enum_type mode,
                              rfcom_band_type_u band,
                              uint32 chan_num,
                              rf_device_execution_type dev_action,
                              rf_buffer_intf *script_buffer,
                              int16 script_timing,
                              script_token_type *script_token) = 0;

  /* Programs the associated tuner in Tx mode and
     applies best possible tune code for Tx mode */
  virtual boolean set_tx_mode(rfcommon_atuner_ca_port_type port_num,
                              rfcom_mode_enum_type mode,
                              rfcom_band_type_u band,
                              uint32 chan_num,
                              rf_device_execution_type dev_action,
                              rf_buffer_intf *script_buffer,
                              int16 script_timing,
                              script_token_type *script_token,
                              rfcommon_atuner_cl_ctrl_info_type *cl_ctrl_info) = 0;

  /* Update the tune code every 200ms */
  virtual boolean update_tune_code(rfcommon_atuner_ca_port_type port_num,
                                   rfcom_mode_enum_type mode,
                                   rf_device_execution_type dev_action,
                                   rf_buffer_intf *script_buffer,
                                   int16 script_timing,
                                   script_token_type *script_token) = 0;

  /* Acknowlegement for the script previously provided */
  virtual boolean acknowledge_script(rfcommon_atuner_ca_port_type port_num,
                                     script_token_type script_token) = 0;

  /* Disables Tx */
  virtual boolean tx_disable( rfcommon_atuner_ca_port_type port_num,
                              rfcom_mode_enum_type mode,
                              rfcom_band_type_u band,
                              uint32 chan_num,
                              rf_device_execution_type dev_action,
                              rf_buffer_intf *script_buffer,
                              int16 script_timing,
                              script_token_type *script_token) = 0;

  /* Registers the call back func for updates on when CL event is scheduled */
  virtual boolean register_cl_cb_fn ( rfcommon_atuner_ca_port_type port_num,
                                      rfcom_mode_enum_type mode, 
                                      rf_cl_event_register_cb_fn cb_func, 
                                      void *obj_ptr ) = 0;

  /* Force re-tune (or de-tune) the antenna tuner */
  virtual boolean force_detune( rfcommon_atuner_ca_port_type port_num,
                                rfcom_mode_enum_type mode,
                                rf_device_execution_type dev_action,
                                rf_buffer_intf *script_buffer,
                                int16 script_timing,
                                script_token_type *script_token ) = 0;

  /* Disables the associated tuner */
  virtual boolean disable(rfcommon_atuner_ca_port_type port_num,
                          rf_device_execution_type dev_action,
                          rf_buffer_intf *script_buffer,
                          int16 script_timing,
                          script_token_type *script_token) = 0;

  virtual boolean set_tuner_config_info( rfcommon_atuner_ca_port_type port_num,
                                         rfcommon_atuner_config_info_params_type *config_payload_ptr,
                                         rf_buffer_intf *script_buffer,
                                         rf_device_execution_type dev_action,  
                                         script_token_type *script_token,
                                         int16 timing )= 0;
  
  virtual boolean get_tuner_config_info ( rfcommon_atuner_ca_port_type port_num,
                                          rfcommon_atuner_config_info_params_type *config_payload_ptr ) = 0;

  /* API for getting scenario list for given tech and band */
  virtual boolean get_scenario_list( rfcommon_atuner_ca_port_type port_num,
                                     rfcom_mode_enum_type mode,
                                     rfcom_band_type_u band,
                                     rfm_antenna_tuner_scenario_data *scenario_data,
                                     uint8 *free_space_index,
                                     rfm_tuner_handle_type *tuner_handle) = 0;

  /* API to set scenario for mcl */
  virtual boolean set_scenario_mcl( rfcommon_atuner_ca_port_type port_num,
                                    uint8 scenario_val,
                                    rfm_tuner_handle_type tuner_handle ) = 0;

  /* API to stop mcl */
  virtual boolean stop_mcl( rfcommon_atuner_ca_port_type port_num,
                            rfm_tuner_handle_type tuner_handle ) = 0;

  virtual void enter_lock( ) = 0;

  virtual void release_lock( ) = 0;

  virtual void set_force_update_tune_code_flag(boolean flag_val) = 0; 

  virtual rfdevice_antenna_tuner_device_override_status_type tune_code_override (rfcommon_atuner_ca_port_type port_num,
                                                                                 uint8  override_flag,
                                                                                 void*  data,
                                                                                 uint8* tuner_nv_ptr,
                                                                                 uint8* tuner_id_ptr) = 0;

  /*API to get the port status of the Tuner*/
  virtual boolean get_port_status( rfcommon_atuner_ca_port_type port_num,
                                      rfcommon_atuner_port_status_type *port_status_buf) = 0;
  

  /* API to program the NVG and detune code of the tuner associated with the Tuner manager*/
  virtual boolean program_nvg_detune_tuner( boolean nvg_off_on,
                                            boolean is_program_nvg,
                                            uint8 detune_code_size,
                                            uint8 *detune_code,
                                            rf_buffer_intf *buff_obj_ptr, 
                                            rf_device_execution_type dev_action,
                                            script_token_type *script_token,
                                            int16 script_timing ) = 0;
  /* API to control the CL timer*/
   virtual  boolean control_cl_timers( boolean cl_off_on ) = 0;
  	

protected:

private:

};

#endif /* RFCOMMON_ATUNER_MANAGER_INTF_H */

