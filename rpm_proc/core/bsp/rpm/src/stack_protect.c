 /*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
GENERAL DESCRIPTION
  This module contains the implementation of stack protection routines. 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2011 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/bsp/rpm/src/stack_protect.c#1 $
$DateTime: 2015/02/25 02:53:43 $

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "err.h"

#if !defined(NO_STACK_CHECK)
    #include "PrngML.h"
#endif


/*===========================================================================

                     DATA DECLARATIONS

===========================================================================*/
unsigned int __stack_chk_guard[] = {0xDEADDEAD};

/*===========================================================================

                     MODULE FUNCTIONS

===========================================================================*/

#if defined(NO_STACK_CHECK)
void __init_stack_chk_guard(void) {}
#else
void
__init_stack_chk_guard(void)
{
   unsigned int random_num = 0xDEADD00D;

   /* Get random number */
   if (PRNGML_ERROR_NONE != PrngML_getdata((uint8*)&random_num, (uint16) sizeof(random_num)))
   {
      ERR_FATAL( "Stack protect cannot get random number", 0, 0, 0 );
   }

   *__stack_chk_guard = random_num;

  return;
}
#endif

void
__stack_chk_fail(void)
{
    ERR_FATAL( "Stack Check Failed", 0, 0, 0 );
}


