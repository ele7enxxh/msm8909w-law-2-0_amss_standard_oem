#ifndef MBA_AC_H
#define MBA_AC_H
 /**===========================================================================
 **
 ** FILE 
 **     mba_access_control.c
 ** 
 ** GENERAL DESCRIPTION 
 **     This file provides the APIs to use QTimers within
 **     the Modem subsystem. All QTimer functionality is based
 **     on a global system counter which runs at 19.2 Mhz.                                        
 ** 
 **==========================================================================*/


/*=============================================================================


                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/mba/mba_ac.h#1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  07/01/12   mm      Initial Revision for MSM8974
 
=============================================================================*/


/*=============================================================================
                                MODULE INCLUDES
                          ADD NEW ONES UNDER THIS LINE    
=============================================================================*/
#include <IxErrno.h>
#include "HALxpu2.h"
#include <string.h>

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

#define MSA_AC_PAGE_4K           4096
/* Clears the lowest 12 bits on xx. */
#define MSA_AC_FLOOR_4K(xx)      ((xx) & ~(MSA_AC_PAGE_4K-1))

#define MBA_READ_DDR_SIZE_FAIL 0x1

#define MBA_XPU_LOCK_INITIALIZE_FAILURE 0x2
#define MBA_XPU_LOCK_CONFIG_INFO_READ_FAILURE_DDR0 0x3
#define MBA_XPU_LOCK_CONFIG_INFO_READ_FAILURE_DDR1 0x4
#define MBA_XPU_LOCK_REQ_OVERLAPS_LOCKED_REGION 0x5
#define MBA_XPU_LOCK_INVALID_RESOURCE_INSTANCE 0x6

#define MBA_XPU_LOCK_NO_BANK_TO_LOCK 0x7
#define MBA_XPU_LOCK_CONFIG_RESOURCE_FAIL_DDR0 0x8
#define MBA_XPU_LOCK_CONFIG_RESOURCE_FAIL_DDR1 0x9
#define MBA_XPU_LOCK_ENABLE_PARTITION_FAIL_DDR0 0xA
#define MBA_XPU_LOCK_ENABLE_PARTITION_FAIL_DDR1 0xB

#define MBA_XPU_LOCK_CONFIG_RESOURCE_FAIL_BOTH_DDR0 0xD
#define MBA_XPU_LOCK_ENABLE_PARTITION_FAIL_BOTH_DDR0 0xE
#define MBA_XPU_LOCK_CONFIG_RESOURCE_FAIL_BOTH_DDR1 0xF
#define MBA_XPU_LOCK_ENABLE_PARTITION_FAIL_BOTH_DDR1 0x10

#define MBA_XPU_UNLOCK_CONFIG_INFO_READ_FAILURE_DDR0 0x11
#define MBA_XPU_UNLOCK_INVALID_RESOURCE_INSTANCE 0x12
#define MBA_XPU_UNLOCK_DISABLE_PARTITION_DDR0_FAIL 0x13
#define MBA_XPU_UNLOCK_CONFIG_INFO_READ_FAILURE_DDR1 0x14
#define MBA_XPU_UNLOCK_DISABLE_PARTITION_DDR1_FAIL 0x15


/* Modem EFS Partition */
typedef enum resource_id_s
{
  MBA_XPU_PARTITION_PMI=0,
  MBA_XPU_PARTITION_MODEM_EFS = 1,
  MBA_XPU_PARTITION_MODEM_UNCACHED_HEAP = 2,
  MBA_XPU_PARTITION_FTM = 3,
 }resource_id_type;


int mba_xpu_initialize(void);

int mba_xpu_lock_region(uint32 start, uint32 end, resource_id_type resource_id);

uint8 mba_seccrtl_allow_unlock_xpu(void);

int mba_xpu_scribble_unlock_region(resource_id_type resource_id);

#endif

