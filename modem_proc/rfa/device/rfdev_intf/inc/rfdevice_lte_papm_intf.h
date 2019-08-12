#ifndef RFDEVICE_LTE_PAPM_INTF_H
#define RFDEVICE_LTE_PAPM_INTF_H
/*!
   @file
   rfdevice_lte_papm_intf.h

   @brief
   

*/

/*===========================================================================

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who   what, where, why
--------   ---   --------------------------------------------------------------- 
03/19/15   al       KW error fix - Add array length as an input parameter 
10/27/14   vv       Add papm device info api support
07/10/13   sbm      non trigger script support. 
03/07/13   sr       changed the "pwr_tracker" name to "papm".
11/20/12   gvn      Featurize LTE for Triton  
11/12/12   cri/jf   Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfcom.h"
#include "rfm_device_types.h"
#ifdef FEATURE_LTE
#include "rfdevice_papm_intf.h"
#include "rfdevice_papm.h"


#include "rfdevice_lte_type_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
boolean rfdevice_lte_papm_enable(rfm_device_enum_type device, 
                                        rfcom_lte_band_type band, 
                                        rfcom_lte_bw_type bw,
                                        rfdevice_lte_script_data_type *script_data
                                        );

/*****************************************************************************/
boolean rfdevice_lte_papm_disable(rfm_device_enum_type device, 
                                         rfcom_lte_band_type band, 
                                         rfdevice_lte_script_data_type *script_data
                                         );

/*****************************************************************************/
boolean rfdevice_lte_papm_set_mode_bias( rfm_device_enum_type device, 
                                             rfcom_lte_band_type band,
                                             rfdevice_papm_mode_type xpt_mode,
                                             uint16 bias, 
                                             rfdevice_lte_script_data_type *script_data
                                             );

/*****************************************************************************/
boolean rfdevice_lte_papm_set_mode_bias_no_trigger( rfm_device_enum_type device, 
                                             rfcom_lte_band_type band,
                                             rfdevice_papm_mode_type xpt_mode,
                                             uint16 bias, 
                                             rfdevice_lte_script_data_type *script_data
                                             );

/*****************************************************************************/
boolean rfdevice_lte_papm_trigger(rfm_device_enum_type device, 
                                        rfcom_lte_band_type band, 
                                        rfdevice_lte_script_data_type *script_data
                                        );

/*****************************************************************************/
boolean rfdevice_lte_papm_get_device_info(rfm_device_enum_type rfm_device,
                                          rfcom_lte_band_type rf_band,
                                          uint16 *mfg_id,
                                          uint16 *prd_id,
                                          uint16 *rev_id,
										  uint16 array_len
                                          );
										  
boolean rfdevice_lte_papm_get_max_bw_supported
(
  rfm_device_enum_type rfm_device, 
  rfcom_lte_band_type rf_band, 
  uint32* max_bw_khz
);										  

#ifdef __cplusplus
}
#endif

#endif /*FEATURE_LTE*/
#endif /* RFDEVICE_PA_INTF_H */
