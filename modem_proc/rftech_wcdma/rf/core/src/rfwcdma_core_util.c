/*!
  @file
  rfwcdma_core_util.c

  @brief
  RF WCDMA Core Utility Functions.

  @details

*/

/*===========================================================================

Copyright (c) 2008 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/core/src/rfwcdma_core_util.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/17/14   rmb     Return LNA id = 0 for all IRAT cases.
02/26/14   zhh    Update XPT Delay structure to support DC-UPA
08/04/14   rmb     Add the core conversion APIs from rfwcdma_core to rfwcdma_core_util.
07/17/14   adk     Added function rfwcdma_core_util_get_min_max_tx_freqs()
06/06/14   ac      nbr support
05/01/14   rmb     Add support for new version of AGC log packet 0x4176 v2
                   and remove the old version of AGC logging interface.
04/20/14   bn      WCDMA 8 PA state support and related cleanup
04/14/14   kai     Fix RGI logging
04/11/14   dw      Add Tx on logging from RFLM
04/09/14   dw      Fix TxAGC logging resolution issue
03/25/14   rmb     Add a new get rxagc API during X2W measurements. 
03/11/14   dw      Fix function name of rfwcdma_core_count_carriers
02/21/14   rmb     Add global NV pointers for each of band specific static NVs.
02/14/14   kai     Add Pin/Pout logging
02/04/14   kai     Add common API to read TxAGC status from RFLM current params
01/15/14   rmb     Fix for new Noise floor correction for lower Rx Power.
12/17/13   kr      added car_id support to rfwcdma_mdsp_is_tx_accum_frozen()
12/05/13   bn      Add IQ gain logging support
12/04/13   dw      Consolidate all get RxAGC core functions
11/14/13   dw      Change rfwcdma_mdsp_async_get_data to rfwcdma_mdsp_async_get_rx_data(
10/28/13   dw      Add LNA state logging support
09/10/13   dw      Remove DBDC featurization and code clean up.
08/28/13   vs      Add API to return the Max Carrier ID
08/23/13   vs      Deprecating the ununsed RF API 
07/19/13   ac      DBDC changes due to mdsp
07/11/13   aro     Compiler Warning fix
06/27/13   ac      Unified RF API to support query of RxAGC for SC and DC case
06/25/13   ac      tx freezeaccum api change for both car
04/05/13   kg      Added API to convert rfcom band type to a number
04/04/13   ka      WCDMA state machine updates
03/29/13   kcj     Round instead of ceiling the ENC BTF value from NV
02/05/13   kg      Added API's to return rfcom type band mask and max/min
                   freq based on band
10/18/12   dw      Fix TxAGC logging
09/18/12   ac      rfc to take device as parameter
08/30/11   aka     Add state check around Tx AGC read APIs.
02/10/12   gh      Update min Tx power from NV
12/15/11   dw      Change the AMSS log packet get RxAGC funtion to include noise floor compensation
07/28/11   ckl     Remove rfwcdma_core_util_get_cgagc_settling_time()
05/16/11   ad      Added exportable APIs to access internal MDSP rxagc cmd status 
                   and hdet buffered tx gain value
03/22/11   dw      cleanup common RFC interface access
01/04/11   pl      Fix compile error -- use rfm_path_enum_type
10/11/10   ad      Added preliminary support for WCDMA APT Phase 1  
08/25/10   ad      Fix HDET/TxPDM logging
08/19/10   ka      Set proper log version in idle case
07/19/10   ka      Removed warnings
06/28/10   dw      Remove RF_HAS_RX_DIVERSITY for Chain 1 LNA logging
06/08/10   ka      Klocwork fixes. 
06/01/10   dw      Fixed dB10 unit error of rfwcdma_core_util_get_tx_agc_val
05/27/10   dw      RxAGC logging changes
03/05/10   ka      Remove hardcoding of BTF.
02/18/10   ka      Change dB10 to dBm units since L1 needs it in dBm units       
02/12/10   ka      Remove direct mdsp access from core layer.
02/03/10   kguo    Changed PACKED struct to PACK(struct)
01/18/10   kguo   fixed compiler errors for merged build
01/10/10   kguo   Merged mdm9k dev branch to main\latest
11/23/09   ka      Added support for DC-HSPA agc log packet.
11/05/09   bn      LUT index(PDM) info in WCDMA_AGC log packet
10/23/09   kguo    Merged changes for 9k bringup
09/03/09   ckl     Include seq.h to remove compiler warning of seq_get_cfn .
07/07/09   bn      TxAGC dB10 changes
07/06/09   ckl     Changed return value as dBm for get_rx_agc_val function.
07/05/09   ckl     Changed function name, rfwcdma_core_util_collect_agc_data()
06/29/09   ckl     Changed return value as dB10 unit for Rx AGC read functions.
05/28/09   ckl     Add rfwcdma_core_util_get_rx_raw_agc_val( ) function.
03/18/09   ckl     Fix duplicate enum names between rfwcdma_mc_state_enum_type
                   and rf_state_enum_type
03/17/09   ckl     Delete double included rfumts.h
03/15/09   ckl     Adapted rfcommon_nv.h
03/09/09   ckl     Code cleanup
11/05/08   kc      Added comments in dOxygen format.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfa_variation.h"
#include "rfumts.h"
#include "rfwcdma_core_util.h"
#include "dldemlog.h"
#include "seq.h"
#include "rfwcdma_core_txplim.h"
#include "rfwcdma_mc.h"
#include "rfc_card.h"
#include "rfc_card_wcdma.h"
#include "rfcommon_nv_mm.h"
#include "rfnv_wcdma.h"
#include "rfwcdma_core_txlin.h"
#include "bsp.h"
#include "rfcommon_math.h"
#include "rfcommon_core.h"
#include "rfwcdma_mdsp_async.h"

#ifndef ABS
#define  ABS( x ) ( ((x) < 0) ? -(x) : (x) )
#endif
#define RFAGC_10_BIT   0x1

extern rfwcdma_core_txlin_record_type rfwcdma_core_current_txlin_data;

int16 rfwcdma_core_util_ue_min_tx_power = RF_UE_MIN_TX_POWER_DB10;

/*  Remove the following two unused variables once the references have been removed
from umtsdiag.c */
uint16                       rf_agc_log_cnt;
WCDMA_AGC_LOG_PACKET_type    rf_agc_log_pkt;

/* Noise Floor */
#define HIGH_THRESHOLD_OFFSET 129

#define NF_LOOKUP_MAX 64

// Each table entry is 1/NF_LOOKUP_STEP_SIZE in dB.
#define NF_LOOKUP_STEP_SIZE 4

// This LUT provides a low threshold value for noise floor compensation.
// The index into the table is the difference between
// RF_MIN_RX_POWER_DBM and the noise floor value, multiplied
// by the table step size.
// The table is derived from the following
const float rfagc_low_threshold[NF_LOOKUP_MAX] =
{
3.005302835,
2.887098603,
2.767491565,
2.65146993,
2.53901891,
2.43011794,
2.324740807,
2.222855823,
2.124426028,
2.029409417,
1.937759204,
1.849424101,
1.764348624,
1.682473412,
1.603735559,
1.528068953,
1.455404631,
1.385671122,
1.3187948,
1.254700224,
1.193310481,
1.134547508,
1.078332414,
1.024585778,
0.973227937,
0.92417926,
0.877360394,
0.832692507,
0.790097497,
0.749498191,
0.710818526,
0.673983706,
0.638920341,
0.605556576,
0.573822193,
0.543648702,
0.51496942,
0.487719527,
0.461836113,
0.437258219,
0.413926852,
0.391785001,
0.370777645,
0.350851737,
0.3319562,
0.314041903,
0.297061636,
0.280970077,
0.265723756,
0.251281018,
0.237601977,
0.224648473,
0.212384019,
0.200773759,
0.18978441,
0.179384217,
0.169542893,
0.160231575,
0.151422766,
0.143090287,
0.135209221,
0.127755869,
0.120707692,
0.114043272
};

