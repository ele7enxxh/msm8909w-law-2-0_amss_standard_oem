/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include "drStd.h"
#include "DrApi/DrApiCommon.h"
#include "TlApi/TlApiError.h"

/* Priority definitions */
#define EXCH_PRIORITY           MAX_PRIORITY
#define IPCH_PRIORITY           (MAX_PRIORITY-1)
#define DCIH_PRIORITY           (MAX_PRIORITY-1)
#ifdef NO_HW_TIMER
#define IRQH_PRIORITY           (3) //Must be lower than trustlet priority
#else
#define IRQH_PRIORITY           (MAX_PRIORITY-1)
#endif

/* Kernel exceptions */
#define TRAP_UNKNOWN        ( 0)    /**< message was sent due to an unknown exception. */
#define TRAP_SYSCALL        ( 1)    /**< message was sent due to an invalid syscall number. */
#define TRAP_SEGMENTATION   ( 2)    /**< message was sent due to an illegal memory access. */
#define TRAP_ALIGNMENT      ( 3)    /**< message was sent due to a misaligned memory access. */
#define TRAP_UNDEF_INSTR    ( 4)    /**< message was sent due to an undefined instruction. */
#define TRAP_BREAKPOINT     ( 5)    /**< message was sent due to a breakpoint exception. */
#define TRAP_ARITHMETIC     ( 6)    /**< message was sent due to an arithmetic exception. */
#define TRAP_INSTR_FETCH    ( 7)    /**< message was sent due to instruction fetch failure. */
#define TRAP_INTERRUPT      (16)    /**< message was sent due to an interrupt. */
#define TRAP_TIMEFAULT      (17)    /**< message was sent due to a timefault exception. */
#define TRAP_TASK_TERMINATE (18)    /**< message was sent due to a child task termination. */
#define TRAP_TASK_ACTIVATE  (19)    /**< message was sent due to a task activation */
#define TRAP_TASK_START     (20)    /**< message was sent due to a task start */

/* RTM specific definitions */
#define RTM_TASK             1
#define RTM_THREADNO_IPCH    3

/* Thread numbers */
#define DRIVER_THREAD_NO_EXCH      1
#define DRIVER_THREAD_NO_IPCH      2
#define DRIVER_THREAD_NO_IRQH      3
#define DRIVER_THREAD_NO_DCIH      4

/* Register ids for THREAD_EX_REGS syscall */
#define THREAD_EX_REGS_IP       (1U << 0)   /**<  Instruction pointer */
#define THREAD_EX_REGS_SP       (1U << 1)   /**<  Stack pointer */

#define THREADID_THREADNO_SHIFT     16
#define THREADID_TASKID_MASK        ((1 << THREADID_THREADNO_SHIFT) - 1)
#define THREADID_THREADNO_MASK      (~THREADID_TASKID_MASK)
#define GET_THREADNO(threadid)      (threadid >> THREADID_THREADNO_SHIFT)

#define CPSR_REG                    0x10
#define MAX_MEM_LENGTH              4096
#define LIFETIME_SEED_SIZE          16
#define STACK_DBG_INIT_VALUE        0xCC
#define fillStack(_name_,_val_) \
  memset(_STACK_ARRAY_VAR(_name_), _val_, getStackSize(_name_))
#define clearStack(_name_)      fillStack(_name_,STACK_DBG_INIT_VALUE)
#define TASKID_GET_THREADID(taskid,threadno) ((threadid_t)((taskid) | ((threadno) << THREADID_THREADNO_SHIFT)))
#define FUNC_PTR(func)              VAL2PTR( PTR2VAL( func ) )
#define getStackTop(_name_)         GET_STACK_TOP(_name_)

