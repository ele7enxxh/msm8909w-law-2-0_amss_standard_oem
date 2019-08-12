#include "rex_os_port.h"

void rex_posix_init(void)
{
   static int rex_posix_inited = 0;
   if (0 == rex_posix_inited)
   {
      rex_posix_inited = 1;
      
      /* Initialize the REX context functionality */
      rex_context_init();

      /* Initialize the task list */
      rex_task_list_mgr_init();

      /* Initialize the REX functionality */
      rex_sigs_init();
   }
}
