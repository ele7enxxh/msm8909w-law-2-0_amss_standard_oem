#ifndef SLEEP_V_H
#define SLEEP_V_H

/*=============================================================================
   FILE:           sleep_v.h

   OVERVIEW:       This file contains declarations of functions that are used
                   initialize sleep subsystem and select low power mode during
                   idle time.

   DEPENDENCIES:   

                   Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
                   All Rights Reserved.
                   Qualcomm Confidential and Proprietary

   $Header: //components/rel/rpm.bf/2.1.1/core/power/sleep/inc/sleep_v.h#1 $
==============================================================================*/

#include <stdint.h>
#include "DALStdDef.h"

/*
===============================================================================

                           GLOBAL FUNCTION DECLARATIONS

===============================================================================
*/

/**
 * sleep_init
 *
 * @brief Initialize the sleep subsystem.
 *
 * Initializing sleep subsystem involves initializing its various components 
 * like logging system, NPA nodes, LPR tracking system and target dependent
 * resources.
 */
void sleep_init(void);

/**
 * sleep_perform_lpm
 *
 * @brief Put the system into the lowest power state possible.
 *
 * This function will eventually call the sleep subsystem and solver to
 * determine the list of low power modes that can be entered during idle time.
 *
 * @return Non-zero on failure.
 */
uint32 sleep_perform_lpm(void);

/**
 * sleep_get_hard_deadline 
 *  
 * @brief Returns the hard deadline that the sleep task must be 
 *        awake by, in absolute ticks.  This is the time that
 *        sleep expects to exit by.
 *  
 *        NOTE:  It only makes sense to call this function from
 *               within the idle context (i.e., from within an
 *               LPR).
 *  
 * @return The hard deadline, in absolute ticks, that sleep 
 *         expects to exit by.
 */
uint64_t sleep_get_hard_deadline( void );

#endif /* SLEEP_V_H */
