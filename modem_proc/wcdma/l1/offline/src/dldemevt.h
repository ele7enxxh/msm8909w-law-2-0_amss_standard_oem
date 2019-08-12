#ifndef DL_DEM_EVT_H
#define DL_DEM_EVT_H
/*===========================================================================
              < insert header file name here >
              < center and add a space between each letter >

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000, 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dldemevt.h_v   1.1   12 Jul 2001 11:43:30   mlevin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/dldemevt.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
08/13/07   vp      Moved event handlers from dltimeline to individual modules.
===========================================================================*/


/*===========================================================================
FUNCTION        DL_PHYCHAN_CFG_EVENT_HANDLER

DESCRIPTION     This function handles the physical channel configuration
                event.

DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void dl_phychan_cfg_event_handler(void);


/*===========================================================================
FUNCTION        DL_PHYCHAN_FRM_BDRY_EVENT_HANDLER

DESCRIPTION     This function handles the physical channel frame boundary
                event.

DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void dl_phychan_frm_bdry_event_handler(void);

#endif

