/**
@file pd_qurt_anysignal.c
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
$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/pd_mon/src/pd_qurt_anysignal.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Change: 11985146 $
$Author: mplcsds1 $
===============================================================================*/

#include "pd_qurt.h"

/////////////////////////////////////////////////////////////////////
// Management of an QURT ANYSIGNAL within HOST Protection Domain
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Localized Type Declarations
/////////////////////////////////////////////////////////////////////

struct pd_qurt_anysignal_device_s
{
   qurt_qdi_obj_t qdiobj; // required to be first

   char name[PD_QURT_DEVICE_NAME_LEN];

   int open_count[PD_QURT_DEVICE_ASID_MAX];

   qurt_anysignal_t anysignal; // managed anysignal

   struct pd_qurt_anysignal_device_s* next;
};

/////////////////////////////////////////////////////////////////////
// Localized Storage
/////////////////////////////////////////////////////////////////////

static struct pd_qurt_anysignal_internal_s
{
   qurt_mutex_t mutex;

   struct pd_qurt_anysignal_device_s* list;

} pd_qurt_anysignal_internal;

/////////////////////////////////////////////////////////////////////
// Implementation Details
/////////////////////////////////////////////////////////////////////

static void pd_qurt_anysignal_device_release(qurt_qdi_obj_t *qdiobj)
{
   qurt_anysignal_destroy(&((struct pd_qurt_anysignal_device_s*)qdiobj)->anysignal);

   free(qdiobj);
}

