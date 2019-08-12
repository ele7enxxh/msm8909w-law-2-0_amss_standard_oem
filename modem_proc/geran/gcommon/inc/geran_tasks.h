#ifndef GERAN_TASKS_H
#define GERAN_TASKS_H
/*===========================================================================

          G S M   T A S K   F U N C T I O N S   H E A D E R   F I L E

GENERAL DESCRIPTION
  This file contains declarations for all the tasks in GERAN.

Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
===========================================================================*/

/* <EJECT>^L */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/inc/geran_tasks.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
14/10/13   pa      CR536820:Added GL1 Msgr IF Task.
09-07-26   tjw     Initial revision.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "rex.h"
#include "geran_dual_sim.h"

/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed to interface with the QDSP Services module.

===========================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/* These IDs are used by the task data access functions (geran_tcb_read etc.) */

typedef enum {
  GERAN_TASK_GRP_GL1,
  GERAN_TASK_GRP_GL2,
  GERAN_TASK_GRP_GLLC,
  GERAN_TASK_GRP_GMAC,
  GERAN_TASK_GRP_GRLC_DL,
  GERAN_TASK_GRP_GRLC_UL,
  GERAN_TASK_GRP_GRR,
  GERAN_TASK_GRP_GPLT,
  GERAN_TASK_GRP_GL1_TCXO_TIMER,
  GERAN_TASK_GRP_GL1_MSGR_IF,
  NUM_GERAN_TASK_GRPS
} geran_task_grp_t;


typedef enum {
  GERAN_TASK_ID_GL1,
  GERAN_TASK_ID_GL1_2,
  GERAN_TASK_ID_GL1_3,
  GERAN_TASK_ID_GL2,
  _GERAN_TASK_ID_GL2, /* Dummy entries for tasks with a single instance */
  __GERAN_TASK_ID_GL2,
  GERAN_TASK_ID_GLLC_1,
  GERAN_TASK_ID_GLLC_2,
  GERAN_TASK_ID_GLLC_3,
  GERAN_TASK_ID_GMAC,
  GERAN_TASK_ID_GMAC2,
  GERAN_TASK_ID_GMAC3,
  GERAN_TASK_ID_GRLC_DL,
  GERAN_TASK_ID_GRLC_DL_2,
  GERAN_TASK_ID_GRLC_DL_3,
  GERAN_TASK_ID_GRLC_UL,
  GERAN_TASK_ID_GRLC_UL_2,
  GERAN_TASK_ID_GRLC_UL_3,
  GERAN_TASK_ID_GRR,
  _GERAN_TASK_ID_GRR,
  __GERAN_TASK_ID_GRR,
  GERAN_TASK_ID_GPLT,
  _GERAN_TASK_ID_GPLT,
  __GERAN_TASK_ID_GPLT,
  GERAN_TASK_ID_GL1_TCXO_TIMER,
  _GERAN_TASK_ID_GL1_TCXO_TIMER,
  __GERAN_TASK_ID_GL1_TCXO_TIMER,
  GERAN_TASK_ID_GL1_MSGR_IF_1,
  GERAN_TASK_ID_GL1_MSGR_IF_2,
  GERAN_TASK_ID_GL1_MSGR_IF_3,
  NUM_GERAN_TASK_IDS
} geran_task_id_t;

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*==========================================================================

  FUNCTION    geran_info_initialise

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
void geran_info_initialise(void);

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
void geran_tcb_set(const geran_task_id_t task_id, const rex_tcb_type *tcb);

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
rex_tcb_type* geran_tcb_read(const geran_task_id_t task_id);

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
void geran_task_info_initialise(void);

#endif /* GERAN_TASKS_H */
