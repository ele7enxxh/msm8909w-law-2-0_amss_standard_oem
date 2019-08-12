#ifndef TRACE_H
#define TRACE_H

/*===========================================================================

                 Trace Service Header File

General Description
  Trace is a temporary service that uses the event reporting mechanism to 
  information that is useful in tracing software in debug.  It is also used
  as a way to try out event reporting before officially defining the events.

Copyright (c) 2000-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                          Edit History 

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/api/trace.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/11   smd     SU API cleanup.
07/27/11   cnx     Merged Optimized Handoff changes.
05/31/11   cnx     Added TRACE_HDR_IDLE_CONNECTION_DENIED_REASON,
                   TRACE_HDR_IDLE_CONNECTION_SETUP_FAIL_REASON,
                   and TRACE_HDR_IDLE_CONNECT_REASON.
06/09/09   smd     Removed static in front of INLINE.
12/10/08   smd     Replaced assert.h with amssassert.h
11/07/08   smd     Changed "INLINE" to "static INLINE" for off-target build.
10/23/08   wsh     Fixed critical/high lint warnings
01/08/07   yll     Added support for CSNA.
06/22/06   mpa     Mainlined FEATURE_DIAG_TRACE.
04/14/05   mpa     Added BCMCS trace events
09/07/01   sh      renamed keep alive related enums
07/09/01   nmn     added SMPKA traces
05/17/01   sh      changed EVENT_HDR to TRACE_HDR and promote some
                   trace events to become HDR events in QXDM
04/19/01   dna     Moved into MSM archive as rev 1.1
04/15/01   vas     Added EVENT_HDR_OVHD_IGNORED_AP_MSG_UNEXPECTED_LINK,
                   EVENT_HDR_OVHD_INFO_UPDATED
03/28/01   vas     Added EVENT_HDR_OVHD_GET_SUBNET
11/16/00   lad     Created file.

===========================================================================*/

#include "hdr_variation.h"
#include "customer.h"   /* Customer specific definitions.   */
#include "comdef.h"     /* Definitions for byte, word, etc. */

#include "amssassert.h"
#include "event.h"
#include "hdrcom_api.h"

#ifdef FEATURE_DIAG_NO_EVENTS
#error Trace needs event reporting services!
#endif


/* Trace uses the event reporting system, so we use macros 
** to convert to the event API. 
*/
INLINE void trace( trace_id_enum_type id ) 
{ 
  event_report((event_id_enum_type) id);
}

INLINE void trace_payload ( 
  trace_id_enum_type id, 
  uint8 length, 
  void *data_ptr 
) 
{
  event_report_payload( (event_id_enum_type) id, length, data_ptr );
}

#endif /* TRACE_H */

