/**
@file tzbsp_syscall.c
@brief Support functions for system call API

*/

/*===========================================================================
   Copyright (c) 2010-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/kernel/syscall/tzbsp_syscall.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
08/19/10   cap      Created

===========================================================================*/

/*===========================================================================

          INCLUDE FILES

=============================================================================*/
#include <stdarg.h>
#include <string.h>

#include "tzbsp.h"
#include "tzbsp_errno.h"
#include "tzbsp_mem.h"
#include "tzbsp_sys.h"
#include "tzbsp_syscall.h"
#include "tz_syscall_pub.h"
#include "tzbsp_config.h"
#include "tzbsp_exec.h"
#include "crypto/tzbsp_crypto.h"
#include "tzbsp_mmu_cache.h"

extern const void *tz_syscall_core_start;
extern const void *tz_syscall_core_end;
extern const void *tz_syscall_chipset_start;
extern const void *tz_syscall_chipset_end;
extern const void *tz_syscall_oem_start;
extern const void *tz_syscall_oem_end;
#if (defined TZBSP_WITH_TEST_SVC)
static const void *tz_syscall_test_start          = NULL;
static const void *tz_syscall_test_end            = NULL;
static const void *tz_syscall_test_chipset_start  = NULL;
static const void *tz_syscall_test_chipset_end    = NULL;
#endif

extern const uint32 N_CPSR;

extern int qsee_do_syscall(void *cb, void *arg1, void *arg2, unsigned int arg3, int syscall_id);

const static struct
{
  const  void **start;
  const  void **end;
} sections[] =
              { {&tz_syscall_core_start,    &tz_syscall_core_end },
                {&tz_syscall_chipset_start, &tz_syscall_chipset_end },
                {&tz_syscall_oem_start,     &tz_syscall_oem_end },
#if (defined TZBSP_WITH_TEST_SVC)
                {&tz_syscall_test_start,    &tz_syscall_test_end },
                {&tz_syscall_test_chipset_start, &tz_syscall_test_chipset_end },
#endif
                    { NULL, NULL } };

#if (defined TZBSP_WITH_TEST_SVC)
/**
   Called by the TZBSP Test Executive image to set the location of the test syscalls
 */
void tzbsp_set_test_syscall_ranges(const void* core_start, const void* core_end,
                                   const void* chipset_start, const void* chipset_end)
{
  tz_syscall_test_start          = core_start;
  tz_syscall_test_end            = core_end;
  tz_syscall_test_chipset_start  = chipset_start;
  tz_syscall_test_chipset_end    = chipset_end;
}
#endif

/**
   Used to set the status of a system call to "done" for system calls
   that use the TZ_SC_FLAG_O_STATUS flag.
 */
static void inline tzbsp_syscall_set_quick_rsp(tz_syscall_rsp_t * rsp,
                                               int32 status)
{
  rsp->len = sizeof(tz_syscall_rsp_t);
  rsp->response_start_off = rsp->len;
  rsp->status = status;
}

/**
  @brief Lookup the system call based on an id

  @param[in]       id    - The command/function id to lookup
  @param[in]       flags - Flags to match in the lookup

  @return Pointer to a syscall_entry struct on success; otherwise NULL
*/
tzbsp_syscall_entry_t * tzbsp_syscall_lookup(uint32 id, uint32 flags)
{
  tzbsp_syscall_entry_t *cur = NULL;
  tzbsp_syscall_entry_t *start = NULL;
  tzbsp_syscall_entry_t *end = NULL;

  uint32 i = 0;

  while (sections[i].start != NULL &&
         sections[i].end != NULL)
  {
    start = *(tzbsp_syscall_entry_t**)sections[i].start;
    cur = start;
    end = *(tzbsp_syscall_entry_t**)sections[i].end;

    if ((uint32)end - (uint32)start < sizeof(tzbsp_syscall_entry_t))
    {
      i++;
      continue;
    }

    while ((uint32)cur < (uint32)end)
    {
      if (id == cur->id && (flags & cur->flags))
      {
        /* Some syscall registered for more args then they can handle */
        if (cur->nargs > TZBSP_SYSCALL_MAX_ARG)
          return NULL;

        return cur;
      }
      cur = (tzbsp_syscall_entry_t*)((uint8*)cur +
            (sizeof(tzbsp_syscall_entry_t) + (sizeof(uint32) * cur->nargs)));
    }
    ++i;
  }
  return NULL;
}

