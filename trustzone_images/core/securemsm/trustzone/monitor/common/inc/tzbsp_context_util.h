#ifndef TZBSP_CONTEXT_UTIL_H
#define TZBSP_CONTEXT_UTIL_H

/*===========================================================================
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/monitor/common/inc/tzbsp_context_util.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/26/12   tk      First version.
============================================================================*/

/**
 * CPU general purpose register context as used by the monitor for context
 * saving and loading.
 */
typedef struct tzbsp_mon_cpu_ctx_s
{
  uint32 mon_lr;
  uint32 mon_spsr;
  uint32 usr_r0;
  uint32 usr_r1;
  uint32 usr_r2;
  uint32 usr_r3;
  uint32 usr_r4;
  uint32 usr_r5;
  uint32 usr_r6;
  uint32 usr_r7;
  uint32 usr_r8;
  uint32 usr_r9;
  uint32 usr_r10;
  uint32 usr_r11;
  uint32 usr_r12;
  uint32 usr_r13;
  uint32 usr_r14;
  uint32 irq_spsr;
  uint32 irq_r13;
  uint32 irq_r14;
  uint32 svc_spsr;
  uint32 svc_r13;
  uint32 svc_r14;
  uint32 abt_spsr;
  uint32 abt_r13;
  uint32 abt_r14;
  uint32 und_spsr;
  uint32 und_r13;
  uint32 und_r14;
  uint32 fiq_spsr;
  uint32 fiq_r8;
  uint32 fiq_r9;
  uint32 fiq_r10;
  uint32 fiq_r11;
  uint32 fiq_r12;
  uint32 fiq_r13;
  uint32 fiq_r14;
} tzbsp_mon_cpu_ctx_t;

/**
 * Structure for overlaying the FIQ handler initial stack context push.
 */
typedef struct tzbsp_mon_fiq_stacked_ctx_s
{
  uint32 usr_r0;
  uint32 usr_r1;
  uint32 usr_r2;
  uint32 usr_r3;
  uint32 usr_r4;
  uint32 usr_r5;
  uint32 usr_r6;
  uint32 usr_r7;
  uint32 usr_r8;
  uint32 usr_r9;
  uint32 usr_r10;
  uint32 usr_r11;
  uint32 usr_r12;
  uint32 mon_spsr;
  uint32 mon_lr;
} tzbsp_mon_fiq_stacked_ctx_t;

/**
 * Structure for overlaying the FIQ context save area for QSEE.
 */
typedef struct tzbsp_mon_fiq_qsee_ctx_s
{
  uint32 s_mon_ip;
  uint32 s_mon_spsr;
  uint32 s_mon_r0;
  uint32 s_mon_r1;
  uint32 s_mon_r2;
  uint32 s_mon_r3;
  uint32 s_mon_r4;
  uint32 s_mon_r5;
  uint32 s_mon_r6;
  uint32 s_mon_r7;
  uint32 s_mon_r8;
  uint32 s_mon_r9;
  uint32 s_mon_r10;
  uint32 s_mon_r11;
  uint32 s_mon_r12;
  uint32 s_mon_r13;
  uint32 s_mon_r14;
} tzbsp_mon_fiq_qsee_ctx_t;

/**
 * Saves the entire interrupted general purpose register context to the
 * provided structure. Some registers are taken from the monitor stack where
 * FIQ handler saves the initial context, @see tzbsp_fiq_get_stacked_ctx.
 * Banked general purpose registers are still intact and the code will cycle
 * through the CPU modes and collect them.
 *
 * @pre Must be called only in FIQ handling context in MON mode.
 *
 * @param [out] ctx     Context structure to store the interrupted CPU context.
 */
void tzbsp_fiq_get_cpu_context(tzbsp_mon_cpu_ctx_t* ctx);

/**
 * Calculates the base address of FIQ handler initial stack context push.
 *
 * @pre Must be called only in FIQ handling context in MON mode.
 *
 * @return Pointer to FIQ handler initial stack context push.
 */
tzbsp_mon_fiq_stacked_ctx_t* tzbsp_fiq_get_stacked_ctx(void);

/**
 * Constructs the FIQ interrupted context area for QSEE.
 *
 * @pre Must be called only in FIQ handling context in MON mode.
 */
void tzbsp_fiq_construct_qsee_ctx(void);

/**
 * Modifies the FIQ interrupted context area.
 *
 * @pre Must be called only in FIQ handling context in MON mode.
 */
void tzbsp_fiq_set_stacked_ctx(void);

#endif /* TZBSP_CONTEXT_UTIL_H */
