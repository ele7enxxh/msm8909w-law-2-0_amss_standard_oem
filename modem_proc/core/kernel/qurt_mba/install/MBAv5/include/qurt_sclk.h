#ifndef QURT_SCLK_H
#define QURT_SCLK_H
/*=============================================================================

                 qurt_sclk.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
   The header file that describes the APIs supported by QURT system sclk
   feature.

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2010 - 2011
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_sclk.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/08/10   balac   File created.
03/03/11   op      Fix warning when running doxygen 
=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/*=============================================================================

                           DATA DECLARATIONS

=============================================================================*/

/**
 * Max. client supported by QURT system sclk module
 */
#define QURT_SCLK_CLIENTS_MAX                      5

/*=============================================================================

                        FUNCTION DECLARATIONS

=============================================================================*/

/**
 * Register a client for QURT system sclk
 *
 * @param   signal       qurt_signal_t to be registered
 * @param   signal_mask  mask to be set to indicate teh sclk expiry
 *
 * @return  client id, E_FATAL for failure 
 *            
 */
int qurt_sysclock_register (qurt_anysignal_t *signal, unsigned int signal_mask);

/**
 * Program the SCLK timer expiry based on SCLK HW referenced count
 *
 * @param   id          Client Id to be programmed for SCLK timer expiry
 * @param   ref_count   sclk count when the match value was calculated
 * @param   match_value match value to be programmed on SCLK HW
 *
 * @return  match value programmed
 *            
 */
unsigned long long qurt_sysclock_alarm_create (int id, unsigned long long ref_count, unsigned long long match_value);

/**
 * Program the SCLK timer expiry based on duration of SCLK ticks
 *
 * @param   id          Client Id to be programmed for SCLK timer expiry
 * @param   duration    duration to be SCLK ticks
 *
 * @return  EOK
 *            
 */
int qurt_sysclock_timer_create (int id, unsigned long long duration);

/**
 * Get tick count on HW. The HW counter is 32-bit counter and it rolls over.
 * This API is a faster way to obtian ticks, if the roll over is taken care
 * externally. For complete uptime ticks, use "qsystem_sclk_attr_getticks".
 *
 * @return             ticks since system started
 */
unsigned long long qurt_sysclock_get_hw_ticks (void);

/**
 * Determines how long it takes for the next timer interrupt. Gets the number of ticks
 * that needs to be elapsed for the next timer interrupt.
 *
 * @return             no. of ticks to take for next timer interrupt
 */
unsigned long long qurt_sysclock_get_expiry (void);

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif /* __cplusplus */

#endif /* QURT_SCLK_H */
