#ifndef RFC_LTE_DATA_H
#define RFC_LTE_DATA_H
/*!
  @file
  rfc_lte_data.h

  @brief
  This file contains the class definition for the rfc_lte_data, which provides the rfc related data
  retrival functionality to LTE.

*/

/*===========================================================================

Copyright (c) 2011-12 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/rfc_lte_data.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------   
11/10/15   fhuo    Add get_lte_properties() for LTE partial band support 
08/26/13   jr      Added FTM_GET_EFS_VERIFICATION_DATA
04/25/13   gvn     Support for Split bands for LTE 
04/01/13   sd      Adding support for SCELL Log Path Swap API
03/12/13   vrb     Adding support for getting band split configuration per RFC
11/07/12   sbm     C function to get rfc timing.
10/11/12   vrb     Added interface to get generated timing information 
08/28/12   pl/sbm  Added C function to retrieve device info
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function ptr table.
05/09/12   sr      init version
============================================================================*/
#ifdef __cplusplus
#include "rfa.h"
#include "rfc_common.h"

/* Definition of rfc_lte_data class */
class rfc_lte_data : public rfa 
{
public:

  static rfc_lte_data * get_instance();

  // LTE rfc data 
  virtual boolean sig_cfg_data_get( rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr );
  virtual boolean devices_cfg_data_get( rfc_cfg_params_type *cfg, rfc_device_info_type **ptr );
  virtual boolean timing_cfg_data_get( rfc_cfg_params_type *cfg, rfc_timing_info_type **ptr );
  virtual boolean band_split_cfg_data_get( rfc_cfg_params_type *cfg, rfc_band_split_info_type **ptr );
  virtual boolean ca_scell_log_path_swap_data_get( rfc_band_pair_list_type **ptr );
  virtual boolean get_lte_properties( rfc_lte_properties_type **ptr );
  virtual boolean get_rfcard_data( void );

  // Destructor
  virtual ~rfc_lte_data();

protected:

  // constructor
  rfc_lte_data();
  
  static rfc_lte_data *rfc_lte_data_ptr;

private:

};
#endif /* __cplusplus */

#ifdef __cplusplus
extern "C"
{
#endif

boolean rfc_lte_data_get_device_info(rfc_cfg_params_type *cfg, rfc_device_info_type **device_info_pptr);
boolean rfc_lte_data_get_timing_info(rfc_cfg_params_type *cfg, rfc_timing_info_type **device_info_pptr);
boolean rfc_lte_data_get_band_split_info(rfc_cfg_params_type *cfg, rfc_band_split_info_type **ptr);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* RFC_LTE_DATA_H */

