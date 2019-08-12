#ifndef QURT_NON_SYSTEM_H
#define QURT_NON_SYSTEM_H 

/*======================================================================

                        qurt_system.h 

GENERAL DESCRIPTION
  Contains misc system calls

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2009-2011
 by Qualcomm Technologies, Inc. All Rights Reserved.
 ======================================================================*/
/*======================================================================
 *
 *											 EDIT HISTORY FOR FILE
 *
 *	 This section contains comments describing changes made to the
 *	 module. Notice that changes are listed in reverse chronological
 *	 order.
 *
 *	$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_system.h#1 $ 
 *	$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when 				who 		what, where, why
 * ---------- 	--- 		------------------------------------------------
 * 2012-04-19 	nt			created
 ======================================================================*/
#include "qurt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
                        FUNCTIONS
=============================================================================*/

  /*
   * Perform a jump from a trusted user mode
   *
   * @param addr    Address where control will resume
   *
   */ 
void qurt_system_jump (qurt_addr_t addr);

#endif /* QURT_SYSTEM_H */
