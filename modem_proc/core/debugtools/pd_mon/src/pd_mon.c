/**
@file pd_mon_host.h
@brief This file contains the API details for the Protection Domain Monitor
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/pd_mon/src/pd_mon.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Change: 11985146 $
$Author: mplcsds1 $
===============================================================================*/

#include "pd_mon_qurt.h"
#include "pd_mon_internal.h"

/////////////////////////////////////////////////////////////////////
// Localized Type Declarations
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Localized Storage
/////////////////////////////////////////////////////////////////////

/**
INTERNAL, Program Domain Monitor (One Context Created Per Program Domain)
*/
static void PROGRAM_MONITOR(void* argv)
{
   PD_MON_STATUS_P stat_p = NULL;
   int i, cid, wid, status;

   pd_mon_internal.args_handle = PD_MON_ERROR;

   // Look for an unused status entry

   for (i = 0; i < sizeof(pd_mon_internal.status)/sizeof(PD_MON_STATUS_T); i++)
   {
      if (pd_mon_internal.status[i].cid == PD_MON_STATUS_NO_CID)
      {
         break;
      }
   }

   // Check if an unused status entry was located

   if (sizeof(pd_mon_internal.status)/sizeof(PD_MON_STATUS_T) > i)
   {
      stat_p = &pd_mon_internal.status[i];                   // Pointer to Status Array (Turns into Opaque Handle)

      stat_p->restart = pd_mon_internal.args_restart;        // Initalize Current Status Array Structure

      stat_p->wid = wid = PD_MON_STATUS_NO_CID;                      // Child Id Return from Last Wait

      stat_p->status = status = 0;                                   // Status Return from Last Wait

      strlcpy(stat_p->name, argv, sizeof(stat_p->name));             // Copy PD Name For Reference

      // MSG_HIGH(SPAWNS A PROGRAM)

      pd_mon_internal.args_handle = stat_p;                  // Return Value back to Callee

      stat_p->cid = cid = qurt_spawn(argv);                          // First Time Spawn of the Program

      stat_p->restart--;                                             // Restart Counter Decrements

      // NOTE: THIS SIGNAL IS UNDER MUTEX PROTECTION. THE PROGRAM MONITOR
      // CONTEXT MUST RESPOND THAT IT HAS COMPLETED ITS UPDATE TO THE
      // MUTEX PROTECTED INFORMATION. THE CREATION AND INITIALIZATION
      // OF THE PROGRAM MONITOR CONTEXT IS CONSIDERED PROTECTED AS WELL.

      qurt_anysignal_set(&pd_mon_internal.signal, PD_MON_SIG_UNLOCK); // Signal Callee to Pick Up Return Value and Unlock Mutex

      // NOW THE PROGRAM MONITOR CONTEXT WILL HAVE TO USE THE MUTEX
      // DIRECTLY BEFORE ANY CHANGES ARE MADE TO THE MUTEX PROTECTED
      // DATA.

      do                                                             // Loop for Restart
      {
         if (0 > cid)                                                // Wait for Child Change in Program Execution
         {
            ERR("PD_MON FAILURE NO SPAWN", 0, 0, 0);                 // Log Failure of PD to Start (NEVER HOST FATAL)

            break;
         }

         // MSG_HIGH(WAITS FOR PROGRAM)

         wid = qurt_wait(&status);                                   // Wait for Child to Status Change (Allows Lower Priority Activity)

         qurt_pimutex_lock(&pd_mon_internal.mutex);          // Callee Mutex Lock

         if (0 < stat_p->restart)
         {
            stat_p->wid = wid;                                       // Wait Id Return

            stat_p->status = status;                                 // Status Return

            // MSG_HIGH(SPAWNS A PROGRAM)

            cid = stat_p->cid = qurt_spawn(argv);                    // Child Id Return

            stat_p->restart--;                                       // Restart Counter Decrements
         }

         qurt_pimutex_unlock(&pd_mon_internal.mutex);        // Callee Mutex Lock

      } while (1);                                                   // Keep Restarting Until Not Possible

      qurt_pimutex_lock(&pd_mon_internal.mutex);             // Callee Mutex Lock

      strlcpy(stat_p->name, "", sizeof(stat_p->name));               // PD Name is NULL

      stat_p->cid = PD_MON_STATUS_NO_CID;

      qurt_pimutex_unlock(&pd_mon_internal.mutex);           // Callee Mutex Unlock
   }

   else
   {
      qurt_anysignal_set(&pd_mon_internal.signal, PD_MON_SIG_UNLOCK); // Signal Callee to Pick Up Return Value and Unlock Mutex

      ERR("PD_MON FAILURE NO SPACE", 0, 0, 0);                       // Log Failure of PD to Start (NEVER HOST FATAL)
   }

}

