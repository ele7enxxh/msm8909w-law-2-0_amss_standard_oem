#ifndef FTM_HDR_MEAS_H
#define FTM_HDR_MEAS_H

/*!
  @file ftm_hdr_meas.h

  @brief
  This file contains all declarations and definitions necessary to use
  the HDR FTM Measurement Main Control.

  @details

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/ftm/inc/ftm_hdr_meas.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/19/13   spa     Initial Check-in
=============================================================================*/
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
void 
ftm_hdr_meas_register
(
  void
);

/*----------------------------------------------------------------------------*/
void 
ftm_hdr_meas_deregister
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* FTM_HDR_MEAS_H */


