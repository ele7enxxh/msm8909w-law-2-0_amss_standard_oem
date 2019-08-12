#ifndef RFC_COMMON_GRFC_PA_H
#define RFC_COMMON_GRFC_PA_H
/*!
   @file
   rfc_common_grfc_pa.h

   @brief
   GRFC PA device driver header file

*/

/*===========================================================================

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/common/inc/rfc_common_grfc_pa.h#1 $

when       who   what, where, why
--------   ---   ---------------------------------------------------------------
07/07/13   sbo   Added BW support for PA device
6/20/13    vbh    Added explicit trigger for the PA device
12/11/12   zhw   void init() API for interface compatibility
12/05/12   zhw   Initial version

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfdevice_pa.h"
#include "rfdevice_pa_intf.h"
#include "rfc_common.h"

class grfc_pa: public rfdevice_pa
{
public:

    boolean config( rfdevice_pa_cfg* pa_params_p,
                    rf_buffer_intf *script_buffer,
                    rf_device_execution_type dev_action,
                    int16 script_timing = 0 );

    boolean set_pa_on_off (  rfdevice_pa_cfg* pa_params_p,
                             rf_buffer_intf *script_buffer,
                             rf_device_execution_type dev_action,
                             int16 script_timing = 0 );


    boolean set_pa_txagc ( rfdevice_pa_cfg* pa_params_p,
                           rf_buffer_intf *script_buffer,
                           rf_device_execution_type dev_action,
                           int16 script_timing = 0 );


    boolean sleep( rfdevice_pa_cfg* pa_params_p,
                   rf_buffer_intf *script_buffer,
                   rf_device_execution_type dev_action,
                   int16 script_timing = 0 );

    boolean wakeup( rfdevice_pa_cfg* pa_params_p,
                    rf_buffer_intf *script_buffer,
                    rf_device_execution_type dev_action,
                    int16 script_timing = 0 );

    uint16 get_script_size();

    boolean set_band_port_mapping( rfcom_mode_enum_type mode,
                                   rfcom_band_type_u band,
                                   uint16 data );

    boolean set_band_map( rfcom_mode_enum_type mode,
                          rfcom_band_type_u band,
                          int32 *data_ptr,
                          uint8 size );

    virtual ~grfc_pa();

    grfc_pa( void );

private:
    void init();
    boolean set_gain_state( rfcom_mode_enum_type mode,
                            rfcom_band_type_u band,
                            rfdevice_pa_state_type pa_gain_state,
                            boolean mod_type,
                            rf_buffer_intf *script_buffer,
                            rf_device_execution_type dev_action,
                            int16 script_timing = 0);

    rfc_sig_cfg_type* cdma_tx_band_mapping[RFM_CDMA_MAX_BAND];

    rfc_sig_cfg_type* gsm_tx_band_mapping[RFCOM_NUM_GSM_BANDS];

    rfc_sig_cfg_type* wcdma_tx_band_mapping[RFCOM_NUM_WCDMA_BANDS];

    rfc_sig_cfg_type* lte_tx_band_mapping[RFCOM_NUM_LTE_BANDS];

    rfc_sig_cfg_type* tdscdma_tx_band_mapping[RFCOM_NUM_TDSCDMA_BANDS];

};

#endif
