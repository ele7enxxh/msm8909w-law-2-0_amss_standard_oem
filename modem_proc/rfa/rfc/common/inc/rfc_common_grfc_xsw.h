#ifndef RFC_COMMON_GRFC_XSW_H
#define RFC_COMMON_GRFC_XSW_H
/*!
   @file
   rfc_common_grfc_xsw.h

   @brief
   GRFC XSW (Cross Switch) device driver header file

*/

/*===========================================================================

Copyright (c) 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/common/inc/rfc_common_grfc_xsw.h#1 $

when       who   what, where, why
--------   ---   ---------------------------------------------------------------
05/14/14   dbc   Initial version

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfdevice_xsw.h"
#include "rfc_common.h"

class grfc_xsw: public rfdevice_xsw
{
public:

    boolean set_cross_switch_config
    (
      rfdevice_xsw_config_type* xsconfig,
      rf_buffer_intf*           buff_obj_ptr, 
      rf_device_execution_type  dev_action, 
      int16 script_timing       = RFCOM_INVALID_TIMING_OFFSET
    );

    boolean get_cross_switch_config
    (
      rfdevice_xsw_config_type* xsconfig
    );

    boolean set_band_map_default
    (
     rfcom_mode_enum_type mode, 
     rfcom_band_type_u band, 
     int32 *data_ptr, 
     uint8 size 
    );

    boolean set_band_map_swap
    (
     rfcom_mode_enum_type mode, 
     rfcom_band_type_u band, 
     int32 *data_ptr, 
     uint8 size 
    );

    boolean set_srs_band_map_default
    (
     rfcom_mode_enum_type mode, 
     rfcom_band_type_u band, 
     int32 *data_ptr, 
     uint8 size 
    );

    boolean set_srs_band_map_swap
    (
     rfcom_mode_enum_type mode, 
     rfcom_band_type_u band, 
     int32 *data_ptr, 
     uint8 size 
    );

    uint16 get_script_size(void);

    virtual ~grfc_xsw();

    boolean init(int32 *rfc_data);

    grfc_xsw( void );

private:
    void init();

    rfc_sig_cfg_type* cdma_band_mapping_default[RFM_CDMA_MAX_BAND];
    rfc_sig_cfg_type* cdma_band_mapping_swap[RFM_CDMA_MAX_BAND];

    rfc_sig_cfg_type* gsm_band_mapping_default[RFCOM_NUM_GSM_BANDS];
    rfc_sig_cfg_type* gsm_band_mapping_swap[RFCOM_NUM_GSM_BANDS];

    rfc_sig_cfg_type* wcdma_band_mapping_default[RFCOM_NUM_WCDMA_BANDS];
    rfc_sig_cfg_type* wcdma_band_mapping_swap[RFCOM_NUM_WCDMA_BANDS];

    rfc_sig_cfg_type* lte_band_mapping_default[RFCOM_NUM_LTE_BANDS];
    rfc_sig_cfg_type* lte_band_mapping_swap[RFCOM_NUM_LTE_BANDS];

    rfc_sig_cfg_type* tdscdma_band_mapping_default[RFCOM_NUM_TDSCDMA_BANDS];
    rfc_sig_cfg_type* tdscdma_band_mapping_swap[RFCOM_NUM_TDSCDMA_BANDS];

    rfc_sig_cfg_type* lte_srs_band_mapping_default[RFCOM_NUM_LTE_BANDS];
    rfc_sig_cfg_type* lte_srs_band_mapping_swap[RFCOM_NUM_LTE_BANDS];


};

#endif
