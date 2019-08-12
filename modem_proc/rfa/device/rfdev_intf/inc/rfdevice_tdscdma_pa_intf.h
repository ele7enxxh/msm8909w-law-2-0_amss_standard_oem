#ifndef RFDEVICE_TDSCDMA_PA_INTF_H
#define RFDEVICE_TDSCDMA_PA_INTF_H
/*!
   @file
   rfdevice_tdscdma_pa_intf.h

   @brief


*/

/*===========================================================================

Copyright (c) 2012, 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header:

when       who    what, where, why
--------   ---    --------------------------------------------------------------- 
03/19/15   al     Add array length as an input parameter to get PA info 
06/25/13   jyu    Added support to call set_gain_range and on_off() w or w/
                  trigger
06/06/13   jyu    Added support to combine set_range and turn on PA
04/03/13   sn     Cleanup PA interface
03/21/13   yzw    Add device driver support to query PA device information {mfg_id, prd_id, rev_id}
03/11/13   sar    Updated file for APQ, GNSS only target.
03/07/13   sr     changed the "pwr_tracker" name to "papm".
01/14/13   jyu    Added RFFE PA support (new format) for TDSCDMA
12/21/12   jyu    Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#ifdef FEATURE_TDSCDMA
#include "rfcom.h"
#include "rfdevice_pa_intf.h"
#include "rfdevice_tdscdma_type_defs.h"
#include "rf_buffer_intf.h"
#include "rfdevice_papm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RF_TDS_PA_USE_RFC_TIMING 0xFFF

    /*===========================================================================
                             Prototypes
    ===========================================================================*/
    /* ----------------------------------------------------------------------- */
    /* function prototypes */
    boolean rfdevice_tdscdma_use_non_grfc_pa
    (
        rfm_device_enum_type device,
        rfcom_tdscdma_band_type rf_band
    );

    boolean rfdevice_tdscdma_pa_config
    (
        rfm_device_enum_type device,
        rfcom_tdscdma_band_type rf_band,
        rf_buffer_intf *script,
        rf_device_execution_type execution_type,
        int16 script_timing
    );

    boolean rfdevice_tdscdma_pa_on_off
    (
        rfm_device_enum_type device,
        rfcom_tdscdma_band_type rf_band,
        boolean on_off,
        rf_buffer_intf *script,
        rf_device_execution_type execution_type,
        int16 script_timing
    );


    boolean rfdevice_tdscdma_pa_set_gain_range
    (
        rfm_device_enum_type device,
        rfcom_tdscdma_band_type rf_band,
        uint16 pa_gain_range,
        rf_buffer_intf *script,
        rf_device_execution_type execution_type,
        int16 script_timing
    );

    boolean rfdevice_tdscdma_pa_set_gain_range_no_trigger
    (
        rfm_device_enum_type device,
        rfcom_tdscdma_band_type rf_band,
        uint16 pa_gain_range,
        rf_buffer_intf *script,
        rf_device_execution_type execution_type,
        int16 script_timing
    );


    boolean rfdevice_tdscdma_pa_set_current_bias
    (
        rfm_device_enum_type device,
        rfcom_tdscdma_band_type rf_band,
        uint8 pa_gain_range,
        uint16 pa_icq_pdm,
        rf_buffer_intf *script,
        rf_device_execution_type execution_type,
        int16 script_timing
    );

    boolean rfdevice_tdscdma_pa_init
    (
        rfm_device_enum_type device,
        rfcom_tdscdma_band_type rf_band,
        void *data,
        rf_buffer_intf *script,
        rf_device_execution_type execution_type
    );

    boolean rfdevice_tdscdma_pa_sleep
    (
        rfm_device_enum_type device,
        rfcom_tdscdma_band_type rf_band,
        rf_buffer_intf *script,
        rf_device_execution_type execution_type,
        int16 script_timing
    );

    boolean rfdevice_tdscdma_pa_wakeup
    (
        rfm_device_enum_type device,
        rfcom_tdscdma_band_type rf_band,
        rf_buffer_intf *script,
        rf_device_execution_type execution_type,
        int16 script_timing
    );

    boolean rfdevice_tdscdma_pa_command_dispatch
    (
        rfm_device_enum_type device,
        rfcom_tdscdma_band_type rf_band,
        rfdevice_pa_cmd_type cmd,
        void *data,
        rf_buffer_intf *script,
        rf_device_execution_type execution_type
    );

    uint16 rfdevice_tdscdma_pa_get_script_size
    (
        rfm_device_enum_type device,
        rfcom_tdscdma_band_type rf_band
    ) ;

    boolean rfdevice_tdscdma_pa_set_band_map
    (
        rfm_device_enum_type rfm_device,
        rfcom_tdscdma_band_type rf_band,
        int32 *data_ptr,
        uint8 size
    );

    boolean rfdevice_tdscdma_pa_get_device_info
    (
        rfm_device_enum_type rfm_device,
        rfcom_tdscdma_band_type rf_band,
        uint16 *mfg_id,
        uint16 *prd_id,
        uint16 *rev_id,
        uint16 array_len
    );


#ifdef __cplusplus
}
#endif
#endif /* FEATURE_TDSCDMA */
#endif /* RFDEVICE_PA_INTF_H */



