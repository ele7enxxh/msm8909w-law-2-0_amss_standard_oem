/** vi: tw=128 ts=3 sw=3 et
@file sys_m_listen_root.c
@brief This file contains the API for the System Monitor, API 2.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014-15 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/sys_m/src/sys_m_listen_root.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Change: 11985146 $
$Author: mplcsds1 $
===============================================================================*/

#if !defined(SYS_M_EXCLUDE_MSG_SWEVT)
#include "msg.h"
#endif

#if !defined(SYS_M_EXCLUDE_TRACER_SWEVT)
#include "tracer.h"
#include "sys_m_tracer_swe.h"
#endif

#include "rcecb.h"
#include "tms_utils.h"
#include "sys_m_messages.h"
#include "sys_m_internal.h"
#include "sys_m_sizes.h"
#include "sys_m_driver_internal.h"

/////////////////////////////////////////////////////////////////////
// Localized Type Declarations
/////////////////////////////////////////////////////////////////////

struct device_s
{
   qurt_qdi_obj_t qdiobj;                                                        /**< REQUIRED TO BE FIRST */

   SYS_M_DEVICE_PID pid;                                                         /**< Device In Use */

   qurt_sem_t sem_evt;                                                           /**< Event Ready */

   qurt_sem_t sem_ack;                                                           /**< Event Processed */

   char event[SYS_M_NAME_MAX];                                                   /**< Event Holding */

};

/////////////////////////////////////////////////////////////////////
// Localized Storage
/////////////////////////////////////////////////////////////////////

static struct internal_s
{

   struct device_s device[SYS_M_DEVICE_PID_MAX];

} sys_m_listen_internal;

/////////////////////////////////////////////////////////////////////
// Implementation Details
/////////////////////////////////////////////////////////////////////

static void SYS_M_LISTEN_RELEASE(qurt_qdi_obj_t *obj)
{
   struct device_s* device_p = (struct device_s*)obj;
   char task_name[QURT_MAX_NAME_LEN];

   qurt_thread_get_name(task_name, sizeof(task_name));

   if (NULL != device_p)
   {
#if !defined(SYS_M_EXCLUDE_MSG_SWEVT)
      MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "device %s by %s %d, %d", "release", task_name, device_p->pid, 0);
#endif

      device_p->pid = SYS_M_DEVICE_PID_NA; // not in use

      qurt_sem_destroy(&device_p->sem_evt);

      qurt_sem_destroy(&device_p->sem_ack);

      qurt_sem_init_val(&device_p->sem_evt, 0);

      qurt_sem_init_val(&device_p->sem_ack, 0);
   }
}

