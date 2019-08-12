#ifndef HDRCON_H
#define HDRCON_H

/*===========================================================================

              C O N N E C T E D   S T A T E   P R O T O C O L
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the Connected State Protocol (CON)

Copyright (c) 2000,2001,2002 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2003,2004,2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrcon.h_v   1.7   05 Mar 2003 14:16:40   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrcon.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/24/10   pxu     Removed FEATURE_HDR_QSM features 
11/26/08   wsh     Fixed crash in logging
01/11/07   sju     Added support to check if CC and TCA are in the same 
                   connection layer packet.
09/05/06   mpa     Added support for FEATURE_HDR_QSM.
07/12/05   hal     Added support for CloseReason 011 (1x handdown)
                   Added parameter hdrhai_abort_reason_enum_type reason to
                   hdrcon_close_connection()
03/05/03   dna     Added hdrcon_connection_is_closing
                   Clean up powerup_init processing
07/28/00   nmn     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdrhai.h"
#include "dsm.h"
#include "comdef.h"
#include "qw.h"
#include "hdrerrno.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRCON_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRCON.

DEPENDENCIES
  None
  
PARAMETERS
  msg - Pointer to the DSM item chain holding the message

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

extern void hdrcon_msg_cb
( 
  dsm_item_type * msg_ptr 
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRCON.

DEPENDENCIES
  None
 
PARAMETERS
  ind_name - Indication to process, is a global enum for all
                    possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

extern void hdrcon_ind_cb
( 
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_ACTIVATE

DESCRIPTION
  This function commands CON to activate.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrcon_activate
(
  hdrhai_protocol_name_enum_type sender
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_DEACTIVATE

DESCRIPTION
  This function commands HDRCON to deactivate

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrcon_deactivate
( 
  hdrhai_abort_reason_enum_type reason,
  hdrhai_protocol_name_enum_type sender
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_CLOSE_CONNECTION

DESCRIPTION
  This function commands CON to close the connection.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrcon_close_connection
(
  hdrhai_protocol_name_enum_type sender,
  hdrhai_abort_reason_enum_type reason
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_POWERUP_INIT

DESCRIPTION
  This function commnds CON to Initialize.  This function is called instead
    of hdrcon_init at powerup.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrcon_powerup_init( void );

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_GET_ADVERTISED_SUSPEND_TIME

DESCRIPTION
  This function returns the suspend time advertised last time we closed a
  connection.  If the CON is in the open state when this function is
  called, it will return an error.  Nobody should ever need the suspend
  time while the connection is open.

DEPENDENCIES
  None

PARAMETERS
  suspend - the value of the suspend time sent over the air in the
            ConnectionClose Message
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  E_SUCCESS - a valid suspend time is returned
  E_NOT_AVAILABLE - No suspend time has been sent, so the requested data
                    is not available.
  E_NOT_ALLOWED - the Connected State Protocol is in Open State (thus making
              the request invalid)

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrcon_get_advertised_suspend_time
(
  qword suspend,
  hdrhai_protocol_name_enum_type caller
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_CONNECTION_IS_CLOSING

DESCRIPTION
  This accessor function tells if the connection is closing

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if the connection is in the process of closing (i.e. a connection
       close message is being sent)
  FALSE otherwise     

SIDE EFFECTS
  None

===========================================================================*/

extern boolean hdrcon_connection_is_closing
( 
  hdrhai_protocol_name_enum_type caller
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_CC_RECEIVED

DESCRIPTION
  This accessor function tells if AN has sent a ConnectionClose from AT

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if AN has sent a connection to AT
  FALSE otherwise     

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrcon_cc_received
( 
  hdrhai_protocol_name_enum_type caller
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCON_SET_HHO_IS_IN_PROGRESS_FLAG

DESCRIPTION
  This function set the hdrcon.hho_is_in_progress flag  

DEPENDENCIES
  None

PARAMETERS
  hho_is_in_progress   - TRUE or FALSE

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrcon_set_hho_is_in_progress_flag
(
  boolean hho_is_in_progress
);

/*===========================================================================

FUNCTION HDRCON_HHO_IS_IN_PROGRESS

DESCRIPTION
  This function retruns if HHO  is in progress 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE/FALSE - if HHO is in progress of not 

SIDE EFFECTS
  None

===========================================================================*/

extern boolean hdrcon_hho_is_in_progress(void);

#endif               /* HDRCON_H */
