/**
@file pd_qurt_pimutex.c
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
$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/pd_mon/src/pd_qurt_pimutex.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Change: 11985146 $
$Author: mplcsds1 $
===============================================================================*/

#include "pd_qurt.h"

/////////////////////////////////////////////////////////////////////
// Management of an QURT PIMUTEX within HOST Protection Domain
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Localized Type Declarations
/////////////////////////////////////////////////////////////////////

struct pd_qurt_pimutex_device_s
{
   qurt_qdi_obj_t qdiobj; // required to be first

   char name[PD_QURT_DEVICE_NAME_LEN];

   int open_count[PD_QURT_DEVICE_ASID_MAX];

   qurt_mutex_t pimutex; // managed pimutex

   struct pd_qurt_pimutex_device_s* next;
};

/////////////////////////////////////////////////////////////////////
// Localized Storage
/////////////////////////////////////////////////////////////////////

static struct pd_qurt_pimutex_internal_s
{
   qurt_mutex_t mutex;

   struct pd_qurt_pimutex_device_s* list;

} pd_qurt_pimutex_internal;

/////////////////////////////////////////////////////////////////////
// Implementation Details
/////////////////////////////////////////////////////////////////////

static void pd_qurt_pimutex_device_release(qurt_qdi_obj_t *qdiobj)
{
   qurt_pimutex_destroy(&((struct pd_qurt_pimutex_device_s*)qdiobj)->pimutex);

   free(qdiobj);
}

static int pd_qurt_pimutex_device_invocation(int client_handle, qurt_qdi_obj_t* obj, int method, qurt_qdi_arg_t a1, qurt_qdi_arg_t a2, qurt_qdi_arg_t a3, qurt_qdi_arg_t a4, qurt_qdi_arg_t a5, qurt_qdi_arg_t a6, qurt_qdi_arg_t a7, qurt_qdi_arg_t a8, qurt_qdi_arg_t a9)
{
   int rc = -1; // failure
   struct pd_qurt_pimutex_device_s* device_p = (struct pd_qurt_pimutex_device_s*)obj;
   qurt_sysenv_procname_t pn;

   /* client_handle means that we're asking this particular client handle to answer the question about its process */
   /* QDI_OS_SYSENV is the standard method for requesting this type of information */
   /* QDI_HANDLE_LOCAL_CLIENT means that this request originated locally */
   /* QURT_SYSENV_PROCNAME_TYPE is the specific piece of data being requested -- to fill in a qurt_sysenv_procname_t structure */
   /* &pn is the structure we want filled in */

   if (0 > (rc = qurt_qdi_handle_invoke(client_handle, QDI_OS_SYSENV, QDI_HANDLE_LOCAL_CLIENT, QURT_SYSENV_PROCNAME_TYPE, &pn)) || PD_QURT_DEVICE_ASID_MAX >= pn.asid)
   {
      MSG_SPRINTF_2(MSG_SSID_TMS, MSG_LEGACY_HIGH, "cannot obtain client asid %d %x", pn.asid, rc);

      return rc;
   }

   switch (method)
   {
      /**
      INTERNAL, Default Method Handler
      */
      default:
      {
         rc = qurt_qdi_method_default(client_handle, obj, method, a1, a2, a3, a4, a5, a6, a7, a8, a9); // pass along

         break;
      }

      /**
      API, pd_qurt_pimutex_init
      @param[in] a1.ptr/const char* NULL Terminated Device Name
      @return
      PD_QURT_HANDLE_PIMUTEX -- Opaque Device Handle
      */
      case QDI_OPEN:
      {
         if (NULL != a1.ptr && PD_QURT_DEVICE_NAME_LEN > strnlen(a1.ptr, PD_QURT_DEVICE_NAME_LEN)) // name length check
         {
            qurt_pimutex_lock(&pd_qurt_pimutex_internal.mutex); // serialize driver access

            // locate existing named device

            for (device_p = pd_qurt_pimutex_internal.list; NULL != device_p; device_p = device_p->next)
            {
               if (0 == strncmp(device_p->name, a1.ptr, sizeof(device_p->name)))
               {
                  break;
               }
            }

            // operate on new

            if (NULL == device_p)
            {
               // allocate new

               if (NULL == (device_p = malloc(sizeof(*device_p))))
               {
                  // allocation fail

                  qurt_pimutex_unlock(&pd_qurt_pimutex_internal.mutex); // serialize driver access

                  break;
               }

               // initialize new

               secure_memset(device_p, 0, sizeof(*device_p));

               device_p->qdiobj.invoke = pd_qurt_pimutex_device_invocation;
               device_p->qdiobj.refcnt = QDI_REFCNT_PERM;
               device_p->qdiobj.release = pd_qurt_pimutex_device_release;

               memsmove(device_p->name, sizeof(device_p->name), a1.ptr, strnlen(a1.ptr, PD_QURT_DEVICE_NAME_LEN));

               qurt_pimutex_init(&device_p->pimutex);

               // link new into device list

               device_p->next = pd_qurt_pimutex_internal.list;
               pd_qurt_pimutex_internal.list = device_p;
            }

            // operate on existing

            if (PD_QURT_DEVICE_ASID_MAX > pn.asid)
            {
               device_p->open_count[pn.asid]++;
            }

            qurt_pimutex_unlock(&pd_qurt_pimutex_internal.mutex); // serialize driver access

            rc = qurt_qdi_handle_create_from_obj_t(client_handle, (qurt_qdi_obj_t*)device_p); // return handle
         }

         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "method open %d %s %s %x", pn.asid, pn.name, device_p->name, rc);

         break;
      }

      /**
      API, pd_qurt_pimutex_destroy
      @param[in] PD_QURT_HANDLE_PIMUTEX Opaque Device Handle
      @return
      PD_QURT_STATUS -- Error Indication
      */
      case QDI_CLOSE:
      {
         qurt_pimutex_lock(&pd_qurt_pimutex_internal.mutex); // serialize driver access

         // operate on existing

         if (PD_QURT_DEVICE_ASID_MAX > pn.asid)
         {
            device_p->open_count[pn.asid]--;
         }

         qurt_pimutex_unlock(&pd_qurt_pimutex_internal.mutex); // serialize driver access

         rc = qurt_qdi_method_default(client_handle, obj, method, a1, a2, a3, a4, a5, a6, a7, a8, a9); // pass along

         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "method close %d %s %s %x", pn.asid, pn.name, device_p->name, rc);

         break;
      }

      /**
      API, pd_qurt_pimutex_lock
      @param[in] PD_QURT_HANDLE_PIMUTEX Opaque Device Handle
      @return
      PD_QURT_STATUS -- Error Indication
      */
      case PD_QURT_METHOD_PIMUTEX_LOCK:
      {
         qurt_qdi_safe_to_kill(); // honor and react to thread kills around potential blocking

         if (qurt_qdi_marked_to_kill())
         {
            qurt_qdi_unsafe_to_kill();

            qurt_thread_stop();
         }

         qurt_qdi_unsafe_to_kill(); // honor and react to thread kills around potential blocking

         MSG_SPRINTF_3(MSG_SSID_TMS, MSG_LEGACY_HIGH, "method lock block %d %s %s", pn.asid, pn.name, device_p->name);

         qurt_pimutex_lock(&device_p->pimutex);

         rc = 0; // success

         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "method lock %d %s %s %x", pn.asid, pn.name, device_p->name, rc);

         break;
      }

      /**
      API, pd_qurt_pimutex_unlock
      @param[in] PD_QURT_HANDLE_PIMUTEX Opaque Device Handle
      @return
      PD_QURT_STATUS -- Error Indication
      */
      case PD_QURT_METHOD_PIMUTEX_UNLOCK:
      {
         qurt_pimutex_unlock(&device_p->pimutex);

         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "method unlock %d %s %s %x", pn.asid, pn.name, device_p->name, rc);

         rc = 0; // success

         qurt_qdi_safe_to_kill(); // honor and react to thread kills around potential blocking

         if (qurt_qdi_marked_to_kill())
         {
            qurt_qdi_unsafe_to_kill();

            qurt_thread_stop();
         }

         qurt_qdi_unsafe_to_kill();

         break;
      }

      /**
      API, pd_qurt_pimutex_try_lock
      @param[in] PD_QURT_HANDLE_PIMUTEX Opaque Device Handle
      @param[in] int* Operation Result
      @return
      PD_QURT_STATUS -- Error Indication
      */
      case PD_QURT_METHOD_PIMUTEX_TRY_LOCK:
      {
         int result;

         qurt_qdi_safe_to_kill(); // honor and react to thread kills around potential blocking

         if (qurt_qdi_marked_to_kill())
         {
            qurt_qdi_unsafe_to_kill();

            qurt_thread_stop();
         }

         qurt_qdi_unsafe_to_kill();

         result = qurt_pimutex_try_lock(&device_p->pimutex);

         if (NULL == a1.ptr)
         {
            if (0 == result)
            {
               rc = 0; // success
            }
         }

         else
         {
            rc = qurt_qdi_copy_to_user(client_handle, a1.ptr, &result, sizeof(result)); // copy to user
         }

         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "method try lock %d %s %s %x", pn.asid, pn.name, device_p->name, rc);

         break;
      }
   }

   return rc;
}

