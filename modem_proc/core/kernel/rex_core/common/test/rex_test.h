#ifndef __REX_TEST_H__
#define __REX_TEST_H__

#include "rex.h"
#include "rex_port.h"

#define REX_TEST_NAME_SIZE          (REX_TASK_NAME_LEN + 1)
#define REX_TEST_STACK_SIZE         1024
#define REX_TEST_MAIN_TASK_PRIO     100
#define REX_TEST_MAIN_TASK_NAME     "REXTST_MAIN"
#define REX_TEST_MAIN_TASK_PARAM    1010

#define REX_TEST_FREEIF(ptr)  \
   if (NULL != ptr)           \
   {                          \
      free(ptr);              \
      ptr = NULL;             \
   }

extern rex_tcb_type rex_test_main_tcb;

#define REX_TEST_LOG_RESULT(name, major, minor, result) rex_log_result(name, major, minor, result)
int rex_test_log_init(void);
int rex_test_log_destroy(void);
int rex_log_result(char *name, int major, int minor, int result);

int rex_test_helper_set_common_task_attributes
(
   rex_task_attributes_type   *p_rex_attr,
   rex_stack_pointer_type     p_stack,
   rex_priority_type          priority,
   rex_task_func_type         p_task,
   rex_task_func_param_type   param,
   char                       *task_name
);

void rex_start_test(void);
int rex_setup_test(void);
int rex_start_preliminary_tests(void);
int rex_start_tls_tests(void);
int rex_start_signal_tests(void);
int rex_start_critical_section_tests(void);
int rex_start_timer_tests(void);
int rex_teardown_test(void);

#endif //__REX_TEST_H__
