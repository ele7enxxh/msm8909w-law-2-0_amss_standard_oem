#ifndef HDRMRLPCTL_H
#define HDRMRLPCTL_H

/*===========================================================================


                        H I G H     D A T A     R A T E

             M U L T I - F L O W  S N - R L P  C O N T R O L  F I L E 
             
                         

GENERAL DESCRIPTION
  This module forms the control module for operations regarding MultiFlow RLP
  instances bound to the SN stream.

Copyright (c) 2004-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrmrlpctl.h#1 $
                     
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/11   vpk     HDR SU API cleanup
03/26/07   sk      Added EMPA MPA handoff 
03/05/07   sk      Added support to route the NETWORK_ACQUIRED indication to 
                   secondary packet manager
10/02/06   sk      Featurized file under FEATURE_HDR_QOS
07/06/06   sk      QoS 2.0 Support
05/30/06   ksu     EMPA: RLP init code review comments
05/04/06   ksu     EMPA: RSP part 2
02/13/06   spn     Added a function to return if MRLP is initialised or 
                   not.
10/24/05   spn     Changed the Number of call backs registered with
                   hdrmrlpctl module.   
08/08/05   sk/sy   Merged semi-dynamic QOS changes from sandbox.
11/22/04   vas     Added GAUP Support
04/04/04   vas     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#ifdef FEATURE_HDR_QOS
#include "dsrlp_v.h"
#include "hdrpacrlpctl.h"
/* <EJECT> */

/*===========================================================================

             DEFINITIONS  FOR THE MODULE

===========================================================================*/

typedef enum
{
  HDRMRLPCTL_SESSION_CLOSED = 0,
  HDRMRLPCTL_SESSION_OPEN_RLP_UNINITIALIZED = 1,
  HDRMRLPCTL_SESSION_OPEN_RLP_INITIALIZED = 2
} hdrmrlpctl_state_enum_type;

typedef struct
{
  uint8 active_rlp_count_fwd;  /* Max simlutaneously activated forward link M-RLPs */
  uint8 active_rlp_count_rev;  /* Max simlutaneously activated reverse link M-RLPs */
  uint8 rlp_flow_fwd[DSRLP_MAX_NUM_HDR_MRLP_FWD]; /* RLP Flows for Fwd Link RLPs */
  uint8 rlp_flow_rev[DSRLP_MAX_NUM_HDR_MRLP_REV]; /* RLP Flows for Rev Link RLPs */
  hdrmrlpctl_state_enum_type state;

  uint8 rlp_flow_fwd_default; /* SR ID corresponding to the default MRLP FWD */
  uint8 rlp_flow_rev_default; /* SR ID corresponding to the default MRLP REV */

  hdrpacrlpctl_session_change_cb_type session_state_change_notification_cb;


} hdrmrlpctl_type;


/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/*===========================================================================

                        FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRMRLPCTL_IS_RLP_DEFAULT

DESCRIPTION
  Determines if the reservation label 0xFF is associated with the 
  particular RLP. Returns TRUE if it is, False otherwise   
  
DEPENDENCIES

RETURN VALUE
  TRUE  - If the RLP is the default
  FALSE - Otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrmrlpctl_is_rlp_default
(
  uint8 rlp_flow,
  dsrlp_ver_enum_type rlp_ver
);

/*===========================================================================
FUNCTION HDRMRLPCTL_INIT_ALL_RLP

DESCRIPTION
  This function loops through and initializes all the RLPs
  
DEPENDENCIES
  hdrmrlp_configure_all_rlp needs to have been called
  MultiFlow RLPa have to be configured before this function is called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmrlpctl_init_all_rlp( void );

/*===========================================================================

FUNCTION HDRMRLPCTL_CLEANUP_ALL_RLP

DESCRIPTION
  This function loops through and cleans up all the multi-flow RLPs.
  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmrlpctl_cleanup_all_rlp( void );

#ifdef FEATURE_HDR_EMPA
/*===========================================================================

FUNCTION HDRMRLPCTL_RLP_INITIALIZE_ROUTE

DESCRIPTION
  This function loops through and resets all the multi-flow RLPs for a
  specific route.
  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmrlpctl_rlp_initialize_route( byte route );
#endif /* FEATURE_HDR_EMPA */

/*===========================================================================

FUNCTION HDRMRLPCTL_PROCESS_GAUP_NOTIFICATION

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmrlpctl_process_rlp_gaup_notification( void );

/*===========================================================================

FUNCTION HDRMRLPCTL_PROCESS_SESSION_CHANGE

DESCRIPTION
  This function is called to notify MRLP about any relevant session related
  events
  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmrlpctl_process_session_change
(
  ds707_sys_qos_state_enum_type event,
  hdrcom_hai_app_subtype_enum_type old_app_subtype,
  hdrcom_hai_app_subtype_enum_type new_app_subtype
);

/*===========================================================================

FUNCTION HDRMRLPCTL_STARTUP

DESCRIPTION
  Does the powerup initialization required for Multiflow RLP.
  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmrlpctl_startup( void );

/*===========================================================================

FUNCTION HDRMRLPCTL_REGISTER_FOR_NOTIFICATIONS

DESCRIPTION
  Sets the callback function pointers to point to those passed in as parameters
  to this function.
  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Overwrites the previous value of all the callback function pointers

===========================================================================*/
errno_enum_type hdrmrlpctl_register_for_notifications
(
  hdrpacrlpctl_session_change_cb_type session_state_change_notification_cb
);

/*===========================================================================

FUNCTION HDRMRLPCTL_IS_MRLP_INITIALISED

DESCRIPTION
  Returns if MRLP has been initialised or not.   
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE if MRLP is initialised
  FALSE if MRLP has not been initialised
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrmrlpctl_is_mrlp_initialised
(
  void
);

/*===========================================================================

FUNCTION HDRMRLPCTL_QUERY_MRLP_STATE

DESCRIPTION
  Returns current MRLP state
  
DEPENDENCIES
  None

RETURN VALUE
  MRLP state type
  
SIDE EFFECTS
  None.

===========================================================================*/
hdrmrlpctl_state_enum_type hdrmrlpctl_query_mrlp_state
(
  void
);

/*===========================================================================
FUNCTION        HDRMRLPCTL_NETWORK_ACQUIRED

DESCRIPTION     This function processes the signal if HDR NETWORK is acquired   
  
DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
void hdrmrlpctl_network_acquired(void);

/*===========================================================================
FUNCTION        HDRMRLPCTL_RESTORE_PRIOR_SESSION_ATTEMPTED

DESCRIPTION     This function processes the signal if prior session is 
                attempted to restore.
                The indication is routed to secondary packet manager using the 
                existing callback mechanism
  
DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
void  hdrmrlpctl_restore_prior_session_attempted(void);
/*===========================================================================
FUNCTION        HDRMRLPCTL_PRIOR_SESSION_RESTORED

DESCRIPTION     This function processes the signal if prior session is restored.
                The indication is routed to secondary packet manager using the 
                existing callback mechanism
  
DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
void hdrmrlpctl_prior_session_restored(void);


/*===========================================================================
FUNCTION HDRMRLPCTL_UPDATE_WMK

DESCRIPTION
  This function updates watermark level based on technology used in current 
  HDR session. Rev0/A vs RevB 

DEPENDENCIES
  None

PARAMETERS
  None  

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmrlpctl_update_wmk( void );


#endif /* FEATURE_HDR_QOS */


#endif /* HDRMRLPCTL_H */

