#ifndef LPA_TIMER_H
#define LPA_TIMER_H
/*===========================================================================


            L P A   T I M E R   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/src/lpa_timer.h#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/14/16   ar      Introduce DS http post retry timer logic
04/11/16   av      Initial revision
===========================================================================*/

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

/* ----------------------------------------------------------------------------
   ENUM:            LPA_TIMER_ENUM_TYPE

   DESCRIPTION: This enum indicates the type of LPA timer
-------------------------------------------------------------------------------*/
typedef enum {
  LPA_TIMER_TYPE_SANITY_TIMER,
  LPA_TIMER_TYPE_GET_BPP_RETRY_TIMER,
  LPA_TIMER_TYPE_HANDLE_INSTALLATION_RESULT_RETRY_TIMER
} lpa_timer_enum_type;


/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION LPA_TIMER_CLEANUP

DESCRIPTION
  Undefines the LPA sanity timer.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_timer_cleanup (
  lpa_slot_id_enum_type           slot_id
);

/*===========================================================================
FUNCTION LPA_TIMER_CLEAR

DESCRIPTION
  Function clears the timer that is passed in as argument.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_timer_clear (
  lpa_slot_id_enum_type          slot_id,
  lpa_timer_enum_type            timer_type
);

/*===========================================================================
FUNCTION LPA_TIMER_SET

DESCRIPTION
  Function sets the timer that is passed in as argument.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_timer_set (
  lpa_slot_id_enum_type          slot_id,
  lpa_timer_enum_type            timer_type
);

/*===========================================================================
FUNCTION LPA_TIMER_GET

DESCRIPTION
  Function gets the current countdown value of the timer, that is passed in
  as argument, in milliseconds.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_timer_get (
  lpa_slot_id_enum_type          slot_id,
  lpa_timer_enum_type            timer_type,
  uint32                         *timer_cnt_ptr
);

/*===========================================================================
FUNCTION LPA_TIMER_HANDLE_SANITY_TIMER_EXPIRE_SIG

DESCRIPTION
  This function, called in lpa_task, is called to process sanity timer
  expiration signal.

DEPENDENCIES
  lpa_task must have finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_timer_handle_sanity_timer_expire_sig (
  lpa_slot_id_enum_type               slot_id
);

/*===========================================================================
FUNCTION LPA_TIMER_INIT

DESCRIPTION
  Function initializes and defines the LPA sanity timer.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_timer_init (
  void
);

#endif /* LPA_TIMER_H */

