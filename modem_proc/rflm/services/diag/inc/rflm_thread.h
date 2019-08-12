/*!
  @file
  rflm_thread.h

  @brief
  RFLM thread definitions.

  @detail
  This is not intended as an abstraction layer for the RTOS implementation.
  It just provides a simpler interface, extra parameter verification, and
  some extra features (like stack overflow checking).

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/diag/inc/rflm_thread.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/


#ifndef __RFLM_THREAD_H_
#define __RFLM_THREAD_H_

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define RFLM_DIAG_LOG_THREAD_PRI        250
#define RFLM_THREAD_MAX_NAME_LEN        128

//! @brief RFLM firmware thread info structure.
typedef struct {
  char              name[RFLM_THREAD_MAX_NAME_LEN];
  int               priority;
  qurt_thread_t     thread;
  void 	            *stackAddr;
  int               stackSize;
  qurt_anysignal_t  signals;
} rflm_thread_info;

//! @brief Macro to initialize a thread structure.
#define RFLM_DEFINE_THREAD_INFO(name, stack_addr, size) \
{ \
  "RFLM_" # name,\
  RFLM_##name##_THREAD_PRI,\
  0,\
  stack_addr,\
  size,\
  {0}\
}

//! @brief Default stack size of RFLM threads. This is defined to be 
// the default autoallocated thread stack sizes.
#define RFLM_DEFAULT_STACK_SZ             4096

/*!< CFW Threads */
#define RFLM_LOG_THREAD_STACK_SZ        RFLM_DEFAULT_STACK_SZ
#define RFLM_CCS_THREAD_STACK_SZ        RFLM_DEFAULT_STACK_SZ

#endif
