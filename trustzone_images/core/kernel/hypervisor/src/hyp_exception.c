/**
@file tzbsp_exception.c
@brief Exception handle functions for Hypervisor
*/
/*===========================================================================
   Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/hypervisor/src/hyp_exception.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
03/12/14   dc       Created

===========================================================================*/


/*===========================================================================

          INCLUDE FILES

============================================================================*/

#include <comdef.h>
#include <stdio.h>
#include <string.h>  /* for memset */
#include <stdint.h>
#include "hyp_asm.h"
#include "hyp_log.h"
#include "hyp_mm.h"
#include "hyp_hvcapi.h"

/*=========================================================================== 
    Preprocessor Definitions and Constants
============================================================================*/
#define SPSR_T_BIT_SHFT 5
#define SPSR_J_BIT_SHFT 24
/*===========================================================================

          FUNCTIONS

============================================================================*/
// Workaround of erratum 814270
// AArch32 EL0/EL1 won't access 
// 0x0000000000000000-0x0000FFFFFFFFFFFF or 
// 0xFFFF000000000000-0xFFFFFFFFFFFFFFFF
// It has misaligned PC only in this erratum
uint32 hyp_check_if_forward_misaligned_pc(uint32 hsr, uint32 hifar, 
                                          uint32 ttbcr, uint32 spsr_hyp)
{
  uint32 result, spsr_mode, spsr_t, spsr_j;

  spsr_mode = spsr_hyp & CPSR_MODE_MASK;
  spsr_t  = (spsr_hyp >> SPSR_T_BIT_SHFT) & 0x1; /* SPSR.T */
  spsr_j  = (spsr_hyp >> SPSR_J_BIT_SHFT) & 0x1; /* SPSR.J */

  if (HYP_Mode_HYP == spsr_mode)
  {
     /* Exception happened in HYP itself, not forwad */
    return 0;
  }

  if (!spsr_t && !spsr_j)
  {
     /* ARM Mode */
     if (!(hifar & 0x3) ) return 0;
  }
  else if (spsr_t && !spsr_j)
  {
     /* Thumb Mode */
     if (!(hifar & 0x1) ) return 0;
  }
  else if (spsr_j)
  {
     /* Jazelle Mode */
     /* forward */
  }

  if (ttbcr & TTBCR_EAE_BIT)
  {
    result = HYP_IFSR_WHEN_EAE1;
  }
  else
  {
    result = HYP_IFSR_WHEN_EAE0;
  }

  HYP_LOG_ERR("Misaligned PC=%d,HSR(%x),SPSR_hyp(%x),HIFAR(%x)", 
              result, hsr, spsr_hyp, hifar);
  
  return result;
}


int32 hyp32_hvc_handler(int32 hvc_id,
                        int32 parm1, int32 parm2, int32 parm3,
                        int32 parm4, int32 parm5)
{
   switch (hvc_id)
   {
   case HVC32_GET_VERSION:
      return HVC_VERSION_INFO;
   case HVC32_GRANT_ACCESS:
      return hyp32_grant_access((uint64_t)parm1, (uint64_t)parm2, 
										  (uint64_t)parm3, (uint64_t)parm4, 
										  (uint64_t)parm5);
   case HVC32_REMOVE_ACCESS:
      return hyp32_remove_access((uint64_t)parm1, (uint64_t)parm2);
   default:
      return HVC_CALL_NON_SUPPORT_ID;
   }
}
