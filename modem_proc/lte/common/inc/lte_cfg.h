/*!
  @file
  lte_cfg.h

  @brief
  This file contains definitions for the features that LTE Access Stratum(AS)
  may need.

*/

/*=============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/common/inc/lte_cfg.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/25/11   ar      added FEATURE_LTE_NIKEL_REDUCED_HARQS
09/17/10   wshaw   add watchdog support for LTE tasks
12/11/09   vatsac  initial version
=============================================================================*/

#ifndef LTE_CFG_H
#define LTE_CFG_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/

#include "lte_variation.h"
#include "customer.h"

/*=============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

=============================================================================*/

/*! @brief Feature for reduced number of uplink HARQs. This feature should be
 * enabled for all chips with Nikel modem
*/
#define FEATURE_LTE_NIKEL_REDUCED_HARQS

#endif /* LTE_CFG_H */

