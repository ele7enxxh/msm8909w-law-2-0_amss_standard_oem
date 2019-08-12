/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 RCINIT SOURCE MODULE

 GENERAL DESCRIPTION
 this module contains the source implementation for the rcinit framework

 EXTERNALIZED FUNCTIONS
 yes

 INITIALIZATION AND SEQUENCING REQUIREMENTS
 yes

 Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

 EDIT HISTORY FOR MODULE

 $Header: //components/rel/core.mpss/3.5.c12.3/debugtools/rcinit/src/example/rcinit_ex_posix.c#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#include "err.h"
#include "rcinit_posix.h"

/*===========================================================================

 FUNCTION rcinit_example_posix

 DESCRIPTION
 example task boiler plate

 DEPENDENCIES
 none

 RETURN VALUE
 operation success

 SIDE EFFECTS
 none
 ===========================================================================*/

/* static */ struct // clients do not need to access this, expose the communications infrastructure by API
{
   boolean init;
   pthread_cond_t cond;
   pthread_mutex_t mutex;

} rcinit_example_posix_events;

void rcinit_example_posix_client_signal(void) // called by other task
{
   if (TRUE != rcinit_example_posix_events.init)
   {
      ERR_FATAL("event trigger", 0, 0, 0);
   }
   else
   {
      pthread_mutex_lock(&rcinit_example_posix_events.mutex);
      pthread_cond_signal(&rcinit_example_posix_events.cond);
      pthread_mutex_unlock(&rcinit_example_posix_events.mutex);
   }
}

void rcinit_example_posix(void* arg_p)
{
   pthread_mutexattr_t attr_mutex;
   pthread_condattr_t attr_cond;

   pthread_mutexattr_init(&attr_mutex);
   pthread_mutex_init(&rcinit_example_posix_events.mutex, &attr_mutex);
   pthread_mutexattr_destroy(&attr_mutex);

   pthread_condattr_init(&attr_cond);
   pthread_cond_init(&rcinit_example_posix_events.cond, &attr_cond);
   pthread_condattr_destroy(&attr_cond);

   rcinit_example_posix_events.init = TRUE;

   rcinit_handshake_startup(); // *REQUIRED*

   for (;;)
   {
      pthread_mutex_lock(&rcinit_example_posix_events.mutex);
      pthread_cond_wait(&rcinit_example_posix_events.cond, &rcinit_example_posix_events.mutex);
      pthread_mutex_unlock(&rcinit_example_posix_events.mutex);

      // ...
   }

   /* NOTREACHED */ // lint, CR421030

   arg_p = arg_p; // unused
}
