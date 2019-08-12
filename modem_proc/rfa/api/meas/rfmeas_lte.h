#ifndef RFM_MEAS_LTE_H
#define RFM_MEAS_LTE_H

/*
   @file
   rfm_meas_lte.h

   @brief
   LTE specific interface file.

   @details
   
*/

/*===========================================================================
Copyright (c) 2008 - 2014 by Qualcomm Technologies, Incorporated.  All Rights
                           Reserved. EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/meas/rfmeas_lte.h#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------  
12/10/14   kab     ML1-RF interface changes for IRAT to support Single Rx
11/20/14   php     ML1-RF interface changes for IRAT to support HORxD/other fixes
07/31/14   ndb     Added support for extended EARFCN for LTE
02/06/13   gvn     Change interface of lte exit params to support Abort
08/29/13   pl      Add flag to allow skipping drx pre-config on same transceiver
03/06/13   pl      Add structure tag to help with non-LTE builds
02/01/13   pl      Update IRAT buils script structure, move common field to common header
01/15/13   pl      Add support for SCELL at source
11/20/12   gvn     Featurize LTE for Triton   
10/31/12   pl      Added CA supported IRAT interface for LTE
10/02/12   pl      Add diversity device for LTE IRAT
08/30/12   aca     LTE IRAT Half script support    
03/28/11   sar     Relocating file for rfa/api decoupling.
03/02/11   tnt     LM integration for LTE-IRAT
10/23/10   jyu     Add L2L interfreq support    
09/10/10   pl      Adding temporary LTE exit mode API for IRAT
08/20/10   pl      Upate IRAT API Interface to support GRFC stop script
08/08/10   pl      Update IRAT API interface to support STOP script
05/07/10   pl      Update LTE build script support for LTE
10/14/08   can     Initial version.

============================================================================*/

#include "rfcom.h"
#include "rfm_types.h"
#include "rfmeas_mc.h"
#include "rfmeas_types.h"
#include "lm_types.h"
#ifdef FEATURE_LTE
#include "rflte_msg.h"

typedef struct
{
   uint32 prx_rxlm_handle;
   uint32 drx_rxlm_handle;
} rfm_meas_rxlm_handle_type;

typedef struct 
{
   rfcom_device_enum_type device; 
   rfcom_lte_earfcn_type rx_channel;             
   rfcom_lte_bw_type bw;  
   int       buffer_index; 
   rfm_meas_rxlm_handle_type tgt_rxlm_handles;
} rfm_meas_lte_exit_param_type;

typedef struct 
{
   rfcom_device_enum_type device; 
   rfcom_lte_earfcn_type rx_channel;             
   rfcom_lte_bw_type bw;  

} rfm_meas_lte_enter_param_type;

typedef struct 
{
   lm_handle_type        rxlm_buf_idx_ant0;
   /*!<  LM buffer index for prx*/

   lm_handle_type        rxlm_buf_idx_ant1;
   /*!<  LM buffer index for drx*/

   lm_handle_type        rxlm_buf_idx_ant2;
   /*!<  LM buffer index for drx2*/

   lm_handle_type        rxlm_buf_idx_ant3;
   /*!<  LM buffer index for drx3*/

} rfm_meas_lte_rxlm_buf_idx_type;

typedef struct rfm_meas_lte_setup_and_build_scripts_param_struct
{
   lm_handle_type        target_rxlm_buf_idx_ant0;
   /*!<  LM buffer index for prx*/

   lm_handle_type        target_rxlm_buf_idx_ant1;
   /*!<  LM buffer index for drx*/

   rfcom_lte_earfcn_type                rx_chan[RFA_RF_LTE_MAX_CELL_SUPPORTED];
   /*!<  Channel on which LTE will be tuned, regardless of source or target tech  */

   rfcom_lte_bw_type     rx_bw[RFA_RF_LTE_MAX_CELL_SUPPORTED];  
   /*!<  Channel bandwidth on which LTE will use, regardless of source or target tech  */

   uint32                buffer_index; 
   /*!<  Buffer index for clean-up or start-up scripts.*/ 

   boolean               skip_drx_pre_config; 
   /*!<  skip pre_config to be performed on the same transceiver */ 

   rfm_meas_lte_rxlm_buf_idx_type source_rxlm_buf_idx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
   /*!<  Source RxLM buffer indices */

   uint8 src_chain_mask[RFA_RF_LTE_MAX_CELL_SUPPORTED];
   /*!<  Chain mask indicating active Rx chains*/

   uint16 single_chain_bitmask[RFA_RF_LTE_MAX_CELL_SUPPORTED];  
   /*!<  Rx_SC_Bit_Mask [8 bits] | Tx_SC_Bit_Mask [8 bits] */  

   uint32                active_carrier_idx_mask; 
   /*!<  Carrier index mask: which carriers is the request for on source */ 

} rfm_meas_lte_setup_and_build_scripts_param_type;

#endif /*FEATURE_LTE*/
#endif /* RFM_MEAS_LTE_H */