static int SYS_M_LISTEN_INVOCATION(int client_handle, qurt_qdi_obj_t* obj, int method, qurt_qdi_arg_t a1, qurt_qdi_arg_t a2, qurt_qdi_arg_t a3, qurt_qdi_arg_t a4, qurt_qdi_arg_t a5, qurt_qdi_arg_t a6, qurt_qdi_arg_t a7, qurt_qdi_arg_t a8, qurt_qdi_arg_t a9)
{
   int rc = SYS_M_DEVICE_STATUS_ERROR;
   struct device_s* device_p = (struct device_s*)obj;

   /* client_handle means that we're asking this particular client handle to answer the question about its process */
   /* QDI_OS_SYSENV is the standard method for requesting this type of information */
   /* QDI_HANDLE_LOCAL_CLIENT means that this request originated locally */
   /* QURT_SYSENV_PROCNAME_TYPE is the specific piece of data being requested to fill in a qurt_sysenv_procname_t structure */
   /* &pn is the structure we want filled in */

   qurt_sysenv_procname_t pn;                                                    // protection domain
   char task_name[QURT_MAX_NAME_LEN];                                            // task name

   qurt_thread_get_name(task_name, sizeof(task_name));

   if (0 > qurt_qdi_handle_invoke(client_handle, QDI_OS_SYSENV, QDI_HANDLE_LOCAL_CLIENT, QURT_SYSENV_PROCNAME_TYPE, &pn))
   {
#if !defined(SYS_M_EXCLUDE_MSG_SWEVT)
      MSG_SPRINTF_2(MSG_SSID_TMS, MSG_LEGACY_HIGH, "device invocation cannot obtain client asid %s %d", task_name, rc);
#endif
      return rc;
   }

   /* Driver Methods */

   switch (method)
   {
      /**
      INTERNAL, Default Method Handler
      */
      default:
      {
         rc = qurt_qdi_method_default(client_handle, obj, method, a1, a2, a3, a4, a5, a6, a7, a8, a9); // pass along

         break; // switch ()
      }

      /**
      INTERNAL, Device Check
      */
      case SYS_M_LISTEN_METHOD_PING:
      {
         if (NULL == device_p)
         {
            break;
         }

         rc = SYS_M_DEVICE_STATUS_SUCCESS;

         break; // switch ()
      }

      /**
      API, sys_m_listen_init
      */
      case QDI_OPEN:
      {
         device_p = &sys_m_listen_internal.device[pn.asid];

         if (pn.asid != device_p->pid)
         {
            device_p->qdiobj.invoke = SYS_M_LISTEN_INVOCATION;
            device_p->qdiobj.refcnt = QDI_REFCNT_INIT;
            device_p->qdiobj.release = SYS_M_LISTEN_RELEASE;

            device_p->pid = pn.asid; // in use
         }

         rc = qurt_qdi_handle_create_from_obj_t(client_handle, (qurt_qdi_obj_t*)device_p);

#if !defined(SYS_M_EXCLUDE_MSG_SWEVT)
         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "device %s by %s %d, %d", "open", task_name, device_p->pid, rc);
#endif

         break; // switch ()
      }

      /**
      API, sys_m_listen_destroy
      */
      case QDI_CLOSE:
      {
         if (NULL == device_p)
         {
            break;
         }

         if (0 <= qurt_qdi_method_default(client_handle, obj, method, a1, a2, a3, a4, a5, a6, a7, a8, a9))
         {
            rc = SYS_M_DEVICE_STATUS_SUCCESS;
         }

#if !defined(SYS_M_EXCLUDE_MSG_SWEVT)
         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "device %s by %s %d, %d", "close", task_name, device_p->pid, rc);
#endif

         break; // switch ()
      }

      /**
      API, sys_m_listen_read
      */
      case SYS_M_LISTEN_METHOD_READ:
      {
         if (NULL == device_p)
         {
            break;
         }

#if !defined(SYS_M_EXCLUDE_MSG_SWEVT)
         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "device %s by %s %d, %d", "method read beg wait", task_name, device_p->pid, 0);
#endif

         qurt_qdi_safe_to_kill();                                                   // honor and react to thread kills around potential blocking

         qurt_sem_down(&device_p->sem_evt);

         qurt_qdi_unsafe_to_kill();                                                 // honor and react to thread kills around potential blocking

#if !defined(SYS_M_EXCLUDE_MSG_SWEVT)
         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "device %s by %s %d, %d", "method read end wait", task_name, device_p->pid, 0);
#endif

         if (NULL != a1.ptr &&
             sizeof(device_p->event) >= a2.num &&
             0 <= qurt_qdi_copy_to_user(client_handle, a1.ptr, device_p->event, a2.num))
         {
            rc = SYS_M_DEVICE_STATUS_SUCCESS;

#if !defined(SYS_M_EXCLUDE_MSG_SWEVT)
            MSG_SPRINTF_5(MSG_SSID_TMS, MSG_LEGACY_HIGH, "device %s by %s %d, %d %s", "method read", task_name, device_p->pid, rc, device_p->event);
#endif
         }

         else
         {

#if !defined(SYS_M_EXCLUDE_MSG_SWEVT)
            MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "device %s by %s %d, %d", "method read", task_name, device_p->pid, rc);
#endif
         }

         break; // switch ()
      }

      /**
      API, sys_m_listen_ack
      */
      case SYS_M_LISTEN_METHOD_ACK:
      {
         if (NULL == device_p)
         {
            break;
         }

         qurt_sem_up(&device_p->sem_ack);

         rc = SYS_M_DEVICE_STATUS_SUCCESS;

#if !defined(SYS_M_EXCLUDE_MSG_SWEVT)
         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "device %s by %s %d, %d", "method ack", task_name, device_p->pid, rc);
