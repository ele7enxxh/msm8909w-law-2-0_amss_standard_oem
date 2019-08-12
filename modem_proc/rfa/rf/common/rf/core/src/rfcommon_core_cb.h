#ifndef RF_COMMON_CORE_CB_H
#define RF_COMMON_CORE_CB_H
/*!
  @file
  rfcommon_core_cb.h

  @brief

  @details
  This file contains all declarations and definitions necessary to use
  the call back services.

*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/src/rfcommon_core_cb.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/13/09   ckl     Modifications for SCMM.
07/01/02   sd      Initial revision.
===========================================================================*/

#include "rfcom.h"
#include "queue.h"

/*!
  @brief
  Maximum number of call back events currently supported.
 */
#define RFCOMMON_CORE_CB_MAX_NUM_EVENTS       1

/*!
  @brief
  Type definition of internal header for event generation structure.
 */
typedef struct
{
  /* Queue link */
  q_link_type q_link;

  /* Pointer for processing internal to this module only */
  void *handler_ptr;

} rfcommon_core_cb_hdr_struct_type;

/*!
  @brief
  Type definition of a single event to generate when processing of a
  particular RF function.
 */
typedef struct
{
  /* Event */
  rfcom_cb_event_enum_type event;

  /* Time to wait in microseconds from previos event before event is
     generated. If this is the first event in the array, this is the
     delay from when the event is registered until the event generated. */
  uint32 delay;

} rfcommon_core_cb_event_struct_type;

/*!
  @brief
  Type definition of the events to generate during processing of a
  particular RF function.  The call back handler is called once for each
  event generated.
 */
typedef struct
{
  /* Buffer header -  The contents of this header should not be
     modified by the caller. */
  rfcommon_core_cb_hdr_struct_type hdr;

  /* Call back handler to call for each event. */
  rfcom_cb_handler_type cb_handler;

  /* Pointer to user data that is passed to call back handler. */
  const void *user_data_ptr;

  /* Number of events to generate.  Defines number of events specified in
     following array of events. */
  int num_events;

  /* Array of events to generate with the num_events entries initialized. */
  rfcommon_core_cb_event_struct_type cb_events[RFCOMMON_CORE_CB_MAX_NUM_EVENTS];

} rfcommon_core_cb_struct_type;

void rfcommon_core_cb_init( void );

rfcommon_core_cb_struct_type* rfcommon_core_cb_get_event_buffer( void );

void rfcommon_core_cb_register_events
(
  rfcommon_core_cb_struct_type *cb_event_ptr
);

#endif /* RF_COMMON_CORE_CB_H */
