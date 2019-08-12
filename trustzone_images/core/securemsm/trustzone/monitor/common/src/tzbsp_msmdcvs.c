/*===========================================================================

FILE:         tzbsp_msmdcvs.c

DESCRIPTION: TZ interface for the MSM DCVS algorithm. See dcvs.h for more 
             details on the API.


===========================================================================
             Copyright © 2011 Qualcomm Technologies Incorporated.
                 All Rights Reserved.
                 QUALCOMM Confidential and Proprietary.
===========================================================================

                        Edit History
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/monitor/common/src/tzbsp_msmdcvs.c#1 $

 when       who     what, where, why
--------   ----    ------------------------------------------------------------
01/06/12    sg     Initial version.

=============================================================================*/

#ifdef FEATURE_MSMDCVS
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include <dcvs.h>
#include "tzbsp.h"
#include "tzbsp_mem.h"


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Data Declarations (external and forward)
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Internal Function Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Public interfaces
 * -------------------------------------------------------------------------*/

/**
 * Initializes the MSM DCVS solution. Must be called from tzbsp_chipset_init()
 * prior to starting any DCVS operations.
 */
void tzbsp_msm_dcvs_init(void)
{
  DcvsInitTZ();
}

/**
 * Non-monitor call from HLOS. Used to initialize the DCVS system with
 * a working physically contiguous, uncached buffer.
 * 
 * @param buf_addr: Physical address of a physically contiguous buffer. Size
 *                  is target dependent.
 * @param buf_size: Size of the buffer in bytes.
 * 
 * @return int:
 *     0 if successful.
 *    -E_ALREADY_DONE: if already initialized.
 *    -E_OUT_OF_RANGE: Buffer location failed security check.
 */
int tzbsp_dcvs_init(uint32 buf_addr, uint32 buf_size)
{
  if(!tzbsp_is_ns_range((void*)buf_addr, buf_size))
  {
    return -E_BAD_ADDRESS;
  }

  return DcvsInit(buf_addr, buf_size);
}

/**
 * Non-monitor call from HLOS. Create a DCVS "group" for linking cores.
 * 
 * @param group_id: Unique identifier for the group.
 * 
 * @return int:
 *     0 if successful.
 *     -E_NOT_ALLOWED: if DcvsInit() was not previously called.
 *     -E_NO_MEMORY: there is insufficient memory.
 *     -E_OUT_OF_RANGE: If group already exists.
 * 
 */
int tzbsp_dcvs_create_group(uint32 group_id)
{
  return DcvsCreateGroup((DcvsIdType)group_id);
}

/**
 * Non-monitor call from HLOS. Register a core with the DCVS system.
 * 
 * @param core_id:  Unique identifier for the core.
 * @param group_id: Group to add the core to.
 * @param core_params: Params for the core.
 * @param freq_entry: 1d array of frequency data.
 * 
 * @return int
 *     0 if successful.
 *     -E_NOT_ALLOWED: if DcvsInit() was not previously called.
 *     -E_NO_MEMORY: there is insufficient memory.
 *     -E_OUT_OF_RANGE: Arguments are invalid or out of range.
 */
int tzbsp_dcvs_register_core(uint32 core_id, uint32 group_id,
                             uint32* core_params, uint32* freq_entry)
{
  if(!tzbsp_is_ns_range((void*)core_params, sizeof(DcvsCoreParams)))
  {
    return -E_BAD_ADDRESS;
  }

  if(!tzbsp_is_ns_range((void*)freq_entry, sizeof(DcvsFreqEntry)))
  {
    return -E_BAD_ADDRESS;
  }

  return DcvsRegisterCore((DcvsIdType)core_id,
                          (DcvsIdType)group_id,
                          (DcvsCoreParams*)core_params,
                          (DcvsFreqEntry*)freq_entry);
}

/**
 * Non-monitor call from HLOS. Set algo params for a core.
 * 
 * @param core_id:  Unique identifier for the core.
 * @param params:   Algo params.
 * 
 * @return int
 *     0 if successful.
 *     -E_NOT_ALLOWED: if DcvsInit() was not previously called.
 *     -E_OUT_OF_RANGE: Arguments are invalid or out of range.
 */
int tzbsp_dcvs_set_alg_params(uint32 core_id, uint32* params)
{
  if(!tzbsp_is_ns_range((void*)params, sizeof(DcvsAlgorithmParams)))
  {
    return -E_BAD_ADDRESS;
  }

  return DcvsSetAlgParams((DcvsIdType)core_id, (DcvsAlgorithmParams*)params);
}

/**
 * Called from monitor context. Handles DCVS system events.
 * 
 * @param core_id:  Unique identifier for the core.
 * @param event_id: DCVS event ID. See dcvs.h for details.
 * @param param0:   See dcvs.h for details.
 * @param param1:   See dcvs.h for details.
 * 
 * @return int
 *     0 if successful.
 *     -E_NOT_ALLOWED: if DcvsInit() was not previously called.
 *     -E_OUT_OF_RANGE: Arguments are invalid or out of range.
 */
int tzbsp_dcvs_system_event(uint32 core_id, uint32 event_id,
                            uint32 param0, uint32 param1)
{
  return DcvsSystemEventHandler((DcvsIdType)core_id,
                                (DcvsEventIdType)event_id,
                                param0, param1);
}

// stub for now
int tzbsp_dcvs_register_ddr_area(void* addr, uint32 len)
{ return E_SUCCESS; }

// stub for now
void tzbsp_msmdcvs_pc_notifier(void) {}

#else // FEATURE_MSMDCVS

#include "tzbsp.h"

/* These globals will be used by the TZ wrapper to obtain the return values
   for DcvsSystemEventHandler(). */
unsigned long g_DcvsReturnVal0 = 0;
unsigned long g_DcvsReturnVal1 = 0;

// Stubs
void tzbsp_msm_dcvs_init(void) {}

int tzbsp_dcvs_init(uint32 buf_addr, uint32 buf_size)
{ return E_NOT_SUPPORTED; }

int tzbsp_dcvs_create_group(uint32 group_id)
{ return E_NOT_SUPPORTED; }

int tzbsp_dcvs_register_core(uint32 core_id, uint32 group_id,
                             uint32* core_params, uint32* freq_entry)
{ return E_NOT_SUPPORTED; }

int tzbsp_dcvs_set_alg_params(uint32 core_id, uint32* params)
{ return E_NOT_SUPPORTED; }

int tzbsp_dcvs_system_event(uint32 * event)
{ return E_NOT_SUPPORTED; }

int tzbsp_dcvs_register_ddr_area(void* addr, uint32 len)
{ return E_SUCCESS; }

void tzbsp_msmdcvs_pc_notifier(void) {}

#endif // FEATURE_MSMDCVS

