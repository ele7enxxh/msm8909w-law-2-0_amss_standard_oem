#ifndef __REX_OS_PORT_H__
#define __REX_OS_PORT_H__

#include "rex_tcb.h"

void rex_posix_init(void);
void rex_os_thread_init(rex_os_tcb_type *p_os_tcb);
void rex_os_thread_deinit(rex_os_tcb_type *p_os_tcb);

typedef void (*rex_task_list_for_each_fp)(rex_os_tcb_type *, unsigned int);

int rex_task_list_mgr_init(void);
int rex_task_list_mgr_add_task(rex_os_tcb_type *p_os_tcb);
int rex_task_list_mgr_remove_task(rex_os_tcb_type *p_os_tcb);
void rex_task_list_mgr_for_each(rex_task_list_for_each_fp fp, unsigned int param);
int rex_get_num_tasks(void);
void rex_sigs_init(void);
void rex_context_init(void);

#endif //__REX_OS_PORT_H__
