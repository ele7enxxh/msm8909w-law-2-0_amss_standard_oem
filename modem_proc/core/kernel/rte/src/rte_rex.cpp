/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           S E C U R I T Y   S E R V I C E S   T A S K
                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/




/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/rte/src/rte_rex.cpp#1 $

===========================================================================*/


#include "core_variation.h"
#include "rex.h"
#include "rexcbregistry.h"
#include "ipcns.h"
#include "pq.h"
#include "msg.h"
#include "ipcrex.h"
#include "rex_restricted.h"
#include "assert.h"

#if defined(FEATURE_QUARTZ_IPC) || defined (FEATURE_REX_IPC)
extern rex_crit_sect_type ipc_info_crit_sect;
extern rex_crit_sect_type ipcns_table_crit_sect;

extern "C"
{
   int rte_rex_init_cb(void*);
   int rte_rex_def_task_cb(rex_tcb_type *p_tcb);
   int rte_rex_kill_task_cb(rex_tcb_type *p_tcb);
}

REXCBRegistry 
rex_core_common_registry
(
   rte_rex_init_cb,
   rte_rex_def_task_cb,
   rte_rex_kill_task_cb
);

int rte_rex_init_cb(void*)
{
   /* Initialize the crit section for ipc_info used in RTE code */
   rex_init_crit_sect(&ipc_info_crit_sect);   
   rex_init_crit_sect(&ipcns_table_crit_sect);   
   
   return REX_SUCCESS;
}

int rte_rex_def_task_cb(rex_tcb_type *p_tcb)
{
   boolean flag = ipcns_node_register(p_tcb);
   
   if (FALSE == flag)
   {
      return REX_ERROR;
   }

   return REX_SUCCESS;
}

int rte_rex_kill_task_cb(rex_tcb_type *p_tcb)
{
   ipcns_node_deregister(p_tcb);
   /* The API above returns void so always return REX_SUCCESS */
   return REX_SUCCESS;
}

int rte_rex_clr_ipc_info(rex_ipc_info_type *p_ipc_info, rex_sigs_type *p_clr_sigs)
{
   rex_sigs_type clr_sigs;
   if(!p_ipc_info)
   {
      return 0;
   }

   clr_sigs = *p_clr_sigs;
   /* Don't clear IPC Receive Signal if a message is pending */
   if ((clr_sigs & p_ipc_info->receive_signal) &&
       !pq_is_empty(p_ipc_info->mq_ptr))
   {
      clr_sigs &= ~(p_ipc_info->receive_signal); /* IPCDEFI_RECEIVE_SIG */
   }
   *p_clr_sigs = clr_sigs;
   
   return 0;
}

rex_ipc_info_type *rte_rex_get_ipc_info(rex_tcb_type *p_tcb)
{
   rex_ipc_info_type *p_node;
   int result;

   result = rex_get_ipc_info_ref(p_tcb, &p_node);
   ASSERT(REX_SUCCESS == result);

   return p_node;
}

#endif //defined(FEATURE_QUARTZ_IPC) || defined (FEATURE_REX_IPC)
