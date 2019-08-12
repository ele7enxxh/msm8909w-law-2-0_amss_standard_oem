#include <stdlib.h>
#include "qurt_qdi.h"
#include "qurt_thread.h"
#include "cbdrv_qdi.h"
#include "cbdrv.h"

#define STKSIZE  8192

static int cbdrv_handle = -1;

static qurt_thread_attr_t attr;
static qurt_thread_t tid;

static void cbdrv_callback_dispatcher(void *arg)
{
   struct cbdrv_qdi_cbinfo info;

   for (;;) {
      qurt_qdi_handle_invoke(cbdrv_handle, CBDRV_GET_CB, &info);
      (*info.pfn)(info.arg, info.value);
   }
}

void cbdrv_init(void)
{
   cbdrv_handle = qurt_qdi_open("/dev/cbdrv");
   qurt_thread_attr_init(&attr);
   qurt_thread_attr_set_stack_size(&attr, STKSIZE);
   qurt_thread_attr_set_stack_addr(&attr, malloc(STKSIZE));
   qurt_thread_attr_set_priority(&attr, qurt_thread_get_priority(qurt_thread_get_id()));
   qurt_thread_create(&tid, &attr, cbdrv_callback_dispatcher, NULL);
}

void cbdrv_register_callback(void (*pfn)(void *, int),
                             void *arg)
{
   qurt_qdi_handle_invoke(cbdrv_handle, CBDRV_REGISTER, pfn, arg);
}

void cbdrv_notify_callbacks(int value)
{
   qurt_qdi_handle_invoke(cbdrv_handle, CBDRV_NOTIFY, value);
}
