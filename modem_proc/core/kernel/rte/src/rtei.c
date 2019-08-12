/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 R U N   T I M E   E N V I R O N M E N T
                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/rte/src/rtei.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
18-May-04  rwh     Changed FEATURE_REX_IPC to FEATURE_RTE_PROCESS
10-May-04  rwh     Feature-ized with FEATURE_REX_IPC. 
                   Included tsvc.h
03/10/13   et      Created module.

===========================================================================*/


/*===========================================================================
   INCLUDES AND VARIABLE DEFINITIONS
===========================================================================*/

/*-------------------------------------------------------------------
   Include Files
-------------------------------------------------------------------*/
#include "comdef.h" /*lint -e766 needed to remove compiler warning*/

#ifdef FEATURE_RTE_PROCESS

#include "rex.h"
#include "err.h"

#include "tsvc.h"

/*-------------------------------------------------------------------
   Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
   Type Declarations
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
   Global Constant Declarations
-------------------------------------------------------------------*/



/*===========================================================================
   FUNCTION DEFINITIONS
===========================================================================*/

#ifndef FEATURE_USE_TIME_VU

/*===========================================================================
FUNCTION RTE_TICK()

DESCRIPTION                                                                */
   /** Create a new process.\\

@param name              Name of the process
@param priority          Priority of the thread in the enclosing process
@param entry_point       Function that represent the thread
@param stack_size        Size of the stack for this thread
@param private_pool_size Size of the process private message pool

@return {\bf The id of the created process. 0 if error.}
*/
/*=========================================================================*/
void rte_tick
(
   /* number of rex ticks to decrement everything by */
   rex_timer_cnt_type p_ticks
)
{
   rex_tick(p_ticks);

   do
   {
      tsvc_tick(p_ticks);
      p_ticks -= 5;
   }while(p_ticks!=0);
}

#endif /* FEATURE_USE_TIME_VU */

#endif /* FEATURE_RTE_PROCESS */

