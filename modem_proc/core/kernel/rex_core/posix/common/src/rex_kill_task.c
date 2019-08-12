#include "assert.h"
#include "pthread.h"
#include "rex_tcb.h"
#include "rex_os_port.h"
#include "rex_port.h"

void rex_os_kill_task(rex_tcb_type *p_tcb)
{
   rex_os_tcb_type *p_os_tcb;
   pthread_t victim_thread_id;
   int err_code;
   
   REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);
   REX_GET_THREAD_ID(p_tcb, &victim_thread_id);

   /* Sanity Check */
   if (0 != victim_thread_id)
   {
      pthread_t killer_thread_id = pthread_self();
      
      /* Is the thread killing itself */
      if (0 != pthread_equal(killer_thread_id, victim_thread_id))
      {
         /*-------------------------------------------------------
         ** Execute the thread destroy callback functions that have
         ** been registered with REX.
         **-----------------------------------------------------*/
         err_code = rex_invoke_cbs_on_thread_destroy(p_tcb);
         if (REX_SUCCESS != err_code)
         {
            REX_ERR_FATAL("Error rex_invoke_cbs_on_init() was not called or did not complete.");
         }
         
         /* Perform any OS specific thread de-initialization */
         rex_os_thread_deinit(p_os_tcb);

         /* Remove TCB from the task list */
         rex_task_list_mgr_remove_task(p_os_tcb);
         
         /* At this point the TCB can be uninitialized and destroyed, 
         ** no more references to the TCB are valid.
         */
         rex_tcb_delete(p_os_tcb);

         /* *************************************** */
         /* No access to p_os_tcb after this point  */
         /* *************************************** */
		 free(p_os_tcb);

         /* Killing self */
         pthread_exit(0);
      }
      else
      {
         void *value_ptr;
         if (pthread_join(victim_thread_id, &value_ptr))
         {
            REX_ERR_FATAL("PThread join failed");
         }
         /* Cancel the victim thread (may not be supported on all platforms) */
         //ASSERT(0 == pthread_cancel(victim_thread_id));
      }
   }
}
