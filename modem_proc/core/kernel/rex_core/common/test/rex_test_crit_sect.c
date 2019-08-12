#include <stdio.h>
#include <string.h>

#include "rex_test.h"

#define REX_TEST_TASK_STARTED       1
#define REX_TEST_TASK_ENTERED_CS    2
#define REX_TEST_TASK_LEAVING_CS    3
#define REX_TEST_TASK_EXIT          4

#define REX_TEST_MULTI_CS_EXPIRATION_SIGS  (0x1000)
#define REX_TEST_MULTI_CS_EXPIRATION_VAL   (1000)

static int rex_cs_major_ctr = 0;
static int rex_cs_minor_ctr = 0;
static rex_crit_sect_type crit_sect_1;
static volatile int task_marker[5] = {0};
static volatile int task_exit_marker[5] = {0};

int rex_preliminary_crit_sect_tests(void);
int rex_crit_sect_multiple_task_tests(void);

static rex_tcb_type rex_test_cs_tcb_1;
static rex_tcb_type rex_test_cs_tcb_2;
static unsigned char rex_test_cs_stack_1[REX_TEST_STACK_SIZE];
static unsigned char rex_test_cs_stack_2[REX_TEST_STACK_SIZE];

void rex_test_cs_task(rex_task_func_param_type param);

int rex_start_critical_section_tests(void)
{
   /* Setup the test env here */
   rex_init_crit_sect(&crit_sect_1);
   
   /* Start the test cases */
   rex_cs_major_ctr++;
   rex_cs_minor_ctr=0;   
   rex_preliminary_crit_sect_tests();

   rex_cs_major_ctr++;
   rex_cs_minor_ctr=0;
   rex_crit_sect_multiple_task_tests();
   
   /* Tear down the test env here */
   rex_del_crit_sect(&crit_sect_1);
   
   return REX_SUCCESS;
}

