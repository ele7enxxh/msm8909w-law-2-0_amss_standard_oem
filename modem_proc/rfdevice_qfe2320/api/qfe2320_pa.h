#ifndef QFE2320_PA_H
#define QFE2320_PA_H
/*!
   @file
   qfe2320_pa.h

   @brief
   QFE2320 PA device driver header file

*/

/*===========================================================================

Copyright (c) 2012,2013 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qfe2320/api/qfe2320_pa.h#1 $

when         who   what, where, why
--------   ---   ---------------------------------------------------------------
05/12/14   vv     Physical device support
07/09/13   sbo    Added support for TDS and LTETDD
04/10/13   sn     Cleanup PA interface
10/17/12   tks    Added support for modifying pa current bias in ftm mode
09/25/12   tks    Added overloaded construtor to support dime rfc
09/12/12   sn     Added function is_band_valid
09/05/12   shb    Added set_band_map for compatibility with DIME interface
07/20/12   sn     Added time_ref_flag
07/12/12   sn     Removed command_dispatch function, added rtuner functions
06/21/12   sn     Initial version

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
#include "rfdevice_pa.h"
#include "qfe2320_typedef_ag.h"
#include "qfe2320_typedef.h"
#include "rfc_common.h"
#include "rfdevice_physical_device.h"

    class qfe2320_pa: public rfdevice_pa
    {
    public:

        boolean config
        (
            rfdevice_pa_cfg* pa_params_p,
            rf_buffer_intf *buff_obj_ptr,
            rf_device_execution_type dev_action,
            int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
        );

        boolean set_pa_txagc(rfdevice_pa_cfg* pa_params_p, rf_buffer_intf *script_buffer, 
		          rf_device_execution_type dev_action, 
                  int16 script_timing);

        boolean set_pa_on_off(rfdevice_pa_cfg* pa_params_p,rf_buffer_intf *script_buffer, 
		          rf_device_execution_type dev_action, 
                  int16 script_timing);
  
        boolean set_pa_trigger(rfdevice_pa_cfg* pa_params_p, rf_buffer_intf *script_buffer, rf_device_execution_type dev_action, 
                  int16 script_timing);
				  
		
        boolean sleep
        (
		rfdevice_pa_cfg* pa_params_p,
            rf_buffer_intf *buff_obj_ptr,
            rf_device_execution_type dev_action,
            int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
        );

        boolean wakeup
        (
		rfdevice_pa_cfg* pa_params_p,
            rf_buffer_intf *buff_obj_ptr,
            rf_device_execution_type dev_action,
            int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
        );

        uint16 get_script_size();

        boolean set_band_map
        (
            rfcom_mode_enum_type mode,
            rfcom_band_type_u band,
            int32 *data_ptr,
            uint8 size
        );

        virtual boolean self_test
        (
            uint32 mfg_id,
            uint32 prd_id,
            uint32 rev_id
        );

        virtual ~qfe2320_pa();

        /* Overloaded constructor to push the device information */
        qfe2320_pa
        (
            rfc_device_cfg_info_type *device_info,
            uint8 rev_id = 0
        );

        /* Overloaded constructor for physical device support */
        qfe2320_pa(rfdevice_physical_device *phy_dev_obj_ptr,rfc_phy_device_info_type *phy_device_info, rfc_logical_device_info_type *logical_device_info); 

        //qfe2320_pa(void);

    protected:

        void init ();

        /*Physical device object to store its parent physical device*/
        rfdevice_physical_device* parent_physical_device_ptr;
		
        boolean config
        (
            rfcom_mode_enum_type mode,
            rfcom_band_type_u band,
            rfdevice_pa_bw_enum_type bw,
            rf_buffer_intf *buff_obj_ptr,
            rf_device_execution_type dev_action,
            int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
        );	

        boolean set_band_port_mapping( rfcom_mode_enum_type mode, rfcom_band_type_u band, uint16 data );

        qfe2320_portnum_type gsm_pa_band_port_mapping[RFCOM_NUM_GSM_BANDS] ;

        qfe2320_portnum_type wcdma_pa_band_port_mapping[RFCOM_NUM_WCDMA_BANDS];

        qfe2320_portnum_type cdma_pa_band_port_mapping[RFM_CDMA_MAX_BAND] ;

        qfe2320_portnum_type ltefdd_pa_band_port_mapping[RFCOM_NUM_LTE_BANDS] ;

        qfe2320_portnum_type ltetdd_pa_band_port_mapping[RFCOM_NUM_LTE_BANDS] ;

        qfe2320_portnum_type tdscdma_pa_band_port_mapping[RFCOM_NUM_LTE_BANDS] ;

        uint8 rtuner_value;

        /*! RFFE Default Unique Slave ID */
        uint8 default_usid;

        uint8* pa_data;

        qfe2320_common_driver_settings_type pa_params;

        void qfe2320_rtuner_write_data(void);

        void qfe2320_rtuner_cal(void);

    };

#ifdef __cplusplus
}
#endif

#endif
