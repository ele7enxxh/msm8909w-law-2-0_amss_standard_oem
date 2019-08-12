#include "string.h"
#include "rex_test.h"

extern rex_tcb_type rex_test_main_tcb;
extern rex_task_func_param_type rex_test_main_task_param;
static int prelim_minor_ctr = 0;
static int prelim_major_ctr = 0;

/* Tests to check validity of the Main Test Thread */
int rex_start_preliminary_tests(void)
{
   char task_name[REX_TEST_NAME_SIZE];
   rex_priority_type prio;
   rex_tcb_type *p_tcb;
   rex_tcb_type *p_main_test_tcb = &rex_test_main_tcb;
   int result;
   size_t task_name_len = 0;

   /* Init the Major Counter */
   prelim_major_ctr++;
   
   /* rex_self() test */
   p_tcb = rex_self();
   result = ((p_tcb == p_main_test_tcb)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("PRELIM_TST", prelim_major_ctr, ++prelim_minor_ctr, result);

   /* Do not run any other tests */
   if (REX_SUCCESS != result)
   {
      return result;
   }

   /* Test REX task name */
   result = rex_get_task_name(p_main_test_tcb, task_name, sizeof(task_name), &task_name_len);
   REX_TEST_LOG_RESULT("PRELIM_TST_NAME", prelim_major_ctr, ++prelim_minor_ctr, result);
   result = ((strlen(task_name) == task_name_len)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("PRELIM_TST_NAME", prelim_major_ctr, ++prelim_minor_ctr, result);
   result = strncmp(task_name, REX_TEST_MAIN_TASK_NAME, REX_TEST_NAME_SIZE);
   result = ((0 == result)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("PRELIM_TST_NAME", prelim_major_ctr, ++prelim_minor_ctr, result);

   /* Test REX task start input param */
   result = ((REX_TEST_MAIN_TASK_PARAM == rex_test_main_task_param)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("PRELIM_TST_PARAM", prelim_major_ctr, ++prelim_minor_ctr, result);
   
   /* Test REX priority */
   prio = rex_get_pri();
   result = ((REX_TEST_MAIN_TASK_PRIO == prio)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("PRELIM_TST_PRIO", prelim_major_ctr, ++prelim_minor_ctr, result);   
   
   /* Change the priority */
   prio = rex_set_pri(REX_TEST_MAIN_TASK_PRIO+1);
   result = ((REX_TEST_MAIN_TASK_PRIO == prio)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("PRELIM_TST_PRIO", prelim_major_ctr, ++prelim_minor_ctr, result);   
   prio = rex_get_pri();
   result = ((REX_TEST_MAIN_TASK_PRIO+1 == prio)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("PRELIM_TST_PRIO", prelim_major_ctr, ++prelim_minor_ctr, result);
   /* Restore the old prio back */
   prio = rex_set_pri(REX_TEST_MAIN_TASK_PRIO);
   result = ((REX_TEST_MAIN_TASK_PRIO+1 == prio)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("PRELIM_TST_PRIO", prelim_major_ctr, ++prelim_minor_ctr, result);
   
   return REX_SUCCESS;
}
