#ifndef RFM_MEAS_TDSCDMA_H
#define RFM_MEAS_TDSCDMA_H

/*
   @file
   rfm_meas_tdscdma.h

   @brief
   TDSCDMA specific interface file.

   @details
   
*/

/*===========================================================================
Copyright (c) 2011, 2013, 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.    
   EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/meas/rfmeas_tdscdma.h#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------    
02/11/14   jyu     Added support to abort scripts
07/09/13   jz      Added support to build special BSP for IRAT
11/09/11   zg      Added irat_rxlm_mode in iRAT interface.    
10/08/11   zg      Initial version.

============================================================================*/

#include "rfcom.h"
#include "rfm_types.h"
#include "rfmeas_mc.h"
#include "rfmeas_types.h"
#include "lm_types.h"
#include "rf_tdscdma_msg.h"

typedef struct 
{
   rfcom_device_enum_type device; 
   uint16    rx_channel;             
   int       buffer_index; 
   lm_handle_type        primary_chain_rxlm_buf_idx;
} rfm_meas_tdscdma_exit_param_type;

typedef struct 
{
   rfcom_device_enum_type device; 
   uint16    rx_channel;

   uint32                source_band;
   /*!< Specify source technology band*/

   uint32				 buffer_index_pre_gap; 
	  /*!<	Buffer index for TDS BSP. It will be used for generating a pre_gap script for T2L iRAT only.*/ 

   uint32				 to_generate_bsp_script; 
	  /*!<	1: BSP script will be generated; 0: not to generate BSP script.*/

} rfm_meas_tdscdma_enter_param_type;

typedef struct 
{
   rfcom_device_enum_type device; 
   /*!<  Device on which TDSCDMA will be tuned  */

   rfcom_mode_enum_type  source_tech;
   /*!<  Specify source technology for IRAT */

   uint32                source_band;
   /*!< Specify source technology band*/

   rfcom_mode_enum_type  target_tech;
   /*!<  Specify target technology for IRAT */

   uint32                target_band;
   /*!<  Specify target technology band */

   lm_handle_type        target_rxlm_buf_idx_ant0;
   /*!<  LM buffer index for prx*/

   lm_handle_type        target_rxlm_buf_idx_ant1;
   /*!<  LM buffer index for drx*/

   uint16                rx_channel;
   /*!<  Channel on which TDSCDMA will be tuned, regardless of source or target tech  */

   uint32                buffer_index; 
   /*!<  Buffer index for clean-up or start-up scripts.*/ 

   rfa_tdscdma_rxlm_mode_t irat_rxlm_mode;
   /*!<  rxlm mode  */

} rfm_meas_tdscdma_setup_and_build_scripts_param_type;

typedef struct 
{
   rfcom_device_enum_type device; 
   /*!<  Device on which TDSCDMA will be tuned  */

   rfcom_mode_enum_type  source_tech;
   /*!<  Specify source technology for IRAT */

   uint32                source_band;
   /*!< Specify source technology band*/

   uint16                source_rx_channel;
   /*!<  Specify source technology DL channel */

   /*! @brief  source diversity enable/disable indicator  */
   uint16                source_div_en;

   //rfcom_tdscdma_bw_type     source_bw;  
   /*!<  Specify source technology channel bandwidth  */

   uint32                source_buffer_index; 
   /*!<  Buffer index for clean-up scripts.*/
   
   rfcom_mode_enum_type  target_tech;
   /*!<  Specify target technology for IRAT */

   uint32                target_band;
   /*!<  Specify target technology band */

   uint16                target_rx_channel;
   /*!<  Specify source technology DL channel */

   /*! @brief  target diversity enable/disable indicator  */
   uint16                target_div_en;

   //rfcom_tdscdma_bw_type     target_bw;  
   /*!<  Specify source technology channel bandwidth  */

   uint32                target_buffer_index; 
   /*!<  Buffer index for clean-up scripts.*/   

   lm_handle_type        target_rxlm_buf_idx_ant0;
   /*!<  LM buffer index for prx*/

   lm_handle_type        target_rxlm_buf_idx_ant1;
   /*!<  LM buffer index for prx*/
} rfm_t2t_setup_and_build_scripts_param_type;

void rftdscdma_mc_meas_irat_exit_mode(void);

#endif /* RFM_MEAS_TDSCDMA_H */


