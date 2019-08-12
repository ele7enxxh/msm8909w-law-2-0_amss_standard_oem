/*===========================================================================

               M E A S    S P E C I F I C   T E S T  F U N C T I O N S

DESCRIPTION
   This file contains implementations for MEAS specific test functions

   Copyright (c) 2009 - 2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/meas/ftm/src/ftm_meas.c#1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/05/14   aw      fixed device_id typo
04/29/14   ac      nbr support
04/18/14   rmb     Change ENUM type for Core AGC to MDSP AGC.
02/27/14   rmb     First phase FTM IRAT checkin for G2W.
02/13/14   dw      [1]Change AGC command function to support multiple device
                   [2]Code cleanup
12/30/13   pl      put FTM IRAT tuning within a flag
12/13/13   rmb     Fix Compiler Error.
12/12/13   rmb     Add FTM W2W IRAT support for Bolt.
12/11/13   pl      RE-enable exit_tech and enter_tech in xtoy_test_y and 
                   x_to_y_test_x respectively
12/04/13   pl      Create handle for WCDMA also
12/04/13   pl      Populate Handle meta properly
11/26/13   nrk     Fixed compiler warnings
08/13/13   pl      Cleanup unused interfaces
08/13/13   pl      Added interface to log and retrieve IRAT script info
05/09/13   aa      updated W2W FTM IRAT for new FTM framework
05/08/13   pl      Fix RXLM allocation when aborting
04/23/13   gvn     Added Abort Support
02/06/13   pl      Don't allocate RXLM buffer for WCDMA, since WCDMA allocates for itself
02/04/13   pl      update to common IRAT header
01/22/13   pl      Implement code review comments
01/20/13   pl      FMT IRAT cleanup
01/20/13   pl      FTM LTE-CA IRAT support
11/08/12   pl      Consolidate function pointer table
10/31/12   pl      adding MCPM meas support into FTM IRAT 
10/24/12   pl      remove obsolete functions
10/24/12   pl      Update ftm framework interface to clarify usage
10/22/12   pl      Update mdsp interface to become re-enterant
10/19/12   pl      fix bug on passing RXLM buffers
10/17/12   pl      enable execution of RXLM buffers
10/17/12   ka      Added support for RFFE writes to W2W test
10/16/12   ka      Fixed data type and warnings
10/16/12   ka      Added W2W FTM test
10/13/12   pl      Enable using RFSW to execute IRAT buffers
10/09/12   pl      FTM IRAT framework revamp
10/09/12   pl      FTM IRAT cleanup
10/05/12   aa      Remove  obsolete wtow measurement
10/02/12   pl      Remove obsolete functions
10/01/12   sbm     Remove  rfd_grfc_engine_to_grfc_out() usage.
10/01/12   sb      Made changes to add rfm device to rf device interface and uppler layer APIs 
09/15/12   lp      Resolved compilation errors with FEATURE_LTE disabled
08/09/12   php     Add support for new FW CA API
08/08/12   kb      Updated access to FW shared memory location to work with new FW interface. 
07/31/12   qzh     Rfdevice interface change to use rfm_device instead of path
07/29/12   jfc     Include "rfd_legacy_inc" since code still references RFD 
07/29/12   jfc     Fix error in merge
07/20/12   sr      Made changes to pass the RFM_DEVICE and rf_buffer to rfc_gsm
07/02/12   kb      updated API functions to support script buffers
06/21/12   pl      Integrate with scripting device interface
06/20/12   shb     Updated GSM device APIs to accept script buffer
06/17/12   pl      update to use CA supported device interface
09/05/12   aro     Added RF-FW Sync callback parameter in HDR MDSP Interface
09/05/12   aro     Added RF-FW Sync callback parameter in MDSP Interface
05/14/12   sar     Removed rtr8600 device.
04/19/12   jfc     Remove unused RFD includes
04/18/12   zg      Fixed a GSM crash in TDS->GSM iRAT. 
04/03/12   aro     callback function for HDR MDSP Rx interfaces
04/02/12   aro     Added CB as argument in Rx / Tx MDSP interfaces
04/02/12   aro     Added CB as argument in Rx MDSP interfaces
03/17/12   jfc     Removed clkrgm_modem.h inclusion as it has been deprecated
02/16/12   zg/jyu  Added FTM MEAS support for TDS<->GSM, TDD LTE->TDS for both 
                   tune-away and tune-back (RF execute GRFC/SSBI script) 
02/15/12   sar     Mainlined RF_GRFC_COMMON_DESIGN_INTERIM_FEATURE.
02/03/12   av      IRAT cleanup phase 1
12/05/11   sar     Fixed featurization for non-lte build. 
11/17/11   aro     Enabled voting for CDMA FW State config and MCPM config
11/17/11   aro     Added new MCPM configure function with voting mechanism
11/17/11   aro     Replaced the FW state config function with new FTM func
11/16/11   swb     Remove additional deprecated clk_regime function calls
11/10/11   aro     Upadated FW state config interface to pass semaphore to 
                   wait on for blocking calls
10/21/11   swb     Remove deprecated clk_regime function calls
09/26/11   dw      Klocwork fixes
08/25/11   whc     Remove all references to legacy RFC structure and related API
08/18/11   Saul    IRAT updates/cleanup.
08/03/11   Saul    IRAT update. Now configuring flags so that FW writes scripts.
07/25/11   Saul    IRAT updates. 
                   - Now sending pilot_meas_cfg message.
                   - GRFC start delta sent in cx8 units.
                   - Override of debug flags done in FTM meas APIs.
07/23/11   Saul    Filled-in L->1x/do test functionality. 
07/19/11   whc     Replace IRAT GRFC mask APIs with GRFC script APIs
07/12/11   vb      rfc cleanup and rfc interface changes to suit the new auto-gen 
                   of rf card files
07/06/11   tnt     Call new function for GRFC mask act/deact
07/05/11   vrb     Replacing direct RTR function call with dev intf function
07/01/11   vrb     Replacing direct calls to rfgsm_ds with wrapper functions
05/10/11   sar     Remove hdrsrch.h for rfa/api cleanup.
04/20/11   pl      LTE FTM refactoring to remove global variables
04/06/11   pl      Fixed Compiler warnings 
04/05/11   tnt     Fix compiler error
04/05/11   dw      GtoW/LtoW RxlM support
04/04/11   bmg     Removed direct coupling with RTR8600
04/01/10   dw      Fix WCDMA band type mismatch in tune device API
04/01/11   dw      Fix warnings 
03/24/11   pl      replace clk configuration with MCPM
03/21/11   pl      fix warnings
03/24/11   vrb     Compiler Warning Fixes
03/24/11   tnt     L2W, L2DO Irat integration
03/17/11   tnt     Replace INO with mempool IQ on Nikel
03/12/11   zg      Wrapped hdrsrch.h with FEATURE_HDR.
03/11/11   zg      Compiler warning fix. 
03/07/11   jyu     Added Level 1&2 RF FTM IRAT test  
03/07/11   tnt     Integrate with new LM and RFC arch
03/03/11   pl      fix compiler error
03/01/11   zg      Cleaned up bbrx hack from ftm_meas_ltodo_test_do().
02/28/11   zg      Initial L->DO irat FTM support 
02/24/11   tnt     Merge to MDM9K PLF110
02/16/11   jyu     Added FTM L2L Interfreq IRAT test support 
02/10/11   jyu     Added LTE INO support to IRAT FTM
02/07/11   cri     Featurize RFC dependent definitions
02/07/11   av      FTM IRAT support. Corrected some issues and add support for returning 
                   script sizes. 
02/03/11   bmg     Removed hard-coded GRFC configuration that isn't modem
                   independent.
01/27/11   dw      Cleanup legacy WCDMA RFC interface call
01/25/11   pl      Featurize Genesis Specific HWIO code
01/25/11   pl      Replace deprecated clk.h with DALSys.h
01/05/11   pl      remove compiler warnings
01/03/11   pl      integrate with RFC LTE class
11/09/10   lcl     Remove compiler warning
10/20/10   jyu     Fix compile warning 
10/20/10   jyu     Update interface per FW change for rxagc_start_lna_state 
10/06/10   jyu     Added new APIs to support IRAT FTM test
10/01/10   kguo    Support integration with fw in FTM for L2W IRAT test
10/19/10   vb      All device functions are now accessed thru device interface functions
09/28/10   kguo    Cleaned up buf_index_cleanup and buf_index_startup in xtol 
                   and ltow test to be consistent with new data structure
09/22/10   tnt     Remove hardcoded GRFC setting, setting are readding from RFC files
09/20/10   tnt     Fix compiler error in ftm_meas_xtol_test_lte()
09/15/10   pl/jyu  W2L FTM FW integration with proper sequence
09/10/10   nsree   Included "ftm_gsm_rfprot.h" and "rfgsm_core_util.h" to avoid
           compiler warnings involved with IRAT. Added default enumeration case,
           and fixed incompatible pointer type warning.
09/14/10   av      LtoG support
08/26/10   kguo    Use feature FEATURE_LTOX_OPTIMIZED_IRAT for L2W test to avoid 
                   compiler error in HB/HC build 
08/24/10   kguo    Updated Ltow test 
                   Mainline feature FEATURE_LTOX_OPTIMIZED_IRAT 
08/23/10   pl      FW integration for IRAT xTOL testcase
08/18/10   pl      Added script-based IRAT functions
08/10/10   pl      Adding support for Stop scripts
08/10/10   pl      Remove non-ship header file header file
08/10/10   pl      Remove unused header file
08/09/10   kguo    Remove unused header file and updated ltoW test with new data structure
08/04/10   ka      Remove warnings 
07/29/10   kguo    Featured ltow test with FEATURE_LTOX_OPTIMIZED_IRAT. 
07/28/10   kguo    Added new test features for ltow test
07/27/10   pl      Remove warning
07/27/10   pl      Added test for x2DO IRAT
07/27/10   pl      Added test for x2L IRAT
06/29/10   pl      Use MDSP layer to abstract FW from RF IRAT module
06/22/10   kguo    Changed mwmory interface with L1 from pointer to index 
06/10/10   kguo    Added test for LtoW
06/08/10   lcl     Added G2W and W2G support.
03/24/10   ka      Restore wtow
02/25/10   ka      Temporarily remove wtow
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.
12/17/09   sr      removed the rtr8600_gsm.h include file.
11/25/09   ntn     code cleanup
12/02/09   ckl     Added W2W test function and removed warning.
10/26/09   ac      added path for tune_to_chan_fp
10/21/09   sr      new device driver interface changes.
09/15/09   jps     Initial version

=============================================================================*/
#include "ftmicap.h"

