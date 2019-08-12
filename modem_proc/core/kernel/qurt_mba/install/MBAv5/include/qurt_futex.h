#ifndef QURT_FUTEX_H
#define QURT_FUTEX_H
/*======================================================================
                        qurt_futex.h

GENERAL DESCRIPTION
  Prototypes of Kernel futex API functions      
  Futex calls directly in case you want to play with them from C
  
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
 
 	$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_futex.h#1 $ 
 	$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 
 
  when 				who 		what, where, why
  ---------- 	--- 		------------------------------------------------
  2011-02-25 	op			Add Header file
 ======================================================================*/
/*=====================================================================
 Functions
======================================================================*/

int qurt_futex_wait(void *lock, int val);
int qurt_futex_wake(void *lock, int n_to_wake);

#endif /* QURT_FUTEX_H */