int rex_preliminary_crit_sect_tests(void)
{
   boolean flag;
   int result = REX_SUCCESS;
   rex_crit_sect_type crit_sect;
   
   rex_init_crit_sect(&crit_sect);
   REX_TEST_LOG_RESULT("REX_CRIT_SECT_PRELIM", rex_cs_major_ctr, ++rex_cs_minor_ctr, result);
   rex_enter_crit_sect(&crit_sect);
   REX_TEST_LOG_RESULT("REX_CRIT_SECT_PRELIM", rex_cs_major_ctr, ++rex_cs_minor_ctr, result);
   rex_leave_crit_sect(&crit_sect);
   REX_TEST_LOG_RESULT("REX_CRIT_SECT_PRELIM", rex_cs_major_ctr, ++rex_cs_minor_ctr, result);
   flag = rex_del_crit_sect(&crit_sect);
   result = ((TRUE==flag)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("REX_CRIT_SECT_PRELIM", rex_cs_major_ctr, ++rex_cs_minor_ctr, result);
   
   rex_init_crit_sect(&crit_sect);
   flag = rex_try_enter_crit_sect(&crit_sect);
   result = ((TRUE==flag)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("REX_CRIT_SECT_PRELIM", rex_cs_major_ctr, ++rex_cs_minor_ctr, result);
   if (TRUE == flag)
   {
      rex_leave_crit_sect(&crit_sect);
   }
   rex_del_crit_sect(&crit_sect);

   return REX_SUCCESS;
}

int rex_crit_sect_multiple_task_tests(void)
{
   rex_task_attributes_type attr;
   int status;
   rex_priority_type prio = rex_get_pri();
   rex_tcb_type *p_tcb = rex_self();
   rex_timer_type tst_timer;

   rex_def_timer(&tst_timer, p_tcb, REX_TEST_MULTI_CS_EXPIRATION_SIGS);

   rex_enter_crit_sect(&crit_sect_1);

   /* Now that we are in a CS mark as entered */
   task_marker[0] = REX_TEST_TASK_ENTERED_CS;

   /* Now that we are in the critical section 
   ** spawn two higher priority tasks than the current 
   ** thread and make sure they block on the common crit 
   ** sect
   */
   rex_test_helper_set_common_task_attributes(&attr, 
                                                rex_test_cs_stack_1,  
                                                (prio + 1),
                                                rex_test_cs_task, 
                                                1,
                                                "REXTST_CS1");
   rex_task_attr_set_cpu_affinity(&attr, REX_ANY_CPU_AFFINITY_MASK);
   status = rex_common_def_task(&rex_test_cs_tcb_1, &attr);
   if (REX_SUCCESS != status)
   {
      return REX_ERROR;
   }
      
   rex_test_helper_set_common_task_attributes(&attr, 
                                                rex_test_cs_stack_2,  
                                                (prio + 2),
                                                rex_test_cs_task, 
                                                2,
                                                "REXTST_CS2");
   rex_task_attr_set_cpu_affinity(&attr, REX_ANY_CPU_AFFINITY_MASK);
   status = rex_common_def_task(&rex_test_cs_tcb_2, &attr);
   if (REX_SUCCESS != status)
   {
      return REX_ERROR;
   }

   /* At this point REXTST_CS1 and REXTST_CS1 
   ** should be allowed to run and should be 
   ** blocked and waiting in the CS, check the task
   ** marker for this
   */
   do
   {
      if ((REX_TEST_TASK_STARTED == task_marker[1]) &&
          (REX_TEST_TASK_STARTED == task_marker[2]))
      {
         break;
      }
      else
      {
         rex_timed_wait(REX_TEST_MULTI_CS_EXPIRATION_SIGS,
                        &tst_timer, 
                        REX_TEST_MULTI_CS_EXPIRATION_VAL);
         rex_clr_sigs(p_tcb, REX_TEST_MULTI_CS_EXPIRATION_SIGS);
      }
   } while(1);

   /* Now that we are about to leave the CS mark it as such */
   task_marker[0] = REX_TEST_TASK_LEAVING_CS;   
   rex_leave_crit_sect(&crit_sect_1);
   
   /* Wait until both tasks have exited so that we 
   ** can clean up any allocated resources
   */
   do
   { 
      if ((REX_TEST_TASK_EXIT == task_exit_marker[1]) && 
          (REX_TEST_TASK_EXIT == task_exit_marker[2]))
      {
         break;
      }
      else
      {
         rex_timed_wait(REX_TEST_MULTI_CS_EXPIRATION_SIGS,
                        &tst_timer, 
                        REX_TEST_MULTI_CS_EXPIRATION_VAL);
         rex_clr_sigs(p_tcb, REX_TEST_MULTI_CS_EXPIRATION_SIGS);
      }      
   } while(1);  
   
   /* Undef the timer */
   rex_undef_timer(&tst_timer);
   
   return REX_SUCCESS;
}

void rex_test_cs_task(rex_task_func_param_type param)
{
   boolean flag;
   int result, tc = 0;
   
   /* At this point the crit sect is taken and when we "try" this shd return FALSE */
   flag = rex_try_enter_crit_sect(&crit_sect_1);
   result = ((FALSE==flag)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("REX_CS_MULTIPLE", param, ++tc, result);

   /* Indicate that task was created and about to enter crit sect */
   task_marker[param] = REX_TEST_TASK_STARTED;

   /* Task is expected to block here since there is a task already in the CS*/
   rex_enter_crit_sect(&crit_sect_1);

   /* Now that we are in a CS mark as entered */
   task_marker[param] = REX_TEST_TASK_ENTERED_CS;

   /* Check if the parent task marked that it has left the CS */
   result = ((REX_TEST_TASK_LEAVING_CS == task_marker[0])?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("REX_CS_MULTIPLE", param, ++tc, result);

   /* If I am thread #2 check if thread #1 (lower prio) started but not yet in the CS */
   if (2 == param)
   {
      result = ((REX_TEST_TASK_STARTED == task_marker[1])?REX_SUCCESS:REX_ERROR);
      REX_TEST_LOG_RESULT("REX_CS_MULTIPLE", param, ++tc, result);      
   }
   /* If I am thread #1 check if thread #2 (higher prio) marked that it has left the CS */
   else
   {
      result = ((REX_TEST_TASK_LEAVING_CS == task_marker[2])?REX_SUCCESS:REX_ERROR);
      REX_TEST_LOG_RESULT("REX_CS_MULTIPLE", param, ++tc, result);
   }

   /* Now that the task is about to leave the CS, mark it as such */
   task_marker[param] = REX_TEST_TASK_LEAVING_CS;
   rex_leave_crit_sect(&crit_sect_1);

   /* Now indicate that the task is going to exit */
   task_exit_marker[param] = REX_TEST_TASK_EXIT;
}
