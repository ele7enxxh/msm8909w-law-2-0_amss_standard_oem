#include <errno.h>
#include "assert.h"
#include "rex.h"
#include "rex_tcb.h"

/** Global mutex for INTLOCK/INTFREE replacement on qdsp6 */
REX_OS_LOCK_TYPE rexl4_int_lock;
/** counter to keep track of how times INTLOCK() call is nested. */
static int rex_int_lock_cnt = 0;
static int rex_stat_disabled = 0;

/* this is a global variable provided by BLAST */
extern unsigned int QURT_MAX_HTHREADS;

//REXTODO Remove this, this is bad
int* rex_get_errno_addr(void)
{
   rex_tcb_type *p_tcb = rex_self();
   rex_os_tcb_type *p_os_tcb;
   ASSERT(NULL != p_tcb);
   REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);
   return &(p_os_tcb->err_num);
}

boolean rex_os_is_null_user_stack_supported(void)
{
   return TRUE;
}

unsigned int rex_os_get_task_valid_cpu_affinity_bitmask(void)
{
   return ((1<<QURT_MAX_HTHREADS) - 1);
}

dword rex_int_lock(void)
{
   REX_OS_MUTEX_LOCK(&rexl4_int_lock);
   rex_int_lock_cnt++;
   return 0;
}

dword rex_int_free(void)
{
   rex_int_lock_cnt--;
   REX_OS_MUTEX_UNLOCK(&rexl4_int_lock);
   return 0;
}

int rex_ints_are_locked (void)
{
  /* This mechanism below should work on single threaded implementations. 
   * The rex_int_lock_cnt variable is incremented everytime we do 
   * INTLOCK and decremented everytime we do INTFREE. There is no 
   * window where another thread, even interrupt threads(ISTs) , are scheduled
   * when INTLOCK is taken. In that case rex_int_lock_cnt is guarenteed to be non-zero
   */ 
  ASSERT(rex_int_lock_cnt >= 0); 
  return rex_int_lock_cnt;
}

/**
  @brief  Empty stub for rex_task_lock
  @return None
*/
void rex_task_lock(void)
{
  /* Do nothing */
}

/**
  @brief  Empty stub for rex_task_free
  @return None
*/
void rex_task_free(void)
{
  /* Do nothing */
}

int rex_check_if_tasks_are_locked(void)
{
   return 0;
}

void rex_stat_disable (void)
{
  rex_stat_disabled++;
}

void rex_stat_enable (void)
{
  if (rex_stat_disabled > 0) rex_stat_disabled--;
}
int rex_os_get_pri(void)
{
  qurt_thread_t tid = REX_OS_GET_RTOS_TID();
  int prio = qurt_thread_get_priority(tid);
  ASSERT(prio != -1);
  return (255 - prio);
}