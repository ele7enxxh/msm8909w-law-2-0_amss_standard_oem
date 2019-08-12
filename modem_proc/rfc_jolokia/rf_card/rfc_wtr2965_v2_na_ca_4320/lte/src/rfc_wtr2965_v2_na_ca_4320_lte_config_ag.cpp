
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfc_jolokia/rf_card/rfc_wtr2965_v2_na_ca_4320/lte/src/rfc_wtr2965_v2_na_ca_4320_lte_config_ag.cpp#1 $ 


=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfc_wtr2965_v2_na_ca_4320_lte_config_ag.h" 
#include "rfc_wtr2965_v2_na_ca_4320_cmn_ag.h" 
#include "rfc_common.h" 
#include "rfcom.h" 



extern "C" 
{
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_init_lte_rx_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_init_lte_rx_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_init_lte_tx_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_init_lte_tx_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b1_alt1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b1_alt1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b1_alt1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b1_alt1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b2_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b2_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b2_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b2_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b2_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b2_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b2_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b2_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b2_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b2_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b2_alt1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b2_alt1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b2_alt1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b2_alt1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b2_alt2_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b2_alt2_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b2_alt2_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b2_alt2_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b4_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b4_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b4_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b4_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b4_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b4_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b4_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b4_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b4_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b4_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b4_alt1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b4_alt1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b4_alt1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b4_alt1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b4_alt2_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b4_alt2_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b4_alt2_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b4_alt2_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b5_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b5_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b5_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b5_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b5_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b5_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b5_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b5_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b5_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b5_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b7_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b7_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b7_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b7_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b7_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b7_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b7_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b7_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b7_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b7_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b7_alt1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b7_alt1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b7_alt1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b7_alt1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b8_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b8_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b8_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b8_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b8_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b8_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b12_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b12_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b12_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b12_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b12_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b12_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b12_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b12_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b12_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b12_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b13_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b13_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b13_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b13_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b13_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b13_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b13_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b13_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b13_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b13_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b17_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b17_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b17_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b17_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b17_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b17_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b17_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b17_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b17_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b17_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b25_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b25_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b25_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b25_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b25_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b25_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b25_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b25_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b25_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b25_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b25_alt1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b25_alt1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b25_alt1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b25_alt1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b25_alt2_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b25_alt2_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b25_alt2_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b25_alt2_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b26_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b26_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b26_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b26_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b26_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b26_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b26_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b26_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b26_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b26_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b29_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b29_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b29_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b29_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b29_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b29_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b29_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b29_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b30_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b30_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b30_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b30_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b30_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b30_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b30_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b30_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b30_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b30_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b30_alt1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b30_alt1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b30_alt1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b30_alt1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b41_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b41_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b41_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b41_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b41_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b41_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_alt1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_alt1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_alt1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_alt1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b41_b_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b41_b_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b41_b_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b41_b_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b41_b_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b41_b_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_b_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_b_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_b_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_b_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_b_alt1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_b_alt1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_b_alt1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_b_alt1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b41_c_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b41_c_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b41_c_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b41_c_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b41_c_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b41_c_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_c_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_c_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_c_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_c_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_c_alt1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_c_alt1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_c_alt1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_c_alt1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal0_fbrx_path_internal_adc_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal0_fbrx_path_internal_adc_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal0_fbrx_path_internal_adc_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal0_fbrx_path_internal_adc_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal0_fbrx_path_drx_iq_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal0_fbrx_path_drx_iq_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal0_fbrx_path_drx_iq_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal0_fbrx_path_drx_iq_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal1_fbrx_path_internal_adc_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal1_fbrx_path_internal_adc_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal1_fbrx_path_internal_adc_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal1_fbrx_path_internal_adc_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal1_fbrx_path_drx_iq_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal1_fbrx_path_drx_iq_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal1_fbrx_path_drx_iq_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal1_fbrx_path_drx_iq_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal2_fbrx_path_internal_adc_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal2_fbrx_path_internal_adc_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal2_fbrx_path_internal_adc_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal2_fbrx_path_internal_adc_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal2_fbrx_path_drx_iq_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal2_fbrx_path_drx_iq_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal2_fbrx_path_drx_iq_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal2_fbrx_path_drx_iq_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_lte_en_ant_swap0_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_lte_en_ant_swap0_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_lte_dis_ant_swap0_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_lte_dis_ant_swap0_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_lte_en_ant_swap1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_lte_en_ant_swap1_sig_cfg;
  extern rfc_device_info_type rf_card_wtr2965_v2_na_ca_4320_lte_dis_ant_swap1_device_info;
  extern rfc_sig_info_type rf_card_wtr2965_v2_na_ca_4320_lte_dis_ant_swap1_sig_cfg;

  extern rfc_timing_info_type rfc_msm_timing_info[RFC_TIMING_PARAMS_NUM];
} /* extern "C" */


