/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef GPIO_DEBUG_H
#define GPIO_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This typedef is used as a bitindex to flag each of these
 *  potential set transitions or other timing info. These are
 *  then used in gpio_test_enable and disable to debug or keep
 *  off.
 */
typedef enum gpio_debug_application
{
    GPIO_SET_TRANSITION_APPS       = 0,
    GPIO_SET_TRANSITION_MODEM      = 1,
    GPIO_SET_TRANSITION_ADSP       = 2,
    GPIO_SET_TRANSITION_PRONTO     = 3,
    GPIO_DEEP_SLEEP                = 4,
    GPIO_DEBUG_SIZE
} gpio_debug_application;

/**
 *  This typedef is used as a bitindex to flag which type of
 *  transitions are used, active or sleep.
 */
typedef enum gpio_debug_transition
{
    GPIO_ACTIVE_TRANSITION         = 0,
    GPIO_SLEEP_TRANSITION          = 1,
    GPIO_DEBUG_TRANSITION_SIZE
} gpio_debug_transition;

/**
 *  Enables gpio pin to logic level HIGH (2v).
 *  
 *  gpio_number: The gpio line being accessed 
 *  
 *  *Note: These functions are only valid for GPIO pins 32-63
 *  because the HWIO_TLMM register is for these pins only.
 *  
 *  *Note: The default level for GPIOs are HIGH!
 */
void gpio_toggle_high(void);

/**
 *  Disables gpio pin to logice level LOW (0v).
 *  
 *  *Note: These functions are only valid for GPIO pins 32-63
 *  because the HWIO_TLMM register is for these pins only.
 *  
 *  *Note: The default level for GPIOs are HIGH!
 */
void gpio_toggle_low(void);

/**
 *  Checks the bitmask for the given configuration of gpio_flag,
 *  then checks the bitmask for the configuration of
 *  gpio_transition, then enables the gpio pin if it passed the
 *  2 checks.
 *  
 *  gpio_application: index of the application calling the gpio_test_high(), when & with gpio_debug_application_bitindex and
 *  creates a non-zero if valid.
 *  
 *  gpio_transition: index of the transition calling the gpio_test_high(), when & with gpio_debug_transition_bitindex and
 *  creates a non-zero if valid.
 *  
 */
void gpio_test_high(gpio_debug_application gpio_application, gpio_debug_transition gpio_transition);

/**
 *  Checks the bitmask for the given configuration of gpio_flag,
 *  then checks the bitmask for the configuration of
 *  gpio_transition, then enables the gpio pin if it passed the
 *  2 checks.
 *  
 *  gpio_application: index of the application calling the gpio_test_high(), when & with gpio_debug_application_bitindex and
 *  creates a non-zero if valid.
 *  
 *  gpio_transition: index of the transition calling the gpio_test_high(), when & with gpio_debug_transition_bitindex and
 *  creates a non-zero if valid.
 *  
 */
void gpio_test_low(gpio_debug_application gpio_application, gpio_debug_transition gpio_transtition);

#ifdef __cplusplus
}
#endif

#endif // GPIO_DEBUG_H

