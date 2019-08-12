#include <stdlib.h>
#include "qurt_rmutex.h"
#include "cbdrv.h"

struct cb_elem {
   struct cb_elem *next;
   void (*pfn)(void *, int);
   void *arg;
};

static qurt_mutex_t cbdrv_mutex;
static struct cb_elem *pfnlist;

void cbdrv_init(void)
{
   qurt_rmutex_init(&cbdrv_mutex);
   pfnlist = NULL;
}

void cbdrv_register_callback(void (*pfn)(void *, int),
                             void *arg)
{
   struct cb_elem *p;

   p = malloc(sizeof(*p));
   if (p) {
      p->pfn = pfn;
      p->arg = arg;
      qurt_rmutex_lock(&cbdrv_mutex);
      p->next = pfnlist;
      pfnlist = p;
      qurt_rmutex_unlock(&cbdrv_mutex);
   }
}

void cbdrv_notify_callbacks(int value)
{
   struct cb_elem *p;

   for (p=pfnlist; p; p=p->next) {
      (*p->pfn)(p->arg, value);
   }
}
