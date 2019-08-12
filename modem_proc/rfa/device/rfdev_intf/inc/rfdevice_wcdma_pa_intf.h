#ifndef RFDEVICE_WCDMA_PA_INTF_H
#define RFDEVICE_WCDMA_PA_INTF_H
/*!
   @file
   rfdevice_wcdma_pa_intf.h

   @brief


*/

/*===========================================================================

Copyright (c) 2012 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header:

when       who   what, where, why
--------   ---   --------------------------------------------------------------- 
03/19/15   al    KW error fix - Add array length as an input parameter 
10/27/14   vv     Add papm device info api support
03/11/13   dw      Add Bolt DCHSUPA support
04/03 13   sn     Cleanup PA interface
03/21/13  yzw     Add device driver support to query PA device information {mfg_id, prd_id, rev_id}
03/11/13   kcj    Added support to enable/disable papm.
03/11/13   sar     Updated file for APQ, GNSS only target.
03/07/13   sr     changed the "pwr_tracker" name to "papm".
10/17/12   tks    Added support for modifying pa current bias in ftm mode
10/11/12   dw     SVVZ bringup updates
08/12/12   vbh    Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#ifdef FEATURE_WCDMA
#include "rfcom.h"
#include "rfdevice_pa_intf.h"
#include "rfdevice_wcdma_type_defs.h"
#include "rf_buffer_intf.h"
#include "rfdevice_papm.h"

#ifdef __cplusplus
extern "C" {
#endif

    /*===========================================================================
                             Prototypes
    ===========================================================================*/
    /* ----------------------------------------------------------------------- */
    /* function prototypes */
    boolean rfdevice_wcdma_pa_config
    (
        rfm_device_enum_type device,
        rfcom_wcdma_band_type rf_band,
        rf_buffer_intf *script,
        rf_device_execution_type execution_type,
        int16 script_timing,
        rfdevice_pa_bw_enum_type tx_bw
    );

    boolean rfdevice_wcdma_pa_on_off
    (
        rfm_device_enum_type device,
        rfcom_wcdma_band_type rf_band,
        boolean on_off,
        rf_buffer_intf *script,
        rf_device_execution_type execution_type,
        int16 script_timing
    );

    boolean rfdevice_wcdma_pa_set_gain_range
    (
        rfm_device_enum_type device,
        rfcom_wcdma_band_type rf_band,
        uint16 pa_gain_range,
        rf_buffer_intf *script,
        rf_device_execution_type execution_type,
        int16 script_timing
    );

    boolean rfdevice_wcdma_pa_set_current_bias
    (
        rfm_device_enum_type device,
        rfcom_wcdma_band_type rf_band,
        uint8 pa_gain_range,
        uint16 pa_icq_pdm,
        rf_buffer_intf *script,
        rf_device_execution_type execution_type,
        int16 script_timing
    );

    boolean rfdevice_wcdma_pa_init
    (
        rfm_device_enum_type device,
        rfcom_wcdma_band_type rf_band,
        void *data,
        rf_buffer_intf *script,
        rf_device_execution_type execution_type
    );

    boolean rfdevice_wcdma_pa_sleep
    (
        rfm_device_enum_type device,
        rfcom_wcdma_band_type rf_band,
        rf_buffer_intf *script,
        rf_device_execution_type execution_type,
        int16 script_timing
    );

    boolean rfdevice_wcdma_pa_wakeup
    (
        rfm_device_enum_type device,
        rfcom_wcdma_band_type rf_band,
        rf_buffer_intf *script,
        rf_device_execution_type execution_type,
        int16 script_timing
    );

    boolean rfdevice_wcdma_pa_command_dispatch
    (
        rfm_device_enum_type device,
        rfcom_wcdma_band_type rf_band,
        rfdevice_pa_cmd_type cmd,
        void *data,
        rf_buffer_intf *script,
        rf_device_execution_type execution_type
    );

    uint16 rfdevice_wcdma_pa_get_script_size
    (
        rfm_device_enum_type device,
        rfcom_wcdma_band_type rf_band
    ) ;

    boolean rfdevice_wcdma_papm_set_mode_bias
    (
        rfm_device_enum_type rfm_device,
        rfcom_wcdma_band_type rf_band,
        rfdevice_papm_mode_type xpt_mode,
        uint16 bias,
        rf_buffer_intf *script,
        rf_device_execution_type execution_type
    );

    boolean rfdevice_wcdma_papm_disable
    (
        rfm_device_enum_type rfm_device,
        rfcom_wcdma_band_type rf_band,
        rf_buffer_intf *script
    );

    boolean rfdevice_wcdma_papm_enable
    (
        rfm_device_enum_type rfm_device,
        rfcom_wcdma_band_type rf_band,
        rf_buffer_intf *script
    );

    boolean rfdevice_wcdma_pa_get_device_info
    (
        rfm_device_enum_type rfm_device,
        rfcom_wcdma_band_type rf_band,
        uint16 *mfg_id,
        uint16 *prd_id,
        uint16 *rev_id,
		uint16 array_len
    );

    boolean rfdevice_wcdma_papm_get_device_info
   (
    rfm_device_enum_type rfm_device,
    rfcom_wcdma_band_type rf_band,
    uint16 *mfg_id,
    uint16 *prd_id,
    uint16 *rev_id,
	uint16 array_len
    );


#ifdef __cplusplus
}
#endif
#endif /* FEATURE_WCDMA */
#endif /* RFDEVICE_PA_INTF_H */


