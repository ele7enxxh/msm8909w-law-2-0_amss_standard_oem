#ifndef RFC_TDSCDMA_DATA_H
#define RFC_TDSCDMA_DATA_H
/*!
  @file
  rfc_tdscdma_data.h

  @brief
  This file contains the class definition for the rfc_tdscdma_data, which provides the rfc related data
  retrival functionality to TDSCDMA.

*/

/*===========================================================================

Copyright (c) 2011-13 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/api/rfc_tdscdma_data.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
12/13/13   cc      Unify gpio type to rfc_sig_cfg_type for FDD and TDD tech
08/26/13   jr      Added FTM_GET_EFS_VERIFICATION_DATA
05/20/13   al      Added function to get split band info
03/12/13   vrb     Adding support for getting band split configuration per RFC
09/27/12   jyu     Added support to query rfdevice_info_ptr
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function ptr table.
05/09/12   sr      init version
============================================================================*/
#ifdef __cplusplus
#include "rfa.h"
#include "rfc_common.h"


/* Definition of rfc_tdscdma_data class */
class rfc_tdscdma_data : public rfa
{
public:

  static rfc_tdscdma_data * get_instance();

  // TDSCDMA rfc data
  virtual boolean sig_cfg_data_get( rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr );
  virtual boolean devices_cfg_data_get( rfc_cfg_params_type *cfg, rfc_device_info_type **ptr );
  virtual boolean band_split_cfg_data_get( rfc_cfg_params_type *cfg, rfc_band_split_info_type **ptr );
  virtual boolean get_rfcard_data( void );

  // Destructor
  virtual ~rfc_tdscdma_data();

protected:

  // constructor
  rfc_tdscdma_data();

  static rfc_tdscdma_data *rfc_tdscdma_data_ptr;

private:

};

#endif /* __cplusplus */

#ifdef __cplusplus
extern "C"
{
#endif

boolean rfc_tdscdma_get_devices_info_data(rfc_cfg_params_type *cfg, rfc_device_info_type **device_info_pptr);
boolean rfc_tdscdma_data_get_band_split_info(rfc_cfg_params_type *cfg, rfc_band_split_info_type **ptr);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* RFC_TDSCDMA_DATA_H */