/**
 * Checks if a system call is available.
 *
 * @param [in]  id      The searched system call ID.
 * @param [out] found   Set to TRUE if system call found, FALSE otherwise.
 * @param [in] found_sz Size of the response (size of found).
 *
 * @return E_SUCCESS on success, error code otherwise.
 */
int tzbsp_is_service_available(uint32 id, boolean* found, uint32 found_sz)
{
  boolean qsee_is_moncall_available(uint32 id);

  if(found_sz < sizeof(boolean))
  {
    return -E_INVALID_ARG;
  }

  if(!tzbsp_is_ns_range(found, sizeof(boolean)))
  {
    return -E_BAD_ADDRESS;
  }

  if(tzbsp_syscall_lookup(id, TZ_SC_FLAG_CONTROL | TZ_SC_FLAG_REGISTER))
  {
    *found = TRUE;
  }
  else if(qsee_is_moncall_available(id))
  {
    *found = TRUE;
  }
  else
  {
    *found = FALSE;
  }

  return E_SUCCESS;
}

/**
  @brief Do some direct validation of the request header. The returned
         copy of the secure memory header is validated.

  @param[in]       sc_req         - Pointer to the HLOS syscall request
  @param[in,out]   sc_req_copy    - Pointer a secure memory area

  @return One of the following error codes
          <ul>
             <li> 0 on success, negative on failure.
          </ul>
*/
int tzbsp_syscall_val_header(tz_syscall_req_t * sc_req,
                             tz_syscall_req_t * sc_req_copy)
{
  if (sc_req_copy == NULL)
    return -E_INVALID_ARG;

  if (!tzbsp_is_ns_range((uint8 *) sc_req, sizeof(tz_syscall_req_t)))
    return -E_BAD_ADDRESS;

  if (tzbsp_is_ns_range((uint8 *) sc_req_copy, sizeof(tz_syscall_req_t)))
    return -E_BAD_ADDRESS;

  /* Invalidate the header to make sure we read fresh DDR */
  tzbsp_dcache_inval_region(sc_req, sizeof(tz_syscall_req_t));

  memcpy(sc_req_copy, sc_req, sizeof(tz_syscall_req_t));

  /* Check the length */
  if (sc_req_copy->len < sizeof(tz_syscall_req_t) ||
      sc_req_copy->len < sizeof(tz_syscall_rsp_t))
    return -E_BAD_ADDRESS;

  /* We can have void syscalls */
  if ((sc_req_copy->command_start_off > sc_req_copy->len) ||
      (sc_req_copy->command_start_off < sizeof(tz_syscall_req_t)))
    return -E_BAD_ADDRESS;

  /* Order of checking does matter so the HLOS cannot change parameters
     between IRQ's. */
  if (!tzbsp_is_ns_range((uint8 *) sc_req, sc_req_copy->len))
    return -E_BAD_ADDRESS;

  return 0;
}

