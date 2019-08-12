#ifndef  _CORE_SERVER_PLATFORM_H_
#define  _CORE_SERVER_PLATFORM_H_
/*---------------------------------------------------------------------------
   CORE_SERVER_PLATFORM.H
---------------------------------------------------------------------------*/
/*!
  @file
    core_server_platform.h

  @brief
    This file contains the platform specific definitions for the core
    server framework

  ---------------------------------------------------------------------------
  Copyright (c) 2011 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
---------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$$

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 

===========================================================================*/

#include <stdlib.h>

#include "task.h"
#include "rex.h"
#include "msg.h"
#include "err.h"
#include "comdef.h"
#include "stringl/stringl.h"

#define ALLOCATOR( _SIZE_IN_BYTES_ )     calloc(1, _SIZE_IN_BYTES_ )
#define DEALLOCATE( _PTR_ )              free( _PTR_ ) 


#define DEBUG MSG_HIGH
#define CLASS_NAME_MAX REX_TASK_NAME_LEN
#define TASK_NAME_MAX REX_TASK_NAME_LEN

//Rex based implementation of task
typedef struct _rex_task_impl {
    rex_tcb_type        rex_tcb;
    rex_sigs_type       rex_sigs;
    rex_priority_type   rex_prio;
    rex_stack_word_type stk_size;
    rex_stack_word_type *rex_stk;
}task_impl;


typedef struct _rex_lock_impl {
    rex_crit_sect_type  crit_sec;
}lock_impl;


typedef struct _rex_timer_impl {
   rex_timer_type      timer_type;
   rex_timer_cnt_type  time_msecs;
}timer_impl_type;

/* Platform specific locking macros */
#define LOCK_DATA(_LOCK_VARIABLE_) \
           rex_enter_crit_sect(&(_LOCK_VARIABLE_.crit_sec))

#define UNLOCK_DATA(_LOCK_VARIABLE_)\
           rex_leave_crit_sect(&(_LOCK_VARIABLE_.crit_sec))

#define LOCK_INIT_DATA(_LOCK_VARIABLE_,_LOCK_ATTR_) \
           rex_init_crit_sect(&(_LOCK_VARIABLE_.crit_sec))

#endif 
