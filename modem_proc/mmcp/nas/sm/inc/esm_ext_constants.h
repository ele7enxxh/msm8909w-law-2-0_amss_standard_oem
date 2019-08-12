/*!
  @file
  esm_ext_constants.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*==============================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/inc/esm_ext_constants.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/24/09   zren    Updated PDN_CONNECTIVITY and PDN_DISCONNECT features 
05/19/09   MNK     Constants needed in NAS integration tests, so exposing them
==============================================================================*/

#ifndef ESM_EXT_CONSTANTS_H
#define ESM_EXT_CONSTANTS_H

#include "mmcp_variation.h"
#include<customer.h>
#ifdef  FEATURE_LTE
/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "comdef.h"

const word ESM_BEARER_RSRC_ALLOC_TIMER_VAL = 8*1000;
const word ESM_BEARER_RSRC_MODIFY_TIMER_VAL = 8*1000;
const word ESM_PDN_CONNECTIVITY_TIMER_VAL =  8*1000;
const word ESM_PDN_DISCONNECT_TIMER_VAL =  6*1000;


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief External type for my clients
*/

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/


#endif /* ESM_EXT_CONSTANTS_H */

#endif /* FEATURE_LTE */
