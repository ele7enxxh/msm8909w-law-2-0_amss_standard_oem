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

 $Header: //components/rel/core.mpss/3.5.c12.3/debugtools/rcinit/src/example/rcinit_ex_initfn.c#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#include "rcinit.h"

/*===========================================================================

 FUNCTION rcinit_example_initfn

 DESCRIPTION
 example task boiler plate

 DEPENDENCIES
 none

 RETURN VALUE
 operation success

 SIDE EFFECTS
 none
 ===========================================================================*/

/*static*/ unsigned long rcinit_internal_initfn_count;

void rcinit_example_initfn(void)
{
   rcinit_internal_initfn_count++;
}