/**
  @brief Perform a direct system call into TZBSP. Up to 10 arguments are
         supported.

  @param[in]      regs  - Pointer to non-secure register state
  @param[in]      func  - The function to be executed
  @param[in]      args  - The number of arguments

  @return One of the following error codes
          <ul>
             <li> 0 on success, negative on failure.
          </ul>
*/
int tzbsp_do_syscall(tzbsp_regs_t * regs, tzbsp_syscall_func_t func,
                     uint32 args)
{
  int ret = 0;

  /* TZBSP SYSCALL supports up to 10 arguments */
  switch (args)
  {
  case 0:
    ret = func();
    break;

  case 1:
    ret = func(regs->reg[2]);
    break;

  case 2:
    ret = func(regs->reg[2], regs->reg[3]);
    break;

  case 3:
    ret = func(regs->reg[2], regs->reg[3], regs->reg[4]);
    break;

  case 4:
    ret = func(regs->reg[2], regs->reg[3], regs->reg[4], regs->reg[5]);
    break;

  case 5:
    ret = func(regs->reg[2], regs->reg[3], regs->reg[4],
               regs->reg[5], regs->reg[6]);
    break;

  case 6:
    ret = func(regs->reg[2], regs->reg[3], regs->reg[4],
               regs->reg[5], regs->reg[6], regs->reg[7]);
    break;

  case 7:
    ret = func(regs->reg[2], regs->reg[3], regs->reg[4],
               regs->reg[5], regs->reg[6], regs->reg[7], regs->reg[8]);
    break;

  case 8:
    ret = func(regs->reg[2], regs->reg[3], regs->reg[4],
               regs->reg[5], regs->reg[6], regs->reg[7],
               regs->reg[8], regs->reg[9]);
    break;

  case 9:
    ret = func(regs->reg[2], regs->reg[3], regs->reg[4],
               regs->reg[5], regs->reg[6], regs->reg[7],
               regs->reg[8], regs->reg[9], regs->reg[10]);
    break;

  case 10:
    ret = func(regs->reg[2], regs->reg[3], regs->reg[4],
               regs->reg[5], regs->reg[6], regs->reg[7],
               regs->reg[8], regs->reg[9], regs->reg[10],
               regs->reg[11]);
    break;

  default:
    ret = -1;
  }

  return ret;
}

