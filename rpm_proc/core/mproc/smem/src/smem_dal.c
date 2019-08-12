/*===========================================================================

                         S H A R E D   M E M O R Y

DESCRIPTION
    Defines the OS and API specific helper functions for SMEM API.  DAL is
    fully supported by all ModemSW and 8974 ADSP, which both also happen to have
    equivalent memory-mapping requirements.

Copyright (c) 2012-2013 by Qualcomm Technologies, Incorporated.  All
Rights Reserved.

===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/22/13   rv      VA mapping is added for HW WONCE region.
11/19/13   bt      Rename IMEM defines to more-generic target info.
05/31/13   bt/bm   Use DAL to map SMEM region.
04/24/13   pa      Added smem_malloc().
02/08/13   bt      KW error: Add assertion for return of DAL_DeviceAttach.
01/09/13   pa      Added smem_set_spin_lock_api.
12/04/12   bt      Separate smem_map_memory_va into Qurt and Rex versions.
10/05/12   bt      Resolve Klocwork error in reference of hwio_mutexes_handle.
06/14/12   bm      Remove macro SMEM_RPM_MSG_RAM_BASE_OFFSET
03/19/12   bm      Fix smem_map_memory_va for returning correct virtual addr
                   for RPM message RAM.
01/20/12   bt      Integrated smem_map_memory_va, replacing smem_map_base_va.
02/21/12   bt      Add timetick DAL OS abstractions.
02/14/12   bt      Initial revision, based on best of smem_qube + smem_rex.
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "smem_v.h"
#include "smem_target.h"
#include "smem_os_common.h"
#include "smem_targ_info.h"

#include "DDIHWIO.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DDITimetick.h"
#ifndef SMEM_RPM_PROC
#include "DDIChipInfo.h"
#endif
#include "err.h"

/*===========================================================================
                  LOCAL DATA DECLARATIONS
===========================================================================*/

/** Defines the HWIO base address used to get target info structure addr */
uint8 *smem_targ_info_addr_base_ptr = NULL;

/** DAL Timetick handle */
static DalDeviceHandle* smem_timetick_handle = NULL;

/** Address containing the SMEM memory info, e.g physical address and size,
 *  and possibly other information. */
static smem_targ_info_type *smem_targ_info_ptr =
    (smem_targ_info_type *)SMEM_INVALID_ADDR;

/*===========================================================================
                  EXTERNAL DATA DECLARATIONS
===========================================================================*/

/** Function pointers for internal spinlock implementation */
extern void (*spin_lock_internal_fn_ptr)(uint32);
extern void (*spin_unlock_internal_fn_ptr)(uint32);
extern uint32 (*spin_lock_get_fn_ptr)(uint32);

/*===========================================================================
                  PUBLIC FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================
  FUNCTION  smem_map_spinlock_region_va
===========================================================================*/
/**
  Creates a virtual address of the HW Mutexes region, used for SMEM spinlocks,
  to enable use of HWIO macros.

  @return
  The spinlocks region base virtual address if successfully mapped, or NULL
  otherwise.

  @dependencies
  None.

  @sideeffects
  None.
*/
/*=========================================================================*/
void *smem_map_spinlock_region_va( void )
{
  DalDeviceHandle *hwio_mutexes_handle;
  uint8           *pBasePtr = NULL;

  if ((DAL_DeviceAttach(DALDEVICEID_HWIO, &hwio_mutexes_handle) == DAL_SUCCESS)
        && (hwio_mutexes_handle != NULL))
  {
    if ( (DalHWIO_MapRegion(hwio_mutexes_handle, SMEM_HW_MUTEX_BASE_NAME,
                          &pBasePtr) != DAL_SUCCESS) ||
         (pBasePtr == NULL) )
    {
      ERR_FATAL("smem_map_spinlock_region_va: Failed to map HW Mutexes region.",
                0, 0, 0);
    }
  }
  else
  {
    ERR_FATAL("smem_map_spinlock_region_va: Failed to attach HWIO DalDevice.",
              0, 0, 0);
  }
  return pBasePtr;
}

