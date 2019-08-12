#ifndef RF_1X_EPT_H
#define RF_1X_EPT_H

/*! 
  @file
  rf_1x_ept.h
 
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/inc/rf_1x_ept.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
04/10/15   vr      Add support for XPT cal when Drx path not present
12/26/13   sar/hdz Remove et_txlm_hanlde from Bolt c2k code, and use one single 
                   TxLm handle allocation.
07/19/13   Saul    XPT. ET delay vs freq vs temp support.
06/03/13   spa     Migrate to new RF dispatch for XPT FW response mssgs 
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
03/19/13   aro     Deleted ET config path
03/19/13   aro     Modularized FB config into transceiver, asm, and rxlm
03/11/13   spa     Removed API rf_1x_pa_params_config_done_response  
02/25/13   aro     Updated XPT configure interface to pass XPT mode from
                   calling function
02/21/13   aro     Updated configure XPT interface to push band and NV chain
                   parameter for calling function
02/21/13   aro     CDMA Tx Resource manager implementation
02/04/13   Saul    CDMA. Reverted new 1XHDR EPT/ET architecture.
02/01/13   Saul    CDMA. HDR EPT/ET call support. New 1XHDR EPT/ET architecture.
01/22/13   aro     Code to use unified IQ capture param
01/22/13   aro     Added common XPT capture type
01/22/13   aro     Removed DPD index from IQ capture interface
01/18/13   aro     Renamed function name to have XPT
01/18/13   aro     Deprecated XPT polling code
01/18/13   aro     Deleted nikel-XPT deprecated FW responses
01/18/13   aro     Deleted nikel-XPT specific code
01/14/13   Saul    CDMA. 1x EPT/ET call support.
12/19/12   cd      Fixes for ET Tx configuration in FTM mode only
12/19/12   Saul    CDMA. TX AGC XPT Dev. Use xpt cfg msg in online.
12/19/12   cd      Fixed compiler warning
12/18/12   cd      Changes to support xPT config response handling
11/16/12   Saul    Sample Capture. Use common response.
11/08/12   cd      Added functionality to configure ET path
10/25/12   cd/Saul Generalized load DPD table API
                   - Changed request packet type
                   - Added response packet type
                   - Made XPT IQ capture common.
10/03/12   Saul    CDMA Sample Capture. Capture done message.
09/19/12   Saul    CDMA Sample Capture. Capture type, do leakage flag, 
                   stop tx agc update fw command
09/10/12   aro     SNUM dispatch callback function for IQ Capture and
                   DPD expansion
09/04/12   aro     Added band and channel to EPT FB config interface
08/31/12   Saul    CDMA. Added support for EPT_PROCESSING_DONE FW message
08/22/12   cri     EPT support for CDMA FW interfaces
08/08/12   vs      Added support to query EPT config params
07/25/12   aro     DPD index in IQ Capture command
07/20/12   aro     Added support to do polling for EPT expansion
07/17/12   aro     Enhanced support of configure feedback path
07/09/12   aro     Moved EPT responses to EPT module
06/26/12   aro     Deleted unused parameter from EPT configure function
06/26/12   aro     Deleted IQ Capture init function
06/26/12   aro     Renamed function to FB configure
06/26/12   aro     Updated variable to flag Expansion at the start of IQ capture
06/25/12   aro     Re-arranged API
06/25/12   aro     Added code to re-initialize EPT variables
06/25/12   aro     Added code to load EPT config param from NV
06/25/12   aro     Fixed bug where RxLM buffer was not getting updated
06/25/12   aro     Added error checking in EPT override
06/14/12   aro     Added check in IQ Cpature function to prevent sending IQ
                   capture command when last process is still pending
06/13/12   aro     Fixed single instance IQ Capture
06/13/12   aro     Implemented DPD coefficient interface
06/13/12   aro     Update EPT configure, iq capture, and override functions
                   to have devices
06/13/12   aro     Removed unrelevant fields from EPT payloads
06/11/12   aro     Added MDSP path/device to EPT configure function
06/11/12   aro     Function ordering
06/11/12   aro     Removed redundant EPT configure function
06/08/12   aro     Updated EPT IQ capture interface to pass device

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfcommon_core_xpt.h"
#include "lm_types.h"
#include "rf_fwrsp_task.h"
#include "rfm_cdma_band_types.h"
#include "rfm_1x_ftm.h"
#include "rf_dispatch_snum.h"

/*----------------------------------------------------------------------------*/
boolean
rf_1x_xpt_configure_fb_rxlm
(
  rfm_device_enum_type device, 
  rfm_device_enum_type device_fb,
  lm_handle_type rxlm_handle,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_xpt_cal_configure
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_xpt_iq_capture
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb,
  rfm_1x_iq_capture_param_type capture_param,
  uint32 * samp_rate
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_xpt_configure
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
rf_1x_xpt_config_done_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_xpt_trans_complete_rsp_cb
(
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

#endif /* RF_1X_EPT_H */