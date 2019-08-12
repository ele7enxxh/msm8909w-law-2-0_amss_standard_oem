#ifndef DS_3GPP2_VEND_H
#define DS_3GPP2_VEND_H
/*===========================================================================

                           D S _ 3 G P P 2 _ V E N D . H

DESCRIPTION
  This file contains the definitions of data structures, defined and 
  enumerated constants and function prototypes required for the 
  3GPP2 vendor-specific features defined in IS-835-D, such as 
  Version/Capability Indication and Max PPP Inactivity Timer.

  The Organizationally-Unique Identifier (OUI) for this is 0xCF002.

  Copyright (c) 2006-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds_3gpp2_vendor.h#1 $
  $Author: mplcsds1 $  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/17/12    ssh    Added ds_3gpp2_mpit_timer_stop()
10/17/11    ssh    eHRPD Always On support
06/06/11    ash    Added support for PPP Partial Context
12/21/06    ssh    Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"                        /* Customer Specific Features */
#include "comdef.h"

#if ( defined(FEATURE_DATA_3GPP2_ALWAYS_ON) || defined(FEATURE_DATA_3GPP2_VS) )
#include "dstask_v.h"
/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION DS_3GPP2_VENDOR_POWERUP_INIT()

DESCRIPTION
  This function registers with the PPP vendor-extensions module to receive 
  all vendor-specific packets for the 3GPP2 OUI (0xCF002). It also 
  initializes vendor-specific configuration per NV items/defaults and such.
  
PARAMETERS
  None

RETURN VALUE
  0: Success
  -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ds_3gpp2_vendor_powerup_init(void);

/*===========================================================================
FUNCTION DS_3GPP2_VENDOR_INIT()

DESCRIPTION
  This function registers with the PPP vendor-extensions module to receive 
  all vendor-specific packets for the 3GPP2 OUI (0xCF002). It also 
  registers for PPP_STARTING_EV.
  
PARAMETERS
  None

RETURN VALUE
  0: Success
  -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ds_3gpp2_vendor_init
(
  void
);

/*===========================================================================
FUNCTION DS_3GPP2_VENDOR_CLEANUP()

DESCRIPTION
  This function de-registers with the LCP starting event and resets the number of retries, 
  and other values.
  
PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp2_vendor_cleanup
(
  void
);

/*===========================================================================
FUNCTION      DS_3GPP2_MPIT_TIMER_START

DESCRIPTION   This function Starts mpit timer, if and only if the 
              timer is not running. Calling this function while the
              timer is running will not have any effect.

PARAMETERS    None 

DEPENDENCIES  The timer should not be running for this function to take effect.

RETURN VALUE 
0 if started 
-1 if not started

SIDE EFFECTS  None
===========================================================================*/
int ds_3gpp2_mpit_timer_start(void);

/*===========================================================================
FUNCTION      DS_3GPP2_MPIT_TIMER_RESTART

DESCRIPTION   This function restarts mpit timer, if and only if the 
              timer is running. Calling this function while the
              timer is not running will not have any effect.

PARAMETERS    None 

DEPENDENCIES  The timer should be running for this function to take effect.

RETURN VALUE 
0 if restarted 
-1 if not restarted

SIDE EFFECTS  None
===========================================================================*/
int ds_3gpp2_mpit_timer_restart(void);

/*===========================================================================
FUNCTION      DS_3GPP2_MPIT_TIMER_STOP

DESCRIPTION   This function stops the MPIT timer (and the associated LSD
              timer.)

PARAMETERS    None 

DEPENDENCIES  None

RETURN VALUE 
0 success
-1 failure

SIDE EFFECTS  None
===========================================================================*/
int ds_3gpp2_mpit_timer_stop(void);

/*===========================================================================
FUNCTION      DS_3GPP2_MPIT_TIMER_PROCESS_CMD

DESCRIPTION   Handles expiry of max ppp inactivity timer. 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp2_mpit_timer_process_cmd
(
  ds_cmd_type * cmd_ptr
);

#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON
/*===========================================================================
FUNCTION      DS_3GPP2_LSD_TIMER_PROCESS_CMD

DESCRIPTION   Handles expiry of Link Status Determination timer

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp2_lsd_timer_process_cmd
(
  ds_cmd_type * cmd_ptr
);
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */

/*===========================================================================
FUNCTION      DS_3GPP2_MPIT_TIMEOUT_CB

DESCRIPTION   MPIT timer callback function.

PARAMETERS    None 

DEPENDENCIES  None

RETURN VALUE 
None
SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp2_mpit_timeout_cb
(
  unsigned long    timer_id
);
#endif /*  #if ( defined(FEATURE_DATA_3GPP2_ALWAYS_ON) || defined(FEATURE_DATA_3GPP2_VS) ) */
#endif /* DS_3GPP2_VEND_H */
