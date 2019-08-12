#ifndef CORE_CPU_H
#define CORE_CPU_H

/*============================================================================
  @file CoreCpu.h

  /core/cpu Public Interface Header

  Provide details of public interface.

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/rpm.bf/2.1.1/core/api/power/CoreCpu.h#1 $ */

#define CORE_CPU_NODE_NAME    "/core/cpu"

/****************************************************************************
 *
 * Resource Interface Documentation
 *
 * "/core/cpu" resource
 *
 * REQUIRED client, Units: MIPS
 * Used by clients who know their needs.  Sum of required 
 * clients set a minimum floor that the processor can go to.  It is likely 
 * that an increase in a client request (from 10 mips to 100 mips) will be 
 * accompanied by a predictive increase in performance.  The same is not 
 * true for the reverse situation.  As example, modem tasks will express 
 * their required levels so the cpu can never artificially limit performance.  
 * SCMM has both apps and modem running on the same cpu, and this will be 
 * especially important there.
 *
 * IMPULSE client, Units: NONE 
 * Used by clients who know they need quick response to an 
 * event, but don't know the amount of work that would be occurring.  As 
 * example, look at keypad input.  The driver knows the key has been 
 * received.  We know that the UI must respond promptly.  The driver doesn't 
 * know how much work will be following.  It will issue an impulse client 
 * against the resource, and the resource will jump to max performance.  
 * From that point, the clock level will be determined normally.
 *
 * ISOCHRONOUS client, Units: Time in us (microseconds)
 * *** Not Yet Available ***
 * Used by clients who know they have a consistent 
 * workload that is repeated, and has a known deadline.  Clients make 
 * requests at the workload start.  They specify a deadline with the request.  
 * When the work actually completes, the client issues a RequestComplete 
 * command.  Initially the system runs at max during these requests, but 
 * during these runs it will be measuring the actual load required by the 
 * client.  After it has learned this value, it will be able to adjust 
 * available performance in order to meet the client needs, meet the client 
 * deadline, and reduce power. Deadline is specified in TBD Units.  This 
 * client type is NOT yet implemented in full.  The interface is active, but 
 * the underlying logic is not.  Active Isochronous clients currently cause 
 * the system to run at max.
 *
 * "/core/cpu/dcvs/responsivity" resource 
 * *** Not Yet Available ***
 *
 * REQUIRED client, Units: UNDEFINED
 * Used by clients who have specific responsivity 
 * requirements.  The aggregated value will be presented to the DCVS 
 * algorithm which is expected to adjust itself accordingly.  Units are TBD.  
 * As such, the meaning of this resource is somewhat TBD.  This is an 
 * expression of how fast the system must respond to an increase in workload.  
 * If the cpu went from 100% idle to 100% busy, how long would it take to 
 * ramp to max.  This example makes poor units since it is not portable from 
 * target to target and algorithm to algorithm.
 *
 * "/core/cpu/dcvs/sensitivity" resource
 * *** Not Yet Available ***
 *
 * REQUIRED client, Units: UNDEFINED 
 * Used by clients who know their use case is not as highly 
 * correlated to idle percentage.  The intent is to make the algorithm more 
 * sensitive to idle measurement.  Units are TBD, and the necessity / 
 * implementation of this node remains in question.  The current algorithm 
 * works by comparing the idle percentage against thresholds.  In order to 
 * increase the performance level, you must be less than 10% idle.  The logic 
 * associated with this parameter would raise that threshold, and maybe allow 
 * 20% idle to raise the level.  This example makes for poor units since it 
 * is not portable from target to target and algorithm to algorithm.
 *
 */

#endif // CORE_CPU_H

