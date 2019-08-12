/*!
  @file
  rflm_features.h

  @brief
  RFLM Featurization Definitions

  @detail
  This file contains non-modem/target featurization macros

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/includes/inc/rflm_features.h#1 $
when         who     what, where, why
----------   ---     --------------------------------------------------------
2014-08-11   CJ      Feature flag definition correction
2014-08-10   CJ      TX sharing flag added under common featue flag definition
2013-07-18   JC      Created

===========================================================================*/

#ifndef RFLM_FEATURES_H
#define RFLM_FEATURES_H

#ifdef FEATURE_MODEM_TX_SHARING
#define FEATURE_TX_SHARING
#endif

/*===========================================================================

                   CONSTANTS

===========================================================================*/

#define RFLM_FEATURE_BUILD_NORMAL         0
#define RFLM_FEATURE_BUILD_STANDALONE     1
#define RFLM_FEATURE_BUILD_FW_LIB         2

#ifndef RFLM_FEATURE_BUILD_MODE
#define RFLM_FEATURE_BUILD_MODE           RFLM_FEATURE_BUILD_NORMAL
#endif

#endif /* RFLM_FEATURES_H */

