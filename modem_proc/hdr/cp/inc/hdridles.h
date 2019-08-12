#ifndef HDRIDLES_H
#define HDRIDLES_H

/*===========================================================================

          I N I T I A L I Z A T I O N S T A T E  P R O T O C O L
                  S T A T E  D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for hdridles.c.

Copyright (c) 2000 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdridles.h_v   1.8   10 Dec 2002 18:11:48   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/inc/hdridles.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
                                         
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/27/11   cnx     Merged Optimized Handoff changes.
11/14/06   wsh     Disallow message on access probe when connection attempt is in
                   progress. This should not affect bundling with ConReq.
07/31/06   mpa     Added support for HDRIDLES_CONN_SETUP_SUSPEND_STATE
12/10/02   dna     Added support for HDRIDLES_SUSPEND_STATE
11/12/02   mpa     Added hdridles_is_previous_state_sleep function
01/10/01   SH      Deleted the open connection state
09/21/00   SH      Added the Open Connection State
05/29/00   SH      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


/*===========================================================================

                    DATA DECLARATIONS

===========================================================================*/

/* 
   Note: The state enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/

#define NUM_IDLE_STATES 7

/* Protocol states */

typedef enum                           /* Protocol state enum              */
{                                      /* -------------------              */
  HDRIDLES_INACTIVE_STATE           =0,  /* Inactive state                   */
  HDRIDLES_MONITOR_STATE            =1,  /* Monitor state                    */
  HDRIDLES_SLEEP_STATE              =2,  /* Sleep state                      */
  HDRIDLES_CONN_SETUP_STATE         =3,  /* Connection setup state           */
  HDRIDLES_SUSPEND_STATE            =4,  /* Suspend state                    */
  HDRIDLES_CONN_SETUP_SUSPEND_STATE =5,  /* Connection setup suspend state  */
  HDRIDLES_TUNNEL_STATE             =6   /* Tunnel state                     */
} hdridles_state_enum_type;

typedef enum                           /* Protocol state enum             */
{                                      /* -------------------             */
  HDRIDLES_CSETUP_INVALID    =0,       /* Invalid state                   */
  HDRIDLES_CSETUP_CREQ_SLP   =1,       /* ConReq queued to SLP            */
  HDRIDLES_CSETUP_CREQ_ACMAC =2,       /* ConReq given to ACMAC           */
  HDRIDLES_CSETUP_TCA_RCVD   =3,       /* TCA msg received                */
} hdridles_csetup_substate_enum_type;



/*===========================================================================

FUNCTION hdridles_change_state
DESCRIPTION
  Called to execute a state transition in IDLE.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridles_change_state
(
  hdridles_state_enum_type new_state
);

/*===========================================================================

FUNCTION hdridles_change_csetup_substate
DESCRIPTION
  Called to execute a state transition in IDLE.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridles_change_csetup_substate
(
  hdridles_csetup_substate_enum_type new_state
);

/*===========================================================================

FUNCTION hdridles_get_csetup_substate
DESCRIPTION
  Called to execute a state transition in IDLE.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state being transitioned to

RETURN VALUE
  Current Connection Setup Substate

SIDE EFFECTS
  None

===========================================================================*/
extern hdridles_csetup_substate_enum_type hdridles_get_csetup_substate
(
  void
);

/*===========================================================================

FUNCTION HDRIDLES_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the IDLE.
  This function is only to be used by other code in the IDLE.


DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  The current state of IDLE

SIDE EFFECTS
  None

===========================================================================*/

extern hdridles_state_enum_type hdridles_get_state( void );

/*===========================================================================

FUNCTION HDRIDLES_IS_PREVIOUS_STATE_SLEEP

DESCRIPTION
  Called to check the previous state of the IDLE for sleep.
  This function is only to be used by other code in the IDLE.


DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE  - Previous IDLE state was sleep
  FALSE - Previous IDLE state was not sleep

SIDE EFFECTS
  None

===========================================================================*/

extern boolean hdridles_is_previous_state_sleep( void );

/*===========================================================================

FUNCTION HDRIDLES_INIT

DESCRIPTION
  This function initializes the idle state protocol's state module.

DEPENDENCIES
  This function must called every time when the acess terminal enters 
  the HDR mode and before the idle state protocol can be activated

PARAMETERS
  None

RETURN VALUE
  none
  
SIDE EFFECTS
  None

===========================================================================*/

extern void hdridles_init( void );




#endif /* HDRIDLES_H */

