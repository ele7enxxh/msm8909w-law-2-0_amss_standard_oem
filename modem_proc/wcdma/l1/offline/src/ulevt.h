#ifndef UL_EVT_H
#define UL_EVT_H

/*===========================================================================
                             U L _ E V T . H

GENERAL DESCRIPTION
  This contains prototypes for uplink controller sequencer event handlers.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

Copyright (c) 2000,2001 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/
#include "customer.h"

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/ulevt.h_v   1.4   19 Apr 2001 18:27:12   skeshava  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/ulevt.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/08/12    gv      Feature HS RACH Initial code check in
02/10/12    rgn     Nikel feature cleanup
02/01/12    gnk     Nikel feature cleanup
02/23/11    asm     Cleanup of unused, mainlined or irrelevant features in offline UL code
05/22/09    sa      Fixed compiler warnings
04/22/09    gnk     Fixed compiler warnings
10/13/08    oh      Fixed a featurization issue.
10/03/08    hk      Bringing in 7k Mailine fixes
08/14/06    yiz     Mainline HSUPA code
09/22/00    sk      file created.
                    
===========================================================================*/

/*===========================================================================
FUNCTION        wenc_phychan_get_data

DESCRIPTION     This function processes data received from L2. It sets up
                appropriate control in the encoder hardware so that the data
                is transmitted properly after encoding.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void wenc_phychan_get_data(void);

/*===========================================================================
FUNCTION        E_UL_EDPCH_CFG_EVENT_HANDLER

DESCRIPTION     This function sends the mdsp cmd related to EDPCH

DEPENDENCIES    The config db information

RETURN VALUE    

SIDE EFFECTS    The mdsp EDPCH shadow buffer will be filled.
===========================================================================*/

extern void e_ul_edpch_cfg_event_handler
(
  void
);

//#ifdef FEATURE_WCDMA_HSUPA_PSEUDO_FIQ
/*===========================================================================
FUNCTION    EUL_START_EDCH_DATA_PATH_FIQ_HANDLER

DESCRIPTION This function updates the serving grant and HARQ process  at every
            TTI boundary and shall take into account the Absolute Grant message,
            Serving Relative Grant and non-serving Relative Grants that apply 
            to the TTI. UEs configured with an E-DCH transport channel shall
            maintain a Serving Grant and the list of active HARQ processes 
            based on the absolute and relative grant commands decoded on the
            configured E-AGCH and E-RGCH(s).

DEPENDENCIES Called when FIQ is raised by FW

RETURN VALUE None

SIDE EFFECTS serving grant may change and HARQ process may become inactive
===========================================================================*/
extern void eul_start_edch_data_path_fiq_handler(void);
//#endif /* FEATURE_WCDMA_HSUPA_PSEUDO_FIQ */

#endif
