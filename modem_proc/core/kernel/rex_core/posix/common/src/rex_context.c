#include "assert.h"
#include "rex.h"
#include "rex_restricted.h"
#include "rex_tcb.h"
#include "rex_port.h"
#include "rex_os_port.h"

pthread_key_t rex_posix_tls_key;

void rex_context_init(void)
{
   /* Init the POSIX TLS key to store REX context info */
    if (pthread_key_create(&rex_posix_tls_key, NULL))
    {
        REX_ERR_FATAL("REX posix tls key is invalid");
    }
}

rex_tcb_type *rex_self(void)
{
   return (rex_tcb_type*) pthread_getspecific(rex_posix_tls_key);
}
   
void rex_context_set_self_ref(rex_tcb_type *p_tcb)
{
   /**
      @note Set the p_tcb ref in the TLS so that rex_self() returns p_tcb.
      ASSERT if an error condtion is encountered.
      @note NULL is acceptable input because during thread destrory
      routine when we want to dereference the thread from REX TCB
   */
   int err_code = pthread_setspecific(rex_posix_tls_key, (void*)p_tcb);
   ASSERT(0 == err_code);
}

int _rex_create_context_for_external_thread
(
   rex_tcb_type *p_tcb, 
   rex_task_attributes_type *p_attr
)
{
   int err_code = REX_SUCCESS;
   rex_os_tcb_type *p_os_tcb;
   pthread_t thread_id;

   do
   {
    
      /* Create the OS TCB */
      p_os_tcb = calloc(1, sizeof(rex_os_tcb_type));
      if (NULL == p_os_tcb)
      {
         err_code = REX_ERROR_NO_MEM;
         break;
      }

      /* Init the ROP TCB */
      if( (err_code =  rex_tcb_init(p_os_tcb)) != REX_SUCCESS )
      {
         break;
      }

      /* Set the user passed in TCB ref in the ROP TCB */
      REX_INIT_OS_AND_USER_TCB_REFS(p_os_tcb, p_tcb);
      thread_id = pthread_self();
      p_tcb->thread_id = thread_id;
      rex_os_set_thread_id(p_os_tcb, thread_id);

      //For now these are the only values taken
      rex_os_set_stack_addr(p_os_tcb, p_attr->p_orig_stack_limit);
      rex_os_set_stack_size(p_os_tcb, p_attr->orig_stack_size);
      rex_os_set_task_name(p_os_tcb, p_attr->task_name);
      
      rex_context_set_self_ref(p_tcb);

      /* Perform any OS specific thread initialization */
      rex_os_thread_init(p_os_tcb);
      
      /* Add this task to the list to list of REX tasks */
      err_code = rex_task_list_mgr_add_task(p_os_tcb);
   } while(0);
   
   if (REX_SUCCESS == err_code)
   {
      /*
      ** Execute the thread create callback functions that have registered
      ** with REX after the OS context was created
      */
      err_code = rex_invoke_cbs_on_thread_create(p_tcb);
   }
   else
   {
      if (NULL != p_os_tcb)
      {
         rex_tcb_delete(p_os_tcb);
         free(p_os_tcb);
      }
   }

   return err_code;
}

int _rex_destroy_context_for_external_thread(rex_tcb_type *p_tcb)
{
   int err_code;
   rex_os_tcb_type *p_os_tcb;
   
   if (NULL == p_tcb)
   {
      return REX_ERROR_BAD_PARAM;
   }

   /*
   ** Execute the thread destroy callback functions that have registered
   ** with REX after the OS context was destroyed.
   */
   do
   {
      err_code = rex_invoke_cbs_on_thread_destroy(p_tcb); 
      if (REX_SUCCESS != err_code)
      {
         break;
      }

      REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);

      /* Remove TCB from the task list */
      err_code = rex_task_list_mgr_remove_task(p_os_tcb);
      if (REX_SUCCESS != err_code)
      {
         break;
      }

      /* Perform any OS specific thread de-initialization */
      rex_os_thread_deinit(p_os_tcb);
      
      /* Set NULL ref in the TLS so that rex_self() returns NULL */
      rex_context_set_self_ref(NULL);

      /* At this point the TCB can be uninitialized and destroyed, 
      ** no more references to the TCB are valid.
      */
      rex_tcb_delete(p_os_tcb);
      
      /* Free the allocated memory from the REX context API */
      free(p_os_tcb);
      free(p_tcb);

   } while(0);

   return err_code;
}

void *rex_create_context_for_external_thread(rex_task_attributes_type *p_attr)
{
   rex_tcb_type *p_tcb = NULL;
   
   do
   {
      if(NULL == p_attr)
      {
         break;
      }

      p_tcb = (rex_tcb_type*)calloc(1, sizeof(rex_tcb_type));
      if (NULL == p_tcb)
      {
         break;
      }

      if (REX_SUCCESS != _rex_create_context_for_external_thread(p_tcb, p_attr))
      {
         free(p_tcb);
         p_tcb = NULL;
         break;
      }

   } while(0);

   return (void*)p_tcb;
}

int rex_destroy_context_for_external_thread(void)
{
   pthread_t thread_id;
   rex_os_tcb_type *p_os_tcb;
   rex_tcb_type *p_tcb = rex_self();
   REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);
   thread_id = rex_os_get_thread_id(p_os_tcb);
   /* Make sure that the exiting thread is the current thread */
   if (0 == pthread_equal(pthread_self(), thread_id)) 
   {
        REX_ERR_FATAL("Threads are not equal");
   }  
   return ((REX_SUCCESS == _rex_destroy_context_for_external_thread(p_tcb))?0:-1);
}
