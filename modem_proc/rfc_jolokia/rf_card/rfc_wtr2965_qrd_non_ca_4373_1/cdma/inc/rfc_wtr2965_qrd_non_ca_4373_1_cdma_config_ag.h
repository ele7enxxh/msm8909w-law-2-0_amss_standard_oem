
#ifndef RFC_WTR2965_QRD_NON_CA_4373_1_CDMA_CONFIG_AG
#define RFC_WTR2965_QRD_NON_CA_4373_1_CDMA_CONFIG_AG

#ifdef __cplusplus
extern "C" {
#endif
/*
WARNING: This file is auto-generated.

Generated using: xmlautogen.exe
Generated from: V5.9.321 of RFC_HWSWCD.xlsm
*/

/*=============================================================================

          R F C     A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

Copyright (c) 2016 Qualcomm Technologies Incorporated.  All Rights Reserved.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfc_jolokia/rf_card/rfc_wtr2965_qrd_non_ca_4373_1/cdma/inc/rfc_wtr2965_qrd_non_ca_4373_1_cdma_config_ag.h#1 $ 


=============================================================================*/

/*=============================================================================
INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfc_msm_typedef.h"
#include "rfc_common.h"
#include "rfc_cdma_data.h"



class rfc_wtr2965_qrd_non_ca_4373_1_cdma_ag:public rfc_cdma_data
{
public:
  static rfc_cdma_data * get_instance();
  boolean sig_cfg_data_get( rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr );
  boolean devices_cfg_data_get( rfc_cfg_params_type *cfg, rfc_device_info_type **ptr );
  boolean band_split_cfg_data_get( rfc_cfg_params_type *cfg, rfc_band_split_info_type **ptr );

protected:
  rfc_wtr2965_qrd_non_ca_4373_1_cdma_ag(void);  /*  Constructor  */
};


#ifdef __cplusplus
}
#endif



#endif

