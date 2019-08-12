#include "rex.h"
#include "rex_port.h"

void rex_kill_task(rex_tcb_type *p_tcb)
{   
   if (NULL == p_tcb)
   {
      REX_ERR_FATAL("REX TCB ptr is invalid");
   }
   
   /* Does OS platform specific thread removal */
   rex_os_kill_task(p_tcb);
}
