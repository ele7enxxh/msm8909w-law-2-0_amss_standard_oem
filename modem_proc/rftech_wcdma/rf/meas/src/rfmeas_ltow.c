/*===========================================================================


   R F  D r i v e r  LTOW Measurement Main Control


DESCRIPTION
  This file contains all declarations and definitions necessary to use
  the WCDMA Main Control.

Copyright (c) 1999 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/meas/src/rfmeas_ltow.c#1 $
$DateTime: 2016/12/13 07:59:54 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/19/14   rmb     Pass device information to config/deconfig CCS.
08/28/14   vbh     Update the call to config_ccs with the correct device parameter
02/10/14   rmb     Add global NV pointers for each of band specific static NVs.
01/08/14   rmb     Added a temporary fix for allocating TQ in L2W.
01/02/14   dw      Bolt cleanup on legacy mdsp function call 
11/20/13   rmb     Bolt IRAT cleanup.
08/23/13   vs      Cleanup of featurization
07/19/13   ac      DBDC bringup related changes
09/18/12   ac      rfc to take device as parameter 
03/17/12   jfc     Remove clkrgm* header files
02/07/11   dw      Fix AGC paramter not updated properly in L2W->W enter->L enter->L2W again  
11/11/11   sr      removed unused IRAT code.
09/19/11   dw      Add get LtoW PLL timing support 
09/19/11   dw      clean up rfwcdma_core_txctl_set_tx_modulator() 
05/23/11   dw      Remove rfwcdmalib_msm.h   
05/20/10   dw      Remove rfwcdmalib_msm_init_config() 
02/10/10   dw      WCDMA MC cleanup
01/05/10   dw      Initial support for RxLM
11/05/10   kguo    Passed band parameter in rfwcdmalib_msm_set_rxfe_bsp.
09/23/10   kguo    Removed cliregime call in rfmeas_ltow_init
09/22/10   dw      Remove rfwcdma_core_rxctl_init_cgagc_params() from ltow init
08/24/10   kguo    Added rfwcdma_core_rxctl_init_cgagc_params in ltow_init
07/19/10   ka      Removed warnings 
06/01/10   kguo    Initial version 

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "rfcom.h"
#include "msg.h"
#include "rfmeas_mc.h"
#include "rfnv_wcdma.h"
#include "rfcommon_nv_mm.h"
#include "rfwcdma_core.h"
#include "rfwcdma_core_rxctl.h"
#include "rfwcdma_core_util.h"

#include "rfwcdma_core_txctl.h"
#include "rfwcdma_core_rxctl.h"
#include "rfwcdma_mc.h"
#include "rfwcdma_msm.h"
#include "rfwcdma_core_util.h"
#include "rfwcdma_core_temp_comp.h"
#include "ftm.h"
#include "rfc_card_wcdma.h"
#include "rfwcdma_mdsp_async.h"
#include "rfwcdma_mdsp_sync.h"
#include "rfwcdma_mdsp_data.h"
#include "rflm_dm_api.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function tunes the synthesizer to the specified channel.  Once
  rf_tune_to_chan has been called, a delay in microseconds must be performed
  by the application to allow the synthesizer to settle.
  The channel is the UTRA Absolute Radio Frequency Channel Number (UARFCN).

  @details
*/
void rfmeas_ltow_select_band_rx(rfcom_wcdma_band_type band)
{
  MSG_HIGH("New WCDMA band %d is selected.", band, 0, 0);
}
 
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function tunes the synthesizer to the specified channel.  Once
  rf_tune_to_chan has been called, a delay in microseconds must be performed
  by the application to allow the synthesizer to settle.
  The channel is the UTRA Absolute Radio Frequency Channel Number (UARFCN).

  @details
*/
void rfmeas_ltow_init
( 
  rfcom_device_enum_type device,
  uint32 handle_id
)
{

  /* This can be removed once the shared memory stuff is removed */
  rfwcdma_mdsp_init();

  /* Initialize MDSP module if it has not been done */
  (void)rfwcdma_mdsp_init_once();

   /* @ Need to be removed once the framework for TQ information from source tech is available 
   Initialize FED CCS module */
   rfwcdma_mdsp_sync_config_ccs(device, TRUE);

  /* Allocate Rx DM buffers for the given RxLM handle */
  (void)rfwcdma_mdsp_allocate_dm_rx_buf((rflm_dm_handle_id_t)handle_id);

  rfwcdma_core_temp_comp_enable(FALSE);
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function tunes the synthesizer to the specified channel.  Once
  rf_tune_to_chan has been called, a delay in microseconds must be performed
  by the application to allow the synthesizer to settle.
  The channel is the UTRA Absolute Radio Frequency Channel Number (UARFCN).

  @details
*/
void rfmeas_ltow_tune_wcdma_rx
( 
  uint16 channel
)
{
  rfcom_wcdma_band_type wcdma_band;

  rfwcdma_mc_set_ltow_flag( TRUE );

  wcdma_band = rfwcdma_core_convert_chan_to_band( channel );

  rfmeas_ltow_select_band_rx( wcdma_band );

}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  

  @details
*/
void rfmeas_ltow_setup
( 
  rfcom_device_enum_type device,
  uint16 channel,
  uint32 handle_id
)
{
  rfmeas_ltow_init(device, handle_id);
  rfmeas_ltow_tune_wcdma_rx( channel );

}
void rfmeas_mc_ltow_get_irat_info_param
(
  rfm_meas_ltow_irat_info_param_type *info_param
)
{
  rfc_wcdma_core_config_type rfc_core_config;
  (void)rfc_wcdma_command_dispatch(RFM_DEVICE_0, RFC_WCDMA_GET_CORE_CONFIG, (void*)(&rfc_core_config));
  info_param->pll_settling_time = rfc_core_config.tune_time;
  info_param->agc_settling_time = rfc_core_config.cgagc_settling_time;
  info_param->ssbi_write_time = (NUM_OF_MAX_SBI_BEFORE_WRITES*SBI_WRITE_TIME_IN_NS)/1000;
}
