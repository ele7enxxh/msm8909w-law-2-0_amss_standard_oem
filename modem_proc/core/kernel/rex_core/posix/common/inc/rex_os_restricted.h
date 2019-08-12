#ifndef __REX_OS_RESTRICTED_H__
#define __REX_OS_RESTRICTED_H__

#include "pthread.h"
#include "rex.h"

/* Returns 0 on success and non zero otherwise */
void *rex_create_context_for_external_thread(rex_task_attributes_type *p_attr);
int rex_destroy_context_for_external_thread(void);

#endif //__REX_OS_RESTRICTED_H__
