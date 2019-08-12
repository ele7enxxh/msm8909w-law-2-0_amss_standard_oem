#ifndef RFC_WCDMA_DATA_H
#define RFC_WCDMA_DATA_H
/*!
  @file
  rfc_wcdma_data.h

  @brief
  This file contains the class definition for the rfc_wcdma_data, which provides the rfc related data
  retrival functionality to WCDMA.

*/

/*===========================================================================

Copyright (c) 2011-13 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfc_wcdma_data.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
08/26/13   jr      Added FTM_GET_EFS_VERIFICATION_DATA
07/08/13   sd      Adding support for WCDMA DBDC data per RFC
03/12/13   vrb     Adding support for getting band split configuration per RFC
09/06/12   aa      added function to get device info
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function ptr table.
05/09/12   sr      init version
============================================================================*/
#ifdef __cplusplus
#include "rfa.h"
#include "rfc_common.h"


/* Definition of rfc_wcdma_data class */
class rfc_wcdma_data : public rfa
{
public:

  static rfc_wcdma_data * get_instance();

  // WCDMA rfc data
  virtual boolean sig_cfg_data_get( rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr );
  virtual boolean devices_cfg_data_get( rfc_cfg_params_type *cfg, rfc_device_info_type **ptr );
  virtual boolean band_split_cfg_data_get( rfc_cfg_params_type *cfg, rfc_band_split_info_type **ptr );
  virtual boolean get_wcdma_properties( rfc_wcdma_properties_type **ptr );
  virtual boolean get_rfcard_data( void );

  // Destructor
  virtual ~rfc_wcdma_data();

protected:

  // constructor
  rfc_wcdma_data();

  static rfc_wcdma_data *rfc_wcdma_data_ptr;

private:

};

#endif /* __cplusplus */

#ifdef __cplusplus
extern "C"
{
#endif

boolean rfc_wcdma_data_get_device_info(rfc_cfg_params_type *cfg, rfc_device_info_type **device_info_pptr);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* RFC_WCDMA_DATA_H */

