#include "rex_test.h"
#define REX_TEST_TIMER_EXPIRATION_SIGS_1  (0x4)
#define REX_TEST_TIMER_EXPIRATION_SIGS_2  (0x8)
#define REX_TEST_TIMER_EXPIRATION_VAL     (500) //ms

static int timer_major_ctr = 0;
static int timer_minor_ctr = 0;
static rex_tcb_type *p_rex_timer_test_start_task_tcb = NULL;

int rex_prelim_timer_tests(void);
int rex_timer_pause_tests(void);

int rex_start_timer_tests(void)
{
   int result;
   
   p_rex_timer_test_start_task_tcb = rex_self();
   
   result = rex_prelim_timer_tests();
   if (REX_SUCCESS != result)
   {
      return result;
   }
   return rex_timer_pause_tests();
}

void timer_cb_func_1(rex_task_func_param_type param)
{
   int result;
   static int cb_tc_ctr = 0;
   result = ((100 == param)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("REX_TST_CB_1", timer_major_ctr, ++cb_tc_ctr, result);
   rex_set_sigs(p_rex_timer_test_start_task_tcb, REX_TEST_TIMER_EXPIRATION_SIGS_1);  
}

int rex_prelim_timer_tests(void)
{
   rex_timer_type tst_timer_1, tst_timer_2;
   rex_timer_type *p_tst_timer = NULL;
   rex_timer_cnt_type timer_ctr;
   int result;
   rex_tcb_type *p_tcb = rex_self();

   /* Init the test counter */
   ++timer_major_ctr;
   timer_minor_ctr = 0;

   /* Prelim Test 1 Std Timer*/
   rex_def_timer(&tst_timer_1, p_tcb, REX_TEST_TIMER_EXPIRATION_SIGS_1);
   result = REX_SUCCESS;
   REX_TEST_LOG_RESULT("REX_TIMER_PRELIM", timer_major_ctr, ++timer_minor_ctr, result);

   /* Set the timer */
   timer_ctr = rex_set_timer(&tst_timer_1, REX_TEST_TIMER_EXPIRATION_VAL);
   result = ((0 == timer_ctr)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("REX_TIMER_PRELIM", timer_major_ctr, ++timer_minor_ctr, result);

   /* Get the timer and check if the timer count value counted <= what was set */
   timer_ctr = rex_get_timer(&tst_timer_1);
   result = ((REX_TEST_TIMER_EXPIRATION_VAL <= timer_ctr)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("REX_TIMER_PRELIM", timer_major_ctr, ++timer_minor_ctr, result);

   /* Wait for timer to expire which then sends the sigs */
   rex_wait(REX_TEST_TIMER_EXPIRATION_SIGS_1);
   /* Got the sigs record test case result and clear the sigs */
   result = REX_SUCCESS;
   REX_TEST_LOG_RESULT("REX_TIMER_PRELIM", timer_major_ctr, ++timer_minor_ctr, result);
   rex_clr_sigs(p_tcb, REX_TEST_TIMER_EXPIRATION_SIGS_1);

   /* Get the timer and check if the timer count value to zero */
   timer_ctr = rex_get_timer(&tst_timer_1);
   result = ((0 == timer_ctr)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("REX_TIMER_PRELIM", timer_major_ctr, ++timer_minor_ctr, result);

   /* Test for rex_timed_wait, set a timeout value and wait until we get the sigs */
   rex_timed_wait(REX_TEST_TIMER_EXPIRATION_SIGS_1, &tst_timer_1, REX_TEST_TIMER_EXPIRATION_VAL);
   /* Got the sigs record test case result and clear the sigs */
   result = REX_SUCCESS;
   REX_TEST_LOG_RESULT("REX_TIMER_PRELIM", timer_major_ctr, ++timer_minor_ctr, result);
   rex_clr_sigs(p_tcb, REX_TEST_TIMER_EXPIRATION_SIGS_1);

   /* Prelim Test 2 Ex Timers */
   rex_def_timer_ex(&tst_timer_2, timer_cb_func_1, 100);
   result = REX_SUCCESS;
   REX_TEST_LOG_RESULT("REX_TIMER_PRELIM", timer_major_ctr, ++timer_minor_ctr, result);

   /* Set the timer */
   timer_ctr = rex_set_timer(&tst_timer_2, REX_TEST_TIMER_EXPIRATION_VAL);
   result = ((0 == timer_ctr)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("REX_TIMER_PRELIM", timer_major_ctr, ++timer_minor_ctr, result);

   /* Get the timer and see if the timer count value counted <= what was set */
   timer_ctr = rex_get_timer(&tst_timer_2);
   result = ((REX_TEST_TIMER_EXPIRATION_VAL <= timer_ctr)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("REX_TIMER_PRELIM", timer_major_ctr, ++timer_minor_ctr, result);

   /* Wait for timer to expire which calls the callback which then sends the sigs */
   rex_wait(REX_TEST_TIMER_EXPIRATION_SIGS_1);
   /* Got the sigs record test case result and clear the sigs */
   result = REX_SUCCESS;
   REX_TEST_LOG_RESULT("REX_TIMER_PRELIM", timer_major_ctr, ++timer_minor_ctr, result);
   rex_clr_sigs(p_tcb, REX_TEST_TIMER_EXPIRATION_SIGS_1);   

   /* Get the timer and check if the timer count value to zero */
   timer_ctr = rex_get_timer(&tst_timer_2);
   result = ((0 == timer_ctr)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("REX_TIMER_PRELIM", timer_major_ctr, ++timer_minor_ctr, result);

   /* Prelim test 3 Create Ex timer */
   p_tst_timer = rex_create_timer_ex(timer_cb_func_1, 100);
   result = ((NULL != p_tst_timer)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("REX_TIMER_PRELIM", timer_major_ctr, ++timer_minor_ctr, result);
   if (NULL != p_tst_timer)
   {
      /* Set the timer */
      timer_ctr = rex_set_timer(p_tst_timer, REX_TEST_TIMER_EXPIRATION_VAL);
      result = ((0 == timer_ctr)?REX_SUCCESS:REX_ERROR);
      REX_TEST_LOG_RESULT("REX_TIMER_PRELIM", timer_major_ctr, ++timer_minor_ctr, result);

      /* Get the timer and see if the timer count value counted <= what was set */
      timer_ctr = rex_get_timer(p_tst_timer);
      result = ((REX_TEST_TIMER_EXPIRATION_VAL <= timer_ctr)?REX_SUCCESS:REX_ERROR);
      REX_TEST_LOG_RESULT("REX_TIMER_PRELIM", timer_major_ctr, ++timer_minor_ctr, result);

      /* Wait for timer to expire which calls the callback which then sends the sigs */
      rex_wait(REX_TEST_TIMER_EXPIRATION_SIGS_1);
      /* Got the sigs record test case and clear the sigs */
      result = REX_SUCCESS;
      REX_TEST_LOG_RESULT("REX_TIMER_PRELIM", timer_major_ctr, ++timer_minor_ctr, result);
      rex_clr_sigs(p_tcb, REX_TEST_TIMER_EXPIRATION_SIGS_1);

      /* Get the timer and check if the timer count value to zero */
      timer_ctr = rex_get_timer(p_tst_timer);
      result = ((0 == timer_ctr)?REX_SUCCESS:REX_ERROR);
      REX_TEST_LOG_RESULT("REX_TIMER_PRELIM", timer_major_ctr, ++timer_minor_ctr, result);

      rex_delete_timer_ex(p_tst_timer);
      result = REX_SUCCESS;
      REX_TEST_LOG_RESULT("REX_TIMER_PRELIM", timer_major_ctr, ++timer_minor_ctr, result);
   }
   
   /* Undef the timers */
   rex_undef_timer(&tst_timer_1);
   rex_undef_timer(&tst_timer_2);

   return REX_SUCCESS;
}

int rex_timer_pause_tests(void)
{
   rex_timer_type tst_timer_1, tst_timer_2;
   rex_sigs_type sigs;
   int result;
   rex_tcb_type *p_tcb = rex_self();

   /* Init the test counters */
   ++timer_major_ctr;
   timer_minor_ctr = 0;
   
   /* Create the timers */
   rex_def_timer(&tst_timer_1, p_tcb, REX_TEST_TIMER_EXPIRATION_SIGS_1);
   rex_def_timer(&tst_timer_2, p_tcb, REX_TEST_TIMER_EXPIRATION_SIGS_2);

   /* Set the timer #1 to have a smaller expiration timeout that #2 */
   rex_set_timer(&tst_timer_1, REX_TEST_TIMER_EXPIRATION_VAL);
   rex_set_timer(&tst_timer_2, REX_TEST_TIMER_EXPIRATION_VAL*2);

   /* Pause the timer #1 */
   rex_pause_timer(&tst_timer_1);
   result = REX_SUCCESS;
   REX_TEST_LOG_RESULT("REX_TIMER_PAUSE", timer_major_ctr, ++timer_minor_ctr, result);
   
   sigs = rex_wait(REX_TEST_TIMER_EXPIRATION_SIGS_1 | REX_TEST_TIMER_EXPIRATION_SIGS_2);
   /* It is expected that timer #2 should expire before #1 */
   result = ((REX_TEST_TIMER_EXPIRATION_SIGS_2 == sigs)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("REX_TIMER_PAUSE", timer_major_ctr, ++timer_minor_ctr, result);
   rex_clr_sigs(p_tcb, REX_TEST_TIMER_EXPIRATION_SIGS_2);
   
   /* Resume the timer #1 */
   rex_resume_timer(&tst_timer_1);
   result = REX_SUCCESS;
   REX_TEST_LOG_RESULT("REX_TIMER_PAUSE", timer_major_ctr, ++timer_minor_ctr, result);

   sigs = rex_wait(REX_TEST_TIMER_EXPIRATION_SIGS_1 | REX_TEST_TIMER_EXPIRATION_SIGS_2);
   /* It is expected that timer #2 should expire before #1 */
   result = ((REX_TEST_TIMER_EXPIRATION_SIGS_1 == sigs)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("REX_TIMER_PAUSE", timer_major_ctr, ++timer_minor_ctr, result);
   rex_clr_sigs(p_tcb, REX_TEST_TIMER_EXPIRATION_SIGS_1);
   
   /* Undef the timers */
   rex_undef_timer(&tst_timer_1);
   rex_undef_timer(&tst_timer_2);
   
   return REX_SUCCESS;
}
