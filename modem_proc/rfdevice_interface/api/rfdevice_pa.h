#ifndef RFDEVICE_PA_H
#define RFDEVICE_PA_H
/*!
   @file
   rfdevice_pa.h

   @brief

*/

/*===========================================================================

Copyright (c) 2012 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_pa.h#1 $

when       who    what, where, why
--------   ---   ---------------------------------------------------------------
08/27/14   vv     Update temperature read from int8 to int16
11/12/13   vv      Corrected the wrapper API set_gain_range (with cfg) which will now be a wrapper for all other PAs except 3rd party PAs
10/17/13   vv      Added new API set_gain range(with cfg)
10/31/13   shb    Converted TRX common device interface to C++
09/17/13   vs     Added API to modify autogen register settings
09/11/13   vs     Added support for PA port A/B switch api
09/04/13   sr/zhw  Add explicit default PA Icq value flag in PA device driver
07/07/13   sbo    Added BW support for PA device
6/25/13    vbh    Added explicit trigger for the PA device
05/28/13   vs     PA therm API changes update
04/10/13   sn     Cleanup PA interface
12/03/12   aca    Added support for timing info in scripts
10/17/12   tks    Added support for modifying pa current bias in ftm mode
07/12/12   sn     Removed command dispatch function
06/21/12   sn     Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfdevice_class.h"
#include "rf_buffer_intf.h"
#include "rfdevice_cmn_type_defs.h"
#include "rfdevice_pa_cfg.h"
#ifdef __cplusplus

class rfdevice_pa:public rfdevice_class
{
public:

    rfdevice_pa();

    virtual ~rfdevice_pa();

    /*----------------------------------------------------------------------------*/
    /*!
      @brief
      interface function for pa config

      @details
      rfdevice_pa_config()

      @param mode
      enum indicating the Radio Access Technology

      @param band
      enum indicating band number

      @param buff_obj_ptr
      void pointer to CCS buffer object

      @param dev_action
      indicates if it is immediate write to hw or build scripts

      @param script_timing
      time offset for the script where a negative value refers to the time BEFORE the event

      @return
      boolean indicating pass or fail
    */
    virtual boolean config
    (
        rfdevice_pa_cfg* pa_params_p,
        rf_buffer_intf *buff_obj_ptr,
        rf_device_execution_type dev_action,
        int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    )  = 0;


    /*----------------------------------------------------------------------------*/
    /*!
      @brief
      set PA TXAGC with cfg as param

      @details
      This function can be used to configure range and current bias values for a particular
      band in ftm mode

      @param cfg
      PA config parameter that contains: mode band on_off pa_gain_range pa_icq_pdm mod_type

      @param script_buffer
      void pointer to pass address of buffer script

      @param execution_type
      boolean to indicate write to hw or prepare script

      @param script_timing
      time offset for the script where a negative value refers to the time BEFORE the event
    */
    virtual boolean set_pa_txagc
    (
        rfdevice_pa_cfg* pa_params_p,
        rf_buffer_intf *buff_obj_ptr,
        rf_device_execution_type dev_action,
        int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    ) =0;

    /*----------------------------------------------------------------------------*/
    /*!
      @brief
      set PA on or off with cfg as param

      @details
      This function can be used to configure PA ON or OFF for a particular
      band in ftm mode

      @param cfg
      PA config parameter that contains: mode band on_off pa_gain_range pa_icq_pdm mod_type

      @param script_buffer
      void pointer to pass address of buffer script

      @param execution_type
      boolean to indicate write to hw or prepare script

      @param script_timing
      time offset for the script where a negative value refers to the time BEFORE the event
    */
    virtual boolean set_pa_on_off
    (
        rfdevice_pa_cfg* pa_params_p,
        rf_buffer_intf *buff_obj_ptr,
        rf_device_execution_type dev_action,
        int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    ) =0 ;

    /*----------------------------------------------------------------------------*/
    virtual boolean set_pa_trigger
    (
        rfdevice_pa_cfg* pa_params_p,
        rf_buffer_intf *buff_obj_ptr,
        rf_device_execution_type dev_action,
        int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    );

    /*----------------------------------------------------------------------------*/
    virtual boolean get_pa_info();


    /*----------------------------------------------------------------------------*/
    /*!
      @brief
      interface function for pa sleep

      @details
      rfdevice_pa_sleep()

      @param buff_obj_ptr
      void pointer to CCS buffer object

      @param dev_action
      indicates if it is immediate write to hw or build scripts

      @param script_timing
      time offset for the script where a negative value refers to the time BEFORE the event

      @return
      boolean indicating pass or fail
    */
    virtual boolean sleep
    (
        rfdevice_pa_cfg* pa_params_p,
        rf_buffer_intf *buff_obj_ptr,
        rf_device_execution_type dev_action,
        int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    )  = 0;


    /*----------------------------------------------------------------------------*/
    /*!
      @brief
      interface function for pa wakeup

      @details
      rfdevice_pa_wakeup()

      @param buff_obj_ptr
      void pointer to CCS buffer object

      @param dev_action
      indicates if it is immediate write to hw or build scripts

      @param script_timing
      time offset for the script where a negative value refers to the time BEFORE the event

      @return
      boolean indicating pass or fail
    */
    virtual boolean wakeup
    (
        rfdevice_pa_cfg* pa_params_p,
        rf_buffer_intf *buff_obj_ptr,
        rf_device_execution_type dev_action,
        int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    )  = 0;


    /*----------------------------------------------------------------------------*/
    /*!
      @brief
      query max script size

      @details
      rfdevice_pa_get_script_size()

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
      populates the band map information from rfc

      @details
      rfdevice_pa_set_band_map

      @param mode
      enum indicating the Radio Access Technology

      @param band
      enum indicating band number

      @param *data_ptr
      band mapping information passed as int32 pointer
    */
    virtual boolean set_band_map
    (
        rfcom_mode_enum_type mode,
        rfcom_band_type_u band,
        int32 *data_ptr,
        uint8 size
    ) = 0;

    /*----------------------------------------------------------------------------*/
    virtual boolean pa_therm_on_off
    (
        boolean turn_on_off
    );

    /*----------------------------------------------------------------------------*/
    /*pa temperature sensor changes*/
    virtual  boolean get_pa_temperature
    (
        int16 *temperature
    );

    /*----------------------------------------------------------------------------*/
    virtual boolean set_switch_state
    (
        rfcom_mode_enum_type mode,
        rfcom_band_type_u band,
        uint8 switch_state_set
    );

    /*----------------------------------------------------------------------------*/
    /*!
      @brief
      interface function for pa autogen settings to be modified

      @details
      modifies autogen file content of a given PA

      @param buff_obj_ptr
      void pointer to CCS buffer object

      @param pa_reg_cfg
      configuration information on registers to be modified

      @return
      boolean indicating pass or fail
    */

    virtual boolean modify_pa_ag_setting
    (
        rfdevice_pa_mod_cfg_type *pa_reg_cfg,
        rfdevice_mod_reg_info_type *pa_reg_data
    );
	
    virtual boolean get_max_bw_supported
    (
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type rf_band,
    uint32 *max_bw_khz
    );

};

#endif /* #ifdef __cplusplus */

#endif
