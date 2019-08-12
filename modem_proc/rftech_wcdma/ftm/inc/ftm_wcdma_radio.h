#ifndef FTM_WCDMA_RADIO_H
#define FTM_WCDMA_RADIO_H

/*!
  @file
  ftm_wcdma_radio.h

  @brief
  This module contains defintions and prototypes. FTM WCDMA Radio Control 
  code which is used to exclusively control Radio for test purposes.
*/

/*==============================================================================

  Copyright (c) 2013-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/ftm/inc/ftm_wcdma_radio.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/06/14   aro     Support to override TempComp mode of operation
05/15/14   aro     Interface to override TxPlim override mode
12/12/13   aro     Support to enable/disable HDET
10/22/13   vs      Add MultiCarrier Tune and Get RxAGC for multi carrier mode
10/12/13   zhh     Support set debug flag by QRCT
01/28/14   zhh     Update WCDMA Radio Test Interface 
12/12/13   aro     Support to enable/disable HDET
10/22/13   vs      Add MultiCarrier Tune and Get RxAGC for multi carrier mode
10/12/13   zhh     Support set debug flag by QRCT
07/30/13   aro     Initial Release : WCDMA Radio Dispatch

==============================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "ftmdiag.h"

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_wcdma_radio_trigger_agc_log
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
boolean 
ftm_wcdma_radio_enter_mode
(
  rfm_device_enum_type device,
  rfm_mode_enum_type rf_mode
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_wcdma_radio_pre_tune
(
  void	*ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_wcdma_radio_tune
(
  void	*ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_wcdma_radio_rx_wakeup
(
  void	*ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_wcdma_radio_rx_sleep
(
  void	*ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_wcdma_radio_tx_init
(
  void	*ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_wcdma_radio_tx_enable
(
  void	*ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_wcdma_radio_tx_disable
(
  void	*ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_wcdma_radio_tx_wakeup
(
  void	*ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_wcdma_radio_get_rxagc
(
  void	*ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_wcdma_radio_override_plim
(
  void *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_wcdma_radio_override_tempcomp
(
  void *ftm_req_data
);


#ifdef __cplusplus
}
#endif

#endif /* FTM_WCDMA_RADIO_H */


