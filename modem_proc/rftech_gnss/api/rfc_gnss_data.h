#ifndef RFC_GNSS_DATA_H
#define RFC_GNSS_DATA_H
/*!
  @file
  rfc_gnss_data.h

  @brief
  This file contains the class definition for the rfc_gnss_data, which provides the rfc related data
  retrival functionality to GNSS.

*/

/*===========================================================================

Copyright (c) 2011-13 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gnss/api/rfc_gnss_data.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
08/26/13   jr      Added FTM_GET_EFS_VERIFICATION_DATA
03/12/13   vrb     Adding support for getting band split configuration per RFC
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function ptr table.
05/09/12   sr      init version
============================================================================*/

#include "rfa.h"
#include "rfc_common.h"


/* Definition of rfc_gnss_data class */
class rfc_gnss_data : public rfa
{
public:

  static rfc_gnss_data * get_instance();

  // GNSS rfc data
  virtual boolean sig_cfg_data_get( rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr );
  virtual boolean devices_cfg_data_get( rfc_cfg_params_type *cfg, rfc_device_info_type **ptr );
  virtual boolean band_split_cfg_data_get( rfc_cfg_params_type *cfg, rfc_band_split_info_type **ptr );
  virtual boolean get_rfcard_data( void );


  // Destructor
  virtual ~rfc_gnss_data();

protected:

  // constructor
  rfc_gnss_data();

  static rfc_gnss_data *rfc_gnss_data_ptr;

private:

};

#endif /* RFC_GNSS_DATA_H */