static int pd_qurt_anysignal_device_invocation(int client_handle, qurt_qdi_obj_t* obj, int method, qurt_qdi_arg_t a1, qurt_qdi_arg_t a2, qurt_qdi_arg_t a3, qurt_qdi_arg_t a4, qurt_qdi_arg_t a5, qurt_qdi_arg_t a6, qurt_qdi_arg_t a7, qurt_qdi_arg_t a8, qurt_qdi_arg_t a9)
{
   int rc = -1; // failure
   struct pd_qurt_anysignal_device_s* device_p = (struct pd_qurt_anysignal_device_s*)obj;
   qurt_sysenv_procname_t pn;

   /* client_handle means that we're asking this particular client handle to answer the question about its process */
   /* QDI_OS_SYSENV is the standard method for requesting this type of information */
   /* QDI_HANDLE_LOCAL_CLIENT means that this request originated locally */
   /* QURT_SYSENV_PROCNAME_TYPE is the specific piece of data being requested -- to fill in a qurt_sysenv_procname_t structure */
   /* &pn is the structure we want filled in */

   if (0 > (rc = qurt_qdi_handle_invoke(client_handle, QDI_OS_SYSENV, QDI_HANDLE_LOCAL_CLIENT, QURT_SYSENV_PROCNAME_TYPE, &pn)) || PD_QURT_DEVICE_ASID_MAX >= pn.asid)
   {
      MSG_SPRINTF_2(MSG_SSID_TMS, MSG_LEGACY_HIGH, "cannot obtain client asid %d %d", pn.asid, rc);

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
      API, pd_qurt_anysignal_init
      @param[in] a1.ptr/const char* NULL Terminated Device Name
      @return
      PD_QURT_HANDLE_ANYSIGNAL -- Opaque Device Handle
      */
      case QDI_OPEN:
      {
         if (NULL != a1.ptr && PD_QURT_DEVICE_NAME_LEN > strnlen(a1.ptr, PD_QURT_DEVICE_NAME_LEN)) // name length check
         {
            qurt_pimutex_lock(&pd_qurt_anysignal_internal.mutex); // serialize driver access

            // locate existing named device

            for (device_p = pd_qurt_anysignal_internal.list; NULL != device_p; device_p = device_p->next)
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

                  qurt_pimutex_unlock(&pd_qurt_anysignal_internal.mutex); // serialize driver access

                  break;
               }

               // initialize new

               secure_memset(device_p, 0, sizeof(*device_p));

               device_p->qdiobj.invoke = pd_qurt_anysignal_device_invocation;
               device_p->qdiobj.refcnt = QDI_REFCNT_PERM;
               device_p->qdiobj.release = pd_qurt_anysignal_device_release;

               memsmove(device_p->name, sizeof(device_p->name), a1.ptr, strnlen(a1.ptr, PD_QURT_DEVICE_NAME_LEN));

               qurt_anysignal_init(&device_p->anysignal);

               // link new into device list

               device_p->next = pd_qurt_anysignal_internal.list;
               pd_qurt_anysignal_internal.list = device_p;
            }

            // operate on existing

            if (PD_QURT_DEVICE_ASID_MAX > pn.asid)
            {
               device_p->open_count[pn.asid]++;
            }

            qurt_pimutex_unlock(&pd_qurt_anysignal_internal.mutex); // serialize driver access

            rc = qurt_qdi_handle_create_from_obj_t(client_handle, (qurt_qdi_obj_t*)device_p); // return handle
         }

         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "method open %d %s %s %x", pn.asid, pn.name, device_p->name, rc);

         break;
      }

      /**
      API, pd_qurt_anysignal_destroy
      @param[in] PD_QURT_HANDLE_ANYSIGNAL Opaque Device Handle
      @return
      PD_QURT_STATUS -- Error Indication
      */
      case QDI_CLOSE:
      {
         qurt_pimutex_lock(&pd_qurt_anysignal_internal.mutex); // serialize driver access

         // operate on existing

         if (PD_QURT_DEVICE_ASID_MAX > pn.asid)
         {
            device_p->open_count[pn.asid]--;
         }

         qurt_pimutex_unlock(&pd_qurt_anysignal_internal.mutex); // serialize driver access

         rc = qurt_qdi_method_default(client_handle, obj, method, a1, a2, a3, a4, a5, a6, a7, a8, a9); // pass along

         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "method close %d %s %s %x", pn.asid, pn.name, device_p->name, rc);

         break;
      }

      /**
      API, pd_qurt_anysignal_wait
      @param[in] PD_QURT_HANDLE_ANYSIGNAL Opaque Device Handle
      @param[in] unsigned int Operation Argument
      @param[in] unsigned int* Operation Result
      @return
      PD_QURT_STATUS -- Error Indication
      */
      case PD_QURT_METHOD_ANYSIGNAL_WAIT:
      {
         unsigned int mask;

         qurt_qdi_safe_to_kill(); // honor and react to thread kills around potential blocking

         if (qurt_qdi_marked_to_kill())
         {
            qurt_qdi_unsafe_to_kill();

            qurt_thread_stop();
         }

         qurt_qdi_unsafe_to_kill();

         MSG_SPRINTF_3(MSG_SSID_TMS, MSG_LEGACY_HIGH, "method wait block %d %s %s", pn.asid, pn.name, device_p->name);

         mask = qurt_anysignal_wait(&device_p->anysignal, a1.num);

         if (NULL == a2.ptr)
         {
            rc = 0; // success
         }

         else
         {
            rc = qurt_qdi_copy_to_user(client_handle, a2.ptr, &mask, sizeof(mask)); // copy to user
         }

         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "method wait %d %s %s %x", pn.asid, pn.name, device_p->name, rc);

         break;
      }

      /**
      API, pd_qurt_anysignal_set
      @param[in] PD_QURT_HANDLE_ANYSIGNAL Opaque Device Handle
      @param[in] unsigned int Operation Argument
      @param[in] unsigned int* Operation Result
      @return
      PD_QURT_STATUS -- Error Indication
      */
      case PD_QURT_METHOD_ANYSIGNAL_SET:
      {
         unsigned int mask;

         qurt_qdi_safe_to_kill(); // honor and react to thread kills around potential blocking

         if (qurt_qdi_marked_to_kill())
         {
            qurt_qdi_unsafe_to_kill();

            qurt_thread_stop();
         }

         qurt_qdi_unsafe_to_kill();

         mask = qurt_anysignal_set(&device_p->anysignal, a1.num);

         if (NULL == a2.ptr)
         {
            rc = 0; // success
         }

         else
         {
            rc = qurt_qdi_copy_to_user(client_handle, a2.ptr, &mask, sizeof(mask)); // copy to user
         }

         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "method set %d %s %s %x", pn.asid, pn.name, device_p->name, rc);

         break;
      }

      /**
      API, pd_qurt_anysignal_get
      @param[in] PD_QURT_HANDLE_ANYSIGNAL Opaque Device Handle
      @param[in] unsigned int* Operation Result
      @return
      PD_QURT_STATUS -- Error Indication
      */
      case PD_QURT_METHOD_ANYSIGNAL_GET:
      {
         unsigned int mask;

         qurt_qdi_safe_to_kill(); // honor and react to thread kills around potential blocking

         if (qurt_qdi_marked_to_kill())
         {
            qurt_qdi_unsafe_to_kill();

            qurt_thread_stop();
         }

         qurt_qdi_unsafe_to_kill();

         mask = qurt_anysignal_get(&device_p->anysignal);

         if (NULL == a1.ptr)
         {
            rc = 0; // success
         }

         else
         {
            rc = qurt_qdi_copy_to_user(client_handle, a1.ptr, &mask, sizeof(mask)); // copy to user
         }

         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "method get %d %s %s %x", pn.asid, pn.name, device_p->name, rc);

         break;
      }

      /**
      API, pd_qurt_anysignal_clear
      @param[in] PD_QURT_HANDLE_ANYSIGNAL Opaque Device Handle
      @param[in] unsigned int Operation Argument
      @param[in] unsigned int* Operation Result
      @return
      PD_QURT_STATUS -- Error Indication
      */
      case PD_QURT_METHOD_ANYSIGNAL_CLEAR:
      {
         unsigned int mask;

         qurt_qdi_safe_to_kill(); // honor and react to thread kills around potential blocking

         if (qurt_qdi_marked_to_kill())
         {
            qurt_qdi_unsafe_to_kill();

            qurt_thread_stop();
         }

         qurt_qdi_unsafe_to_kill();

         mask = qurt_anysignal_clear(&device_p->anysignal, a1.num);

         if (NULL == a2.ptr)
         {
            rc = 0; // success
         }

         else
         {
            rc = qurt_qdi_copy_to_user(client_handle, a2.ptr, &mask, sizeof(mask)); // copy to user
         }

         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "method clear %d %s %s %x", pn.asid, pn.name, device_p->name, rc);

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

} opener = { { pd_qurt_anysignal_device_invocation, QDI_REFCNT_PERM, 0 } };

