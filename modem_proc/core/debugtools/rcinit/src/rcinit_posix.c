/** vi: tw=128 ts=3 sw=3 et :
@file rcinit_posix.c
@brief This file contains the API for the Run Control Framework, API 3.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/rcinit/src/rcinit_posix.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Change: 11985146 $
$Author: mplcsds1 $
===============================================================================*/

#if !defined(RCINIT_EXCLUDE_KERNEL_POSIX)

#define DLL_API_BUILD_RCINIT

#include "err.h"
#include "stringl/stringl.h"

#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
#include "tracer.h"
#include "rcinit_tracer_swe.h"
#endif

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
#include "msg.h"
#endif

#include "rcinit_posix.h"
#include "tms_rcinit.h"

pthread_t rcinit_lookup_posix_info(RCINIT_INFO info)
{
   rcinit_info_t* rcinit_p = (rcinit_info_t*)info;

   if (RCINIT_NULL != rcinit_p && RCINIT_NULL != rcinit_p->handle && RCINIT_TASK_POSIX == rcinit_p->type)
   {
      return(pthread_t)(rcinit_p->handle->tid_p);
   }

   return 0;
}

pthread_t rcinit_lookup_posix(RCINIT_NAME name)
{
   RCINIT_INFO info = rcinit_lookup(name);

   if (RCINIT_NULL != info)
   {
      return rcinit_lookup_posix_info(info);
   }

   return 0;
}

static void* TASK_ENTRY(void* argv)
{
   void* rc = NULL;
   const rcinit_info_t* rcinit_p = (const rcinit_info_t*)argv;

   if (RCINIT_NULL != rcinit_p && RCINIT_NULL != rcinit_p->handle)
   {
      void* (*entry)(void*) = (void* (*)(void*))rcinit_p->handle->entry;

      rcinit_internal_tls_set_specific(rcinit_internal.tls_key, (void*)rcinit_p);

      if (RCINIT_NULL != entry && RCINIT_ENTRY_NONE != entry)
      {
         unsigned long restart = 0;
         int policy;
         struct sched_param sched;

         pthread_getschedparam((pthread_t)(rcinit_p->handle->tid_p), &policy, &sched);
         sched.sched_priority = RCINIT_MAP_PRIO_POSIX(rcinit_p->prio);
         pthread_setschedparam((pthread_t)(rcinit_p->handle->tid_p), policy, &sched);

         rcinit_internal_hs_list_add(rcinit_internal.hs_init, rcinit_internal.group_curr, rcinit_p->name);

#if !defined(RCINIT_EXCLUDE_RCXH_HANDLER)

         do
         {
            RCXH_TRY
            {
#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
               tracer_event_simple_vargs(RCINIT_SWE_INIT_TASK_RN, 2, rcinit_internal.group_curr, rcinit_p->hash);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
               MSG_SPRINTF_3(MSG_SSID_TMS, MSG_LEGACY_HIGH, "task begins group %x task_hash %x task_name %s", rcinit_internal.group_curr, rcinit_p->hash, rcinit_p->name);
#endif
               rc = entry((void*)restart);

               restart = 0;
            }

            RCXH_CATCH(RCXH_RCINIT_TASK_RESTART)
            {
#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
               tracer_event_simple_vargs(RCINIT_SWE_INIT_TASK_RS, 2, rcinit_internal.group_curr, rcinit_p->hash);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
               MSG_SPRINTF_3(MSG_SSID_TMS, MSG_LEGACY_HIGH, "task exception restart group %x task_hash %x task_name %s", rcinit_internal.group_curr, rcinit_p->hash, rcinit_p->name);
#endif
               restart++;
            }

            RCXH_CATCH(RCXH_RCINIT_TASK_END)
            {
#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
               tracer_event_simple_vargs(RCINIT_SWE_INIT_TASK_E, 2, rcinit_internal.group_curr, rcinit_p->hash);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
               MSG_SPRINTF_3(MSG_SSID_TMS, MSG_LEGACY_HIGH, "task exception end group %x task_hash %x task_name %s", rcinit_internal.group_curr, rcinit_p->hash, rcinit_p->name);
#endif
               restart = 0;
            }

            RCXH_ENDTRY;

         } while (0 < restart);

#else

         rc = entry((void*)restart);

#endif

#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
         tracer_event_simple_vargs(RCINIT_SWE_INIT_TASK_XT, 2, rcinit_internal.group_curr, rcinit_p->hash);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
         MSG_SPRINTF_3(MSG_SSID_TMS, MSG_LEGACY_HIGH, "task exit group %x task_hash %x task_name %s", rcinit_internal.group_curr, rcinit_p->hash, rcinit_p->name);
#endif
      }
   }

   else
   {
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
      MSG_SPRINTF_3(MSG_SSID_TMS, MSG_LEGACY_HIGH, "task exit group %x no context", rcinit_internal.group_curr, 0, 0);
#endif
   }

   return rc;
}

void rcinit_internal_start_posix(const rcinit_info_t* rcinit_p, void* entry)
{
   if (RCINIT_NULL != rcinit_p && RCINIT_NULL != rcinit_p->handle)
   {
      pthread_attr_t attr;
      cpu_set_t cpuset;

      if (RCINIT_NULL != rcinit_p->handle->stack)
      {
         secure_memset(rcinit_p->handle->stack, RCINIT_STACK_INIT, rcinit_p->stksz);
      }

      pthread_attr_init(&attr);
      pthread_attr_setthreadname(&attr, (void*)rcinit_p->name);
      pthread_attr_setstacksize(&attr, rcinit_p->stksz);
      pthread_attr_setstackaddr(&attr, rcinit_p->handle->stack);

      cpuset = rcinit_p->cpu_affinity;
      pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);

      rcinit_p->handle->entry = (rcinit_entry_p)entry;

      pthread_create((pthread_t*)(&(rcinit_p->handle->tid_p)), &attr, TASK_ENTRY, (void*)rcinit_p);

      pthread_attr_destroy(&attr);
   }
}

#else

typedef unsigned int pthread_t;

#include "tms_rcinit.h"

pthread_t rcinit_lookup_posix_info(RCINIT_INFO info)
{
   return RCINIT_ZERO;
}

pthread_t rcinit_lookup_posix(RCINIT_NAME name)
{
   return RCINIT_ZERO;
}

void rcinit_internal_start_posix(const rcinit_info_t* rcinit_p, void* entry)
{
}

#endif
