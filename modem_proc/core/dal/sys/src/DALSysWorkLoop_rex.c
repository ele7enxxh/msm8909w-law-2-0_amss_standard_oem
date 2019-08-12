/*==============================================================================
@file  DALSYSWorkLoop_rex.c

DALSYS Work Loop implementation using REX thread

        Copyright © 2013 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary
==============================================================================*/
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALSysCmn.h"
#include "qurt.h"
#include "rex.h"

extern void _WorkLoopExecute(void *pArg);

DALResult 
DALSYS_WorkLoopThreadCreate(_DALSYSWorkLoopObj *pWorkLoopObj, 
                             char * pszname, 
                             unsigned int dwStackSize, 
                             unsigned int dwPriority )
{
   rex_task_attributes_type attr;
   
   //malloc rex tcb.
   if(DAL_SUCCESS != DALSYS_Malloc(sizeof(rex_tcb_type),
                                  (void**)&pWorkLoopObj->hThread))
   { 
      return DAL_ERROR;
   }
   DALSYS_memset((void*)pWorkLoopObj->hThread,0, sizeof(rex_tcb_type));
   
   // set the attribute for workloop.   
   rex_task_attr_init(&attr);
   if(REX_SUCCESS != rex_task_attr_set_stack_size(&attr, dwStackSize))
   {
      goto thread_create_abort;
   }
   
   if(REX_SUCCESS != rex_task_attr_set_stack_addr(&attr, 
                     (unsigned char * )pWorkLoopObj->pStackBuffer))
   {
      goto thread_create_abort;
   }
   
   if(REX_SUCCESS != rex_task_attr_set_prio(&attr, dwPriority))
   {
      goto thread_create_abort;   
   }
   
   if(REX_SUCCESS != rex_task_attr_set_task_name(&attr, pszname))
   {
      goto thread_create_abort;   
   }

   if(REX_SUCCESS != rex_task_attr_set_entry_func(&attr, 
         (rex_task_func_type)_WorkLoopExecute, 
         (rex_task_func_param_type)pWorkLoopObj))
   {
      goto thread_create_abort;   
   }

   //create rex thread
   if(REX_SUCCESS == rex_common_def_task((rex_tcb_type *)pWorkLoopObj->hThread,
            &attr))
   {
      return DAL_SUCCESS;
   }

thread_create_abort:
   DALSYS_LOG_ERROR_EVENT(
      "Failed to create thread- pWorkLoopObj:0x%x pszname:%s", 
      pWorkLoopObj, pszname);
   DALSYS_Free((void *)pWorkLoopObj->hThread);  
   return DAL_ERROR;
}

void DALSYS_WorkLoopThreadExit(_DALSYSWorkLoopObj * pWorkLoopObj)
{
   // kill myself
   rex_kill_task((rex_tcb_type *)pWorkLoopObj->hThread);
}

void DALSYS_WorkLoopThreadJoin(_DALSYSWorkLoopObj * pWorkLoopObj)
{
   if (pWorkLoopObj->hThread)
   {
      // kill workloop thread
      rex_kill_task((rex_tcb_type *)pWorkLoopObj->hThread);
      DALSYS_Free((void *)pWorkLoopObj->hThread);
      pWorkLoopObj->hThread = 0;
   }
}
