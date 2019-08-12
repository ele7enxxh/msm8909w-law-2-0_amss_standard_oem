#include "rex.h"
#include "dog.h"
#include "rex_tcb.h"
#include "rex_port.h"

void rex_os_pause_dog_monitor(rex_os_tcb_type *p_os_tcb)
{
   if (rex_os_get_dog_registration_handle(p_os_tcb) >= 0)
   {
      dog_monitor_pause(rex_os_get_dog_registration_handle(p_os_tcb));
   }
}

void rex_os_resume_dog_monitor(rex_os_tcb_type *p_os_tcb)
{
   if (rex_os_get_dog_registration_handle(p_os_tcb) >= 0)
   {
      dog_monitor_resume(rex_os_get_dog_registration_handle(p_os_tcb));
   }
}