#ifdef FTM_HAS_IRAT
#include "rfm_mode_types.h"
#include "ftm_meas.h"
#include "rxlm_intf.h"
#include "rf_hal_sbi.h"
#include "rf_hal_rffe.h"
#include "rfm_meas.h"
#include "rfmeas_ftm_mdsp.h"
#include "mcpm_api.h"
#include "rflm_api_wcdma.h"
#include "rflm_time.h"
#include "rfwcdma_core_rxctl.h"
#include "rfm_wcdma.h"

#include "ftm_wcdma_ctl.h"
#include "rflm_dm_api.h"
extern void rfmeas_mc_wtow_build_script_ftm( rfm_meas_w2w_setup_and_build_scripts_param_type *w2w_meas_param );

/*============================================================================
             LOCAL/EXTERNAL VARIABLES                                                      
=============================================================================*/
#ifdef FEATURE_WCDMA
extern void rfwcdma_mc_meas_init(void);
#endif /* FEATURE_WCDMA */

typedef enum
{
  FTM_MEAS_LEVEL_RF_EXECUTE_SCRIPT = 1,
  FTM_MEAS_LEVEL_FW_EXECUTE_SCRIPT = 2,
  FTM_MEAS_LEVEL_RF_ABORT_SCRIPT   = 3,
  FTM_MEAS_LEVEL_MAX = 4,
  FTM_MEAS_LEVEL_INVALID = 0xFFFF
} ftm_meas_level_enum_type;

typedef struct
{
  ftm_meas_level_enum_type test_level;
  lm_handle_type           tgt_rxlm_ant0_idx;
  lm_handle_type           tgt_rxlm_ant1_idx;
  ftm_meas_func_tbl_type   *func_tbl_ptr[RFM_NUM_MODES];
} ftm_meas_state_type;
ftm_meas_state_type ftm_meas_state;

typedef struct
{
  uint16 num_trx;
  uint16 num_trx_task;
  uint16 num_non_trx;
  uint16 num_non_trx_task;
  uint16 execute_time_us;
} ftm_meas_irat_script_info_type;
ftm_meas_irat_script_info_type ftm_meas_irat_info[2];

/* RxLM handle for wcdma neighbour */
lm_handle_type ftm_wcdma_neighbour_rxlm_buffer;

extern void rfmeas_mc_wtow_build_script( rfm_meas_setup_param_type *meas_scripts_param );
void ftm_meas_clock_setup(rfm_meas_generic_params_type *test_param_ptr, boolean tune_away_flag);
void ftm_meas_allocate_lm_buffer(rfm_meas_generic_params_type *test_param_ptr);
void ftm_meas_deallocate_lm_buffer(rfm_meas_generic_params_type *test_param_ptr);
static  rfm_meas_w2w_setup_and_build_scripts_param_type test_w2w_script_param;
extern ftm_lm_buf_type ftm_wcdma_rxlm_buffer[];

boolean ftm_irat_tune = TRUE;

/*===========================================================================
             FUNCTION IMPLEMENTATIONS
===========================================================================*/

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function will test interfreq and interband for WCDMA to WCDMA.

   @param serv_cell rfcom_wcdma_band_type
   @param serv_chan wcdma channel
   @param meas_band rfcom_gsm_band_type
   @param meas_chan arfcn

*/
void ftm_meas_wtow_test_wcdma(uint16 home_buffer, uint16 serv_chan, uint16 nbr_buffer, uint16 meas_chan)
{
  
  MSG_4(MSG_SSID_FTM, MSG_LEGACY_HIGH,"ftm_meas_wtow_test_wcdma start: %d %d %d %d",home_buffer,serv_chan,nbr_buffer,meas_chan);

  rfwcdma_mc_meas_init();

  test_w2w_script_param.carrier_type = RFCOM_SINGLE_CARRIER;
  test_w2w_script_param.home_arfcn[0] = serv_chan;
  test_w2w_script_param.home_buffer_index = home_buffer;
  test_w2w_script_param.nbr_arfcn = meas_chan;
  test_w2w_script_param.nbr_buffer_index = nbr_buffer;
 
  rfmeas_mc_wtow_build_script_ftm(&test_w2w_script_param);
}

void ftm_meas_wtow_execute_buffer(uint16 buffer_id)
{
  uint16 arfcn[RFCOM_MAX_CARRIERS];
 

  MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"ftm_meas_wtow_execute_buffer start: %d",buffer_id);


  if( buffer_id == 0)
  {
    rfmeas_ftm_mdsp_execute_irat_script(RFM_MEAS_PROGRAM_CLEANUP, buffer_id);
    arfcn[0] = test_w2w_script_param.home_arfcn[0];
  }else
    {
    rfmeas_ftm_mdsp_execute_irat_script(RFM_MEAS_PROGRAM_STARTUP, buffer_id);
    arfcn[0] = test_w2w_script_param.nbr_arfcn ;
  }

  MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH,"ftm_meas_wtow_execute_buffer start  arfcn[0] = %d, %d ",  arfcn[0], 0);


