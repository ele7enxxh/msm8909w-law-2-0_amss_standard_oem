#include "rex.h"
#include "rex_restricted.h"
#include "rex_port.h"
#include "rex_private.h"

/* Set to 1 after rex_init */
static int rex_init_complete = 0;

#ifdef REX_CORE_TEST
extern void rex_start_test(void);
#endif //REX_CORE_TEST

int rex_core_common_init(void)
{
   int err_code;

   /*
   ** Call any initialization callback functions registered
   ** with REX before any REX task starts.
   */
   err_code = rex_invoke_cbs_on_init(); 
   if(REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Failed in Init REX Callbacks");
   }

   /* Create the REX DPC task */
   err_code = rex_create_dpc_task();
   if(REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Failed to create DPC Task");
   }
   
   rex_init_complete = 1;

   return REX_SUCCESS;
}

boolean rex_check_if_initialized(void)
{
   return (rex_init_complete?TRUE:FALSE);
}
