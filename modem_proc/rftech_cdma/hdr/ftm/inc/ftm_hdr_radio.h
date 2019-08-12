#ifndef FTM_HDR_RADIO_H
#define FTM_HDR_RADIO_H

/*!
  @file
  ftm_hdr_radio.h

  @brief
  This module contains defintions and prototypes. FTM 1x Radio Control 
  code which is used to exclusively control Radio for test purposes.
*/

/*==============================================================================

  Copyright (c) 2010 - 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/ftm/inc/ftm_hdr_radio.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/21/11   aro     Added HDR Enable and disable radio functions
08/12/11   aro     Added support for HDR LNA override clear
06/13/11   sty     Added prototype for ftm_hdr_radio_disable_div
05/05/11   sty     Added prototype for ftm_hdr_radio_enable_div
03/07/11   aro     Added support for HDR retune control from FTM
02/13/11   aro     Added Set Tx Power Interface
02/13/11   aro     [1] Updated LNA query interface
                   [2] Added support for Get TxAGC
02/12/11   aro     Deleted Get LNA state interface to merge with Get RxAGC
02/08/11   aro     Moved HDR Radio Control to FTM
02/04/11   sty     Added ftm_hwtc_hdr_radio_sleep_tx and 
                   ftm_hwtc_hdr_radio_sleep_rx
02/03/11   sty     Added ftm_hwtc_hdr_radio_wakeup_tx
01/24/11   aro     Added LNA Range Query Interface
01/20/11   aro     Initial Release : Based on SVDO Design

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftmdiag.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_hdr_radio_enter
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_hdr_radio_enable
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_hdr_radio_disable
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_hdr_radio_wakeup_rx
(
  void *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_hdr_radio_enable_div
(
  void *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_hdr_radio_disable_div
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_hdr_radio_wakeup_tx
(
  void *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_hdr_radio_sleep_tx
(
  void *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_hdr_radio_set_tx_power
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_hdr_radio_sleep_rx
(
  void *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_hdr_radio_retune
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_hdr_radio_clear_lna_override
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_hdr_radio_get_rx_agc
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_hdr_radio_get_tx_agc
(
  void  *ftm_req_data
);

#ifdef __cplusplus
}
#endif

#endif /*FEATURE_FACTORY_TESTMODE */

#endif /* FTM_HDR_RADIO_H */

