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

 $Header: //components/rel/core.mpss/3.5.c12.3/debugtools/rcinit/src/example/rcinit_ex_qurt.c#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#include "err.h"
#include "rcinit_qurt.h"

/*===========================================================================

 FUNCTION rcinit_example_qurttask

 DESCRIPTION
 example task boiler plate

 DEPENDENCIES
 none

 RETURN VALUE
 operation success

 SIDE EFFECTS
 none
 ===========================================================================*/

#define RCINIT_EXAMPLE_QURTTASK_MASK0  0x00000001
#define RCINIT_EXAMPLE_QURTTASK_MASK1  0x00000002
#define RCINIT_EXAMPLE_QURTTASK_MASKS  (RCINIT_EXAMPLE_QURTTASK_MASK0|RCINIT_EXAMPLE_QURTTASK_MASK1)

/* static */ struct // clients do not need to access this, expose the communications infrastructure by API
{
   boolean init;
   qurt_anysignal_t Events;

} rcinit_example_qurttask_events;

void rcinit_example_qurttask_client_signal_event0(void) // called by other task
{
   if (TRUE != rcinit_example_qurttask_events.init)
   {
      ERR_FATAL("event trigger", 0, 0, 0);
   }
   else
   {
      qurt_anysignal_set(&rcinit_example_qurttask_events.Events, RCINIT_EXAMPLE_QURTTASK_MASK0);
   }
}

void rcinit_example_qurttask_client_signal_event1(void) // called by other task
{
   if (TRUE != rcinit_example_qurttask_events.init)
   {
      ERR_FATAL("event trigger", 0, 0, 0);
   }
   else
   {
      qurt_anysignal_set(&rcinit_example_qurttask_events.Events, RCINIT_EXAMPLE_QURTTASK_MASK1);
   }
}

void rcinit_example_qurttask(void* arg_p)
{
   qurt_anysignal_init(&rcinit_example_qurttask_events.Events);

   rcinit_example_qurttask_events.init = TRUE;

   rcinit_handshake_startup(); // *REQUIRED*

   for (;;)
   {
      unsigned long mask = qurt_anysignal_wait(&rcinit_example_qurttask_events.Events, RCINIT_EXAMPLE_QURTTASK_MASKS);

      if (RCINIT_EXAMPLE_QURTTASK_MASK0 & mask)
      {
         unsigned long rc = qurt_anysignal_clear(&rcinit_example_qurttask_events.Events, RCINIT_EXAMPLE_QURTTASK_MASK0);
         mask &= ~RCINIT_EXAMPLE_QURTTASK_MASK0;
         rc = rc; // unused
      }

      if (RCINIT_EXAMPLE_QURTTASK_MASK1 & mask)
      {
         unsigned long rc = qurt_anysignal_clear(&rcinit_example_qurttask_events.Events, RCINIT_EXAMPLE_QURTTASK_MASK1);
         mask &= ~RCINIT_EXAMPLE_QURTTASK_MASK1;
         rc = rc; // unused
      }

      // ...

      if (0 != mask)
      {
         ERR_FATAL("unknown event mask", 0, 0, 0);
      }
   }

   /* NOTREACHED */ // lint, CR421030

   rcinit_example_qurttask_events.init = FALSE;

   qurt_anysignal_destroy(&rcinit_example_qurttask_events.Events);

   arg_p = arg_p; // unused
}
