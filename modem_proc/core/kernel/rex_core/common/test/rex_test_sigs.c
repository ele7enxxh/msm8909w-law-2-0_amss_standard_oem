#include "rex_test.h"

#define REX_SIGS_ALL_SIG_BITS          (0xFFFFFFFF)

#define REX_SIGS_TEST_HS_PING          (0x1000)
#define REX_SIGS_TEST_HS_PONG          (0x2000)
#define REX_SIGS_TEST_EXTRA_SIG_BITS   (0x1)

static int sigs_minor_ctr = 0;
static int sigs_major_ctr = 0;

static rex_tcb_type rex_test_sigs_ping_tcb;
static unsigned char rex_test_sigs_stack[REX_TEST_STACK_SIZE];
void rex_test_sigs_ping_task(rex_task_func_param_type param);

int rex_start_signal_tests(void)
{
   int result;
   rex_task_attributes_type attr;
   rex_sigs_type sigs[REX_SIGS_ARRAY_LEN];
   rex_sigs_type wait_ext_sigs[REX_SIGS_ARRAY_LEN];
   boolean ret_val;
   
   /* Init test counter */
   sigs_major_ctr++;

   /* Clear out any prior sigs and test if all bits are clear */
   sigs[0] = rex_clr_sigs(&rex_test_main_tcb, REX_SIGS_ALL_SIG_BITS);
   result = ((0 == sigs[0])?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("SIG_HS_TST", sigs_major_ctr, ++sigs_minor_ctr, result);

   /* Make sure get sigs also reports the same value */
   sigs[0] = rex_get_sigs(&rex_test_main_tcb);
   result = ((0 == sigs[0])?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("SIG_HS_TST", sigs_major_ctr, ++sigs_minor_ctr, result);
   
   /* Create the pong thread */
   rex_test_helper_set_common_task_attributes(&attr, 
                                                rex_test_sigs_stack,  
                                                (REX_TEST_MAIN_TASK_PRIO - 1),
                                                rex_test_sigs_ping_task, 
                                                0, 
                                                "REXTST_PING");
   rex_common_def_task(&rex_test_sigs_ping_tcb, &attr);

   /* Wait for ping sigs */
   sigs[0] = rex_wait(REX_SIGS_TEST_HS_PING | REX_SIGS_TEST_EXTRA_SIG_BITS);
   result = ((REX_SIGS_TEST_HS_PING == sigs[0])?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("SIG_HS_TST", sigs_major_ctr, ++sigs_minor_ctr, result);

   /* Clear out the ping sigs */
   sigs[0] = rex_clr_sigs(&rex_test_main_tcb, REX_SIGS_TEST_HS_PING);
   result = ((REX_SIGS_TEST_HS_PING == sigs[0])?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("SIG_HS_TST", sigs_major_ctr, ++sigs_minor_ctr, result);

   /* Make sure get sigs also reports the same value */
   sigs[0] = rex_get_sigs(&rex_test_main_tcb);
   result = ((0 == sigs[0])?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("SIG_HS_TST", sigs_major_ctr, ++sigs_minor_ctr, result);

   /* Now repeat the same test with ext signals using the ping thread */
   sigs_major_ctr++;
   sigs_minor_ctr=0;

   /* Clear out any prior sigs and test if all bits are clear */
   sigs[0] = REX_SIGS_ALL_SIG_BITS;
   sigs[1] = REX_SIGS_ALL_SIG_BITS;
   ret_val = rex_clr_sigs_ext(&rex_test_main_tcb, sigs);
   result = ((TRUE == ret_val)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("SIG_HS_TST", sigs_major_ctr, ++sigs_minor_ctr, result);

   /* Make sure get sigs also reports the same value */
   ret_val = rex_get_sigs_ext(&rex_test_main_tcb, sigs);
   if (ret_val)
   {
      if ((0 != sigs[0]) || (0 != sigs[1]))
      {
         ret_val = FALSE;
      }
   }
   result = ((TRUE == ret_val)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("SIG_HS_TST", sigs_major_ctr, ++sigs_minor_ctr, result);

   /* Send the pong sigs for the ping task and test what got set */
   sigs[0] = rex_set_sigs(&rex_test_sigs_ping_tcb, REX_SIGS_TEST_HS_PONG);
   result = ((REX_SIGS_TEST_HS_PONG | sigs[0])?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("SIG_HS_TST", sigs_major_ctr, ++sigs_minor_ctr, result);
   
   /* Wait for ext ping sigs */
   sigs[0] = (REX_SIGS_TEST_HS_PING | REX_SIGS_TEST_EXTRA_SIG_BITS);
   sigs[1] = (REX_SIGS_TEST_HS_PING | REX_SIGS_TEST_EXTRA_SIG_BITS);
   wait_ext_sigs[0] = 0;
   wait_ext_sigs[1] = 0;

   ret_val = rex_wait_ext(sigs, wait_ext_sigs);
   if (ret_val)
   {
      if ((REX_SIGS_TEST_HS_PING != wait_ext_sigs[0]) || (REX_SIGS_TEST_HS_PING != wait_ext_sigs[1]))
      {
         ret_val = FALSE;
      }
   }
   result = ((TRUE == ret_val)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("SIG_HS_TST", sigs_major_ctr, ++sigs_minor_ctr, result);

   /* Clear out the ping sigs */
   sigs[0] = sigs[1] = REX_SIGS_TEST_HS_PING;
   ret_val = rex_clr_sigs_ext(&rex_test_main_tcb, sigs);
   result = ((TRUE == ret_val)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("SIG_HS_TST", sigs_major_ctr, ++sigs_minor_ctr, result);

   /* Make sure get sigs reports that ping was cleared */
   ret_val = rex_get_sigs_ext(&rex_test_main_tcb, sigs);
   if (ret_val)
   {
      if ((0 != sigs[0]) || (0 != sigs[1]))
      {
         ret_val = FALSE;
      }
   }
   result = ((TRUE == ret_val)?REX_SUCCESS:REX_ERROR);
   REX_TEST_LOG_RESULT("SIG_HS_TST", sigs_major_ctr, ++sigs_minor_ctr, result);

   return REX_SUCCESS;
}

void rex_test_sigs_ping_task(rex_task_func_param_type param)
{
   rex_sigs_type sigs[REX_SIGS_ARRAY_LEN];

   /* Send ping sigs */
   rex_set_sigs(&rex_test_main_tcb, REX_SIGS_TEST_HS_PING);

   /* Wait for pong sigs */
   rex_wait(REX_SIGS_TEST_HS_PONG);

   /* Send ext ping sigs */
   sigs[0] = sigs[1] = REX_SIGS_TEST_HS_PING;
   rex_set_sigs_ext(&rex_test_main_tcb, sigs);
   
   /* task exits here */
}