/**
  @brief Check the type of syscall being done (register or memory based)
         and create a common syscall interface to tzbsp_do_syscall

  @param[in]      regs  - Pointer to non-secure register state

  @return One of the following error codes
          <ul>
             <li> 0 on success, negative on failure.
          </ul>
*/
int tzbsp_execute_syscall(tzbsp_regs_t * regs)
{

  int id = 0, i = 0, r0 = 0, classid = 0;
  int len = 0, ret = 0;
  uint8 *start = NULL, *param = NULL;
  uint32 nargs = 0;
  uint32 int_flags = 0;

  tzbsp_regs_t tmp_regs;
  tz_syscall_req_t sc_req;
  tzbsp_syscall_entry_t *sc_entry = NULL;

  if (regs == 0)
    return -E_FAILURE;

  r0 = regs->reg[0];
  classid = TZ_SYSCALL_CLASS(r0);

  /* r2 holds a pointer to the command if CONTROL CLASS */
  start = (uint8 *)regs->reg[2];

  memcpy(&tmp_regs, regs, sizeof(*regs));

  /* Is this a TZ syscall */
  if (classid != TZ_REGISTER_CLASSID && classid != TZ_CONTROL_CLASSID)
  {
    return -E_INVALID_ARG;
  }

  if (classid == TZ_CONTROL_CLASSID)
  {
    int ret = tzbsp_syscall_val_header((tz_syscall_req_t *) start, &sc_req);
    if (ret < 0)
      return ret;

    id = sc_req.command_id;

    /* We can make a memory based command look like a direct syscall */
    if ((sc_entry = tzbsp_syscall_lookup(id, TZ_SC_FLAG_CONTROL)) == NULL)
      return -E_NOT_SUPPORTED;

    /* Make sure the response header pointer fits within the given buffer
     * when the call has a response header or response header & payload
     */
    if ((sc_entry->flags &
         (TZ_SC_FLAG_RSP | TZ_SC_FLAG_O_STATUS)) &&
        (sc_req.response_write_off >
         (sc_req.len - sizeof(tz_syscall_rsp_t))))
      return -E_BAD_ADDRESS;

    /* Invalidate syscall shared buffer arguments before reading. */
    if (sc_entry->nargs > 0)
    {
      tzbsp_dcache_inval_region(start, sc_req.len);
    }

    len = sc_req.len - sc_req.command_start_off;
    param = start + sc_req.command_start_off;

    for (; i < sc_entry->nargs && len > 0; ++i)
    {
      if (!tzbsp_is_ns_range(param, sc_entry->len[i]))
      {
        return -E_BAD_ADDRESS;
      }

      if (sc_entry->len[i] > sizeof(int))
      {
        /* pass a pointer to the arg */
        tmp_regs.reg[2 + i] = (int)param;
      }
      else
      {
        /* pass by value. We checked if param was in secure memory
           above so we can derefence it */
        tmp_regs.reg[2 + i] = *(int*)param;

        /* Convert input size to uint32 by trimming unused bits */
        if (sc_entry->len[i] < sizeof(int))
        {
          tmp_regs.reg[2 + i] &= (uint32)((0x1 << (sc_entry->len[i] * 8)) - 1);
        }
      }

      param += sc_entry->len[i];
      len -= sc_entry->len[i];
    }

    /* HLOS tried to pass invalid values */
    if (i != sc_entry->nargs || len < 0)
      return -E_INVALID_ARG;

    nargs = sc_entry->nargs;
    if (sc_entry->flags & TZ_SC_FLAG_RSP)
    {
      /* for now the classid based paramater passing is limiting the
         number of args */
      if (i >= (int32)TZBSP_SYSCALL_MAX_ARG -2)
        return -E_FAILURE;

      /* the last two register are used to pass the return */
      tmp_regs.reg[2 + i] = (uint32)start +
          sc_req.response_write_off + sizeof(tz_syscall_rsp_t);
      tmp_regs.reg[2 + i + 1] = sc_req.len - (sc_req.response_write_off +
                                              sizeof(tz_syscall_rsp_t));
	   //check if response pointer points to ns region of memory.
	  if (!tzbsp_is_ns_range(start +
								sc_req.response_write_off + sizeof(tz_syscall_rsp_t) ,sc_req.len - (sc_req.response_write_off + 
																								sizeof(tz_syscall_rsp_t))))
	    {
	       return -E_BAD_ADDRESS;
	    }
      nargs = sc_entry->nargs + 2;
    }
  }

  else if (classid == TZ_REGISTER_CLASSID)
  {
    id = TZ_SYSCALL_CALLID(r0);

    if ((sc_entry = tzbsp_syscall_lookup(id, TZ_SC_FLAG_REGISTER)) == NULL)
      return -E_NOT_SUPPORTED;

    if (sc_entry->flags & TZ_SC_FLAG_RSP)
    {
      if (TZ_SYSCALL_ARG(r0) != sc_entry->nargs + 2)
        return -E_INVALID_ARG;

      nargs = sc_entry->nargs + 2;
    }
    else
    {
      if (TZ_SYSCALL_ARG(r0) != sc_entry->nargs)
        return -E_INVALID_ARG;

      nargs = sc_entry->nargs;
    }
  }
 
  /* nargs >=13 because later we are using value nargs+1 */
  if(nargs >= 13 )
    return -E_NOT_SUPPORTED; 
  
  if (sc_entry == NULL || sc_entry->func == NULL)
    return -E_NOT_AVAILABLE;

  /* Make sure the resource is available */
  if ((sc_entry->flags & TZ_SC_FLAG_USE_CRYPTO) &&
      (tzbsp_crypto_lock_engine(TRUE) == FALSE))
    return -E_BUSY;

  int_flags = tzbsp_int_read();
  if ((sc_entry->flags & TZ_SC_FLAG_ENABLE_INT) &&
      /* Do not enable IRQs when NS has IRQs disabled */
      !(N_CPSR & TZBSP_IRQ_BIT))
    tzbsp_int_enable(int_flags | TZBSP_IRQ_BIT);

  /* Now we have what we need */
  ret = qsee_do_syscall((void *)tzbsp_do_syscall, (void *)&tmp_regs, (void *)(sc_entry->func), nargs, id);

  tzbsp_int_disable(TZBSP_IRQ_BIT);

  if (sc_entry->flags & TZ_SC_FLAG_USE_CRYPTO)
    tzbsp_crypto_lock_engine(FALSE);

  if (sc_entry->flags & TZ_SC_FLAG_O_STATUS &&
      classid == TZ_CONTROL_CLASSID)
  {
    tzbsp_syscall_set_quick_rsp(
      (tz_syscall_rsp_t*)(start + sc_req.response_write_off),
       1);
  }

  /* The start address is only valid for control class of syscalls */
  if (classid == TZ_CONTROL_CLASSID)
  {
    /* Flush the mem for the entire syscall so HLOS sees changes */
    tzbsp_dcache_flush_region((void *) start, sc_req.len);
  }
  /* For register based calls, just flush the response, if
   * applicable
   */
  else if ((classid == TZ_REGISTER_CLASSID) &&
           (sc_entry->flags & TZ_SC_FLAG_RSP))
  {
    tzbsp_dcache_flush_region((void *)tmp_regs.reg[nargs],
                              tmp_regs.reg[nargs + 1]);
  }

  return ret;
}

