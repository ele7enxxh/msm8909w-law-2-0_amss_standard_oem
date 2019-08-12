#ifndef MSMHWIOREG_PLATFORM_H
#define MSMHWIOREG_PLATFORM_H
/*!
  @file msmhwioreg_platform.h

  @brief
   A header file that will include the appropriate mhmhwioreg header file 
   depending on the platform. 

   Clients can include this file, and the registers should be properly defined
   for that platform.
  
*/

/*==============================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/common/inc/msmhwioreg_platform.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/04/09   sah     Initial Checkin

==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/



/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

// Todo, featurize and point to different headers depending on the build.

#include "lte_variation.h"
#ifndef FEATURE_DIME_MODEM
#include "msmhwioreg.h"
#endif

/* These definitions are missing from the SWI. Need to hand-edit the FLAT file
   next time so they get included in msmhwioreg.h */
#define HWIO_O_TX_ENCIB_MEM_BASE_MIN_ADDR            0xb3a00000
#define HWIO_O_TX_ENCIB_MEM_BASE_MAX_ADDR            0xb3a0F8FC


#endif /* MSMHWIOREG_PLATFORM_H */

