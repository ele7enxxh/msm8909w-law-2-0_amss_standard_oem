#ifndef RFC_CONFIGURABLE_TEST_CARD_CMN_DATA
#define RFC_CONFIGURABLE_TEST_CARD_CMN_DATA

/*!
  @file
  rfc_configurable_test_card_cmn_data.h

  @brief
  This file contains the class definition for the rfc_configurable_test_card_cmn_data, which implements the
  functionality to get/set the RFC related data.

*/

/*===========================================================================

Copyright (c) 2011-12 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/rf_card/rfc_configurable_test_card/common/inc/rfc_configurable_test_card_cmn_data.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------   
08/11/14   vrb     Updated to use physical device structure definitions
02/09/13   sr      Added Destructor.
11/08/12   sr      initial version
============================================================================*/
#include "comdef.h"

#include "rfc_common.h" 

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
  RFC_CONFIGURABLE_TEST_CARD_TIMING_PA_CTL,
  RFC_CONFIGURABLE_TEST_CARD_TIMING_PA_RANGE,
  RFC_CONFIGURABLE_TEST_CARD_TIMING_ASM_CTL,
  RFC_CONFIGURABLE_TEST_CARD_PA_ON,
  RFC_CONFIGURABLE_TEST_CARD_PA_R0,
  RFC_CONFIGURABLE_TEST_CARD_PA_R1,
  RFC_CONFIGURABLE_TEST_CARD_RX_ON0,
  RFC_CONFIGURABLE_TEST_CARD_RF_ON0,
  RFC_CONFIGURABLE_TEST_CARD_RX_ON1,
  RFC_CONFIGURABLE_TEST_CARD_RF_ON1,
  RFC_CONFIGURABLE_TEST_CARD_RF_PATH_SEL_00,
  RFC_CONFIGURABLE_TEST_CARD_RF_PATH_SEL_01,
  RFC_CONFIGURABLE_TEST_CARD_RF_PATH_SEL_02,
  RFC_CONFIGURABLE_TEST_CARD_RF_PATH_SEL_03,
  RFC_CONFIGURABLE_TEST_CARD_RF_PATH_SEL_04,
  RFC_CONFIGURABLE_TEST_CARD_RF_PATH_SEL_05,
  RFC_CONFIGURABLE_TEST_CARD_RF_PATH_SEL_06,
  RFC_CONFIGURABLE_TEST_CARD_SSBI_SPARE_0,
  RFC_CONFIGURABLE_TEST_CARD_GPDATA0_0,
  RFC_CONFIGURABLE_TEST_CARD_GPDATA0_1,
  RFC_CONFIGURABLE_TEST_CARD_RFFE1_CLK,
  RFC_CONFIGURABLE_TEST_CARD_RFFE1_DATA,
  RFC_CONFIGURABLE_TEST_CARD_RFFE2_CLK,
  RFC_CONFIGURABLE_TEST_CARD_RFFE2_DATA,
  RFC_CONFIGURABLE_TEST_CARD_EXT_GPS_LNA_EN,
  RFC_CONFIGURABLE_TEST_CARD_SIG_NUM,
  RFC_CONFIGURABLE_TEST_CARD_SIG_INVALID,
} rfc_CONFIGURABLE_TEST_data_sig_type;


#ifdef __cplusplus

#include "rfc_common_data.h"

class rfc_configurable_test_card_cmn_data:public rfc_common_data
{
  public:
    static rfc_common_data * get_instance(rf_hw_type rf_hw);

    uint32 sig_info_table_get(rfc_signal_info_type **rfc_info_table);
    rfc_phy_device_info_type* get_phy_device_cfg( void );
    rfc_logical_device_info_type* get_logical_device_cfg( void );
    boolean get_logical_path_config(rfm_devices_configuration_type* dev_cfg);

    uint32 sig_info_table_set(rfc_signal_info_type *rfc_info_table);
    boolean phy_device_cfg_set( rfc_phy_device_info_type *device_config );
    boolean log_device_cfg_set( rfc_logical_device_info_type *device_config );
    boolean logical_path_config_set(rfm_devices_configuration_type *log_path_cfg);

    // Destructor
    virtual ~rfc_configurable_test_card_cmn_data();

  protected:
    rfc_configurable_test_card_cmn_data(rf_hw_type rf_hw);
};

#endif   /*  __cplusplus  */


#ifdef __cplusplus
}
#endif



#endif


