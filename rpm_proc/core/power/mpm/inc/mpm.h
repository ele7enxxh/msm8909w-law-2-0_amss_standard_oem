#ifndef MPM_H
#define MPM_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

           M U L T I P R O C E S S O R   P O W E R   M A N A G E R

GENERAL DESCRIPTION

  This module contains routines for the multiprocessor power manager (MPM).

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  mpm_init( ) must be called once, before any other mpm_ function.


Copyright (c) 2005-2009,2011 by Qualcomm Technologies, Inc.  All Rights Reserved.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=


===============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //components/rel/rpm.bf/2.1.1/core/power/mpm/inc/mpm.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who   what, where, why
--------   ---   -----------------------------------------------------------
04/09/12   mda   MPMv2 for 8974

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include <stdint.h>
#include <stdbool.h>

/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typedefs,
and other items needed by this module.

=============================================================================*/

typedef enum
{
  MPM_VOLTAGE_RAIL_VDD_DIG = 0,
  MPM_VOLTAGE_RAIL_VDD_MEM = 1,

  /* internal definitions */
  MPM_VOLTAGE_RAIL_COUNT,
  MPM_VOLTAGE_RAIL_TYPE_SIZE = 0x7fffffff /* force enum to 32 bits */
} mpm_voltage_rail_type;

typedef enum
{
  MPM_SLEEP_MODE_VDD_LOW = 0,
  MPM_SLEEP_MODE_VDD_MIN = 1,
  MPM_SLEEP_MODE_DEBUG = 2,
  MPM_SLEEP_MODE_COUNT,
  MPM_SLEEP_MODE_TYPE_SIZE = 0x7fffffff /* force enum to 32 bits */
} mpm_sleep_mode_type;


/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/
/**
 * @brief Initialize the MPM registers to known values.
 */
void mpm_init (void);

/**
 * @brief Initialize the CXO related MPM registers based on whether or not
 *        an external sleep clock is being used
 */
void mpm_init_xtal (void);


/**
 * @brief Configure the MPM HW to enter chip sleep
 */
void mpm_setup_chip_sleep(void);

/**
 * @brief Configure the maximum amount of time to stay asleep.
 *
 * @param duration_sclk: The maximum sleep duration, in sclk ticks (32768 Hz).
 */
void mpm_setup_sleep_timer(uint64_t duration_sclk);

/**
 * @brief Clear the sleep configuration of all XO's and rails from the HW.
 */
void mpm_teardown_sleep(void);

/*=============================================================================

FUNCTION MPM_SET_VMIN_VOLTAGES

DESCRIPTION
  This function is used to tell the MPM driver what the voltages are for 
  entering vdd min and exiting vdd min of a

PARAMETERS
  mx_off_uv - vdd min MX voltage level
  mx_on_uv - vdd min exit MX voltage level
  cx_off_uv - vdd min CX voltage level
  cx_on_uv - vdd min exit CX voltage level

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void mpm_set_vmin_voltages
(
  uint32_t mx_off_uv,
  uint32_t mx_on_uv,
  uint32_t cx_off_uv,
  uint32_t cx_on_uv
);

/*=============================================================================

FUNCTION MPM_config_ebi1_freeze_io

DESCRIPTION
  This function is used to tell the MPM driver to enable or disable the ebi1 freeze IO

PARAMETERS
  enable    TRUE tells the MPM to enable the ebi1 freeze IO. False tells the MPM driver to disable the ebi1 freeze IO.
                      
DEPENDENCIES
  The BSP bit for SW Ebi1 CTL must be set to TRUE, otherwise software control of this bit is disallowed

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void mpm_config_ebi1_freeze_io
(
  bool enable
);

/*=============================================================================

FUNCTION mpm_config_freeze_ios

DESCRIPTION
  This function is used to tell the MPM driver to enable or disable all freeze_io_* signals during sleep

PARAMETERS
  enable    TRUE tells the MPM to use BSP config data for freeze_io_* signals enablement during sleep.
            FALSE tells the MPM driver to disable all freeze_io_* signals during sleep.

DEPENDENCIES
  It is a functional requirement that during mock vdd_min/vdd_low we cannot freeze the IOs.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void mpm_config_freeze_ios(bool enable);

/*=============================================================================

FUNCTION MPM_SW_DONE

DESCRIPTION
  Halt the processor and hand control off to the MPM.  This function is
  required (as opposed to a simple halt instruction or register write) to work
  around issues with the MPM system integration.

DEPENDENCIES
  Must be called from an INTLOCK'd context.

RETURN VALUE
  None

SIDE EFFECTS
  The processor will be halted until the MPM detects a wakeup event.

=============================================================================*/
void mpm_sw_done( uint64_t sleep_duration);

uint64_t mpm_deep_sleep_enter_latency(void);
uint64_t mpm_deep_sleep_exit_latency(void);

#endif /* !MPM_H */