#endif

         break; // switch ()
      }
   }

   return rc;
}

static void sys_m_listen_write(SYS_M_NAME event)
{
   SYS_M_DEVICE_PID pid;

   for (pid = SYS_M_DEVICE_PID_0; pid < SYS_M_DEVICE_PID_MAX; pid++)
   {
      struct device_s* device_p = &sys_m_listen_internal.device[pid];

      if (SYS_M_DEVICE_PID_NA != device_p->pid &&
          SYS_M_NULL != event &&
          sizeof(device_p->event) > strnlen(event, sizeof(device_p->event)) &&
          sizeof(device_p->event) > strlcpy(device_p->event, event, sizeof(device_p->event)))
      {
         qurt_sem_up(&device_p->sem_evt);
      }
   }

   for (pid = SYS_M_DEVICE_PID_0; pid < SYS_M_DEVICE_PID_MAX; pid++)
   {
      struct device_s* device_p = &sys_m_listen_internal.device[pid];

      if (SYS_M_DEVICE_PID_NA != device_p->pid)
      {
#if !defined(SYS_M_EXCLUDE_MSG_SWEVT)
         MSG_SPRINTF_3(MSG_SSID_TMS, MSG_LEGACY_HIGH, "event %s to %d", "write beg wait", event, pid);
#endif

         qurt_sem_down(&device_p->sem_ack);

#if !defined(SYS_M_EXCLUDE_MSG_SWEVT)
         MSG_SPRINTF_3(MSG_SSID_TMS, MSG_LEGACY_HIGH, "event %s to %d", "write end wait", event, pid);
#endif

      }
   }
}

// forwarding callbacks

static void forward_sys_m_ssr_wcnss_after_powerup(void)
{
   sys_m_listen_write(SYS_M_SSR_WCNSS_AFTER_POWERUP);
}

static void forward_sys_m_ssr_wcnss_before_shutdown(void)
{
   sys_m_listen_write(SYS_M_SSR_WCNSS_BEFORE_SHUTDOWN);
}

static void forward_sys_m_ssr_modem_after_powerup(void)
{
   sys_m_listen_write(SYS_M_SSR_MODEM_AFTER_POWERUP);
}

static void forward_sys_m_ssr_modem_after_shutdown(void)
{
   sys_m_listen_write(SYS_M_SSR_MODEM_AFTER_SHUTDOWN);
}

static void forward_register(void)
{
   if (rcecb_register_name(SYS_M_SSR_WCNSS_AFTER_POWERUP, forward_sys_m_ssr_wcnss_after_powerup)) {;}

   if (rcecb_register_name(SYS_M_SSR_WCNSS_BEFORE_SHUTDOWN, forward_sys_m_ssr_wcnss_before_shutdown)) {;}

   if (rcecb_register_name(SYS_M_SSR_MODEM_AFTER_POWERUP, forward_sys_m_ssr_modem_after_powerup)) {;}

   if (rcecb_register_name(SYS_M_SSR_MODEM_AFTER_SHUTDOWN, forward_sys_m_ssr_modem_after_shutdown)) {;}
}

// Driver Initialization

void sys_m_listen_root_init(void)
{
   SYS_M_DEVICE_PID pid;

   static struct
   {
      qurt_qdi_obj_t qdiobj;

   } opener = { { SYS_M_LISTEN_INVOCATION, QDI_REFCNT_PERM, 0 } };

   secure_memset(sys_m_listen_internal.device, 0, sizeof(sys_m_listen_internal.device));

   for (pid = SYS_M_DEVICE_PID_0; pid < SYS_M_DEVICE_PID_MAX; pid++)
   {
      sys_m_listen_internal.device[pid].pid = SYS_M_DEVICE_PID_NA;

      qurt_sem_init_val(&sys_m_listen_internal.device[pid].sem_evt, 0);

      qurt_sem_init_val(&sys_m_listen_internal.device[pid].sem_ack, 0);
   }

   forward_register();

   qurt_qdi_devname_register(SYS_M_LISTEN_DEVICE_PATH, &opener.qdiobj);
}

void sys_m_listen_root_term(void)
{
   /* NULL */ /* DECISION TO NOT CLEANUP SERVICE FOR POST MORTEM REASONS */
}