/*===========================================================================
  FUNCTION  smem_timetick_init
===========================================================================*/
/**
  Creates the handle for the OS-specific timetick handle, if necessary.

  @return
  SMEM_ERROR on failure
  SMEM_SUCCESS on success
*/
/*=========================================================================*/
int32 smem_timetick_init( void )
{
  int8 ret;
  if ( (DalTimetick_Attach("SystemTimer", &smem_timetick_handle) != DAL_SUCCESS)
          || (smem_timetick_handle == NULL) )
  {
    ERR("smem_timetick: DAL timetick handle failed to attach.", 0, 0, 0);
    ret = SMEM_ERROR;
  }
  else
  {
    ret = SMEM_SUCCESS;
  }
  return ret;
}

/*===========================================================================
  FUNCTION  smem_timetick_get
===========================================================================*/
/**
  Returns the current timetick value.

  @return
  The current timetick value, measured in ticks.
  0 if an error occurred.
*/
/*=========================================================================*/
uint32 smem_timetick_get( void )
{
  DalTimetickTime32Type time_ret;
  if (DalTimetick_Get(smem_timetick_handle, &time_ret) != DAL_SUCCESS)
  {
    ERR("smem_timetick: Cannot get DAL timetick value.", 0, 0, 0);
    time_ret = 0;
  }
  return (uint32)time_ret;
}

#ifndef SMEM_RPM_PROC
/*===========================================================================
  FUNCTION  smem_set_spin_lock_api
===========================================================================*/
/**
  Lookup chip family and version to determine whether to use one of two
  spinlock mechanisms:
  For 9x25/8974 below version 2 (and not WCNSS):
    use exclusive memory access spinlock API
  Otherwise:
    use HW mutex spinlock API.

  @return
  None.
*/
/*=========================================================================*/
void smem_set_spin_lock_api( void )
{
  DalDeviceHandle * hChipinfo;
  DalChipInfoFamilyType family;
  DalChipInfoVersionType version;
  DALResult ret;

  /* Set spinlock function pointer based on chip info */
  ret = DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &hChipinfo);
  ASSERT(ret == DAL_SUCCESS && hChipinfo != NULL);
  DalChipInfo_GetChipFamily(hChipinfo, &family);
  DalChipInfo_GetChipVersion(hChipinfo, &version);

#ifndef SMEM_RIVA_PROC
  if ((DALCHIPINFO_FAMILY_MDM9x25 == family && version < 0x00020001) ||
      (DALCHIPINFO_FAMILY_MSM8974 == family && version < 0x00020000))
  {
    /* Use locked memory instructions for spinlock access */
    spin_lock_internal_fn_ptr   = spin_lock_internal_asm;
    spin_unlock_internal_fn_ptr = spin_unlock_internal_asm;
    spin_lock_get_fn_ptr        = spin_lock_get_asm;
  }
  else
#endif
  {
    /* Use HW mutex for spinlock access */
    spin_lock_internal_fn_ptr   = spin_lock_internal;
    spin_unlock_internal_fn_ptr = spin_unlock_internal;
    spin_lock_get_fn_ptr        = spin_lock_get;
  }
}
#endif

/*===========================================================================
  FUNCTION  smem_malloc
===========================================================================*/
/**
  Dynamically allocate a region of memory from the heap.

  @param[in]  size   The number of bytes to be allocated.

  @return    The pointer to the region of memory that was allocated.
             NULL if the allocation failed.
*/
/*=========================================================================*/
void * smem_malloc(uint32 size)
{
  void * pMem;
  return DAL_SUCCESS == DALSYS_Malloc(size, &pMem) ? pMem : NULL;
}

/*===========================================================================
  FUNCTION  smem_map_memory_va
===========================================================================*/
/**
  Map the memory region and return the virtual address of the mapped (physical)
  memory region.

  @return
  Virtual address of the mapped memory region.

  @param[in]    mem_region   Memory region base (physical) address.
  @param[in]    size         Size of the memory region.

  @dependencies
  None.

  @sideeffects
  None
*/
/*=========================================================================*/
void *smem_map_memory_va( void* mem_region, uint32 size )
{
  if(mem_region == (void*)SMEM_RPM_MSG_RAM_BASE_PHYS )
  {
    return (void*) SMEM_RPM_MSG_RAM_BASE ;
  }
  else
  {
#if defined(SMEM_RPM_PROC) || defined(SMEM_BOOT_IMAGE) || defined(SMEM_TZ_IMAGE)
    /* Assume mapping is 1-to-1. */
    return mem_region;
#else
    DALSYSMemHandle hSmem;
    DALSYSMemInfo smemInfo;
    DALResult result;

    result = DALSYS_MemRegionAlloc(
        DALSYS_MEM_PROPS_UNCACHED | DALSYS_MEM_PROPS_PHYS_CONT,
        DALSYS_MEM_ADDR_NOT_SPECIFIED,
        (DALSYSMemAddr)mem_region, size,
        &hSmem, NULL);

    ASSERT( DAL_SUCCESS == result );

    result = DALSYS_MemInfo( hSmem, &smemInfo);

    ASSERT( DAL_SUCCESS == result );

    return (void *)(smemInfo.VirtualAddr);
#endif
  }
}

