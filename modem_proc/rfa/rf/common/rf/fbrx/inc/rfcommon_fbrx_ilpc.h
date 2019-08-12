#ifndef RFCOMMON_FBRX_ILPC_H
#define RFCOMMON_FBRX_ILPC_H

/*!
  @file
  rfcommon_fbrx_ilpc.h

  @brief
  This module is reponsible for Power meter based ilpc correction
*/

/*==================================================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document are confidential 
  and proprietary information of Qualcomm Technologies Incorporated and all rights therein are 
  expressly reserved. By accepting this material the recipient agrees that this material and the 
  information contained therein are held in confidence and in trust and will not be used, copied, 
  reproduced in whole or in part, nor its contents revealed in any manner to others without the 
  express written permission of Qualcomm Technologies Incorporated.

==================================================================================================*/

/*==================================================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/fbrx/inc/rfcommon_fbrx_ilpc.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------------------------------
08/15/14   aro/gh  Add API for FBRX ILPC sample capture
07/31/14   aro     Added structures to hold ILPC Data
07/31/14   aro     Initial Revision

==================================================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rflm_api_cmn.h"

/*------------------------------------------------------------------------------------------------*/
boolean
rfcommon_fbrx_ilpc_setup
(
  rfm_device_enum_type device,
  rflm_handle_tx_t tx_handle,
  rflm_handle_rx_t fbrx_handle
);

/*------------------------------------------------------------------------------------------------*/
boolean
rfcommon_fbrx_iplc_override_disable
(
  void
);

/*------------------------------------------------------------------------------------------------*/
boolean
rfcommon_fbrx_iplc_override_enable
(
  void
);

/*------------------------------------------------------------------------------------------------*/
boolean
rfcommon_fbrx_ilpc_config_sample_capture
(
  rfm_device_enum_type device,
  rflm_handle_tx_t tx_handle,
  rflm_handle_rx_t fbrx_handle
);

#endif /* RFCOMMON_FBRX_ILPC_H */



