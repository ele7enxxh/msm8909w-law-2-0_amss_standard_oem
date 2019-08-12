#ifndef RFC_EFS_CARD_CDMA_DATA
#define RFC_EFS_CARD_CDMA_DATA
/*!
  @file
  rfc_efs_card_cdma_data.h

  @brief
  This file contains the class definition for the rfc_efs_card_cdma_data, which implements the
  functionality to get/set the RFC related data.

*/

/*===========================================================================

Copyright (c) 2011-12 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/rf_card/rfc_efs_card/cdma/inc/rfc_efs_card_cdma_data.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------   
11/08/12   sr      initial version
============================================================================*/
#include "comdef.h"
#include "rfc_msm_typedef.h" 
#include "rfc_common.h" 
#include "rfc_cdma_data.h" 

#ifdef __cplusplus
extern "C" {
#endif


class rfc_efs_card_cdma_data:public rfc_cdma_data
{
public:
  static rfc_cdma_data * get_instance();

  boolean sig_cfg_data_get( rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr );
  boolean devices_cfg_data_get( rfc_cfg_params_type *cfg, rfc_device_info_type **ptr );

  // Destructor
  virtual ~rfc_efs_card_cdma_data();


protected:
  rfc_efs_card_cdma_data(void);  /*  Constructor  */

private:
  void efs_data_load(void);
  boolean verify_rfc_data(void);
};


#ifdef __cplusplus
}
#endif



#endif


