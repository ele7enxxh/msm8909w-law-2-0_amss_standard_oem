
#ifndef DSHDR_CORD_MGR_H
#define DSHDR_CORD_MGR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                               C O R D

GENERAL DESCRIPTION
  This file contains functions used to handle FEATURE_DS_CORD changes.

EXTERNALIZED FUNCTIONS
  DSHDR_PKT_CORD_REORIG_AFTER_PHYS_LINK_DOWN
    Tries to reorigante phys_link immediately after going dormant

  DSHDR_PKT_CORD_ORIG_HDLR
    Tries to origante phys_link on receipt of incoming DoS packet
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  NONE

Copyright (c) 2006 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/dshdr_cord_mgr.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
09/09/05   ssr     New file: contains prototypes for functions that implement 
                   FEATURE_DS_CORD. This feature provides for traffic channel
                   origination on receiving incoming DoS messages. Note that 
                   this feature is valid only for EV-DO.
===========================================================================*/

/*===========================================================================
                       INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_HDR_DOS
#ifdef FEATURE_DS_CORD
/*===========================================================================
                                 EXTERNAL FUNCTION DEFINIZTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DSHDR_PKT_CORD_REORIG_AFTER_PHYS_LINK_DOWN

DESCRIPTION   This function attempts a reorig after the physlink goes down

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
extern void dshdr_pkt_cord_reorig_after_phys_link_down (void);

/*===========================================================================
FUNCTION      DSHDR_PKT_CORD_ORIG_HDLR

DESCRIPTION   This function is called whenever incoming DoS packet is received. 
              It tries to bring up the traffic channel if phys_link is
              dormant. Else, tries to reorig after phys_link goes dormant.
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
extern void dshdr_pkt_cord_orig_hdlr (void);

#endif /* FEATURE_DS_CORD */
#endif /* FEATURE_HDR_DOS */
#endif /* DSHDR_CORD_MGR_H */
