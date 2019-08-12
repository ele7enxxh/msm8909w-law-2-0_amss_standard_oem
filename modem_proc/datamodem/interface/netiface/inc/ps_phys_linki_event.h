#ifndef PS_PHYS_LINKI_EVENT_H
#define PS_PHYS_LINKI_EVENT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              P S _ P H Y S _ L I N K I _ E V E N T S . H

GENERAL DESCRIPTION
  This is the implementation of the PS phys link event functions. This
  file implement functions used for phys_link events.

EXTERNAL FUNCTIONS

  PS_PHYS_LINK_EVENT_INIT()
    Called at power-up to initialize event arrays, etc.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Must call ps_phys_link_event_init() at startup.

Copyright (c) 2002 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2004 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_phys_linki_event.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/22/06    msr    Using single critical section.
02/06/06    msr    Updated for L4 tasklock/crit sections.
12/29/03    ak     created file.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "ps_iface_defs.h"
#include "ps_phys_link.h"
#include "queue.h"

/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  GLOBAL_PHYS_LINK_EVENT_Q_ARRAY - this is used to store callbacks that are
    intended to be called when events happen on ANY phys_link.
---------------------------------------------------------------------------*/
extern q_type  global_phys_link_event_q_array[PHYS_LINK_MAX_EV];

/*===========================================================================
TYPEDEF PS_PHYS_LINKI_EVENT_BUF_TYPE

DESCRIPTION
  This is the type that is placed on the phys link event callback queues.  It
  is only used internally and so should not be referenced outside of this
  file.
===========================================================================*/
typedef struct
{
  q_link_type                     link;
  ps_iface_event_enum_type        event_name;
  q_type                         *event_q_ptr;
  ps_phys_link_event_cback_type   event_cback_f_ptr;
  void                           *user_data_ptr;
} ps_phys_link_event_buf_type;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                      PUBLIC FUNCTION DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION PS_PHYS_LINK_EVENT_INIT()

DESCRIPTION
  This function initializes the global queues for ps_phys_link.  It also
  registers the phys_link function on the global phys_link queue.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_event_init
(
  void
);

/*===========================================================================
FUNCTION PS_PHYS_LINKI_GET_EVENT_Q_ARRAY_INDEX()

DESCRIPTION
  This function returns the event queue number for given event. 
 
PARAMETERS
  event      : phys link event
 
RETURN VALUE
  Returns the queue number

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_phys_linki_get_event_q_array_index
(
  ps_iface_event_enum_type event
);
#endif /* PS_PHYS_LINKI_EVENT_H */
