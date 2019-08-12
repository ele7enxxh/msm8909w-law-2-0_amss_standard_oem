#ifndef FTM_CDMA_DISPATCH_H
#define FTM_CDMA_DISPATCH_H

/*!
  @file
  ftm_cdma_dispatch.h

  @brief
  This module defines the function prototypes and defintions used by FTM CDMA
  Dispatch.
*/

/*==============================================================================

  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/ftm/inc/ftm_cdma_dispatch.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/08/14   pk      Implementation of HDR FTM module
02/11/12   sb      added ftm_cmd.h
09/16/11   Saul    HDR non-sig support.
03/16/11   aro     Added C++ protection
01/15/11   aro     Added include file to make the header file self-sufficient
12/16/10   aro     Doxygen Update
12/07/10   aro     Initial Revision : CDMA Dispatch

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm.h"
#include "ftmdiag.h"
#include "rfm_device_types.h"
#include "ftm_cmd.h"
#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Data Structure for Data received from Diag to MC Dispatch
*/
typedef PACK(struct)
{

  diagpkt_subsys_header_type diag_hdr;
  /*!< Diag Header */

  ftm_cmd_header_type        ftm_hdr;
  /*!< FTM Header */

  ftm_packed_param_type      param;
  /*!< Packet Parameter */

} ftm_mc_pkt_type; /*!< Data Structure for Data received from Diag to MC Dispatch */

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_cdma_dispatch
(
  rfm_device_enum_type device,
  ftm_pkt_type  * ftm
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_cdma_cal_v2_dispatch
(
  void* req
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_cdma_radio_dispatch
(
  void  * ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_cdma_mc_dispatch
(
  ftm_mc_pkt_type *data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_hdr_mc_dispatch
(
  ftm_mc_pkt_type *ftm
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_hdr_dispatch
(
  rfm_device_enum_type device,
  ftm_pkt_type *ftm
);

#ifdef __cplusplus
}
#endif

#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FTM_CDMA_DISPATCH_H */
