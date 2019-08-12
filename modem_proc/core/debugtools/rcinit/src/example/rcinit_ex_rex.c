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

 $Header: //components/rel/core.mpss/3.5.c12.3/debugtools/rcinit/src/example/rcinit_ex_rex.c#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#include "err.h"
#include "rcinit_rex.h"

/*===========================================================================

 FUNCTION rcinit_example_rextask

 DESCRIPTION
 example task boiler plate

 DEPENDENCIES
 none

 RETURN VALUE
 operation success

 SIDE EFFECTS
 none
 ===========================================================================*/

#define RCINIT_EXAMPLE_REXTASK_MASK0   0x00000001
#define RCINIT_EXAMPLE_REXTASK_MASK1   0x00000002
#define RCINIT_EXAMPLE_REXTASK_MASKS   (RCINIT_EXAMPLE_REXTASK_MASK0|RCINIT_EXAMPLE_REXTASK_MASK1)

/* static */ struct // clients do not need to access this, expose the communications infrastructure by API
{
   boolean init;
   rex_tcb_type* myself;

} rcinit_example_rextask_events;

void rcinit_example_rextask_client_signal_event0(void) // called by other task
{
   if (TRUE != rcinit_example_rextask_events.init)
   {
      ERR_FATAL("event trigger", 0, 0, 0);
   }
   else
   {
      rex_set_sigs(rcinit_example_rextask_events.myself, RCINIT_EXAMPLE_REXTASK_MASK0);
   }
}

void rcinit_example_rextask_client_signal_event1(void) // called by other task
{
   if (TRUE != rcinit_example_rextask_events.init)
   {
      ERR_FATAL("event trigger", 0, 0, 0);
   }
   else
   {
      rex_set_sigs(rcinit_example_rextask_events.myself, RCINIT_EXAMPLE_REXTASK_MASK1);
   }
}

void rcinit_example_rextask(unsigned long arg_p)
{
   rcinit_example_rextask_events.myself = rex_self();

   rcinit_example_rextask_events.init = TRUE;

   rcinit_handshake_startup(); // *REQUIRED*

   for (;;)
   {
      unsigned long mask = rex_wait(RCINIT_EXAMPLE_REXTASK_MASKS);

      if (RCINIT_EXAMPLE_REXTASK_MASK0 & mask)
      {
         rex_clr_sigs(rcinit_example_rextask_events.myself, RCINIT_EXAMPLE_REXTASK_MASK0);
         mask &= ~RCINIT_EXAMPLE_REXTASK_MASK0;
      }

      if (RCINIT_EXAMPLE_REXTASK_MASK1 & mask)
      {
         rex_clr_sigs(rcinit_example_rextask_events.myself, RCINIT_EXAMPLE_REXTASK_MASK1);
         mask &= ~RCINIT_EXAMPLE_REXTASK_MASK1;
      }

      if (0 != mask)
      {
         ERR_FATAL("unknown event mask", 0, 0, 0);
      }
   }

   /* NOTREACHED */ // lint, CR421030

   rcinit_example_rextask_events.init = FALSE;

   rcinit_example_rextask_events.myself = NULL;

   arg_p = arg_p; // unused
}
