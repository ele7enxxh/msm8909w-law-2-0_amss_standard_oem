#include "assert.h"
#include "rex.h"
#include "rex_port.h"
#include "rex_tcb.h"


void rex_init_crit_sect(rex_crit_sect_type  *p_crit_sect)
{
   /* Check critical section pointer */
   ASSERT(NULL != p_crit_sect);
   REX_OS_MUTEX_INIT(p_crit_sect);
}

void rex_enter_crit_sect(rex_crit_sect_type *p_crit_sect)
{
   // rex_tcb_type *p_tcb;
   // rex_os_tcb_type *p_os_tcb = NULL;

   /* Check critical section pointer */
   ASSERT(NULL != p_crit_sect);

   /* If this is a REX task, tell WDog to stop monitoring this task. */
   // p_tcb = rex_self();
   // if (NULL != p_tcb)
   // {
      // REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);
      // REX_PAUSE_DOG_MONITOR(p_os_tcb);
   // }

   /* Enter the mutex, function call expected to assert on error */
   REX_OS_MUTEX_LOCK(p_crit_sect);

   /* If this is a REX task, tell WDog to resume monitoring this task. */
   // if (NULL != p_os_tcb)
   // {
      // REX_RESUME_DOG_MONITOR(p_os_tcb);
   // }
}

boolean rex_try_enter_crit_sect(rex_crit_sect_type *p_crit_sect)
{
   /**@note Try enter has never provided watchdog pausing */
 
   /* Check critical section pointer */
   ASSERT(NULL != p_crit_sect);
   return ((0 == REX_OS_MUTEX_TRY_LOCK(p_crit_sect))?TRUE:FALSE);
}

void rex_leave_crit_sect(rex_crit_sect_type *p_crit_sect)
{
   /* Check critical section pointer */
   ASSERT(NULL != p_crit_sect);
   /* Unlock mutex */   
   REX_OS_MUTEX_UNLOCK(p_crit_sect);
}

boolean rex_del_crit_sect(rex_crit_sect_type *p_crit_sect)
{
   /* Check critical section pointer */
   ASSERT(NULL != p_crit_sect);
   /* Destroy mutex */
   REX_OS_MUTEX_DELETE(p_crit_sect);
   return TRUE;
}
