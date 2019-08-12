#ifndef RFM_FTM_H
#define RFM_FTM_H
/*! 
  @file
  rfm_ftm.h
 
  @brief
  This file defines the RFM interfaces to be used by <b>FTM module only</b>.
*/

/*==============================================================================

  Copyright (c) 2013 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/mc/inc/rfm_ftm.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/06/14   aro     Support to override ILPC mode of operation
11/14/13   cd      Support for RFLM-based Tx AGC override
03/05/13   aro     Enhanced XPT override command and migrated to FTM common
                   dispatch
01/23/13   aro     Command to perform slave resource configuration in 
                   Tx resource manager
01/23/13   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfcommon_core_xpt.h"

/*----------------------------------------------------------------------------*/
boolean
rfm_ftm_configure_xpt_override
(
  rfm_device_enum_type device,
  boolean override_en,
  xpt_cfg_type xpt_mode
);

/*----------------------------------------------------------------------------*/
boolean
rfm_ftm_apply_xpt_tx_agc_override
(
  rfm_mode_enum_type rfm_mode,
  rfm_device_enum_type device,
  rfcom_band_type_u rfm_band
);

/*----------------------------------------------------------------------------*/
boolean
rfm_ftm_fbrx_iplc_override
(
  rfm_device_enum_type device,
  boolean ilpc_enable
);

#endif /* RFM_FTM_H */
