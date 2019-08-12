#ifndef HDRSMP_H
#define HDRSMP_H

/*===========================================================================

            S E S SI O N   M A N A G E M E N T   P R O T O C O L
                           D E F I N I T I O N S

DESCRIPTION
  This contains the external declarations for the Session Management
  Protocol (SMP)

Copyright (c) 2000 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrsmp.h_v   1.9   11 Feb 2002 16:24:54   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrsmp.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/15/14   vko     Fixed race condition while session close
05/06/13   cnx     Added hdrsmp_session_is_valid.
05/31/11   cnx     Added hdrsmp_get_close_reason().
04/15/09   pba     Added support for IRAT pilot measurement
08/29/08   wsh     Fixed double UATIReq when AT powers up w/ KA expired
08/13/08   sju     Added hdrsmp_restart_ka_timer() for KA enhancement. 
12/10/07   wsh     DCM releated changes
10/04/06   yll     Added several external functions
05/09/06   yll     Added support for Minimal UATI Mode.
12/13/05   pba     Changes to support multiple personality negotiation
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
01/17/05   mpa     Added hdrsmp_session_is_closing() accessor function.
10/19/04   mpa     Added hdrsmp_session_is_open().  (Moved from scp)
02/25/04   mpa     Added close_is_silent param to hdrsmp_deactivate().
01/17/02   dna     Remove hdrsmp_init() because it was never called.
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
11/21/00   nmn     Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "hdrhai.h"
#include "dsm.h"
#include "comdef.h"
#include "hdrerrno.h"
#include "hdrind.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* struct for all session attributes */
typedef struct
{
  uint16 keep_alive_expiration; /* in minutes */
} hdrsmp_config_params_type;

typedef enum
{
  HDRSMP_DUP_ACT_DETECT_DISABLED,
  HDRSMP_DUP_ACT_DETECT_ENABLED,
  HDRSMP_DUP_ACT_DETECT_TRIGGERED
} hdrsmp_dup_act_detect_enum_type;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRSMP_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRSMP.

DEPENDENCIES
  None

PARAMETERS
  msg - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrsmp_msg_cb
(
  dsm_item_type * msg_ptr
);

/*===========================================================================

FUNCTION HDRSMP_CONFIG_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRSMP in-config instance.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrsmp_config_msg_cb
(
  dsm_item_type * msg_ptr
);

/*===========================================================================

FUNCTION HDRSMP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRSMP.

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

extern void hdrsmp_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void * ind_data_ptr
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_ACTIVATE

DESCRIPTION
  This function commands SMP to activate.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrsmp_activate
(
  hdrhai_protocol_name_enum_type sender
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_DEACTIVATE

DESCRIPTION
  This function commands SMP to deactivate.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)
  close_is_silent - TRUE if the session should be closed silently
                    FALSE if the session should be closed explicitely
                    (i.e. by sending a SessionClose message to the AN first)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrsmp_deactivate
(
  hdrhai_protocol_name_enum_type sender,
  boolean close_is_silent
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_POWERUP_INIT

DESCRIPTION
  This function commands SMP to Initialize.  This function is called
    instead of hdrsmp_init at powerup.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrsmp_powerup_init ( void );


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_SESSION_IS_INACTIVE

DESCRIPTION
  This boolean accessor function tells the caller whether the session is
  inactive or not.  It is intended to be used after initialization is completed.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - the session is inactive
  FALSE - the session is not inactive

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrsmp_session_is_inactive
(
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRSMP_SESSION_IS_OPEN

DESCRIPTION
  This boolean accessor function tells the caller whether the session is
  open or not.  It is intended to be used after initialization is completed.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - the session is open
  FALSE - the session is not open

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean hdrsmp_session_is_open
(
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRSMP_SESSION_IS_VALID

DESCRIPTION
  This boolean accessor function tells the caller whether the session is
  valid or not.
 
  Normally hdrsmp_session_is_open is used to check whether session is open or
  not. Sometimes, this is not enough. E.g., in a scenario where AT enters
  HDR network. AT has HRPD session but the eHPRD is allowed. AT does not closes
  session until OVHD is updated. Before that, session is open but not valid
  since we will close the session for sure.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - the session is valid
  FALSE - the session is not valid

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrsmp_session_is_valid
(
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRSMP_SESSION_IS_CLOSING

DESCRIPTION
  This boolean accessor function tells the caller whether the session is
  in the process of closing or not.  
  
  It is intended to be used after initialization is completed.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - the session is closing
  FALSE - the session is not closing

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrsmp_session_is_closing
(
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRSMP_SET_CONFIG_TO_DEFAULT

DESCRIPTION


DEPENDENCIES
  None.

PARAMETERS
   None

RETURN VALUE
  None.

SIDE EFFECTS
  None 

===========================================================================*/
extern void hdrsmp_set_config_to_default
( 
  hdrsmp_config_params_type *current_config 
);

