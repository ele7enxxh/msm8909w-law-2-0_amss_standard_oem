#ifndef SLEEPNPA_H
#define SLEEPNPA_H

/*============================================================================
  FILE:         SleepNPA.h
  
  OVERVIEW:     This file provides the public externs and definitions that
                are exported by the sleep module for accessing the NPA nodes
                provided by sleep.
 
                Copyright (c) 2010 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/power/SleepNPA.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
========================================================================*/

#include "npa.h"

/**
 * @brief Sleep node names.
 * 
 * These macros can be used when creating clients of the nodes provided
 * by the sleep sybsystem.  See documentation below for more information
 * about each of these nodes.
*/
#define SLEEP_DURATION_NODE_NAME        "/core/cpu/wakeup"
#define SLEEP_LATENCY_NODE_NAME         "/core/cpu/latency"
#define SLEEP_VDD_DIG_NODE_NAME         "/rail/vdd_dig"
#define SLEEP_VDD_MEM_NODE_NAME         "/rail/vdd_mem"

/**
 * @brief vdd_dig_state_type
 * 
 * This is the list of states that are acceptable by the /rail/vdd_dig
 * node.  Any state other than this will not take effect.
 * 
*/
typedef enum                     
{
  SLEEP_VDD_DIG_RETENTION = 0x0, /* Client is OK with VDD min. and waking 
                                  * for only MPM ints */
  SLEEP_VDD_DIG_LATCHING,        /* Client wants VDD min. to allow interrupt
                                  * latching on the local CPU. */
  SLEEP_VDD_DIG_ACTIVE           /* Client requires that VDD dig stay at 
                                  * its active voltages */
} vdd_dig_state_type;
            
/****************************************************************************
 *
 * Resource Interface Documentation
 *
 * "/core/cpu/wakeup" resource
 *
 * REQUIRED client, Units: microseconds
 * Used by clients who know that a specific event is going to wake them up in
 * the near future.  At the time a required request is made, a timestamp is
 * taken and the expected duration (i.e., the request itself) is added.  This
 * represents the sleep deadline.  The resource uses the earliest deadline that
 * is present as the expected duration of the next sleep event and sleep modes
 * will be selected accordingly.  These requests must be released--that is, a
 * deadline that has been passed will still be considered active and the next
 * sleep will be assumed to be infinitesimally short for some period of time,
 * however a deadline that has been passes by a considerable margin will be
 * discarded as invalid.
 *
 * NOTE: Do not use this node in an attempt to disable specific sleep modes.
 *       This node is basically about hinting to the idle task how soon your
 *       task plans to wake up, potentially allowing better power savings for
 *       your use case.  If you need a specific resource to stay on during
 *       sleep, place a request to that resource.
 *
 * The expected time until wakeup can be queried from this node using npa_query
 * with a query id of SLEEP_QUERY_WAKEUP_TIME.
 *
 * The time at which the processor MUST wake up (e.g., to service a timer) can
 * be queried from this node using npa_query with a query id of
 * SLEEP_QUERY_HARD_WAKEUP_TIME.
 *
 * ---------------------------------------------------------------------------
 *
 * "/core/cpu/latency" resource
 *
 * REQUIRED client, Units: microseconds
 * Used by clients who have restrictions on the maximum interrupt latency that
 * they can endure and still function correctly.  When a request is made the
 * sleep code will make sure that no mode or combination of modes is entered
 * that could potentially delay the handling of an interrupt beyond the
 * required period of time.
 *
 * NOTE: Do not use this node in an attempt to disable specific sleep modes.
 *       This node does restrict which sleep modes are entered, however future
 *       optimization to those modes may allow them to be entered in the future
 *       even if the same latency restriction is specified.  If you need a
 *       specific resource to remain on during sleep, request into that
 *       resource directly.
 *
 * ---------------------------------------------------------------------------
 *
 * "/rail/vdd_dig" resource
 *
 * REQUIRED client, Units: vdd_dig_state_type
 * Used by clients who would like to prevent VDD dig from going all the way 
 * down to its lowest retention voltage during CXO shutdown.  If a client 
 * requires that a wakeup interrupt other than the interrupts routed to the
 * MPM must fire during VDD min., then they should request 
 * SLEEP_VDD_DIG_LATCHING.  If a client requires that VDD dig minimization be
 * prevented altogether, and instead VDD dig must remain at its active voltage
 * during CXO shutdown, then they should request SLEEP_VDD_DIG_ACTIVE.
 * Otherwise, the client should cancel their request.
 *
 * ---------------------------------------------------------------------------
 *
 * "/rail/vdd_mem" resource
 *
 * REQUIRED client, Units: Active/Retention
 * Used by clients who would like to prevent VDD mem from going all the way 
 * down to its lowest retention voltages during CXO shutdown.  If a client 
 * requires that VDD mem minimization be prevented altogether, and instead VDD 
 * mem must remain at its active voltages during CXO shutdown, then they should 
 * request 1.  Otherwise, the client should cancel their request.
 *
 */

#endif /* SLEEPNPA_H */

