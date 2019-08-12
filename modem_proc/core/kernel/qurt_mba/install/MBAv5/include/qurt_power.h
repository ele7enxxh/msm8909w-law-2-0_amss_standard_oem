#ifndef QURT_POWER_H
#define QURT_POWER_H
/*=============================================================================

                 qurt_power.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
   Prototypes of power API  

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2010
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_power.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file 
=============================================================================*/


/**
 * Prepare for Power Collapse. 
 * Determine if ISDB is disabled and all other hardware threads in the QURT application
 * system are in WAIT or STOP mode.
 * If so, mask the global interrupt and return the result value EOK.
 *
 * @return             EOK: Recovering from warm boot
 *                          NOTE - This value indicates that the processor is ready for
 *                          power-collapse mode.
 * @return             EPC_ISDB: ISDB not disabled
 * @return             EPC_NOSTM: Processor not in single-threaded mode.
 */
int qurt_power_shutdown_prepare (void);

/**
 * Power collapse cannot proceed; return from power collapse mode.
 * All it does is unmask Global interrupt.
 * This operation is used only when the thread is recovering from a failed power collapse
 * operation
 *
 * @return             EOK: Operation successfully performed.
 */
int qurt_power_shutdown_fail_exit (void);

/**
 * Trigger power collapse.
 * Determine if no valid interrupts are pending. (Valid interrupts are determined using the
 * interrupt registration status.)
 * If so, put the current hardware thread into WAIT mode.
 * When all hardware threads have entered WAIT mode, the Hexagon processor can enter
 * power-collapse mode.
 *
 * @param  type        The L2 cache retention type
 *                     QURT_POWER_SHUTDOWN_TYPE_L2NORET
 *                     QURT_POWER_SHUTDOWN_TYPE_L2RET
 *
 * @return             EOK:   Operation successfully performed
 *                     NOTE - This value indicates that the processor has
 *                     returned from power-collapse mode.
 * @return             non-zero: IPEND mask value indicating pending interrupts.
 */
int qurt_power_shutdown_enter (int type);

/**
 * Undo state changes made preparing for power collapse.. It
 * - Unmasks Global interrupt
 * - Restore the interrupt mask to the value defined when the operation
 *   qurt_power_shutdown was previously called.
 *
 * @return             EOK: Operation successfully performed.
 */
int qurt_power_shutdown_exit (void);

/**
 * Prepare for TCXO shutdown.
 * Determine if all other hardware threads in the QURT application system are in WAIT or
 * STOP mode.
 * If so, mask the global interrupt and return the result value EOK.
 * If any hardware thread is not in WAIT or STOP mode, return EPC_NOSTM.
 *
 * @return             EOK: Operation successfully performed, global interrupt is masked
 * @return             EPC_NOSTM: Operation failed (global interrupt not masked).
 */
int qurt_power_tcxo_prepare (void);

/**
 * TCXO shutdown cannot proceed; return from TCXO shutdown mode.
 * Unmasks the global interrupt.
 * This operation is used only when the thread is recovering from a failed TCXO shutdown
 * operation
 *
 * @return             EOK: Operation successfully performed.
 */
int qurt_power_tcxo_fail_exit (void);

/**
 * Trigger TCXO shutdown.
 * If any active (i.e., registered) interrupts are pending, return a bitmask value indicating the
 * active pending interrupts.
 * If no pending interrupts exist, change the interrupt mask to the value read from symbol
 * QURTK_tcxo_intmask (which is specified as part of QURT configuration).
 * 
 * NOTE The default value of QURTK_tcxo_intmask is 0xffffffff, which specifies
 *      that no interrupts are enabled.
 *
 * @return             EOK: Operation successfully performed.
 * @return             IPEND value masking out invalid interrupts.
 */
int qurt_power_tcxo_enter (void);

/**
 * Return from TCXO shutdown mode.
 * Restore the interrupt mask to the value defined when the operation
 * qurt_power_tcxo_enter was previously called.
 *
 * @return             EOK: Operation successfully performed.
 */
int qurt_power_tcxo_exit (void);

/**
 * Suspend current thread until all other threads are suspended.
 * The caller thread is suspended until all other threads in the QURT application system are
 * also suspended. When this happens the caller thread is awakened.
 * If all other threads are suspended when this function is called, the caller thread continues
 * executing.
 * NOTE This function is used to perform power management
 *
 * @return             void
 */
void qurt_power_wait_for_idle (void);

/**
 * Suspend current thread until any other thread is awakened.
 * The caller thread is suspended until any other thread in the QURT application system is
 * awakened. When this happens the caller thread is awakened.
 * If any other thread is ready when this function is called, the caller thread is immediately
 * scheduled.
 * NOTE This function is used to perform power management . It is
 *			useful only when paired with qurt_power_wait_for_idle.
 *
 * @return             void
 */
void qurt_power_wait_for_active (void);

/**
 * Clear pending interrupts. 
 * This API helps clearing pending interrupts in "ipend" registers. 
 *
 * @return             void
 */
void qurt_system_ipend_clear (unsigned int ipend);


/**
 * get pending interrupts. 
 * This API helps get pending interrupts in "ipend" registers. 
 *
 * @return             pending interrupt 
 */
unsigned int qurt_system_ipend_get (void);

/**
 * set AVS configuration register. 
 * This API sets AVS config register accessible only in supervisor mode
 *
 * avscfg_value   - AVS config value to be set
 *
 * @return             void
 */
void qurt_system_avscfg_set(unsigned int avscfg_value);


/**
 * get AVS configuration register. 
 * This API gets AVS config register accessible only in supervisor mode
 *
 * @return             AVS configuration register value
 */
unsigned int qurt_system_avscfg_get(void);


/**
 * get VID system register. 
 * This API gets the value of VID register accessible only in supervisor mode
 *
 * @return             VID register value
 */
unsigned int qurt_system_vid_get(void);

/**
 * Get number of power collapses and counts of processor cycles 
 * for entering and exiting most recent power collapse. If there 
 * is no power collape occurs yet, processor cycles numbers will 
 * be zero. 
 * @param out   enter_pcycles - Number of processor cycles for entering most
 *              recent power collapse.
 * @param out   exit_pcycles - Number of processor cycles for exiting most
 *              recent power collapse.
 * @return      Zero - No power collapses have occurred.
 *              Non-zero - Number of power collapses that have occurred since
 *              the processor was reset
 */ 
int qurt_get_shutdown_pcycles( unsigned long long *enter_pcycles,  unsigned long long *exit_pcycles );

#endif /* QURT_POWER_H */
