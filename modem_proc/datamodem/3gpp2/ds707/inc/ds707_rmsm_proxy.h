#ifndef DS707_RMSM_PROXY_H
#define DS707_RMSM_PROXY_H

/*===========================================================================

                          D S 7 0 7 _ R M S M _ P R O X Y. H

DESCRIPTION

  The Data Services RmSm Proxy'S main operations are to handle 1X Dial String
  Callbacks, set up the port bridge between the MDM and MSM, handle modem 
  control signals such as Data Terminal Ready(DTR), Carrier Detect(CD)
  signals and handle abort commands.

Copyright (c) 2001 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_rmsm.h_v   1.6   23 Jan 2003 16:34:28   randrew  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_rmsm_proxy.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/21/10    vs     Initial File
01/07/11    vs     Changes to make dstask free-floating.

===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DSAT_MDM_FUSION
#include "dsat_v.h"

/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Enumeration of the different states in RmSm Proxy State machine
---------------------------------------------------------------------------*/
typedef enum
{
  NULL_STATE = 0,
  WAITING_FOR_BRIDGE_UP_STATE,
  BRIDGE_UP_STATE,
  MAX_STATE
} ds707_rmsm_proxy_states_type;

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
  FUNCTION DS707_RMSM_INIT()

  DESCRIPTION
    This function initializes the SIOLIB function table.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
void ds707_rmsm_proxy_init
(
  void
);

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_DIAL_STR_CB()

  DESCRIPTION
    This callback function fetches the forwarding port information from ATCOP
	and sets the Call coming up state information on HSUSB and HSUART ports.

  PARAMETERS
    dial_string: The dial string
    digit_mode: Flag to tell whether dial string contains digits only

  RETURN VALUE
    Abort handler callback.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_proxy_call_abort_cb_type  ds707_rmsm_proxy_dial_str_cb
(
  const byte                      *dial_string,
  boolean                          digit_mode,
  dsat_dial_modifier_info_type    *modifiers
);

/*===========================================================================
  FUNCTION DS707_RMSM_PROXY_ATD_CONNECT_CB()

  DESCRIPTION
    This callback function registers for modem control signals and Rx, Tx 
	watermarks with HSUSB and HSUART ports. It changes the stream mode to PKT 
	effectively setting up a port bridge. Finally it forwards the CONNECT 
	result code to SIOLIB on HSUSB port.

  PARAMETERS

  RETURN VALUE
    Abort handler callback.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds707_rmsm_proxy_atd_connect_cb
(
  void
);

#endif /* FEATURE_DSAT_MDM_FUSION */
							 
#endif /* DS707_RMSM_H */