rfc_lte_data * rfc_wtr2965_v2_na_ca_4320_lte_ag::get_instance()
{
  if (rfc_lte_data_ptr == NULL)
  {
    rfc_lte_data_ptr = (rfc_lte_data *)new rfc_wtr2965_v2_na_ca_4320_lte_ag();
  }
  return( (rfc_lte_data *)rfc_lte_data_ptr);
}

//constructor
rfc_wtr2965_v2_na_ca_4320_lte_ag::rfc_wtr2965_v2_na_ca_4320_lte_ag()
  :rfc_lte_data()
{
}

boolean rfc_wtr2965_v2_na_ca_4320_lte_ag::sig_cfg_data_get( rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr )
{

  boolean ret_val = FALSE;

  if (NULL==ptr)
  {
    return FALSE;
  }

  if (NULL==cfg)
  {
    *ptr = NULL;
    return FALSE;
  }

  *ptr = NULL;

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->req == RFC_REQ_INIT ) )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_init_lte_rx_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->req == RFC_REQ_INIT ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_init_lte_tx_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b1_alt1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b1_alt1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b2_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b2_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b2_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b2_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b2_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b2_alt1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b2_alt1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 2 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b2_alt2_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 2 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b2_alt2_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b4_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b4_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b4_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b4_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b4_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b4_alt1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b4_alt1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 2 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b4_alt2_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 2 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b4_alt2_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b5_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b5_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b5_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b5_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b5_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b7_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b7_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b7_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b7_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b7_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b7_alt1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b7_alt1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B8 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b8_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B8 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b8_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B8 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b8_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b12_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b12_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b12_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b12_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b12_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b13_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b13_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b13_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b13_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b13_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B17 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b17_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B17 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b17_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B17 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b17_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B17 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b17_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B17 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b17_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b25_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b25_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b25_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b25_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b25_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b25_alt1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b25_alt1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 2 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b25_alt2_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 2 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b25_alt2_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b26_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b26_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b26_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b26_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b26_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B29 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b29_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B29 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b29_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B29 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b29_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B29 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b29_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b30_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b30_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b30_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b30_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b30_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b30_alt1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b30_alt1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b41_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b41_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b41_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_alt1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_alt1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b41_b_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b41_b_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b41_b_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_b_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_b_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_b_alt1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_b_alt1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b41_c_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b41_c_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b41_c_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_c_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_c_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_c_alt1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_c_alt1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B5 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B8 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B12 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B13 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B17 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B26 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B29 )) &&  ( cfg->req == RFC_REQ_ENABLE_FBRX ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal0_fbrx_path_internal_adc_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B5 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B8 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B12 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B13 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B17 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B26 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B29 )) &&  ( cfg->req == RFC_REQ_DISABLE_FBRX ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal0_fbrx_path_internal_adc_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B5 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B8 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B12 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B13 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B17 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B26 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B29 )) &&  ( cfg->req == RFC_REQ_ENABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal0_fbrx_path_drx_iq_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B5 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B8 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B12 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B13 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B17 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B26 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B29 )) &&  ( cfg->req == RFC_REQ_DISABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal0_fbrx_path_drx_iq_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B1 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B2 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B4 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B25 )) &&  ( cfg->req == RFC_REQ_ENABLE_FBRX ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal1_fbrx_path_internal_adc_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B1 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B2 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B4 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B25 )) &&  ( cfg->req == RFC_REQ_DISABLE_FBRX ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal1_fbrx_path_internal_adc_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B1 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B2 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B4 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B25 )) &&  ( cfg->req == RFC_REQ_ENABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal1_fbrx_path_drx_iq_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B1 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B2 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B4 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B25 )) &&  ( cfg->req == RFC_REQ_DISABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal1_fbrx_path_drx_iq_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B7 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B30 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_B )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_C )) &&  ( cfg->req == RFC_REQ_ENABLE_FBRX ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal2_fbrx_path_internal_adc_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B7 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B30 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_B )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_C )) &&  ( cfg->req == RFC_REQ_DISABLE_FBRX ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal2_fbrx_path_internal_adc_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B7 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B30 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_B )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_C )) &&  ( cfg->req == RFC_REQ_ENABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal2_fbrx_path_drx_iq_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B7 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B30 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_B )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_C )) &&  ( cfg->req == RFC_REQ_DISABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal2_fbrx_path_drx_iq_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B1 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B2 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B4 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B5 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B8 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B12 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B13 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B17 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B25 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B26 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B29 )) && ( cfg->req == RFC_REQ_ENABLE_ANT_SWAP_1 ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_en_ant_swap0_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B1 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B2 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B4 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B5 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B8 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B12 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B13 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B17 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B25 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B26 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B29 )) && ( cfg->req == RFC_REQ_DISABLE_ANT_SWAP_1 ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_dis_ant_swap0_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B7 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B30 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_B )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_C )) && ( cfg->req == RFC_REQ_ENABLE_ANT_SWAP_1 ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_en_ant_swap1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B7 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B30 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_B )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_C )) && ( cfg->req == RFC_REQ_DISABLE_ANT_SWAP_1 ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_dis_ant_swap1_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }


  

  return ret_val;
}

