/*****************************************************************************
*
* @file  secboot_hw.h (Secboot Hardware API)
*
* @brief Secboot Hardware API to get authentication information from the
*        Security Control fuses
*        
*
* Copyright (c) 2013 by QUALCOMM, Technology Inc.  All Rights Reserved.
*
*****************************************************************************/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/securemsm/secboot/hw/secctrl/msm8937/src/secboot_hw_apps.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/03/13   hw      Init version, Code update for bear pbl
============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "secboot_hw.h"
#include "secboot.h"

#ifdef USE_BOOT_UTILS
#include "boot_msmhwio.h"
#else
#include "HALhwio.h"
#include "msmhwiobase.h"
#endif

#include "msmhwioreg.h"
/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

secboot_hw_etype secboot_hw_get_root_cert_sel
(
  uint32* oem_root_cert_sel_ptr
)
{
  secboot_hw_etype ret_val = E_SECBOOT_HW_SUCCESS;

  if (NULL == oem_root_cert_sel_ptr)
  {
    ret_val = E_SECBOOT_HW_INVALID_PARAM;
    return ret_val;
  }

  *oem_root_cert_sel_ptr = HWIO_INF(QFPROM_CORR_ANTI_ROLLBACK_2_MSB, 
                                    ROOT_CERT_PK_HASH_INDEX);

  return ret_val;
}

