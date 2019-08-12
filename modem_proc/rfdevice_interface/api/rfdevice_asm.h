#ifndef RFDEVICE_ASM_H
#define RFDEVICE_ASM_H
/*!
   @file
   rfdevice_asm.h

   @brief

*/

/*===========================================================================

Copyright (c) 2012 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_asm.h#1 $

when       who    what, where, why
--------   ---   -------------------------------------------------------------------
01/27/15   dbc    Add set_asm_xsw_port_config API declaration
09/16/14   sn     ASM set band RFM Device support
06/16/14   aca    ASM temp support
05/05/14   sn     ASM class based API
10/31/13   shb    Converted TRX common device interface to C++
10/31/13   sn     Added API to support IRAT for QFE2320
09/17/13   vs     Added API to modify autogen register settings
06/19/13   vs     updated the driver interface for asm port status based on band
06/13/13   vbh    Added explicit trigger for the ASM device
06/04/13   vs     updated the driver interface for asm port status
04/10/13   sn     Cleanup ASM interface
12/10/12   aca    ASM timing, CA support
07/12/12   sn     Removed command_dispatch function
06/21/12   sn     Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rf_buffer_intf.h"
#include "rfdevice_class.h"
#include "rfdevice_cmn_type_defs.h"
#include "rfdevice_asm_cfg.h"

class rfdevice_asm:public rfdevice_class
{
public:
  rfdevice_asm();

  virtual ~rfdevice_asm();
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    interface function for asm rx config

    @details
    rfdevice_asm_config_rx()

    @param mode
    enum indicating the Radio Access Technology

    @param band
    enum indicating band number

    @param buff_obj_ptr
    void pointer to CCS buffer object

    @param dev_action
    indicates if it is immediate write to hw or build scripts

    @param script_timing
    timing offset for CCS scripts

    @return
    boolean indicating pass or fail
  */
  virtual boolean enable
  (
    rfdevice_asm_cfg* asm_cfg_p, rf_buffer_intf *buff_obj_ptr,
    rf_device_execution_type dev_action, int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  )  = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    interface function for asm disable rx

    @details
    rfdevice_asm_disable_rx()

    @param mode
    enum indicating the Radio Access Technology

    @param band
    enum indicating band number

    @param buff_obj_ptr
    void pointer to CCS buffer object

    @param dev_action
    indicates if it is immediate write to hw or build scripts

    @param script_timing
    timing offset for CCS scripts

    @return
    boolean indicating pass or fail
  */
  virtual boolean disable
  (
    rfdevice_asm_cfg* asm_cfg_p, rf_buffer_intf *buff_obj_ptr,
    rf_device_execution_type dev_action, int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  )  = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    interface function for asm sleep

    @details
    rfdevice_asm_sleep()

    @param buff_obj_ptr
    void pointer to CCS buffer object

    @param dev_action
    indicates if it is immediate write to hw or build scripts

    @param script_timing
    timing offset for CCS scripts

    @return
    boolean indicating pass or fail
  */
  virtual boolean sleep
  (
    rfdevice_asm_cfg* asm_cfg_p, rf_buffer_intf *buff_obj_ptr, rf_device_execution_type dev_action, int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  )  = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    interface function for asm wakeup

    @details
    rfdevice_asm_wakeup()

    @param buff_obj_ptr
    void pointer to CCS buffer object

    @param dev_action
    indicates if it is immediate write to hw or build scripts

    @param script_timing
    timing offset for CCS scripts

    @return
    boolean indicating pass or fail
  */
  virtual boolean wakeup
  (
    rfdevice_asm_cfg* asm_cfg_p, rf_buffer_intf *buff_obj_ptr, rf_device_execution_type dev_action, int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  )  = 0;


  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    interface function for asm trigger, to latch the pending settings.

    @details
    rfdevice_asm_trigger()

    @param buff_obj_ptr
    void pointer to CCS buffer object

    @param dev_action
    indicates if it is immediate write to hw or build scripts

    @param script_timing
    time offset for the script where a negative value refers to the time BEFORE the event

    @return
    boolean indicating pass or fail
  */
  virtual boolean trigger
  (
    rfdevice_asm_cfg* asm_cfg_p,
    rf_buffer_intf *buff_obj_ptr,
    rf_device_execution_type dev_action,
    int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  ) = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    query max script size

    @details
    rfdevice_asm_get_script_size()

    @return
    max script size of type uint16
  */
  virtual uint16 get_script_size
  (
    void
  )  = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    populates the rx band map information into the asm device class

    @details
    populates the rx band port map information into the asm device class

    @param mode
    enum indicating the Radio Access Technology

    @param band
    enum indicating band number

    @param data
    band mapping information passed as int32 pointer
  */
  virtual boolean set_rx_band_map
  (
    rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode, rfcom_band_type_u band,
    int32 *data_ptr, uint8 size
  ) = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    populates the tx band map information into the asm device class

    @details
    populates the tx band port map information into the asm device class

    @param mode
    enum indicating the Radio Access Technology

    @param band
    enum indicating band number

    @param data
    band mapping information passed as int32 pointer
  */
  virtual boolean set_tx_band_map
  (
    rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode, rfcom_band_type_u band,
    int32 *data_ptr, uint8 size
  ) = 0;  


  /*----------------------------------------------------------------------------*/
  /*@brief: set the xsw (cross switch port) to be used for device/tech/band
    @input: port state
    @return value: pass or fail
  */

  virtual boolean set_asm_xsw_port_config
  (
    rfm_device_enum_type rfm_device,
    rfcom_mode_enum_type mode,
    rfcom_band_type_u band
  );

  /*----------------------------------------------------------------------------*/
  /*@brief: switch port state variable of the asm object
    @input: port state
    @return value: pass or fail
  */

  virtual boolean set_switch_state(uint8 switch_state_set);


  /*----------------------------------------------------------------------------*/
  /*@brief: switch port state variable of the asm object
    @input: port state
    @return value: pass or fail
  */
  virtual boolean get_switch_state(uint8 *switch_state_get);

  /*----------------------------------------------------------------------------*/
  /*@brief: switch port state variable of the asm object
    @input: port state
    @return value: pass or fail
  */

  virtual boolean set_rx_switch_state(rfcom_mode_enum_type mode,
                                      rfcom_band_type_u band,
                                      uint8 switch_state_set);
  /*----------------------------------------------------------------------------*/
  /*@brief: switch port state variable of the asm object
    @input: port state
    @return value: pass or fail
  */
  virtual boolean set_tx_switch_state(rfcom_mode_enum_type mode,
                                      rfcom_band_type_u band,
                                      uint8 switch_state_set);
   /*----------------------------------------------------------------------------*/
   /*@brief: enable/disable therm
    @input: flag for on/off
    @return value: pass or fail
  */
    virtual boolean asm_therm_on_off
    (
        boolean turn_on_off
    );

    /*----------------------------------------------------------------------------*/
    /*@brief: get temp in degree C
    @param: pointer to return data 
    @return value: pass or fail
  */
    virtual  boolean get_asm_temperature
    (
        int16 *temperature
    );
  /*----------------------------------------------------------------------------*/
  /*!
    @brief

    @details
    to update the autogen file register settings

    @param asm_reg_cfg
    ASM register configuration information

    @param asm_reg_data
    ASM register data content to be modified

    @return
    boolean indicating pass or fail
  */

  virtual boolean modify_asm_ag_setting
  (
    rfdevice_asm_mod_cfg_type *asm_reg_cfg,
    rfdevice_mod_reg_info_type *asm_reg_data
  );

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Special APis to support the GRFC Tuner. Not Applicable for ASM Device.   
  */

  virtual boolean update_scenario(uint8 scenario_num);

  virtual boolean update_state(rfdevice_asm_cfg *asm_cfg_p, rf_buffer_intf *script_buffer,
                               rf_device_execution_type dev_action,
                               int16 script_timing);
};
#endif
