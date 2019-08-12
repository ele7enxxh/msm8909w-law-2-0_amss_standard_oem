
#ifndef RFC_WTR2965_V2_NA_CA_4320_CMN_AG
#define RFC_WTR2965_V2_NA_CA_4320_CMN_AG

#ifdef __cplusplus
extern "C" {
#endif

/*
WARNING: This file is auto-generated.

Generated using: xmlautogen.exe
Generated from: V5.9.321 of RFC_HWSWCD.xlsm
*/

/*=============================================================================

          R F C     A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

Copyright (c) 2016 Qualcomm Technologies Incorporated.  All Rights Reserved.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfc_jolokia/rf_card/rfc_wtr2965_v2_na_ca_4320/common/inc/rfc_wtr2965_v2_na_ca_4320_cmn_ag.h#1 $ 


=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfc_common.h"



typedef enum
{
  RFC_WTR2965_V2_NA_CA_4320_TIMING_PA_CTL,
  RFC_WTR2965_V2_NA_CA_4320_TIMING_PA_RANGE,
  RFC_WTR2965_V2_NA_CA_4320_TIMING_ASM_CTL,
  RFC_WTR2965_V2_NA_CA_4320_TIMING_TUNER_CTL,
  RFC_WTR2965_V2_NA_CA_4320_TIMING_PAPM_CTL,
  RFC_WTR2965_V2_NA_CA_4320_TIMING_TX_TX_RF_ON0,
  RFC_WTR2965_V2_NA_CA_4320_TIMING_TX_RX_RF_ON0,
  RFC_WTR2965_V2_NA_CA_4320_TIMING_ASM_TRIGGER,
  RFC_WTR2965_V2_NA_CA_4320_TIMING_PAPM_TX_TX_TRIGGER,
  RFC_WTR2965_V2_NA_CA_4320_TIMING_PAPM_OFF_TX_RX_TX_TRIGGER,
  RFC_WTR2965_V2_NA_CA_4320_TIMING_PA_TRIGGER,
  RFC_WTR2965_V2_NA_CA_4320_TIMING_PAPM_OFF_TX_RX_TX_CTL,
  RFC_WTR2965_V2_NA_CA_4320_TIMING_PAPM_TX_TX_CTL,
  RFC_WTR2965_V2_NA_CA_4320_TIMING_PAPM_MULTISLOT_CTL,
  RFC_WTR2965_V2_NA_CA_4320_RF_PATH_SEL_01,
  RFC_WTR2965_V2_NA_CA_4320_RF_PATH_SEL_09,
  RFC_WTR2965_V2_NA_CA_4320_RF_PATH_SEL_02,
  RFC_WTR2965_V2_NA_CA_4320_RF_PATH_SEL_11,
  RFC_WTR2965_V2_NA_CA_4320_RF_PATH_SEL_04,
  RFC_WTR2965_V2_NA_CA_4320_RF_PATH_SEL_17,
  RFC_WTR2965_V2_NA_CA_4320_RF_PATH_SEL_15,
  RFC_WTR2965_V2_NA_CA_4320_RF_PATH_SEL_05,
  RFC_WTR2965_V2_NA_CA_4320_RF_PATH_SEL_20,
  RFC_WTR2965_V2_NA_CA_4320_RF_PATH_SEL_06,
  RFC_WTR2965_V2_NA_CA_4320_RF_PATH_SEL_14,
  RFC_WTR2965_V2_NA_CA_4320_RF_PATH_SEL_07,
  RFC_WTR2965_V2_NA_CA_4320_RF_PATH_SEL_16,
  RFC_WTR2965_V2_NA_CA_4320_RF_PATH_SEL_08,
  RFC_WTR2965_V2_NA_CA_4320_GPDATA0_0,
  RFC_WTR2965_V2_NA_CA_4320_RFFE1_CLK,
  RFC_WTR2965_V2_NA_CA_4320_RFFE1_DATA,
  RFC_WTR2965_V2_NA_CA_4320_RFFE2_CLK,
  RFC_WTR2965_V2_NA_CA_4320_RFFE2_DATA,
  RFC_WTR2965_V2_NA_CA_4320_RFFE4_CLK,
  RFC_WTR2965_V2_NA_CA_4320_RFFE4_DATA,
  RFC_WTR2965_V2_NA_CA_4320_RFFE5_CLK,
  RFC_WTR2965_V2_NA_CA_4320_RFFE5_DATA,
  RFC_WTR2965_V2_NA_CA_4320_INTERNAL_GNSS_BLANK,
  RFC_WTR2965_V2_NA_CA_4320_INTERNAL_GNSS_BLANK_CONCURRENCY,
  RFC_WTR2965_V2_NA_CA_4320_TX_GTR_TH,
  RFC_WTR2965_V2_NA_CA_4320_PA_IND,
  RFC_WTR2965_V2_NA_CA_4320_SIG_NUM,
  RFC_WTR2965_V2_NA_CA_4320_SIG_INVALID,
}wtr2965_v2_na_ca_4320_sig_type;



#ifdef __cplusplus

#include "rfc_common_data.h"

class rfc_wtr2965_v2_na_ca_4320_cmn_ag:public rfc_common_data
{
  public:
    uint32 sig_info_table_get(rfc_signal_info_type **rfc_info_table);
    rfc_phy_device_info_type* get_phy_device_cfg( void );
    rfc_logical_device_info_type* get_logical_device_cfg( void );
    boolean get_logical_path_config(rfm_devices_configuration_type* dev_cfg);
    
    rfc_alt_path_sel_type* get_alt_path_selection_tbl (uint32*tbl_size);
    boolean get_cmn_properties(rfc_cmn_properties_type **ptr);
    static rfc_common_data * get_instance(rf_hw_type rf_hw);

  protected:
    rfc_wtr2965_v2_na_ca_4320_cmn_ag(rf_hw_type rf_hw);
};

#endif   /*  __cplusplus  */


#ifdef __cplusplus
}
#endif



#endif


