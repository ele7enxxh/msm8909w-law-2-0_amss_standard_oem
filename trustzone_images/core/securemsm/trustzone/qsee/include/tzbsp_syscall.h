#ifndef TZBSP_SYSCALL_H
#define TZBSP_SYSCALL_H

/**
@file tzbsp_syscall.h
@brief Provide internal system calls functions for the SYSCALL API
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_syscall.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
08/20/10   cap      Initial version.

===========================================================================*/
#include "tz_syscall_pub.h"
#include "tzbsp_syscall_priv.h"

struct tzbsp_regs_s;

/*===========================================================================

                              DEFINES

===========================================================================*/

#pragma diag_suppress 1295 /* Allow argument less function prototypes. */

/** Maxmimum number of system call arguments. */
#define TZBSP_SYSCALL_MAX_ARG       10

/**
 * This is used to tell the syscall handling to take the rest of the
 * memory in the request or response buffer at the point in the buffer
 * processing when an argument of this size is read and pass a pointer
 * to that location in the request/response buffer.
 */
#define TZBSP_PASS_BY_PTR_SIZE 16

/* Put all system calls in a specific memory section */
#define TZBSP_DEFINE_SYSCALL(id, flags, func, nargs, ...) \
   extern int func ();\
   static TZBSP_SYSCALL_ENTRY(nargs) tz_syscall_entry_ ##func \
   __attribute__((section("_tz_syscall"),used)) = \
  { id, #func, flags, func, nargs, __VA_ARGS__}

#define TZBSP_DEFINE_SYSCALL0(id, flags, func) \
  extern int func (); \
  static TZBSP_SYSCALL_ENTRY0 tz_syscall_entry_ ##func \
  __attribute__((section("_tz_syscall"),used)) = \
  { id, #func, flags, func, 0 }

/* Put all system calls in a specific memory section */
#define TZBSP_DEFINE_WEAK_SYSCALL(id, flags, func, nargs, ...) \
   extern int __weak func ();\
   static TZBSP_SYSCALL_ENTRY(nargs) tz_syscall_entry_ ##func \
   __attribute__((section("_tz_syscall"),used)) = \
  { id, #func, flags, func, nargs, __VA_ARGS__}

#define TZBSP_DEFINE_WEAK_SYSCALL0(id, flags, func) \
  extern int __weak func (); \
  static TZBSP_SYSCALL_ENTRY0 tz_syscall_entry_ ##func \
  __attribute__((section("_tz_syscall"),used)) = \
  { id, #func, flags, func, 0 }

extern const void * _tz_syscall$$Base;
extern const void * _tz_syscall$$Limit;

typedef int (*tzbsp_syscall_func_t) ();

typedef struct tzbsp_syscall_entry_s
{
  uint32 id;                    ///< id of the function to be called
  const char* name;
  uint32 flags;                 ///< flags for special syscall handling
  tzbsp_syscall_func_t func;    ///< Syscall function
  uint32 nargs;                 ///< number of arguments for the function
  uint32 len[];                 ///< array of the length of each argument

} tzbsp_syscall_entry_t;

/** overwrite resp status with return code */
#define TZ_SC_FLAG_O_STATUS        0x00000001

/** Pass void *rsp and rsp_len to syscall */
#define TZ_SC_FLAG_RSP             0x00000002

#define TZ_SC_FLAG_REGISTER        0x00000004
#define TZ_SC_FLAG_CONTROL         0x00000008
#define TZ_SC_FLAG_USE_CRYPTO      0x00000010
#define TZ_SC_FLAG_ENABLE_INT      0x00000020

/* Make no distinction between Register Class and Control Class
   syscalls */
#define TZ_SC_FLAG_SHARED_SYSCALL \
  (TZ_SC_FLAG_O_STATUS | \
   TZ_SC_FLAG_REGISTER | TZ_SC_FLAG_CONTROL)

/* The ARM C99 compiler does not support variable length arrays yet
   so we have to do this via macros and trick the compiler */
#define TZBSP_SYSCALL_ENTRY(num_args) \
  struct \
  { \
    uint32                id; \
    const char*           name; \
    uint32                flags; \
    tzbsp_syscall_func_t  func; \
    uint32                nargs; \
    uint32                len[num_args]; \
  }

/* The ARM compiler does not handle 0 length arrays */
#define TZBSP_SYSCALL_ENTRY0 \
  struct \
  { \
    uint32                id; \
    const char*           name; \
    uint32                flags; \
    tzbsp_syscall_func_t  func; \
    uint32                nargs; \
    uint32                len[]; \
  }


/**
  @brief Handle the parsing of the syscall and then call tz_do_syscall

  @param[in]      regs  - Pointer to non-secure register state

  @postcondition A side effect is that r0 in regs is set to the return code.
                  The return code is 0 on success, negative on failure.
*/
void tzbsp_syscall(struct tzbsp_regs_s * regs);

#endif
