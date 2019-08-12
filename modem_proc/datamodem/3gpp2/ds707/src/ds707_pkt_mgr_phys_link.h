#ifndef DS707_PKT_MGR_PHYS_LINK_H
#define DS707_PKT_MGR_PHYS_LINK_H

/*===========================================================================

               D S 7 0 7 _ P K T _ M G R _ P H Y S _ L I N K . H
               
GENERAL DESCRIPTION
  This file is the controller for 1X packet calls and the PS phys link.
  
EXTERNALIZED FUNCTIONS
  ds707_pkt_phsy_link_init() - Creates the 1x pkt ps_phys_link struct.
  
  ds707_pkt_phys_link_process_cmd() - Process cmd's specifc to the 1x
    ps_phys_link handling.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Must call ds707_pkt_phys_link_init() at startup.

  Copyright (c) 2002 - 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pkt_mgr.c_v   1.45   25 Feb 2003 14:25:02   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_pkt_mgr_phys_link.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/02/12    msh    Coan: Feature cleanup
03/09/09    sa     AU level CMI modifications.
01/03/06    ksu    EMPA: hook up forward link with new phys link architecture
09/08/04    vas    Added code for QOS (sec pkt mgr)
12/30/03    ak     Code created from original ds707_pkt_mgr.c.
===========================================================================*/


/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "ps_phys_link.h"
#include "dstask_v.h"
#include "ds707_pkt_mgri.h"

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKT_PHYS_LINK_PROCESS_CMD

DESCRIPTION   Processes the phys link commands.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_phys_link_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_PHYS_LINK_INIT

DESCRIPTION   Called once at mobile power-up.  Registers the appropriate
              iface unctionality with PS IFACE.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_phys_link_init
(
  void
);


/*===========================================================================
FUNCTION      DS707_SETUP_PHYS_LINK_F_PTRS

DESCRIPTION   This funtion with set up the primary and secondary 
              physical link pointers.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_setup_phys_link_f_ptrs
(
    ps_phys_link_type *phys_link_ptr,
    int index
);

#endif /* DS707_PKT_MGR_PHYS_LINK_H */

