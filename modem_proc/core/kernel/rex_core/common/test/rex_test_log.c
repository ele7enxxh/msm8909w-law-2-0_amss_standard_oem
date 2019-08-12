#include <string.h>
#include "rex_test.h"

extern size_t strlcpy(char *dst, const char *src, size_t siz);

typedef struct _rex_test_log_struct
{
   char test_name[REX_TEST_NAME_SIZE];
   int test_major_num;
   int test_minor_num;
   int test_result;
   struct _rex_test_log_struct *next;
   struct _rex_test_log_struct *prev;
} rex_test_log_type;


static rex_test_log_type *p_rex_log_list_head = NULL;
static rex_test_log_type *p_rex_log_list_tail = NULL;
static rex_crit_sect_type rex_test_log_crit_sect;

int rex_test_log_init(void)
{
   int err_code;
   
   rex_init_crit_sect(&rex_test_log_crit_sect);
   
   p_rex_log_list_head = (rex_test_log_type*)malloc(sizeof(rex_test_log_type));
   if(!p_rex_log_list_head)
   {
      err_code = REX_ERROR_NO_MEM;
      goto rex_log_init_exit;
   }
   
   p_rex_log_list_tail = (rex_test_log_type*)malloc(sizeof(rex_test_log_type));
   if(!p_rex_log_list_tail)
   {
      err_code = REX_ERROR_NO_MEM;
      goto rex_log_init_exit;   
   }

   p_rex_log_list_head->next = p_rex_log_list_tail;
   p_rex_log_list_head->prev = NULL;
   p_rex_log_list_tail->next = NULL;
   p_rex_log_list_tail->prev = p_rex_log_list_head;

   return REX_SUCCESS;

rex_log_init_exit:   
   REX_TEST_FREEIF(p_rex_log_list_head);
   REX_TEST_FREEIF(p_rex_log_list_tail);

   return err_code;
}

int rex_test_log_destroy(void)
{
   rex_test_log_type *p_log, *p_log_next;
   p_log = p_rex_log_list_head;
   while(p_log)
   {
      p_log_next = p_log->next;
      free(p_log);
      p_log = p_log_next;
   }
   p_rex_log_list_head = NULL;
   p_rex_log_list_tail = NULL;
   
   rex_del_crit_sect(&rex_test_log_crit_sect);
   
   return REX_SUCCESS;
}

int rex_log_result(char *name, int major, int minor, int result)
{
   rex_test_log_type *p_log;
   
   p_log = (rex_test_log_type*)malloc(sizeof(rex_test_log_type));
   if (NULL == p_log)
   {
      REX_ERR_FATAL("Could not log REX Test, No mem");
   }
   if (NULL != name)
   {
      strlcpy(p_log->test_name, name, REX_TEST_NAME_SIZE);
   }
   p_log->test_major_num = major;
   p_log->test_minor_num = minor;
   p_log->test_result = result;

   /* Insert next to head of the log list */
   rex_enter_crit_sect(&rex_test_log_crit_sect);
   p_log->prev = p_rex_log_list_head;
   p_log->next = p_rex_log_list_head->next;
   p_rex_log_list_head->next->prev = p_log;
   p_rex_log_list_head->next = p_log;
   rex_leave_crit_sect(&rex_test_log_crit_sect);
   
   return REX_SUCCESS;   
}
