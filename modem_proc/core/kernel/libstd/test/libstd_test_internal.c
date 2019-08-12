/*===============================================================================================
 * FILE:        libstd_test_internal.c
 *
 * DESCRIPTION: Implementation of libstd test environment functions.
 *
 *              Copyright (c) 1999 - 2010 Qualcomm Technologies Incorporated.
 *              All Rights Reserved. QUALCOMM Proprietary and Confidential.
 *===============================================================================================*/
 
/*===============================================================================================
 *
 *                            Edit History
 * $Header: //components/rel/core.mpss/3.5.c12.3/kernel/libstd/test/libstd_test_internal.c#1 $ 
 * $DateTime: 2016/12/13 07:59:23 $
 *
 *===============================================================================================*/
#include <stdlib.h>
#include <string.h>
#include "libstd_test.h"

#define LIBSTD_FREEIF(ptr)  \
    if (NULL != ptr)        \
    {                       \
        free(ptr);          \
    }
    
typedef struct _libstd_test_buffer
{    
    int     test_number;
    int     line_number;    
    char    function_name[LIBSTD_FUNC_NAME_LEN];
    boolean result;
    struct _libstd_test_buffer *next;
    struct _libstd_test_buffer *prev;
} libstd_test_buffer;

static libstd_test_buffer *p_libstd_log_list_head = NULL;
static libstd_test_buffer *p_libstd_log_list_tail = NULL;

int libstd_test_log_init(void)
{
    p_libstd_log_list_head = (libstd_test_buffer *)malloc(sizeof(libstd_test_buffer));
    if (!p_libstd_log_list_head)
    {
        goto libstd_test_log_init_err_exit;
    }
    p_libstd_log_list_tail = (libstd_test_buffer *)malloc(sizeof(libstd_test_buffer));
    if (!p_libstd_log_list_tail)
    {
        goto libstd_test_log_init_err_exit;
    }
    p_libstd_log_list_head->next = p_libstd_log_list_tail;
    p_libstd_log_list_head->prev = NULL;
    p_libstd_log_list_tail->next = NULL;
    p_libstd_log_list_tail->prev = p_libstd_log_list_head;
    
    return 0;

libstd_test_log_init_err_exit:
    LIBSTD_FREEIF(p_libstd_log_list_head);
    LIBSTD_FREEIF(p_libstd_log_list_tail);
    return -1;
}

int libstd_test_log_destroy(void)
{
    libstd_test_buffer *p_log, *p_log_next;
    p_log = p_libstd_log_list_head;
    while(p_log)
    {
        p_log_next = p_log->next;
        free(p_log);
        p_log = p_log_next;
    }
    p_libstd_log_list_head = NULL;
    p_libstd_log_list_tail = NULL;

    return 0;
}

int libstd_log_result(int test_number, boolean result, int line_number, const char *function_name)
{
    libstd_test_buffer *node = NULL;
    node = (libstd_test_buffer *)malloc(sizeof(libstd_test_buffer));
    if (NULL != node)
    {
        node->test_number = test_number;
        node->result = result;
        node->line_number = line_number;
        memcpy(node->function_name, function_name, LIBSTD_FUNC_NAME_LEN);
        node->prev = p_libstd_log_list_head;
        node->next = p_libstd_log_list_head->next;
        p_libstd_log_list_head->next->prev = node;
        p_libstd_log_list_head->next = node;
        return 0;
    }
    return -1;
}
