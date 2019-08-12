#ifndef FTM_HWTC_CDMA_IQ_CAPTURE_H
#define FTM_HWTC_CDMA_IQ_CAPTURE_H

/*! 
  @file
  ftm_hwtc_cdma_iq_capture.h
 
  @brief
  This file contains prototypes and definitions pertaining to IQ Capture.
*/

/*==============================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/hwtc/inc/ftm_hwtc_cdma_iq_capture.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/24/11   aro     Re-structured IQ Capture to have generic "Get Config"
12/20/10   aro     Initial Revision.

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"


#ifdef FEATURE_FACTORY_TESTMODE

#ifdef FEATURE_FTM_HWTC

#include "ftm_hwtc_iqcapture.h"
#include "ftm.h"

/*----------------------------------------------------------------------------*/
void
ftm_hwtc_cdma_set_iq_capture_antenna
(
  enum_iq_antenna_type ant
);

/*----------------------------------------------------------------------------*/
void
ftm_hwtc_cdma_get_iq_capture_data
(
  ftm_hwtc_iq_data_type *data
);

#endif /* FEATURE_FTM_HWTC */

#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FTM_HWTC_CDMA_IQ_CAPTURE_H */

