#include "rex.h"
#include "rex_tcb.h"
#include "rex_tcb.h"
#include "rex_os_port.h"

typedef struct _rex_task_list_node
{
   /* This is being done this way because it facilitates user debugging */
   rex_os_tcb_type    *p_os_tcb;
   struct _rex_task_list_node *next_ptr;
   struct _rex_task_list_node *prev_ptr;
} rex_task_list_node;

static rex_task_list_node *rex_task_list = NULL;
static int rex_num_tasks = 0;
static rex_crit_sect_type rex_task_list_mutex;

/*----------------------------------------------------------------------
** The kernel task which is the first member on the task list
**--------------------------------------------------------------------*/
static rex_os_tcb_type rex_kernel_tcb;

#define REX_TASK_LIST_FRONT() (rex_task_list->next_ptr)

#define REX_TASK_LIST_PUSH(p_node, p_os_tcb)           \
   p_node->prev_ptr = rex_task_list;               \
   p_node->next_ptr = rex_task_list->next_ptr;       \
   if (NULL != rex_task_list->next_ptr)    \
   {                                                   \
      rex_task_list->next_ptr->prev_ptr = p_node;    \
   }                                                   \
   rex_task_list->next_ptr = p_node;             \
   p_node->p_os_tcb = p_os_tcb

INLINE
void rex_task_list_lock(void)
{
   rex_enter_crit_sect(&rex_task_list_mutex);
}

INLINE
void rex_task_list_unlock(void)
{
   rex_leave_crit_sect(&rex_task_list_mutex);
}

int rex_get_num_tasks(void)
{
   return rex_num_tasks;
}

int rex_task_list_mgr_init(void)
{
   /*Initialize the lock used for REX task list maintenence */
   rex_init_crit_sect(&rex_task_list_mutex);

   rex_task_list = (rex_task_list_node*)malloc(sizeof(rex_task_list_node));
   if (rex_task_list == NULL)
   {
      return REX_ERROR_NO_MEM;
   }

   /* Initialize the the first TCB */
   rex_tcb_init(&rex_kernel_tcb);
   
   /* Head TCB List */
   rex_task_list->p_os_tcb  = &rex_kernel_tcb;
   rex_task_list->next_ptr  = NULL;
   rex_task_list->prev_ptr  = NULL;

   return REX_SUCCESS;
}

void rex_task_list_mgr_for_each(rex_task_list_for_each_fp fp, unsigned int param)
{
   rex_task_list_node *p_node = REX_TASK_LIST_FRONT();
   rex_task_list_lock();
   {
      while (p_node)
      {
         fp(p_node->p_os_tcb, param);
         p_node = p_node->next_ptr;
      }
   }
   rex_task_list_unlock();   
}

/*===========================================================================
FUNCTION rex_task_list_mgr_remove_task

DESCRIPTION
  Function that implements the process of adding a TCB from
  the REX task list.
===========================================================================*/
int rex_task_list_mgr_add_task(rex_os_tcb_type *p_os_tcb)
{
   rex_task_list_node *p_node;
   int ret_val;

   p_node = (rex_task_list_node*)malloc(sizeof(rex_task_list_node));   
   if (p_node)
   {
      rex_task_list_lock();
      {
         /* Insert @ list head */
         REX_TASK_LIST_PUSH(p_node, p_os_tcb);
         rex_num_tasks++;
      }
      rex_task_list_unlock();
      ret_val = REX_SUCCESS;
   }
   else
   {
      ret_val = REX_ERROR_NO_MEM;
   }
   return ret_val;
}

/*===========================================================================
FUNCTION rex_task_list_mgr_remove_task

DESCRIPTION
  Function that implements the process of removing a TCB from
  the REX task list.
===========================================================================*/
int rex_task_list_mgr_remove_task(rex_os_tcb_type *p_os_tcb)
{
   rex_task_list_node *p_prev;
   rex_task_list_node *p_node;
   int ret_val = 0;

   rex_task_list_lock();
   {
      p_prev = rex_task_list;
      p_node = REX_TASK_LIST_FRONT();
      while (p_node)
      {
         if (0 != pthread_equal(rex_os_get_thread_id(p_node->p_os_tcb), rex_os_get_thread_id(p_os_tcb)))
         {
            //Found the thread now pull out node
            p_prev->next_ptr = p_node->next_ptr;

            //If not the last node...
            if (NULL != p_node->next_ptr)
            {
               p_node->next_ptr->prev_ptr = p_prev;
            }

            ret_val = 1;
            rex_num_tasks--;
            break;
         }
         p_prev = p_node;
         p_node = p_node->next_ptr;
      }
   }
   rex_task_list_unlock();

   if (ret_val)
   {
      free(p_node);
      ret_val = REX_SUCCESS;
   }
   else
   {
      ret_val = REX_ERROR_INVALID;
   }

   return ret_val;
}
