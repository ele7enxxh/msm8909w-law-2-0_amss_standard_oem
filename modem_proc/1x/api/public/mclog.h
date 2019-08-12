#ifndef MCLOG_H
#define MCLOG_H

/*===========================================================================

                  Main Control Event and Logging Functions

GENERAL DESCRIPTION
  This module contains utilities and functions related to logging of events
  and DIAG/DM packets inside MC

  Copyright (c) 2000 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mclog.h_v   1.10   02 Oct 2002 22:52:32   louiel  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/mclog.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/09   jj      Split this file as part of CMI phase2.
02/15/07   sb      Removed featurization from VOIP states.
02/14/07   sb      Added VOIP logging structure.
11/21/06   sb      Added support for VOIP-1X handoffs.
07/10/06   fc      Added log packet definition for UMTS.
06/13/06   fc      Added support for status service order.
06/02/06   fc      Added IHO/AHO/APHO event definitions.
10/15/05   fc      Cleanup NCC featurization.
07/05/05   sb      Added MEID support.
06/17/05   fc      Fixed the issue of MER counters being incremented 
                   incorrectly if multiple GPM received in the assigned slot.
                   Added the suport for MER benchmark in non-slotted mode.
03/23/05   fc      Merged the following from MSMSHARED:
03/25/05   an       Changed the DIAG SUBSYS CMD values for DF
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
02/25/04   fc      Merged the following from MSMSHARED:
02/17/05   sb       Added event to indicate a registration was suppressed.
09/27/04   va      Merged from REL B/C archive:
07/20/04   bkm      Update MER support for 6100.
07/08/04   bkm      Re-organize support for MER counters.
08/23/04   az      Rel D Negative SCI - changes for dynamic feature
08/17/04   az      Diag command MC_DIAG_SUBSYS_SET_CUR_PREF_SCI for setting
                   current preferred sci
06/24/04   sb      Release C addendum changes for PLCM.
06/16/04   fc      Added DF support for Sync ID and Reconnect message.
03/15/04   yll     Added one more byte payload in WAKEUP_SLOT event.
12/16/03   fc      Added support for EVENT_SEG_UPM_ADDR_MISMATCH.
12/02/03   bkm     Updated CH_IND/EXT_CH_IND event reporting.
11/06/03   bkm     Added diag command to retrieve Paging MER counters.
10/17/03   ph      added support for new DIAG interface to retrieve MC state.
10/03/03   bkm     Add match type to EVENT_CP_MATCHED_MSG.
08/01/03   yll     Added support for Dynamic Feature.
12/02/02   yll     Added event logging for wakeup slot, matched msg, call
                   release, pref system reselection, etc.
11/07/02   az      Mainlined FEATURE_IS95B
10/02/02   yll     Linted the code.
10/02/02   az      Mainlined FEATURE_IS2000
03/27/02   sb      Added bcch_acquired event.
03/27/02   jrp     Moved mclog_ms_handoff_type outside FEATURE_IS2000_REL_A ifdef
03/26/02   va      Added mclog_ms_handoff_type for HO event reporting.
03/14/02   va      Added mclog_event_cc_instantiated_type and mclog_event_cc
                   _conref_change_type
02/26/02   fc      Added support for common channel monitored event.
02/13/02   va      Support for CC related events
06/26/01   sb      Merged the following from common archive:
           06/06/01   lcc     Added BS P_REV, and F/R RCs for FCH, DCCH,
                      and SCH to the IS2000 status packet.
04/27/01   fc      Changed function prototype of
                   mclog_event_report_pch_acquired.
04/18/01   fc      Featurized mclog_report_event_ch_ind under FEATURE_IS2000.
04/12/01   fc      Added/Moved all events logging data types / function
                   prototypes from mc.h and mccdma.h to here.
04/10/01   jq      Created the file to handle logging and diagpkt needs.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "log.h"
#include "diagpkt.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/*===========================================================================

FUNCTION MCLOG_STAT

DESCRIPTION
  This procedure processes a Status request and formats a Status
  Response type.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/

PACKED void * mclog_stat 
(
  PACKED void *req_pkt,      /* pointer to request packet  */
  word len                   /* length of request packet   */
);

/*===========================================================================
FUNCTION MCLOG_MARKOV

DESCRIPTION
  This returns Markov mode statistics.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void * mclog_markov 
(
  PACKED void *req_pkt,      /* pointer to request packet  */
  word len                   /* length of request packet   */
);

/*===========================================================================
FUNCTION MCLOG_MARKOV_RESET

DESCRIPTION
  This procedure resets the markov statistics.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void * mclog_markov_reset 
(
  PACKED void *req_pkt,      /* pointer to request packet  */
  word len                   /* length of request packet   */
);

/*===========================================================================

FUNCTION mclog_get_is2k_status

  This function sends IS2000 related channel information to DM via diag service

DEPENDENCIES
  None

RETURN VALUE
  The next state.

SIDE EFFECTS
  None

===========================================================================*/

PACKED void * mclog_get_is2k_status
(
  PACKED void *req_pkt,        /* pointer to request packet  */
  word len                     /* length of request packet   */
);

#endif  /* MCLOG_H */