boolean rfc_wtr2965_v2_na_ca_4320_lte_ag::devices_cfg_data_get( rfc_cfg_params_type *cfg, rfc_device_info_type **ptr )
{

  boolean ret_val = FALSE;

  if (NULL==ptr)
  {
    return FALSE;
  }

  if (NULL==cfg)
  {
    *ptr = NULL;
    return FALSE;
  }

  *ptr = NULL;

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->req == RFC_REQ_INIT ) )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_init_lte_rx_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->req == RFC_REQ_INIT ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_init_lte_tx_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b1_alt1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b1_alt1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b2_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b2_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b2_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b2_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b2_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b2_alt1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b2_alt1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 2 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b2_alt2_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 2 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b2_alt2_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b4_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b4_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b4_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b4_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b4_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b4_alt1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b4_alt1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 2 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b4_alt2_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 2 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b4_alt2_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b5_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b5_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b5_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b5_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b5_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b7_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b7_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b7_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b7_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b7_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b7_alt1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b7_alt1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B8 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b8_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B8 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b8_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B8 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b8_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b12_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b12_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b12_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b12_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b12_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b13_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b13_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b13_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b13_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b13_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B17 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b17_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B17 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b17_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B17 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b17_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B17 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b17_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B17 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b17_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b25_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b25_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b25_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b25_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b25_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b25_alt1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b25_alt1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 2 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b25_alt2_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 2 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b25_alt2_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b26_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b26_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b26_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b26_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b26_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B29 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b29_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B29 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b29_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B29 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b29_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B29 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b29_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b30_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b30_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b30_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b30_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b30_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b30_alt1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b30_alt1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b41_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b41_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b41_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_alt1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_alt1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b41_b_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b41_b_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b41_b_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_b_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_b_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_b_alt1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_b_alt1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b41_c_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b41_c_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b41_c_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_c_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_c_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_c_alt1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_c_alt1_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B5 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B8 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B12 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B13 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B17 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B26 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B29 )) &&  ( cfg->req == RFC_REQ_ENABLE_FBRX ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal0_fbrx_path_internal_adc_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B5 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B8 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B12 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B13 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B17 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B26 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B29 )) &&  ( cfg->req == RFC_REQ_DISABLE_FBRX ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal0_fbrx_path_internal_adc_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B5 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B8 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B12 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B13 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B17 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B26 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B29 )) &&  ( cfg->req == RFC_REQ_ENABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal0_fbrx_path_drx_iq_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B5 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B8 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B12 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B13 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B17 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B26 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B29 )) &&  ( cfg->req == RFC_REQ_DISABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal0_fbrx_path_drx_iq_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B1 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B2 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B4 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B25 )) &&  ( cfg->req == RFC_REQ_ENABLE_FBRX ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal1_fbrx_path_internal_adc_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B1 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B2 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B4 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B25 )) &&  ( cfg->req == RFC_REQ_DISABLE_FBRX ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal1_fbrx_path_internal_adc_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B1 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B2 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B4 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B25 )) &&  ( cfg->req == RFC_REQ_ENABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal1_fbrx_path_drx_iq_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B1 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B2 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B4 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B25 )) &&  ( cfg->req == RFC_REQ_DISABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal1_fbrx_path_drx_iq_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B7 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B30 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_B )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_C )) &&  ( cfg->req == RFC_REQ_ENABLE_FBRX ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal2_fbrx_path_internal_adc_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B7 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B30 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_B )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_C )) &&  ( cfg->req == RFC_REQ_DISABLE_FBRX ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal2_fbrx_path_internal_adc_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B7 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B30 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_B )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_C )) &&  ( cfg->req == RFC_REQ_ENABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_en_et_cal2_fbrx_path_drx_iq_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B7 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B30 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_B )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_C )) &&  ( cfg->req == RFC_REQ_DISABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_dis_et_cal2_fbrx_path_drx_iq_device_info);  ret_val = TRUE; }

  if ( ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B1 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B2 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B4 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B5 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B8 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B12 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B13 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B17 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B25 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B26 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B29 )) && ( cfg->req == RFC_REQ_ENABLE_ANT_SWAP_1 ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_en_ant_swap0_device_info);  ret_val = TRUE; }

  if ( ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B1 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B2 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B4 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B5 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B8 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B12 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B13 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B17 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B25 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B26 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B29 )) && ( cfg->req == RFC_REQ_DISABLE_ANT_SWAP_1 ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_dis_ant_swap0_device_info);  ret_val = TRUE; }

  if ( ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B7 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B30 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_B )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_C )) && ( cfg->req == RFC_REQ_ENABLE_ANT_SWAP_1 ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_en_ant_swap1_device_info);  ret_val = TRUE; }

  if ( ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B7 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B30 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_B )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_C )) && ( cfg->req == RFC_REQ_DISABLE_ANT_SWAP_1 ) && !ret_val )
  { *ptr = &(rf_card_wtr2965_v2_na_ca_4320_lte_dis_ant_swap1_device_info);  ret_val = TRUE; }





  return ret_val;
}

