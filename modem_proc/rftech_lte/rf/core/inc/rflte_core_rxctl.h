#ifndef RFLTE_CORE_RXCTL_H
#define RFLTE_CORE_RXCTL_H
/*!
  @file
  rflte_core_rxctl.h

  @brief
  This is the header file for LTE Rx control functionality. 

  @details

*/

/*============================================================================== 
   
  Copyright (c) 2009 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved 
   
  Qualcomm Proprietary 
   
  Export of this technology or software is regulated by the U.S. Government. 
  Diversion contrary to U.S. law prohibited. 
   
  All ideas, data and information contained in or disclosed by 
  this document are confidential and proprietary information of 
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved. 
  By accepting this material the recipient agrees that this material 
  and the information contained therein are held in confidence and in 
  trust and will not be used, copied, reproduced in whole or in part, 
  nor its contents revealed in any manner to others without the express 
  written permission of Qualcomm Technologies Incorporated. 
   
==============================================================================*/ 

/*==============================================================================
   
                        EDIT HISTORY FOR MODULE 
   
This section contains comments describing changes made to the module. 
Notice that changes are listed in reverse chronological order. 
 
$DateTime: 2016/12/13 07:59:25 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/rf/core/inc/rflte_core_rxctl.h#1 $ 
   
when       who     what, where, why 
--------   ---     ------------------------------------------------------------- 
07/31/14   ndb     Added support for extended EARFCN for LTE
04/18/13   vss     Add support for dynamic scell switch
11/15/12   pl      Update RxAGC Freq comp for intra-band CA support 
10/24/12   pl      Combined Rx gain freq comp into single re-entrant function
08/27/12   pl/sbm  Update to new device-based MDSP interfaces
11/04/11   pl      Added comdef.h for customer compilation
06/21/10   qma     Added OLTA support
05/21/10   qma     Initial version and added RX frequency compensation.
==============================================================================*/ 

#include "comdef.h"
#include "rflte_nv.h"
void rflte_core_rxctl_update_olta_rise_fall_to_mdsp( rfm_device_enum_type device, rflte_nv_tbl_type *lte_nv_tbl_ptr, uint32 carrier_idx );

void rflte_core_rxctl_update_rx_gain_freq_comp_to_mdsp
(
  rfm_device_enum_type device, 
  rfcom_lte_band_type band, 
  rfcom_lte_earfcn_type rx_chan, 
  rflte_nv_tbl_type * nv_tbl_ptr,
  boolean meas_nbr_flag,
  boolean intra_band_ca_flag,
  uint32 carrier_idx
);


#endif 
