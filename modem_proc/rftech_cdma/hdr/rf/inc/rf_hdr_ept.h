#ifndef RF_HDR_EPT_H
#define RF_HDR_EPT_H

/*! 
  @file
  rf_hdr_ept.h
 
  @brief
  This file defines the 1x interfaces to be used for Envelope Power Tracking. 
*/

/*==============================================================================

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/rf/inc/rf_hdr_ept.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/26/13   sar/hdz Remove et_txlm_hanlde from Bolt c2k code, and use one single 
                   TxLm handle allocation.
07/19/13   Saul    XPT. ET delay vs freq vs temp support.
06/03/13   spa     Migrate to new RF dispatch for XPT FW response mssgs 
02/25/13   aro     Updated XPT configure interface to pass XPT mode from
                   calling function
02/21/13   aro     Updated configure XPT interface to push band and NV chain
                   parameter for calling function
02/21/13   aro     CDMA Tx Resource manager implementation
02/01/13   Saul    CDMA. HDR EPT/ET call support. New 1XHDR EPT/ET architecture.
01/15/13   Saul    CDMA. HDR EPT/ET call support. 
12/19/12   cd      Fixes for ET Tx configuration in FTM mode only
12/19/12   Saul    CDMA. TX AGC XPT Dev. Use xpt cfg msg in online.
07/19/12   cri     Initial revision

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfcommon_core_xpt.h"
#include "lm_types.h"
#include "rf_fwrsp_task.h"
#include "rf_hdr_fw_response.h"
#include "rfm_cdma_band_types.h"
#include "rf_cdma_nv.h"
#include "rf_dispatch_snum.h"

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_xpt_configure
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan,
  rf_cdma_nv_path_type tx_nv_path,
  lm_handle_type prim_txlm_handle,
  xpt_cfg_type xpt_mode
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_xpt_config_done_rsp_cb
(
   void *cmd_ptr,
   const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_xpt_trans_complete_rsp_cb
(
   void *cmd_ptr,
   const struct rf_dispatch_snum_info_type_t *snum_data
);

#endif /* RF_HDR_EPT_H */
