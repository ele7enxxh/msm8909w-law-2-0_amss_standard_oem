#include "rex_test.h"

rex_tcb_type rex_test_main_tcb;
rex_task_func_param_type rex_test_main_task_param = 0;
static unsigned char rex_test_main_stack[REX_TEST_STACK_SIZE];

void rex_test_main_task(rex_task_func_param_type param);
void rex_start_test(void)
{
   int err_code;
   err_code = rex_setup_test();
   if (REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Cannot Init REX Tests");
   }
}

int rex_setup_test(void)
{
   int err_code;
   rex_task_attributes_type attr;
   
   err_code = rex_test_log_init();
   if(REX_SUCCESS != err_code)
   {
      goto err_rex_setup_test;
   }

   rex_test_helper_set_common_task_attributes(&attr, 
                                          rex_test_main_stack,  
                                          REX_TEST_MAIN_TASK_PRIO,
                                          rex_test_main_task, 
                                          REX_TEST_MAIN_TASK_PARAM, 
                                          REX_TEST_MAIN_TASK_NAME);
   err_code = rex_common_def_task(&rex_test_main_tcb, &attr);
   if(REX_SUCCESS != err_code)
   {
      goto err_rex_setup_test;
   }

   /* At this point we are successful */
   return REX_SUCCESS;
   
err_rex_setup_test:
   rex_test_log_destroy();
   return err_code;
}

int rex_teardown_test(void)
{
   //rex_test_log_destroy();
   rex_kill_task(&rex_test_main_tcb);

   return REX_SUCCESS;
}

void rex_test_main_task(rex_task_func_param_type param)
{
   int status;
   do
   {
      rex_test_main_task_param = param;
      status = rex_start_preliminary_tests();
      if (REX_SUCCESS != status)
      {
         /* No need to run other test cases unless all preliminary 
         ** tests have passed
         */
         break;
      }
      
      rex_start_signal_tests();
      rex_start_timer_tests();
      rex_start_critical_section_tests();
   } while(0);
   
   rex_teardown_test();
}

int rex_test_helper_set_common_task_attributes
(
   rex_task_attributes_type   *p_rex_attr,
   rex_stack_pointer_type     p_stack,
   rex_priority_type          priority,
   rex_task_func_type         p_task,
   rex_task_func_param_type   param,
   char                       *task_name
)
{
   int err_code;
   rex_task_attr_init(p_rex_attr);

   err_code = rex_task_attr_set_stack_addr(p_rex_attr, p_stack);
   if(REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Invalid Stack addr specified when creating REX Task");
   }

   err_code = rex_task_attr_set_stack_size(p_rex_attr, REX_TEST_STACK_SIZE);
   if(REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Invalid Stack size specified when creating REX Task");
   }
   
   err_code = rex_task_attr_set_prio(p_rex_attr, priority);
   if(REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Invalid priority specified when creating REX Task");
   }
   rex_task_attr_set_cpu_affinity(p_rex_attr, REX_ANY_CPU_AFFINITY_MASK);
   rex_task_attr_set_entry_func(p_rex_attr, p_task, param);
   rex_task_attr_set_task_name(p_rex_attr, task_name);
   rex_task_attr_set_initially_suspended(p_rex_attr, FALSE);
   
   return REX_SUCCESS;
}