/*
   rfm_update_rxlm_buffer( RFM_DEVICE_0,   
                                              arfcn,  
                                              RFCOM_SINGLE_CARRIER,  
                                              ftm_wcdma_rxlm_buffer[RF_PATH_0].buf_idx);


    rfwcdma_core_rxctl_enable_rxagc(RFWCDMA_CORE_AGC_CAR_0,
                                  RFWCDMA_CORE_AGC_CHAIN_0,
                                  RFWCDMA_MDSP_AGC_CMD_DISABLE,
                                  ftm_wcdma_rxlm_buffer[RF_PATH_0].buf_idx,
                                  0,
                                  FALSE );

  rfwcdma_core_rxctl_enable_rxagc(RFWCDMA_CORE_AGC_CAR_0,
                                  RFWCDMA_CORE_AGC_CHAIN_0,
                                  RFWCDMA_MDSP_AGC_CMD_ENABLE,
                                  ftm_wcdma_rxlm_buffer[RF_PATH_0].buf_idx,
                                  0,
                                  FALSE );
  rf_buffer_destroy( rfmeas_mc_wtow_script_buffer_ptr[buffer_id] );*/

}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function will build scripts during interfreq and interband for WCDMA to WCDMA.

   @param serv_chan wcdma channel
   @param meas_chan arfcn

*/
void ftm_meas_wtow_build_scripts(uint16 serv_chan, uint16 meas_chan)
{
  rfm_meas_setup_param_type test_meas_param;
  lm_tech_type tech = LM_UMTS;
  lm_status_type status;
  ftm_receive_chain_handle_type chain = FTM_RECEIVE_CHAIN_0;

  rfwcdma_mc_meas_init();

  /* Check if RxLM buffer is allocated for Home cell. If not create one */
  if( ftm_wcdma_rxlm_buffer[RF_PATH_0].is_buf_allocated == FALSE )
  {
    ftm_wcdma_allocate_rxlm(chain);
  }

  /*   Also create RxLM buffer for Neighbour cell */
  status = rxlm_allocate_buffer((rxlm_chain_type)chain, tech,
                                  &ftm_wcdma_neighbour_rxlm_buffer);
  if (status != LM_SUCCESS )
  {
     MSG_ERROR("RxLM allocate buffer for neighbour failed, status:%d",status,0,0);
     return;
  }
  else
  {
    MSG_HIGH("RXLM buffer allocated for neighbour chain:%d, buffer index:%d",
             chain, ftm_wcdma_neighbour_rxlm_buffer, 0);
  }
  /* RxLM buffer for neighbour allocated */

  MSG_4(MSG_SSID_FTM, MSG_LEGACY_HIGH,"ftm_meas_wtow_build_scripts start: %d %d %d %d",ftm_wcdma_rxlm_buffer[chain].buf_idx,serv_chan,ftm_wcdma_neighbour_rxlm_buffer,meas_chan);

  test_w2w_script_param.device = RFM_DEVICE_0;
  test_w2w_script_param.carrier_type = RFCOM_SINGLE_CARRIER;
  test_w2w_script_param.home_arfcn[0] = serv_chan;
  test_w2w_script_param.home_buffer_index = ftm_wcdma_rxlm_buffer[chain].buf_idx;
  test_w2w_script_param.nbr_arfcn = meas_chan;
  test_w2w_script_param.nbr_buffer_index = ftm_wcdma_neighbour_rxlm_buffer;

  test_meas_param.header.num_source_item = 1; /* Only single band support, no DBDC*/
  test_meas_param.header.source_tech = RFCOM_WCDMA_MODE;
  test_meas_param.header.target_tech = RFCOM_WCDMA_MODE;

  /* Populate Source Params of Header */
  test_meas_param.header.source_param[0].band = rfwcdma_core_convert_chan_to_band(serv_chan);
  test_meas_param.header.source_param[0].device = RFM_DEVICE_0;
  test_meas_param.header.source_param[0].div_device = RFM_DEVICE_1;
  test_meas_param.header.source_param[0].div_en = FALSE;

  /* Populate Target Params of Header */
  test_meas_param.header.target_param.band = rfwcdma_core_convert_chan_to_band(meas_chan);
  test_meas_param.header.target_param.device = RFM_DEVICE_0;
  test_meas_param.header.target_param.div_device = RFM_DEVICE_1;
  test_meas_param.header.target_param.div_en = FALSE;

  test_meas_param.target_param = (void*)(&test_w2w_script_param);

  test_meas_param.source_param = (void*)(&test_w2w_script_param);

  /* Update RxLM buffer for Neighbour before building scripts */
  rfm_update_rxlm_buffer( test_w2w_script_param.device, 
                          &(test_w2w_script_param.nbr_arfcn),
                          test_w2w_script_param.carrier_type,
                          (uint32)test_w2w_script_param.nbr_buffer_index);
 
  rfmeas_mc_wtow_build_script(&test_meas_param);

}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function will test interfreq and interband for WCDMA to WCDMA
   by writing preload and trigger scripts for either home or neighbour

   @param tune_neighbour Tune neighbour or home

*/
void ftm_meas_wtow_write_scripts(boolean tune_neighbour)
{
  rflm_wcdma_rf_send_script_in_t  in_preload = {0,FALSE,RF_WCDMA_EVENT_RX_MEAS_PRELOAD};
  rflm_wcdma_rf_send_script_in_t  in_trigger = {0,FALSE,RF_WCDMA_EVENT_RX_MEAS_TRIGGER};
  rflm_wcdma_rf_send_script_out_t out_preload, out_trigger;
  rflm_err_t ret_err = RFLM_ERR_NONE;
  rfwcdma_mdsp_agc_action_type cmd_action[RFM_DEVICE_4] = 
  {
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID,
    RFWCDMA_MDSP_AGC_CMD_INVALID
  };
  uint32 rxlm_buf_idx[RFM_DEVICE_4] = {0};

  MSG(MSG_SSID_FTM, MSG_LEGACY_HIGH,"ftm_meas_wtow_write_scripts start");

  in_preload.effect_time = rflm_time_read_ustmr_count();

  /* Add about 1 ms (19200) between Preload and trigger write time */
  in_trigger.effect_time = rflm_time_read_ustmr_count() + 19200;

  if(tune_neighbour)
  {

    MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"ftm_meas_wtow_write_scripts: Neighbour with handle %d:", ftm_wcdma_neighbour_rxlm_buffer);

    /* Push DM buffer to CCS for Preload event */
    ret_err |= rflm_wcdma_rf_send_script( ftm_wcdma_neighbour_rxlm_buffer,
                                          &in_preload,
                                          &out_preload );

    /* Push DM buffer to CCS for Trigger/Non Trx event */
    ret_err |= rflm_wcdma_rf_send_script( ftm_wcdma_neighbour_rxlm_buffer,
                                          &in_trigger,
                                          &out_trigger );

    cmd_action[RFM_DEVICE_0] = RFWCDMA_MDSP_AGC_CMD_DISABLE;
    rxlm_buf_idx[RFM_DEVICE_0] =  ftm_wcdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx;

    /* Stop RxAGC on home and start on Neighbour */
    rfwcdma_core_rxctl_enable_rxagc(RFCOM_MULTI_CARRIER_ID__1,
                                    cmd_action,
                                    rxlm_buf_idx,
                                    FALSE,0,0);

    cmd_action[RFM_DEVICE_0] = RFWCDMA_MDSP_AGC_CMD_ENABLE;
    rxlm_buf_idx[RFM_DEVICE_0] =  ftm_wcdma_neighbour_rxlm_buffer;

    rfwcdma_core_rxctl_enable_rxagc(RFCOM_MULTI_CARRIER_ID__1,
                                    cmd_action,
                                    rxlm_buf_idx,
                                    FALSE,0,0);
  }

  else
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"ftm_meas_wtow_write_scripts: Home with handle %d:", ftm_wcdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx);

    /* Push DM buffer to CCS for Preload event */
    ret_err |= rflm_wcdma_rf_send_script( ftm_wcdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx,
                                          &in_preload,
                                          &out_preload );
    /* No Preload for Home */
    /* Push DM buffer to CCS for Trigger/Non Trx event */
    ret_err |= rflm_wcdma_rf_send_script( ftm_wcdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx,
                                          &in_trigger,
                                          &out_trigger ); 


    cmd_action[RFM_DEVICE_0] = RFWCDMA_MDSP_AGC_CMD_DISABLE;
    rxlm_buf_idx[RFM_DEVICE_0] = ftm_wcdma_neighbour_rxlm_buffer;

     /* Stop RxAGC on Neighbour and start on Home */
    rfwcdma_core_rxctl_enable_rxagc(RFCOM_MULTI_CARRIER_ID__1,
                                    cmd_action,
                                    rxlm_buf_idx,
                                    FALSE,0,0);


    cmd_action[RFM_DEVICE_0] = RFWCDMA_MDSP_AGC_CMD_ENABLE;
    rxlm_buf_idx[RFM_DEVICE_0] = ftm_wcdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx;

    rfwcdma_core_rxctl_enable_rxagc(RFCOM_MULTI_CARRIER_ID__1,
                                    cmd_action,
                                    rxlm_buf_idx,
                                    FALSE,0,0);

  }

  if(ret_err != RFLM_ERR_NONE)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"ftm_meas_wtow_execute_buffer: RF Send script failed with error %d", ret_err);
  }
}


