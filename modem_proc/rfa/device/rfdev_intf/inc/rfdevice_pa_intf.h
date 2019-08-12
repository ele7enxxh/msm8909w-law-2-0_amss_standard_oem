#ifndef RFDEVICE_PA_INTF_H
#define RFDEVICE_PA_INTF_H
/*!
   @file
   rfdevice_pa_intf.h

   @brief


*/

/*==============================================================================

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_pa_intf.h#1 $

when       who   what, where, why
--------   ---   ---------------------------------------------------------------
11/19/14   vv     Added rfdevice_reg_dump() API  for debug
08/27/14   vv     Update temperature read from int8 to int16
09/17/13   vs      Added API to modify autogen register settings
09/11/13   vs      Added support for PA port A/B switch api
09/04/13   sr/zhw  Add explicit default PA Icq value flag in PA device driver
07/07/13   sbo   Added BW support for PA device
07/03/13   vbh   Added non-trigger APIs for PA_on_OFF and PA_SET_GAIN_RANGE
04/03 13   sn    Cleanup PA interface
03/21/13   yzw   Add device driver support to query PA device information {mfg_id, prd_id, rev_id}
12/03/12   aca   Added support for timing info in scripts
10/17/12   tks   Added support for modifying pa current bias in ftm mode
08/01/12   tks   Added support for set_band_map
07/12/12   sn    Removed command dispatch function
06/21/12   sn    Initial version
==============================================================================*/

/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "rfcom.h"
#include "rfdevice_cmn_intf.h"
#include "rfdevice_pa.h"

#ifdef __cplusplus
extern "C" {
#endif


    /*==============================================================================
                             Data Definitions
    ==============================================================================*/

    typedef enum
    {
        RFDEVICE_PA_GET_MAX_SCRIPT_SIZE,
        RFDEVICE_PA_BAND_PORT_MAPPING,
        RFDEVICE_PA_INVALID_CMD
    }
    rfdevice_pa_cmd_type;

    typedef enum
    {
        RFDEVICE_PA_STATE_ET,
        RFDEVICE_PA_STATE_EPT,
        RFDEVICE_PA_STATE_APT,
        RFDEVICE_PA_STATE_LPM,
        RFDEVICE_PA_STATE_INVALID
    } rfdevice_pa_state_type;

    typedef struct
    {
        rfm_device_enum_type rfm_device;
        rfcom_mode_enum_type mode;
        rfcom_band_type_u band;
        rfdevice_pa_bw_enum_type bw;
        rfdevice_pa_trigger_enum_type  trigger_type;
    } rfdevice_pa_config_type;

    typedef struct
    {
        uint16  pa_gain_range;
        boolean is_mod_type_gmsk;
        boolean use_default_icq_pdm;
        uint16* pa_icq_array_p;
        uint16  pa_icq_array_count;
    } rfdevice_pa_txagc_type;


    /*==============================================================================
                             Prototypes
    ==============================================================================*/
    /*----------------------------------------------------------------------------*/
    boolean rfdevice_pa_config
    (
        rfdevice_pa_config_type *pa_config_p,
        void *device_ptr,
        void *buff_obj_ptr,
        rf_device_execution_type dev_action,
        int16 script_timing
    );

    /*----------------------------------------------------------------------------*/
    boolean rfdevice_set_pa_txagc
    (
        rfdevice_pa_config_type *pa_config_p,
        rfdevice_pa_txagc_type *pa_txagc_p,
        void *device_ptr,
        void *buff_obj_ptr,
        rf_device_execution_type dev_action,
        int16 script_timing
    );

    /*----------------------------------------------------------------------------*/
    boolean rfdevice_set_pa_on_off
    (
        rfdevice_pa_config_type *pa_config_p,
        boolean on_off,
        void *device_ptr,
        void *buff_obj_ptr,
        rf_device_execution_type dev_action,
        int16 script_timing
    );

    /*----------------------------------------------------------------------------*/

    boolean rfdevice_set_pa_trigger
    (
        rfdevice_pa_config_type *pa_config_p, void *device_ptr, void *buff_obj_ptr, rf_device_execution_type dev_action, int16 script_timing
    );


    /*----------------------------------------------------------------------------*/
    boolean rfdevice_get_pa_info(void *device_ptr);


    /*----------------------------------------------------------------------------*/
    boolean rfdevice_pa_sleep
    (
        rfdevice_pa_config_type *pa_config_p,
        void *device_ptr,
        void *buff_obj_ptr,
        rf_device_execution_type dev_action,
        int16 script_timing
    );

    /*----------------------------------------------------------------------------*/
    boolean rfdevice_pa_wakeup
    (
        rfdevice_pa_config_type *pa_config_p,
        void *device_ptr,
        void *buff_obj_ptr,
        rf_device_execution_type dev_action,
        int16 script_timing
    );

    /*----------------------------------------------------------------------------*/
    uint16 rfdevice_pa_get_script_size
    (
        void *device_ptr
    ) ;

    /*----------------------------------------------------------------------------*/
    boolean rfdevice_pa_set_band_map
    (
        void *device_ptr,
        rfcom_mode_enum_type mode,
        rfcom_band_type_u band,
        int32 *data_ptr,
        uint8 size
    );

    /*----------------------------------------------------------------------------*/
    boolean rfdevice_pa_get_device_info
    (
        void *device_ptr, uint16 *mfg_id, uint16 *prd_id, uint16 *rev_id
    );

    /*----------------------------------------------------------------------------*/
    boolean rfdevice_pa_therm_on_off
    (
        void *device_ptr, boolean on_off
    );

    /*----------------------------------------------------------------------------*/
    boolean rfdevice_get_pa_temperature
    (
        void *device_ptr, int16 *temperature
    );

    /*----------------------------------------------------------------------------*/
    boolean rfdevice_pa_set_switch_state
    (
        void *device_ptr,
        rfcom_mode_enum_type mode,
        rfcom_band_type_u band,
        uint8 switch_state_set
    );

   /*----------------------------------------------------------------------------*/
   boolean rfdevice_pa_dump_reg
   (
    void *device_ptr

   );

    /*----------------------------------------------------------------------------*/
    boolean rfdevice_pa_modify_ag_setting
    (
        void *device_ptr,
        rfdevice_pa_mod_cfg_type *pa_reg_cfg,
        rfdevice_mod_reg_info_type *pa_reg_data
    );

    boolean rfdevice_pa_get_max_bw_supported
    (
        void *device_ptr,	
        rfm_device_enum_type rfm_device, 
        rfcom_lte_band_type rf_band, 
        uint32* max_bw_khz
    );
  
#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_PA_INTF_H */