/**
INTERNAL, QDI Opener Invocation Object
*/
static struct
{
   qurt_qdi_obj_t qdiobj;

} opener = { { pd_qurt_pimutex_device_invocation, QDI_REFCNT_PERM, 0 } };

/**
API, Initialization of service prior to use
@return
None.
*/
void pd_qurt_pimutex_driver_init(void)
{
   qurt_pimutex_init(&pd_qurt_pimutex_internal.mutex); // serialize driver access

   qurt_pimutex_lock(&pd_qurt_pimutex_internal.mutex); // serialize driver access

   qurt_qdi_devname_register(PD_QURT_DEVICE_PIMUTEX, &opener.qdiobj); // register driver

   qurt_pimutex_unlock(&pd_qurt_pimutex_internal.mutex); // serialize driver access

   // API Check

   {
      PD_QURT_HANDLE_PIMUTEX handle;

      if (PD_QURT_STATUS_SUCCESS <= (handle = pd_qurt_pimutex_init("PD_QURT")) &&
          PD_QURT_STATUS_SUCCESS == pd_qurt_pimutex_lock(handle) &&
          PD_QURT_STATUS_SUCCESS == pd_qurt_pimutex_unlock(handle) &&
          PD_QURT_STATUS_SUCCESS == pd_qurt_pimutex_destroy(handle))
      {
         MSG_SPRINTF_1(MSG_SSID_TMS, MSG_LEGACY_HIGH, "test success %x", 0);
      }

      else
      {
         MSG_SPRINTF_1(MSG_SSID_TMS, MSG_LEGACY_HIGH, "test failure %x", 0);
      }
   }
}

/**
Termination of service following use
@return
None.
*/
void pd_qurt_pimutex_driver_term(void)
{
   /* NULL */ /* DECISION TO NOT CLEANUP SERVICE FOR POST MORTEM REASONS */
}