/*===========================================================================

  FUNCTION: ftm_meas_clock_setup

===========================================================================*/
/*!
    @brief
    This function is called to setup MCPM clock and power mgmt state for 
    IRAT measurement

    @return
    None.
*/
/*=========================================================================*/
void ftm_meas_clock_setup(rfm_meas_generic_params_type *test_param_ptr, boolean tune_away_flag)
{
  boolean update_ok_flag = FALSE;
  mcpm_request_type mcpm_req = MCPM_1X_PARMS_UPDATE_REQ;
  mcpm_request_parms_type mcpm_param;

  if(test_param_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "Null input pointer");
    return;
  }

  /* check which srouce tech we need to update */
  switch(test_param_ptr->src_tech)
  {
     case RFCOM_1X_MODE:
       mcpm_req = MCPM_1X_PARMS_UPDATE_REQ;
       break;
     case RFCOM_1XEVDO_MODE:
       mcpm_req = MCPM_DO_PARMS_UPDATE_REQ;
       break;
     case RFCOM_GSM_MODE:
       mcpm_req = MCPM_GERAN_PARMS_UPDATE_REQ;
       break;
     case RFCOM_WCDMA_MODE:
       mcpm_req = MCPM_WCDMA_PARMS_UPDATE_REQ;
       break;
     case RFCOM_LTE_MODE:
       mcpm_req = MCPM_LTE_PARMS_UPDATE_REQ;
       break;
     case RFCOM_TDSCDMA_MODE:
       mcpm_req = MCPM_TDSCDMA_PARMS_UPDATE_REQ;
       break;
     default:
       MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Unknow src_tech %d",test_param_ptr->src_tech);
       return;
  }

  /* Now check which neighbor we are searching for */
  mcpm_param.update_info = MCPM_NEIGHBOR_MEAS_UPDATE;
  switch(mcpm_req)
  {
     case MCPM_1X_PARMS_UPDATE_REQ:
       if(test_param_ptr->tgt_tech == RFCOM_LTE_MODE)
       {
         mcpm_param.neighbor_meas = (tune_away_flag == TRUE)? MCPM_1X2L_MEAS_START : MCPM_1X2L_MEAS_STOP;
         update_ok_flag = TRUE;
       }
       else
       {
         MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
               "Unsupport IRAT pair, src_tech %d tgt_tech %d",
               test_param_ptr->src_tech, test_param_ptr->tgt_tech);
         // do nothing
       }
       break;

     case MCPM_DO_PARMS_UPDATE_REQ:
       if(test_param_ptr->tgt_tech == RFCOM_LTE_MODE)
       {
         mcpm_param.neighbor_meas = (tune_away_flag == TRUE)? MCPM_DO2L_MEAS_START : MCPM_DO2L_MEAS_STOP;
         update_ok_flag = TRUE;
       }
       else
       {
         MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
               "Unsupport IRAT pair, src_tech %d tgt_tech %d",
               test_param_ptr->src_tech, test_param_ptr->tgt_tech);
         // do nothing
       }
       break;

     case MCPM_GERAN_PARMS_UPDATE_REQ:
       if(test_param_ptr->tgt_tech == RFCOM_LTE_MODE)
       {
         mcpm_param.neighbor_meas = (tune_away_flag == TRUE)? MCPM_G2L_MEAS_START : MCPM_G2L_MEAS_STOP;
         update_ok_flag = TRUE;
       }
       else if (test_param_ptr->tgt_tech == RFCOM_WCDMA_MODE)
       {
         mcpm_param.neighbor_meas = (tune_away_flag == TRUE)? MCPM_G2W_MEAS_START : MCPM_G2W_MEAS_STOP;
         update_ok_flag = TRUE;
       }
       else if (test_param_ptr->tgt_tech == RFCOM_TDSCDMA_MODE)
       {
         mcpm_param.neighbor_meas = (tune_away_flag == TRUE)? MCPM_G2TDS_MEAS_START : MCPM_G2TDS_MEAS_STOP;
         update_ok_flag = TRUE;
       }
       else
       {
         MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
               "Unsupport IRAT pair, src_tech %d tgt_tech %d",
               test_param_ptr->src_tech, test_param_ptr->tgt_tech);
         // do nothing
       }
       break;

     case MCPM_WCDMA_PARMS_UPDATE_REQ:
       if(test_param_ptr->tgt_tech == RFCOM_LTE_MODE)
       {
         mcpm_param.neighbor_meas = (tune_away_flag == TRUE)? MCPM_W2L_MEAS_START : MCPM_W2L_MEAS_STOP;
         update_ok_flag = TRUE;
       }
       else if (test_param_ptr->tgt_tech == RFCOM_GSM_MODE)
       {
         mcpm_param.neighbor_meas = (tune_away_flag == TRUE)? MCPM_W2G_MEAS_START : MCPM_W2G_MEAS_STOP;
         update_ok_flag = TRUE;
       }
       else if (test_param_ptr->tgt_tech == RFCOM_WCDMA_MODE)
       {
         // Nothing to do if it is W2W
       }
       else
       {
         MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
               "Unsupport IRAT pair, src_tech %d tgt_tech %d",
               test_param_ptr->src_tech, test_param_ptr->tgt_tech);
         // do nothing
       }
       break;

     case MCPM_LTE_PARMS_UPDATE_REQ:
       if(test_param_ptr->tgt_tech == RFCOM_LTE_MODE)
       {
         // Nothing to do if it is L2L
       }
       else if (test_param_ptr->tgt_tech == RFCOM_GSM_MODE)
       {
         mcpm_param.neighbor_meas = (tune_away_flag == TRUE)? MCPM_L2G_MEAS_START : MCPM_L2G_MEAS_STOP;
         update_ok_flag = TRUE;
       }
       else if (test_param_ptr->tgt_tech == RFCOM_WCDMA_MODE)
       {
         mcpm_param.neighbor_meas = (tune_away_flag == TRUE)? MCPM_L2W_MEAS_START : MCPM_L2W_MEAS_STOP;
         update_ok_flag = TRUE;
       }
       else if((test_param_ptr->tgt_tech == RFCOM_1XEVDO_MODE) || 
               (test_param_ptr->tgt_tech == RFCOM_1X_MODE))
       {
         mcpm_param.neighbor_meas = (tune_away_flag == TRUE)? MCPM_L21XDO_MEAS_START : MCPM_L21XDO_MEAS_STOP;
         update_ok_flag = TRUE;
       }
       else
       {
         MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
               "Unsupport IRAT pair, src_tech %d tgt_tech %d",
               test_param_ptr->src_tech, test_param_ptr->tgt_tech);
         // do nothing
       }
       break;

     case MCPM_TDSCDMA_PARMS_UPDATE_REQ:
       if(test_param_ptr->tgt_tech == RFCOM_GSM_MODE)
       {
         mcpm_param.neighbor_meas = (tune_away_flag == TRUE)? MCPM_TDSCDMA2G_MEAS_START : MCPM_TDSCDMA2G_MEAS_STOP;
         update_ok_flag = TRUE;
       }
       else if (test_param_ptr->tgt_tech == RFCOM_LTE_MODE)
       {
         mcpm_param.neighbor_meas = (tune_away_flag == TRUE)? MCPM_TDSCDMA2L_MEAS_START : MCPM_TDSCDMA2L_MEAS_STOP;
         update_ok_flag = TRUE;
       }
       else
       {
         MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
               "Unsupport IRAT pair, src_tech %d tgt_tech %d",
               test_param_ptr->src_tech, test_param_ptr->tgt_tech);
         // do nothing
       }
       break;

     default:
       MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Unknown mcpm update %d", mcpm_req);
       break;
  }

  if(update_ok_flag == TRUE)
  {
    MCPM_Config_Modem(mcpm_req, &mcpm_param);
  }
}

