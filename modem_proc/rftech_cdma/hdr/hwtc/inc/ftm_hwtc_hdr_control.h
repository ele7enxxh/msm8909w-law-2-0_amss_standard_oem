#ifndef FTM_HWTC_HDR_CONTROL_H
#define FTM_HWTC_HDR_CONTROL_H

/*!
  @file
  ftm_hwtc_hdr_control.h

  @brief
  This module contains defintions and prototypes to be used by CDMA HDR HWTC.
*/

/*==============================================================================

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/hwtc/inc/ftm_hwtc_hdr_control.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/25/12   Saul    MCDO/FTM. Added DVGA offset dynamic configuration.
01/19/12   Saul    MCDO/FTM. Added ICIFIR configuration override.
09/27/11   aro     Added support to set Tx DP IQ Gain from FTM
09/27/11   aro     Initial Release

==============================================================================*/

#include "comdef.h"
#include "ftmdiag.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_hwtc_hdr_set_tx_datapath_iq_gain
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_hwtc_hdr_update_icifir_items
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_hwtc_hdr_update_dvga_items
(
  void  *ftm_req_data
);

#ifdef __cplusplus
}
#endif

#endif /* FTM_HWTC_HDR_CONTROL_H */

