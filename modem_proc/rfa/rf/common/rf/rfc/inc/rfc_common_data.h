#ifndef RFC_COMMON_DATA_H
#define RFC_COMMON_DATA_H
/*!
  @file
  rfc_data.h

  @brief
  This file contains the class definition for the rfc_common_data, which provides the rfc related
  data retrival functionality to RFC clients.

*/

/*===========================================================================

Copyright (c) 2011-2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/rfc/inc/rfc_common_data.h#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
02/24/15   am      Change IRAT alt path lookup implementation
09/29/14   Saul    New alt path selection APIs.
08/08/14   tks     Modified get alt path table function param list 
07/14/14   vrb     Support for Alternate Path Selection Table
06/16/14   vv      Added physical and logical device cfg structures
04/23/14   tks     Support for dynamic path selection
03/17/14   kg      Port from Dime
02/11/14   sr      Re-design asdiv_tuner to support multiple RF configurations
08/26/13   jr      Added FTM_GET_EFS_VERIFICATION_DATA
05/21/13   sd      Add new api to get cmn device properties
11/26/12   sr      Added flag to capture the rfc data init status.
09/24/12   sr      Made changes to remove hand-coded RFC files.
07/18/12   sr      Changes to create devices through device factory.
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function ptr table.
05/09/12   sr      init version
============================================================================*/

#include "rfa.h"
#include "rfc_common.h"
#include "rfm_device_config_type.h"

/* Definition of rfc_data class */
class rfc_common_data : public rfa
{
public:

  static rfc_common_data *get_instance(void);

  // rfc common data
  virtual uint32 sig_info_table_get(rfc_signal_info_type **rfc_info_table);

  virtual rfc_logical_device_info_type* get_logical_device_cfg( void );

  virtual rfc_phy_device_info_type* get_phy_device_cfg( void );

  virtual boolean get_rfcard_data( void );

  virtual boolean get_logical_path_config(rfm_devices_configuration_type* dev_cfg);

  virtual boolean get_cmn_properties(rfc_cmn_properties_type **ptr);

  virtual rfc_alt_path_sel_type* get_alt_path_selection_tbl(uint32 *tbl_size);
  virtual rfc_alt_path_sel_type* get_irat_alt_path_selection_tbl(uint32 *tbl_size, uint32 *num_band_groups);
  
  virtual boolean get_asd_device_info( rfc_asd_cfg_params_type *cfg, rfc_asdiv_config_info_type **ptr );

  void set_rfc_init_fail_flag(rfm_mode_enum_type mode);
  uint32 get_rfc_init_fail_flag(void);

  // Destructor
  virtual ~rfc_common_data();

protected:

  // constructor
  rfc_common_data(rf_hw_type rf_hw);

  static rfc_common_data *rfc_common_data_ptr;

  rf_hw_type m_rf_hw;

private:

  uint32 rfc_init_flag;

};

#endif /* RFC_COMMON_DATA_H */

