#ifndef DS707_SO33_H
#define DS707_SO33_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                         D S 7 0 7 _ S 0 3 3 

GENERAL DESCRIPTION
  This file contains functions specific to service option 33.  It does NOT
  contain RLP 3 services.

EXTERNALIZED FUNCTIONS
  DS707_SO33_PKT_INITIALIZER
    Called when an SO 33 call connects.  Initializes RLP 3 and initializes
    the RRM state (which includes SCRM & Control/Hold).
    
  DS707_SO33_REG_INITIALIZERS
    Called at startup.  Registers initializer with SNM for when SO 33 
    call comes up.  Also registers RLP 3 BLOB generator asw well as QOS
    functionality.
    
  DS707_SO33_ORIG_QOS
    Asks RLP to create a QOS blob for CM origination parameters.
    
  DS707_SO33_PROCESS_QOS
    Called when QOS info received from BS.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Must call ds707_so33_reg_initializers at startup.

   Copyright (c) 2002 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_so33.h_v   1.3   19 Nov 2002 19:31:20   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_so33.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/17/02   ak      Updated file header comments.
09/24/02   atp     Added support for QOS. (1x Release-A feature).

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "caii.h"
#include "cm.h"


/*===========================================================================
                         EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SO33_PKT_INITIALIZER

DESCRIPTION   Called when a so 33 packet data call is connected.  Goes ahead
              and sets up the associated RLP service.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so33_pkt_initializer
(
  const caii_srv_cfg_type*  cfg_info,
  word                      so_index
);


/*===========================================================================
FUNCTION      DS707_SO33_REG_INITIALIZERS

DESCRIPTION   Register initializer functions used by snm when a Service
              Option 33 is connected.

DEPENDENCIES  This function should only be called after all SO's have been
              added to the SNM.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so33_reg_initializers(void);

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================
FUNCTION      DS707_SO33_ORIG_QOS

DESCRIPTION   Returns the quality of service info, if using SO 33.

DEPENDENCIES  NONE

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so33_orig_qos
(
  cm_cdma_orig_params_s_type    *orig_params
);

/*===========================================================================
FUNCTION      DS707_SO33_PROCESS_QOS

DESCRIPTION   Asks RLP to process QOS blob received from BS.

DEPENDENCIES  NONE

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so33_process_qos
(
  cm_call_mode_info_s_type  *mode_info_ptr
);
#endif /* FEATURE_IS2000_REL_A */

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_SO33_H       */