/*===========================================================================

  FUNCTION: ftm_meas_allocate_lm_buffer

===========================================================================*/
/*!
    @brief
    This function is called to allocate LM buffer for IRAT testing in FTM mode.
    It should be called at the very beginning of IRAT testing.

    @return
    None.
*/
/*=========================================================================*/
void ftm_meas_allocate_lm_buffer(rfm_meas_generic_params_type *test_param_ptr)
{
  lm_status_type lm_status = LM_SUCCESS;
  lm_tech_type   lm_tech = LM_TECH_MAX;
  rflm_dm_handle_meta_s handle_meta;
  boolean dm_status = TRUE;

  if (test_param_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_FATAL,"NULL pointer in ftm_meas_allocate_lm_buffer()");
    return ;
  }

  if(test_param_ptr->tgt_tech >= RFM_NUM_MODES)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"Invalid target tech %d", test_param_ptr->tgt_tech);
    return ;
  }

  /* Set Target RXLM buffer first */
  switch(test_param_ptr->tgt_tech)
  {
    case RFCOM_1X_MODE:
      lm_tech = LM_1X;
      break;
    case RFCOM_GSM_MODE:
      lm_tech = LM_GSM;
      break;
    case RFCOM_1XEVDO_MODE:
      lm_tech = LM_HDR;
      break;
    case RFCOM_LTE_MODE:
      lm_tech = LM_LTE;
      break;
    case RFCOM_TDSCDMA_MODE:
      lm_tech = LM_TDSCDMA;
      break;
  case RFCOM_WCDMA_MODE:
      lm_tech = LM_UMTS;
      break;
    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"Target Mode %d FTM IRAT doesn't support", 
            test_param_ptr->tgt_tech );
      break;
  }
  if(lm_tech < LM_TECH_MAX)
  {
    if (ftm_meas_state.tgt_rxlm_ant0_idx < RXLM_MAX_BUFFERS) 
    {
      /* Already allocated, no need to allocate again */
      lm_status = LM_SUCCESS;
      test_param_ptr->tgt_rxlm_buf_idx_ant0 = ftm_meas_state.tgt_rxlm_ant0_idx;
    }
    else
    {
      lm_status = rxlm_allocate_buffer(RXLM_CHAIN_0,lm_tech,&(test_param_ptr->tgt_rxlm_buf_idx_ant0));
    }
    if (lm_status != LM_SUCCESS)
    {
      MSG_2( MSG_SSID_RF, MSG_LEGACY_FATAL,"Cannot allocate tech %d tune-away RXLM buffer for Ant- status %d",lm_tech,lm_status);
    }
    else
    {
      ftm_meas_state.tgt_rxlm_ant0_idx = test_param_ptr->tgt_rxlm_buf_idx_ant0;
      dm_status = rflm_dm_get_handle_info(test_param_ptr->tgt_rxlm_buf_idx_ant0, &handle_meta);
      if (dm_status == TRUE)
      {
//        handle_meta.type = RFLM_IRAT_HANDLE;
        handle_meta.device_id = RFM_DEVICE_0;
        (void)rflm_dm_set_handle_info(test_param_ptr->tgt_rxlm_buf_idx_ant0, &handle_meta);
      }
    }
    if (test_param_ptr->tgt_div_en == 1)
    {
      if (ftm_meas_state.tgt_rxlm_ant1_idx < RXLM_MAX_BUFFERS) 
      {
        /* Already allocated, no need to allocate again */
        lm_status = LM_SUCCESS;
        test_param_ptr->tgt_rxlm_buf_idx_ant1 = ftm_meas_state.tgt_rxlm_ant1_idx;
      }
      else
      {
        lm_status = rxlm_allocate_buffer(RXLM_CHAIN_1,lm_tech,&(test_param_ptr->tgt_rxlm_buf_idx_ant1));
      }
      if (lm_status != LM_SUCCESS)
      {
        MSG_2( MSG_SSID_RF, MSG_LEGACY_FATAL,"Cannot allocate tech %d tune-away RXLM buffer for Ant-1 status %d",lm_tech,lm_status);
      }
      else
      {
        ftm_meas_state.tgt_rxlm_ant1_idx = test_param_ptr->tgt_rxlm_buf_idx_ant1;
        dm_status = rflm_dm_get_handle_info(test_param_ptr->tgt_rxlm_buf_idx_ant1, &handle_meta);
        if (dm_status == TRUE)
        {
//          handle_meta.type = RFLM_IRAT_HANDLE;
          handle_meta.device_id = RFM_DEVICE_1;
          (void)rflm_dm_set_handle_info(test_param_ptr->tgt_rxlm_buf_idx_ant1, &handle_meta);
        }
      }
    }
    else
    {
      test_param_ptr->tgt_rxlm_buf_idx_ant1 = RXLM_MAX_BUFFERS;
    }
  }
}