/*===========================================================================

FUNCTION HDRSMP_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of SMP

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Pointer to inconfig instance of SMP

SIDE EFFECTS
  None

===========================================================================*/
extern void* hdrsmp_get_inconfig_handle(void);

/*===========================================================================

FUNCTION HDRSMP_IS_TO_MAINTAIN_EVDO_SESSION

DESCRIPTION
  This function returns if AT should maintain the EVDO session or not.
  
  Maintain EVDO session is true if any of the followings are TRUE:
  - Minimal UATI Mode (MUM) is off
  - The user has attempted an EVDO data call within the last 3 minutes
  - PPP is established with an active or dormant data session

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  TRUE - Yes, AT should maintain EVDO session
  FALSE - No, AT shouldn't maintain EVDO session

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrsmp_is_to_maintain_evdo_session(void);


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_MUM_IS_ENABLED

DESCRIPTION
  This function checks if MUM mode is enabled through NV
  
DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  TRUE - If MUM mode is enabled through NV
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrsmp_mum_is_enabled(void);


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMP_PROCESS_CALL_ORIG

DESCRIPTION
  This function does the necessary processing for data call origination.
  It sets the maintain EVDO session timer, which implicitly enables the
  maintaining EVDO session.
  
DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
extern void hdrsmp_process_call_orig(void);

/*===========================================================================

FUNCTION HDRSMP_RESTART_KA_TIMER

DESCRIPTION
  This function restart TSMPClose timer 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The Keep Alive Interval Timer will be restarted

===========================================================================*/
extern void hdrsmp_restart_ka_timer ( void );

#if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
/*===========================================================================

FUNCTION HDRSMP_DCM_DEACTIVATE

DESCRIPTION
  This function commands HDRSMP to deactivate for DCM

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
extern void hdrsmp_dcm_deactivate(void);
#endif /* FEATURE_DEDICATED_C2K_MODE || FEATURE_IRAT_PILOT_MEAS */

#ifdef FEATURE_IRAT_PILOT_MEAS
/*===========================================================================

FUNCTION HDRSMP_SET_MEAS_MODE_IS_ON_FLAG

DESCRIPTION
  This function sets the measurement mode is on flag to TRUE or FALSE.

DEPENDENCIES
  None

PARAMETERS
  meas_mode_is_on - whether to turn measeurement mode ON or OFF.
                     
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void hdrsmp_set_meas_mode_is_on_flag
(
  boolean meas_mode_is_on
);
#endif

/*===========================================================================

FUNCTION HDRSMP_SET_DUP_ACT_DETECT

DESCRIPTION
  This function commands HDRSMP to deactivate for DCM

DEPENDENCIES
  None

PARAMETERS
  dup_act - duplicate SMP activate detection state
                     
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void hdrsmp_set_dup_act_detect
(
  hdrsmp_dup_act_detect_enum_type dup_act_detect
);

/*===========================================================================

FUNCTION HDRSMP_GET_CLOSE_REASON

DESCRIPTION
  This function gets the internal close reason.

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Internal close reason.
  
SIDE EFFECTS
  None

===========================================================================*/
hdrhai_session_close_reason_enum_type hdrsmp_get_close_reason( void );


/*===========================================================================

FUNCTION HDRSMP_SET_SESSION_CLOSING

DESCRIPTION
  This function sets the session closing flag, when SMP is about to enter INACTIVE.

DEPENDENCIES
  None

PARAMETERS
  boolean value to indicate whether to set to TRUE/FALSE.
  
RETURN VALUE
 None.
  
SIDE EFFECTS
  None

===========================================================================*/
void hdrsmp_set_session_closing( boolean value );

/*===========================================================================

FUNCTION HDRSMP_GET_SESSION_CLOSING

DESCRIPTION
  This function gets the current value of session closing flag.

DEPENDENCIES
  None

PARAMETERS
  None.
  
RETURN VALUE
  TRUE - if it is SET
  FALSE - if it is not SET

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrsmp_get_session_closing( void );
#endif               /* HDRSMP_H */
