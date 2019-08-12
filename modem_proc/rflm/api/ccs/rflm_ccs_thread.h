#ifndef RFLM_CCS_THREAD_H
#define RFLM_CCS_THREAD_H

/*!
  @file
  rflm_ccs_thread.h

  @brief
  RFLM CCS IST Thread.

*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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
/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/ccs/rflm_ccs_thread.h#1 $
$Date: 2016/12/13 $
when         who     what, where, why
--------   ---     -----------------------------------------------------------
05/22/14   ljl      Initial
==============================================================================*/


 
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/ 
#include "rflm.h"

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/



/*===========================================================================

                                FUNCTIONS

===========================================================================*/

extern void rflm_ccs_thread_init
(
  uint32 int_idx
);


#ifdef __cplusplus
}
#endif


#endif  // RFLM_CCS_THREAD_H

