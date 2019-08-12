#ifndef __TZBSP_EXEC_H__
#define __TZBSP_EXEC_H__

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_exec.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
02/16/2011 pre      Initial
===========================================================================*/
#include "tzbsp_exec_defs.h"

typedef struct tzbsp_memblock_s
{
  uint32 start;
  uint32 end;
  uint32 attributes;
} tzbsp_memblock_t;

typedef int (*tzbsp_exec_smc_func)(const void *req, uint32 req_len,
                                   const void *rsp, uint32 rsp_len);
typedef void (*tzbsp_exec_fiq_notifier_t)(uint32);
typedef void (*tzbsp_exec_fiq_notifier_ext_t)(uint32, void *ctxt);

typedef int (*tzbsp_tzos_smc_func_t)(uint32 app_id,
                                     const void *req, uint32 req_len,
                                     const void *rsp, uint32 rsp_len);
typedef void (*tzbsp_exec_warmboot_notifier_t) (void);
typedef void (*tzbsp_exec_power_collapse_notifier_t) (void);

typedef struct
{
  tzbsp_exec_smc_func                   func_smc;
  union
  {
    tzbsp_exec_fiq_notifier_t           func_callback;
    tzbsp_exec_fiq_notifier_ext_t       func_callback_ext;
  } __attribute__ ((packed)) fiq;
  tzbsp_tzos_smc_func_t                 func_tzos_smc;
  tzbsp_exec_warmboot_notifier_t        func_warmboot;
  tzbsp_exec_power_collapse_notifier_t  func_power_collapse;
} __attribute__ ((packed)) tzbsp_exec_monitor_handlers_table_t;

/**
 * @param [in]
 * @param [in]
 * @param [in]
 *
 * @return Zero on success, otherwise an error code.
 */
__weak int tzbsp_exec_smc_handler(const void *req, uint32 req_len,
                                  const void *rsp, uint32 rsp_len);

__weak int tzbsp_exec_smc_handler_ext(const void *req, uint32 req_len,
                                  const void *rsp, uint32 rsp_len);

__weak int tzbsp_tzos_smc_handler(uint32 app_id,  const void *req, 
                                  uint32 req_len, const void *rsp, 
                                  uint32 rsp_len);

__weak int tzbsp_tzos_smc_handler_ext(uint32 app_id,  const void *req, 
                                      uint32 req_len, const void *rsp, 
                                      uint32 rsp_len);

/**
 * Called to notify client that warmboot has occurred
 */
__weak void tzbsp_exec_warmboot_notifier(void);

/**
 * Called to notify client that the system is going into power
 * collapse
 */
__weak void tzbsp_exec_power_collapse_notifier(void);

/**
 * Does not return structure so we do not have to publicly define
 * structure.
 *
 * @returns Pointer to function groups passed to QSEE on boot
 */
__weak uint32 tzbsp_exec_get_fgroups_ptr(void);

/**
 * @returns imem allocated to MobiCore. Deprecated since MobiCore now runs in
 *          DDR only
 */
__weak const tzbsp_memblock_t* tzbsp_exec_get_imem_ptr(void);

/**
 * @returns location in DDR where MobiCore will be loaded and is allocated for
 *          both the MobiCore kernel and its trustlets
 */
__weak const tzbsp_memblock_t* tzbsp_exec_get_ddr_ptr(void);

/**
 * @returns Reset function for EXEC */
int tzbsp_reset_exec(uint32 addr);


/* Maximum number of interrupts that TZBSP supports */
#define TZBSP_MAX_ISR_CNT    32

#endif