boolean rfc_wtr2965_v2_na_ca_4320_lte_ag::timing_cfg_data_get( rfc_cfg_params_type *cfg, rfc_timing_info_type **ptr )
{

  boolean ret_val = FALSE;

  if (NULL==ptr)
  {
    return FALSE;
  }

  if (NULL==cfg)
  {
    *ptr = NULL;
    return FALSE;
  }

  *ptr = NULL;

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->req == RFC_REQ_INIT ) )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->req == RFC_REQ_INIT ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B1 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 2 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 2 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B2 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 2 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 2 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B4 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B5 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B7 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B8 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B8 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B8 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B12 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B13 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B17 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B17 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B17 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B17 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B17 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 2 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 2 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B25 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B26 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B29 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B29 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B29 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B29 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B30 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_B ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 0 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_2 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_3 ) && ( cfg->alternate_path == 1 ) && ( cfg->band == (int)RFCOM_BAND_LTE_B41_C ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B5 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B8 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B12 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B13 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B17 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B26 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B29 )) && ( cfg->req == RFC_REQ_ENABLE_FBRX ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B5 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B8 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B12 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B13 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B17 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B26 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B29 )) && ( cfg->req == RFC_REQ_DISABLE_FBRX ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B5 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B8 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B12 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B13 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B17 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B26 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B29 )) && ( cfg->req == RFC_REQ_ENABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B5 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B8 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B12 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B13 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B17 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B26 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B29 )) && ( cfg->req == RFC_REQ_DISABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B1 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B2 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B4 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B25 )) && ( cfg->req == RFC_REQ_ENABLE_FBRX ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B1 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B2 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B4 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B25 )) && ( cfg->req == RFC_REQ_DISABLE_FBRX ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B1 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B2 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B4 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B25 )) && ( cfg->req == RFC_REQ_ENABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B1 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B2 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B4 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B25 )) && ( cfg->req == RFC_REQ_DISABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B7 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B30 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_B )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_C )) && ( cfg->req == RFC_REQ_ENABLE_FBRX ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B7 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B30 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_B )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_C )) && ( cfg->req == RFC_REQ_DISABLE_FBRX ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B7 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B30 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_B )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_C )) && ( cfg->req == RFC_REQ_ENABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B7 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B30 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_B )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_C )) && ( cfg->req == RFC_REQ_DISABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B1 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B2 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B4 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B5 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B8 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B12 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B13 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B17 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B25 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B26 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B29 )) && ( cfg->req == RFC_REQ_ENABLE_ANT_SWAP_1 ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B1 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B2 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B4 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B5 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B8 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B12 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B13 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B17 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B25 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B26 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B29 )) && ( cfg->req == RFC_REQ_DISABLE_ANT_SWAP_1 ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B7 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B30 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_B )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_C )) && ( cfg->req == RFC_REQ_ENABLE_ANT_SWAP_1 ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }

  if ( ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_LTE_B7 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B30 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41 )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_B )||  ( cfg->band == (int)RFCOM_BAND_LTE_B41_C )) && ( cfg->req == RFC_REQ_DISABLE_ANT_SWAP_1 ) && !ret_val )
  { *ptr = &(rfc_msm_timing_info[0]);  ret_val = TRUE; }


  return ret_val;
}

