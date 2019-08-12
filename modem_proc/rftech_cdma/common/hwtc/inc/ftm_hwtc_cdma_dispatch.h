#ifndef FTM_HWTC_1XHDR_DISPATCH_H
#define FTM_HWTC_1XHDR_DISPATCH_H

/*!
  @file
  ftm_hwtc_cdma_dispatch.h

  @brief
  This module contains necessary definitions and function headers for the 1xHDR
  FTM dispatches.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/hwtc/inc/ftm_hwtc_cdma_dispatch.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/07/11   aro     Copyright Date Update
01/05/11   aro     Added Response Packets for HWTC CDMA
12/09/10   aro     Initial Release : FTM 1xHDR HWTC Dispatch Support

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE

#ifdef FEATURE_FTM_HWTC

#include "ftmdiag.h"

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_hwtc_cdma_dispatch
(
  void  * ftm_req_data
);

#endif /* FEATURE_FTM_HWTC */

#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FTM_HWTC_1XHDR_DISPATCH_H */
