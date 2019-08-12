#ifndef FTM_CDMA_RADIO_H
#define FTM_CDMA_RADIO_H

/*!
  @file
  ftm_cdma_radio.h

  @brief
  This module contains defintions and prototypes. FTM CDMA Radio Control 
  code which is used to exclusively control Radio for test purposes.
*/

/*==============================================================================

  Copyright (c) 2011, 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/ftm/inc/ftm_cdma_radio.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/22/14   sty     Added ftm_cdma_radio_set_antenna()
03/20/14   spa     Initial support to test Rx on/off on RFLM side
08/19/13   spa     Added support for common CDMA enter mode (1x/HDR/SB)
11/22/11   aro     Added interface to disable iC for debug purpose
11/12/11   aro     Added CDMA driver verification sandbox function
11/12/11   aro     Added command to configure debug mode for CDMA 
11/12/11   aro     Added radio command to get current ic power mode
11/12/11   aro     Added radio command to clear ic power mode override
11/12/11   aro     Added radio command to override ic power mode
11/11/11   aro     Initial Release : CDMA Radio Dispatch

==============================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "ftmdiag.h"

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_cdma_radio_power_mode_override
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_cdma_radio_clear_power_mode_override
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_cdma_radio_get_power_mode
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_cdma_radio_configure_ic_operation
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_cdma_driver_verification_sandbox
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_cdma_radio_config_debug_mode
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_cdma_radio_enter_mode
(
  rfm_device_enum_type device,
  rfm_mode_enum_type rf_mode
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_cdma_radio_test_rx_on_off
(
   void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_cdma_radio_set_antenna
(
  rfm_device_enum_type device
);

#ifdef __cplusplus
}
#endif

#endif /* FTM_CDMA_RADIO_H */

