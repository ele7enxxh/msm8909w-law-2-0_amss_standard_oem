#ifndef DS707_PKT_MGR_HDR_H
#define DS707_PKT_MGR_HDR_H

/*===========================================================================

                      D S 7 0 7 _ P K T _ M G R _ H D R. H
                      
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2004  - 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pkt_mgr.c_v   1.45   25 Feb 2003 14:25:02   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_pkt_mgr_hdr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/09/09    sa      AU level CMI modifications.
04/18/08    mga    Added function ds707_ln_update_status_cmd
05/18/06    gr     Fix to send DRS=0 orig for IDM change after PHYS_LINK
                   goes down (if IDM Change was received before CALL_END)
11/11/05    vrk/gr Avoiding ppp resync on failed ALN on MIP
11/30/04    kvd    Added new function proto-types for CAM module.
01/05/04    ak     First revision.  Copied IOCTL over to this file.

===========================================================================*/


/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_HDR
#include "dstask_v.h"
#include "sys.h"




/*===========================================================================
                           FORWARD DECLARATIONS
===========================================================================*/

/*===========================================================================
                            VARIABLES
===========================================================================*/

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION       DS707_PKT_POST_IDM_CHANGE_CMD

DESCRIPTION    If we move a dormant data session from 1X to HDR or from HDR
               to 1X, send an orig with DRS=0 to notify the base station.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_pkt_post_idm_change_cmd
(
  sys_sys_mode_e_type  new_system                /* New System             */
);

/*===========================================================================
FUNCTION       DS707_PKT_IDM_REORIG_AFTER_PHYS_LINK_DOWN

DESCRIPTION    If DRS=0 orig as a result of IDM change failed to go out, 
               this function attempts a re-origination after the PHYS_LINK
	       goes down (dormancy) 

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_pkt_idm_reorig_after_phys_link_down
(
  void
);
#ifdef FEATURE_DS_CAM

/*===========================================================================
FUNCTION       DS707_PKT_POST_COLOC_INFO_CHANGED_CMD

DESCRIPTION    Posts new colocation info along with the INFO_CHANGE_CMD
               to DS. DS  updated its local copy of colocation info on
               receiving this command.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/

void ds707_pkt_post_coloc_info_changed_cmd
(
  boolean evdo_colocated
);

/*===========================================================================
FUNCTION      DS707_PKT_EVDO_COLOCATED

DESCRIPTION   Process sys_info change event posted by CM.
              DS needs this information to see if we have evdo colocated.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_pkt_evdo_colocated
(
  void
);

#endif /* FEATURE_DS_CAM */

/*===========================================================================
FUNCTION       DS707_PKT_POST_ALN_CMD

DESCRIPTION    Post the INITIATE_ALN command from HDR protocols to DS

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_pkt_post_aln_cmd
(
  void
);

/*===========================================================================
FUNCTION      DS707_PKT_HDR_PROCESS_CMD

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
void ds707_pkt_hdr_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
);

/*===========================================================================
FUNCTION       DS707_ALN_CMD_HDLR

DESCRIPTION    Processed INITIATE_ALN_CMD from HDR MC. If a MIP call is up,
               calls ppp_resync, else initiates orig with DRS 1 if we need
               to do a PPP resync.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_aln_cmd_hdlr
(
  void
);


#ifdef FEATURE_DS_LN_UPDATE
/*===========================================================================
FUNCTION       ds707_ln_update_status_cmd

DESCRIPTION    Post LN_update status command to DS task

DEPENDENCIES   

RETURN VALUE   None

SIDE EFFECTS   
===========================================================================*/
void ds707_ln_update_status_cmd 
(
  sys_hdr_location_notification_status_e_type  ln_status
);

#endif /*FEATURE_DS_LN_UPDATE*/
#endif /* FEATURE_HDR         */
#endif /* FEATURE_DATA_IS707  */
#endif /* DS707_PKT_MGR_HDR_H */

