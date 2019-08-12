#ifndef RFM_MEAS_1X_H
#define RFM_MEAS_1X_H

/*
   @file
   rfm_meas_1x.h

   @brief
   1x\DO specific interface file.

   @details
   
*/

/*===========================================================================
Copyright (c) 2011 - 2012 by Qualcomm Technologies, Incorporated.  All Rights
                           Reserved. EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/meas/rfmeas_1x.h#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/15/12  Saul/aro IRAT. 1x2L 1x Stop Rx sequence change to delay premature 
                   RF sleep/vreg shutdown which affects LTE measurements.
07/15/11   Saul    IRAT updates.
07/14/11   Saul    IRAT updates.
07/08/11   Saul    Preliminary IRAT changes.
04/16/11   Les     Initial version.

============================================================================*/

#include "rfcom.h"
#include "rfm_types.h"
#include "rfmeas_mc.h"
#include "rfmeas_types.h"

  
/*! @brief  This structure will be used for L<->1x  */ 
typedef struct  
{
  rfm_device_enum_type device;
  /*!<  Specifies the device to configure */ 
   
  sys_channel_type   band_chan;
  /*!< @brief  Band and channel on which 1x will be tuned  */ 

  lm_handle_type        rxlm_buf_index;
  /*!< @brief  RxLM buffer index  */
     
} rfm_meas_1x_enter_param_type; 
   
   
/*! @brief  This structure will be used for L<->1x  */ 
typedef struct  
{ 
  rfm_device_enum_type device;
  /*!<  Specifies the device to configure */
   
  sys_channel_type   band_chan;
  /*!< @brief  Band and channel on which 1x was tuned  */ 
  
  lm_handle_type        rxlm_buf_index;
  /*!< @brief  RxLM buffer index  */

} rfm_meas_1x_exit_param_type; 
   
/*! @brief  This structure will be used for L<->1x  */ 
typedef struct  
{ 
  rfm_device_enum_type  device;
  /*!<  Specifies the device to configure */

  rfcom_mode_enum_type  source_tech;
  /*!<  Specify source technology for IRAT */

  uint32                source_band;
  /*!< Specify source technology band,, if source tech is not 1x*/

  rfm_mode_enum_type  target_tech;
  /*!<  Specify target technology for IRAT */

  uint32                target_band;
  /*!< Specify target technology band, if target tech is not 1x */

  sys_channel_type      band_chan;
  /*!<  Band and channel on which 1x will be tuned, 
        regardless of the source or target tech type  */ 
   
  rfcom_lte_bw_type     source_bw;
  /*!< Specify LTE system bandwidth if on of the tech is LTE */
   
  uint32                buffer_index; 
  /*!<  SBI buffer index on which scripts for tuning 1x will be stored */      

  lm_handle_type        rxlm_buf_index;
  /*!< @brief  RxLM buffer index  */

} rfm_meas_1x_setup_and_build_scripts_param_type; 
#endif /* RFM_MEAS_1X_H */




