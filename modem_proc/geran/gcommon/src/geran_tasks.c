/*==============================================================================

                  G E R A N   T A S K   I N F O R M A T I O N

GENERAL DESCRIPTION

      This module contains the GERAN task infomation such as TCBs used
      within GERAN (but no longer by GERAN clients).

EXTERNALIZED FUNCTIONS

      geran_tcb_read()
      geran_task_info_initialise()
      geran_tcb_set()


Copyright (c) 2012-2014 Qualcomm Technologies, Inc.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/src/geran_tasks.c#1 $

when       who    what, where, why
--------   ---    --------------------------------------------------------------
2014-08-12 tjw    CR 707726 - removed mutexes.
                  Add geran_task_info_initialise().
2012-04-24 tjw    New file
==============================================================================*/

/*==============================================================================

                      INCLUDE FILES FOR MODULE

==============================================================================*/
#include "customer.h"
#include "comdef.h"
#include "rex.h"
#include "geran_tasks.h"
#include "geran_msgs.h"
#include "err.h"

/*==============================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/
typedef struct
{
   boolean initialised;
   rex_tcb_type* tcb;
} geran_task_info_t;

/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/
static geran_task_info_t geran_task_info[NUM_GERAN_TASK_IDS];

/*==============================================================================

                    FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/*==========================================================================

  FUNCTION    geran_tcb_read

  DESCRIPTION

    This function provides the tcb for the specified GERAN task

  PARAMETERS

    geran_task_id_t  task_id

  DEPENDENCIES
    None.

  RETURN VALUE
    rex_tcb_type *tcb.

  SIDE EFFECTS
    None.

===========================================================================*/
rex_tcb_type* geran_tcb_read(const geran_task_id_t task_id)
{
  rex_tcb_type *result = NULL;

  if (task_id < NUM_GERAN_TASK_IDS)
  {
    if (geran_task_info[task_id].initialised)
    {
      result = geran_task_info[task_id].tcb;
    }
    if (result == NULL)
    {
#ifdef PERLUTF
     #error code not present
#else
     ERR_FATAL("geran_task_info %d not initialised", task_id, 0, 0);
#endif /*PERLUTF*/
    }
  }
  else
  {
    ERR_FATAL("geran_task_info %d out of range", task_id, 0, 0);
  }
  return result;
}

/*==========================================================================

  FUNCTION    geran_tcb_set

  DESCRIPTION

    This function sets the tcb for the specified GERAN task

  PARAMETERS

    geran_task_id_t  task_id
    rex_tcb_type *tcb.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
void geran_tcb_set(const geran_task_id_t task_id, const rex_tcb_type *tcb)
{
  if (task_id < NUM_GERAN_TASK_IDS)
  {
    geran_task_info[task_id].tcb = (rex_tcb_type *)tcb; /* (cast avoids compiler warning) */
    geran_task_info[task_id].initialised = TRUE;
  }
  else
  {
    ERR_FATAL("geran_task_info %d out of range", task_id, 0, 0);
  }
}

/*==========================================================================

  FUNCTION    geran_task_info_initialise

  DESCRIPTION

    This function initialises the tcb table for all GERAN tasks

  PARAMETERS

    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
void geran_task_info_initialise(void)
{
  int32 task_id;

  for (task_id = 0; task_id < NUM_GERAN_TASK_IDS; task_id++)
  {
    geran_task_info[task_id].initialised = FALSE;
  }
}
