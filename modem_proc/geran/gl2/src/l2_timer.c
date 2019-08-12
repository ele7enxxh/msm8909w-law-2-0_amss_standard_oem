/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          L 2   T I M E R

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl2/src/l2_timer.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who       what, where, why
--------   ---       ---------------------------------------------------------
14/09/12   sjw       Created
dd/mm/yy   xxx       Format
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "environ.h"
#include "ms.h"
#include "sys_type.h"
#include "sys_stru.h"
#include "sys_cnst.h"
#include "timers.h"
#include "gs.h"
#include "ms_timer.h"
#include "rr_l2_l1.h"
#include "l2i.h"
#include "l2_timer.h"
#include "geran_multi_sim.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

static rex_crit_sect_type gl2_timer_critical_section = {{0}};

#define GL2_TIMER_LOCK()   rex_enter_crit_sect(&gl2_timer_critical_section)
#define GL2_TIMER_UNLOCK() rex_leave_crit_sect(&gl2_timer_critical_section)

typedef struct
{
  l2_store_T       *l2_data_ptr;
  SAPI_T            sapi;
  l2_channel_type_T channel;
  byte              sequence;
  byte              reserved;
} l2_timer_T;

/* Each data space needs at least enough timer parameter storage */
/* for 1 x T200 and 2 x Tack timers for each of SAPI0 and SAPI3  */
#define MAX_GL2_TIMER_INDEX (NUM_GERAN_DATA_SPACES << 3)

static l2_timer_T l2_timers[MAX_GL2_TIMER_INDEX];

static void l2_timer_reset_entry(byte timer_index)
{
  l2_timers[timer_index].l2_data_ptr = NULL;
  l2_timers[timer_index].sapi        = 0;
  l2_timers[timer_index].channel     = DCCH;
  l2_timers[timer_index].sequence    = 0;
  l2_timers[timer_index].reserved    = 0xCB;

  return;
}

void l2_timer_init(void)
{
  byte i;

  rex_init_crit_sect(&gl2_timer_critical_section);

  for (i = 0; i < MAX_GL2_TIMER_INDEX; i++)
  {
    l2_timer_reset_entry(i);
  }

  return;
}

byte l2_timer_set_params(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel, byte seq_num)
{
  byte i;

  GL2_TIMER_LOCK();

  for (i = 0; i < MAX_GL2_TIMER_INDEX; i++)
  {
    if (l2_timers[i].l2_data_ptr == NULL)
    {
      /* this entry is unused, define it now */
      l2_timers[i].l2_data_ptr = l2_data_ptr;
      l2_timers[i].sapi        = sapi;
      l2_timers[i].channel     = channel;
      l2_timers[i].sequence    = seq_num;
      break;
    }
  }

  if (MAX_GL2_TIMER_INDEX == i)
  {
    /* end of loop reached with no free entry found */
    i = BAD_GL2_TIMER_INDEX;
  }

  GL2_TIMER_UNLOCK();

  return(i);
}

l2_store_T *l2_timer_get_params(byte timer_index, SAPI_T *sapi_ptr, l2_channel_type_T *chan_ptr, byte *seq_num_ptr)
{
  l2_store_T *l2_data_ptr;

  GL2_TIMER_LOCK();

  if (timer_index < MAX_GL2_TIMER_INDEX)
  {
    l2_data_ptr = l2_timers[timer_index].l2_data_ptr;

    if (sapi_ptr != NULL)
    {
      *sapi_ptr = l2_timers[timer_index].sapi;
    }

    if (chan_ptr != NULL)
    {
      *chan_ptr = l2_timers[timer_index].channel;
    }

    if (seq_num_ptr != NULL)
    {
      *seq_num_ptr = l2_timers[timer_index].sequence;
    }
  }
  else
  {
    l2_data_ptr = NULL;
  }

  GL2_TIMER_UNLOCK();

  return(l2_data_ptr);
}

void l2_timer_clr_params(byte timer_index)
{
  GL2_TIMER_LOCK();

  if (timer_index < MAX_GL2_TIMER_INDEX)
  {
    l2_timer_reset_entry(timer_index);
  }

  GL2_TIMER_UNLOCK();

  return;
}

