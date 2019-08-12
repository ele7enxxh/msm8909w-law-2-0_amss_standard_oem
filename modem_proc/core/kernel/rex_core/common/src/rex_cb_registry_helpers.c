#include "rexcbregistry.h"
#include "rex_port.h"

int rex_invoke_cbs_on_init(void)
{
   /* Registry of callback functions to be called when REX is initialized*/
   const rex_init_cb_fp* cb_registry = NULL;
   int err_code = 0;
   static int rex_init_cb_funcs_called = 0;

   /* Issue the init callback functions only once */
   if (0 == rex_init_cb_funcs_called)
   {
      rex_init_cb_funcs_called++;
      cb_registry = rex_get_init_cb_list();
      if(NULL != cb_registry)
      {
         while(NULL != *cb_registry)
         {
            err_code = (*cb_registry)(NULL);
            if(0 != err_code)
            {
               REX_ERR_FATAL("REX Init Callback Failed");
            }
            cb_registry++;
         }
      }
   }

   return REX_SUCCESS;
}

int rex_invoke_cbs_on_thread_create(rex_tcb_type *tcb_ptr)
{
   /* registry of callback functions to be called when a thread is created */
   const rex_thread_create_cb_fp* cb_registry = NULL;
   int err_code = 0;
 
   /* Execute the thread create callback functions that have been registered with REX */
   cb_registry = rex_get_thr_create_cb_list();
   if(NULL != cb_registry)
   {
      while(NULL != *cb_registry)
      {
         err_code = (*cb_registry)(tcb_ptr);
         if(0 != err_code)
         {
            REX_ERR_FATAL("Thread Create Callback Failed");
         }
         cb_registry++;
      }
   }
   
   return REX_SUCCESS;
}

int rex_invoke_cbs_on_thread_destroy(rex_tcb_type *tcb_ptr)
{
   /* registry of callback functions to be called when a thread is destroyed */
   const rex_thread_destroy_cb_fp* cb_registry = NULL;      
   int err_code = 0;
      
   /*
   ** Execute the thread destroy callback functions that have been registered
   ** with REX. These functions will execute in reverse order of the execution
   ** of the corresponding thread create callback function. 
   */
   cb_registry = rex_get_thr_destroy_cb_list();
   if(NULL != cb_registry)
   {
      while(NULL != *cb_registry)
      {
         err_code = (*cb_registry)(tcb_ptr);
         if(0 != err_code)
         {
            REX_ERR_FATAL("Thread Destroy Callback Failed");
         }
         cb_registry++;
      }
   }
   
   return REX_SUCCESS;   
}
