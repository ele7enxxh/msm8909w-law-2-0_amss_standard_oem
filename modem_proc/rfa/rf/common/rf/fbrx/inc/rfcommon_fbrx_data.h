#ifndef RFCOMMON_FBRX_DATA_H
#define RFCOMMON_FBRX_DATA_H

/*!
  @file
  rfcommon_fbrx_data.h

  @brief
  This module is reponsible for handling FBRx Data
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/fbrx/inc/rfcommon_fbrx_data.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------------------------------
07/31/14   aro     Added structures to hold ILPC Data
07/31/14   aro     Initial Revision

==================================================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------------------------*/
/*! Structure to hold data pertaining to Power meter based based ILPC correction */
typedef struct
{
  boolean enable_ilpc;
  /*!< Flag indicating if ILPC is to be enabled or not */

  boolean override_disable_ilpc;
  /*!< Flag to indicate if the ILPC is to be overridden to disable mode */

} rfcommon_fbrx_ilpc_data_type;

/*------------------------------------------------------------------------------------------------*/
/*! Structure to hold FBRx state data on RF Common side */
typedef struct
{
  rfcommon_fbrx_ilpc_data_type ilpc_data;
  /*!< Structure to hold data pertaining to Power meter based based ILPC correction */

} rfcommon_fbrx_data_type;

#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_FBRX_DATA_H */