// This table provides a correction factor in RSSI dBm.
// The index into the table is the difference between the noise
// floor and the measured RSSI, multiplied by the table step size.
const float noise_floor_delta[NF_LOOKUP_MAX] = 
{-26.38284215,
-12.52284341,
-9.635744808,
-7.996835136,
-6.868253244,
-6.01876253,
-5.345365206,
-4.793120335,
-4.329234333,
-3.932555956,
-3.58864459,
-3.287201657,
-3.020624399,
-2.783140534,
-2.570264512,
-2.378441818,
-2.204808305,
-2.047022609,
-1.903146539,
-1.771557845,
-1.650885386,
-1.53996013,
-1.437777556,
-1.343468427,
-1.256275775,
-1.175536595,
-1.100667122,
-1.031150888,
-0.966528953,
-0.906391845,
-0.850372865,
-0.798142493,
-0.749403674,
-0.703887835,
-0.661351487,
-0.621573327,
-0.584351739,
-0.549502642,
-0.516857621,
-0.486262296,
-0.457574906,
-0.430665056,
-0.405412622,
-0.381706779,
-0.359445142,
-0.338533003,
-0.318882649,
-0.300412755,
-0.283047838,
-0.26671777,
-0.251357335,
-0.236905835,
-0.223306727,
-0.210507301,
-0.198458381,
-0.187114061,
-0.176431457,
-0.166370484,
-0.156893654,
-0.147965887,
-0.139554339,
-0.131628248,
-0.124158787,
-0.11711893
};

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns whether a given feature is required by the current RF target.

  @details
  Checks if RF card support DM for hdet, tech dependent LNa switch points.

  @param feature RF specific feature

  @return boolean TRUE if feature is needed or enabled and FALSE otherwise.

  @pre
  rfm_init is called.
