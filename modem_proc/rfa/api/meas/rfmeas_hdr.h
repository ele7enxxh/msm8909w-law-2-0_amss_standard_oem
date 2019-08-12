#ifndef RFMEAS_HDR_H 
#define RFMEAS_HDR_H 
  
/* 
  @file 
  rfmeas_hdr.h 
  
  @brief 
  measurement hdr specific interface file. 
   
   @details 
    
*/ 
   
/*=========================================================================== 
Copyright (c) 2010 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.  
   
                    EDIT HISTORY FOR FILE 
   
This section contains comments describing changes made to this file. 
Notice that changes are listed in reverse chronological order. 
   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/meas/rfmeas_hdr.h#1 $ 
$DateTime: 2016/12/13 07:59:52 $ 
$Author: mplcsds1 $ 
   
when       who     what, where, why 
--------   ---     ---------------------------------------------------------- 
03/15/12  Saul/aro IRAT. 1x2L 1x Stop Rx sequence change to delay premature 
                   RF sleep/vreg shutdown which affects LTE measurements.
12/16/11   Saul    IRAT. D2L HDR Stop Rx sequence change to delay premature 
                   RF sleep/vreg shutdown which affects LTE measurements.
07/15/11   Saul    IRAT updates.
07/14/11   Saul    IRAT updates.
03/28/11   sar     Relocating file for rfa/api decoupling.
08/20/10   pl      Upate IRAT API Interface to support GRFC stop script
08/08/10   pl      Update IRAT API interface to support STOP script
05/25/10   pl      Update IRAT API Interface
05/17/10   ans     Updates for interRAT API interface.
03/23/10   can     Fixes.
03/15/10   ans     Initial version. 
   
============================================================================*/ 
   
#include "rfcom.h" 
#include "rfm_types.h" 
#include "rfmeas_mc.h" 
#include "rfmeas_types.h" 
   
  
/*! @brief  This structure will be used for L<->G and W<->G  */ 
typedef struct  
{
  rfm_device_enum_type device;
  /*!<  Specifies the device to configure */ 
    
  sys_channel_type   band_chan;
  /*!<  @brief  Band and channel on which HDR will be tuned  */
      
  lm_handle_type        rxlm_buf_index;
  /*!< @brief  RxLM buffer index  */
      
} rfm_meas_hdr_enter_param_type; 
   
   
/*! @brief  This structure will be used for L<->G and W<->G  */ 
typedef struct  
{
  rfm_device_enum_type device;
  /*!<  Specifies the device to configure */ 
    
  sys_channel_type   band_chan;
  /*!<  @brief  Band and channel on which HDR was tuned  */

  lm_handle_type        rxlm_buf_index;
  /*!< @brief  RxLM buffer index  */
  
} rfm_meas_hdr_exit_param_type; 
   
   
/*! @brief  This structure will be used for L<->DO and W<->DO  */
typedef struct  
{ 
  rfm_device_enum_type  device;
  /*!<  Specifies the device to configure */

  rfcom_mode_enum_type  source_tech;
  /*!<  Specify source technology for IRAT */

  uint32                source_band;
  /*!< Specify source technology band,, if source tech is not HDR*/

  rfm_mode_enum_type  target_tech;
  /*!<  Specify target technology for IRAT */

  uint32                target_band;
  /*!< Specify target technology band, if target tech is not HDR */

  sys_channel_type      band_chan;
  /*!<  Band and channel on which HDR will be tuned, 
        regardless of the source or target tech type  */ 

  rfcom_lte_bw_type     source_bw;
  /*!< Specify LTE system bandwidth if on of the tech is LTE */
   
  rfm_hdr_agc_mode_type agc_mode;
  /*!< Specify HDR RxAGC Mode */
   
  uint32                agc_rtc_offset; 
  /*!< Specify HDR RxAGC RTC Offset */
   
  int16                 prev_agc; 
  /*!< Specify HDR RxAGC previous AGC value */   

  uint32                buffer_index; 
  /*!<  SBI buffer index on which scripts for tuning HDR will be stored */      

  lm_handle_type        rxlm_buf_index;
  /*!< @brief  RxLM buffer index  */

} rfm_meas_hdr_setup_and_build_scripts_param_type; 
   
#endif /* RFM_MEAS_HDR_H */ 
   
   

