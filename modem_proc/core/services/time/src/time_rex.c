/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements time-keeping rex specific functionality.


EXTERNALIZED FUNCTIONS
  
  
REGIONAL FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2003 - 2013 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/time_rex.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/13/09   gparikh     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/


/*-----------------------------------------------------------------------------
  Time source includes
-----------------------------------------------------------------------------*/
#include "time_dal_v.h"

/*Define all the extern functions here that will be used to initialize
 the function pointers and may be called.*/
extern void time_get_time_defines(time_defines_type*);

/*static functions go here*/
void time_init_fn_ptr(time_fn_ptr_type* time_fn_ptr);
void time_rex_call_init_fn(void);

/*=============================================================================

FUNCTION TIME_INIT_FN_PTR
 
DESCRIPTION
  This function will initialize all the function pointers for BMP OS
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_init_fn_ptr(time_fn_ptr_type* time_fn_ptr)
{
  time_fn_ptr->time_call_init_functions_fp=time_rex_call_init_fn;
}

/*=============================================================================

FUNCTION TIME_REX_CALL_INIT_FN
 
DESCRIPTION
  Calls the initializing functions
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_rex_call_init_fn
(
  void
)
{
}


