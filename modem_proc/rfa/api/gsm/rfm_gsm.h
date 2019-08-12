#ifndef RFM_GSM_H
#define RFM_GSM_H
/*
   @file
   rfm_gsm.h

   @brief
   RF Driver's GSM external interface file.

   @details
   
*/

/*===========================================================================
Copyright (c) 1999 - 2002, 2006, 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/gsm/rfm_gsm.h#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------  
05/27/14   sb      Changes to add RFM APIs for GSM arfcn to frequency conversion
02/18/14   sc      Remove unused code and dependencies  
08/06/13   sc      RFM GSM clean up unused/unrequired RFM functions 
07/30/13   ec      Add function to set active device for IQ capture 
02/25/13   sc      Add cm enter/exit API for init/de-init of RF GSM iRAT
02/19/13   sc      Added APIs to allow GL1 to alloc and free Rx meas buffers
10/11/12   sc      Added prepare Rx bursts API for inline GL1 call and cleaned 
                   up unused burst/buffer parameters
04/04/12   ggs     Added rfm_gsm_override_pa_range to provide a cleaner top-down 
                   call flow to RF driver APIs from FTM mode
03/20/12   sc      Added RFM return status type for GSM RF Task dispatch functions
02/03/12   av      IRAT cleanup phase 1 
11/29/11   sb      Added functionality for GSM IQ CAPTURE 
10/19/11   sb      Apply therm read in idle frame and enable FTM temp comp and vbatt comp during idle frame 
08/26/11    sr     Added  set tx band function prototype.
03/21/11    sr     Added  timing info access interface.
10/14/08   sr     Initial version to separate gsm specific external interface.

============================================================================*/

#include "rfcom.h"
#include "rfm_types.h"
#include "rfgsm_msg.h"
#include "rfgsm_core_types.h"


typedef enum
{
  RF_GSM_RFM_SUCCESS,
  RF_GSM_RFM_SM_ERROR,
  RF_GSM_RFM_STATUS_MAX,
} rfgsm_rfm_status_type;



/*----------------------------------------------------------------------------*/
/*
  This function calls the relevant RF APIs to calculate and create the RF CCS
  events for Rx burst.
*/
void rfm_prepare_rx_burst_ccs_events( rfa_rf_gsm_rx_burst_req_s rx_burst_info );



/*----------------------------------------------------------------------------*/
void rfm_log_iq_data(GfwIqSamplesBuffer* data);


/*----------------------------------------------------------------------------*/
/*
  @brief
  Sets the active device for IQ capture, Layer 1 will call an RFM API and the
  active device will be populated with the samples.

  @details
  
  @param
  rfm_dev

*/
void rfm_set_active_iq_device( rfm_device_enum_type rfm_dev );
uint32 rfm_gsm_convert_arfcn_to_tx_freq(rfgsm_band_type band, uint16 arfcn);
uint32 rfm_gsm_convert_arfcn_to_rx_freq(rfgsm_band_type band, uint16 arfcn);


#endif /* RFM_GSM_H */

