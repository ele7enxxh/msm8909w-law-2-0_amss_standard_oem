#include <stdlib.h>
#include "qurt_rmutex.h"
#include "qurt_qdi.h"
#include "cbdrv_qdi.h"
#include "cbdrv.h"

struct cbq_elem {
   struct cbq_elem *next;
   struct cbdrv_cbinfo *cbinfo;
   int value;
};

struct cbdrv_cbinfo {
   struct cbdrv_object *object;
   void (*pfn)(void *, int);
   void *arg;
};

struct cbdrv_object {
   int (*invoke)();
   struct cbq_elem *cbqueue;
   qurt_mutex_t mtx;
};

struct cbdrv_opener {
   int (*invoke)();
};

static void cbdrv_qdi_do_callback(void *ptr, int value)
{
   struct cbdrv_cbinfo *pCbInfo = ptr;
   struct cbq_elem *cbq;

   cbq = malloc(sizeof(*cbq));
   if (cbq) {
      cbq->cbinfo = pCbInfo;
      cbq->value = value;
      qurt_rmutex_lock(&pCbInfo->object->mtx);
      cbq->next = pCbInfo->object->cbqueue;
      pCbInfo->object->cbqueue = cbq;
      qurt_rmutex_unlock(&pCbInfo->object->mtx);
      qurt_futex_wake((void *)&pCbInfo->object->cbqueue, 1);
   }
}

static int cbdrv_qdi_get_callback(struct cbdrv_object *pCbDrv,
                                  void *buf)
{
   struct cbdrv_qdi_cbinfo info;
   struct cbq_elem *cbq;

   for (;;) {
      qurt_rmutex_lock(&pCbDrv->mtx);
      cbq = pCbDrv->cbqueue;
      if (cbq)
         pCbDrv->cbqueue = cbq->next;
      qurt_rmutex_unlock(&pCbDrv->mtx);
      if (cbq)
         break;
      qurt_futex_wait((void *)&pCbDrv->cbqueue, 0);
   }

   info.pfn = cbq->cbinfo->pfn;
   info.arg = cbq->cbinfo->arg;
   info.value = cbq->value;

   free(cbq);

   return qurt_qdi_copyout(buf, &info, sizeof(info));
}

int cbdrv_qdi_invocation(void *ptr, int method,
                         qurt_qdi_arg_t a1, qurt_qdi_arg_t a2,
                         qurt_qdi_arg_t a3, qurt_qdi_arg_t a4)
{
   struct cbdrv_object *pCbDrv = ptr;
   struct cbdrv_cbinfo *pCbInfo;
   int ret;

   switch (method) {
   case QDI_OPEN:
      pCbDrv = malloc(sizeof(*pCbDrv));
      if (pCbDrv == NULL)
         return -1;
      pCbDrv->invoke = cbdrv_qdi_invocation;
      pCbDrv->cbqueue = NULL;
      qurt_rmutex_init(&pCbDrv->mtx);
      ret = qurt_qdi_new_handle_from_obj(pCbDrv);
      if (ret < 0)
         free(pCbDrv);
      return ret;
   case QDI_CLOSE:
      return 0;
   case CBDRV_REGISTER:
      pCbInfo = malloc(sizeof(*pCbInfo));
      if (pCbInfo == NULL)
         return -1;
      pCbInfo->object = pCbDrv;
      pCbInfo->pfn = (void (*)(void *, int))(a1.ptr);
      pCbInfo->arg = a2.ptr;
      cbdrv_register_callback(cbdrv_qdi_do_callback, pCbInfo);
      return 0;
   case CBDRV_NOTIFY:
      cbdrv_notify_callbacks(a1.num);
      return 0;
   case CBDRV_GET_CB:
      return cbdrv_qdi_get_callback(pCbDrv, a1.ptr);
   default:
      return -1;
   }
}

const struct cbdrv_opener opener = {
   cbdrv_qdi_invocation
};

void cbdrv_qdi_init(void)
{
   cbdrv_init();
   qurt_qdi_register_devname("/dev/cbdrv",
                        (void *)&opener);
}