*/
boolean rfwcdma_core_util_get_rf_capability( rfcom_rf_cap_type feature )
{
  boolean rfcap = FALSE;

  switch ( feature )
  {
    case RF_CAP_DM_FOR_HDET:
      //rfcap = rfi_rf_needs_data_mover_for_hdet();
      break;

    case RF_CAP_TECH_DEPENDENT_LNA_SWITCH:
      rfcap = TRUE; //rfi_tech_dependent_lna_switch();
      break;

    case RF_CAP_LDO_HAS_EXTRA_DELAY:
    /* This is TRUE only for 3U and RFCMOS targets, which this driver
      no longer supports */
      //rfcap = rf_pm_ldo_on_has_extra_delay;
    break;

    default:
    MSG_ERROR( "RF capability not recognized: %d",
              feature, 0, 0);
  }
  return rfcap;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reads TX AGC from RFLM current params and returns in dBm unit.

  @retval int16 Tx AGC value in units of dBm.

  @pre
  rfm_init is called and mc_state in RXTX.
*/

int16 rfwcdma_core_util_get_rflm_txagc_dBm( void )
{
  return (rfwcdma_core_util_convert_tx_agc_to_dbm_unit(rfwcdma_core_util_get_rflm_txagc_dB10()));
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reads TX AGC from RFLM current params and returns in dB10 unit.

  @retval int16 Tx AGC value in units of dB10.

  @pre
  rfm_init is called and mc_state in RXTX.
*/

int16 rfwcdma_core_util_get_rflm_txagc_dB10( void )
{
  rfwcdma_mdsp_get_data_param_type mdsp_param = {0};
  /* Initialize default TxAGC in dBm10 to min value*/
  int16 txagc_dbm10 = RF_MIN_TX_POWER_DBM * RF_TXAGC_RESOLUTION;

  if(rfwcdma_mc_state.car_path_state[RF_PATH_0][0].rf_state == RFWCDMA_MC_STATE_RXTX)
  {
    mdsp_param.get_data_cmd = RFWCDMA_MDSP_GET_TXAGC_VAL;
    mdsp_param.rflm_handle = (rflm_dm_handle_id_t)rfwcdma_mc_state.txlm_buf_idx;
    if (rfwcdma_mdsp_async_get_tx_data(&mdsp_param))
    {
      /* RFLM retuns in dBm10 units */
      txagc_dbm10 = (int16)(mdsp_param.data.gen_w);
    }
  }
  return (rfwcdma_core_util_convert_dbm10_to_txagc_unit(txagc_dbm10));
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reads RGI from RFLM current params.

  @retval int16 rgi.

  @pre
  rfm_init is called and mc_state in RXTX.
*/
int16 rfwcdma_core_util_get_rflm_rgi( void )
{
  int16 rgi = 0xFF;
  uint8 pa_range = 0xFF;
  uint16 row_idx_smps = 0;
  rfwcdma_mdsp_get_data_param_type mdsp_param = {0};
  if(rfwcdma_mc_state.car_path_state[RF_PATH_0][0].rf_state == RFWCDMA_MC_STATE_RXTX)
  {
    mdsp_param.get_data_cmd = RFWCDMA_MDSP_GET_TXAGC_PARAM;
    mdsp_param.rflm_handle = (rflm_dm_handle_id_t)rfwcdma_mc_state.txlm_buf_idx;
    if (rfwcdma_mdsp_async_get_tx_data(&mdsp_param))
    {
      row_idx_smps = mdsp_param.data.get_txagc_param.row_idx_smps;
      pa_range = (uint8)mdsp_param.data.get_txagc_param.pa_state;
      if ((pa_range >= RFCOM_TXLIN_NUM_LINEARIZERS)||(row_idx_smps >= RFCOM_TXLIN_NUM_SEGMENTS ))
      {
        MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
              "Invalid PA=%d range or RGI SMPS row index = %d", pa_range, row_idx_smps);
        return rgi;
      }
      /*! @todo: add HS linearizer if required */
      rgi = 
        rfwcdma_core_current_txlin_data.current_lin_tbl_comp[pa_range].table[row_idx_smps].rgi;
    }
  }
  return rgi;  
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reads PA range from RFLM current params.

  @retval uint8 pa_range.

  @pre
  rfm_init is called.
*/
uint8 rfwcdma_core_util_get_rflm_pa_range( void )
{
  uint8 pa_range = 0xFF;
  rfwcdma_mdsp_get_data_param_type mdsp_param = {0};
  if(rfwcdma_mc_state.car_path_state[RF_PATH_0][0].rf_state == RFWCDMA_MC_STATE_RXTX)
  {
    mdsp_param.get_data_cmd = RFWCDMA_MDSP_GET_TXAGC_PARAM;
    mdsp_param.rflm_handle = (rflm_dm_handle_id_t)rfwcdma_mc_state.txlm_buf_idx;
    if (rfwcdma_mdsp_async_get_tx_data(&mdsp_param))
    {
      pa_range = (uint8)mdsp_param.data.get_txagc_param.pa_state;
      if(pa_range >= RFCOM_TXLIN_NUM_LINEARIZERS)
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Invalid PA=%d range", pa_range);
        return pa_range;
      }      
    }
  }
  return pa_range;  
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Tx ON status from RFLM
 
  @details
  This function question RF MDSP/RFLM to get Tx ON status. Tx On/OFF status is
  logged as soon as RFLM API for Tx ON/OFF is called by FW which is ~2BPG before
  the actual Tx and PA ON/OFF time on the HW

  @param void

  @retval boolean
  Tx ON status 

*/ 

boolean rfwcdma_core_util_get_rflm_tx_on( void )
{
  boolean tx_on = FALSE;

  rfwcdma_mdsp_get_data_param_type mdsp_param = {0};
  if(rfwcdma_mc_state.car_path_state[RF_PATH_0][0].rf_state == RFWCDMA_MC_STATE_RXTX)
  {
    mdsp_param.get_data_cmd = RFWCDMA_MDSP_GET_TX_ON_STATUS;
    mdsp_param.rflm_handle = (rflm_dm_handle_id_t)rfwcdma_mc_state.txlm_buf_idx;
    if (rfwcdma_mdsp_async_get_tx_data(&mdsp_param))
    {
      tx_on = (boolean)mdsp_param.data.gen_b; 
    }
  }
  return tx_on;  
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reads TxAGC status from RFLM current params.

  @retval boolean True for success.

  @pre
  rfm_init is called and mc_state in RXTX.
*/
boolean rfwcdma_core_util_get_rflm_txagc_logging
(
  rfwcdma_core_util_txagc_logging_type* data
)
{
  boolean ret_val= TRUE;
  rfwcdma_mdsp_get_data_param_type mdsp_param = {0};
  data->rgi = 0xFF;
  data->pa_range = 0xFF;
  data->row_idx = 0;
  data->icq_current = 0xFFFF;
  data->iq_gain = 0xFFFF;
  data->env_scale = 0xFFFF;
  data->xpt_mode = 0xFF;
  data->dpd_index = 0xFF;
  data->xpt_delay = 0xFFFF;
  data->smps_bias = 0xFFFF;
  data->pin_comp = 0xFFFF;
  data->pout_comp = 0xFFFF;

  if(rfwcdma_mc_state.car_path_state[RF_PATH_0][0].rf_state == RFWCDMA_MC_STATE_RXTX)
  {
    mdsp_param.get_data_cmd = RFWCDMA_MDSP_GET_TXAGC_PARAM;
    mdsp_param.rflm_handle = (rflm_dm_handle_id_t)rfwcdma_mc_state.txlm_buf_idx;
    if (rfwcdma_mdsp_async_get_tx_data(&mdsp_param))
    {
      data->row_idx = mdsp_param.data.get_txagc_param.row_idx_const;
      data->row_idx_smps = mdsp_param.data.get_txagc_param.row_idx_smps;
      data->pa_range = (uint8)mdsp_param.data.get_txagc_param.pa_state;
      data->iq_gain = mdsp_param.data.get_txagc_param.iq_gain;
      data->env_scale = mdsp_param.data.get_txagc_param.env_scale;
      data->xpt_mode = mdsp_param.data.get_txagc_param.xpt_mode;
      data->xpt_delay = mdsp_param.data.get_txagc_param.xpt_delay;
  
      if ((data->pa_range >= RFCOM_TXLIN_NUM_LINEARIZERS)||(data->row_idx >= RFCOM_TXLIN_NUM_SEGMENTS ))
      {
        MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "Invalid PA=%d range or RGI index = %d", 
              data->pa_range, data->row_idx);
        return FALSE;
      }
      /*! @todo: add HS linearizer if required */
      data->rgi = 
        rfwcdma_core_current_txlin_data.current_lin_tbl_comp[data->pa_range].table[data->row_idx_smps].rgi;
      data->icq_current = 
        rfwcdma_core_current_txlin_data.current_lin_tbl_comp[data->pa_range].table[data->row_idx_smps].apt_pa_struct.pa_q_current;
      data->smps_bias = 
        rfwcdma_core_current_txlin_data.current_lin_tbl_comp[data->pa_range].table[data->row_idx_smps].apt_pa_struct.pa_bias_voltage;
      data->dpd_index = 
        rfwcdma_core_current_txlin_data.current_lin_tbl_comp[data->pa_range].table[data->row_idx_smps].dpd_index;
      data->pin_comp =
        rfwcdma_core_current_txlin_data.txlin_pin_comp[data->pa_range];
      data->pout_comp =
        rfwcdma_core_current_txlin_data.txlin_pout_comp[data->pa_range];
    }
    else
    {
      ret_val = FALSE;
    }
  }
  else
  {
    ret_val = FALSE;
}

  return ret_val; 
} /* rfwcdma_core_util_get_rflm_txagc_logging */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Counts the number of carriers in a given multi_carrier_index.
  Equivalent to counting the number of ones in a bitmask.
 
  @param multi_carrier_index
 
  @return
  rfcom_multi_carrier_hspa_id_type enum representing number of carriers
*/
rfcom_multi_carrier_hspa_id_type rfwcdma_core_count_carriers(uint32 multi_carrier_index)
{
  rfcom_multi_carrier_hspa_id_type result;
  uint8 count = 0;
  while (multi_carrier_index > 0)
  {
    count += (multi_carrier_index & 1);
    multi_carrier_index >>= 1;
  }
  result = (rfcom_multi_carrier_hspa_id_type)(count-1);
  return result;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the Max carrier ID corresponding to the Multi Carrier configuration
 
  @param multi_carrier_cfg
    SC/DC/3C configuration
 
  @return
  rfcom_multi_carrier_id_type enum representing Carrier ID
*/
rfcom_multi_carrier_id_type rfwcdma_core_util_get_max_carr_id(rfcom_multi_carrier_hspa_id_type multi_carrier_cfg)
{
  rfcom_multi_carrier_id_type result = RFCOM_MULTI_CARRIER_ID__0;

  switch(multi_carrier_cfg)
  {
    case RFCOM_SINGLE_CARRIER:
    default:
      result = RFCOM_MULTI_CARRIER_ID__0 ;
      break;

    case RFCOM_DUAL_CARRIER:
      result = RFCOM_MULTI_CARRIER_ID__1 ;
      break;
      
    case RFCOM_3_CARRIER:
     result = RFCOM_MULTI_CARRIER_ID__2 ;
     break;

  }
  return result;
}
/*-----------------------------------------------------------------------------*/
/*!
  @brief
  This function gets the rotator value from WFW interface for NBR CSR/XO
  calculation

  @details
 
  @param void

  @return rotator value in KHz 
*/
uint32 rfwcdma_core_util_get_rot_freq_for_nbr
(
  rfm_device_enum_type device,
  rfcom_multi_carrier_id_type car_id
)
{
  rfwcdma_mdsp_get_data_param_type param;
  param.get_data_cmd = RFWCDMA_MDSP_GET_ROT_FREQ_NBR;
  param.data.get_rxagc_val.car_idx = (uint8)rfwcdma_mc_state.rx_multi_carrier_idx[device];

  /* Get RxAGC data from MDSP module */
  if (rfwcdma_mdsp_async_get_rx_data(&param))
  {
    return(param.data.gen_dw);
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfwcdma_core_util_get_rot_freq_for_nbr. device:%d,car_id:%d",
          device,car_id);
    return 0xFF;
  }
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query function to get RxAGC on status from MDSP module

  @details
  RxAGC on/off state is a sub state after RxAGC is enabled. RxAGC on means RxAGC is
  currently running. When RxAGC is enabled, it can be frozen due to sample server freeze
  or other dynamic power saving features. 

  @param rx_chain
  Primary and Secondary path
 
  @param car_id
  Carrier ID 
 
  @retval
  Boolean state for RxAGC ON/Off.

*/
boolean rfwcdma_core_util_is_rxagc_on
(
  rfagc_receive_chain_type rx_chain,
  rfcom_multi_carrier_id_type car_id
)
{
  rfwcdma_mdsp_get_data_param_type param;
  param.get_data_cmd = RFWCDMA_MDSP_GET_RXAGC_VAL;
  param.data.get_rxagc_val.ant_idx = (uint8)rx_chain;
  param.data.get_rxagc_val.car_idx = (uint8)car_id;

  /* Get RxAGC data from MDSP module */
  if (rfwcdma_mdsp_async_get_rx_data(&param))
  {
    return((boolean)(param.data.get_rxagc_val.rxagc_on));
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfwcdma_core_util_is_rxagc_on. rx_chan:%d,car_id:%d",
          rx_chain,car_id);
    return FALSE;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query function to get RxAGC enabled status from MDSP module

  @details
  RxAGC enable/disable status follows RxAGC command from SW. RxAGC is enabled
  when SW send start RxAGC command on the given chain/carrier.
 
  @param rx_chain
  Primary and Secondary path
 
  @param car_id
  Carrier ID 
 
  @retval
  Boolean state for RxAGC ON/Off.

*/

boolean rfwcdma_core_util_is_rxagc_enabled
(
  rfagc_receive_chain_type rx_chain,
  rfcom_multi_carrier_id_type car_id
)
{
  rfwcdma_mdsp_get_data_param_type param;
  param.get_data_cmd = RFWCDMA_MDSP_GET_RXAGC_VAL;
  param.data.get_rxagc_val.ant_idx = (uint8)rx_chain;
  param.data.get_rxagc_val.car_idx = (uint8)car_id;

  /* Get RxAGC data from MDSP module */
  if (rfwcdma_mdsp_async_get_rx_data(&param))
  {
    return( (boolean)(param.data.get_rxagc_val.rxagc_enable));
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfwcdma_core_util_is_rxagc_on. rx_chan:%d,car_id:%d",
          rx_chain,car_id);
    return FALSE;
  }

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get both RxAGC value and RxAGC ON Flag status from Async interface

  @details
  This funtions quries RF-MDSP module to get the RxAGC value and RxAGC ON flag status

  @param rx_chain
  Primary and Secondary path
 
  @param rxagc_type
  Type of RxAGC (compensated or uncompensated)
 
  @param car_id
  Carrier ID 
 
  @param rxagc_unit
  Unit of return value (dB10 or dBm)
 
  @param *rxagc_val
  int16 Rx AGC value in dB10 or dBm
  
  @retval
  Boolean state for RxAGC ON/Off.

*/
boolean rfwcdma_core_util_get_rxagc
(
  rfagc_receive_chain_type rx_chain,
  rfcom_rxagc_type rxagc_type,
  rfcom_multi_carrier_id_type car_id,
  rfcom_rxagc_unit_type rxagc_unit,
  int16 *rxagc_val
)
{
  rfwcdma_mdsp_get_data_param_type param;
  rf_path_enum_type temp_rf_path = RF_PATH_0;
  int16 rxagcval = 0; // Erroneous RxAGC
  boolean rxagc_on = FALSE;
  float rxagcconvert = 0;
  uint16 nf_lookup_idx = 0;
  float noise_floor = 0; 
  float noise_floor_dBm;
  uint16 adjusted_nf_index = 0;
  float adjusted_noise_floor = 0;
  int16 rfagc_high_thresh_val = -336, rfagc_low_thresh_val = -465;
  rfcom_wcdma_band_type band;

  /* Extract band information from rx_chain and carrier for NV table */
  if (rx_chain == RFAGC_RECEIVE_CHAIN_0)
  {
    temp_rf_path = RF_PATH_0;
  }

  else if (rx_chain == RFAGC_RECEIVE_CHAIN_1)
  {
    temp_rf_path = RF_PATH_1;
  }

  else
  {
     *rxagc_val = -512;
     return FALSE;
  }


  switch (car_id)
  {
  case RFCOM_MULTI_CARRIER_ID__0:
    band = rfwcdma_mc_state.car_path_state[temp_rf_path][RFCOM_SINGLE_CARRIER].curr_band_rx;
    break;

  case RFCOM_MULTI_CARRIER_ID__1:
    band = rfwcdma_mc_state.car_path_state[temp_rf_path][RFCOM_DUAL_CARRIER].curr_band_rx;
    break;

  case RFCOM_MULTI_CARRIER_ID__2:
    band = rfwcdma_mc_state.car_path_state[temp_rf_path][RFCOM_3_CARRIER].curr_band_rx;
    break;

  default:
    *rxagc_val = -512;
    return FALSE;
    break;
  }

  /* Band information extracted at this point */

  param.get_data_cmd = RFWCDMA_MDSP_GET_RXAGC_VAL;
  param.data.get_rxagc_val.ant_idx = (uint8)rx_chain;
  param.data.get_rxagc_val.car_idx = (uint8)car_id;

  /* Get RxAGC data from MDSP module */
  if (rfwcdma_mdsp_async_get_rx_data(&param))
  {
    /* If RxAGC is not ON and RxAGC type is not force read, make RxAGC value -512 */
    if (( param.data.get_rxagc_val.rxagc_on == 0)
        && (rxagc_type!=RFCOM_SERVINGCELL_FORCE_READ_RXAGC)
        && (rxagc_type!=RFCOM_SERVINGCELL_FORCE_READ_RXAGC_RAW))
    {
      *rxagc_val = -512;
      return FALSE;
    }
    else
    {
      rxagcval = param.data.get_rxagc_val.rxagc_val;

      if ((rxagc_type == RFCOM_INGAP_RXAGC) 
          || (rxagc_type == RFCOM_SERVINGCELL_RXAGC)
          || (rxagc_type == RFCOM_SERVINGCELL_FORCE_READ_RXAGC))
      {
         /* Null pointer check */
         if (band < RFCOM_NUM_WCDMA_BANDS)
         {
            if (rfnv_wcdma_tbl_ptr[band] == NULL)
            {
              *rxagc_val = -512;
              return FALSE;
            }

            /* Apply noise floor compensation using characterised NV */
            if (rx_chain == RFAGC_RECEIVE_CHAIN_0)
            {
              // Get the noise floor for the current chain/carrier.
              if (car_id == RFCOM_MULTI_CARRIER_ID__0)
              {
               noise_floor = (float)(rfnv_wcdma_tbl_ptr[band]->noise_floor);
              }
              else if (car_id == RFCOM_MULTI_CARRIER_ID__1)
              {
                noise_floor = (float)(rfnv_wcdma_tbl_ptr[band]->noise_floor_car1);
              }
            }

            else if (rx_chain == RFAGC_RECEIVE_CHAIN_1)
            {
              // Get the noise floor for the current chain/carrier.
              if (car_id == RFCOM_MULTI_CARRIER_ID__0)
              {
                noise_floor = (float)(rfnv_wcdma_tbl_ptr[band]->c1_noise_floor_car0);
              }
              else if (car_id == RFCOM_MULTI_CARRIER_ID__1)
              {
                noise_floor = (float)(rfnv_wcdma_tbl_ptr[band]->c1_noise_floor_car1);
              }
            }

            /* Determine the low power level thresholds based on the noise floor. */
            /* Find the delta between the noise floor and the minimum power in dBm by first converting noise floor into dBm */
            /* Scale the value up by the lookup table step size. */
            noise_floor_dBm = (noise_floor-RFAGC_MIN_AGC_VAL)/10 + RF_MIN_RX_POWER_DBM;

            adjusted_nf_index = (uint16)((float)((noise_floor_dBm - RF_MIN_RX_POWER_DBM)*NF_LOOKUP_STEP_SIZE));

            // Bounds check before arrary access.
            if (adjusted_nf_index >= NF_LOOKUP_MAX)
            {
              adjusted_nf_index = NF_LOOKUP_MAX-1;
            }

            // Adjust the noise floor.
            adjusted_noise_floor = noise_floor_dBm  + rfagc_low_threshold[adjusted_nf_index];

            // Set the lower agc threshold in db10 from the adjusted noise floor.
            rfagc_low_thresh_val = (int16)((float)((adjusted_noise_floor-RF_MIN_RX_POWER_DBM)*10)+RFAGC_MIN_AGC_VAL);

            // Set the upper agc threshold, after which no compensation will be applied.
            rfagc_high_thresh_val = rfagc_low_thresh_val + HIGH_THRESHOLD_OFFSET;

            /* rxagc becomes nonlinear at low Rx power. Apply noise floor correction at low pwr */
            if (rxagcval < rfagc_low_thresh_val)
            {
              rxagcval = RFAGC_MIN_AGC_VAL;
            }
            else if (rxagcval <= rfagc_high_thresh_val)
            {
               // Convert the ADC value to RSSI
               rxagcconvert = ((float)rxagcval-RFAGC_MIN_AGC_VAL)/10+RF_MIN_RX_POWER_DBM;

               // Find the index into the noise floor offset table.
               // The noise floor is by definition always lower than the RSSI value.
               // Multiply by the dB step size for the table.
               // Convert to uint for table index
               nf_lookup_idx = (uint16)((float)((rxagcconvert-noise_floor_dBm)*NF_LOOKUP_STEP_SIZE));

               // Ensure we don't have an out of bounds access
               if (nf_lookup_idx >= NF_LOOKUP_MAX)
               {
                 nf_lookup_idx = NF_LOOKUP_MAX-1;
               }

               // Apply the correction factor based on the table lookup.
               rxagcconvert += noise_floor_delta[nf_lookup_idx];

               // Convert the RSSI level back to db10
               rxagcval = (int16)((float)((rxagcconvert-RF_MIN_RX_POWER_DBM)*10)+RFAGC_MIN_AGC_VAL);
              
               // Cap at minimum value
               if (rxagcval < RFAGC_MIN_AGC_VAL)
               {
                 rxagcval = RFAGC_MIN_AGC_VAL;
               }
            } /* else if (rxagcval <= rfagc_high_thresh_val)*/
         } /* if (band < RFCOM_NUM_WCDMA_BANDS)*/

         else
         {
           *rxagc_val = -512; //Not a valid band in use
           return FALSE; 
         }
       } /*      if ((rxagc_type == RFCOM_INGAP_RXAGC) 
          || (rxagc_type == RFCOM_SERVINGCELL_RXAGC)
          || (rxagc_type == RFCOM_SERVINGCELL_FORCE_READ_RXAGC))*/
       
       /* If we are here, then the we return a valid rxagc value*/
       rxagc_on = TRUE;

       /* Check if we need to convert it to dBm unit */
       if (rxagc_unit == RFCOM_RXAGC_IN_DBM)
       {
         *rxagc_val = (RF_MIN_RX_POWER_DBM + ((rxagcval - RFAGC_MIN_AGC_VAL) / 10));
       }

       else
       {
         *rxagc_val = rxagcval;
       }

    } /*    else(( param.data.get_rxagc_val.rxagc_on == 0)
        && (rxagc_type!=RFCOM_SERVINGCELL_FORCE_READ_RXAGC)
        && (rxagc_type!=RFCOM_SERVINGCELL_FORCE_READ_RXAGC_RAW))*/

  } /* if (rfwcdma_mdsp_async_get_rx_data(&param)) */

  else
  /* MDSP API failed */
  {
    MSG_4(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfwcdma_core_util_get_rxagc failed. rx_chan:%d, rxagc_type:%d, car_id:%d, rxagc_unit:%d ",
          rx_chain,rxagc_type,car_id,rxagc_unit);
    *rxagc_val = -512;
  }

  return rxagc_on;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get RxAGC value from RF-MDSP module during X2W measurement

  @details
  This funtions queries RF-MDSP module to get the RxAGC value

  @param rx_chain
  Primary and Secondary path
 
  @param rxagc_type
  Type of RxAGC (compensated or uncompensated)
 
  @param car_id
  Carrier ID 
 
  @param rxagc_unit
  Unit of return value (dB10 or dBm)
 
  @param band
  Band information for neighbour

  @param bypass_rxagcon_flag
  Flag to indicate whether RxAGC on needs to be bypassed for measurements
 
  @retval
  int16 Rx AGC value in dB10 or dBm

*/
int16 rfwcdma_core_util_get_rxagc_xtow_meas
(
  rfagc_receive_chain_type rx_chain,
  rfcom_rxagc_type rxagc_type,
  rfcom_multi_carrier_id_type car_id,
  rfcom_rxagc_unit_type rxagc_unit,
  rfcom_wcdma_band_type band,
  boolean bypass_rxagcon_flag
)
{
  rfwcdma_mdsp_get_data_param_type param;
  int16 rxagcval = -512; // Erroneous RxAGC
  float rxagcconvert = 0;
  uint16 nf_lookup_idx = 0;
  float noise_floor = 0; 
  float noise_floor_dBm;
  uint16 adjusted_nf_index = 0;
  float adjusted_noise_floor = 0;
  int16 rfagc_high_thresh_val = -336, rfagc_low_thresh_val = -465;

  param.get_data_cmd = RFWCDMA_MDSP_GET_RXAGC_VAL;
  param.data.get_rxagc_val.ant_idx = (uint8)rx_chain;
  param.data.get_rxagc_val.car_idx = (uint8)car_id;

  /* Get RxAGC data from MDSP module */
  if (rfwcdma_mdsp_async_get_rx_data(&param))
  {
    /* If RxAGC is not ON when bypass rxagcon flag is false, make RxAGC value -512 */
    if ( (bypass_rxagcon_flag == FALSE)
        && ( param.data.get_rxagc_val.rxagc_on == 0) )
    {
      RF_MSG(RF_ERROR,
             "rfwcdma_core_util_get_rxagc_x2w_meas: RxAGC On flag is not enabled");
      rxagcval = -512;
    }

    /* 1. If bypass flag is true OR
    2. when flag is false, if rxagc is ON then in both cases, proceed with reading rxagc value from interface*/
    else if ( (bypass_rxagcon_flag == TRUE) || 
              ((bypass_rxagcon_flag == FALSE) && (param.data.get_rxagc_val.rxagc_on == 1)) )
    {
      rxagcval = param.data.get_rxagc_val.rxagc_val;

      if ( rxagc_type == RFCOM_INGAP_RXAGC )
      {
         /* Null pointer check */
         if (band < RFCOM_NUM_WCDMA_BANDS)
         {
            if (rfnv_wcdma_tbl_ptr[band] == NULL)
            {
              RF_MSG_1(RF_ERROR,
              "rfwcdma_core_util_get_rxagc_x2w_meas: NULL NV tbl ptr for band %d",
              band);
              return -512;
            }

            /* Apply noise floor compensation using characterised NV */
            if (rx_chain == RFAGC_RECEIVE_CHAIN_0)
            {
              // Get the noise floor for the current chain/carrier.
              if (car_id == RFCOM_MULTI_CARRIER_ID__0)
              {
               noise_floor = (float)(rfnv_wcdma_tbl_ptr[band]->noise_floor);
              }
              else if (car_id == RFCOM_MULTI_CARRIER_ID__1)
              {
                noise_floor = (float)(rfnv_wcdma_tbl_ptr[band]->noise_floor_car1);
              }
            }

            else if (rx_chain == RFAGC_RECEIVE_CHAIN_1)
            {
              // Get the noise floor for the current chain/carrier.
              if (car_id == RFCOM_MULTI_CARRIER_ID__0)
              {
                noise_floor = (float)(rfnv_wcdma_tbl_ptr[band]->c1_noise_floor_car0);
              }
              else if (car_id == RFCOM_MULTI_CARRIER_ID__1)
              {
                noise_floor = (float)(rfnv_wcdma_tbl_ptr[band]->c1_noise_floor_car1);
              }
            }

            /* Determine the low power level thresholds based on the noise floor. */
            /* Find the delta between the noise floor and the minimum power in dBm by first converting noise floor into dBm */
            /* Scale the value up by the lookup table step size. */
            noise_floor_dBm = (noise_floor-RFAGC_MIN_AGC_VAL)/10 + RF_MIN_RX_POWER_DBM;

            adjusted_nf_index = (uint16)((float)((noise_floor_dBm - RF_MIN_RX_POWER_DBM)*NF_LOOKUP_STEP_SIZE));

            // Bounds check before arrary access.
            if (adjusted_nf_index >= NF_LOOKUP_MAX)
            {
              adjusted_nf_index = NF_LOOKUP_MAX-1;
            }

            // Adjust the noise floor.
            adjusted_noise_floor = noise_floor_dBm  + rfagc_low_threshold[adjusted_nf_index];

            // Set the lower agc threshold in db10 from the adjusted noise floor.
            rfagc_low_thresh_val = (int16)((float)((adjusted_noise_floor-RF_MIN_RX_POWER_DBM)*10)+RFAGC_MIN_AGC_VAL);

            // Set the upper agc threshold, after which no compensation will be applied.
            rfagc_high_thresh_val = rfagc_low_thresh_val + HIGH_THRESHOLD_OFFSET;

            /* rxagc becomes nonlinear at low Rx power. Apply noise floor correction at low pwr */
            if (rxagcval < rfagc_low_thresh_val)
            {
              rxagcval = RFAGC_MIN_AGC_VAL;
            }
            else if (rxagcval <= rfagc_high_thresh_val)
            {
               // Convert the ADC value to RSSI
               rxagcconvert = ((float)rxagcval-RFAGC_MIN_AGC_VAL)/10+RF_MIN_RX_POWER_DBM;

               // Find the index into the noise floor offset table.
               // The noise floor is by definition always lower than the RSSI value.
               // Multiply by the dB step size for the table.
               // Convert to uint for table index
               nf_lookup_idx = (uint16)((float)((rxagcconvert-noise_floor_dBm)*NF_LOOKUP_STEP_SIZE));

               // Ensure we don't have an out of bounds access
               if (nf_lookup_idx >= NF_LOOKUP_MAX)
               {
                 nf_lookup_idx = NF_LOOKUP_MAX-1;
               }

               // Apply the correction factor based on the table lookup.
               rxagcconvert += noise_floor_delta[nf_lookup_idx];

               // Convert the RSSI level back to db10
               rxagcval = (int16)((float)((rxagcconvert-RF_MIN_RX_POWER_DBM)*10)+RFAGC_MIN_AGC_VAL);
              
               // Cap at minimum value
               if (rxagcval < RFAGC_MIN_AGC_VAL)
               {
                 rxagcval = RFAGC_MIN_AGC_VAL;
               }
            }
         } /* if (band < RFCOM_NUM_WCDMA_BANDS) */

         else
         {
           
           RF_MSG_1(RF_ERROR,
                   "rfwcdma_core_util_get_rxagc_x2w_meas: Not a valid band %d",
                   band);
           return -512; //Not a valid band in use
         }
      } /* if ( rxagc_type == RFCOM_INGAP_RXAGC ) */

      /* Check if we need to convert it to dBm unit */
      if (rxagc_unit == RFCOM_RXAGC_IN_DBM)
      {
        rxagcval = (RF_MIN_RX_POWER_DBM + ((rxagcval - RFAGC_MIN_AGC_VAL) / 10));
      }
    } /* else if ( (bypass_rxagcon_flag == TRUE) || 
              ((bypass_rxagcon_flag == FALSE) && (param.data.get_rxagc_val.rxagc_on == 1)) )*/

    else
    {
      RF_MSG_1(RF_ERROR,
            "rfwcdma_core_util_get_rxagc_x2w_meas: Invalid input paramters with rxagc on flag %d", param.data.get_rxagc_val.rxagc_on);
      rxagcval = -512;
    }
  } /* if (rfwcdma_mdsp_async_get_rx_data(&param)) */

  else
  /* MDSP API failed */
  {
    MSG_4(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfwcdma_core_util_get_rxagc_x2w_meas failed. rx_chan:%d, rxagc_type:%d, car_id:%d, rxagc_unit:%d ",
          rx_chain,rxagc_type,car_id,rxagc_unit);
    rxagcval = -512;
  }
  return rxagcval;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Util function to get current LNA state info from MDSP module 

  @details
  This is core util function that will call rfwcdma_mdsp_async_get_data() to get
  the current LNA state info from FW
 
  @param device
  RFM device enum ID
 
  @return
  LNA state info. Return 0xFF to indicate API failure
 
*/
uint8 rfwcdma_core_util_get_lna_state(rfm_device_enum_type device)
{
  rfwcdma_mdsp_get_data_param_type param = {0};

  param.get_data_cmd = RFWCDMA_MDSP_GET_CUR_LNA_STATE;
  param.rflm_handle = (rflm_dm_handle_id_t)rfwcdma_mc_state.rxlm_buf_idx[device];

  if (rfwcdma_mdsp_async_get_rx_data(&param))
  {
    return param.data.gen_b;
  }
  else
  {
    /* Failed to get LNA state data, return 0xFF as error indicator */
    return 0xFF;
  }
  
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Retrieves maximum TX power limit value from MDSP.

  @details
  This function returns the maximum TX power limit value in mdsp agc units.
  To convert to dBm use the formula dBm = (mdsp units + 512)/RF_TXAGC_RESOLUTION  - 57.

  @retval int16 tx power limit value. range is -512..511 with 1/10 dB resolution.

  @pre
  WCDMA mdsp fw image must be loaded.
*/
int16 rfwcdma_core_util_get_mdsp_tx_power_limit( void )
{
  return rfwcdma_mdsp_get_tx_power_limit();
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the rf warmup time to L1.

  @details

  @retval uint16 Rf warmup time in uS.
*/
uint16 rfwcdma_core_util_get_rf_warmup_time( void )
{
  rfc_wcdma_core_config_type rfc_core_config;
  (void)rfc_wcdma_command_dispatch(RFM_DEVICE_0, RFC_WCDMA_GET_CORE_CONFIG, (void*)(&rfc_core_config));
  return(rfc_core_config.warmup_time);
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the contribution to the transmit power that reflects the accumulation of UL TPC commands.

  @details

  @retval int16

  @pre
  rfm_init is called.
  WCDMA mdsp fw image must be loaded.
*/
int16 rfwcdma_core_util_get_txagctablestore(rfcom_multi_carrier_id_type car_idx)
{
  return(rfwcdma_mdsp_get_txagctablestore(car_idx));
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the Tx rotator angle values from RF NV.

  @details

  @param tx_rot_angle Tx rotator angle structure

*/
void rfwcdma_core_util_get_tx_rotator_angle
(
  rfwcdma_mc_rot_angle_type* tx_rot_angle
)
{
  rfcom_wcdma_band_type band = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx;// Use Rx band as Tx band has not been initialised
    
  /* Null pointer check */
  /*! @todo expand to 8 PA state including WL1 interface change */
  if (band < RFCOM_NUM_WCDMA_BANDS)
  {
    if (rfnv_wcdma_tbl_ptr[band] != NULL)
    {
      if ( tx_rot_angle->tx_rot_ang_pa_00_p != NULL )
        *(tx_rot_angle->tx_rot_ang_pa_00_p) = rfnv_wcdma_tbl_ptr[band]->pa_static.tx_rot_angle[0];

      if ( tx_rot_angle->tx_rot_ang_pa_01_p != NULL )
        *(tx_rot_angle->tx_rot_ang_pa_01_p) = rfnv_wcdma_tbl_ptr[band]->pa_static.tx_rot_angle[1];

      if ( tx_rot_angle->tx_rot_ang_pa_10_p != NULL )
        *(tx_rot_angle->tx_rot_ang_pa_10_p) = rfnv_wcdma_tbl_ptr[band]->pa_static.tx_rot_angle[2];

      if ( tx_rot_angle->tx_rot_ang_pa_11_p != NULL )
        *(tx_rot_angle->tx_rot_ang_pa_11_p) = rfnv_wcdma_tbl_ptr[band]->pa_static.tx_rot_angle[3];
    }
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the PA range map from RF NV.

  @details

  @param PA index value

*/
uint8 rfwcdma_core_util_get_pa_range_from_nv(uint8 pa_index)
{
  rfcom_wcdma_band_type band = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx;// Use Rx band as Tx band has not been initialised
    
  /* Null pointer check */
  if (band < RFCOM_NUM_WCDMA_BANDS)
  {
    /* Rx and Tx on the same band */
    if (rfnv_wcdma_tbl_ptr[band] != NULL)
    {
      return(rfnv_wcdma_tbl_ptr[band]->pa_static.pa_range_map[pa_index]);
    }
  }

  return 0;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the Beta Scaling Comp from RF NV.

  @details

  @param MPR index

*/
uint16 rfwcdma_core_util_get_beta_scaling_comp(uint8 mpr_idx)
{
  rfcom_wcdma_band_type band = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx;// Use Rx band as Tx band has not been initialised

  if ( band < RFCOM_NUM_WCDMA_BANDS )
  {
    if (rfnv_wcdma_tbl_ptr[band] != NULL)
    {
      return(rfnv_wcdma_tbl_ptr[band]->wcdma_rel6_tx_beta_scaling_comp[mpr_idx]);
    }
  }

  return 0;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the maximum TX power that UE is capable to transmit in dbm unit.

  @details

  @param chan_type DCH or RACH

  @return int16 maximum TX power in dbm unit.
*/
int16 rfwcdma_core_util_get_ue_max_tx_power( rfcom_txplim_channel_type chan_type )
{
  int16 max_tx_power = RF_UE_MAX_TX_POWER;

  if (chan_type != RFCOM_TXPLIM_RACH && chan_type != RFCOM_TXPLIM_DCH)
  {
     MSG_ERROR( "Invalid control channel specified for max Tx power %d", chan_type,0,0);
  }
  else
  {
    max_tx_power = rfwcdma_core_txplim_get_desired_limit();
  }

  return (max_tx_power);
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the minimum TX power that UE is capable to transmit in dbm unit.

  @details

  @retval int16 Minimum TX power in db10 unit.

  @pre
  rfm_init is called.
*/
int16 rfwcdma_core_util_get_ue_min_tx_power( void )
{
  return(rfwcdma_core_util_ue_min_tx_power);
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return TX AGC value in db10 unit based on the passed in dbm unit value.

  @details
  <A detailed description of what the function does, how it does it, and any dependencies>

  @param dbmval <Parameter description>

  @return int16 TX AGC value in db10 unit.
*/
int16 rfwcdma_core_util_convert_tx_dbm_to_agc_unit( int16 dbmval )
{
  return ((int16) ( ( (dbmval - RF_MIN_TX_POWER_DBM) * RF_TXAGC_RESOLUTION) + RFAGC_MIN_AGC_VAL ));
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns TX power value in dbm unit based on the passed in txagcval value in
  db10 unit.

  @details

  @param txagcval Tx AGC value

  @return int16 TX AGC value in dbm unit.

*/
int16 rfwcdma_core_util_convert_tx_agc_to_dbm_unit( int16 txagcval )
{
  return (((txagcval - RFAGC_MIN_AGC_VAL) / RF_TXAGC_RESOLUTION ) + RF_MIN_TX_POWER_DBM);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns TX power value in TxAGC dB10 unit based on the passed Tx power in dBm10 unit

  @details

  @param dbm10val Tx power is dBm 10 unit

  @return int16 TX AGC value in db10 unit.

*/
int16 rfwcdma_core_util_convert_dbm10_to_txagc_unit( int16 dbm10val )
{
  return ((dbm10val - (RF_MIN_TX_POWER_DBM * RF_TXAGC_RESOLUTION)) + RFAGC_MIN_AGC_VAL);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns whether the RF is sleeping right now.

  @details

  @param rx_chain Primary or Diversity path

  @retval boolean True if RF is in sleep state.
*/
boolean rfwcdma_core_util_is_sleeping( rf_path_enum_type rx_chain )
{
  /* Check to determine the rf state variable for sleep condition */
  if ( rfwcdma_mc_state.car_path_state[rx_chain][0].rf_state == RFWCDMA_MC_STATE_SLEEP )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Update Tx minimum power value with stored RF parameter

*/
void rfwcdma_core_update_min_power_from_nv(void)
{
  int16 min_tx_power;

    min_tx_power = rfcommon_nv_mm_tbl_ptr->min_tx_power;
    if (min_tx_power >= RFAGC_MIN_AGC_VAL)
    {
      rfwcdma_core_util_ue_min_tx_power = min_tx_power;
    }
    /* Update MDSP with min Tx power from NV (in db10 unit)
       Note: calculation algo shown in rfwcdma_core_util_convert_tx_dbm_to_agc_unit()
     */
    rfwcdma_mdsp_set_tx_lower_limit(rfwcdma_core_util_ue_min_tx_power);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Get ENC BTF value

  @return uint16 ENC BTF value

*/
/*!@todo anupamav: comment out for now*/
uint16 rfwcdma_core_get_enc_btf_val(void)
{
  #define RFAGC_MDSP_BTFDELAY_VAL               -357
  uint32 btfdelay = RFAGC_MDSP_BTFDELAY_VAL;

  rfcom_wcdma_band_type band = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx;// Use Rx band as Tx band has not been initialised
 
  /* Null pointer check */
  if (band < RFCOM_NUM_WCDMA_BANDS)
  {
    if (rfnv_wcdma_tbl_ptr[band] != NULL)
    {
      btfdelay = rfnv_wcdma_tbl_ptr[band]->enc_btf_dly;
    }
  }
  /* The enc btf delay is stored in chipx8 unit in NV as uint32*/
  /* Convert to int16 value and divide by 8 to put the value in cx1 */
  return( (uint16)(rf_divide_with_rounding(abs(btfdelay),8)) );
}
/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details

*/

int32 rfwcdma_core_util_math_floor(
  /* dividend */
  int32 dividend,
  /* divisor */
  int32 divisor)
{
  int32 quotient;

  if (divisor == 0)
  {
    MSG_ERROR("Divide by zero",0,0,0);
    return 0;
  }

  /* Get exact/truncated value after integer division */
  quotient = dividend/divisor;

  /* Determine integer x such that x-1 < floor(x) <= x */
  if ((quotient < 0) ||
      ((quotient == 0) && (SIGN(dividend) != SIGN(divisor))))  
     // leave floor(x) truncated for +ve quotients
  {
    if (ABS(divisor * quotient) != ABS(dividend))
      quotient--;  // round towards -infinity
  }

  /* Note that -22/5 = -5, not -4 */
  return quotient;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the NV index for a given PA range form the PA range map RF NV.

  @details

  @param PA range value.

*/
uint8 rfwcdma_core_util_get_pa_range_map_nv_index(uint8 pa_range)
{
  uint8 pa_index;

  rfcom_wcdma_band_type band = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_rx;// Use Rx band as Tx band has not been initialised

  if (band < RFCOM_NUM_WCDMA_BANDS)
  {
    if (rfnv_wcdma_tbl_ptr[band] != NULL)
    {
      for(pa_index=0; pa_index<RFCOM_TXLIN_NUM_LINEARIZERS; pa_index++)
      {
        if(pa_range == rfnv_wcdma_tbl_ptr[band]->pa_static.pa_range_map[pa_index])
        {
          return pa_index;
        }
      }
    }
  }
  /* PA range not found */
  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"PA range not found in the pa_range_map NV",0);
  return (0xFF);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function retrieves the buffered TxAGC value from MDSP when HDET is triggered.

  @details
  It reads the following mDSP variable.
  - bufferedTxGain: Buffered TX AGC value when HDET is triggered.

  @return int16 TxAGC value.

  Example Usage:
  @code
     txagc_val = rfwcdma_core_util_get_hdet_tx_agc_value();
  @endcode

*/
int16 rfwcdma_core_util_get_hdet_tx_agc_value(void)
{
  return (rfwcdma_mdsp_get_hdet_tx_agc_value());
} /* end of rfwcdma_core_util_get_hdet_tx_agc_value() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the mDSP cmd status flag.

  @details

  @return
  the boolean value in rfwcdma_mdsp_get_rxagc_cmd_status()

*/
boolean rfwcdma_core_util_get_rxagc_cmd_status(void)
{
  return (rfwcdma_mdsp_get_rxagc_cmd_status());
} /* end of rfwcdma_core_util_get_rxagc_cmd_status() */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function gets the WCDMA mask

  @return
  uint8 WCDMA mask  
*/

uint64 rfwcdma_core_util_get_band_mask(rfcom_wcdma_band_type band)
{
  uint64 ret_val = 0;

  switch(band)	 
  {
  case  RFCOM_BAND_IMT:
    ret_val = RF_WCDMA_B1_MASK;
    break;
   
  case RFCOM_BAND_1900:
     ret_val = RF_WCDMA_B2_MASK;
    break;

  case  RFCOM_BAND_BC3:
    ret_val = RF_WCDMA_B3_MASK;
    break;

  case  RFCOM_BAND_BC4:
    ret_val = RF_WCDMA_B4_MASK;
    break;
   
  case  RFCOM_BAND_800:
    ret_val = RF_WCDMA_B5_MASK;
    break;

  case  RFCOM_BAND_BC8:
    ret_val = RF_WCDMA_B8_MASK;
    break;
   
  case  RFCOM_BAND_BC9:
    ret_val = RF_WCDMA_B9_MASK;
    break;
   
  case  RFCOM_BAND_BC11:
    ret_val = RF_WCDMA_B11_MASK;
    break;

  case  RFCOM_BAND_BC19:
    ret_val = RF_WCDMA_B19_MASK;
    break;
    	
  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"RF_get_sys_band_mask: Invalid WCDMA band %d!", band);  
    break;
  }

  return ret_val;
} 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function gets the min and max Rx freq (in KHz) of a WCDMA band

  @details

  @return
  boolean TRUE if band is supported, FALSE otherwise
*/
boolean rfwcdma_core_util_get_min_max_rx_freqs(rfcom_wcdma_band_type band, uint32* min, uint32* max)
{
  boolean ret_val= TRUE;
  switch(band)
  {
  case RFCOM_BAND_IMT:
    *min = BSP_RF_RX_FREQ_WCDMA_MIN;
    *max = BSP_RF_RX_FREQ_WCDMA_MAX;
    break;

  case RFCOM_BAND_1900:
    *min = BSP_RF_RX_FREQ_WCDMA_BC2_MIN;
    *max = BSP_RF_RX_FREQ_WCDMA_BC2_MAX;
    break;

  case RFCOM_BAND_BC3:
    *min = BSP_RF_RX_FREQ_WCDMA_BC3_MIN;
    *max = BSP_RF_RX_FREQ_WCDMA_BC3_MAX;
    break;

  case RFCOM_BAND_BC4:
    *min = BSP_RF_RX_FREQ_WCDMA_BC4_MIN;
    *max = BSP_RF_RX_FREQ_WCDMA_BC4_MAX;
    break;

  case RFCOM_BAND_800:
    *min = BSP_RF_RX_FREQ_WCDMA_BC5_MIN;
    *max = BSP_RF_RX_FREQ_WCDMA_BC5_MAX;
    break;

  case RFCOM_BAND_BC8:
    *min = BSP_RF_RX_FREQ_WCDMA_BC8_MIN;
    *max = BSP_RF_RX_FREQ_WCDMA_BC8_MAX;
    break;

  case RFCOM_BAND_BC9:
    *min = BSP_RF_RX_FREQ_WCDMA_BC9_MIN;  //CORRECTED
    *max = BSP_RF_RX_FREQ_WCDMA_BC9_MAX;  //CORRECTED
    break;

  case RFCOM_BAND_BC11:
    *min = BSP_RF_RX_FREQ_WCDMA_BC11_MIN;
    *max = BSP_RF_RX_FREQ_WCDMA_BC11_MAX;
    break;

  case RFCOM_BAND_BC19:
    *min = BSP_RF_RX_FREQ_WCDMA_BC19_MIN;
    *max = BSP_RF_RX_FREQ_WCDMA_BC19_MAX;
    break;

  default:
     MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"rfwcdma_core_util_get_min_max_rx_freqs : Invalid WCDMA band %d!", band);  
     ret_val = FALSE;
  } // end switch

  return ret_val;
} /*rfwcdma_core_util_get_min_max_rx_freqs*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function gets the min and max Tx freq (in KHz) of a WCDMA band

  @details

  @return
  boolean TRUE if band is supported, FALSE otherwise
*/
boolean rfwcdma_core_util_get_min_max_tx_freqs(rfcom_wcdma_band_type band, uint32* min, uint32* max)
{
  boolean ret_val= TRUE;
  switch( band)
  {
  case RFCOM_BAND_IMT:
    *min = BSP_RF_TX_FREQ_WCDMA_MIN;
    *max = BSP_RF_TX_FREQ_WCDMA_MAX;
    break;

  case RFCOM_BAND_1900:
    *min = BSP_RF_TX_FREQ_WCDMA_BC2_MIN;
    *max = BSP_RF_TX_FREQ_WCDMA_BC2_MAX;
    break;

  case RFCOM_BAND_BC3:
    *min = BSP_RF_TX_FREQ_WCDMA_BC3_MIN;
    *max = BSP_RF_TX_FREQ_WCDMA_BC3_MAX;
    break;

  case RFCOM_BAND_BC4:
    *min = BSP_RF_TX_FREQ_WCDMA_BC4_MIN;
    *max = BSP_RF_TX_FREQ_WCDMA_BC4_MAX;
    break;

  case RFCOM_BAND_800:
    *min = BSP_RF_TX_FREQ_WCDMA_BC5_MIN;
    *max = BSP_RF_TX_FREQ_WCDMA_BC5_MAX;
    break;

  case RFCOM_BAND_BC8:
    *min = BSP_RF_TX_FREQ_WCDMA_BC8_MIN;
    *max = BSP_RF_TX_FREQ_WCDMA_BC8_MAX;
    break;

  case RFCOM_BAND_BC9:
    *min = BSP_RF_TX_FREQ_WCDMA_BC9_MIN;
    *max = BSP_RF_TX_FREQ_WCDMA_BC9_MAX;
    break;

  case RFCOM_BAND_BC11:
    *min = BSP_RF_TX_FREQ_WCDMA_BC11_MIN;
    *max = BSP_RF_TX_FREQ_WCDMA_BC11_MAX;
    break;

  case RFCOM_BAND_BC19:
    *min = BSP_RF_TX_FREQ_WCDMA_BC19_MIN;
    *max = BSP_RF_TX_FREQ_WCDMA_BC19_MAX;
    break;

  default:
     MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"rfwcdma_core_util_get_min_max_tx_freqs : Invalid WCDMA band %d!", band);  
     ret_val = FALSE;
  } // end switch

  return ret_val;
} /*rfwcdma_core_util_get_min_max_tx_freqs*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function gets the band num for the rfcom wcdma band type

  @details

  @return
  boolean TRUE if band is supported, FALSE otherwise
*/
boolean rfwcdma_core_util_get_band_num(rfcom_wcdma_band_type band, uint16* num)
{
  boolean ret_val= TRUE;
  switch(band)
  {
  case RFCOM_BAND_IMT:
    *num = 1;
    break;

  case RFCOM_BAND_1900:
    *num = 2;
    break;

  case RFCOM_BAND_BC3:
    *num = 3;
    break;

  case RFCOM_BAND_BC4:
    *num = 4;
    break;

  case RFCOM_BAND_800:
    *num = 5;
    break;

  case RFCOM_BAND_BC8:
    *num = 8;
    break;

  case RFCOM_BAND_BC9:
    *num = 9;
    break;

  case RFCOM_BAND_BC11:
    *num = 11;
    break;

  case RFCOM_BAND_BC19:
    *num = 19;
    break;

  default:
     MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"rfwcdma_core_util_get_min_max_rx_freqs : Invalid WCDMA band %d!", band);  
     ret_val = FALSE;
  } // end switch

  return ret_val;
} /*rfwcdma_core_util_get_band_num*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function handles device to antenna index mapping

  @param device Devices supported by RF driver

  @return antenna index

*/
uint8 rfwcdma_core_util_device_to_antenna( rfcom_device_enum_type device )
{
  uint8 temp_antenna;

  switch (device)
  {
     case RFM_DEVICE_0 :
     case RFM_DEVICE_2 :
      temp_antenna = 0;
      break;

     case RFM_DEVICE_1 :
     case RFM_DEVICE_3 :
      temp_antenna = 1;
      break;

   default :
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"invalid device: no anntenna index found.",0);
      temp_antenna = 0;
      break;
  }

  return temp_antenna;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function handles rfm device to lna device index mapping

  @param device Devices supported by RF driver

  @return lna device index

*/
uint8 rfwcdma_core_util_device_to_lna_id( rfcom_device_enum_type device, rfwcdma_mdsp_rx_irat_meas_type meas_scenario )
{
  uint8 lna_id = 0;

  rfwcdma_mc_state_type *rfwcdma_mc_state_copy;

  if (meas_scenario == RFLM_WCDMA_HOME_MEAS) 
  {
    rfwcdma_mc_state_copy = rfwcdma_mc_get_state();
    if (device == rfwcdma_mc_state_copy->primary_rx_device || device == rfwcdma_mc_state_copy->secondary_rx_device)
    {
       lna_id = 0;
    }

    else
    {
       lna_id = 1;
    }
  }

  /* For all IRAT scenarios */
  else
  {
     lna_id = 0;
  }

  //RF_MSG_2(RF_HIGH, "rfwcdma_core_device_to_lna_id: Device passed: %d, LNA Id: %d ", device, lna_id);

  return lna_id ;
}
/*----------------------------------------------------------------------------*/
/*!
  @details
  This function get the multiple carrier number from carrier mode

  @param carrier_mode: carrier mode

  @return tx_multi_carriers: carrier id
*/
rfcom_multi_carrier_hspa_id_type 
rfwcdma_core_get_multi_carriers_from_carrier_mode
(
rfwcdma_mc_carrier_idx_enum_type carrier_mode
)
{
  rfcom_multi_carrier_hspa_id_type multi_carriers;

  switch(carrier_mode)
  {
    case RFWCDMA_MC_SC:
      multi_carriers = RFCOM_SINGLE_CARRIER;
      break;

    case RFWCDMA_MC_DC:
      multi_carriers = RFCOM_DUAL_CARRIER;
      break;

    case RFWCDMA_MC_3C:
      multi_carriers = RFCOM_3_CARRIER;
      break;

    default:
      multi_carriers = RFCOM_MAX_CARRIERS;
      break;
  }

  return multi_carriers;
}
