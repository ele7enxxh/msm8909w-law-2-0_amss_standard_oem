#ifndef HDRINITS_H
#define HDRINITS_H

/*===========================================================================

          I N I T I A L I Z A T I O N S T A T E  P R O T O C O L
                  S T A T E  D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for hdrinits.c.

Copyright (c) 2000 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrinits.h_v   1.4   10 May 2001 11:41:46   shuang  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrinits.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/29/13   vko     FR2812 - Pausing sync timer from HDRCP when HDRSRCH is in 2a state
06/21/10   cnx     Fixed compiler warnings.
05/11/10   pba     Added new states to handle LTE to HDR reselection.
05/29/00   SH      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* <EJECT> */

/*===========================================================================

                    DATA DECLARATIONS

===========================================================================*/

/* 
   Note: The state enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/

#define NUM_INIT_STATES 7

/* Protocol states */

typedef enum                               /* Protocol state enum              */
{                                          /* -------------------              */
  HDRINITS_INACTIVE_STATE      =0,         /* Inactive state                   */
  HDRINITS_NET_DET_STATE       =1,         /* Network Determiantion state      */
  HDRINITS_PILOT_ACQ_STATE     =2,         /* Pilot Acquisition state          */
  HDRINITS_SYNC_STATE          =3,         /* Synchronization state            */
  HDRINITS_TIME_TRANSFER_STATE =4,         /* Time transfer state              */
  HDRINITS_FAST_ACQ_STATE      =5,         /* Fast Acquisition state           */
  HDRINITS_SYNC_SUSPEND_STATE  =6          /* Synchronization suspend state    */
} hdrinits_state_enum_type;



/*===========================================================================

FUNCTION hdrinits_change_state
DESCRIPTION
  Called to execute a state transition in INIT.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state being transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrinits_change_state
(
  hdrinits_state_enum_type new_state
);


/*===========================================================================

FUNCTION HDRINITS_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the initialization state
  protocol.  This funciton is only to be used by other code in the
  initialization state protocol.


DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  The current state of INIT

SIDE EFFECTS
  None

===========================================================================*/
extern hdrinits_state_enum_type hdrinits_get_state( void );


/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_INIT

DESCRIPTION
  This function initializes the initialzation state protoco's state module.

DEPENDENCIES
  This function must called before the initialization state protocol can be
  activated

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrinits_init ( void );

#endif /* HDRINITS_H */

