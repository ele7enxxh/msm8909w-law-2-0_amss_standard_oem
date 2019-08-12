/*===========================================================================


      R F C  M e a s  W C D M A  c o n t r o l   A P I

                            S o u r c e  F i l e

DESCRIPTION
  This file contains LTE MEAS functions for the RFC.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
 
$DateTime: 2016/12/13 07:59:54 $ 
$Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/rfc/src/rfc_meas_wcdma.c#1 $


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/02/14   kr      Update GRFC Tx scripts for W2G tune back scripts in tx mode.
09/24/14   rmb     Fix CW.
04/23/14   ac      asdiv fix
07/22/13   aro     Removed test code
07/19/13   aro     Featurized ASDIV
07/18/13   aro     Compiler warning fix 
07/17/13   aro     Appended ASDIv switch to GRFC script 
05/29/13   spa     Added delay param to APIs for getting GRFC scripts
09/18/12   ac      rfc to take device as a parameter
06/29/12   sr      RFC changes : use rf_hal_buffer to generate scripts 
07/20/11   vb      Populating wcdma mode signal list as part of rfc_meas_wcdma_generate_grfc_script()
07/19/11   whc     Initial version
============================================================================*/


/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "rfc_meas_wcdma.h"
#include "rfcom.h"
#include "msg.h"

#include "rfcommon_core.h"
#include "rfc_common.h"
#include "rfc_card_wcdma.h"
#include "rfcommon_asdiv_manager.h"

void
rfc_meas_wcdma_generate_grfc_script
(
  uint32 band,
  rfc_grfc_device_type *device_info,
  rf_buffer_intf *grfc_buf_ptr,
  boolean status
)
{

  rfc_sig_cfg_type *rfc_mode_grfc_tbl_ptr = NULL;
  rfc_sig_cfg_type *rfc_prx_grfc_tbl_ptr = NULL;
  rfc_sig_cfg_type *rfc_drx_grfc_tbl_ptr = NULL;
  rfc_sig_cfg_type *rfc_tx_grfc_tbl_ptr = NULL;
  #ifdef FEATURE_RF_ASDIV
  rfc_sig_cfg_type *rfc_asd_signals_ptr = NULL;
  #endif

  rfc_mode_grfc_tbl_ptr =  
    (rfc_sig_cfg_type *)rfc_wcdma_get_mode_config_data( RFM_DEVICE_0,
                                                        RFC_MODE_GRFC_DATA );

  if (status == TRUE)
  {
    rfc_tx_grfc_tbl_ptr = 
     	(rfc_sig_cfg_type *)rfc_wcdma_get_tx_band_config( RFM_DEVICE_0,
                                   (rfcom_wcdma_band_type)band, RFC_GRFC_DATA );
  }
  
  if(device_info->primary_rx_device != RFM_INVALID_DEVICE)
  {
    rfc_prx_grfc_tbl_ptr =  
      (rfc_sig_cfg_type *)rfc_wcdma_get_rx_band_config( device_info->primary_rx_device,
                                   (rfcom_wcdma_band_type)band, RFC_GRFC_DATA );

    #ifdef FEATURE_RF_ASDIV
    /* Append the Antenna switch position script. This may change between
     different wakeups. During wakeup, the script pertaining to last
     switch position before going to sleep is used */
    rfc_asd_signals_ptr = 
      (rfc_sig_cfg_type *)rfc_wcdma_get_asd_xsw_config( device_info->primary_rx_device,
          (rfcom_wcdma_band_type)band,
          rfcommon_asdiv_get_current_position(device_info->primary_rx_device));
    #endif
  }
  
  if(device_info->secondary_rx_device != RFM_INVALID_DEVICE)
  {
    rfc_drx_grfc_tbl_ptr =  
      (rfc_sig_cfg_type *)rfc_wcdma_get_rx_band_config(device_info->secondary_rx_device,
                                  (rfcom_wcdma_band_type)band, RFC_GRFC_DATA);
  }
  
   /*==========================================================================
    For each valid pointer, Look up GRFC Engine Number 
           and populate the IRAT GRFC buffer with correct logic.
   ==========================================================================*/

  
  if( rfc_mode_grfc_tbl_ptr != NULL )
  { 
    /* Dime RFC changes: Call the following API with rf_hal_buffer ptr */
     rfc_common_generate_grfc_script( rfc_mode_grfc_tbl_ptr, grfc_buf_ptr ,
                                      0 /* delay */ );
  
  }
    else
  {
    MSG_HIGH("rfc_mode_grfc_tbl_ptr is NULL",0,0,0);
  }
  
  if( rfc_prx_grfc_tbl_ptr != NULL )
  { 
    /* Dime RFC changes: Call the following API with rf_hal_buffer ptr */
     rfc_common_generate_grfc_script( rfc_prx_grfc_tbl_ptr, grfc_buf_ptr, 
                                      0 /* delay */ );  
  }
  else
  {
    if(device_info->primary_rx_device != RFM_INVALID_DEVICE)
    {
      MSG_HIGH("rfc_prx_grfc_tbl_ptr is NULL",0,0,0);
    }
  }
  
  if( rfc_drx_grfc_tbl_ptr != NULL )
  { 
    /* Dime RFC changes: Call the following API with rf_hal_buffer ptr */
     rfc_common_generate_grfc_script( rfc_drx_grfc_tbl_ptr, grfc_buf_ptr ,
                                      0 /* delay */);
  
  }
  else
  {
    if(device_info->secondary_rx_device != RFM_INVALID_DEVICE)
    {
      MSG_HIGH("rfc_drx_grfc_tbl_ptr is NULL",0,0,0);
    }
  }
  
  #ifdef FEATURE_RF_ASDIV
  if( rfc_asd_signals_ptr != NULL )
  { 
    rfc_common_generate_grfc_script( rfc_asd_signals_ptr,
                                     grfc_buf_ptr,
                                     0 );  
  }
  else
  {
    MSG_HIGH("rfc_asd_signals_ptr is NULL",0,0,0);
  }
  #endif
  
  if ( rfc_tx_grfc_tbl_ptr != NULL)
  {
    rfc_common_generate_grfc_script( rfc_tx_grfc_tbl_ptr,
                                     grfc_buf_ptr,
                                     0 );
  }
  
  
} /*rfc_meas_wcdma_generate_grfc_script*/
