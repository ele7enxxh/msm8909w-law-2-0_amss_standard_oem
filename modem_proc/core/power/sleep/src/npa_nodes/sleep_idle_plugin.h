#ifndef SLEEP_IDLE_PLUGIN_H
#define SLEEP_IDLE_PLUGIN_H
/*==============================================================================
  FILE:         sleep_idle_plugin.h
  
  OVERVIEW:     This file provides the externs and declarations needed for the
                sleep idle plugin subsystem.
 
  DEPENDENCIES: None

                Copyright (c) 2011-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/npa_nodes/sleep_idle_plugin.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $
==============================================================================*/
#include "npa.h"

/* ===========================================================================
                            GLOBAL DEFINITIONS
============================================================================*/

typedef enum
{
  SLEEP_IDLE_PLUGIN_DEFAULT = 0x0, /* Normal Sleep Processing with solver */
  SLEEP_IDLE_PLUGIN_SWFI_ONLY,     /* Only clock gating */
  SLEEP_IDLE_PLUGIN_HOLD_OFF,      /* No low power mode at all - busy wait */
  SLEEP_IDLE_PLUGIN_MAX
} sleep_idle_plugin_type;

typedef uint32 ( *sleep_idle_entry_ptr_type )( void );

/**
 * SLEEP_IDLE_PLUGIN_CLIENT_MIN_MODE_LATENCY
 *
 * @brief NPA client type to pass the minimum latency among enabled modes.
 */
#define SLEEP_IDLE_PLUGIN_CLIENT_MIN_MODE_LATENCY  NPA_CLIENT_CUSTOM1

/**
 * SLEEP_IDLE_PLUGIN_CLIENT_LATENCY
 *
 * @brief NPA client type to pass the latency restrictions.
 */
#define SLEEP_IDLE_PLUGIN_CLIENT_LATENCY           NPA_CLIENT_CUSTOM2

/**
 * SLEEP_IDLE_PLUGIN_CLIENT_MIN_SLEEP_LATENCY 
 *
 * @brief NPA client type to pass minimum sleep processing overhead
 *        without any mode.
 */
#define SLEEP_IDLE_PLUGIN_CLIENT_MIN_SLEEP_LATENCY NPA_CLIENT_CUSTOM3

/**
 * SLEEP_IDLE_PLUGIN_CLIENT_LPM_HOLD_OFF
 *
 * @brief NPA client type that can hold off Sleep from executing any
 *        low power modes.
 */
#define SLEEP_IDLE_PLUGIN_CLIENT_LPM_HOLD_OFF      NPA_CLIENT_CUSTOM4

/* ===========================================================================
                        FUNCTION DECLARATIONS
============================================================================*/

/**
 * sleep_idle_plugin_init
 * 
 * @brief Initialize the sleep idle plugin subsystem.  This function registers 
 *        the idle plugin node. 
 */
void sleep_idle_plugin_init( void );

/**
 * sleep_swfi
 *
 * @brief This function performs all_wait immediately, and is 
 *        used when the system is in a situation that other low
 *        power modes are not viable.
 *
 * @return Non-zero on failure.
 */
uint32 sleep_swfi(void);

/**
 * sleep_holdoff_no_mode
 *
 * @brief This function simply returns (and hence prevents system from going
 *        in to any low power modes). It is used until everyone is ready for 
 *        system to be in any low power state.
 *
 * @return Always 0.
 */ 
uint32 sleep_holdoff_no_mode( void );

/**
 * sleep_os_set_idle_entry
 *
 * @brief This function sets the idle entry point used by the 
 *        sleep code.  It is changed by the /sleep/idle/plugin
 *        node.
 *  
 * @param core_num            Core number to change the idle 
 *                            entry pointer for.
 * @param new_idle_entry_ptr  Pointer to the function to call 
 *                            when the system is idle next.
 */
void sleep_os_set_idle_entry( uint32 core_num, 
                              sleep_idle_entry_ptr_type new_idle_entry_ptr );

#endif /* SLEEP_IDLE_PLUGIN_H */
