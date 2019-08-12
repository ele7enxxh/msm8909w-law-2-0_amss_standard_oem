/*===========================================================================

                    Crypto Engine Core Environment

GENERAL DESCRIPTION

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE
 
  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/securemsm/cryptodrivers/ce/chipset/msm8909/inc/CeCL_Env.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/29/12   ejt     original file
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "CeCL_Target.h"

/*===========================================================================
                 DEFINITIONS AND TYPE DECLARATIONS
===========================================================================*/

// Enable BAM for Device Programmer image
#ifdef FEATURE_FIREHOSE_ENABLE_CRYPTO_BAM
   #define CECL_BAM_MODE_IS_POLLING() 1
   #define CECL_BAM_IS_SUPPORTED() 1
   #define CECL_NON_BLOCKING_HASH_IS_SUPPORTED() 1
#else
   #define CECL_BAM_MODE_IS_POLLING() 0
   #define CECL_BAM_IS_SUPPORTED() 0
   #define CECL_NON_BLOCKING_HASH_IS_SUPPORTED() 0
#endif