/**
API, Spawn Program Domain under Program Domain Monitor
*/
PD_MON_HANDLE pd_mon_spawn(PD_MON_RESTART* restart_p, PD_MON_NAME const program, ...)
{
   int i;
   PD_MON_HANDLE rc;
   va_list ap;

   va_start(ap, program);                                            // Variable Arguments Setup

   qurt_pimutex_lock(&pd_mon_internal.mutex);                // Callee Mutex Lock

   pd_mon_internal.args_handle = PD_MON_ERROR;               // Assume Failure Return Result

   // Look for an unused status entry

   for (i = 0; i < sizeof(pd_mon_internal.status)/sizeof(PD_MON_STATUS_T); i++)
   {
      if (pd_mon_internal.status[i].cid == PD_MON_STATUS_NO_CID)
      {
         break;
      }
   }

   // Check if an unused status entry was located

   if (sizeof(pd_mon_internal.status)/sizeof(PD_MON_STATUS_T) > i)
   {
      char name[PD_MON_NAME_MAX];
      unsigned char* stack;
      qurt_thread_t tid;
      qurt_thread_attr_t attr;

      // Check if we can obtain stack resource for the Program Monitor Context

      if (NULL != (stack = malloc(PD_MON_STACK_SIZ)))                // Attempt to allocate stack for the Program Monitor Context
      {
         strlcpy(name, "%", sizeof(name));                           // Name the Program Monitor Context to reflect "%Program"
         strlcat(name, program, sizeof(name));

         qurt_thread_attr_init(&attr);                               // Init the Program Monitor Context

         qurt_thread_attr_set_name(&attr, name);
         qurt_thread_attr_set_stack_addr(&attr, stack);
         qurt_thread_attr_set_stack_size(&attr, PD_MON_STACK_SIZ);
         qurt_thread_attr_set_priority(&attr, qurt_thread_get_priority(qurt_thread_get_id()) - 1);
         qurt_thread_attr_set_affinity(&attr, QURT_THREAD_ATTR_AFFINITY_DEFAULT);

         if (NULL != restart_p)
         {
            pd_mon_internal.args_restart = *restart_p;       // Passing Restart Argument
         }

         else
         {
            pd_mon_internal.args_restart = PD_MON_RESTART_ALWAYS; // Setting Default
         }

         qurt_anysignal_clear(&pd_mon_internal.signal, PD_MON_SIG_UNLOCK);

         qurt_thread_create(&tid, &attr, PROGRAM_MONITOR, (void*)program); // Create Program Monitor Context

         // NOTE: THIS WAIT IS UNDER MUTEX PROTECTION. THE PROGRAM MONITOR
         // CONTEXT MUST RESPOND THAT IT HAS COMPLETED ITS UPDATE TO THE
         // MUTEX PROTECTED INFORMATION. THE CREATION AND INITIALIZATION
         // OF THE PROGRAM MONITOR CONTEXT IS CONSIDERED PROTECTED AS WELL.

         qurt_anysignal_wait(&pd_mon_internal.signal, PD_MON_SIG_UNLOCK); // Wait for the Program Monitor Context to Signal it has a Return Value
      }
   }

   rc = pd_mon_internal.args_handle;                         // Return Value from Program Monitor Context

   qurt_pimutex_unlock(&pd_mon_internal.mutex);              // Callee Mutex Unlock

   va_end(ap);                                                       // Variable Arguments End

   return rc;
}

/**
API, Get Restart Counter of PD by Opaque Handle
*/
PD_MON_HANDLE pd_mon_restart_get(PD_MON_HANDLE handle, PD_MON_RESTART* restart_p)
{
   if (NULL != handle && PD_MON_ERROR != handle && NULL != restart_p)
   {
      qurt_pimutex_lock(&pd_mon_internal.mutex);             // Callee Mutex Lock

      *restart_p = ((PD_MON_STATUS_P)handle)->restart;

      qurt_pimutex_unlock(&pd_mon_internal.mutex);           // Callee Mutex Unlock
   }

   return handle;
}

/**
API, Set Restart Count of PD by Opaque Handle
*/
PD_MON_HANDLE pd_mon_restart_set(PD_MON_HANDLE handle, PD_MON_RESTART* restart_p)
{
   PD_MON_RESTART tmp;

   if (NULL != handle && PD_MON_ERROR != handle && NULL != restart_p)
   {
      qurt_pimutex_lock(&pd_mon_internal.mutex);             // Callee Mutex Lock

      // MSG_HIGH(API CHANGES VALUE)

      tmp = ((PD_MON_STATUS_P)handle)->restart;
      ((PD_MON_STATUS_P)handle)->restart = *restart_p;
      *restart_p = tmp;

      qurt_pimutex_unlock(&pd_mon_internal.mutex);           // Callee Mutex Unlock
   }

   return handle;
}

/**
API, Initialization of service prior to use
@return
None.
*/
void pd_mon_host_init(void)
{
   int i;

   secure_memset(&pd_mon_internal, 0, sizeof(pd_mon_internal)); // Initialize Local Storage

   qurt_pimutex_init(&pd_mon_internal.mutex);                // Initalize Mutex

   qurt_anysignal_init(&pd_mon_internal.signal);             // Initalize Signal

   for (i = 0; i < sizeof(pd_mon_internal.status)/sizeof(PD_MON_STATUS_T); i++)
   {
      pd_mon_internal.status[i].cid = PD_MON_STATUS_NO_CID;
   }
}

/**
Termination of service following use
@return
None.
*/
void pd_mon_host_term(void)
{
   /* NULL */ /* DECISION TO NOT CLEANUP SERVICE FOR POST MORTEM REASONS */
}
