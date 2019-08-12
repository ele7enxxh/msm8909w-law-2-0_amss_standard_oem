#ifndef RFDEVICE_LTE_PA_INTF_H
#define RFDEVICE_LTE_PA_INTF_H
/*!
   @file
   rfdevice_lte_pa_intf.h

   @brief


*/

/*===========================================================================

Copyright (c) 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header:

when       who   what, where, why
--------   ---   --------------------------------------------------------------- 
03/19/15   al    KW error fix - Add array length as an input parameter 
07/10/13   sbm   non trigger script support.
07/07/13   sbo   Added BW support for PA device
04/03 13   sn    Cleanup PA interface
03/21/13  yzw    Add FTM cmd and device driver support to query RFFE device information {mfg_id, prd_id, rev_id}
03/11/13   sar     Updated file for APQ, GNSS only target.
10/31/12   vss   Change parameters in set band port mappings to pass device instance,
                 instead of rfm device
10/17/12   tks    Added support for modifying pa current bias in ftm mode
08/01/12   vss    Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#ifdef FEATURE_LTE

#include "rfcom.h"
#include "rfdevice_pa_intf.h"
#include "rfdevice_lte_type_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

    /*===========================================================================
                             Prototypes
    ===========================================================================*/
    /* ----------------------------------------------------------------------- */
    /* function prototypes */
    boolean rfdevice_lte_pa_config
    (
        rfm_device_enum_type device,
        rfcom_lte_band_type rf_band,
        rfcom_lte_bw_type bw,
        rfdevice_lte_script_data_type* script_data_ptr,
        int16 script_timing
    );


    boolean rfdevice_lte_pa_on_off_no_trigger
    (
        rfm_device_enum_type device,
        rfcom_lte_band_type rf_band,
        boolean on_off,
        rfdevice_lte_script_data_type* script_data_ptr,
        int16 script_timing
    );

    boolean rfdevice_lte_pa_set_txagc_no_trigger
    (
        rfm_device_enum_type device,
        rfcom_lte_band_type rf_band,
        uint16 pa_gain_range,
        uint16 pa_icq_pdm,
        rfdevice_lte_script_data_type* script_data_ptr,
        int16 script_timing
    );

    boolean rfdevice_lte_pa_set_gain_range_no_trigger
    (
        rfm_device_enum_type device,
        rfcom_lte_band_type rf_band,
        uint16 pa_gain_range,
        rfdevice_lte_script_data_type* script_data_ptr,
        int16 script_timing
    );

    boolean rfdevice_lte_pa_set_current_bias_no_trigger
    (
        rfm_device_enum_type device,
        rfcom_lte_band_type rf_band,
        uint8 pa_gain_range,
        uint16 pa_icq_pdm,
        rfdevice_lte_script_data_type* script_data_ptr,
        int16 script_timing
    );

    boolean rfdevice_lte_pa_init
    (
        rfm_device_enum_type device,
        rfcom_lte_band_type rf_band,
        void *data,
        rfdevice_lte_script_data_type* script_data_ptr
    );

    boolean rfdevice_lte_pa_sleep
    (
        rfm_device_enum_type device,
        rfcom_lte_band_type rf_band,
        rfdevice_lte_script_data_type* script_data_ptr,
        int16 script_timing
    );

    boolean rfdevice_lte_pa_wakeup
    (
        rfm_device_enum_type device,
        rfcom_lte_band_type rf_band,
        rfdevice_lte_script_data_type* script_data_ptr,
        int16 script_timing
    );

    boolean rfdevice_lte_pa_command_dispatch
    (
        rfm_device_enum_type device,
        rfcom_lte_band_type rf_band,
        rfdevice_pa_cmd_type cmd,
        void *data,
        rfdevice_lte_script_data_type* script_data_ptr
    );

    uint16 rfdevice_lte_pa_get_script_size
    (
        rfm_device_enum_type device,
        rfcom_lte_band_type rf_band
    ) ;

    boolean rfdevice_lte_pa_get_device_info
    (
        rfm_device_enum_type rfm_device,
        rfcom_lte_band_type rf_band,
        uint16 *mfg_id,
        uint16 *prd_id,
        uint16 *rev_id,
		uint16 array_len
    );

    boolean rfdevice_lte_pa_trigger
    (
        rfm_device_enum_type device,
        rfcom_lte_band_type rf_band,
        rfdevice_lte_script_data_type* script_data_ptr,
        int16 script_timing
    );
	
    boolean rfdevice_lte_pa_get_max_bw_supported
    (
        rfm_device_enum_type rfm_device, 
        rfcom_lte_band_type rf_band, 
        uint32* max_bw_khz
    );

#ifdef __cplusplus
}
#endif
#endif /* FEATURE_LTE */

#endif /* RFDEVICE_PA_INTF_H */