/*===========================================================================

  FUNCTION: ftm_meas_deallocate_lm_buffer

===========================================================================*/
/*!
    @brief
    This function is called to allocate LM buffer for IRAT testing in FTM mode.
    It should be called at the very beginning of IRAT testing.

    @return
    None.
*/
/*=========================================================================*/
void ftm_meas_deallocate_lm_buffer(rfm_meas_generic_params_type *test_param_ptr)
{
  lm_status_type lm_status;
  lm_tech_type   lm_tech = LM_TECH_MAX;

  if (test_param_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_FATAL,"NULL pointer in ftm_meas_allocate_lm_buffer()");
    return ;
  }

  if(test_param_ptr->tgt_tech >= RFM_NUM_MODES)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"Invalid target tech %d", test_param_ptr->tgt_tech);
    return ;
  }
  /* Set Target RXLM buffer first */
  switch(test_param_ptr->tgt_tech)
  {
    case RFCOM_1X_MODE:
      lm_tech = LM_1X;
      break;
    case RFCOM_GSM_MODE:
      lm_tech = LM_GSM;
      break;
    case RFCOM_1XEVDO_MODE:
      lm_tech = LM_HDR;
      break;
    case RFCOM_WCDMA_MODE:
      break;
    case RFCOM_LTE_MODE:
      lm_tech = LM_LTE;
      break;
    case RFCOM_TDSCDMA_MODE:
      lm_tech = LM_TDSCDMA;
      break;
    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"Target Mode %d FTM IRAT doesn't support", 
            test_param_ptr->tgt_tech );
      break;
  }
  if(lm_tech < LM_TECH_MAX)
  {
    lm_status = rxlm_deallocate_buffer(test_param_ptr->tgt_rxlm_buf_idx_ant0);
    if (lm_status != LM_SUCCESS)
    {
      MSG_2( MSG_SSID_RF, MSG_LEGACY_FATAL,"Cannot deallocate tech %d tune-away RXLM buffer for Ant-0 status %d",lm_tech,lm_status);
    }
    else
    {
      test_param_ptr->tgt_rxlm_buf_idx_ant0 = RXLM_MAX_BUFFERS;
      ftm_meas_state.tgt_rxlm_ant0_idx = RXLM_MAX_BUFFERS;
    }
    if (test_param_ptr->tgt_div_en == 1)
    {
      lm_status = rxlm_deallocate_buffer(test_param_ptr->tgt_rxlm_buf_idx_ant1);
      if (lm_status != LM_SUCCESS)
      {
        MSG_2( MSG_SSID_RF, MSG_LEGACY_FATAL,"Cannot deallocate tech %d tune-away RXLM buffer for Ant-1 status %d",lm_tech,lm_status);
      }
      else
      {
        test_param_ptr->tgt_rxlm_buf_idx_ant1 = RXLM_MAX_BUFFERS;
        ftm_meas_state.tgt_rxlm_ant1_idx = RXLM_MAX_BUFFERS;
      }
    }
    else
    {
      test_param_ptr->tgt_rxlm_buf_idx_ant1 = RXLM_MAX_BUFFERS;
    }
  }
}

/*===========================================================================

  FUNCTION: ftm_meas_xtoy_test_y

===========================================================================*/
/*!
    @brief
    This function acts the dispatcher for IRAT FTM tests. According to the input
    source/target technology, the corrsponding IRAT FTM API for that combination
    will be called. 

    @return
    None.
*/
/*=========================================================================*/
void ftm_meas_xtoy_test_y(rfm_meas_generic_params_type *test_param_ptr)
{
  ftm_meas_func_tbl_type *target_func_tbl_ptr = NULL;
  ftm_meas_func_tbl_type *source_func_tbl_ptr = NULL;
  boolean status = FALSE;
  rfm_meas_enter_param_type enter_param;
  rfm_meas_setup_param_type setup_param;
  rfm_meas_exit_param_type exit_param;

  /* Input parameter checking */
  if (test_param_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_FATAL,"NULL pointer in ftm_meas_xtoy_test_y()");
    return ;
  }

  if( (test_param_ptr->src_tech >= RFM_NUM_MODES) ||
      (test_param_ptr->tgt_tech >= RFM_NUM_MODES))
  {
    MSG_3( MSG_SSID_RF, MSG_LEGACY_HIGH, 
           "Invalid ftm_meas_xtoy_test_y for source tech = %d and target tech =%d",
           test_param_ptr->test_level, test_param_ptr->src_tech, test_param_ptr->tgt_tech);
    return;
  }

  /* Check for module registration */
  if((ftm_meas_state.func_tbl_ptr[test_param_ptr->tgt_tech] == NULL) ||
     (ftm_meas_state.func_tbl_ptr[test_param_ptr->src_tech] == NULL) )
  {
    MSG_4( MSG_SSID_RF, MSG_LEGACY_HIGH, 
           "Function table no registered for src_tech %d (tbl 0x%X) or target_tech %d (tbl 0x%X)",
           test_param_ptr->src_tech, ftm_meas_state.func_tbl_ptr[test_param_ptr->src_tech],
           test_param_ptr->tgt_tech, ftm_meas_state.func_tbl_ptr[test_param_ptr->tgt_tech]);
    return;
  }
  target_func_tbl_ptr = ftm_meas_state.func_tbl_ptr[test_param_ptr->tgt_tech];
  source_func_tbl_ptr = ftm_meas_state.func_tbl_ptr[test_param_ptr->src_tech];

  /* Determine Test mode */
  if ((test_param_ptr->test_level < FTM_MEAS_LEVEL_MAX) || (test_param_ptr->test_level > 0))
  {
    // Level I test: RF exercises scripts
    ftm_meas_state.test_level = test_param_ptr->test_level;
  }
  else
  {  
    MSG_3( MSG_SSID_RF, MSG_LEGACY_HIGH, 
           "Level %d not supported in this command ftm_meas_xtoy_test_y for source tech = %d and target tech =%d",
           test_param_ptr->test_level, test_param_ptr->src_tech, test_param_ptr->tgt_tech);
    return;
  }

  /* Allocate RXLM buffer for Target Tech since RXLM buffer is not allocated yet */
  ftm_meas_allocate_lm_buffer(test_param_ptr);
  if(source_func_tbl_ptr->get_src_rxlm_buffer_fp!= NULL)
  {
    status = source_func_tbl_ptr->get_src_rxlm_buffer_fp(test_param_ptr);
    if(status == FALSE)
    {
      MSG_1(MSG_SSID_RF,MSG_LEGACY_ERROR,
          "Failed getting source RXLM buffer for Tech %d",test_param_ptr->src_tech);
      return;
    }
  }

  /* Setup Clocks */
  ftm_meas_clock_setup(test_param_ptr, TRUE);

  /* Enter Meas */
  enter_param.header.source_tech = test_param_ptr->src_tech;
  enter_param.header.source_param[0].device = RFM_DEVICE_0;
  enter_param.header.source_param[0].div_device = RFM_DEVICE_1;
  enter_param.header.source_param[0].band = test_param_ptr->src_band;
  enter_param.header.source_param[0].div_en = test_param_ptr->src_div_en;
  if (test_param_ptr->src_extra_carrier_flag == TRUE) 
  {
    enter_param.header.source_param[1].device = RFM_DEVICE_2;
    enter_param.header.source_param[1].div_device = RFM_DEVICE_3;
    enter_param.header.source_param[1].band = test_param_ptr->src_band_carrier_1;
    enter_param.header.source_param[1].div_en = test_param_ptr->src_div_en_carrier_1;
    enter_param.header.num_source_item = 2;
  }
  else
  {
    enter_param.header.num_source_item = 1;
  }
  enter_param.header.target_tech = test_param_ptr->tgt_tech;
  enter_param.header.target_param.device = RFM_DEVICE_0;
  enter_param.header.target_param.div_device = RFM_DEVICE_1;
  enter_param.header.target_param.band = test_param_ptr->tgt_band;
  enter_param.header.target_param.div_en = test_param_ptr->tgt_div_en;
  enter_param.source_param = NULL;
  enter_param.target_param = NULL;
  if(source_func_tbl_ptr->get_enter_param_fp!=NULL)
  {
    enter_param.source_param = source_func_tbl_ptr->get_enter_param_fp(test_param_ptr);
  }
  if(target_func_tbl_ptr->get_enter_param_fp!=NULL)
  {
    enter_param.target_param = target_func_tbl_ptr->get_enter_param_fp(test_param_ptr);
  }
  if(target_func_tbl_ptr->enter_meas_fp != NULL)
  {
    status = target_func_tbl_ptr->enter_meas_fp(&enter_param);
    if(status == FALSE)
    {
      MSG_1(MSG_SSID_RF,MSG_LEGACY_ERROR,
          "Failed ENTER_MEAS for Tech %d",test_param_ptr->tgt_tech);
      return;
    }
  }

  /* Build script */
  setup_param.header.source_tech = test_param_ptr->src_tech;
  setup_param.header.source_param[0].device = RFM_DEVICE_0;
  setup_param.header.source_param[0].div_device = RFM_DEVICE_1;
  setup_param.header.source_param[0].band = test_param_ptr->src_band;
  setup_param.header.source_param[0].div_en = test_param_ptr->src_div_en;
  if (test_param_ptr->src_extra_carrier_flag == TRUE) 
  {
    setup_param.header.source_param[1].device = RFM_DEVICE_2;
    setup_param.header.source_param[1].div_device = RFM_DEVICE_3;
    setup_param.header.source_param[1].band = test_param_ptr->src_band_carrier_1;
    setup_param.header.source_param[1].div_en = test_param_ptr->src_div_en_carrier_1;
    setup_param.header.num_source_item = 2;
  }
  else
  {
    setup_param.header.num_source_item = 1;
  }
  setup_param.header.target_tech = test_param_ptr->tgt_tech;
  setup_param.header.target_param.device = RFM_DEVICE_0;
  setup_param.header.target_param.div_device = RFM_DEVICE_1;
  setup_param.header.target_param.band = test_param_ptr->tgt_band;
  setup_param.header.target_param.div_en = test_param_ptr->tgt_div_en;
  setup_param.source_param = NULL;
  setup_param.target_param = NULL;
  if(source_func_tbl_ptr->get_setup_param_fp != NULL)
  {
    setup_param.source_param = source_func_tbl_ptr->get_setup_param_fp(test_param_ptr);
  }
  if(target_func_tbl_ptr->get_setup_param_fp != NULL)
  {
    setup_param.target_param = target_func_tbl_ptr->get_setup_param_fp(test_param_ptr);
  }
  if(target_func_tbl_ptr->build_script_fp != NULL)
  {
    status = target_func_tbl_ptr->build_script_fp(&setup_param);
    if(status == FALSE)
    {
      MSG_1(MSG_SSID_RF,MSG_LEGACY_ERROR,
          "Failed BUILD_SCRIPT for Tech %d",test_param_ptr->tgt_tech);
      return;
    }
  }

  if (ftm_irat_tune == TRUE)
  {
    /* If abort is called */
    if (test_param_ptr->test_level == FTM_MEAS_TEST_LEVEL_RF_ABORT)
    {
      exit_param.header.source_tech = test_param_ptr->src_tech;
      exit_param.header.source_param[0].device = RFM_DEVICE_0;
      exit_param.header.source_param[0].div_device = RFM_DEVICE_1;
      exit_param.header.source_param[0].band = test_param_ptr->src_band;
      exit_param.header.source_param[0].div_en = test_param_ptr->src_div_en;
      if (test_param_ptr->src_extra_carrier_flag == TRUE) 
      {
        exit_param.header.source_param[1].device = RFM_DEVICE_2;
        exit_param.header.source_param[1].div_device = RFM_DEVICE_3;
        exit_param.header.source_param[1].band = test_param_ptr->src_band_carrier_1;
        exit_param.header.source_param[1].div_en = test_param_ptr->src_div_en_carrier_1;
        exit_param.header.num_source_item = 2;
      }
      else
      {
        exit_param.header.num_source_item = 1;
      }
      exit_param.header.target_tech = test_param_ptr->tgt_tech;
      exit_param.header.target_param.device = RFM_DEVICE_0;
      exit_param.header.target_param.div_device = RFM_DEVICE_1;
      exit_param.header.target_param.band = test_param_ptr->tgt_band;
      exit_param.header.target_param.div_en = test_param_ptr->tgt_div_en;
      exit_param.is_measurement_gap_aborted = TRUE;
      exit_param.source_param = NULL;
      exit_param.target_param = NULL;
      if(source_func_tbl_ptr->get_exit_param_fp != NULL)
      {
        exit_param.source_param = source_func_tbl_ptr->get_exit_param_fp(test_param_ptr);
      }
      if(target_func_tbl_ptr->get_exit_param_fp != NULL)
      {
        exit_param.target_param = target_func_tbl_ptr->get_exit_param_fp(test_param_ptr);
      }
      if (target_func_tbl_ptr->exit_meas_fp != NULL)
      {
        status = target_func_tbl_ptr->exit_meas_fp(&exit_param);
      }
    }
    else /* No Abort so continue with tune away */
    {
      /* Exit Source tech */
      if (source_func_tbl_ptr->meas_exit_tech_fp != NULL ) 
      {
        status = source_func_tbl_ptr->meas_exit_tech_fp(test_param_ptr, TRUE);
        if(status == FALSE)
        {
          MSG_1(MSG_SSID_RF,MSG_LEGACY_ERROR,
              "Failed Exiting Tech %d in IRAT Tune-Away",test_param_ptr->src_tech);
          return;
        }
      }

      /* Enter Target Tech */
      if (target_func_tbl_ptr->meas_enter_tech_fp != NULL ) 
      {
        status = target_func_tbl_ptr->meas_enter_tech_fp(test_param_ptr, FALSE);
        if(status == FALSE)
        {
          MSG_1(MSG_SSID_RF,MSG_LEGACY_ERROR,
              "Failed Entering Tech %d in IRAT Tune-Away",test_param_ptr->tgt_tech);
          return;
        }
      }
    }
  }
}

