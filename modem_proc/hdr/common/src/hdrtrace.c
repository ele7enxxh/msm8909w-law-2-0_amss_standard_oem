/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         H D R   S O F T W A R E   T R A C E   S E R V I C E S

GENERAL DESCRIPTION
  This module contains functions for logging trace messages to the QXDM.
  It also contains arrays of strings used for off-target testing, to
  allow trace events to be output as printfs.

EXTERNALIZED FUNCTIONS (Global)
  hdrtrace_report0() - Log trace event with no payload
  hdrtrace_report1() - Log trace event with one payload item
  hdrtrace_report2() - Log trace event with two payload items
  hdrtrace_report3() - Log trace event with three payload items
  hdrtrace_report4() - Log trace event with four payload items

EXTERNALIZED FUNCTIONS (Regional)

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrtrace.c_v   1.22   05 Sep 2001 18:10:56   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/src/hdrtrace.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/12/14   sat     Fixing Klockwork issues
12/19/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
03/05/07   sju     Added support to handle the same message ID with different 
                   subtype. 
06/22/06   mpa     Mainlined FEATURE_DIAG_TRACE and FEATURE_HDR_DEBUG_TRACE.
04/14/05   mpa     Added BCMCS trace events
04/04/05   pba     Removed Lint errors
03/07/05   pba     Updated command, message, state and indication table for 
                   off-target testing
06/06/01   dna     use memcpy, not b_pack to fill log buffer, to make byte
                   ordering consistent with what QXDM expects
05/24/01   sh      added stream protocol related indications
05/17/01   sh      added hdrtrace_report5
03/10/01   sh      Added Idle and Session related indication names
11/16/00   dna     Created module

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "trace.h"
#include "hdrtrace.h"
#include "bit.h"

#ifdef FEATURE_MEMCPY_REMOVAL
#include "stringl.h"
#endif /* FEATURE_MEMCPY_REMOVAL */

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION HDRTRACE_REPORT0

DESCRIPTION
  This function reports a trace event to DIAG.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrtrace_report0( trace_id_enum_type event_id )
{
  trace( event_id );
}

/* <EJECT> */
/*===========================================================================

FUNCTION HDRTRACE_REPORT1

DESCRIPTION
  This function reports a trace event to DIAG.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrtrace_report1(
  trace_id_enum_type event_id,
  uint32 item1,
  uint8 item1_size
)
{
  trace_payload( event_id, item1_size, &item1 );
}

/* <EJECT> */
/*===========================================================================

FUNCTION HDRTRACE_REPORT2

DESCRIPTION
  This function reports a trace event to DIAG.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrtrace_report2(
  trace_id_enum_type event_id,
  uint32 item1,
  uint8 item1_size,
  uint32 item2,
  uint8 item2_size
)
{
  uint8 data[8];
  uint8 size = 0;
#ifdef FEATURE_MEMCPY_REMOVAL
  uint8 size_copied = 0;
#endif /* FEATURE_MEMCPY_REMOVAL */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MEMCPY_REMOVAL
  size_copied = memscpy(data, sizeof(data), &item1, MIN( sizeof(item1), item1_size));
  size = size_copied;
  size_copied = memscpy(data + size, (sizeof(data)-size*sizeof(data[0])), 
                        &item2, MIN( sizeof(item2), item2_size));
  size += size_copied;
#else
  memcpy(data, &item1, item1_size);
  size = item1_size;
  memcpy(data + size, &item2, item2_size);
  size += item2_size;
#endif /* FEATURE_MEMCPY_REMOVAL */
  trace_payload( event_id, size, data );
}

/* <EJECT> */
/*===========================================================================

FUNCTION HDRTRACE_REPORT3

DESCRIPTION
  This function reports a trace event to DIAG.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrtrace_report3(
  trace_id_enum_type event_id,
  uint32 item1,
  uint8 item1_size,
  uint32 item2,
  uint8 item2_size,
  uint32 item3,
  uint8 item3_size
)
{
  uint8 data[12];
  uint8 size = 0;
#ifdef FEATURE_MEMCPY_REMOVAL
  uint8 size_copied = 0;
#endif /* FEATURE_MEMCPY_REMOVAL */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MEMCPY_REMOVAL
  size_copied = memscpy(data, sizeof(data), &item1, MIN( sizeof(item1), item1_size));
  size = size_copied;
  size_copied = memscpy(data + size, (sizeof(data)-size*sizeof(data[0])), 
                        &item2, MIN( sizeof(item2), item2_size));
  size += size_copied;
  size_copied = memscpy(data + size, (sizeof(data)-size*sizeof(data[0])), 
                        &item3, MIN( sizeof(item3), item3_size));
  size += size_copied;
#else
  memcpy(data, &item1, item1_size);
  size = item1_size;
  memcpy(data + size, &item2, item2_size);
  size += item2_size;
  memcpy(data + size, &item3, item3_size);
  size += item3_size;
#endif /* FEATURE_MEMCPY_REMOVAL */
  trace_payload( event_id, size, data );
}

