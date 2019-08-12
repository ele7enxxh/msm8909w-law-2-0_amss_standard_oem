#ifndef RFC_EFS_CARD_LTE_DATA
#define RFC_EFS_CARD_LTE_DATA
/*!
  @file
  rfc_efs_card_lte_data.h

  @brief
  This file contains the class definition for the rfc_efs_card_lte_data, which implements the
  functionality to get/set the RFC related data.

*/

/*===========================================================================

Copyright (c) 2011-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/rf_card/rfc_efs_card/lte/inc/rfc_efs_card_lte_data.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------   
03/14/14   Saul    Added RF_HW_EFS_CARD
01/03/13   vrb     Add in Timing config get LTE interface for EFS card
11/08/12   sr      initial version
============================================================================*/
#include "comdef.h"
#include "rfc_msm_typedef.h" 
#include "rfc_common.h" 
#include "rfc_lte_data.h" 

#ifdef __cplusplus
extern "C" {
#endif


class rfc_efs_card_lte_data:public rfc_lte_data
{
public:
  static rfc_lte_data * get_instance();

  boolean sig_cfg_data_get( rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr );
  boolean devices_cfg_data_get( rfc_cfg_params_type *cfg, rfc_device_info_type **ptr );
  boolean timing_cfg_data_get( rfc_cfg_params_type *cfg, rfc_timing_info_type **ptr );
  boolean ca_scell_log_path_swap_data_get( rfc_band_pair_list_type **ptr );
  boolean band_split_cfg_data_get( rfc_cfg_params_type *cfg, rfc_band_split_info_type **ptr );
  // Destructor
  virtual ~rfc_efs_card_lte_data();

protected:
  rfc_efs_card_lte_data(void);  /*  Constructor  */

private:
  void efs_data_load(void);
  boolean verify_rfc_data(void);


};


#ifdef __cplusplus
}
#endif



#endif