/*===========================================================================

  FUNCTION: ftm_meas_xtoy_test_x

===========================================================================*/
/*!
    @brief
    This function acts the dispatcher for IRAT FTM tests. According to the input
    source/target technology, the corrsponding IRAT FTM API for that combination
    will be called. 

    @return
    None.
*/
/*=========================================================================*/
void ftm_meas_xtoy_test_x(rfm_meas_generic_params_type *test_param_ptr)
{
  ftm_meas_func_tbl_type *target_func_tbl_ptr = NULL;
  ftm_meas_func_tbl_type *source_func_tbl_ptr = NULL;
  boolean status = FALSE;
  rfm_meas_exit_param_type exit_param;

  /* Input parameter checking */
  if (test_param_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_FATAL,"NULL pointer in ftm_meas_xtoy_test_x()");
    return ;
  }

  if( (test_param_ptr->src_tech >= RFM_NUM_MODES) ||
      (test_param_ptr->tgt_tech >= RFM_NUM_MODES))
  {
    MSG_3( MSG_SSID_RF, MSG_LEGACY_HIGH, 
           "Invalid ftm_meas_xtoy_test_x for source tech = %d and target tech =%d",
           test_param_ptr->test_level, test_param_ptr->src_tech, test_param_ptr->tgt_tech);
    return;
  }

  /* Check for module registration */
  if((ftm_meas_state.func_tbl_ptr[test_param_ptr->tgt_tech] == NULL) ||
     (ftm_meas_state.func_tbl_ptr[test_param_ptr->src_tech] == NULL) )
  {
    MSG_4( MSG_SSID_RF, MSG_LEGACY_HIGH, 
           "Function table not registered for src_tech %d (tbl 0x%X) or target_tech %d (tbl 0x%X)",
           test_param_ptr->src_tech, ftm_meas_state.func_tbl_ptr[test_param_ptr->src_tech],
           test_param_ptr->tgt_tech, ftm_meas_state.func_tbl_ptr[test_param_ptr->tgt_tech]);
    return;
  }
  target_func_tbl_ptr = ftm_meas_state.func_tbl_ptr[test_param_ptr->tgt_tech];
  source_func_tbl_ptr = ftm_meas_state.func_tbl_ptr[test_param_ptr->src_tech];

  /* Get the source RXLM buffers */
  if(source_func_tbl_ptr->get_src_rxlm_buffer_fp!= NULL)
  {
    status = source_func_tbl_ptr->get_src_rxlm_buffer_fp(test_param_ptr);
    if(status == FALSE)
    {
      MSG_1(MSG_SSID_RF,MSG_LEGACY_ERROR,
          "Failed getting source RXLM buffer for Tech %d",test_param_ptr->src_tech);
      return;
    }
  }

  /* Get the Taget RXLM buffers from state machine stored xtoy_test_y()*/
  test_param_ptr->tgt_rxlm_buf_idx_ant0 = ftm_meas_state.tgt_rxlm_ant0_idx;
  test_param_ptr->tgt_rxlm_buf_idx_ant1 = ftm_meas_state.tgt_rxlm_ant1_idx;

  if (ftm_irat_tune == TRUE)
  {
    /* Exit Target tech */
    if (target_func_tbl_ptr->meas_exit_tech_fp != NULL ) 
    {
      status = target_func_tbl_ptr->meas_exit_tech_fp(test_param_ptr, FALSE);
      if(status == FALSE)
      {
        MSG_1(MSG_SSID_RF,MSG_LEGACY_ERROR,
            "Failed Exiting Tech %d in IRAT Tune-Back",test_param_ptr->tgt_tech);
        return;
      }
    }

    /* Enter Source tech */
    if (source_func_tbl_ptr->meas_enter_tech_fp != NULL ) 
    {
      status = source_func_tbl_ptr->meas_enter_tech_fp(test_param_ptr, TRUE);
      if(status == FALSE)
      {
        MSG_1(MSG_SSID_RF,MSG_LEGACY_ERROR,
            "Failed Entering Tech %d in IRAT Tune-Back",test_param_ptr->src_tech);
        return;
      }
    }
  }

  /* Exit Meas */
  exit_param.header.source_tech = test_param_ptr->src_tech;
  exit_param.header.source_param[0].device = RFM_DEVICE_0;
  exit_param.header.source_param[0].div_device = RFM_DEVICE_1;
  exit_param.header.source_param[0].band = test_param_ptr->src_band;
  exit_param.header.source_param[0].div_en = test_param_ptr->src_div_en;
  if (test_param_ptr->src_extra_carrier_flag == TRUE) 
  {
    exit_param.header.source_param[1].device = RFM_DEVICE_2;
    exit_param.header.source_param[1].div_device = RFM_DEVICE_3;
    exit_param.header.source_param[1].band = test_param_ptr->src_band_carrier_1;
    exit_param.header.source_param[1].div_en = test_param_ptr->src_div_en_carrier_1;
    exit_param.header.num_source_item = 2;
  }
  else
  {
    exit_param.header.num_source_item = 1;
  }
  exit_param.is_measurement_gap_aborted = FALSE;
  exit_param.header.target_tech = test_param_ptr->tgt_tech;
  exit_param.header.target_param.device = RFM_DEVICE_0;
  exit_param.header.target_param.div_device = RFM_DEVICE_1;
  exit_param.header.target_param.band = test_param_ptr->tgt_band;
  exit_param.header.target_param.div_en = test_param_ptr->tgt_div_en;
  exit_param.source_param = NULL;
  exit_param.target_param = NULL;
  if(source_func_tbl_ptr->get_exit_param_fp != NULL)
  {
    exit_param.source_param = source_func_tbl_ptr->get_exit_param_fp(test_param_ptr);
  }
  if(target_func_tbl_ptr->get_exit_param_fp != NULL)
  {
    exit_param.target_param = target_func_tbl_ptr->get_exit_param_fp(test_param_ptr);
  }
  if(target_func_tbl_ptr->exit_meas_fp != NULL)
  {
    status = target_func_tbl_ptr->exit_meas_fp(&exit_param);
    if(status == FALSE)
    {
      MSG_1(MSG_SSID_RF,MSG_LEGACY_ERROR,
          "Failed EXIT_MEAS for Tech %d",test_param_ptr->tgt_tech);
      return;
    }
  }

  /* Set back to Original Clocks */
  ftm_meas_clock_setup(test_param_ptr, FALSE);

  ftm_meas_deallocate_lm_buffer(test_param_ptr);
}

