#ifndef TZBSP_ISR_H
#define TZBSP_ISR_H

/**
@file tzbsp_isr.h
@brief Provides API to install and control secure interrupts.
*/

/*===========================================================================
   Copyright (c) 2011-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_isr.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
10/04/11   mm       Added description to ISR
03/06/11   tk       Initial version.

===========================================================================*/
#include <comdef.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define TZBSP_INT_TARGET_CPU(x)  (x)

/* Only valid when generating an SGI. */
#define TZBSP_INT_TARGET_OTHERS  (0x10000000)
/* Only valid when generating an SGI. */
#define TZBSP_INT_TARGET_SELF    (0x20000000)
/* Only valid when generating an SGI. */
#define TZBSP_INT_TARGET_ALL     (TZBSP_INT_TARGET_OTHERS | TZBSP_INT_TARGET_SELF)

#define TZBSP_INTF_TRIGGER_LEVEL        0x00000000
#define TZBSP_INTF_TRIGGER_EDGE         0x00000001
/* The interrupt is marked registered for the internal OS (QSEE). */
#define TZBSP_INTF_INT_FLAG             0x00000000
/* If active, the current interrupt configuration is saved and will be restored
 * when the ISR is uninstalled. */
#define TZBSP_INTF_SAVE_INT             0x00004000
/* The interrupt is marked registered for the external OS (MobiCore). */
#define TZBSP_INTF_INT_FLAG_EXT         0x00008000
/* Interrupt is configured non-secure. Valid only for unregister. */
#define TZBSP_INTF_INT_FLAG_NON_SECURE  0x10000000
/* The interrupt is configured for CPU0 only. */
#define TZBSP_INTF_CPU0_ONLY            0x20000000
/* The interrupt is marked registered for all CPUs. This affects only the
 * interrupt registry state, not the interupt controller. */
#define TZBSP_INTF_ALL_CPUS             0x40000000
/* The ISR is installed, but the interrupt itself won't be configured on the
 * interrupt controller. */
#define TZBSP_INTF_SKIP_INT_CONFIG      0x80000000

/* @deprecated Use \c TZBSP_INTF_TRIGGER_LEVEL */
#define TZBSP_INT_TRIGGER_LEVEL         0
/* @deprecated Use \c TZBSP_INTF_TRIGGER_EDGE */
#define TZBSP_INT_TRIGGER_EDGE          1

/* Maximum number of ISRs */
#define TZBSP_MAX_ISR_COUNT             32

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * Installs an ISR function. The corresponding interrupt is registered for the
 * calling CPU (interrupt target is self).
 *
 * @param [in] intnum   Interrupt number the ISR is registered for.
 * @param [in] fn       The ISR function itself.
 * @param [in] ctx      The context passed to ISR.
 * @param [in] flags    Combination of \c TZBSP_INTF_* flags.
 * @param [in] enable   Initial interrupt enable/disable status on the
 *                      interrupt controller.
 *
 * @return Zero on success, error code otherwise.
 */
int tzbsp_register_isr(uint32 intnum, const char *intdesc, void* (*fn)(void*),
                       void* ctx, uint32 flags, boolean enable);

/**
 * Uninstalls an ISR function. If no installed ISR is found for the interrupt
 * number, then it is not an error and is ignored silently.
 *
 * @param [in] intnum   Interrupt number the ISR is registered for.
 *
 * @return Zero on success, error code otherwise.
 */
int tzbsp_unregister_isr(uint32 intnum, uint32 flags);

/**
 * Invokes the interrupt handler for the given interrupt. If no interrupt
 * handler is installed for the interrupt, then this function just returns.
 *
 * @param [in] intnum   The number of the occured interrupt.
 *
 * @return Always returns zero.
 */
uint32 tzbsp_dispatch_int(uint32 intnum);

/**
 * Configures a secure interrupt on the interrupt controller.
 *
 * @param [in] intnum   The interrupt to be configured.
 * @param [in] target   Target processor for the raised interrupt, one of
 *                      \c TZBSP_INT_TARGET_*.
 * @param [in] flags    Either \c TZBSP_INTF_TRIGGER_LEVEL or
 *                             \c TZBSP_INTF_TRIGGER_EDGE.
 * @param [in] enable   Initial interrupt enable/disable status on the
 *                      interrupt controller.
 */
void tzbsp_config_int(uint32 intnum, uint32 target, uint32 flags,
                      boolean enable);

/**
 * Disables the given interrupt on the interrupt controller.
 *
 * @param [in] intnum   The interrupt to be disabled.
 */
void tzbsp_disable_int(uint32 intnum);

/**
 * Enables the given interrupt on the interrupt controller.
 *
 * @param [in] intnum   The interrupt to be enabled.
 */
void tzbsp_enable_int(uint32 intnum);

/**
 * Sets the IRQ intercept function. Setting this function allows TZBSP to
 * inspect raised IRQs. The function is not supposed to acknowledge/end the IRQ
 * interrupts as IRQ handling is done in non-secure side. If the function is
 * set to NULL (default), then the monitor just hands the IRQ to non-secure
 * side. The function will be executed in monitor mode.
 *
 * @param [in] fn       The function to be used as IRQ interceptor.
 */
void tzbsp_install_irq_function(void (*fn) (void));

/**
 * Sets the FIQ handler function. Setting this function allows TZBSP to handle
 * raised FIQs. The function is supposed to acknowledge/end the FIQ interrupts.
 * This function must always be set when receiving FIQs. That is, if FIQ is
 * raised while the handler function is NULL, then the monitor will start
 * waiting in an error loop. TZBSP installs a default handler which calls \c
 * tzbsp_dispatch_int. This function will be executed in monitor mode.
 *
 * Returning non-zero value from the FIQ handler function will trigger monitor
 * to dump the current CPU context and reboot to the download mode where memory
 * dumps can be collected.
 *
 * @param [in] fn       The function to be used as FIQ handler.
 */
void tzbsp_install_fiq_function(int (*fn) (void));

#endif
