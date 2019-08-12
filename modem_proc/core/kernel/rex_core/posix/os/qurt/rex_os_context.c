#include "assert.h"
#include "pthread.h"
#include "rex.h"
#include "rex_port.h"
#include "rex_restricted.h"

void *rex_create_fake_tcb(void *stack_addr, size_t stack_size, char* task_name)
{
   rex_task_attributes_type attr;
   void *p_tcb;

   rex_task_attr_init(&attr);
   rex_task_attr_set_stack_size(&attr, stack_size);
   rex_task_attr_set_stack_addr(&attr, stack_addr);
   rex_task_attr_set_task_name(&attr, task_name);
   p_tcb = rex_create_context_for_external_thread(&attr);
   if (NULL == p_tcb)
   {
      REX_ERR_FATAL("Error when trying to Create REX TCB for a non REX thread");
   }
   
   return ((void*)rex_self());
}

void rex_destroy_fake_tcb(void *fake_tcb)
{
   rex_destroy_context_for_external_thread();
}
