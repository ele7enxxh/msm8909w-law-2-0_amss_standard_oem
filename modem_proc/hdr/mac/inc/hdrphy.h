#ifndef HDRPHY_H
#define HDRPHY_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              H D R   P H Y S I C A L  L A Y E R  P R O T O C O L       

                            H E A D E R  F I L E

GENERAL DESCRIPTION
  This module deals with AT-initiated configuration of Physical layer
  protocol. 
  
EXTERNALIZED FUNCTIONS
  hdrphy_configure  

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2007-2008 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/mac/inc/hdrphy.h#1 $ $DateTime: 2016/12/13 07:58:05 $Author: $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/18/08   etv     Added support for indication processing.
                   Added support for DTXMode attribute.
08/15/07   etv     Created.

===========================================================================*/

/* EJECT */
/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "comdef.h"
#include "err.h"
#include "hdrdebug.h"
#include "hdrhai.h"
#include "hdrlog.h"
#include "hdrind.h"

/* EJECT */
/*===========================================================================
                   
                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions and declarations for constants, macros,
types, variables and other items needed by this module.

===========================================================================*/

/* DTXMode Monitor Type */
typedef void (*hdrphy_dtx_mode_monitor_type)(boolean enabled);

/*===========================================================================
FUNCTION HDRPHY_INIT                                   EXTERNAL

DESCRIPTION
  This function does the initialization for Physical layer protocol

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrphy_init (void);

/*===========================================================================
FUNCTION HDRPHY_IND_CB                                              EXTERNAL

DESCRIPTION
  This function processes incoming indications for this protocol.

DEPENDENCIES
  None.

PARAMETERS
  ind_name     - Name of the indication from hdrhai.h
  ind_data_ptr - Pointer to any indication-specific data

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrphy_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void *ind_data
);

/* EJECT */
/*===========================================================================

FUNCTION HDRPHY_PROCESS_IND 

DESCRIPTION
  This function processes indications given to Physical layer protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  ind_name - Name of the indication for PHY to process
  ind_data_ptr - pointer to data associated with the indication
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrphy_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
);


/*===========================================================================
FUNCTION HDRPHY_CONFIGURE                                   EXTERNAL

DESCRIPTION
  This function does the AT-init configuration of MaxTransmitPower for 
  Subtype-3 physical layer protocol

DEPENDENCIES
  None.

PARAMETERS
  sender - The calling protocol.

RETURN VALUE
  TRUE - If the configuration was needed.
  FALSE - Otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrphy_configure
(
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================
FUNCTION HDRPHY_SET_DIAG_DTX_MODE                          EXTERNAL

DESCRIPTION
  Sets the diag user DTXMode preference and the action for later use.

DEPENDENCIES
  None.

PARAMETERS
  action - (HDRLOG_RESET_DTX_MODE/HDRLOG_OVERRIDE_DTX_MODE) DTX modes
  mode   - DTXMode to override (valid only when action is Override)

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrphy_set_diag_dtx_mode
(
  hdrlog_dtx_mode_action_enum_type action,
  hdrlog_dtx_mode_enum_type mode
);

/*===========================================================================
FUNCTION HDRPHY_REGISTER_DTX_MODE_MONITOR                          EXTERNAL

DESCRIPTION
  This function is used to register callbacks to monitor changes to DTXMode.

  At present this module supports only one Monitor.

DEPENDENCIES
  None.

PARAMETERS
  cb - DTXMode monitor

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrphy_register_dtx_mode_monitor 
(
  hdrphy_dtx_mode_monitor_type cb
);

#endif /* HDRPHY_H */
