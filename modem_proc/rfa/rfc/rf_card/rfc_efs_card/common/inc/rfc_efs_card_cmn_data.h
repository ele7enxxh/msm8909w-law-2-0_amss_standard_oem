#ifndef RFC_EFS_CARD_CMN_DATA
#define RFC_EFS_CARD_CMN_DATA

/*!
  @file
  rfc_efs_card_cmn_data.h

  @brief
  This file contains the class definition for the rfc_efs_card_cmn_data, which implements the
  functionality to get/set the RFC related data.

*/

/*===========================================================================

Copyright (c) 2011-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/rf_card/rfc_efs_card/common/inc/rfc_efs_card_cmn_data.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------   
05/09/14   Saul    Fix EFS RFC
03/14/14   Saul    Added RF_HW_EFS_CARD
07/28/13   jr      Support for concurrency restriction, common properties and
                   GSM spur desense channels.
07/11/13   jr      increased RFC_EFS_MAX_DATA_SIZE to 2048
11/08/12   sr      initial version
============================================================================*/
#include "comdef.h"

#include "rfc_common.h" 

const uint16 RFC_EFS_MAX_DATA_SIZE = 4096;
const int8  RFC_EFS_MAX_STRING_SIZE = 80;
const char RFC_EFS_FILE_PATH[] =  "/rfc" ;              // Path to RFC files in EFS

typedef enum
{
  RFC_SIG_CFG_DATA,
  RFC_DEVICE_CFG_DATA,
  RFC_PROPERTIES_DATA,
  RFC_SPLIT_CFG_DATA,
  RFC_DATA_TYPE_MAX,
  RFC_DATA_TYPE_INVALID = RFC_DATA_TYPE_MAX,
} rfc_data_enum_type;


#ifdef __cplusplus

#include "rfc_common_data.h"

class rfc_efs_card_cmn_data:public rfc_common_data
{
  public:
    static rfc_common_data * get_instance(rf_hw_type rf_hw = RF_HW_UNDEFINED);

    uint32 sig_info_table_get(rfc_signal_info_type **rfc_info_table);
    rfc_phy_device_info_type* get_phy_device_cfg( void );
    rfc_logical_device_info_type* get_logical_device_cfg( void );
    boolean get_logical_path_config(rfm_devices_configuration_type* dev_cfg);

    int32 read_init_data(rfm_mode_enum_type mode, rfc_rxtx_enum_type rx_tx, 
                         rfc_data_enum_type rfc_data_type, uint8 **rfc_data);
    int32 read_band_data(rfm_device_enum_type path, rfm_mode_enum_type mode, rfc_rxtx_enum_type rx_tx, 
                         int band, rfc_data_enum_type rfc_data_type, uint8 **rfc_data);

    int32 read_tech_specific_data(rfm_mode_enum_type mode,rfc_data_enum_type rfc_data_type, uint8 **rfc_data);

    boolean verify_rfc_data(rfc_data_enum_type rfc_data_type, uint8 *rfc_data);

    boolean get_cmn_properties(rfc_cmn_properties_type **ptr);

    // Destructor
    virtual ~rfc_efs_card_cmn_data();

  protected:
    rfc_efs_card_cmn_data(rf_hw_type rf_hw);

    virtual boolean construct_file_name(rfm_device_enum_type path, rfm_mode_enum_type mode, 
                                            rfc_rxtx_enum_type rx_tx, int band, rfc_data_enum_type rfc_data_type);
    virtual boolean construct_file_name(rfm_mode_enum_type mode, rfc_rxtx_enum_type rx_tx, 
                                        rfc_data_enum_type rfc_data_type);
    virtual boolean construct_file_name(rfm_mode_enum_type mode, rfc_data_enum_type rfc_data_type);
  

    char m_rfc_file_name[RFC_EFS_MAX_STRING_SIZE + 1];

private:

  void common_data_load();

  int32 read(char *file_name, uint8 **rfc_data);
};

#endif   /*  __cplusplus  */



#endif


