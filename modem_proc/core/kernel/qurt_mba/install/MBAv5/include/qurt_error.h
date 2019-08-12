#ifndef QURT_ERROR_H
#define QURT_ERROR_H

/*======================================================================

                        qurt_error.h 

GENERAL DESCRIPTION
  Error results- QURT defines a set of standard symbols for the error result values. This file lists the
  symbols and their corresponding values.

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2009-2011
 by Qualcomm Technologies, Inc. All Rights Reserved.
 ======================================================================*/
/*======================================================================
 
											 EDIT HISTORY FOR FILE
 
	 This section contains comments describing changes made to the
	 module. Notice that changes are listed in reverse chronological
	 order.
 
	$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_error.h#1 $ 
	$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
  
	when				who 		what, where, why
	----------	--- 		------------------------------------------------
	2011-02-25	op			Add Header file
 ======================================================================*/

/*=====================================================================
Constants and macros
======================================================================*/
#define QURT_EOK                             0  /**< Operation successfully performed */
#define QURT_EVAL                            1  /**< Wrong values for parameters/ Specified page does not exist */
#define QURT_EMEM                            2  /**< Not enough memory to perform operation.*/

#define QURT_EINVALID                        4  /**< Invalid argument value, invalid key */ 
#define QURT_EUNKNOWN                        6  /**< Defined but never used in BLAST */
#define QURT_ENOMSGS                         7  /**< Message queue empty */
#define QURT_EFAILED                        12  /**< Operation failed */
#define QURT_ENOTALLOWED                    13  /**< Not one shot timer/ not allowed/ Interrupt already registered */
#define QURT_EDUPCLSID                      14  /**< Duplicate class ID */
#define QURT_ENOREGISTERED                  20  /**< No registered interrupts.*/
#define QURT_EISDB                          21  /**< Power Collapse failed due to ISDB enabled */
#define QURT_ESTM                           22  /**< Power Collapse failed in Single threaded mode check */
#define QURT_ETLSAVAIL                      23  /**< No free TLS key available */
#define QURT_ETLSENTRY                      24  /**< TLS key is not already free */
#define QURT_EINT                           26  /**< Invalid interrupt number (not registered). */
#define QURT_ESIG                           27  /**< Invalid signal bitmask (can't set more than one signal at a time) */
#define QURT_EHEAP                          28  /**< No heap space available */
#define QURT_EMEMMAP                        29  /**< Physical address layout not supportable by kernel */
#define QURT_ENOTHREAD                      30  /**< Thread no longer exists */
#define QURT_EL2CACHE                       31  /**< L2cachable is not supported in kernel invalidate/cleaninv */
#define QURT_EALIGN                         32  /**< Error - Not alignment */
#define QURT_EDEREGISTERED                  33  /**< error interrupt is already de-registered*/
#define QURT_ETLBCREATESIZE                 34  /**< TLB create error: incorrect sizes*/
#define QURT_ETLBCREATEUNALIGNED            35  /**< TLB create error: unaligned address*/

#define QURT_EFATAL                         -1  /**< FATAL error which should never happen */

/* Enable Floating point exceptions */
#define QURT_FP_EXCEPTION_ALL        0x1F << 25
#define QURT_FP_EXCEPTION_INEXACT    0x1 << 29
#define QURT_FP_EXCEPTION_UNDERFLOW  0x1 << 28
#define QURT_FP_EXCEPTION_OVERFLOW   0x1 << 27
#define QURT_FP_EXCEPTION_DIVIDE0    0x1 << 26
#define QURT_FP_EXCEPTION_INVALID    0x1 << 25


#endif /* QURT_ERROR_H */
