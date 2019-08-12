/**
@file pd_dump.c
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
$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/pd_mon/src/pd_dump.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Change: 11985146 $
$Author: mplcsds1 $
===============================================================================*/

#include "pd_dump.h"

/////////////////////////////////////////////////////////////////////
// Localized Type Declarations
/////////////////////////////////////////////////////////////////////

struct pd_dump_device_s
{
   qurt_qdi_obj_t qdiobj; // required to be first

   char name[PD_DUMP_DEVICE_NAME_LEN];

   int open_count[PD_DUMP_DEVICE_ASID_MAX];

   struct pd_dump_device_s* next;
};

struct MemBlock_s
{
   void* addr;
   unsigned long len;
};

struct Thread_s
{
   void* addr;
};

/////////////////////////////////////////////////////////////////////
// Localized Storage
/////////////////////////////////////////////////////////////////////

static struct pd_dump_internal_s
{
   qurt_mutex_t mutex;

   struct pd_dump_device_s* list;

} pd_dump_internal;

/////////////////////////////////////////////////////////////////////
// Implementation Details
/////////////////////////////////////////////////////////////////////

static void pd_dump_device_release(qurt_qdi_obj_t *qdiobj)
{
   free(qdiobj);
}

static int pd_dump_device_invocation(int client_handle, qurt_qdi_obj_t* obj, int method, qurt_qdi_arg_t a1, qurt_qdi_arg_t a2, qurt_qdi_arg_t a3, qurt_qdi_arg_t a4, qurt_qdi_arg_t a5, qurt_qdi_arg_t a6, qurt_qdi_arg_t a7, qurt_qdi_arg_t a8, qurt_qdi_arg_t a9)
{
   int rc = -1; // failure
   struct pd_dump_device_s* device_p = (struct pd_dump_device_s*)obj;
   qurt_sysenv_procname_t pn;

   /* client_handle means that we're asking this particular client handle to answer the question about its process */
   /* QDI_OS_SYSENV is the standard method for requesting this type of information */
   /* QDI_HANDLE_LOCAL_CLIENT means that this request originated locally */
   /* QURT_SYSENV_PROCNAME_TYPE is the specific piece of data being requested -- to fill in a qurt_sysenv_procname_t structure */
   /* &pn is the structure we want filled in */

   if (0 > (rc = qurt_qdi_handle_invoke(client_handle, QDI_OS_SYSENV, QDI_HANDLE_LOCAL_CLIENT, QURT_SYSENV_PROCNAME_TYPE, &pn)) || PD_DUMP_DEVICE_ASID_MAX >= pn.asid)
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
      API, pd_dump_init
      @param[in] a1.ptr/const char* NULL Terminated Device Name
      @return
      PD_DUMP_HANDLE -- Opaque Device Handle
      */
      case QDI_OPEN:
      {
         if (NULL != a1.ptr && PD_DUMP_DEVICE_NAME_LEN > strnlen(a1.ptr, PD_DUMP_DEVICE_NAME_LEN)) // name length check
         {
            qurt_pimutex_lock(&pd_dump_internal.mutex); // serialize driver access

            // locate existing named device

            for (device_p = pd_dump_internal.list; NULL != device_p; device_p = device_p->next)
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

                  qurt_pimutex_unlock(&pd_dump_internal.mutex); // serialize driver access

                  break;
               }

               // initialize new

               secure_memset(device_p, 0, sizeof(*device_p));

               device_p->qdiobj.invoke = pd_dump_device_invocation;
               device_p->qdiobj.refcnt = QDI_REFCNT_INIT;
               device_p->qdiobj.release = pd_dump_device_release;

               memsmove(device_p->name, sizeof(device_p->name), a1.ptr, strnlen(a1.ptr, PD_DUMP_DEVICE_NAME_LEN));

               // link new into device list

               device_p->next = pd_dump_internal.list;
               pd_dump_internal.list = device_p;
            }

            // operate on existing

            if (PD_DUMP_DEVICE_ASID_MAX > pn.asid)
            {
               device_p->open_count[pn.asid]++;
            }

            qurt_pimutex_unlock(&pd_dump_internal.mutex); // serialize driver access

            rc = qurt_qdi_handle_create_from_obj_t(client_handle, (qurt_qdi_obj_t*)device_p); // return handle
         }

         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "method open %d %s %s %x", pn.asid, pn.name, device_p->name, rc);

         break;
      }

      /**
      API, pd_dump_destroy
      @param[in] PD_DUMP_HANDLE Opaque Device Handle
      @return
      PD_DUMP_STATUS -- Error Indication
      */
      case QDI_CLOSE:
      {
         qurt_pimutex_lock(&pd_dump_internal.mutex); // serialize driver access

         // operate on existing

         if (PD_DUMP_DEVICE_ASID_MAX > pn.asid)
         {
            device_p->open_count[pn.asid]--;
         }

         qurt_pimutex_unlock(&pd_dump_internal.mutex); // serialize driver access

         rc = qurt_qdi_method_default(client_handle, obj, method, a1, a2, a3, a4, a5, a6, a7, a8, a9); // pass along

         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "method close %d %s %s %x", pn.asid, pn.name, device_p->name, rc);

         break;
      }

      /**
      INTERNAL, OS_DUMP_PROCESS_EXIT Handler
      a1.ptr is a pointer to a list of (address, size) pairs
      a2.num is the number of entries pointed to by a1.ptr
      a3.ptr is a pointer to a list of TCB addresses for the threads
      a4.num is the number of entries pointed to by a3.ptr
      a5.num is the size of each TCB
      */
      case QDI_OS_DUMP_PROCESS_EXIT:
      {
         int j;
         struct Thread_s* Thread;
         struct MemBlock_s* MemBlock;

         qurt_pimutex_lock(&pd_dump_internal.mutex); // serialize driver access

         MemBlock = (struct MemBlock_s*)a1.ptr;

         for (j = 0; j < a2.num; j++)
         {
            MSG_SPRINTF_2(MSG_SSID_TMS, MSG_LEGACY_HIGH, ".ram @%x, %x", MemBlock[j].addr, MemBlock[j].len);
         }

         Thread = (struct Thread_s*)a3.ptr;

         for (j = 0; j < a4.num; j++)
         {
            MSG_SPRINTF_2(MSG_SSID_TMS, MSG_LEGACY_HIGH, ".tcb @%x, %x", Thread[j].addr, a5.num);
         }

#if 0 // compile time hack

         int fd;
         char filename[128];
         unsigned int counter;

         // 1. hlos_fs, check enable

         if (sizeof(filename) <= tms_utils_fmt(filename, sizeof(filename), "%s.enable", pn.name))
         {
            qurt_pimutex_unlock(&pd_dump_internal.mutex); // serialize driver access

            rc = 0;

            MSG_SPRINTF_1(MSG_SSID_TMS, MSG_LEGACY_HIGH, "enable filename too long %s", filename);

            break;
         }

         if (0 > (fd = rfsa_open(filename, O_RDONLY)))
         {
            qurt_pimutex_unlock(&pd_dump_internal.mutex); // serialize driver access

            rc = 0;

            MSG_SPRINTF_1(MSG_SSID_TMS, MSG_LEGACY_HIGH, "disabled %s", filename);

            break;
         }

         rfsa_close(fd);

         // 2. hlos_fs, read counter dump

         if (sizeof(filename) <= tms_utils_fmt(filename, sizeof(filename), "%s.counter", pn.name))
         {
            qurt_pimutex_unlock(&pd_dump_internal.mutex); // serialize driver access

            rc = 0;

            MSG_SPRINTF_1(MSG_SSID_TMS, MSG_LEGACY_HIGH, "read counter filename too long %s", filename);

            break;
         }

         if (0 > (fd = rfsa_open(filename, O_RDONLY)) ||
             sizeof(counter) != rfsa_read(fd, (char*)&counter, sizeof(counter)))
         {
            counter = 0;

            MSG_SPRINTF_2(MSG_SSID_TMS, MSG_LEGACY_HIGH, "read counter failed %s %x", filename, counter);
         }

         else
         {
            MSG_SPRINTF_2(MSG_SSID_TMS, MSG_LEGACY_HIGH, "read counter %s %x", filename, counter);
         }

         rfsa_close(fd);

         // 3. hlos_fs, open elf

         if (sizeof(filename) <= tms_utils_fmt(filename, sizeof(filename), "%s.%x.elf", pn.name, counter))
         {
            qurt_pimutex_unlock(&pd_dump_internal.mutex); // serialize driver access

            rc = 0;

            MSG_SPRINTF_1(MSG_SSID_TMS, MSG_LEGACY_HIGH, "elf filename too long %s", filename);

            break;
         }

         if (0 > (fd = rfsa_open(filename, O_CREAT)))
         {
            qurt_pimutex_unlock(&pd_dump_internal.mutex); // serialize driver access

            rc = 0;

            MSG_SPRINTF_1(MSG_SSID_TMS, MSG_LEGACY_HIGH, "output disabled %s", filename);

            break;
         }

         MSG_SPRINTF_1(MSG_SSID_TMS, MSG_LEGACY_HIGH, "open %s", filename);

         // 4. build elf headers .ram/.tcb

         MSG_SPRINTF_1(MSG_SSID_TMS, MSG_LEGACY_HIGH, "build elf headers %s", filename);

         // 5. hlos_fs, write all elf headers

         MSG_SPRINTF_1(MSG_SSID_TMS, MSG_LEGACY_HIGH, "write elf headers %s", filename);

         // 6. hlos_fs, write .ram/.tcb

         MSG_SPRINTF_1(MSG_SSID_TMS, MSG_LEGACY_HIGH, "write memory %s", filename);

         // 7. destroy elf headers .ram/.tcb

         MSG_SPRINTF_1(MSG_SSID_TMS, MSG_LEGACY_HIGH, "destroy elf headers %s", filename);

         // 8. hlos_fs, close elf

         rfsa_close(fd);

         MSG_SPRINTF_1(MSG_SSID_TMS, MSG_LEGACY_HIGH, "close %s", filename);

         // 9. increment counter

         counter++;

         // 10. hlos_fs, write counter dump, activity is trigger for hlos

         if (sizeof(filename) <= tms_utils_fmt(filename, sizeof(filename), "%s.counter", pn.name))
         {
            qurt_pimutex_unlock(&pd_dump_internal.mutex); // serialize driver access

            rc = 0;

            MSG_SPRINTF_1(MSG_SSID_TMS, MSG_LEGACY_HIGH, "write counter filename too long %s", filename);

            break;
         }



         if (0 > (fd = rfsa_open(filename, O_CREAT)) ||
             sizeof(counter) != rfsa_write(fd, (char*)&counter, sizeof(counter)))
         {
            MSG_SPRINTF_2(MSG_SSID_TMS, MSG_LEGACY_HIGH, "write counter failed %s %x", filename, counter);
         }

         else
         {
            MSG_SPRINTF_2(MSG_SSID_TMS, MSG_LEGACY_HIGH, "write counter %s %x", filename, counter);
         }

         rfsa_close(fd);

#endif // compile time hack

         qurt_pimutex_unlock(&pd_dump_internal.mutex); // serialize driver access

         // rc = qurt_qdi_method_default(client_handle, obj, method, a1, a2, a3, a4, a5, a6, a7, a8, a9); // pass along

         rc = 0;

         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "method os_dump %d %s %s %x", pn.asid, pn.name, device_p->name, rc);

         break;
      }

      /**
      API, pd_dump_method_0
      @param[in] PD_DUMP_HANDLE Opaque Device Handle
      @return
      PD_DUMP_STATUS -- Error Indication
      */
      case PD_DUMP_METHOD_0:
      {
         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "method 0 %d %s %s %x", pn.asid, pn.name, device_p->name, rc);

         break;
      }

      /**
      API, pd_dump_method_1
      @param[in] PD_DUMP_HANDLE Opaque Device Handle
      @return
      PD_DUMP_STATUS -- Error Indication
      */
      case PD_DUMP_METHOD_1:
      {
         MSG_SPRINTF_4(MSG_SSID_TMS, MSG_LEGACY_HIGH, "method 1 %d %s %s %x", pn.asid, pn.name, device_p->name, rc);

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

} opener = { { pd_dump_device_invocation, QDI_REFCNT_PERM, 0 } };

/**
API, Initialization of service prior to use
@return
None.
*/
void pd_dump_driver_init(void)
{
   qurt_pimutex_init(&pd_dump_internal.mutex); // serialize driver access

   qurt_pimutex_lock(&pd_dump_internal.mutex); // serialize driver access

   qurt_qdi_devname_register(PD_DUMP_DEVICE, &opener.qdiobj); // register driver

   qurt_pimutex_unlock(&pd_dump_internal.mutex); // serialize driver access

   // API Check

   {
      PD_DUMP_HANDLE handle;

      if (PD_DUMP_STATUS_SUCCESS <= (handle = pd_dump_init("PD_DUMP")) &&
          PD_DUMP_STATUS_SUCCESS == pd_dump_destroy(handle))
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
void pd_dump_driver_term(void)
{
   /* NULL */ /* DECISION TO NOT CLEANUP SERVICE FOR POST MORTEM REASONS */
}
