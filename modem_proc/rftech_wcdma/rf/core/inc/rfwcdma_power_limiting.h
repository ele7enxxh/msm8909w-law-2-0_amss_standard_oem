#ifndef RFWCDMA_POWER_LIMITING_H
#define RFWCDMA_POWER_LIMITING_H
/*!
  @file
  rfwcdma_power_limiting.h

  @brief
  This module is reponsible for handling Tx Power limiting operation
*/

/*==================================================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document are confidential 
  and proprietary information of Qualcomm Technologies Incorporated and all rights therein are 
  expressly reserved. By accepting this material the recipient agrees that this material and the 
  information contained therein are held in confidence and in trust and will not be used, copied, 
  reproduced in whole or in part, nor its contents revealed in any manner to others without the 
  express written permission of Qualcomm Technologies Incorporated.

==================================================================================================*/

/*==================================================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/core/inc/rfwcdma_power_limiting.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------------------------------
05/01/14   aro     Initial Release

==================================================================================================*/

#include "comdef.h"

/*------------------------------------------------------------------------------------------------*/
/*! Structure to hold the Power Limiting Data for WCDA */
typedef struct
{
  boolean enable_plim;
  /*!< Flag to enable the Tx power limiting Loop */

} rfwcdma_power_limiting_data_type;


#endif /* RFWCDMA_POWER_LIMITING_H */

