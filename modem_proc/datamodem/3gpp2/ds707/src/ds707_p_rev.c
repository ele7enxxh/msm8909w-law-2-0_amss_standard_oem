/*===========================================================================

                         D S 7 0 7 _ P _ R E V
GENERAL DESCRIPTION
  This file contains functions relating the system P_REV, which is determined
  by MC/CM.
  
EXTERNAL FUNCTIONS
  DS707_P_REV_GET_INDEX
    Returns an enum indicating the current p_rev.
    
  DS707_P_REV_CHANGE_HANDLER
    Called by MC when the system P_REV changes.
  
  DS707_P_REV_REGISTER  
    Registers te P_REV change handler with MC.

INITIALIZATION
  Need to register P_REV handler with MC.
    
 Copyright (c) 2002-2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_p_rev.c_v   1.1   18 Nov 2002 21:28:58   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_p_rev.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/05/14   sd      Added new API to fetch p_rev value from MC
08/18/14   sd      To originate call on 1x with correct SO only after ESPM 
                   indication is received.
03/04/14   sd      Changes made to notify DSD to indicate 1x service 
                   availability only after ESPM is received.
11/02/09   as      Fixed ARM Compiler warning - included header
07/23/04   atp     Added P_REV 10 & 11 support for Rel D.
04/22/04   ak      Lint changes.
10/14/03   sy      Added P_REV 9 support for Rel C.
06/26/03   sy      Added P_REV 8 support for Rel B.
11/17/02   ak      Updated header comments.
02/20/02   ak      First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#ifdef FEATURE_DATA_IS707
#include "cm.h"
#include "ds707_p_rev.h"
#include "ds707_so_pkt.h"
#include "err.h"
#include "mc.h"
#include "mccdma_v.h"
#include "cai_v.h"
#include "ds_dsd_ext_i.h"
#include "data_msg.h"
#include "ps_iface_defs.h"
#include "ds707_pkt_mgri.h"
#include "ds_flow_control.h"


/*===========================================================================
                       Global Variables
===========================================================================*/
static boolean espm_ind_recvd = FALSE;

/*===========================================================================
                       Function Declaration
===========================================================================*/
void ds707_sync_espm_ind_cback
(
  byte     msg_type,
  byte     p_rev
);

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_P_REV_GET_INDEX

DESCRIPTION   Based on the p_rev_in_use, returns the index into the enum list
              of p_revs.
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
word ds707_p_rev_get_index(void)
{
  byte  p_rev_index = 0;                    /* the sys's p_rev index       */
  byte  sys_p_rev;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  // Get MC p_rev
  sys_p_rev = mc_get_p_rev_in_use();

  if (sys_p_rev <= 3)
  {
    p_rev_index = (word)DS707_P_REV_3;
  }
  else if (sys_p_rev <= 5)
  {
    p_rev_index = (word)DS707_P_REV_5;
  }
  else if (sys_p_rev == 6)
  {
    p_rev_index = (word)DS707_P_REV_6;
  }
  else if (sys_p_rev == 7)
  {
    p_rev_index = (word)DS707_P_REV_7;
  }
  else if (sys_p_rev == 8)
  {
    p_rev_index = (word)DS707_P_REV_8;
  }
  else if (sys_p_rev == 9)
  {
    p_rev_index = (word)DS707_P_REV_9;
  }
  else if (sys_p_rev == 10)
  {
    p_rev_index = (word)DS707_P_REV_10;
  }
  else if (sys_p_rev == 11)
  {
    p_rev_index = (word)DS707_P_REV_11;
  }
  else
  {
    ERR_FATAL("Unknown system p_rev %d",sys_p_rev,0,0);
  }
  return (p_rev_index);
} /* ds707_p_rev_get_index() */


/*===========================================================================
FUNCTION      DS707_P_REV_CHANGE_HANDLER

DESCRIPTION   Enable/Disable the appropriate Packet Data service options and 
              set up the originating Packet Data service options, based on
              the new base station protocol revision.  This allows the phone
              to intelligently know whether or not to try IS95B or 3G data
              service options.
  
              This function is a callback registered with CM in dsi_cm_init.
              It is called whenever the prev_in_use changes.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_p_rev_change_handler
(
  cm_ss_event_e_type                ss_event,
  const cm_mm_msim_ss_info_s_type  *ss_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((ss_event == CM_SS_EVENT_SRV_CHANGED)                                 &&
      (ss_info_ptr->stack_info[0].changed_fields & CM_SS_EVT_BS_P_REV_MASK) &&
      ((ss_info_ptr->stack_info[0].sys_mode == SYS_SYS_MODE_CDMA) ||
       (ss_info_ptr->stack_info[0].sys_mode == SYS_SYS_MODE_HDR)  /* UNFINISHED - HDR? */
      )
     )
  {
    /*-------------------------------------------------------------------------
      Re-initialize the service options used, since the p_rev_in_use has
      changed, and system is digital.
    -------------------------------------------------------------------------*/
    ds707_so_pkt_recal();
  }
} /* ds707_p_rev_change_handler() */