boolean rfc_wtr2965_v2_na_ca_4320_lte_ag::ca_scell_log_path_swap_data_get( rfc_band_pair_list_type **ptr )
{
  if (NULL==ptr)
  {
    return FALSE;
  }

  *ptr = NULL;

  return TRUE;
}

extern "C" 
{
  extern rfc_band_split_info_type rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b41_split_cfg;
  extern rfc_band_split_info_type rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b41_split_cfg;
  extern rfc_band_split_info_type rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_split_cfg;
  extern rfc_band_split_info_type rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_split_cfg;
  extern rfc_band_split_info_type rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b41_split_cfg;
}  /* extern "C" */

boolean rfc_wtr2965_v2_na_ca_4320_lte_ag::band_split_cfg_data_get( rfc_cfg_params_type *cfg, rfc_band_split_info_type **ptr )
{
  boolean ret_val = FALSE;

  if (NULL==ptr)
  {
    return FALSE;
  }

  if (NULL==cfg)
  {
    *ptr = NULL;
    return FALSE;
  }

  *ptr = NULL;

  if ( (cfg->rx_tx == RFC_CONFIG_RX) && (cfg->logical_device == RFM_DEVICE_0) && (cfg->band == (int)RFCOM_BAND_LTE_B41) )
  { *ptr = &rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b41_split_cfg; ret_val = TRUE; }

  else if ( (cfg->rx_tx == RFC_CONFIG_RX) && (cfg->logical_device == RFM_DEVICE_0) && (cfg->band == (int)RFCOM_BAND_LTE_B41_B) )
  { *ptr = &rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b41_split_cfg; ret_val = TRUE; }

  else if ( (cfg->rx_tx == RFC_CONFIG_RX) && (cfg->logical_device == RFM_DEVICE_0) && (cfg->band == (int)RFCOM_BAND_LTE_B41_C) )
  { *ptr = &rf_card_wtr2965_v2_na_ca_4320_rx0_lte_b41_split_cfg; ret_val = TRUE; }

  else if ( (cfg->rx_tx == RFC_CONFIG_TX) && (cfg->logical_device == RFM_DEVICE_0) && (cfg->band == (int)RFCOM_BAND_LTE_B41) )
  { *ptr = &rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b41_split_cfg; ret_val = TRUE; }

  else if ( (cfg->rx_tx == RFC_CONFIG_TX) && (cfg->logical_device == RFM_DEVICE_0) && (cfg->band == (int)RFCOM_BAND_LTE_B41_B) )
  { *ptr = &rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b41_split_cfg; ret_val = TRUE; }

  else if ( (cfg->rx_tx == RFC_CONFIG_TX) && (cfg->logical_device == RFM_DEVICE_0) && (cfg->band == (int)RFCOM_BAND_LTE_B41_C) )
  { *ptr = &rf_card_wtr2965_v2_na_ca_4320_tx0_lte_b41_split_cfg; ret_val = TRUE; }

  else if ( (cfg->rx_tx == RFC_CONFIG_RX) && (cfg->logical_device == RFM_DEVICE_1) && (cfg->band == (int)RFCOM_BAND_LTE_B41) )
  { *ptr = &rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b41_split_cfg; ret_val = TRUE; }

  else if ( (cfg->rx_tx == RFC_CONFIG_RX) && (cfg->logical_device == RFM_DEVICE_1) && (cfg->band == (int)RFCOM_BAND_LTE_B41_B) )
  { *ptr = &rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b41_split_cfg; ret_val = TRUE; }

  else if ( (cfg->rx_tx == RFC_CONFIG_RX) && (cfg->logical_device == RFM_DEVICE_1) && (cfg->band == (int)RFCOM_BAND_LTE_B41_C) )
  { *ptr = &rf_card_wtr2965_v2_na_ca_4320_rx1_lte_b41_split_cfg; ret_val = TRUE; }

  else if ( (cfg->rx_tx == RFC_CONFIG_RX) && (cfg->logical_device == RFM_DEVICE_2) && (cfg->band == (int)RFCOM_BAND_LTE_B41) )
  { *ptr = &rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_split_cfg; ret_val = TRUE; }

  else if ( (cfg->rx_tx == RFC_CONFIG_RX) && (cfg->logical_device == RFM_DEVICE_2) && (cfg->band == (int)RFCOM_BAND_LTE_B41_B) )
  { *ptr = &rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_split_cfg; ret_val = TRUE; }

  else if ( (cfg->rx_tx == RFC_CONFIG_RX) && (cfg->logical_device == RFM_DEVICE_2) && (cfg->band == (int)RFCOM_BAND_LTE_B41_C) )
  { *ptr = &rf_card_wtr2965_v2_na_ca_4320_rx2_lte_b41_split_cfg; ret_val = TRUE; }

  else if ( (cfg->rx_tx == RFC_CONFIG_RX) && (cfg->logical_device == RFM_DEVICE_3) && (cfg->band == (int)RFCOM_BAND_LTE_B41) )
  { *ptr = &rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_split_cfg; ret_val = TRUE; }

  else if ( (cfg->rx_tx == RFC_CONFIG_RX) && (cfg->logical_device == RFM_DEVICE_3) && (cfg->band == (int)RFCOM_BAND_LTE_B41_B) )
  { *ptr = &rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_split_cfg; ret_val = TRUE; }

  else if ( (cfg->rx_tx == RFC_CONFIG_RX) && (cfg->logical_device == RFM_DEVICE_3) && (cfg->band == (int)RFCOM_BAND_LTE_B41_C) )
  { *ptr = &rf_card_wtr2965_v2_na_ca_4320_rx3_lte_b41_split_cfg; ret_val = TRUE; }

  return ret_val;
}