/*
 * Syscall called from secure or monitor context
 *
 * @param[in] regs    The non-secure saved registers
 */
void tzbsp_syscall(tzbsp_regs_t * regs)
{
  int classid = TZ_SYSCALL_CLASS(regs->reg[0]);
  int ret = 0;

  tzbsp_int_enable(TZBSP_EA_BIT | TZBSP_FIQ_BIT);

  if (classid == TZ_REGISTER_CLASSID ||
      classid == TZ_CONTROL_CLASSID)
  {
    ret = tzbsp_execute_syscall(regs);
    regs->reg[0] = ret;

    /* On error condition, log the end of a system call to separate out the
     * error code traces. */
    TZBSP_LOG_ERR_E(ret, "(end)");
  }
  else if (classid == TZ_VERSION_CLASSID)
  {
    regs->reg[0] = 0;
    regs->reg[1] = TZBSP_MAJOR_VERSION << 16 | TZBSP_MINOR_VERSION;
  }
  else
  {
    regs->reg[0] = (uint32)-E_NOT_SUPPORTED;
  }
}

int tzbsp_exec_smc(const void *req, const void *rsp, uint32 rsp_len)
{
  int32 ret = -E_NOT_SUPPORTED;

  if ((uint32)rsp < (uint32)req)
    return -E_INVALID_ARG;

  ret = tzbsp_exec_smc_handler(req,
                               (uint32)rsp - (uint32)req - sizeof(tz_syscall_rsp_t),
                               rsp,
                               rsp_len);
  return ret;
}

int tzbsp_exec_smc_ext(const void *req, const void *rsp, uint32 rsp_len)
{
  int32 ret = -E_NOT_SUPPORTED;

  if ((uint32)rsp < (uint32)req)
    return -E_INVALID_ARG;

  ret = tzbsp_exec_smc_handler_ext(req,
                                   (uint32)rsp - (uint32)req,
                                   rsp,
                                   rsp_len);
  return ret;
}

int tzbsp_tzos_smc(uint32 app_id, const void *req, const void *rsp, uint32 rsp_len)
{
  int32 ret = -E_NOT_SUPPORTED;

  if ((uint32)rsp < (uint32)req)
    return -E_INVALID_ARG;

  ret = tzbsp_tzos_smc_handler(app_id, req, (uint32)rsp - (uint32)req, rsp, rsp_len);
  return ret;
}
