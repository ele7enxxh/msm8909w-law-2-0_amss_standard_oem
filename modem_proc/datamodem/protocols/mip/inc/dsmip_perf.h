#ifndef DSMIP_PERF_H
#define DSMIP_PERF_H

/*===========================================================================

                           D S M I P _ P E R F . H

DESCRIPTION

  The Data Services Mobile IP call performance header file.

  Triggers for the start and end times for the various delays are outlined
  below.
  
                           
  Um PPP delay:   
  +-------------------------+               
  | ppplcp.c                |              
  |                         |-->----+       
  | lcp_init(), Um          |       |       +-------------------------+
  +-------------------------+       |       | pppipcp.c               |
                                    +---->--|                         |
  +-------------------------+       |       | ipcp_opening(), Um      |
  | dssnet.c                |       |       +-------------------------+
  |                         |-->----+        
  | PPP_RESYNC_EV (Um only) |    handoff           
  +-------------------------+       
                             
  
  Rm PPP delay:
  +-------------------------+               +-------------------------+
  | ppplcp.c                |               | pppipcp.c               |
  |                         |-->--------->--|                         |
  | lcp_init(), Rm          |               | ipcp_opening(), Rm      |
  +-------------------------+               +-------------------------+

  
  Initial Mobile IP registration:  
  +-------------------------+               +-------------------------+
  | dsmip_metasm.c          |               | dsmip_regsm.c           |
  | metasm state:           |-->--------->--| regsm state:            |
  | closed -> open          |               | init_rrq -> registered  |
  +-------------------------+               +-------------------------+
  
              
  Mobile IP re-registration:
  +-------------------------+               +-------------------------+
  | dsmip_regsm.c           |               | dsmip_regsm.c           |
  | regsm state:            |-->--------->--| regsm state:            |
  | registered -> re_reg    |               | re_reg -> registered    |
  +-------------------------+               +-------------------------+

                 
  Total call setup:  
                                            +-------------------------+
                                non-netmdl  | dsmip_regsm.c           |
                                    +---->--| regsm state:            |
  +-------------------------+       |       | init_rrq -> registered  |
  | dsmgr.c                 |       |       +-------------------------+
  |                         |-->----+
  | origination cmd cb      |       |       +-------------------------+
  +-------------------------+       |       | dsmip_rmsm.c            | 
                                    +---->--| rmsm state: -> um_rm_up |
                                  netmdl    +-------------------------+
                                            | dsiface.c               |
                                            | dsiface_up, Rm          |
                                            +-------------------------+
  Total handoff delay:                                            
  +-------------------------+               
  | dssoctl.c               |   in-traffic            
  |                         |-->----+       
  | PZID active change      |       |       +-------------------------+
  +-------------------------+       |       | dsmip_regsm.c           |
                                    +---->--| regsm state:            |
  +-------------------------+       |       | re_rrq -> registered    |
  | dssoctl.c               |       |       +-------------------------+
  |                         |-->----+        
  | PZID idle change        |    dormant           
  +-------------------------+       
             
             
EXTERNALIZED FUNCTIONS

  mip_perf_init()
    Initializes Mobile IP performance timers.

Copyright (c) 2002-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_perf.h_v   1.4   16 Nov 2002 00:34:04   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/inc/dsmip_perf.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
09/04/09    ss     CMI SU level modifications.
12/07/06    as     Added PERF_MIP_DEREG_DELAY for MIP deregistration.
11/10/03    igt    Added PERF_CANCEL_TS to be able to reset timers for specific
                   delays.
02/20/02    sjy    Updated delay descriptions to reflect changes made in the
                   performance metrics.
02/19/02    sjy    Updated delay descriptions to reflect changes made in the
                   performance metrics. 
01/24/02    sjy    Created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "qw.h"
#include "dsmip_api.h"

/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
  FUNCTION MIP_PERF_INIT

  DESCRIPTION
    This function initializes the Mobile IP performance info block.
      
  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void mip_perf_init
(
  void
);

#endif /* DSMIP_PERF_H */