/*===========================================================================
  FUNCTION  smem_map_smem_targ_info
===========================================================================*/
/**
  Map the SMEM target info region (if it has not been mapped by this function
  already) and return the virtual base address of the structure.

  If this is not the first time this function has been called, it will simply
  return the saved pointer, rather than attempting to do the mapping again.

  @return
  Pointer to the structure that holds the SMEM target info like physical
  address and size.
*/
/*=========================================================================*/
smem_targ_info_type *smem_map_smem_targ_info( void )
{
  if (smem_targ_info_ptr == SMEM_INVALID_ADDR)
  {
#if defined(SMEM_BOOT_IMAGE) || defined(SMEM_TZ_IMAGE)
    /* Assume mapping is 1-to-1. */
    smem_targ_info_ptr = (smem_targ_info_type *)SMEM_TARG_INFO_ADDR;
#else
#if !defined(SMEM_RPM_PROC)
    DALSYSMemHandle hSmemProp;
    DALSYSMemInfo smemPropInfo;
    DALResult result;
    uint32 smem_prop_virt_addr;
#endif
    DalDeviceHandle *hwio_handle;

    /* Map virtual address of the HW WONCE region, used to get SMEM target
     * info structure addr and to enable use of HWIO macros.
     */
    if (smem_targ_info_addr_base_ptr == NULL)
    {
      if ((DAL_DeviceAttach(DALDEVICEID_HWIO, &hwio_handle) == DAL_SUCCESS)
            && (hwio_handle != NULL))
      {
        if ((DalHWIO_MapRegion(hwio_handle, SMEM_TARG_INFO_ADDR_BASE_NAME,
                              &smem_targ_info_addr_base_ptr) != DAL_SUCCESS) ||
             (smem_targ_info_addr_base_ptr == NULL))
        {
          ERR_FATAL("Failed to map HW WONCE region.", 0, 0, 0);
        }
      }
      else
      {
        ERR_FATAL("Failed to attach HWIO DalDevice.", 0, 0, 0);
      }
    }

    /* Check if TCSR WONCE registers hold proper SMEM target info address */
    if (SMEM_TARG_INFO_ADDR == 0)
    {
      return NULL;
    }
#if !defined(SMEM_RPM_PROC)
    result = DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_ARCH_HW_NORM_MEM_SHARE |
                         DALSYS_MEM_PROPS_UNCACHED | DALSYS_MEM_PROPS_PHYS_CONT,
                         DALSYS_MEM_ADDR_NOT_SPECIFIED,
                         (DALSYSMemAddr)SMEM_TARG_INFO_ADDR,
                         sizeof(smem_targ_info_type),
                         &hSmemProp, NULL);
    ASSERT( DAL_SUCCESS == result );

    result = DALSYS_MemInfo(hSmemProp, &smemPropInfo);
    ASSERT( DAL_SUCCESS == result );

    smem_prop_virt_addr = (uint32)smemPropInfo.VirtualAddr;
    if ((smem_prop_virt_addr & SMEM_PAGE_ALIGN_MASK) == 0)
    {
      /* The mapping function rounded down to the 4kB page.  Must add the
       * offset back again.
       */
      smem_prop_virt_addr += (SMEM_TARG_INFO_ADDR & SMEM_PAGE_ALIGN_MASK);
    }
    smem_targ_info_ptr = (smem_targ_info_type *)smem_prop_virt_addr;
#else
    /* On RPM Bear DDR is not directly mapped */
    smem_targ_info_ptr = (smem_targ_info_type *)
                           SMEM_DDR_SOC_TO_RPM_MAP(SMEM_TARG_INFO_ADDR);
#endif /* !SMEM_RPM_PROC */
#endif
  }

  return smem_targ_info_ptr;
}
