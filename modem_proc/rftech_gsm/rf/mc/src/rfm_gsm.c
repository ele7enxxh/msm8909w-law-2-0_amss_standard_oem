/*!
   @file
   rfm_gsm.c

   @brief
   This file contains all declarations and definitions necessary to use the RF
   GSM drivers.

   @details
   
*/

/*===========================================================================
Copyright (c) 1999 - 2002, 2006, 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/mc/src/rfm_gsm.c#1 $
$DateTime: 2016/12/13 07:59:51 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/24/14   tsr     Initialise GL1 Lower CCS prio flag to false for IRAT
06/12/14   sw      Use GL1 api to map sub_id to as_id
05/27/14   sb      Changes to add RFM APIs for GSM arfcn to frequency conversion
04/10/14   sc      Fix KW error of possible dereference to RxD argument
03/11/14   tsr     Added RXD support for RX burst 
02/26/14   sc      Pass core data through cmd processing and mc layer
02/18/14   tsr     Add RXD parameters to ccs event building.
08/01/14   cj      Updated buffer id in rfm_prepare_rx_burst_ccs_events
08/06/13   sc      RFM GSM clean up unused/unrequired RFM functions
07/30/13   ec      Add variable to track IQ capture device, and api to set the 
                   active device
07/29/13   sml/svi ASD support for GSM tech
07/25/13   ec	   Pass rf_task id into mc functions 
02/25/13   sc      Add cm enter/exit API for init/de-init of RF GSM iRAT
02/19/13   sc      Added APIs to allow GL1 to alloc and free Rx meas buffers
10/11/12   sc      Added prepare Rx burst API for inline GL1 iRAT readiness
10/11/12   sc      Cleanup up unrequired Rx burst buffer params
08/02/12   sc      Add num slots to setup Tx burst API
07/26/12   sr      Changes to APIS to take rfm_dev as parameter.
07/09/12   sc      Removed arguments to setup Rx/Tx burst API calls for CCS format
04/04/12   ggs     Added rfm_gsm_override_pa_range to provide a cleaner top-down 
                   call flow to RF driver APIs from FTM mode
04/23/12   sc      Added shared memory interface integration for Tx band, 
                   Rx/Tx burst, and Tx power APIs
02/03/12   av      IRAT cleanup phase 1
11/29/11   sb      Added functionality for GSM IQ CAPTURE
10/19/11   sb      Apply therm read in idle frame and enable FTM temp comp and vbatt comp during idle frame
07/12/11   vrb     Added critical section locks for all rfm gsm APIs
03/21/11   sr      Added timing info access interface.
09/21/10   ggs     moved the api to return rf_warmup time from rf_stubs.c file to this file.
12/28/09   sr      High/Medium lint errors fix
12/02/09   sr      Fixed lint warnings with typecast.
10/14/08   sr      Initial version to separate gsm specific external interface.

============================================================================*/

#include "rfcom.h"
#include "msg.h"
#include "rfgsm_mc.h"
#include "lm_types.h"
#include "rxlm_intf.h"
#include "rfcommon_locks.h"
#include "rfm_internal.h"
#include "gl1_hw.h"

/* Holds active device for IQ capture */
rfm_device_enum_type active_iq_capture_device = RFM_INVALID_DEVICE;



/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function calls the rfgsm_mc function to create the RF CCS events for all 
  the bursts requested from GL1.

  This function can be called from GL1 for iRAT scenarios.

  @param device                   : RF Multimode device
  @param num_rx_bursts            : Number of Rx/Monitor bursts required
  @param rx_chan_params           : Rx burst parameters for each burst i.e arfcn
  @param gfw_rf_burst_event_ptrs  : Pointer to an array of pointers to shared mem
*/
void rfm_prepare_rx_burst_ccs_events( rfa_rf_gsm_rx_burst_req_s rx_burst_info )
{
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;
  /*RXD will always be disabled for IRAT*/
  rfgsm_core_data_type rfgsm_core_data;

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rfm_get_lock_data());

  /* Get the core handle ptr */
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rx_burst_info.device);
  if( rfgsm_core_handle_ptr == NULL )
  {
    RF_MSG(MSG_LEGACY_ERROR, "RF GSM prepare Rx measurments rfgsm_core_handle_ptr NULL!");
    /* Release Critical Section Lock */
    rf_common_leave_critical_section(rfm_get_lock_data());
    return;
  }

  /* Set the core data */
  rfgsm_core_data.rf_task = RF_MEAS_CALL;
  rfgsm_core_data.rfm_dev = rx_burst_info.device;
  rfgsm_core_data.sub_id = rx_burst_info.sub_id;
  rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rx_burst_info.sub_id);
  rfgsm_core_data.triple_buffer_id = rx_burst_info.buffer_id;
  rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;
  rfgsm_core_data.rfgsm_core_handle_ptr_rxd = NULL;
  /*For IRAT, RXD is always off*/
  rfgsm_core_data.enable_rx_diversity = FALSE;
  rfgsm_core_data.gl1_lower_rx_ccs_prio = FALSE;

  rfgsm_mc_process_rx_burst(&rfgsm_core_data,
                             rx_burst_info.rx_burst_params->num_rx_bursts,
                             rx_burst_info.rx_burst_params->rx_chan_params,
                             NULL,
							 rx_burst_info.rx_burst_params->gfw_rf_burst_event);


  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rfm_get_lock_data());
}



/*----------------------------------------------------------------------------*/
/*
  @brief
  This function is called to enable RFSW start copy captured IQ samples from the location pointed to by GL1 struct ptr to local buffer

  @details
  
  @param
  GL1 struct ptr : GfwIqSamplesBuffer*

*/
void rfm_log_iq_data( GfwIqSamplesBuffer* data )
{
    if ( active_iq_capture_device < RFM_MAX_WAN_DEVICES ) 
    {
        /* Acquire Critical Section Lock */
        rf_common_enter_critical_section(rfm_get_lock_data());

        rfgsm_mc_log_iq_data(active_iq_capture_device, data);

        /* Release Critical Section Lock */
        rf_common_leave_critical_section(rfm_get_lock_data());
    }
    else
    {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "IQ capture device not configured/bad value: %d", active_iq_capture_device);
    }
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  Sets the active device for IQ capture, Layer 1 will call an RFM API and the
  active device will be populated with the samples.

  @details
  
  @param
  rfm_dev

*/
void rfm_set_active_iq_device( rfm_device_enum_type rfm_dev )
{
    if (rfm_dev < RFM_MAX_WAN_DEVICES) 
    {
        active_iq_capture_device = rfm_dev;
        MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "IQ capture device configured as %d.", active_iq_capture_device);
    } 
    else if ( rfm_dev == RFM_INVALID_DEVICE ) 
    {
        active_iq_capture_device = rfm_dev;
        MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "IQ capture device deconfigured: %d", active_iq_capture_device);
    }
    else
    {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Invalid device specified for IQ capture: %d", rfm_dev);
    }

}




uint32 rfm_gsm_convert_arfcn_to_tx_freq(rfgsm_band_type band, uint16 arfcn)
{

   return (rfgsm_mc_convert_arfcn_to_tx_freq (band,arfcn));
}


uint32 rfm_gsm_convert_arfcn_to_rx_freq(rfgsm_band_type band, uint16 arfcn)
{

   return (rfgsm_mc_convert_arfcn_to_rx_freq (band,arfcn));
}

