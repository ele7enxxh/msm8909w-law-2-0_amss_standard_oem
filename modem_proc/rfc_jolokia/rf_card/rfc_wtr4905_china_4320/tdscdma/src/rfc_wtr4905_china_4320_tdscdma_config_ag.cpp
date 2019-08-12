
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfc_jolokia/rf_card/rfc_wtr4905_china_4320/tdscdma/src/rfc_wtr4905_china_4320_tdscdma_config_ag.cpp#1 $ 


=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfc_wtr4905_china_4320_tdscdma_config_ag.h" 
#include "rfc_wtr4905_china_4320_cmn_ag.h" 
#include "rfc_common.h" 
#include "rfcom.h" 



extern "C" 
{
  extern rfc_device_info_type rf_card_wtr4905_china_4320_init_tdscdma_rx_device_info;
  extern rfc_sig_info_type rf_card_wtr4905_china_4320_init_tdscdma_rx_sig_cfg;
  extern rfc_device_info_type rf_card_wtr4905_china_4320_init_tdscdma_tx_device_info;
  extern rfc_sig_info_type rf_card_wtr4905_china_4320_init_tdscdma_tx_sig_cfg;
  extern rfc_device_info_type rf_card_wtr4905_china_4320_rx0_tdscdma_b34_device_info;
  extern rfc_sig_info_type rf_card_wtr4905_china_4320_rx0_tdscdma_b34_sig_cfg;
  extern rfc_device_info_type rf_card_wtr4905_china_4320_rx1_tdscdma_b34_device_info;
  extern rfc_sig_info_type rf_card_wtr4905_china_4320_rx1_tdscdma_b34_sig_cfg;
  extern rfc_device_info_type rf_card_wtr4905_china_4320_tx0_tdscdma_b34_device_info;
  extern rfc_sig_info_type rf_card_wtr4905_china_4320_tx0_tdscdma_b34_sig_cfg;
  extern rfc_device_info_type rf_card_wtr4905_china_4320_rx0_tdscdma_b39_device_info;
  extern rfc_sig_info_type rf_card_wtr4905_china_4320_rx0_tdscdma_b39_sig_cfg;
  extern rfc_device_info_type rf_card_wtr4905_china_4320_rx1_tdscdma_b39_device_info;
  extern rfc_sig_info_type rf_card_wtr4905_china_4320_rx1_tdscdma_b39_sig_cfg;
  extern rfc_device_info_type rf_card_wtr4905_china_4320_tx0_tdscdma_b39_device_info;
  extern rfc_sig_info_type rf_card_wtr4905_china_4320_tx0_tdscdma_b39_sig_cfg;
  extern rfc_device_info_type rf_card_wtr4905_china_4320_tdscdma_en_et_cal0_fbrx_path_internal_adc_device_info;
  extern rfc_sig_info_type rf_card_wtr4905_china_4320_tdscdma_en_et_cal0_fbrx_path_internal_adc_sig_cfg;
  extern rfc_device_info_type rf_card_wtr4905_china_4320_tdscdma_dis_et_cal0_fbrx_path_internal_adc_device_info;
  extern rfc_sig_info_type rf_card_wtr4905_china_4320_tdscdma_dis_et_cal0_fbrx_path_internal_adc_sig_cfg;
  extern rfc_device_info_type rf_card_wtr4905_china_4320_tdscdma_en_et_cal0_fbrx_path_drx_iq_device_info;
  extern rfc_sig_info_type rf_card_wtr4905_china_4320_tdscdma_en_et_cal0_fbrx_path_drx_iq_sig_cfg;
  extern rfc_device_info_type rf_card_wtr4905_china_4320_tdscdma_dis_et_cal0_fbrx_path_drx_iq_device_info;
  extern rfc_sig_info_type rf_card_wtr4905_china_4320_tdscdma_dis_et_cal0_fbrx_path_drx_iq_sig_cfg;
} /* extern "C" */