void ftm_meas_init(void)
{
   rfm_mode_enum_type mode = RFM_PARKED_MODE;

   ftm_meas_state.test_level = FTM_MEAS_LEVEL_RF_EXECUTE_SCRIPT;
   ftm_meas_state.tgt_rxlm_ant0_idx = RXLM_MAX_BUFFERS;
   ftm_meas_state.tgt_rxlm_ant1_idx = RXLM_MAX_BUFFERS;
   for(mode=RFM_PARKED_MODE; mode < RFM_NUM_MODES; mode++)
   {
     ftm_meas_state.func_tbl_ptr[mode]=NULL;
   }
}

boolean ftm_meas_register(rfm_mode_enum_type tech, ftm_meas_func_tbl_type *func_tbl_ptr)
{
  if(func_tbl_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_FATAL, "NULL function pointer table");
    return FALSE;
  }
  
  if(tech >= RFM_NUM_MODES)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL, "Invalid Tech %d",tech);
    return FALSE;
  }
  
  ftm_meas_state.func_tbl_ptr[tech] = func_tbl_ptr;
  return TRUE;
}

boolean ftm_meas_deregister(rfm_mode_enum_type tech)
{
  if(tech >= RFM_NUM_MODES)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL, "Invalid Tech %d",tech);
    return FALSE;
  }
  
  ftm_meas_state.func_tbl_ptr[tech] = NULL;
  return TRUE;
}


void ftm_meas_log_irat_script_info
(
   rfm_meas_program_script_type script_type,
   uint16 num_trx, 
   uint16 num_trx_task, 
   uint16 num_non_trx, 
   uint16 num_non_trx_task, 
   uint16 exec_time_us
)
{
  uint8 index = 0;
  if (script_type == RFM_MEAS_PROGRAM_STARTUP) 
  {
    index = 0;
  }
  else if (script_type == RFM_MEAS_PROGRAM_CLEANUP)
  {
    index = 1;
  }
  else
  {
    return;
  }
  memset(&(ftm_meas_irat_info[index]),0,sizeof(ftm_meas_irat_script_info_type));
  ftm_meas_irat_info[index].num_trx = num_trx;
  ftm_meas_irat_info[index].num_trx_task = num_trx_task;
  ftm_meas_irat_info[index].num_non_trx = num_non_trx;
  ftm_meas_irat_info[index].num_non_trx_task = num_non_trx_task;
  ftm_meas_irat_info[index].execute_time_us = exec_time_us;
}

/*----------------------------------------------------------------------------*/
void ftm_meas_get_irat_script_info
(
   rfm_meas_program_script_type script_type,
   uint16 *num_trx, 
   uint16 *num_trx_task, 
   uint16 *num_non_trx, 
   uint16 *num_non_trx_task, 
   uint16 *exec_time_us
)
{
  uint8 index = 0;
  if (script_type == RFM_MEAS_PROGRAM_STARTUP) 
  {
    index = 0;
  }
  else if (script_type == RFM_MEAS_PROGRAM_CLEANUP)
  {
    index = 1;
  }
  else
  {
    return;
  }

  if ((num_trx!= NULL) &&
      (num_trx_task!= NULL) &&
      (num_non_trx!= NULL) &&
      (num_non_trx_task!= NULL) &&
      (exec_time_us!= NULL))
  {
    *num_trx = ftm_meas_irat_info[index].num_trx;
    *num_trx_task = ftm_meas_irat_info[index].num_trx_task;
    *num_non_trx = ftm_meas_irat_info[index].num_non_trx;
    *num_non_trx_task = ftm_meas_irat_info[index].num_non_trx_task;
    *exec_time_us = ftm_meas_irat_info[index].execute_time_us;
  }
}

#endif /* FTM_HAS_IRAT */
