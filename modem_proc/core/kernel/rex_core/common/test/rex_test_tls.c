#include "rex.h"
#include "rex_test.h"
#include "pthread.h"

static int tls_minor_ctr = 0;
static int tls_major_ctr = 1;

static rex_tcb_type rex_test_tls_tcb;
static unsigned char rex_test_tls_stack[REX_TEST_STACK_SIZE];
void rex_test_tls_task(rex_task_func_param_type param);

#define NUM_TLS_KEYS    5
#define BAD_KEY         100

static pthread_key_t keys[NUM_TLS_KEYS] = {0};

void rex_test_tls_task_destr(void *param);

int rex_start_tls_tests(void)
{
    rex_task_attributes_type attr;
    int status;
    
    /* Create the other thread to access TLS */
    rex_test_helper_set_common_task_attributes(&attr, 
                                                rex_test_tls_stack,  
                                                (REX_TEST_MAIN_TASK_PRIO - 1),
                                                rex_test_tls_task, 
                                                0, 
                                                "REXTST_TLS");
    
    rex_task_attr_set_cpu_affinity(&attr, REX_ANY_CPU_AFFINITY_MASK);
    status = rex_common_def_task(&rex_test_tls_tcb, &attr);
    if (REX_SUCCESS != status)
    {
      return REX_ERROR;
    }

    return REX_SUCCESS;
}

void rex_test_tls_task(rex_task_func_param_type param)
{
    int result, i; 
    param = param;

    for(i=0; i<NUM_TLS_KEYS; i++)
    {
        result = pthread_key_create((&(keys[i])), rex_test_tls_task_destr);
        result = ((0 == result)?REX_SUCCESS:REX_ERROR);
        REX_TEST_LOG_RESULT("REXTLS_CREATE", tls_major_ctr, ++tls_minor_ctr, result);
    }
    
    tls_minor_ctr = 0;
    tls_major_ctr++;
    for(i=0; i<NUM_TLS_KEYS; i++)
    {
        result = pthread_setspecific(keys[i], (void*)(i+100));
        result = ((0 == result)?REX_SUCCESS:REX_ERROR);
        REX_TEST_LOG_RESULT("REXTLS_SET", tls_major_ctr, ++tls_minor_ctr, result);
    }

    tls_minor_ctr = 0;
    tls_major_ctr++;
    for(i=0; i<NUM_TLS_KEYS; i++)
    {
        result = (int)pthread_getspecific(keys[i]);
        result = ((i+100 == result)?REX_SUCCESS:REX_ERROR);
        REX_TEST_LOG_RESULT("REXTLS_GET", tls_major_ctr, ++tls_minor_ctr, result);
    }

    tls_minor_ctr = 0;
    tls_major_ctr++;
    result = (int)pthread_getspecific(BAD_KEY);
    result = ((0 == result)?REX_SUCCESS:REX_ERROR);
    REX_TEST_LOG_RESULT("REXTLS_BADKEY", tls_major_ctr, ++tls_minor_ctr, result);
    
    tls_minor_ctr = 0;
    tls_major_ctr++;
    result = (int)pthread_key_delete(keys[0]);
    result = ((0 == result)?REX_SUCCESS:REX_ERROR);
    REX_TEST_LOG_RESULT("REXTLS_DEL", tls_major_ctr, ++tls_minor_ctr, result);

    result = (int)pthread_getspecific(keys[0]);
    result = ((0 != result)?REX_SUCCESS:REX_ERROR);
    REX_TEST_LOG_RESULT("REXTLS_DEL", tls_major_ctr, ++tls_minor_ctr, result);
    
    ++tls_major_ctr;
    tls_minor_ctr = 0;
}

void rex_test_tls_task_destr(void *param)
{
    int result = (int)param;
    switch(result)
    {
        case 101:
        case 102:
        case 103:
        case 104:
            result = REX_SUCCESS;
            break;
        default:
            result = REX_ERROR;
    }
    REX_TEST_LOG_RESULT("REXTLS_DESTR", tls_major_ctr, ++tls_minor_ctr, result);
}