rfc_tdscdma_data * rfc_wtr4905_china_4320_tdscdma_ag::get_instance()
{
  if (rfc_tdscdma_data_ptr == NULL)
  {
    rfc_tdscdma_data_ptr = (rfc_tdscdma_data *)new rfc_wtr4905_china_4320_tdscdma_ag();
  }
  return( (rfc_tdscdma_data *)rfc_tdscdma_data_ptr);
}

//constructor
rfc_wtr4905_china_4320_tdscdma_ag::rfc_wtr4905_china_4320_tdscdma_ag()
  :rfc_tdscdma_data()
{
}

boolean rfc_wtr4905_china_4320_tdscdma_ag::sig_cfg_data_get( rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr )
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
  { *ptr = &(rf_card_wtr4905_china_4320_init_tdscdma_rx_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->req == RFC_REQ_INIT ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_init_tdscdma_tx_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B34 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_rx0_tdscdma_b34_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B34 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_rx1_tdscdma_b34_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B34 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_tx0_tdscdma_b34_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B39 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_rx0_tdscdma_b39_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B39 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_rx1_tdscdma_b39_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B39 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_tx0_tdscdma_b39_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B34 )||  ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B39 )) &&  ( cfg->req == RFC_REQ_ENABLE_FBRX ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_tdscdma_en_et_cal0_fbrx_path_internal_adc_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B34 )||  ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B39 )) &&  ( cfg->req == RFC_REQ_DISABLE_FBRX ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_tdscdma_dis_et_cal0_fbrx_path_internal_adc_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B34 )||  ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B39 )) &&  ( cfg->req == RFC_REQ_ENABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_tdscdma_en_et_cal0_fbrx_path_drx_iq_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B34 )||  ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B39 )) &&  ( cfg->req == RFC_REQ_DISABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_tdscdma_dis_et_cal0_fbrx_path_drx_iq_sig_cfg.cfg_sig_list[0]);  ret_val = TRUE; }


  

  return ret_val;
}

boolean rfc_wtr4905_china_4320_tdscdma_ag::devices_cfg_data_get( rfc_cfg_params_type *cfg, rfc_device_info_type **ptr )
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
  { *ptr = &(rf_card_wtr4905_china_4320_init_tdscdma_rx_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->req == RFC_REQ_INIT ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_init_tdscdma_tx_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B34 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_rx0_tdscdma_b34_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B34 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_rx1_tdscdma_b34_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B34 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_tx0_tdscdma_b34_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B39 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_rx0_tdscdma_b39_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_RX ) && ( cfg->logical_device == RFM_DEVICE_1 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B39 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_rx1_tdscdma_b39_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) && ( cfg->logical_device == RFM_DEVICE_0 ) && ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B39 ) && ( cfg->req == RFC_REQ_DEFAULT_GET_DATA ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_tx0_tdscdma_b39_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B34 )||  ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B39 )) &&  ( cfg->req == RFC_REQ_ENABLE_FBRX ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_tdscdma_en_et_cal0_fbrx_path_internal_adc_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B34 )||  ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B39 )) &&  ( cfg->req == RFC_REQ_DISABLE_FBRX ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_tdscdma_dis_et_cal0_fbrx_path_internal_adc_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B34 )||  ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B39 )) &&  ( cfg->req == RFC_REQ_ENABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_tdscdma_en_et_cal0_fbrx_path_drx_iq_device_info);  ret_val = TRUE; }

  if ( ( cfg->rx_tx == RFC_CONFIG_TX ) &&  ( cfg->alternate_path == 0 /*Warning: not specified*/ ) && ( ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B34 )||  ( cfg->band == (int)RFCOM_BAND_TDSCDMA_B39 )) &&  ( cfg->req == RFC_REQ_DISABLE_XPT_CAPTURE ) && !ret_val )
  { *ptr = &(rf_card_wtr4905_china_4320_tdscdma_dis_et_cal0_fbrx_path_drx_iq_device_info);  ret_val = TRUE; }





  return ret_val;
}

boolean rfc_wtr4905_china_4320_tdscdma_ag::band_split_cfg_data_get( rfc_cfg_params_type *cfg, rfc_band_split_info_type **ptr )
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

  return ret_val;
}