/*===========================================================================
FUNCTION      DS707_P_REV_REGISTER

DESCRIPTION   Registers the P_REV with MC/CM.  Called once, at startup.
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_p_rev_register
(
  cm_client_id_type        cm_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* TODO: This function is dead code. Need to check if this function can be removed */
} /* ds707_p_rev_register() */

/*===========================================================================
FUNCTION       DS707_GET_SO_MASK_FROM_P_REV

DESCRIPTION    Gets SO mask for a given p_rev
 
DEPENDENCIES   None.

RETURN VALUE   so_mask - based on p_rev value

SIDE EFFECTS   None
===========================================================================*/
uint32 ds707_get_so_mask_from_p_rev 
(
  byte p_rev
)
{
  uint32  so_mask = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (p_rev <= 4)
  {
    so_mask = PS_IFACE_CDMA_1X_IS95;
  }
  else if (p_rev >= 7)
  {
    so_mask = PS_IFACE_CDMA_1X_IS2000_REL_A;
  }
  else
  {
    /* p_rev 5 is undefined and 6 is IS2000. 
       Default to IS2000 for both 5 and 6 */
    so_mask = PS_IFACE_CDMA_1X_IS2000;
  }

  return so_mask;
} /* ds707_get_so_mask_from_p_rev() */

/*===========================================================================
FUNCTION       DS707_REGISTER_SYNC_ESPM_IND

DESCRIPTION    Registers a callback to get indication for SYNC and ESPM 
               messages. Called once at startup.
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_register_sync_espm_ind
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mcc_ds_reg_sync_espm_prev_cb(ds707_sync_espm_ind_cback);
} /*ds707_register_sync_espm_ind() */

/*===========================================================================
FUNCTION       DS707_SYNC_ESPM_IND_CBACK

DESCRIPTION    Callback called to indicate SYNC and ESPM indications. Called by
               MC.
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_sync_espm_ind_cback
(
  byte     msg_type,
  byte     p_rev
)
{
  ds_dsd_event_info_type  event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                   "ds707_sync_espm_ind_cback called with msg_type : %d", 
                   msg_type );

  switch(msg_type)
  {
    case CAI_SYNC_MSG :
      ds707_phys_link_disable_flow(DS_FLOW_NO_SERVICE_MASK);
      espm_ind_recvd = FALSE;
      break;

    case CAI_EXT_SYS_PARM_MSG :
      ds707_phys_link_enable_flow(DS_FLOW_NO_SERVICE_MASK);
      memset(&event_info, 0, sizeof(ds_dsd_event_info_type));

      event_info.event = DS_DSD_CLIENT_RADIO_NETWORK_PARM_CHANGED_EV;
      event_info.data.cdma_p_rev_ind.rat_mask = PS_IFACE_CDMA_1X;
      event_info.data.cdma_p_rev_ind.so_mask = ds707_get_so_mask_from_p_rev(p_rev);

      ds_dsd_ext_event_notify(DS_DSD_RADIO_1X, &event_info); 
      espm_ind_recvd = TRUE;
      break;

    default:
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, "Invalid msg_type : %d", 
                       msg_type );
      break;
  }

  return;
} /* ds707_sync_espm_ind_cback() */

/*===========================================================================
FUNCTION       DS707_GET_ESPM_IND_RECVD

DESCRIPTION    Get API for espm_ind_recvd.
 
DEPENDENCIES   None.

RETURN VALUE   Booelan

SIDE EFFECTS   None
===========================================================================*/
boolean ds707_get_espm_ind_recvd
(
  void
)
{
  DATA_IS707_MSG1( MSG_LEGACY_HIGH, "espm_ind_recvd = %d", espm_ind_recvd);

  return espm_ind_recvd;
} /* ds707_get_espm_ind_recvd() */

/*===========================================================================
FUNCTION      DS707_ORIG_P_REV_GET_INDEX

DESCRIPTION   Based on the p_rev_in_use, returns the index into the enum list
              of p_revs during call origination.
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
word ds707_orig_p_rev_get_index(void)
{
  byte  p_rev_index = 0;                    /* the sys's p_rev index       */
  byte  sys_p_rev;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  // Get MC p_rev
  sys_p_rev = mc_ds_get_p_rev_in_use();

  if (sys_p_rev <= 3)
  {
    p_rev_index = (word)DS707_P_REV_3;
  }
  else if (sys_p_rev <= 5)
  {
    p_rev_index = (word)DS707_P_REV_5;
  }
  else if (sys_p_rev == 6)
  {
    p_rev_index = (word)DS707_P_REV_6;
  }
  else if (sys_p_rev == 7)
  {
    p_rev_index = (word)DS707_P_REV_7;
  }
  else if (sys_p_rev == 8)
  {
    p_rev_index = (word)DS707_P_REV_8;
  }
  else if (sys_p_rev == 9)
  {
    p_rev_index = (word)DS707_P_REV_9;
  }
  else if (sys_p_rev == 10)
  {
    p_rev_index = (word)DS707_P_REV_10;
  }
  else if (sys_p_rev == 11)
  {
    p_rev_index = (word)DS707_P_REV_11;
  }
  else
  {
    ERR_FATAL("Unknown system p_rev %d",sys_p_rev,0,0);
  }
  return (p_rev_index);
} /* ds707_orig_p_rev_get_index() */

#endif /* FEATURE_DATA_IS707 */
