#ifndef FTM_HDR_MC_H
#define FTM_HDR_MC_H

/*!
  @file
  ftm_hdr_mc.h

  @brief
  APIs from ftm_hdr_mc.c

  @{
*/

/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/ftm/inc/ftm_hdr_mc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/04/13   APU     Brought over FTM HDR radio Tx control from Dime. 
08/20/13   zhw/sty Update FTM HDR radio Tx control interface
06/15/12   zhw     Initial revision.

==============================================================================*/
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

void ftm_hdr_mc_control_modulator
(
  uint8 carrier_mask,
  boolean skip_dac_toggle
);

#ifdef __cplusplus
}
#endif

#endif /* FTM_HDR_MC_H */

