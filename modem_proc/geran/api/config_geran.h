/*!
  @file config_geran.h

  @brief Controls backwards-compatibility of GERAN Component Interface Decoupling
*/

/*===========================================================================

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/config_geran.h#1 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------
11-02-07   tjw       Initial Revision
===========================================================================*/
/*
GERAN Component Interface Decoupling
====================================

This file was used to control backwards-compatibility during the steps towards
stripping the legacy GERAN API down to a true API.  It's work is done.

*/
#ifndef CONFIG_GERAN_H
#define CONFIG_GERAN_H

#include "customer.h"

#if ((defined(__ARMCC_VERSION) && (__ARMCC_VERSION > 200000)) || \
(defined(__GNUC__) && \
    (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))))
  /* Use #warning to generate warnings */
  #define COMPILER_IS_GCC_OR_RVCT
  /* else use #pragma message() to generate warnings */
#endif

#endif /* CONFIG_GERAN_H */