/* <EJECT> */
/*===========================================================================

FUNCTION HDRTRACE_REPORT4

DESCRIPTION
  This function reports a trace event to DIAG.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrtrace_report4(
  trace_id_enum_type event_id,
  uint32 item1,
  uint8 item1_size,
  uint32 item2,
  uint8 item2_size,
  uint32 item3,
  uint8 item3_size,
  uint32 item4,
  uint8 item4_size
)
{
  uint8 data[16];
  uint8 size = 0;
#ifdef FEATURE_MEMCPY_REMOVAL
  uint8 size_copied = 0;
#endif /* FEATURE_MEMCPY_REMOVAL */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MEMCPY_REMOVAL
  size_copied = memscpy(data, sizeof(data), &item1, MIN( sizeof(item1), item1_size));
  size = size_copied;
  size_copied = memscpy(data + size, (sizeof(data)-size*sizeof(data[0])), 
                        &item2, MIN( sizeof(item2), item2_size));
  size += size_copied;
  size_copied = memscpy(data + size, (sizeof(data)-size*sizeof(data[0])), 
                        &item3, MIN( sizeof(item3), item3_size));
  size += size_copied;
  size_copied = memscpy(data + size, (sizeof(data)-size*sizeof(data[0])), 
                        &item4, MIN( sizeof(item4), item4_size));
  size += size_copied;
#else
  memcpy(data, &item1, item1_size);
  size = item1_size;
  memcpy(data + size, &item2, item2_size);
  size += item2_size;
  memcpy(data + size, &item3, item3_size);
  size += item3_size;
  memcpy(data + size, &item4, item4_size);
  size += item4_size;
#endif /* FEATURE_MEMCPY_REMOVAL */
  trace_payload( event_id, size, data );
}

/* <EJECT> */
/*===========================================================================

FUNCTION HDRTRACE_REPORT5

DESCRIPTION
  This function reports a trace event to DIAG.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrtrace_report5(
  trace_id_enum_type event_id,
  uint32 item1,
  uint8 item1_size,
  uint32 item2,
  uint8 item2_size,
  uint32 item3,
  uint8 item3_size,
  uint32 item4,
  uint8 item4_size,
  uint32 item5,
  uint8 item5_size

)
{
  uint8 data[20];
  uint8 size = 0;
#ifdef FEATURE_MEMCPY_REMOVAL
  uint8 size_copied = 0;
#endif /* FEATURE_MEMCPY_REMOVAL */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MEMCPY_REMOVAL
  size_copied = memscpy(data, sizeof(data), &item1, MIN( sizeof(item1), item1_size));
  size = size_copied;
  size_copied = memscpy(data + size, (sizeof(data)-size*sizeof(data[0])), 
                        &item2, MIN( sizeof(item2), item2_size));
  size += size_copied;
  size_copied = memscpy(data + size, (sizeof(data)-size*sizeof(data[0])), 
                        &item3, MIN( sizeof(item3), item3_size));
  size += size_copied;
  size_copied = memscpy(data + size, (sizeof(data)-size*sizeof(data[0])), 
                        &item4, MIN( sizeof(item4), item4_size));
  size += size_copied;
  size_copied = memscpy(data + size, (sizeof(data)-size*sizeof(data[0])), 
                        &item5, MIN( sizeof(item5), item5_size));
  size += size_copied;
#else
  memcpy(data, &item1, item1_size);
  size = item1_size;
  memcpy(data + size, &item2, item2_size);
  size += item2_size;
  memcpy(data + size, &item3, item3_size);
  size += item3_size;
  memcpy(data + size, &item4, item4_size);
  size += item4_size;
  memcpy(data + size, &item5, item5_size);
  size += item5_size;
#endif /* FEATURE_MEMCPY_REMOVAL */  
  
  trace_payload( event_id, size, data );
}

/* <EJECT> */
/*===========================================================================

FUNCTION HDRTRACE_GET_SUBTYPE

DESCRIPTION
  This function select the subtype value based on application subtype.
  Selected subtype value is only used to generate qxdm event id. 

DEPENDENCIES
  None

PARAMETERS
  app_subtype - Application subtype

RETURN VALUE
  subtype    -  subtype value used to generate event id

SIDE EFFECTS
  None

===========================================================================*/

uint8 hdrtrace_get_subtype( hdrhai_app_subtype_enum_type app_subtype )
{

  uint8 subtype;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  switch (app_subtype)
  {
    /* Separate DPA from MPA/EMPA/MMPA because:
     * With the same Message IDs, some message names are different between
     * DPA and MPA/EMPA/MMPA (e.g. Reset vs ResetTxIndication) 
     */
    case HDRHAI_DEF_PACKET_APP_AN:
    case HDRHAI_DEF_PACKET_APP_SN: 
      subtype = HDRTRACE_APP_DPA;
      break;

    /* Message IDs/Names used by MPA, EMPA and MMP are the same*/
    case HDRHAI_MULTIFLOW_PACKET_APP_AN:
    case HDRHAI_MULTIFLOW_PACKET_APP_SN:
    case HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_AN:
    case HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN:
    case HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_AN:
    case HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN:
      subtype = HDRTRACE_APP_MPA_EMPA_MMPA;
      break;

    default:
      /* Other than DPA, MPA, EMPA, MMPA (for example, CSNA, Test App, etc),
       * real application subtype will be used to generate message event id. 
       */
      subtype = app_subtype;
      break;
  }

  return subtype;
}
