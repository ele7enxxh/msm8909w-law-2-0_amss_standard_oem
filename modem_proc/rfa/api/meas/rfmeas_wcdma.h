#ifndef RFM_MEAS_WCDMA_H
#define RFM_MEAS_WCDMA_H

/*
   @file
   rfm_meas_wcdma.h

   @brief
   WCDMA specific interface file.

   @details
   
*/

/*===========================================================================
Copyright (c) 1999 - 2002, 2006, 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2008 by Qualcomm Technologies, Incorporated.  All Rights
                           Reserved. EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/meas/rfmeas_wcdma.h#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------  
03/26/14   rmb     Add RxLM Buffer index for Abort script to work.
09/13/13   aa      Support for DBDC IRAT  
04/14/11   dw      Add back RxLM buffer index
04/08/11   sar     Removed condition #def's for CMI-4.
04/05/11   dw      GtoW/LtoW RxlM support
03/28/11   sar     Relocating file for rfa/api decoupling.
09/15/10   kguo    Used one buf_index for both start and cleanup script.
08/20/10   pl      Upate IRAT API Interface to support GRFC stop script
08/06/10   kguo    Updated parameters in rfm_meas_wcdma_setup_and_build_scripts_param_type    
                   to support cleanup script 
06/28/10   kguo    Updated rfm_meas_wcdma_setup_and_build_scripts_param_type    
                   with memory index
05/26/10   kguo    Updated rfm_meas_wcdma_setup_and_build_scripts_param_type 
04/12/10   kguo    Added data structure for W
10/14/08   can     Initial version.

============================================================================*/

#include "rfcom.h"
#include "rfm_types.h"
#include "rfmeas_mc.h"
#include "rfmeas_types.h"
#include "lm_types.h"

typedef struct 
{
   /*! @brief  Device on which WCDMA will be tuned  */
   rfcom_device_enum_type device; 

   /*! @brief  Channel on which WCDMA will be tuned  */
   uint16 channel[RFCOM_MAX_CARRIERS];

   /*! @brief  Specify SC or DC mode  */
   rfcom_multi_carrier_hspa_id_type carrier_type;      
   
   uint32                 buf_index;
   /*!<  Buffer index for startup/cleanup scripts.*/ 

   /*! @brief  RxLM buffer index  */
   lm_handle_type rxlm_buf_index; 

} rfm_meas_wcdma_exit_param_type;

typedef struct 
{
   /*! @brief  Device on which WCDMA will be tuned  */
   rfcom_device_enum_type device; 

   /*! @brief  Channel on which WCDMA will be tuned  */
   uint16 channel[RFCOM_MAX_CARRIERS]; 

   /*! @brief  Specify SC or DC mode  */
   rfcom_multi_carrier_hspa_id_type carrier_type;      
   
   /*! @brief  RxLM buffer index  */
   lm_handle_type rxlm_buf_index;        

} rfm_meas_wcdma_enter_param_type;

typedef struct 
{
   rfcom_device_enum_type device; 
   /*!<  Device on which WCDMA will be tuned  */

   rfcom_mode_enum_type   source_tech;
   /*!<  Specify source technology for IRAT */

   uint32                 source_band;
   /*!< Specify source technology band*/

   rfcom_mode_enum_type   target_tech;
   /*!<  Specify target technology for IRAT */

   uint32                 target_band;
   /*!<  Specify target technology for IRAT */

   uint16                 channel[RFCOM_MAX_CARRIERS];
   /*!<  DL Channel on which WCDMA will be tuned  */

   rfcom_multi_carrier_hspa_id_type carrier_type;
   /*!<  Specify SC or DC mode  */

  
  uint16           channels[MAX_SOURCE_ITEM][RFCOM_MAX_CARRIERS];

  rfcom_multi_carrier_hspa_id_type carrier_types[MAX_SOURCE_ITEM];

   rfcom_lte_bw_type      bw;
   /*!<  Specify target technology BW for IRAT if it's LTE */

   uint32                 buf_index;
   /*!<  Buffer index for startup/cleanup scripts.*/ 

   /*! @brief  RxLM buffer index  */
   lm_handle_type rxlm_buf_index;        

} rfm_meas_wcdma_setup_and_build_scripts_param_type;

#endif /* RFM_MEAS_WCDMA_H */




