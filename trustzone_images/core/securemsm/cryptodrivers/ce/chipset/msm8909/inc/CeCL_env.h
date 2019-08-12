/*===========================================================================

                    Crypto Engine Core Environment

GENERAL DESCRIPTION

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE
 
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/cryptodrivers/ce/chipset/msm8909/inc/CeCL_env.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/23/14   nk      Enable BAM
02/03/14   nk      Disable BAM for bringup
04/29/10   amen    added for crypto sharing in 9x25
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "CeCL_Target.h"

/*===========================================================================
                 DEFINITIONS AND TYPE DECLARATIONS
===========================================================================*/
//Only enable CEEL_BAM_IS_SUPPORTED or CEEL_BAM_CE_SHARING_SUPPORTED or neither. 
//BAM_IS_SUPPORTED is for non sharing crypto
//CE_SHARING_SUPPORTED is for sharing crypto

#define CEEL_BAM_IS_SUPPORTED() 0

#define CEEL_BAM_CE_SHARING_SUPPORTED() 1

#if CEEL_BAM_IS_SUPPORTED() == 0 && CEEL_BAM_CE_SHARING_SUPPORTED() == 0
    #define CEEL_REG_MODE_EN 1
#else
    #define CEEL_REG_MODE_EN 0
#endif

//Define CE/BAM V1 HW version numbers
#define CEEL_V1_MAJ_VER 5
#define CEEL_V1_MIN_VER 0

//Set up clocks
#define CeElClkEnable()   0//CeClClockEnable()
#define CeElClkDisable()  0//CeClClockDisable()

//Define CE/BAM V1 HW version macro. Resolves to true if HW is V1
#define CEEL_CE_HW_IS_V1() ((HWIO_INF(CECL_CE_VERSION, MAJ_VER) == CEEL_V1_MAJ_VER) && \
                            (HWIO_INF(CECL_CE_VERSION, MIN_VER) == CEEL_V1_MIN_VER))
