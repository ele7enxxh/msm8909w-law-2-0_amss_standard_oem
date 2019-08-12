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

 $Header: //components/rel/core.mpss/3.5.c12.3/debugtools/rcinit/src/example/rcinit_ex_policy.c#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#include "rcinit.h"
#include "string.h"

/*===========================================================================

 FUNCTION rcinit_example_policyfn

 DESCRIPTION
 example policy function

 DEPENDENCIES
 tech provided dependency analysis must be performed such that all tasks
 or services requried by this policy function are started before its call.

 RETURN VALUE
 policy name from provided list

 SIDE EFFECTS
 system alteration at next group if policy function decision returns
 any policy name on the provided list. the poilcy function can utilize
 any task or service as a dependency *IN SO MUCH AS* the prerequsite
 dependency has already been started.

 ===========================================================================*/

// this function has no implementation; it is merely an example of the syntax
// and return result required from the tech area that provides the policy
// decision logic.

RCINIT_NAME rcinit_example_policyfn(RCINIT_NAME* list)
{
   while (RCINIT_NULL != *list) // list is terminated by RCINIT_NULL
   {
      if (0 == strcmp("default", *list)) // list is comprised of policy tags (const strings) from build time
      {
         break; // match exits loop
      }

      list++; // iterate for next policy tag
   }

   return(*list); // "default" or RCINIT_NULL
}
