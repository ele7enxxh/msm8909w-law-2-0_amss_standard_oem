#ifndef DS707_SO_PKT_H
#define DS707_SO_PKT_H
/*===========================================================================

                          D S 7 0 7 _ P K T _ S O
GENERAL DESCRIPTION
  This file contains the service option configuration for packet data calls
  in a 1x CDMA system.  Each service option has a configuration associated
  with it, which tells MC/SNM things such as:
    
    - which RC's this SO can work on
    - what mux option this rateset is valid with
    - alternate service options 

  In addition, this file contains two tables.  The first table defines when
  a particular service option is enabled/disabled.  The deciding factors
  include things such as system p_rev, the QCSO setting, & desired rateset.
  As these factors change, each service option becomes either enabled or 
  disabled.  This state must be updated with SNM as it changes.
  
  The second table indicates which packet service option the mobile should
  originate with, based on factors similar to those in the first table.
  
EXTERNALIZED FUNCTIONS    
  DS707_SO_PKT_INIT
    Called at startup.  Registers each packet SO with SNM.  Then tells SNM
    which SO's are (currently)_enabled and disabled.
    
  DS707_SO_PKT_GET_ORIG
    Returns the packet SO to originate with.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  Must call ds707_so_pkt_init() at startup.

 Copyright (c) 2002-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_so_pkt.h_v   1.3   21 Jan 2003 22:26:22   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_so_pkt.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/11/11    ack    Global Variable Cleanup
09/04/09    ss     CMI SU level modifications.
01/21/02    ak     Moved sr_id allocation to pkt_mgr.
12/30/02    sy     Added new variable ds707_sr_id to support allocation of
                   SR_ID by DS.
02/20/02    ak     First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "ds707.h"
#include "ds707_extif.h"

/*===========================================================================
                     DECLARATIONS & DEFINITIONS
===========================================================================*/

#define DS707_SO_PKT_NUM   7


/*===========================================================================/
                       EXTERNAL FUNCTION DEFINITIONS
============================================================================*/

/*===========================================================================
FUNCTION       DS707_SO_PKT_INIT

DESCRIPTION    Wrapper function for register_pkt_so() and set_pkt_so_state()
               functions, in order to simplify the external interface. Any
               power-up initialization that needs to be performed for packet
               data service options should be done here.

DEPENDENCIES   This function should be called only after ATCOP has been
               initialized, since it invokes functions that use some AT
               command settings.

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_so_pkt_init(void);

/*===========================================================================
FUNCTION       DS707_SO_PKT_GET_ORIG

DESCRIPTION    Returns the SO to orig with.

DEPENDENCIES   NONE

RETURN VALUE   Service Option to originate with.

SIDE EFFECTS   None
===========================================================================*/
ds707_so_type ds707_so_pkt_get_orig
(
  ds707_rateset_type         rateset
);

/*===========================================================================
FUNCTION       DS707_GET_SO_PKT_TYPE

DESCRIPTION    Returns the requested Packet Data service option

DEPENDENCIES   NONE

RETURN VALUE   Returns the requested Packet Data service option

SIDE EFFECTS   None
===========================================================================*/
ds707_so_type ds707_get_so_pkt_type
(
  byte index
);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_SO_PKT_H     */

