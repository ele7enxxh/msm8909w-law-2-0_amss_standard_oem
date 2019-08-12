/*
===========================================================================

FILE:         HALmpmintRegisters.c

DESCRIPTION:  
  Per Target register layout definitions

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/mpm/hal/source/8909/HALmpmintRegisters.c#1 $
$Date: 2015/02/25 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
             Copyright (c) 2012 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
              QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "HALmpm.h"
#include "HALmpmintInternal.h"
#include "HALhwio.h"
#include "tlmm_hwio.h"
#include "mpm2_hwio.h"

/* We need to define the appropriate MPM registers, which can vary depending
 * on whather we need to talk to the actual MPM or to the virtual MPM, and
 * in the case of the virtual MPM it depends on the processor. */

HAL_mpmint_PlatformType registerLayout =
{
  /*.nEnableReg     = */HWIO_MPM2_MPM_INT_EN_1_ADDR,
  /*.nEnableRegPad  = */HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR,
  /*.nStatusReg     = */HWIO_MPM2_MPM_INT_STATUS_1_ADDR,
  /*.nClearReg      = */HWIO_MPM2_MPM_INT_CLEAR_1_ADDR,
  /*.nDetectReg     = */HWIO_MPM2_MPM_DETECT_CTL_1_ADDR,
  /*.nDetectReg3    = */HWIO_MPM2_MPM_DETECT_CTL_3_ADDR,
  /*.nPolarityReg   = */HWIO_MPM2_MPM_INT_POLARITY_1_ADDR
};

