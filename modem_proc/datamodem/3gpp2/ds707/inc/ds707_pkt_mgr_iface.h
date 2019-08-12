#ifndef DS707_PKT_MGR_IFACE_H
#define DS707_PKT_MGR_IFACE_H
/*===========================================================================

                  D S 7 0 7 _ P K T _ M G R _ I F A C E . H
                  
GENERAL DESCRIPTION
  This file is the controller for 1X packet calls and the PS_IFACE.
  
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2002 - 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.
  
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pkt_mgr.c_v   1.45   25 Feb 2003 14:25:02   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_pkt_mgr_iface.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/09/09    sa     AU level CMI modifications.
07/02/02    sc     Changed the ds707_pkt_mgr_iface_init() signature to support
                   Application Profiles and Dormant2 (Iface Lingering) features
04/24/04    ak     Lint fixes.
02/19/04    ak     Removed get_active_iface_ptr function.
01/09/04    ak     First version of file.
===========================================================================*/


/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "ps_iface.h"
#include "ps_acl.h"
#include "dstask_v.h"
#include "ps_phys_link.h"
#include "ds707_data_session_profile.h"


/*===========================================================================
                        FORWARD DECLARATIONS
===========================================================================*/

/*===========================================================================
                              TYPEDEFS
===========================================================================*/

/*===========================================================================
                              VARIABLES
===========================================================================*/

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKT_IFACE_ENABLE_IND

DESCRIPTION   Called when user wants to enable the 707 pkt ifaces.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_iface_enable_ind(void);

/*===========================================================================
FUNCTION      DS707_PKT_IFACE_DISABLE_IND

DESCRIPTION   Called when user wants to disable the 707 pkt ifaces.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_iface_disable_ind(void);

/*===========================================================================
FUNCTION      DS707_PKT_IFACE_PROCESS_CMD

DESCRIPTION   Called by 3G DSMGR, to handle commands specific to this
              mode handler.

              There is no featurization of commands here.  Functionality
              should be FEATURE'ized elsewhere, so that these commands are
              generated appopriately.  For example, the SID module only
              generates its command if the SID FEATURE's is defined.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  Can result in various actions, such as call bring up/tear
              down, etc.
===========================================================================*/
void ds707_pkt_iface_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_INIT

DESCRIPTION   Called once at mobile power-up.  Registers the appropriate
              iface unctionality with PS IFACE.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_iface_init
(
  ps_iface_type             *iface_ptr,       /* ptr to 1x iface          */
  acl_type                  *my_acl_ptr,      /* ptr to acl               */
  ps_phys_link_type         *phys_link_ptr,   /* ptr to phys link(s)      */
  uint8                      num_phys_links   /* num phys links passed in */
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_PROCESS_CMD

DESCRIPTION   

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  
===========================================================================*/
void ds707_pkt_mgr_iface_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
);

/*=========================================================================
FUNCTION       DS707_PKT_MGR_IFACE_CONN_DELAY_TIMER_CBACK

DESCRIPTION    Function registered when the Timer for a Connection release 
               delay is expired.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
void ds707_pkt_mgr_iface_conn_delay_timer_cback
(
  unsigned long instance_ptr
);

/*=========================================================================
FUNCTION       DS707_PKT_MGR_IFACE_START_CONN_DELAY_TIMER

DESCRIPTION   Starts the Connection Release Delay Timer for the Specified 
               Duration.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
=========================================================================*/
void ds707_pkt_mgr_iface_start_conn_delay_timer
( 
  ps_iface_type *ps_iface_ptr
);

/*=========================================================================
FUNCTION       DS707_PKT_GET_CONN_DELAY_TIMER_FROM_IFACE_PTR

DESCRIPTION    Given the IFACE returns pointer to connection release delay timer
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
rex_timer_type * ds707_pkt_get_conn_delay_timer_from_iface_ptr
( 
  ps_iface_type  *iface_ptr
);

#endif /* FEATURE_DATA_IS707   */
#endif /*DS707_PKT_MGR_IFACE_H */