/**
API, Initialization of service prior to use
@return
None.
*/
void pd_qurt_anysignal_driver_init(void)
{
   qurt_pimutex_init(&pd_qurt_anysignal_internal.mutex); // serialize driver access

   qurt_pimutex_lock(&pd_qurt_anysignal_internal.mutex); // serialize driver access

   qurt_qdi_devname_register(PD_QURT_DEVICE_ANYSIGNAL, &opener.qdiobj); // register driver

   qurt_pimutex_unlock(&pd_qurt_anysignal_internal.mutex); // serialize driver access

   // API Check

   {
      unsigned int out_mask;
      PD_QURT_HANDLE_ANYSIGNAL handle;

      if (PD_QURT_STATUS_SUCCESS <= (handle = pd_qurt_anysignal_init("PD_QURT")) &&
          PD_QURT_STATUS_SUCCESS == pd_qurt_anysignal_clear(handle, ((unsigned int)-1), &out_mask) &&
          PD_QURT_STATUS_SUCCESS == pd_qurt_anysignal_set(handle, ((unsigned int)-1), &out_mask) &&
          PD_QURT_STATUS_SUCCESS == pd_qurt_anysignal_wait(handle, ((unsigned int)-1), &out_mask) &&
          PD_QURT_STATUS_SUCCESS == pd_qurt_anysignal_get(handle, &out_mask) &&
          PD_QURT_STATUS_SUCCESS == pd_qurt_anysignal_destroy(handle))
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
void pd_qurt_anysignal_driver_term(void)
{
   /* NULL */ /* DECISION TO NOT CLEANUP SERVICE FOR POST MORTEM REASONS */
}
