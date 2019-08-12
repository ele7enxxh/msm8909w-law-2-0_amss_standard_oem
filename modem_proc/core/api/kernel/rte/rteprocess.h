#ifndef RTEPROCESS_H
#define RTEPROCESS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/kernel/rte/rteprocess.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/13   et      Created module.

===========================================================================*/


/*===========================================================================
   INCLUDES AND VARIABLE DEFINITIONS
===========================================================================*/

/*-------------------------------------------------------------------
   Include Files
-------------------------------------------------------------------*/
#include "comdef.h"
#include "rte.h"
#include "rteprocessrex.h"

/*-------------------------------------------------------------------
   Defines
-------------------------------------------------------------------*/
#define RTEPROCESS_DECLARE_PROCESS( pname ) \
   __RTEPROCESS_DECLARE_PROCESS( pname )

/*
** When defining the body of the process or thread
*/
#define RTEPROCESS(pname) \
   __RTEPROCESS_PROCESS(pname)

#define RTETHREAD(tname) \
   __RTEPROCESS_THREAD(tname)

/*
** When declaring processes and threads
*/
#define RTEPROCESS_DEFINE_PROCESS(name, pri, ssize, psize, prehook) \
   __RTEPROCESS_DEFINE_PROCESS(name, pri, ssize, psize, prehook)


#define RTEPROCESS_DEFINE_THREAD(name, pri, ssize, prehook) \
   __RTEPROCESS_DEFINE_THREAD(name, 0, pri, ssize, prehook)


#define RTEPROCESS_DEFINE_SERVICE(name, sid, pri, ssize, prehook) \
   __RTEPROCESS_DEFINE_THREAD(name, sid, pri, ssize, prehook)


/*-------------------------------------------------------------------
   Type Declarations
-------------------------------------------------------------------*/
typedef void (*rteprocess_hook_type)(void);

typedef enum
{
   RTEPROCESS_PROCESS_STATE_IDLE,
   RTEPROCESS_PROCESS_STATE_INIT,
   RTEPROCESS_PROCESS_STATE_READY,
   RTEPROCESS_PROCESS_STATE_COMING_UP,
   RTEPROCESS_PROCESS_STATE_UP,
   RTEPROCESS_PROCESS_STATE_COUNT   
} rteprocess_process_state_type;

struct rteprocess_thread_type
{
   struct rteprocess_process_type* process_ptr;
   struct rteprocess_thread_type*  next_ptr;
   char*                           name;
   uint32                          service_id;
   uint32                          priority;
   rte_thread_fptr_type            entry_point;
   uint32                          stack_size;
   rteprocess_hook_type            pre_hook;
   rte_thread_os_info_type         os_info;
};

struct rteprocess_process_type
{
   rteprocess_process_state_type  state;
   char*                          name;
   uint32                         priority;
   rte_process_fptr_type          entry_point;
   uint32                         stack_size;
   uint32                         pool_size;
   rteprocess_hook_type           pre_hook;
   struct rteprocess_thread_type  thread_list;
   struct rteprocess_thread_type* last_thread_ptr;
   uint32                         threads_count;
   rte_thread_os_info_type        os_info;   
};


/*-------------------------------------------------------------------
   Global Constant Declarations
-------------------------------------------------------------------*/


/*===========================================================================
   FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION
** Used by the System Monitor to create and start processes

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
#define RTEPROCESS_INIT(pname) rteprocess_init(&pname##_pobject)
void rteprocess_init
(
   struct rteprocess_process_type* process_ptr
);


/*===========================================================================
FUNCTION 
** Used by the System Monitor to create and start processes

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
#define RTEPROCESS_RUN(pname) rteprocess_run(&pname##_pobject)
void rteprocess_run
(
   struct rteprocess_process_type* process_ptr
);


/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
#define RTEPROCESS_REGISTER_THREAD(pname, tname) \
   __RTEPROCESS_REGISTER_THREAD(pname, tname)

/*=========================================================================*/
#endif /* RTEPROCESS_H */

