
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 H D R    B U F F E R    S E R V I C E S

GENERAL DESCRIPTION
  This file contains common functions used by tasks when queueing messages, 
  commands, and indications for the protocols that run in their context.

EXTERNALIZED FUNCTIONS

  hdrbuf_done() - Called for each buffer when it is no longer being used

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2005 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MSM4500/vcs/hdrbuf.c_v   1.5   18 Sep 2000 10:35:18   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/src/hdrbuf.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/24/05   pba     Lint cleanup
09/10/03   mpa     Converted F3 msg to use new MSG2.0 SSID
02/02/00   dna     Created module.

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "queue.h"
#include "hdrhai.h"
#include "hdrdebug.h"
#include "err.h"
#include "hdrbuf.h"

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBUF_DONE

DESCRIPTION
  This function returns the buffer to its free queue, if necessary.

DEPENDENCIES
  None
  
PARAMETERS
  hdr_ptr - pointer to the header of the buffer to be returned

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrbuf_done( hdrbuf_hdr_type * hdr_ptr )
{
  HDR_MSG_PROT_1 (MSG_LEGACY_LOW,  "hdrbuf_done %d", 
             ( hdr_ptr->done_q_ptr != NULL ));

  /* place buffer on done queue if requested */
  if ( hdr_ptr->done_q_ptr != NULL )
  {
    /* place command on requested queue */
    q_put( hdr_ptr->done_q_ptr, &hdr_ptr->link );
  }
}              /* hdrbuf_done */

